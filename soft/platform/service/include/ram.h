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

#ifndef _RAM_H_
#define _RAM_H_

#include <csw.h>


#define EV_RAM_MODEM_COM_OPENED_IND                         1213
#define EV_RAM_MODEM_COM_CLOSED_IND                         1214
#define EV_RAM_MODEM_TRANS_BEGIN_IND                        1215
#define EV_RAM_MODEM_TRANS_OVER_IND                         1216



typedef enum _COMM_STATE_
{
    COMM_IDLE = 0,
    COMM_USED = 1,
} COMM_STATE;


typedef struct _RAM_COMMUNICATION_MANAGE
{
    HANDLE RamManageSem ;
    DRV_UARTID UartID;
    COMM_STATE CommState;
    DRV_UARTCFG drv_uartcfg;
    VOID *pRamModemApiMsg;
} RAM_COMMUNICATION_MANAGE;



INT32 RAM_ModemTaskStart(UINT8  ModemTaskPriority);
VOID  RAM_MmiEventProc(CFW_EVENT *pEvent);
INT32 RAM_ModemOpen(VOID );
INT32 RAM_ModemClose(VOID );
VOID RAM_GetModemManageInfo(RAM_COMMUNICATION_MANAGE *pModemCmuManage);
VOID RAM_StopTrans(VOID);

#endif



