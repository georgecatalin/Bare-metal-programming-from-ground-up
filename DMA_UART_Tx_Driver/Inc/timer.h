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
void tim3_input_capture(void); // when reading the alternate function mapping on the datasheet, given to the fact the we have a timer TIM4_CH2 and we wish to watch it with another timer, then we are going use PA7

#endif /* TIMER_H_ */
