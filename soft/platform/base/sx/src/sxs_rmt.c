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





#include "sxr_ops.h"
#include "sxs_rmc.h"
#include "sxs_rmt.h"
#include "sxs_rmt.hp"
#include "sxs_lib.h"
#include "sxs_io.h"
#include "sxr_cpu.h"

#include "sxs_srl.hp"

#ifndef __TARGET__
#include "sxs_udp.h"
#endif

#include "string.h"


// Trace the number of lost msgs when tracing time stamp
#define SXS_RMT_TRACE_DATA_LOST_NUM
#define SXS_RMT_DATA_LOST_NUM_MAX 0xFF


PRIVATE u8 g_sxsRmtInitDone = FALSE;
PRIVATE u8 g_sxsRmtTxDataResetCnt = 0;
PRIVATE u8 g_sxsRmtRxDataResetCnt = 0;


// =============================================================================
// sxs_RmtInit
// -----------------------------------------------------------------------------
/// Data exchange with remote computer initialization.
// =============================================================================
void sxs_RmtInit (bool fullInit)
{
#if (SXS_RMT_NB_BUF == 0)
    pal_TraceSetTxEnabled(TRUE);
    sxs_InitRmc();
    pal_TraceTxFlush(500 MS_WAITING); // maybe there are too many trace before
    pal_TraceSetTxEnabled(FALSE);
    pal_TraceSetRxProc(sxs_PrcsRmtData);
#endif

#if (SXS_RMT_NB_BUF > 0)
    u32 i;
    /* 5ms is not long enough for 3G init, extend to 15ms */
    const u32 MAX_RMT_INIT_TIME = (15 MS_WAITING);
    u32 startTime = hal_TimGetUpTime();

    // NOTE: RMT will be initialized twice if EARLY_TRACE_OPEN defined

    // sxs_InitRmc() will send the init flow, and g_sxsRmtInitDone will be checked
    // during the send
    g_sxsRmtInitDone = TRUE;

    for (i=0; i<SXS_RMT_NB_BUF; i++)
    {
        sxs_PrcsRmtDataArray [i] = sxs_PrcsRmtData;
    }

#if defined(CHIP_HAS_WCPU) && (!defined(WCPU_TRACE_SINGLE_BUFFER))
    memset (&wd_Rmt [0], 0, sizeof (wd_Rmt [0]));
#endif

    for (i=0; i<SXS_RMT_NB_BUF; i++)
    {
        memset (&sxs_Rmt [i], 0, sizeof (sxs_Rmt [i]));
        if (fullInit)
        {
#ifdef __SXS_RMT_PROCESS_IDLE__
            sxs_Rmt [i].RmtQueue = sxr_NewQueue ("Rmt.");
#endif
            sxs_InitRmc ();
        }
    }

    // Make sure the init flow is sent
    if (fullInit)
    {
        // Allow trace output
        sxs_LockFlowControl(PAL_DBG_PORT_IDX, SXS_RELEASE_TX);
        // Send out the init flow
        while (sxs_SerialFlush () &&
                (hal_TimGetUpTime() - startTime) < MAX_RMT_INIT_TIME);
        while (sxs_Flush (PAL_DBG_PORT_IDX) &&
                (hal_TimGetUpTime() - startTime) < MAX_RMT_INIT_TIME);
        // Restore the original trace flow control setting
        sxs_UnlockFlowControl(PAL_DBG_PORT_IDX);
    }
#endif
}


void sxs_ResetTxRmtData (void)
{
    g_sxsRmtTxDataResetCnt++;
}


void sxs_ResetRxRmtData (void)
{
    g_sxsRmtRxDataResetCnt++;
}


// =============================================================================
// sxs_SetPrcsRmtData
// -----------------------------------------------------------------------------
/// Set the function to be called to process the data coming from
/// the specified remote processor.
/// @param RmtCpuIdx            Index of the remote processor.
/// @param sxs_PrcsRmtDataFunc  Function for data process.
// =============================================================================
void sxs_SetPrcsRmtData (u8 RmtCpuIdx, void (*sxs_PrcsRmtDataFunc)(u8, u8*, u16))
{
#if (SXS_RMT_NB_BUF > 0)
    if (SXS_RMT_NB_BUF == 1)
    {
        RmtCpuIdx = 0;
    }

    sxs_PrcsRmtDataArray [RmtCpuIdx] = sxs_PrcsRmtDataFunc;
#endif
}


#ifndef __EMBEDDED__
// =============================================================================
// sxs_RmtBufferFlush
// -----------------------------------------------------------------------------
/// In simulation, flush the remote buffer.
/// @return TRUE when the buffer is not empty, FALSE otherwise.
// =============================================================================
u8 sxs_RmtBufferFlush (void)
{
#ifdef __RMT_TEST_MODE__
#define SXS_FLUSH_BUFFER_SIZE 512
#define SXS_FLUSH_BUFFER_MSK  (SXS_FLUSH_BUFFER_SIZE - 1)

    static u8 Buffer [SXS_FLUSH_BUFFER_SIZE];
    static u16 WIdx = 0;
    static u16 RIdx = 0;
    static u8 DoNotEnter = FALSE;

    u8  RetValue = FALSE;
    u16 Size;
    u8  RmtCpuIdx;

    if (DoNotEnter)
    {
        return FALSE;
    }

    DoNotEnter = TRUE;

    for (RmtCpuIdx = 0; RmtCpuIdx < SXS_RMT_NB_BUF; RmtCpuIdx++)
    {
        do
        {
            Size = (RIdx - WIdx -1) & SXS_FLUSH_BUFFER_MSK;

            if (WIdx + Size > SXS_FLUSH_BUFFER_SIZE)
            {
                Size = SXS_FLUSH_BUFFER_SIZE - WIdx;
            }

            Size = sxs_ReadTxRmtData (&Buffer [WIdx], Size, RmtCpuIdx);

            RetValue = Size != 0;

            WIdx = (WIdx + Size) & SXS_FLUSH_BUFFER_MSK;

            Size = (WIdx - RIdx) & SXS_FLUSH_BUFFER_MSK;

            if (RIdx + Size > SXS_FLUSH_BUFFER_SIZE)
            {
                Size = SXS_FLUSH_BUFFER_SIZE - RIdx;
            }

            //Size = sxs_WriteRxRmtData (&Buffer [RIdx], Size, RmtCpuIdx);
            Size = sxs_WriteRxRmtData (&Buffer [RIdx], Size, 1);

            RIdx = (RIdx + Size) & SXS_FLUSH_BUFFER_MSK;

        }
        while (RetValue);
    }

    DoNotEnter = FALSE;

    sxs_UdpIdleHook ();

    return RetValue;
#else
    return sxs_SerialFlush ();
#endif
}
#endif

