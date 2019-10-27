#ifndef QUEUE_H
#define QUEUE_H
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include  <stdlib.h> // malloc
//#define DEBUG
typedef struct
{
    int16_t * buffer; 
    uint16_t head;
    uint16_t tail;
    uint16_t bufSize;
} queue_t;
// ACQUIRE BUFFER USING MALLOC, AND ONLY USE POINTERS
void init_queue(queue_t * ptr_to_existing_q, int capacity)
{
    int16_t * new_buff = (int16_t *) malloc((capacity + 1) * sizeof(int16_t));
    ptr_to_existing_q ->buffer = new_buff;
    ptr_to_existing_q ->head = 0;
    ptr_to_existing_q ->tail = 0;
    ptr_to_existing_q ->bufSize = capacity + 1;
}

//API
bool get_q(queue_t *q0, int16_t *datum);
bool put_q(queue_t *q0, int16_t datum);
queue_t x;
void init_q(queue_t* q0, int16_t capacity); // Can hold (capacity) integers
#endif

///////////////////////////////////////////////////////////////////

bool get_q(queue_t *q0, int16_t *datum)
{
    // writable parameter = datum;
    #ifdef DEBUG
        printf("Head @ %d, Tail @ %d\n", (int)(q0->head), (int)(q0->tail));
    #endif
    bool success = false;
    if (q0->head == q0->tail){ // empty
        *datum = 0;
        success = false; //{datum==0,success == false}
    }
    else
    {
        *datum = *((q0->buffer) + q0->head); // access as *(ptr + int). See put_q
        q0->head++;
        if (q0->head >= q0->bufSize) q0->head=0;
        success = true;
    }
    return success;
}

bool put_q(queue_t *q0, int16_t datum)
{
    #ifdef DEBUG
        printf("Head @ %d, Tail @ %d\n", (int)(q0->head), (int)(q0->tail));
    #endif
    uint16_t new_tail = q0->tail + 1;
    if (new_tail >= q0->bufSize) new_tail = 0;
    if (new_tail == q0->head) return false; //full-error
    else
    {
        (q0->buffer)[q0->tail] = datum;  // access as (ptr)[int] see read_q
        q0->tail =new_tail;
    }
    return true;
}
