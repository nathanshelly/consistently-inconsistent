#include "interrupts.h"

void NMI_Handler        { while(1){} }
void HardFault_Handler  { while(1){} }
void MemManage_Handler  { while(1){} }
void BusFault_Handler   { while(1){} }
void UsageFault_Handler { while(1){} }
void SVC_Handler        { while(1){} }
void DebugMon_Handler   { while(1){} }
void PendSV_Handler     { while(1){} }
void SysTick_Handler    { while(1){} }

/* Peripherals handlers */
void SUPC_Handler   { while(1){} }
void RSTC_Handler   { while(1){} }
void RTC_Handler    { while(1){} }
void RTT_Handler    { while(1){} }
void WDT_Handler    { while(1){} }
void PMC_Handler    { while(1){} }
void EFC0_Handler   { while(1){} }
#ifdef _SAM4S_EFC1_INSTANCE_
void EFC1_Handler   { while(1){} }
#endif /* _SAM4S_EFC1_INSTANCE_ */
void UART0_Handler  { while(1){} }
void UART1_Handler  { while(1){} }
void PIOA_Handler   { while(1){} }
void PIOB_Handler   { while(1){} }
#ifdef _SAM4S_PIOC_INSTANCE_
void PIOC_Handler   { while(1){} }
#endif /* _SAM4S_PIOC_INSTANCE_ */
void USART0_Handler { while(1){} }
#ifdef _SAM4S_USART1_INSTANCE_
void USART1_Handler { while(1){} }
#endif /* _SAM4S_USART1_INSTANCE_ */
#ifdef _SAM4S_HSMCI_INSTANCE_
void HSMCI_Handler  { while(1){} }
#endif /* _SAM4S_HSMCI_INSTANCE_ */
void TWI0_Handler   { while(1){} }
void TWI1_Handler   { while(1){} }
void SPI_Handler    { while(1){} }
void SSC_Handler    { while(1){} }
void TC0_Handler    { while(1){} }
void TC1_Handler    { while(1){} }
void TC2_Handler    { while(1){} }
#ifdef _SAM4S_TC1_INSTANCE_
void TC3_Handler    { while(1){} }
#endif /* _SAM4S_TC1_INSTANCE_ */
#ifdef _SAM4S_TC1_INSTANCE_
void TC4_Handler    { while(1){} }
#endif /* _SAM4S_TC1_INSTANCE_ */
#ifdef _SAM4S_TC1_INSTANCE_
void TC5_Handler    { while(1){} }
#endif /* _SAM4S_TC1_INSTANCE_ */
void ADC_Handler    { while(1){} }
#ifdef _SAM4S_DACC_INSTANCE_
void DACC_Handler   { while(1){} }
#endif /* _SAM4S_DACC_INSTANCE_ */
void PWM_Handler    { while(1){} }
void CRCCU_Handler  { while(1){} }
void UDP_Handler    { while(1){} }
void ACC_Handler    { while(1){} }