// Speaker module for ECE 371 motion alarm project
// Author: Lucas Wagler (ldwagler15@my.trine.edu)

#include "speaker.h"

queue_t speakerQ;

static uint32_t tick_count = 0;
static uint8_t movement_flag = 0;

void InitTpm1()
{
	SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	
	SIM->SOPT2 |= (SIM_SOPT2_TPMSRC(1) | SIM_SOPT2_PLLFLLSEL_MASK);
	
	TPM1->MOD = 12000UL;
	
	TPM1->CONTROLS[0].CnSC = TPM_CnSC_MSA_MASK | TPM_CnSC_ELSA_MASK | TPM_CnSC_CHIE_MASK;
	
	// PORTE->PCR[20] &= ~PORT_PCR_MUX_MASK;
	// PORTE->PCR[20] &= PORT_PCR_MUX(3);
	
	TPM1->SC = TPM_SC_CMOD(1) | TPM_SC_PS(0) | TPM_SC_TOIE_MASK;
	NVIC_SetPriority(TPM1_IRQn, 3);
	NVIC_ClearPendingIRQ(TPM1_IRQn);
	NVIC_EnableIRQ(TPM1_IRQn);
	__enable_irq();
}

void TPM1_IRQHandler()
{
	TPM1->SC |= TPM_SC_TOIE_MASK;
	TPM1->STATUS |= TPM_STATUS_CH0F_MASK | TPM_STATUS_CH1F_MASK | TPM_STATUS_TOF_MASK;
	PTC->PTOR |= (1UL << SPEAKER_PIN);
	tick_count++;
	PTB->PTOR = 1 << 18;
	// TPM1->SC |= TPM_SC_TOF_MASK;
}

void InitPort()
{
	// Speaker Pin
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK; // could turn on alarm by controlling clock gate
	PORTC->PCR[SPEAKER_PIN] &= ~PORT_PCR_MUX_MASK; //potential pain point if done elsewhere in project?
	PORTC->PCR[SPEAKER_PIN] |= PORT_PCR_MUX(1);
	PTC->PDDR |= (1UL << SPEAKER_PIN);
	PTC->PCOR |= (1UL << SPEAKER_PIN);
	
	// Test Pin (Blue LED)
	// SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	// PORTD->PCR[test_pin] &= ~PORT_PCR_MUX_MASK; //potential pain point if done elsewhere in project?
	// PORTD->PCR[test_pin] |= PORT_PCR_MUX(1);
	// PTD->PDDR |= (1UL << test_pin);
	// PTD->PSOR |= (1UL << test_pin);
}

void SoundOff()
{
	PTC->PDDR &= ~(1UL << SPEAKER_PIN);
}

void SoundOn()
{
	PTC->PDDR |= (1UL << SPEAKER_PIN);
}

void InitSpeaker()
{
	InitPort();
	SoundOff();
	InitTpm1();
}

void speaker_task()
{
	static bool moved = false;
	static int sec_count = 0;
	
	if(tick_count >= 4000)
	{
		sec_count++;
		tick_count = 0;//tick_count - 4000; // reset to number of ticks overflowed
	}
	
	if(get_q(&speakerQ, &movement_flag))
	{
		if(movement_flag)
		{
			moved = true;
		}
	}
	
	if(moved)
	{
		if(sec_count < 3)
		{
			SoundOn();
			// PTD->PCOR |= (1UL << test_pin);
			// display "beep"
		}
		else if(sec_count >= 3)
		{
			SoundOff();
			// PTD->PSOR |= (1UL << test_pin);
			// clear display
		}
		if(sec_count >= 10)
		{
			sec_count = 0;
			moved = false;
		}
	}
}

/*
// for testing
int main()
{
	init_q(&speakerQ, (int)1);
	InitSpeaker();
	put_q(&speakerQ, (uint8_t)1);
	//put_q(&speakerQ, (uint8_t)0);
	
	while(1)
	{		
		speaker_task();		
	}
}
*/
