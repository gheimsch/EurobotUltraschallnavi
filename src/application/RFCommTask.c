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
//Receive Buffer
volatile char RxMsg[BUFFERSIZE];
//Messagefilter
unsigned char Filterstr[9];
unsigned char stringout[15];
volatile unsigned char i = 0;

/* ----------------------- module procedure declaration ----------------------*/

void initRFCommTask(void);
static void RFCommTask(void* pvParameters);
void getrad(unsigned char *, unsigned char *, unsigned int *);
unsigned short SendRFMsg(const unsigned char *);
void USARTPrint(const unsigned char *, unsigned char);
void Delay_ms(unsigned int);

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

//USART RX IRQ Handler
void USART1_IRQHandler(void) {
	__disable_irq(); // kein disable nötig Vorsicht, allfällig critical section beim Auslesen (grosi)

	if (USART_GetITStatus(USART1, USART_IT_RXNE ) != RESET) {
		//Daten in Buffer Kopieren
		RxMsg[i] = USART_ReceiveData(USART1 );
		i++;
		if (i >= (sizeof(RxMsg) - 1)) {
			i = 0;
			//Wenn Buffer voll alles null setzten
			memset(&RxMsg, 0, sizeof(RxMsg));
		}
	}
	__enable_irq();
}

void Delay_ms(unsigned int ms) {

	int tms;
	tms = 5000 * ms;
	while (tms--)
		;
}

//Entfernung von Tag zu Receiver auslesen
void getrad(unsigned char *RecID, unsigned char *TagID, unsigned int * rad) {

	volatile unsigned char i = 0;
	unsigned char value[4] = { 0, 0, 0, 0 };
	unsigned int valueret = 0;
	unsigned char Msgpos;
	//Filterstring zusammensetzten
	strcpy(Filterstr, RecID);
	strcat(Filterstr, " ");
	strcat(Filterstr, TagID);
	strcat(Filterstr, " ");
	strcat(Filterstr, "A");

	//Prüfen ob Filterstring im Buffer enthalten ist
	if (strstr(RxMsg, Filterstr) != NULL ) {
		//Position des Strings der Distanz rechnen
		Msgpos = ((unsigned char) (strstr(RxMsg, Filterstr) - RxMsg))
				+ sizeof(Filterstr);
		Delay_ms(1);
		//Solange kein Leerzeichen oder der Buffer fertig ist
		while ((RxMsg[Msgpos] != ' ') || (Msgpos <= (BUFFERSIZE - 1))) {
			if ((i > 3) || Msgpos > (BUFFERSIZE - 4)) {
				//Bei mehr als drei Zeichen abbrechen
				break;
			}
			value[i] = RxMsg[Msgpos + i];
			i++;
		}
	}
	//Filterstring zurücksetzten
	memset(&Filterstr, 0, sizeof(Filterstr));
	//Integer Wert der Distanz zurückgeben
	valueret = atoi(value);
	if ((valueret > 10) && (valueret < 3700)) {
		*rad = valueret;
	}

}

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
