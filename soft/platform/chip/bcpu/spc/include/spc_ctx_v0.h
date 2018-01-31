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


#ifndef _SPC_CTX_V0_H_
#define _SPC_CTX_V0_H_


#include "cs_types.h"
#include "spp_m.h"
#include "baseband_defs.h"
#include "spc_mailbox_v0.h"
#include "spp_gsm.h"

// =============================================================================
//  MACROS
// =============================================================================
/// SRAM special mappings: ADDR bit 18 prevents the effective write access to the
/// SRAM
#define NO_SRAM_WRITE                            (0X40000)
/// SCH symbol buffer offset (aVOID overwriting during late SCH windows)
#define SCH_BUF_OFFSET                           ((BB_BURST_TOTAL_SIZE*4))
/// Interleaver Buffer sizes
#define SPC_ITLV_BUFF_TX_FACCH                   ((8*4))
/// 4burst*4word*4bytes
#define SPC_ITLV_BUFF_TX_CTRL                    ((4*4))
#define SPC_ITLV_BUFF_TX_DEDICATED               ((22*4))
#define SPC_ITLV_BUFF_RX_CTRL                    ((4*32))
#define SPC_ITLV_BUFF_RX_DEDICATED               ((22*32))
#define SPC_ITLV_BUFF_SIZE                       (SPC_ITLV_BUFF_TX_FACCH + SPC_ITLV_BUFF_TX_CTRL + SPC_ITLV_BUFF_TX_DEDICATED + SPC_ITLV_BUFF_RX_CTRL + SPC_ITLV_BUFF_RX_DEDICATED)
#define SPC_STATIC_BUF_SIZE                      (4*BB_BURST_TOTAL_SIZE + 3*MBOX_MON_WIN_SIZE)

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// SPC_ITLV_BUFFERS_T
// -----------------------------------------------------------------------------
/// Interleaver Buffer Structure
// =============================================================================
typedef struct
{
    UINT32                         RxCtrl[SPC_ITLV_BUFF_RX_CTRL]; //0x00000000
    UINT32                         RxDedicated[SPC_ITLV_BUFF_RX_DEDICATED]; //0x00000200
    UINT32                         TxCtrl[SPC_ITLV_BUFF_TX_CTRL]; //0x00000D00
    UINT32                         TxDedicated[SPC_ITLV_BUFF_TX_DEDICATED]; //0x00000D40
    UINT32                         TxFacch[SPC_ITLV_BUFF_TX_FACCH]; //0x00000EA0
} SPC_ITLV_BUFFERS_T; //Size : 0xF20



// ============================================================================
// SPC_STATIC_BUFFERS_T
// -----------------------------------------------------------------------------
/// Overall structure : Rx (Ifc)Buffer + Vitac Buffers
// =============================================================================
typedef struct
{
    UINT32                         RxIfcBuffer[4*(BB_BURST_ACTIVE_SIZE+2*BB_RX_NBURST_MARGIN)]; //0x00000000
    UINT32                         MonIfcBuffer[3*MBOX_MON_WIN_SIZE]; //0x000009C0
} SPC_STATIC_BUFFERS_T; //Size : 0xD20



// ============================================================================
// SPC_DATA_BLOCK_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef UINT32 SPC_DATA_BLOCK_T[14];


// ============================================================================
// SPC_TX_SPEECH_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef struct
{
    UINT32*                        InputBuffer;                  //0x00000000
    UINT16                         CodecModeInd;                 //0x00000004
    UINT8                          TxFrameType;                  //0x00000006
} SPC_TX_SPEECH_T; //Size : 0x8



// ============================================================================
// SPC_RX_LOCALS_T
// -----------------------------------------------------------------------------
/// structure for local rx variables
// =============================================================================
typedef struct
{
    UINT32*                        ItlvBuffInput;                //0x00000000
    UINT32*                        BlockDecodedAdd;              //0x00000004
    UINT16                         Copy_ChMode;                  //0x00000008
    UINT8                          burstIdx;                     //0x0000000A
    UINT8                          buffIdx;                      //0x0000000B
    UINT8                          slotIdx;                      //0x0000000C
    UINT8                          RxBuffOffset;                 //0x0000000D
    UINT8                          itlv_mode;                    //0x0000000E
    UINT8                          StealingResult;               //0x0000000F
    UINT8                          voc_bfi;                      //0x00000010
    UINT8                          voc_sid;                      //0x00000011
    UINT8                          voc_taf;                      //0x00000012
    UINT8                          voc_ufi;                      //0x00000013
    UINT8                          RxFrameType;                  //0x00000014
    UINT8                          RX_decode;                    //0x00000015
} SPC_RX_LOCALS_T; //Size : 0x18



