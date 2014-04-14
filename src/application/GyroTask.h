#ifndef GYROTASK_H_
#define GYROTASK_H_

/**
 ******************************************************************************
 * \file	GyroTask.h
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

/* ----------------------- module constant declaration -----------------------*/
#define GYROTASK_NAME			"Gyro Task"					/*!< Detailed description after the member */
#define GYROTASK_STACK_SIZE		( configMINIMAL_STACK_SIZE ) 		/*!< size of the receive and transmit task */
#define GYROTASK_PRIORITY		(configMAX_PRIORITIES - 1UL) 	/*!< priority of the receive and transmit task */

/* ------------------------- module type declaration -------------------------*/

/* ------------------------- module data declaration -------------------------*/
extern float yaw;		// measured angle
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

