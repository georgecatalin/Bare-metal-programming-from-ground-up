/*
 * uart.c
 *
 *  Created on: 16 Mar 2023
 *  Author: George Calin
 *  Code for: Nucleo 144 family
 */

/* ******************************
 * FOR THE NUCLEO 144 :
  * you need to configure the PD8 and PD9 with UART3.
 * ***************************** */


#include "uart.h"

#define GPIODEN (1UL<<3)
#define UART3EN (1UL<<18)

#define SYSTEM_FREQ (16000000)
#define APB1_CLOCK	SYSTEM_FREQ

#define UART_BAUDRATE (115200)

#define CR1_TE (1UL<<3)
#define CR1_RE	(1UL<<2)

#define CR1_UE (1UL<<13)

#define USART_SRTXE (1UL<<7)
#define SR_RXNE (1UL<<5)

#define USART_CR1TXEIE	(1UL<<7)

#define RCC_AHB1ENR__DMA1ENR (1UL<<21)
#define DMA_SxCR__EN (1UL<<0)
#define DMA_SxCR__CHSEL (1UL<<27)
#define DMA_SxCR__MINC (1UL<<10)
#define DMA_SxCR__DIR (1UL<<6)
#define DMA_SxCR__TCIE (1UL<<4)
#define DMA_SxCR__EN (1UL<<0)
#define USART_CR3__DMAT (1UL<<7)


static uint16_t compute_uart_bd(uint32_t PeriphClock, uint32_t BaudRate);
static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClock, uint32_t BaudRate );

/* Create a new function to handle the specific DMA module and the stream of DMA that refers to UART Tx */
/* ********************************************************************************************************************************************************************************
 * Explanation: The info is taken from RM0090: DMA1 request mapping or DMA2 request mapping, depending upon which DMA module we wish to use
 * In our case we wish to use USART3_TX that works with this Nucleo 144 board, hence we are going to address to Stream 3(on column) and its Channel 4(on row).
 * ********************************************************************************************************************************************************************************
 */
