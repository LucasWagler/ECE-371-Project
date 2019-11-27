// Main module for ECE 371 motion alarm project
// Authors: Alex Jasper (anjasper17@my.trine.edu)
//					Lucas Wagler (ldwagler15@my.trine.edu)
//					Jake Garlits (jtgarlits16@my.trine.edu)
// Updated: 2019-11-27 17:15
//
//TODO: Add in Accelerometer task

#include "main.h"

#define DOGMINITLENGTH ((PRINTLENGTH*3) + (DISPLAYLENGTH*3))

queue_t movedQ;

void filler(void)
{
	uint8_t value = 0;
	if(get_q(&movedQ, &value))
	{
		if(value == 1)
		{
			for(int i = 0; i < PRINTLENGTH; i++)
			{
				prepData(printSeq[i], 1);
			}
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
		}
	}	
}

void accelerometer_task()
{
	//disable interrupts
	uint32_t mask;
	mask = __get_PRIMASK();
	__disable_irq();
	put_q(&movedQ, (uint8_t)1);
	put_q(&speakerQ, (uint8_t)1);
	filler();
	//re-enable interrupts
	__set_PRIMASK(mask);
}

int main()
{
	//Queue Setup
	init_q(&movedQ, (int)q_capacity);
	init_q(&speakerQ, (int)q_capacity);
	init_q(&spiQ, (int)spi_capacity);
	init_q(&espSendQ, (int)SENDSize);
	init_q(&espMsgQ, (int)MSGSize);
	init_q(&espCloseQ, (int)CLOSESize);
	
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
		
	accelerometer_task();
	//Wait for initial wifi connection
	for(int i = 0; i < 90; i++)
	{
		for(uint64_t i = 0; i < 0xFFFFF; i++){;}
	}
	
	while(1)
	{		
		static bool virgin = false;
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
