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

#ifndef BLUETOOTH_LOWENERGY_SMP_API_DECLARED
#define BLUETOOTH_LOWENERGY_SMP_API_DECLARED

#ifdef __cplusplus
extern "C" {
#endif

typedef struct st_smp_ind_msg
{
    u_int8 is_disp;
    u_int32 value;
    void *device;
} smp_ind_msg_t;

enum
{
    LE_STATIC_ADDR = 0,
    LE_NON_RESOLVABLE_ADDR = 1,
    LE_RESOVABLE_ADDR = 2
};

APIDECL1 t_api APIDECL2 SMP_PINResponse(u_int8* pincode,t_bdaddr address);
APIDECL1 t_api APIDECL2 SMP_StartPair(t_bdaddr address);
APIDECL1 t_api APIDECL2 SMP_SecRequest(u_int8 auth_req);
APIDECL1 t_api APIDECL2 SMP_DataSign(u_int16 handle,u_int8 *input,u_int8 *output,u_int8 len);

APIDECL1 t_api APIDECL2 SMP_GenRandomAddress(t_bdaddr address, t_bdaddr *pRanddomAddress, u_int8 type);

//input buffer should contain PDU data, signature
APIDECL1 t_api APIDECL2 SMP_SignatureCheck(u_int16 handle,u_int8 *input,u_int8 len);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif