#include "wifi.h"
#include "camera.h"
#include "microphone.h"

volatile uint8_t usart_buffer[BUFFER_SIZE] = {0};
volatile uint32_t usart_buffer_index = 0;
volatile uint8_t timeout_counter = 0;
volatile uint32_t data_recieved = 0;
volatile uint32_t wifi_setup_flag = false;
volatile uint32_t received_byte_wifi = 0;

volatile uint32_t i2s_send_index = 0;
uint32_t post_counter = 0;
uint8_t ws_handle = NO_WEBSOCKET_OPEN;
uint32_t reopen_delay_seconds = 5;

/**
 * \brief Interrupt handler for USART.
 *
 * Increment the number of bytes received in the current second and start
 * another transfer if the desired bps has not been met yet.
 *
 */
void handler_usart()
{
	usart_disable_interrupt(BOARD_USART, US_IER_RXRDY);
	
	/* Read USART status. */
	uint32_t ul_status = usart_get_status(BOARD_USART);

	/* Receive buffer is full. */
	uint8_t recieved_char_flag = usart_read(BOARD_USART, &received_byte_wifi);
	
	if (ul_status & US_CSR_RXRDY) {
		recieved_char_flag = usart_read(BOARD_USART, &received_byte_wifi);
		usart_buffer[usart_buffer_index] = (uint8_t) received_byte_wifi;
		usart_buffer_index++;
	}	
	
	usart_enable_interrupt(BOARD_USART, US_IER_RXRDY);
}

/**
 *  Configure board USART communication with PC or other terminal.
 */
void configure_usart()
{
	static uint32_t ul_sysclk;
	const sam_usart_opt_t usart_console_settings = {
		BOARD_USART_BAUDRATE,
		US_MR_CHRL_8_BIT,
		US_MR_PAR_NO,
		US_MR_NBSTOP_1_BIT,
		US_MR_CHMODE_NORMAL,
		/* This field is only used in IrDA mode. */
		0
	};

	/* Get peripheral clock. */
	ul_sysclk = sysclk_get_peripheral_hz();

	/* Enable peripheral clock. */
	sysclk_enable_peripheral_clock(BOARD_ID_USART);

	/* Configure USART. */
	usart_init_hw_handshaking(BOARD_USART, &usart_console_settings, ul_sysclk);
	//usart_init_rs232(BOARD_USART, &usart_console_settings, ul_sysclk);

	/* Disable all the interrupts. */
	usart_disable_interrupt(BOARD_USART, ALL_INTERRUPT_MASK);
	
	/* Enable TX & RX function. */
	usart_enable_tx(BOARD_USART);
	usart_enable_rx(BOARD_USART);

	/* Configure and enable interrupt of USART. */
	NVIC_EnableIRQ(USART_IRQn);
	
	//usart_enable_interrupt(BOARD_USART, US_IER_RXRDY);
}

/**
 *  \brief Command response handler for wifi.
 */
void handler_command_complete(uint32_t ul_id, uint32_t ul_mask) {
	unused(ul_id);
	unused(ul_mask);
}

/**
 *  \brief Configures communication pin for wifi.
 */
void configure_command_complete(){
	// do something here
	// just configuring a rising edge interrupt on whichever pin
	// we set as the wifi command pin
	// we should be able to do this
	
	//configure button from the button project?
	
	pmc_enable_periph_clk(WIFI_COMM_ID); // put these definitions in wifi.h
	// see which pins we used
	// just the rising edge option
	
	pio_set_debounce_filter(WIFI_COMM_PIO, WIFI_COMM_PIN_MSK, 10);
	
	pio_handler_set(WIFI_COMM_PIO, WIFI_COMM_ID, WIFI_COMM_PIN_MSK, WIFI_COMM_ATTR, handler_command_complete);
	
	NVIC_EnableIRQ((IRQn_Type)WIFI_COMM_ID);
	
	pio_enable_interrupt(WIFI_COMM_PIO, WIFI_COMM_PIN_MSK);
	data_recieved = 0;
}

/**
 *  \brief Handler for setting up wifi.
 */
void handler_web_setup(uint32_t ul_id, uint32_t ul_mask) {
	unused(ul_id);
	unused(ul_mask);

	wifi_setup_flag = true;
}

/**
 *  \brief Configures wifi setup pin.
 */
