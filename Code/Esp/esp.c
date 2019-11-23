#include "esp.h"

const uint8_t Send_data_in_multiple_connection_mode[SENDSize]	= "AT+CIPSEND=0,5\r\n";
const uint8_t message[MSGSize] = "Moved";
const uint8_t Close_connection[CLOSESize] = "AT+CIPCLOSE=0\r\n";

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
	uint8_t Attention[ATSize]																	= "AT\r\n";															//Attention
	uint8_t Reset[RSTSize]																		= "AT+RST\r\n";													//Reset
	uint8_t Config_access_point[SAPSize]											= "AT+CWSAP=\"AJ\",\"\",3,0\r\n";				//AT+CWSAP*=<ssid>,<pwd><chl><ecn> - Set up access point
	uint8_t Set_Operating_Mode[MODESize]											= "AT+CWMODE=3\r\n";										//set the operating mode to AP+STA (access point + station?)	
	uint8_t Set_IP_address_of_Access_Point[PAPSize]						= "AT+CIPAP=\"192.168.0.222\"\r\n";			//Set IP of the ESP
	uint8_t Configure_Server[SERVERSize]											= "AT+CIPSERVER=1,80\r\n";							//A value of 1 creates the the server while 0 deletes the server in which case the module needs to restarted
	uint8_t Set_connection_mode[MUXSize]											= "AT+CIPMUX=1\r\n";										//Single connection mode is set with a 0 value while multiple connection mode is set with a value of 1. 
	
	
	//ATTENTION																			
	for(uint8_t i = 0; i < ATSize; i++){send_UART(Attention[i]);}						//transmit message
	for(uint32_t a = 0; a < 5000000; a++){;}														//wait to not send message multiple times
	
	//RESET													
	for(uint8_t i = 0; i < RSTSize; i++){send_UART(Reset[i]);}								//transmit message
	for(uint32_t a = 0; a < 5000000; a++){;}														//wait to not send message multiple times

	//CONFIG ACCESS POINT																			
	for(uint8_t i = 0; i < SAPSize; i++){send_UART(Config_access_point[i]);}	//transmit message
	for(uint32_t a = 0; a < 5000000; a++){;}														//wait to not send message multiple times

	//Set IP address of Access Point																				
	for(uint8_t i = 0; i < PAPSize; i++){send_UART(Set_IP_address_of_Access_Point[i]);}		//transmit message
	for(uint32_t a = 0; a < 5000000; a++){;}														//wait to not send message multiple times			
		
	//SET CONNECTION MODE																				
	for(uint8_t i = 0; i < MUXSize; i++){send_UART(Set_connection_mode[i]);}	//transmit message
	for(uint32_t a = 0; a < 5000000; a++){;}														//wait to not send message multiple times

	//SET OPERATING MODE																				
	for(uint8_t i = 0; i < MODESize; i++){send_UART(Set_Operating_Mode[i]);}	//transmit message
	for(uint32_t a = 0; a < 5000000; a++){;}														//wait to not send message multiple times		
		
	//CONFIGURE A SERVER																					
	for(uint8_t i = 0; i < SERVERSize; i++){send_UART(Configure_Server[i]);}		//transmit message
	for(uint32_t a = 0; a < 5000000; a++){;}													//wait to not send message multiple times
}

void send_UART(uint8_t msg)
{
		UART0->D = msg;
		while((UART0->S1 & UART0_S1_TC_MASK) == 0) {;} //Stay while Tx not complete
}

/*
int main()
{		
	config_UART();
	config_ESP();		
	
	while(1)
	{
		//SEND MESSAGE																																
		for(uint8_t i = 0; i < 20; i++){send_UART(Send_data_in_multiple_connection_mode[i]);}			//transmit message
		for(uint32_t a = 0; a < 40000; a++){;}																										//wait for ESP8266 to confirm before sending message
		for(uint8_t i = 0; i < 5; i++){send_UART(message[i]);}	
		for(uint32_t a = 0; a < 5000000; a++){;}																									//wait to not send message multiple times
			
		//CLOSE CONNECTION
		for(uint8_t i = 0; i < 17; i++){send_UART(Close_connection[i]);}		//transmit message
		for(uint32_t a = 0; a < 5000000; a++){;}													//wait to not send message multiple times						
	}
}
*/
