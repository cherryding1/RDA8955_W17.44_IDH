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

#include "pal_gsm.h"

#include "calib_m.h"


//  ----------------- L1 relative defines.

#define L1_FCH_WIN_QB_POS (5000 - 625 - PAL_PLL_LOCK_TIME)

#define L1S_SACCH0_BUFFER  4
#define L1S_TCH0_BUFFER    1
#define L1S_PDCH_BUFFER    0
#define L1S_PTCCH_BUFFER   4
#define L1S_SDCCH_BUFFER   4

#define L1S_FCH_WIN_IDX    0
#define L1S_SCH_WIN_IDX    0
#define L1S_RX_NB_WIN_IDX  2
#define L1S_TX_NB_WIN_IDX  1

#define SL1_GET_BST_IDX(i)  (i & 3)
#define SL1_BST(i)   ((1 << 2) | i)
#define SL1_EC_FA           (1 << 3)
#define SL1_DC_FA           (1 << 4)
#define SL1_DC_TC           (1 << 5)
#define SL1_EC_SA           (1 << 6)
#define SL1_DC_SA           (1 << 7)

typedef enum
{
    MY1_SYNC_NO_STATE,
    MY1_SYNC_FCH_SEARCH_STATE,
    MY1_SYNC_FCH_READ_STATE,
    MY1_SYNC_SCH_STATE
} MY1_SYNC_STATE_T;

#define MY1_SYNC_SCHFOUND_POS 0
#define MY1_SYNC_SCHFOUND_MSK (1<<MY1_SYNC_SCHFOUND_POS)
#define MY1_SYNC_SET_SCHFOUND(Id)  ((Id<<MY1_SYNC_SCHFOUND_POS) & MY1_SYNC_SCHFOUND_MSK)
#define MY1_SYNC_GET_SCHFOUND(Id)  ((Id & MY1_SYNC_SCHFOUND_MSK)>>MY1_SYNC_SCHFOUND_POS)

#define MY1_SYNC_SCHCNT_POS 1
#define MY1_SYNC_SCHCNT_MSK (0x7<<MY1_SYNC_SCHCNT_POS)
#define MY1_SYNC_SET_SCHCNT(Id)  ((Id<<MY1_SYNC_SCHCNT_POS) & MY1_SYNC_SCHCNT_MSK)
#define MY1_SYNC_GET_SCHCNT(Id)  ((Id & MY1_SYNC_SCHCNT_MSK)>>MY1_SYNC_SCHCNT_POS)

#define MY1_SYNC_PRE_STATE_POS 4
#define MY1_SYNC_PRE_STATE_MSK (0x7<<MY1_SYNC_PRE_STATE_POS)
#define MY1_SYNC_SET_PRE_STATE(Id)  ((Id<<MY1_SYNC_PRE_STATE_POS) & MY1_SYNC_PRE_STATE_MSK)
#define MY1_SYNC_GET_PRE_STATE(Id)  ((Id & MY1_SYNC_PRE_STATE_MSK)>>MY1_SYNC_PRE_STATE_POS)

#define MY1_SYNC_NXT_STATE_POS 7
#define MY1_SYNC_NXT_STATE_MSK (0x7<<MY1_SYNC_NXT_STATE_POS)
#define MY1_SYNC_SET_NXT_STATE(Id)  ((Id<<MY1_SYNC_NXT_STATE_POS) & MY1_SYNC_NXT_STATE_MSK)
#define MY1_SYNC_GET_NXT_STATE(Id)  ((Id & MY1_SYNC_NXT_STATE_MSK)>>MY1_SYNC_NXT_STATE_POS)

typedef enum
{
    FOF_FOUND,
    FOF_SEARCH,
    FOF_NOT_FOUND
} myFOf_t;



//  ----------------- The global context variable.

//  Pointer to the context, uncached.
EXPORT VOLATILE CALIB_STUB_CTX_T* g_calibStubGlobalCtxPtr;

//  Local context, used to pass some variables.
EXPORT CALIB_STUB_CTX_T g_calibStubLocalCtx;



//  ----------------- The L1 functions.

//  Initialize PAL with the selected band.
VOID myl1_BandInit(UINT8 Band);

//  Functions used for Rx statistics.
UINT32 myl1_Synch(VOID);
UINT32  myl1_Fn(UINT16 T1, UINT8 T2, UINT8 T3);
VOID myl1_Idle(VOID);
VOID myl1_GetNBEqualisationResult(pal_NBurstResult_t *NBurstResult);

//  Monitor the frequency on a specified ARFCN and Band.
myFOf_t myl1_GetFOf(BOOL init, UINT16 Arfcn, UINT8 Band, UINT8 ExpPower, INT32 *MeanFOf);

//  Monitor the power on a specified ARFCN and Band and Expected Power.
BOOL myl1_Monit(UINT16 Arfcn, UINT8 Band, UINT8 ExpPower, UINT8 T2,
                UINT8 *MonPower, UINT8 *NBPower);

//  Send bursts of modulated stuff.
VOID myl1_SendBurst(UINT16 Arfcn, UINT8 Pcl, UINT8 Band, UINT32 Tsc);

//  Send bursts of modulated stuff, at Dac power (index or value).
VOID myl1_SendBurstDac(UINT16 Arfcn, UINT16 DacId, UINT8 Band, UINT32 Tsc);



//  ----------------- Misc functions.

VOID my_msg2str(UINT8* msgp,char* strp,UINT8 size);

