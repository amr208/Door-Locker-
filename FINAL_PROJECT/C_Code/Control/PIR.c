/*
 * PIR.c
 *
 *  Created on: Oct 22, 2024
 *      Author: amr mohamed
 */

#include "std_types.h"
#include "PIR.h"
#include "gpio.h"

void PIR_init(void)
{
	GPIO_setupPinDirection(PIR_PORT, PIR_PIN, PIN_INPUT);
}

uint8 PIR_getState(void)
{
	return GPIO_readPin(PIR_PORT, PIR_PIN);
}
