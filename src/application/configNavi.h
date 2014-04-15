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
// small robot
#define ROBO_BIG 	( 0 )
// big robot
#else
#define ROBO_BIG 	( 1 )
#endif

#define X_TAG20_RED			( 3000 )
#define Y_TAG20_RED			( 1000 )
#define X_TAG21_RED			( 0 )
#define Y_TAG21_RED			( 2000 )
#define X_TAG22_RED			( 0 )
#define Y_TAG22_RED			( 0 )

#define X_TAG20_YELLOW		( 0 )
#define Y_TAG20_YELLOW		( 1000 )
#define X_TAG21_YELLOW		( 3000 )
#define Y_TAG21_YELLOW		( 0 )
#define X_TAG22_YELLOW		( 3000 )
#define Y_TAG22_YELLOW		( 2000 )
/* ----------------------- module constant declaration -----------------------*/

/* ------------------------- module type declaration -------------------------*/

/* ------------------------- module data declaration -------------------------*/

/* ----------------------- module procedure declaration ----------------------*/

/* ****************************************************************************/
/* End Header : configNavi.h */
/* ****************************************************************************/

#endif /* CONVIGNAVI_H_ */

