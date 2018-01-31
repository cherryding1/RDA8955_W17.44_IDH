//------------------------------------------------------------------------------
//  $Log: rlp_defs.h,v $
//  Revision 1.5  2006/05/23 10:37:37  fch
//  Update Copyright
//
//  Revision 1.4  2006/01/27 15:13:34  fch
//  Added interrogation from API of the current RLP system parameters settings
//
//  Revision 1.3  2006/01/27 10:14:45  fch
//  Handling of RLP system parameters provides by user and XID command sending
//
//  Revision 1.2  2005/12/29 17:39:12  pca
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
  File       : RLP_DEFS.H
--------------------------------------------------------------------------------

  Scope      : RLP constants, macros and internal structures defintion  

  History    :
--------------------------------------------------------------------------------
  Mar 31 04  |  FCH   | Creation            
================================================================================
*/
#ifndef __RLP_DEFS_H__
#define __RLP_DEFS_H__

#include "cmn_defs.h"   
#include "sxs_io.h"     

#include "sxr_tls.h"    

#include "rlp_trc.h"    
#include "itf_rlp.h"
#include "rlp_msg.h"    







#define RLP_FRM_SHORT_SIZE  30    
#define RLP_FRM_LONG_SIZE   72    

#define RLP_FCS_SIZE        3     

#define RLP_MAX_WINDOW_LEN  62    

  



#define RLP_TX_RES_HYST     (RLP_MAX_WINDOW_LEN/2)




#define RLP_SN_MOD(_x)  ( ((_x) + RLP_MAX_WINDOW_LEN) % RLP_MAX_WINDOW_LEN )

  
#define RLP_IS_IN_WINDOW(_v, _low, _up)                 \
        ( RLP_SN_MOD(_v - _low) <= RLP_SN_MOD(_up - _low) )

  
#define RLP_INC_SEQNUM(_s)      ( _s = (((_s + 1) == RLP_MAX_WINDOW_LEN) ? 0 : (_s + 1)) )

  
#define RLP_DEC_SEQNUM(_s)      ( _s = ((_s == 0) ? (RLP_MAX_WINDOW_LEN-1) : (_s - 1)) )

  
  
#define RLP_THRESHOLD_ENTER_REJ   (rlp_Data.SysPar.Kim>>1)





  
  
  

#define RLP_DEF_VERSION     0
#define RLP_DEF_KIM         61
#define RLP_DEF_KMI         61
#define RLP_DEF_T1_48       54  
#define RLP_DEF_T1_96       48  
#define RLP_DEF_T1_144      52  
#define RLP_DEF_N2          6
#define RLP_DEF_T2          8
#define RLP_DEF_NA          0   

typedef api_RlpSysPar_t rlp_SysPar_t;

#define RLP_USRPAR_KIM    API_RLP_USRPAR_KIM
#define RLP_USRPAR_KMI    API_RLP_USRPAR_KMI
#define RLP_USRPAR_T1     API_RLP_USRPAR_T1
#define RLP_USRPAR_N2     API_RLP_USRPAR_N2
#define RLP_USRPAR_VERS   API_RLP_USRPAR_VERS
#define RLP_USRPAR_T4     API_RLP_USRPAR_T4
#define RLP_USRPAR_T2     API_RLP_USRPAR_T2
#define RLP_NB_PAR        API_RLP_NB_PAR



  
  
  

  
#define RLP_FRM_TYPE_MSK    0x1F80
#define RLP_UFRM_TYPE       0x1F80
#define RLP_SFRM_TYPE       0x0F80

  
#define RLP_M_BIT_MSK   0x003E
#define RLP_GET_M_BIT(_h)   ( ((_h) & RLP_M_BIT_MSK) >> 1 )

#define RLP_SABM    0x1C
#define RLP_UA      0x06
#define RLP_DISC    0x02
#define RLP_DM      0x18
#define RLP_NULL    0x1E
#define RLP_UI      0x00
#define RLP_XID     0x1D
#define RLP_TEST    0x07

  

  
#define RLP_CMD_FRM   1   
#define RLP_RSP_FRM   0   

#define RLP_CR_BIT_MSK    0x8000
#define RLP_GET_CR(_h)    ( ((_h) & RLP_CR_BIT_MSK) ? RLP_CMD_FRM : RLP_RSP_FRM )

  
#define RLP_PF_BIT_MSK    0x0040
#define RLP_GET_PF(_h)    ( ((_h) & RLP_PF_BIT_MSK) ? 1 : 0 )

  
#define RLP_NR_MSK        0x003F
#define RLP_GET_NR(_h)    ( (sxr_MsbToLsb[(_h) & RLP_NR_MSK]) >> 2 )

  
#define RLP_S_RR          0
#define RLP_S_REJ         1
#define RLP_S_RNR         2
#define RLP_S_SREJ        3

#define RLP_S_BIT_MSK     0x6000
#define RLP_GET_S(_h)     ( ((_h) & RLP_S_BIT_MSK) >> 13 )

  
#define RLP_NS_MSK        RLP_FRM_TYPE_MSK
#define RLP_GET_NS(_h)    ( (sxr_MsbToLsb[((_h) & RLP_NS_MSK)>>7]) >> 2 )


  
  
  

    
typedef u8 rlp_XidBitMap_t;
    
#define RLP_XID_VERSION     RLP_USRPAR_VERS
#define RLP_XID_KIM         RLP_USRPAR_KIM
#define RLP_XID_KMI         RLP_USRPAR_KMI
#define RLP_XID_T1          RLP_USRPAR_T1
#define RLP_XID_N2          RLP_USRPAR_N2
#define RLP_XID_T4          RLP_USRPAR_T4
#define RLP_XID_T2          RLP_USRPAR_T2

#define RLP_ALL_XID_PAR     ( RLP_XID_VERSION | RLP_XID_KIM | RLP_XID_KMI |       \
                              RLP_XID_T1 | RLP_XID_N2 | RLP_XID_T2 )

  
#define RLP_XID_VERSION_TYPE  ( 1 << 4 )
#define RLP_XID_KIM_TYPE      ( 2 << 4 )
#define RLP_XID_KMI_TYPE      ( 3 << 4 )
#define RLP_XID_T1_TYPE       ( 4 << 4 )
#define RLP_XID_N2_TYPE       ( 5 << 4 )
#define RLP_XID_T2_TYPE       ( 6 << 4 )


  
  
  
typedef struct
{
  u8  State;    
  u16 SREJCount;
  Msg_t *Data;
} rlp_RxSlot_t;


  
  
  
typedef struct
{
  u8  State;    
  u8  Count;
  Msg_t *Data;
} rlp_TxSlot_t;


  
  
  
#define RLP_IDLE    0
#define RLP_SEND    1   
#define RLP_WAIT    2   
                        
#define RLP_RCV     3   
#define RLP_SREJ    4   






enum
{
    
  S_FRM,
  
    
  I_FRM,
  
    
  UI_FRM,
  DM_FRM,
  SABM_FRM,
  DISC_FRM,
  UA_FRM,
  NULL_FRM,

  XID_FRM,
  TEST_FRM,
  UNDEFINED_FRM,
  
  RLP_NB_FRM_TYP
};


#endif // __RLP_DEFS_H__











