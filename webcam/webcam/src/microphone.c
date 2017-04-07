/*
 * Created: 4/5/2017 20:55:00
 * Author: Nathan
 */ 


#include "microphone.h"
uint16_t wave_samples[NUMBER_OF_SAMPLES] = {0};

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