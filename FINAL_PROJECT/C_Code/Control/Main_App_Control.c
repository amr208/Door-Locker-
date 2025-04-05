/*
 * Main_App_Control.c
 *
 *  Created on: Oct 24, 2024
 *      Author: amr mohamed
 */

#include "buzzer.h"
#include "common_macros.h"
#include "external_eeprom.h"
#include "gpio.h"
#include "I2C.h"
#include "motor.h"
#include "PIR.h"
#include "PWM.h"
#include "std_types.h"
#include "Timer.h"
#include "UART.h"
#include <util/delay.h>
#include <avr/io.h> /* To use the SREG register */

/*
 * Variable to track the elapsed time in seconds.
 * This is used to manage timing for various operations, such as door opening and closing.
 */
uint8 seconds = 0;

/*
 * Variable to count the number of consecutive password mismatches.
 * This is used to trigger alarm conditions after three failed attempts.
 */
uint8 g_error = 0;

/*
 * Variable to manage different phases of the system.
 * This is used to control state transitions within the functions in here.
 */
uint8 phaseSwitches = 1;
/************************************************************************************************************/
/************************************************************************************************************/
/*
 * Enumeration to define the different states of the door motor.
 * This helps in managing the door's operation and transitions between states.
 */
typedef enum {
    OPENING_DOOR,      /* State when the door is in the process of opening */
    WAITING_FOR_PEOPLE, /* State when the door is open and waiting for people to enter */
    CLOSING_DOOR,     /* State when the door is in the process of closing */
    Done               /* State indicating the operation is complete */
} doorState;

/*
 * Initialize the timer state for the door motor to OPENING_DOOR.
 * This sets the initial state of the system when it starts.
 */
doorState timerState = OPENING_DOOR;

/*
 * Variable to track the alarm state.
 * This is set to 0 initially, and may be updated based on system conditions.
 */
uint8 alarmState = 0;

/*
 * Enumeration to define the different UART states for door operations.
 * These constants are used to send specific commands to the HMI.
 */
typedef enum {
    OPEN_BYTE = 'a',  /* Command to indicate the door is opening */
    WAIT_BYTE = 'b',  /* Command to indicate the system is waiting for entry */
    CLOSE_BYTE = 'c', /* Command to indicate the door is closing */
    Alarm_Byte = 'd'  /* Command to indicate an alarm condition */
} UARTDoorState;
/************************************************************************************************************/
/************************************************************************************************************/
/*
 * Function to check and store passwords.
 * This function receives a password from the HMI, compares it with the stored password in EEPROM,
 * and handles errors if there are mismatches.
 */
void passStoreCheck(void);

/*
 * Function to manage the door motor's operation.
 * This function handles the states of opening, waiting for people, and closing the door,
 * including sending appropriate signals to the HMI.
 */
void doorHandler(void);

/*
 * Function to handle the alarm state.
 * This function activates a buzzer for 60 seconds if the password is entered incorrectly three times,
 * and then resets the system to normal operations.
 */
void alarmStage(void);
/************************************************************************************************************/
/************************************************************************************************************/
void timerCallBackRuntime(void) {
    /* Switch statement to handle different timer states */
    switch (timerState) {
        /* State for opening the door */
        case OPENING_DOOR:
            seconds++;  /* Increment the seconds counter while the door is opening */
            break;

        /* State for waiting for people to enter */
        case WAITING_FOR_PEOPLE:
            seconds = 0;  /* Reset seconds counter while waiting for people */
            break;

        /* State for closing the door */
        case CLOSING_DOOR:
            seconds++;  /* Increment the seconds counter while the door is closing */
            break;

        /* State indicating the operation is done */
        case Done:
            seconds = 0;  /* Reset seconds counter when the operation is complete */
            timerState = OPENING_DOOR;
            break;
    }

    /* Check if the alarm state is activated */
    if (alarmState == 0xFF) {
        seconds++;  /* Increment the seconds counter if the alarm is active */
    }
}
/************************************************************************************************************/
/************************************************************************************************************/
int main(void) {
    /* Configuration structures for various peripherals */
    UART_Config UARTRuntime = {9600, DISABLED, EIGHT_BITS, ONE_BIT};  /* UART configuration */
    Timer_ConfigType TimerRuntime = {0, 2930, Timer_1, Fcpu_1024, COMPARE_MODE};  /* Timer configuration */
    I2C_Config I2CRuntime = {CPU_8MHZ, I2C_400KHZ, 0xAA};  /* I2C configuration with address 0xAA */

    /* Initialize peripherals */
    Timer_init(&TimerRuntime);  /* Initialize the timer with the specified configuration */
    UART_Init(&UARTRuntime);    /* Initialize UART communication */
    DcMotor_Init();              /* Initialize the DC motor control */
    PIR_init();                  /* Initialize the PIR sensor */
    Buzzer_init();               /* Initialize the buzzer */
    PWM_Timer0_Start(100);      /* Start PWM on Timer0 with a duty cycle of 100 */
    I2C_init(&I2CRuntime);       /* Initialize I2C communication */
    Timer_setCallBack(timerCallBackRuntime, Timer_1);  /* Set the callback function for Timer_1 */

    /* Main loop */
    while (1) {
        /* Check the current phase and execute the corresponding function */
        if (phaseSwitches == 1) {
            SREG &= ~(1<<7);  /* Disable Global Interrupt (I-Bit) for safe operation */
            passStoreCheck();  /* Call the function to check and store passwords */
        }
        else if (phaseSwitches == 2) {
            SREG |= (1<<7);  /* Enable Global Interrupt (I-Bit) for door handling */
            doorHandler();    /* Call the function to manage the door operation */
        }
        else if (phaseSwitches == 3) {
            SREG |= (1<<7);  /* Enable Global Interrupt (I-Bit) for alarm handling */
            alarmStage();     /* Call the function to manage the alarm state */
        }
    }
}

