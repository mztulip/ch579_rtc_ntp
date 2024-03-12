#include "CH57x_common.h"

extern unsigned long _sidata;   
extern unsigned long _sdata;   
extern unsigned long _edata;   
extern unsigned long _sbss;    
extern unsigned long _ebss;     
extern unsigned long _estack; 

int main(void);


void Default_Handler(void) {
  while(1)
	{
		
		mDelaymS(100);
		GPIOB_SetBits( GPIO_Pin_0 ); 
		mDelaymS(100);
		GPIOB_ResetBits( GPIO_Pin_0 );
	}
}

static void Reset_Handler(void) {
  unsigned long *src, *dst;

  for (dst = &_sdata, src = &_sidata; dst < &_edata; ++dst, ++src)
    *dst = *src;
 
  for (dst = &_sbss; dst < &_ebss; ++dst)
    *dst = 0;
  main();
  for (;;);
}

#define WEAK __attribute__ ((weak, alias("Default_Handler")))

WEAK void NMI_Handler(void);
WEAK void HardFault_Handler(void);
WEAK void MemManage_Handler(void);
WEAK void BusFault_Handler(void);
WEAK void UsageFault_Handler(void);
WEAK void SVC_Handler(void);
WEAK void DebugMon_Handler(void);
WEAK void PendSV_Handler(void);
WEAK void SysTick_Handler(void);
WEAK void TMR0_IRQHandler(void);
WEAK void GPIO_IRQHandler(void);
WEAK void SLAVE_IRQHandler(void);
WEAK void SPI0_IRQHandler(void);
WEAK void BB_IRQHandler(void);
WEAK void LLE_IRQHandler(void);
WEAK void USB_IRQHandler(void);
WEAK void ETH_IRQHandler(void);
WEAK void TMR1_IRQHandler(void);
WEAK void TMR2_IRQHandler(void);
WEAK void UART0_IRQHandler(void);
WEAK void UART1_IRQHandler(void);
WEAK void RTC_IRQHandler(void);
WEAK void ADC_IRQHandler(void);
WEAK void SPI1_IRQHandler(void);
WEAK void LED_IRQHandler(void);
WEAK void TMR3_IRQHandler(void);
WEAK void UART2_IRQHandler(void);
WEAK void UART3_IRQHandler(void);
WEAK void WDT_IRQHandler(void);

__attribute__ ((section(".isr_vector")))
void (* const interrupt_vectors[])(void) = {
  (void*)&_estack,
  Reset_Handler,
  NMI_Handler,
  HardFault_Handler,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  SVC_Handler,
  0,
  0,
  PendSV_Handler,
  SysTick_Handler,
  TMR0_IRQHandler,
  GPIO_IRQHandler,
  SLAVE_IRQHandler,
  SPI0_IRQHandler,
  BB_IRQHandler,
  LLE_IRQHandler,
  USB_IRQHandler,
  ETH_IRQHandler,
  TMR1_IRQHandler,
  TMR2_IRQHandler,
  UART0_IRQHandler,
  UART1_IRQHandler,
  RTC_IRQHandler,
  ADC_IRQHandler,
  SPI1_IRQHandler,
  LED_IRQHandler,
  TMR3_IRQHandler,
  UART2_IRQHandler,
  UART3_IRQHandler,
  WDT_IRQHandler,
};


