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
#include "systick.h"

#define GPIOB_ENABLE (1UL<<1)
#define PIN7	(1UL<<7)
#define LED_PIN	PIN7



static void systick_callback();

int main(void)
{
	/* Enable the clock access via AHB1 to GPIO B */
	RCC->AHB1ENR=GPIOB_ENABLE;

	/* Set the mode in the MODER registry to output for port B7 */
	GPIOB->MODER &=~(1UL<<15); //'0'
	GPIOB->MODER |=(1UL<<14); //'1'

	uart3_tx_init();
	systick_1Hz_interrupt();

	while(1)
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
 * In our case we check which of the name describes the interrupt service routine we need: SysTick_Handler
 * *************************************************************************************************************************************
 */

void SysTick_Handler(void)
{
	systick_callback();
}

static void systick_callback()
{
	/* Toggle the user LED */
	GPIOB->ODR ^=LED_PIN;
	printf("A second just passed......\n\r");
	fflush(stdout);
}
