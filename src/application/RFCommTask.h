#ifndef __APP_RFCOMMTASK_H_
#define __APP_RFCOMMTASK_H_
/******************************************************************************/
/*! \file RFComm.h
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

#define RFCOMMTASK_NAME			"RFComm Task"					/*!< Detailed description after the member */
#define RFCOMMTASK_STACK_SIZE	configMINIMAL_STACK_SIZE 		/*!< size of the receive and transmit task */
#define RFCOMMTASK_PRIORITY		(configMAX_PRIORITIES - 1UL) 	/*!< priority of the receive and transmit task */

#define RFCOMMBUFFERSIZE		35								/*!< size of the RF Buffer */

#define RF_QUEUE_LENGTH			( 10 ) 							/*!< Length of the Message Queue */
#define RF_ITEM_SIZE			( RFCOMMBUFFERSIZE ) 			/*!< Item size of the Elements in the Message Queue */
/* ------------------------- module type declaration -------------------------*/

/* ------------------------- module data declaration -------------------------*/
extern xQueueHandle msgqRFComm;
/* ----------------------- module procedure declaration ----------------------*/

extern void initRFCommTask(void);
extern unsigned short SendRFMsg(const char *);
/* ****************************************************************************/
/* End Header : RFComm.h												  */
/* ****************************************************************************/
#endif /* __APP_RFCOMM_H_ */
