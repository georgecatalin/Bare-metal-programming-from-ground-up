/*
 * i2c.c
 *
 *  Created on: 30 Mar 2023
 *  Author: George Calin
 * 	Target Development Board: STM32 Nucleo F429ZI with the ADXL345 Accelerometer sensor
 */

#include "i2c.h"

#define RCC_AHB1ENR__GPIOB	(1UL<<1)
#define RCC_APB1ENR__I2C1	(1UL<<21)
#define I2C_100Khz	(80) //0b 0101 0000 this can be calculated with STMCubeMX
#define SD_MODE_MAX_RISE_TIME	(17) //This can be taken from STMCubeMX
#define I2C_CR1__PE (1UL<<0)
#define I2C_SR2__BUSY (1UL<<1)
#define I2C_CR1__START (1UL<<8)
#define I2C_SR1__SB (1UL<<0)
#define I2C_SR1__ADDR (1UL<<1)
#define I2C_SR1__TxE (1UL<<7)
#define I2C_CR1__ACK  (1UL<<10)
#define I2C_CR1__STOP (1UL<<9)
#define I2C_CR1__RxNE (1UL<<6)
#define I2C_SR1__BTF   (1UL<<2)

/* ***************************************************************************************************************************************************************
 * Explanation: info taken from  STM32F4XX Datasheet block diagram: there exist 3 I2Cs on the MCU
 * 		I2C1, I2C2 and I2C3 and all are connected to the APB1 bus
 * We are going to be using I2C1 to connect the accelerometer sensor to the board
 * *************************************************************************************************************************************************************** */
