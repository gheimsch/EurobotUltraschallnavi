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

/* Startangle of the Robot */
#define STARTANGLE			( 270 )

/* size of the Playfield */
#define XPLAYFIELD			( 3000 )
#define YPLAYFIELD			( 2000 )
/* ------------------------- module type declaration -------------------------*/

/* ------------------------- module data declaration -------------------------*/
/* ID's of the Tags */
char Tag1[] = "P20";
char Tag2[] = "P21";
char Tag3[] = "P22";

/* ID's of the Receivers of own Robots */
#ifndef SET_ROBO_BIG	/* if the small Robot is activated */
char Receiver1[] = "R42";
char Receiver2[] = "R43";
#else	/* if the big robot is activated */
static char Receiver1[] = "R43";
static char Receiver2[] = "R42";

#endif
/* ID's of the Receivers of Enemy Robots */
char Receiver3[] = "R40";
char Receiver4[] = "R41";
/* ----------------------- module procedure declaration ----------------------*/

/* ****************************************************************************/
/* End Header : configNavi.h */
/* ****************************************************************************/

#endif /* CONVIGNAVI_H_ */
