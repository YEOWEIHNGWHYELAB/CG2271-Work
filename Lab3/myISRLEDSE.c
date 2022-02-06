#include "MKL25Z4.h"                    // Device header
#define SWITCH_PIN  6 // PortD Pin 6
#define RED_LED 18 // PortB Pin 18
#define GREEN_LED 19 // PortB Pin 19
#define BLUE_LED 1 // PortD Pin 1
#define MASK(x) (1 << (x))

volatile int toggle = 1;
volatile unsigned int counter = 0;

typedef enum led_colors {
  red_led = RED_LED,
  green_led = GREEN_LED,
  blue_led = BLUE_LED,
}led_colors_t;

led_colors_t ledArray[3] = {red_led,green_led,blue_led};

void PORTD_IRQHandler()
{
  // Clear Pending IRQ
  NVIC_ClearPendingIRQ(PORTD_IRQn);
  // Updating some variable / flag
  //toggle ^= 1;
  if (counter >= 0x02) {
      counter = 0x0;
  } else {
      counter++;
  } 
  //Clear INT Flag
  PORTD->ISFR |= MASK(SWITCH_PIN);
}

void initSwitch() {
  // enable clock for PortD
  SIM->SCGC5 |= SIM_SCGC5_PORTD(1);
  // Select GPIO
  PORTD->PCR[SWITCH_PIN] &= ~PORT_PCR_MUX_MASK;
  PORTD->PCR[SWITCH_PIN] |= PORT_PCR_MUX(1);
  // Enable pull-up resistors
  PORTD->PCR[SWITCH_PIN] &= ~(PORT_PCR_PE_MASK | PORT_PCR_PS_MASK);
  PORTD->PCR[SWITCH_PIN] |= (PORT_PCR_PE(1) | PORT_PCR_PS(1)); 
  // Setup interrupt on falling edge
  PORTD->PCR[SWITCH_PIN] &= ~(PORT_PCR_IRQC_MASK);
  PORTD->PCR[SWITCH_PIN] |= PORT_PCR_IRQC(0b1010);
  // Set PORT D Switch bit to input
  PTD->PDDR &= ~MASK(SWITCH_PIN);
  //Enable Interrupts
  NVIC_SetPriority(PORTD_IRQn, 2);
  NVIC_ClearPendingIRQ(PORTD_IRQn);
  NVIC_EnableIRQ(PORTD_IRQn);
  
}

void initLED() {
  // enable clock for PortD
  SIM->SCGC5 |= SIM_SCGC5_PORTD(1) | SIM_SCGC5_PORTB(1);
  /* Select GPIO */
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
  
void ledControl(led_colors_t ledColor) {
  offLed();
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
}

int main(void)
{
  initSwitch();
  initLED();
  while(1)
  {
    /*if (toggle == 1) {
      ledControl(ledArray[counter]);
      delay(0x80000);
      if (counter >= 0x02) {
        counter = 0x0;
      } else {
        counter++;
      } 
    } else {
      offLed();
    }*/
    ledControl(ledArray[counter]);
  }
}