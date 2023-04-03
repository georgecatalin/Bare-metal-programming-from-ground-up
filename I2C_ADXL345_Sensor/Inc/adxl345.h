/*
 * adxl345.h
 *
 *  Created on: 3 Apr 2023
 *  Author: George Calin
 * 	Target Development Board: STM32 Nucleo F429ZI
 */

#ifndef ADXL345_H_
#define ADXL345_H_

#include <i2c.h>
#include <stdint.h>

#define   DEVID_R					(0x00)
#define   DEVICE_ADDR    			(0x53)
#define   DATA_FORMAT_R   			(0x31)
#define   POWER_CTL_R 				(0x2D)
#define   DATA_START_ADDR			(0x32)
#define   DATA_FORMAT_R   			(0x31)


#define		FOUR_G					(0x01)
#define		RESET					(0x00)
#define     SET_MEASURE_B		    (0x08)


void adxl_init (void);
void adxl_read_values (uint8_t reg);

#endif /* ADXL345_H_ */
