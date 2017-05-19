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

int main (void)
{
	// system level initialization calls
	sysclk_init();
	board_init();
	configure_tc();
	tc_start(TC0, 0);
		
	// Custom configuration calls
	configure_wifi();		// configures and initializes wifi 
	configure_camera();		// configures and initializes camera module
	
	uint8_t status_code = 0;
	uint32_t im_length;
	
	reboot_wifi();
	//safe_mode_recovery();
	
	configure_i2s(); // microphone configuration
	
	configure_websocket();
	
	start_i2s_capture();	
	while(1) {
		if(wifi_setup_flag) {	// if the user pressed the wifi setup button, 
			setup_wifi();		// the wifi chip tries to reassociate to a new network
		}
		
		start_capture();		// captures image to internal memory. Owned by camera but sends audio data
		im_length = find_image_len();
		if(im_length) {
			send_image(start_of_image_ptr, im_length);
		}
		
		//send_image_dummy();
	}
	return 0;
}
