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
	configure_wifi();
	configure_camera();
	
	reboot_wifi();
	
	// microphone configuration
	configure_i2s();
	// open initial websocket
	configure_websocket();
	
	start_i2s_capture();
	
	uint32_t im_length;
	while(1) {
		// trigger setup to new network when setup button pressed
		if(wifi_setup_flag)
			setup_wifi();
		
		// captures image to internal memory
		// Owned by camera but sends audio data
		start_capture();
		im_length = find_image_len();
		if(im_length)
			send_image(start_of_image_ptr, im_length);
	}
	return 0;
}
