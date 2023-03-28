/*
 * adc.c
 *
 *  Created on: 28 Mar 2023
 *  Author: George Calin
 * 	Target Development Board: STM32 Nucleo F429ZI
 */

#include "adc.h"

#define ADC1EN	(1UL<<8)
#define GPIOA_ENR	(1UL<<0)
#define ADC_SQR3	(1UL<<0)
#define ADC_SQR1_LEN 0x00 // setting the entire registry SQR1 at once to 0 bit by bit
#define CR2_ADON (1UL<<0)
#define CR2_SWSTART (1UL<<30)


#define ADCCR1_EOCIE (1UL<<5)

void pa1_adc_init(void)
{
	/* *** CONFIGURE THE ADC GPIO PIN *** */

	/* Enable clock access to ADC GPIO pin which is PA1 */
	RCC->AHB1ENR |= GPIOA_ENR;

	/* Set the mode of PA1 to analog */
	GPIOA->MODER |= (1UL<<3); // '1'
	GPIOA->MODER |= (1UL<<2); // '1'

	/* *** CONFIGURE THE ADC Module *** */
	/* Enable clock access to ADC */
	RCC->APB2ENR |= ADC1EN;

	/* ** Configure the parameters of the ADC ** */

	/* parameter: Conversion sequence start */
	ADC1->SQR3 = ADC_SQR3; // we set it to the value so writing all other bits, not only the one on position SQR1 cause we have only one channel in this use case

	/* parameter: Conversion sequence length */
	ADC1->SQR1 = ADC_SQR1_LEN;


	/* Enable ADC module */
	ADC1->CR2 |= CR2_ADON;
}


void pa1_adc_interrupt_init(void)
{
	/* *** CONFIGURE THE ADC GPIO PIN *** */

	/* Enable clock access to ADC GPIO pin which is PA1 */
	RCC->AHB1ENR |= GPIOA_ENR;

	/* Set the mode of PA1 to analog */
	GPIOA->MODER |= (1UL<<3); // '1'
	GPIOA->MODER |= (1UL<<2); // '1'

	/* *** CONFIGURE THE ADC Module *** */
	/* Enable clock access to ADC */
	RCC->APB2ENR |= ADC1EN;

	/* ** Configure the parameters of the ADC ** */

	/* Enable ADC end-of-conversion interrupt */
	/* *************************************************************************************************************************************************************
	 * Explanations: Info about this is taken from RM0090: ADC control register 1 (ADC_CR1)
	 * We are interested in bit 5 of this registry which stands for EOCIE Interrupt enable for EOC as it says
	 * 		This bit is set and cleared by software to enable/disable the end of conversion interrupt. 0: EOC interrupt disabled, 1: EOC interrupt enabled. An interrupt is generated when the EOC bit is set.
	 * *************************************************************************************************************************************************************
	 */
	ADC1->CR1 |= ADCCR1_EOCIE;

	/* Enable ADC interrupt in NVIC */
	NVIC_EnableIRQ(ADC_IRQn);

	/* parameter: Conversion sequence start */
	ADC1->SQR3 = ADC_SQR3; // we set it to the value so writing all other bits, not only the one on position SQR1 cause we have only one channel in this use case

	/* parameter: Conversion sequence length */
	ADC1->SQR1 = ADC_SQR1_LEN;


	/* Enable ADC module */
	ADC1->CR2 |= CR2_ADON;
}

void start_conversion(void)
{
	/* Start ADC Conversion */
	ADC1->CR2 |= CR2_SWSTART;
}

uint32_t adc_read(void)
{
	/* Wait for conversion to be completed */
	while(!(ADC1->SR & SR_EOC));

	/*Read the result of the conversion*/
	return (ADC1->DR);
}
