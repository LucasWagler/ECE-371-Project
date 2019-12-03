// Speaker module for ECE 371 motion alarm project
// Author: Lucas Wagler (ldwagler15@my.trine.edu)
// Updated: 2019-12-03 16:52
// Software-Derived Requirements:
//  J01-01/PTC7 --> Resistor --> Speaker
//  Speaker --> GND


#ifndef SPEAKER_H
#define SPEAKER_H

#include <MKL25Z4.h>
#include <stdint.h>
#include <stdbool.h>

#include "queue.h"

// Speaker: Port C
#define SPEAKER_PIN 7

//API
extern queue_t speakerQ;

void InitTpm1(void);
void TPM1_IRQHandler(void);
void InitPort(void);
void SoundOff(void);
void SoundOn(void);

// For use in main:
void InitSpeaker(void);
void speaker_task(void);

#endif
