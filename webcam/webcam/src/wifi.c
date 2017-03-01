/*
 * wifi.c
 *
 * Created: 2/2/2017 19:57:10
 *  Author: Nathan
 */ 

#include "wifi.h"
#include "camera.h"

volatile uint8_t input_buffer[BUFFER_SIZE] = {0};
volatile uint32_t buffer_index = 0;
volatile uint32_t wifi_setup_flag = false;
volatile uint32_t received_byte_wifi = 0;
volatile uint8_t timeout_counter = 0;
volatile uint32_t data_recieved = 0;

/**
 *  \brief Interrupt handler for USART.
 *
 * Increment the number of bytes received in the current second and start
 * another transfer if the desired bps has not been met yet.
 *
 */
void WIFI_USART_Handler(void)
{	
	uint8_t recieved_char_flag = 0;
	uint32_t ul_status;
	
	usart_disable_interrupt(BOARD_USART, US_IER_RXRDY);
	
	/* Read USART status. */
	ul_status = usart_get_status(BOARD_USART);

	/* Receive buffer is full. */
	recieved_char_flag = usart_read(BOARD_USART, &received_byte_wifi);
	// he doesn't have this line?
	
	if (ul_status & US_CSR_RXRDY) {
		recieved_char_flag = usart_read(BOARD_USART, &received_byte_wifi);
		process_incoming_byte((uint8_t) received_byte_wifi);
	}	
	
	usart_enable_interrupt(BOARD_USART, US_IER_RXRDY);
}

/**
 *  \brief Processes incoming bytes.
 */
void process_incoming_byte(uint8_t inByte) {
	if(!data_recieved)
	{
		input_buffer[buffer_index] = inByte;
		buffer_index++;
	}
}

/**
 *  Configure board USART communication with PC or other terminal.
 */
void configure_usart_wifi(void)
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
void wifi_command_response_handler(uint32_t ul_id, uint32_t ul_mask) {
	unused(ul_id);
	unused(ul_mask);
	ioport_toggle_pin_level(LED_PIN);
	process_data_wifi();
}

/**
 *  \brief Processes data from wifi.
 */
void process_data_wifi(void){
	input_buffer[buffer_index] = 0;
	data_recieved = 1;
	buffer_index = 0;
}

/**
 *  \brief Configures communication pin for wifi.
 */
void configure_wifi_comm_pin(void){
	// do something here
	// just configuring a rising edge interrupt on whichever pin
	// we set as the wifi command pin
	// we should be able to do this
	
	//configure button from the button project?
	
	pmc_enable_periph_clk(WIFI_COMM_ID); // put these definitions in wifi.h
	// see which pins we used
	// just the rising edge option
	
	pio_set_debounce_filter(WIFI_COMM_PIO, WIFI_COMM_PIN_MSK, 10);
	
	pio_handler_set(WIFI_COMM_PIO, WIFI_COMM_ID, WIFI_COMM_PIN_MSK, WIFI_COMM_ATTR, wifi_command_response_handler);
	
	NVIC_EnableIRQ((IRQn_Type)WIFI_COMM_ID);
	
	pio_enable_interrupt(WIFI_COMM_PIO, WIFI_COMM_PIN_MSK);
	data_recieved = 0;
}

/**
 *  \brief Handler for setting up wifi.
 */
void wifi_web_setup_handler(uint32_t ul_id, uint32_t ul_mask) {
	unused(ul_id);
	unused(ul_mask);

	wifi_setup_flag = true;
}

/**
 *  \brief Configures wifi setup pin.
 */
void configure_wifi_web_setup_pin(void){
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
			WIFI_SETUP_ATTR, wifi_web_setup_handler);
			
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
	while(timeout_counter < cnt && !data_recieved) {
		delay_ms(1000);
		timeout_counter++;
	}
	
	delay_ms(1000);
}

/**
 *  \brief Writes image to file.
 */
void write_image_to_file(void){
	// will have already checked if image is valid
	write_wifi_command("fde image.jpg\r\n", 2);
	char* templated_command[40];
	sprintf(templated_command, "fcr image.jpg %d\r\n", image_length);
	usart_write_line(BOARD_USART, templated_command);
	
	for (int i = 0; i < image_length; i++)
	{
		usart_putchar(BOARD_USART, (uint32_t) ((uint8_t*) start_of_image_ptr)[i]);
	}
}

void print_to_file(char* message, int num_bytes) {
	usart_write_line(BOARD_USART, "fde batman.txt\r\n");
	char* templated_command[40];
	sprintf(templated_command, "fcr batman.txt %d\r\n", num_bytes);
	usart_write_line(BOARD_USART, templated_command);
	usart_write_line(BOARD_USART, message);
}

void blink_LED(int ms_blink){
	delay_ms(ms_blink);
	ioport_toggle_pin_level(LED_PIN);
	delay_ms(ms_blink);
	ioport_toggle_pin_level(LED_PIN);
	delay_ms(ms_blink);
}