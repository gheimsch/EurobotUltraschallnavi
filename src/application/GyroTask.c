/**
 ****************************************************************************
 * \file	GyroTask.c
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
 * \history 10.04.2014 File Created
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

#include "../lib/I2C.h"		/* Own header include */
#include "../lib/UART.h"
#include "math.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "string.h"

#include "GyroTask.h"
#include "PositionTask.h"

/* ------------------------- module data declaration -------------------------*/
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


/* data to config the gyro */
static uint8_t CTRL_REG1_G_DATA = 0x0F;
static uint8_t CTRL_REG2_G_DATA = 0x00;
static uint8_t CTRL_REG3_G_DATA = 0x08;
static uint8_t CTRL_REG4_G_DATA = 0x00;
static uint8_t CTRL_REG5_G_DATA = 0x00;

static float gyrScale = 0; 	// scale of the Gyro set with the sensivity of the devise
static float gyrOffset = 0;	// measured Offset of the gyro
static float driftyaw = 0;	// constants to compensate linear drift

float yaw = 0;		// measured angle

typedef struct RungeKuta {
	float fourth_Prev;
	float third_Prev;
	float second_Prev;
	float Prev;
} RungeKutta;

RungeKutta RukaRaw_yaw;		// struct to approximate the angle

/* ----------------------- module procedure declaration ----------------------*/

void initGyroTask(void);
void initGyr(void);

void setGyrSensitivity(uint16_t);
void calculateGyroOffsets(void);
void calculateDrift(void);
int16_t getGyrValues(void);
float getTempValue(void);
void calculateAngle(int16_t, uint32_t);
float computeRungeKuttaNormal(RungeKutta *rk, float CurrentValue);
static void GyroTask(void* pvParameters);
void USARTPrint(const char *ToSend, unsigned char length);
void itoa( int z, char* Buffer );
/* ****************************************************************************/
/* End Header : GyroTask.c													  */
/* ****************************************************************************/

/******************************************************************************/
/* Function: initGyroTask */
/******************************************************************************/
/*! \brief initialise the Gyro Task
 *
 * \author zursr1
 *
 * \version 0.0.1
 *
 * \date 10.04.2014 Function created
 *
 *
 *******************************************************************************/

void initGyroTask(void) {

	/* create the task */
	xTaskCreate(GyroTask, (signed char *) GYROTASK_NAME, GYROTASK_STACK_SIZE,
			NULL, GYROTASK_PRIORITY, NULL);

	/* initialise the I2C-Interface to the Gyro */
	initI2C();

	/* TODO initialise when start received */
	initGyr();

}

/* ****************************************************************************/
/* End : initGyroTask */
/* ****************************************************************************/

/**
 *************************************************************************************
 * @brief	initialise the gyro, setting the specifics registers
 * @param	sensitivity: sensitivity of the sensor
 * @note 	just called for initialisation
 **************************************************************************************
 */

void initGyr(void) {

	setGyrSensitivity(250);		// set sensivity and gyro Scale

	// Check the datasheet (52ff)

	// Enable x, y, z and turn off power down:

	// If you'd like to adjust/use the HPF, you can edit the line below to configure CTRL_REG2:
	writeI2C(Gyro, CTRL_REG1_G, &CTRL_REG1_G_DATA, 1, 100);

	// If you'd like to adjust/use the HPF, you can edit the line below to configure CTRL_REG2:
	writeI2C(Gyro, CTRL_REG2_G, &CTRL_REG2_G_DATA, 1, 100);

	// Configure CTRL_REG3 to generate data ready interrupt on INT2
	// No interrupts used on INT1, if you'd like to configure INT1
	// or INT2 otherwise, consult the datasheet:
	writeI2C(Gyro, CTRL_REG3_G, &CTRL_REG3_G_DATA, 1, 100);

	// CTRL_REG4 controls the full-scale range, among other things:
	writeI2C(Gyro, CTRL_REG4_G, &CTRL_REG4_G_DATA, 1, 100);

	// CTRL_REG5 controls high-pass filtering of outputs, use it
	// if you'd like:
	writeI2C(Gyro, CTRL_REG5_G, &CTRL_REG5_G_DATA, 1, 100);
}

