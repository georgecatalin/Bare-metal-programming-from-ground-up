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
#include "adc.h"

uint32_t sensor_val;

static void adc_callback();

int main(void)
{
	uart3_tx_init();
	pa1_adc_interrupt_init();


	while(1)
	{
		start_conversion();
	}

}


/* ***********************************************************************************************************************************
 * Explanations: At this stage we wish to fire up the interrupt. Hence we need to implement the ISR
 * (Interrupt Service Routine) function which is also called Interrupt Hander (IH)
 * The name of the function matters as it has to exist in the vector table recognized by the NVIC hardware
 * so one gotta check the possibilities of names for the ISR in the Startup folder > startup_stm32f429zitx.s
 * There is a section that referts to the vector table
 * line 129 : g_pfnVectors: ....
 * In our case we check which of the name describes the interrupt service routine we need: ADC_IRQHandler
 * *************************************************************************************************************************************
 */

void ADC_IRQHandler(void)
{
	/* Check if the EOC end of conversion was achieved */
	/* *********************************************************************************************************************************
	 * Explanations: The info is taken from RM0090: ADC status register (ADC_SR)
	 * This registry uses only bits 0..5 while the rest of the bits are reserved.
	 * We are interested in bit 5 EOC Regular channel end of conversion as it says
	 * 		This bit is set by hardware at the end of the conversion of a regular group of channels.
	 * 		0: Conversion not complete (EOCS=0), or sequence of conversions not complete (EOCS=1)
	 * 		1: Conversion complete (EOCS=0), or sequence of conversions complete (EOCS=1)
	 * *********************************************************************************************************************************
	 */

	if(ADC1->SR & SR_EOC)
	{
		/* Clear the EOC flag */
		ADC1->SR &=~ SR_EOC;
		adc_callback();
	}
}

static void adc_callback()
{
	sensor_val = ADC1->DR;
	printf("Sensor value: %d \n\r", (int) sensor_val);
}
