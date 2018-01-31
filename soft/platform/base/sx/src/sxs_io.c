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





#if defined (__TARGET__)
#define __SXS_REMOTE_TRACE__
#endif

#include "sxs_type.h"
#define __SXS_IO_VAR__
#include "sxs_io.h"
#undef __SXS_IO_VAR__
#include "sxs_rmt.h"
#include "sxs_rmc.h"
#include "sxs_lib.h"
#include "sxs_io.hp"
#include "sxs_flsh.h"
#include "sxs_srl.h"
#include "sxr_cpu.h"
#include "sxr_jbsd.h"
#include "sxr_tksd.h"
#include "sxr_mem.h"
#include "sxr_csp.h"
#include "sxs_srl.hp"
#ifndef __TARGET__
#include "sxs_udp.h"
#endif

#include <stdarg.h>

#define __SXS_REMOTE_TRACE__

#ifndef __SXS_REMOTE_TRACE__
#include <stdio.h>
#endif

#ifdef CHIP_HAS_AP
#include "hal_ap_comm.h"
#endif

#ifdef LTE_NBIOT_SUPPORT
#include "tra.h"
#endif

#ifdef CT_NO_DEBUG
#define SXS_RAISE_FLUSH_TIMEOUT     (1000 MS_WAITING)
#else
#define SXS_RAISE_FLUSH_TIMEOUT     (2500 MS_WAITING)
#endif


// =============================================================================
// g_sxRaiseTime
// -----------------------------------------------------------------------------
/// Holding the system time when the last raise occurs.
// =============================================================================
PUBLIC UINT32 g_sxRaiseTime = 0;

UtsCmdWcdmaSignalingEvent_t g_sSignalMsg;

U8      g_sxTraceEnabled = TRUE;
U8      bSpySendBufIdx = 0;
U8      abSpySendBufList[SPY_SEND_BUF_NUM][SPY_MAX_ESCAPE_DATA_SIZE];
U32     lSpySendBufFullCnt = 0;


/*
==============================================================================
 Function   : sxs_SetTraceEnabled
 -----------------------------------------------------------------------------
 Scope      : Enable or disable trace and dump.
 Parameters : enabled.
 Return     : None.
==============================================================================
*/
void sxs_SetTraceEnabled(u8 enabled)
{
    g_sxTraceEnabled = enabled;
}

/*
==============================================================================
   Function   : sxs_IoInit
 -----------------------------------------------------------------------------
   Scope      : I/O initialization.
   Parameters : None.
   Return     : None.
==============================================================================
*/
void sxs_IoInit (void)
{
#ifdef __TARGET__
    sxs_GetRmtBuffer ();
#endif

    sxs_RmtInit (TRUE);

#ifndef __SXS_NO_FLASH_HANDLER__
    sxs_FlashHandlerInit ();
    sxs_FileInit ();
#endif
}

#ifdef __SXS_REMOTE_TRACE__
/*
==============================================================================
   Function   : sxs_TraceOutNS
 -----------------------------------------------------------------------------
   Scope      : Format and send a trace to the remote computer, optimized
                version without string handling.
   Parameters : Id (User Id + trace level + ...)
                Pointer onto formatted string.
                Arguments list.
   Return     : None.
==============================================================================
*/
static void sxs_TraceOutNS(u32 Id, u32 fmt, va_list args)
{
#define ASSIGN(p16, v32)    do { u32 _v = (v32); *p16++ = _v & 0xffff; *p16++ = _v >> 16; } while(0)

    u8  nargs = (Id & TARG) ? TGET_NB_ARG(Id) : SXS_DEFAULT_NB_ARG;
    u32 size = nargs*4 + 6;
    u16 *data = (u16*)pal_TraceRequestBuf (SXS_TRACE_RMC, size);

    if (data != NIL)
    {
        u32 i;

        *data++ = (u16)Id;
        ASSIGN(data, fmt);
        for (i = 0; i < nargs; i++)
        {
            u32 v = va_arg(args, u32);
            ASSIGN(data, v);
        }

        pal_TraceBufFilled (data);
    }

#undef ASSIGN
}

/*
==============================================================================
   Function   : sxs_TraceOut
 -----------------------------------------------------------------------------
   Scope      : Format and send a trace to the remote computer.
   Parameters : Id (User Id + trace level + ...)
                Pointer onto formatted string.
                Arguments list.
   Return     : None.
==============================================================================
*/
static void sxs_TraceOut (u32 Id, const ascii *Fmt, va_list ArgPtr)
{
    u32 Arg [8];
    u16 ArgSize [8];
    u8  *Out;
    u32 i;
    u8  NbArgs   = (Id & TARG) ? TGET_NB_ARG(Id) : SXS_DEFAULT_NB_ARG;
    u16 ArgsSize = 0;
    u8  Tdb = (Id & TDB) != 0;
    u16 FmtSize  = Tdb ? 4 : strlen (Fmt) + 1;


    for (i=0; i<NbArgs; i++)
    {
        Arg [i] = va_arg (ArgPtr, u32);

        if (TGET_SMAP(Id) & (1 << i))
        {
            ArgSize [i] = strlen ((ascii *)Arg [i]) + 1;
            ArgsSize += ArgSize [i];
        }
        else
        {
            ArgsSize += 4;
        }
    }

    if ((Out = pal_TraceRequestBuf (SXS_TRACE_RMC, (u16)(2 + ArgsSize + FmtSize + 4))) != NIL)  //4
    {
        *(u16 *)Out = (u16)Id;

#ifdef __MULTI_RAT__
        if (Tdb)
            memcpy (&Out[2 + 4], &Fmt, FmtSize);
        else
            memcpy (&Out[2 + 4], Fmt, FmtSize);

        Out += FmtSize + 2 + 4;
#else
        if (Tdb)
            memcpy (&Out[2], &Fmt, FmtSize);
        else
            memcpy (&Out[2], Fmt, FmtSize);

        Out += FmtSize + 2;
#endif

        if (TGET_SMAP(Id))
        {
            for (i=0; i<NbArgs; i++)
            {
                if (TGET_SMAP(Id) & (1 << i))
                {
                    memcpy (Out, (ascii *)Arg [i], ArgSize [i]);
                    Out += ArgSize [i];
                }
                else
                {
                    memcpy (Out, (ascii *)&Arg [i], 4);
                    Out += 4;
                }
            }
        }
        else if (ArgsSize)
        {
            memcpy (Out, (u8 *)Arg, ArgsSize);
        }

        pal_TraceBufFilled(Out);
    }
}

