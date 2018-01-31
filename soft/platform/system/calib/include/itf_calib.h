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



#ifndef __ITF_CALIB_H__
#define __ITF_CALIB_H__

#define CALIB_NST_BASE                           0xc0

#define CALIB_NST_SE_BER_CONNECT_REQ             ( CALIB_NST_BASE + 0 )
#define CALIB_NST_SE_BER_CONNECT_RSP             ( CALIB_NST_BASE + 1 )
#define CALIB_NST_SE_BER_CONFIGRURE_REQ          ( CALIB_NST_BASE + 2 )
#define CALIB_NST_SE_BER_CONFIGRURE_RSP          ( CALIB_NST_BASE + 3 )
#define CALIB_NST_SE_BER_START_LOG_REQ           ( CALIB_NST_BASE + 4 )
#define CALIB_NST_SE_BER_START_LOG_RSP           ( CALIB_NST_BASE + 5 )
#define CALIB_NST_SE_BER_STOP_LOG_REQ            ( CALIB_NST_BASE + 6 )
#define CALIB_NST_SE_BER_STOP_LOG_RSP            ( CALIB_NST_BASE + 7 )
#define CALIB_NST_SE_BER_LOOP_AB_ORDER_REQ       ( CALIB_NST_BASE + 8 )
#define CALIB_NST_SE_BER_LOOP_AB_ORDER_RSP       ( CALIB_NST_BASE + 9 )
#define CALIB_NST_SIMU_LOG_REQ                   ( CALIB_NST_BASE + 10 )
#define CALIB_NST_SIMU_LOG_RSP                   ( CALIB_NST_BASE + 11 )
#define CALIB_NST_SE_BER_DISCONNECT_REQ          ( CALIB_NST_BASE + 12 )
#define CALIB_NST_SE_BER_DISCONNECT_RSP          ( CALIB_NST_BASE + 13 )
#endif

