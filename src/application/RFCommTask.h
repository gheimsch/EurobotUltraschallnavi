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
#define BUFFERSIZE 150
#define RFCOMMTASK_NAME			"RFComm Task"					/*!< Detailed description after the member */
#define RFCOMMTASK_STACK_SIZE	configMINIMAL_STACK_SIZE 		/*!< size of the receive and transmit task */
#define RFCOMMTASK_PRIORITY		(configMAX_PRIORITIES - 1UL) 	/*!< priority of the receive and transmit task */

/* ------------------------- module type declaration -------------------------*/

/* ------------------------- module data declaration -------------------------*/

//ID's von Tags und Receivern
static const unsigned char Tag1[] = "P20";
static const unsigned char Tag2[] = "P21";
static const unsigned char Tag3[] = "P22";
static const unsigned char Receiver1[] = "R40";
static const unsigned char Receiver2[] = "R41";
static const unsigned char Receiver3[] = "R42";
static const unsigned char Receiver4[] = "R43";

//Synchronisations String
static const unsigned char SyncString[] = "M&q0:20,21,22 s3 $";
/* ----------------------- module procedure declaration ----------------------*/

extern void initRFCommTask(void);
extern void RFCommTask(void*);
extern void getrad(unsigned char *, unsigned char *, unsigned int *);
extern unsigned short SendRFMsg(const unsigned char *);
/* ****************************************************************************/
/* End Header : RFComm.h												  */
/* ****************************************************************************/
#endif /* __APP_RFCOMM_H_ */
