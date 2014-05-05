#ifndef SYNCTASK_H_
#define SYNCTASK_H_

/******************************************************************************/
/*! \file SyncTask.h
 ******************************************************************************
 * \brief Short description of the files function
 *
 *
 * Function : More detailed description of the files function
 *
 * Procedures : ExportedFunction1()
 * ExportedFunction2()...
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
/* Ultraschallnavi Eurobot 2014 */
/* ****************************************************************************/

/* --------------------------------- imports ---------------------------------*/

/* ----------------------- module constant declaration -----------------------*/
#define SYNCTASK_NAME			"Synchronisation Task"			/*!< Detailed description after the member */
#define SYNCTASK_STACK_SIZE		configMINIMAL_STACK_SIZE 		/*!< size of the receive and transmit task */
#define SYNCTASK_PRIORITY		(configMAX_PRIORITIES - 1UL) 	/*!< priority of the receive and transmit task */
/* ------------------------- module type declaration -------------------------*/

/* ------------------------- module data declaration -------------------------*/
//Synchronization String
static const char SyncString[] = "M&q0:20,21,22 s3 $";
/* ----------------------- module procedure declaration ----------------------*/
extern void initSyncTask(void);
/* ****************************************************************************/
/* End Header : SyncTask.h */
/* ****************************************************************************/

#endif /* SYNCTASK_H_ */
