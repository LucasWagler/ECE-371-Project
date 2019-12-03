// Accelerometer module for ECE 371 motion alarm project
// Authors: Jake Garlits (jtgarlits16@my.trine.edu)
//					Alex Jasper (anjasper17@my.trine.edu)
// Updated: 2019-11-24 10:43
// Software-Derived Requirements:
//	Wire a 4.7kOhm resistor from Jumper 9 - Pin 8 on the Freedom Board (3.3V) to Jumper 10 - Pin 4 on the Freedom Board (I2C_SDA)
//	Wire a 4.7kOhm resistor from Jumper 9 - Pin 8 on the Freedom Board (3.3V) to Jumper 10 - Pin 6 on the Freedom Board (I2C_SCL)
//	Wire MMA8451 Pin - SDA between resistor and I2C_SDA
//	Wire MMA8451 Pin - SCL between resistor and I2C_SCL
//	Wire MMA8451 Pin - VIN to Jumper 9 - Pin 8 on the Freedom Board (3.3V)
//	Wire MMA8451 Pin - GND to Jumper 9 - Pin 14 on the Freedom Board (Ground)

#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <MKL25Z4.H>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "queue.h"
#include "main.h"

//configure I2C control register 1; I2C0->C1 to select master mode 
//(for details, see I2C register mask in mkl25z4.h file)
#define MMA_ADDR 0x3A //Accel. device address 0x3A

#define I2C_M_START 	I2C0->C1 |= I2C_C1_MST_MASK                     // Select Mater Mode (Start) logic '1' indicates START otherwise STOP
#define I2C_M_STOP 		I2C0->C1 &= ~I2C_C1_MST_MASK  	                                          // Select Master Mode (Stop) 
#define I2C_M_RSTART 	I2C0->C1 |= I2C_C1_RSTA_MASK                    // Select Repeat Start

//configure I2C register "I2C_C1_TX_MASK" to allow I2C bus to transmit sensor data; logic '1' to transmit mode, otherwise receiving mode 
#define I2C_TRAN			I2C0->C1 |= I2C_C1_TX_MASK                      // I2C bus to Transmit
#define I2C_REC 			I2C0->C1 &= ~I2C_C1_TX_MASK		                                          //I2C bus to receive

#define BUSY_ACK 	    while(I2C0->S & 0x01)                            //I2C Bus Busy
#define TRANS_COMP		while(!(I2C0->S & 0x80))                         //I2C Bus data transafer Complete
																																			 //Enable Interrupt waiting flag
#define I2C_WAIT			while((I2C0->S & I2C_S_IICIF_MASK)==0) {} \
                                 I2C0->S |= I2C_S_IICIF_MASK;
	
#define NACK 	        I2C0->C1 |= I2C_C1_TXAK_MASK                      //Acknolegement of data NOT received
#define ACK           I2C0->C1 &= ~I2C_C1_TXAK_MASK                     //Acknolegement of data received
	
#define RED_LED_POS (18)		// on port B
#define GREEN_LED_POS (19)	// on port B
#define BLUE_LED_POS (1)		// on port D

#define MASK(x) (1UL << (x))
	
#define DELAYVALUE 10
#define SENSITIVITY 5
	
//API
void Init_RGB_LEDs(void);
void Control_RGB_LEDs(unsigned int red_on, unsigned int green_on, unsigned int blue_on);
void i2c_init(void);
void i2c_start(void);
uint8_t i2c_read_byte(uint8_t dev, uint8_t address);
void i2c_write_byte(uint8_t dev, uint8_t address, uint8_t data);
void Delay(uint32_t dly);
void accelerometer_task(void);
void filler(void);

#endif