void sxs_RmtBufferBack(u8 RmtCpuIdx)
{
#if (SXS_RMT_NB_BUF > 0)
#if defined(CHIP_HAS_WCPU) && (!defined(WCPU_TRACE_SINGLE_BUFFER))
    wd_Rmt[0].RIdx = wd_Rmt[0].RIdx - 256;
#endif
    sxs_Rmt[0].RIdx = sxs_Rmt[0].RIdx - 256;
#endif
}

// =============================================================================
// sxs_SendToRemote
// -----------------------------------------------------------------------------
/// Format data corresponding to the identified flow and store
/// them in remote buffer.
/// @param Id        Flow Id.
/// @param Data      Pointer onto data or NIL.
/// @param Size      Size of data.
/// @param RmtCpuIdx Remote CPU index.
/// @return          Pointer where data can be stored.
// =============================================================================
u8 *_sxs_SendToRemote (u8 Id, u8 *Data, u16 Size, u8 RmtCpuIdx)
{
#if (SXS_RMT_NB_BUF == 0)
    return NIL;
#endif

#if (SXS_RMT_NB_BUF > 0)
    u16 ReqSize, FreeSizeToBufferEnd;
    u32 FreeSize; // declared as u32 to allow 64K SXS_RMT_BUF_SIZE
    u16 Length;
    u16 WIdx = 0;
    u32 i;
    u32 Status;
    u8  *buf = NIL;
    bool success = FALSE;

    if (UNLIKELY(!g_sxsRmtInitDone))
    {
        return NIL;
    }

    if (UNLIKELY(Size == 0))
    {
        return NIL;
    }

#ifndef __EMBEDDED__
    sxs_RmtBufferFlush ();
#endif

    if (SXS_RMT_NB_BUF == 1)
    {
        RmtCpuIdx = 0;
    }

    // The frame structure in the remote buffer is the following.
    // SXS_RMT_START_FRM
    // Frame Length Msb
    // Frame Length Lsb
    // Flow Id
    // Data 1
    //   .
    //   .
    //   .
    // Data n
    // Next Frame offset.

    ReqSize = (Size + SXS_RMT_CASING_SIZE + 3) & ~((u16)3);

    Status = sxr_EnterSc ();
#if defined(CHIP_HAS_WCPU) && defined(WCPU_TRACE_SINGLE_BUFFER)
    while (1)
    {
        if (hal_MemScTake(HAL_MEM_SC_WCPU_TRACE))
            break;
        sxr_ExitSc (Status);
        // Allow others to work while waiting for WCPU releasing the semaphore.
        // Wait for 3 us before next lock so that CPU can see the pending IRQs.
        hal_SysWaitMicrosecond(3);
        Status = sxr_EnterSc ();
    }
#endif

    if ( (ReqSize > SXS_RMT_BUF_SIZE)
            ||   (sxs_Rmt [RmtCpuIdx].Suspended == TRUE) )
    {
#ifdef SXS_RMT_TRACE_DATA_LOST_NUM
        if (sxs_Rmt [RmtCpuIdx].DataLost < SXS_RMT_DATA_LOST_NUM_MAX)
        {
            sxs_Rmt [RmtCpuIdx].DataLost++;
        }
#else
        sxs_Rmt [RmtCpuIdx].DataLost = TRUE;
#endif
        goto _exit;
    }

    if (!sxs_Rmt [RmtCpuIdx].StampStarted)
    {
#ifdef SXS_RMT_TRACE_DATA_LOST_NUM
        if ( (sxs_Rmt [RmtCpuIdx].DataLost > 0)
                ||   ( (sxs_Rmt [RmtCpuIdx].FnStamp != NIL)
                       &&   ( (sxs_Rmt [RmtCpuIdx].Fn & ~(0xff<<24)) !=
                              (*sxs_Rmt [RmtCpuIdx].FnStamp & ~(0xff<<24)) )
                       &&   (Id == SXS_RMC_YIELD)
                     )
           )
#else
        if ( (sxs_Rmt [RmtCpuIdx].DataLost == TRUE)
                ||   ( (sxs_Rmt [RmtCpuIdx].FnStamp != NIL)
                       &&   ( (sxs_Rmt [RmtCpuIdx].Fn & ~(1<<31)) !=
                              (*sxs_Rmt [RmtCpuIdx].FnStamp & ~(1<<31)) )
                       &&   (Id == SXS_RMC_YIELD)
                     )
           )
#endif
        {
            sxs_Rmt [RmtCpuIdx].Fn = (sxs_Rmt [RmtCpuIdx].FnStamp != NIL) ?
                                     *sxs_Rmt [RmtCpuIdx].FnStamp :
                                     0;

#ifdef SXS_RMT_TRACE_DATA_LOST_NUM
            sxs_Rmt [RmtCpuIdx].Fn &= ~(0xff<<24);
            if (sxs_Rmt [RmtCpuIdx].DataLost > 0)
            {
                sxs_Rmt [RmtCpuIdx].Fn |= (sxs_Rmt [RmtCpuIdx].DataLost << 24);
            }
#else
            if (sxs_Rmt [RmtCpuIdx].DataLost == TRUE)
            {
                sxs_Rmt [RmtCpuIdx].Fn |= (1 << 31);
            }
#endif

            sxs_Rmt [RmtCpuIdx].StampStarted = TRUE;
            u8   TimeData[8];
            u32 ticktime = hal_TimGetUpTime();
            memcpy(TimeData, &sxs_Rmt [RmtCpuIdx].Fn, 4);
            memcpy(TimeData + 4, &ticktime, 4);
            _sxs_SendToRemote (SXS_TIME_STAMP_RMC, TimeData, 8, RmtCpuIdx);
            //_sxs_SendToRemote (SXS_TIME_STAMP_RMC, (u8 *)&sxs_Rmt [RmtCpuIdx].Fn, 4, RmtCpuIdx);
            sxs_Rmt [RmtCpuIdx].StampStarted = FALSE;
        }
    }

    WIdx = sxs_Rmt [RmtCpuIdx].WIdx;

    FreeSize = (sxs_Rmt [RmtCpuIdx].RIdx - WIdx - 1) & SXS_RMT_BUF_MOD;

    if ( ( (!(Id & SXS_RMC_YIELD)) && (FreeSize < (SXS_RMT_BUF_SIZE / 2)) )
            ||  (FreeSize < ReqSize))
    {
#ifdef SXS_RMT_TRACE_DATA_LOST_NUM
        if ( (Id & SXS_RMC_YIELD) != 0
                &&   sxs_Rmt [RmtCpuIdx].DataLost < SXS_RMT_DATA_LOST_NUM_MAX )
        {
            sxs_Rmt [RmtCpuIdx].DataLost++;
        }
#else
        sxs_Rmt [RmtCpuIdx].DataLost = ((Id & SXS_RMC_YIELD) != 0)||sxs_Rmt [RmtCpuIdx].DataLost;
#endif
        goto _exit;
    }

    if (Data == NIL)
    {
        FreeSizeToBufferEnd = SXS_RMT_BUF_SIZE - WIdx;

        if (FreeSizeToBufferEnd < ReqSize)
        {
            /* Jump on buffer top necessary. */

            /* If the buffer is empty, we will start from the beginning.
             * Otherwise, test that the empty space at the beginning of the
             * buffer is big enough to fit. */
            if ((FreeSize - FreeSizeToBufferEnd) < ReqSize && (sxs_Rmt [RmtCpuIdx].RIdx != WIdx))
            {
#ifdef SXS_RMT_TRACE_DATA_LOST_NUM
                if ( (Id & SXS_RMC_YIELD) != 0
                        &&   sxs_Rmt [RmtCpuIdx].DataLost < SXS_RMT_DATA_LOST_NUM_MAX )
                {
                    sxs_Rmt [RmtCpuIdx].DataLost++;
                }
#else
                sxs_Rmt [RmtCpuIdx].DataLost = (Id & SXS_RMC_YIELD) != 0;
#endif
                goto _exit;
            }

            if (sxs_Rmt [RmtCpuIdx].RIdx == WIdx)
            {
                sxs_Rmt [RmtCpuIdx].RIdx = 0;
                FreeSize = SXS_RMT_BUF_SIZE;
            }
            else
            {
                FreeSize = sxs_Rmt [RmtCpuIdx].RIdx - 1;
            }

            sxs_Rmt [RmtCpuIdx].Buf [sxs_Rmt [RmtCpuIdx].NextIdx & SXS_RMT_BUF_MOD] = 0;
            WIdx = 0;
        }
    }

    if (Id == SXS_TIME_STAMP_RMC)
    {
#ifdef SXS_RMT_TRACE_DATA_LOST_NUM
        sxs_Rmt [RmtCpuIdx].DataLost = 0;
#else
        sxs_Rmt [RmtCpuIdx].DataLost = FALSE;
#endif
    }

    Length = Size + 1;   /* One byte Id comes on top. */

    sxs_Rmt [RmtCpuIdx].Buf [WIdx & SXS_RMT_BUF_MOD] = SXS_RMT_START_FRM;
    WIdx = (WIdx + 1) & SXS_RMT_BUF_MOD;

    sxs_Rmt [RmtCpuIdx].Buf [WIdx] = Length >> 8;
    WIdx = (WIdx + 1) & SXS_RMT_BUF_MOD;

    sxs_Rmt [RmtCpuIdx].Buf [WIdx] = (u8) Length;
    WIdx = (WIdx + 1) & SXS_RMT_BUF_MOD;

    sxs_Rmt [RmtCpuIdx].Buf [WIdx] = Id;
    WIdx = (WIdx + 1) & SXS_RMT_BUF_MOD;

#ifdef __MULTI_RAT__
    if(Id == SXS_TRACE_RMC)
    {
        u32 ticktime = hal_TimGetUpTime();
        //reseved 2 bytes for id
        sxs_Rmt [RmtCpuIdx].Buf [WIdx + 2] = ticktime & 0xff;
        sxs_Rmt [RmtCpuIdx].Buf [WIdx + 3] = (ticktime>>8) & 0xff;
        sxs_Rmt [RmtCpuIdx].Buf [WIdx + 4] = (ticktime>>16) & 0xff;
        sxs_Rmt [RmtCpuIdx].Buf [WIdx + 5] = (ticktime>>24) & 0xff;
    }
#endif

    sxs_Rmt [RmtCpuIdx].NextIdx = (WIdx + Size) & SXS_RMT_BUF_MOD;
    sxs_Rmt [RmtCpuIdx].WIdx    = ((sxs_Rmt [RmtCpuIdx].NextIdx + 3) & ~((u16)3)) & SXS_RMT_BUF_MOD;

    if (sxs_Rmt [RmtCpuIdx].NextIdx == sxs_Rmt [RmtCpuIdx].WIdx)
    {
        sxs_Rmt [RmtCpuIdx].WIdx += 4;
    }

    sxs_Rmt [RmtCpuIdx].Buf [sxs_Rmt [RmtCpuIdx].NextIdx] = ((sxs_Rmt [RmtCpuIdx].WIdx - sxs_Rmt [RmtCpuIdx].NextIdx - 1) & 3) + 1;

    success = TRUE;

_exit:
#if defined(CHIP_HAS_WCPU) && defined(WCPU_TRACE_SINGLE_BUFFER)
    hal_MemScRelease(HAL_MEM_SC_WCPU_TRACE);
#endif
    sxr_ExitSc (Status);

    if (success)
    {
        if (Data == NIL)
        {
            buf = &sxs_Rmt [RmtCpuIdx].Buf [WIdx& SXS_RMT_BUF_MOD];
        }
        else
        {
            for (i=0; i<Size; i++)
            {
                sxs_Rmt [RmtCpuIdx].Buf [(i+WIdx) & SXS_RMT_BUF_MOD] = Data [i];
            }

#ifndef __EMBEDDED__
            sxs_RmtBufferFlush ();
#endif
        }
    }

    return buf;
#endif
}


