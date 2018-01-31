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
#include "bb_ifc.h"
#include "global_macros.h"

//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------
PUBLIC VOID spal_IfcTxFifoReset(VOID)
{
#if  (CHIP_IFC_RF_TX_CHANNEL==1)
    hwp_bbIfc->ch3_control = BB_IFC_DISABLE;
#endif
}

//======================================================================
// spal_IfcTxFifoSetup
//----------------------------------------------------------------------
/// Configure the Tx dedicate channel to output the data contained in the
/// Tx fifo (if any) through the Rf Interface.
/// @param pFifo. Pointer to the fifo start address
/// @param sizeFifo. Size of the fifo
//======================================================================
PUBLIC VOID spal_IfcTxFifoSetup(UINT32* pFifo, UINT32 sizeFifo)
{

#if  (!CHIP_IFC_RF_TX_CHANNEL)

    ;

#else // (CHIP_IFC_RF_TX_CHANNEL)

    // Only when CH3 exist: setup IFC xfer
    hwp_bbIfc->ch3_control = BB_IFC_DISABLE;
    hwp_bbIfc->ch3_start_addr = (UINT32)pFifo;
    hwp_bbIfc->ch3_tc = sizeFifo;
    hwp_bbIfc->ch3_control = BB_IFC_ENABLE;

#endif //(!CHIP_IFC_RF_TX_CHANNEL)

}

//======================================================================
// spal_IfcDisableRf
//----------------------------------------------------------------------
/// Disable the Ifc channel CH2 used for Rf samples
//======================================================================
PUBLIC VOID spal_IfcDisableRf(VOID)
{
    hwp_bbIfc->ch2_control = BB_IFC_DISABLE;
    hwp_bbIfc->ch2_int_clear = BB_IFC_END_TC | BB_IFC_HALF_TC;
}



//======================================================================
// spal_IfcParamRf
//----------------------------------------------------------------------
/// Set the Channel2 parameters for Rf Ifc
//======================================================================
PUBLIC VOID spal_IfcParamRf(UINT32 mask,
                            UINT32 startAddr,
                            UINT32 endAddr,
                            UINT32 tc,
                            UINT32 control)
{
    hwp_bbIfc->ch2_int_mask = mask;
    hwp_bbIfc->ch2_start_addr = startAddr;
    hwp_bbIfc->ch2_end_addr = endAddr;
    hwp_bbIfc->ch2_tc = tc;
    hwp_bbIfc->ch2_control = control;
}
