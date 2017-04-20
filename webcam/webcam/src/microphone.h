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

#define NUMBER_OF_SAMPLES 10000
#define MULTISAMPLE_SIZE 5
#define VOLT_REF 3300
#define MAX_DIGITAL 4095
#define MIN_PICKUP 696 // 824 is center counts for the mic, so 824-128 gives the min value
#define MAX_PICKUP 952 // same logic
#define ADC_DONE_MASK   ( (1<<NUM_CHANNELS) - 1 )
#define NUM_CHANNELS 1
#define MIC_CHANNEL ADC_CHANNEL_9
/* Tracking Time*/
#define TRACKING_TIME    1
/* Transfer Period */
#define TRANSFER_PERIOD  2

// uint8_t input_buffer[BUFFER_SIZE];

uint16_t *generate_spoof(uint32_t tone_frequency);
void send_data_ws(uint16_t *samples_data);
void start_adc(void);
void capture_audio_segment(void);
static uint32_t adc_read_buffer(Adc * pADC, int16_t * multisample_buffer, uint32_t multisample_buffer_size);
void ADC_Handler(void);


struct {
	uint8_t uc_ch_num[NUM_CHANNELS];
	uint16_t us_value[NUM_CHANNELS];
	uint16_t us_done;
} g_adc_sample_data;


/** The SSC interrupt IRQ priority. */
#define SSC_IRQ_PRIO				4

#define OVER_SAMPLE_RATE			32
#define SAMPLE_RATE					32000
//#define SSC_BIT_RATE				(OVER_SAMPLE_RATE * SAMPLE_RATE)
#define SSC_BIT_RATE				(8 * 8000)

#define BIT_LEN_PER_CHANNEL			8

#endif /* MICROPHONE_H_ */