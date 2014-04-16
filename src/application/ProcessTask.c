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
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "ProcessTask.h"
#include "UARTPeripherial.h"
#include "RFCommTask.h"
#include "configNavi.h"
/* ------------------------- module data declaration -------------------------*/
xQueueHandle msgqRobo1;
xQueueHandle msgqRobo2;
xQueueHandle msgqEnemy1;
xQueueHandle msgqEnemy2;
xQueueHandle msgqProcessRFGComm;
xQueueSetHandle msgqSetProcessPosition;

Position Robo1Pos;
Position Robo2Pos;
Position Enemy1Pos;
Position Enemy2Pos;

//ID's von Tags und Receivern
static char Tag1[] = "P20";
static char Tag2[] = "P21";
static char Tag3[] = "P22";

#ifndef SET_ROBO_BIG	// if the small Robot is activated
static char Receiver1[] = "R41";
static char Receiver2[] = "R40";
#else	//if the big robot is activated
static char Receiver1[] = "R41";
static char Receiver2[] = "R42";

#endif
static char Receiver3[] = "R40";
static char Receiver4[] = "R43";

char UARTMsg[UARTBUFFERSIZE];	//Receive Buffer

uint8_t nbrEnemys = 0;
uint8_t nbrConfederate = 0;

/* ----------------------- module procedure declaration ----------------------*/
void initProcessTask(void);
void getrad(char *RecID, char *TagID, unsigned int *rad );
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
	msgqRobo1 = xQueueCreate( QUEUE_LENGTH_1, ITEM_SIZE_QUEUE_1 );
	msgqRobo2 = xQueueCreate( QUEUE_LENGTH_2, ITEM_SIZE_QUEUE_2 );
	msgqEnemy1 = xQueueCreate( QUEUE_LENGTH_3, ITEM_SIZE_QUEUE_3 );
	msgqEnemy2 = xQueueCreate( QUEUE_LENGTH_4, ITEM_SIZE_QUEUE_4 );
	msgqProcessRFGComm =
			xQueueCreate( PROCESSRFCOMM_QUEUE_LENGTH, PROCESSRFCOMM_ITEM_SIZE );

	/* Add the queues to the set */
	xQueueAddToSet(msgqRobo1, msgqSetProcessPosition);
	xQueueAddToSet(msgqRobo2, msgqSetProcessPosition);
	xQueueAddToSet(msgqEnemy1, msgqSetProcessPosition);
	xQueueAddToSet(msgqEnemy2, msgqSetProcessPosition);

}

/* ****************************************************************************/
/* End : initProcessTask */
/* ****************************************************************************/

/******************************************************************************/
/* Function: getrad */
/******************************************************************************/
/*! \brief reads out the Distance from the Receiver to the Tag
 *
 * \param[in] RecID Receiver ID
 * \param[in] TagID Tag ID
 *
 * \author heimg1, zursr1
 *
 * \version 0.0.1
 *
 * \date 14.04.2014 Function created
 *
 *
 *******************************************************************************/
void getrad(char *RecID, char *TagID, unsigned int *rad ) {

	volatile unsigned char i = 0;
	char value[4] = { 0, 0, 0, 0 };
	uint32_t valueret = 0;
	unsigned char Msgpos;
	char Filterstr[9];		//Messagefilter

	//create the Filterstring
	strcpy(Filterstr, RecID);
	strcat(Filterstr, " ");
	strcat(Filterstr, TagID);
	strcat(Filterstr, " ");
	strcat(Filterstr, "A");

	//check if Filterstring is in the UARTMsg
	if (strstr(UARTMsg, Filterstr) != NULL ) {
		//read out the Position of the Distance into the String
		Msgpos = ((unsigned char) (strstr(UARTMsg, Filterstr) - UARTMsg))
				+ sizeof(Filterstr);

		//Solange kein Leerzeichen oder der Buffer fertig ist
		while ((UARTMsg[Msgpos] != ' ') || (Msgpos <= (UARTBUFFERSIZE - 1))) {
			if ((i > 3) || Msgpos > (UARTBUFFERSIZE - 4)) {
				//Bei mehr als drei Zeichen abbrechen
				break;
			}
			value[i] = UARTMsg[Msgpos + i];
			i++;
		}
	}
	//Filterstring zurücksetzten
	memset(&Filterstr, 0, sizeof(Filterstr));
	//Integer Wert der Distanz zurückgeben
	valueret = atoi(value);
	if ((valueret > 10) && (valueret < 3700)) {
		*rad = valueret;
	}

}

