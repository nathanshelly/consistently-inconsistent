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

// declare all of the variables!!!
volatile uint8_t input_buffer[BUFFER_SIZE];
volatile uint32_t buffer_index;
volatile uint8_t timeout_counter;
volatile uint32_t data_recieved;
volatile uint32_t wifi_setup_flag;
volatile uint32_t received_byte_wifi;

void handler_usart(void);
//void process_incoming_byte(uint8_t inByte);
void configure_usart(void);
void handler_command_complete(uint32_t ul_id, uint32_t ul_mask);
//void process_data_wifi(void);
void configure_command_complete(void);
void handler_web_setup(uint32_t ul_id, uint32_t ul_mask);
void configure_web_setup(void);
void write_wifi_command(char* comm, uint8_t cnt);
void write_image_usart(void);
void print_to_file(char* message, int num_bytes);
void blink_LED(int ms_blink);
void setup_wifi(void);
void configure_wifi(void);
void reboot_wifi(void);

#define BOARD_ID_USART				ID_USART0
#define BOARD_USART					USART0
#define BOARD_USART_BAUDRATE		115200
#define handler_usart			USART0_Handler
#define USART_IRQn					USART0_IRQn

#define WIFI_COMM_PIN_MSK			PIO_PB0
#define WIFI_COMM_PIO				PIOB
#define WIFI_COMM_ID				ID_PIOB
#define WIFI_COMM_TYPE				PIO_INPUT
#define WIFI_COMM_ATTR				PIO_IT_RISE_EDGE

//#define WIFI_SETUP				(PIO_PB14, PIOB, ID_PIOB, PIO_INPUT, PIO_PULLUP | PIO_DEBOUNCE | PIO_IT_RISE_EDGE)
#define WIFI_SETUP_MASK          PIO_PB14
#define WIFI_SETUP_PIO           PIOB
#define WIFI_SETUP_ID            ID_PIOB
#define WIFI_SETUP_TYPE          PIO_INPUT
#define WIFI_SETUP_ATTR          PIO_PULLUP | PIO_DEBOUNCE | PIO_IT_RISE_EDGE

#endif /* WIFI_H_ */