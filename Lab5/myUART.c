#include "MKL25Z4.h"

// Current Code is running at 48MHz core freq and 24 MHz Bus clk freq

#define BAUD_RATE 9600
#define UART_TX_PORTE22 22
#define UART_RX_PORTE23 23
#define UART2_INT_PRIO 128

/* Init UART2 */
void Init_UART2(uint32_t baud_rate) {
	uint32_t divisor, bus_clock;
	
	// enable clock to UART and Port E
	SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	
	// connect UART to pins for PTE22, PTE23
	PORTE->PCR[UART_TX_PORTE22] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[UART_TX_PORTE22] |= ~PORT_PCR_MUX(4);
	
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
	
	// Enable transmitter and receiver
	UART2->C2 = UART_C2_TE_MASK | UART_C2_RE_MASK;
}

// UART2 Transmit Poll
void UART2_Transmit_Poll(uint8_t data) {
	// Keep checking for the transmit data register
	while(!(UART2->S1 & UART_S1_TDRE_MASK));
	UART2->D = data;	
}

// UART2 Receive Poll
uint8_t UART2_Reveive_Poll(void) {
	// Wait until receive data register is full
	while(!(UART2->S1 & UART_S1_RDRF_MASK));
	return (UART2->D);	
}

/* Delay Function */
static void delay(volatile uint32_t nof) {
	while(nof!=0) {
		__asm("NOP");
		nof--;
	}
}

int main() {
	uint8_t rx_data = 0x01; 
	
	SystemCoreClockUpdate();
	Init_UART2(BAUD_RATE);
	
	for(;;) {
		UART2_Transmit_Poll(0x69);
		delay(0x80000);
		rx_data++;
	}
}
