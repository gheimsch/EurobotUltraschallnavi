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
#include "CANGatekeeper.h"
#include "configNavi.h"

/* ------------------------- module data declaration -------------------------*/
xQueueSetMemberHandle xActivatedMember;

Position Robo1;
Position Robo2;
Position Enemy1;
Position Enemy2;

uint8_t team = tbd;
/* ----------------------- module procedure declaration ----------------------*/

void initPositionTask(void);
void Trilateration2D(int32_t x1, int32_t x2, int32_t x3, int32_t y1, int32_t y2,
		int32_t y3, uint16_t r1, uint16_t r2, uint16_t r3, Position * pos);
static void PositionTask(void* pvParameters);
void posRobo1Request(uint16_t id, CAN_data_t* data);
void posRobo2Request(uint16_t id, CAN_data_t* data);
void posEnemy1Request(uint16_t id, CAN_data_t* data);
void posEnemy2Request(uint16_t id, CAN_data_t* data);
float compensateXaxisRoboyellow(float x);
float compensateYaxisRoboyellow(float x, float y);
float compensateXaxisEnemyyellow(float x);
float compensateYaxisEnemyyellow(float x, float y);
float compensateXaxisRobored(float x);
float compensateYaxisRobored(float x, float y);
float compensateXaxisEnemyred(float x);
float compensateYaxisEnemyred(float x, float y);
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

	/* set the Position Request Listener for the own Robot */
	setFunctionCANListener(posRobo1Request, NAVI_POSITION_REQUEST);

	/* set the Position Request Listener for the confederated Robot */
	setFunctionCANListener(posRobo2Request, CONFEDERATE_POSITION_REQUEST);

	/* set the Position Request Listener for the first Enemy Robot */
	setFunctionCANListener(posEnemy1Request, ENEMEY_1_POSITION_REQUEST);

	/* set the Position Request Listener for the second Enemy Robot */
	setFunctionCANListener(posEnemy2Request, ENEMEY_2_POSITION_REQUEST);

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
 * \param[in] x1 x-pos of the Transmitter 1
 * \param[in] x2 x-pos of the Transmitter 2
 * \param[in] x3 x-pos of the Transmitter 3
 * \param[in] y1 y-pos of the Transmitter 1
 * \param[in] y2 y-pos of the Transmitter 2
 * \param[in] y3 y-pos of the Transmitter 3
 * \param[in] r1 Measured Distance from Transmitter 1 to the Receiver
 * \param[in] r2 Measured Distance from Transmitter 2 to the Receiver
 * \param[in] r3 Measured Distance from Transmitter 3 to the Receiver
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

