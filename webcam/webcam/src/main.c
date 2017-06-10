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
	configure_wifi();	// configures and initializes wifi 
	configure_camera();	// configures and initializes camera module
	
	reboot_wifi();
	
	configure_i2s(); // microphone configuration
	configure_websocket();
	
	start_i2s_capture();
	
	uint32_t im_length;
	while(1) {
		if(wifi_setup_flag) {	// if the user pressed the wifi setup button, 
			setup_wifi();		// the wifi chip tries to reassociate to a new network
		}
		
		start_capture();		// captures image to internal memory. Owned by camera but sends audio data
		im_length = find_image_len();
		if(im_length) {
			send_image(start_of_image_ptr, im_length);
		}
	}
	return 0;
}
