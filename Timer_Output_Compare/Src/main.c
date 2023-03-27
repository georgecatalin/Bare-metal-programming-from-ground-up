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


#include "timer.h"



int main(void)
{
	tim4_output_compare();

	for(;;)
	{
	}
}


