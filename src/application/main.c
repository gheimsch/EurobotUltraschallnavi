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

/* Application includes */
#include "PositionTask.h"			/* Own header include */
#include "GyroTask.h"			/* Own header include */
#include "CANGatekeeper.h"			/* Own header include */
#include "SyncTask.h"
#include "UARTPeripherial.h"
#include "ProcessTask.h"
#include "RFCommTask.h"

/* ------------------------- module data declaration -------------------------*/

/* ----------------------- module procedure declaration ----------------------*/

/* ****************************************************************************/
/* End Header : main.c														  */
/* ****************************************************************************/

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

	initGyroTask();
	initUARTPeripherial();
	initRFCommTask();
	initProcessTask();
	initPositionTask();
    //USART_SendData(USART1,'A');
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){
		;
	}
    SendRFMsg("![Hallo]");

    /* Application initializations */
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
