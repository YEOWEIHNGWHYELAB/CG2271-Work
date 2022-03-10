/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
 
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include "LEDControl.h"

osMutexId_t myMutex; // Mutex variable
 
/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
void blink_red_thread (void *argument) {
  for (;;) {
		osMutexAcquire(myMutex, osWaitForever);
		
		ledControllerPlus(0);
		osDelay(1000);
		offRGB();
		osDelay(1000); 
		
		osMutexRelease(myMutex);
	}
}

void blink_green_thread (void *argument) {
  for (;;) {
		osMutexAcquire(myMutex, osWaitForever); // Commenting this along with osMutexRelease will lead to blink yellow
		
		ledControllerPlus(1);
		osDelay(1000); 
		offRGB();
		osDelay(1000); 
		
		osMutexRelease(myMutex); // Commenting this results in blinking red then green then nothing
	}
}

int main (void) {
  // Mutex protects access to shared resource (LED in this case)
  SystemCoreClockUpdate();
  InitGPIO();
	offRGB();
	
  osKernelInitialize();
  myMutex = osMutexNew(NULL); // Create a mutex.
	
	// Here we are treating LED as the shared resource.
  osThreadNew(blink_red_thread, NULL, NULL);
	osThreadNew(blink_green_thread, NULL, NULL);
  osKernelStart(); 
  for (;;) {}
}
