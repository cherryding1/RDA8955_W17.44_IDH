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
#include "string.h"

#include "hal_mem_map.h"
#include "sxs_io.h"

// MEMD includes
#include "memd_m.h"

// PAL includes
#include "pal_imei.h"
#include "palp_cfg.h"

// Target includes, to access the factory settings.
#include "tgt_m.h"


#define   IMEI_LENGTH  9


//--------------------------------------------------------------------//
//                                                                    //
//                    PAL FLASH Internal Structure                    //
//                                                                    //
//--------------------------------------------------------------------//

// (see 3GPP TS 23.003)
// (see 24.008 section 10.5.1.4)
//static UINT8 g_palDefaultImeiSv[9] =
//    { 0x03, 0x21, 0x43, 0x65, 0x87, 0x09, 0x21, 0x43, 0xf5 };

/// 359759002514931
/// 359738009553407
/// 352740015810083
/// 356261013740590


PRIVATE CONST UINT8 g_palDefaultImeiSv_4sim[4][9] =
{
    { 0x33, 0x95, 0x57, 0x09, 0x20, 0x15, 0x94, 0x13, 0xf0 },
    { 0x33, 0x95, 0x37, 0x08, 0x90, 0x55, 0x43, 0x70, 0xf0 },
    { 0x33, 0x25, 0x47, 0x00, 0x51, 0x18, 0x00, 0x38, 0xf0 },
    { 0x33, 0x65, 0x62, 0x01, 0x31, 0x47, 0x50, 0x09, 0xf0 }
};

PRIVATE UINT8 g_palImeiSv_sim[4][9];
//--------------------------------------------------------------------//
//                                                                    //
//                       PAL FLASH API functions                      //
//                                                                    //
//--------------------------------------------------------------------//

// check imei number in the range 0...9
PUBLIC BOOL pal_CheckImei(CONST UINT8* imei)
{
    UINT8 i;

    if (((imei[0] >> 4) & 0xf) > 9)
    {
        return FALSE;
    }
    for (i=1; i<8; i++)
    {
        if (((imei[i] >> 4) & 0xf) > 9)
        {
            return FALSE;
        }
        if ((imei[i] & 0xf) > 9)
        {
            return FALSE;
        }
    }
    return TRUE;
}


PUBLIC BOOL pal_GetFactoryImei(UINT8 simIndex,UINT8 *imei)
{
    INT32 result;
    UINT8 imeiSv[9];

    PAL_ASSERT(simIndex < 4, "Invalid simIndex: %d", simIndex);
    result = tgt_GetImeiSv(simIndex,(UINT8*)imeiSv);
    if(result)
    {
        // fail!!!
    }
    // Check IMEI validity: written status and CRC check.
    if(pal_CheckImei(imeiSv))
    {
        memcpy(imei,(UINT8*)imeiSv,9);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

//======================================================================
// Phone's IMEI Getter :
//----------------------------------------------------------------------
/// This function returns the IMEI of the phone. It is usually
/// retrieved from the factory settings of the flash. Check the CRC of
/// the factory settings structure and the "factory step" bit is set,
/// then get the IMEI. Otherwise, return the default IMEI.
///
/// One IMEI is defined
/// per SIM in the case where the phone has several SIMs.
///
/// @param simIndex Index of the SIM whose related IMEI is to be
/// returned. Use 0 when the phone is single-sim.
/// @return Pointer to the IMEI for the sim index passed as a parameter.
//======================================================================

PUBLIC UINT8* pal_GetImei(UINT8 simIndex)
{
    BOOL  ret;
    UINT8* imei;

    imei = (UINT8*)(g_palImeiSv_sim[simIndex]);
    ret = pal_GetFactoryImei(simIndex,imei);
    if(!ret)
    {
        memcpy(imei,(UINT8*)g_palDefaultImeiSv_4sim[simIndex],9);
    }
    return imei;
}

PUBLIC BOOL pal_SaveFactoryImei(UINT8 simIndex, UINT8 *imei)
{
    INT32 result;
    UINT8 imeiSv[9];


    if (!pal_CheckImei(imei))
    {
        return FALSE;
    }
    result = tgt_GetImeiSv(simIndex,imeiSv);
    if(result)
    {
        // fail!!!
    }
    memcpy((UINT8*)imeiSv,imei,9);
    result = tgt_SetImeiSv(simIndex,imeiSv);
    if(result)
    {
        // fail!!!
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}


