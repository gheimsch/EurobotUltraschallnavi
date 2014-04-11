#ifndef __LIB_I2C_H_
#define __LIB_I2C_H_
/******************************************************************************/
/*! \file I2C.h
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
#include "stm32f4xx.h"
/* ----------------------- module constant declaration -----------------------*/
/* pin/port and interface configurations */
#define I2C_INTERFACE               I2C1 /* use I2C1 */
#define I2C_PORT_CLK                RCC_APB1Periph_I2C1
#define I2C_AF                      GPIO_AF_I2C1
#define I2C_ERROR_INTERRUPT_CHANNEL I2C1_ER_IRQn
#define I2C_ERROR_INTERRUPT         I2C1_ER_IRQHandler

#define I2C_SCL_PIN         GPIO_Pin_6      /* PB.6 */
#define I2C_SCL_SOURCE      GPIO_PinSource6
#define I2C_SCL_PORT        GPIOB
#define I2C_SDA_PIN         GPIO_Pin_7        /* PB.9 */
#define I2C_SDA_SOURCE      GPIO_PinSource7
#define I2C_SDA_PORT        GPIOB


/* Speed of I2C Bus , 100kHz */
#define CLOCK_SPEED         1000

/* own Adress */
#define OWN_ADRESS          0x20
/* ------------------------- module type declaration -------------------------*/
/* add here the address of the I2C-Slave */
typedef enum{
    AUDIO = 0x94,
    DISPLAY_7Segment = 0x70,
    Accelerometer  	 = 0x32,
    Gyro			 = 0xD4
}SlaveI2C;
/* ------------------------- module data declaration -------------------------*/

/* ----------------------- module procedure declaration ----------------------*/
/* initialize th I2C interface*/
extern void initI2C(void);

/* write data with I2C */
extern void writeI2C(SlaveI2C SlaveAddr, uint8_t WriteAddr, uint8_t* pBuffer, uint16_t NumByteToWrite, uint32_t timeout );

/* read data with I2C */
extern void readI2C(SlaveI2C SlaveAddr, uint8_t* pBuffer, uint16_t NumByteToRead, uint32_t timeout );

/* while-timeout flag, 1 if timeout occured, 0 if no timeout occured */
extern uint8_t i2c_timeout_flag;

/* ****************************************************************************/
/* End Header : I2C.h												  */
/* ****************************************************************************/
#endif /* __LIB_I2C_H_ */
