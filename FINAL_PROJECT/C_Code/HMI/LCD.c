/*
 * LCD.c
 *
 *  Created on: Sep 27, 2024
 *      Author: amr mohamed
 */

/*
 * LCD Driver
 *
 * The driver supports:
 * - Sending commands and characters to the LCD.
 * - Displaying strings and numbers.
 * - Moving the cursor to specific positions.
 * - Clearing the display.
 *
 * Features:
 * 1. LCD Initialization:
 *    - Initializes the LCD in 4-bit or 8-bit mode depending on the configuration.
 *    - Configures GPIO pins connected to the LCD for data transfer and control.
 *
 * 2. Sending Commands and Characters:
 *    - `LCD_SendCommand()`: Sends control commands like clearing the screen or changing cursor position.
 *    - `LCD_SendCharacter()`: Displays a single character on the screen.
 *
 * 3. Displaying Strings:
 *    - `LCD_SendString()`: Sends a string to the LCD, displaying characters one by one.
 *
 * 4. Cursor Management:
 *    - `LCD_MoveCursor()`: Moves the cursor to a specific row and column on the LCD.
 *    - `LCD_SendStringAtRowColumn()`: Displays a string starting at a specific row and column.
 *    - `LCD_SendCharacterAtRowColumn()`: Displays a character at a specific row and column.
 *
 * 5. Displaying Numbers:
 *    - `LCD_intgerToString()`: Converts an integer to a string and displays it on the LCD.
 *
 * 6. Clearing the Screen:
 *    - `LCD_ClearScreen()`: Sends the command to clear the LCD display.
 *
 * How to Use:
 * 1. Initialize the LCD using `LCD_init()`.
 * 2. Send commands to control the LCD or send characters and strings to display on the screen.
 * 3. Optionally, move the cursor to specific positions and display characters or strings at those positions.
 *
 * note: The driver assumes that the LCD data and control pins are predefined in `LCD.h`. This driver supports both 4-bit and 8-bit modes depending on the configuration.
 *
 */


#include "LCD.h"
#include"gpio.h"
#include"std_types.h"
#include <util/delay.h>
#include "common_macros.h"
#include <stdlib.h>

/*LCD initialization*/
void LCD_init(void)
{
	/*Setting the direction of the main pins as OUTPUT*/
	GPIO_setupPinDirection(LCD_RS_PORT, LCD_RS_PIN, 		PIN_OUTPUT);
	GPIO_setupPinDirection(LCD_Enable_PORT, LCD_Enable_PIN, PIN_OUTPUT);

	/* LCD Power ON delay (always > 15ms) */
	_delay_ms(15);
#if(LDC_MODE == 8)
	GPIO_setupPortDirection(LCD_Command_Data_PORT, PORT_OUTPUT);
#else

	GPIO_setupPinDirection(LCD_Command_Data_PORT, LCD_Command_Data_FIRST_PIN, 	PIN_OUTPUT);
	GPIO_setupPinDirection(LCD_Command_Data_PORT, LCD_Command_Data_SECOND_PIN, 	PIN_OUTPUT);
	GPIO_setupPinDirection(LCD_Command_Data_PORT, LCD_Command_Data_THIRD_PIN, 	PIN_OUTPUT);
	GPIO_setupPinDirection(LCD_Command_Data_PORT, LCD_Command_Data_FOURTH_PIN,  PIN_OUTPUT);
#endif


	/*Setting LCD modes & initial Setup*/
#if(LDC_MODE == 8)
	LCD_SendCommand(LCD_TWO_LINES_EIGHT_BITS_MODE);
#else
	/* Send for 4 bit initialization of LCD  */
	LCD_SendCommand(LCD_TWO_LINES_FOUR_BITS_MODE_INIT1);
	LCD_SendCommand(LCD_TWO_LINES_FOUR_BITS_MODE_INIT2);
	LCD_SendCommand(LCD_TWO_LINES_FOUR_BITS_MODE);
#endif
	LCD_SendCommand(LCD_CURSOR_OFF);
	LCD_SendCommand(LCD_CLEAR_COMMAND);
}