void configure_web_setup(){
	// Configuration of a button to initiate web setup
	// Pin is PB14
	//pio_configure_pin(GPIO_WIFI_RESET_PB, GPIO_WIFI_RESET_PB_FLAGS);
	
	///* Set direction and pullup on the given button IOPORT */
	//ioport_set_pin_dir(GPIO_WIFI_RESET_PB, IOPORT_DIR_INPUT);
	//ioport_set_pin_mode(GPIO_WIFI_RESET_PB, IOPORT_MODE_PULLUP);
	/* Configure PIO clock. */
	pmc_enable_periph_clk(WIFI_SETUP_ID);

	/* Adjust PIO debounce filter using a 10 Hz filter. */
	pio_set_debounce_filter(WIFI_SETUP_PIO, WIFI_SETUP_MASK, 10);

	/* Initialize PIO interrupt handler, see PIO definition in conf_board.h */
	pio_handler_set(WIFI_SETUP_PIO, WIFI_SETUP_ID, WIFI_SETUP_MASK,
			WIFI_SETUP_ATTR, handler_web_setup);
			
	/* Enable PIO controller IRQs. */
	NVIC_EnableIRQ((IRQn_Type) WIFI_SETUP_ID);

	/* Enable PIO interrupt lines. */
	pio_enable_interrupt(WIFI_SETUP_PIO, WIFI_SETUP_MASK);
}

/**
 *  \brief Writes wifi command.
 */
void write_wifi_command(char* comm, uint8_t cnt){
	data_recieved = 0;
	usart_write_line(BOARD_USART, comm);
	
	timeout_counter = 0;
	cnt = cnt*20;
	while(timeout_counter < cnt && !data_recieved) {
		// leave long
		delay_ms(10);
		timeout_counter++;
	}
}

uint8_t open_websocket(uint8_t number_of_attempts) {
	// figure out handle
	uint8_t status_code;
	for(int i=0; i<number_of_attempts; i++){
		uint8_t status_code = write_wifi_command_safe("websocket_client -f bin wss://bigbrothersees.me/source_socket\r\n", "Opened: ", 20000, 1);
		if (status_code >= 10){
			if (status_code > 18){
				write_wifi_command_safe("close all\r\n","Success",200,0);
				continue;
			}
			return status_code - 10;
		}
	}
	// should check last thing in input buffer for handle
	return NO_WEBSOCKET_OPEN; // indicate failure
}

uint8_t check_ws_handle(uint8_t theoretical_handle){
	char* templated_response[40];
	sprintf(templated_response, "%d WEBC", theoretical_handle);
	
	uint8_t command_response = write_wifi_command_safe("list\r\n", templated_response, 100, 0);
	return command_response == COMMAND_SUCCESS ? COMMAND_SUCCESS : COMMAND_FAILURE;
}

/*
	\brief Blinks a defined LED_PIN.
	Delays, switches, delays, switches, delays
	Total time is 3x ms_blink
 */
void blink_LED(int ms_blink){
	delay_ms(ms_blink);
	ioport_toggle_pin_level(LED_PIN);
	delay_ms(ms_blink);
	ioport_toggle_pin_level(LED_PIN);
	delay_ms(ms_blink);
}

/**
 *  \brief Sets up wifi chip on new network.
 */
void setup_wifi(){
	// command wifi chip to setup
	write_wifi_command_safe("setup web\r\n","Associated]", 60000, 0);
	// turn off setup flag
	wifi_setup_flag = false;
}

/**
 *  \brief Wrapper function for wifi module configuration.
 */
void configure_wifi(){
	configure_usart();
	configure_command_complete();
	configure_web_setup();	
	usart_enable_interrupt(BOARD_USART, US_IER_RXRDY);
}

void safe_mode_recovery(){
	write_wifi_command("get system.safemode.status\r\n",2);
	write_wifi_command("faults_print\r\n",2);
	write_wifi_command("faults_reset\r\n",2);
	write_wifi_command("faults_print\r\n",2);
	reboot_wifi();
}

/**
 *  \brief Reboots the wifi chip.
 */
void reboot_wifi() {
	wifi_setup_flag = false;
	uint8_t status_code;
	
	while(write_wifi_command_safe("reboot\r\n", "Associated]", 10000,0) != COMMAND_SUCCESS){
		if(wifi_setup_flag){
			setup_wifi();
		}
	}
	
	status_code = write_wifi_command_safe("set sy c p off\r\n","Set OK",100,0);
	status_code = write_wifi_command_safe("set sy c e off\r\n","Set OK", 100, 0);
}

void configure_websocket(){
	// try 5 times to open the socket
	ws_handle = open_websocket(5); 
}

void reopen_websockets(){
	// reopen websocket connections if something breaks
	write_wifi_command_safe("close all\r\n","Success",100,0); // fix this
	ws_handle = NO_WEBSOCKET_OPEN;
	
	while(ws_handle == NO_WEBSOCKET_OPEN) {
		delay_s(reopen_delay_seconds);
		ws_handle = open_websocket(5); // try 5 times to open the socket
		reopen_delay_seconds += 5; // this could take a while
	}
	reopen_delay_seconds = 5;
}

