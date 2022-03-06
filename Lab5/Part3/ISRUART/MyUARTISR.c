// Current Code is running at 48MHz core freq and 24 MHz Bus clk freq
#include "MKL25Z4.h"

#define BAUD_RATE 9600
#define UART_TX_PORTE22 22
#define UART_RX_PORTE23 23
#define UART2_INT_PRIO 128

/* Led Control */
#define RED_LED 18 // PortB Pin 18
#define GREEN_LED 19 // PortB Pin 19
#define BLUE_LED 1 // PortD Pin 1
#define MASK(x) (1 << (x)) 

/* UART RX Command Control */
#define RX_PERIPHERAL_MASK (0xf0)
#define RX_PERIPHERAL(X) (((X) & RX_PERIPHERAL_MASK) >> 1)
#define RX_COMMAND_MASK (0x0f)
#define RX_COMMAND(X) (((X) & RX_COMMAND_MASK))

//Peripherals
#define RX_MOVE (0x2)
#define RX_LED (0x3)

//LED Commands
#define RED_OFF (0x0)
#define	RED_ON (0x1)
#define GREEN_OFF (0x2)
#define GREEN_ON (0x3)
#define	BLUE_OFF (0x4)
#define	BLUE_ON (0x5)


volatile int count = 0;

typedef enum led_colors {
  red_led = RED_LED,
  green_led = GREEN_LED,
  blue_led = BLUE_LED,
}led_colors_t;

typedef enum switch_mode {
	on,
	off,
}switch_mode_t;

led_colors_t ledArray[3] = {red_led, green_led, blue_led};


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

void offLed() {
	PTB->PSOR |= MASK(RED_LED);
	PTB->PSOR |= MASK(GREEN_LED);
	PTD->PSOR |= MASK(BLUE_LED);
}
	
void ledControl(led_colors_t ledColor, switch_mode_t switchMode) {
	//Turn on LED
	if (switchMode == on) {
		//Set PCOR registers to clear LED output
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
	
	//Turn off LED
	if (switchMode == off){
		//Set PSOR registers to set LED output
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

// UART2 Transmit Poll
void UART2_Transmit_Poll(uint8_t data) {
  // Keep checking for the transmit data register
  while(!(UART2->S1 & UART_S1_TDRE_MASK));
  UART2->D = data;  
}

// UART2 Receive Poll
uint8_t UART2_Receive_Poll(void) {
  // Wait until receive data register is full
  while(!(UART2->S1 & UART_S1_RDRF_MASK));
  return (UART2->D);  
}

void UART2_IRQHandler(void) {
	NVIC_ClearPendingIRQ(UART2_IRQn);
	count++;
	/*if (UART2->S1 & UART_S1_TDRE_MASK) {
		//transmitter is free
	}*/
	if (UART2->S1 & UART_S1_RDRF_MASK) {
		//received a character
		uint8_t rx_data = UART2->D;
		uint8_t rx_peripheral = RX_PERIPHERAL(rx_data); //((rx_data & RX_PERIPHERAL_MASK) >> 1);
		uint8_t rx_command = RX_COMMAND(rx_data); //((rx_data & RX_COMMAND_MASK));
		switch(rx_peripheral) {
			case(RX_MOVE):
				/*switch(rx_data) {
					
				}*/
				break;
			case(RX_LED): 
				switch(rx_command) {
					case(RED_OFF):
						ledControl(red_led, off); 
						break;
					case(RED_ON):
						ledControl(red_led, on); 
						break;
					case(GREEN_OFF):
						ledControl(green_led, off);
						break;
					case(GREEN_ON):
						ledControl(green_led, on);
						break;
					case(BLUE_OFF):
						ledControl(blue_led, off);
						break;
					case(BLUE_ON):
						ledControl(blue_led, on);
						break;
					default:
						offLed();
						break;
				}
				break;
			default:
				break;
			}
		}
		
	/*if (UART2->S1 & (UART_S1_OR_MASK | UART_S1_NF_MASK | UART_S1_FE_MASK | UART_S1_PF_MASK)) {
		//handle error
	}*/
}

int main() {
  SystemCoreClockUpdate();
  Init_UART2(BAUD_RATE);
	InitGPIO();
	offLed();
  while(1) {
	}
}
