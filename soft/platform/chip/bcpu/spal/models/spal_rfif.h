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


#ifndef __SPAL_RFIF_H__
#define __SPAL_RFIF_H__

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "cs_types.h"
#include "global_macros.h"
#include "spp_gsm.h"


//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------



//======================================================================
// spal_RfIfTxFifoReset
//----------------------------------------------------------------------
/// reset the Tx fifo.
//======================================================================

INLINE VOID spal_RfIfTxFifoReset(VOID)
{
    ;
}

//======================================================================
// spal_RfIfWriteCtrl
//----------------------------------------------------------------------
/// write the RfIf Ctrl register.
/// @param ctrl
//======================================================================

INLINE VOID spal_RfIfWriteCtrl(UINT32 ctrl)
{
    ;
}

//======================================================================
// spal_RfIfTxFifoWrite
//----------------------------------------------------------------------
/// Write to the Rfif Tx fifo. Depending on implementation, this function will
/// directly write to the fifo iof the Rfif interface on APB or write in a
/// software fifo emptied by a dedicated IFC channel.
//
/// @param pFifo UINT32*. Pointer to the software Tx fifo (if used).
/// @param data UINT3*. Data to be written.
//======================================================================
INLINE VOID spal_RfIfTxFifoWrite(UINT32* pFifo, UINT32 data)
{
    ;
}




//======================================================================
// spal_RfIfTxFifoWrite
//----------------------------------------------------------------------
//
/// @param bstIdx
/// @param nbSymbols
/// @param BstMod
/// @param endOfWin
//======================================================================
INLINE VOID spal_RfIfTxFifoSetBurst(UINT8 bstIdx,
                                    UINT16 nbSymbols,
                                    SPP_MODULATION_TYPE_T BstMod,
                                    BOOL endOfWin)
{
    ;
}


#endif // __SPAL_RFIF_H__

