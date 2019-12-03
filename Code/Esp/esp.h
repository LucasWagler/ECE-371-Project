// ESP8266 module for ECE 371 motion alarm project
// Author: Alex Jasper (anjasper17@my.trine.edu)
// Updated: 2019-12-03 16:52
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

#include "queue.h"
#include "main.h"

#define SYS_CLOCK (48e6)
#define OVERSAMPLE (16)
#define BAUD_RATE (115200)
#define ATSize 4
#define RSTSize 8
#define SAPSize 22
#define MODESize 13
#define CLOSESize 15
#define PAPSize 26
#define SERVERSize 19
#define MUXSize 13
#define SENDSize 16
#define MSGSize 5

extern queue_t espSendQ;
extern queue_t espMsgQ;
extern queue_t espCloseQ;

extern const uint8_t Close[CLOSESize];
extern const uint8_t Send[SENDSize];
extern const uint8_t message[MSGSize];

void config_UART(void);
void config_ESP(void);
void send_UART_Config(uint8_t msg);
void esp_task(void);

#endif
