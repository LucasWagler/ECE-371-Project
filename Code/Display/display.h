// Main module for ECE 371 motion alarm project
// Authors: Alex Jasper (anjasper17@my.trine.edu)
//					Lucas Wagler (ldwagler15@my.trine.edu)
// Updated: 2019-11-24 10:43
// Software-Derived Requirements:
//	Wire a 1uF capacitor from DOGM Pin - 23 to Jumper 9 - Pin 14 on the Freedom Board (Ground)
//	Wire a 1uF capacitor from DOGM Pin - 24 to Jumper 9 - Pin 14 on the Freedom Board (Ground)
//	Wire a 1uF capacitor from DOGM Pin - 25 to Jumper 9 - Pin 14 on the Freedom Board (Ground)
//	Wire a 1uF capacitor from DOGM Pin - 26 to Jumper 9 - Pin 14 on the Freedom Board (Ground)
//	Wire a 1uF capacitor from DOGM Pin - 27 to Jumper 9 - Pin 14 on the Freedom Board (Ground)
//	Wire a 1uF capacitor from DOGM Pin - 28 to Jumper 9 - Pin 14 on the Freedom Board (Ground)
//	Wire DOGM Pin - 29 to Jumper 9 - Pin 14 on the Freedom Board (Ground)
//	Wire DOGM Pin - 30 to Jumper 9 - Pin 8 on the Freedom Board (3.3V)
//	Wire a 1uF capacitor from DOGM Pin - 31 to DOGM Pin - 32
//	Wire DOGM Pin - 31 to Jumper 9 - Pin 8 on the Freedom Board (3.3V)
//	Wire DOGM Pin - 32 to Jumper 9 - Pin 14 on the Freedom Board (Ground)
//	Wire DOGM Pin - 39 to Jumper 1 - Pin 11 on the Freedom Board (MOSI)
//	Wire DOGM Pin - 40 to Jumper 1 - Pin 9 on the Freedom Board (SCLK)
//	Wire DOGM Pin - 41 to Jumper 9 - Pin 8 on the Freedom Board (3.3V)
//	Wire DOGM Pin - 42 to Jumper 9 - Pin 8 on the Freedom Board (3.3V)
//	Wire DOGM Pin - 43 to Jumper 9 - Pin 8 on the Freedom Board (3.3V)
//	Wire DOGM Pin - 44 to Jumper 1 - Pin 14 on the Freedom Board (Reset)

#ifndef DISPLAY_H
#define DISPLAY_H

#include <MKL25Z4.h>
#include <stdint.h>
#include "main.h"
#include "queue.h"

#define SCK (5)
#define MOSI (6)
#define SPI_MUX (2)
#define GPIO_MUX (1)
#define CS (7)
#define RESET (8)
#define PRINTLENGTH 26
#define INITLENGTH 11
#define DISPLAYLENGTH 80

//API
extern const uint8_t dogm_init_seq[INITLENGTH];
extern const uint8_t printSeq[PRINTLENGTH];
extern queue_t spiQ;

void config_spi(void);
void resetDOGM(void);
void clearDOGM(void);
void initDOGM(void);
void display_task(void);
void prepData(uint8_t hexValue, uint8_t cod);

#endif