/*=============================================================================
 * FUNCTION     : CRC_CalculateCcittCpu
 * DESCRIPTION  : This function calculate a new CRC given the current CRC
 *                and the new data.
 * PARAMETER    :
 *      bpSrcDataBuf : Pointer to source data buffer to be calculated.
 *      dwSrcDataLen : Length, in bytes, of source data.
 *      wSeed :
 * RETURN VALUE :
 *      U16     new CRC.
 *===========================================================================*/
const U16 cSpyCRC16Table[] =
{
    0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
    0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
    0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
    0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
    0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
    0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
    0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
    0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
    0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
    0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
    0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
    0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
    0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
    0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
    0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
    0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
    0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
    0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
    0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
    0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
    0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
    0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
    0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
    0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
    0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
    0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
    0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
    0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
    0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
    0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
    0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
    0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};


u16 CRC_CalculateCcittCpu(const U8 *bpSrcDataBuf, U32 dwSrcDataLen, U16 wSeed)
{
    u32 i;
    u16 wCRC = wSeed;

    if( !bpSrcDataBuf || !dwSrcDataLen) return wCRC;

    for(i=0; i<dwSrcDataLen; i++)
        wCRC = (wCRC >> 8) ^ cSpyCRC16Table[(wCRC ^ bpSrcDataBuf[i]) & 0x00FF];

    return wCRC;
}


u16 SpyLibTxPrepareBuf(U8 *pBufDst, U8 *pBufSrc, U16 wLen, U16 usMaxLen)
{
    u16 wCrc,i;
    u16 wDstLen=0;

    /***************************************************************************
        Calcuate 16bit - Reverse CRC
    ***************************************************************************/

#ifdef __MULTI_RAT__
    pBufDst[wDstLen++] = 0;
    pBufDst[wDstLen++] = 0;
#endif
    wCrc = CRC_CalculateCcittCpu(pBufSrc, wLen, SPY_CRC_16_SEED);
    wCrc ^= SPY_CRC_16_SEED;

    /***************************************************************************
        Character Escaping
    ***************************************************************************/
    for(i=0; i<wLen; i++)
    {
        //SpyLibCharacterEscape(pBufDst, pBufSrc[i], &wDstLen);
        pBufDst[wDstLen++] = pBufSrc[i];
        if(wDstLen + 3 > usMaxLen)      //SPY_MAX_ESCAPE_DATA_SIZE
            return(0);
    }
    //SpyLibCharacterEscape(pBufDst, wCrc & 0xff, &wDstLen);
    //SpyLibCharacterEscape(pBufDst, (wCrc>>8) & 0xff, &wDstLen);
    pBufDst[wDstLen++] = wCrc & 0xff;
    pBufDst[wDstLen++] = (wCrc>>8) & 0xff;

    pBufDst[wDstLen++] = SPY_FRAME_END;
#ifdef __MULTI_RAT__
    pBufDst[wDstLen++] = 0;
#endif
    return wDstLen;

}

/****************************************************************************
    SpySemTxCmdIn
****************************************************************************/
void SpySemTxCmdIn(u16  wLength,u8 *pBuffer)
{
    u8  *Out;
    u16 ArgsSize = 0;
    u8  Tdb = 0;
    u16 FmtSize  = Tdb ? 4 : wLength /*+ 1*/;

    //NUC_WCDMA=0x95
    if ((Out = sxs_SendToRemote (0x95, (u8 *)pBuffer, (u16)(/*2 +*/ ArgsSize + FmtSize))) != NIL)
    {
        // *(u16 *)Out = (u16)Id;

        if (Tdb)
            memcpy (&Out[0], &pBuffer, FmtSize);
        else
            memcpy (&Out[0], pBuffer, FmtSize);
    }
}

void SpySendToHost(U8 *pBuf, U16 wLen)
{
    u8  *pabSendBuf;
    u16 wSendLen;

    if(bSpySendBufIdx < SPY_SEND_BUF_NUM)
    {
        pabSendBuf = &abSpySendBufList[bSpySendBufIdx++][0];
        wSendLen = SpyLibTxPrepareBuf(pabSendBuf, pBuf, wLen, SPY_MAX_ESCAPE_DATA_SIZE);

        if(wSendLen > 0)
        {
            SpySemTxCmdIn(wSendLen, pabSendBuf);
            bSpySendBufIdx--;
            return; // PROCESSING OK
        }

        bSpySendBufIdx--;
        //cSpyTaskDebugReport(TASK_SPY,DEBUG_LEVEL_ERROR, "SpySendToHost ERR(a)"); // ERROR print
    }
    else
    {
        lSpySendBufFullCnt++;
        //cSpyTaskDebugReport(TASK_SPY,DEBUG_LEVEL_ERROR, "SpySendToHost ERR(b)"); // ERROR print
    }
}
/****************************************************************************
    Send Signal information to Host porting from wcpu
    lLogicCh : Logical Channel : RRC (1-56 ,100-117 (NAS))
               1-15     : RRC Message Mask
               15-100   : RRC Etc Message Mask
               100-117  : NAS Message Mask
    lTransCh : Transport Channel
    pData : Protocol Messages buffers
    wLen : pData Length

    CMD : UTSCMD_WCDMA_SIGNAL_EVENT (0xab, 171)
****************************************************************************/
void UtsWcdmaSignalEvent(U32 lLogicCh,U32 lTransCh,U8 bMsgId, U8 *pData,U16 wLen)
{
    U32 lMask = 0;
    //if(g_SpyInfo.lUtsEventMask & E_UTS_WCDMA_SYS_SIGNALING)
    {
        if(lLogicCh >0 && lLogicCh < 12)            // RRC Mask
            lMask = (1 << (lLogicCh - 1));
        else if(lLogicCh >= 12 && lLogicCh < 100)   // RRC etc Mask
        {
            lMask = (1 << 16);
        }
        else if(lLogicCh >= 100 && lLogicCh < 118)  // NAS Message Mask
        {
            lMask = (1 << (lLogicCh - 100));
        }
        else
            lMask = 0;

        //if(g_SpyInfo.lUtsSignalMask & lMask)
        {
            g_sSignalMsg.bCmd = 0xab;//UTSCMD_WCDMA_SIGNAL_EVENT=0xab
            //SpyGetSystemTime((U8 *)g_sSignalMsg.abTime);
            g_sSignalMsg.lLogicalChannel    = lLogicCh;
            g_sSignalMsg.lTransportChannel  = lTransCh;
            g_sSignalMsg.bMsgId             = bMsgId;
            wLen = MIN(wLen,UTS_WCDMA_MAX_SINGNAL_BYTES);
            g_sSignalMsg.wLength = wLen;
            //memset((U8*)g_sSignalMsg.abMessage,0,wLen);
            memcpy((U8*)g_sSignalMsg.abMessage,pData,wLen);
#ifdef CSPY_ADD_ABSOLUTE_TIME
            g_sSignalMsg.wAbsoluteTime = _localSysTime();
            memcpy(&(g_sSignalMsg.abMessage[wLen]), (U8 *)&g_sSignalMsg.wAbsoluteTime,4);
            SpySendToHost((U8*)&g_sSignalMsg,wLen +UTS_TIME_STAMP_LENGTH + 16);
#else
            SpySendToHost((U8*)&g_sSignalMsg,wLen +UTS_TIME_STAMP_LENGTH + 12);
#endif

        }
    }
}