/**
 *************************************************************************************
 * @brief	set the sensitivy of the gyroscope in fps
 * @param	sensitivity: sensitivity of the sensor
 * @note 	just called while initialisation
 **************************************************************************************
 */

void setGyrSensitivity(uint16_t sensitivity) {

	if (sensitivity == 500) {
		gyrScale = 0.0175; // from the datasheet
		CTRL_REG4_G_DATA = 0x10;
	} else if (sensitivity == 2000) {
		gyrScale = 0.070; // from the datasheet
		CTRL_REG4_G_DATA = 0x20;
	} else {
		// default value : 250 dps
		gyrScale = 0.00875; // from the datasheet
		CTRL_REG4_G_DATA = 0x00;
	}
}

/**
 *************************************************************************************
 * @brief	calculates the offset of the gyroscope (for better data reading later)
 * @param	none
 * @note	just called while initialisation
 * @warning The gyro must not be in movement when calculating the offset
 **************************************************************************************
 */

void calculateGyroOffsets(void) {

	float gyrValue = 0;
	float totalGz = 0;

	uint8_t n = 100;
	uint8_t i = 0;

	for (i = 0; i < n; i++) {
		gyrValue = getGyrValues();

		totalGz += gyrValue;

		vTaskDelay(10 / portTICK_RATE_MS);
	}

	gyrOffset = totalGz / (float) n;
}

/**
 *************************************************************************************
 * @brief	calculates the gyro drift (for better data reading later)
 * @param	none
 * @warning The gyro must not be in movement when calculating the drift
 **************************************************************************************
 */

void calculateDrift(void) {

	static float Gz[100] = { 0 };	// all read out angular rates over 1 sec
	static float tz[100] = { 0 };	// time between the measurements
	static float a[99] = { 0 };	// drift
	float gyrValue;

	uint32_t t_Start = 0;	// start timer of the measurement
	uint32_t t_End = 0;		// end timer of the measurement
	uint32_t delta_t;	// measured time

	uint16_t nbIterations = 100;
	uint16_t i = 0;

	t_Start = xTaskGetTickCount();

	for (i = 0; i < nbIterations; i++) {

		vTaskDelay(10 / portTICK_RATE_MS);

		gyrValue = getGyrValues();

		t_End = xTaskGetTickCount();

		delta_t = t_End - t_Start;
		t_Start = t_End;

		Gz[i] = (gyrValue - gyrOffset) * gyrScale;

		tz[i] = (float) delta_t / 1000;

		if (i > 0) {

			a[i - 1] = (Gz[i] - Gz[i - 1]) / tz[i];		//calculate the drift
		}

		if (i == 99) {
			uint32_t j = 0;

			for (j = 0; j < 99; j++) {
				driftyaw += a[j];
			}

			driftyaw = driftyaw / (j);

		}
	}
}

/**
 *************************************************************************************
 * @brief	get the Data Values of the Gyro
 * @param	none
 * @return	gyrValue: angular rate in z-axis
 **************************************************************************************
 */
int16_t getGyrValues(void) {

	uint8_t readBuf[6] = { 0 };	// Buffer to save the read out register

	int16_t gyrValue;	// measured angular rate

	writeI2C(Gyro, (OUT_X_L_G), OUT_X_L_G, 0, 10);
	readI2C(Gyro, readBuf, 6, 10);

	gyrValue = readBuf[4] + (readBuf[5] << 8);

	return gyrValue;

}

/**
 *************************************************************************************
 * @brief	calculates the angles of the three axis
 * @param	GyrX: measured angular rate in x-axis
 * 			GyrY: measured angular rate in y-axis
 * 			GyrZ: measured angular rate in z-axis
 * 			t: time between two calculations
 **************************************************************************************
 */
