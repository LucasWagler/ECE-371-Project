#include <MKL25Z4.h>
#include "display.h"

const uint8_t dogm_init_seq[INITLENGTH] = { 0x3a, 0x09, 0x06, 0x1e, 0x39, 0x1b, 0x6e, 0x57, 0x72, 0x38, 0x0f };
const uint8_t printSeq[PRINTLENGTH] = { "The device has been moved!" };

void config_spi(void)
{
	//Enable Clock
	SIM->SCGC4 |= SIM_SCGC4_SPI0_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;	
	//Disable configuration
	SPI0->C1 &= ~SPI_C1_SPE_MASK;	
	//Setup SCK
	PORTC->PCR[SCK] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[SCK] |= PORT_PCR_MUX(SPI_MUX);	
	//Setup MOSI
	PORTC->PCR[MOSI] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[MOSI] |= PORT_PCR_MUX(SPI_MUX);	
	//Make Master
	SPI0->C1 |= SPI_C1_MSTR_MASK;	
	//Active Low, Sample on Rising, LSB First
	SPI0->C1 |= SPI_C1_CPOL_MASK | SPI_C1_CPHA_MASK | SPI_C1_LSBFE_MASK;
	//CS
	PORTC->PCR[CS] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[CS] |= PORT_PCR_MUX(GPIO_MUX);
	PTC->PDDR |= (1UL << CS);
	PTC->PCOR |= (1UL << CS);	
	//RESET DOGM
	PORTC->PCR[RESET] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[RESET] |= PORT_PCR_MUX(GPIO_MUX);
	PTC->PDDR |= (1UL << RESET);
	PTC->PSOR |= (1UL << RESET);	
	//Rate
	SPI0->BR = SPI_BR_SPPR(5) | SPI_BR_SPR(1);
	//Enable configuration
	SPI0->C1 |= SPI_C1_SPE_MASK;
}

void sendData(uint8_t value)
{
	//Send
	while(!(SPI0->S & (SPI_S_SPTEF_MASK))) {;}
	SPI0->D = value;
	
	//Wait to Receive
	while(!(SPI0->S & (SPI_S_SPRF_MASK))) {;}
	uint8_t d_in = SPI0->D;
}

void resetDOGM(void)
{
	PTC->PCOR |= (1UL << RESET);
	for(uint32_t i = 0; i < 10000; i++){;}
	PTC->PSOR |= (1UL << RESET);
}

void prepData(uint8_t hexValue, uint8_t cod)
{
	uint8_t start = 0x00;
	if(cod == 0)
	{
		start = 0x1F;	
	}
	else
	{
		start = 0x5F;
	}
	
	uint8_t low = hexValue;
	low &= 0x0F;
	
	uint8_t high = hexValue;
	high = high >> 4;
	
	sendData(start);
	sendData(low);
	sendData(high);
}

void clearDOGM(void)
{
	for(uint8_t i = 0; i < DISPLAYLENGTH; i++)
	{
		prepData(' ', 1);
	}
}
/*
int main()
{
	config_spi();
	resetDOGM();
	clearDOGM();
	
	//Initialization
	for(uint8_t i = 0; i < INITLENGTH; i++)
	{
		prepData(dogm_init_seq[i], 0);
	}
	
	for(int i = 0; i < 10; i++)
	{
		//Name
		for(uint8_t j = 0; j < PRINTLENGTH; j++)
		{
			prepData(print[j], 1);
		}
		for(int j = 0; j < 5000000; j++);
		clearDOGM();
		for(int j = 0; j < 5000000; j++);
	}	
}
*/