void dma1_stream3_init(uint32_t source, uint32_t destination, uint32_t length)
{
	/* Enable clock access to DMA1 */
	/* *****************************************************************************************************************************************************************************
	 * Explanations: Information taken from STM32Fxx Datasheet: Block Diagram that DMA1 is connected to AHB1
	 * Information taken from RM0090: RCC AHB1 peripheral clock register (RCC_AHB1ENR)
	 * We are interested into the bit 21 of this registry which bit refers to DMA1EN as it says
	 * 		DMA1EN: DMA1 clock enable
	 * 		0: DMA1 clock disabled, 1: DMA1 clock enabled
	 * ******************************************************************************************************************************************************************************
	 */
	RCC->AHB1ENR |= RCC_AHB1ENR__DMA1ENR;

	/* Disable DMA1 Stream3 */
	/* ******************************************************************************************************************************************************************************
	 * Explanation: Info is taken from RM0090: DMA stream x configuration register (DMA_SxCR) (x = 0..7)
	 * We are interested in bit 0 which corresponds to EN: Stream enable / flag stream ready when read low
	 * 		This bit is set and cleared by software.
	 * 		0: Stream disabled, 1: Stream enabled
	 * ******************************************************************************************************************************************************************************
	 */
	DMA1_Stream3->CR &=~DMA_SxCR__EN;

	/* Clear all interrupt flags of Stream 3 */
	/* **********************************************************************************************************************************************************************************
	 * Explanation: Info taken from RM0090: DMA high interrupt flag clear register (DMA_HIFCR) (for streams 4..7) or DMA low interrupt flag clear register (DMA_LIFCR) (for streams 0..3)
	 * In this registry that comprises 32 bits, and we are interested in particular to clear out the bits that refers to stream 3 (so they will be located in DMA_LIFCR
	 * Note: Pay attention to the description of the bits in RM0090, we find out that writing 1 to the bits clears the corresponding flags ( and not writing 0 as one normally would think)
	 * **********************************************************************************************************************************************************************************
	 */
	DMA1->LIFCR |=(1UL<<22); //bit 22 of CFEIF3
	DMA1->LIFCR |=(1UL<<24); //bit 24 of CDMEIF3
	DMA1->LIFCR |=(1UL<<25); //bit 25 of CTEIF3
	DMA1->LIFCR |=(1UL<<26); //bit 26 of CHTIF3
	DMA1->LIFCR |=(1UL<<27); //bit 27 of CTCIF3

	/* Set the destination buffer */
	/* **************************************************************************************************************************************************************************************
	 * Explanation: Info comes from RM0090: DMA stream x peripheral address register (DMA_SxPAR) (x = 0..7)
	 * 		Bits 31:0 PAR[31:0]: Peripheral address
	 * 		in this case the destination is UART which is a peripheral
	 * **************************************************************************************************************************************************************************************
	 */
	DMA1_Stream3->PAR=destination;

	/* Set the source buffer */
	/* **************************************************************************************************************************************************************************************
	 * Explanation: Info taken from RM0090: DMA stream x memory 0 address register (DMA_SxM0AR) (x = 0..7)
	 * 		Bits 31:0 M0A[31:0]: Memory 0 address
	 * 		in this case the source is a memory address
	 * **************************************************************************************************************************************************************************************
	 */
	DMA1_Stream3->M0AR=source;

	/* Set the length */
	/* **************************************************************************************************************************************************************************************
	 * Explanation: Info taken from RM0090:DMA stream x number of data register (DMA_SxNDTR) (x = 0..7)
	 * 		Bits 15:0 NDT[15:0]: Number of data items to transfer
	 * **************************************************************************************************************************************************************************************
	 */
	DMA1_Stream3->NDTR=length;

	/* Select Stream 3 Channel 4 */
	/* *************************************************************************************************************************************************************************************
	 * Explanation: Info taken from RM0090: DMA stream x configuration register (DMA_SxCR) (x = 0..7)
	 * We are going to pay attention to bits 25,26,27 which refer to CHSEL[2:0]: Channel selection
	 * 		...
	 * 		100: channel 4 selected
	 * 		...
	 * ************************************************************************************************************************************************************************************
	 */
	DMA1_Stream3->CR = DMA_SxCR__CHSEL; // we assigned it to erase all other bits and start from scratch

	/* Enable memory increment */
	/* *************************************************************************************************************************************************************************************
	 * Explanation: Info taken from RM0090: DMA stream x configuration register (DMA_SxCR) (x = 0..7)
	 * We are going to pay attention to bit 10 MINC: Memory increment mode
	 * 		This bit is set and cleared by software.
	 * 		0: Memory address pointer is fixed, 1: Memory address pointer is incremented after each data transfer (increment is done according to MSIZE)
	 * 		This bit is protected and can be written only if bit 0 EN is ‘0’.
	 ************************************************************************************************************************************************************************************
	*/
	DMA1_Stream3->CR |=DMA_SxCR__MINC;

	/* Configure transfer direction */
	/* *************************************************************************************************************************************************************************************
	 * Explanation: Info taken from RM0090: DMA stream x configuration register (DMA_SxCR) (x = 0..7)
	 * We are going to be interested in the bits 7 and 6 which make the DIR
	 * 		DIR[1:0]: Data transfer direction. These bits are set and cleared by software.
	 * 		00: Peripheral-to-memory
	 * 		01: Memory-to-peripheral (this is the case of UART TX DMA driver)
	 * 		10: Memory-to-memory
	 * 		11: reserved
	 * 		These bits are protected and can be written only if EN is ‘0’.
	 * *************************************************************************************************************************************************************************************
	 */
	DMA1_Stream3->CR |=DMA_SxCR__DIR;

	/* Enable DMA transfer complete interrupt */
	/* *************************************************************************************************************************************************************************************
	 * Explanation: Info taken from RM0090: DMA stream x configuration register (DMA_SxCR) (x = 0..7)
	 * We are going to be interested in the one of the bits whose description ends in "IE", as it goes from "interrupt enable"
	 * Particularly we are to be interested in bit no 4 which stands for TCIE: Transfer complete interrupt enable
	 * 		This bit is set and cleared by software.
	 * 		0: TC interrupt disabled, 1: TC interrupt enabled
	 * ************************************************************************************************************************************************************************************ */
	DMA1_Stream3->CR |=DMA_SxCR__TCIE;

	/* Enable direct mode and disable FIFO mode*/
	/* ***************************************************************************************************************************************************************************************
	 * Explanation: Info taken from RM0090: DMA stream x FIFO control register (DMA_SxFCR) (x = 0..7)
	 * We need to write 0 to all the bits in this register because this way it will set bit 7 FEIE FIFO error interrupt enable to 0
	 * 		This bit is set and cleared by software. 0: FE interrupt disabled. 1: FE interrupt enabled
	 * 	And simultaneously it will set Bit 2 DMDIS: Direct mode disable to 0
	 * 		This bit is set and cleared by software. It can be set by hardware.
	 * 		0: Direct mode enabled. 1: Direct mode disabled.
	 * 		This bit is protected and can be written only if EN is ‘0’.
	 *  ***************************************************************************************************************************************************************************************
	 */
	DMA1_Stream3->FCR = 0x0;

	/* Enable DMA1 stream 3 */
	/* ****************************************************************************************************************************************************************************************
	 * Explanation: Info taken from RM0090: DMA stream x configuration register (DMA_SxCR) (x = 0..7)
	 * We are going to mind the bit 0 which is EN: Stream enable / flag stream ready when read low
	 * 		This bit is set and cleared by software.
	 * 		0: Stream disabled, 1: Stream enabled
	 * **************************************************************************************************************************************************************************************** */
	 DMA1_Stream3->CR |=DMA_SxCR__EN;

	/* Enable UART3 Transmitter DMA*/
	/* ****************************************************************************************************************************************************************************************
	 * Explanation: Info taken from RM0090: Control register 3 (USART_CR3)
	 * We are going to be interested in bit no. 7 as it refers to DMAT: DMA enable transmitter
	 * 		This bit is set/reset by software
	 * 		1: DMA mode is enabled for transmission. 0: DMA mode is disabled for transmission.
	 * *************************************************************************************************************************************************************************************** */
	 USART3->CR3 |=USART_CR3__DMAT;

	/* DMA Interrupt enable in NVIC*/
	 NVIC_EnableIRQ(DMA1_Stream3_IRQn);

}

