/**
 ******************************************************************************
 * @file           : main.c
 * @author         : George Calin
 * @brief          : Main program body
 * Target board: Nucleo 144 family
 *
 * Notes: User LD3: a red user LED is connected to PB14 on the Nucleo 144 family boards
 ******************************************************************************
 */

#include <stdio.h>
#include "uart.h"
#include "adc.h"

uint32_t sensor_val;

int main(void)
{
	uart3_tx_init();
	pa1_adc_init();
	start_conversion();

	for(;;)
	{
		sensor_val = adc_read();
		printf("Sensor value: %d \n\r", (int) sensor_val);
	}

}


