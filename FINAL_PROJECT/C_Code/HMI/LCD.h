/*
 * LCD.h
 *
 *  Created on: Sep 27, 2024
 *      Author: amr mohamed
 */

#ifndef LCD_H_
#define LCD_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
/*Pin Configuration*/
/*Control pins.*/
#define LCD_RS_PORT									PORTC_ID
#define LCD_RS_PIN 									PIN0_ID 	/*Register select (HIGH = send char/LOW = send command)*/
#define LCD_Enable_PORT								PORTC_ID
#define LCD_Enable_PIN 								PIN1_ID
#define LCD_Command_Data_PORT 						PORTA_ID	/*The value to be presented on the LCD*/
#define LCD_Command_Data_FIRST_PIN					PIN3_ID
#define LCD_Command_Data_SECOND_PIN					PIN4_ID
#define LCD_Command_Data_THIRD_PIN					PIN5_ID
#define LCD_Command_Data_FOURTH_PIN					PIN6_ID
#define LCD_NUM_OF_PINS								3

/*modes and commands config.*/
#define LDC_MODE									8
#define LCD_TWO_LINES_EIGHT_BITS_MODE 				0x38
#define LCD_TWO_LINES_FOUR_BITS_MODE_INIT1  		0x33
#define LCD_TWO_LINES_FOUR_BITS_MODE_INIT2   		0x32
#define LCD_TWO_LINES_FOUR_BITS_MODE 				0x28
#define LCD_CURSOR_OFF								0x0C
#define LCD_CLEAR_COMMAND							0x01
#define LCD_SET_CURSOR_LOCATION        				0x80
#define LCD_First_Row_address						0x00
#define LCD_Second_Row_address						0x40
#define LCD_Third_Row_address						0x10
#define LCD_Fourth_Row_address						0x50
/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/

/*LCD initialization*/
void LCD_init(void);

/*LCD Commands display*/
void LCD_SendCommand(uint8 command);

/*LCD Character display*/
void LCD_SendCharacter(uint8 character);

/*Displaying a specified located CHARACTER (On a specific row and column)*/
void LCD_SendCharacterAtRowColumn(uint8 row,uint8 col, uint8 character);

/*Displaying a normal Located string (Always displayed at row 0 and column 0)*/
void LCD_SendString(uint8 *strPtr);

/*Setting the cursor to a specific location*/
void LCD_MoveCursor(uint8 row, uint8 col);

/*Displaying a specified located string (On a specific row and column)*/
void LCD_SendStringAtRowColumn(uint8 row,uint8 col,const char *Str);

/*Converting an integer to string*/
void LCD_intgerToString(uint8 data);

/*Removing what is displayed on the screen*/
void LCD_ClearScreen(void);

#endif /* LCD_H_ */
