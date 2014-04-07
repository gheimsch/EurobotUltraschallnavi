/******************************************************************************/
/*! \file default_task.c
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
* \author Name
*
* \version 0.0.1
*
* \history dd.mm.jjjj File Created
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
/* Motorcontroller Eurobot 2014												  */
/* ****************************************************************************/

/* --------------------------------- imports ---------------------------------*/

#include <memPoolService.h>		/* Memory pool manager service */
#include "stm32f4xx.h"			/* uC includes */

/* application */
#include "default_task.h"		/* Own header include */


/* ------------------------- module data declaration -------------------------*/

int testVar = 0;	/*!< Detailed description after the member */

/* ----------------------- module procedure declaration ----------------------*/

static void vDefaultTask(void*);


/* ****************************************************************************/
/* End Header : default_task.c												  */
/* ****************************************************************************/



/******************************************************************************/
/* Function:  InitDefaultTask												  */
/******************************************************************************/
/*! \brief Short description of the function
*
* Function : More detailed description of the function
*
* \return None
*
* \author Name
*
* \version 0.0.1
*
* \date dd.mm.jjjj File Created
*
*
* \todo If u have some todo's for the function, add it here
*
* \bug Description of the bug
*
*******************************************************************************/

void InitDefaultTask(void)
{
	/* Do some inits here */


    /* create the task */
    xTaskCreate( vDefaultTask, ( signed char * ) DEFAULT_TASK_NAME, DEFAULT_STACK_SIZE, NULL, DEFAULT_TASK_PRIORITY, NULL );
}

/* ****************************************************************************/
/* End      :  InitDefaultTask												  */
/* ****************************************************************************/



/******************************************************************************/
/* Function:  vDefaultTask													  */
/******************************************************************************/
/*! \brief Short description of the function2
*
* Function : More detailed description of the function2
*
* \param[in] pvParameters  Pointer for given parameter
*
* \return None
*
* \author Name
*
* \version 0.0.1
*
* \date dd.mm.jjjj File Created
*
*
* \todo If u have some todo's for the function, add it here
*
* \bug Description of the bug
*
*******************************************************************************/

static void vDefaultTask(void* pvParameters )
{
    portTickType xLastFlashTime;

    /* We need to initialise xLastFlashTime prior to the first call to
    vTaskDelayUntil(). */
    xLastFlashTime = xTaskGetTickCount();

    /* wait */
    vTaskDelayUntil( &xLastFlashTime, 10 );

    for(;;)
    {
    	/* Code here */

        vTaskDelayUntil( &xLastFlashTime, 500 / portTICK_RATE_MS);
    }
}

/* ****************************************************************************/
/* End      :  vDefaultTask													  */
/* ****************************************************************************/
