/*
 * Main_App.c
 *
 *  Created on: Oct 24, 2024
 *      Author: amr mohamed
 */

#include <avr/io.h>
#include "common_macros.h"
#include "gpio.h"
#include "keypad.h"
#include "LCD.h"
#include "std_types.h"
#include "Timer.h"
#include "UART.h"
#include <util/delay.h>
#include <stdlib.h>

// Enumeration to define the different states for UART communication related to door operations
typedef enum
{
    OPEN_BYTE = 'a',    // Command to indicate the door is opening
    WAIT_BYTE = 'b',    // Command to indicate the system is waiting for people to enter
    CLOSE_BYTE = 'c',   // Command to indicate the door is closing
    Alarm_BYTE = 'd'    // Command to indicate an alarm condition triggered by incorrect password attempts
} UARTDoorState;


// Variable to manage phase transitions within the system
uint8 PhasesSwitch = 1;

// Array to store the initial or reset password entered by the user
uint8 passSetArr[5];

// Array to store the password entered for comparison during verification
uint8 passCompareArr[5];


void phaseOne(void);    /* Initial password setup or reset*/
void phaseTwo(void);    /* Password verification and error tracking*/
void phaseThree(void);  /* Options to open door or change password*/
void phaseFour(void);   /* Door operation status display*/
void phaseFive(void);   /* System lock display after multiple failed attempts*/
int main(void)
{
    // UART configuration: Baud rate 9600, No parity, 8 data bits, 1 stop bit
    UART_Config UARTRuntime = {9600, DISABLED, EIGHT_BITS, ONE_BIT};

    // Initialize the LCD display
    LCD_init();

    // Initialize UART communication with specified settings
    UART_Init(&UARTRuntime);

    // Main control loop
    while(1)
    {
        // If in phase 1 or reset phase (6), handle initialization or reset logic
        if(PhasesSwitch == 1 || PhasesSwitch == 6)
        {
            phaseOne();
        }

        // If in phase 2, handle password verification
        else if(PhasesSwitch == 2)
        {
            phaseTwo();
        }

        // If in phase 3, display options for door or password change
        else if(PhasesSwitch == 3)
        {
            phaseThree();
        }

        // If in phase 4, display door state messages
        else if(PhasesSwitch == 4)
        {
            phaseFour();
        }

        // If in phase 5, activate lock mode due to failed password attempts
        else if(PhasesSwitch == 5)
        {
            phaseFive();
        }
    }
}


/*
 * This function is responsible for displaying the "PLZ ENTER PASS" message when entering the password.
 * It is also shown when clicking (+) or (-) in phase three, and when unmatched passwords occur.
 */
