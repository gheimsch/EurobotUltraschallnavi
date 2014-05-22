#ifndef __APP_POSITIONTASK_H_
#define __APP_POSITIONTASK_H_

/******************************************************************************/
/*! \file PositionTask.h
 ******************************************************************************
 * \brief Calculates the Position of the Robots and sends them via CAN to the
 * 			System
 *
 *
 * \author zursr1,heimg1
 *
 * \version 0.0.1
 *
 * \history 25.03.2014 File Created
 *
 *
 */
/* ****************************************************************************/
/*Ultraschallnavi Eurobot 2014												  */
/* ****************************************************************************/

/* --------------------------------- imports ---------------------------------*/

/* ----------------------- module constant declaration -----------------------*/
#define POSITIONTASK_NAME			"Position Task"					/*!< Detailed description after the member */
#define POSITIONTASK_STACK_SIZE		configMINIMAL_STACK_SIZE +70		/*!< size of the receive and transmit task */
#define POSITIONTASK_PRIORITY		(configMAX_PRIORITIES - 1UL) 	/*!< priority of the receive and transmit task */

#define POSITIONCAN_QUEUE_LENGTH	( 3 ) 				/*!< Length of the Message Queue */
#define POSITIONCAN_ITEM_SIZE		( sizeof(long) ) 	/*!< Item size of the Elements in the Message Queue */

/* ------------------------- module type declaration -------------------------*/
enum color {
	yellow = 0, red = 1, tbd  = 2
};
/* ------------------------- module data declaration -------------------------*/
extern uint8_t team;

/* ----------------------- module procedure declaration ----------------------*/
extern void initPositionTask(void);

/* ****************************************************************************/
/* End Header : PositionTask.h												  */
/* ****************************************************************************/
#endif /* __APP_POSITIONTASK_H_ */
