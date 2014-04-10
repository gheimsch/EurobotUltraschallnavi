/******************************************************************************/
/*! \file main.c
******************************************************************************
* \brief Sehr kurzer Beschrieb der Funktion dieses Moduls
*
*
* Function : More detailed description of the files function
*
* Procedures : 	main()
* 				vApplicationTickHook()
* 				vApplicationMallocFailedHook()
* 				vApplicationIdleHook()
* 				vApplicationStackOverflowHook( pxTask, *pcTaskName )
*
* \author zursr1,heimg1
*
* \version 0.0.1
*
* \history 25.03.2014 File Created
*
*
* \ingroup application
*
*/
/* ****************************************************************************/
/* Ultraschallnavi Eurobot 2014												  */
/* ****************************************************************************/

/* --------------------------------- imports ---------------------------------*/

/* Kernel includes */
#include "FreeRTOS.h"			/* must appear before #include task.h */
#include "task.h"

/* Hardware includes */
#include <memPoolService.h>		/* Memory pool manager service */
#include "stm32f4xx.h"

/* BSP includes */
//#include "arm_math.h"

/* Application includes */
#include "default_task.h"
#include "Position.h"			/* Own header include */
#include "GyroTask.h"			/* Own header include */
#include "CANGatekeeper.h"			/* Own header include */
#include <stdio.h>

/* lib includes */
#include "../lib/can.h"		/* Own header include */
#include "../lib/UART.h"	/* Own header include */
#include "../lib/I2C.h"		/* Own header include */

/* ------------------------- module data declaration -------------------------*/
uint32_t t_Start;	// Start Timer of the Measurement
uint32_t t_End;		// End Timer of the Measurement
uint32_t delta_t = 0;	// measured time

int16_t outGyr;		// angular rate
float temp = 0;		// temperature
/* ----------------------- module procedure declaration ----------------------*/
void SWV_printnum(uint32_t);
void SWV_printfloat(float, uint32_t);

/* ****************************************************************************/
/* End Header : main.c														  */
/* ****************************************************************************/

void posRequest(void){
	txNaviPositionResponse(300,300,45,0);
}
/******************************************************************************/
/* Function:  main															  */
/******************************************************************************/
/*! \brief Function to init the libs
*
* Function : More detailed description of the function
*
* \return 0 if the program finished correctly
*
* \author heimg1
*
* \version 0.0.1
*
* \date 25.03.2014 Function created
*
*
* \todo Add the new files and init all tasks
*
*******************************************************************************/
int main(void) {

    /* Configure the NVIC Preemption Priority Bits for STM-Lib V3.1+ */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    /* lib initializations */


    //initUART(); // initialise UART
	initI2C();
	initGyr();

//    initUART(); // initialise UART
//	initI2C();
//	initGyr();

//
//	calculateDrift();

    //USART_SendData(USART1,"Hello World");
    /* Application initializations */
    setFunctionCANListener((CAN_function_listener_t)posRequest, 0x040);
    initCANGatekeeper(); /* have to the last initialisation modul! */

    /* Start the scheduler */
    vTaskStartScheduler();

    /* endless loop */
    for (;;);

    return 0;
}

/* ****************************************************************************/
/* End      :  main			 						  					      */
/* ****************************************************************************/


/* Hooks: */ //TODO remove or add header
void vApplicationTickHook( void )
{

}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

/**
 * @brief   This function sends numbers to the serial wire viewer.
 * @param  number: number to be displayed on SWV
 * @retval None
 */
void SWV_printnum(uint32_t number) {
	uint8_t buf[8 * sizeof(uint32_t)]; // Assumes 8-bit chars.
	uint16_t i = 0;

	//if number is 0
	if (number == 0) {
		ITM_SendChar('0'); //if number is zero
		return;
	}
	//account for negative numbers
	if (number < 0) {
		ITM_SendChar('-');
		number = number * -1;
	}
	while (number > 0) {
		buf[i++] = number % 10; //display in base 10
		number = number / 10;
		//NOTE: the effect of i++ means that the i variable will be at number of digits + 1
	}
	for (; i > 0; i--) {
		ITM_SendChar((char) ('0' + buf[i - 1]));
	}
}

/**
 * @brief  This function sends numbers to the serial wire viewer.
 * @param  number: number to be displayed on SWV
 * @param  digits: number of digits after decimal point
 * @retval None
 */

void SWV_printfloat(float number, uint32_t digits) {
	uint32_t i = 0;
	//handle negative numbers
	if (number < 0.0) {
		ITM_SendChar('-');
		number = -number;
	}
	//round correctly so that uart_printfloat(1.999, 2) shows as "2.00"
	float rounding = 0.5;
	for (i = 0; i < digits; ++i)
		rounding = rounding / 10.0;
	number = number + rounding;

	//extract the integer part of the number and print it
	uint64_t int_part = (uint64_t) number;
	float remainder = (float) (number - (float) int_part);
	SWV_printnum(int_part); //print the integer part
	if (digits > 0)
		ITM_SendChar('.'); //print decimal pint
	uint32_t toprint;
	while (digits-- > 0) {
		remainder = remainder * 10.0;
		toprint = (uint32_t) remainder;
		SWV_printnum(toprint);
		remainder = remainder - toprint;
	}

}