// =============================================================================
// sxs_ReadTxRmtData
// -----------------------------------------------------------------------------
/// Read a bunch of data from the debug remote buffer.
/// @param DataOut   points a buffer where data must be copied.
/// @param Size      Size in bytes of buffer pointed by DataOut.
/// @param RmtCpuIdx Remote CPU index.
/// @return          Number of bytes copied in buffer.
// =============================================================================
u16 sxs_ReadTxRmtData (u8 *DataOut, u16 Size, u8 RmtCpuIdx)
{
#if (SXS_RMT_NB_BUF == 0)
    return 0;
#endif

#if (SXS_RMT_NB_BUF > 0)
    static struct
    {
        u8  CheckSum;
        u8  HeaderCnt;
        u8  CiphKey;
        u16 Length;
    } sxs_ReadTxRmt [SXS_RMT_NB_BUF];

    static u8 txResetCnt = 0;

    u16 RIdx, SizeAvailable;
    u16 DataSize = 0;
    u16 i, Index = 0;
    u16 *RIdxPtr, *WIdxPtr;
    u8  *Buf;

    if (txResetCnt != g_sxsRmtTxDataResetCnt)
    {
        txResetCnt = g_sxsRmtTxDataResetCnt;
        memset (sxs_ReadTxRmt, 0, sizeof(sxs_ReadTxRmt));
    }

    if (SXS_RMT_NB_BUF == 1)
    {
        RmtCpuIdx = 0;
    }

#if defined(CHIP_HAS_WCPU) && !defined(WCPU_TRACE_SINGLE_BUFFER)
    static u8 procWdData = FALSE;
    if (procWdData)
    {
        if (((wd_Rmt [0].WIdx - wd_Rmt [0].RIdx) & SXS_RMT_BUF_MOD) == 0)
        {
            // Switch to SXS trace buffer
            procWdData = FALSE;
            RIdxPtr = &sxs_Rmt [RmtCpuIdx].RIdx;
            WIdxPtr = &sxs_Rmt [RmtCpuIdx].WIdx;
            Buf = sxs_Rmt [RmtCpuIdx].Buf;
        }
        else
        {
            // Use WD trace buffer
            RIdxPtr = &wd_Rmt [0].RIdx;
            WIdxPtr = &wd_Rmt [0].WIdx;
            Buf = wd_Rmt [0].Buf;
        }
    }
    else // !procWdData
    {
        if (((sxs_Rmt [RmtCpuIdx].WIdx - sxs_Rmt [RmtCpuIdx].RIdx) &
                SXS_RMT_BUF_MOD) == 0)
        {
            // Switch to WD trace buffer
            procWdData = TRUE;
            RIdxPtr = &wd_Rmt [0].RIdx;
            WIdxPtr = &wd_Rmt [0].WIdx;
            Buf = wd_Rmt [0].Buf;
        }
        else
        {
            // Use SXS trace buffer
            RIdxPtr = &sxs_Rmt [RmtCpuIdx].RIdx;
            WIdxPtr = &sxs_Rmt [RmtCpuIdx].WIdx;
            Buf = sxs_Rmt [RmtCpuIdx].Buf;
        }
    }
#else // not WCPU dual trace buffer
    RIdxPtr = &sxs_Rmt [RmtCpuIdx].RIdx;
    WIdxPtr = &sxs_Rmt [RmtCpuIdx].WIdx;
    Buf = sxs_Rmt [RmtCpuIdx].Buf;
#endif

    RIdx = *RIdxPtr;
    SizeAvailable = (*WIdxPtr - *RIdxPtr) & SXS_RMT_BUF_MOD;

    if ( (SizeAvailable == 0)
            ||   (Size == 0) )
    {
        return 0;
    }

    if (sxs_ReadTxRmt [RmtCpuIdx].HeaderCnt < 3)
    {
        u32 HdLp = MIN (Size, SizeAvailable);
        HdLp = MIN (HdLp, (u32)(3 - sxs_ReadTxRmt [RmtCpuIdx].HeaderCnt));

        for (i=0; i< HdLp; i++)
        {
            switch (sxs_ReadTxRmt [RmtCpuIdx].HeaderCnt)
            {
                case 0:
                    if (Buf [*RIdxPtr] != SXS_RMT_START_FRM)
                    {
                        *RIdxPtr = (*RIdxPtr + 1) & SXS_RMT_BUF_MOD;
                        return 0;
                    }
                    DataOut [0] = SXS_RMT_START_FRM;
                    break;

                case 1:
                    sxs_ReadTxRmt [RmtCpuIdx].Length = (Buf [RIdx] << 8);
                    break;

                case 2:
                    sxs_ReadTxRmt [RmtCpuIdx].CiphKey = SXS_RMT_START_FRM; // Buf [RIdx];
                    sxs_ReadTxRmt [RmtCpuIdx].Length |= Buf [RIdx];
                    sxs_ReadTxRmt [RmtCpuIdx].Length += 1; /* Add one byte for Check sum. */
                    break;
            }

            DataOut [Index++] = Buf [RIdx];
            RIdx = (RIdx + 1) & SXS_RMT_BUF_MOD;
            sxs_ReadTxRmt [RmtCpuIdx].HeaderCnt++;
        }
    }

    DataSize = MIN (Size - Index, sxs_ReadTxRmt [RmtCpuIdx].Length);

    if (DataSize > SizeAvailable)
    {
        DataSize = SizeAvailable;
    }

    for (i=0; i<DataSize; i++)
    {
        if (sxs_ReadTxRmt [RmtCpuIdx].Length != 1)
        {
#ifdef __SXS_RMT_CIPH__
            DataOut [Index] = Buf [RIdx] ^ sxs_ReadTxRmt [RmtCpuIdx].CiphKey;
#else
            DataOut [Index] = Buf [RIdx];
#endif
            sxs_ReadTxRmt [RmtCpuIdx].CheckSum ^= DataOut [Index];
            RIdx = (RIdx + 1) & SXS_RMT_BUF_MOD;
        }
        else // if (sxs_ReadTxRmt [RmtCpuIdx].Length == 0)
        {
            DataOut [Index] = sxs_ReadTxRmt [RmtCpuIdx].CheckSum;
        }

        sxs_ReadTxRmt [RmtCpuIdx].Length--;

        Index++;
    }

    *RIdxPtr = RIdx;

    if ( (sxs_ReadTxRmt [RmtCpuIdx].HeaderCnt == 3)
            &&   (sxs_ReadTxRmt [RmtCpuIdx].Length == 0) )
    {
        u32 Status = sxr_EnterSc ();

        if (Buf [*RIdxPtr] == 0)
        {
            // This is an error
            *RIdxPtr = 0;
        }
        else
        {
            *RIdxPtr = (*RIdxPtr + Buf [*RIdxPtr]) & SXS_RMT_BUF_MOD;
        }

        sxs_ReadTxRmt [RmtCpuIdx].HeaderCnt = 0;
        sxs_ReadTxRmt [RmtCpuIdx].CheckSum  = 0;

        sxr_ExitSc (Status);
    }

    return Index;
#endif
}