#endif
/*
==============================================================================
   Function   : sxs_vprintf
 -----------------------------------------------------------------------------
   Scope      : Send a formatted trace string to the remote computer.
   Parameters : Id (User Id + trace level + ...)
                Pointer onto formatted string.
                Arguments list.
   Return     : None.
==============================================================================
*/
#ifndef LTE_NBIOT_SUPPORT
void sxs_vprintf (u32 Id, const ascii *Fmt, va_list argPtr)
{
    if (!g_sxTraceEnabled)
        return;

    if ((sxs_IoCtx.TraceBitMap [TGET_ID(Id)] & (1L << TGET_LEVEL(Id)))
            ||  (Id & TSTDOUT))
    {
#ifdef __SXS_REMOTE_TRACE__
        if ((Id & TDB) && TGET_SMAP(Id) == 0)
        {
            sxs_TraceOutNS (Id, Fmt, argPtr);
        }
        else
        {
            sxs_TraceOut (Id, Fmt, argPtr);
        }
#ifndef __EMBEDDED__
        sxs_RmtBufferFlush ();
#endif
#else
        if (!(Id & TIDU))
            printf ("%s : %2i ", sxs_TraceDesc [TGET_ID(Id)], TGET_LEVEL(Id));
        vfprintf (stdout, Fmt, argPtr);
#endif
    }
}
#else
void sxs_vprintf (u32 Id, const ascii *Fmt, va_list argPtr)
{
    char uart_buf[256];
    u8  TID;
    u32 MID;
    TID = TGET_ID(Id);

    switch( TID )
    {
        case _SXR:
            memcpy(uart_buf,  "SXR: ", 5);
            vsnprintf(uart_buf + 5, sizeof(uart_buf), Fmt, argPtr);
            MID = TRA_SXR;
            if(Id & TSTDOUT)
            {
                MID = 0xFFFFFFFF;
            }
            TRACE_PUB_STR(TRA_SIM1, MID, TRA_ALL, uart_buf);
            break;
        case _PAL:
            memcpy(uart_buf,  "PAL: ", 5);
            vsnprintf(uart_buf + 5, sizeof(uart_buf), Fmt, argPtr);
            MID = TRA_PAL;
            if(Id & TSTDOUT)
            {
                MID = 0xFFFFFFFF;
            }
            TRACE_PUB_STR(TRA_SIM1, MID, TRA_ALL, uart_buf);
            break;
        case _L1A:
            memcpy(uart_buf,  "L1A: ", 5);
            vsnprintf(uart_buf + 5, sizeof(uart_buf), Fmt, argPtr);
            MID = TRA_L1A;
            if(Id & TSTDOUT)
            {
                MID = 0xFFFFFFFF;
            }
            TRACE_GGE_STR(TRA_SIM1, MID, TRA_ALL, uart_buf);
            break;
        case _L1S:
            memcpy(uart_buf,  "L1S: ", 5);
            vsnprintf(uart_buf + 5, sizeof(uart_buf), Fmt, argPtr);
            MID = TRA_L1S;
            if(Id & TSTDOUT)
            {
                MID = 0xFFFFFFFF;
            }
            TRACE_GGE_STR(TRA_SIM1, MID, TRA_ALL, uart_buf);
            break;
        case _LAP:
            memcpy(uart_buf,  "LAP: ", 5);
            vsnprintf(uart_buf + 5, sizeof(uart_buf), Fmt, argPtr);
            MID = TRA_LAP;
            if(Id & TSTDOUT)
            {
                MID = 0xFFFFFFFF;
            }
            TRACE_GGE_STR(TRA_SIM1, MID, TRA_ALL, uart_buf);
            break;
        case _RLU:
            memcpy(uart_buf,  "RLU: ", 5);
            vsnprintf(uart_buf + 5, sizeof(uart_buf), Fmt, argPtr);
            MID = TRA_RLU;
            if(Id & TSTDOUT)
            {
                MID = 0xFFFFFFFF;
            }
            TRACE_GGE_STR(TRA_SIM1, MID, TRA_ALL, uart_buf);
            break;
        case _RLD:
            memcpy(uart_buf,  "RLD: ", 5);
            vsnprintf(uart_buf + 5, sizeof(uart_buf), Fmt, argPtr);
            MID = TRA_RLD;
            if(Id & TSTDOUT)
            {
                MID = 0xFFFFFFFF;
            }
            TRACE_GGE_STR(TRA_SIM1, MID, TRA_ALL, uart_buf);
            break;
        case _LLC:
            memcpy(uart_buf,  "LLC: ", 5);
            vsnprintf(uart_buf + 5, sizeof(uart_buf), Fmt, argPtr);
            MID = TRA_LLC;
            if(Id & TSTDOUT)
            {
                MID = 0xFFFFFFFF;
            }
            TRACE_GGE_STR(TRA_SIM1, MID, TRA_ALL, uart_buf);
            break;
        case _MM:
            memcpy(uart_buf,  "CC: ", 4);
            vsnprintf(uart_buf + 4, sizeof(uart_buf), Fmt, argPtr);
            MID = TRA_MM;
            if(Id & TSTDOUT)
            {
                MID = 0xFFFFFFFF;
            }
            TRACE_GGE_STR(TRA_SIM1, MID, TRA_ALL, uart_buf);
            break;
        case _CC:
            memcpy(uart_buf,  "MM: ", 4);
            vsnprintf(uart_buf + 4, sizeof(uart_buf), Fmt, argPtr);
            MID = TRA_CC;
            if(Id & TSTDOUT)
            {
                MID = 0xFFFFFFFF;
            }
            TRACE_GGE_STR(TRA_SIM1, MID, TRA_ALL, uart_buf);
            break;
        case _SS:
            memcpy(uart_buf,  "SS: ", 4);
            vsnprintf(uart_buf + 4, sizeof(uart_buf), Fmt, argPtr);
            MID =  TRA_SS;
            if(Id & TSTDOUT)
            {
                MID = 0xFFFFFFFF;
            }
            TRACE_GGE_STR(TRA_SIM1, MID, TRA_ALL, uart_buf);
            break;
        case _SMS:
            memcpy(uart_buf,  "SMS: ", 5);
            vsnprintf(uart_buf + 5, sizeof(uart_buf), Fmt, argPtr);
            MID = TRA_SMS;
            if(Id & TSTDOUT)
            {
                MID = 0xFFFFFFFF;
            }
            TRACE_GGE_STR(TRA_SIM1, MID, TRA_ALL, uart_buf);
            break;
        case _SM:
            memcpy(uart_buf,  "SM: ", 4);
            vsnprintf(uart_buf + 4, sizeof(uart_buf), Fmt, argPtr);
            MID = TRA_SM;
            if(Id & TSTDOUT)
            {
                MID = 0xFFFFFFFF;
            }
            TRACE_GGE_STR(TRA_SIM1, MID, TRA_ALL, uart_buf);
            break;
        case _SND:
            memcpy(uart_buf,  "SND: ", 5);
            vsnprintf(uart_buf + 5, sizeof(uart_buf), Fmt, argPtr);
            MID = TRA_SND;
            if(Id & TSTDOUT)
            {
                MID = 0xFFFFFFFF;
            }
            TRACE_GGE_STR(TRA_SIM1, MID, TRA_ALL, uart_buf);
            break;
        case _API:
            memcpy(uart_buf,  "API: ", 5);
            vsnprintf(&uart_buf[5], sizeof(uart_buf), Fmt, argPtr);
            MID = TRA_API;
            if(Id & TSTDOUT)
            {
                MID = 0xFFFFFFFF;
            }
            TRACE_GGE_STR(TRA_SIM1, MID, TRA_ALL, uart_buf);
            break;
        case _MMI:
            memcpy(uart_buf,  "MMI: ", 5);
            vsnprintf(uart_buf + 5, sizeof(uart_buf), Fmt, argPtr);
            MID = TRA_MMI;
            if(Id & TSTDOUT)
            {
                MID = 0xFFFFFFFF;
            }
            TRACE_PUB_STR(TRA_SIM1, MID, TRA_ALL, uart_buf);
            break;
        case _SIM:
            memcpy(uart_buf,  "SIM: ", 5);
            vsnprintf(uart_buf + 5, sizeof(uart_buf), Fmt, argPtr);
            MID = TRA_SIM;
            if(Id & TSTDOUT)
            {
                MID = 0xFFFFFFFF;
            }
            TRACE_GGE_STR(TRA_SIM1, MID, TRA_ALL, uart_buf);
            break;
        case _AT:
            memcpy(uart_buf,  "AT: ", 4);
            vsnprintf(uart_buf + 4, sizeof(uart_buf), Fmt, argPtr);
            MID = TRA_AT;
            if(Id & TSTDOUT)
            {
                MID = 0xFFFFFFFF;
            }
            TRACE_PUB_STR(TRA_SIM1, MID, TRA_ALL, uart_buf);
            break;
        case _M2A:
            memcpy(uart_buf,  "M2A: ", 5);
            vsnprintf(uart_buf + 5, sizeof(uart_buf), Fmt, argPtr);
            MID = TRA_M2A;
            if(Id & TSTDOUT)
            {
                MID = 0xFFFFFFFF;
            }
            TRACE_PUB_STR(TRA_SIM1, MID, TRA_ALL, uart_buf);
            break;
        case _STT:
            memcpy(uart_buf,  "STT: ", 5);
            vsnprintf(uart_buf + 5, sizeof(uart_buf), Fmt, argPtr);
            MID = TRA_STT;
            if(Id & TSTDOUT)
            {
                MID = 0xFFFFFFFF;
            }
            TRACE_PUB_STR(TRA_SIM1, MID, TRA_ALL, uart_buf);
            break;
        case _RRI:
            memcpy(uart_buf,  "RRI: ", 5);
            vsnprintf(uart_buf + 5, sizeof(uart_buf), Fmt, argPtr);
            MID = TRA_RRI;
            if(Id & TSTDOUT)
            {
                MID = 0xFFFFFFFF;
            }
            TRACE_GGE_STR(TRA_SIM1, MID, TRA_ALL, uart_buf);
            break;
        case _RRD:
            memcpy(uart_buf,  "RRD: ", 5);
            vsnprintf(uart_buf + 5, sizeof(uart_buf), Fmt, argPtr);
            MID = TRA_RRD;
            if(Id & TSTDOUT)
            {
                MID = 0xFFFFFFFF;
            }
            TRACE_GGE_STR(TRA_SIM1, MID, TRA_ALL, uart_buf);
            break;
        case _RLP:
            memcpy(uart_buf,  "RLP: ", 5);
            vsnprintf(uart_buf + 5, sizeof(uart_buf), Fmt, argPtr);
            MID = TRA_RLP;
            if(Id & TSTDOUT)
            {
                MID = 0xFFFFFFFF;
            }
            TRACE_GGE_STR(TRA_SIM1, MID, TRA_ALL, uart_buf);
            break;
        case _HAL:
            memcpy(uart_buf,  "HAL: ", 5);
            vsnprintf(uart_buf + 5, sizeof(uart_buf), Fmt, argPtr);
            MID = TRA_HAL;
            if(Id & TSTDOUT)
            {
                MID = 0xFFFFFFFF;
            }
            TRACE_PUB_STR(TRA_SIM1, MID, TRA_ALL, uart_buf);
            break;
        case _BCPU:
            memcpy(uart_buf,  "BCPU: ", 6);
            vsnprintf(uart_buf + 6, sizeof(uart_buf), Fmt, argPtr);
            MID = TRA_BCPU;
            if(Id & TSTDOUT)
            {
                MID = 0xFFFFFFFF;
            }
            TRACE_PUB_STR(TRA_SIM1, MID, TRA_ALL, uart_buf);
            break;
        case _CSW:
            memcpy(uart_buf,  "CSW: ", 5);
            vsnprintf(uart_buf + 5, sizeof(uart_buf), Fmt, argPtr);
            MID = TRA_CSW;
            if(Id & TSTDOUT)
            {
                MID = 0xFFFFFFFF;
            }
            TRACE_PUB_STR(TRA_SIM1, MID, TRA_ALL, uart_buf);
            break;
        case _EDRV:
            memcpy(uart_buf,  "EDRV: ", 6);
            vsnprintf(uart_buf + 6, sizeof(uart_buf), Fmt, argPtr);
            MID = TRA_EDRV;
            if(Id & TSTDOUT)
            {
                MID = 0xFFFFFFFF;
            }
            TRACE_PUB_STR(TRA_SIM1, MID, TRA_ALL, uart_buf);
            break;
        case _MCI:
            memcpy(uart_buf,  "MCI: ", 5);
            vsnprintf(uart_buf + 5, sizeof(uart_buf), Fmt, argPtr);
            MID = TRA_MCI;
            if(Id & TSTDOUT)
            {
                MID = 0xFFFFFFFF;
            }
            TRACE_PUB_STR(TRA_SIM1, MID, TRA_ALL, uart_buf);
            break;
        case _SVC1:
            memcpy(uart_buf,  "SVC1: ", 6);
            vsnprintf(uart_buf + 6, sizeof(uart_buf), Fmt, argPtr);
            MID = TRA_SVC1;
            if(Id & TSTDOUT)
            {
                MID = 0xFFFFFFFF;
            }
            TRACE_PUB_STR(TRA_SIM1, MID, TRA_ALL, uart_buf);
            break;
        case _SVC2:
            memcpy(uart_buf,  "SVC2: ", 6);
            vsnprintf(uart_buf + 6, sizeof(uart_buf), Fmt, argPtr);
            MID = TRA_SVC2;
            if(Id & TSTDOUT)
            {
                MID = 0xFFFFFFFF;
            }
            TRACE_PUB_STR(TRA_SIM1, MID, TRA_ALL, uart_buf);
            break;
        case _WCPU:
            memcpy(uart_buf,  "WCPU: ", 6);
            vsnprintf(uart_buf + 6, sizeof(uart_buf), Fmt, argPtr);
            MID = TRA_WCPU;
            if(Id & TSTDOUT)
            {
                MID = 0xFFFFFFFF;
            }
            TRACE_PUB_STR(TRA_SIM1, MID, TRA_ALL, uart_buf);
            break;
defualt:
            vsnprintf(uart_buf, sizeof(uart_buf), Fmt, argPtr);
            MID = 0xFFFFFFFF;
            if(Id & TSTDOUT)
            {
                MID = 0xFFFFFFFF;
            }
            TRACE_PUB_STR(TRA_SIM1, MID, TRA_ALL, uart_buf);
            break;
    }
}
#endif /* _SWITCH_TRACE_TO_DEFAULT_ */

