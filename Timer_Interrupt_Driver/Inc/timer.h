/*
 * timer.h
 *
 *  Created on: 29 Mar 2023
 *  Author: George Calin
 * 	Target Development Board: STM32 Nucleo F429ZI
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <stm32f429xx.h>

void tim2_everysecond_init(void);
void tim2_everysecond_interrupt(void);

#endif /* TIMER_H_ */
