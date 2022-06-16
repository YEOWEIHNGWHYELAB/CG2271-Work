#include "MKL25Z4.h"        // Device header

#define RED_LED 18 // PortB Pin 18
#define GREEN_LED 19 // PortB Pin 19
#define BLUE_LED 1 // PortD Pin 1
#define MASK(x) (1 << (x))
#define NUM_LEDS 3
#define RED 0
#define BLUE 1
#define GREEN 2
#define OFF_RED 3
#define OFF_BLUE 4
#define OFF_GREEN 5

const uint32_t led_pin[]  = {RED_LED, BLUE_LED, GREEN_LED};

/* Main code */
void InitGPIO(void)
{
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

/* Control Behaviour of RGB LED */
void ledControl(int colour) {
  
  switch(colour)
  {
    case RED_LED:
      PTB->PCOR = MASK(RED_LED);
      break;
    case GREEN_LED:
      PTB->PCOR = MASK(GREEN_LED);
      break;
    case BLUE_LED:
      PTD->PCOR = MASK(BLUE_LED);
      break;
    case OFF_RED:
      PTB->PSOR = MASK(RED_LED);
		break;
		case OFF_GREEN:
      PTB->PSOR = MASK(GREEN_LED);
		break;
		case OFF_BLUE:
     PTD->PSOR = MASK(BLUE_LED);
		break;
  }
}

/* Delay Function */
static void delay(volatile uint32_t nof) {
  while(nof!=0) {
    __asm("NOP");
    nof--;
  }
}
