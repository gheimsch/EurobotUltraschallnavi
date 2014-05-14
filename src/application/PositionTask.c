/**
 ******************************************************************************
 * \file	PositionTask.c
 ******************************************************************************
 * \brief Calculates the Position of the Robots and sends them via CAN to the
 * 			System
 *
 * Function : Gets the measured distances of the Process Task via Message Queue.
 * 				In every Message is the Type of Robot (own, Enemy) as well as the
 * 				three distances from the Tags to the Receiver. With these distances
 * 				we calculate with Triangulation the x-component and the y-component
 * 				of the robot. The own Robot is updated with the direction (yaw of
 * 				the Gyro Task).
 *
 *
 * Procedures :
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
/* Ultraschallnavi Eurobot 2014												  */
/* ****************************************************************************/

/* --------------------------------- imports ---------------------------------*/
#include "stm32f4xx.h"			/* uC includes */
#include <math.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

/* application */
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
uint8_t trusttemp = 0;
uint32_t oldxtemp = 0;
uint32_t oldytemp = 0;

/* ----------------------- module procedure declaration ----------------------*/

void initPositionTask(void);
static void PositionTask(void* pvParameters);
void Trilateration2D(int32_t x1, int32_t x2, int32_t x3, int32_t y1, int32_t y2,
		int32_t y3, uint16_t r1, uint16_t r2, uint16_t r3, Position * pos);
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


	/* for ever */
	for (;;) {

		/* check if message is in QueueSet and store active Message in the Member */
		xActivatedMember = xQueueSelectFromSet(msgqSetProcessPosition,
				portMAX_DELAY);

		/* data of own Robot arrived */
		if (xActivatedMember == msgqRobo1) {

			/*save old values*/
			trusttemp = Robo1.trust;
			oldxtemp = Robo1.x;
			oldytemp = Robo1.y;

			/* get the distances */
			xQueueReceive( xActivatedMember, &Robo1, 0);

			/*restore old values*/
			Robo1.trust = trusttemp;
			Robo1.x = oldxtemp;
			Robo1.y = oldytemp;

			/* calculate the position for team red */
			if (team == red) {
				Trilateration2D(X_TAG20_RED, X_TAG21_RED, X_TAG22_RED,
						Y_TAG20_RED, Y_TAG21_RED, Y_TAG22_RED, Robo1.r1,
						Robo1.r2, Robo1.r3, &Robo1);

				/* compensate the measured values */
				Robo1.x = Robo1.x + compensateXaxisRobored(Robo1.x);
				Robo1.y = Robo1.y + compensateYaxisRobored(Robo1.x, Robo1.y);
			}

			/* calculate the position for team yellow */
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

		/* data of other confederate Robot arrived */
		if (xActivatedMember == msgqRobo2) {

			/*save old values*/
			trusttemp = Robo2.trust;
			oldxtemp = Robo2.x;
			oldytemp = Robo2.y;

			/* get the distances */
			xQueueReceive( xActivatedMember, &Robo2, 0);

			/*restore old values*/
			Robo2.trust = trusttemp;
			Robo2.x = oldxtemp;
			Robo2.y = oldytemp;

			/* calculate the position for team red */
			if (team == red) {
				Trilateration2D(X_TAG20_RED, X_TAG21_RED, X_TAG22_RED,
						Y_TAG20_RED, Y_TAG21_RED, Y_TAG22_RED, Robo2.r1,
						Robo2.r2, Robo2.r3, &Robo2);

				/* compensate the measured values */
				Robo2.x = Robo2.x + compensateXaxisRobored(Robo2.x);
				Robo2.y = Robo2.y + compensateYaxisRobored(Robo2.x, Robo2.y);

			}

			/* calculate the position for team yellow */
			if (team == yellow) {
				Trilateration2D(X_TAG20_YELLOW, X_TAG21_YELLOW, X_TAG22_YELLOW,
						Y_TAG20_YELLOW, Y_TAG21_YELLOW, Y_TAG22_YELLOW,
						Robo2.r1, Robo2.r2, Robo2.r3, &Robo2);

				/* compensate the measured values */
				Robo2.x = Robo2.x + compensateXaxisRoboyellow(Robo2.x);
				Robo2.y = Robo2.y + compensateYaxisRoboyellow(Robo2.x, Robo2.y);
			}

		}

		/* data of the first Enemy arrived */
		if (xActivatedMember == msgqEnemy1) {

			/*save old values*/
			trusttemp = Enemy1.trust;
			oldxtemp = Enemy1.x;
			oldytemp = Enemy1.y;

			/* get the distances */
			xQueueReceive( xActivatedMember, &Enemy1, 0);

			/*restore old values*/
			Enemy1.trust = trusttemp;
			Enemy1.x = oldxtemp;
			Enemy1.y = oldytemp;

			taskENTER_CRITICAL();
			/* calculate the position for team red */
			if (team == red) {
				Trilateration2D(X_TAG20_RED, X_TAG21_RED, X_TAG22_RED,
						Y_TAG20_RED, Y_TAG21_RED, Y_TAG22_RED, Enemy1.r1,
						Enemy1.r2, Enemy1.r3, &Enemy1);

				/* compensate the measured values */
				Enemy1.x = Enemy1.x + compensateXaxisEnemyred(Enemy1.x);
				Enemy1.y = Enemy1.y
						+ compensateYaxisEnemyred(Enemy1.x, Enemy1.y);
			}

			/* calculate the position for team yellow */
			if (team == yellow) {
				Trilateration2D(X_TAG20_YELLOW, X_TAG21_YELLOW, X_TAG22_YELLOW,
						Y_TAG20_YELLOW, Y_TAG21_YELLOW, Y_TAG22_YELLOW,
						Enemy1.r1, Enemy1.r2, Enemy1.r3, &Enemy1);

				/* compensate the measured values */
				Enemy1.x = Enemy1.x + compensateXaxisEnemyyellow(Enemy1.x);
				Enemy1.y = Enemy1.y
						+ compensateYaxisEnemyyellow(Enemy1.x, Enemy1.y);
			}
			taskEXIT_CRITICAL();

		}

		/* data of the second Enemy arrived */
		if (xActivatedMember == msgqEnemy2) {

			/*save old values*/
			trusttemp = Enemy2.trust;
			oldxtemp = Enemy2.x;
			oldytemp = Enemy2.y;

			/* get the distances */
			xQueueReceive( xActivatedMember, &Enemy2, 0);

			/*restore old values*/
			Enemy2.trust = trusttemp;
			Enemy2.x = oldxtemp;
			Enemy2.y = oldytemp;

			/* calculate the position for team red */
			if (team == red) {
				Trilateration2D(X_TAG20_RED, X_TAG21_RED, X_TAG22_RED,
						Y_TAG20_RED, Y_TAG21_RED, Y_TAG22_RED, Enemy2.r1,
						Enemy2.r2, Enemy2.r3, &Enemy2);

				/* compensate the measured values */
				Enemy2.x = Enemy2.x + compensateXaxisEnemyred(Enemy2.x);
				Enemy2.y = Enemy2.y
						+ compensateYaxisEnemyred(Enemy2.x, Enemy2.y);
			}

			/* calculate the position for team yellow */
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
 *
 *******************************************************************************/

void Trilateration2D(int32_t x1, int32_t x2, int32_t x3, int32_t y1, int32_t y2,
		int32_t y3, uint16_t r1, uint16_t r2, uint16_t r3, Position * pos) {

	/* initialise the parameters for the calculation */
	int32_t xn[2][2];
	int32_t yn[2][2];
	double_t d[2][2];
	double_t x[2][2] = { { 0, 0 }, { 0, 0 } };
	double_t y[2][2] = { { 0, 0 }, { 0, 0 } };
	int32_t detd = 0;
	double_t tmp = 0;
	uint32_t calc_x = 0;
	uint32_t calc_y = 0;

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

	/* Inverse of the 2D-Matrix d */
	detd = d[0][0] * d[1][1] - d[0][1] * d[1][0];
	tmp = d[0][0];
	d[0][0] = d[1][1] * 1 / detd;
	d[1][1] = tmp * 1 / detd;
	d[0][1] = -d[0][1] * 1 / detd;
	d[1][0] = -d[1][0] * 1 / detd;
	/* multiply the Matrix xn with d */
	x[0][0] = xn[0][0] * d[0][0] + xn[0][1] * d[1][0];
	x[0][1] = xn[0][0] * d[0][1] + xn[0][1] * d[1][1];
	x[1][0] = xn[1][0] * d[0][0] + xn[1][1] * d[1][0];
	x[1][1] = xn[1][0] * d[0][1] + xn[1][1] * d[1][1];
	/* calculate the x-Axis */
	calc_x = x[0][0] * x[1][1] - x[0][1] * x[1][0];

	/* multiply the Matrix yn with d */
	y[0][0] = yn[0][0] * d[0][0] + yn[0][1] * d[1][0];
	y[0][1] = yn[0][0] * d[0][1] + yn[0][1] * d[1][1];
	y[1][0] = yn[1][0] * d[0][0] + yn[1][1] * d[1][0];
	y[1][1] = yn[1][0] * d[0][1] + yn[1][1] * d[1][1];
	/* calculate the y-Axis */
	calc_y = y[0][0] * y[1][1] - y[0][1] * y[1][0];
	/* check if new position is to far from old one */
	if (((pos->x - calc_x) * (pos->x - calc_x)
			+ (pos->y - calc_y) * (pos->y - calc_y)
			< (TRUSTRADIUS * TRUSTRADIUS)) || (pos->trust >= 1)){
	pos->x = calc_x;
	pos->y = calc_y;
	pos->trust = 0;
	} else {
	pos->trust++;
	}
}
/******************************************************************************/
/* End: Trilateration2D */
/******************************************************************************/

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
//	Robo1.r1 = 0;
//	Robo1.r2 = 0;
//	Robo1.r3 = 0;
//	Robo1.x = 0;
//	Robo1.y = 0;
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
	txConfederatePositionResponse(Robo2.x, Robo2.y, 0, 0);
//	Robo2.r1 = 0;
//	Robo2.r2 = 0;
//	Robo2.r3 = 0;
//	Robo2.x = 0;
//	Robo2.y = 0;
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
	txEnemey1PositionResponse(Enemy1.x, Enemy1.y, 0, 0);
//	Enemy1.r1 = 0;
//	Enemy1.r2 = 0;
//	Enemy1.r3 = 0;
//	Enemy1.x = 0;
//	Enemy1.y = 0;
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
	txEnemey2PositionResponse(Enemy2.x, Enemy2.y, 0, 0);
//	Enemy2.r1 = 0;
//	Enemy2.r2 = 0;
//	Enemy2.r3 = 0;
//	Enemy2.x = 0;
//	Enemy2.y = 0;
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

	/* Parameter of the Polynom */
	float a = -0.0801;
	float b = 103.7;

	/* Compensationpolynom */
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

	/* Parameter of the Polynom */
	float a = -0.04324;
	float b = -0.1853;
	float c = 0.00004126;
	float d = 0.00002256;
	float e = 134.8;

	/* Compensationpolynom */
	return a * x + b * y + c * x * y + d * pow(y, 2) + e;
}

/* ****************************************************************************/
/* End : compensateYaxisRobored */
/* ****************************************************************************/

/******************************************************************************/
/* Function: compensateXaxisEnemyred */
/******************************************************************************/
/*! \brief compensates the Error of the x-Axis of our Enemy's with a linear
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

	/* Parameter of the Polynom */
	float a = -0.08;
	float b = 118.4;

	/* Compensationpolynom */
	return a * x + b;
}

/* ****************************************************************************/
/* End : compensateXaxisEnemyred */
/* ****************************************************************************/

/******************************************************************************/
/* Function: compensateYaxisEnemyred */
/******************************************************************************/
/*! \brief compensates the Error of the y-Axis of our Enemy's with a polynomical
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

	/* Parameter of the Polynom */
	float a = -0.04328;
	float b = -0.1443;
	float c = 0.00004424;
	float d = 0.00000723;
	float e = 112.2;

	/* Compensationpolynom */
	return a * x + b * y + c * x * y + d * pow(y, 2) + e;
}

