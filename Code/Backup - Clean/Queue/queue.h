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
bool get_q(queue_t *q0, int16_t *datum);
bool put_q(queue_t *q0, int16_t datum);

#endif
