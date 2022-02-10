#include "MKL25Z4.h"
/* Default Core Clk Frequencey is 20.97152MHz
	#include CLOCK_SETUP 1 in <system_MKL2574.h> to change it to Core Clk Freq to 48MHz */

/*
// TM: Timer, CH: Channel
PTB18 -> TM2 CH0
PTB19 -> TM2 CH1
PTB0 -> TM1 CH0
PTB1 -> TM1 CH1
*/

#define SW_POS 6
#define MASK(x) (1 << (x))

#define PTB0_Pin 0
#define PTB1_Pin 1
#define MUSICAL_NOTE_CNT 7
#define FREQ_2_MOD(x) (375000 / x)

int musical_notes[] = {262, 294, 330, 349, 392, 440, 494};

void initPWM(void) {
	// Enable Clock Gating for PORT B
	// Whenever any PORT is used, must enable clock gating for it
	// All port are map to SCGC5 Reg
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
	
	// Configure Mode 3 for the PWM pin operation
	PORTB->PCR[PTB0_Pin] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[PTB0_Pin] |= PORT_PCR_MUX(3); // The alternate function we wanted is ALT3 which is timer mode  
	
	PORTB->PCR[PTB1_Pin] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[PTB1_Pin] |= PORT_PCR_MUX(3); 
	
	// Enable Clock Gating for Timer1
	// Also must enable clock gating for the pheripheral currently mapped to PORT B
	SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK; // SCGC6 is for the TPM 
	
	// Select Clock for TPM module
	// For selecting the internal clock
	// CLOCK_SETUP 1 is 48 MHz
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1); // MCGFLLCLK or MCGPLLCLK/2
	
	// Set Modulo Value 20971520 / 128 = 163840 / 3276 = 50 Hz
	// TPM1->MOD = 3276;
	
	// Set Modulo Value 48000000 / 128 = 375000, 375000 / 7500 = 50 Hz, so you must count up to 7500 before you switch to get 50 Hz
	TPM1->MOD = 7500;
	
	// Edge-Aligned PWM 
	// Update SnC reg: CMOD = 01, PS = 111 (128)
	TPM1->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM1->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
	TPM1->SC &= ~(TPM_SC_CPWMS_MASK);
	
	// Enable PWM on TPM1 Channel 0 -> PTB0
	TPM1_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM1_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	
	// Enable PWM on TPM1 Channel 1 -> PTB1
	// Clear all the bits first
	// Set only the B bits for the ELSB and MSB, A bits will remain 0
	TPM1_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM1_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
}

void initSwitch(void) {
	// Enable clock for PortD
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	
	// Select GPIO and enable pull-up resistors and interrupts on
	PORTD->PCR[SW_POS] |= (PORT_PCR_MUX(1) |
												PORT_PCR_PS_MASK | 
												PORT_PCR_PE_MASK | 
												PORT_PCR_IRQC(0x0a));
	
	// Set PORT D Switch bit to input
	PTD->PDDR &= ~MASK(SW_POS); 
	
	// Enable Interrupts and config
	NVIC_SetPriority(PORTD_IRQn, 2); 
	NVIC_ClearPendingIRQ(PORTD_IRQn); 
	NVIC_EnableIRQ(PORTD_IRQn); 
}

int main(void) {
	char i = 0;
	SystemCoreClockUpdate();
	initPWM();
	// initSwtich();
	// TPM1_C0V = 0;
	
	
	// 0 is channel 0, and this cnV value is what you will count up to to create a match.
	// TPM1_C0V = 0x666;
	TPM1_C0V = 0xEA6; // 0xEA6 = 3750 (half of 7500) -> 50% duty cycle
	TPM1_C1V = 0x753; // 0x753 (half of 0xEA6) -> 25% duty cycle 
	
	/*
	for(;;) {
			for (i = 0; i < MUSICAL_NOTE_CNT; i++) {
				TPM1->MOD = FREQ_2_MOD(musical_notes[i]);
				TPM1_C0V = (FREQ_2_MOD(musical_notes[i]))
				delay_mult100(0xFFFF);
			}
			TPM1_C0V++;
			delay_mult100(0xFFFF);
	}
	*/
}