/*Sending required commands*/
void LCD_SendCommand(uint8 command)
{
#if(LDC_MODE == 8)
	/*following this scenario from the Data Sheet Timing Diagram*/
	GPIO_writePin(LCD_RS_PORT, LCD_RS_PIN, LOGIC_LOW);
	_delay_ms(1);
	GPIO_writePin(LCD_Enable_PORT, LCD_Enable_PIN, LOGIC_HIGH);
	_delay_ms(1);
	GPIO_writePort(LCD_Command_Data_PORT, command);
	_delay_ms(1);
	GPIO_writePin(LCD_Enable_PORT, LCD_Enable_PIN, LOGIC_LOW);
	_delay_ms(1);
#else
	GPIO_writePin(LCD_RS_PORT, LCD_RS_PIN, LOGIC_LOW);

	_delay_ms(1);
	GPIO_writePin(LCD_Enable_PORT, LCD_Enable_PIN, LOGIC_HIGH);
	_delay_ms(1);
	GPIO_writePin(LCD_Command_Data_PORT, LCD_Command_Data_FIRST_PIN, 	GET_VAR_BIT(command,4));
	GPIO_writePin(LCD_Command_Data_PORT, LCD_Command_Data_SECOND_PIN, 	GET_VAR_BIT(command,5));
	GPIO_writePin(LCD_Command_Data_PORT, LCD_Command_Data_THIRD_PIN, 	GET_VAR_BIT(command,6));
	GPIO_writePin(LCD_Command_Data_PORT, LCD_Command_Data_FOURTH_PIN,   GET_VAR_BIT(command,7));
	_delay_ms(1);
	GPIO_writePin(LCD_Enable_PORT, LCD_Enable_PIN, LOGIC_LOW);
	_delay_ms(1);


	GPIO_writePin(LCD_Enable_PORT, LCD_Enable_PIN, LOGIC_HIGH);
	_delay_ms(1);
	GPIO_writePin(LCD_Command_Data_PORT, LCD_Command_Data_FIRST_PIN, 	GET_VAR_BIT(command,0));
	GPIO_writePin(LCD_Command_Data_PORT, LCD_Command_Data_SECOND_PIN, 	GET_VAR_BIT(command,1));
	GPIO_writePin(LCD_Command_Data_PORT, LCD_Command_Data_THIRD_PIN, 	GET_VAR_BIT(command,2));
	GPIO_writePin(LCD_Command_Data_PORT, LCD_Command_Data_FOURTH_PIN,   GET_VAR_BIT(command,3));
	_delay_ms(1);
	GPIO_writePin(LCD_Enable_PORT, LCD_Enable_PIN, LOGIC_LOW);
	_delay_ms(1);
#endif
}