void i2c1_init(void)
{

	/* *************************************************************************************************************************************************************
	 * Explanation: info taken from  STM32F4XX Datasheet : Alternate Function mapping table
	 * 		We observe that the function I2C1_SCL(Serial clock line) can be associated to PB8, and the function I2C1_SDA(serial data line) can be associated to PB9
	 * 		For both pins, the alternate mode is AF4
	 * ************************************************************************************************************************************************************* */
	/* Choose the GPIO pins for the SPL and SPA lines */

	/* ****************************************************************************************************************************************************************
	 * Explanation: info taken from RM0090: RCC AHB1 peripheral clock register (RCC_AHB1ENR)
	 * 		We are interested in bit 1 GPIOBEN: IO port B clock enable
	 * 		This bit is set and cleared by software. 0: IO port B clock disabled, 1: IO port B clock enabled
	 * **************************************************************************************************************************************************************** */
	/* Enable clock access to GPIOB */
	RCC->AHB1ENR |= RCC_AHB1ENR__GPIOB;

	/* ****************************************************************************************************************************************************************
	 * Explanation: info taken from RM0090:GPIO port mode register (GPIOx_MODER) (x = A..I/J/K)
	 * 		We are interested in the bits 19,18 (port 9) and bits 17,16 (port 8)
	 *		These bits are written by software to configure the I/O direction mode.
	 *		...
	 *		10: Alternate function mode
	 *		...
	 * **************************************************************************************************************************************************************** */
	/* Set PB8 and PB9 modes to alternate functions */
	GPIOB->MODER |=(1UL<<19); //'1' port 9
	GPIOB->MODER &= ~(1UL<<18); //'0' port 9

	GPIOB->MODER |=(1UL<<17); //'1' port 8
	GPIOB->MODER &= ~(1UL<<16); //'0' port 8


	/* ****************************************************************************************************************************************************************
	 * Explanation: info taken from RM0090: GPIO port output type register (GPIOx_OTYPER) (x = A..I/J/K)
	 * 		We are interested in the bits 9 and 8
	 * 		OTy: Port x configuration bits (y = 0..15)
	 * 			These bits are written by software to configure the output type of the I/O port.
	 * 			0: Output push-pull (reset state)
	 * 			1: Output open-drain
	 * **************************************************************************************************************************************************************** */
	/* Set PB8 and PB9 output type to open drain */
	GPIOB->OTYPER |=(1UL<<9); //'1'
	GPIOB->OTYPER |=(1UL<<8);//'1' the rest of the bits are by defaut '0' since the reset value is 0x0 for this registry



	/* ****************************************************************************************************************************************************************
	 * Explanation: info taken from RM0090: GPIO port pull-up/pull-down register (GPIOx_PUPDR) (x = A..I/J/K)
	 *We are going to pay attention to the bits 19,18 (Port 9) and bits 17,16 (Port 8)
	 *		PUPDRy[1:0]: Port x configuration bits (y = 0..15)
	 *			These bits are written by software to configure the I/O pull-up or pull-down
	 *				00: No pull-up, pull-down
	 *				01: Pull-up
	 *				10: Pull-down
	 *				11: Reserved
	 * **************************************************************************************************************************************************************** */
	/* Enable the pull-ups for PB8 and PB9 */
	GPIOB->PUPDR &=~(1UL<<19); //'0' port 9
	GPIOB->PUPDR |= (1UL<<18); //'1' port 9

	GPIOB->PUPDR &=~(1UL<<17); //'0' port 8
	GPIOB->PUPDR |= (1UL<<16); //'1' port 9

	/* ******************************************************************************************************************************************************************
	 * Explanation: info taken from RM0090: RCC APB1 peripheral clock enable register (RCC_APB1ENR)
	 * 	We are interested in the bit no.21 I2C1EN: I2C1 clock enable
	 * 		This bit is set and cleared by software. 0: I2C1 clock disabled. 1: I2C1 clock enabled
	 * ***************************************************************************************************************************************************************** */
	/* Enable clock access to I2C1 */
	RCC->APB1ENR |= RCC_APB1ENR__I2C1;

	/* ******************************************************************************************************************************************************************
	 * Explanation: info taken from RM0090: I2C Control register 1 (I2C_CR1)
	 * We are interested in the bit no. 15 SWRST: Software reset as it says:
	 *		When set, the I2C is under reset state. Before resetting this bit, make sure the I2C lines are released and the bus is free.
	 * 			0: I2C Peripheral not under reset, 	1: I2C Peripheral under reset state
	 * ***************************************************************************************************************************************************************** */
	/* Enter reset mode */
	I2C1->CR1 |=(1UL<<15);

	/* Revert from reset mode */
	I2C1->CR1 &=~(1UL<<15);

	/* ******************************************************************************************************************************************************************
	 * Explanation: info taken from RM0090: I2C Control register 2 (I2C_CR2)
	 * We are going to be interested in bits 5..0 FREQ[5:0]: Peripheral clock frequency
	 * 		The FREQ bits must be configured with the APB clock frequency value (I2C peripheral connected to APB).
	 *		The minimum allowed frequency is 2 MHz, the maximum frequency is limited by the maximum APB frequency and cannot exceed 50 MHz (peripheral intrinsic maximum limit).
	 * 			0b000000: Not allowed
	 * 			0b000001: Not allowed
	 * 			0b000010: 2 MHz
	 * 			...
	 * 			0b110010: 50 MHz
	 * 			Higher than 0b101010: Not allowed
	 * 			So 0b(binary_representation_of_decimal Mhz)
	 * ***************************************************************************************************************************************************************** */
	/* Set the peripheral clock frequency */
	I2C1->CR2 |=(1UL<<4); //'010000' 16Mhz

	/* ********************************************************************************************************************************************************************
	 * Explanation: info taken from RM0090: I2C Clock control register (I2C_CCR)
	 * We are interested in bits 11..0 CCR[11:0]: Clock control register in Fm/Sm mode (Master mode)
	 * ******************************************************************************************************************************************************************* */
	/* Set I2C to standard mode, 100Khz clock */
	I2C1->CCR = I2C_100Khz;

	/* **********************************************************************************************************************************************************************
	 * Explanation: info taken from RM0090: I2C TRISE register (I2C_TRISE)
	 * We are interested in bits 5..0 TRISE[5:0]: Maximum rise time in Fm/Sm mode (Master mode)
	 * ********************************************************************************************************************************************************************* */
	/* Set the rise time */
	I2C1->TRISE =SD_MODE_MAX_RISE_TIME;

	/* **********************************************************************************************************************************************************************
	 * Explanation: info taken from RM0090: I2C Control register 1 (I2C_CR1)
	 * We are interested in bit no. 0 PE: Peripheral enable
	 * 		0: Peripheral disable
	 * 		1: Peripheral enable
	 * ********************************************************************************************************************************************************************* */
	/* Enable the I2C1 module */
	I2C1->CR1 |=I2C_CR1__PE;
}

