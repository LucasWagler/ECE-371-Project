// ESP8266 module for ECE 371 motion alarm project
// Author: Alex Jasper (anjasper17@my.trine.edu)
// Updated: 2019-11-24 10:44
// Software-Derived Requirements:
//	Wire ESP8266 Module Pin - 1 to Jumper 9 - Pin 14 on the Freedom Board (Ground)
//	Wire ESP8266 Module Pin - 4 to Jumper 9 - Pin 8 on the Freedom Board (3.3V)
//	Wire ESP8266 Module Pin - 8 to Jumper 9 - Pin 8 on the Freedom Board (3.3V)
//	Wire ESP8266 Module Pin - 7 to Jumper 1 - Pin 4 on the Freedom Board (Rx to Tx)
//	Wire ESP8266 Module Pin - 2 to Jumper 1 - Pin 2 on the Freedom Board (Tx to Rx)

#ifndef ESP_H
#define ESP_H

#include <MKL25Z4.h>
#include <stdlib.h>

#define SYS_CLOCK (48e6)
#define OVERSAMPLE (16)
#define BAUD_RATE (115200)
#define ATSize 4
#define RSTSize 10
#define SAPSize 30
#define MODESize 15
#define CLOSESize 17
#define PAPSize 30
#define SERVERSize 23
#define MUXSize 15
#define SENDSize 20
#define MSGSize 5

extern const uint8_t Close_connection[CLOSESize];
extern const uint8_t Send_data_in_multiple_connection_mode[SENDSize];
extern const uint8_t message[MSGSize];

void config_UART(void);
void config_ESP(void);
void send_UART(uint8_t msg);

#endif
