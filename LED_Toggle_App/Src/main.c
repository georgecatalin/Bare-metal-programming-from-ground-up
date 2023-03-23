/**
 ******************************************************************************
 * @file           : main.c
 * @author         : George Calin
 * @brief          : Main program body

 *
 ******************************************************************************
 */

#define PERIPH_BASE		(0x40000000UL)

#define AHB1_OFFSET		(0x20000UL)
#define AHB1_BASE		(PERIPH_BASE+AHB1_OFFSET)

#define GPIOG_OFFSET	(0x1800UL)
#define GPIOG_BASE		(AHB1_BASE+GPIOG_OFFSET)

#define RCC_OFFSET		(0x3800UL)
#define RCC_BASE		(AHB1_BASE+RCC_OFFSET)

#define RCC_AHB1ENR_OFFSET 		 (0x30UL)
#define RCC_AHB1ENR				(* (volatile unsigned int *)(RCC_BASE + RCC_AHB1ENR_OFFSET))
#define GPIOGEN (1UL<<6)

#define GPIOG_MODER_OFFSET	(0X00UL)
#define GPIOG_MODER				(* (volatile unsigned int *) (GPIOG_BASE + GPIOG_MODER_OFFSET))

/*
 * since for this microcontroller on PORT G, the reset value is 0x0000 0000 it has zeros on all positions, then we need to set only 1 to position 26
 * &=~(1<<27) //this sets the bit 27 to 0
 * (1<<26) //this sets the bit 26 to 1
 */

#define GPIOG_ODR_OFFSET	(0x14UL)
#define	GPIOG_ODR				(* (volatile unsigned int *) (GPIOG_BASE + GPIOG_ODR_OFFSET))

#define PIN13	(1UL<<13)
#define ODR13	PIN13

#include <stdint.h>

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

int main(void)
{
	// Enable the clock access to PG13 Port G Pin 13
	RCC_AHB1ENR |= GPIOGEN;

	// Set the direction(mode) of Pin 13 on the Port G to OUTPUT
	GPIOG_MODER &=~(1<<27); //set the 27th bit to 0
	GPIOG_MODER |=(1<<26); //set the 26th bit to 1

    /* Loop forever */
	for(;;)
	{
		/*
		// Set the data output to the Pin 13 of the Port G to HIGH
		GPIOG_ODR |=(ODR13);
		*/

		// Toggle the data data outout on Pin 13 of Port G to HIGH
		GPIOG_ODR ^=(ODR13);
		for(int i=0;i<200000;++i);

	}
}