void calculateAngle(int16_t gyrValue, uint32_t t) {

	float dt = (float) t / 1000;  // Convert from miliseconds to seconds
	float gyrScaled;	// scaled angular rate
	float raw_yaw = 0;

	/* calculate Runge Kutta value from gyroscope */
	raw_yaw = computeRungeKuttaNormal(&RukaRaw_yaw, (float) gyrValue);

	/* calculate angular rate with offset and scale factor and make a linear drift compensation */
	gyrScaled = (raw_yaw - gyrOffset) * gyrScale + driftyaw * dt;

	yaw += gyrScaled * dt;  // Sum up the angle

	/* Check, if the angle reached 360 degrees */
	if (yaw >= 360) {
		yaw -= 360;
	} else if (yaw < 0) {
		yaw += 360;
	}



}

/******************************************************************************/
/*   Procedure  :   computeRungeKuttaNormal  	    											 		  */
/******************************************************************************/
/*!  \brief					Runge Kutta fourth order
 *
 *	\param					rk: pointer on struct with the four last values
 *									CurrentValue: the current raw value, read from the gyroscope
 *
 *	\return					meant value
 *
 *	\author					S.Briner
 *
 *	\date						07.01.2013
 *
 ******************************************************************************/
float computeRungeKuttaNormal(RungeKutta *rk, float CurrentValue) {
	rk->Prev = 0.1666666667
			* (rk->fourth_Prev + 2 * rk->third_Prev + 2 * rk->second_Prev
					+ CurrentValue);

	rk->fourth_Prev = rk->third_Prev;
	rk->third_Prev = rk->second_Prev;
	rk->second_Prev = rk->Prev;

	return rk->Prev;
}

/******************************************************************************/
/* Function: GyroTask */
/******************************************************************************/
/*! \brief Gyro Task measures the angular rate and calculates the angle
 *
 * \author zursr1
 *
 * \version 0.0.1
 *
 * \date 11.04.2014 Function created
 *
 *
 *******************************************************************************/

static void GyroTask(void* pvParameters) {

	uint32_t t_start = 0;
	uint32_t t_end = 0;
	uint32_t t_delta = 0;
	char Buffer[4] = {0};

	int16_t outGyr = 0;

	t_start = xTaskGetTickCount();

	calculateGyroOffsets();
	calculateDrift();

	/* for ever */
	for (;;) {

		//calc Drift here when message stillstanding arrived

		vTaskDelay(10 / portTICK_RATE_MS);

		outGyr = getGyrValues();

		t_end = xTaskGetTickCount();

		t_delta = t_end - t_start;
		t_start = t_end;

		calculateAngle(outGyr, t_delta);

		itoa((int) round(yaw), Buffer);
		USARTPrint(Buffer, strlen(Buffer) + 1);

		//SWV_printfloat(yaw, 4);


	}
}

/* ****************************************************************************/
/* End : GyroTask */
/* ****************************************************************************/

/**
 * @brief  This function sends numbers to the serial wire viewer.
 * @param  number: number to be displayed on SWV
 * @param  digits: number of digits after decimal point
 * @retval None
 */

void USARTPrint(const char *ToSend, unsigned char length) {
	unsigned int i;

	/* Output a message  */
	for (i = 0; i < length; i++) {
		USART_SendData(USART1, (uint16_t) *ToSend++);
		/* Loop until the end of transmission */
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC ) == RESET) {
		}
	}
}

// Eigenimplementation von itoa(int to ascii)
void itoa( int z, char* Buffer ){
 int i = 0;
 int j;
 char tmp;
 unsigned u;

 //Vorzeichen berücksichtigen
 if( z < 0 ) {
 Buffer[0] = '-';
 Buffer++;
 u = ( (unsigned)-(z+1) ) + 1;
 }
 else {
 u = (unsigned)z;
 }
 //Konvertieren der Zeichen
 do {
 Buffer[i++] = '0' + u % 10;
 u /= 10;
 } while( u > 0 );

 //Zusammensetzen und spiegeln
 for( j = 0; j < i / 2; ++j ) {
 tmp = Buffer[j];
 Buffer[j] = Buffer[i-j-1];
 Buffer[i-j-1] = tmp;
 }
 Buffer[i] = '\0';
}
