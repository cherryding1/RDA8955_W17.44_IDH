//------------------------------------------------------------------------------
//  $Log: itf_il.h,v $
//  Revision 1.3  2006/06/09 07:33:33  aba
//  Bad memory allocation for gprs sms
//
//  Revision 1.2  2005/12/29 17:38:44  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or 
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (c) 2003

================================================================================
*/

/*
================================================================================
  File       : itf_il.h
--------------------------------------------------------------------------------

  Scope      : Definition of interLayer messages body structure

  History    :
--------------------------------------------------------------------------------
  Nov 02 04  |  Aba   | Add radio offset and footer for SM
  May 10 04  |  Fch   | Add radio offset and footer for MMP and SNDCP
  Feb 09 04  |  MCE   | Changed DataLen to an u16
  Aug 12 03  |  MCE   | Creation            
================================================================================
*/
#ifndef __ITF_IL_H__
#define __ITF_IL_H__
#include "sxs_type.h"
#include "itf_defs.h"

/*
** InterLayer body generic definition
** ILHdr_t must be defined in each entity, 
** and must not be bigger than ILHDR_SPECIFIC_SIZE 
*/

typedef union
{
 u8      Standard [ILHDR_SPECIFIC_SIZE] ;
 IlHdr_t Specific ;   // must be defined by each entity
} IlHeader_t ;
 

#ifdef __BORLANDC__
#if (sizeof(IlHdr_t) > ILHDR_SPECIFIC_SIZE)
#error (IlHdr_t) exceeds ILHDR_SPECIFIC_SIZE
#endif
#endif

typedef struct 
{
 IlHeader_t Hdr        ; // layer-to-layer control info  
 u16        DataLen    ; // Nb of bytes of the Radio Data (Offset + Radio data)
 u8         DataOffset ; // Offset of the data of the upper layer
                         // in the Data field
 u8         Pad        ;
 u8         Data[1]    ; // Offset + Radio Data [+ Footer]
} IlBody_t ;

/*
** size of the interlayer body (excluding radio data)
*/
#define ILBODY_SIZE  ( OFFSETOF(IlBody_t, Data) )

/*
** minimum size to allocate for the Data field 
** Size of the radio block (23) + overhead due to 
** allocation of maximum radio header by SMS ( = 
** to L1 SACCH header size )
*/
#define RAD_DATA_MIN_ALLOC  25

#define RAD_HDR_L1_SIZE_SACCH     2   // size of L1 header on SACCH
#define RAD_HDR_L1_SIZE_DCH       0   // size of L1 header on main DCH
#define RAD_HDR_L1_SIZE_COMMONCH  0   // size of L1 header on common channels
#define RAD_HDR_LAP_SIZE_I        3   // size of LAP header in I frames    
#define RAD_HDR_LAP_SIZE_UI       3   // size of LAP header in UI frames

#define RAD_HDR_RLC_SIZE          4   // max size if no segmentation
#define RAD_HDR_MAC_SIZE          2   // MAC header size

#define RAD_HDR_LLC_SIZE          (36+1) // max size for I frame SACK format
#define RAD_FOOTER_LLC_SIZE       PSDATA_FOOTER_SIZE   // FCS size

#define RAD_HDR_SMS_SIZE          19  // max size for SMR + SMC header

/*
** Offset Possible values
*/
// for CM radio messages
#define RAD_OFFS_CM         ( RAD_HDR_L1_SIZE_DCH   + RAD_HDR_LAP_SIZE_I )

// for SMS radio messages
#define RAD_OFFS_SMS        ( RAD_HDR_L1_SIZE_SACCH + RAD_HDR_LAP_SIZE_I )

// for MM radio messages
#define RAD_OFFS_MM         ( RAD_HDR_L1_SIZE_DCH   + RAD_HDR_LAP_SIZE_I )

// for RR I frames on main DCH
#define RAD_OFFS_RR_DCHI    ( RAD_HDR_L1_SIZE_DCH   + RAD_HDR_LAP_SIZE_I )

// for RR I frames on SACCH
#define RAD_OFFS_RR_SACCHI  ( RAD_HDR_L1_SIZE_SACCH + RAD_HDR_LAP_SIZE_I )

// for RR UI frames on SACCH
#define RAD_OFFS_RR_SACCHUI ( RAD_HDR_L1_SIZE_SACCH + RAD_HDR_LAP_SIZE_UI )

// for MMP PDUs
#define RAD_OFFS_MMP        ( RAD_HDR_RLC_SIZE + RAD_HDR_LLC_SIZE )
#define RAD_FOOTER_MMP      ( RAD_FOOTER_LLC_SIZE )

// for SNDCP PDUs
#define RAD_OFFS_SNDCP      RAD_OFFS_MMP
#define RAD_FOOTER_SNDCP    RAD_FOOTER_MMP

// for SM PDUs
#define RAD_OFFS_SM           RAD_OFFS_MMP
#define RAD_FOOTER_SM         RAD_FOOTER_MMP

// for LLC PDUs
#define RAD_OFFS_LLC          RAD_HDR_RLC_SIZE 

// for RLC PDUs ( in RRP_ULDATA_REQ and in RRP_DLDATA_IND )
#define RAD_OFFS_RLC          RAD_HDR_MAC_SIZE

// for API SMS
#define RAD_OFFS_APISMS      ( RAD_HDR_L1_SIZE_SACCH + RAD_HDR_LAP_SIZE_I + RAD_HDR_SMS_SIZE)
#define RAD_OFFS_APISMS_GPRS ( RAD_OFFS_MMP + RAD_HDR_SMS_SIZE )
#define RAD_FOOTER_API_SMS_GPRS RAD_FOOTER_MMP

#endif // __ITF_IL_H__
