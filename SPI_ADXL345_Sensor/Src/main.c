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
#include "timer.h"

#define GPIOB_ENABLE (1UL<<1)
#define PIN7	(1UL<<7)
#define LED_PIN	PIN7

#define DMA_LISR__TCIF3 (1UL<<27)
#define DMA_LIFCR__CTCIF3 (1UL<<27)

static void dma1_callback();

int main(void)
{
	char message[50] = "Good day, Alligator from DMA UART TX\n\r";

	/* Enable the clock access via AHB1 to GPIO B */
	RCC->AHB1ENR|=GPIOB_ENABLE;

	/* Set the mode in the MODER registry to output for port B7 */
	GPIOB->MODER &=~(1UL<<15); //'0'
	GPIOB->MODER |=(1UL<<14); //'1'

	uart3_tx_init();
	dma1_stream3_init((uint32_t) message, (uint32_t) &USART3->DR, 50);

	for(;;)
	{


	}

}

static void dma1_callback()
{
	/* Light the user LED */
	GPIOB->ODR |=LED_PIN;
}

void DMA1_Stream3_IRQHandler(void)
{
	/* Check for transfer complete interrupt for Stream 3 */
	/* ***********************************************************************************************************************************************
	 * Explanation: This info is taken from RM0090: DMA low interrupt status register (DMA_LISR) (because we refer to channel 3)
	 * We are going to be interested in bit no.27 TCIFx: Stream x transfer complete interrupt flag (x = 3..0)
	 * 	This bit is set by hardware. It is cleared by software writing 1 to the corresponding bit in the DMA_LIFCR register.
	 * 	0: No transfer complete event on stream x
	 * 	1: A transfer complete event occurred on stream x
	 * 	********************************************************************************************************************************************* */
	 if(DMA1->LISR & DMA_LISR__TCIF3)
	 {
		 /* Clear the flag */
		 /* *******************************************************************************************************************************************
		  * Explanation: This info is taken from RM0090: DMA low interrupt flag clear register (DMA_LIFCR) as it said above when in need to clear the flag
		  * We are going to be interested in bit no.27 which refers to CTCIFx: Stream x clear transfer complete interrupt flag (x = 3..0)
		  * 	Writing 1 to this bit clears the corresponding TCIFx flag in the DMA_LISR register
		  * ***************************************************************************************************************************************** */
		 DMA1->LIFCR |= DMA_LIFCR__CTCIF3;

		 /* Do something */
		 dma1_callback();
	 }
}
