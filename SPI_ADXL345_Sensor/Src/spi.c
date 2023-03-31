/*
 * spi.c
 *
 *  Created on: 31 Mar 2023
 *  Author: George Calin
 * 	Target Development Board: STM32 Nucleo F429ZI
 */

#include "spi.h"

#define RCC_APB2ENR__SPI1 (1UL<<12)
#define RCC_AHB1ENR__GPIOAEN (1UL<<0)

// PA5 -> CLK
// PA6 -> MISO
// PA7 -> MOSI

// PA9 -> Slave Select

#define SPI_CR1__CPOL (1UL<<1)
#define SPI_CR1__CPHA  (1UL<<0)
#define SPI_SR__TXE (1UL<<1)
#define SPI_SR__BSY (1UL<<7)
#define SPI_SR__RXNE (1UL<<0)

void spi1_gpio_init(void)
{
	/* Identify the SPI interfaces which are available on the STM32 Nucleo 144 board */
	/* **********************************************************************************************************************************************************************************************************
	 * Explanation: the information is taken from STM32F427xx STM32F429xx datasheet: Block diagram
	 * For this board we can spot SPI1..SPI6
	 * We are going to be interested in SPI1 which is connected to APB2
	 * ******************************************************************************************************************************************************************************************************** */

	 /* The SPI requires 4 GPIO Pins for MISO, MOSI, SCK and SS. Hence we gotta check in the alternate function table of the datasheet */
	 /* ***********************************************************************************************************************************************************************************************************
	  * Explanation: information is taken from RM0090 Table 12. STM32F427xx and STM32F429xx alternate function mapping
	  * We notice that SPI1_SCK is connected to PA5, SPI1_MISO is connected to PA6, SPI1_MOSI is connected to PA7. For SPI1_SS we can use any pin, thus we are taking PA9 for SPI1_SS
	  *
	  * Connection from the board to the sensor: PA9(SS)->CS ; PA6(MISO) ->SDO, PA7(MOSI) ->SDA, PA5(CLK) -> SCL
	  * ********************************************************************************************************************************************************************************************************* */

	  /* Enable clock access to GPIO A */
	  /* **********************************************************************************************************************************************************************************************************
		 * Explanation: information is taken from RM0090 RCC AHB1 peripheral clock register (RCC_AHB1ENR)
		 * We are going to be interested in bit no. 0 GPIOAEN: IO port A clock enable
		 * 		This bit is set and cleared by software.
		 * 		0: IO port A clock disabled
		 * 		1: IO port A clock enabled
		 * ********************************************************************************************************************************************************************************************************* */
	 RCC->AHB1ENR |=RCC_AHB1ENR__GPIOAEN;

	 /* Configure PA5,PA6,PA7 to alternate function mode */
	 /* **********************************************************************************************************************************************************************************************************
	    Explanation: information is taken from RM0090 GPIO port mode register (GPIOx_MODER) (x = A..I/J/K)
	    We are going to be interested in the bits 10,11(port 5), bits 13,12(port 6) and bits 14,15(port 7) and we shall set these to Alternate Function mode  '10'
	    ********************************************************************************************************************************************************************************************************* */

	 //Port 5
	 GPIOA->MODER |= (1UL<<11); // '1'
	 GPIOA->MODER &=~(1UL<<10); // '0'

	 //Port 6
	 GPIOA->MODER |= (1UL<<13); // '1'
	 GPIOA->MODER &=~(1UL<<12); // '0'

	 //Port 7
	 GPIOA->MODER |= (1UL<<15); // '1'
	 GPIOA->MODER &=~(1UL<<14); // '0'

	 /* Set PA9 as an output pin */

	 /* Configure PA5,PA6,PA7 to alternate function mode */
	 /* **********************************************************************************************************************************************************************************************************
	    Explanation: information is taken from RM0090 GPIO port mode register (GPIOx_MODER) (x = A..I/J/K)
	    We are going to be interested in the bits 19,18 (port 9) and we will set it to General purpose output mode '01'
	    ********************************************************************************************************************************************************************************************************* */
	 GPIOA->MODER &= ~(1UL<<19); //'0'
	 GPIOA->MODER |=(1UL<<18); //'1'

	 /* Set the PA5,PA6 and PA7 alternate function type to SP1 which is AF5 */
	 /* **********************************************************************************************************************************************************************************************************
	    Explanation: information is taken from RM0090 GPIO alternate function low register (GPIOx_AFRL) (x = A..I/J/K)
	    We are going to use the bits 20..31 of this registry to set it to AF5, port by port, where AF5 is '0101'
	    ********************************************************************************************************************************************************************************************************* */
	//Port 5
	 GPIOA->AFR[0] &=~(1UL<<23); //'0'
	 GPIOA->AFR[0] |=(1UL<<22); //'1'
	 GPIOA->AFR[0] &=~(1UL<<21); //'0'
	 GPIOA->AFR[0] |=(1UL<<20); //'1'

   //Port 6
	 GPIOA->AFR[0] &=~(1UL<<27); //'0'
	 GPIOA->AFR[0] |=(1UL<<26); //'1'
	 GPIOA->AFR[0] &=~(1UL<<25); //'0'
	 GPIOA->AFR[0] |=(1UL<<24); //'1'

   //Port 7
	 GPIOA->AFR[0] &=~(1UL<<31); //'0'
	 GPIOA->AFR[0] |=(1UL<<30); //'1'
	 GPIOA->AFR[0] &=~(1UL<<29); //'0'
	 GPIOA->AFR[0] |=(1UL<<28); //'1'

}

