/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
 
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
 
#define SW_POS  6 
#define RED_LED 18 // PortB Pin 18
#define GREEN_LED 19 // PortB Pin 19
#define BLUE_LED 1 // PortD Pin 1
#define MASK(x) (1 << (x)) 


/*UART Commands */
#define BAUD_RATE 9600
#define UART_TX_PORTE22 22
#define UART_RX_PORTE23 23
#define UART2_INT_PRIO 128

/* UART RX Command Control 
#define RX_PERIPHERAL_MASK (0xf0)
#define RX_PERIPHERAL(X) (((X) & RX_PERIPHERAL_MASK) >> 4)
#define RX_COMMAND_MASK (0x0f)
#define RX_COMMAND(X) (((X) & RX_COMMAND_MASK))*/

//Peripherals
#define RX_MOVE (0x2)
#define RX_LED (0x3)

/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
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

osSemaphoreId_t mySem;

uint8_t rx_data;

/* Delay Function */
static void delay(volatile uint32_t nof) {
  while(nof!=0) {
    __asm("NOP");
    nof--;
  }
}

/*
void PORTD_IRQHandler() {
  NVIC_ClearPendingIRQ(PORTD_IRQn);
  delay(0x80000);
  osSemaphoreRelease(mySem);
  PORTD->ISFR |= MASK(SW_POS);
}*/

void UART2_IRQHandler(void) {
  NVIC_ClearPendingIRQ(UART2_IRQn);
  /*if (UART2->S1 & UART_S1_TDRE_MASK) {
    //transmitter is free
  }*/
  if (UART2->S1 & UART_S1_RDRF_MASK) {
    //received a character
    rx_data = UART2->D;
    osSemaphoreRelease(mySem);
  }

  /*if (UART2->S1 & (UART_S1_OR_MASK | UART_S1_NF_MASK | UART_S1_FE_MASK | UART_S1_PF_MASK)) {
    //handle error
  }*/
}

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

void initSwitch() {
  // enable clock for PortD
  SIM->SCGC5 |= SIM_SCGC5_PORTD(1);
	
  // Select GPIO
  PORTD->PCR[SW_POS] &= ~PORT_PCR_MUX_MASK;
  PORTD->PCR[SW_POS] |= PORT_PCR_MUX(1);
	
  // Enable pull-up resistors
  PORTD->PCR[SW_POS] &= ~(PORT_PCR_PE_MASK | PORT_PCR_PS_MASK);
  PORTD->PCR[SW_POS] |= (PORT_PCR_PE(1) | PORT_PCR_PS(1)); 
	
  // Setup interrupt on falling edge
  PORTD->PCR[SW_POS] &= ~(PORT_PCR_IRQC_MASK);
  PORTD->PCR[SW_POS] |= PORT_PCR_IRQC(0b1010);
	
  // Set PORT D Switch bit to input
  PTD->PDDR &= ~MASK(SW_POS);
	
  //Enable Interrupts
  NVIC_SetPriority(PORTD_IRQn, 2);
  NVIC_ClearPendingIRQ(PORTD_IRQn);
  NVIC_EnableIRQ(PORTD_IRQn);
  
}

void Init_UART2(uint32_t baud_rate) {
  uint32_t divisor, bus_clock;
  
  // enable clock to UART and Port E
  SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
  SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
  
  // connect UART to pins for PTE22, PTE23
  PORTE->PCR[UART_TX_PORTE22] &= ~PORT_PCR_MUX_MASK;
  PORTE->PCR[UART_TX_PORTE22] |= PORT_PCR_MUX(4);
  
  PORTE->PCR[UART_RX_PORTE23] &= ~PORT_PCR_MUX_MASK;
  PORTE->PCR[UART_RX_PORTE23] |= PORT_PCR_MUX(4);
  
  // ensure tx and rx are disabled before configuration
  UART2->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);
  
  // Set baud rate to desired value
  bus_clock = (DEFAULT_SYSTEM_CLOCK) / 2; // Clock used by UART module is half the system clock
  divisor = bus_clock /(baud_rate * 16); // Rearrangement of baud rate formula
  UART2->BDH = UART_BDH_SBR(divisor >> 8); // divisor is the SBR
  UART2->BDL = UART_BDL_SBR(divisor); // SBR is 13 bit value -> Need to be broken to 2 reg BDH and BDL 
  
  // No parity, 8 bits, two stop bits, other settings;
  UART2->C1 = 0;
  UART2->S2 = 0;
  UART2->C3 = 0;
  
  //Enable interrupts
  NVIC_SetPriority(UART2_IRQn, 128);
  NVIC_ClearPendingIRQ(UART2_IRQn);
  NVIC_EnableIRQ(UART2_IRQn);
  UART2->C2 |= UART_C2_RIE_MASK;

  // Enable transmitter and receiver
  UART2->C2 |= UART_C2_TE_MASK | UART_C2_RE_MASK;
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

void led_green_thread (void *argument) {
  for (;;) {
		osSemaphoreAcquire(mySem, osWaitForever);
		
		ledControl(green_led, on);
		osDelay(1000); //delay(0x80000);
    ledControl(green_led, off);
    osDelay(1000); //delay(0x80000); 
		
    // osSemaphoreRelease(mySem);
  }
}
 
void led_red_thread (void *argument) {
  for (;;) {
    osSemaphoreAcquire(mySem, osWaitForever);
		
    ledControl(red_led, on);
    osDelay(1000);//delay(0x80000);
    ledControl(red_led, off);
    osDelay(1000);
		
    // osSemaphoreRelease(mySem);
  }
  
}
 
int main (void) {
  // System Initialization
  SystemCoreClockUpdate();
  InitGPIO();
  //initSwitch();
  Init_UART2(BAUD_RATE);
  offLed();

  osKernelInitialize();
  mySem = osSemaphoreNew(1, 0, NULL);
  osThreadNew(led_red_thread, NULL, NULL);
  osThreadNew(led_green_thread, NULL, NULL);
  osKernelStart();
  for (;;) {}
}
