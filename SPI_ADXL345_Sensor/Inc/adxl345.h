/*
 * adxl.h
 *
 *  Created on: 3 Apr 2023
 *  Author: George Calin
 * 	Target Development Board: STM32 Nucleo F429ZI
 */

#ifndef ADXL_H_
#define ADXL_H_


#include "spi.h"
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
void adxl_read(uint8_t address, uint8_t * rxdata);


#endif /* ADXL_H_ */
