void NMI_Handler      (void)  { while(1){} }
void HardFault_Handler(void)  {}
void MemManage_Handler(void)  { while(1){} }
void BusFault_Handler (void)  { while(1){} }
void UsageFault_Handler(void) { while(1){} }
void SVC_Handler      (void)  { while(1){} }
void DebugMon_Handler (void)  { while(1){} }
void PendSV_Handler   (void)  { while(1){} }
void SysTick_Handler  (void)  { while(1){} }

/* Peripherals handlers */
void SUPC_Handler (void)  { while(1){} }
void RSTC_Handler (void)  { while(1){} }
void RTC_Handler  (void)  { while(1){} }
void RTT_Handler  (void)  { while(1){} }
void WDT_Handler  (void)  { while(1){} }
void PMC_Handler  (void)  { while(1){} }
void EFC0_Handler (void)  { while(1){} }
#ifdef _SAM4S_EFC1_INSTANCE_
void EFC1_Handler (void)  { while(1){} }
#endif /* _SAM4S_EFC1_INSTANCE_ */
void UART0_Handler(void)  { while(1){} }
void UART1_Handler(void)  { while(1){} }
//void PIOA_Handler (void)  { while(1){} }
//void PIOB_Handler (void)  { while(1){} }
#ifdef _SAM4S_PIOC_INSTANCE_
void PIOC_Handler (void)  { while(1){} }
#endif /* _SAM4S_PIOC_INSTANCE_ */
//void USART0_Handler(void) { while(1){} }
#ifdef _SAM4S_USART1_INSTANCE_
void USART1_Handler(void) { while(1){} }
#endif /* _SAM4S_USART1_INSTANCE_ */
#ifdef _SAM4S_HSMCI_INSTANCE_
void HSMCI_Handler(void)  { while(1){} }
#endif /* _SAM4S_HSMCI_INSTANCE_ */
void TWI0_Handler (void)  { while(1){} }
void TWI1_Handler (void)  { while(1){} }
void SPI_Handler  (void)  { while(1){} }
//void SSC_Handler  (void)  { while(1){} }
//void TC0_Handler  (void)  { while(1){} }
void TC1_Handler  (void)  { while(1){} }
void TC2_Handler  (void)  { while(1){} }
#ifdef _SAM4S_TC1_INSTANCE_
void TC3_Handler  (void)  { while(1){} }
#endif /* _SAM4S_TC1_INSTANCE_ */
#ifdef _SAM4S_TC1_INSTANCE_
void TC4_Handler  (void)  { while(1){} }
#endif /* _SAM4S_TC1_INSTANCE_ */
#ifdef _SAM4S_TC1_INSTANCE_
void TC5_Handler  (void)  { while(1){} }
#endif /* _SAM4S_TC1_INSTANCE_ */
void ADC_Handler  (void)  { while(1){} }
#ifdef _SAM4S_DACC_INSTANCE_
void DACC_Handler (void)  { while(1){} }
#endif /* _SAM4S_DACC_INSTANCE_ */
void PWM_Handler  (void)  { while(1){} }
void CRCCU_Handler(void)  { while(1){} }
void UDP_Handler  (void)  { while(1){} }
void ACC_Handler  (void)  { while(1){} }