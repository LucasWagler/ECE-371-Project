#include <MKL25Z4.h>
#include <stdlib.h>

#define SCK (5)
#define MOSI (6)
#define SPI_MUX (2)
#define GPIO_MUX (1)
#define CS (7)
#define RESET (8)

#define SYS_CLOCK (48e6)
#define OVERSAMPLE (16)
#define BAUD_RATE (115200)

const uint8_t dogm_init_seq[] = 
{
	0x3a, 0x09, 0x06, 0x1e, 0x39, 0x1b, 0x6e, 0x57, 0x72, 0x38, 0x0f
};

//uint8_t our_msg[] = "AT\r\n"; //Status
//uint8_t our_msg[] = "AT+RST\r\n"; //Reset
//uint8_t our_msg[] = "AT+GMR\r\n"; //Firmware Version
//uint8_t our_msg[] = "AT+CWMODE?\r\n"; //Get mode

//Set as Access Point
//uint8_t our_msg[] = "AT+CWSAP=\"ApacheTest\",\"\",3,0\r\n"; //Setup access point with name and password (8+ chars on password)
//uint8_t our_msg[] = "AT+CWSAP?\r\n"; //Check the Access Point Settings
//uint8_t our_msg[] = "AT+CIPAP?\r\n"; //Get IP of Address Point
//uint8_t our_msg[] = "AT+CIPAP=\"192.168.0.101\"\r\n"; //Set IP 
//uint8_t our_msg[] = "AT+CIPAPMAC=\"1a:fe:34:a4:35:42\"\r\n"; //Set MAC address
//uint8_t our_msg[] = "AT+CWDHCP=0,1\r\n"; //Set DHCP mode
//uint8_t our_msg[] = "AT+CWLIF\r\n"; //Show connected devices

//Configure server
//uint8_t our_msg[] = "AT+CIFSR\r\n"; //Get local IP
//uint8_t our_msg[] = "AT+CIPMUX=1\r\n"; //Multi Connection DO THIS BEFORE SERVER
//uint8_t our_msg[] = "AT+CIPSERVER=1,80\r\n"; //Start server
//uint8_t our_msg[] = "AT+CIPSTO?\r\n"; //Get server timeout
//uint8_t our_msg[] = "AT+CIPSTO=30\r\n"; //Set server timeout
uint8_t our_msg[] = "AT+CIPSTART=1,\"TCP\",\"192.168.0.102\",80\r\n"; //Enter IP of machine you want to connect to
//uint8_t our_msg[] = "AT+CIPSTATUS\r\n"; //Connection status
uint8_t our_msg2[] = "AT+CIPSEND=1,31\r\n";//GET /connection.py HTTP/1.0";//\r\nHost: 192.168.0.102\r\nConnection: close\r\n\r\n";
uint8_t our_msg3[] = "GET /connection.py HTTP/1.0\r\n";
//uint8_t our_msg3[] = "AT+CIPCLOSE=1\r\n"; //Close connection

void config_UART()
{
	uint16_t sbr = 0;
	
	//Clk to UART and PORTA
	SIM_SCGC4 |= SIM_SCGC4_UART0_MASK;
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
	
	//Disable Tx and Rx for config
	UART0->C2 &= ~UART0_C2_TE_MASK & ~UART0_C2_RE_MASK;
	
	//UART 48MHz clk
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1);
	SIM->SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK;
	
	//UART Pins on Port A
	PORTA->PCR[1] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[1] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2); //Rx
	PORTA->PCR[2] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[2] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2); //Tx
	
	//Set Baud Scaler
	sbr = (uint16_t)((SYS_CLOCK)/(BAUD_RATE * OVERSAMPLE));
	UART0->BDH &= ~UART0_BDH_SBR_MASK;
	UART0->BDH |= UART0_BDH_SBR(sbr >> 8);
	UART0->BDL = UART0_BDL_SBR(sbr);
	UART0->C4 = UART0_C4_OSR(OVERSAMPLE - 1);
	
	//Disable loopback, 8 bit, no parity
	UART0->C1 = 0;
	
	//Invert
	UART0->C3 = UART0_C3_TXINV(0);
	
	//LSB
	UART0->S2 = UART0_S2_MSBF(0);
	
	//Enable UART Tx and Rx
	UART0->C2 |= UART0_C2_TE_MASK | UART0_C2_RE_MASK;
}

void config_spi()
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
	
	//Active Low, Sample on Rising
	SPI0->C1 |= SPI_C1_CPOL_MASK;
	SPI0->C1 |= SPI_C1_CPHA_MASK;
	
	//LSB First
	SPI0->C1 |= SPI_C1_LSBFE_MASK;
	
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

void send_SPI(uint8_t value)
{
	//Send
	while(!(SPI0->S & (SPI_S_SPTEF_MASK))) {;}
	SPI0->D = value;
	
	//Wait to Receive
	while(!(SPI0->S & (SPI_S_SPRF_MASK))) {;}
	uint8_t d_in = SPI0->D;
}

void resetDOGM()
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
	
	send_SPI(start);
	send_SPI(low);
	send_SPI(high);
}

void send_UART(uint8_t msg)
{
		UART0->D = msg;
		while((UART0->S1 & UART0_S1_TC_MASK) == 0) {;} //Stay while Tx not complete
}

uint8_t receive_UART()
{
	while(!(UART0->S1 & UART_S1_RDRF_MASK))	{;}
	return UART0->D;
}

void clear_DOGM()
{
	for(int i = 0; i < 80; i++)
	{
		prepData(' ', 1);
	}	
}

int main()
{
	uint8_t data[80];
	size_t dataLength = sizeof (data) / sizeof (data[0]);
		
	config_spi();
	config_UART();
	resetDOGM();
	
	//Initialization DOGM
	for(uint8_t i = 0; i < 11; i++)
	{
		prepData(dogm_init_seq[i], 0);
	}
	
	clear_DOGM();
	
	size_t arrayLength = sizeof (our_msg) / sizeof (our_msg[0]);
	size_t arrayLength2 = sizeof (our_msg2) / sizeof (our_msg2[0]);
	size_t arrayLength3 = sizeof (our_msg3) / sizeof (our_msg3[0]);
	
//	//Connection
//	for(uint8_t i = 0; i < arrayLength; i++)
//	{
//		send_UART(our_msg[i]);
//	}
//	//Read
//	for(uint8_t i = 0; i < dataLength; i++)
//	{
//		data[i] = receive_UART();
//		if(data[i] == '\r' || data[i] == '\n')
//			data[i] = ' ';	
//			prepData(data[i], 1);
//	}	
	//Send request
	for(uint8_t i = 0; i < arrayLength2; i++)
	{
		send_UART(our_msg2[i]);
	}
	//Read
	for(uint8_t i = 0; i < dataLength; i++)
	{
		data[i] = receive_UART();
		//if(data[i] == '\r' || data[i] == '\n')
		if(data[i] == '>')
		{
			for(uint8_t j = 0; j < arrayLength3; j++)
			{
				send_UART(our_msg3[j]);
			}
		}
			
	}
//	//GET
//	for(uint8_t i = 0; i < arrayLength3; i++)
//	{
//		send_UART(our_msg3[i]);
//	}
	
	//Read
	for(uint8_t i = 0; i < dataLength; i++)
	{
		data[i] = receive_UART();
		if(data[i] == '\r' || data[i] == '\n')
			data[i] = ' ';	
			prepData(data[i], 1);
	}
}
