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
#include "task.h"
#include "semphr.h"
#include "queue.h"

#include "UARTTask.h"
/* ------------------------- module data declaration -------------------------*/
xSemaphoreHandle semaphoreUART;
xQueueHandle msgqUARTProcess;
/* ----------------------- module procedure declaration ----------------------*/
void initUARTTask(void);
static void UARTTask(void* pvParameters);
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

void initUARTTask(void) {

	/* create the task */
	xTaskCreate(UARTTask, (signed char *) UARTTASK_NAME, UARTTASK_STACK_SIZE,
			NULL, UARTTASK_PRIORITY, NULL);

	/* create Semaphore and take it */
	vSemaphoreCreateBinary(semaphoreUART);
	xSemaphoreTake(semaphoreUART, 0);

	/* create Message Queue UART to Process Task */
	msgqUARTProcess =
			xQueueCreate(UARTPROCESS_QUEUE_LENGTH, UARTPROCESS_ITEM_SIZE);

}

/* ****************************************************************************/
/* End : initUARTTask */
/* ****************************************************************************/

/******************************************************************************/
/* Function: UARTTask */
/******************************************************************************/
/*! \brief UART Task
 *
 * \author heimg1, zursr1
 *
 * \version 0.0.1
 *
 * \date 12.04.2014 Function created
 *
 *
 *******************************************************************************/

static void UARTTask(void* pvParameters) {

	//init values

	/* for ever */
	for (;;) {


	}
}

/* ****************************************************************************/
/* End : UARTTask */
/* ****************************************************************************/
