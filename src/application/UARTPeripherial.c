/******************************************************************************/
/*! \file UARTTask.c
 ******************************************************************************
 * \brief Short description of the files function
 *
 * Function : More detailed description of the files function
 *
 *
 * \author heimg1, zursr1
 *
 * \version 0.0.1
 *
 * \history 10.04.2014 File Created
 *
 *
 */
/* ****************************************************************************/
/* Ultraschallnavi Eurobot 2014	*/
/* ****************************************************************************/

/* --------------------------------- imports ---------------------------------*/
#include "FreeRTOS.h"
#include "queue.h"

#include "UARTPeripherial.h"
#include <string.h>
#include "../lib/UART.h"	/* Own header include */
/* ------------------------- module data declaration -------------------------*/
xQueueHandle msgqUARTProcess;
char RxMsg[UARTBUFFERSIZE];
unsigned char ctr = 0;
/* ----------------------- module procedure declaration ----------------------*/
void initUARTPeripherial(void);
/* ****************************************************************************/
/* End Header : UARTTask.c */
/* ****************************************************************************/

/******************************************************************************/
/* Function: initUARTTask */
/******************************************************************************/
/*! \brief initialise the UART Task
 *
 * \author zursr1
 *
 * \version 0.0.1
 *
 * \date 10.04.2014 Function created
 *
 *
 *******************************************************************************/

void initUARTPeripherial(void) {

	/* initialise the UART-Interface*/
	initUART();

	/* create Message Queue UART to Process Task */
	msgqUARTProcess =
			xQueueCreate(UARTPROCESS_QUEUE_LENGTH, UARTPROCESS_ITEM_SIZE);

}

/* ****************************************************************************/
/* End : initUARTTask */
/* ****************************************************************************/

/******************************************************************************/
/* Function: UARTISRHandler */
/******************************************************************************/
/*! \brief UART ISR Handler
 *
 * \author heimg1, zursr1
 *
 * \version 0.0.1
 *
 * \date 12.04.2014 Function created
 *
 *
 *******************************************************************************/
void USART1_IRQHandler(void) {

	char rxchar = 0;

	if (USART_GetITStatus(USART1, USART_IT_RXNE ) != RESET) {
		/*Daten in Buffer Kopieren*/
		rxchar = USART_ReceiveData(USART1);
		if((rxchar == 'R') || (ctr != 0)){
			RxMsg[ctr] = rxchar;
			ctr++;
		}
		/*Check if termination is reached or if buffer is full*/
		if ((RxMsg[ctr-1] == '\n') || (ctr >= (sizeof(RxMsg)-1))) {

			xQueueSendFromISR( msgqUARTProcess, &RxMsg, 0);
			ctr = 0;
			/*Wenn Buffer voll alles null setzten*/
			memset(&RxMsg, 0, sizeof(RxMsg));
		}
	}

}
/* ****************************************************************************/
/* End : UARTISRHandler */
/* ****************************************************************************/
