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

#include "hal_speech.h"
#include "hal_sys.h"
#include "hal_clk.h"
#include "halp_debug.h"
#include "halp_speech.h"


// =============================================================================
//
// -----------------------------------------------------------------------------
// =============================================================================



// =============================================================================
//  MACROS
// =============================================================================

#define HAL_SPEECH_RX_TX_HALF_FULL  g_halSpeechFifoPushRxIdx = 0; \
                                    g_halSpeechFifoPopRxIdx  = 0 - (HAL_SPEECH_FIFO_DEPTH/2);

#define HAL_SPEECH_TX_HALF_FULL     g_halSpeechFifoPopRxIdx  = g_halSpeechFifoPushRxIdx + 1 - (HAL_SPEECH_FIFO_DEPTH/2);

#define HAL_SPEECH_RX_HALF_FULL     g_halSpeechFifoPushRxIdx  = g_halSpeechFifoPopRxIdx + 1 + (HAL_SPEECH_FIFO_DEPTH/2);

#define HAL_SPEECH_RX_OVF_TX_UDF    ((g_halSpeechFifoPushRxIdx  - g_halSpeechFifoPopRxIdx) == (HAL_SPEECH_FIFO_DEPTH))

#define HAL_SPEECH_TX_OVF_RX_UDF    ((g_halSpeechFifoPushRxIdx  - g_halSpeechFifoPopRxIdx) == (0))


// =============================================================================
//  TYPES
// =============================================================================



// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

// =============================================================================
// g_halSpeechFifo
// -----------------------------------------------------------------------------
/// The speech FIFO
// =============================================================================
PROTECTED HAL_SPEECH_FIFO_T g_halSpeechFifo;



// =============================================================================
// g_halSpeechFifoPushRxIdx
// -----------------------------------------------------------------------------
/// Rx push index. This is also the Tx pop index
// =============================================================================
PRIVATE UINT32 g_halSpeechFifoPushRxIdx  = 0;


// =============================================================================
// g_halSpeechFifoPopRxIdx
// -----------------------------------------------------------------------------
/// Rx pop index. This is also the Tx push index.
// =============================================================================
PRIVATE UINT32 g_halSpeechFifoPopRxIdx  = 0;


// =============================================================================
// g_halSpeechFifoPushRxHappened
// -----------------------------------------------------------------------------
/// Rx push happened global reminder. (From the 'PAL' side.)
// =============================================================================
PRIVATE BOOL g_halSpeechFifoPushRxHappened = FALSE;



// =============================================================================
// g_halSpeechFifoPopRxHappened
// -----------------------------------------------------------------------------
/// Rx pop happened global reminder. (From the 'PAL' side.)
// =============================================================================
PRIVATE BOOL g_halSpeechFifoPopRxHappened = FALSE;




// =============================================================================
//  FUNCTIONS
// =============================================================================

#include "string.h"
#include "spc_mailbox.h"

#define HAL_SPEECH_MAX_MUTED_COUNT 10

// Use CoolWatcher to enable/disable muting voice frame
VOLATILE UINT32 g_voiceMuteEnabled = 1;

VOLATILE UINT32 g_halSpeechMutedCount = 0;

VOID hal_SpeechMuteVoiceFrame(HAL_SPEECH_ENC_OUT_T* encOut)
{
    EXPORT const UINT32 MutedFrame_FR[9];
    EXPORT const UINT32 MutedFrame_HR[4];
    EXPORT const UINT32 MutedFrame_EFR[8];
#define RATE(ChMode) (ChMode&0x1)
#define SPEECH(ChMode) ((ChMode>>4)&0x3)
#define FR_CODEC  0
#define EFR_CODEC 1
#define AMR_CODEC 2
    UINT16 *pEncMode = &encOut->encMode;
    UINT16 *pEncodeFrame = &encOut->encFrameType;
    UINT16 *pSp=&encOut->sp;
    UINT16 *encOutBuf=encOut->encOutBuf;
    UINT32 ChMode = g_mailbox.pal2spc.statWin.dedicated.mode;



    if(SPEECH(ChMode) == FR_CODEC)
    {
        *pEncMode = HAL_INVALID;
        *pSp = 0;
        if(RATE(ChMode))
        {
            memcpy(encOutBuf, MutedFrame_HR, sizeof(MutedFrame_HR));
        }
        else
        {
            memcpy(encOutBuf, MutedFrame_FR, sizeof(MutedFrame_FR));
        }
    }
    else if(SPEECH(ChMode) == EFR_CODEC)
    {
        *pEncMode = HAL_INVALID;
        *pSp = 0;
        memcpy(encOutBuf, MutedFrame_EFR, sizeof(MutedFrame_EFR));
    }
    else if(SPEECH(ChMode) == AMR_CODEC)
    {

        *pSp = 0;
        *pEncodeFrame = HAL_TX_NO_DATA;
        *pEncMode = HAL_AMR475_ENC;

    }
}



