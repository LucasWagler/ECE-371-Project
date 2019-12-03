// Main module for ECE 371 motion alarm project
// Author: Alex Jasper (anjasper17@my.trine.edu)

#include "esp.h"

const uint8_t Send[SENDSize]	 = "AT+CIPSEND=0,5\r\n";
const uint8_t message[MSGSize] = "Moved";
const uint8_t Close[CLOSESize] = "AT+CIPCLOSE=0\r\n";

queue_t espSendQ;
queue_t espMsgQ;
queue_t espCloseQ;

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

void config_ESP()
{
	uint8_t Attention[ATSize]		= "AT\r\n";															//Attention
	uint8_t Reset[RSTSize]			= "AT+RST\r\n";													//Reset
	uint8_t Config_AP[SAPSize]	= "AT+CWSAP=\"AJ\",\"\",3,0\r\n";				//AT+CWSAP*=<ssid>,<pwd><chl><ecn> - Set up access point
	uint8_t Set_mode[MODESize]	= "AT+CWMODE=3\r\n";										//set the operating mode to AP+STA (access point + station?)	
	uint8_t Set_IP[PAPSize]			= "AT+CIPAP=\"192.168.0.222\"\r\n";			//Set IP of the ESP
	uint8_t Server[SERVERSize]	= "AT+CIPSERVER=1,80\r\n";							//A value of 1 creates the the server while 0 deletes the server in which case the module needs to restarted
	uint8_t Set_mux[MUXSize]		= "AT+CIPMUX=1\r\n";										//Single connection mode is set with a 0 value while multiple connection mode is set with a value of 1. 
	
	
	//ATTENTION																			
	for(uint8_t i = 0; i < ATSize; i++){send_UART_Config(Attention[i]);}						//transmit message
	for(uint32_t a = 0; a < 5000000; a++){;}														//wait to not send message multiple times
	
	//RESET													
	for(uint8_t i = 0; i < RSTSize; i++){send_UART_Config(Reset[i]);}								//transmit message
	for(uint32_t a = 0; a < 5000000; a++){;}														//wait to not send message multiple times

	//CONFIG ACCESS POINT																			
	for(uint8_t i = 0; i < SAPSize; i++){send_UART_Config(Config_AP[i]);}	//transmit message
	for(uint32_t a = 0; a < 5000000; a++){;}														//wait to not send message multiple times

	//Set IP address of Access Point																				
	for(uint8_t i = 0; i < PAPSize; i++){send_UART_Config(Set_IP[i]);}		//transmit message
	for(uint32_t a = 0; a < 5000000; a++){;}														//wait to not send message multiple times			
		
	//SET CONNECTION MODE																				
	for(uint8_t i = 0; i < MUXSize; i++){send_UART_Config(Set_mux[i]);}	//transmit message
	for(uint32_t a = 0; a < 5000000; a++){;}														//wait to not send message multiple times

	//SET OPERATING MODE																				
	for(uint8_t i = 0; i < MODESize; i++){send_UART_Config(Set_mode[i]);}	//transmit message
	for(uint32_t a = 0; a < 5000000; a++){;}														//wait to not send message multiple times		
		
	//CONFIGURE A SERVER																					
	for(uint8_t i = 0; i < SERVERSize; i++){send_UART_Config(Server[i]);}		//transmit message
	for(uint32_t a = 0; a < 5000000; a++){;}													//wait to not send message multiple times
}

void send_UART_Config(uint8_t msg)
{
		UART0->D = msg;
		while((UART0->S1 & UART0_S1_TC_MASK) == 0) {;} //Stay while Tx not complete
}

void esp_task()
{
	//disable interrupts
	uint32_t mask;
	mask = __get_PRIMASK();
	__disable_irq();
	
	static int cmd = 0;
	if((UART0->S1 & UART0_S1_TC_MASK))
	{
		uint8_t msg = 0;
		switch(cmd)
		{
			case 0:
				if(get_q(&espSendQ, &msg))
				{
					UART0->D = msg;
				}
				else
				{
					cmd++;
				}
				break;
			case 6000:
				if(get_q(&espMsgQ, &msg))
				{
					UART0->D = msg;
				}
				else
				{
					cmd++;
				}
				break;
			case 20000:
				if(get_q(&espCloseQ, &msg))
				{
					UART0->D = msg;
				}
				else
				{
					cmd = 0;
				}
				break;
			default: cmd++;
		}
	}
	//re-enable interrupts
	__set_PRIMASK(mask);
}

/*
int main()
{		
	config_UART();
	config_ESP();	
	init_q(&espSendQ, (int)SENDSize);
	init_q(&espMsgQ, (int)MSGSize);
	init_q(&espCloseQ, (int)CLOSESize);

		
	
	for(int i = 0; i < 90; i++)
	{
		for(uint64_t i = 0; i < 0xFFFFF; i++){;}
	}

	while(1)
	{
		if(repeat)
		{
			for(uint8_t i = 0; i < SENDSize; i++)
			{
				put_q(&espSendQ, Send[i]);
			}																					
			for(uint8_t i = 0; i < MSGSize; i++)
			{	
				put_q(&espMsgQ, message[i]);
			}		
			for(uint8_t i = 0; i < CLOSESize; i++)
			{
				put_q(&espCloseQ, Close[i]);
			}
			repeat = 0;
		}		
		send_UART_Data();				
	}
}
*/
