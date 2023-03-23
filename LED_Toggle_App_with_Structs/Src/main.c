/**
 ******************************************************************************
 * @file           : main.c
 * @author         : George Calin
 * @brief          : Main program body

 *
 ******************************************************************************
 */

#include <stdint.h>

#define PERIPH_BASE		(0x40000000UL)

#define AHB1_OFFSET		(0x20000UL)
#define AHB1_BASE		(PERIPH_BASE+AHB1_OFFSET)

#define GPIOG_OFFSET	(0x1800UL)
#define GPIOG_BASE		(AHB1_BASE+GPIOG_OFFSET)

#define RCC_OFFSET		(0x3800UL)
#define RCC_BASE		(AHB1_BASE+RCC_OFFSET)

#define GPIOGEN (1UL<<6)

/*
 * since for this microcontroller on PORT G, the reset value is 0x0000 0000 it has zeros on all positions, then we need to set only 1 to position 26
 * &=~(1<<27) //this sets the bit 27 to 0
 * (1<<26) //this sets the bit 26 to 1
 */

#define PIN13	(1UL<<13)
#define ODR13	PIN13

#define __IO virtual      //this is a macro which is used in the MISRA C programming style


typedef struct
{
	volatile uint32_t DUMMY[12]; //There are 12 other different registries in the RCC Registries until AHB1ENR (see RCC Register map in RM009)
	volatile uint32_t AHB1ENR;
}RCC_typedef;

typedef struct
{
	volatile uint32_t  MODER;

	/*
	 * **************************************************************************************************************************************************************
	 * Even if we do not need all the registries to be implemented in the struct, we have to uphold their order because we need to match the offset for each members.
	 * Hence for 4 members of uint32_t that do not interest us with simulate the space with an array uint32_t DUMMY[4] , each member of 4 bytes (32 bits) . One can observe this best in the GPIO register map of RM009
	 * **************************************************************************************************************************************************************
	 */
	volatile uint32_t DUMMY[4];

	volatile uint32_t ODR;
}GPIO_typedef;

#define RCC	((RCC_typedef *) RCC_BASE)
#define GPIO	((GPIO_typedef *) GPIOG_BASE)



#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

int main(void)
{

	// Enable the clock access to PG13 Port G Pin 13
	RCC->AHB1ENR |= GPIOGEN;

	// Set the direction(mode) of Pin 13 on the Port G to OUTPUT
	GPIO->MODER &=~(1<<27); //set the 27th bit to 0
	GPIO->MODER |= (1<<26); //set the 26th bit to 1

    /* Loop forever */
	for(;;)
	{
		// Toggle the data output on Pin 13 of Port G to HIGH
		GPIO->ODR ^=(ODR13);
		for(int i=0;i<500000;++i);

	}
}
