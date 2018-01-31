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

#ifndef _GATT_IAS_H_
#define _GATT_IAS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define IAS_ATT_MTU     23

typedef struct
{
    u_int8 result;
} ias_connect_cnf_t;

//APIDECL1 t_api APIDECL2 ias_connect(t_bdaddr address);

t_api rdabt_gatt_ias_process_msg(u_int16 msgId, void *data);
void ias_init();

#ifdef __cplusplus
}
#endif

#endif