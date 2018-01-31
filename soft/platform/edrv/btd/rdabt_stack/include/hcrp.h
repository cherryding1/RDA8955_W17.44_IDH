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


#ifndef __RDABT_HCRP_H__
#define __RDABT_HCRP_H__

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_HCRP_BUFFER_SIZE 4096
#define PJL_HEAD_LENGTH    1200


typedef struct
{
    u_int16 cid;
    u_int16 result;
    t_bdaddr bdaddr;
} hcrp_connect_cnf_msg_t;

typedef struct
{
    u_int16 result;
    u_int32 credit_num;
} hcrp_credit_request_rsp_t;

int Hcrp_Start_Signal_Connect(t_bdaddr address);
u_int8 Hcrp_Start_Data_Connect(void);
int Hcrp_Disconnect_Signal_Channel(void);
void Hcrp_Disconnect_Data_Channel(void);

APIDECL1 t_api APIDECL2  Hcrp_CR_DataChannelCreditGrant(void);
APIDECL1 t_api APIDECL2  Hcrp_CR_DataChannelCreditRequest(void);
APIDECL1 t_api APIDECL2  Hcrp_CR_GetLPTStatus(void);


int Hcrp_Send_Data(u_int32 length, u_int8* p_data);


#ifdef __cplusplus
}
#endif

#endif //__RDABT_HCRP_H__