/*
 * This function interfaces with the Human-Machine Interface (HMI) as follows:
 *
 * 1. Initially, it waits for a byte ('S'), which indicates successful password initialization or re-entry.
 * 2. Upon receiving 'S', it stores 5 password digits (bytes) in the EEPROM.
 * 3. The function also listens for a byte ('F'), which signals a password verification request against the stored EEPROM password.
 * 4. If a mismatch occurs during verification, a global variable (g_error) tracks the number of mismatches.
 * 5. An indication of failure ('Z') is sent to the HMI to prompt the user to try again.
 * 6. If g_error reaches 3, the function transitions to an alarm state.
 */
void passStoreCheck(void) {
    /* Static variables to hold the state of the password storage and comparison */
    static uint8 storedByte, storeLimit, compareByte, RByte;
    uint8 flag = 0;  /* Flag to indicate if a mismatch has occurred */

    /* Receive a byte from UART */
    RByte = UART_recieveByte();

    /* Check if the received byte is 'F' to start password comparison */
    if (RByte == 'F') {
        storeLimit = 0;  /* Reset store limit for comparison */

        /* Loop to compare 5 stored password bytes */
        while (storeLimit < 5) {
            storedByte = UART_recieveByte();  /* Receive the password byte */

            /* Check if the received byte is not a space */
            if (storedByte != ' ') {

                /* Read the stored byte from EEPROM for comparison */
                if (EEPROM_readByte(0x0001 + storeLimit, &compareByte) == ERROR) {
                    /* Set a GPIO pin high if there's an error reading EEPROM */
                    GPIO_writePin(PORTA_ID, PIN0_ID, LOGIC_HIGH);
                }

                /* Compare the received byte with the stored byte */
                if (storedByte != compareByte) {
                    flag = 1;  /* Set flag indicating a mismatch */

                    /* Increment error count */
                    g_error++;
                    /* Check if error count has reached 3 */
                    if (g_error == 3) {
                        alarmState = 0xFF;  /* Trigger alarm state */
                        phaseSwitches = 3;   /* Change phase */
                        g_error = 0;         /* Reset error count */
                        UART_sendByte(Alarm_Byte);  /* Send alarm byte */
                    } else {
                        UART_sendByte('Z');  /* Send indication of mismatch */
                    }

                    break;  /* Exit the loop on mismatch */
                }
                storeLimit++;  /* Increment store limit for next byte */
            }
        }

        /* If no mismatch was found */
        if (flag == 0) {
            UART_sendByte('X');  /* Send indication of successful match */
            phaseSwitches = 2;    /* Change phase */
        }

    /* Check if the received byte is 'S' to start storing the password */
    } else if (RByte == 'S') {
        storeLimit = 0;  /* Reset store limit for storing new password */

        /* Loop to store 5 password bytes */
        while (storeLimit < 5) {
            storedByte = UART_recieveByte();  /* Receive the password byte */

            /* Check if the received byte is not a space */
            if (storedByte != ' ') {
                /* Write the received byte to EEPROM */
                EEPROM_writeByte(0x0001 + storeLimit, storedByte);
                _delay_ms(10);  /* Delay to allow EEPROM write completion */
                storeLimit++;  /* Increment store limit for next byte */
            }
        }
        storeLimit = 0;  /* Reset store limit after storing */
    }
}

