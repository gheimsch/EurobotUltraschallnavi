/**
 ******************************************************************************
 * \file	SyncTask.c
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
#include "queue.h"

#include "PositionTask.h"
#include "ProcessTask.h"
#include "SyncTask.h"
#include "CANGatekeeper.h"
/* ------------------------- module data declaration -------------------------*/

/* ----------------------- module procedure declaration ----------------------*/
void initSyncTask(void);
static void SyncTask(void* pvParameters);
void CheckNaviRequest(uint16_t id, CAN_data_t* data);
void SetConfiguration(uint16_t id, CAN_data_t* data);
void EmergenyStopActivated(uint16_t id, CAN_data_t* data);
/* ****************************************************************************/
/* End Header : SyncTask.c */
/* ****************************************************************************/

/******************************************************************************/
/* Function: initSyncTask */
/******************************************************************************/
/*! \brief initialise the synchronisation Task
 *
 * \author zursr1
 *
 * \version 0.0.1
 *
 * \date 10.04.2014 Function created
 *
 *
 *******************************************************************************/

void initSyncTask(void) {

	/* create the task */
	xTaskCreate(SyncTask, (signed char *) SYNCTASK_NAME, SYNCTASK_STACK_SIZE,
			NULL, SYNCTASK_PRIORITY, NULL);

	/* set the Check Navi Request Listener */
	setFunctionCANListener(CheckNaviRequest, CHECK_NAVI_REQUEST);

	/* set the Start Configuration Set Listener */
	setFunctionCANListener(SetConfiguration, START_CONFIGURATION_SET);

	/* set the Emergency Stop Listener */
	setFunctionCANListener(EmergenyStopActivated, EMERGENCY_STOP);
}

/* ****************************************************************************/
/* End : initSyncTask */
/* ****************************************************************************/

/******************************************************************************/
/* Function: SyncTask */
/******************************************************************************/
/*! \brief Sync Task
 *
 * \author heimg1, zursr1
 *
 * \version 0.0.1
 *
 * \date 12.04.2014 Function created
 *
 *
 *******************************************************************************/

static void SyncTask(void* pvParameters) {

	//init values

	/* for ever */
	for (;;) {

		vTaskDelay(100 / portTICK_RATE_MS);
	}
}

/* ****************************************************************************/
/* End : SyncTask */
/* ****************************************************************************/

/******************************************************************************/
/* Function: CheckNaviRequest */
/******************************************************************************/
/*! \brief Answer over CAN-Bus on a "are you alive" Request
 *
 * \param[in] id ID of the Message
 * \param[in] data Data of the Message
 *
 * \author heimg1, zursr1
 *
 * \version 0.0.1
 *
 * \date 15.04.2014 Function created
 *
 *
 *******************************************************************************/

void CheckNaviRequest(uint16_t id, CAN_data_t* data) {

	/* give Response */
	txCheckNaviResponse();
}

/* ****************************************************************************/
/* End : CheckNaviRequest */
/* ****************************************************************************/

/******************************************************************************/
/* Function: SetConfiguration */
/******************************************************************************/
/*! \brief Set the Configuration done on the Menue
 *
 * \param[in] id ID of the Message
 * \param[in] data Data of the Message
 *
 * \author heimg1, zursr1
 *
 * \version 0.0.1
 *
 * \date 15.04.2014 Function created
 *
 *
 *******************************************************************************/

void SetConfiguration(uint16_t id, CAN_data_t* data) {

	/* get the teamcolor */
	team = data->gip_color;

	/* get the Number of Enemys Robots */
	nbrEnemys = data->gip_enemy;

	/* get the Number of confederate Robots */
	nbrConfederate = data->gip_confederate;

	/* Synchronise hexamite */
	//TODO synchronisation Message for hexamite

	/* give Response */
	txStartConfigurationConfirm();
}

/* ****************************************************************************/
/* End : SetConfiguration */
/* ****************************************************************************/

/******************************************************************************/
/* Function: EmergenyStopActivated */
/******************************************************************************/
/*! \brief
 *
 * \param[in] id ID of the Message
 * \param[in] data Data of the Message
 *
 * \author heimg1, zursr1
 *
 * \version 0.0.1
 *
 * \date 15.04.2014 Function created
 *
 *
 *******************************************************************************/

void EmergenyStopActivated(uint16_t id, CAN_data_t* data) {

	/* Emergency Stop on top of Robot activated */

	//do something relevant

}

/* ****************************************************************************/
/* End : EmergenyStopActivated */
/* ****************************************************************************/
