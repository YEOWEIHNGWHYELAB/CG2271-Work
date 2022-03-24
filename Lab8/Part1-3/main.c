/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
 
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include "LEDControl.h"

#define SW_POS 6
 
osSemaphoreId_t mySem;

void initSwitch(void) {
	// enable clock for PortD
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	
	/* Select GPIO and enable pull-up resistors and interrupts on
	falling edges of pin connected to switch*/
	PORTD->PCR[SW_POS] |= (PORT_PCR_MUX(1) |
												PORT_PCR_PS_MASK |
												PORT_PCR_PE_MASK |
												PORT_PCR_IRQC(0x0a));
	
	// Set PORT D Switch bit to input
	PTD->PDDR &= ~MASK(SW_POS);
	
	//Enable Interrupts
	NVIC_SetPriority(PORTD_IRQn, 2);
	NVIC_ClearPendingIRQ(PORTD_IRQn);
	NVIC_EnableIRQ(PORTD_IRQn);
}

void PORTD_IRQHandler() {
	NVIC_ClearPendingIRQ(PORTD_IRQn);
	
	delay(0x80000); // To bypass the debouncing, easiest way to debounce...
	osSemaphoreRelease(mySem);
	osSemaphoreRelease(mySem);
	
	PORTD->ISFR |= MASK(SW_POS);
}

/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
void led_red_thread (void *argument) {
  for (;;) {
		osSemaphoreAcquire(mySem, osWaitForever);
		
		ledControllerPlus(0);
		osDelay(1000);
		offRGB();
		osDelay(1000); 
		
		// osSemaphoreRelease(mySem);
	}
}

void led_green_thread (void *argument) {
  for (;;) {
		osSemaphoreAcquire(mySem, osWaitForever);
		
		ledControllerPlus(1);
		osDelay(1000); 
		offRGB();
		osDelay(1000); 
		
		// osSemaphoreRelease(mySem);
	}
}
 
int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
	initSwitch();
	InitGPIO();
	offRGB();
  
	// ...
  osKernelInitialize();
  // mySem = osSemaphoreNew(1, 1, NULL);
	// mySem = osSemaphoreNew(1, 0, NULL);
	mySem = osSemaphoreNew(2, 0, NULL);
	osThreadNew(led_red_thread, NULL, NULL);
  osThreadNew(led_green_thread, NULL, NULL);
	osKernelStart();
	
  for (;;) {}
}