void uart3_rxtx_init(void)
{
	/* *** CONFIGURE UART GPIO PIN *** */
	/* Enable clock access to gpioD */
	RCC->AHB1ENR |= GPIODEN;

	/* Set PD8 mode to alternate function mode */
	GPIOD->MODER |=(1UL<<17); // '1'
	GPIOD->MODER &=~(1UL<<16); // '0'

	/* Set PD8 alternate function type to UART_TX(AF7) */
	GPIOD->AFR[1] &=~(1UL<<3); //'0'
	GPIOD->AFR[1] |=(1UL<<2); //'1'
	GPIOD->AFR[1] |=(1UL<<1); //'1'
	GPIOD->AFR[1] |=(1UL<<0);//'1'

	/* Set PD9 mode to alternate function mode */
	GPIOD->MODER |=(1UL<<19); // '1'
	GPIOD->MODER &=~(1UL<<18); // '0'

	/* Set PD9 alternate function type to UART_TX(AF7) */
	GPIOD->AFR[1] &=~(1UL<<7); //'0'
	GPIOD->AFR[1] |=(1UL<<6); //'1'
	GPIOD->AFR[1] |=(1UL<<5); //'1'
	GPIOD->AFR[1] |=(1UL<<4);//'1'



	/* **** Configure UART Module *** */
	/* Enable clock access to UART2 */
	RCC->APB1ENR |= UART3EN;

	/* Configure baudrate */
	uart_set_baudrate(USART3, APB1_CLOCK, UART_BAUDRATE);


	/* Configure the transfer direction */
	USART3->CR1 = (CR1_TE | CR1_RE); // Set for both TX and RX

	/* Enable the UART module */
	USART3->CR1 |= CR1_UE; // |= so to say, write only that particular bit, and leave the others unchanged cause we already set the bit 3 at the previous line of code
}


