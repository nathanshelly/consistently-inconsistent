/*
 * Created: 4/5/2017 20:55:00
 * Author: Nathan
 */ 
#include "microphone.h"

/** Receive index. */
volatile uint32_t i2s_receive_index = 0;

/** Receiver buffer content. */
volatile uint16_t i2s_rec_buf[AUDIO_BUFFER_SIZE] = {0};
	
volatile uint8_t capture_toggle = 0;

void start_i2s_capture(void){ ssc_enable_interrupt(SSC, SSC_IDR_RXRDY); }

/**
 * \brief Synchronous Serial Controller Handler.
 */
void SSC_Handler(void)
{
	uint32_t ul_data;
	ssc_get_status(SSC);
	ssc_read(SSC, &ul_data);
	
	if(!(capture_toggle++ % 2))
		i2s_rec_buf[i2s_receive_index++] = modify_data(ul_data);

	if (i2s_receive_index >= AUDIO_BUFFER_SIZE)
		i2s_receive_index = 0;
}

// get rid of zero padding and tristated signal
uint16_t modify_data(uint32_t data_to_modify) { return (uint16_t) (data_to_modify >> 16); }

/**
 * \brief Set up clock.
 * \param p_ssc Pointer to an SSC instance.
 * \param ul_bitrate Desired bit clock.
 * \param ul_mck MCK clock.
*/
void configure_i2s(void){
	/* Initialize the SSC module and work in loop mode. */
	pmc_enable_periph_clk(ID_SSC);
	ssc_reset(SSC);
	uint32_t ul_mck = sysclk_get_cpu_hz();
	
	if (ssc_set_clock_divider(SSC, SSC_BIT_RATE, ul_mck) != SSC_RC_YES) { return 1; }
	
	// is2 setup
	ssc_i2s_set_receiver(SSC, SSC_I2S_MASTER_IN, SSC_RCMR_CKS_RK, SSC_AUDIO_MONO_LEFT, 32);

	/* Enable the rx function. */
	ssc_enable_rx(SSC);

	/* Enable SSC interrupt line from the core */
	NVIC_DisableIRQ(SSC_IRQn);
	NVIC_ClearPendingIRQ(SSC_IRQn);
	NVIC_SetPriority(SSC_IRQn, SSC_IRQ_PRIO);
	NVIC_EnableIRQ(SSC_IRQn);
}