/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
 
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include <MKL25Z4.h>
#include "LEDControl.h"
 
/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
void blink_red_thread (void *argument) {
  for (;;) {
		ledControllerPlus(0);
		osDelay(1000); // Allows current thread to be put into block state, 1 tick is approximately 1ms
		// delay(0x80000);
		offRGB();
		osDelay(1000); 
		// delay(0x80000);
	}
}

void blink_green_thread (void *argument) {
  for (;;) {
		ledControllerPlus(1);
		osDelay(1000); 
		// delay(0x80000);
		offRGB();
		osDelay(1000); 
		// delay(0x80000);
	}
}

void blink_blue_thread (void *argument) {
  for (;;) {
		ledControllerPlus(2);
		osDelay(1000); 
		// delay(0x80000);
		offRGB();
		osDelay(1000); 
		// delay(0x80000);
	}
}
 
int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
	InitGPIO();
  // ...
 
  osKernelInitialize(); // Call of the OS or Underlying framework of OS
  osThreadNew(blink_red_thread, NULL, NULL); // Create application main thread
	// osThreadNew(blink_blue_thread, NULL, NULL);
	osThreadNew(blink_green_thread, NULL, NULL);
  osKernelStart(); // Transition from single thread to multithread, and will not come back to main anymore
  for (;;) {}
}
