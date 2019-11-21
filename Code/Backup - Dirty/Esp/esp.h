#ifndef ESP_H
#define ESP_H

#include <MKL25Z4.h>
#include <stdlib.h>
#include <stdbool.h>
#include "main.h"

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
extern const uint8_t Send_data[SENDSize];
extern const uint8_t message[MSGSize];

void config_UART(void);
void config_ESP(void);
void send_UART_TASK(void);
void send_UART_CONFIG(uint8_t msg);
void fillEspQ(void);

#endif
