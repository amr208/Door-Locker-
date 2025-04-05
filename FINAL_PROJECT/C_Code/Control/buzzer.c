/*
 * buzzer.c
 *
 *  Created on: Oct 3, 2024
 *      Author: amr mohamed
 */


#include "buzzer.h"
#include "std_types.h"
#include "gpio.h"
#include "common_macros.h"

/*Buzzer initialization (Pin direction and write 0 on it)*/
void Buzzer_init(void)
{
	GPIO_setupPinDirection(BUZZER_PORT_ID, BUZZER_PIN_ID, PIN_OUTPUT);
	GPIO_writePin(BUZZER_PORT_ID, BUZZER_PIN_ID, LOGIC_LOW);
}

/*Turning the Buzzer on (Putting 1 on the pin)*/
void Buzzer_on(void)
{
	GPIO_writePin(BUZZER_PORT_ID, BUZZER_PIN_ID, LOGIC_HIGH);
}

/*Turning the Buzzer on (Putting 0 on the pin)*/
void Buzzer_off(void)
{
	GPIO_writePin(BUZZER_PORT_ID, BUZZER_PIN_ID, LOGIC_LOW);
}
