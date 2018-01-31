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


#define __SAP_CUST_VAR__
#include "sap_cust.h"
#undef  __SAP_CUST_VAR__


/*
==============================================================================
   Function   : sxs_ProtoStackAvail
 ----------------------------------------------------------------------------
   Scope      :
   Parameters : None.
   Return     : TRUE to enable protocol stack, FALSE otherwise.
==============================================================================
*/
bool sxs_ProtoStackAvail (void)
{
#ifdef NO_PROTOSTACK
    return FALSE;
#else
    return TRUE;
#endif
}


/*
==============================================================================
   Function   : sxs_UsimSupported
 ----------------------------------------------------------------------------
   Scope      :
   Parameters : None.
   Return     : TRUE to support USIM, FALSE otherwise.
==============================================================================
*/
bool sxs_UsimSupported (void)
{
#ifdef USIM_SUPPORT
    return TRUE;
#else
    return FALSE;
#endif
}


/*
==============================================================================
   Function   : sxs_SimLimitRfTxPower
 ----------------------------------------------------------------------------
   Scope      :
   Parameters : None.
   Return     : TRUE to allow limit RF Tx power for SIM access, FALSE otherwise.
==============================================================================
*/
bool sxs_SimLimitRfTxPower (void)
{
#ifdef SIM_LIMIT_RF_TX_POWER
    return TRUE;
#else
    return FALSE;
#endif
}

/*
==============================================================================
   Function   : sxs_SimFastAtr
 ----------------------------------------------------------------------------
   Scope      :
   Parameters : None.
   Return     : TRUE to reduce atr timeout time, FALSE otherwise.
==============================================================================
*/
bool sxs_SimFastAtr (void)
{
#ifdef MODULE_GPE
    return TRUE;
#else
    return FALSE;
#endif
}


/*
==============================================================================
   Function   : sxs_IsExtULTBFSupported
 ----------------------------------------------------------------------------
   Scope      :
   Parameters : None.
   Return     : TRUE to support, FALSE otherwise.
==============================================================================
*/
bool  sxs_IsExtULTBFSupported(void)
{
#ifndef STACK_NOT_SUPPORT_EXT_UL_TBF
    return TRUE;
#else
    return FALSE;
#endif
}