// =============================================================================
// sxs_WriteRxRmtData
// -----------------------------------------------------------------------------
/// Extract and process a frame coming from the remote computer
/// from the reception buffer.
/// @param DataIn    points to the remote computer Rx data.
/// @param Size      is the number of bytes available in DataIn.
/// @param RmtCpuIdx Remote CPU index.
/// @return          The number of bytes processed in DataIn.
// =============================================================================
u16 sxs_WriteRxRmtData (u8 *DataIn, u16 Size, u8 RmtCpuIdx)
{
#if (SXS_RMT_NB_BUF == 0)
    return 0;
#endif

#if (SXS_RMT_NB_BUF > 0)
    static struct
    {
        u8 *Data;
        u16 Length;
        u16 Cnt;
        u8  HeaderCnt;
        u8  Id;
        u8  CiphKey;
        u8  CheckSum;
    } sxs_WriteRxRmt [SXS_RMT_NB_BUF];

    static u8 rxResetCnt = 0;

    u32 i;
    u16 Index      = 0;
    u16 DataSize   = 0;

    if (rxResetCnt != g_sxsRmtRxDataResetCnt)
    {
        rxResetCnt = g_sxsRmtRxDataResetCnt;
#ifndef __SXS_RMT_PROCESS_IDLE__
        for (i = 0; i < SXS_RMT_NB_BUF; i++)
        {
            if (sxs_WriteRxRmt [i].Data)
            {
                sxr_HFree (sxs_WriteRxRmt [i].Data);
            }
        }
#endif
        memset (sxs_WriteRxRmt, 0, sizeof(sxs_WriteRxRmt));
    }

    if (Size == 0)
    {
        return 0;
    }

    if (SXS_RMT_NB_BUF == 1)
    {
        RmtCpuIdx = 0;
    }

    if (sxs_WriteRxRmt [RmtCpuIdx].HeaderCnt < 4)
    {
        u32 HdLp = MIN (Size, 4 - sxs_WriteRxRmt [RmtCpuIdx].HeaderCnt);

        for (i=0; i<HdLp; i++)
        {
            switch (sxs_WriteRxRmt [RmtCpuIdx].HeaderCnt)
            {
                case 0:
                    if (DataIn [Index] != SXS_RMT_START_FRM)
                    {
                        return 1;
                    }
                    break;

                case 1:
                    sxs_WriteRxRmt [RmtCpuIdx].Length  = DataIn [Index] << 8;
                    break;

                case 2:
                    sxs_WriteRxRmt [RmtCpuIdx].Length |= DataIn [Index];
                    sxs_WriteRxRmt [RmtCpuIdx].CiphKey = SXS_RMT_START_FRM; //DataIn [Index];

#ifdef __TARGET__
                    if (sxs_WriteRxRmt [RmtCpuIdx].Length < 4000)
#endif
                    {
                        sxs_WriteRxRmt [RmtCpuIdx].Data = (u8 *)sxr_HMalloc ((u16)(sxs_WriteRxRmt [RmtCpuIdx].Length + sizeof (sxs_RmtHeader_t)));
                    }

                    if (sxs_WriteRxRmt [RmtCpuIdx].Data == NIL)
                    {
                        SXS_TRACE (_SXR|TSTDOUT|TDB|TNB_ARG(1),TSTR("Rejected frame, size %i\n",0x06be0001), sxs_WriteRxRmt [RmtCpuIdx].Length);
                        sxs_WriteRxRmt [RmtCpuIdx].HeaderCnt = 0;
                        return Size;    /* No more memory, frame is lost. */
                    }
                    break;

                case 3:
#ifdef __SXS_RMT_CIPH__
                    sxs_WriteRxRmt [RmtCpuIdx].Id = DataIn [Index] ^ sxs_WriteRxRmt [RmtCpuIdx].CiphKey;
#else
                    sxs_WriteRxRmt [RmtCpuIdx].Id = DataIn [Index];
#endif
                    sxs_WriteRxRmt [RmtCpuIdx].CheckSum ^= DataIn [Index];
                    break;

            }
            sxs_WriteRxRmt [RmtCpuIdx].HeaderCnt++;
            Index++;
        }
    }

    DataSize = MIN (Size - Index, sxs_WriteRxRmt [RmtCpuIdx].Length - sxs_WriteRxRmt [RmtCpuIdx].Cnt);

    for (i=0; i<DataSize; i++)
    {
#ifdef __SXS_RMT_CIPH__
        sxs_WriteRxRmt [RmtCpuIdx].Data [sizeof (sxs_RmtHeader_t) + sxs_WriteRxRmt [RmtCpuIdx].Cnt++] = DataIn [Index] ^ sxs_WriteRxRmt [RmtCpuIdx].CiphKey;
#else
        sxs_WriteRxRmt [RmtCpuIdx].Data [sizeof (sxs_RmtHeader_t) + sxs_WriteRxRmt [RmtCpuIdx].Cnt++] = DataIn [Index];
#endif
        sxs_WriteRxRmt [RmtCpuIdx].CheckSum ^= DataIn [Index];
        Index++;
    }

    if ( (sxs_WriteRxRmt [RmtCpuIdx].HeaderCnt == 4)
            &&   (sxs_WriteRxRmt [RmtCpuIdx].Length <= sxs_WriteRxRmt [RmtCpuIdx].Cnt) )
    {
        u8 UsrPrcsDone = FALSE;

        /* Original length includes Id, not check sum. Here it must be understood        */
        /* as length of data + 1 byte checksum. Checksum is the last byte of the buffer. */

        if (sxs_WriteRxRmt [RmtCpuIdx].CheckSum != 0)
        {
            SXS_TRACE (_SXR|TSTDOUT|TDB|TNB_ARG(3),TSTR("Check sum error, flow 0x%x Size %i (0x%x)\n",0x06be0002), sxs_WriteRxRmt [RmtCpuIdx].Id, sxs_WriteRxRmt [RmtCpuIdx].Length, &sxs_WriteRxRmt);
        }
        else
        {
#ifndef __SXS_RMT_PROCESS_IDLE__
            if (sxs_Rmt [RmtCpuIdx].UserPrcsRmtData != NIL)
            {
                if (sxs_Rmt [RmtCpuIdx].UserPrcsRmtData (sxs_WriteRxRmt [RmtCpuIdx].Id, &sxs_WriteRxRmt [RmtCpuIdx].Data [sizeof (sxs_RmtHeader_t)], sxs_WriteRxRmt [RmtCpuIdx].Length))
                {
                    UsrPrcsDone = TRUE;
                }
            }

            // When there is trace buffer to recover during startup (sxs_GetRmtBuffer), target will run to here during startup.
            // At the same time, if remote computer sends any command (e.g., trace levels) to target, crash will happen, as
            // sxs_PrcsRmtDataArray[] has NOT been initialized yet (sxs_RmtInit),
            if (!UsrPrcsDone && sxs_PrcsRmtDataArray[RmtCpuIdx])
            {
                sxs_PrcsRmtDataArray [RmtCpuIdx] (sxs_WriteRxRmt [RmtCpuIdx].Id, &sxs_WriteRxRmt [RmtCpuIdx].Data [sizeof (sxs_RmtHeader_t)], sxs_WriteRxRmt [RmtCpuIdx].Length);
            }

#else
            {
                sxs_RmtHeader_t *sxs_RmtHeader = (sxs_RmtHeader_t *)sxs_WriteRxRmt [RmtCpuIdx].Data;
                sxs_RmtHeader -> Id = sxs_WriteRxRmt [RmtCpuIdx].Id;
                sxs_RmtHeader -> Length = sxs_WriteRxRmt [RmtCpuIdx].Length;
                sxr_PopIn(sxs_WriteRxRmt [RmtCpuIdx].Data, sxs_Rmt [RmtCpuIdx].RmtQueue);
            }
#endif

            //SXS_DUMP (_SXR, "%2x", sxs_WriteRxRmt [RmtCpuIdx].Data, sxs_WriteRxRmt [RmtCpuIdx].Length);
        }

#ifndef __SXS_RMT_PROCESS_IDLE__
        sxr_HFree (sxs_WriteRxRmt [RmtCpuIdx].Data);
#endif

        sxs_WriteRxRmt [RmtCpuIdx].HeaderCnt = FALSE;
        sxs_WriteRxRmt [RmtCpuIdx].Cnt       = 0;
        sxs_WriteRxRmt [RmtCpuIdx].CheckSum  = 0;
        sxs_WriteRxRmt [RmtCpuIdx].Data      = 0;
    }

    return Index;
#endif
}


