#ifndef __APP_POSITIONTASK_H_
#define __APP_POSITIONTASK_H_

/******************************************************************************/
/*! \file PositionTask.h
 ******************************************************************************
 * \brief Short description of the files function
 *
 *
 * Function : More detailed description of the files function
 *
 * \author zursr1,heimg1
 *
 * \version 0.0.1
 *
 * \history 25.03.2014 File Created
 *
 *
 * \ingroup <group name> [<group name 2> <group name 3>]
 *
 * \todo If u have some todo's for the h-file, add it here
 *
 */
/* ****************************************************************************/
/*Ultraschallnavi Eurobot 2014												  */
/* ****************************************************************************/

/* --------------------------------- imports ---------------------------------*/

/* ----------------------- module constant declaration -----------------------*/
#define POSITIONTASK_NAME			"Position Task"					/*!< Detailed description after the member */
#define POSITIONTASK_STACK_SIZE		configMINIMAL_STACK_SIZE 		/*!< size of the receive and transmit task */
#define POSITIONTASK_PRIORITY		(configMAX_PRIORITIES - 1UL) 	/*!< priority of the receive and transmit task */

#define POSITIONCAN_QUEUE_LENGTH	( 3 ) 				/*!< Length of the Message Queue */
#define POSITIONCAN_ITEM_SIZE		( sizeof(long) ) 	/*!< Item size of the Elements in the Message Queue */
/* ------------------------- module type declaration -------------------------*/
//Robo Position struct
typedef struct Position {
	unsigned int x;
	unsigned int y;
	unsigned int r1;
	unsigned int r2;
	unsigned int r3;
} Position;

/* ------------------------- module data declaration -------------------------*/
extern xQueueHandle msgqPositionCAN;

Position Robo1Pos;
Position Robo2Pos;
Position Enemy1Pos;
Position Enemy2Pos;
/* ----------------------- module procedure declaration ----------------------*/
extern void initPositionTask(void);
extern void PositionTask(void*);
extern void initRFComm(void);
extern void Trilateration2D(int, int, int, int, int, int, int, int, int,
		Position *);

/* ****************************************************************************/
/* End Header : PositionTask.h												  */
/* ****************************************************************************/
#endif /* __APP_POSITIONTASK_H_ */

