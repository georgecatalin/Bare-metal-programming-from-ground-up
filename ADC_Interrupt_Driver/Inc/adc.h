/*
 * adc.h
 *
 *  Created on: 28 Mar 2023
 *  Author: George Calin
 * 	Target Development Board: STM32 Nucleo F429ZI
 */

#ifndef ADC_H_
#define ADC_H_

#include <stm32f429xx.h>
#include <stdint.h>

void pa1_adc_init(void);
void pa1_adc_interrupt_init(void);

void start_conversion(void);
uint32_t adc_read(void);

#define SR_EOC (1UL<<1)

#endif /* ADC_H_ */
