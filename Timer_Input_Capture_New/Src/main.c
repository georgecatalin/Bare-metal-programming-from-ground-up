/**
 ******************************************************************************
 * @file           : main.c
 * @author         : George Calin
 * @brief          : Main program body
 * Target board: Nucleo 144 family
 *
 * Notes: User LD3: a red user LED is connected to PB14 on the Nucleo 144 family boards
 * User LD2: a blue user LED is connected to PB7.
 ******************************************************************************
 */

#include <stdio.h>
#include "uart.h"
#include "timer.h"

#define TIM2_SR_UIF (1UL<<0)  // Bit 0 UIF: Update interrupt flag

#define TIM3_SR_CC2IF	(1UL<<2)

int time_stamp=0;

/* Set up : Connect a jumper from PB7 to PA7 */
int main(void)
{
	tim4_output_compare();
	tim3_input_capture();

	for(;;)
	{
		//Wait until the edge is captured
		while(!(TIM3->SR & TIM3_SR_CC2IF));

		/* Read value into variable */
		time_stamp = TIM3->CCR1;

	}

}




