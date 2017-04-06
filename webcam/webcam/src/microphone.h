/*
 * microphone.h
 *
 * Created: 4/5/2017 20:53:18
 *  Author: Nathan
 */ 


#ifndef MICROPHONE_H_
#define MICROPHONE_H_
#include <asf.h>
#include "conf_board.h"
#include "conf_clock.h"
#include "wifi.h"
#include "math.h"

#define NUMBER_OF_SAMPLES 1000
// uint8_t input_buffer[BUFFER_SIZE];

char *generate_spoof(int tone_frequency);
void send_data(uint8_t *samples_data);

#endif /* MICROPHONE_H_ */