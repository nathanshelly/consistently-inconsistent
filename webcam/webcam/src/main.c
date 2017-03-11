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
	
	board_init();
	
	// timer configuration
	configure_tc();
	tc_start(TC0, 0);
	
	// Custom configuration calls
	configure_wifi();
			
	// set up camera	
	configure_camera();
	
	reboot_wifi();

	uint8_t* token = 0;

	while(1) {
		if(wifi_setup_flag) {
			setup_wifi();
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
					write_image_usart();
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
