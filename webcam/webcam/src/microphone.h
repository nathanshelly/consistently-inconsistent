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