//------------------------------------------------------------------------------
//  $Log: mmc_mmp.h,v $
//  Revision 1.6  2006/06/08 10:00:03  fch
//  Update CVS comment for last commit
//
//  Revision 1.5  2006/06/08 09:17:14  fch
//  Reintroduction of mmp_mmcEnterDedicated to suspend LLC when MMC enters dedicated (Mantis #437)
//
//  Revision 1.4  2006/05/23 10:51:30  fch
//  Update Copyright
//
//  Revision 1.3  2006/04/20 13:50:47  fch
//  Suppress mmp_mmcEnterDedicated and mmp_mmcExitDedicated
//
//  Revision 1.2  2005/12/29 17:38:47  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is property of Coolsand. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or 
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright Coolsand (C) 2003-2006

================================================================================
*/

/*
================================================================================
  File       : MMC_MMP.H
--------------------------------------------------------------------------------

  Scope      : Structures and functions prototypes for MMC <=> MMP exchanges
  History    :
--------------------------------------------------------------------------------
  Aug 18 04  |  FCH   | Creation            
================================================================================
*/

#ifndef __MMC_MMP_H__
#define __MMC_MMP_H__
 
#include "sxs_type.h"
#include "mm_def.h"


/*
================================================================================
AUTHENTICATION AND CIPHERING REJECT
================================================================================
*/

  /*
  ** Associated function prototype
  */
void mmc_mmpAuthAndCiphRej (void);


/*
================================================================================
COMBINED GPRS procedures (ATTACH and RAU)
================================================================================
  Structure  : mmp_mmcCombRes_t
--------------------------------------------------------------------------------
  Direction  : MMP => MMC
  
  Scope      : MMP gives MMC the result of a combined GPRS procedure
  
  Comment    : If Cause equal 0xFFFF this means successful,
               Lai provides a pointer to the received LAI and MsId provide a
               pointer to the received MS identity. MsId may be null if no
               identity was present in the message
================================================================================
*/
typedef struct
{
  u16   Cause;
  u8    UpdateType; // Update Type used in the RAU REQ before RAU REJ
  u8    IdSize;   // Size of the Identity pointed by MsId
  u8    *Lai;
  u8    *MsId;
} mmp_mmcCombRes_t;

  /*
  ** Possible values for Cause
  */
#define MMC_MMP_COMB_SUCCESS   0xFFFF
#define MMC_MMP_COMB_GEN_FAIL  0xFFFE  // Generic FAILURE
// + all causes defined in mm_def.h

  /*
  ** Possible values for UpdateType
  */
#define MMC_MMP_ATTACH            0 // for combined GPRS ATTACH
#define MMC_MMP_RA_UPDATE         1 // for normal RAU
#define MMC_MMP_COMB_RA_UPDATE    2 // for combined RAU (2 cases)
#define MMC_MMP_PERIODIC_UPDATING 3 // for periodic RAU
#define MMC_MMP_NWK_DETACH        4 // for Detach from the network



  /*
  ** Associated function prototype
  */
bool mmc_mmpCombResult (mmp_mmcCombRes_t *);




/*
================================================================================
GPRS DETACH
================================================================================
  Structure  : mmc_mmpDetach
--------------------------------------------------------------------------------
  Direction  : MMP => MMC
  
  Scope      : MMP informs MMC of the start or the end of a combined GPRS detach
  
  Comment    : 
================================================================================
*/
#define MMC_MMP_DETACH_STARTED    0
#define MMC_MMP_DETACH_ENDED      1

void mmc_mmpDetach              ( u8 );



/*
================================================================================
OTHER MMC-MMP INTERWORKING FUNCTIONS
================================================================================
*/
  // Call from MMP
void mmc_mmpSetLocUpNeeded      ( void );
void mmc_mmpStartT3212          ( void );
void mmc_mmpStaNotUpdated       ( void );


  // Call from MMC
bool mmp_mmcCombImsiAtt         ( void );
void mmp_mmcInvalidGSim         ( void );
void mmp_mmcEnterDedicated      ( void );

#define mmp_mmcAuthRej  mmp_mmcInvalidGSim


#endif  // __MMC_MMP_H__
