/*
 * timer.c
 *
 *  Created on: 22 Mar 2023
 *  Author: George Calin
 */

#include <timer.h>

#define GPIOA_ENABLE (1UL<<0)
#define GPIOB_ENABLE (1UL<<1)

#define TIM4_ENR (1UL<<2)
#define TIM3_ENR (1UL<<1)

#define TIM4_CCRM1_OC2M	((1UL<<13)|(1UL<<12))
#define TIM4_CCER_CC2E	(1UL<<4)

#define TIM4_CR1_CEN (1UL<<0)

#define TIM3_CCMR1_CC2S (1UL<<9)
#define TIM3_CCER_CC2E (1UL<<4)
#define TIM3_CR1_CEN	(1UL<<0)


void tim4_output_compare(void) // when reading the alternate function mapping on the datasheet we observe that TIM4_CH1 is connected to PB7 where the user LED connects
{
	/* Enable clock access to PB7 */
	RCC->AHB1ENR |= GPIOB_ENABLE;

	/* Set PB7 mode to alternate function */
	GPIOB->MODER |=(1UL<<15); //'1'
	GPIOB->MODER &=~(1UL<<14); // '0'

	/* Set PB7 alternate function type to TIM4_CH2 which is AF2 */
	GPIOB->AFR[0] &= ~(1UL<<31); // '0'
	GPIOB->AFR[0] &= ~(1UL<<30); // '0'
	GPIOB->AFR[0] |= (1UL<<29); // '1'
	GPIOB->AFR[0] &= ~(1UL<<28); // '0'

	/* Enable clock access to Timer2 */
	RCC->APB1ENR |= TIM4_ENR;

	/* Set the prescaler value */
	TIM4->PSC=1600 -1 ; // the clock speed is 16 000 000 / 16 00 = 10 000 is the new rate after prescaling. We place 1600 -1 because we are counting from 0

	/* Set the auto-reload value */
	TIM4->ARR=10000-1; // 10000-1 because we are counting from 0 . Because we want something to happen every second that is 1 Hz, because our second level clock speed is 10 000 => our reload value had to be 1 * 10 000

	/* Set output compare toggle mode */
	TIM4->CCMR1 = TIM4_CCRM1_OC2M;

	/* Enable TIM2 CH1 in compare mode */
	TIM4->CCER |= TIM4_CCER_CC2E;

	/* Clear the counter */
	TIM4->CNT=0;

	/* Enable the timer */
	TIM4->CR1 |= TIM4_CR1_CEN;

}