void phaseOne(void) {
    uint8 var;  /* Variables for temporary storage of key input and verification */
    static uint8 passDigit, initialPassLimit; /* Variables to store the pressed key and count of entered characters */

    /* Clear the LCD display */
    LCD_ClearScreen();

    /* Move the LCD cursor to the first row, first column */
    LCD_MoveCursor(0, 0);

    /* Display the message prompting the user to enter the password */
    LCD_SendString("PLZ enter pass:");

    /* Loop until the '=' key is pressed or 5 characters have been entered */
    while (KEYPAD_getPressedKey() != '=' || initialPassLimit < 5) {
        /* Get the key pressed from the keypad */
    	passDigit = KEYPAD_getPressedKey();

        /* Check if the key is a valid number (0-9) and ensure less than 5 characters have been entered */
        if ((passDigit <= 9) && (passDigit >= 0) && initialPassLimit < 5) {
            /* Move the cursor to the corresponding position in the second row */
            LCD_MoveCursor(1, initialPassLimit);

            /* Display a '*' character for each entered digit (masks the actual input) */
            LCD_SendCharacter('*');

            /* Store the pressed key in the password array */
            passSetArr[initialPassLimit] = passDigit;

            /* Increment the count to move to the next position on the display */
            initialPassLimit++;

            /* Small delay to allow time for the key press and avoid multiple detections */
            _delay_ms(250);
        }
    }

    /* Check if the '=' key was pressed and exactly 5 characters have been entered */
    if (KEYPAD_getPressedKey() == '=' && initialPassLimit == 5) {
        /* If in phase 6, send the password for verification */
        if (PhasesSwitch == 6) {
            UART_sendByte('F');  /* Indicate that password entry has started */
            for (var = 0; var < 5; ++var) {
                UART_sendByte(passSetArr[var]);  /* Send each digit of the password */
                _delay_ms(10);  /* Short delay between sends */
            }

            /* Wait for a response from the HMI */
            while (1) {
                uint8 temp = UART_recieveByte();  /* Receive a byte from UART */
                if (temp == 'Z') {
                    break;  /* Exit the loop on valid response */
                } else if (temp == 'X') {
                    LCD_ClearScreen();  /* Clear the display on mismatch */
                    initialPassLimit = 0;  /* Reset password limit */
                    PhasesSwitch = 4;   /* Transition to phase 4 */
                    break;  /* Exit the loop */
                } else if (temp == Alarm_BYTE) {
                    /* Handle alarm condition */
                	initialPassLimit = 0;  /* Reset password limit */
                    PhasesSwitch = 5;   /* Transition to alarm phase */
                    break;  /* Exit the loop */
                }
            }
            initialPassLimit = 0;  /* Reset password limit after processing */
        } else {
        	initialPassLimit = 0;  /* Reset password limit */
            PhasesSwitch = 2;    /* Transition to phase 2 */
        }
    }
}


/*
 * This phase is responsible for displaying the "Re_enter pass" message when confirming the password.
 */
void phaseTwo(void) {
    uint8 var, wrongPass = 0, passLimit = 0;  /* Variables for tracking the number of entered characters and error state */
    static uint8 passDigit;  /* Variable to store the pressed key */

    /* Clear the LCD display */
    LCD_ClearScreen();

    /* Move the LCD cursor to the first row, first column */
    LCD_MoveCursor(0, 0);

    /* Display the message prompting the user to re-enter the password */
    LCD_SendString("Re_enter pass:  ");

    /* Loop until the '=' key is pressed or 5 characters have been entered */
    while (KEYPAD_getPressedKey() != '=' || passLimit < 5) {
        /* Get the key pressed from the keypad */
    	passDigit = KEYPAD_getPressedKey();

        /* Check if the key is a valid number (0-9) and ensure less than 5 characters have been entered */
        if ((passDigit <= 9) && (passDigit >= 0) && passLimit < 5) {
            /* Move the cursor to the corresponding position in the second row */
            LCD_MoveCursor(1, passLimit);

            /* Display a '*' character for each entered digit (masks the actual input) */
            LCD_SendCharacter('*');

            /* Store the pressed key in the comparison array */
            passCompareArr[passLimit] = passDigit;

            /* Increment the count to move to the next position on the display */
            passLimit++;

            /* Small delay to allow time for the key press and avoid multiple detections */
            _delay_ms(250);
        }
    }

    /* Check if the '=' key was pressed and exactly 5 characters have been entered */
    if (KEYPAD_getPressedKey() == '=' && passLimit == 5) {
        /* Compare the entered password with the stored password */
        for (var = 0; var < 5; ++var) {
            if (passSetArr[var] != passCompareArr[var]) {
                passLimit = 0;  /* Reset password limit on mismatch */
                wrongPass = 1;  /* Set flag indicating the passwords do not match */
            }
        }

        /* If a mismatch occurred, transition back to phase 1 */
        if (wrongPass == 1) {
            PhasesSwitch = 1;  /* Transition to phase 1 for re-entry */
        } else {
            /* If passwords match, send confirmation to the HMI */
            UART_sendByte('S');  /* Indicate successful password entry */
            for (var = 0; var < 5; ++var) {
                UART_sendByte(passSetArr[var]);  /* Send the stored password for verification */
                _delay_ms(10);  /* Short delay between sends */
            }
            PhasesSwitch = 3;  /* Transition to phase 3 */
        }
    }
}

