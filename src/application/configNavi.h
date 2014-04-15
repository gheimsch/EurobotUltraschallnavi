#ifndef CONVIGNAVI_H_
#define CONVIGNAVI_H_

/**
 ******************************************************************************
 * \file	configNavi.h
 ******************************************************************************
 * \brief configurate the typ of Robot (small or big)
 *
 * \author heimg1, zursr1
 *
 * \version 0.0.1
 *
 * \history 14.04.2014 File Created
 *
 *
 */
/* ****************************************************************************/
/* Ultraschallnavi Eurobot 2014 */
/* ****************************************************************************/

/* --------------------------------- imports ---------------------------------*/
#define SET_ROBO_BIG

#ifndef SET_ROBO_BIG
//small robot
#define ROBO_BIG 	( 0 )
#else
#define ROBO_BIG 	( 1 )
#endif
/* ----------------------- module constant declaration -----------------------*/

/* ------------------------- module type declaration -------------------------*/

/* ------------------------- module data declaration -------------------------*/

/* ----------------------- module procedure declaration ----------------------*/

/* ****************************************************************************/
/* End Header : configNavi.h */
/* ****************************************************************************/

#endif /* CONVIGNAVI_H_ */