// =============================================================================
// hal_SpeechFifoReset
// -----------------------------------------------------------------------------
/// Initializes the speech related mechanisms in HAL (Speech Fifo, ...)
// =============================================================================
PUBLIC VOID hal_SpeechFifoReset(VOID)
{
    HAL_PROFILE_FUNCTION_ENTER(hal_SpeechReset);
    UINT32 i;

    HAL_TRACE(TSTDOUT|TNB_ARG(0), 0, "HAL Speech - Fifo reset");

    // Init global variables
    g_halSpeechFifoPushRxHappened = FALSE;
    g_halSpeechFifoPopRxHappened  = FALSE;

    g_halSpeechMutedCount = 0;

    // Ideally, pop is made HAL_SPEECH_FIFO_LENGTH/2
    // behind push (and push is as far behind pop ...)
    HAL_SPEECH_RX_TX_HALF_FULL;

    // Reset FIFO state.
    for (i=0 ; i<HAL_SPEECH_FIFO_DEPTH ; i++)
    {
        g_halSpeechFifo.fifo[i].rx.codecMode = HAL_INVALID;
        g_halSpeechFifo.fifo[i].tx.encMode = HAL_INVALID;
    }

    // Reset overflow status bits.
    g_halSpeechFifo.rxOverflowTxUnderflow = FALSE;
    g_halSpeechFifo.txOverflowRxUnderflow = FALSE;

    HAL_PROFILE_FUNCTION_EXIT(hal_SpeechReset);
}


// =============================================================================
// hal_SpeechFifoInvalidateRx
// -----------------------------------------------------------------------------
/// Invalidate all Rx frames from the fifo without changing their mode
// =============================================================================
INLINE VOID hal_SpeechFifoInvalidateRx(VOID)
{
    UINT8 i;
    // Invalidate Rx FIFO.
    for (i=0 ; i<HAL_SPEECH_FIFO_DEPTH ; i++)
    {
        g_halSpeechFifo.fifo[i].rx.bfi = 1;
        g_halSpeechFifo.fifo[i].rx.decFrameType = HAL_RX_NO_DATA;
    }
}


// =============================================================================
// hal_SpeechFifoInvalidateTx
// -----------------------------------------------------------------------------
/// Invalidate all Tx frames from the fifo without changing their mode
// =============================================================================
INLINE VOID hal_SpeechFifoInvalidateTx(VOID)
{
    UINT8 i;
    // Invalidate Tx FIFO.
    for (i=0 ; i<HAL_SPEECH_FIFO_DEPTH ; i++)
    {
        g_halSpeechFifo.fifo[i].tx.encMode = HAL_INVALID;
    }
}


//PRIVATE VOID hal_SpeechDataLoop(HAL_SPEECH_DEC_IN_T* pRx,
//                                HAL_SPEECH_ENC_OUT_T* pTx)
//{
//    int i;
//
//    for (i =0; i< HAL_SPEECH_FRAME_SIZE_COD_BUF/2; ++i)
//    {
//        pRx->decInBuf[i] = pTx->encOutBuf[i];
//    }
//}



