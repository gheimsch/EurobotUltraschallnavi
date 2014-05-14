#ifndef GYROTASK_H_
#define GYROTASK_H_

/**
 ******************************************************************************
 * \file	GyroTask.h
 ******************************************************************************
 * \brief Reads out the angulare rate of the Gyro and calculates the direction
 * 			of the Robot
 *
 *
 * \author heimg1, zursr1
 *
 * \version 0.0.1
 *
 * \history 10.04.2014 File Created
 *
 *
 */
/* ****************************************************************************/
/* Ultraschallnavi Eurobot 2014 */
/* ****************************************************************************/

/* --------------------------------- imports ---------------------------------*/
#include "semphr.h"
/* ----------------------- module constant declaration -----------------------*/
#define GYROTASK_NAME			"Gyro Task"					/*!< Detailed description after the member */
#define GYROTASK_STACK_SIZE		( configMINIMAL_STACK_SIZE ) 		/*!< size of the receive and transmit task */
#define GYROTASK_PRIORITY		(configMAX_PRIORITIES - 2UL) 	/*!< priority of the receive and transmit task */

/* register used to setup the gyro */
#define CTRL_REG1_G   	0x20
#define CTRL_REG2_G   	0x21
#define CTRL_REG3_G   	0x22
#define CTRL_REG4_G   	0x23
#define CTRL_REG5_G   	0x24
#define STATUS_REG1_G   0x27

/* Register to read out data */
#define OUT_X_L_G   	0xA8
#define OUT_TEMP_G 		0x26
/* ------------------------- module type declaration -------------------------*/

/* ------------------------- module data declaration -------------------------*/
extern float yaw;		/* measured angle (used in Position Task) */
extern xSemaphoreHandle xSyncSemaphore;//TODO
extern xTaskHandle xGyroTaskHandle;
/* ----------------------- module procedure declaration ----------------------*/
extern void initGyroTask(void);
extern void initGyr(void);
extern void setGyrSensitivity(uint16_t);
extern void calculateGyroOffsets(void);
extern void calculateDrift(void);
extern int16_t getGyrValues(void);
extern float getTempValue(void);
extern void calculateAngle(int16_t, uint32_t);
/* ****************************************************************************/
/* End Header : GyroTask.h */
/* ****************************************************************************/

#endif /* GYROTASK_H_ */