void Trilateration2D(int32_t x1, int32_t x2, int32_t x3, int32_t y1, int32_t y2,
		int32_t y3, uint16_t r1, uint16_t r2, uint16_t r3, Position * pos) {

	int32_t xn[2][2];
	int32_t yn[2][2];
	double_t d[2][2];
	double_t x[2][2] = { { 0, 0 }, { 0, 0 } };
	double_t y[2][2] = { { 0, 0 }, { 0, 0 } };
	int32_t detd = 0;
	double_t tmp = 0;

	xn[0][0] = (int32_t) (pow(r1, 2) - pow(r2, 2)) - (pow(x1, 2) - pow(x2, 2))
			- (pow(y1, 2) - pow(y2, 2));
	xn[1][0] = (int32_t) (pow(r1, 2) - pow(r3, 2)) - (pow(x1, 2) - pow(x3, 2))
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


	team = red;

	/* for ever */
	for (;;) {

		xActivatedMember = xQueueSelectFromSet(msgqSetProcessPosition,
				portMAX_DELAY);

		if (xActivatedMember == msgqRobo1) {

			// get the distances
			xQueueReceive( xActivatedMember, &Robo1, 0);

			// calculate the position
			if (team == red) {
				Trilateration2D(X_TAG20_RED, X_TAG21_RED, X_TAG22_RED,
						Y_TAG20_RED, Y_TAG21_RED, Y_TAG22_RED, Robo1.r1,
						Robo1.r2, Robo1.r3, &Robo1);

				/* compensate the measured values */
				Robo1.x = Robo1.x + compensateXaxisRobored(Robo1.x);
				Robo1.y = Robo1.y + compensateYaxisRobored(Robo1.x, Robo1.y);
			}
			if (team == yellow) {
				Trilateration2D(X_TAG20_YELLOW, X_TAG21_YELLOW, X_TAG22_YELLOW,
						Y_TAG20_YELLOW, Y_TAG21_YELLOW, Y_TAG22_YELLOW,
						Robo1.r1, Robo1.r2, Robo1.r3, &Robo1);

				/* compensate the measured values */
				Robo1.x = Robo1.x + compensateXaxisRoboyellow(Robo1.x);
				Robo1.y = Robo1.y + compensateYaxisRoboyellow(Robo1.x, Robo1.y);
			}

			/* get the angle */
			Robo1.angle = yaw;

		}
		if (xActivatedMember == msgqRobo2) {

			// get the distances
			xQueueReceive( xActivatedMember, &Robo2, 0);

			// calculate the position
			if (team == red) {
				Trilateration2D(X_TAG20_RED, X_TAG21_RED, X_TAG22_RED,
						Y_TAG20_RED, Y_TAG21_RED, Y_TAG22_RED, Robo2.r1,
						Robo2.r2, Robo2.r3, &Robo2);

				/* compensate the measured values */
				Robo2.x = Robo2.x + compensateXaxisRobored(Robo2.x);
				Robo2.y = Robo2.y + compensateYaxisRobored(Robo2.x, Robo2.y);

			}
			if (team == yellow) {
				Trilateration2D(X_TAG20_YELLOW, X_TAG21_YELLOW, X_TAG22_YELLOW,
						Y_TAG20_YELLOW, Y_TAG21_YELLOW, Y_TAG22_YELLOW,
						Robo2.r1, Robo2.r2, Robo2.r3, &Robo2);

				/* compensate the measured values */
				Robo2.x = Robo2.x + compensateXaxisRoboyellow(Robo2.x);
				Robo2.y = Robo2.y + compensateYaxisRoboyellow(Robo2.x, Robo2.y);
			}

		}
		if (xActivatedMember == msgqEnemy1) {

			// get the distances
			xQueueReceive( xActivatedMember, &Enemy1, 0);

			// calculate the position
			if (team == red) {
				Trilateration2D(X_TAG20_RED, X_TAG21_RED, X_TAG22_RED,
						Y_TAG20_RED, Y_TAG21_RED, Y_TAG22_RED, Enemy1.r1,
						Enemy1.r2, Enemy1.r3, &Enemy1);

				/* compensate the measured values */
				Enemy1.x = Enemy1.x + compensateXaxisEnemyred(Enemy1.x);
				Enemy1.y = Enemy1.y
						+ compensateYaxisEnemyred(Enemy1.x, Enemy1.y);
			}
			if (team == yellow) {
				Trilateration2D(X_TAG20_YELLOW, X_TAG21_YELLOW, X_TAG22_YELLOW,
						Y_TAG20_YELLOW, Y_TAG21_YELLOW, Y_TAG22_YELLOW,
						Enemy1.r1, Enemy1.r2, Enemy1.r3, &Enemy1);

				/* compensate the measured values */
				Enemy1.x = Enemy1.x + compensateXaxisEnemyyellow(Enemy1.x);
				Enemy1.y = Enemy1.y
						+ compensateYaxisEnemyyellow(Enemy1.x, Enemy1.y);
			}
		}

		if (xActivatedMember == msgqEnemy2) {

			// get the distances
			xQueueReceive( xActivatedMember, &Enemy2, 0);

			// calculate the position
			if (team == red) {
				Trilateration2D(X_TAG20_RED, X_TAG21_RED, X_TAG22_RED,
						Y_TAG20_RED, Y_TAG21_RED, Y_TAG22_RED, Enemy2.r1,
						Enemy2.r2, Enemy2.r3, &Enemy2);

				/* compensate the measured values */
				Enemy2.x = Enemy2.x + compensateXaxisEnemyred(Enemy2.x);
				Enemy2.y = Enemy2.y
						+ compensateYaxisEnemyred(Enemy2.x, Enemy2.y);
			}
			if (team == yellow) {
				Trilateration2D(X_TAG20_YELLOW, X_TAG21_YELLOW, X_TAG22_YELLOW,
						Y_TAG20_YELLOW, Y_TAG21_YELLOW, Y_TAG22_YELLOW,
						Enemy2.r1, Enemy2.r2, Enemy2.r3, &Enemy2);

				/* compensate the measured values */
				Enemy2.x = Enemy2.x + compensateXaxisEnemyyellow(Enemy2.x);
				Enemy2.y = Enemy2.y
						+ compensateYaxisEnemyyellow(Enemy2.x, Enemy2.y);
			}
		}
	}
}
/* ****************************************************************************/
/* End : PositionTask */
/* ****************************************************************************/