void send_image_dummy(){
	uint8_t status_code;

	if (ws_handle != NO_WEBSOCKET_OPEN){
		//websocket open
		// the audio still has stuff to send (should be most of the time)
		status_code = write_audio_data_safe(i2s_rec_buf, ws_handle, "Success", 500);
		if(status_code == COMMAND_STCLOSE){
			ws_handle = NO_WEBSOCKET_OPEN;
		} else if (status_code == COMMAND_FAILURE){
			if(check_ws_handle(ws_handle) != COMMAND_SUCCESS){
				reopen_websockets();
			}
		}
	} else{
		// websocket not open
		reopen_websockets();
	}	
}

uint8_t is_audio_caught_up(){
	uint32_t end_index = (i2s_send_index + AUDIO_PACKET_SIZE) % (AUDIO_BUFFER_SIZE); // the last uint16 index that will be hit if it sends
	
	// make sure that the end index isn't in front of the receive index
	uint32_t first_condition = ((i2s_capture_index - i2s_send_index) < AUDIO_PACKET_SIZE) && ((i2s_capture_index - i2s_send_index) > 0);
	uint32_t second_condition = ((end_index - i2s_capture_index) > 0) && ((end_index - i2s_capture_index) < AUDIO_PACKET_SIZE);
	return (first_condition || second_condition) ? 1 : 0;
}

void send_image(uint8_t *start_of_image_ptr, uint32_t image_length){
	// sends image data over ws connection while still streaming audio
	// throttled to audio speed
	
	uint32_t image_byte_index = 0;
	uint8_t status_code;
	
	while (image_byte_index < image_length){
		// there's still image data to send
		//send audio data until caught up
		if (ws_handle != NO_WEBSOCKET_OPEN){
			//websocket open
			if(is_audio_caught_up()){
				// if the audio sending is caught up, we can send an image packet
				status_code = write_image_data_safe(start_of_image_ptr, image_byte_index, image_length, ws_handle, "Success", 500);
				if(status_code == COMMAND_STCLOSE){
					ws_handle = NO_WEBSOCKET_OPEN;
				} else if (status_code == COMMAND_FAILURE){
					if(check_ws_handle(ws_handle) != COMMAND_SUCCESS){
						reopen_websockets();
					}
				} else if (status_code == COMMAND_SUCCESS){
					image_byte_index += IMAGE_PACKET_SIZE;
				}

			} else{
				// the audio still has stuff to send (should be most of the time)
				status_code = write_audio_data_safe(i2s_rec_buf, ws_handle, "Success", 500);
				if(status_code == COMMAND_STCLOSE){
					ws_handle = NO_WEBSOCKET_OPEN;
				} else if (status_code == COMMAND_FAILURE){
					if(check_ws_handle(ws_handle) != COMMAND_SUCCESS){
						reopen_websockets();
					}
				}
			}
		} else{
			// websocket not open
			reopen_websockets();
		}	
	}
		
	char* templated_command[30];
	sprintf(templated_command, "write %d 10\r\nimage done", ws_handle);

	write_wifi_command_safe(templated_command, "Success", 500, 0);
}

uint8_t send_audio_packet(){
	// sends one audio packet, assuming everything is OK everywhere
	// called by camera functions to stream audio while capturing image
	uint8_t status_code;
	
	if (ws_handle != NO_WEBSOCKET_OPEN && ws_handle != PREV_COMMAND_FAILED && !is_audio_caught_up()){
			//websocket open, no recent command failure, and enough data to send
		status_code = write_audio_data_safe(i2s_rec_buf, ws_handle, "Success", 500);
		
		if(status_code == COMMAND_STCLOSE){
			ws_handle = NO_WEBSOCKET_OPEN;
		} else if (status_code == COMMAND_FAILURE){
			ws_handle = PREV_COMMAND_FAILED;
		}
		return 1;
	}
	return 0;
}

void prep_stream_for_data(uint8_t handle, uint32_t num_bytes) {
	char* templated_command[30];
	usart_write_line(BOARD_USART,"\r\n");
	sprintf(templated_command, "write %d %d\r\n", handle, num_bytes);
	usart_write_line(BOARD_USART, templated_command);
}

uint8_t get_stream_response(char* resp, uint32_t timeout_ms, uint8_t is_audio_data_send) {
	uint32_t ms_counter = 0;
	uint8_t command_response = COMMAND_UNSET;

	while(command_response == COMMAND_UNSET) {
		if(strstr(usart_buffer, resp)){
			if(is_audio_data_send)
				// recompute send index after loop execution
				i2s_send_index = (i2s_send_index + AUDIO_PACKET_SIZE) % AUDIO_BUFFER_SIZE;

			command_response = COMMAND_SUCCESS;
		}
		else if(strstr(usart_buffer, "[Closed: "))
			command_response = COMMAND_STCLOSE;
		else if(strstr(usart_buffer, "Command failed"))
			command_response = COMMAND_FAILURE;
		else if(ms_counter++ > timeout_ms)
			command_response = COMMAND_TIMEOUT;

		delay_ms(1);
	}
	
	usart_buffer_index = 0;
	return command_response;
}

