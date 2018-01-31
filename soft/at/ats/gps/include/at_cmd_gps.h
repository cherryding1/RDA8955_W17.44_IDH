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

#ifndef __AT_CMD_GPS_H__
#define __AT_CMD_GPS_H__

#ifdef GPS_SUPPORT

struct GPS_STATE
{
    UINT8 on;           //   0: closed, 1: opened
    UINT8 state;        //   0: NOPOS, 1: LBS, 2: GPS
    DOUBLE lat;         //   latitude
    DOUBLE lng;         //   longitude
    UINT8 pEPH[4096];   //   EPH buffer point
    UINT32 EPHlen;      //   EPH size
    UINT8 gpsbinary;    //   0: NEMA,  1: binary
    TM_SYSTEMTIME epht; //   EPH valid time
};

VOID AT_CmdFunc_GPS(AT_CMD_PARA *pParam);
VOID AT_CmdFunc_GPSRD(AT_CMD_PARA *pParam);
VOID AT_CmdFunc_GPSCMD(AT_CMD_PARA *pParam);
VOID AT_CmdFunc_EPH(AT_CMD_PARA *pParam);
VOID AT_CmdFunc_LBS(AT_CMD_PARA *pParam);
void InitGPSState();
void gps_Pro(void *p_arg, u16 size);
void Isuartbin149(char *aBinTemp);
void isbinright(char *aBinTemp, int len);

void process_HTTP_agps(const unsigned char *data, size_t data_len);
void check_finish_HTTP_agps(void);
#define HTTP_URL_SIZE 255

#endif

#endif
