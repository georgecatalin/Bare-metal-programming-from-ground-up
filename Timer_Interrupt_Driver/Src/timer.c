/*
 * timer.c
 *
 *  Created on: 29 Mar 2023
 *  Author: George Calin
 * 	Target Development Board: STM32 Nucleo F429ZI
 */

#include <timer.h>

#define TIM2_ENR (1UL<<0)
#define TIM2_CR1_EN (1UL<<0)
#define TIM2_DIER__UIE (1UL<<0)

void tim2_everysecond_init(void) // every second means 1Hz
{
	/* Enable clock access to Timer2 */
	RCC->APB1ENR |= TIM2_ENR;

	/* In other words, the prescaler register allows you to slow down the rate at which a timer or counter counts. This is useful when you want to measure longer time intervals or when you want to reduce
       the rate at which a device performs some operation.

       The prescaler register typically has a certain number of bits that determine the division factor. For example, a 8-bit prescaler register can divide
       the input clock by any value between 1 and 256 (2^8), while a 16-bit prescaler register can divide the input clock by any value between 1 and 65536 (2^16).

       To use the prescaler register, you typically write a value to it that corresponds to the desired division factor. This value is then used by the timer or counter to count at a slower rate. For example,
       if you set the prescaler register to 64, the timer or counter will count at a rate that is 64 times slower than the input clock.

     */

	/* Set the prescaler value */
	TIM2->PSC=1600 -1 ; // the clock speed is 16 000 000 / 16 00 = 10 000 is the new rate after prescaling. We place 1600 -1 because we are counting from 0


	/* The auto-reload value is also known as the period register or the reload value. It specifies the maximum value that the timer/counter will count up to or down from before it resets to zero and generates an update event.
	   Once the timer/counter reaches the auto-reload value, it will reset to zero and start counting again from the beginning.
	*/

	/* Set the auto-reload value */
	TIM2->ARR=10000-1; // 10000-1 because we are counting from 0 . Because we want something to happen every second that is 1 Hz, because our second level clock speed is 10 000 => our reload value had to be 1 * 10 000

	/* Clear the counter */
	TIM2->CNT=0;

	/* Enable the timer */
	TIM2->CR1 |= TIM2_CR1_EN;

}

void tim2_everysecond_interrupt(void)
{
	/* Enable clock access to Timer2 */
	RCC->APB1ENR |= TIM2_ENR;



	/* In other words, the prescaler register allows you to slow down the rate at which a timer or counter counts. This is useful when you want to measure longer time intervals or when you want to reduce
       the rate at which a device performs some operation.

       The prescaler register typically has a certain number of bits that determine the division factor. For example, a 8-bit prescaler register can divide
       the input clock by any value between 1 and 256 (2^8), while a 16-bit prescaler register can divide the input clock by any value between 1 and 65536 (2^16).

       To use the prescaler register, you typically write a value to it that corresponds to the desired division factor. This value is then used by the timer or counter to count at a slower rate. For example,
       if you set the prescaler register to 64, the timer or counter will count at a rate that is 64 times slower than the input clock.

     */

	/* Set the prescaler value */
	TIM2->PSC=1600 -1 ; // the clock speed is 16 000 000 / 16 00 = 10 000 is the new rate after prescaling. We place 1600 -1 because we are counting from 0


	/* The auto-reload value is also known as the period register or the reload value. It specifies the maximum value that the timer/counter will count up to or down from before it resets to zero and generates an update event.
	   Once the timer/counter reaches the auto-reload value, it will reset to zero and start counting again from the beginning.
	*/

	/* Set the auto-reload value */
	TIM2->ARR=10000-1; // 10000-1 because we are counting from 0 . Because we want something to happen every second that is 1 Hz, because our second level clock speed is 10 000 => our reload value had to be 1 * 10 000

	/* Clear the counter */
	TIM2->CNT=0;

	/* Enable the timer */
	TIM2->CR1 |= TIM2_CR1_EN;

	/* Enable timer interrupt */
	/* ***********************************************************************************************************************************************************************
	 * Explanation: The info is in RM0090: TIMx DMA/Interrupt enable register (TIMx_DIER)
	 * We are interested in Bit 0 of this registry: UIE: Update interrupt enable as it says
	 * 		 0: Update interrupt disabled
	 * 		 1: Update interrupt enabled
	 * ***********************************************************************************************************************************************************************
	 */
	TIM2->DIER |=TIM2_DIER__UIE;

	/* Enable interrupt in NVIC */
	/* ***********************************************************************************************************************************************************************
	 * Explanations: the typedef name has to be taken from the .h file that described the board which is in this case "stm32f429xx.h"
	 * typedef enum{...}
	 * ***********************************************************************************************************************************************************************
	 */
	NVIC_EnableIRQ(TIM2_IRQn);
}

