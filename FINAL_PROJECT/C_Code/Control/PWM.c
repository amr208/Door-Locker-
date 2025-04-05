/*
 * PWM.c
 *
 *  Created on: Oct 3, 2024
 *      Author: amr mohamed
 */
#include "PWM.h"
#include "std_types.h"
#include "common_macros.h"
#include <avr/io.h>
#include "gpio.h"



void PWM_Timer0_Start(uint8 duty_cycle) {
	TCNT0 = 0; // Set Timer Initial Value to 0
	/* configure the timer
	 * 1. Fast PWM mode FOC0=0
	 * 2. Fast PWM Mode WGM01=1 & WGM00=1
	 * 3. Clear OC0 when match occurs (non inverted mode) COM00=0 & COM01=1
	 * 4. clock = F_CPU/64 CS00=1, CS01=1, CS02=0
	 */
	TCCR0 = (1 << WGM00) | (1 << WGM01) | (1 << COM01) | (1 << CS01) | (1 << CS00);

	OCR0 = (uint8)(duty_cycle * 2.55);
	GPIO_setupPinDirection(PORTB_ID, PIN3_ID, PIN_OUTPUT);/*set PB3/OC0 as output pin*/
}

