/*
 * Timer.c
 *
 *  Created on: Oct 22, 2024
 *      Author: amr mohamed
 */

#include <avr/io.h>
#include "common_macros.h"
#include "std_types.h"
#include "Timer.h"
#include <avr/interrupt.h> /* For the Timers ISR */

/**
 * Static volatile pointer to function (callback)
 *  1. Initialized to NULL_PTR.
 *  2. Used to store the address of the timer callback function.
 */
static volatile void (*g_CallBackTimer0)(void) = NULL_PTR;
static volatile void (*g_CallBackTimer1)(void) = NULL_PTR;
static volatile void (*g_CallBackTimer2)(void) = NULL_PTR;

ISR(TIMER0_OVF_vect)
{
    if(g_CallBackTimer0 != NULL_PTR)
    {
        /** Call the callback function */
        (*g_CallBackTimer0)();
    }
}

ISR(TIMER0_COMP_vect)
{
    if(g_CallBackTimer0 != NULL_PTR)
    {
        /** Call the callback function */
        (*g_CallBackTimer0)();
    }
}

ISR(TIMER1_OVF_vect)
{
    if(g_CallBackTimer1 != NULL_PTR)
    {
        /** Call the callback function */
        (*g_CallBackTimer1)();
    }
}

ISR(TIMER1_COMPA_vect)
{
    if(g_CallBackTimer1 != NULL_PTR)
    {
        /** Call the callback function */
        (*g_CallBackTimer1)();
    }
}

ISR(TIMER2_OVF_vect)
{
    if(g_CallBackTimer2 != NULL_PTR)
    {
        /** Call the callback function */
        (*g_CallBackTimer2)();
    }
}

ISR(TIMER2_COMP_vect)
{
    if(g_CallBackTimer2 != NULL_PTR)
    {
        /** Call the callback function */
        (*g_CallBackTimer2)();
    }
}

/*
 * The Timer_init function initializes a specified timer (Timer 0, Timer 1, or Timer 2)
 * based on the configuration provided in the Timer_ConfigType structure.
 * It sets the timer mode (Normal or Compare Match), configures the clock source,
 * enables interrupts as needed, loads compare match values, and clears the timer counter.
 */
void Timer_init(const Timer_ConfigType * Config_Ptr)
{
	switch (Config_Ptr ->timer_ID)
	{
		case Timer_0:
			/* Clear Timer 0 counter */
			TCNT0 = Config_Ptr -> timer_InitialValue;

			/* Check if Timer 0 is in Normal Mode */
			if(Config_Ptr -> timer_mode == NORMAL_MODE) {
				/* Enable Timer 0 Overflow Interrupt */
				TIMSK |= (1 << TOIE0);
				/* Set Timer 0 to Normal Mode and configure the clock source */
				TCCR0 = (1 << FOC0) | (Config_Ptr -> timer_clock);
			}
			/* Check if Timer 0 is in Compare Match Mode */
			else if(Config_Ptr -> timer_mode == COMPARE_MODE) {
				/* Set the compare match value for Timer 0 */
				OCR0 = Config_Ptr -> timer_compare_MatchValue;
				/* Enable Timer 0 Compare Match Interrupt */
				TIMSK |= (1 << OCIE0);
				/* Set Timer 0 to Compare Match Mode and configure the clock source */
				TCCR0 = (1 << FOC0) | (1 << WGM01) | (Config_Ptr -> timer_clock);
			}
			break;

		case Timer_1:
			/* Clear Timer 1 counter */
			TCNT1 = Config_Ptr -> timer_InitialValue;

			/* Check if Timer 1 is in Normal Mode */
			if(Config_Ptr -> timer_mode == NORMAL_MODE) {
				/* Enable Timer 1 Overflow Interrupt */
				TIMSK |= (1 << TOIE1);
				/* Set Timer 1 to Normal Mode and configure the clock source */
				TCCR1B = (Config_Ptr -> timer_clock);
			}
			/* Check if Timer 1 is in Compare Match Mode */
			else if(Config_Ptr -> timer_mode == COMPARE_MODE) {
				/* Set the compare match value for Timer 1 (Channel A) */
				OCR1A = Config_Ptr -> timer_compare_MatchValue;
				/* Enable Timer 1 Compare Match A Interrupt */
				TIMSK |= (1 << OCIE1A);
				/* Force Output Compare for Channel A in non-PWM mode */
				TCCR1A = (1 << FOC1A);
				/* Set Timer 1 to CTC Mode (Clear Timer on Compare Match) and configure the clock source */
				TCCR1B = (1 << WGM12) | (Config_Ptr -> timer_clock);
			}
			break;

		case Timer_2:
			/* Clear Timer 2 counter */
			TCNT2 = Config_Ptr -> timer_InitialValue;

			/* Check if Timer 2 is in Normal Mode */
			if(Config_Ptr -> timer_mode == NORMAL_MODE) {
				/* Enable Timer 2 Overflow Interrupt */
				TIMSK |= (1 << TOIE2);
				/* Set Timer 2 to Normal Mode and configure the clock source */
				TCCR2 = (1 << FOC2) | (Config_Ptr -> timer_clock);
			}
			/* Check if Timer 2 is in Compare Match Mode */
			else if(Config_Ptr -> timer_mode == COMPARE_MODE) {
				/* Set the compare match value for Timer 2 */
				OCR2 = Config_Ptr -> timer_compare_MatchValue;
				/* Enable Timer 2 Compare Match Interrupt */
				TIMSK |= (1 << OCIE2);
				/* Set Timer 2 to Compare Match Mode and configure the clock source */
				TCCR2 = (1 << FOC2) | (1 << WGM21) | (Config_Ptr -> timer_clock);
			}
			break;
	}
}

/*Disabling all registers*/
void Timer_deInit(Timer_ID_Type timer_type)
{
    switch (timer_type)
    {

        case Timer_0:
            /* Reset all relevant Timer 0 registers to 0 */
            TCCR0 = 0;   /* Disable clock and reset mode */
            TIMSK &= ~( (1 << TOIE0) | (1 << OCIE0) ); /* Disable interrupts */
            TCNT0 = 0;   /* Clear counter */
            OCR0 = 0;    /* Clear compare match register */
            break;

        case Timer_1:
            TCCR1A = 0;  /* Reset mode and FOC bits */
            TCCR1B = 0;  /* Disable clock */
            TIMSK &= ~( (1 << TOIE1) | (1 << OCIE1A) | (1 << OCIE1B) ); /* Disable interrupts */
            TCNT1 = 0;   /* Clear counter */
            OCR1A = 0;   /* Clear compare match register A */
            OCR1B = 0;   /* Clear compare match register B */
            break;

        case Timer_2:
            TCCR2 = 0;   /* Disable clock and reset mode */
            TIMSK &= ~( (1 << TOIE2) | (1 << OCIE2) ); /* Disable interrupts */
            TCNT2 = 0;   /* Clear counter */
            OCR2 = 0;    /* Clear compare match register */
            break;
    }
}

void Timer_setCallBack(void(*a_ptr)(void), Timer_ID_Type a_timer_ID )
{
    switch (a_timer_ID)
    {
        case Timer_0:
        	g_CallBackTimer0 = a_ptr;
            break;

        case Timer_1:
        	g_CallBackTimer1 = a_ptr;
            break;

        case Timer_2:
        	g_CallBackTimer2 = a_ptr;
            break;
    }
}

