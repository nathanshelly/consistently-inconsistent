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
#define MULTISAMPLE_SIZE 10
#define VOLT_REF 3300
#define MAX_DIGITAL 4095

/* Tracking Time*/
#define TRACKING_TIME    1
/* Transfer Period */
#define TRANSFER_PERIOD  2

// uint8_t input_buffer[BUFFER_SIZE];

uint16_t *generate_spoof(uint32_t tone_frequency);
void send_data_ws(uint16_t *samples_data);
void ADC_setup(void);
void audio_capture(void);
static uint32_t adc_read_buffer(Adc * pADC, int16_t * multisample_buffer, uint32_t multisample_buffer_size);
void ADC_Handler(void);

/** The SSC interrupt IRQ priority. */
#define SSC_IRQ_PRIO           4

#define OVER_SAMPLE_RATE			32
#define SAMPLE_RATE					32000
//#define SSC_BIT_RATE				(OVER_SAMPLE_RATE * SAMPLE_RATE)
#define SSC_BIT_RATE				(8 * 8000)

#define BIT_LEN_PER_CHANNEL			8

#endif /* MICROPHONE_H_ */