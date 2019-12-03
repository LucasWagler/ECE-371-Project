// Main module for ECE 371 motion alarm project
// Author: Alex Jasper (anjasper17@my.trine.edu)
// Updated: 2019-11-24 10:32

#ifndef MAIN_H
#define MAIN_H

#include <MKL25Z4.h>
#include <stdio.h>
#include <stdint.h>

#include "queue.h"
#include "display.h"
#include "esp.h"
#include "speaker.h"

#define q_capacity 1
#define spi_capacity 2000

extern queue_t movedQ;

#endif
