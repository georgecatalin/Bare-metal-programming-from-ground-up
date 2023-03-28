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

void tim3_input_capture(void) // when reading the alternate function mapping on the datasheet, given to the fact the we have a timer TIM4_CH2 and we wish to watch it with another timer on the same channel, then we are going use PA7 and TIM3_CH2
{
	/* Enable clock access to GPIOA */
	RCC->AHB1ENR |= GPIOA_ENABLE;

	/* Set PA7 mode to alternate function mode */
	GPIOA->MODER |= (1UL<<15); // '1'
	GPIOA->MODER &= ~(1UL<<14); // '0'

	/* Set PA7 alternate function type to TIM3_CH2 which is AF2 */
	GPIOA->AFR[0] &= ~(1UL<<31); // '0'
	GPIOA->AFR[0] &= ~(1UL<<30); // '0'
	GPIOA->AFR[0] |=(1UL<<29); // '1'
	GPIOA->AFR[0] &= ~(1UL<<28); // '0'

	/* Enable clock access to tim3 */
	RCC->APB1ENR |= TIM3_ENR;

	/* Set the prescaler by dividing the clock */
	TIM3->PSC = 16000 - 1;  // This is diving our clock 16 000 000 /16 000

	/* Set CH2 to input mode */
	TIM3->CCMR1 = TIM3_CCMR1_CC2S;

	/* Enable CH2 to capture rising edge , rising edge is the default without altering other bits in the registry */
	TIM3->CCER = TIM3_CCER_CC2E;

	/* Enable TIM3 */
	TIM3->CR1 = TIM3_CR1_CEN;
}

