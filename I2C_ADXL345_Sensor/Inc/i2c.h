/*
 * i2c.h
 *
 *  Created on: 30 Mar 2023
 *  Author: George Calin
 * 	Target Development Board: STM32 Nucleo F429ZI
 */

#ifndef I2C_H_
#define I2C_H_

#include "stm32f429xx.h"

void i2c1_init(void);
void i2c1_ByteRead(char slave_address, char memory_address, char *ptrToVariableWhereWeSaveData);
void i2c1_BurstRead(char slave_address, char memory_address_read_from, int number_bytes_to_read, char *data);

#endif /* I2C_H_ */
