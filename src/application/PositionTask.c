/**
 ******************************************************************************
 * \file	PositionTask.c
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
#include "queue.h"
#include "task.h"

/* application */
#include "default_task.h"		/* Own header include */
#include "PositionTask.h"			/* Own header include */
#include "ProcessTask.h"
#include "GyroTask.h"

/* ------------------------- module data declaration -------------------------*/
xQueueHandle msgqPositionCAN;
/* ----------------------- module procedure declaration ----------------------*/

void initPositionTask(void);
void Trilateration2D(int, int, int, int, int, int, int, int, int, Position *);
static void PositionTask(void* pvParameters);

/* ****************************************************************************/
/* End Header : Trilateration.c												  */
/* ****************************************************************************/

/******************************************************************************/
/* Function: initPositionTask */
/******************************************************************************/
/*! \brief initialise the Position Task
 *
 * \author zursr1
 *
 * \version 0.0.1
 *
 * \date 10.04.2014 Function created
 *
 *
 *******************************************************************************/

void initPositionTask(void) {

	/* create the task */
	xTaskCreate(PositionTask, (signed char *) POSITIONTASK_NAME,
			POSITIONTASK_STACK_SIZE, NULL, POSITIONTASK_PRIORITY, NULL);

	/* create Message Queue Position to CAN Task */
	msgqPositionCAN =
			xQueueCreate(POSITIONCAN_QUEUE_LENGTH, POSITIONCAN_ITEM_SIZE);
}

/* ****************************************************************************/
/* End : initPositionTask */
/* ****************************************************************************/

//// Eigenimplementation von itoa(int to ascii)
//void itoa( int z, char* Buffer ){
//  int i = 0;
//  int j;
//  char tmp;
//  unsigned u;
//
//    //Vorzeichen berücksichtigen
//    if( z < 0 ) {
//      Buffer[0] = '-';
//      Buffer++;
//      u = ( (unsigned)-(z+1) ) + 1;
//    }
//    else {
//      u = (unsigned)z;
//    }
//    //Konvertieren der Zeichen
//    do {
//      Buffer[i++] = '0' + u % 10;
//      u /= 10;
//    } while( u > 0 );
//
//    //Zusammensetzen und spiegeln
//    for( j = 0; j < i / 2; ++j ) {
//      tmp = Buffer[j];
//      Buffer[j] = Buffer[i-j-1];
//      Buffer[i-j-1] = tmp;
//    }
//    Buffer[i] = '\0';
//}
/******************************************************************************/
/* Function: Trilateration2D */
/******************************************************************************/
/*! \brief calculates the position of the robot by triangulation
 *
 * Function : More detailed description of the function
 *
 * \param[in] x1
 * \param[in] x2
 * \param[in] x3
 * \param[in] y1
 * \param[in] y2
 * \param[in] y3
 * \param[in] r1
 * \param[in] r2
 * \param[in] r3
 * \param[in] pos
 *
 * \author heimg1
 *
 * \version 0.0.1
 *
 * \date 10.04.2014 Function created
 *
 * \todo Comment parameters better
 *
 *******************************************************************************/

void Trilateration2D(int x1, int x2, int x3, int y1, int y2, int y3, int r1,
		int r2, int r3, Position *pos) {

	__disable_irq();
	long xn[2][2];
	long yn[2][2];
	double d[2][2];
	double x[2][2] = { { 0, 0 }, { 0, 0 } };
	double y[2][2] = { { 0, 0 }, { 0, 0 } };
	long detd = 0;
	double tmp = 0;

	xn[0][0] = (int) (pow(r1, 2) - pow(r2, 2)) - (pow(x1, 2) - pow(x2, 2))
			- (pow(y1, 2) - pow(y2, 2));
	xn[1][0] = (int) (pow(r1, 2) - pow(r3, 2)) - (pow(x1, 2) - pow(x3, 2))
			- (pow(y1, 2) - pow(y3, 2));
	xn[0][1] = 2 * (y2 - y1);
	xn[1][1] = 2 * (y3 - y1);

	d[0][0] = 2 * (x2 - x1);
	d[1][0] = 2 * (x3 - x1);
	d[0][1] = 2 * (y2 - y1);
	d[1][1] = 2 * (y3 - y1);

	yn[0][0] = d[0][0];
	yn[1][0] = d[1][0];
	yn[0][1] = xn[0][0];
	yn[1][1] = xn[1][0];

	//Inverse der 2D-Matrix d
	detd = d[0][0] * d[1][1] - d[0][1] * d[1][0];
	tmp = d[0][0];
	d[0][0] = d[1][1] * 1 / detd;
	d[1][1] = tmp * 1 / detd;
	d[0][1] = -d[0][1] * 1 / detd;
	d[1][0] = -d[1][0] * 1 / detd;
	//Multiplizieren der zwei Matrizen
	x[0][0] = xn[0][0] * d[0][0] + xn[0][1] * d[1][0];
	x[0][1] = xn[0][0] * d[0][1] + xn[0][1] * d[1][1];
	x[1][0] = xn[1][0] * d[0][0] + xn[1][1] * d[1][0];
	x[1][1] = xn[1][0] * d[0][1] + xn[1][1] * d[1][1];
	//X-Position ermitteln
	pos->x = x[0][0] * x[1][1] - x[0][1] * x[1][0];
	//Multiplizieren der zwei Matrizen
	y[0][0] = yn[0][0] * d[0][0] + yn[0][1] * d[1][0];
	y[0][1] = yn[0][0] * d[0][1] + yn[0][1] * d[1][1];
	y[1][0] = yn[1][0] * d[0][0] + yn[1][1] * d[1][0];
	y[1][1] = yn[1][0] * d[0][1] + yn[1][1] * d[1][1];
	//Y-Position ermitteln
	pos->y = y[0][0] * y[1][1] - y[0][1] * y[1][0];
	__enable_irq();
}
/******************************************************************************/
/* End: Trilateration2D */
/******************************************************************************/