void i2c1_ByteRead(char slave_address, char memory_address, char *data)
{
	volatile int temporary_var;

	/* ************************************************************************************************************************************************************************
	 * Explanation: info taken from RM0090:I2C Status register 2 (I2C_SR2)
	 * We are interested in bit no.1 BUSY: Bus busy as it says
	 * 		0: No communication on the bus
	 * 		1: Communication ongoing on the bus
	 * ************************************************************************************************************************************************************************ */
	/* Check if the I2C1 Status if it is busy */
	while(I2C1->SR2 & I2C_SR2__BUSY);



	/* ************************************************************************************************************************************************************************
	 * Explanation: info taken from RM0090:I2C Control register 1 (I2C_CR1)
	 * We are interested in bit no.8 START: Start generation
	 * 		This bit is set and cleared by software and cleared by hardware when start is sent or PE=0.
	 * 		In Master Mode:
	 * 		0: No Start generation
	 *		1: Repeated start generation
			In Slave mode:
			0: No Start generation
			1: Start generation when the bus is free
	 * ************************************************************************************************************************************************************************ */
	/* Generate a START Condition */
	I2C1->CR1 |=I2C_CR1__START;

	/* ************************************************************************************************************************************************************************
		 * Explanation: info taken from RM0090: I2C Status register 1 (I2C_SR1)
		 * We are interested in bit no.0 SB: Start bit (Master mode) as it says
		 * 		0: No Start condition
		 * 		1: Start condition generated.
		 * ************************************************************************************************************************************************************************ */
	/* Wait until the START flag is set */
	while(!(I2C1->SR1 & I2C_SR1__SB));

	/* ************************************************************************************************************************************************************************
		 * Explanation: info taken from RM0090: I2C Data register (I2C_DR)
		 * We need the bits 7..0 DR[7:0] 8-bit data register
		 * ************************************************************************************************************************************************************************ */
	/* Transmit the address of the slave and write */
	I2C1->DR = slave_address<<1;

	/* ************************************************************************************************************************************************************************
	* Explanation: info taken from RM0090:I2C Status register 1 (I2C_SR1)
	* We are interested in bit no. 1 of this registry ADDR: Address sent (master mode)/matched (slave mode)
	* 		This bit is cleared by software reading SR1 register followed reading SR2, or by hardware when PE=0.
	* 			Address matched (Slave). 0: Address mismatched or not received. 1: Received address matched.
	* ************************************************************************************************************************************************************************ */
	/* Wait until the address flag is set */
	while(!(I2C1->SR1 & I2C_SR1__ADDR));

	/* Clear the address flag */
	temporary_var=I2C1->SR2;

	/* Send the memory address */
	I2C1->DR=memory_address;

	/* ************************************************************************************************************************************************************************
	* Explanation: info taken from RM0090:I2C Status register 1 (I2C_SR1)
	* We are interested in the bit no. 7 TxE: Data register empty (transmitters)
	* 	0: Data register not empty
	* 	1: Data register empty
	* ************************************************************************************************************************************************************************ */
	/* Wait until the data registry/transmitter is empty */
	while(!(I2C1->SR1 & I2C_SR1__TxE));




	/* ************************************************************************************************************************************************************************
	 * Explanation: info taken from RM0090:I2C Control register 1 (I2C_CR1)
	 * We are interested in bit no.8 START: Start generation
	 * 		This bit is set and cleared by software and cleared by hardware when start is sent or PE=0.
	 * 		In Master Mode:
	 * 		0: No Start generation
	 *		1: Repeated start generation
			In Slave mode:
			0: No Start generation
			1: Start generation when the bus is free
	 * ************************************************************************************************************************************************************************ */
	/* Generate a restart condition */
	I2C1->CR1 |=I2C_CR1__START;



	/* ************************************************************************************************************************************************************************
		 * Explanation: info taken from RM0090: I2C Status register 1 (I2C_SR1)
		 * We are interested in bit no.0 SB: Start bit (Master mode) as it says
		 * 		0: No Start condition
		 * 		1: Start condition generated.
		 * ************************************************************************************************************************************************************************ */
	/* Wait again until the START flag is set */
	while(!(I2C1->SR1 & I2C_SR1__SB));

	/* Transmit slave address + read */
	I2C1->DR = slave_address<<1 |1;


	/* ************************************************************************************************************************************************************************
	* Explanation: info taken from RM0090:I2C Status register 1 (I2C_SR1)
	* We are interested in bit no. 1 of this registry ADDR: Address sent (master mode)/matched (slave mode)
	* 		This bit is cleared by software reading SR1 register followed reading SR2, or by hardware when PE=0.
	* 			Address matched (Slave). 0: Address mismatched or not received. 1: Received address matched.
	* ************************************************************************************************************************************************************************ */
	/* Wait until the address flag is set */
	while(!(I2C1->SR1 & I2C_SR1__ADDR));


	/* ************************************************************************************************************************************************************************
	* Explanation: info taken from RM0090: I2C Control register 1 (I2C_CR1)
	* We are interested in bit no. 10 ACK: Acknowledge enable
			This bit is set and cleared by software and cleared by hardware when PE=0.
			0: No acknowledge returned
			1: Acknowledge returned after a byte is received (matched address or data)
	* ************************************************************************************************************************************************************************ */
	/* Disable Acknowledge */
	I2C1->CR1 &=~I2C_CR1__ACK;

	/* Clear the address flag */
	temporary_var=I2C1->SR2;

	/* ************************************************************************************************************************************************************************
	 * Explanation: info taken from RM0090: I2C Control register 1 (I2C_CR1)
	 * We are interested in bit no. 9 STOP: Stop generation
	 * The bit is set and cleared by software, cleared by hardware when a Stop condition is detected, set by hardware when a timeout error is detected.
	 * In Master Mode:
	 * 		0: No Stop generation.
	 * 		1: Stop generation after the current byte transfer or after the current Start condition is sent.
	 * In Slave mode:
	 * 		0: No Stop generation.
	 * 		1: Release the SCL and SDA lines after the current byte transfer.
	 * ************************************************************************************************************************************************************************ */
	/* Generate a STOP condition after data has been received */
	I2C1->CR1 |=I2C_CR1__STOP;


	/* *************************************************************************************************************************************************************************
	 * Explanation: info taken from RM0090: I2C Status register 1 (I2C_SR1)
	 * We are interested in bit no.6 RxNE: Data register not empty (receivers)
	 * 	0: Data register empty
		1: Data register not empty
	 * ************************************************************************************************************************************************************************* */
	/* Wait until RXNE flag is set */
	while(!(I2C1->SR1 & I2C_CR1__RxNE));

	/* Read the data from DR */
	*data++=I2C1->DR;
}

