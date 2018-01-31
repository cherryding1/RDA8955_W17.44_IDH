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



#if (CHIP_CAP_ON_APB)

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "cs_types.h"
#include "global_macros.h"
#include "cap.h"

#include "spal_cap.h"
#include "spalp_private.h"
#include "spalp_debug.h"


//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------


//=============================================================================
// spal_CapDecodeCrc
//-----------------------------------------------------------------------------
// CAP driver function to decode CRC
//
// @param param SPAL_CAP_CONV_PARAM_T. Cap configuration for crc.
// @param In UINT32*. Pointer to the input buffer.
// @param Out UINT32*. Pointer to the output buffer.
//
//=============================================================================
PUBLIC VOID spal_CapDecodeCrc(SPAL_CAP_CRC_PARAM_T param, UINT32* In, UINT32* Out)
{
    // configure control register
    hwp_cap->Ctrl = param.capCfg;

    // source address data
    hwp_cap->Src_Data_addr = (UINT32)In;

    // launch decoding with the proper number of bits
    hwp_cap->Operation = CAP_BIT_NUMBER(param.nbDecBits) | CAP_START_CRC_DECODING;

    // wait end by pooling
    while(hwp_cap->Status & CAP_ENABLE);

    // Get results
    Out[0] = hwp_cap->CRC_code_LSB;
    Out[1] = hwp_cap->CRC_code_MSB;
}



//=============================================================================
// spal_CapEncodeCrc
//-----------------------------------------------------------------------------
// CAP driver function to encode CRC
//
// @param param SPAL_CAP_CONV_PARAM_T. Cap configuration for crc.
// @param In UINT32*. Pointer to the input buffer.
// @param Out UINT32*. Pointer to the output buffer.
//
//=============================================================================
PUBLIC VOID spal_CapEncodeCrc(SPAL_CAP_CRC_PARAM_T param, UINT32* In, UINT32* Out)
{
    // configure control register
    hwp_cap->Ctrl = param.capCfg;

    // source address data
    hwp_cap->Src_Data_addr = (UINT32)In;

    // launch encoding with the proper number of bits
    hwp_cap->Operation = CAP_BIT_NUMBER(param.nbEncBits) | CAP_START_CRC_ENCODING;

    // wait end by pooling
    while(hwp_cap->Status & CAP_ENABLE);

    // Get results
    Out[0] = hwp_cap->CRC_code_LSB;
    Out[1] = hwp_cap->CRC_code_MSB;
}

//=============================================================================
// spal_CapConvoluate
//-----------------------------------------------------------------------------
// CAP driver function to convoluate ???
//
// @param param SPAL_CAP_CONV_PARAM_T. Cap configuration for convolution.
// @param puncTable UINT32*. Pointer to the puncturing table.
// @param In UINT32*. Pointer to the input buffer.
// @param Out UINT32*. Pointer to the output buffer.
//
//=============================================================================
PUBLIC VOID spal_CapConvoluate(SPAL_CAP_CONV_PARAM_T param, UINT32* punctTable, UINT32* In, UINT32* Out)
{
    UINT32  i;
    UINT32  localPuncTable[SPAL_PUNCTURE_TABLE_SIZE];
    UINT32* localPunctTableUncached = MEM_ACCESS_UNCACHED(localPuncTable);

    if (param.nbInBits)
    {

        // configure control register
        hwp_cap->Ctrl = param.capCfg;

        // source address data
        hwp_cap->Src_Data_addr = (UINT32)In;

        // dst address data
        hwp_cap->Dst_Data_addr = (UINT32)Out;

        // Eventual puncturing
        // TODO : table copy might be unnecessary in case of Egprs IR
        if (CS_IS_PUNCTURED(param.capCfg))
        {
            for (i=0; i < SPAL_PUNCTURE_TABLE_SIZE; i++)
                localPunctTableUncached[i] = punctTable[i];

            hwp_cap->Punct_addr = (UINT32)localPunctTableUncached;
        }

        // launch Conv with the proper number of bits
        hwp_cap->Operation = CAP_BIT_NUMBER(param.nbInBits) | CAP_START_CONV;

        // wait end by pooling
        while(hwp_cap->Status & CAP_ENABLE);
    }

}

// TODO:This is the same function as spal_CapConvoluate
//      Just uses other tables
//=============================================================================
// spal_CapConvSrc
//-----------------------------------------------------------------------------
// CAP driver function to convoluate ???
//
// @param param SPAL_CAP_CONV_PARAM_T. Cap configuration for convolution.
// @param In UINT32*. Pointer to the input buffer.
// @param Out UINT32*. Pointer to the output buffer.
//
//=============================================================================
PUBLIC VOID spal_CapConvSrc(SPAL_CAP_CONV_PARAM_T param, UINT32* In, UINT32* Out)
{
    if (param.nbInBits)
    {
        // configure control register
        hwp_cap->Ctrl = param.capCfg;

        // source address data
        hwp_cap->Src_Data_addr = (UINT32)In;

        // dst address data
        hwp_cap->Dst_Data_addr = (UINT32)Out;

        // launch Conv with the proper number of bits
        hwp_cap->Operation = CAP_BIT_NUMBER(param.nbInBits) | CAP_START_CONV;

        // wait end by pooling
        while(hwp_cap->Status & CAP_ENABLE);
    }
}

//=============================================================================
// spal_CapFireCorrection
//-----------------------------------------------------------------------------
// CAP driver function for FIRE code decoding ?
//
// @param Out UINT32*. Pointer to the output buffer.
//
// @return UINT8.
//
//=============================================================================
PUBLIC UINT8 spal_CapFireCorrection(UINT32* Out)
{
    UINT32 error=0;
    UINT32 position=0;

    hwp_cap->Operation = CAP_BIT_NUMBER(224) | CAP_START_TRAPPING;

    // wait end by pooling
    while(hwp_cap->Status & CAP_ENABLE);
    position = ((hwp_cap->Operation>>16) & 0x3FF);
    if (position>11)
    {
        UINT32 idx=0;
        UINT32 rest=0;
        error = hwp_cap->CRC_code_LSB;
        position=224-position+12;
        idx=position>>5;
        rest=position-(idx<<5);
        if (rest<12)
        {
            Out[idx]^=error>>(12-rest);
            Out[idx-1]^=error<<(rest+20);
        }
        else
        {
            Out[idx]^=error<<(rest-12);
        }
        return  0;
    }
    else
        return 1;
}

#endif // (CHIP_CAP_ON_APB)