// =============================================================================
// sxs_RmtIdleHook
// -----------------------------------------------------------------------------
/// Process remote data during idle.
// =============================================================================
void sxs_RmtIdleHook (void)
{
#if (SXS_RMT_NB_BUF > 0)
#ifdef __SXS_RMT_PROCESS_IDLE__
    u32 i;

    for (i=0; i<SXS_RMT_NB_BUF; i++)
    {
        sxs_RmtHeader_t *sxs_RmtHeader;

        //sxr_CheckQueue (sxs_Rmt [i].RmtQueue);

        while ((sxs_RmtHeader = (sxs_RmtHeader_t *)sxr_PopOut(sxs_Rmt [i].RmtQueue )) != NIL)
        {
            //sxs_PrcsRmtData (sxs_RmtHeader -> Id, ((u8 *)sxs_RmtHeader) + sizeof (sxs_RmtHeader_t), sxs_RmtHeader -> Length);
            sxs_PrcsRmtDataArray [i] (sxs_RmtHeader -> Id, ((u8 *)sxs_RmtHeader) + sizeof (sxs_RmtHeader_t), sxs_RmtHeader -> Length);
            sxr_HFree (sxs_RmtHeader);
        }
    }
#endif
#ifndef __TARGET__
    sxs_RmtBufferFlush ();
#endif
#endif // SXS_RMT_NB_BUF

}


