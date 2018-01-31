/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/

#ifndef DEVICE_DRV_H
#define  DEVICE_DRV_H
#if 0
#include "hal_uart.h"
#include "hal_gpio.h"

#define  DEV_NOTIFY         0x81
#define  DEV_SRC            0x88

enum _dev_Action
{
    DEV_READ,
    DEV_WRITE

};
typedef struct  _device
{
    UINT8 port;
    UINT8 RecMbx;
    UINT8  RxUseDMA;
    UINT8  TxUseDMA;
    HAL_UART_CFG_T  uartCfg;
    HAL_UART_IRQ_STATUS_T  mask;
    void (*registered_func)(UINT8 port);
    struct  CircularBuf  *pWriteBuf;
    struct  CircularBuf  *pReadBuf;
    UINT8  Next;
} DEVICE;

typedef enum
{
    NO_PORT,
    PORT1,
    PORT2,
    PORT3
} PORT;
/*typedef enum _dev_type
{
     DEV_UART,
     DEV_USB,
     DEV_I2C

}DEV_TYPE;*/
typedef enum
{
    NO_CLOCK,
    CLOCK_2HZ, //T =500ms
    CLOCK_5HZ,//T = 200ms
    CLOCK_10HZ,//T=100ms
    CLOCK_100HZ,//T=10ms
    CLOCK_200HZ,//T=5ms
} CLOCK_TYPE;
#define READ_BUFF_NULL   3



/*BOOL UART_Open(PORT port,UINT16 Ower_Task);
BOOL UART_Close(PORT port);
UINT16  UART_PutBytes( PORT port, UINT8 *pBuff, UINT16 NumToWrite,UINT16 Owner);
UINT16  UART_GetBytes(  PORT port, UINT8 *pBuff,UINT16 NumToRead,UINT8 *pstate,UINT16 Owner);
void UART_SetDCBConfig(PORT port, UINT32 band, UINT8 flowctrl );
void UART_Register_RX_cb(PORT port,UINT16 owner,void(*callfunc)(UINT16 Ower_Task));*/
VOID  GPIO_INT_Registration(HAL_APO_ID_T gpio, UINT8 type, void(*pfun)());
VOID GPIO_WriteData(UINT8 gpio, UINT8 Val);
//void  write_uart_log( UINT8 *data, UINT16 num, UINT8 rx_tx);

//BOOL Is_Mcd_exist();

VOID I2C_Open(VOID);
VOID I2C_Close(VOID);
#endif
#endif