/*Sending a specific character*/
void LCD_SendCharacter(uint8 character)
{
#if(LDC_MODE == 8)
	/*following this scenario from the Data Sheet Timing Diagram*/
	GPIO_writePin(LCD_RS_PORT, LCD_RS_PIN, LOGIC_HIGH);
	_delay_ms(1);
	GPIO_writePin(LCD_Enable_PORT, LCD_Enable_PIN, LOGIC_HIGH);
	_delay_ms(1);
	GPIO_writePort(LCD_Command_Data_PORT, character);
	_delay_ms(1);
	GPIO_writePin(LCD_Enable_PORT, LCD_Enable_PIN, LOGIC_LOW);
	_delay_ms(1);
#else
	GPIO_writePin(LCD_RS_PORT, LCD_RS_PIN, LOGIC_HIGH);

	_delay_ms(1);
	GPIO_writePin(LCD_Enable_PORT, LCD_Enable_PIN, LOGIC_HIGH);
	_delay_ms(1);
	GPIO_writePin(LCD_Command_Data_PORT, LCD_Command_Data_FIRST_PIN, 	GET_VAR_BIT(character,4));
	GPIO_writePin(LCD_Command_Data_PORT, LCD_Command_Data_SECOND_PIN, 	GET_VAR_BIT(character,5));
	GPIO_writePin(LCD_Command_Data_PORT, LCD_Command_Data_THIRD_PIN, 	GET_VAR_BIT(character,6));
	GPIO_writePin(LCD_Command_Data_PORT, LCD_Command_Data_FOURTH_PIN,   GET_VAR_BIT(character,7));
	_delay_ms(1);
	GPIO_writePin(LCD_Enable_PORT, LCD_Enable_PIN, LOGIC_LOW);
	_delay_ms(1);


	GPIO_writePin(LCD_Enable_PORT, LCD_Enable_PIN, LOGIC_HIGH);
	_delay_ms(1);
	GPIO_writePin(LCD_Command_Data_PORT, LCD_Command_Data_FIRST_PIN, 	GET_VAR_BIT(character,0));
	GPIO_writePin(LCD_Command_Data_PORT, LCD_Command_Data_SECOND_PIN, 	GET_VAR_BIT(character,1));
	GPIO_writePin(LCD_Command_Data_PORT, LCD_Command_Data_THIRD_PIN, 	GET_VAR_BIT(character,2));
	GPIO_writePin(LCD_Command_Data_PORT, LCD_Command_Data_FOURTH_PIN,   GET_VAR_BIT(character,3));
	_delay_ms(1);
	GPIO_writePin(LCD_Enable_PORT, LCD_Enable_PIN, LOGIC_LOW);
	_delay_ms(1);
#endif
}

/*Sending a specific string*/
void LCD_SendString(uint8 *strPtr)
{
	/*Looping on the string elements and sending them One By One*/
	uint8 var = 0;
	while (strPtr[var] != '\0') {
		LCD_SendCharacter(strPtr[var]);	/*Sending each character*/
		var++;
	}
}

/*Setting the cursor (displaying at row & column) */
void LCD_MoveCursor(uint8 row, uint8 col)
{
	/* Calculate the required address in the LCD DDRAM */
	uint8 LCD_Cursor_Address;
	switch (row) {
		case 0:
			LCD_Cursor_Address = col + LCD_First_Row_address;
			break;
		case 1:
			LCD_Cursor_Address = col + LCD_Second_Row_address;
			break;
		case 2:
			LCD_Cursor_Address = col + LCD_Third_Row_address;
			break;
		case 3:
			LCD_Cursor_Address = col + LCD_Fourth_Row_address;
			break;
	}

	/* Move the LCD cursor to this specific address */
	LCD_SendCommand(LCD_Cursor_Address | LCD_SET_CURSOR_LOCATION);
}

/*Display the required string in a specific row and column index on the screen*/
void LCD_SendStringAtRowColumn(uint8 row,uint8 col,const char *Str)
{
	/*First we set the cursor, Then we display the string*/
	LCD_MoveCursor(row, col);
	LCD_SendString(Str);
}

/*display any number as string*/
void LCD_intgerToString(uint8 data)
{
	uint8 ASCII_Str[16];			/*an array to hold that string*/
	itoa(data, ASCII_Str, 10);		/*A function that turns each digit in a number into its ASCII*/
	LCD_SendString(ASCII_Str);
}

/*Display the required character in a specific row and column index on the screen*/
void LCD_SendCharacterAtRowColumn(uint8 row,uint8 col, uint8 character)
{
	/*First we set the cursor, Then we display the string*/
		LCD_MoveCursor(row, col);
		LCD_SendCharacter(character);
}

/*Send the clear screen command*/
void LCD_ClearScreen(void)
{
	LCD_SendCommand(LCD_CLEAR_COMMAND);
}