// =============================================================================
// sxs_RmtSuspend
// -----------------------------------------------------------------------------
/// Suspend trace storage remote buffer.
/// @param RmtCpuIdx Remote CPU index.
// =============================================================================
void sxs_RmtSuspend (u8 RmtCpuIdx)
{
#if (SXS_RMT_NB_BUF > 0)
    if (SXS_RMT_NB_BUF == 1)
    {
        RmtCpuIdx = 0;
    }

    sxs_Rmt [RmtCpuIdx].Suspended = TRUE;
#endif
}


// =============================================================================
// sxs_RmtResume
// -----------------------------------------------------------------------------
/// Resume trace storage in remote buffer.
/// @param RmtCpuIdx Remote CPU index.
// =============================================================================
void sxs_RmtResume (u8 RmtCpuIdx)
{
#if (SXS_RMT_NB_BUF > 0)
    if (SXS_RMT_NB_BUF == 1)
    {
        RmtCpuIdx = 0;
    }

    sxs_Rmt [RmtCpuIdx].Suspended = FALSE;
#endif
}

// =============================================================================
// sxs_GetRmtBufferStatus
// -----------------------------------------------------------------------------
/// Get the status on the remote buffer.
/// @param RmtCpuIdx Remote CPU index.
/// @return          Number of data present in remote buffer.
// =============================================================================
u16 sxs_GetRmtBufferStatus (u8 RmtCpuIdx)
{
#if (SXS_RMT_NB_BUF == 0)
    return 0;
#endif

#if (SXS_RMT_NB_BUF > 0)
    if (SXS_RMT_NB_BUF == 1)
    {
        RmtCpuIdx = 0;
    }

    return (sxs_Rmt [RmtCpuIdx].WIdx - sxs_Rmt [RmtCpuIdx].RIdx) & SXS_RMT_BUF_MOD;
#endif
}


