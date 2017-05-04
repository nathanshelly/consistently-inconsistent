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
	configure_wifi();		// configures and initializes wifi 
	//configure_camera();		// configures and initializes camera module
	
	uint8_t status_code = 0;
	
	while(write_wifi_command_safe("reboot\r\n", "[Associated]", 20000,0)){}
	
	status_code = write_wifi_command_safe("set sy c p off\r\n","Set OK",100,0);
	
	status_code = write_wifi_command_safe("set sy c e off\r\n","Set OK", 100, 0);
	
	//status_code = write_wifi_command_safe("write 0 4\r\n","Success",500, 0);
	
	//reboot_wifi();			// reboots the wifi chip (takes several seconds)
	
	configure_i2s(); // microphone configuration
	
	uint8_t ws_handle = open_websocket(5); // try 5 times to open the socket

	start_i2s_capture();
	
	while(!buffer_filled){};
	
	while(1) {
		//if(wifi_setup_flag) {	// if the user pressed the wifi setup button, 
		//	setup_wifi();		// the wifi chip tries to reassociate to a new network
		//}
		if (ws_handle != NO_WEBSOCKET_OPEN){
		// websocket not opened sucessfully
			status_code = send_data_ws(i2s_rec_buf, ws_handle);
		}
		
			
		//post_audio_usart((uint8_t *) i2s_rec_buf, 2000);			
		

		//start_capture();		// capture the image to internal memorys
		//post_image();			// send the image to the wifi chip
	}
	return 0;
}