/*
==============================================================================
   Function   : sxs_fprintf
 -----------------------------------------------------------------------------
   Scope      : Send a formatted trace string to the remote computer.
   Parameters : Id (User Id + trace level + ...)
                Pointer onto formatted string.
                Arguments list.
   Return     : None.
==============================================================================
*/
void sxs_fprintf (u32 Id, const ascii *Fmt, ...)
{
    va_list ArgPtr;

    va_start (ArgPtr, Fmt);
    sxs_vprintf(Id, Fmt, ArgPtr);
    va_end( ArgPtr );
}

/*
==============================================================================
   Function   : sxs_Raise
 -----------------------------------------------------------------------------
   Scope      : Raise an exception. Send formatted trace string to remote
                computer. In case of TABORT, exit of the program.
   Parameters : Id (User Id + trace level + Abort + ...)
                Arguments list.
                Pointer onto formatted string.
   Return     : None.
==============================================================================
*/
void sxs_Raise (u32 Id, const ascii *Fmt, ...)
{
    static u8 FunctionLocked = FALSE;
    u32 Ra;
    u32 Sp;
    u8 FlowCtrl;
    va_list ArgPtr;

    SXS_GET_RA(Ra);
    SXS_GET_SP(Sp);

    if (FunctionLocked && (!(Id & TABORT)))
        return;

    FunctionLocked = TRUE;

    va_start (ArgPtr, Fmt);

    g_sxRaiseTime = hal_TimGetUpTime();

    if (!(Id & TABORT))
    {
        Id |= TRAISE;  // de Lisieux
    }
    else
    {
        // Prohibit task scheduling
        sxr_EnterScSchedule ();
        // Prohibit interrupt handling
        sxr_EnterSc ();
        // Save exception info if it is not saved before
#ifdef CHIP_HAS_AP
        hal_ApCommSaveExcInfo((UINT32)Fmt, 0);
#endif
        // Save current BCPU context before any time-consume job
        // (e.g., flushing trace)
        hal_DbgTriggerBcpuDebugIrq();

        // Allow trace output
        pal_TraceSetTxEnabled(TRUE);

        // Print PAL abort information
        pal_SoftAbort ();
    }

#ifdef __SXS_REMOTE_TRACE__
    sxs_TraceOut (Id, Fmt, ArgPtr);
#ifndef __EMBEDDED__
    sxs_RmtBufferFlush ();
#endif
#else
    printf ("Exception %s ", sxs_TraceDesc [TGET_ID(Id)]);
    vfprintf (stdout, Fmt, ArgPtr);
#endif

    if (Id & TABORT)
    {
#if defined (__TARGET__) && (!defined(__SIMU__))
        sxs_Boot -> ExceptionCount++;
        sxs_Boot -> ExceptionExpected = TRUE;
        sxs_BackTrace (Ra, &Sp);

        if (sxs_Boot -> RmtBufferSaved == 0)
            sxs_SaveRmtBuffer ();
#endif
    }

    while (Id & TABORT)
    {
        pal_SoftBreak ();

#ifndef __EMBEDDED__
        sxs_RmtBufferFlush ();
#endif
    }
    FunctionLocked = FALSE;

    va_end( ArgPtr );
}

