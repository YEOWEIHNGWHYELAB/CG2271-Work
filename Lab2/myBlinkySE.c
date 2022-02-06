# include <MKL25Z4.h>

#define RED_LED 18 // PortB Pin 18
#define GREEN_LED 19 // PortB Pin 19
#define BLUE_LED 1 // PortD Pin 1
#define MASK(x) (1 << (x)) // Left shift binary 1 

#define YELLOW_LED_INDEX 0 
#define CYAN_LED_INDEX 1 
#define PINK_LED_INDEX 2 

typedef enum led_colors {
	red_led = RED_LED,
	green_led = GREEN_LED,
	blue_led = BLUE_LED,
}led_color_selection;

unsigned int counter = 0;
char led_mapping[3][2] = {{0, red_led}, {1, green_led}, {2, blue_led}};

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

/* Switch OFF the RGB LED */
void offRGB(void) {
	PTB->PSOR = (MASK(RED_LED) | MASK(GREEN_LED));
	PTD->PSOR = MASK(BLUE_LED);
}

/* Control the LED to turn on */
void ledController(led_color_selection color) {
	offRGB();
	
	switch(color) {
		case RED_LED:
			PTB->PCOR = MASK(RED_LED);
			break;
		case GREEN_LED:
			PTB->PCOR = MASK(GREEN_LED);
			break;
		case BLUE_LED:
			PTD->PCOR = MASK(BLUE_LED);
			break;
		default:
			offRGB();
	}
}

void ledControllerPlus(int color) {
	offRGB();
	
	switch(color) {
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
		default:
			offRGB();
	}
}

/* Delay Function */
static void delay(volatile uint32_t nof) {
	while(nof!=0) {
		__asm("NOP");
		nof--;
	}
}

/* MAIN Function */
int main(void) {
	SystemCoreClockUpdate();
	InitGPIO();
	
	while(1) {
		// ledController(led_mapping[counter][1]);
		ledControllerPlus(counter);
		counter++;
		
		if (counter > 0x02) 
			counter = 0;
		
		delay(0x80000);
	}
}