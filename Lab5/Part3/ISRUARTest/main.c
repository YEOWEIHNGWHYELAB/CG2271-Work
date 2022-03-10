#include <MKL25Z4.h>
#include <stdbool.h>

#define RED_LED 18 // PortB Pin 18
#define GREEN_LED 19 // PortB Pin 19
#define BLUE_LED 1 // PortD Pin 1
#define MASK(x) (1 << (x))
#define SW_POS 6

#define RX_PERIPHERAL_MASK (0xf0)
#define RX_PERIPHERAL(X) (X >> 4)
#define RX_COMMAND_MASK (0x0f)
#define RX_COMMAND(X) ((X) & RX_COMMAND_MASK)

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

#define BAUD_RATE 9600
#define UART_TX_PORTE22 22
#define UART_RX_PORTE23 23
#define UART2_INT_PRIO 128

typedef enum led_colors {
	red_led = RED_LED,
	green_led = GREEN_LED,
	blue_led = BLUE_LED,
}led_colors_t;

unsigned int counter = 0;
char led_mapping[3][2] = {{0, red_led}, {1, green_led}, {2, blue_led}};
bool led_control = 1;
int int_count = 0;

/* GPIO Initialization Function */
void initGPIO(void) {
	// Enable Clock to PORTB and PORTD
	SIM->SCGC5 |= ((SIM_SCGC5_PORTB_MASK) | (SIM_SCGC5_PORTD_MASK));
	
	// Configure MUX settings to make all 3 pins GPIO
	PORTB->PCR[RED_LED] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[RED_LED] |= PORT_PCR_MUX(1);
	PORTB->PCR[GREEN_LED] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[GREEN_LED] |= PORT_PCR_MUX(1);
	PORTD->PCR[BLUE_LED] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[BLUE_LED] |= PORT_PCR_MUX(1);
	
	// Set Data Direction Registers for PortB and PortD LEDs
	PTB->PDDR |= (MASK(RED_LED) | MASK(GREEN_LED));
	PTD->PDDR |= MASK(BLUE_LED);
}

void init_UART2(uint32_t baud_rate) {
	uint32_t divisor, bus_clock;
	
	// enable clock to UART and Port E
	SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	
	// connect UART to pins for PTE22, PTE23
	// Page 162 (Alt 4)
  PORTE->PCR[UART_TX_PORTE22] &= ~PORT_PCR_MUX_MASK;
  PORTE->PCR[UART_TX_PORTE22] |= PORT_PCR_MUX(4); 
  
  // Page 162 (Alt 4)
  PORTE->PCR[UART_RX_PORTE23] &= ~PORT_PCR_MUX_MASK;
  PORTE->PCR[UART_RX_PORTE23] |= PORT_PCR_MUX(4);
	
	// ensure tx and rx are disabled before configuration
	UART2->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);
	
	// Set baud rate to desired value
	bus_clock = (DEFAULT_SYSTEM_CLOCK) / 2; // Clock used by UART module is half the system clock
	divisor = bus_clock / (baud_rate * 16); // Rearrangement of baud rate formula
	UART2->BDH = UART_BDH_SBR(divisor >> 8); // divisor is the SBR
	UART2->BDL = UART_BDL_SBR(divisor); // SBR is 13 bit value -> Need to be broken to 2 reg BDH and BDL 
	
	// No parity, 8 bits, two stop bits, other settings;
	UART2->C1 = 0;
	UART2->S2 = 0;
	UART2->C3 = 0;
	
	// Enable interrupts, Most Important Chunk!
	NVIC_SetPriority(UART2_IRQn, 128);
	NVIC_ClearPendingIRQ(UART2_IRQn);
	NVIC_EnableIRQ(UART2_IRQn);
	UART2->C2 |= UART_C2_RIE_MASK;
	
	// Enable transmitter and receiver
	UART2->C2 |= ((UART_C2_TE_MASK) | (UART_C2_RE_MASK));
}

void UART2_IRQHandler(void) {
	NVIC_ClearPendingIRQ(UART2_IRQn);
	
	if (UART2->S1 & UART_S1_RDRF_MASK) {
		uint8_t rx_data = UART2->D;
		uint8_t rx_command = RX_COMMAND(rx_data);
		uint8_t rx_peripheral = RX_PERIPHERAL(rx_data);
		
		// If you don't put one more, the setting of breakpoint will just skip the watch for the last variable...
		uint8_t test = 1;
	}
}

int main(void) {
	SystemCoreClockUpdate();
  init_UART2(BAUD_RATE);
	initGPIO();
	
	while(1) {
	}
}
