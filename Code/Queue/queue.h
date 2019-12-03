// Queue module for ECE 371 motion alarm project
// Author: Dr. Sean Carroll (carrolls@trine.edu)
// Updated: 2019-11-24 10:44

#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include  <stdlib.h>

typedef struct
{
    int16_t * buffer; 
    uint16_t head;
    uint16_t tail;
    uint16_t bufSize;
} queue_t;

//API
void init_q(queue_t* ptr_to_existing_q, int capacity);
bool get_q(queue_t *q0, uint8_t *datum);
bool put_q(queue_t *q0, uint8_t datum);

#endif
