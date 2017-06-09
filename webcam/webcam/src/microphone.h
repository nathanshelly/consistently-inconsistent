#ifndef MICROPHONE_H_
#define MICROPHONE_H_
#include <asf.h>
#include "conf_board.h"
#include "conf_clock.h"
#include "wifi.h"
#include "math.h"

#define AUDIO_BUFFER_SIZE	16000

volatile uint32_t i2s_capture_index;
volatile uint16_t i2s_rec_buf[AUDIO_BUFFER_SIZE];

void configure_i2s(void);
void start_i2s_capture(void);

/** The SSC interrupt IRQ priority. */
#define SSC_IRQ_PRIO			4

#define OVER_SAMPLE_RATE		64
#define SAMPLE_RATE				32000
#define SSC_BIT_RATE			OVER_SAMPLE_RATE * SAMPLE_RATE

#define BIT_LEN_PER_CHANNEL		8

/** RD pin definition */
#define RD_GPIO                 PIO_PA18_IDX
#define RD_FLAGS                PIO_PULLUP | PIO_IT_RISE_EDGE
#define RD_MASK                 PIO_PA18
#define RD_PIO                  PIOA
#define RD_ID                   ID_PIOA
#define RD_TYPE                 PIO_PULLUP

/** RF pin definition */
#define RF_GPIO                 PIO_PA19_IDX
#define RF_FLAGS                PIO_PERIPH_A | PIO_DEFAULT
#define RF_MASK                 PIO_PA19
#define RF_PIO                  PIOA
#define RF_ID                   ID_PIOA
#define RF_TYPE                 PIO_PULLUP

/** RK pin definition */
#define RK_GPIO                 PIO_PA20_IDX
#define RK_FLAGS                PIO_PERIPH_A | PIO_DEFAULT
#define RK_MASK                 PIO_PA20
#define RK_PIO                  PIOA
#define RK_ID                   ID_PIOA
#define RK_TYPE                 PIO_PULLUP

#endif /* MICROPHONE_H_ */