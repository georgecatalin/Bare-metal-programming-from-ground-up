/*
 * spi.h
 *
 *  Created on: 31 Mar 2023
 *  Author: George Calin
 * 	Target Development Board: STM32 Nucleo F429ZI
 */

#ifndef SPI_H_
#define SPI_H_

#include "stm32f429xx.h"
#include <stdint.h>

void spi1_gpio_init(void);
void spi1_config(void);
void spi1_transmit(uint8_t *data, uint32_t size);
void spi1_receive(uint8_t *data, uint32_t size);
void cs_enable(void);
void cs_disable(void);

#endif /* SPI_H_ */
