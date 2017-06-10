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
#define COMMAND_UNSET		99

#define NO_WEBSOCKET_OPEN	10
#define PREV_COMMAND_FAILED	11

#define AUDIO_PACKET_SIZE	200 // uint-16s
#define IMAGE_PACKET_SIZE	399 // bytes

// variable declarations
volatile uint32_t wifi_setup_flag;

// function declarations
void blink_LED(int);
void setup_wifi(void);
void configure_wifi(void);
void safe_mode_recovery(void);
void reboot_wifi(void);
void configure_websocket();

void send_image(uint8_t*, uint32_t);
uint8_t send_audio_packet();
void write_wifi_command(char*, uint8_t);
uint8_t write_wifi_command_safe(char*, char*, uint32_t, uint8_t);
uint8_t write_audio_data_safe(uint16_t*, uint8_t, char*, uint32_t);
uint8_t write_image_data_safe(uint8_t*, uint32_t, uint32_t, uint8_t, char*, uint32_t);

// Usart communication definitions
#define BOARD_ID_USART				ID_USART0
#define BOARD_USART					USART0
#define BOARD_USART_BAUDRATE		2500000
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