// returns 0 for a successful write
// returns 1 for a failed write
// returns 2 for a timeout
// returns 3 for an unexpected stream closure
// returns 10+stream handle for opening a stream
uint8_t write_audio_data_safe(uint16_t* data_pointer, uint8_t handle, char* resp, uint32_t timeout_ms){
	
	// first thing: check if we've received any transmission since the last time
	if (usart_buffer_index != 0 && strstr(usart_buffer, "[Closed: ")) {
		return COMMAND_STCLOSE; // return a value indicating closure of the stream
	}

	// clear buffer
	memset(usart_buffer, 0, BUFFER_SIZE);
	usart_buffer_index = 0;
	
	if(is_audio_caught_up()) // make sure that the end index isn't in front of the receive index
		return COMMAND_RETRYIT;

	prep_stream_for_data(handle, AUDIO_PACKET_SIZE * 2);
	
	uint8_t curr_data_point;
	for (int i = i2s_send_index*2; i < i2s_send_index*2 + (AUDIO_PACKET_SIZE*2); i++) {
		curr_data_point = ((uint8_t*) data_pointer)[i % (AUDIO_BUFFER_SIZE*2)];
		usart_putchar(BOARD_USART, curr_data_point);
	}

	return get_stream_response(resp, timeout_ms, 1);
}

// returns 0 for a successful write
// returns 1 for a failed write
// returns 2 for a timeout
// returns 3 for an unexpected stream closure
// returns 10+stream handle for opening a stream
uint8_t write_image_data_safe(uint8_t* array_start_pointer, uint32_t start_index, uint32_t im_len, uint8_t handle, char* resp, uint32_t timeout_ms){
	
	// first thing: check if we've received any transmission since the last time
	if (usart_buffer_index != 0 && strstr(usart_buffer, "[Closed: ")) {
		return COMMAND_STCLOSE; // return a value indicating closure of the stream
	}
	
	// clear buffer
	memset(usart_buffer, 0, BUFFER_SIZE);
	usart_buffer_index = 0;
	
	uint32_t end_index = start_index + IMAGE_PACKET_SIZE;
	// if the end index goes past the end of the array, don't go there
	end_index = end_index < im_len ? end_index : im_len;
	
	// usually PACKET_SIZE, unless it's the last one
	uint32_t bytes_to_send = end_index - start_index;
	prep_stream_for_data(handle, bytes_to_send);

	uint8_t curr_data_point;	
	for (int i = start_index; i < end_index; i++) {
		curr_data_point = (array_start_pointer)[i];	
		usart_putchar(BOARD_USART, curr_data_point);
	}
	
	return get_stream_response(resp, timeout_ms, 0);
}

// returns 0 for a successful write
// returns 1 for a failed write
// returns 2 for a timeout
// returns 10+stream handle for opening a stream
uint8_t write_wifi_command_safe(char* command, char* resp, uint32_t timeout_ms, uint8_t handle_expected){

	// clear buffer
	memset(usart_buffer, 0, BUFFER_SIZE);
	usart_buffer_index = 0;
	
	usart_write_line(BOARD_USART,"\r\n");
	usart_write_line(BOARD_USART, command);
	
	uint32_t ms_counter = 0;
	uint8_t command_response = COMMAND_UNSET;
	
	while(command_response == COMMAND_UNSET) {			
		// makes sure we have complete response before matching on expected
		// 10 is new line, should be the last thing in the buffer
		if( (strstr(usart_buffer, resp)) && (usart_buffer[usart_buffer_index-1] == 10)){
			if(!handle_expected) command_response = COMMAND_SUCCESS; // successful response
			// otherwise, parse for handle
			else {
				char *opened_pointer = strstr(usart_buffer, resp);
				uint32_t buffer_offset = ((uint8_t *) opened_pointer) - usart_buffer;
				uint8_t handle = usart_buffer[buffer_offset+12] - '0';
				command_response = handle+10;
			}
		}
		else if(strstr(usart_buffer, "[Closed: ")){
			// return a value indicating closure of the stream
			command_response = COMMAND_STCLOSE;
		}
		else if (strstr(usart_buffer, "Command failed")){
			command_response = COMMAND_FAILURE; // command failed
		}
		else if (ms_counter++ > timeout_ms){
			command_response = COMMAND_TIMEOUT;
		}
		delay_ms(1);
	}
	
	usart_buffer_index = 0;
	return command_response;
}