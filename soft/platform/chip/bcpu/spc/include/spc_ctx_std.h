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


#ifndef _SPC_CTX_STD_H_
#define _SPC_CTX_STD_H_


#include "cs_types.h"
#include "spp_m.h"
#if (CHIP_EDGE_SUPPORTED == 1)
#include "spc_define_edge.h"
#else
#include "spc_define_std.h"
#endif
#include "spc_define_std.h"
#include "baseband_defs.h"
#include "spc_mailbox.h"
#include "spp_gsm.h"

// =============================================================================
//  MACROS
// =============================================================================
/// SCH symbol buffer offset (aVOID overwriting during late SCH windows)
#define SCH_BUF_OFFSET                           ((BB_BURST_TOTAL_SIZE*4))
/// Interleaver Buffer sizes
#define SPC_ITLV_BUFF_TX_FACCH                   ((8*4))
/// 4burst*4word*4bytes
#define SPC_ITLV_BUFF_TX_CTRL                    ((4*4))
#define SPC_ITLV_BUFF_RX_CTRL                    ((4*32))

// ============================================================================
// SPC_FCCH_SEARCH_MODE_T
// -----------------------------------------------------------------------------
/// fcch search algorithm select
// =============================================================================
typedef enum
{
/// Original Delay2 Correlation Algo
    FCCH_SEARCH_ALGO_DELAY2                     = 0x00000000,
/// New Delay4 Correlation Algo
    FCCH_SEARCH_ALGO_DELAY4                     = 0x00000001,
/// New Delay4 Correlation Algo
    FCCH_SEARCH_ALGO_FILTER                     = 0x00000002
} SPC_FCCH_SEARCH_MODE_T;


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
/// Rx (Ifc)Buffer
// =============================================================================
typedef struct
{
    UINT32                         RxIfcBuffer[4*(BB_BURST_ACTIVE_SIZE+2*BB_RX_NBURST_MARGIN)]; //0x00000000
    UINT32                         MonIfcBuffer[MBOX_WINDOWS_QTY*MBOX_MON_WIN_SIZE]; //0x000009C0
} SPC_STATIC_BUFFERS_T; //Size : 0xD40



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
// SPC_EGPRS_PDCH_DATA_BLOCK_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef UINT32 SPC_EGPRS_PDCH_DATA_BLOCK_T[39];


