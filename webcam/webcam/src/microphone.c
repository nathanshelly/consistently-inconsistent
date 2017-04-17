/*
 * Created: 4/5/2017 20:55:00
 * Author: Nathan
 */ 


#include "microphone.h"
uint16_t wave_samples[NUMBER_OF_SAMPLES] = {0};
	
/** Receiver buffer content. */
uint32_t i2s_rec_buf[NUMBER_OF_SAMPLES];

/** Receive done flag. */
volatile uint8_t i2s_rec_done = 0;

/** Receive index. */
uint32_t i2s_buf_index = 0;


void i2s_capture(void){
	
	i2s_rec_done = 0;
	i2s_buf_index = 0;
	
	ssc_enable_interrupt(SSC, SSC_IDR_RXRDY);
	
	while (!i2s_rec_done){
		
	}
	
	blink_LED(100);
	
}

/**
 * \brief Synchronous Serial Controller Handler.
 *
 */
void SSC_Handler(void)
{
	uint32_t ul_data;
	ssc_get_status(SSC);

	ssc_read(SSC, &ul_data);
	i2s_rec_buf[i2s_buf_index++] = ul_data;

	if (NUMBER_OF_SAMPLES == i2s_buf_index) {
		i2s_rec_done = 1;
		ssc_disable_interrupt(SSC, SSC_IDR_RXRDY);
	}
}


void configure_i2s(void){
	
	
	/**
 * \brief Set up clock.
 *
 * \param p_ssc Pointer to an SSC instance.
 * \param ul_bitrate Desired bit clock.
 * \param ul_mck MCK clock.
*/
	/* Initialize the SSC module and work in loop mode. */
	pmc_enable_periph_clk(ID_SSC);
	ssc_reset(SSC);
	uint32_t ul_mck = sysclk_get_cpu_hz();
	
	if (!ssc_set_clock_divider(SSC, SSC_BIT_RATE, ul_mck) == SSC_RC_YES){
		return 1;
	}
	
	// ssc setup
	
	clock_opt_t rx_clk_option;
	data_frame_opt_t rx_data_frame_option;
	///* Receiver clock mode configuration. */
	//rx_clk_option.ul_cks = SSC_RCMR_CKS_RK;
	//rx_clk_option.ul_cko = SSC_RCMR_CKO_NONE;
	//rx_clk_option.ul_cki = 0;
	//rx_clk_option.ul_ckg = 0;
	//rx_clk_option.ul_period = 0;
	//rx_clk_option.ul_sttdly = 0;
	//rx_clk_option.ul_start_sel = SSC_RCMR_START_RF_EDGE;
	///* Receiver frame mode configuration. */
	//rx_data_frame_option.ul_datlen = 24;
	//rx_data_frame_option.ul_msbf = SSC_TFMR_MSBF;
	//rx_data_frame_option.ul_datnb = 15;
	//rx_data_frame_option.ul_fslen = 15;
	//rx_data_frame_option.ul_fslen_ext = 15;
	//rx_data_frame_option.ul_fsos = SSC_RFMR_FSOS_TOGGLING;
	//rx_data_frame_option.ul_fsedge = SSC_TFMR_FSEDGE_POSITIVE;
	///* Configure the SSC receiver. */
	//ssc_set_receiver(SSC, &rx_clk_option, &rx_data_frame_option);
	//
	// is2 setup
	ssc_i2s_set_receiver(SSC, SSC_I2S_MASTER_IN, SSC_RCMR_CKS_RK, SSC_AUDIO_STERO, 32);

	/* Enable the tx and rx function. */
	ssc_enable_rx(SSC);
	//ssc_enable_tx(SSC);

	/* Configure the RX interrupt. */
	//ssc_enable_interrupt(SSC, SSC_IER_RXRDY);

	/* Enable SSC interrupt line from the core */
	NVIC_DisableIRQ(SSC_IRQn);
	NVIC_ClearPendingIRQ(SSC_IRQn);
	NVIC_SetPriority(SSC_IRQn, SSC_IRQ_PRIO);
	NVIC_EnableIRQ(SSC_IRQn);

}


// generates sine wave with given number of samples at given frequency
uint16_t *generate_spoof(uint32_t tone_frequency) {
	int sampling_frequency = 32000;
	
	for (int i = 0; i < NUMBER_OF_SAMPLES; i++)
		wave_samples[i] = (uint16_t) (signed short) (32767*sinf(2*M_PI*tone_frequency*i/sampling_frequency));
	
	return wave_samples;
}

void send_data_ws(uint16_t* samples_data) {
	uint8_t curr_point;
	
	write_wifi_command("close all\r\n", 2);
	write_wifi_command("websocket_client wss://bigbrothersees.me/source_audio_socket\r\n", 2);

	char* templated_command[35];
	sprintf(templated_command, "write 0 %d\r\n", 12); // each sample is 32 bits, 4 bytes
	usart_write_line(BOARD_USART, templated_command);
	
	usart_putchar(BOARD_USART,'"');
	
	for (int i = 0; i < 10; i++) 
	{
		curr_point = (uint8_t) samples_data[i];
		if (curr_point > 127){
			curr_point = 100;
		}
		usart_putchar(BOARD_USART, curr_point);
	}
	
	usart_putchar(BOARD_USART,'"');
	
}