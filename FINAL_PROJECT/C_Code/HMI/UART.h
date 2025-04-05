/*
 * UART.h
 *
 *  Created on: Oct 11, 2024
 *      Author: amr mohamed
 */

#ifndef UART_H_
#define UART_H_
/***********************************************/
#include "std_types.h"
/***********************************************/
/* UART Register Flags */
#define UART_RECEIVE_COMPLETE RXC          /* Receive complete flag */
#define UART_TRANSMIT_COMPLETE TXC          /* Transmit complete flag */
#define UART_DATA_REGISTER_EMPTY UDRE       /* Data register empty flag */
#define UART_FRAME_ERROR FE                 /* Frame error flag */
#define UART_PARITY_ERROR PE                /* Parity error flag */
#define UART_DOUBLE_SPEED U2X               /* Double speed mode flag */
/***********************************************/
/* UART Interrupt Enables */
#define UART_RX_INTERRUPT_EN RXCIE          /* Receive complete interrupt enable */
#define UART_TX_INTERRUPT_EN TXCIE          /* Transmit complete interrupt enable */
#define UART_DATA_EMPTY_INTERRUPT_EN UDRIE  /* Data register empty interrupt enable */
/***********************************************/
/* UART Enable */
#define UART_RECEIVER_ENABLE RXEN           /* Enable UART receiver */
#define UART_TRANSMITTER_ENABLE TXEN         /* Enable UART transmitter */
/***********************************************/
/* UART Register and Mode Select */
#define UART_UCSRC_or_UBRRH URSEL           /* Select UCSRC or UBRRH register */
#define UART_MODE_SELECT UMSEL              /* Mode select (Asynchronous/Synchronous) */
/***********************************************/
/* UART Parity Mode Enumeration */
typedef enum
{
    DISABLED,             /* No parity */
    EVEN_PARITY = 2,      /* Even parity */
    ODD_PARITY            /* Odd parity */
} UART_PARITY_MODE;

/***********************************************/
/* UART Data Bits Size Enumeration */
typedef enum
{
    FIVE_BITS,            /* 5-bit data */
    SIX_BITS,             /* 6-bit data */
    SEVEN_BITS,           /* 7-bit data */
    EIGHT_BITS            /* 8-bit data */
} UART_DATA_BITS_SIZE;

/***********************************************/
/* UART Stop Bit Type Enumeration */
typedef enum
{
    ONE_BIT,              /* 1 stop bit */
    TWO_BITS              /* 2 stop bits */
} UART_STOP_BIT_TYPE;

/***********************************************/
/* UART Baud Rate Definition */
typedef uint16 UART_BAUD_RATE;              /* UART Baud Rate as a 16-bit unsigned integer */

/***********************************************/
/* Structure to configure the UART */
typedef struct {
    UART_BAUD_RATE baudRate;                /* UART baud rate */
    UART_PARITY_MODE parityType;            /* UART parity type (Disabled, Even, Odd) */
    UART_DATA_BITS_SIZE characterSize;      /* UART character size (data bits) */
    UART_STOP_BIT_TYPE stopSelect;          /* UART stop bit selection (1 or 2 bits) */
} UART_Config;
/***********************************************/

/***********************************************/

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_Init(UART_Config *UART_configPtr);

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data);

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_recieveByte(void);

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str);

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str); // Receive until #

#endif /* UART_H_ */
