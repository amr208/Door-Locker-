/*
 * I2C.h
 *
 *  Created on: Oct 19, 2024
 *      Author: amr mohamed
 */

#ifndef I2C_H_
#define I2C_H_

#include "std_types.h"

typedef enum {
    I2C_100KHZ,
	I2C_400KHZ
} I2C_clockSpeed;

typedef enum {
	CPU_16MHZ,
	CPU_8MHZ,
	CPU_1MHZ
}CPUSpeed;
/* Structure to hold I2C configuration*/
typedef struct {
	CPUSpeed CPU_frequency;
	I2C_clockSpeed I2C_clockSpeed;
    uint8 deviceAddress; // I2C device address (My address)
} I2C_Config;


/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/

/* I2C Status Bits in the TWSR Register */
#define I2C_START         0x08 /* start has been sent */
#define I2C_REP_START     0x10 /* repeated start */
#define I2C_MT_SLA_W_ACK  0x18 /* Master transmit ( slave address + Write request ) to slave + ACK received from slave. */
#define I2C_MT_SLA_R_ACK  0x40 /* Master transmit ( slave address + Read request ) to slave + ACK received from slave. */
#define I2C_MT_DATA_ACK   0x28 /* Master transmit data and ACK has been received from Slave. */
#define I2C_MR_DATA_ACK   0x50 /* Master received data and send ACK to slave. */
#define I2C_MR_DATA_NACK  0x58 /* Master received data but doesn't send ACK to slave. */

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/**
 * Initialize the I2C with the provided configuration.
 *
 * This function initializes the I2C hardware with the settings provided
 * in the `I2C_Config` structure. The function sets up the clock speed,
 * device address, and any other necessary parameters for communication.
 *
 * I2CPtr A pointer to the I2C_Config structure that contains the configuration settings.
 */
void I2C_init(I2C_Config *I2CPtr);

/**
 * Generate a START condition on the I2C bus.
 *
 * This function generates a START condition to begin communication with
 * a slave device on the I2C bus. The START condition is essential for
 * initializing any I2C transaction.
 */
void I2C_start(void);

/**
 * Generate a STOP condition on the I2C bus.
 *
 * This function generates a STOP condition to terminate communication with
 * a slave device on the I2C bus. It is used at the end of the I2C transaction
 * to release the bus.
 */
void I2C_stop(void);

/**
 * Write a byte of data to the I2C bus.
 *
 * This function writes a single byte of data to the I2C bus. It is typically
 * used after sending a START condition and addressing a slave device. The byte
 * is written to the data register and transmitted to the slave device.
 */
void I2C_writeByte(uint8 data);

/**
 * Read a byte of data from the I2C bus with an ACK.
 *
 * This function reads a byte of data from the I2C bus and sends an ACK (acknowledgment)
 * signal back to the slave device. This is useful when reading multiple bytes
 * from the device, as it indicates the master is ready to continue reading.
 *
 * return The byte of data read from the I2C bus.
 */
uint8 I2C_readByteWithACK(void);

/**
 * Read a byte of data from the I2C bus with a NACK.
 *
 * This function reads a byte of data from the I2C bus and sends a NACK (no acknowledgment)
 * signal back to the slave device. This is typically used when reading the last byte
 * of data from the slave, indicating that the master has finished the read operation.
 *
 * return The byte of data read from the I2C bus.
 */
uint8 I2C_readByteWithNACK(void);

/**
 * Get the current status of the I2C bus.
 *
 * This function returns the status of the I2C bus, indicating the current
 * state of the I2C communication (e.g., whether a START condition has been
 * sent, if data has been transmitted successfully, or if there's an error).
 *
 * The status register can be used to check whether operations have completed
 * successfully or if an error occurred.
 *
 * return The current status of the I2C bus (status code).
 */
uint8 I2C_getStatus(void);

#endif /* I2C_H_ */
