/******************************************************************************/
/*! \UART.c
******************************************************************************
* \brief Short description of the files function
*
*
* Function : More detailed description of the files function
*
* Procedures : 	vDefaultTask(void*)
* 				InitDefaultTask()
*              	function3()...
*
* \author zursr1,heimg1
*
* \version 0.0.1
*
* \history 25.03.2014 File Created
*
*
* \ingroup <group name> [<group name 2> <group name 3>]
*
* \todo If u have some todo's for the c-file, add it here
*
* \bug Description of the bug
*
*/
/* ****************************************************************************/
/* Ultraschallnavi Eurobot 2014												  */
/* ****************************************************************************/

/* --------------------------------- imports ---------------------------------*/
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "misc.h"

/* ------------------------- module data declaration -------------------------*/

/* ----------------------- module procedure declaration ----------------------*/
void initUART(void);
/* ****************************************************************************/
/* End Header : UART.c													  */
/* ****************************************************************************/


void initUART(void){
	  GPIO_InitTypeDef GPIO_InitStructure;
	  USART_InitTypeDef USART_InitStructure;
	  NVIC_InitTypeDef NVIC_InitStructure;

	  /*Enable GPIO and USART clock*/
	  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA,ENABLE);
	  RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1,ENABLE);

	  /*  USART1_TX -> PA9 , USART1_RX ->	PA10*/
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);

	  GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	  GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);

	  /*256kbit/s,8 bit,1 stop,parity none*/
	  USART_InitStructure.USART_BaudRate = 256000;
	  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	  USART_InitStructure.USART_StopBits = USART_StopBits_1;
	  USART_InitStructure.USART_Parity = USART_Parity_No;
	  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	  USART_Init(USART1, &USART_InitStructure);
	  USART_Cmd(USART1, ENABLE);

	  // Enable the USART RX Interrupt
	  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  NVIC_Init(&NVIC_InitStructure);

}
