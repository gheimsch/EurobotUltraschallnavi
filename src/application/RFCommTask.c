/******************************************************************************/
/*! \RFCommTask.c
 ******************************************************************************
 * \brief Short description of the files function
 *
 *
 * Function : More detailed description of the files function
 *
 * Procedures : 	vDefaultTask(void*)
 * 				InitDefaultTask()
 *              	function3()...
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
 * \todo If u have some todo's for the c-file, add it here
 *
 * \bug Description of the bug
 *
 */
/* ****************************************************************************/
/* Ultraschallnavi Eurobot 2014												  */
/* ****************************************************************************/

/* --------------------------------- imports ---------------------------------*/

#include <memPoolService.h>		/* Memory pool manager service */
#include "stm32f4xx.h"			/* uC includes */
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "FreeRTOS.h"
#include "task.h"

/* application */
#include "default_task.h"		/* Own header include */
#include "PositionTask.h"			/* Own header include */
#include "RFCommTask.h"				/* Own header include */
#include "../lib/UART.h"		/* Own header include */
#include "ProcessTask.h"

/* ------------------------- module data declaration -------------------------*/

/* ----------------------- module procedure declaration ----------------------*/

void initRFCommTask(void);
static void RFCommTask(void* pvParameters);
unsigned short SendRFMsg(const unsigned char *);
//void USARTPrint(const unsigned char *, unsigned char);
volatile unsigned char i = 0;
/* ****************************************************************************/
/* End Header : GyroTask.c													  */
/* ****************************************************************************/

/******************************************************************************/
/* Function: initRFCommTask */
/******************************************************************************/
/*! \brief initialise the RF Communication Task
 *
 * \author zursr1
 *
 * \version 0.0.1
 *
 * \date 10.04.2014 Function created
 *
 *
 *******************************************************************************/

void initRFCommTask(void) {

	/* create the task */
	xTaskCreate(RFCommTask, (signed char *) RFCOMMTASK_NAME,
			RFCOMMTASK_STACK_SIZE, NULL, RFCOMMTASK_PRIORITY, NULL);
}

/* ****************************************************************************/
/* End : initRFCommTask */
/* ****************************************************************************/

/******************************************************************************/
/* Function: RFCommTask */
/******************************************************************************/
/*! \brief RFComm Task
 *
 * \author heimg1, zursr1
 *
 * \version 0.0.1
 *
 * \date 12.04.2014 Function created
 *
 *
 *******************************************************************************/

static void RFCommTask(void* pvParameters) {

	//init values

	/* for ever */
	for (;;) {


	}
}

/* ****************************************************************************/
/* End : RFCommTask */
/* ****************************************************************************/

void USARTPrint(const unsigned char *ToSend, unsigned char length) {
	unsigned int i;

	/* Output a message  */
	for (i = 0; i < length; i++) {
		USART_SendData(USART1, (uint16_t) *ToSend++);
		/* Loop until the end of transmission */
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC ) == RESET) {
		}
	}
}
/*
unsigned short SendRFMsg(const unsigned char *str) {
	unsigned char len = 0, i = 0, j = 0;
	unsigned int check = 0;
	unsigned char buffer[50] = { 0 };
	unsigned char hx[4] = { 0 };

	if (strlen(str) > 116) {
		return 0;
	} else {
		len = strlen(str);
		for (i = 0; i < len; i++) {
			check += str[i];
		}

		//String zum senden generieren
		strcat(buffer, str);
		strcat(buffer, "/");
		//Hex Checksumme anfügen
		sprintf(hx, "%x", check);
		//Kleinbuchstaben zu Grossbuchstaben wandeln
		while (hx[j] != 0) {
			if (hx[j] >= 97) {
				hx[j] = hx[j] - 32;
			}
			j++;
		}
		strcat(buffer, hx);
		//CR anhängen
		strcat(buffer, "\r");
		strcat(buffer, "\0");
		USARTPrint(buffer, strlen(buffer));
		return 1;
	}
}
*/
