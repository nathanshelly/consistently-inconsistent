#include "camera.h"
#include "ov2640.h"

uint8_t *start_of_image_ptr = 0;
volatile uint32_t vsync_rising_edge_flag = false;
uint8_t image_dest_buffer_ptr[CAM_BUFFER_SIZE] = {0};

/**
 * \brief Handler for vertical synchronisation using by the OV2640 image sensor.
 **/
void handler_vsync(uint32_t ul_id, uint32_t ul_mask) {
	unused(ul_id);
	unused(ul_mask);

	vsync_rising_edge_flag = true;
}

/**
 * \brief Intialize Vsync_Handler.
 **/
void configure_vsync(void) {
	/* Initialize PIO interrupt handler, see PIO definition in conf_board.h */
	pio_handler_set(OV_VSYNC_PIO, OV_VSYNC_ID, OV_VSYNC_MASK,
					OV_VSYNC_TYPE, handler_vsync);

	/* Enable PIO controller IRQs */
	NVIC_EnableIRQ((IRQn_Type) OV_VSYNC_ID);
}

/**
 * \brief Configures TWI.
 **/
void configure_twi(void) {
	twi_options_t opt;

	/* Enable TWI peripheral */
	pmc_enable_periph_clk(ID_BOARD_TWI);

	/* Init TWI peripheral */
	opt.speed = TWI_CLK;
	opt.master_clk = sysclk_get_cpu_hz();
	twi_master_init(BOARD_TWI, &opt);

	/* Configure TWI interrupts */
	NVIC_DisableIRQ(BOARD_TWI_IRQn);
	NVIC_ClearPendingIRQ(BOARD_TWI_IRQn);
	NVIC_SetPriority(BOARD_TWI_IRQn, 0);
	NVIC_EnableIRQ(BOARD_TWI_IRQn);
}

/**
 * \brief Configuration and initialization of parallel capture.
 **/
void init_pio_capture(Pio *p_pio, uint32_t ul_id) {
	/* Enable periphral clock */
	pmc_enable_periph_clk(ul_id);

	/* Disable pio capture */
	p_pio->PIO_PCMR &= ~((uint32_t)PIO_PCMR_PCEN);

	/* Disable rxbuff interrupt */
	p_pio->PIO_PCIDR |= PIO_PCIDR_RXBUFF;

	/* 32bit width*/
	p_pio->PIO_PCMR &= ~((uint32_t)PIO_PCMR_DSIZE_Msk);
	p_pio->PIO_PCMR |= PIO_PCMR_DSIZE_WORD;

	/* Only HSYNC and VSYNC enabled */
	p_pio->PIO_PCMR &= ~((uint32_t)PIO_PCMR_ALWYS);
	p_pio->PIO_PCMR &= ~((uint32_t)PIO_PCMR_HALFS);
}

/**
 * \brief Initialize camera.
 **/
void init_camera(void) {
	pmc_enable_pllbck(7, 0x1, 1); /* PLLA work at 96 Mhz */

	configure_vsync();
	init_pio_capture(OV_DATA_BUS_PIO, OV_DATA_BUS_ID);
	
	/* Init PCK1 to work at 24 Mhz - 96/4=24 Mhz */
	PMC->PMC_PCK[1] = (PMC_PCK_PRES_CLK_4 | PMC_PCK_CSS_PLLB_CLK);
	PMC->PMC_SCER = PMC_SCER_PCK1;
	
	while (!(PMC->PMC_SCSR & PMC_SCSR_PCK1)) {}

	configure_twi();
}

/**
 * \brief Configure camera.
 **/	