void i2c1_BurstRead(char slave_address, char memory_address, int number_bytes_to_read, char *data)
{
	volatile int temporary;

	/* Wait until bus is not busy */
	while(I2C1->SR2 & I2C_SR2__BUSY);

	/* Generate a START condition */
	I2C1->CR1 |=I2C_CR1__START;

	/* Wait for the START bit to be set */
	while(!(I2C1->SR1 & I2C_SR1__SB));

	/* Transmit the address of the slave and write */
	I2C1->DR = slave_address<<1;

	/* Wait until address flag is set */
	while(!(I2C1->SR1 & I2C_SR1__ADDR));

	/* Clear address flag */
	temporary=I2C1->SR2;

	/* Wait until transmitter is empty */
	while(!(I2C1->SR1 & I2C_SR1__TxE));

	/* Send memory address */
	I2C1->DR=memory_address;

	/* Wait for the transmitter to be empty again */
	while(!(I2C1->SR1 & I2C_SR1__TxE));

	/* Send a restart condition */
	I2C1->CR1 |=I2C_CR1__START;

	/* Wait for the start bit to be set */
	while(!(I2C1->SR1 & I2C_SR1__SB));

	/* Transmit the slave address + read */
	I2C1->DR = slave_address<<1 |1;

	/* Wait until address flag is set */
	while(!(I2C1->SR1 & I2C_SR1__ADDR));

	/* 	Clear the address flag */
	temporary=I2C1->SR2;

	/* Enable acknowledge */
	I2C1->CR1 |=I2C_CR1__ACK;

	while(number_bytes_to_read>0)
	{
		/* Check if number_bytes_to_read = 1 */
		if(number_bytes_to_read==1)
		{
			/* Disable the Acknowledge */
			I2C1->CR1 &=~I2C_CR1__ACK;

			/* Generate Stop condition */
			I2C1->CR1 |=I2C_CR1__STOP;

			/* Wait until RXNE flag set */
			while(!(I2C1->SR1 & I2C_CR1__RxNE));

			/* Read the data from DR */
			*data++=I2C1->DR;

			break;
		}
		else
		{
			/* Wait for the data to arrive = Wait until RXNE flag set */
			while(!(I2C1->SR1 & I2C_CR1__RxNE));

			/* Read the data from DR */
			*data++=I2C1->DR;

			number_bytes_to_read--;
		}
	}

}

void i2c1_BurstWrite(char slave_address, char memory_address, int number_bytes_to_write, char *data)
{
	volatile int temporary;

	/* Wait until the bus is not busy */
	while(I2C1->SR2 & I2C_SR2__BUSY);

	/* Generate a START condition */
	I2C1->CR1 |=I2C_CR1__START;

	/* Wait for the START bit to be set */
	while(!(I2C1->SR1 & I2C_SR1__SB));

	/* Transmit the address of the slave and write */
	I2C1->DR = slave_address<<1;

	/* Wait until address flag is set */
	while(!(I2C1->SR1 & I2C_SR1__ADDR));

	/* 	Clear the address flag */
	temporary=I2C1->SR2;

	/* Wait until the data registry/transmitter is empty */
	while(!(I2C1->SR1 & I2C_SR1__TxE));

	/* Send memory address */
	I2C1->DR=memory_address;

	for(int i=0;i<number_bytes_to_write;i++)
	{
		/* Wait until transmitter is empty */
		while(!(I2C1->SR1 & I2C_SR1__TxE));

		/* Transmit memory address */
		I2C1->DR = *data++;
	}

	/* *************************************************************************************************************************************************************************
		 * Explanation: info taken from RM0090: I2C Status register 1 (I2C_SR1)
		 * We are interested in bit no. 2 BTF: Byte transfer finished
		 * 		0: Data byte transfer not done
				1: Data byte transfer succeeded
	   ************************************************************************************************************************************************************************* */
	/* Wait until the transfer is finished */
	while(!(I2C1->SR1 & I2C_SR1__BTF));

}