// =============================================================================
// sxs_SetFnStamp
// -----------------------------------------------------------------------------
/// Start trace time stamping.
/// @param FnStamp   Address of the frame number stamp.
/// @param RmtCpuIdx Remote CPU index.
// =============================================================================
void _sxs_SetFnStamp (u32 *FnStamp, u8 RmtCpuIdx)
{
#if (SXS_RMT_NB_BUF == 0)
    pal_TraceSetFnStamp(FnStamp);
#endif

#if (SXS_RMT_NB_BUF > 0)
    if (SXS_RMT_NB_BUF == 1)
    {
        RmtCpuIdx = 0;
    }

    sxs_Rmt [RmtCpuIdx].FnStamp = FnStamp;
#endif
}


// =============================================================================
// sxs_GetFnStamp
// -----------------------------------------------------------------------------
/// Get current stamping value.
/// @param RmtCpuIdx Remote CPU index.
/// @return          Stamp value.
// =============================================================================
u32 _sxs_GetFnStamp (u8 RmtCpuIdx)
{
#if (SXS_RMT_NB_BUF == 0)
    return 0;
#endif

#if (SXS_RMT_NB_BUF > 0)
    if (SXS_RMT_NB_BUF == 1)
    {
        RmtCpuIdx = 0;
    }

    return *sxs_Rmt [RmtCpuIdx].FnStamp;
#endif
}


// =============================================================================
// sxs_SetUserPrcsRmtData
// -----------------------------------------------------------------------------
/// Set the user function designed to process data from remote computer.
/// @param UserPrcsRmtData  User function.
/// @param RmtCpuIdx        Remote CPU index.
// =============================================================================
void sxs_SetUserPrcsRmtData (u8 (*UserPrcsRmtData) (u8, u8 *, u16), u8 RmtCpuIdx)
{
#if (SXS_RMT_NB_BUF > 0)
    if (SXS_RMT_NB_BUF == 1)
    {
        RmtCpuIdx = 0;
    }

    sxs_Rmt [RmtCpuIdx].UserPrcsRmtData = UserPrcsRmtData;
#endif
}


#ifdef __TARGET__
// =============================================================================
// sxs_SaveRmtBuffer
// -----------------------------------------------------------------------------
/// Save the remote buffer to the end of head to allow trace recovery after reset.
// =============================================================================
void sxs_SaveRmtBuffer (void)
{
#if (SXS_RMT_NB_BUF > 0)
#ifdef __SXS_SAVE_RMT_BUFFER_IN_HEAP__
    sxs_Rmt_t *Address = (sxs_Rmt_t *) sxr_GetHeapEnd (0);
    // Heaps are allocated only after OS is started
    if (NIL == Address)
    {
        return;
    }
    Address--;
    memcpy (Address, &sxs_Rmt [SXS_RMT_DBG_CPU_IDX],
            sizeof (sxs_Rmt [SXS_RMT_DBG_CPU_IDX]));
#else
    sxs_RmtSuspend (SXS_RMT_DBG_CPU_IDX);
    sxs_Rmt [SXS_RMT_DBG_CPU_IDX].SavedRIdx = sxs_Rmt [SXS_RMT_DBG_CPU_IDX].RIdx;
#endif
#endif // SXS_RMT_NB_BUF

    sxs_Boot -> RmtBufferSaved = SXS_RMT_BUFFER_SAVED_FLAG;
}


// =============================================================================
// sxs_GetRmtBuffer
// -----------------------------------------------------------------------------
/// Restore saved remote buffer by sxs_SaveRmtBuffer() and place it in new trace
/// buffer.
// =============================================================================
static void sxs_InitSxsBoot (void)
{
    sxs_Boot -> Pattern = SXS_BOOT_PATTERN;
    sxs_Boot -> ExceptionCount = 0;
    sxs_Boot -> DwlReq = FALSE;
    sxs_Boot -> ExceptionExpected = FALSE;
    sxs_Boot -> RmtBufferSaved = 0;
}

#ifdef __SXS_SAVE_RMT_BUFFER_IN_HEAP__
static int sxs_CheckRmtBuffer (sxs_Rmt_t *rmt)
{
    if (rmt -> WIdx >= SXS_RMT_BUF_SIZE)
        return 0;
    if (rmt -> RIdx >= SXS_RMT_BUF_SIZE)
        return 0;
    if (rmt -> NextIdx >= SXS_RMT_BUF_SIZE)
        return 0;
    // Reset all sxs_Rmt pointers to NIL to avoid crashes when
    // this is not a real RMT buffer content
    rmt -> FnStamp = NIL;
    rmt -> UserPrcsRmtData = NIL;
    rmt -> ATWriteRxData = NIL;

    return 1;
}
#endif

