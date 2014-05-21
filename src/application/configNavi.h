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

/* ----------------------- module constant declaration -----------------------*/
//#define SET_ROBO_BIG

#ifndef SET_ROBO_BIG
/* small robot */
#define ROBO_BIG 	( 0 )
/* big robot */
#else
#define ROBO_BIG 	( 1 )
#endif

/* Tag Positions for the red Team  */
#define X_TAG20_RED			( 3060 )
#define Y_TAG20_RED			( 1000 )
#define X_TAG21_RED			( -60 )
#define Y_TAG21_RED			( 2060 )
#define X_TAG22_RED			( -60 )
#define Y_TAG22_RED			( -60 )

/* Tag Positions for the yellow Team  */
#define X_TAG20_YELLOW		( -60 )
#define Y_TAG20_YELLOW		( 1000 )
#define X_TAG21_YELLOW		( 3060 )
#define Y_TAG21_YELLOW		( -60 )
#define X_TAG22_YELLOW		( 3060 )
#define Y_TAG22_YELLOW		( 2060 )

/*Starting angle*/
#define STARTANGLE			( 270 )

/* max radius in which the new point could be */
#define TRUSTRADIUS			( 400 )

/* size of the Playfield */
#define XPLAYFIELD			( 3000 )
#define YPLAYFIELD			( 2000 )
/* ------------------------- module type declaration -------------------------*/

/* ------------------------- module data declaration -------------------------*/

/* ----------------------- module procedure declaration ----------------------*/

/* ****************************************************************************/
/* End Header : configNavi.h */
/* ****************************************************************************/

#endif /* CONVIGNAVI_H_ */
