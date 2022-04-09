#include "MKL25Z4.h"                    // Device header
#include "myblinky.h"
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2

osEventFlagsId_t red_flag, green_flag, blue_flag, ctrl_flag;

/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
void led_red_thread(void *argument) {
  for (;;) {
    osEventFlagsWait(ctrl_flag, 0x1, osFlagsWaitAny, osWaitForever);
    ledControl(led_pin[RED]);
    osDelay(1000);
    ledControl(OFF_RED);
    osDelay(1000);
  }
}

void led_green_thread(void *argument) {
  for(;;) {
    osEventFlagsWait(ctrl_flag, 0x2, osFlagsWaitAny, osWaitForever);
    ledControl(led_pin[GREEN]);
    osDelay(1000);
    ledControl(OFF_GREEN);
    osDelay(1000);
  }
}

void led_blue_thread(void *argument) {
	for(;;) {
    osEventFlagsWait(ctrl_flag, 0x3, osFlagsWaitAny, osWaitForever);
    ledControl(led_pin[BLUE]);
    osDelay(1000);
    ledControl(OFF_BLUE);
		osDelay(1000);

	}
}

void control_thread(void *argument) {
  for (;;) {
		osEventFlagsSet(ctrl_flag, 0x1);
    osEventFlagsSet(ctrl_flag, 0x3);
    osDelay(3000);
  }
}

int main(void) {
  SystemCoreClockUpdate();
  InitGPIO();
  offRGB();
	
  // red_flag = osEventFlagsNew(NULL);
  // green_flag = osEventFlagsNew(NULL);
  // blue_flag = osEventFlagsNew(NULL);
  
  osKernelInitialize();
	ctrl_flag = osEventFlagsNew(NULL); // Only 1 flag is used in this part 4.
  osThreadNew(led_red_thread, NULL, NULL);
  osThreadNew(led_green_thread, NULL, NULL);
  osThreadNew(led_blue_thread, NULL, NULL);
  osThreadNew(control_thread, NULL, NULL);
  osKernelStart();
  
  for(;;) {}
}