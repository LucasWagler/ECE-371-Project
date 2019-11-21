#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>

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
void config_spi(void);
void resetDOGM(void);
void clearDOGM(void);
void sendData(uint8_t value);
void prepData(uint8_t hexValue, uint8_t cod);

#endif
