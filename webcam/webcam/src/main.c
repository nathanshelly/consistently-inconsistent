/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */
/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include "wifi.h"
#include "camera.h"
#include "microphone.h"
#include "ov2640.h"
#include "timer_interface.h"

/**
 *  \brief Sends image to the wifi chip
 */
void send_image(void){
	
	uint32_t im_length;
	
	im_length = find_image_len();
	if (im_length)
	{
		write_image_usart(start_of_image_ptr, im_length);
	}
				
	delay_ms(50);	
}

/**
 * \brief Posts image to server
**/
void post_image(void) {
	uint32_t im_length;
	
	im_length = find_image_len();
	if(im_length) {
		post_image_usart(start_of_image_ptr, im_length);
	}

	//delay_ms(50);
}

/**
 * \brief Testing posting
**/
void post_test(void) {
	write_wifi_command("close all\r\n", 2);
	write_wifi_command("http_post -o https://bigbrothersees.me/post_test application/json\r\n", 2);
	write_wifi_command("http_add_header 0 message-type test\r\n", 2);
	
	char* templated_command[35];
	sprintf(templated_command, "write 0 %d\r\n", 15);
	usart_write_line(BOARD_USART, templated_command);
	
	usart_putchar(BOARD_USART, '{');
	usart_putchar(BOARD_USART, '\"');
	usart_putchar(BOARD_USART, 'd');
	usart_putchar(BOARD_USART, 'a');
	usart_putchar(BOARD_USART, 't');
	usart_putchar(BOARD_USART, 'a');
	usart_putchar(BOARD_USART, '\"');
	usart_putchar(BOARD_USART, ':');
	usart_putchar(BOARD_USART, '\"');

	usart_putchar(BOARD_USART, 'd');
	usart_putchar(BOARD_USART, 'a');
	usart_putchar(BOARD_USART, 't');
	usart_putchar(BOARD_USART, 'a');

	usart_putchar(BOARD_USART, '\"');
	usart_putchar(BOARD_USART, '}');

	write_wifi_command("http_read_status 0\r\n", 2);
}


/**
 *  \brief Messages all connected streams to refresh their images.
 */
void message_streams(void){
	
	uint8_t* token = 0;	// token for string parsing

	write_wifi_command("list\r\n", 2);
	//delay_ms(50);
				
	/* throw out first two tokens */
	token = strtok(input_buffer, "#");
	token = strtok(NULL, "#");
				
	/* walk through other tokens, sending a message to each */
	while(token != NULL)
	{
		token = strtok(NULL, "#");
		if(strstr(token, "WEBS"))
		{
			char* templated_command[15];
			sprintf(templated_command, "write %c 1\r\n", token[1]);
			write_wifi_command(templated_command, 2);
			write_wifi_command("0", 2);
		}
	}
}

int main (void)
{
	// system level initialization calls
	sysclk_init();
	board_init();	
	configure_tc();
	tc_start(TC0, 0);
	
	// Custom configuration calls
	configure_wifi();		// configures and initializes wifi module
	//configure_camera();		// configures and initializes camera module
	//reboot_wifi();			// reboots the wifi chip (takes several seconds)

	char *samples_data;
	uint32_t tr = 440;
	uint32_t sr = 800;

	samples_data = generate_spoof(440);
	while(1) {
		if(wifi_setup_flag) {	// if the user pressed the wifi setup button, 
			setup_wifi();		// the wifi chip tries to reassociate to a new network
		}

		//start_capture();		// capture the image to internal memory
		//post_test();
		//post_image();			// send the image to the wifi chip
		send_data(samples_data);
	}
	return 0;
}
