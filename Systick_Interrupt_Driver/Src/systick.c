/*
 * systick.c
 *
 *  Created on: 29 Mar 2023
 *  Author: George Calin
 * 	Target Development Board: STM32 Nucleo F429ZI
 */

#include "systick.h"

#define SYSTICK_LOAD_VALUE	(16000)
#define CTRL_ENBLE	(1UL<<0)
#define CTRL_CLKSOURCE (1UL<<2)
#define CTRL_COUNTFLAG	(1UL<<16)
#define ON_LOAD_SECOND (16000000)
#define SYST_CSR_TICKINT (1UL<<1)

void systickDelayMilliseconds(int milliseconds)
{
	/* Reload with the number of clocks per milliseconds */
    SysTick->LOAD=SYSTICK_LOAD_VALUE;

	/* Clear the systick current value register */
    SysTick->VAL=0;

	/* Enable the systick and select the processor as clock source */
    SysTick->CTRL= CTRL_ENBLE | CTRL_CLKSOURCE;

    for(int i=0;i<milliseconds;++i)
    {
    	/* Wait until the timer counter downwards to value 0 */
    	while(!(SysTick->CTRL & CTRL_COUNTFLAG));
    }

    /* Disable the systick */
    SysTick->CTRL=0;
}

void systick_1Hz_interrupt(void)
{
	/* Reload with the number of clocks per milliseconds */
	/* ***********************************************************************************************
	 * Explanations: 16000000 Mhz is the clock speed of the ARM processor
	 * **********************************************************************************************
	 */
    SysTick->LOAD=ON_LOAD_SECOND;

	/* Clear the systick current value register */
    SysTick->VAL=0;

	/* Enable the systick and select the processor as clock source */
    SysTick->CTRL= CTRL_ENBLE | CTRL_CLKSOURCE;

    /* Enable Systick Interrupt */
    /* ***************************************************************************
     * Explanation: the info is taken from Cortex-M4 Devices Generic User Guide: System timer, SysTick
     * TICKINT Enables SysTick exception request:
     * 	0 = counting down to zero does not assert the SysTick exception request
     * 	1 = counting down to zero asserts the SysTick exception request.
     * ***************************************************************************
     */
    SysTick->CTRL |= SYST_CSR_TICKINT;
}