/*
==============================================================================
   Function   : sxs_Dump
 -----------------------------------------------------------------------------
   Scope      : Dump some data with required format.
   Parameters : Pointer onto data to be dump.
                Size expressed in bytes of the data.
                Format string. Max 3 characters.
                %2x     dump bytes in hexa.
                %4x     dump word in hexa.
                %8x     dump double word in hexa.
                i for signed decimal, d for unsigned decimal...
   Return     : None.
==============================================================================
*/
void sxs_Dump (u32 Id, const ascii *Fmt, const void *Data, u16 Size)
{
    if (!g_sxTraceEnabled)
        return;

    if ((sxs_IoCtx.TraceBitMap [TGET_ID(Id)] & (1L << TGET_LEVEL(Id)))
            ||  (Id & TSTDOUT))
    {
        sxs_FlwDmp_t *FlwDmp = (sxs_FlwDmp_t *)pal_TraceRequestBuf (SXS_DUMP_RMC, (u16)(Size + sizeof (sxs_FlwDmp_t)));
        if (FlwDmp == NIL)
            return;

        FlwDmp -> Id   = (u16)Id;
        FlwDmp -> Size = Size;
        FlwDmp -> Address = Data;

        memcpy (FlwDmp -> Data, Data, Size);

        if (Fmt)
        {
            FlwDmp -> Fmt [0] = Fmt [0];
            FlwDmp -> Fmt [1] = Fmt [1];
            FlwDmp -> Fmt [2] = Fmt [2];
            FlwDmp -> Fmt [3] = 0;
        }
        else
        {
            FlwDmp -> Fmt [0] = '%';
            FlwDmp -> Fmt [1] = '2';
            FlwDmp -> Fmt [2] = 'x';
            FlwDmp -> Fmt [3] = 0;
        }

        pal_TraceBufFilled (FlwDmp);

#ifndef __EMBEDDED__
        sxs_RmtBufferFlush ();
#endif
    }
}

