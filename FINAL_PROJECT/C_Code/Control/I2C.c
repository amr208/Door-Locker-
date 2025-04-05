/*
 * I2C.c
 *
 *  Created on: Oct 19, 2024
 *      Author: amr mohamed
 */

#include "I2C.h"
#include "common_macros.h"
#include <avr/io.h>
void I2C_init(I2C_Config * I2CPtr)
{
	if (I2CPtr -> CPU_frequency == CPU_16MHZ) {
		if (I2CPtr -> I2C_clockSpeed == I2C_100KHZ)
		{
			TWBR = 0x48;
			TWSR = 0x00;
		}
		else if (I2CPtr -> I2C_clockSpeed == I2C_400KHZ)
		{
			TWBR = 0x0C;
			TWSR = 0x00;
		}
	}

	if (I2CPtr -> CPU_frequency == CPU_8MHZ) {
		if (I2CPtr -> I2C_clockSpeed == I2C_100KHZ)
		{
			TWBR = 0x20;
			TWSR = 0x00;
		}
		else if (I2CPtr -> I2C_clockSpeed == I2C_400KHZ)
		{
			TWBR = 0x02;
			TWSR = 0x00;
		}
	}

	if (I2CPtr -> CPU_frequency == CPU_1MHZ) {
		if (I2CPtr -> I2C_clockSpeed == I2C_100KHZ)
		{
			TWBR = 0x03;
			TWSR = 0x01;
		}
		else if (I2CPtr -> I2C_clockSpeed == I2C_400KHZ)
		{
			TWBR = 0x03;
			TWSR = 0x01;
		}
	}


    /* Two Wire Bus address my address if any master device want to call me
       General Call Recognition: Off */
	TWAR = (I2CPtr -> deviceAddress >> 1);

    TWCR = (1<<TWEN); /* enable TWI */
}

void I2C_start(void)
{
    /*
	 * Clear the TWINT flag before sending the start bit TWINT=1
	 * send the start bit by TWSTA=1
	 * Enable TWI Module TWEN=1
	 */
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

    /* Wait for TWINT flag set in TWCR Register (start bit is send successfully) */
    while(BIT_IS_CLEAR(TWCR,TWINT));
}

void I2C_stop(void)
{
    /*
	 * Clear the TWINT flag before sending the stop bit TWINT=1
	 * send the stop bit by TWSTO=1
	 * Enable TWI Module TWEN=1
	 */
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

void I2C_writeByte(uint8_t data)
{
    /* Put data On TWI data Register */
    TWDR = data;
    /*
	 * Clear the TWINT flag before sending the data TWINT=1
	 * Enable TWI Module TWEN=1
	 */
    TWCR = (1 << TWINT) | (1 << TWEN);
    /* Wait for TWINT flag set in TWCR Register(data is send successfully) */
    while(BIT_IS_CLEAR(TWCR,TWINT));
}

uint8_t I2C_readByteWithACK(void)
{
	/*
	 * Clear the TWINT flag before reading the data TWINT=1
	 * Enable sending ACK after reading or receiving data TWEA=1
	 * Enable TWI Module TWEN=1
	 */
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    /* Wait for TWINT flag set in TWCR Register (data received successfully) */
    while(BIT_IS_CLEAR(TWCR,TWINT));
    /* Read Data */
    return TWDR;
}

uint8_t I2C_readByteWithNACK(void)
{
	/*
	 * Clear the TWINT flag before reading the data TWINT=1
	 * Enable TWI Module TWEN=1
	 */
    TWCR = (1 << TWINT) | (1 << TWEN);
    /* Wait for TWINT flag set in TWCR Register (data received successfully) */
    while(BIT_IS_CLEAR(TWCR,TWINT));
    /* Read Data */
    return TWDR;
}

uint8_t I2C_getStatus(void)
{
    uint8 status;
    /* masking to eliminate first 3 bits and get the last 5 bits (status bits) */
    status = TWSR & 0xF8;
    return status;
}

