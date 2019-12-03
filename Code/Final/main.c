// Main module for ECE 371 motion alarm project
// Authors: Alex Jasper (anjasper17@my.trine.edu)
//					Lucas Wagler (ldwagler15@my.trine.edu)
//					Jake Garlits (jtgarlits16@my.trine.edu)
// Updated: 2019-12-03 16:52

#include "main.h"

bool filled = false;

#define DOGMINITLENGTH ((PRINTLENGTH*3) + (DISPLAYLENGTH*3))

int main()
{
	//Queue Config
	init_q(&speakerQ, (int)q_capacity);
	init_q(&spiQ, (int)spi_capacity);
	init_q(&espSendQ, (int)SENDSize);
	init_q(&espMsgQ, (int)MSGSize);
	init_q(&espCloseQ, (int)CLOSESize);
	
	//Accelerometer Config
	uint8_t CTRL_REG1_Data;
	Init_RGB_LEDs();
	Control_RGB_LEDs(0,0,0);
	i2c_init();
	i2c_write_byte(0x00, 0x00, 0x00); //Send out pilot signal
	Delay(DELAYVALUE);	
	i2c_write_byte(0x00, 0x00, 0x00); //Send out pilot signal
	Delay(DELAYVALUE);	
	i2c_write_byte(MMA_ADDR, 0x2A, 0x18); //Set the device in 100 Hz ODR, Standby
	Delay(DELAYVALUE);
	CTRL_REG1_Data = i2c_read_byte(MMA_ADDR, 0x2A);
	CTRL_REG1_Data |= 0x01;
	Delay(DELAYVALUE);
	i2c_write_byte(MMA_ADDR, 0x2A, CTRL_REG1_Data);
	Delay(DELAYVALUE);
	uint8_t IntSourceMFF = i2c_read_byte(MMA_ADDR, 0x0C);
	Delay(DELAYVALUE);
	IntSourceMFF = i2c_read_byte(MMA_ADDR, 0x16);
	Delay(DELAYVALUE);
	
	//Display Config
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

	//Wait for initial wifi connection
	for(int i = 0; i < 90; i++)
	{
		for(uint64_t i = 0; i < 0xFFFFF; i++){;}
	}
	
	while(1)
	{
		accelerometer_task();
		esp_task();
		speaker_task();		
		display_task();
	}	
}
