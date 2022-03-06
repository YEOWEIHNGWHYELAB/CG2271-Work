#include "MKL25Z4.h"                    // Device header

#define RED_LED 18 // PortB Pin 18
#define GREEN_LED 19 // PortB Pin 19
#define BLUE_LED 1 // PortD Pin 1
#define MASK(x) (1 << (x))
#define NUM_LEDS 3
#define RED 0
#define GREEN 2
#define BLUE 1

typedef enum led_colors {
  red_led = RED_LED,
  green_led = GREEN_LED,
  blue_led = BLUE_LED,
}led_colors_t;

typedef enum switch_mode {
	on,
	off,
}switch_mode_t;

led_colors_t ledArray[3] = {red_led,green_led,blue_led};

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

/* Delay Function */
static void delay(volatile uint32_t nof) {
  while(nof!=0) {
    __asm("NOP");
    nof--;
  }
}

void offLed () {
	PTB->PSOR |= MASK(RED_LED);
	PTB->PSOR |= MASK(GREEN_LED);
	PTD->PSOR |= MASK(BLUE_LED);
}
	
void ledControl(led_colors_t ledColor, switch_mode_t switchMode) {
	if (switchMode == on) {
		switch(ledColor){
		case(red_led):
			PTB->PCOR |= MASK(RED_LED);
			break;
		case(blue_led):
			PTD->PCOR |= MASK(BLUE_LED);
			break;
		case(green_led):
			PTB->PCOR |= MASK(GREEN_LED);
			break;
		}
	} else {
		switch(ledColor){
		case(red_led):
			PTB->PSOR |= MASK(RED_LED);
			break;
		case(blue_led):
			PTD->PSOR |= MASK(BLUE_LED);
			break;
		case(green_led):
			PTB->PSOR |= MASK(GREEN_LED);
			break;
		}
	}
	
}