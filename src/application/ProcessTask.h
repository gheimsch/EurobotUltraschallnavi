#ifndef PROCESSTASK_H_
#define PROCESSTASK_H_

/******************************************************************************/
/*! \file ProcessTask.h
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
 * \ingroup <group name> [<group name 2> <group name 3>]
 *
 * \todo If u have some todo's for the h-file, add it here
 *
 */
/* ****************************************************************************/
/* Ultraschallnavi Eurobot 2014 */
/* ****************************************************************************/

/* --------------------------------- imports ---------------------------------*/
#define PROCESSTASK_NAME			"Gyro Task"					/*!< Detailed description after the member */
#define PROCESSTASK_STACK_SIZE		configMINIMAL_STACK_SIZE 		/*!< size of the receive and transmit task */
#define PROCESSTASK_PRIORITY		(configMAX_PRIORITIES - 1UL) 	/*!< priority of the receive and transmit task */

#define QUEUE_LENGTH_1		( 5 )		/*!< Length of the Queue Receiver 1 */
#define QUEUE_LENGTH_2		( 5 )		/*!< Length of the Queue Receiver 2 */
#define QUEUE_LENGTH_3		( 5 )		/*!< Length of the Queue Receiver 3 */
#define QUEUE_LENGTH_4		( 5 )		/*!< Length of the Queue Receiver 4 */
#define PROCESSRFCOMM_QUEUE_LENGTH		( 5 )		/*!< Length of the Queue Process to RFComm */

#define ITEM_SIZE_QUEUE_1	( sizeof(Position) )	/*!< Item size of the Elements in the Message Queue */
#define ITEM_SIZE_QUEUE_2	( sizeof(Position) )	/*!< Item size of the Elements in the Message Queue */
#define ITEM_SIZE_QUEUE_3	( sizeof(Position) )	/*!< Item size of the Elements in the Message Queue */
#define ITEM_SIZE_QUEUE_4	( sizeof(Position) )	/*!< Item size of the Elements in the Message Queue */
#define PROCESSRFCOMM_ITEM_SIZE	( sizeof(Position) )	/*!< Item size of the Elements in the Message Queue */

#define COMBINED_LENGTH		( QUEUE_LENGTH_1 + QUEUE_LENGTH_2 + QUEUE_LENGTH_3 + QUEUE_LENGTH_4 )	/*!< Length of the combined Queues in the QueueSet */

#define BUFFERSIZE ( 50 )
/* ----------------------- module constant declaration -----------------------*/

/* ------------------------- module type declaration -------------------------*/
//Robo Position struct
typedef struct Position {
	unsigned int x;
	unsigned int y;
	unsigned int r1;
	unsigned int r2;
	unsigned int r3;
	unsigned int angle;
} Position;
/* ------------------------- module data declaration -------------------------*/
extern xQueueHandle msgqRobo1;
extern xQueueHandle msgqRobo2;
extern xQueueHandle msgqEnemy1;
extern xQueueHandle msgqEnemy2;
extern xQueueHandle msgqProcessRFGComm;
extern xQueueSetHandle msgqSetProcessPosition;

extern uint8_t nbrEnemys;
extern uint8_t nbrConfederate;
/* ----------------------- module procedure declaration ----------------------*/
extern void initProcessTask(void);
/* ****************************************************************************/
/* End Header :ProcessTask.h */
/* ****************************************************************************/

#endif /* PROCESSTASK_H_ */
