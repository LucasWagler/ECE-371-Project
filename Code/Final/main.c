#include "main.h"
#define DOGMINITLENGTH ((PRINTLENGTH*3) + (DISPLAYLENGTH*3))

queue_t displayQ; 
queue_t speakerQ;
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
	put_q(&displayQ, (uint8_t)0);	
	//put_q(&espQ, (uint8_t)1);	
	filler();
}

void esp_task()
{
	uint8_t espData = 0;
	if(get_q(&espQ, &espData))
	{
		if(espData == 1)
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
		}
	}
}

void speaker_task()
{
	
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
	
	accelerometer_task();
	
	while(1)
	{
		
		//put_q(&displayQ, (int16_t)0);	
		//put_q(&espQ, (int16_t)0);
		//speaker_task();
		//esp_task();
		display_task();	
	}	
}
