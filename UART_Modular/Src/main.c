/**
 ******************************************************************************
 * @file           : main.c
 * @author         : George Calin
 * @brief          : Main program body
 ******************************************************************************
 */

#include <stdio.h>
#include "uart.h"


/* ******************************
 * CORRECTION FOR THE NUCLEO 144 :
  * you need to configure the PD8 and PD9 with UART3.
 * ***************************** */

int main(void)
{
	uart3_tx_init();

	for(;;)
	{
		printf("Welcome in bare-metal programming, alligator...\n\r");

	}
}


