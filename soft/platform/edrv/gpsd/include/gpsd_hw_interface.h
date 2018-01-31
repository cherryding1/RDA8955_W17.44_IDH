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
#include "hal_uart.h"
#include "hal_gpio.h"
#include "at_common.h"
#include "at_device_uart.h"

#define CIRCULAR_BUF_LEN	4096
 
#define GET_DATA_BLOCK_LEN(_start,_end,_cycle) ((_start<=_end)?(_end-_start):(_end+_cycle - _start) )
#define STATIC_RX_BUF_LEN	1024
#define STATIC_TX_BUF_LEN	512
#define UART_FIFO_LEN		32
#define MAX_UART_DEV_NUM	2

struct CircularBuf
{
	UINT8 Buf[CIRCULAR_BUF_LEN];	
	UINT16 Get;
	UINT16 Put;
	UINT16 Buf_len;
	UINT8  NotEmpty;
	UINT8  Name[10];
};

struct Uart_hw_buf
{
	UINT8 rx_buf[STATIC_RX_BUF_LEN];	
	UINT8 tx_buf[STATIC_TX_BUF_LEN];	
};


typedef struct  _device
{	 
 UINT8 port;	 
 UINT8 RecMbx;
 UINT8 RxPending; 
 UINT8	RxUseDMA;
 UINT8	TxUseDMA;
 HAL_UART_CFG_T  uartCfg;
 HAL_UART_IRQ_STATUS_T	mask;  
 void (*registered_func)(UINT8 port);
 struct Uart_hw_buf *pUart_hw_buf;
 struct CircularBuf *pWriteBuf;	 
 struct CircularBuf *pReadBuf; 
 UINT8	Next;
} DEVICE;

typedef struct
{
	UINT32				baud; 
	UINT32				dataBits;
	UINT32				stopBits;
	UINT32				parity;
	UINT8				flowControl;
	UINT8				xonChar;
	UINT8				xoffChar;
	UINT32				DSRCheck;
}UARTDCBStruct;

typedef enum
{
    NEMA_MODE,		// 0
    NEMA_MODE_OK,	// 1
    BIN_MODE,		// 2
    BIN_MODE_OK,	// 3
    HOTSTART,		// 4
    COLDSTART,		// 5
    BAND_1M,
    BAND_3M,
    APPARATUS_TEST,
} RAD5900MODE;

typedef struct 
{
    U32 index;
    U8 str[50];
}RAD5900MODE_string;





#endif