/******************************************************************************/
/* Function: posRobo1Request */
/******************************************************************************/
/*! \brief sends the Position of the own Robot via CAN
 *
 * \param[in] id ID of the Message
 * \param[in] data Data of the Message
 *
 * \author zursr1
 *
 * \version 0.0.1
 *
 * \date 14.04.2014 Function created
 *
 *
 *******************************************************************************/

void posRobo1Request(uint16_t id, CAN_data_t* data) {
	txNaviPositionResponse(Robo1.x, Robo1.y, Robo1.angle, 0);
}

/* ****************************************************************************/
/* End : posRobo1Request */
/* ****************************************************************************/

/******************************************************************************/
/* Function: posRobo2Request */
/******************************************************************************/
/*! \brief sends the Position of the confederated Robot via CAN
 *
 * \param[in] id ID of the Message
 * \param[in] data Data of the Message
 *
 * \author zursr1
 *
 * \version 0.0.1
 *
 * \date 14.04.2014 Function created
 *
 *
 *******************************************************************************/

void posRobo2Request(uint16_t id, CAN_data_t* data) {
	txNaviPositionResponse(Robo2.x, Robo2.y, 0, 0);
}

/* ****************************************************************************/
/* End : posRobo2Request */
/* ****************************************************************************/

/******************************************************************************/
/* Function: posEnemy1Request */
/******************************************************************************/
/*! \brief sends the Position of first Enemyrobot via CAN
 *
 * \param[in] id ID of the Message
 * \param[in] data Data of the Message
 *
 * \author zursr1
 *
 * \version 0.0.1
 *
 * \date 14.04.2014 Function created
 *
 *
 *******************************************************************************/

void posEnemy1Request(uint16_t id, CAN_data_t* data) {
	txNaviPositionResponse(Enemy1.x, Enemy1.y, 0, 0);
}

/* ****************************************************************************/
/* End : posEnemy1Request */
/* ****************************************************************************/

/******************************************************************************/
/* Function: posEnemy2Request */
/******************************************************************************/
/*! \brief sends the Position of the second Enemyrobot via CAN
 *
 * \param[in] id ID of the Message
 * \param[in] data Data of the Message
 *
 * \author zursr1
 *
 * \version 0.0.1
 *
 * \date 14.04.2014 Function created
 *
 *
 *******************************************************************************/

void posEnemy2Request(uint16_t id, CAN_data_t* data) {
	txNaviPositionResponse(Enemy2.x, Enemy2.y, 0, 0);
}

/* ****************************************************************************/
/* End : posEnemy2Request */
/* ****************************************************************************/

/******************************************************************************/
/* Function: compensateXaxisRobored */
/******************************************************************************/
/*! \brief compensates the Error of the x-Axis of our Robots with a linear
 * 		   function
 *
 * \param[in] x measured x-Axis
 *
 * \return compensation value
 *
 * \author zursr1
 *
 * \version 0.0.1
 *
 * \date 16.04.2014 Function created
 *
 *
 *******************************************************************************/

float compensateXaxisRobored(float x) {

	float a = -0.0801;
	float b = 103.7;

	return a * x + b;
}

/* ****************************************************************************/
/* End : compensateXaxisRobored */
/* ****************************************************************************/

/******************************************************************************/
/* Function: compensateYaxisRobored */
/******************************************************************************/
/*! \brief compensates the Error of the y-Axis of our Robots with a polynomical
 * 		   function
 *
 *
 * \param[in] x measured x-Axis
 * \param[in] y measured y-Axis
 *
 * \return compensation value
 *
 * \author zursr1
 *
 * \version 0.0.1
 *
 * \date 16.04.2014 Function created
 *
 *
 *******************************************************************************/

float compensateYaxisRobored(float x, float y) {

	float a = -0.04324;
	float b = -0.1853;
	float c = 0.00004126;
	float d = 0.00002256;
	float e = 134.8;

	return a * x + b * y + c * x * y + d * pow(y, 2) + e;
}

/* ****************************************************************************/
/* End : compensateYaxisRobored */
/* ****************************************************************************/

