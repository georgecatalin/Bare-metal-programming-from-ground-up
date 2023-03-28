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

#define GPIOB_ENABLE (1UL<<1)
#define PIN7	(1UL<<7)
#define LED_PIN	PIN7

#define USART_SR_RXNE (1ul<<5)

static void usart3_callback(void);

char myKey;


int main(void)
{
	/* Enable clock access to GPIOB */
	RCC->AHB1ENR |= GPIOB_ENABLE;

	/* Set the mode in the MODER registry to output for port B7 */
	GPIOB->MODER &=~(1UL<<15); //'0'
	GPIOB->MODER |=(1UL<<14); //'1'

	uart3_rx_interrupt_init();

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
 * In our case we check which of the name describes the interrupt service routine we need: USART3_IRQHandler
 * *************************************************************************************************************************************
 */

void USART3_IRQHandler()
{
  /* Check if the RXNE bit is set */
  /* **********************************************************************************************************************************
   * Explanations: This info is extracted from RM0090: Status register (USART_SR)
   * The bit 5 RXNE Read data register not empty and it is described as: 0: Data is not received, 1: Received data is ready to be read.
   * *********************************************************************************************************************************
   */

	if(USART3->SR & USART_SR_RXNE)
	{
		usart3_callback();
	}
}

static void usart3_callback(void)
{
	myKey = USART3->DR;


	if(myKey=='1')
			{
				GPIOB->ODR |= LED_PIN;
			}
			else
			{
				GPIOB->ODR &=~(LED_PIN);
			}
}

