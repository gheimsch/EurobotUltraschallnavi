#ifndef __APP_DEFAULT_TASK_H_
#define __APP_DEFAULT_TASK_H_
/******************************************************************************/
/*! \file default_task.h
******************************************************************************
* \brief Short description of the files function
*
*
* Function : More detailed description of the files function
*
* Procedures : InitDefaultTask()
*
* \author Name
*
* \version 0.0.1
*
* \history dd.mm.jjjj File Created
*
*
* \ingroup <group name> [<group name 2> <group name 3>]
*
* \todo If u have some todo's for the h-file, add it here
*
*/
/* ****************************************************************************/
/* Motorcontroller Eurobot 2014												  */
/* ****************************************************************************/

/* --------------------------------- imports ---------------------------------*/


/* ----------------------- module constant declaration -----------------------*/

/* default-task RTOS configuration */
#define DEFAULT_TASK_NAME		"Default Task"					/*!< Detailed description after the member */
#define TEST_QUEUE_LENGTH		20 								/*!< Size of the message queues */
#define DEFAULT_STACK_SIZE		configMINIMAL_STACK_SIZE 		/*!< size of the receive and transmit task */
#define DEFAULT_TASK_PRIORITY	(configMAX_PRIORITIES - 1UL) 	/*!< priority of the receive and transmit task */


/* ------------------------- module type declaration -------------------------*/


/* ------------------------- module data declaration -------------------------*/


/* ----------------------- module procedure declaration ----------------------*/

extern void InitDefaultTask(void);

/* ****************************************************************************/
/* End Header : default_task.h												  */
/* ****************************************************************************/
#endif /* __APP_DEFAULT_TASK_H_ */