// =============================================================================
// hal_SpeechPushRxPopTx
// -----------------------------------------------------------------------------
/// PAL side of the FIFO accessor. The PAL speech operation is done under FINT IRQ.
///
/// @param pushedRx Pointer to the received trame to push in the FIFO
/// @param popedTx  Pointer to the buffer where the trame to send will
/// be copied.
// =============================================================================
PUBLIC VOID HAL_FUNC_INTERNAL hal_SpeechPushRxPopTx(CONST HAL_SPEECH_DEC_IN_T* pushedRx,
        HAL_SPEECH_ENC_OUT_T* popedTx)
{
    HAL_PROFILE_FUNCTION_ENTER(hal_SpeechPushRxPopTx);

    // update fifo's Pop Rx state
    if (!g_halSpeechFifoPopRxHappened)
    {
        // Initial life of the FIFO: sliding mode with only pushed Rx from PAL side.
        // Pop (from VoiS side) hasn't happened yet.
        g_halSpeechFifoPopRxIdx++;
    }
    else
    {
        // If we try to push a Rx frame at a place that is still not popped, the VoiS stream is considered stopped,
        // so we go back to the initial state of the fifo (sliding mode only pushed Rx from PAL side)
        if  (HAL_SPEECH_RX_OVF_TX_UDF)
        {
            g_halSpeechFifo.rxOverflowTxUnderflow = TRUE;
            HAL_TRACE(TSTDOUT|TNB_ARG(0), 0, "HAL Speech - Fifo reset on PushRx");
            hal_SpeechFifoInvalidateTx();
            g_halSpeechFifoPopRxHappened  = FALSE;
            HAL_SPEECH_TX_HALF_FULL;
        }
    }

    // push a valid Rx frame from PAL
    g_halSpeechFifo.fifo[g_halSpeechFifoPushRxIdx % HAL_SPEECH_FIFO_DEPTH].rx = *pushedRx;

    // pop a Tx frame from the fifo (could be invalid if g_halSpeechFifoPopRxHappened == 0)
    *popedTx = g_halSpeechFifo.fifo[g_halSpeechFifoPushRxIdx % HAL_SPEECH_FIFO_DEPTH].tx;

    //hal_SpeechTraceTx(popedTx);

    if(g_voiceMuteEnabled != 0)
    {

        if (!hal_SwUserIsActive(FOURCC_VOIS))
        {
            hal_SpeechMuteVoiceFrame(popedTx);
        }
        else if(g_halSpeechMutedCount++ < HAL_SPEECH_MAX_MUTED_COUNT)
        {
            hal_SpeechMuteVoiceFrame(popedTx);
            HAL_TRACE(TSTDOUT|TNB_ARG(0), 0, "HAL Speech - mute frame");

            if(g_halSpeechMutedCount == HAL_SPEECH_MAX_MUTED_COUNT)
            {
                HAL_TRACE(TSTDOUT|TNB_ARG(0), 0, "HAL Speech - Stop muting frame");
            }
        }
    }

    // update fifo's Push Rx state
    g_halSpeechFifoPushRxHappened = TRUE;
    g_halSpeechFifoPushRxIdx++;

    HAL_PROFILE_FUNCTION_EXIT(hal_SpeechPushRxPopTx);
}


// =============================================================================
// hal_SpeechPopRxPushTx
// -----------------------------------------------------------------------------
/// VoiS side of the FIFO accessor. The VoiS speech operation is done under VoC IRQ.
///
/// @param pushedRx Pointer where to copy the received trame.
/// @param popedTx  Pointer to the trame to send to push in the FIFO.
// =============================================================================
PUBLIC VOID hal_SpeechPopRxPushTx(HAL_SPEECH_DEC_IN_T* popedRx,
                                  CONST HAL_SPEECH_ENC_OUT_T* pushedTx)
{
    HAL_PROFILE_FUNCTION_ENTER(hal_SpeechPopRxPushTx);
    UINT32 status;

    // FINT priority is higher than VoC IRQ. So, only
    // VoiS can be interrupted by PAL speech operation.
    status = hal_SysEnterCriticalSection();

    // update fifo's Pop Tx state
    if (!g_halSpeechFifoPushRxHappened)
    {
        // Initial life of the FIFO: only pushed Rx ("sliding" mode).
        // Pop (from VOC side) hasn't happened yet.
        g_halSpeechFifoPushRxIdx++;
    }
    else
    {
        // If one pointer is too fast (underflow), we go back to the "sliding"
        // mode with a reset of the fifo. This should not be interrupted.
        if  (HAL_SPEECH_TX_OVF_RX_UDF)
        {
            g_halSpeechFifo.txOverflowRxUnderflow = TRUE;
            HAL_TRACE(TSTDOUT|TNB_ARG(0), 0, "HAL Speech - Fifo reset on PopRx");
            hal_SpeechFifoInvalidateRx();
            g_halSpeechFifoPushRxHappened = FALSE;
            HAL_SPEECH_RX_HALF_FULL;
        }
    }

    // pop a Rx frame from the fifo (invalid if g_halSpeechFifoPushRxHappened == 0)
    *popedRx = g_halSpeechFifo.fifo[g_halSpeechFifoPopRxIdx % HAL_SPEECH_FIFO_DEPTH].rx;

    //hal_SpeechTraceRx(popedRx);

    // push a valid Tx frame from VoiS
    g_halSpeechFifo.fifo[g_halSpeechFifoPopRxIdx % HAL_SPEECH_FIFO_DEPTH].tx = *pushedTx;

    // update fifo's Push Tx state
    g_halSpeechFifoPopRxHappened = TRUE;
    g_halSpeechFifoPopRxIdx++;

    hal_SysExitCriticalSection(status);

    HAL_PROFILE_FUNCTION_EXIT(hal_SpeechPopRxPushTx);
}


// =============================================================================
//
// -----------------------------------------------------------------------------
// =============================================================================





