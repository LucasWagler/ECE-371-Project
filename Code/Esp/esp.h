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

void config_UART(void);
void config_ESP(void);
void send_UART(uint8_t msg);

extern uint8_t Attention[ATSize];
extern uint8_t Reset[RSTSize];
extern uint8_t Config_access_point[SAPSize];
extern uint8_t Set_Operating_Mode[MODESize];
extern uint8_t Close_connection[CLOSESize];
extern uint8_t Set_IP_address_of_Access_Point[PAPSize];
extern uint8_t Configure_Server[SERVERSize];
extern uint8_t Set_connection_mode[MUXSize];
extern uint8_t Send_data_in_multiple_connection_mode[SENDSize];
extern uint8_t message[MSGSize];

extern int16_t espData;

#endif
