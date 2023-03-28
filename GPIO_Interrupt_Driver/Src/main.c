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
#include "exti.h"
#include "uart.h"

#define EXTI_PR_LINE_13	(1UL<<13)

#define GPIOB_ENABLE (1UL<<1)
#define PIN7	(1UL<<7)
#define LED_PIN	PIN7

static void exti_callback(void);


int main(void)
{
	/* Enable the clock access via AHB1 to GPIO B */
	RCC->AHB1ENR=GPIOB_ENABLE;

	/* Set the mode in the MODER registry to output for port B7 */
	GPIOB->MODER &=~(1UL<<15); //'0'
	GPIOB->MODER |=(1UL<<14); //'1'


	pc13_exti_init();
	uart3_tx_init();

	for(;;)
	{

	}

}

/* ***********************************************************************************************************************************
 * Explanations: At this stage we wish to fire up the interrupt. Hence we need to implement the ISR
 * (Interrupt Service Routine) function which is also called Interrupt Hander (IH)
 * The name of the function matters as it has to exist in the vector table recognized by the NVIC hardware
 * so one gotta check the possibilities of names for the ISR in the Startup folder > startup_stm32f429zitx.s
 * There is a section that referts to the vector table
 * line 129 : g_pfnVectors: ....
 * In our case we check which of the name describes the interrupt service routine we need: EXTI15_10_IRQHandler()
 * Note: EXTI15_10_IRQHandler() function is servicing all the interrupts coming from Line 10,11,12,13,14,15 hence we need to clarify which
 * is the line where the interrupts comes from. This is clarified using the RM0090:  Pending register (EXTI_PR) as it says
 * "PRx: Pending bit -> 0: No trigger request occurred ; 1: selected trigger request occurred "
 * If that bit is '1' then it means the interrupt came from that line
 * *************************************************************************************************************************************
 */

void EXTI15_10_IRQHandler()
{
	/* Check if the interrupt originated from Line 13 */
	if(EXTI->PR & EXTI_PR_LINE_13)
	{
		/* Clear the PR Flag */
		EXTI->PR |=(EXTI_PR_LINE_13);

		exti_callback();
	}
}

static void exti_callback(void)
{
	printf("Someone pressed that button, alligator...\n\r");

	/* Toggle the user LED */
	GPIOB->ODR ^=LED_PIN;
}
