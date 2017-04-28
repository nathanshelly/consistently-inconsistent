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

#define NUMBER_OF_SAMPLES 200
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

volatile uint16_t i2s_rec_buf[NUMBER_OF_SAMPLES];

uint16_t *generate_spoof(uint32_t tone_frequency);
void start_adc(void);
void capture_audio_segment(void);
static uint32_t adc_read_buffer(Adc * pADC, int16_t * multisample_buffer, uint32_t multisample_buffer_size);
uint16_t modify_data(uint32_t data_to_modify);
void i2s_capture(void);
void configure_i2s(void);
void ADC_Handler(void);

struct {
	uint8_t uc_ch_num[NUM_CHANNELS];
	uint16_t us_value[NUM_CHANNELS];
	uint16_t us_done;
} g_adc_sample_data;

/** The SSC interrupt IRQ priority. */
#define SSC_IRQ_PRIO				4

#define OVER_SAMPLE_RATE			64
#define SAMPLE_RATE					32000
#define SSC_BIT_RATE				(OVER_SAMPLE_RATE * SAMPLE_RATE)
//#define SSC_BIT_RATE				(8 * 8000)

#define BIT_LEN_PER_CHANNEL			8

/** RD pin definition */
#define RD_GPIO                  PIO_PA18_IDX
#define RD_FLAGS                 (PIO_PULLUP | PIO_IT_RISE_EDGE)
#define RD_MASK                  PIO_PA18
#define RD_PIO                   PIOA
#define RD_ID                    ID_PIOA
#define RD_TYPE                  PIO_PULLUP

/** RF pin definition */
#define RF_GPIO                  PIO_PA19_IDX
#define RF_FLAGS                 (PIO_PERIPH_A | PIO_DEFAULT)
#define RF_MASK                  PIO_PA19
#define RF_PIO                   PIOA
#define RF_ID                    ID_PIOA
#define RF_TYPE                  PIO_PULLUP

/** RK pin definition */
#define RK_GPIO                  PIO_PA20_IDX
#define RK_FLAGS                 (PIO_PERIPH_A | PIO_DEFAULT)
#define RK_MASK                  PIO_PA20
#define RK_PIO                   PIOA
#define RK_ID                    ID_PIOA
#define RK_TYPE                  PIO_PULLUP

#endif /* MICROPHONE_H_ */