/* ****************************************************************************/
/* End : getrad */
/* ****************************************************************************/

/******************************************************************************/
/* Function: ProcessTask */
/******************************************************************************/
/*! \brief Process Task gets the measured Distances out of the Messages send by
 * 		   the UART Task and send the Distances to the Position Task
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


	/* for ever */
	for (;;) {

		// waiting for UART Messages
		xQueueReceive(msgqUARTProcess, &UARTMsg, portMAX_DELAY);

		// get measured Distance out of the Message
		getrad(Receiver1, Tag1, &Robo1Pos.r1);
		getrad(Receiver1, Tag2, &Robo1Pos.r2);
		getrad(Receiver1, Tag3, &Robo1Pos.r3);
		getrad(Receiver2, Tag1, &Robo2Pos.r1);
		getrad(Receiver2, Tag2, &Robo2Pos.r2);
		getrad(Receiver2, Tag3, &Robo2Pos.r3);
		getrad(Receiver3, Tag1, &Enemy1Pos.r1);
		getrad(Receiver3, Tag2, &Enemy1Pos.r2);
		getrad(Receiver3, Tag3, &Enemy1Pos.r3);
		getrad(Receiver4, Tag1, &Enemy2Pos.r1);
		getrad(Receiver4, Tag2, &Enemy2Pos.r2);
		getrad(Receiver4, Tag3, &Enemy2Pos.r3);

		// send measured Distances of Robo1 to the Position Task
		if ((Robo1Pos.r1 != 0) && (Robo1Pos.r2 != 0) && (Robo1Pos.r3 != 0)) {

			xQueueSend(msgqRobo1, &Robo1Pos, 0);

			// reset the Distances
			Robo1Pos.r1 = 0;
			Robo1Pos.r2 = 0;
			Robo1Pos.r3 = 0;

		}
		nbrConfederate = 1;
		// send measured Distances of Robo2 to the Position Task
		if ((Robo2Pos.r1 != 0) && (Robo2Pos.r2 != 0) && (Robo2Pos.r3 != 0) && (nbrConfederate == 1)) {

			xQueueSend(msgqRobo2, &Robo2Pos, 0);

			// reset the Distances
			Robo2Pos.r1 = 0;
			Robo2Pos.r2 = 0;
			Robo2Pos.r3 = 0;

		}
		nbrEnemys = 2;
		// send measured Distances of Enemy1 to the Position Task
		if ((Enemy1Pos.r1 != 0) && (Enemy1Pos.r2 != 0) && (Enemy1Pos.r3 != 0) && (nbrEnemys >= 1)) {

			xQueueSend(msgqEnemy1, &Enemy1Pos, 0);

			// reset the Distances
			Enemy1Pos.r1 = 0;
			Enemy1Pos.r2 = 0;
			Enemy1Pos.r3 = 0;

		}
		// send measured Distances of Enemy2 to the Position Task
		if ((Enemy2Pos.r1 != 0) && (Enemy2Pos.r2 != 0) && (Enemy2Pos.r3 != 0) && (nbrEnemys == 2)) {

			xQueueSend(msgqEnemy2, &Enemy2Pos, 0);

			// reset the Distances
			Enemy2Pos.r1 = 0;
			Enemy2Pos.r2 = 0;
			Enemy2Pos.r3 = 0;

		}

	}
}

/* ****************************************************************************/
/* End : ProcessTask */
/* ****************************************************************************/
