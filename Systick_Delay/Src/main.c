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


int main(void)
{
	/* Enable the clock access via AHB1 to GPIO B */
	RCC->AHB1ENR=GPIOB_ENABLE;

	/* Set the mode in the MODER registry to output for port B7 */
	GPIOB->MODER &=~(1UL<<15); //'0'
	GPIOB->MODER |=(1UL<<14); //'1'

	uart3_tx_init();

	for(;;)
	{

		systickDelayMilliseconds(1000);

		/* Toggle the user LED */
		GPIOB->ODR ^=LED_PIN;
		printf("A second just passed......\n\r");
		fflush(stdout);

	}

}


