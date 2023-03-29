/*
 * adc.c
 *
 *  Created on: 16 Mar 2023
 *  Author: George Calin
 *  Target board: Nucleo 144 family
 */

#include "adc.h"

#define ADC1EN	(1UL<<8)
#define GPIOA_ENR	(1UL<<0)
#define ADC_SQR3	(1UL<<0)
#define ADC_SQR1_LEN 0x00 // setting the entire registry SQR1 at once to 0 bit by bit
#define CR2_ADON (1UL<<0)
#define CR2_SWSTART (1UL<<30)
#define SR_EOC (1UL<<1)

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
