#ifndef MAIN_H
#define MAIN_H

#include <MKL25Z4.h>
#include <stdio.h>
#include <stdint.h>
#include "queue.h"
#include "display.h"
#include "esp.h"
#define q_capacity 1

extern queue_t displayQ; 
extern queue_t speakerQ;
extern queue_t espQ;

#endif
