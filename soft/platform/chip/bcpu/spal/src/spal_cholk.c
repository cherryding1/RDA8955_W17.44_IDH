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
// chip/include
#include "global_macros.h"
#include "cholk.h"
// spal public header
#include "spal_cholk.h"

#include "spalp_private.h"

//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------
//=============================================================================
// spal_CholkBusy
//-----------------------------------------------------------------------------
/// Checks whether the CHOLK is busy or not.
/// @return BOOL. Return the status of CHOLK,if it is busy or not,if TRUE,CHOLK is busy,else CHOLK is free.
//=============================================================================
PUBLIC BOOL spal_CholkBusy()
{
    volatile  UINT32 busy;
    busy = hwp_cholk->CHOLK_STATUS;
    if (busy != 1)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

//=============================================================================
// spal_CholkCg
//-----------------------------------------------------------------------------
/// Cg (conjugate gradient) method, return the estimated vector x of the function  Ax = b.
///
/// @param RR UINT32*. INPUT.  the address of positive definite matrix.
/// @param bb UINT32*. OUTPUT. the address known vector.
/// @param Len UINT8.INPUT. Matrix dimension indicator .
/// @param Num UINT8.INPUT. Matrix elements number in word.
/// @param Iternum UINT8.INPUT. Matrix calculation iteration number.
/// @param g  UINT32*.OUTPUT. The start address of estimated vector x.

//=============================================================================

PUBLIC VOID spal_CholkCg(UINT32* RR , UINT32 *bb, UINT8 Len,  UINT8 Num,  UINT8 Iternum, UINT32 *g)
{

    spal_InvalidateDcache();
    hwp_cholk->GAIN = 0x1;
    hwp_cholk->ITER_THRE = 0x2;
    hwp_cholk->MCOVA_BASE = (CHOLK_ADDR((UINT32)RR)) >> 2;
    hwp_cholk->MCE_BASE = (CHOLK_ADDR((UINT32)bb)) >> 2;
    hwp_cholk->MCOEF_BASE = (CHOLK_ADDR((UINT32)g)) >> 2;
    hwp_cholk->NUMBER = CHOLK_ROW_NUMBER(Len)|CHOLK_MCOVA_NUMBER(Num -1)|CHOLK_ITER_NUMBER(Iternum -1);
    hwp_cholk->CHOLK_INT = 0x2;
    hwp_cholk->CHOLK_CTRL = 0x3;

    while (spal_CholkBusy()) ;
    hwp_cholk->CHOLK_STATUS = 0;

}

//=============================================================================
// spal_CholkCgStep1
//-----------------------------------------------------------------------------
/// Cg (conjugate gradient) method, return the estimated vector x of the function  Ax = b.
/// Setting and start of Cg module.
///
/// @param RR UINT32*. INPUT.  the address of positive definite matrix.
/// @param bb UINT32*. OUTPUT. the address known vector.
/// @param Len UINT8.INPUT. Matrix dimension indicator .
/// @param Num UINT8.INPUT. Matrix elements number in word.
/// @param Iternum UINT8.INPUT. Matrix calculation iteration number.
/// @param g  UINT32*.OUTPUT. The start address of estimated vector x.

//=============================================================================

PUBLIC VOID spal_CholkCgStep1(UINT32* RR , UINT32 *bb, UINT8 Len,  UINT8 Num,  UINT8 Iternum, UINT32 *g)
{
    spal_InvalidateDcache();
    hwp_cholk->GAIN = 0x1;
    hwp_cholk->ITER_THRE = 0x2;
    hwp_cholk->MCOVA_BASE = (CHOLK_ADDR((UINT32)RR)) >> 2;
    hwp_cholk->MCE_BASE = (CHOLK_ADDR((UINT32)bb)) >> 2;
    hwp_cholk->MCOEF_BASE = (CHOLK_ADDR((UINT32)g)) >> 2;

    hwp_cholk->NUMBER = CHOLK_ROW_NUMBER(Len)|CHOLK_MCOVA_NUMBER(Num -1)|CHOLK_ITER_NUMBER(Iternum -1);
    hwp_cholk->CHOLK_INT = 0x2;
    hwp_cholk->CHOLK_CTRL = 0x3;

}







