/*
 * timer.h
 *
 *  Created on: 22 Mar 2023
 *  Author: George Calin
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <stm32f429xx.h>

void tim4_output_compare(void); // when reading the alternate function mapping on the datasheet we observe that TIM4_CH2 is connected to PB7 where the user LED connects

#endif /* TIMER_H_ */
