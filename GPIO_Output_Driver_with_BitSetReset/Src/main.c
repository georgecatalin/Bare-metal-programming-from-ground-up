/**
 ******************************************************************************
 * @file           : main.c
 * @author         : George Calin
 * @brief          : Main program body
 ******************************************************************************
 */

#include <stm32f429xx.h>
#include <stdint.h>

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

#define GPIOGEN (1UL<<6)
#define GPIOAEN (1UL<<0)

#define PIN13	(1UL<<13)
#define PIN0	(1<<0)


int main(void)
{
	/* *****************************************************************************
	 * Enable GPIO G and GPIO A clock access
	 * ***************************************************************************** */
	RCC->AHB1ENR |= GPIOGEN;
	RCC->AHB1ENR |= GPIOAEN;

	/* *******************************************************************************
	 * Set the mode of Port G Pin 13 to Output; Set the mode of Port A to Input
	 * ******************************************************************************* */
	GPIOG->MODER &= ~(1UL<<27); // '0'
	GPIOG->MODER |= (1UL<<26); // '1'

	GPIOA->MODER &= ~(1UL<<0); // '0'
	GPIOA->MODER &= ~(1UL<<1); // '0'


	while(1)
	{
		/* **********************************************************************************************
		 * Verify the status of the user button in the GPIOx_IDR registry
		 * ********************************************************************************************** */
		if(GPIOA->IDR & PIN0)
		{
			GPIOG->BSRR = PIN13; //switch on USER LED
		}
		else
		{
			GPIOG->BSRR = (1UL<<29); //switch off USER LED
		}
	}
}
