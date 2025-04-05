/*
 * motor.c
 *
 *  Created on: Oct 3, 2024
 *      Author: amr mohamed
 */
#include "motor.h"
#include "gpio.h"
#include "PWM.h"
#include "std_types.h"
#include <avr/io.h>

/*Important global variable that indicated whether the fan is turned on or off*/
uint8 fanState;

/*	Motor initialization, IN1, IN2, ENABLE pins are output
 *  initially, the fan state is OFF*/
void DcMotor_Init(void)
{
	GPIO_setupPinDirection(MOTOR_ENABLE_PORT_ID, MOTOR_ENABLE_PIN_ID, PIN_OUTPUT);
	GPIO_setupPinDirection(MOTOR_IN1_PORT_ID, MOTOR_IN1_PIN_ID, PIN_OUTPUT);
	GPIO_setupPinDirection(MOTOR_IN2_PORT_ID, MOTOR_IN2_PIN_ID, PIN_OUTPUT);

	GPIO_writePin(MOTOR_IN1_PORT_ID, MOTOR_IN1_PIN_ID, LOGIC_LOW);
	GPIO_writePin(MOTOR_IN2_PORT_ID, MOTOR_IN2_PIN_ID, LOGIC_LOW);

	fanState = LOGIC_LOW;
}

/*checking the state of motor, and it's rotation speed*/
void DcMotor_Rotate(DcMotor_State state, uint8 speed)
{
	switch (state) {
		case CW:
			fanState = LOGIC_HIGH;
			GPIO_writePin(MOTOR_IN1_PORT_ID, MOTOR_IN1_PIN_ID, LOGIC_LOW);
			GPIO_writePin(MOTOR_IN2_PORT_ID, MOTOR_IN2_PIN_ID, LOGIC_HIGH);
			break;
		case A_CW:
			fanState = LOGIC_HIGH;
			GPIO_writePin(MOTOR_IN1_PORT_ID, MOTOR_IN1_PIN_ID, LOGIC_HIGH);
			GPIO_writePin(MOTOR_IN2_PORT_ID, MOTOR_IN2_PIN_ID, LOGIC_LOW);
			break;
		case STOP:
			fanState = LOGIC_LOW;
			GPIO_writePin(MOTOR_IN1_PORT_ID, MOTOR_IN1_PIN_ID, LOGIC_LOW);
			GPIO_writePin(MOTOR_IN2_PORT_ID, MOTOR_IN2_PIN_ID, LOGIC_LOW);
			break;
	}

//	/*putting the speed to the duty cycle*/
//	PWM_Timer0_Start(speed);
}
