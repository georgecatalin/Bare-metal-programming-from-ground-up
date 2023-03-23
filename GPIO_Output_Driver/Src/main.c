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

#define PIN13	(1UL<<13)
#define ODR13	PIN13

int main(void)
{
	RCC->AHB1ENR |= GPIOGEN;

	GPIOG->MODER &= ~(1<<27);
	GPIOG->MODER |= (1<<26);

	while(1)
	{
		GPIOG->BSRR |= PIN13; //switch on USER LED
		for(int i=0;i<800000;++i);

		GPIOG->BSRR |= (1<<29); //switch off USER LED
		for(int i=0;i<800000;++i);
	}

}
