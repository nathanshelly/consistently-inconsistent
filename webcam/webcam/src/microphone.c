/*
 * Created: 4/5/2017 20:55:00
 * Author: Nathan
 */ 


#include "microphone.h"



uint16_t wave_samples[NUMBER_OF_SAMPLES] = {0};
	
/** Receiver buffer for multisampling. */
volatile uint16_t multisample_buf[MULTISAMPLE_SIZE] = {0};
	
/** Audio buffer */
volatile uint8_t rec_buff[NUMBER_OF_SAMPLES] = {0};

/** Receive done flag. */
volatile uint8_t rec_done = 0;

/** Receive index. */
volatile uint32_t buf_index = 0;




void start_adc(void){
	pmc_enable_periph_clk(ID_ADC);
	
	uint32_t ADC_clk = 6000000;
	
	// ADC clk = 6Mhz, prescaler = 9 because MCK = 120MHz
	uint32_t temp_cpu = sysclk_get_cpu_hz();
	adc_init(ADC, sysclk_get_cpu_hz(), ADC_clk, ADC_STARTUP_TIME_4);
	
	memset((void *)&g_adc_sample_data, 0, sizeof(g_adc_sample_data));
	
	adc_configure_timing(ADC, TRACKING_TIME, ADC_SETTLING_TIME_3, TRANSFER_PERIOD);
	
	adc_enable_tag(ADC);
	
	adc_stop_sequencer(ADC);
	
	adc_enable_channel(ADC,MIC_CHANNEL); 
	
	g_adc_sample_data.uc_ch_num[0] = MIC_CHANNEL;
	
	adc_disable_anch(ADC);
	
	adc_set_channel_input_gain(ADC, MIC_CHANNEL, ADC_GAINVALUE_0);
	
	adc_disable_channel_input_offset(ADC,MIC_CHANNEL);
	
	adc_configure_power_save(ADC,0,0);
	
	// not using PDC channel interrupt (it looks too complicated)
	adc_enable_interrupt(ADC, ADC_IER_DRDY);
	
	NVIC_EnableIRQ(ADC_IRQn);
	
	adc_configure_trigger(ADC, ADC_TRIG_SW, 0);

}

void capture_audio_segment(void){
	
	buf_index = 0;
	
	while(buf_index < NUMBER_OF_SAMPLES){
		
		uint32_t multisample_index = 0;
		uint32_t multisample = 0;
		
		while (multisample_index < MULTISAMPLE_SIZE){ // take a number of samples (MULTISAMPLE_SIZE) to average for each point
			
			adc_start(ADC);
			
			if (g_adc_sample_data.us_done == ADC_DONE_MASK) {
				for (int i = 0; i < NUM_CHANNELS; i++) {
					uint32_t temp_chan_num = g_adc_sample_data.uc_ch_num[i];
					uint32_t temp_val = g_adc_sample_data.us_value[i];
					multisample += temp_val;
					multisample_index++;
				}
				g_adc_sample_data.us_done = 0;
			}
			
		}
		
		uint32_t avg_samp =	multisample / MULTISAMPLE_SIZE; 
		
		uint8_t compressed_samp;
		
		if (avg_samp < MIN_PICKUP){
			compressed_samp = 0;
		}else if (avg_samp > MAX_PICKUP){
			compressed_samp = 255;
		} else {
			compressed_samp = (uint8_t) (avg_samp-MIN_PICKUP);
		}
		
		rec_buff[buf_index++] = compressed_samp;
				
	}
	
	ioport_toggle_pin_level(LED_PIN);
	
}

static uint32_t adc_read_buffer(Adc * pADC, int16_t * multisample_buffer, uint32_t multisample_buffer_size)
{
	/* Check if the first PDC bank is free. */
	if ((pADC->ADC_RCR == 0) && (pADC->ADC_RNCR == 0)) {
		pADC->ADC_RPR = (uint32_t) multisample_buffer;
		pADC->ADC_RCR = multisample_buffer_size;
		pADC->ADC_PTCR = ADC_PTCR_RXTEN;

		return 1;
		} else {	/* Check if the second PDC bank is free. */
		if (pADC->ADC_RNCR == 0) {
			pADC->ADC_RNPR = (uint32_t) multisample_buffer;
			pADC->ADC_RNCR = multisample_buffer_size;

			return 1;
			} else {
			return 0;
		}
	}
}


/**
 * \brief Interrupt handler for the ADC.
 */
void ADC_Handler(void)
{
	uint32_t i;
	uint32_t ul_temp;
	uint8_t uc_ch_num;

    /* Without PDC transfer */
    if ((adc_get_status(ADC) & ADC_ISR_DRDY) == ADC_ISR_DRDY) {
        ul_temp = adc_get_latest_value(ADC);
        for (i = 0; i < NUM_CHANNELS; i++) {
            uc_ch_num = (ul_temp & ADC_LCDR_CHNB_Msk) >> ADC_LCDR_CHNB_Pos;
            if (g_adc_sample_data.uc_ch_num[i] == uc_ch_num) {
                g_adc_sample_data.us_value[i] = ul_temp & ADC_LCDR_LDATA_Msk;
                g_adc_sample_data.us_done |= 1 << i;
            }
        }
    }
	
}

///**
 //* \brief ADC interrupt handler.
 //*/
//void ADC_Handler(void)
//{
//
	//adc_disable_interrupt(ADC, ADC_ISR_RXBUFF);
	//
	//uint32_t multisample_sum = 0;
	//uint32_t sample_millivolts;
//
	//if ((adc_get_status(ADC) & ADC_ISR_RXBUFF) == ADC_ISR_RXBUFF) {
//
		///* Multisample */
		//for (int i = 0; i < MULTISAMPLE_SIZE; i++) {
			//multisample_sum += multisample_buf[i];
		//}
		///* Averaging */
		//uint32_t sample_val = multisample_sum / MULTISAMPLE_SIZE;
//
		//sample_millivolts = sample_val * VOLT_REF / MAX_DIGITAL;
//
		//rec_buff[buf_index++] = sample_millivolts;
		//
		//if(buf_index >= BUFFER_SIZE){
			//rec_done = 1;
			//return;
		//}
		////f_temp = (float)(l_vol - 800) * 0.37736 + 27.0;
//
		///* Clear the buffer. */
		//memset(multisample_buf, 0x0, MULTISAMPLE_SIZE);
		///* Start new pdc transfer. */
		//adc_read_buffer(ADC, multisample_buf, MULTISAMPLE_SIZE);
	//}
	//adc_enable_interrupt(ADC, ADC_ISR_RXBUFF);
//}

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