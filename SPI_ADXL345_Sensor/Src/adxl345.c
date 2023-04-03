/*
 * adxl345.c
 *
 *  Created on: 3 Apr 2023
 *  Author: George Calin
 * 	Target Development Board: STM32 Nucleo F429ZI
 */


#include "adxl345.h"


#define   MULTI_BYTE_EN				0x40
#define	  READ_OPERATION		    0x80



void adxl_read(uint8_t address, uint8_t * rxdata)
{

	  /*Set read operation*/
	  address |= READ_OPERATION;

	  /*Enable multi-byte*/
	  address |= MULTI_BYTE_EN;

	  /*Pull cs line low to enable slave*/
	  cs_enable();

      /*Send address*/
	  spi1_transmit(&address,1);

	  /*Read 6 bytes */
	  spi1_receive(rxdata,6);

	  /*Pull cs line high to disable slave*/
	  cs_disable();

}

void adxl_write (uint8_t address, uint8_t value)
{
  uint8_t data[2];

  /*Enable multi-byte, place address into buffer*/
  data[0] = address|MULTI_BYTE_EN;

  /*Place data into buffer*/
  data[1] = value;

  /*Pull cs line low to enable slave*/
  cs_enable();

  /*Transmit data and address*/
  spi1_transmit(data, 2);

  /*Pull cs line high to disable slave*/
  cs_disable();


}



void adxl_init (void)
{
	/*Enable SPI gpio*/
	spi_gpio_init();

	/*Config SPI*/
	spi1_config();

	/*Set data format range to +-4g*/
	adxl_write (DATA_FORMAT_R, FOUR_G);

	/*Reset all bits*/
	adxl_write (POWER_CTL_R, RESET);

	/*Configure power control measure bit*/
	adxl_write (POWER_CTL_R, SET_MEASURE_B);
}
