#include <MKL25Z4.h>
#include <stdlib.h> // defines size_t
#include <stdio.h> 
#define Mask (1UL<<(x))
#define UART_OVERSAMPLE_RATE (16)
#define SYS_CLOCK (48e6)
#define BAUD_RATE 115200UL

void config_Pins(){
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;         // Ties clock to C Pins
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;         // Ties clock to E Pins
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;         // Ties clock to A Pins
	
	//UART Pins
	PORTA->PCR[1]  &= ~PORT_PCR_MUX_MASK;				//  clears choice of ALT for Pin A1
	PORTA->PCR[1]  |=  PORT_PCR_MUX(2);					// chooses choice of ALT for Pin A1 (UART0_RX)
	
	PORTA->PCR[2]  &= ~PORT_PCR_MUX_MASK;				//  clears choice of ALT for Pin A1
	PORTA->PCR[2]  |=  PORT_PCR_MUX(2);					// chooses choice of ALT for Pin A1 (UART0_TX)
	
}

void config_UART(){

	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK; // clock enable to UART0
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK; // clock enable to port A
	
	UART0->C2 &= ~UART0_C2_TE_MASK; //DISABLE UART TO CONFIG
	
	//SET UART CLOCK TO 48 MHz 
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1);
	SIM->SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK;
	
	//clearing control registers
	UART0->C1 = 0x00; 
	UART0->C2 = 0x00;
	UART0->C3 = 0x00;
	UART0->C4 = 0x00;
	UART0->C5 = 0x00;
	
	//setting control registers
	UART0->C1 = UART0_C1_LOOPS(0);  // Loop Mode Select (DONT ENABLE LOOPBACK MODE)
	UART0->C1 = UART0_C1_M(0);      // 9-Bit or 8-Bit Mode Select (USE 8 DATA BIT MODE)
	UART0->C1 = UART0_C1_PE(0);     // Parity Enable  (not currently used)
	
	UART0->C2 = UART0_C2_TIE(0);  // Transmit Interrupt Enable for TDRE ()
	
	PORTA->PCR[2] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2);	//SET OUTPUT PIN
	
	//SET BAUD RATE AND OVERSAMPLING RATIO
	uint16_t sbr = (uint16_t) ((SYS_CLOCK)/( BAUD_RATE * UART_OVERSAMPLE_RATE));
	UART0->BDH &= ~UART0_BDH_SBR_MASK;
	UART0->BDH |=  UART0_BDH_SBR(sbr>>8);	
	UART0->BDL  =  UART0_BDL_SBR(sbr);	
	UART0->C4  |=  UART0_C4_OSR(UART_OVERSAMPLE_RATE-1);
	
	UART0->BDH |= UART0_BDH_SBNS(0);	//SELECT NUMBER OF STOP BITS 0 - 1 STOP BIT, 1 - 2 STOP BITS
	
	UART0->C2 |= UART0_C2_TE(1); //ENABLE UART
	
}

void Send_UART(uint8_t message){
	UART0->D = message;
	while(!(UART0->S1 & UART0_S1_TC_MASK)){;}
	
}