// ============================================================================
// SPC_EGPRS_SRB_STATE_T
// -----------------------------------------------------------------------------
/// Egprs SRB loop test state
// =============================================================================
typedef struct
{
    UINT8                          AsyncState;                   //0x00000000
    UINT8                          BlockCnt[2];                  //0x00000001
    UINT8                          FalseNum;                     //0x00000003
} SPC_EGPRS_SRB_STATE_T; //Size : 0x4



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
    /// Header structure for Egprs
    SPP_HEADER_RESULT_T            Header_result;                //0x00000026
    /// AMR Config
    SPP_AMR_CFG_T                  AMR_Cfg_Tx;                   //0x00000032
    /// AMR Config
    SPP_AMR_CFG_T                  AMR_Cfg_Rx;                   //0x00000041
    UINT32                         Rx_Hu_TCH_N_1;                //0x00000050
    UINT32                         Rx_Hu_TCH_N;                  //0x00000054
    UINT32                         Rx_Hl_TCH_N;                  //0x00000058
    /// Tx buffers
    UINT32                         ABurstBuffer[5];              //0x0000005C
    SPC_DATA_BLOCK_T               CSD_NBlockDataIn;             //0x00000070
    SPC_DATA_BLOCK_T               CCH_NBlockDataIn;             //0x000000A8
    SPC_PDCH_DATA_BLOCK_T          DCH_NBlockDataIn[MBOX_MAX_BUF_IDX_QTY-1]; //0x000000E0
    UINT32*                        NBlockDataIn[MBOX_MAX_BUF_IDX_QTY+1]; //0x000001C0
    /// Rx buffers
    SPC_MBOX_SPEECH_ENC_OUT_T      speechEncOut;                 //0x000001D8
    SPC_DATA_BLOCK_T               CSD_NBlockDataOut;            //0x00000204
    SPC_DATA_BLOCK_T               CCH_NBlockDataOut;            //0x0000023C
    SPC_PDCH_DATA_BLOCK_T          DCH_NBlockDataOut[MBOX_MAX_BUF_IDX_QTY-1]; //0x00000274
    UINT32*                        NBlockDataOut[MBOX_MAX_BUF_IDX_QTY+1]; //0x00000354
    /// interleaver input buffer pointers
    UINT32*                        ItlvBufRxCtrl;                //0x0000036C
    UINT32*                        ItlvBufRxDedicated;           //0x00000370
    UINT32*                        ItlvBufTxCtrl;                //0x00000374
    UINT32*                        ItlvBufTxDedicated;           //0x00000378
    UINT32*                        ItlvBufTxFacch;               //0x0000037C
    /// interleaver output buffer
    VOLATILE UINT32*               Malloc;                       //0x00000380
    /// next address must be aligned on 128 bytes
    UINT32*                        ItlvBufRxFacch;               //0x00000384
    UINT8*                         loopC_buffer;                 //0x00000388
    INT8*                          CIest;                        //0x0000038C
    UINT32                         BaseAddress;                  //0x00000390
    UINT16                         FCCH_Offset;                  //0x00000394
    UINT16                         ChMode;                       //0x00000396
    UINT16                         PrevChMode;                   //0x00000398
    BOOL                           DedicatedActive;              //0x0000039A
    /// Structure used to describe the logical channel associated with a Rx buffer
    /// index and to indicate wether a decoding interruption is asked or not.
    SPC_MBOX_STATWIN_RX_CFG_T      statWinRxCfg[MBOX_MAX_BUF_IDX_QTY]; //0x0000039B
    BOOL                           Tx_off;                       //0x000003AF
    BOOL                           Init_FCCH;                    //0x000003B0
    INT16                          BlockSnR;                     //0x000003B2
    INT16                          LastBlockSnR;                 //0x000003B4
    SPC_8UINT8_T                   Rx_q_CCH[4];                  //0x000003B6
    /// Was UINT8 Rx_q_CCH[4][8];, but coolXml has its own ...
    UINT8                          Rx_CS[4];                     //0x000003D6
    UINT8                          BurstNb;                      //0x000003DA
    UINT8                          currentSnap;                  //0x000003DB
    UINT8                          TxBlockOffset;                //0x000003DC
    UINT8                          RxBlockOffset;                //0x000003DD
    UINT8                          cs_identifier[4];             //0x000003DE
    UINT8                          Tx_Hu;                        //0x000003E2
    UINT8                          Tx_Hl;                        //0x000003E3
    UINT8                          FacchEncoded;                 //0x000003E4
    UINT8                          DTX_on;                       //0x000003E5
    UINT32                         RfIfCtrl;                     //0x000003E8
    BOOL                           DCOC_on;                      //0x000003EC
    UINT8                          FcchFound;                    //0x000003ED
    UINT8                          txQty;                        //0x000003EE
    UINT8                          rxQty;                        //0x000003EF
    UINT8                          Taf_Flag;                     //0x000003F0
    UINT8                          DTX_dwnlk_flag;               //0x000003F1
    UINT8                          DTX_dwnlk_count;              //0x000003F2
    UINT8                          BFI_count;                    //0x000003F3
    UINT8                          sendEqItr;                    //0x000003F4
    UINT8                          resIdx;                       //0x000003F5
    UINT8                          frame_over;                   //0x000003F6
    UINT8                          loop_bufIdx;                  //0x000003F7
    UINT8                          CodecModeRequest;             //0x000003F8
    UINT8                          ratscch_to_encode;            //0x000003F9
    UINT8                          ACK_Activation;               //0x000003FA
    UINT8                          REQ_Activation;               //0x000003FB
    UINT8                          RATSCCH_REQ;                  //0x000003FC
    UINT8                          ratscch_detected;             //0x000003FD
    UINT8                          sidupdate_ctr;                //0x000003FE
    UINT8                          AFNmod104;                    //0x000003FF
    /// old defines moved to variables for romming purpose
    INT16                          FacchThreshold;               //0x00000400
    UINT8                          FR_BfiThreshold;              //0x00000402
    UINT8                          EFR_BfiThreshold;             //0x00000403
    UINT8                          HR_BfiThreshold;              //0x00000404
    UINT8                          UfiThreshold;                 //0x00000405
    UINT8                          FsBfiBerThresh;               //0x00000406
    UINT8                          FsBfiBer2Thresh;              //0x00000407
    INT8                           FsBfiSnrThresh;               //0x00000408
    UINT8                          HsBfiBerThresh;               //0x00000409
    UINT8                          HsBfiBer2Thresh;              //0x0000040A
    INT8                           HsBfiSnrThresh;               //0x0000040B
    UINT8                          AFS_ber1_threshold[8];        //0x0000040C
    UINT8                          AHS_ber1_threshold[6];        //0x00000414
    UINT8                          AFS_ber2_threshold[8];        //0x0000041A
    UINT8                          AHS_ber2_threshold[6];        //0x00000422
    VOLATILE BOOL                  equ_hburst_mode;              //0x00000428
    VOLATILE BOOL                  ifc2_burst_mode_fcch;         //0x00000429
    VOLATILE BOOL                  ifc2_burst_mode_sbnb;         //0x0000042A
    /// extern volatile UINT32 bb_irq_cause;
    VOLATILE UINT32                bcpuIrqCause;                 //0x0000042C
    VOLATILE UINT8                 MonWinSize;                   //0x00000430
    VOLATILE UINT8                 FcchWinSize;                  //0x00000431
    VOLATILE UINT8                 FcchPreOpening;               //0x00000432
    VOLATILE UINT8                 FcchInit;                     //0x00000433
    VOLATILE UINT8                 FcchActive;                   //0x00000434
    VOLATILE UINT8                 FcchSet;                      //0x00000435
    VOLATILE UINT8                 rx_count;                     //0x00000436
    VOLATILE UINT8                 sched_rx_count;               //0x00000437
    VOLATILE UINT8                 slot_count;                   //0x00000438
    VOLATILE UINT8                 sched_slot_count;             //0x00000439
    VOLATILE UINT8                 ifc2_pending;                 //0x0000043A
    VOLATILE BOOL                  NBurstSchIfc2Done;            //0x0000043B
    VOLATILE UINT8                 rx_slot_nb;                   //0x0000043C
    VOLATILE UINT8                 sched_rx_slot_nb;             //0x0000043D
    VOLATILE UINT8                 tx_left;                      //0x0000043E
    VOLATILE UINT32                burst_to_send[10];            //0x00000440
    VOLATILE UINT32*               SilentFrame;                  //0x00000468
    /// enable for DTX uplink
    VOLATILE UINT32                voc_dtx_en;                   //0x0000046C
    /// AMR codec variables
    VOLATILE UINT32                TxCodecMode;                  //0x00000470
    VOLATILE UINT32                RxCodecMode;                  //0x00000474
    /// used for EGPRS only
    SPP_MODULATION_TYPE_T          tx_modulation;                //0x00000478
    SPP_MODULATION_TYPE_T          rx_modulation;                //0x0000047C
    SPP_HEADER_TYPE_T              tx_headertype[4];             //0x00000480
    SPP_HEADER_TYPE_T              rx_headertype[4];             //0x00000490
    SPP_HEADER_TYPE_T              curr_rx_headertype[4];        //0x000004A0
    SPP_HEADER_TYPE_T              prev_rx_headertype[4];        //0x000004B0
    UINT8                          egprs_rx_buffer_bmp;          //0x000004C0
    /// Pointer to a function called during the idle time of the BCPU scheduler.
    VOLATILE VOID*                 schedulerHook;                //0x000004C4
    /// Pointer to a function called by the BCPU IRQ handler.
    VOLATILE VOID*                 irqHook;                      //0x000004C8
    /// Mask used to enable dump.
    VOLATILE UINT32                dumpMask;                     //0x000004CC
    /// Pointer to a dump function.
    VOLATILE VOID*                 dumpPtr;                      //0x000004D0
    /// Pointer to the EGPRS IR buffer.
    SPP_EGPRS_IR_BUF_T*            egprsIrBuffer;                //0x000004D4
    /// Fch Search Mode(Delay2,Delay4 and Filter)
    SPC_FCCH_SEARCH_MODE_T         FchSearchMode;                //0x000004D8
    /// XCV power down enable flag when stop fch searching.
    UINT8                          xcvPdnEnable;                 //0x000004DC
    /// amr frame filter enable flag.
    UINT8                          amrFrmFltEnable;              //0x000004DD
    /// amr cfg Tx on Chnage.
    UINT32                         AmrCfgTxChg;                  //0x000004E0
    SPC_EGPRS_PDCH_DATA_BLOCK_T    DCH_NBlockDataOut_EGPRS[MBOX_MAX_BUF_IDX_QTY-1]; //0x000004E4
    SPC_EGPRS_PDCH_DATA_BLOCK_T    DCH_NBlockDataIn_EGPRS[MBOX_MAX_BUF_IDX_QTY-1]; //0x00000754
    /// max downlink rx slot number
    UINT8                          max_dl_total_slots;           //0x000009C4
    /// max downlink rx slot number
    UINT8                          first_half_slots;             //0x000009C5
    /// header filtering mode: 1: filter header. 0: not filter
    BOOL                           edge_headfilter_mode;         //0x000009C6
    /// decode or not those blocks with tfi not matched. 1: decode. 0: not decode
    BOOL                           decode_tfierror_mode;         //0x000009C7
    VOLATILE BOOL                  decode_irq_flag;              //0x000009C8
    VOLATILE UINT8                 decode_irq_buffIdx;           //0x000009C9
    BOOL                           buf_exchange_flag[MBOX_MAX_BUF_IDX_QTY-1]; //0x000009CA
    VOLATILE UINT8                 last_egprs;                   //0x000009CE
    UINT8                          last_rx_modulation[MBOX_MAX_BUF_IDX_QTY-1]; //0x000009CF
    UINT8                          max_tof;                      //0x000009D3
    UINT8                          max_fof;                      //0x000009D4
    UINT32                         block_snr[MBOX_MAX_BUF_IDX_QTY-1]; //0x000009D8
    /// bcpu send decode done irq flag
    VOLATILE BOOL                  send_dec_irq;                 //0x000009E8
    VOLATILE BOOL                  edge_digfilt_en;              //0x000009E9
    /// Malloc for Egprs functions
    VOLATILE UINT32*               EgprsMalloc;                  //0x000009EC
    /// Egprs SRB loop test state
    SPC_EGPRS_SRB_STATE_T          EgprsSrb;                     //0x000009F0
    VOLATILE UINT8                 gprs_measure_mode;            //0x000009F4
    /// Is2xMode: 1: 2x mode. 0: default 1x mode
    BOOL                           Is2xMode;                     //0x000009F5
    /// IsDmEnable: 1: dm enable,ifc put data into dm. 0: default,ifc put data into
    /// bbsram.
    BOOL                           IsDmEnable;                   //0x000009F6
    /// IsAciDetectionEnable:1:enable.0:disable.
    BOOL                           IsAciDetectionEnable;         //0x000009F7
    /// &#xd; SaicModeSel:SAIC_LLR_SELECT,softvalues of the better one between no
    /// saic and saic is selected;SAIC_CRC_SELECT,softvalues of no saic and saic
    /// are both selected by crc.
    SPC_SAIC_SELECT_MODE_T         SaicModeSel;                  //0x000009F8
    /// interleaver input buffer pointers
    UINT32*                        ItlvBufRxCtrlSaic;            //0x000009FC
    UINT32*                        ItlvBufRxDedicatedSaic;       //0x00000A00
} SPC_CONTEXT_T; //Size : 0xA04



// ============================================================================
// SPC_CONTEXT_EXT_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef struct
{
    UINT8                          rxQty;                        //0x00000000
    UINT8                          frame_over;                   //0x00000001
    VOLATILE UINT8                 rx_count;                     //0x00000002
    VOLATILE UINT8                 sched_rx_count;               //0x00000003
    VOLATILE UINT8                 sched_slot_count;             //0x00000004
    VOLATILE UINT8                 rx_count_flag;                //0x00000005
} SPC_CONTEXT_EXT_T; //Size : 0x6










register SPC_CONTEXT_T* g_spcCtx asm("$16");
EXPORT PROTECTED SPC_ITLV_BUFFERS_T g_spcItlvBufTab;
EXPORT PROTECTED SPC_STATIC_BUFFERS_T g_spcStaticBufTab;



#endif