/* ****************************************************************************/
/* End : compensateYaxisEnemyred */
/* ****************************************************************************/

/******************************************************************************/
/* Function: compensateXaxisRoboyellow */
/******************************************************************************/
/*! \brief compensates the Error of the x-Axis of our Robot with a linear
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

	/* Parameter of the Polynom */
	float a = -0.0756;
	float b = 131;

	/* Compensationpolynom */
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

	/* Parameter of the Polynom */
	float a = 0.0331;
	float b = -0.02139;
	float c = -0.00003513;
	float d = 0.000004503;
	float e = 34.35;

	/* Compensationpolynom */
	return a * x + b * y + c * x * y + d * pow(y, 2) + e;
}

/* ****************************************************************************/
/* End : compensateYaxisRoboyellow */
/* ****************************************************************************/

/******************************************************************************/
/* Function: compensateXaxisEnemyyellow */
/******************************************************************************/
/*! \brief compensates the Error of the x-Axis of our Enemy's Robot with a linear
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

	/* Parameter of the Polynom */
	float a = -0.0756;
	float b = 145.7;

	/* Compensationpolynom */
	return a * x + b;
}

/* ****************************************************************************/
/* End : compensateXaxisEnemyyellow */
/* ****************************************************************************/

/******************************************************************************/
/* Function: compensateYaxisEnemyyellow */
/******************************************************************************/
/*! \brief compensates the Error of the y-Axis of our Enemy's Robot with a polynomical
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

	/* Parameter of the Polynom */
	float a = 0.04147;
	float b = -0.04321;
	float c = -0.00004131;
	float d = 0.00001893;
	float e = 18.28;

	/* Compensationpolynom */
	return a * x + b * y + c * x * y + d * pow(y, 2) + e;
}

/* ****************************************************************************/
/* End : compensateYaxisEnemyyellow */
/* ****************************************************************************/
