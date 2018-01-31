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





#include "cs_types.h"
// common/include
#include "gsm.h"
#include "baseband_defs.h"
// spal public headers
#include "spal_xcor.h"
#include "spal_cap.h"
#include "spal_itlv.h"
#include "spal_debug.h"
#include "spal_mem.h"
// spp public headers
#include "spp_gsm.h"
#include "spp_profile_codes.h"
// spp private headers
#include "sppp_gsm_private.h"
#include "sppp_cfg.h"
#include "sppp_debug.h"

PROTECTED CONST INT8 g_sppPCH_vector_a[3][114] =
{
    {
        -1, 1,-1, 1, 1, 1, 1, 0, 1,-1, 1, 0, 0,-1, 0,-1, 0,-1, 1,-1, 1,  0, 1,-1, 1, 1,
        0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1,-1, 1, 1, 0,-1, 0,-1,-1,-1, 1, -1, 1, 0, 1,-1,
        1, 1, 1, 1, 0, 1,-1, 1,-1, 1, 1, 0, 1, 0, 1, 1, 1,-1, 0,-1,-1, -1, 0,-1, 1, 0,
        1, 0, 1, 1, 1, 1, 0, 1,-1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1,-1, 0, -1, 1,-1,-1,-1,
        1,-1, 1, 0, 1, 1, 1, 1, 0, 1
    },
    {
        0, 1,-1, 1,-1,-1,-1, 1,-1, 1, 0,-1, 0,-1, 0,-1, 1,-1, 1, 0, 1, -1, 1, 1, 0, 1,
        0, 1,-1, 1,-1, 1,-1, 0,-1,-1,-1, 1,-1,-1, 0,-1,-1,-1, 0,-1, 1,  0, 1, 0, 1, 1,
        1, 1, 0, 1,-1, 1, 0, 1,-1, 0,-1, 0,-1, 0,-1,-1, 0,-1,-1,-1, 1, -1, 1, 0, 1, 0,
        1,-1, 1, 1, 0, 1, 1, 1,-1, 1,-1, 1,-1, 0,-1, 1,-1,-1,-1,-1,-1, -1,-1,-1, 1,-1,
        1, 0, 1,-1, 1, 1, 0, 1, 0, 1
    },
    {
        1, 1, 1, 0, 1,-1, 1, 1, 0,-1, 0,-1,-1,-1, 1,-1, 1, 0, 1,-1, 1, -1, 1, 1, 0, 1,
        1, 1,-1, 1, 1, 0, 1, 0, 1, 1, 1,-1, 0,-1, 1,-1, 0,-1, 1, 0, 1,  0, 1, 0, 1, 1,
        0, 1,-1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1,-1, 0,-1,-1,-1, 1,-1, 1, -1, 1, 0, 1,-1,
        1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0,-1, 0,-1, 0, -1, 1,-1, 1, 0,
        1, 1, 1, 1, 0, 1, 0, 1, 0, 1
    }
};


PROTECTED CONST INT8 g_sppPCH_vector_b1[3][27]=
{
    { 1, 1,-1, 1, 1, 1, 1, 1,-1, 1, 1, 1,-1,-1, 1,-1,-1, 1,-1,-1, 1,-1, 1,-1, 1, 1, 0},
    { 1, 1,-1,-1,-1, 1, 1,-1, 1,-1,-1,-1, 1, 1, 1, 1, 1, 1,-1, 1,-1,-1,-1,-1,-1, 0, 0},
    {-1,-1,-1,-1,-1, 1,-1, 1, 1,-1,-1, 1, 1,-1, 1,-1,-1, 1,-1, 1, 1,-1, 1,-1, 1,-1,-1}

};

PROTECTED CONST INT8 g_sppPCH_vector_b2[3][27]=
{
    { 1,-1,-1,-1, 1,-1, 1,-1,-1, 1,-1, 1, 1, 1, 1, 1, 1,-1,-1, 1, 1, 1, 1, 1, 1,-1, 0},
    { 1,-1,-1,-1,-1,-1, 1, 1,-1, 1,-1, 1,-1,-1, 1, 1, 1, 1,-1,-1,-1,-1, 1, 1, 1, 0, 0},
    { 1, 1,-1, 1,-1, 1,-1,-1,-1,-1, 1, 1,-1,-1,-1, 1,-1,-1, 1, 1, 1,-1, 1,-1, 1, 1,-1}
};


PROTECTED CONST INT8 g_sppPCH_vector_b3[3][27]=
{
    { 1, 1,-1,-1, 1,-1, 1,-1,-1,-1,-1,-1, 1,-1,-1,-1, 1, 1, 1,-1, 1,-1, 1,-1,-1,-1, 0},
    { 1, 1,-1,-1, 1,-1,-1, 1, 1,-1, 1,-1,-1, 1,-1,-1, 1, 1,-1,-1, 1,-1,-1, 1, 1, 0, 0},
    {-1, 1, 1, 1, 1, 1,-1, 1, 1, 1, 1, 1,-1,-1,-1, 1,-1, 1, 1, 1,-1, 1, 1, 1, 1, 1,-1}

};