#ifndef __SXS_NO_FLASH_HANDLER__
/*
==============================================================================
   Function   : sxs_FileInit
 -----------------------------------------------------------------------------
   Scope      : File manager context initialization.
   Parameters : none
   Return     : none
==============================================================================
*/
void sxs_FileInit (void)
{
#ifndef __SXS_STD_FILE_LIB__
    u32 FilesStart [SXS_FLH_NB_FILES_MAX];
    u32 FilesEnd [SXS_FLH_NB_FILES_MAX];
    u32 i;

    memset ((u8 *)&sxs_FileCtx, 0, sizeof (sxs_FileCtx));

    sxs_FlashBlockRead (0, (u8 *)sxs_FileCtx.Desc, sizeof (sxs_FileCtx.Desc));

    sxs_GetFilesLimit (FilesStart, FilesEnd);

    for (i=1; i<SXS_FLH_NB_FILES_MAX; i++)
    {
        if (sxs_FileCtx.Desc [i].Name [0] != 0)
        {
            sxs_FileCtx.NbFiles++;
            sxs_FileCtx.Desc [i].Size = (u16)(FilesEnd [i] - FilesStart [i]);
        }
        else
            sxs_FileCtx.Handle [sxs_FileCtx.FreeHandle++] = (u8)i;
    }
    sxs_FileCtx.Handle [sxs_FileCtx.FreeHandle] = 0xFF;
    sxs_FileCtx.FreeHandle = 0;
#endif
}

#ifndef __SXS_STD_FILE_LIB__
/*
==============================================================================
   Function   : sxs_GetFileHandle
 -----------------------------------------------------------------------------
   Scope      : Retreive a free handle.
   Parameters : none
   Return     : Handle number or -1 when no handle is left free.
==============================================================================
*/
s16 sxs_GetFileHandle (void)
{
    sxs_FileCtx.NbFiles++;

    if (sxs_FileCtx.Handle [sxs_FileCtx.FreeHandle] != 0xFF)
        return (s16)sxs_FileCtx.Handle [sxs_FileCtx.FreeHandle++];
    else
        SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(1),TSTR("Too much files %i\n",0x06ba0001), sxs_FileCtx.NbFiles));

    return -1;
}

/*
==============================================================================
   Function   : sxs_SearchFile
 -----------------------------------------------------------------------------
   Scope      : Check if the file identified by its name already exists and
                if so, retreive its Handle.
   Parameters : Pointer on file name
   Return     : File Handle or -1 when file has not been found.
==============================================================================
*/
s16 sxs_SearchFile (const ascii *FileName)
{
    s16 i;

    for (i=1; i<SXS_FLH_NB_FILES_MAX; i++)
    {
        if (!strcmp (sxs_FileCtx.Desc [i].Name, FileName))
            return i;
    }
    return -1;
}


/*
==============================================================================
   Function   : sxs_FreeFileHandle
 -----------------------------------------------------------------------------
   Scope      : Free an allocated Handle.
   Parameters : Handle number.
   Return     : none
==============================================================================
*/
void sxs_FreeFileHandle (s16 Handle)
{
    sxs_FileCtx.NbFiles--;
    sxs_FileCtx.Handle [--sxs_FileCtx.FreeHandle] = (u8)Handle;
}