void configure_camera(void) {
	init_camera();
	/* ov2640 Initialization */
	// First, make sure reg 0xFF=1
	const uint8_t cont_reg_val = 1;
	twi_packet_t init_packet = 
	{
		.addr         = 0xFF,                   // TWI slave memory address data
		.addr_length  = 1,                      // TWI slave memory address data size
		.chip         = OV_I2C_SENSOR_ADDRESS,	// TWI slave bus address
		.buffer       = &cont_reg_val,          // transfer data destination buffer
		.length       = 1                       // transfer data size (bytes)
	};
	ov_write_reg(BOARD_TWI, &init_packet);
	
	while (ov_init(BOARD_TWI) == 1) {}

	/* ov2640 configuration */
	ov_configure(BOARD_TWI, JPEG_INIT);
	ov_configure(BOARD_TWI, YUV422);
	ov_configure(BOARD_TWI, JPEG);
	ov_configure(BOARD_TWI, JPEG_640x480);
}   

/**
 * \brief Capture OV2640 data to a buffer.
 *
 * \param p_pio PIO instance which will capture data from OV2640 image sensor.
 * \param p_uc_buf Buffer address where captured data must be stored.
 * \param ul_size Data frame size.
 **/
uint8_t capture_pio(Pio *p_pio, uint8_t *uc_buf, uint32_t ul_size) {
	/* Check if the first PDC bank is free */
	if ((p_pio->PIO_RCR == 0) && (p_pio->PIO_RNCR == 0)) {
		p_pio->PIO_RPR = (uint32_t)image_dest_buffer_ptr;
		p_pio->PIO_RCR = ul_size;
		p_pio->PIO_PTCR = PIO_PTCR_RXTEN;
		return 1;
		} 
	else if (p_pio->PIO_RNCR == 0) {
		p_pio->PIO_RNPR = (uint32_t)image_dest_buffer_ptr;
		p_pio->PIO_RNCR = ul_size;
		return 1;
	} 
	else {
		return 0;
	}
}

/**
 * \brief Start picture capture.
 **/
void start_capture(void) {
	/* Enable vsync interrupt*/
	pio_enable_interrupt(OV_VSYNC_PIO, OV_VSYNC_MASK);

	/* Capture acquisition will start on rising edge of Vsync signal.
	 * So wait vsync_rising_edge_flag = 1 before start process*/
	// not really busy waiting in a meaningful sense
	while (!vsync_rising_edge_flag) {}
	
	/* Disable vsync interrupt*/
	pio_disable_interrupt(OV_VSYNC_PIO, OV_VSYNC_MASK);

	/* Enable pio capture*/
	pio_capture_enable(OV_DATA_BUS_PIO);

	/* Capture data and send it to external SRAM memory thanks to PDC
	 * feature */
	capture_pio(OV_DATA_BUS_PIO, image_dest_buffer_ptr, CAM_BUFFER_SIZE/4);
	
	uint32_t counter = 0;
	/* Wait end of capture*/
	// send 0-10 packets here - useful but not critical
	while (!((OV_DATA_BUS_PIO->PIO_PCISR & PIO_PCIMR_RXBUFF) == PIO_PCIMR_RXBUFF)) {
		counter += send_audio_packet(); // send one audio packet if available
	}

	/* Disable pio capture*/
	pio_capture_disable(OV_DATA_BUS_PIO);

	/* Reset vsync flag*/
	vsync_rising_edge_flag = false;
}

/**
 *  \brief Finds image len. Returns a uint32_t for the length
 **/
uint32_t find_image_len(void) {
	uint16_t *reading_ptr =  image_dest_buffer_ptr;
	while((*reading_ptr != 0xD8FF) && (reading_ptr < (image_dest_buffer_ptr + CAM_BUFFER_SIZE)))
	{
		reading_ptr++;
	}
	
	if (*reading_ptr != 0xD8FF)
	{
		return 0;
	}
	else
	{
		start_of_image_ptr = (uint8_t *) reading_ptr;
	}
	
	while((*reading_ptr != 0xD9FF) && (reading_ptr < (image_dest_buffer_ptr + CAM_BUFFER_SIZE)))
	{
		reading_ptr++;
	}
	if (reading_ptr > (image_dest_buffer_ptr + CAM_BUFFER_SIZE))
	{
		return 0;
	}
	// need to get past end of file to include it in image
	reading_ptr++;
	return (uint32_t) (((uint8_t*) reading_ptr) - ((uint8_t*) image_dest_buffer_ptr));
}