PROTECTED CONST INT8 g_sppPCH_vector_b4[3][27]=
{
    { 1,-1,-1, 1, 1, 1, 1, 1,-1,-1, 1,-1,-1, 1,-1, 1,-1,-1, 1, 1, 1, 1, 1, 1,-1, 1, 0},
    { 1,-1,-1,-1, 1, 1,-1,-1,-1, 1, 1, 1, 1,-1,-1,-1, 1, 1,-1, 1, 1,-1, 1,-1,-1, 0, 0},
    { 1,-1, 1,-1, 1, 1,-1,-1,-1, 1,-1, 1, 1,-1, 1,-1,-1,-1,-1, 1,-1, 1, 1, 1, 1,-1,-1}
};


PROTECTED CONST UINT8 g_sppPCH_b_pos[3][27]=
{
    {7,11,12,14,16,21,26,28,30,35,40,42,49,56,63,65,70,74,77,79,84,91,93,98,107,112,0},
    {0,10,12,14,19,24,26,33,40,44,47,49,54,58,61,63,65,68,75,77,82,91,105,110,112,0,0},
    {3,8,10,17,24,31,33,38,42,45,47,49,52,59,61,66,75,79,80,89,94,96,98,103,108,110,112}
};


PROTECTED CONST UINT8 g_sppPCH_size_vector_b[3]= {26,25,27};

extern INT32 spp_DummyPCH_DisAsm(INT8* softBit, INT8* pa, INT32* distance, INT32 cnt);

UINT8 spp_DetectDummyPCH(UINT32 *Buffer, UINT8 Idx, UINT8 Thresh_a, UINT8 Thresh_b)
{
    UINT8* softBit = (UINT8*)Buffer;
    INT32 distance = 0;
    INT32 mean = 0;
    UINT8 i;
    INT8* pa = (INT8*)&g_sppPCH_vector_a[Idx][0];


    //SPP_PROFILE_FUNCTION_ENTRY(spp_DetectDummyPCH);

#if 0
    for (i = 0; i < 114; i++)
    {
        INT8 sign = (((INT8)softBit[i]) >> 7);
        UINT8 value = softBit[i] & 0x7f;

        mean += value;
        distance += (sign ^ value)*(*pa++);
    }
#else
    mean = spp_DummyPCH_DisAsm((INT8*)softBit, pa, &distance, 114);
#endif
    mean /= 114;
    if (distance < (mean*Thresh_a))
    {
        // dummy PCH not detected
        //SPP_PROFILE_FUNCTION_EXIT(spp_DetectDummyPCH);
        return SPP_DUMMY_PCH_NOT_DETECTED;
    }
    else if(Thresh_b == 0)
    {
        return SPP_DUMMY_PCH_AB_DETECTED;
    }
    else
    {
        // compute distance between received buffer and vector b1 & b2
        INT8* pb1 = (INT8*)&g_sppPCH_vector_b1[Idx][0];
        INT8* pb2 = (INT8*)&g_sppPCH_vector_b2[Idx][0];
        INT8* pb3 = (INT8*)&g_sppPCH_vector_b3[Idx][0];
        INT8* pb4 = (INT8*)&g_sppPCH_vector_b4[Idx][0];

        INT32 distance1 = 0;
        INT32 distance2 = 0;
        INT32 distance3 = 0;
        INT32 distance4 = 0;

        for (i = 0; i < g_sppPCH_size_vector_b[Idx]; i++)
        {
            INT8 soft = (INT8)softBit[g_sppPCH_b_pos[Idx][i]];
            INT8 value;

            value = ((soft & 0x7f) ^ (soft >> 7));
            distance1 += value * (*pb1++);
            distance2 += value * (*pb2++);
            distance3 += value * (*pb3++);
            distance4 += value * (*pb4++);
        }

        distance1 = (distance1>distance2)? distance1 : distance2 ;
        distance3 = (distance3>distance4)? distance3 : distance4 ;
        distance1 = (distance1>distance3)? distance1 : distance3 ;

        if (distance1 < (mean*Thresh_b))
        {
            // dummy PCH not detected
            //SPP_PROFILE_FUNCTION_EXIT(spp_DetectDummyPCH);
            //SPP_PROFILE_PULSE(DUMMY_PCH_FOUND);
            return SPP_DUMMY_PCH_A_DETECTED;
        }
        else
        {
            // dummy PCH detected
            //SPP_PROFILE_FUNCTION_EXIT(spp_DetectDummyPCH);
            //SPP_PROFILE_PULSE(DUMMY_PCH_FOUND);
            return SPP_DUMMY_PCH_AB_DETECTED;
        }
    }
}