/*
==============================================================================
   Function   : sxs_Open
 -----------------------------------------------------------------------------
   Scope      : opens the file specified by filename and prepares the file
                for reading or writing, as specified by oflag.
                Files are always assumed binary. Text files are not managed.
   Parameters : - pointer on file name,
                - OFlag:
   SXS_O_APPEND      Moves file pointer to end of file before every write operation.
   SXS_O_BINARY      Opens file in binary mode. No Effect as file are assumed binary.
   SXS_O_CREAT       Creates and opens new file for writing. If file specified
                     by filename exists, existing file is opened for writing.
   SXS_O_RDONLY      Opens file for reading only.
   SXS_O_WRONLY      Opens file for writing only.
   SXS_O_RDWR        Opens file for both reading and writing.
   SXS_O_TRUNC       Opens file and truncates it to zero length. Used with,
                     _O_CREAT opens an existing file or creates a new file.
   SXS_O_TEMPORARY   Create file as temporary. File is created in stacked bank
                     and is deleted when stacked bank is full.
   SXS_O_SHORT_LIVED Create file as temporary. File is created in dynamic bank
                     and won't be moved in static bank in case of static bank
                     update.
                - PMode exists for compatibility purpose only. Access right
                are not managed.
   Return     : Handle number in case of success or -1.
==============================================================================
*/
s16 sxs_Open (const ascii *FileName, s16 OFlag, s16 PMode)
{
    s16 Handle = -1;

    if ((Handle = sxs_SearchFile (FileName)) != -1)
    {
        if (OFlag & SXS_O_TRUNC)
        {
            sxs_FlashBlockDelete (SXS_SET_FILE_IDX(Handle), SXS_FILE_IDX_MSK, TRUE);
            sxs_FileCtx.Desc [Handle].Size = 0;
        }

        if (OFlag & SXS_O_APPEND)
            sxs_FileCtx.Desc [Handle].Offset = sxs_FileCtx.Desc [Handle].Size; // -1;
        else
            sxs_FileCtx.Desc [Handle].Offset = 0;

        sxs_FileCtx.Desc [Handle].OFlag |= OFlag;
    }
    else if (OFlag & SXS_O_CREAT)
    {
        Handle = sxs_GetFileHandle ();

        sxs_FileCtx.Desc [Handle].Size = 0;
        sxs_FileCtx.Desc [Handle].Offset = 0;
        memcpy ((u8 *)&sxs_FileCtx.Desc [Handle], FileName, SXS_FILE_NAME_MAX_LEN);
        sxs_FileCtx.Desc [Handle].OFlag = (OFlag & ~SXS_O_RDWR);

        sxs_FlashBlockWrite ((Handle * sizeof (sxs_FileDesc_t)) | SXS_FLH_PROTECTED_BLOCK,
                             (u8 *)&sxs_FileCtx.Desc [Handle],
                             sizeof (sxs_FileDesc_t));

        sxs_FileCtx.Desc [Handle].OFlag = OFlag | SXS_O_RDWR;
    }

    return Handle;
}


/*
==============================================================================
   Function   : sxs_Close
 -----------------------------------------------------------------------------
   Scope      : Close identified file.
   Parameters : Handle referring to open file
   Return     : 0 in case of success, -1 otherwise.
==============================================================================
*/
s16 sxs_Close (s16 Handle)
{
    if (sxs_FileCtx.Desc [Handle].OFlag & SXS_O_RDWR)
    {
        sxs_FileCtx.Desc [Handle].OFlag &= ~SXS_O_RDWR;
        return 0;
    }
    else
        return -1;
}

/*
==============================================================================
   Function   : sxs_Read
 -----------------------------------------------------------------------------
   Scope      : Read a bunch of data in the identified file.
   Parameters : Handle referring to open file
                Buffer Storage location for data
                Count Maximum number of bytes
   Return     : Number of read bytes in case of success, -1 otherwise.
==============================================================================
*/
s16 sxs_Read (s16 Handle, void *Buffer, u16 Count)
{
    u16 Size;
    u32 Id;

    if (!(sxs_FileCtx.Desc [Handle].OFlag & SXS_O_RDWR))
    {
        SXS_RAISE ((_SXR|TDB|TNB_ARG(1),TSTR("File not opened %i\n",0x06ba0002), Handle));
        return -1;
    }

    Size = MIN (Count, sxs_FileCtx.Desc [Handle].Size - sxs_FileCtx.Desc [Handle].Offset);

    Id = (sxs_FileCtx.Desc [Handle].OFlag & SXS_O_TEMPORARY ? SXS_FLH_STACKED_BLOCK : 0)
         | (sxs_FileCtx.Desc [Handle].OFlag & SXS_O_SHORT_LIVED ? SXS_FLH_PROTECTED_BLOCK : 0);
    Id |= sxs_FileCtx.Desc [Handle].Offset | SXS_SET_FILE_IDX (Handle);

    sxs_FileCtx.Desc [Handle].Offset += Size;

    sxs_FlashBlockRead (Id, (u8 *)Buffer, Size);

    return Size;
}

/*
==============================================================================
   Function   : sxs_Write
 -----------------------------------------------------------------------------
   Scope      : Write a bunch of data in the identified file.
   Parameters : Handle referring to open file
                Buffer Storage location for data
                Count Maximum number of bytes
   Return     : Number of written bytes in case of success, -1 otherwise.
==============================================================================
*/
s16 sxs_Write (s16 Handle, const void *Buffer, u16 Count)
{
    u32 Id;

    if (!(sxs_FileCtx.Desc [Handle].OFlag & SXS_O_WRONLY))
    {
        SXS_RAISE ((_SXR|TDB|TNB_ARG(1),TSTR("File not opened for write %i\n",0x06ba0003), Handle));
        return -1;
    }

    Id = (sxs_FileCtx.Desc [Handle].OFlag & SXS_O_TEMPORARY ? SXS_FLH_STACKED_BLOCK : 0)
         | (sxs_FileCtx.Desc [Handle].OFlag & SXS_O_SHORT_LIVED ? 0 : SXS_FLH_PROTECTED_BLOCK);
    Id |= sxs_FileCtx.Desc [Handle].Offset | SXS_SET_FILE_IDX (Handle);

    sxs_FileCtx.Desc [Handle].Offset += Count;

    if (sxs_FileCtx.Desc [Handle].Size < sxs_FileCtx.Desc [Handle].Offset)
        sxs_FileCtx.Desc [Handle].Size = sxs_FileCtx.Desc [Handle].Offset;

    sxs_FlashBlockWrite (Id, (u8 *)Buffer, Count);

    return 0;
}

