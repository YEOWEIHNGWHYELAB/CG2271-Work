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