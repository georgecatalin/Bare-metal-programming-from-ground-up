/*
 * systick.h
 *
 *  Created on: 29 Mar 2023
 *  Author: George Calin
 * 	Target Development Board: STM32 Nucleo F429ZI
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_

#include <stm32f429xx.h>

void systickDelayMilliseconds(int milliseconds);
void systick_1Hz_interrupt(void);

#endif /* SYSTICK_H_ */
