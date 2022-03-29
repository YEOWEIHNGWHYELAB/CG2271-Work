#include "MKL25Z4.h"                    // Device header
#include "myblinky.h"
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2

#define MSG_COUNT 1

osMessageQueueId_t redMsg, greenMsg, blueMsg;

typedef struct {
	uint8_t cmd;
	uint8_t data;
}myDataPkt;

void blink(int loopN, int delay, int color, int color_off) {
	for (int i = 0; i < loopN; i++) {
		ledControl(led_pin[color]);
		osDelay(delay);
		ledControl(color_off);
		osDelay(delay);
	}
}

/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
void led_red_thread(void *argument) {
	myDataPkt myRxData;
  for (;;) {
		osMessageQueueGet(redMsg, &myRxData, NULL,osWaitForever);
		
		if (myRxData.data == 0x01){
			ledControl(led_pin[RED]);
			osDelay(1000);
			ledControl(OFF_RED);
			osDelay(1000);
		} else if (myRxData.data == 0x02) {
			ledControl(led_pin[RED]);
			osDelay(500);
			ledControl(OFF_RED);
			osDelay(500);
		} else if (myRxData.data == 0x03) {
			ledControl(led_pin[RED]);
			osDelay(250);
			ledControl(OFF_RED);
			osDelay(250);
		} else if (myRxData.data == 0x04) {
			ledControl(led_pin[RED]);
			osDelay(125);
			ledControl(OFF_RED);
			osDelay(125);
		} else if (myRxData.data == 0x05) {
			blink(10, 125, RED, OFF_RED);
		}
	}
}

void led_green_thread(void *argument) {
	myDataPkt myRxData;
	for (;;) {
		osMessageQueueGet(greenMsg, &myRxData, NULL,osWaitForever);
		
		if (myRxData.data == 0x01){
			ledControl(led_pin[GREEN]);
			osDelay(1000);
			ledControl(OFF_GREEN);
			osDelay(1000);
		} else if (myRxData.data == 0x02) {
			ledControl(led_pin[GREEN]);
			osDelay(500);
			ledControl(OFF_GREEN);
			osDelay(500);
		} else if (myRxData.data == 0x03) {
			ledControl(led_pin[GREEN]);
			osDelay(250);
			ledControl(OFF_GREEN);
			osDelay(250);
		} else if (myRxData.data == 0x04) {
			ledControl(led_pin[GREEN]);
			osDelay(125);
			ledControl(OFF_GREEN);
			osDelay(125);
		} else if (myRxData.data == 0x05) {
			blink(10, 125, GREEN, OFF_GREEN);
		}
	}
}

void led_blue_thread(void *argument) {
	myDataPkt myRxData;
	for (;;) {
		osMessageQueueGet(blueMsg, &myRxData, NULL,osWaitForever);
		
		if (myRxData.data == 0x01){
			ledControl(led_pin[BLUE]);
			osDelay(1000);
			ledControl(OFF_BLUE);
			osDelay(1000);
		} else if (myRxData.data == 0x02) {
			ledControl(led_pin[BLUE]);
			osDelay(500);
			ledControl(OFF_BLUE);
			osDelay(500);
		} else if (myRxData.data == 0x03) {
			ledControl(led_pin[BLUE]);
			osDelay(250);
			ledControl(OFF_BLUE);
			osDelay(250);
		} else if (myRxData.data == 0x04) {
			ledControl(led_pin[BLUE]);
			osDelay(125);
			ledControl(OFF_BLUE);
			osDelay(125);
		} else if (myRxData.data == 0x05) {
			blink(10, 125, BLUE, OFF_BLUE);
		}
	}
}

void control_thread(void *argument) {
	myDataPkt myData;
	myData.cmd = 0x01;
	
	for (;;) {
		myData.data = 0x01;
		osMessageQueuePut(redMsg, &myData, NULL, 0);
		osDelay(2000);
		
		myData.data = 0x02;
		osMessageQueuePut(greenMsg, &myData, NULL, 0);
		osDelay(2000);
		
		myData.data = 0x03;
		osMessageQueuePut(blueMsg, &myData, NULL, 0);
		osDelay(2000);
		
		myData.data = 0x04;
		osMessageQueuePut(redMsg, &myData, NULL, 0);
		osMessageQueuePut(greenMsg, &myData, NULL, 0);
		osMessageQueuePut(blueMsg, &myData, NULL, 0);
		osDelay(2000);
		
		myData.data = 0x05;
		osMessageQueuePut(redMsg, &myData, NULL, 0);
		osMessageQueuePut(greenMsg, &myData, NULL, 0);
		osMessageQueuePut(blueMsg, &myData, NULL, 0);
		osDelay(3000); // If your delay is too short, the thread will run concurrently leading to mixed color that you do not desire.
	}
}

int main(void) {
	SystemCoreClockUpdate();
	InitGPIO();
	offRGB();
	
	osKernelInitialize();
	osThreadNew(led_red_thread,NULL,NULL);
	osThreadNew(led_green_thread,NULL,NULL);
	osThreadNew(led_blue_thread,NULL,NULL);
	osThreadNew(control_thread,NULL,NULL);
	
	redMsg = osMessageQueueNew(MSG_COUNT, sizeof(myDataPkt), NULL);
	greenMsg = osMessageQueueNew(MSG_COUNT, sizeof(myDataPkt), NULL);
	blueMsg = osMessageQueueNew(MSG_COUNT, sizeof(myDataPkt), NULL);
	
	osKernelStart();
	
	for(;;) {}
}