void uart3_tx_interrupt_init(void)
{
	/* *** CONFIGURE UART GPIO PIN *** */
	/* Enable clock access to gpioD */
	RCC->AHB1ENR |= GPIODEN;

	/* Set PD8 mode to alternate function mode */
	GPIOD->MODER |=(1UL<<17); // '1'
	GPIOD->MODER &=~(1UL<<16); // '0'

	/* Set PD8 alternate function type to UART_TX(AF7) */
	GPIOD->AFR[1] &=~(1UL<<3); //'0'
	GPIOD->AFR[1] |=(1UL<<2); //'1'
	GPIOD->AFR[1] |=(1UL<<1); //'1'
	GPIOD->AFR[1] |=(1UL<<0);//'1'

	/* Set PD9 mode to alternate function mode */
	GPIOD->MODER |=(1UL<<19); // '1'
	GPIOD->MODER &=~(1UL<<18); // '0'

	/* Set PD9 alternate function type to UART_TX(AF7) */
	GPIOD->AFR[1] &=~(1UL<<7); //'0'
	GPIOD->AFR[1] |=(1UL<<6); //'1'
	GPIOD->AFR[1] |=(1UL<<5); //'1'
	GPIOD->AFR[1] |=(1UL<<4);//'1'



	/* **** Configure UART Module *** */
	/* Enable clock access to UART3 */
	RCC->APB1ENR |= UART3EN;

	/* Configure baudrate */
	uart_set_baudrate(USART3, APB1_CLOCK, UART_BAUDRATE);


	/* Configure the transfer direction */
	USART3->CR1 = (CR1_TE | CR1_RE); // Set for both TX and RX

	/* Enable the TXEIE interrupt */
	/* *******************************************************************************************************************************************************************
	 * Explanations: The info is taken from RM0090: Control register 1 (USART_CR1)
	 * In this registry only the bits 0..15 (are used). For the purpose of interrupts we concern
	 *  bit 8 PEIE  PE interrupt enable
	 *  bit 7 TXEIE TXE interrupt enable
	 *  bit 6 TCIE Transmission complete interrupt enable
	 *  bit 5 RXNEIE RXNE interrupt enable
	 *
	 *  More about Bit 7 TXEIE: TXE interrupt enable
	 *  	This bit is set and cleared by software. 0: Interrupt is inhibited, 1: An USART interrupt is generated whenever PE=1 in the USART_SR register
	 * ********************************************************************************************************************************************************************
	 */
	USART3->CR1 |= USART_CR1TXEIE;

	/* Enable UART3 interrupt in NVIC */
	NVIC_EnableIRQ(USART3_IRQn);

	/* Enable the UART module */
	USART3->CR1 |= CR1_UE; // |= so to say, write only that particular bit, and leave the others unchanged cause we already set the bit 3 at the previous line of code
}


void uart3_tx_init(void)
{
	/* *** CONFIGURE UART GPIO PIN *** */
	/* Enable clock access to gpioD */
	RCC->AHB1ENR |= GPIODEN;

	/* Set PD8 mode to alternate function mode */
	GPIOD->MODER |=(1UL<<17); // '1'
	GPIOD->MODER &=~(1UL<<16); // '0'

	/* Set PD8 alternate function type to UART_TX(AF7) */
	GPIOD->AFR[1] &=~(1UL<<3); //'0'
	GPIOD->AFR[1] |=(1UL<<2); //'1'
	GPIOD->AFR[1] |=(1UL<<1); //'1'
	GPIOD->AFR[1] |=(1UL<<0);//'1'


	/* **** Configure UART Module *** */
	/* Enable clock access to UART2 */
	RCC->APB1ENR |= UART3EN;

	/* Configure baudrate */
	uart_set_baudrate(USART3, APB1_CLOCK, UART_BAUDRATE);


	/* Configure the transfer direction */
	USART3->CR1 = CR1_TE; // this overwrites all bits to 0, except the one in the desired position (3) => thus all the parameters of the communication are set as per CR1 bits values

	/* Enable the UART module */
	USART3->CR1 |= CR1_UE; // |= so to say, write only that particular bit, and leave the others unchanged cause we already set the bit 3 at the previous line of code
}



char uart3_read(void)
{
	/* Make sure the receive data register is not empty */
	while(!(USART3->SR & SR_RXNE))


	/* Read data */
	return  USART3->DR;
}

void uart3_write(int charYouWantToWrite)
{
	/* Make sure the transmit data register is empty */
	while(!(USART3->SR &  USART_SRTXE));  //execute this until data is transmitted , then go to the next step

	/* Write to transmit data register */
	USART3->DR = (charYouWantToWrite & 0xFF);
}


static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClock, uint32_t BaudRate )
{
	USARTx->BRR = compute_uart_bd(PeriphClock, BaudRate);
}

static uint16_t compute_uart_bd(uint32_t PeriphClock, uint32_t BaudRate)
{
	return ((PeriphClock + (BaudRate/2UL))/BaudRate);
}

int __io_putchar(int myCharacter)
{
	uart3_write(myCharacter);
	return myCharacter;
}

