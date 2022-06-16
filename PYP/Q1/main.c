#include "MKL25Z4.h"                    // Device header
#include "RTX_Config.h"                 // ARM::CMSIS:RTOS2:Keil RTX5
#include "rtx_os.h"                     // ARM::CMSIS:RTOS2:Keil RTX5
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#include "myblinky.h"

osThreadId_t green_id;
osThreadId_t red_id;
osSemaphoreId_t mySem;

void led_red_thread(void *argument) {
	for(;;) {
		osSemaphoreAcquire(mySem, osWaitForever);
		ledControl(led_pin[RED]);
		osDelay(1000);
		ledControl(OFF_RED);
		osDelay(1000);
	}
}

void led_green_thread(void *argument) {
	static int init = 1;
	for(;;) {
		osSemaphoreAcquire(mySem, osWaitForever);
		ledControl(led_pin[GREEN]);
		osDelay(1000);
		ledControl(OFF_GREEN);
		osDelay(1000);
		
		if (init == 1) {
			osThreadSetPriority(red_id, osPriorityAboveNormal);
			init = 0;
		}
	}
}

void control_thread(void *argument) {
	for(;;) {
		osDelay(5000);
		osSemaphoreRelease(mySem);
	}
}

int main (void) {
	SystemCoreClockUpdate();
	InitGPIO();
	offRGB();
	
	osKernelInitialize();
	mySem = osSemaphoreNew(1, 0, NULL);
	osThreadNew(control_thread, NULL, NULL);
	red_id = osThreadNew(led_red_thread, NULL, NULL);
	green_id = osThreadNew(led_green_thread, NULL, NULL);
	
	osKernelStart();
	for(;;) {}
}
