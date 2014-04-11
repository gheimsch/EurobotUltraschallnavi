#ifndef UARTTASK_H_
#define UARTTASK_H_

/******************************************************************************/
/*! \file UARTTask.h
 ******************************************************************************
 * \brief Short description of the files function
 *
 *
 * Function : More detailed description of the files function
 *
 * Procedures : ExportedFunction1()
 * ExportedFunction2()...
 *
 * \author zursr1
 *
 * \version 0.0.1
 *
 * \history 10.04.2014 File Created
 *
 *
 */
/* ****************************************************************************/
/* Ultraschallnavi Eurobot 2014 */
/* ****************************************************************************/

/* --------------------------------- imports ---------------------------------*/
#include "semphr.h"
/* ----------------------- module constant declaration -----------------------*/
#define UARTTASK_NAME			"UART Task"						/*!< Detailed description after the member */
#define UARTTASK_STACK_SIZE		configMINIMAL_STACK_SIZE 		/*!< size of the receive and transmit task */
#define UARTTASK_PRIORITY		(configMAX_PRIORITIES - 1UL) 	/*!< priority of the receive and transmit task */

#define UARTPROCESS_QUEUE_LENGTH	( 3 ) 			/*!< Length of the Message Queue */
#define UARTPROCESS_ITEM_SIZE		( sizeof(long) ) 	/*!< Item size of the Elements in the Message Queue */
/* ------------------------- module type declaration -------------------------*/

/* ------------------------- module data declaration -------------------------*/
extern xSemaphoreHandle semaphoreUART;
extern xQueueHandle msgqUARTProcess;
/* ----------------------- module procedure declaration ----------------------*/
extern void initUARTTask(void);
extern void UARTTask(void*);
/* ****************************************************************************/
/* End Header : UARTTask.h */
/* ****************************************************************************/

#endif /* UARTTASK_H_ */
