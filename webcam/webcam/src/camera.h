/*
 * camera.h
 *
 * Created: 2/2/2017 19:58:28
 *  Author: Nathan
 */ 

#ifndef CAMERA_H_
#define CAMERA_H_

#include <asf.h>
#include "wifi.h"
#include "conf_board.h"
#include "conf_clock.h"

#define CAM_BUFFER_SIZE					100000

// variable declarations
volatile uint32_t vsync_rising_edge_flag;
uint8_t image_dest_buffer_ptr[CAM_BUFFER_SIZE];
uint8_t *start_of_image_ptr;

// function declarations
void handler_vsync(uint32_t ul_id, uint32_t ul_mask);
void configure_vsync(void);
void init_pio_capture(Pio *p_pio, uint32_t ul_id);
void configure_twi(void);
void init_camera(void);
void configure_camera(void);
uint8_t capture_pio(Pio *p_pio, uint8_t *uc_buf, uint32_t ul_size);
void start_capture(void);
uint32_t find_image_len(void);

/** OV_VSYNC pin definition */
#define OV_VSYNC_GPIO                  PIO_PA15_IDX
#define OV_VSYNC_FLAGS                 (PIO_PULLUP | PIO_IT_RISE_EDGE)
#define OV_VSYNC_MASK                  PIO_PA15
#define OV_VSYNC_PIO                   PIOA
#define OV_VSYNC_ID                    ID_PIOA
#define OV_VSYNC_TYPE                  PIO_PULLUP
#define OV_DATA_BUS_PIO                PIOA
#define OV_DATA_BUS_ID                 ID_PIOA

/** OV_HSYNC pin definition */
#define OV_HSYNC_GPIO                  PIO_PA16_IDX
#define OV_HSYNC_FLAGS                 (PIO_PULLUP | PIO_IT_RISE_EDGE)
#define OV_HSYNC_MASK                  PIO_PA16
#define OV_HSYNC_PIO                   PIOA
#define OV_HSYNC_ID                    ID_PIOA
#define OV_HSYNC_TYPE                  PIO_PULLUP

// TWI definitions
#define ID_BOARD_TWI				   ID_TWI0
#define BOARD_TWI			           TWI0
#define BOARD_TWI_IRQn		           TWI0_IRQn

/* TWI clock frequency in Hz (400KHz) */
#define TWI_CLK							(100000UL)

/** OV Data Bus pins */
#define OV_DATA_BUS_D2                 PIO_PA24_IDX
#define OV_DATA_BUS_D3                 PIO_PA25_IDX
#define OV_DATA_BUS_D4                 PIO_PA26_IDX
#define OV_DATA_BUS_D5                 PIO_PA27_IDX
#define OV_DATA_BUS_D6                 PIO_PA28_IDX
#define OV_DATA_BUS_D7                 PIO_PA29_IDX
#define OV_DATA_BUS_D8                 PIO_PA30_IDX
#define OV_DATA_BUS_D9                 PIO_PA31_IDX
#define OV_DATA_BUS_FLAGS              (PIO_INPUT | PIO_PULLUP)
#define OV_DATA_BUS_MASK               (0xFF000000UL)
#define OV_DATA_BUS_PIO                PIOA
#define OV_DATA_BUS_ID                 ID_PIOA
#define OV_DATA_BUS_TYPE               PIO_INPUT
#define OV_DATA_BUS_ATTR               PIO_DEFAULT
      
#endif /* CAMERA_H_ */