void spi1_config(void)
{

	/* Enable clock access to SPI1 */
	/* **********************************************************************************************************************************************************************************************************
	 * Explanation: information is taken from RM0090 RCC APB2 peripheral clock enable register (RCC_APB2ENR)
	 * In this case we are going to be interested in bit no.12 SPI1EN: SPI1 clock enable
	 * 		This bit is set and cleared by software.
	 * 		0: SPI1 clock disabled, 1: SPI1 clock enabled
	 * ********************************************************************************************************************************************************************************************************** */
	 RCC->APB2ENR |=RCC_APB2ENR__SPI1;

	 /* Set the clock of the SPI1 module to fPCLK/4 */
	 /* **********************************************************************************************************************************************************************************************************
	 	* Explanation: information is taken from RM0090 SPI control register 1 (SPI_CR1) (not used in I2S mode)
	 	* We are interested in bits 3..5 BR[2:0]: Baud rate control
	 	* 	000: fPCLK/2 ; 001: fPCLK/4 ; 010: fPCLK/8; 011: fPCLK/16 ; 100: fPCLK/32 ; 101: fPCLK/64 ; 110: fPCLK/128 ; 111: fPCLK/256
	 	* ********************************************************************************************************************************************************************************************************** */
	 SPI1->CR1 &= ~(1UL<<5); //'0'
	 SPI1->CR1 &=~(1UL<<4); // '0'
	 SPI1->CR1 |=(1UL<<3); // '1'

	 /* Set the clock phase to 1 and the clock polarity to 1 */
	 /* **********************************************************************************************************************************************************************************************************
	  * Explanation: information is taken from RM0090 SPI control register 1 (SPI_CR1) (not used in I2S mode)
	  * We are interested in bits no. 1 CPOL: Clock polarity and bits no.0 CPHA: Clock phase
	  * 	Bit1 CPOL: Clock polarity
	  * 		0: CK to 0 when idle
	  * 		1: CK to 1 when idle
	  * 	Bit 0 CPHA: Clock phase
	  * 		0: The first clock transition is the first data capture edge
	  * 		1: The second clock transition is the first data capture edge
	  * ********************************************************************************************************************************************************************************************************** */
	 SPI1->CR1 |=SPI_CR1__CPOL; //'1'
	 SPI1->CR1 |=SPI_CR1__CPHA; //'1'

	 /* Enable full duplex */
	 /* **********************************************************************************************************************************************************************************************************
	  * Explanation: information is taken from RM0090 SPI control register 1 (SPI_CR1) (not used in I2S mode)
	  * We are interested in bit no. 10 RXONLY: Receive only
	  * 	0: Full duplex (Transmit and receive)
	  * 	1: Output disabled (Receive-only mode)
	  * ********************************************************************************************************************************************************************************************************* */
	 SPI1->CR1 &=~(1UL<<10); //'0'

	/* Set the frame format */
	 /* **********************************************************************************************************************************************************************************************************
	  * Explanation: information is taken from RM0090 SPI control register 1 (SPI_CR1) (not used in I2S mode)
	  * We are interested in bit no. 7 LSBFIRST: Frame format
	  * 	0: MSB transmitted first
	  * 	1: LSB transmitted first
	  * ********************************************************************************************************************************************************************************************************* */
	 SPI1->CR1 &=~(1UL<<7);

	 /* Select the Master */
	 /* **********************************************************************************************************************************************************************************************************
	  * Explanation: information is taken from RM0090 SPI control register 1 (SPI_CR1) (not used in I2S mode)
	  * We are going to be interested in bit no.2 MSTR: Master selection
	  * 	0: Slave configuration
	  * 	1: Master configuration
	  * ******************************************************************************************************************************************************************************************************** */

	 /* Set the data mode to 8 bit*/
	 /* **********************************************************************************************************************************************************************************************************
	  * Explanation: information is taken from RM0090 SPI control register 1 (SPI_CR1) (not used in I2S mode)
	  * We are interested in bit no.11 DFF: Data frame format
	  * 	0: 8-bit data frame format is selected for transmission/reception
	  * 	1: 16-bit data frame format is selected for transmission/reception
	  * ********************************************************************************************************************************************************************************************************** */
	 SPI1->CR1 &= ~(1UL<<11); //'0'

	 /* Configure the SPI to work in a software slave management this implies we need to set SSI bit to 1 and SSM  bit to 1 */
	 /* **********************************************************************************************************************************************************************************************************
	  * Explanation: information is taken from RM0090 SPI control register 1 (SPI_CR1) (not used in I2S mode)
	  * We need to set bit 9 SSM: Software slave management
	  * 	0: Software slave management disabled
	  * 	1: Software slave management enabled
	  * And we need to set bit 8 SSI: Internal slave select
	  * 	This bit has an effect only when the SSM bit is set. The value of this bit is forced onto the NSS pin and the IO value of the NSS pin is ignored.
	  * ********************************************************************************************************************************************************************************************************* */
	 SPI1->CR1 |=(1UL<<9); //'1'
	 SPI1->CR1 |=(1UL<<8); //'1'

	 /* Enable the SPI module */
	 /* **********************************************************************************************************************************************************************************************************
	  * Explanation: information is taken from RM0090 SPI control register 1 (SPI_CR1) (not used in I2S mode)
	  * We are interested in bit no. 6 SPE: SPI enable
	  * 	0: Peripheral disabled
	  * 	1: Peripheral enabled
	  * ********************************************************************************************************************************************************************************************************* */
	 SPI1->CR1 |=(1UL<<6); //'1'

}

