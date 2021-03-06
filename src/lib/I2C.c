/******************************************************************************/
/*! \I2C.c
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
* \history 25.03.2014 File Created
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
#include "i2c.h"
/* ------------------------- module data declaration -------------------------*/
uint8_t i2c_timeout_flag = 0;
/* ----------------------- module procedure declaration ----------------------*/
void initI2C(void){
    GPIO_InitTypeDef GPIO_InitStruct;
    I2C_InitTypeDef I2C_InitStruct;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* enable I2C */
    RCC_APB1PeriphClockCmd(I2C_PORT_CLK, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    /* initilaizes I2C_SCL*/
    GPIO_InitStruct.GPIO_Pin = I2C_SCL_PIN;             /* set SCL pin*/
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;           /* set pins to alternate function */
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;/* set GPIO speed */
    GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;     /* set output to open drain --> the line has to be only pulled low, not driven high */
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;   /* enable pull up resistors */
    GPIO_Init(I2C_SCL_PORT,&GPIO_InitStruct); /* enable SCL Pin*/
    GPIO_PinAFConfig(I2C_SCL_PORT, I2C_SCL_SOURCE, I2C_AF);

    /* initilaizes I2C_SDA*/
    GPIO_InitStruct.GPIO_Pin = I2C_SDA_PIN;             /* set SDA pin*/
    GPIO_Init(I2C_SDA_PORT,&GPIO_InitStruct); /* enable SDA Pin*/
    GPIO_PinAFConfig(I2C_SDA_PORT, I2C_SDA_SOURCE, I2C_AF);


    /* initializes I2C error interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = I2C_ERROR_INTERRUPT_CHANNEL;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);


    /* configure I2C Interface */
    I2C_DeInit(I2C_INTERFACE);
    I2C_InitStruct.I2C_ClockSpeed = CLOCK_SPEED;        /* set speed */
    I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;                 /* set I2C mode */
    I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2; /* set 50% duty cycle --> standard */
    I2C_InitStruct.I2C_OwnAddress1 = OWN_ADRESS;        /* own address */
    I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;                /* enable acknowledge when reading */
    I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; /* set address length to 7 bit addresses */
    I2C_Init(I2C_INTERFACE, &I2C_InitStruct);             /* init I2C1 */

    /* enable I2C */
    I2C_Cmd(I2C_INTERFACE, ENABLE);
}


/**
    *************************************************************************************
  * @brief  start a conversation
  * @param  adress: adress of the device
  * @param  direction:  I2C_Direction_Transmitter or I2C_Direction_Receiver
  * @param  timeout: Number of times a while loop checks for status, to prevent deadlock
  * @retval None
  * @note   sets global variable "i2c_timeout_flag" to TRUE if timeout value reached
    **************************************************************************************
  */
static void startByteI2C(uint8_t Address, uint8_t Direction, uint32_t timeout){

    /* timeout variable */
    i2c_timeout_flag = 0;

    /* wait until I2C1 is not busy anymore */
    while(I2C_GetFlagStatus(I2C_INTERFACE, I2C_FLAG_BUSY)){

    }

    /* Send I2C1 START condition */
    I2C_GenerateSTART(I2C_INTERFACE, ENABLE);

    /* wait for I2C1 EV5 --> Slave has acknowledged start condition */
    while(!I2C_CheckEvent(I2C_INTERFACE, I2C_EVENT_MASTER_MODE_SELECT)){ //TODO: Why does slave ack when nothing connected??

    }

    /* Send slave Address for write */
    I2C_Send7bitAddress(I2C_INTERFACE, Address, Direction);

    /* wait for I2C1 EV6, check if
     * either Slave has acknowledged Master transmitter or
     * Master receiver mode, depending on the transmission
     * direction
     */
    if(Direction == I2C_Direction_Transmitter){
        while(!I2C_CheckEvent(I2C_INTERFACE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){

        }
    }
    else if(Direction == I2C_Direction_Receiver){

        while(!I2C_CheckEvent(I2C_INTERFACE, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)){

        }
    }
}

/**
    *************************************************************************************
  * @brief  write one byte
  * @param  Data: Byte to write
  * @param  timeout: Number of times a while loop checks for status, to prevent deadlock
  * @retval None
  * @note   sets global variable "i2c_timeout_flag" to TRUE if timeout value reached
    **************************************************************************************
  */
static void writeByteI2C(uint8_t Data, uint32_t timeout){

    /* timeout variable*/
    i2c_timeout_flag = 0;

    /* write byte*/
    I2C_SendData(I2C_INTERFACE, Data);

    /* wait while the byte has been transmitted */
    while(!I2C_CheckEvent(I2C_INTERFACE, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){

    }
}

/**
    *************************************************************************************
  * @brief  write one byte and stop condition
  * @param  Data: Byte to write
  * @param  timeout: Number of times a while loop checks for status, to prevent deadlock
  * @retval None
  * @note   sets global variable "i2c_timeout_flag" to TRUE if timeout value reached
    **************************************************************************************
  */
static void writeEndByteI2C(uint8_t Data, uint32_t timeout){

    /* timeout variable*/
    i2c_timeout_flag = 0;

    /* write byte*/
    I2C_SendData(I2C_INTERFACE, Data);

    /* waitwhile the byte has been transmitted */
    while(!I2C_CheckEvent(I2C_INTERFACE, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){

    }

    /* send stop condition */
    I2C_GenerateSTOP(I2C_INTERFACE, ENABLE);
}

/**
    *************************************************************************************
  * @brief  read one byte and acknowledge
  * @param  timeout: Number of times a while loop checks for status, to prevent deadlock
  * @retval the received byte
  * @note   sets global variable "i2c_timeout_flag" to TRUE if timeout value reached
    **************************************************************************************
  */
static uint8_t readByteI2C(uint32_t timeout){

    /* timeout variable*/
    i2c_timeout_flag = 0;

    /* enable acknowledge of recieved data */
    I2C_AcknowledgeConfig(I2C_INTERFACE, ENABLE);

    /* wait until one byte has been received */
    while( !I2C_CheckEvent(I2C_INTERFACE, I2C_EVENT_MASTER_BYTE_RECEIVED) ){

    }

    /* read data from I2C data register and return data byte */
    return I2C_ReceiveData(I2C_INTERFACE);
}


/**
    *************************************************************************************
  * @brief  read one byte without acknowledge and end conversation
  * @param  timeout: Number of times a while loop checks for status, to prevent deadlock
  * @retval the received byte
  * @note   sets global variable "i2c_timeout_flag" to TRUE if timeout value reached
    **************************************************************************************
  */
static uint8_t readEndByteI2C(uint32_t timeout){

    /* timeout variable*/
    i2c_timeout_flag = 0;

    /* buffer for the received byte */
    uint8_t ReceivedByte;

    /* disabe acknowledge of received data */
    I2C_AcknowledgeConfig(I2C_INTERFACE, DISABLE);

    /* wait until one byte has been received */
    while( !I2C_CheckEvent(I2C_INTERFACE, I2C_EVENT_MASTER_BYTE_RECEIVED) ){

    }


    /* read data from I2C data register and return data byte */
    ReceivedByte = I2C_ReceiveData(I2C_INTERFACE);

    /* send stop condition */
    I2C_GenerateSTOP(I2C_INTERFACE, ENABLE);

    /* return value */
    return ReceivedByte;
}


/**
    *************************************************************************************
  * @brief  write data to the SPI-Bus
  * @param  SlaveAddr: adress of the slave
  * @param  WriteAddr: Adress of the register
  * @param  pBuffer: data to send
  * @param  NumByteToWrite: number of bytes to send
  * @param  timeout: Number of times a while loop checks for status, to prevent deadlock
  * @retval the received byte
  **************************************************************************************
  */
void writeI2C(SlaveI2C SlaveAddr, uint8_t WriteAddr, uint8_t* pBuffer,uint16_t NumByteToWrite, uint32_t timeout ){

    /* Send the Address of the indexed register */
    startByteI2C(SlaveAddr,I2C_Direction_Transmitter, timeout);

    /* Send the data that will be written into the device (MSB First) */
    writeByteI2C(WriteAddr, timeout);

    /* if there are bytes to send */
    if(NumByteToWrite > 0x00){

        /* send every byte */
        while(NumByteToWrite > 0x01)
        {
            /* send byte */
            writeByteI2C(*pBuffer, timeout);

            /* decrement byte to send */
            NumByteToWrite--;

            /* incrment the buffer position */
            pBuffer++;
        }

        /* send the last byte */
        writeEndByteI2C(*pBuffer, timeout);

    }else{

        /* send stop condition */
        I2C_GenerateSTOP(I2C_INTERFACE, ENABLE);

    }
}

/**
    *************************************************************************************
  * @brief  Reads a block of data
  * @param  SlaveAddr : address of the slave
  * @param  pBuffer : pointer to the buffer that receives the data
  * @param  NumByteToRead : number of bytes to read
  * @param  timeout: Number of times a while loop checks for status, to prevent deadlock
  * @retval None
    **************************************************************************************
  */
void readI2C(SlaveI2C SlaveAddr, uint8_t* pBuffer, uint16_t NumByteToRead, uint32_t timeout ){

  /* Send the Address of the indexed register */
  startByteI2C(SlaveAddr,I2C_Direction_Receiver, timeout);

  /* Receive the data that will be read from the device (MSB First) */
  while(NumByteToRead > 0x01)
  {
    /* read byte */
    *pBuffer = readByteI2C(timeout);

        /* decrement byte to read */
    NumByteToRead--;

        /* increment buffer of the received bytes */
    pBuffer++;
  }

    /* read last byte */
    *pBuffer = readEndByteI2C(timeout);
}


/**
 * \fn  i2c_handleErrorInterrupt
 * \brief   I2C-interrupt-handler
 */
void I2C_ERROR_INTERRUPT(void)
{
    I2C_GenerateSTOP(I2C_INTERFACE, ENABLE);

    I2C_ClearFlag(I2C_INTERFACE, I2C_FLAG_AF);
    I2C_ClearFlag(I2C_INTERFACE, I2C_FLAG_ARLO);
    I2C_ClearFlag(I2C_INTERFACE, I2C_FLAG_BERR);
}

/* ****************************************************************************/
/* End Header : I2C.c													  */
/* ****************************************************************************/
