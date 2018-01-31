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
#include "sxs_rmt.h"
#include "sxs_rmc.h"
#include "sxs_lib.h"
#include "sxs_io.h"
#include "sxs_srl.h"
#include "sxs_spy.h"

#if !defined (__EMBEDDED__) || defined (__SERIAL__)
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdarg.h>
#if defined (__WIN32__) || defined (linux)
#include "sxs_udp.h"
#endif
#endif




/*
==============================================================================
   Function   : sxs_PrcsRmtData
 ----------------------------------------------------------------------------
   Scope      :  Process a frame coming from remote computer.
   Parameters :  Flow Id.
                 Pointer onto Data.
                 Data size.
   Return     :  none.
==============================================================================
*/
void sxs_PrcsRmtData (u8 Id, u8 *Data, u16 Size)
{

// SXS_TRACE (_SXR|TDB|TNB_ARG(1),TSTR("Process RMC %x\n",0x06bd0001), Id);
// SXS_DUMP (_SXR, 0, Data, Size);

    switch (Id)
    {
#ifdef _TTCN_TEST_
        case SXS_WCPU_RMC:
            // uart backup
            //_sxs_SendToRemote(Id, Data, Size-1, SXS_RMT_SPL_CPU_IDX);

            // length -1 for the checksum

            //SXS_TRACE(_SXR|TSTDOUT,  "xcpu receive data \n");
            sxs_sendTtcnDataToWcpu(Id, Data, Size-1);
            //SXS_TRACE(_SXR|TSTDOUT,  "xcpu send data to w \n");
            break;
#endif

#if !defined(__EMBEDDED__) && (defined (__WIN32__) || defined (linux))
        case SXS_TRACE_RMC:
        case SXS_TIME_STAMP_RMC:
        case SXS_DUMP_RMC:
        case SXS_WIN_RMC:
        case SXS_SPY_RMC:
//  case SXS_INIT_RMC:
        case SXS_MEM_RMC:
//  sxs_SendToUdp (Id, Data, Size);
            break;
#endif

#if (!defined(__EMBEDDED__)) && (!defined (__WIN32__)) && (!defined(linux)) || defined (__SERIAL__)
        case SXS_TRACE_RMC:
        {
            u16  TId = *((u16 *)Data);
            u16  FmtSize = strlen ((ascii *)&Data [2]);
            u32 *StrBuffer;
            u8   PCnt = 0, i = 0, SMap = 0;
            u8  *Fmt = &Data [2];

            if (!(TId & TIDU))
                printf ("%s %2i: ", sxs_TraceDesc [TGET_ID(TId)], TGET_LEVEL(TId) + 1);

            StrBuffer = (u32 *)malloc (Size);
            memcpy (StrBuffer, Data + FmtSize + 2 + 1, Size - (FmtSize + 2 + 1));

            while (i<FmtSize)
            {
                if (Fmt [i] == '%')
                {
                    i++;

                    if (Fmt [i] == 's')
                        SMap |= (1 << PCnt);

                    PCnt++;
                }
                i++;
            }

            if (SMap)
            {
                u16 SSize = FmtSize + 2 + 1;
                u8 i = 0;

                while (SSize < (Size - 2))
                {
                    if (SMap & (1 << i))
                    {
                        StrBuffer [i] = (u32)(Data + SSize);
                        SSize += strlen ((ascii *)StrBuffer [i]) + 1;
                    }
                    else
                    {
                        memcpy ((u8 *)&StrBuffer [i], Data + SSize, 4);
                        SSize += 4;
                    }

                    i++;
                }
            }

            vfprintf (stdout, (ascii *)&Data [2], (va_list) StrBuffer);

            free (StrBuffer);

            break;
        }
#endif
#if !defined(__EMBEDDED__) && !defined (__WIN32__) && !defined(linux)
        case SXS_TIME_STAMP_RMC:
        {
            u32 Fn = *((u32 *)Data);
            printf ("Fn %i T1 %i T2 %i T3 %i\n", Fn, Fn/1326, Fn%26, Fn%51);
            break;
        }
#endif
#if !defined(__EMBEDDED__) && !defined (__WIN32__) && !defined(linux)
        case SXS_DUMP_RMC:
        {
            sxs_FlwDmp_t *FlwDmp = (sxs_FlwDmp_t *)Data;
            u8 DataSize = (FlwDmp -> Fmt [1] - '0') / 2;
            u16 SizeDmp, i;
            const ascii *Fmt = "%02x";

            if (!(FlwDmp -> Id & TIDU))
                printf ("%s %2i:", sxs_TraceDesc [TGET_ID(FlwDmp -> Id)], TGET_LEVEL (FlwDmp -> Id) + 1);

            if ((DataSize != 1) && (DataSize != 2) && (DataSize != 4))
            {
                DataSize = 1;
                strcpy (FlwDmp -> Fmt, "%2x");
            }

            SizeDmp = FlwDmp -> Size / DataSize;

            switch (DataSize)
            {
                case 1:

                    for (i=0; i<SizeDmp; i++)
                    {
                        if (((i & 0xF) == 0)
                                &&   (i!= 0))
                            printf ("\n        ");
                        else
                            printf (" ");

                        printf (Fmt, FlwDmp -> Data [i]);
                    }
                    break;

                case 2:

                    for (i=0; i<SizeDmp; i++)
                    {
                        if (((i & 0xF) == 0)
                                &&   (i != 0))
                            printf ("\n         ");
                        else
                            printf (" ");

                        printf (FlwDmp -> Fmt, ((u16 *)FlwDmp -> Data) [i]);
                    }
                    break;

                case 4:

                    for (i=0; i<SizeDmp; i++)
                    {
                        if (((i & 0xF) == 0)
                                &&   (i != 0))
                            printf ("\n         ");
                        else
                            printf (" ");

                        printf (FlwDmp -> Fmt, ((u32 *)FlwDmp -> Data) [i]);
                    }
                    break;
            }
            printf ("\n");
            break;
        }
#endif

        case SXS_TRACE_LEVEL_RMC :
        {
            sxs_FlwTLev_t *FlwTLev = (sxs_FlwTLev_t *)Data;
            if(FlwTLev->flwCtrl == 0)
            {
                sxs_SetTraceLevel (FlwTLev -> Id, FlwTLev->BitMap);
            }
            else //flow control enable
            {
                if (FlwTLev->BitMap & SXS_HOLD_TX)
                    pal_TraceSetTxEnabled(FALSE);
                else if (FlwTLev->BitMap & SXS_RELEASE_TX)
                    pal_TraceSetTxEnabled(TRUE);

                if (FlwTLev->BitMap & SXS_HOLD_RX)
                    pal_TraceSetRxEnabled(FALSE);
                else if (FlwTLev->BitMap & SXS_RELEASE_RX)
                    pal_TraceSetRxEnabled(TRUE);
            }
            break;
        }

        case SXS_RPC_RMC :
        {
            sxs_FlwRpc_t *FlwRpc = (sxs_FlwRpc_t *)Data;
            u32 Param [SXS_NB_RPC_PARAM];
            u8  NbParam;
            u32 RetValue;

            for (NbParam=0; NbParam<SXS_NB_RPC_PARAM; NbParam++)
            {
                if (FlwRpc -> Len [NbParam] != SXS_RPC_NO_PARAM)
                {
                    if (FlwRpc -> Len [NbParam] == SXS_RPC_VALUE_PARAM)
                        Param [NbParam] = FlwRpc -> Param [NbParam];
                    else
                        Param [NbParam] = (u32)&FlwRpc -> Param [NbParam];
                }
                else
                    break;
            }
            RetValue = FlwRpc -> Rpc (Param [0], Param [1], Param [2], Param [3]);

            if (FlwRpc -> Id != SXS_RPC_RETURN_VOID)
            {
                sxs_FlwRpcRsp_t *FlwRpcRsp = (sxs_FlwRpcRsp_t *)pal_TraceRequestBuf (SXS_RPC_RSP_RMC, (u16)(SXS_GET_RPC_RET_SZE(FlwRpc -> Id) + sizeof (sxs_FlwRpcRsp_t)));
                if (FlwRpcRsp == NIL)
                    break;

                FlwRpcRsp -> Id = FlwRpc -> Id;

                if (FlwRpc -> Id & SXS_RPC_RETURN_VALUE)
                    FlwRpcRsp -> Data [0] = RetValue;
                else
                    memcpy ((u8 *)&FlwRpcRsp -> Data [0], (u8 *)RetValue, SXS_GET_RPC_RET_SZE(FlwRpc -> Id));

                pal_TraceBufFilled (FlwRpcRsp);
            }
            break;
        }

        case SXS_RPC_RSP_RMC :
        {
            sxs_FlwRpcRsp_t *FlwRpcRsp = (sxs_FlwRpcRsp_t *)Data;

            u32 Return;

            if (FlwRpcRsp -> Id & SXS_RPC_RETURN_VALUE)
                Return = FlwRpcRsp -> Data [0];
            else
            {
                Return = (u32)FlwRpcRsp -> Data;
                sxr_Link (Data);
            }

            if (FlwRpcRsp -> Id & SXS_RPC_JOB)
                sxr_RaiseFrameJEvent ((u8)SXS_GET_RPC_USR_ID(FlwRpcRsp -> Id), (void *)Return);
            else
            {
                u32 Evt [2];
                Evt [0] = Return;

                sxr_Send (Evt, (u8)SXS_GET_RPC_USR_ID(FlwRpcRsp -> Id), SXR_SEND_EVT);
            }

            break;
        }

        case SXS_READ_RMC :
        {
            sxs_FlwRW_t *InFlwRW  = (sxs_FlwRW_t *)Data;
            sxs_FlwDmp_t *OutFlwDmp = (sxs_FlwDmp_t *)pal_TraceRequestBuf (SXS_DUMP_RMC, (u16)(InFlwRW -> Size + sizeof (sxs_FlwDmp_t)));
            if (OutFlwDmp == NIL)
                break;

            u16 SizeRW, i;
            u8  readId;
            // todo: what if OutFlwDmp == 0

            //release Tx && open SXR level
            pal_TraceSetTxEnabled(TRUE);
            sxs_SetTraceLevel( TID(__SXR), 0x3);

            // FIXME In this case, the packet should be discarded.
            if ((InFlwRW -> Access != 1) && (InFlwRW -> Access != 2) && (InFlwRW -> Access != 4))
                InFlwRW -> Access = 1;

            SizeRW = InFlwRW -> Size / InFlwRW -> Access;

            readId = InFlwRW -> ReadId;

            OutFlwDmp -> Id   = _SXR;
            OutFlwDmp -> Size = InFlwRW -> Size;
            OutFlwDmp -> Address = InFlwRW -> Address;

            switch (InFlwRW -> Access)
            {
                // The "x" in the format byte array is replaced
                // by the readId. (This is where you should start digging.)
                case 1:
                    memcpy (OutFlwDmp -> Data, InFlwRW -> Address, SizeRW);
                    strcpy (OutFlwDmp -> Fmt, " 2x");
                    OutFlwDmp -> Fmt[2] = readId;
                    break;

                case 2:
                {
                    u16 *Add = (u16 *)InFlwRW -> Address;
                    for (i=0; i<SizeRW; i++)
                        ((u16 *)OutFlwDmp -> Data) [i] = *Add++;
                    strcpy (OutFlwDmp -> Fmt, " 4x");
                    OutFlwDmp -> Fmt[2] = readId;
                }
                break;

                case 4:
                {
                    u32 *Add = (u32 *)InFlwRW -> Address;
                    for (i=0; i<SizeRW; i++)
                        ((u32 *)OutFlwDmp -> Data) [i] = *Add++;
                    strcpy (OutFlwDmp -> Fmt, " 8x");
                    OutFlwDmp -> Fmt[2] = readId;
                }
                break;
            }

            pal_TraceBufFilled (OutFlwDmp);

            SXS_FPRINTF ((_SXR|TDB|TNB_ARG(2),TSTR("Read %i bytes at 0x%lx\n",0x06bd0002), InFlwRW -> Size, InFlwRW -> Address));
            break;
        }
        case SXS_WRITE_RMC :
        {
            sxs_FlwRW_t *FlwRW  = (sxs_FlwRW_t *)Data;

            u16 SizeRW, i;

            // FIXME In this case, the packet should be discarded.
            if ((FlwRW -> Access != 1) && (FlwRW -> Access != 2) && (FlwRW -> Access != 4))
                FlwRW -> Access = 1;

            SizeRW = FlwRW -> Size / FlwRW -> Access;

            switch (FlwRW -> Access)
            {
                case 1:
                    memcpy (FlwRW -> Address, FlwRW -> Data, SizeRW);
                    break;

                case 2:
                {
                    u16 *Add = (u16 *)FlwRW -> Address;
                    for (i=0; i<SizeRW; i++)
                        *Add++ = ((u16 *)FlwRW -> Data) [i];
                }
                break;

                case 4:
                {
                    u32 *Add = (u32 *)FlwRW -> Address;
                    for (i=0; i<SizeRW; i++)
                        *Add++ = ((u32 *)FlwRW -> Data) [i];
                }
                break;
            }
            SXS_FPRINTF ((_SXR|TDB|TNB_ARG(2),TSTR("Write %i bytes at 0x%lx\n",0x06bd0003), FlwRW -> Size, FlwRW -> Address));
            break;
        }
        case SXS_DWL_START_RMC :
        {
            break;
        }
        case SXS_DWL_RMC :
        {
            break;
        }
        case SXS_MBX_RMC :
        {
            sxs_RmtMbx_t *RmtMbx = (sxs_RmtMbx_t *)Data;

#ifndef __TARGET__
            if (sxr_IsRmtSbx (RmtMbx->Mbx))
            {
//    sxs_SendToUdp (Id, Data, Size);
            }
            else
#endif
            {
                u8  Mbx = RmtMbx -> Mbx;
                u8  MsgStatus = RmtMbx -> MsgStatus;
                u8 *Msg = (u8 *)sxr_Malloc (Size - 4);

                if(RmtMbx->WaitResponse)
                {
                    pal_TraceSetTxEnabled (TRUE);
                }

                memcpy (Msg, (u8 *)RmtMbx -> Data, Size - 4); // - sizeof (sxs_RmtMbx_t) + 4);

                sxr_Send (sxr_SbxRxMsg (Msg), Mbx, MsgStatus);
            }
            break;
        }

        case SXS_RAZ_RMC:
#ifdef __TARGET__
            sxr_Reset (0);
#endif

            break;


        case SXS_SERIAL_RMC :
            break;

        case SXS_INIT_RMC:
            sxs_InitRmc ();
            break;

        case SXS_EXIT_RMC:
            SXS_TRACE (_SXR|TDB|TNB_ARG(1),TSTR("Process RMC %x\n",0x06bd0001), Id);

            break;

        default:
//  SXS_TRACE (_SXR|TDB|TNB_ARG(1),TSTR("RMC not processed %x\n",0x06bd0004), Id);
            break;
    }

#ifdef __SXS_SPLIT_2_CPU__
    switch (Id)
    {
        case SXS_SPY_RMC:
        case SXS_TRACE_RMC:
        case SXS_WCPU_RMC:
        case SXS_DUMP_RMC:
        case SXS_RPC_RSP_RMC:
//  _sxs_SendToRemote (Id, Data, Size, SXS_RMT_DBG_CPU_IDX);
            break;

        case SXS_READ_RMC:
        case SXS_WRITE_RMC:
        case SXS_TRACE_LEVEL_RMC:
        case SXS_RPC_RMC:
        case SXS_RAZ_RMC:
            _sxs_SendToRemote (Id, Data, Size, SXS_RMT_SPL_CPU_IDX);
            break;
    }
#endif
}


