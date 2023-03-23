/*
 * systick.c
 *
 *  Created on: 21 Mar 2023
 *  Author: George Calin
 */

#include "systick.h"

#define SYSTICK_LOAD_VALUE	(16000)
#define CTRL_ENBLE	(1UL<<0)
#define CTRL_CLKSOURCE (1UL<<2)
#define CTRL_COUNTFLAG	(1UL<<16)

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
