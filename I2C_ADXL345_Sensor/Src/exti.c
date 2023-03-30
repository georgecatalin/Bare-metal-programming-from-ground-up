/*
 * exti.c
 *
 *  Created on: 28 Mar 2023
 *  Author: George Calin
 *  Target Development Board: STM32 Nucleo F429ZI
 */

#include "exti.h"

#define GPIOC_ENR  (1UL<<2) //this info is taken from RM0090: RCC AHB1 peripheral clock register (RCC_AHB1ENR)
#define SYSCFG_EN (1UL<<14) //this info is taken from RM0090: RCC APB2 peripheral clock enable register (RCC_APB2ENR)


/* From the USER MANUAL of the board we observe that the push button is connected to PC13 */
void pc13_exti_init(void)
{
	/* optional, but good practice: Disable global interrupt */
	__disable_irq();

	/* Enable clock access for GPIOC */
	RCC->AHB1ENR |= GPIOC_ENR;

	/* Set PC13 as Input Pin , even this is the default mode we need to be explicit about it */
	GPIOC->MODER &= ~(1<<27); // '0' from RM0090: GPIO port mode register (GPIOx_MODER) (x = A..I/J/K)
	GPIOC->MODER &= ~(1<<26); // '0' from RM0090: GPIO port mode register (GPIOx_MODER) (x = A..I/J/K)

	/* Enable clock access to SYSCFG */
	RCC->APB2ENR |= SYSCFG_EN;

	/* Select port C for EXTI13 */
	/* *************************************************************************************************************************************
	 * Explanations: This info is taken from RM0090: SYSCFG external interrupt configuration register 4 (SYSCFG_EXTICR4)
	 * We delve into RM0090 SYSCFG_EXTICR1 and we notice the it binds the registry to EXTI lines so EXTI1[3:0] is the line 1.
	 * Since we need to address to EXTI13, then we gotta to use SYSCFG_EXTICR4 as it sweeps from EXTI12 to EXTI15 and EXTI13 is in between.
	 * So we need to set the bits 20..23 from SYSCFG_EXTICR4 to '0010' to enable PC[13] pin
	 * Since the reset value for the  SYSCFG_EXTICR4 registry is 0x0000 0000, then we ought to set a single bit which is bit 5 to 1
	 * *************************************************************************************************************************************
	 */
	SYSCFG->EXTICR[3] |= (1UL<<5);

	/* Unmask EXTI13  */
	/* *************************************************************************************************************************************
	 * Explanations: This info is taken from RM0090: Interrupt mask register (EXTI_IMR)
	 * We need to set the bit that refers to MR13 which is bit 13 to '1' as it says
	 * "MRx: Interrupt mask on line x -> 1: Interrupt request from line x is not masked"
	 * *************************************************************************************************************************************
	 */
	EXTI->IMR |= (1UL<<13);

	/* Select falling edge trigger */
	/* *************************************************************************************************************************************
	 * Explanations: This info is taken from RM0090: Falling trigger selection register (EXTI_FTSR)
	 * We need to set the bit that refers to TR13 which is bit 13 to '1' as it says
	 * "TRx: Falling trigger event configuration bit of line x ->1: Falling trigger enabled (for Event and Interrupt) for input line."
	 * *************************************************************************************************************************************
     */
	EXTI->FTSR |= (1UL<<13);

	/* Enable EXTI line in the NVIC Nested Vector Interrupt Controller */
	NVIC_EnableIRQ(EXTI15_10_IRQn); //this is a function from the .h STM32 header files

	/* Enable  global Interrupts */
	__enable_irq();
}
