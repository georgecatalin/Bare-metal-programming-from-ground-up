/**
 ******************************************************************************
 * @file           : main.c
 * @author         : George Calin
 * @brief          : Main program body
 *
 *
 * Notes: User LD3: a red user LED is connected to PB14 on the Nucleo 144 family boards
 ******************************************************************************
 */

#include <stdio.h>
#include "uart.h"

#define GPIODENR (1UL<<1)
#define PIN14	(1UL<<14)

char receivedKey;

int main(void)
{
	uart3_rxtx_init();

	for(;;)
	{
G		receivedKey=uart3_rx_read();

		/* Enable clock access to GPIOD on AHB1 */
		RCC->AHB1ENR |= GPIODENR;

		/* Set the mode of PB14 to output */
		GPIOB->MODER &=~(1UL<<29);
		GPIOB->MODER |=(1UL<<28);

		if(receivedKey=='g')
		{
			//light user LED1
			GPIOB->ODR |=PIN14;
		}
		else
		{
			//disable user LED1
			GPIOB->ODR &= ~(PIN14);
		}

	}
}