int main(){
	config_Pins();
	config_UART();
	
	uint8_t z[8] = "Moved";
	
	uint8_t Attention[4]																			= "AT\r\n";																			//Attention
	uint8_t Reset[10]																					= "AT+RST\r\n";																	//Reset
	uint8_t Show_Firmware_Version[11]													= "AT+GMR\r\n";																	//Firmware Version
	uint8_t Config_access_point[30]														= "AT+CWSAP=\"AJ\",\"\",3,0\r\n";							//AT+CWSAP*=<ssid>,<pwd><chl><ecn> - Set up access point
	//uint8_t Show_Operating_Mode[14]														= "AT+CWMODE?\r\n";															//what is the Operating Mode
	uint8_t Set_Operating_Mode[15]														= "AT+CWMODE=3\r\n";													//set the operating mode to AP+STA (access point + station?)
	uint8_t Close_connection[17]															= "AT+CIPCLOSE=0\r\n";													//set the operating mode to AP+STA (access point + station?)
	
	//uint8_t Hello[5]																					= "Hello";																			//send hello
	//uint8_t Goodbye[7]																				= "Goodbye";																		//send goodbye
	//uint8_t Verify_AP_settings[9]															= "AT+CWSAP?";
	//uint8_t Get_IP_address_of_Access_Point[9]									= "AT+CIPAP?";
	uint8_t Set_IP_address_of_Access_Point[30]								= "AT+CIPAP=\"192.168.0.222\"\r\n";
	//uint8_t Get_MAC_address_of_Access_Point[9]								= "AT+CIPAP?";
	//uint8_t Set_MAC_address_of_Access_Point[32]								= "AT+CIPAPMAC=\"1a:fe:34:a4:35:42\"";
	//uint8_t Enable_DHCP_for_AP_mode[13]												= "AT+CWDHCP=0,1";															//DHCP is 
	//uint8_t Get_IP_addresses_of_clients[8]										= "AT+CWLIF";
	
	//for server???
	//uint8_t Get_local_IP_address[8]														= "AT+CIFSR";
	uint8_t Configure_Server[23]															= "AT+CIPSERVER=1,80\r\n";										//A value of 1 creates the the server while 0 deletes the server in which case the module needs to restarted
	uint8_t Set_connection_mode[15]														= "AT+CIPMUX=1\r\n";														//Single connection mode is set with a 0 value while multiple connection mode is set with a value of 1. 
	//uint8_t ask_connection_mode[10]														= "AT+CIPMUX?";
	//uint8_t Set_transfer_mode[12]															= "AT+CIPMODE=0";
	//uint8_t Get_current_server_timeout[10]										= "AT+CIPSTO?";
	//uint8_t Set_server_timeout[12]														= "AT+CIPSTO=10";																//Server timeout value can range between 0 and 7200 seconds. Default value is 10 seconds.
	//uint8_t Start_TCP_or_UDP_conn_in_single_conn_mode[36]			= "AT+CIPSTART=\"TCP\",\"192.168.0.65\",333";
	//uint8_t Start_TCP_or_UDP_conn_in_multiple_conn_mode[39]		= "AT+CIPSTART=1,\"TCP\",\"192.168.0.65\",333";
	//uint8_t Send_data_in_single_connection_mode[16]						= "AT+CIPSEND=5\r\n";																//When in single connection mode only the length of the data in bytes is required. Maximum length is 2048 bytes. Send command should be immediately followed by the actual data that matches the length specified.
	uint8_t Send_data_in_multiple_connection_mode[20]					= "AT+CIPSEND=0,5\r\n";													//When in multiple connection mode the first parameter is the id of the connection followed by length in bytes.
	//uint8_t Receive_Data_in_single_connection_mode[9]					= "+IPD,4:data";																//The length of the data that is to be received is specified first followed by a colon and the variable where the received data will be stored.
	//uint8_t Receive_Data_in_multiple_connection_mode[9]				= "+IPD,1,4:data";															//In addition to the length and data the connection id needs to be specified as the first argument.
	//uint8_t Close_TCP_or_UDP_conn_in_single_conn_mode[11]			= "AT+CIPCLOSE";
	//uint8_t Close_TCP_or_UDP_conn_in_multiple_conn_mode[13]		= "AT+CIPCLOSE=1";															//To close all connections a value of 5 can be used, otherwise the connection id should be specified.
	//uint8_t Check_current_connection_status[12]								= "AT+CIPSTATUS";																//Current module condition is determined by the STATUS value which can be 2 for IP acquired, 3 for connected and 4 for disconnected. In multiple connection mode the id of the connection is also listed. Connection type will be either TCP or UDP. The ip address and port of the connection are listed as well. The last variable represents the module mode, 0 for client mode or 1 for server mode.

	/*
	- SSID is the network name that will appear when connecting to the wireless network. 
	- The password will be required except when encryption mode is set to OPEN. 
	- The channel is the network channel that will be used by the module. 
	- Security mode is set with the encryption parameter and can be one of four modes. 
	- To allow connections without a password set encryption parameter to 0 which is OPEN mode. 
	- To allow only users with a valid password, set encryption to 1, 2, 3 or 4. 
	- These encryption modes correspond to WEP, WPA-PSK, WPA2-PSK and WPA/WPA2-PSK with WPA2-PSK being the most secure and common for general use.
	
	DHCP:
	- DHCP can be set for different modes by setting the first parameter. 
	- Apply DHCP setting to AP mode only with 0, Station mode only with 1 or both modes with 2. 
	- The second parameter can be set to 0 to enable or 1 to disable DHCP for selected modes.
	
	For server:
	- Single connection mode is set with a 0 value while multiple connection mode is set with a value of 1. 
	- This mode can only be changed after all connections are disconnected. 
	- If server is started, reboot is required.

	*/
			

		//Button 1 - ATTENTION																			
		for(uint8_t i = 0; i < 6; i++){Send_UART(Attention[i]);}						//transmit message
		for(uint32_t a = 0; a < 5000000; a++){;}														//wait to not send message multiple times
			
		//Button 2 - RESET													
		for(uint8_t i = 0; i < 10; i++){Send_UART(Reset[i]);}								//transmit message
		for(uint32_t a = 0; a < 5000000; a++){;}														//wait to not send message multiple times
		
		//Button 3 - CONFIG ACCESS POINT																			
		for(uint8_t i = 0; i < 30; i++){Send_UART(Config_access_point[i]);}	//transmit message
		for(uint32_t a = 0; a < 5000000; a++){;}														//wait to not send message multiple times

		//Button 9 - Set IP address of Access Point																				
		for(uint8_t i = 0; i < 30; i++){Send_UART(Set_IP_address_of_Access_Point[i]);}		//transmit message
		for(uint32_t a = 0; a < 5000000; a++){;}														//wait to not send message multiple times			
			
		//Button 4 - SET CONNECTION MODE																				
		for(uint8_t i = 0; i < 15; i++){Send_UART(Set_connection_mode[i]);}	//transmit message
		for(uint32_t a = 0; a < 5000000; a++){;}														//wait to not send message multiple times					

		//Button 5 - SHOW FIRMWARE
		for(uint8_t i = 0; i < 11; i++){Send_UART(Show_Firmware_Version[i]);}//transmit message
		for(uint32_t a = 0; a < 5000000; a++){;}	 													 //wait to not send message multiple times

		//Button 6 - SET OPERATING MODE																				
		for(uint8_t i = 0; i < 15; i++){Send_UART(Set_Operating_Mode[i]);}	//transmit message
		for(uint32_t a = 0; a < 5000000; a++){;}														//wait to not send message multiple times		
		
		//Button 7 - CONFIGURE A SERVER																					
		for(uint8_t i = 0; i < 23; i++){Send_UART(Configure_Server[i]);}		//transmit message
		for(uint32_t a = 0; a < 5000000; a++){;}													//wait to not send message multiple times
	
	while(1)
	{

		//Button 8 - SEND MESSAGE																																
		for(uint8_t i = 0; i < 20; i++){Send_UART(Send_data_in_multiple_connection_mode[i]);}			//transmit message
		for(uint32_t a = 0; a < 40000; a++){;}																										//wait for ESP8266 to confirm before sending message
		for(uint8_t i = 0; i < 5; i++){Send_UART(z[i]);}	
		for(uint32_t a = 0; a < 5000000; a++){;}																									//wait to not send message multiple times	
		//for(uint32_t a = 0; a < 5000000; a++){;}
		//for(uint32_t a = 0; a < 5000000; a++){;}
			
		//Button 10 - close connection to browser																		
		for(uint8_t i = 0; i < 17; i++){Send_UART(Close_connection[i]);}		//transmit message
		for(uint32_t a = 0; a < 5000000; a++){;}													//wait to not send message multiple times						
	}
}
