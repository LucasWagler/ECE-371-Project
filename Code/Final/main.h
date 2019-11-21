#ifndef MAIN_H
#define MAIN_H

#include <MKL25Z4.h>
#include <stdio.h>
#include <stdint.h>
#include "queue.h"
#include "display.h"
#include "esp.h"
#define q_capacity 1

int16_t displayData = 0;
int16_t espData = 0;

queue_t displayQ; 
queue_t speakerQ;
queue_t espQ;

#endif
