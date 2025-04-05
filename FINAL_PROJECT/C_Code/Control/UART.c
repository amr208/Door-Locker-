/*
 * UART.c
 *
 *  Created on: Oct 11, 2024
 *      Author: amr mohamed
 */

/**
 * UART Driver for AVR Microcontrollers
 *
 * This file contains the implementation of the UART (Universal Asynchronous Receiver/Transmitter) driver.
 * UART is a serial communication protocol that enables data exchange between devices such as microcontrollers and peripheral devices.
 * This driver supports data transmission and reception, with options for configurable baud rate, data size, parity, and stop bits.
 *
 * Features:
 * 1. UART Initialization:
 *    - `UART_Init()`: Initializes the UART module with the specified settings, including baud rate, parity, stop bits, and data size.
 *
 * 2. Sending and Receiving Data:
 *    - `UART_sendByte()`: Sends a single byte via UART to another device.
 *    - `UART_recieveByte()`: Receives a single byte from another UART device.
 *
 * 3. Sending and Receiving Strings:
 *    - `UART_sendString()`: Sends a string of characters via UART.
 *    - `UART_receiveString()`: Receives a string of characters via UART, terminated by a specific delimiter.
 *
 * Configuration:
 * The UART driver can be configured using the following parameters:
 * - Baud rate: Determines the speed of data transmission.
 * - Parity: Configures the type of parity (None, Even, or Odd) used in communication.
 * - Stop bit selection: Choose between 1 or 2 stop bits.
 * - Data size: Configures the number of bits in each data frame (5, 6, 7, 8 bits).
 *
 * How to Use:
 * 1. Define a `UART_Config` structure with the desired settings.
 * 2. Call `UART_Init()` to initialize the UART interface with the provided configuration.
 * 3. Use `UART_sendByte()` or `UART_sendString()` to transmit data, and `UART_recieveByte()` or `UART_receiveString()` to receive data.
 *
 * example:
 * Usage of the UART driver:
 * UART_Config uartConfig = {9600, PARITY_NONE, ONE_STOP_BIT, EIGHT_BITS};
 * UART_Init(&uartConfig);
 * UART_sendByte('A');  // Transmit the character 'A'
 * uint8 receivedChar = UART_recieveByte();  // Receive a character
 *
 */


#include "UART.h"
#include "std_types.h"
#include <avr/io.h>
#include "common_macros.h"


void UART_Init(UART_Config *UART_configPtr)
{
	uint16 ubrr_value = 0;

	/* U2X = 1 for double transmission speed */
	UCSRA |= (1 << U2X);




	UCSRC = (1 << URSEL); /*URSEL = 1 The URSEL must be one when writing the UCSRC*/
	UCSRC |= (UART_configPtr->parityType << 4); /* Set UPM1:0 (bit 5:4) for parity mode */
	UCSRC |= (UART_configPtr->stopSelect << 3); /* Set USBS (bit 3) for stop bit selection */
	UCSRC |= (UART_configPtr->characterSize << 1); /* Set UCSZ1:0 (bit 2:1) for character size */




	/* Calculate the UBRR register value */
	ubrr_value = (uint16) (((F_CPU / (UART_configPtr->baudRate * 8UL))) - 1);

	/* First 8 bits from the BAUD_PRESCALE inside UBRRL and last 4 bits in UBRRH*/
	UBRRH = ubrr_value >> 8;
	UBRRL = ubrr_value;
	/************************** UCSRB Description **************************
	 * RXCIE = 0 Disable USART RX Complete Interrupt Enable
	 * TXCIE = 0 Disable USART Tx Complete Interrupt Enable
	 * UDRIE = 0 Disable USART Data Register Empty Interrupt Enable
	 * RXEN  = 1 Receiver Enable
	 * RXEN  = 1 Transmitter Enable
	 * UCSZ2 = 0 For 8-bit data mode
	 * RXB8 & TXB8 not used for 8-bit data mode
	 ***********************************************************************/
	UCSRB = (1 << RXEN) | (1 << TXEN);
}

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data)
{
	/**
	 * Wait until the UDRE flag (UART_DATA_REGISTER_EMPTY) is set in the UCSRA register.
	 * UDRE flag indicates that the transmit buffer (UDR) is empty
	 * and ready for the next byte to be transmitted.
	 * The loop continues until UDRE becomes '1', signaling that
	 * data can be written to the UDR register.
	 */
	while(BIT_IS_CLEAR(UCSRA, UART_DATA_REGISTER_EMPTY))   /* Wait until UDRE flag is set (buffer is empty) */
	{
	    /* wait for the flag to be set */
	}


	/*
	 * Put the required data in the UDR register and it also clear the UDRE flag as
	 * the UDR register is not empty now
	 */
	UDR = data;

	/************************* Another Method *************************
	UDR = data;
	while(BIT_IS_CLEAR(UCSRA,TXC)){} // Wait until the transmission is complete TXC = 1
	SET_BIT(UCSRA,TXC); // Clear the TXC flag
	*******************************************************************/
}

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_recieveByte(void)
{
	/**
	 * Wait until the RXC flag (UART_RECEIVE_COMPLETE) is set in the UCSRA register.
	 * The RXC flag indicates that there is data available in the
	 * UART receive buffer (UDR).
	 * The loop continues until RXC becomes '1', signaling that
	 * a byte has been received and can be read from UDR.
	 */
	while(BIT_IS_CLEAR(UCSRA, UART_RECEIVE_COMPLETE))   /* Wait until RXC flag is set (data received) */
	{
	    /* wait for the flag to be set */
	}


	/*
	 * Read the received data from the Rx buffer (UDR)
	 * The RXC flag will be cleared after read the data
	 */
    return UDR;
}

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str)
{
	uint8 i = 0;

	/*Send the entire string byte by byte*/
	while(Str[i] != '\0')
	{
		UART_sendByte(Str[i]);
		i++;
	}
}

/**
 * Description: Receiving a string through UART until the '#' symbol is encountered.
 *
 * This function continuously receives characters from the UART interface and stores them
 * in the provided string buffer until the '#' character is received. The '#' character acts
 * as a delimiter, indicating the end of the string. Once the '#' character is received, it is
 * replaced with a null terminator '\0' to properly terminate the string.
 *
 * Str Pointer to the buffer where the received string will be stored.
 */
void UART_receiveString(uint8 *Str)
{
    uint8 i = 0;

    /* Receive the first byte and store it in the buffer */
    Str[i] = UART_recieveByte();

    /* Continue receiving bytes until the '#' character is encountered */
    while(Str[i] != '#')
    {
        i++;
        Str[i] = UART_recieveByte();
    }

    /* Replace the '#' character with a null terminator to end the string */
    Str[i] = '\0';
}