/*
 * Function: phaseThree
 * --------------------
 * This function displays two options on the LCD:
 * 1. Open Door: Triggered by pressing the '+' key.
 * 2. Change Password: Triggered by pressing the '-' key.
 *
 * If the '+' key is pressed, it transitions to phase 6 (door opening),
 * sends the '+' command over UART to the Main Controller to start the door-opening process.
 * If the '-' key is pressed, it clears the screen, transitions to phase 1,
 * and sends the '-' command over UART to initiate the password change process.
 */
void phaseThree(void)
{
    LCD_MoveCursor(0, 0);
    LCD_SendString("+ : Open Door   ");
    LCD_MoveCursor(1, 0);
    LCD_SendString("- : Change Pass ");

    if (KEYPAD_getPressedKey() == '+')
    {
        LCD_ClearScreen();
        PhasesSwitch = 6;    // Switch to door opening phase
        UART_sendByte('+');  // Notify Main Controller to open the door
    }
    else if (KEYPAD_getPressedKey() == '-')
    {
        LCD_ClearScreen();
        PhasesSwitch = 1;    // Switch to password change phase
        UART_sendByte('-');  // Notify Main Controller to start password change
    }
}

/*
 * Function: phaseFour
 * --------------------
 * This function handles door operation states based on UART feedback:
 * 1. OPEN_BYTE: Indicates that the door is unlocking, prompting a message.
 * 2. WAIT_BYTE: Indicates that the system is waiting for people to enter,
 *               displaying a waiting message.
 * 3. CLOSE_BYTE: Indicates that the door is locking, updating the display.
 *
 * The function listens for a byte sent from the Main Controller through UART
 * to update the LCD display with the current state of the door operation.
 * If no valid state is received, it returns to phase 3.
 */
void phaseFour(void)
{
    static UARTDoorState doorState;
    doorState = UART_recieveByte();  // Receive door state byte from Main Controller

    switch (doorState)
    {
    case OPEN_BYTE:
        LCD_MoveCursor(0, 0);
        LCD_SendString("Door Unlocking  ");
        LCD_MoveCursor(1, 0);
        LCD_SendString("Please wait..   ");
        break;
    case WAIT_BYTE:
        LCD_MoveCursor(0, 0);
        LCD_SendString("Wait For People ");
        LCD_MoveCursor(1, 0);
        LCD_SendString("   to enter..   ");
        break;
    case CLOSE_BYTE:
        LCD_MoveCursor(0, 0);
        LCD_SendString("  Door locking  ");
        LCD_MoveCursor(1, 0);
        LCD_SendString("                ");
        break;
    default:
        PhasesSwitch = 3;  // Return to main options if no valid byte is received
        break;
    }
}

/*
 * Function: phaseFive
 * --------------------
 * This function handles the system's locked state after three failed
 * password attempts. It displays a "SYSTEM LOCKED" message, indicating
 * that the system will remain locked for one minute.
 *
 * A static UART variable listens for a byte from the Main Controller
 * to check if the lock period has ended. Upon receiving the OPEN_BYTE,
 * it resets PhasesSwitch to 3, returning the system to the main options.
 * It also toggles a bit on PORTA, PIN0, which could be used for a visual
 * or audible indicator that the system is locked.
 */
void phaseFive(void)
{
	static UARTDoorState alarmState;
    LCD_MoveCursor(0, 0);
    LCD_SendString("SYSTEM LOCKED   ");
    LCD_MoveCursor(1, 0);
    LCD_SendString("Wait for 1 min  ");

    TOGGLE_BIT(PORTA, 0);  // Toggle indicator for system locked state
    alarmState = UART_recieveByte();  // Receive state from Main Controller

    if (alarmState == OPEN_BYTE)
    {
        PhasesSwitch = 3;  // Return to main options upon unlock
    }
}
