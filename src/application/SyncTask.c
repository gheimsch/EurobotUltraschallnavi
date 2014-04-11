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

#include "SyncTask.h"
/* ------------------------- module data declaration -------------------------*/

/* ----------------------- module procedure declaration ----------------------*/
void initSyncTask(void);
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
}

/* ****************************************************************************/
/* End : initSyncTask */
/* ****************************************************************************/