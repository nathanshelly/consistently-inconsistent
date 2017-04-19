/*
 * Created: 4/5/2017 20:55:00
 * Author: Nathan
 */ 


#include "microphone.h"
uint16_t wave_samples[NUMBER_OF_SAMPLES] = {0};
	
/** Receiver buffer for multisampling. */
volatile uint16_t multisample_buf[MULTISAMPLE_SIZE] = {0};
	
/** Audio buffer */
volatile uint32_t rec_buff[NUMBER_OF_SAMPLES] = {0};

/** Receive done flag. */
volatile uint8_t rec_done = 0;

/** Receive index. */
volatile uint32_t buf_index = 0;


void ADC_setup(void){
	pmc_enable_periph_clk(ID_ADC);
	
	uint32_t ADC_clk = 6000000;
	
	// ADC clk = 6Mhz, prescaler = 9 because MCK = 120MHz
	adc_init(ADC, sysclk_get_cpu_hz(), ADC_clk, ADC_STARTUP_TIME_4);
	
	adc_configure_timing(ADC, TRACKING_TIME, ADC_SETTLING_TIME_3, TRANSFER_PERIOD);
	
	adc_configure_trigger(ADC, ADC_TRIG_SW, 0);

	//adc_check(ADC, sysclk_get_cpu_hz());

	/* Enable channel for potentiometer. */
	adc_enable_channel(ADC, ADC_CHANNEL_8);

	/* Enable ADC interrupt. */
	NVIC_EnableIRQ(ADC_IRQn);
	
	adc_start(ADC);
	
	adc_read_buffer(ADC, multisample_buf, MULTISAMPLE_SIZE);
	
}

void audio_capture(void){
	rec_done = 0;
	buf_index = 0;
	
	adc_enable_interrupt(ADC, ADC_ISR_RXBUFF);
	
	while (!rec_done){
		blink_LED(200);
	}
	
	adc_disable_interrupt(ADC,ADC_ISR_RXBUFF);
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
 * \brief ADC interrupt handler.
 */
void ADC_Handler(void)
{

	adc_disable_interrupt(ADC, ADC_ISR_RXBUFF);
	
	uint32_t multisample_sum = 0;
	uint32_t sample_millivolts;

	if ((adc_get_status(ADC) & ADC_ISR_RXBUFF) == ADC_ISR_RXBUFF) {

		/* Multisample */
		for (int i = 0; i < MULTISAMPLE_SIZE; i++) {
			multisample_sum += multisample_buf[i];
		}
		/* Averaging */
		uint32_t sample_val = multisample_sum / MULTISAMPLE_SIZE;

		sample_millivolts = sample_val * VOLT_REF / MAX_DIGITAL;

		rec_buff[buf_index++] = sample_millivolts;
		
		if(buf_index >= BUFFER_SIZE){
			rec_done = 1;
			return;
		}
		//f_temp = (float)(l_vol - 800) * 0.37736 + 27.0;

		/* Clear the buffer. */
		memset(multisample_buf, 0x0, MULTISAMPLE_SIZE);
		/* Start new pdc transfer. */
		adc_read_buffer(ADC, multisample_buf, MULTISAMPLE_SIZE);
	}
	adc_enable_interrupt(ADC, ADC_ISR_RXBUFF);
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