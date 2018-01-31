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





#include "sxs_type.h"
#include "sxs_lib.h"
#include "sxr_ops.h"
#include "sxs_rmc.h"
#include "sxs_rmt.h"
#include "sxs_spy.h"
#include "sxs_spy.hp"

#define __SXS_COMPLEX_SPY__

/*
===============================================================================
   Function   : sxs_NewSpy
 ------------------------------------------------------------------------------
   Scope      : New spy creation.
   Parameters : Spy name,
                Pointer to spy string description array.
                User Id.
                Pointer to data array to be spyed
                Size of the arrays.
   Return     : Spy index.
===============================================================================
*/
u8 sxs_NewSpy (const ascii *Name, u8 UsrId, const ascii *const *Str, s16 *Data, u8 Size)
{
    u32 i;
    u16 StrLen = 0;

    u32 Status = sxr_EnterSc ();

    if (sxs_Spy.Idx == SXS_SPY_NB_MAX)
    {
        sxr_ExitSc (Status);
        return 0xFF;
    }

    u8 spyIdx = sxs_Spy.Idx;
    sxs_Spy.Idx++;

    sxr_ExitSc (Status);

    sxs_Spy.Desc [spyIdx].Name = Name;
    sxs_Spy.Desc [spyIdx].Str  = Str;
    sxs_Spy.Desc [spyIdx].UsrId = UsrId;

    /* Compute the sum of the length of all the spy items descriptor. */
    for (i=0; i<Size; i++)
        StrLen += strlen (Str [i]) + 1;

    sxs_Spy.Desc [spyIdx].StrLen = StrLen;

    sxs_Spy.Desc [spyIdx].Data = Data;
    memset (sxs_Spy.Desc [spyIdx].Data, 0, Size * 2);

    sxs_Spy.Desc [spyIdx].LastData = (s16 *)sxr_HMalloc ((u16)(Size * 2));
    memset (sxs_Spy.Desc [spyIdx].LastData, 0xFF, Size * 2);

    sxs_Spy.Desc [spyIdx].Enable = FALSE;
    sxs_Spy.Desc [spyIdx].Size   = Size;

    sxs_SpyStr (spyIdx);

    return spyIdx;
}


/*
===============================================================================
   Function   : sxs_SpyStr
 ------------------------------------------------------------------------------
   Scope      : Send spy name and string description array of the identified
                spy to the remote computer.
   Parameters : Spy index.
   Return     : none.
===============================================================================
*/
void sxs_SpyStr (u8 SpyIdx)
{
    sxs_SpyInfo_t *SpyInfo;
    u16 Size = sizeof (sxs_SpyInfo_t) + strlen (sxs_Spy.Desc [SpyIdx].Name) + 1;

    if ((SpyInfo = (sxs_SpyInfo_t *)sxs_SendToRemote (SXS_SPY_RMC, NIL, Size)) != NIL )
    {
        /* Send Spy name. */
        SpyInfo -> SpyIdx = SpyIdx;
        SpyInfo -> Id     = SXS_SPY_NAME_ID;
        SpyInfo -> Data   = sxs_Spy.Desc [SpyIdx].Size;

        memcpy ((u8 *)SpyInfo + sizeof(sxs_SpyInfo_t), sxs_Spy.Desc [SpyIdx].Name, Size - sizeof(sxs_SpyInfo_t));
    }

    Size = sizeof (sxs_SpyInfo_t) + sxs_Spy.Desc [SpyIdx].StrLen;

    if ((SpyInfo = (sxs_SpyInfo_t *)sxs_SendToRemote (SXS_SPY_RMC, NIL, Size)) != NIL )
    {
        u8 *SpyData = (u8 *)SpyInfo + sizeof(sxs_SpyInfo_t);
        u8  StrLen;
        u32 i;

        /* Send spy descriptor. */
        SpyInfo -> SpyIdx = SpyIdx;
        SpyInfo -> Id     = SXS_SPY_STR_ID;
        SpyInfo -> Data   = sxs_Spy.Desc [SpyIdx].Size;

        for (i=0; i<sxs_Spy.Desc [sxs_Spy.Idx].Size; i++)
        {
            StrLen = strlen (sxs_Spy.Desc [SpyIdx].Str [i]) + 1;
            memcpy (SpyData, sxs_Spy.Desc [SpyIdx].Str [i], StrLen);
            SpyData += StrLen;
        }
    }
}


