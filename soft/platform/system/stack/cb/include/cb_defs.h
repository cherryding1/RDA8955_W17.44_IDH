//------------------------------------------------------------------------------
//  $Log: cb_defs.h,v $
//  Revision 1.3  2006/05/23 10:36:37  fch
//  Update Copyright
//
//  Revision 1.2  2005/12/29 17:38:40  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or 
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (C) 2003-2006

================================================================================
*/

/*
================================================================================

  File       : CB_DEFS.H
--------------------------------------------------------------------------------

  Scope      : CB constants, macros and internal structures defintion  

  History    :
--------------------------------------------------------------------------------
  Dec 07 04  |  FCH   | Creation            
================================================================================
*/
#ifndef __CB_DEFS_H__
#define __CB_DEFS_H__

#include "sxs_io.h"     // Trace constants
#include "cmn_defs.h"   // Common definitions

#include "itf_cb.h"
#include "itf_api.h"
#include "cb_msg.h"
#include "cb_trc.h"


//============================================================================
//  CONSTANTS
//============================================================================
#define CB_LAI_SIZE   (CB_PLMN_SIZE + CB_LAC_SIZE)
#define CB_PLMN_SIZE  3
#define CB_LAC_SIZE   2

  // Block constants
#define CB_FIRST_BLK          0x00
#define CB_SECOND_BLK         0x01
#define CB_THIRD_BLK          0x02
#define CB_FOURTH_BLK         0x03
#define CB_FIRST_SCHED_BLK    0x08
#define CB_NULL_BLK           0x0F
#define CB_BLK_MSK            0x0F

#define CB_LB                 0x10

#define CB_LPD                0x20
#define CB_LPD_MSK            0x60

#define CB_BLK_SIZE           23
#define CB_BLK_DATA_SIZE      22
#define CB_BLK_DATA_OFFSET    1

  // Schedule message constants
#define CB_SCHED_TYPE_OFF     0
#define CB_SCHED_TYPE_MSK     0xC0
#define CB_SCHED_TYPE         0x00

#define CB_SCHED_SLOTNUM_MSK  0x3F
#define CB_SCHED_SLOTNUM_MIN  1
#define CB_SCHED_SLOTNUM_MAX  48
#define CB_SCHED_BEGSLOTNUM_OFF 0
#define CB_SCHED_ENDSLOTNUM_OFF 1

#define CB_NB_MAX_SLOT  48

#define CB_SCHED_BITMAP_OFF   2
#define CB_SCHED_DSC_OFF      8


  // CB message constants (message of 88 bytes)
#define CB_SERNUM_MSB_OFFSET  0
#define CB_SERNUM_LSB_OFFSET  1
#define CB_MID_MSB_OFFSET     2
#define CB_MID_LSB_OFFSET     3
#define CB_DCS_OFFSET         4
#define CB_PAGEPAR_OFFSET     5
#define CB_DATA_OFFSET        6

#define CB_FIRST_BLK_DATA_SIZE  (CB_BLK_DATA_SIZE - CB_DATA_OFFSET)
#define CB_OTHER_BLK_DATA_SIZE  CB_BLK_DATA_SIZE

    // Geographical Scope constants
#define CB_GS_MSK             0xC000
#define CB_GS_PLMN_WIDE       0x4000
#define CB_GS_LA_WIDE         0x8000
#define CB_GS_CELL_WIDE1      0x0000
#define CB_GS_CELL_WIDE2      0xC000


  // Number of data bytes in a SMSCB page
#define CB_SMSCB_PAGE_SIZE    (CB_FIRST_BLK_DATA_SIZE + (3*CB_OTHER_BLK_DATA_SIZE))

/*
** Shortcuts
*/
#define CB_IMSG_IL gp_cb_Data->IMsg->B.IL


/*
** Miscellaneous
*/
#define CB_NOT_FOUND    0xFF
#define CB_NB_MAX_PAGE  15

