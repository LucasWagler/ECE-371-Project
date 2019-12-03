// Queue module for ECE 371 motion alarm project
// Author: Dr. Sean Carroll (carrolls@trine.edu)

#include "queue.h"

void init_q(queue_t * ptr_to_existing_q, int capacity)
{
    int16_t * new_buff = (int16_t *) malloc((capacity + 1) * sizeof(int16_t));
    ptr_to_existing_q ->buffer = new_buff;
    ptr_to_existing_q ->head = 0;
    ptr_to_existing_q ->tail = 0;
    ptr_to_existing_q ->bufSize = capacity + 1;
}

bool get_q(queue_t *q0, uint8_t *datum)
{
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

bool put_q(queue_t *q0, uint8_t datum)
{
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

bool peek_q(queue_t *q0)
{
    bool success = false;
    if (q0->head == q0->tail){ // empty
        success = false; //{datum==0,success == false}
    }
    else
    {
        success = true;
    }
    return success;
}
