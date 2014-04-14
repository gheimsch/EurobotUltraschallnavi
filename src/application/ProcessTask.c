/******************************************************************************/
/*! \file ProcessTask.c
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
 * \ingroup <group name> [<group name 2> <group name 3>]
 *
 * \todo If u have some todo's for the c-file, add it here
 *
 */
/* ****************************************************************************/
/* Ultraschallnavi Eurobot 2014	*/
/* ****************************************************************************/

/* --------------------------------- imports ---------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "ProcessTask.h"
#include "UARTTask.h"
/* ------------------------- module data declaration -------------------------*/
xQueueHandle msgqRecv1;
xQueueHandle msgqRecv2;
xQueueHandle msgqRecv3;
xQueueHandle msgqRecv4;
xQueueHandle msgqProcessRFGComm;
xQueueSetHandle msgqSetProcessPosition;
/* ----------------------- module procedure declaration ----------------------*/
void initProcessTask(void);
static void ProcessTask(void* pvParameters);
/* ****************************************************************************/
/* End Header : ProcessTask.c */
/* ****************************************************************************/

/******************************************************************************/
/* Function: initProcessTask */
/******************************************************************************/
/*! \brief initialise the Process Task
 *
 * \author zursr1
 *
 * \version 0.0.1
 *
 * \date 10.04.2014 Function created
 *
 *
 *******************************************************************************/

void initProcessTask(void) {

	/* create the task */
	xTaskCreate(ProcessTask, (signed char *) PROCESSTASK_NAME,
			PROCESSTASK_STACK_SIZE, NULL, PROCESSTASK_PRIORITY, NULL);

	/* Create the queue set */
	msgqSetProcessPosition = xQueueCreateSet(COMBINED_LENGTH);

	/* Create the queues that will be contained in the set. */
	msgqRecv1 = xQueueCreate( QUEUE_LENGTH_1, ITEM_SIZE_QUEUE_1 );
	msgqRecv2 = xQueueCreate( QUEUE_LENGTH_2, ITEM_SIZE_QUEUE_2 );
	msgqRecv3 = xQueueCreate( QUEUE_LENGTH_3, ITEM_SIZE_QUEUE_3 );
	msgqRecv4 = xQueueCreate( QUEUE_LENGTH_4, ITEM_SIZE_QUEUE_4 );
	msgqProcessRFGComm =  xQueueCreate( PROCESSRFCOMM_QUEUE_LENGTH, PROCESSRFCOMM_ITEM_SIZE );

	/* Add the queues to the set */
	xQueueAddToSet(msgqRecv1, msgqSetProcessPosition);
	xQueueAddToSet(msgqRecv2, msgqSetProcessPosition);
	xQueueAddToSet(msgqRecv3, msgqSetProcessPosition);
	xQueueAddToSet(msgqRecv4, msgqSetProcessPosition);

}

/* ****************************************************************************/
/* End : initProcessTask */
/* ****************************************************************************/

/******************************************************************************/
/* Function: ProcessTask */
/******************************************************************************/
/*! \brief Process Task
 *
 * \author heimg1, zursr1
 *
 * \version 0.0.1
 *
 * \date 12.04.2014 Function created
 *
 *
 *******************************************************************************/

static void ProcessTask(void* pvParameters) {

	//init values

	/* for ever */
	for (;;) {



	}
}

/* ****************************************************************************/
/* End : ProcessTask */
/* ****************************************************************************/
