/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <stdint.h>

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

int main(void)
{
	uint32_t *regClockAddress = (uint32_t *) 0x40023830;
	uint32_t *gpioBModeRegAddress = (uint32_t *) 0x40020400;
	uint32_t *gpioBPortOutputRegAddress = (uint32_t *) 0x40020414;


	//*regClockAddress |= 0x02;
	*regClockAddress |= (1<<1);

	/*
	*gpioBModeRegAddress &= 0xFFFF3FFF;
	*gpioBModeRegAddress |= 0X4000;
	*/
	*gpioBModeRegAddress &= ~ (1<<14); //LD2
	*gpioBModeRegAddress &= ~ (1<<15); //LD2

	*gpioBModeRegAddress &= ~ (1<<28); //LD3
	*gpioBModeRegAddress &= ~ (1<<29); //LD3

	/*
	*gpioBModeRegAddress |= 0X4000;
	*/
	*gpioBModeRegAddress |= (1<<14); //LD2

	*gpioBModeRegAddress |= (1<<28); //LD3


	/*
	*gpioBPortOutputRegAddress |=0x80;
	*/
	*gpioBPortOutputRegAddress |= (1<<7); //LD2
	*gpioBPortOutputRegAddress |= (1<<14); //LD3


    /* Loop forever */
	for(;;);
}
