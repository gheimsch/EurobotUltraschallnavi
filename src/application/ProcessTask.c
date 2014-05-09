/******************************************************************************/
/*! \file ProcessTask.c
 ******************************************************************************
 * \brief Filters the distances out of the UART Message
 *
 * Function : gets the distance as well as the Tag and the Receiver ID from the
 * 				UART Task, and reads out the measured distances from every Robot
 * 				to the three Tags. If all three Tags are received, they are send
 * 				to the Position Task
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
xQueueHandle msgqRobo1;						/* Message Queue to Position Task with own Robot data */
xQueueHandle msgqRobo2;						/* Message Queue to Position Task with confederate Robot data */
xQueueHandle msgqEnemy1;					/* Message Queue to Position Task with first Enemy data */
xQueueHandle msgqEnemy2;					/* Message Queue to Position Task with second Enemy data */
xQueueHandle msgqProcessRFGComm;			/* Message Queue to RFCommunication Task */
xQueueSetHandle msgqSetProcessPosition;		/* Queue Set for all Message Queues to the Position Task */

Position Robo1Pos;		/* Position data of own Robot */
Position Robo2Pos;		/* Position data of confederate Robot */
Position Enemy1Pos;		/* Position of first Enemy Robot */
Position Enemy2Pos;		/* Position of second Enemy Robot */

/* ID's of the Tags */
char Tag1[] = "P20";
char Tag2[] = "P21";
char Tag3[] = "P22";

/* ID's of the Receivers of own Robots */
#ifndef SET_ROBO_BIG	/* if the small Robot is activated */
char Receiver1[] = "R42";
char Receiver2[] = "R43";
#else	/* if the big robot is activated */
static char Receiver1[] = "R43";
static char Receiver2[] = "R42";

#endif
/* ID's of the Receivers of Enemy Robots */
char Receiver3[] = "R40";
char Receiver4[] = "R41";

/* Receive Buffer for UART Message Queue*/
char UARTMsg[UARTBUFFERSIZE];

/* Number of Enemys and Confederate Robots */
uint8_t nbrEnemys = 0;
uint8_t nbrConfederate = 0;

/* ----------------------- module procedure declaration ----------------------*/
void initProcessTask(void);
void getrad(char *RecID, char *TagID, uint32_t *rad );
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
void getrad(char *RecID, char *TagID, uint32_t *rad ) {

	/* initalise used Parameters */
	volatile unsigned char i = 0;
	char value[4] = { 0, 0, 0, 0 };
	uint32_t valueret = 0;
	unsigned char Msgpos;
	char Filterstr[9];		/* Messagefilter */

	/* create the Filterstring */
	strcpy(Filterstr, RecID);
	strcat(Filterstr, " ");
	strcat(Filterstr, TagID);
	strcat(Filterstr, " ");
	strcat(Filterstr, "A");

	taskENTER_CRITICAL();
	/* check if Filterstring is in the UARTMsg */
	if (strstr(UARTMsg, Filterstr) != NULL ) {
		/* read out the Position of the Distance into the String */
		Msgpos = ((unsigned char) (strstr(UARTMsg, Filterstr) - UARTMsg))
				+ sizeof(Filterstr);

		/* read out the Message as long as there is no space or the end of Buffer isn't reached */
		while ((UARTMsg[Msgpos] != ' ') || (Msgpos <= (UARTBUFFERSIZE - 1))) {

			/* break condition */
			if ((i > 3) || Msgpos > (UARTBUFFERSIZE - 4)) {
				break;
			}
			value[i] = UARTMsg[Msgpos + i];
			i++;
		}
	}
	taskEXIT_CRITICAL();
	/* set back the Filterstring */
	memset(&Filterstr, 0, sizeof(Filterstr));
	/* converte the read out distance into a integer  */
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

		/* waiting for UART Messages */
		xQueueReceive(msgqUARTProcess, &UARTMsg, portMAX_DELAY);

		/* get measured Distance out of the Message */
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

		/* send measured Distances of Robo1 to the Position Task */
		if ((Robo1Pos.r1 != 0) && (Robo1Pos.r2 != 0) && (Robo1Pos.r3 != 0)) {

			xQueueSend(msgqRobo1, &Robo1Pos, 0);

			/* reset the Distances */
			Robo1Pos.r1 = 0;
			Robo1Pos.r2 = 0;
			Robo1Pos.r3 = 0;

		}

		/* send measured Distances of Robo2 to the Position Task */
		if ((Robo2Pos.r1 != 0) && (Robo2Pos.r2 != 0) && (Robo2Pos.r3 != 0) && (nbrConfederate == 1)) {

			xQueueSend(msgqRobo2, &Robo2Pos, 0);

			/* reset the Distances */
			Robo2Pos.r1 = 0;
			Robo2Pos.r2 = 0;
			Robo2Pos.r3 = 0;

		}

		/* send measured Distances of Enemy1 to the Position Task */
		if ((Enemy1Pos.r1 != 0) && (Enemy1Pos.r2 != 0) && (Enemy1Pos.r3 != 0) && (nbrEnemys >= 1)) {

			xQueueSend(msgqEnemy1, &Enemy1Pos, 0);

			/* reset the Distances */
			Enemy1Pos.r1 = 0;
			Enemy1Pos.r2 = 0;
			Enemy1Pos.r3 = 0;

		}
		/* send measured Distances of Enemy2 to the Position Task */
		if ((Enemy2Pos.r1 != 0) && (Enemy2Pos.r2 != 0) && (Enemy2Pos.r3 != 0) && (nbrEnemys == 2)) {

			xQueueSend(msgqEnemy2, &Enemy2Pos, 0);

			/* reset the Distances */
			Enemy2Pos.r1 = 0;
			Enemy2Pos.r2 = 0;
			Enemy2Pos.r3 = 0;

		}

	}
}

/* ****************************************************************************/
/* End : ProcessTask */
/* ****************************************************************************/
