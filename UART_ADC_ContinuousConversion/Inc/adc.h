/*
 * adc.h
 *
 *  Created on: 16 Mar 2023
 *  Author: George Calin
 */

#ifndef ADC_H_
#define ADC_H_

#include <stm32f429xx.h>
#include <stdint.h>

void pa1_adc_init(void);
void start_conversion(void);
uint32_t adc_read(void);


#endif /* ADC_H_ */