/*
==============================================================================
   Function   : sxs_CtxInit
 -----------------------------------------------------------------------------
   Scope      :
   Parameters : none
   Return     : none
==============================================================================
*/
void sxs_RmcCtxInit (void)
{
#ifndef __SXS_NO_FLASH_HANDLER__
    sxs_IoCtxHandle = sxs_Open ("SXR.SET", SXS_O_CREAT | SXS_O_RDWR | SXS_O_BINARY, SXS_S_IREAD | SXS_S_IWRITE);

    if (sxs_IoCtxHandle != -1)
    {
        if (sxs_LSeek (sxs_IoCtxHandle, 0, SXS_SEEK_END) < sizeof (sxs_IoCtx_t))
        {
            sxs_LSeek (sxs_IoCtxHandle, 0, SXS_SEEK_SET);
            sxs_Write (sxs_IoCtxHandle, &sxs_IoCtx, sizeof (sxs_IoCtx_t));
        }
        else
        {
            u32 i;

            sxs_LSeek (sxs_IoCtxHandle, 0, SXS_SEEK_SET);
            sxs_Read  (sxs_IoCtxHandle, &sxs_IoCtx, sizeof (sxs_IoCtx_t));

            for (i=0; i<32; i++)
            {
                if (sxs_IoCtx.SpyEnableBitmap & (1 << i))
                    sxs_SpyEnable (i, TRUE);
            }
        }
    }
#endif
}

