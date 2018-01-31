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

#ifndef __HTP_H__
#define __HTP_H__

#ifdef __cplusplus
extern "C"
{
#endif

enum
{
    HTP_CONNECT_SUCCEED = 0,
    HTP_CONNECT_FAILED,
};

typedef struct
{
    u_int8 result;
} gatt_tester_conn_cnf_t;

typedef struct
{
    u_int16 reqCode;
    u_int16 payLoadLen;
    u_int8  result;
    u_int8* rspPayLoad;
} gatt_tester_req_rsp_t;

int GATT_Tester_Connect(t_bdaddr address);

int GATT_Tester_Disconnect(t_bdaddr address);

int GATT_Tester_DiscoverAllService(u_int8 type, u_int8* value, u_int8 valueLen);

int GATT_Tester_DiscoverAllCharacteristic(u_int16 StartHandle, u_int16 EndHandle);

int GATT_Tester_ReadCharValue(u_int16 attHandle, u_int8 isLongAtt, u_int16 offset);

int GATT_Tester_ReadCharValueByUUID(u_int8 *uuid_l, u_int16 uuid_s);

int GATT_Tester_ReadMultiple(u_int8* attHandle, u_int8 length);

int GATT_Tester_GetAllDescriptor(u_int16 startHandle, u_int16 endHandle);

int GATT_Tester_WriteCharValue(u_int16 attHandle, void *value, u_int16 length);

int GATT_Tester_SignedWrite(u_int16 attHandle, void *value, u_int16 length);

int GATT_Tester_WriteWithoutRsp(u_int16 attHandle, void *value, u_int16 length);

int GATT_Tester_ExchangeMTU();

int GATT_Tester_FindIncludeService();

int GATT_Tester_GetManufactureName();

#ifdef __cplusplus
}
#endif

#endif