/*
===============================================================================
   Function   : sxs_SpyData
 ------------------------------------------------------------------------------
   Scope      : Send spy data of all existing spy spy to the remote computer.
   Parameters : none.
   Return     : none.
==============================================================================
*/
void sxs_SpyData (void)
{
    u32 j;
    u8  i = sxs_Spy.CurIdx;

#ifdef __SXS_COMPLEX_SPY__
    u8  NbUpdate;
    u8  NbDispatch;
    u8  NbCompressedData;
    u8  SetCompresseData;
    u8  SpyIdx [SXS_NB_MAX_SPY];
#endif

    if (++sxs_Spy.CurIdx >= sxs_Spy.Idx)
        sxs_Spy.CurIdx = 0;

// for (i=0; i < sxs_Spy.Idx; i++)
    {
#ifdef __SXS_COMPLEX_SPY__
        NbUpdate = 0;
        NbDispatch = 0;
        NbCompressedData = 0;
        SetCompresseData = TRUE;
#endif

        if (sxs_Spy.Desc [i].Enable)
        {
            for (j=0; j<sxs_Spy.Desc [i].Size; j++)
            {
                if (sxs_Spy.Desc [i].Data [j] != sxs_Spy.Desc [i].LastData [j])
                {
#ifndef __SXS_COMPLEX_SPY__
                    sxs_SpyInfo_t *SpyInfo;
                    sxs_SpyData_t *SpyData;
                    u16 Size = sizeof (sxs_SpyInfo_t) + sizeof (sxs_SpyData_t);

                    if ((SpyInfo = (sxs_SpyInfo_t *)sxs_SendToRemote (SXS_SPY_RMC, NIL, Size)) != NIL )
                    {
                        SpyInfo -> SpyIdx = i;
                        SpyInfo -> Id     = SXS_SPY_DATA_ID;

                        SpyData = (sxs_SpyData_t *)(SpyInfo + 1);

                        SpyData->Id = j;
                        SpyData->Data =
                            sxs_Spy.Desc [i].LastData [j] = sxs_Spy.Desc [i].Data [j];
                    }
                    else
                        break;
#else
                    SpyIdx [NbUpdate] = j;

                    if (SetCompresseData)
                    {
                        if (sxs_Spy.Desc [i].Data [j] < 128)
                        {
                            NbCompressedData++;
                            SpyIdx [NbUpdate] |= (1 << 7);
                        }
                        else
                            SetCompresseData = FALSE;
                    }

                    if (sxs_Spy.Desc [i].DispatchFields & (1 << j))
                        NbDispatch++;

                    if (++NbUpdate >= SXS_NB_MAX_SPY)
                        break;
#endif
                }
            }
#ifdef __SXS_COMPLEX_SPY__
            if (NbUpdate > 0)
            {
                sxs_SpyInfo_t *SpyInfo;
                s8  *SpyDB;
                s16 *SpyDW;

                if (NbUpdate != NbCompressedData)
                {
                    if (NbCompressedData & 1)
                    {
                        NbCompressedData &= 0xFE;
                        SpyIdx [NbCompressedData] &= ~(1 << 7);
                    }
                }

                if ((SpyInfo = (sxs_SpyInfo_t *)pal_TraceRequestBuf (SXS_SPY_RMC, sizeof (sxs_SpyInfo_t) + (((NbUpdate * 3) + 1) & 0xFE)  - NbCompressedData)) != NIL)
                {
                    SpyInfo -> SpyIdx = i;
                    SpyInfo -> Id     = SXS_SPY_MULTI_ID;
                    SpyInfo -> Data   = NbUpdate;

                    memcpy ((u8 *)(SpyInfo + 1), SpyIdx, NbUpdate);
                    SpyDB = (s8 *)((u8 *)(SpyInfo + 1) + ((NbUpdate + 1) & 0xFE));
                    SpyDW = (s16 *)((u8 *)(SpyInfo + 1) + ((NbUpdate + NbCompressedData + 1) & 0xFE));

                    for (j=0; j<NbCompressedData; j++)
                    {
                        u8 Idx = (SpyIdx [j] & ~(1 << 7));
                        sxs_Spy.Desc [i].LastData [Idx] = (s16)sxs_Spy.Desc [i].Data [Idx];
                        SpyDB [j] = (s8)sxs_Spy.Desc [i].Data [Idx];
                    }

                    for (j=0; j<(u32)(NbUpdate-NbCompressedData); j++)
                    {
                        u8 Idx = (SpyIdx [NbCompressedData + j] & ~(1 << 7));
                        SpyDW [j] =
                            sxs_Spy.Desc [i].LastData [Idx] = sxs_Spy.Desc [i].Data [Idx];
                    }

                    pal_TraceBufFilled (SpyInfo);

                    if (sxs_Spy.Desc [i].DispatchFields != 0)
                    {
                        u16 *BaseData = (u16 *)sxr_Malloc ((NbDispatch * 4) + sxs_Spy.Desc [i].DispatchDataOffset);
                        u16 *Data = BaseData + (sxs_Spy.Desc [i].DispatchDataOffset / 2);

                        for (j=0; j<(u32)NbDispatch; j++)
                        {
                            u8 Idx = (SpyIdx [j] & ~(1 << 7));
                            Data [2*j]     = (sxs_Spy.Desc [i].UsrId << 8) | Idx;
                            Data [(2*j)+1] = sxs_Spy.Desc [i].Data [Idx];
                        }

                        sxs_Spy.Desc [i].DispatchFunction (sxs_Spy.Desc [i].UsrId, NbDispatch, BaseData);

                        sxr_Free (BaseData);
                    }
                }
                else
                {
                    return;
                }
            }
#endif
        }
    }
}