void spi1_transmit(uint8_t *data, uint32_t size)
{
	uint32_t i=0;
	uint8_t temporary;

	while(i<size)
	{
		/* Wait until transmit buffer is empty */
		 /* **********************************************************************************************************************************************************************************************************
		  * Explanation: information is taken from RM0090 SPI status register (SPI_SR)
		  * We are interested in bit no. 1 TXE: Transmit buffer empty
		  * 	0: Tx buffer not empty
		  * 	1: Tx buffer empty
		  * ********************************************************************************************************************************************************************************************************** */
		  while(!(SPI1->SR & SPI_SR__TXE)) ;

		  /* Write the data to the data register */
		  SPI1->DR=data[i];
		  i++;
	}

	/* Wait until TXE is set */
	  while(!(SPI1->SR & SPI_SR__TXE)) ;

	/* Wait for the busy flag to reset */
	 /* **********************************************************************************************************************************************************************************************************
	    * Explanation: information is taken from RM0090 SPI status register (SPI_SR)
	    * We are interested in bit no. 7 BSY: Busy flag
	    * 	0: SPI (or I2S) not busy
	    * 	1: SPI (or I2S) is busy in communication or Tx buffer is not empty
	    * This flag is set and cleared by hardware.
	    * ******************************************************************************************************************************************************************************************************* */
	while(SPI1->SR & SPI_SR__BSY);

	/* Clear the overflow OVR flag by reading the data and the status register */
	/* ************************************************************************************************************************************************************************************************************
	 * Overrun condition
		An overrun condition occurs when the master device has sent data bytes and the slave
		device has not cleared the RXNE bit resulting from the previous data byte transmitted.
		When an overrun condition occurs:
		• the OVR bit is set and an interrupt is generated if the ERRIE bit is set.
		In this case, the receiver buffer contents will not be updated with the newly received data from the master device. A read from the SPI_DR register returns this byte. All other subsequently transmitted bytes are lost.

		Clearing the OVR bit is done by a read from the SPI_DR register followed by a read access to the SPI_SR register.
		************************************************************************************************************************************************************************************************************/
	  temporary=SPI1->DR;
	  temporary=SPI1->SR;
}

void spi1_receive(uint8_t *data, uint32_t size)
{
	while(size)
	{
		/* Send Dummy Data */
		SPI1->DR=0;

		/* Wait for the receive buffer not empty */
		/* **********************************************************************************************************************************************************************************************************
		 * Explanation: information is taken from RM0090 SPI status register (SPI_SR)
		 * We are interested in bit no.0 RXNE: Receive buffer not empty
		 * 		0: Rx buffer empty
		 * 		1: Rx buffer not empty
		 * 	********************************************************************************************************************************************************************************************************/
		while(!(SPI1->SR & SPI_SR__RXNE));

		/* Read data from data register */
		*data++ = SPI1->DR;
		size--;


	}
}

void cs_enable(void)
{
	//PA9
	GPIOA->ODR &=~(1UL<<9);
}

//pull high to disable
void cs_disable(void)
{
	GPIOA->ODR |=(1UL<<9);
}
