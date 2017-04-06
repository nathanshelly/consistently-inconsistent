/*
 * Created: 4/5/2017 20:55:00
 * Author: Nathan
 */ 


#include "microphone.h"
int wave_samples[NUMBER_OF_SAMPLES] = {0};

// generates sine wave with given number of samples at given frequency
char *generate_spoof(int tone_frequency) {
	int sampling_frequency = 32000;
	
	for (int i = 0; i < NUMBER_OF_SAMPLES; i++)
		wave_samples[i] = ((int) 127*sinf(2*M_PI*tone_frequency*i/sampling_frequency));
	
	return wave_samples;
}

void send_data(uint8_t* samples_data) {
	write_wifi_command("close all\r\n", 2);
	write_wifi_command("websocket_client wss://bigbrothersees.me/source_audio_socket", 2);

	char* templated_command[35];
	sprintf(templated_command, "write 0 %d\r\n", NUMBER_OF_SAMPLES);
	usart_write_line(BOARD_USART, templated_command);
	
	for (int i = 0; i < NUMBER_OF_SAMPLES; i++)
	{
		usart_putchar(BOARD_USART, samples_data[i]);
	}
}