void sxs_GetRmtBuffer (void)
{
    const u32 MAX_TRACE_RECOVERY_TIME = (1000 MS_WAITING);
    u32 startTime = hal_TimGetUpTime();
    u8 rmtInitStatus;

    if (sxs_Boot -> Pattern != SXS_BOOT_PATTERN)
    {
        sxs_InitSxsBoot ();
    }
    else
    {
        if (sxs_Boot -> RmtBufferSaved == SXS_RMT_BUFFER_SAVED_FLAG)
        {
            sxs_Boot -> RmtBufferSaved = 0;

#if (SXS_RMT_NB_BUF == 0)
            sxs_InitSxsBoot();
#endif

#if (SXS_RMT_NB_BUF > 0)
#ifdef __SXS_SAVE_RMT_BUFFER_IN_HEAP__
            sxs_Rmt_t *Address = (sxs_Rmt_t *) sxr_GetHeapEnd (0);
            // Heaps are allocated only after OS is started
            if (NIL == Address)
            {
                sxs_InitSxsBoot ();
                return;
            }

            Address--;
            if (!sxs_CheckRmtBuffer (Address))
            {
                sxs_InitSxsBoot ();
                return;
            }

#if 0
            SXS_TRACE (_SXR|TSTDOUT|TDB,TSTR("--------- Trace buffer recovery ----------\n",0x06be0003));
            while (sxs_SerialFlush () &&
                    (hal_TimGetUpTime() - startTime) < MAX_TRACE_RECOVERY_TIME);
#endif

            memcpy (&sxs_Rmt [SXS_RMT_DBG_CPU_IDX], Address,
                    sizeof (sxs_Rmt) / SXS_RMT_NB_BUF);
#else
            sxs_Rmt [SXS_RMT_DBG_CPU_IDX].RIdx = sxs_Rmt [SXS_RMT_DBG_CPU_IDX].SavedRIdx;
            sxs_RmtResume (SXS_RMT_DBG_CPU_IDX);
#endif

            // Get current RMT init status
            rmtInitStatus = g_sxsRmtInitDone;
            // Allow to store trace in RMT buffer
            g_sxsRmtInitDone = TRUE;
            // Allow trace output
            sxs_LockFlowControl(PAL_DBG_PORT_IDX, SXS_RELEASE_TX);

            // Output previous trace content
            while (sxs_SerialFlush () &&
                    (hal_TimGetUpTime() - startTime) < MAX_TRACE_RECOVERY_TIME);

            SXS_TRACE (_SXR|TSTDOUT|TDB,TSTR("--------- End of trace buffer recovery ----------\n",0x06be0004));
            while (sxs_SerialFlush () &&
                    (hal_TimGetUpTime() - startTime) < MAX_TRACE_RECOVERY_TIME);
            while (sxs_Flush (PAL_DBG_PORT_IDX) &&
                    (hal_TimGetUpTime() - startTime) < MAX_TRACE_RECOVERY_TIME);

            // Restore the original trace flow control setting
            sxs_UnlockFlowControl(PAL_DBG_PORT_IDX);
            // Restore RMT init status
            g_sxsRmtInitDone = rmtInitStatus;
#endif
        }
    }
}
#endif


// =============================================================================
// sxs_ChangeSerialSpeed
// -----------------------------------------------------------------------------
/// Change serial speed and inform remote computer.
/// @param Speed     New Speed.
/// @param SerialIdx Serial port idx.
// =============================================================================
#ifdef __SXS_SPLIT_2_CPU__
void sxs_ChangeSerialSpeed (u8 Speed, u8 SerialIdx)
{
    u16 Setting;
    sxs_FlwSerial_t *sxs_FlwSerial = (sxs_FlwSerial_t *)_sxs_SendToRemote (SXS_SERIAL_RMC, NIL, sizeof (sxs_FlwSerial_t), SXS_RMT_SPL_CPU_IDX);

    sxs_FlwSerial -> Id        = SXS_RMC_REQ;
    sxs_FlwSerial -> Speed     = Speed;
    sxs_FlwSerial -> SerialIdx = 0;

    while (sxs_SerialFlush ());

    Setting = sxs_GetSerialCfg (SerialIdx);
    sxs_SetSerialCfg ((Setting & ~SXS_RATE_MSK) | SXS_SET_RATE(Speed), SerialIdx);

}
#endif


// =============================================================================
// sxs_GetRmtBufInfo
// -----------------------------------------------------------------------------
/// Get the address and length of the first RMT buffer
/// @param addr     Pointer to save the buffer address
/// @param len      Pointer to save the buffer len
// =============================================================================
void sxs_GetRmtBufInfo (u32 *addr, u32 *len)
{
#if (SXS_RMT_NB_BUF > 0)
    if (addr)
        *addr = (u32)&sxs_Rmt[0];
    if (len)
        *len = sizeof(sxs_Rmt[0]);
#endif
}


#if defined(CHIP_HAS_WCPU) && !defined(WCPU_TRACE_SINGLE_BUFFER)
// =============================================================================
// sxs_GetWdRmtBufInfo
// -----------------------------------------------------------------------------
/// Get the address and length of the WCPU RMT buffer
/// @param addr     Pointer to save the buffer address
/// @param len      Pointer to save the buffer len
// =============================================================================
void sxs_GetWdRmtBufInfo (u32 *addr, u32 *len)
{
    if (addr)
        *addr = (u32)&wd_Rmt[0];
    if (len)
        *len = sizeof(wd_Rmt[0]);
}
#endif

#ifdef _TTCN_TEST_
void sxs_sendTtcnDataToWcpu(u8 Id, u8 *Data, u16 len)
{
    u16 rIdx = wd_Rmt [0].ttcnRIdx;
    u16 wIdx = wd_Rmt [0].ttcnWIdx;
    u16 room = 0;

    if (rIdx == wIdx)
    {
        room = SXS_ZSP_TTCN_RMT_BUF_SIZE_MOD;
    }
    else if (rIdx < wIdx)
    {
        room = SXS_ZSP_TTCN_RMT_BUF_SIZE_MOD - (wIdx - rIdx);
    }
    else //  if (rIdx > wIdx)
    {
        room = rIdx - wIdx - 1;
    }

    if (room == 0)
    {
        SXS_RAISE((_SXR|TABORT, "ZSP TTCN BUFFER is full\n"));
    }

    // If the packet should be sent as a whole part,
    // check whether the free space is enough
    if (len > room)
    {
        SXS_RAISE((_SXR|TABORT, "ZSP TTCN BUFFER is nearly full, send data length too long\n"));
    }

    len = (len < room) ? len : room;

    if (wIdx + len <= SXS_ZSP_TTCN_RMT_BUF_SIZE)
    {
        memcpy(&wd_Rmt [0].ttcnBuf[wIdx], Data, len);
    }
    else
    {
        memcpy(&wd_Rmt [0].ttcnBuf[wIdx], Data, SXS_ZSP_TTCN_RMT_BUF_SIZE-wIdx);
        memcpy(&wd_Rmt [0].ttcnBuf[0],
               Data+(SXS_ZSP_TTCN_RMT_BUF_SIZE-wIdx),
               len-(SXS_ZSP_TTCN_RMT_BUF_SIZE-wIdx));
    }

    // Update the write index
    wd_Rmt [0].ttcnWIdx = (wIdx + len) & SXS_ZSP_TTCN_RMT_BUF_SIZE_MOD;
}

#endif



