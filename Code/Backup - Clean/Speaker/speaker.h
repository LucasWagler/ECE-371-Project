// Speaker module for ECE 371 motion alarm project
// Author: Lucas Wagler (ldwagler15@my.trine.edu)
// Updated: 2019-11-23 19:18
// Software-Derived Requirements:
//  J01-01/PTC7 --> Resistor --> Speaker
//  Speaker --> GND
//
// TODO: convert movement_flag to queue read

#ifndef SPEAKER_H
#define SPEAKER_H

#include <MKL25Z4.h>

// Speaker: Port C
#define SPEAKER_PIN 7

// for testing
uint8_t movement_flag = 1;

uint32_t tick_count = 0;
// uint8_t sec_count = 0;

void InitTpm1(void);
void TPM1_IRQHandler(void);
void InitPort(void);
void SoundOff(void);
void SoundOn(void);

// For use in main:
void InitSpeaker(void);
void SpeakerTask(void);

#endif