/*
==============================================================================
   Function   : sxs_SetTraceLevel
 -----------------------------------------------------------------------------
   Scope      : Set trace levels as required by the bit field.
   Parameters : User Id and trace level bit map to be set.
   Return     : None.
==============================================================================
*/
void sxs_SetTraceLevel (u8 Id, u16 LevelBitMap)
{
    if (TGET_ID(Id) < SXS_NB_ID)
        sxs_IoCtx.TraceBitMap [TGET_ID(Id)] = LevelBitMap;

#ifndef __SXS_NO_FLASH_HANDLER__
    if (sxs_IoCtxHandle != -1)
    {
        sxs_LSeek (sxs_IoCtxHandle, OFFSETOF(sxs_IoCtx_t, TraceBitMap), SXS_SEEK_SET);
        sxs_Write (sxs_IoCtxHandle, &sxs_IoCtx.TraceBitMap , SXS_NB_ID * sizeof (u16));
    }
#endif
}

/*
===============================================================================
   Function   : sxs_SetSpy
 ------------------------------------------------------------------------------
   Scope      : Enable or disable identified spy.
   Parameters : Spy index.
   Return     : none.
==============================================================================
*/
void sxs_SetSpy (u32 SpyEnableBitmap)
{
    u32 i;
    sxs_IoCtx.SpyEnableBitmap = SpyEnableBitmap;

#ifndef __SXS_NO_FLASH_HANDLER__
    if (sxs_IoCtxHandle != -1)
    {
        sxs_LSeek (sxs_IoCtxHandle, OFFSETOF(sxs_IoCtx_t, SpyEnableBitmap), SXS_SEEK_SET);
        sxs_Write (sxs_IoCtxHandle, &sxs_IoCtx.SpyEnableBitmap, sizeof (u32));
    }
#endif

    for (i=0; i<SXS_NB_MAX_SPY; i++)
    {
        if (sxs_IoCtx.SpyEnableBitmap & (1 << i))
            sxs_SpyEnable (i, TRUE);
        else
            sxs_SpyEnable (i, FALSE);
    }
}

/*
==============================================================================
   Function   : sxs_InitRmc
 -----------------------------------------------------------------------------
   Scope      : Send the Init flow to the remote CPU.
                The Init flow contains the list of trace descriptors (3 char
                / descriptor).
   Parameters : none
   Return     : none
==============================================================================
*/
void  sxs_InitRmc (void)
{
#if 1
    u32 dummy = 0;
    pal_TraceSendData(SXS_INIT_RMC, &dummy, 4);
#else
    sxs_FlwInit_t *FlwInit = (sxs_FlwInit_t *)sxs_SendToRemote (SXS_INIT_RMC, NIL, (3 * SXS_NB_ID) + sizeof (sxs_FlwInit_t) - 4);
    u32 i, j=0;

    FlwInit -> NbId = SXS_NB_ID;

    for (i=0; i<SXS_NB_ID; i++)
    {
        memcpy (&FlwInit -> Data [j], sxs_TraceDesc [i], 3);
        j += 3;
    }
#endif
}

