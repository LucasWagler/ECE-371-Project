// Main module for ECE 371 motion alarm project
// Authors: Alex Jasper (anjasper17@my.trine.edu)
//					Lucas Wagler (ldwagler15@my.trine.edu)
//					Jake Garlits (jtgarlits16@my.trine.edu)
// Updated: 2019-11-25 16:18
//
//TODO: Add in Accelerometer task


#include "main.h"

#define DOGMINITLENGTH ((PRINTLENGTH*3) + (DISPLAYLENGTH*3))

queue_t displayQ;
queue_t espQ;

void filler(void)
{
	uint8_t value = 0;
	if(get_q(&displayQ, &value))
	{
		if(value == 1)
		{
			for(int i = 0; i < PRINTLENGTH; i++)
			{
				prepData(printSeq[i], 1);
			}
		}
	}	
}

void accelerometer_task()
{
	//disable interrupts
	uint32_t mask;
	mask = __get_PRIMASK();
	__disable_irq();
	put_q(&displayQ, (uint8_t)1);
	put_q(&speakerQ, (uint8_t)1);
	put_q(&espQ, (uint8_t)1);	
	filler();
	//re-enable interrupts
	__set_PRIMASK(mask);
}

void esp_task()
{	
	//disable interrupts
	uint32_t mask;
	mask = __get_PRIMASK();
	__disable_irq();
	
	static bool send = false;	
	uint8_t espData = 0;
	if(get_q(&espQ, &espData))
	{
		if(espData == 1)
		{
			send = true;
		}
	}
	
	if(send)
	{
		for(int i = 0; i < SENDSize; i++)
		{
			send_UART(Send_data_in_multiple_connection_mode[i]);
		}
		for(uint32_t a = 0; a < 5000000; a++){;} //WAIT
		for(int j = 0; j < MSGSize; j++)
		{
			send_UART(message[j]);
		}
		for(uint32_t a = 0; a < 5000000; a++){;} //WAIT
		for(uint8_t k = 0; k < CLOSESize; k++)
		{
			send_UART(Close_connection[k]);
		}
		for(uint32_t a = 0; a < 5000000; a++){;} //WAIT
		send = false;
	}	
	
	//re-enable interrupts
	__set_PRIMASK(mask);
}

int main()
{
	//Queue Setup
	init_q(&displayQ, (int)q_capacity);
	init_q(&speakerQ, (int)q_capacity);
	init_q(&espQ, (int)q_capacity);
	init_q(&spiQ, (int)spi_capacity);
	
	//Config Display
	config_spi();	
	resetDOGM();
	initDOGM();
	clearDOGM();
	for(int i = 0; i < DOGMINITLENGTH; i++)
	{
		display_task();
	}
		
	//ESP Config
	config_UART();
	config_ESP();
	
	//Speaker Config
	InitSpeaker();
		
	//accelerometer_task();
	//Wait for initial wifi connection
	for(int i = 0; i < 90; i++)
	{
		for(uint64_t i = 0; i < 0xFFFFF; i++){;}
	}
	
	while(1)
	{		
		static bool virgin = true;
		if(virgin)
		{
			accelerometer_task();
			virgin = false;
		}	

		esp_task();
		speaker_task();		
		display_task();
	}	
}