/******************************************************************************/
/* Function: PositionTask */
/******************************************************************************/
/*! \brief Position Task calculates the positions and reads out the angle to
 * 		   send the data to the CAN gatekeeper
 *
 * \author zursr1
 *
 * \version 0.0.1
 *
 * \date 11.04.2014 Function created
 *
 *
 *******************************************************************************/

static void PositionTask(void* pvParameters) {

	//initialise stuff

	float angle = 0;

	/* for ever */
	for (;;) {


		angle = yaw;

		//SWV_printfloat(yaw, 4);
		vTaskDelay(100 / portTICK_RATE_MS);
	}
}

/* ****************************************************************************/
/* End : GyroTask */
/* ****************************************************************************/

/*******************************************************************************
 * Function Name  : main
 * Description    : Main program
 * Input          : None
 * Output         : None
 * Return         : None
 * Attention		 : None
 *******************************************************************************/
//int main(void)
//{
//
//	LCD_Initializtion();
//	LCD_BackLight_Init();
//	LCD_Clear(Black);
//	UART_Config();
//	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){
//		;
//	}
//	SendRFMsg(SyncString);
//	LCD_DrawPicture(0,0,200,300,&Spielfeld);
//
//  /* Infinite loop */
//  while (1)
//  {
//	  if((GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 0)){
//		  SendRFMsg("![Hallo]");
//	  }
//
//		getrad(Receiver4,Tag1,&Robo1Pos.r1);
//		getrad(Receiver4,Tag2,&Robo1Pos.r2);
//		getrad(Receiver4,Tag3,&Robo1Pos.r3);
//		getrad(Receiver3,Tag1,&Robo2Pos.r1);
//		getrad(Receiver3,Tag2,&Robo2Pos.r2);
//		getrad(Receiver3,Tag3,&Robo2Pos.r3);
//	  //GPIO_WriteBit(GPIOA,GPIO_Pin_1,Bit_SET);
//	  //GPIO_WriteBit(GPIOA,GPIO_Pin_1,Bit_RESET);
//
//	 if((Robo1Pos.r1 != 0) && (Robo1Pos.r2 != 0) && (Robo1Pos.r3 != 0)){
//		 __disable_irq();
//		  Trilateration2D(0,3000,3000,1000,0,2000,Robo1Pos.r1,Robo1Pos.r2,Robo1Pos.r3,&Robo1Pos);
//		  LCD_FillCircle(200-(int)(Robo1Pos.y/10),300-(int)(Robo1Pos.x/10),3,Blue);
//		  if((Robo1Pos.x>2480) || (Robo1Pos.x<2450)){
//			  test = 1;
//		  }
//		  sprintf(stringout,"%4d",Robo1Pos.x);
//		  GUI_Text(200,0,stringout,Blue,Black);
//		  sprintf(stringout,"%4d",Robo1Pos.y);
//		  GUI_Text(200,20,stringout,Blue,Black);
////		  sprintf(stringout,"%4d",Robo1Pos.r1);
////		  GUI_Text(200,0,stringout,Blue,Black);
////		  sprintf(stringout,"%4d",Robo1Pos.r2);
////		  GUI_Text(200,20,stringout,Blue,Black);
////		  sprintf(stringout,"%4d",Robo1Pos.r3);
////		  GUI_Text(200,40,stringout,Blue,Black);
//		  Robo1Pos.r1 = 0;
//		  Robo1Pos.r2 = 0;
//		  Robo1Pos.r3 = 0;
//		  __enable_irq();
//	  }
//
//	 if((Robo2Pos.r1 != 0) && (Robo2Pos.r2 != 0) && (Robo2Pos.r3 != 0)){
//		 __disable_irq();
//		  Trilateration2D(0,3000,3000,1000,0,2000,Robo2Pos.r1,Robo2Pos.r2,Robo2Pos.r3,&Robo2Pos);
//		  LCD_FillCircle(200-(int)(Robo2Pos.y/10),300-(int)(Robo2Pos.x/10),3,Red);
//		  if((Robo2Pos.y>1380) || (Robo2Pos.y<1350)){
//			  test = 1;
//		  }
//		  sprintf(stringout,"%4d",Robo2Pos.x);
//		  GUI_Text(200,40,stringout,Red,Black);
//		  sprintf(stringout,"%4d",Robo2Pos.y);
//		  GUI_Text(200,60,stringout,Red,Black);
//		  Robo2Pos.r1 = 0;
//		  Robo2Pos.r2 = 0;
//		  Robo2Pos.r3 = 0;
//		  __enable_irq();
//	  }
//  }
//}
/*********************************************************************************************************
 END FILE
 *********************************************************************************************************/
