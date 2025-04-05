/*
 * Timer.h
 *
 *  Created on: Oct 22, 2024
 *      Author: amr mohamed
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "std_types.h"

/**
 * Enumeration to define the timer IDs (Timer 0, Timer 1, Timer 2).
 * Used to identify which timer is being configured or accessed.
 */
typedef enum {
	Timer_0,  /** Timer 0 */
	Timer_1,  /** Timer 1 */
	Timer_2   /** Timer 2 */
} Timer_ID_Type;

/**
 * Enumeration to define the clock prescaler options for the timers.
 * It determines how the system clock is divided to set the timer's clock.
 */
typedef enum {
	NO_CLOCK,   /** No clock source (Timer stopped) */
	Fcpu_1,     /** No prescaling, timer runs at CPU clock */
	Fcpu_8,     /** CPU clock divided by 8 */
	Fcpu_64,    /** CPU clock divided by 64 */
	Fcpu_256,   /** CPU clock divided by 256 */
	Fcpu_1024   /** CPU clock divided by 1024 */
} Timer_ClockType;

/**
 * Enumeration to define the mode of operation for the timer.
 * The timer can operate in Normal Mode (overflow) or Compare Match Mode.
 */
typedef enum {
	NORMAL_MODE,   /** Normal mode (timer overflows when reaching max value) */
	COMPARE_MODE   /** Compare mode (timer resets when it matches compare value) */
} Timer_ModeType;

/**
 * Structure to hold the configuration parameters for the timer.
 * It includes the initial value, compare match value (used in compare mode),
 * the timer ID, clock source, and the mode of operation.
 */
typedef struct {
	uint16 timer_InitialValue;       /** Initial value to load into the timer counter */
	uint16 timer_compare_MatchValue; /** Compare match value (used in compare mode only) */
	Timer_ID_Type timer_ID;          /** Identifier for which timer to configure */
	Timer_ClockType timer_clock;     /** Clock prescaler selection */
	Timer_ModeType timer_mode;       /** Mode of operation (Normal or Compare Mode) */
} Timer_ConfigType;

/**
 * Function to initialize the specified timer with the given configuration.
 * It sets the timer's mode, initial value, compare match value (if applicable),
 * clock prescaler, and enables the relevant interrupts.
 *
 * Config_Ptr Pointer to a configuration structure containing timer settings.
 */
void Timer_init(const Timer_ConfigType * Config_Ptr);

/**
 * Function to de-initialize (stop) the specified timer.
 * It disables the timer, stops the clock, and clears the relevant registers.
 *
 * @param timer_type The ID of the timer to de-initialize (Timer_0, Timer_1, or Timer_2).
 */
void Timer_deInit(Timer_ID_Type timer_type);

/**
 * Function to set a callback function for the specified timer.
 * The callback is executed inside the timer interrupt service routine.
 *
 *  a_ptr Pointer to the callback function to be executed on the interrupt.
 *  a_timer_ID The ID of the timer for which the callback is set (Timer_0, Timer_1, or Timer_2).
 */
void Timer_setCallBack(void(*a_ptr)(void), Timer_ID_Type a_timer_ID);


#endif /* TIMER_H_ */
