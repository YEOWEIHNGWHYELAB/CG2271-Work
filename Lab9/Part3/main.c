/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
 
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include <MKL25Z4.h>

#define RED_LED 18 // PortB Pin 18
#define GREEN_LED 19 // PortB Pin 19
#define BLUE_LED 1 // PortD Pin 1
#define MASK(x) (1 << (x)) // Left shift binary 1 

#define RED_LED_INDEX 0
#define GREEN_LED_INDEX 1
#define BLUE_LED_INDEX 2
#define YELLOW_LED_INDEX 3 
#define CYAN_LED_INDEX 4
#define PINK_LED_INDEX 5 
#define WHITE_LED_INDEX 6

osThreadId_t redLED_Id, greenLED_Id, blueLED_Id, control_Id;

/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
 
/* GPIO Initialization Function */
void InitGPIO(void) {
	// Enable Clock to PORTB and PORTD
	SIM->SCGC5 |= ((SIM_SCGC5_PORTB_MASK) | (SIM_SCGC5_PORTD_MASK));
	
	// Configure MUX settings to make all 3 pins GPIO
	PORTB->PCR[RED_LED] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[RED_LED] |= PORT_PCR_MUX(1);
	PORTB->PCR[GREEN_LED] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[GREEN_LED] |= PORT_PCR_MUX(1);
	PORTD->PCR[BLUE_LED] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[BLUE_LED] |= PORT_PCR_MUX(1);
	
	// Set Data Direction Registers for PortB and PortD
	PTB->PDDR |= (MASK(RED_LED) | MASK(GREEN_LED));
	PTD->PDDR |= MASK(BLUE_LED);
}

void offRGB(void) {
	PTB->PSOR = (MASK(RED_LED) | MASK(GREEN_LED));
	PTD->PSOR = MASK(BLUE_LED);
}

/* Delay Function */
static void delay(volatile uint32_t nof) {
	while(nof!=0) {
		__asm("NOP");
		nof--;
	}
}

/* Control the LED to turn on */
void ledControllerPlus(int color) {
	// offRGB();
	
	switch(color) {
		case RED_LED_INDEX:
			PTB->PCOR = MASK(RED_LED);
			break;
		case GREEN_LED_INDEX:
			PTB->PCOR = MASK(GREEN_LED);
			break;
		case BLUE_LED_INDEX:
			PTD->PCOR = MASK(BLUE_LED);
			break;
		case YELLOW_LED_INDEX:
			PTB->PCOR = MASK(RED_LED);
			PTB->PCOR = MASK(GREEN_LED);
			break;
		case CYAN_LED_INDEX:
			PTB->PCOR = MASK(GREEN_LED);
			PTD->PCOR = MASK(BLUE_LED);
			break;
		case PINK_LED_INDEX:
			PTD->PCOR = MASK(BLUE_LED);
			PTB->PCOR = MASK(RED_LED);
			break;
		case WHITE_LED_INDEX:
			PTB->PCOR = MASK(GREEN_LED);
			PTD->PCOR = MASK(BLUE_LED);
			PTB->PCOR = MASK(RED_LED);
			break;
		default:
			offRGB();
	}
}

void blink_red_thread(void *argument) {
  for (;;) {
		osThreadFlagsWait(0x00000001, osFlagsWaitAny, osWaitForever);
		ledControllerPlus(0);
		osDelay(1000);
		offRGB();
		osDelay(1000);
	}
}

void blink_green_thread(void *argument) {
  for (;;) {
		osThreadFlagsWait(0x00000001, osFlagsWaitAny, osWaitForever);
		ledControllerPlus(1);
		osDelay(1000);
		offRGB();
		osDelay(1000);
	}
}

void blink_blue_thread(void *argument) {
  for (;;) {
		osThreadFlagsWait(0x00000001, osFlagsWaitAny, osWaitForever);
		ledControllerPlus(2);
		osDelay(1000);
		offRGB();
		osDelay(1000);
	}
}

void control_thread(void *argument) {
  for (;;) {
		osThreadFlagsSet(redLED_Id, 0x0000001);
		osDelay(1000);
		osThreadFlagsSet(greenLED_Id, 0x0000001);
		osDelay(1000);
		osThreadFlagsSet(blueLED_Id, 0x0000001);
		osDelay(1000);
	}
}
 
int main (void) {
  // System Initialization
  SystemCoreClockUpdate();
	InitGPIO();
	offRGB();
 
  osKernelInitialize();
  redLED_Id = osThreadNew(blink_red_thread, NULL, NULL);
	greenLED_Id = osThreadNew(blink_green_thread, NULL, NULL);
  blueLED_Id = osThreadNew(blink_blue_thread, NULL, NULL);
	control_Id = osThreadNew(control_thread, NULL, NULL);
	osKernelStart();
  for (;;) {}
}
