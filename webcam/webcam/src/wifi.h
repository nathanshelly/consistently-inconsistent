/*
 * wifi.h
 *
 * Created: 2/2/2017 19:57:25
 *  Author: Nathan
 */ 

#ifndef WIFI_H_
#define WIFI_H_

#include <asf.h>
#include "conf_board.h"
#include "conf_clock.h"
#include <string.h>

/** All interrupt mask. */
#define ALL_INTERRUPT_MASK  0xffffffff
#define BUFFER_SIZE			500

#define COMMAND_SUCCESS		0
#define COMMAND_FAILURE		1
#define COMMAND_TIMEOUT		2
#define COMMAND_STCLOSE		3
#define COMMAND_RETRYIT		4

#define NO_WEBSOCKET_OPEN	10

// variable declarations
volatile uint8_t input_buffer[BUFFER_SIZE];
volatile uint32_t buffer_index;
volatile uint8_t timeout_counter;
volatile uint32_t data_recieved;
volatile uint32_t wifi_setup_flag;
volatile uint32_t received_byte_wifi;

// function declarations
void handler_usart(void);
void configure_usart(void);
void handler_command_complete(uint32_t ul_id, uint32_t ul_mask);
void configure_command_complete(void);
void handler_web_setup(uint32_t ul_id, uint32_t ul_mask);
void configure_web_setup(void);
void write_wifi_command(char* comm, uint8_t cnt);
uint8_t write_wifi_command_safe(char* command, char* resp, uint32_t timeout_ms,uint8_t handle_expected);
void post_image_usart(uint8_t *start_of_image_ptr, uint32_t image_length);
void print_to_file(char* message, int num_bytes);
void blink_LED(int ms_blink);
void setup_wifi(void);
void configure_wifi(void);
void reboot_wifi(void);
void safe_mode_recovery(void);
uint8_t send_data_ws(uint16_t* samples_data, uint8_t handle);
uint8_t open_websocket(uint8_t number_of_attempts);
uint8_t check_ws_handle(uint8_t theoretical_handle);
send_image_ws(uint8_t *start_of_image_ptr, uint32_t image_length);

// Usart communication definitions

#define BOARD_ID_USART				ID_USART0
#define BOARD_USART					USART0
#define BOARD_USART_BAUDRATE		921600
#define handler_usart				USART0_Handler
#define USART_IRQn					USART0_IRQn

// Wifi communication pin definitions

#define WIFI_COMM_PIN_MSK			PIO_PB0
#define WIFI_COMM_PIO				PIOB
#define WIFI_COMM_ID				ID_PIOB
#define WIFI_COMM_TYPE				PIO_INPUT
#define WIFI_COMM_ATTR				PIO_IT_RISE_EDGE

// Wifi setup pin definitions

#define WIFI_SETUP_MASK				PIO_PB14
#define WIFI_SETUP_PIO				PIOB
#define WIFI_SETUP_ID				ID_PIOB
#define WIFI_SETUP_TYPE				PIO_INPUT
#define WIFI_SETUP_ATTR				PIO_PULLUP | PIO_DEBOUNCE | PIO_IT_RISE_EDGE

#endif /* WIFI_H_ */