#define CB_MAX_CHAR_PER_PAGE  93

  
#define CB_IS_8BITALPHABET(_dcs)  ( ( (_dcs & 0xCC) == 0x44) ||   \
                                    ( (_dcs & 0xF4) == 0xF4) )

  
#define CB_IS_UCS2(_dcs)          ( (_dcs == 0x11) ||             \
                                    ( (_dcs & 0xCC) == 0x48) )


//============================================================================
//  INTERNAL STRUCTURES AND ASSOCIATED CONSTANTS
//============================================================================
typedef cb_ActivateReq_t      cb_Cfg_t;
#define CB_ITF_MSG  API_SMSCB_ITF_MSG
#define CB_ITF_PAGE API_SMSCB_ITF_PAGE

  //-------------------------------------------------------
  //  Cell Parameters
  //-------------------------------------------------------
typedef struct
{
  u16   Arfcn;
  u8    Bsic;
  u8    Lai[CB_LAI_SIZE];
} cb_CellPar_t;


  //-------------------------------------------------------
  //  SMS CB messages
  //-------------------------------------------------------

  // History of the SMS CB
typedef struct
{
  bool          InUsed;
  u8            DCS;
  u16           MId;
  u16           SerNum;
  cb_CellPar_t  Cell;
} cb_SmsCbHist_t;

#define CB_HISTORY_SIZE   20    // Number of SMSCB in the history
                                // If changed to more than 254, change CB_NOT_FOUND
                                // and the type of the index used to scan history
#define CB_HIST_INUSED    TRUE  // Used to know if an history elements is
                                // free or not

  // SMS CB in reception
typedef struct
{
  Msg_t *Page[CB_NB_MAX_PAGE];  
}cb_PageList_t;

#if 0
typedef union
{
  Msg_t         *ItfMsg;    
  cb_PageList_t *ItfPage;   
                            
}cb_SmsCbItf_t;
#endif
  
typedef struct
{
  u16             SerNum; // Serial number
  u16             MId;
  u8              DCS;
  u16             Pages;  // Bit map of the pages remaining to receive
  cb_PageList_t*  PageList;
#if 0
  cb_SmsCbItf_t Msg;    // Message
#endif
} cb_SmsCbCtx_t;

#define CB_SMSCB_CTX_SIZE   8   // Number of SMS CB in reception
                                // If changed to more than 254, change CB_NOT_FOUND
                                // and the type of the index used to scan SMSCB Ctx


  //-------------------------------------------------------
  //  Schedule messages
  //-------------------------------------------------------
typedef struct
{
  u8  FirstPos;
  u8  LastPos;
  u16 BeginSlot;
  u16 EndSlot;
  u8  Slot[CB_NB_MAX_SLOT];
} cb_SchedCtx_t;










#define CB_SLOT_MOD   (((26*51*2048)/51)/8)

#define CB_SCHEDCTX_SLOT_MSK    0x3F
#define CB_SCHEDCTX_DSC_MSK     0xC0

#define CB_SCHEDCTX_DSC_FREE    ( 0 << 6 )
#define CB_SCHEDCTX_DSC_TOREAD  ( 1 << 6 )
#define CB_SCHEDCTX_DSC_READ    ( 2 << 6 )
#define CB_SCHEDCTX_DSC_REP     ( 3 << 6 )

//============================================================================
//  MACROS
//============================================================================
#define CB_LPD_NOT_CORRECT(_x)    ( (((_x) & CB_LPD_MSK) != CB_LPD) ? TRUE : FALSE)

#define CB_SCHED_SLOTNUM_OUTOFRANGE(_x)                                       \
          (( ((_x) < CB_SCHED_SLOTNUM_MIN) || ((_x) > CB_SCHED_SLOTNUM_MAX) ) \
          ? TRUE : FALSE)


#endif // endif __CB_DEFS_H__