// ============================================================================
// SPC_8UINT8_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef UINT8 SPC_8UINT8_T[8];


// ============================================================================
// SPC_CONTEXT_T
// -----------------------------------------------------------------------------
/// Global structure of Context statically placed in BB_SRAM Should be referenced
/// by a pointer placed in a reserved register to ease address calculation and reduce
/// code size.
// =============================================================================
typedef struct
{
    /// FCCH Result
    SPP_FCCH_RESULT_T              FCCH_Result;                  //0x00000000
    /// SCH Result
    SPP_SCH_RESULT_T               SCH_Result;                   //0x00000004
    /// Normal Burst Result
    SPP_N_BURST_RESULT_T           NBurst_result;                //0x00000010
    /// Normal block Result
    SPP_N_BLOCK_RESULT_T           NBlock_result;                //0x0000001C
    /// AMR Config
    SPP_AMR_CFG_T                  AMR_Cfg_Tx;                   //0x00000026
    /// AMR Config
    SPP_AMR_CFG_T                  AMR_Cfg_Rx;                   //0x00000035
    UINT32                         Rx_Hu_TCH_N_1;                //0x00000044
    UINT32                         Rx_Hu_TCH_N;                  //0x00000048
    UINT32                         Rx_Hl_TCH_N;                  //0x0000004C
    /// Tx buffers
    UINT32                         ABurstBuffer[5];              //0x00000050
    SPC_DATA_BLOCK_T               TxBufferIn_tab[MBOX_MAX_BUF_IDX_QTY+1]; //0x00000064
    UINT32*                        TxBufferIn[MBOX_MAX_BUF_IDX_QTY+1]; //0x000001B4
    /// Rx buffers
    SPC_MBOX_SPEECH_ENC_OUT_T      speechEncOut;                 //0x000001CC
    SPC_DATA_BLOCK_T               StoredNBlockDataOut;          //0x000001F8
    SPC_DATA_BLOCK_T               NBlockDataOut[MBOX_MAX_BUF_IDX_QTY]; //0x00000230
    /// interleaver input buffer pointers
    UINT32*                        ItlvBufRxCtrl;                //0x00000348
    UINT32*                        ItlvBufRxDedicated;           //0x0000034C
    UINT32*                        ItlvBufTxCtrl;                //0x00000350
    UINT32*                        ItlvBufTxDedicated;           //0x00000354
    UINT32*                        ItlvBufTxFacch;               //0x00000358
    /// interleaver output buffer
    VOLATILE UINT32*               Malloc;                       //0x0000035C
    /// next address must be aligned on 128 bytes
    UINT32*                        ItlvBufRxFacch;               //0x00000360
    UINT8*                         loopC_buffer;                 //0x00000364
    INT8*                          CIest;                        //0x00000368
    UINT32                         BaseAddress;                  //0x0000036C
    UINT16                         FCCH_Offset;                  //0x00000370
    UINT16                         ChMode;                       //0x00000372
    UINT16                         PrevChMode;                   //0x00000374
    BOOL                           DedicatedActive;              //0x00000376
    /// Structure used to describe the logical channel associated with a Rx buffer
    /// index and to indicate wether a decoding interruption is asked or not.
    SPC_MBOX_STATWIN_RX_CFG_T      statWinRxCfg[MBOX_MAX_BUF_IDX_QTY]; //0x00000377
    BOOL                           Tx_off;                       //0x00000381
    BOOL                           Init_FCCH;                    //0x00000382
    INT16                          BlockSnR;                     //0x00000384
    INT16                          LastBlockSnR;                 //0x00000386
    SPC_8UINT8_T                   Rx_q_CCH[4];                  //0x00000388
    /// Was UINT8 Rx_q_CCH[4][8];, but coolXml has its own ...
    UINT8                          Rx_CS[4];                     //0x000003A8
    UINT8                          BurstNb;                      //0x000003AC
    UINT8                          currentSnap;                  //0x000003AD
    UINT8                          TxBlockOffset;                //0x000003AE
    UINT8                          RxBlockOffset;                //0x000003AF
    UINT8                          cs_identifier[4];             //0x000003B0
    UINT8                          Tx_Hu;                        //0x000003B4
    UINT8                          Tx_Hl;                        //0x000003B5
    UINT8                          FacchEncoded;                 //0x000003B6
    UINT8                          DTX_on;                       //0x000003B7
    UINT32                         ExtDtxSetting;                //0x000003B8
    UINT32                         RfIfCtrl;                     //0x000003BC
    BOOL                           DCOC_on;                      //0x000003C0
    UINT8                          FcchFound;                    //0x000003C1
    UINT8                          txQty;                        //0x000003C2
    UINT8                          rxQty;                        //0x000003C3
    UINT8                          Taf_Flag;                     //0x000003C4
    UINT8                          DTX_dwnlk_flag;               //0x000003C5
    UINT8                          DTX_dwnlk_count;              //0x000003C6
    UINT8                          BFI_count;                    //0x000003C7
    UINT8                          sendEqItr;                    //0x000003C8
    UINT8                          resIdx;                       //0x000003C9
    UINT8                          frame_over;                   //0x000003CA
    UINT8                          loop_bufIdx;                  //0x000003CB
    UINT8                          CodecModeRequest;             //0x000003CC
    UINT8                          ratscch_to_encode;            //0x000003CD
    UINT8                          ACK_Activation;               //0x000003CE
    UINT8                          REQ_Activation;               //0x000003CF
    UINT8                          RATSCCH_REQ;                  //0x000003D0
    UINT8                          ratscch_detected;             //0x000003D1
    UINT8                          sidupdate_ctr;                //0x000003D2
    UINT8                          AFNmod104;                    //0x000003D3
    /// old defines moved to variables for romming purpose
    INT16                          FacchThreshold;               //0x000003D4
    UINT8                          BfiThreshold;                 //0x000003D6
    UINT8                          UfiThreshold;                 //0x000003D7
    UINT8                          FsBfiBerThresh;               //0x000003D8
    UINT8                          FsBfiBer2Thresh;              //0x000003D9
    INT8                           FsBfiSnrThresh;               //0x000003DA
    UINT8                          HsBfiBerThresh;               //0x000003DB
    UINT8                          HsBfiBer2Thresh;              //0x000003DC
    INT8                           HsBfiSnrThresh;               //0x000003DD
    UINT8                          AFS_ber1_threshold[8];        //0x000003DE
    UINT8                          AHS_ber1_threshold[6];        //0x000003E6
    UINT8                          AFS_ber2_threshold[8];        //0x000003EC
    UINT8                          AHS_ber2_threshold[6];        //0x000003F4
    /// extern volatile UINT32 bb_irq_cause;
    VOLATILE BOOL                  equ_hburst_mode;              //0x000003FA
    VOLATILE UINT8                 FcchInit;                     //0x000003FB
    VOLATILE UINT8                 FcchActive;                   //0x000003FC
    VOLATILE UINT8                 FcchSet;                      //0x000003FD
    VOLATILE UINT8                 rx_count;                     //0x000003FE
    VOLATILE UINT8                 sched_rx_count;               //0x000003FF
    VOLATILE UINT8                 slot_count;                   //0x00000400
    VOLATILE UINT8                 sched_slot_count;             //0x00000401
    VOLATILE UINT8                 ifc2_pending;                 //0x00000402
    VOLATILE UINT8                 rxslotNb;                     //0x00000403
    VOLATILE UINT8                 tx_left;                      //0x00000404
    VOLATILE UINT32                burst_to_send[10];            //0x00000408
    /// power returned by apc
    VOLATILE UINT32                apc_power;                    //0x00000430
    VOLATILE UINT8                 apc_step;                     //0x00000434
    VOLATILE UINT32*               SilentFrame;                  //0x00000438
    /// enable for DTX uplink
    VOLATILE UINT32                voc_dtx_en;                   //0x0000043C
    /// AMR codec variables
    VOLATILE UINT32                TxCodecMode;                  //0x00000440
    VOLATILE UINT32                RxCodecMode;                  //0x00000444
} SPC_CONTEXT_T; //Size : 0x448







// *************************************************
// * Conditional compilation Flags
// *************************************************
//#define CHANNEL_EST_WEIGTH
//#define FIRE_CORRECTION

// IRQ handler shared variables

register SPC_CONTEXT_T* g_spcCtx asm("$16");
EXPORT PROTECTED SPC_ITLV_BUFFERS_T g_spcItlvBufTab;
EXPORT PROTECTED SPC_STATIC_BUFFERS_T g_spcStaticBufTab;
// *************************************************
// S0 Register reserved to contain Context pointer
// *************************************************
// interleaver buffer
// Static Buffer pointer declaration

VOID spc_SchedulerMain(VOID);
// *************************************************
// Scheduler entry point
// *************************************************



#endif

