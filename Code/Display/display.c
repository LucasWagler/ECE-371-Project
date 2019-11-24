// Main module for ECE 371 motion alarm project
// Authors: Alex Jasper (anjasper17@my.trine.edu)
//					Lucas Wagler (ldwagler15@my.trine.edu)

#include "display.h"

queue_t spiQ;

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

void display_task(void)
{
	
	static bool virgin = true;
	static bool ready = true;

	if((SPI0->S & (SPI_S_SPRF_MASK)))// && ((SPI0->S & (SPI_S_SPTEF_MASK))))
	{
		ready = true;
	}

	if(ready)
	{
		if(virgin)
		{
			virgin = false;
		}
		else
		{
			uint8_t d_in = SPI0->D;
		}
		uint8_t value = 0;
		
		//Diable interrupts
		uint32_t mask;
		mask = __get_PRIMASK();
		__disable_irq();
		
		if(get_q(&spiQ, &value))
		{
			//Send
			SPI0->D = value;
			ready = false;
		}
		//re-enable interrupts
		__set_PRIMASK(mask);
	}	
}

void resetDOGM(void)
{
	PTC->PCOR |= (1UL << RESET);
	for(uint32_t i = 0; i < 1500; i++){;}
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
	
	put_q(&spiQ, start);
	put_q(&spiQ, low);
	put_q(&spiQ, high);
}

void initDOGM()
{
	for(int i = 0; i < INITLENGTH; i++)
	{
		prepData(dogm_init_seq[i], 0);
	}
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
	init_q(&spiQ, (int)1000);
	config_spi();
	resetDOGM();	
	initDOGM();
	clearDOGM();
	
	for(int i = 0; i < PRINTLENGTH; i++)
	{
		prepData(printSeq[i], 1);
	}
	
	while(1)
	{
		display_task();
	}
}
*/