/*
==============================================================================
   Function   : sxs_LSeek
 -----------------------------------------------------------------------------
   Scope      : Move the file pointer associated with handle to a new
                location that is offset bytes from origin. The next operation
                on the file occurs at the new location.
   Parameters : Handle referring to open file
                Bytes offset
                Origin:
                SXS_SEEK_SET Beginning of file
                SXS_SEEK_CUR Current position of file pointer
                SXS_SEEK_END End of file
   Return     : none
==============================================================================
*/
s32 sxs_LSeek (s16 Handle, u32 Offset, u16 Origin)
{
    if (!(sxs_FileCtx.Desc [Handle].OFlag & SXS_O_RDWR))
    {
        SXS_RAISE ((_SXR|TDB|TNB_ARG(1),TSTR("File not opened %i\n",0x06ba0004), Handle));
        return -1;
    }

    if (Origin == SXS_SEEK_SET)
        sxs_FileCtx.Desc [Handle].Offset = (u16)MIN (Offset, (u16)(sxs_FileCtx.Desc [Handle].Size)); // sxs_FileCtx.Desc [Handle].Offset);
    else if (Origin == SXS_SEEK_CUR)
    {
        sxs_FileCtx.Desc [Handle].Offset += (u16)Offset;

        if (sxs_FileCtx.Desc [Handle].Offset > sxs_FileCtx.Desc [Handle].Size)
            sxs_FileCtx.Desc [Handle].Offset = sxs_FileCtx.Desc [Handle].Size;
    }
    else if (Origin == SXS_SEEK_END)
//  sxs_FileCtx.Desc [Handle].Offset = sxs_FileCtx.Desc [Handle].Size ? sxs_FileCtx.Desc [Handle].Size - 1 : 0;
        sxs_FileCtx.Desc [Handle].Offset = sxs_FileCtx.Desc [Handle].Size;

    return sxs_FileCtx.Desc [Handle].Offset;
}
#endif

/*
==============================================================================
   Function   : sxs_DelFile
 -----------------------------------------------------------------------------
   Scope      : Delete an existing file.
   Parameters : Handle referring to open file
   Return     : none
==============================================================================
*/
void sxs_DelFile (s16 Handle)
{
#ifndef __SXS_STD_FILE_LIB__
    if (sxs_FileCtx.Desc [Handle].Name [0] != 0)
    {
        sxs_FlashBlockDelete ( SXS_SET_FILE_IDX(Handle)
                               |(SXS_O_TEMPORARY ? SXS_FLH_STACKED_BLOCK : 0)
                               |(SXS_O_SHORT_LIVED ? 0 : SXS_FLH_STATIC_BLOCK),
                               SXS_FILE_IDX_MSK, TRUE);

        sxs_FlashBlockDelete ((Handle * sizeof (sxs_FileDesc_t)),
                              SXS_OFFSET_MSK | SXS_FILE_IDX_MSK, TRUE);

        memset ((u8 *)&sxs_FileCtx.Desc [Handle], 0, sizeof (sxs_FileDesc_t));

        sxs_FreeFileHandle (Handle);

    }
    else
        SXS_RAISE ((_SXR|TDB|TNB_ARG(1),TSTR("File not found for deletion %i\n",0x06ba0005), Handle));
#endif
}


/*
==============================================================================
   Function   : sxs_CheckFiles
 -----------------------------------------------------------------------------
   Scope      : Print a status for all existing files.
   Parameters : none
   Return     : none
==============================================================================
*/
void sxs_CheckFiles (void)
{
#ifndef __SXS_STD_FILE_LIB__
    u32 i;
    u32 TotalSize = 0;

    for (i=1; i<SXS_FLH_NB_FILES_MAX; i++)
    {
        if (sxs_FileCtx.Desc [i].Name [0] != 0)
        {
            SXS_TRACE (_SXR|TSTDOUT|TSMAP(1)|TDB|TNB_ARG(4),TSTR("File %s Size %i Offset %i state 0x%lx\n",0x06ba0006),
                       sxs_FileCtx.Desc [i].Name,
                       sxs_FileCtx.Desc [i].Size,
                       sxs_FileCtx.Desc [i].Offset,
                       sxs_FileCtx.Desc [i].OFlag);

            TotalSize += sxs_FileCtx.Desc [i].Size;
        }

    }
    SXS_TRACE (_SXR|TSTDOUT|TDB|TNB_ARG(2),TSTR("%i files Total size %i\n",0x06ba0007), sxs_FileCtx.NbFiles, TotalSize);

#endif
}
#endif

/*
==============================================================================
   Function   :
 -----------------------------------------------------------------------------
   Scope      :
   Parameters : none
   Return     : none
==============================================================================
*/
void sxs_BackTrace (u32 Ra, u32 *Sp)
{
    u32 i = 0;
    u32 Cnt = 0;
    static u8 Lock = FALSE;

    pal_SoftAbort ();

    if (!sxr_SchedulingStarted())
    {
        SXS_TRACE (_SXR|TSTDOUT,"Scheduling not started");
    }
    else if (Lock != TRUE)
    {
        if (!sxs_CheckStkJb (Sp))
        {
            if (!sxs_CheckStkTk (Sp))
            {
                SXS_TRACE (_SXR|TSTDOUT|TDB|TNB_ARG(1),TSTR("Stack owner not found 0x%x\n",0x06ba0008), Sp);
                sxr_CheckJob ();
            }
        }
        else
        {
            Lock = TRUE;
            sxs_CheckPendingTk ();
            Lock = FALSE;
        }
    }

    SXS_TRACE (_SXR|TSTDOUT|TDB,TSTR("Back trace\n",0x06ba0009));

    SXS_TRACE (_SXR|TSTDOUT|TDB|TNB_ARG(1),TSTR("%lxy\n",0x06ba000a), Ra, Sp);

    while ((Cnt < 20) && (i++ < 300))
    {
        if (PAL_IS_ROM(*Sp))
        {
            SXS_TRACE (_SXR|TSTDOUT|TDB|TNB_ARG(1),TSTR("%lxy\n",0x06ba000b), *Sp);
            Cnt++;
        }
        (Sp)++;
    }

}