/*
===============================================================================
   Function   : sxs_SpyEnable
 ------------------------------------------------------------------------------
   Scope      : Enable or disable identified spy.
   Parameters : Spy index.
   Return     : none.
==============================================================================
*/
void sxs_SpyEnable (u8 SpyIdx, u8 Enable)
{
    if (SpyIdx < SXS_SPY_NB_MAX)
        sxs_Spy.Desc [SpyIdx].Enable = Enable;
}

/*
===============================================================================
   Function   : sxs_UsrSpyEnable
 ------------------------------------------------------------------------------
   Scope      : Enable or disable the spy identified by the user Id.
   Parameters : Spy user Id.
   Return     : none.
==============================================================================
*/
void sxs_UsrSpyEnable (u8 UsrId, u8 Enable)
{
    u32 i;

    for (i=0; i<SXS_SPY_NB_MAX; i++)
    {
        if (sxs_Spy.Desc [i].UsrId == UsrId)
        {
            sxs_SpyEnable (i, Enable);
            return;
        }
    }
}


/*
===============================================================================
   Function   : sxs_SpyDispatchRegistration
 ------------------------------------------------------------------------------
   Scope      : Register a spy to dispatch the required fields to the required
                mail box.
   Parameters : - Index identifying the spy,
                - Dispatch function called at each spy update.
                  -> Spy User Id
                  -> Number of updates
                  -> Pointer to update (u16 Field Idx + u16 field data)
                - Bitmap of the field to be dispatch in the required mailbox.
                - number of bytes (modulo 4) that must be left free to ...
   Return     : none.
==============================================================================
*/
void sxs_SpyDispatchRegistration (u8 UsrId,
                                  void (*DispatchFunction)(u8, u8, u16 *),
                                  u32 DispatchFields,
                                  u8 DispatchDataOffset)
{
    u32 i;

    for (i=0; i<SXS_SPY_NB_MAX; i++)
    {
        if (sxs_Spy.Desc [i].UsrId == UsrId)
        {
            sxs_Spy.Desc [i].DispatchFields     = DispatchFields;
            sxs_Spy.Desc [i].DispatchFunction   = DispatchFunction;
            sxs_Spy.Desc [i].DispatchDataOffset = DispatchDataOffset;
        }
    }
}




/*
===============================================================================
   Function   :
 ------------------------------------------------------------------------------
   Scope      :
   Parameters :
   Return     : none.
==============================================================================
*/
/*
===============================================================================
   Function   :
 ------------------------------------------------------------------------------
   Scope      :
   Parameters :
   Return     : none.
==============================================================================
*/
/*
===============================================================================
   Function   :
 ------------------------------------------------------------------------------
   Scope      :
   Parameters :
   Return     : none.
==============================================================================
*/



