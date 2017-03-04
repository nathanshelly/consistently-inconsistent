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
#include "ov2640.h"
#include "timer_interface.h"
uint8_t placeholder = 0;

int main (void)
{
	sysclk_init();
	wdt_disable(WDT);
	board_init();
	
	// timer configuration
	configure_tc();
	tc_start(TC0, 0);
	
	// Custom configuration calls
	configure_usart_wifi();
	configure_wifi_comm_pin();
	configure_wifi_web_setup_pin();	
	usart_enable_interrupt(BOARD_USART, US_IER_RXRDY);
			
	// set up camera	
	init_camera();
	configure_camera();
		
	write_wifi_command("reboot\r\n", 10);
	
	
	int associated = 0;
	int seconds = 0;
	uint8_t* token = 0;
	wifi_setup_flag = false;
		
	while(!associated){
		if(wifi_setup_flag) {
			wifi_setup();
		}
		associated = strstr(input_buffer, "[Associated]\r\n");
		if (seconds > 100){
			blink_LED(50);
		}
		delay_ms(200);
		seconds++;
	}
	
	buffer_index = 0;
	
	write_wifi_command("set sy c e off\r\n", 5);	
	write_wifi_command("set sy c p off\r\n", 5);

	while(1) {
		if(wifi_setup_flag) {
			wifi_setup();
		}
		
		write_wifi_command("get wl n s\r\n", 1);
		if(strstr(input_buffer, "2"))
		{
			// have network
			write_wifi_command("poll all\r\n", 2);
			if(!strstr(input_buffer, "None"))
			{
				// have open streams
				start_capture();
				if (find_image_len())
				{
					write_image_to_file();
				}
				
				delay_ms(50);
				write_wifi_command("list\r\n", 2);
				delay_ms(50);
				
				/* throw out first two tokens */
				token = strtok(input_buffer, "#");
				token = strtok(NULL, "#");
				
				/* walk through other tokens */
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
			} else {
				delay_ms(1000);
			}
		}
	}
	return 0;
}
