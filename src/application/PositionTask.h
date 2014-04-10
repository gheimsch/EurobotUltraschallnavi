#ifndef __APP_POSITION_H_
#define __APP_POSITION_H_
/******************************************************************************/
/*! \file Position.h
******************************************************************************
* \brief Short description of the files function
*
*
* Function : More detailed description of the files function
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
* \todo If u have some todo's for the h-file, add it here
*
*/
/* ****************************************************************************/
/*Ultraschallnavi Eurobot 2014												  */
/* ****************************************************************************/

/* --------------------------------- imports ---------------------------------*/

/* ----------------------- module constant declaration -----------------------*/

/* ------------------------- module type declaration -------------------------*/
//Robo Position struct
typedef struct Position{
	unsigned int x;
	unsigned int y;
	unsigned int r1;
	unsigned int r2;
	unsigned int r3;
}Position;

/* ------------------------- module data declaration -------------------------*/
Position Robo1Pos ;
Position Robo2Pos ;
Position Enemy1Pos ;
Position Enemy2Pos ;
/* ----------------------- module procedure declaration ----------------------*/

/* ****************************************************************************/
/* End Header : Position.h												  */
/* ****************************************************************************/
#endif /* __APP_POSITION_H_ */