/******************************************************************************/
/* Function: compensateXaxisEnemyred */
/******************************************************************************/
/*! \brief compensates the Error of the x-Axis of our Enemys with a linear
 * 		   function
 *
 * \param[in] x measured x-Axis
 *
 * \return compensation value
 *
 * \author zursr1
 *
 * \version 0.0.1
 *
 * \date 16.04.2014 Function created
 *
 *
 *******************************************************************************/

float compensateXaxisEnemyred(float x) {

	float a = -0.08;
	float b = 118.4;

	return a * x + b;
}

/* ****************************************************************************/
/* End : compensateXaxisEnemyred */
/* ****************************************************************************/

/******************************************************************************/
/* Function: compensateYaxisEnemyred */
/******************************************************************************/
/*! \brief compensates the Error of the y-Axis of our Enemys with a polynomical
 * 		   function
 *
 *
 * \param[in] x measured x-Axis
 * \param[in] y measured y-Axis
 *
 * \return compensation value
 *
 * \author zursr1
 *
 * \version 0.0.1
 *
 * \date 16.04.2014 Function created
 *
 *
 *******************************************************************************/

float compensateYaxisEnemyred(float x, float y) {

	float a = -0.04328;
	float b = -0.1443;
	float c = 0.00004424;
	float d = 0.00000723;
	float e = 112.2;

	return a * x + b * y + c * x * y + d * pow(y, 2) + e;
}

/* ****************************************************************************/
/* End : compensateYaxisEnemyred */
/* ****************************************************************************/

/******************************************************************************/
/* Function: compensateXaxisRoboyellow */
/******************************************************************************/
/*! \brief compensates the Error of the x-Axis of our own Robot with a linear
 * 		   function
 *
 * \param[in] x measured x-Axis
 *
 * \return compensation value
 *
 * \author zursr1
 *
 * \version 0.0.1
 *
 * \date 16.04.2014 Function created
 *
 *
 *******************************************************************************/

float compensateXaxisRoboyellow(float x) {

	float a = -0.0756;
	float b = 145.7;

	return a * x + b;
}

/* ****************************************************************************/
/* End : compensateXaxisRoboyellow */
/* ****************************************************************************/

/******************************************************************************/
/* Function: compensateYaxisRoboyellow */
/******************************************************************************/
/*! \brief compensates the Error of the y-Axis of our Robot with a polynomical
 * 		   function
 *
 *
 * \param[in] x measured x-Axis
 * \param[in] y measured y-Axis
 *
 * \return compensation value
 *
 * \author zursr1
 *
 * \version 0.0.1
 *
 * \date 16.04.2014 Function created
 *
 *
 *******************************************************************************/

float compensateYaxisRoboyellow(float x, float y) {

	float a = 0.04147;
	float b = -0.04321;
	float c = -0.00004131;
	float d = 0.00001893;
	float e = 18.28;

	return a * x + b * y + c * x * y + d * pow(y, 2) + e;
}

/* ****************************************************************************/
/* End : compensateYaxisRoboyellow */
/* ****************************************************************************/

/******************************************************************************/
/* Function: compensateXaxisEnemyyellow */
/******************************************************************************/
/*! \brief compensates the Error of the x-Axis of our Enemys Robot with a linear
 * 		   function
 *
 * \param[in] x measured x-Axis
 *
 * \return compensation value
 *
 * \author zursr1
 *
 * \version 0.0.1
 *
 * \date 16.04.2014 Function created
 *
 *
 *******************************************************************************/

float compensateXaxisEnemyyellow(float x) {

	float a = -0.0756;
	float b = 131;

	return a * x + b;
}

/* ****************************************************************************/
/* End : compensateXaxisEnemyyellow */
/* ****************************************************************************/

/******************************************************************************/
/* Function: compensateYaxisEnemyyellow */
/******************************************************************************/
/*! \brief compensates the Error of the y-Axis of our Enemys Robot with a polynomical
 * 		   function
 *
 *
 * \param[in] x measured x-Axis
 * \param[in] y measured y-Axis
 *
 * \return compensation value
 *
 * \author zursr1
 *
 * \version 0.0.1
 *
 * \date 16.04.2014 Function created
 *
 *
 *******************************************************************************/

float compensateYaxisEnemyyellow(float x, float y) {

	float a = 0.0331;
	float b = -0.02139;
	float c = -0.00003513;
	float d = 0.000004503;
	float e = 34.35;

	return a * x + b * y + c * x * y + d * pow(y, 2) + e;
}

/* ****************************************************************************/
/* End : compensateYaxisEnemyyellow */
/* ****************************************************************************/