/*
 * This function manages the operation of a door motor based on the following states:
 *
 * 1. OPENING_DOOR:
 *    - The motor runs forward (clockwise) for 15 seconds to open the door.
 *    - If the state has not been previously sent, it sends the OPEN_BYTE to the UART.
 *    - After 15 seconds, the motor stops, and the state transitions to WAITING_FOR_PEOPLE.
 *
 * 2. WAITING_FOR_PEOPLE:
 *    - The motor remains stopped while waiting for people to enter.
 *    - If the state has not been previously sent, it sends the WAIT_BYTE to the UART.
 *    - The function checks the PIR sensor state; if the sensor detects motion (LOGIC_LOW), it transitions to CLOSING_DOOR.
 *
 * 3. CLOSING_DOOR:
 *    - The motor runs backward (anti-clockwise) for 15 seconds to close the door.
 *    - If the state has not been previously sent, it sends the CLOSE_BYTE to the UART.
 *    - After 15 seconds, the motor stops, and the state resets to OPENING_DOOR, completing the cycle.
 *    - It also sends a completion byte (Done) to indicate the operation is finished.
 *
 * The function uses a static state variable to track the current motor state and a flag to ensure that state change notifications are sent only once per state.
 */
void doorHandler(void) {
    /* Static variable to track the current state of the door motor */
    static doorState motorState = OPENING_DOOR;
    static uint8_t byteSent = 0;  /* Flag to indicate if a byte was sent for the current state */


    /* Switch statement to handle different motor states */
    switch (motorState) {

        /* State for opening the door */
        case OPENING_DOOR:
            /* Check if the byte has not been sent yet */
            if (!byteSent) {
                UART_sendByte(OPEN_BYTE);  /* Send a byte to indicate the door is opening */
                byteSent = 1;  /* Set flag to prevent re-sending */
            }

            DcMotor_Rotate(CW, 100);  /* Rotate the motor clockwise to open the door */

            /* Check if 15 seconds have passed */
            if (seconds > 15) {
                DcMotor_Rotate(STOP, 0);  /* Stop the motor */
                motorState = WAITING_FOR_PEOPLE;  /* Transition to waiting state */
                byteSent = 0;  /* Reset byte sent flag */
                timerState = WAITING_FOR_PEOPLE;  /* Update timer state */
            }
            break;

        /* State for waiting for people to enter */
        case WAITING_FOR_PEOPLE:
            DcMotor_Rotate(STOP, 0);  /* Keep the motor stopped */

            /* Check if the byte has not been sent yet */
            if (!byteSent) {
                UART_sendByte(WAIT_BYTE);  /* Send a byte indicating the system is waiting */
                byteSent = 1;  /* Set flag to prevent re-sending */
            }

            /* Check the PIR sensor state */
            if (PIR_getState() == LOGIC_LOW) {
                motorState = CLOSING_DOOR;  /* Transition to closing state if motion is detected */
                byteSent = 0;  /* Reset byte sent flag */
                timerState = CLOSING_DOOR;  /* Update timer state */
            }
            break;

        /* State for closing the door */
        case CLOSING_DOOR:
            /* Check if the byte has not been sent yet */
            if (!byteSent) {
                UART_sendByte(CLOSE_BYTE);  /* Send a byte to indicate the door is closing */
                byteSent = 1;  /* Set flag to prevent re-sending */
            }

            DcMotor_Rotate(A_CW, 100);  /* Rotate the motor counter-clockwise to close the door */

            /* Check if 15 seconds have passed */
            if (seconds > 15) {
                DcMotor_Rotate(STOP, 0);  /* Stop the motor */
                motorState = OPENING_DOOR;  /* Reset state to opening */
                phaseSwitches = 1;  /* Update phase switches */
                byteSent = 0;  /* Reset byte sent flag */
                timerState = Done;  /* Update timer state */
                UART_sendByte(Done);  /* Send a completion byte indicating the operation is finished */
            }
            break;
    }
}

/*
 * this function works as follows:
 *
 * 1. if the password is wrong for 3 consecutive times, we come here
 * 2. i turn the buzzer on for 60 seconds, while i already in the first function sent to the HMI to display error message.
 * 3. after the 60 seconds, everything returns to the inital state and the system returns to the normal operations
 * */
void alarmStage(void) {
    /* Static variable to track if a byte has been sent for this state */
    static uint8_t byteSent = 0;

    /* Check if the elapsed time is less than 60 seconds */
    if (seconds < 60)
    {
        Buzzer_on();  /* Activate the buzzer to indicate an alarm state */
    }
    /* If 60 seconds have passed */
    else {
        /* Check if the byte has not been sent yet */
        if (!byteSent)
        {
            UART_sendByte(OPEN_BYTE);  /* Send a byte to inform the HMI of the alarm state */
            byteSent = 1;  /* Set flag to prevent re-sending */
        }

        Buzzer_off();  /* Deactivate the buzzer after the alarm duration */
        byteSent = 0;  /* Reset the byte sent flag for future operations */
        alarmState = 0;  /* Reset the alarm state to indicate normal operation */
        phaseSwitches = 1;  /* Update phase switches to return to normal operations */
    }
}

