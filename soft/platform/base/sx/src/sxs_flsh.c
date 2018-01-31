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
#include "sxs_io.h"
#define __PAL_FLSH_VAR__
#ifdef __SXS_FLASH_INTEL__
#include "pal_flhi.h"
#else
#include "pal_flhr.h"
#endif
#undef __PAL_FLSH_VAR__
#include "sxr_ops.h"
#include "sxs_io.h"
#include "sxs_lib.h"
#include "sxs_flsh.hp"
#include "sxs_flsh.h"


#ifndef __SXS_FILE_IN_RAM__
const sxs_FlashDesc_t *sxs_FlashDesc [] =
{
#ifdef __SXS_FLASH_INTEL__
    &pal_Intel16Mx16T,
    &pal_Intel32Mx16T,
#else
    &pal_RamFlashDesc
#endif
};

const u8 sxs_NbFlashHandled = sizeof (sxs_FlashDesc) / sizeof (sxs_FlashDesc_t *);
#endif


#define ZOB(a) 0  // Pad in Footer used to store data.

#ifndef __SXS_FILE_IN_RAM__
/*
==============================================================================
   Function   : sxs_FlashInit
 -----------------------------------------------------------------------------
   Scope      : Initialize data context and check for flash consistency.
   Parameters : none
   Return     : none
==============================================================================
*/
u8 sxs_FlashInit (void)
{
    sxs_FlashId_t FlashId;
    u32 i;
    u32 Status = sxr_EnterSc ();

// SXS_TRACE (_SXR|TSTDOUT|TDB|TNB_ARG(2),TSTR("Flash 0x%x 0x%x\n",0x06b90001), FlashId.ManufCode, FlashId.DeviceId);

    for (i=0; i<sxs_NbFlashHandled; i++)
    {
        sxs_FlashDesc [i] -> FlhIdentification (&FlashId);

        if ((sxs_FlashDesc [i] -> Id.ManufCode == FlashId.ManufCode)
                &&  (sxs_FlashDesc [i] -> Id.DeviceId  == FlashId.DeviceId))
            break;
    }

    sxr_ExitSc (Status);

    if (i >= sxs_NbFlashHandled)
        return FALSE;

    sxs_FlashCtx.Desc = sxs_FlashDesc [i];

    sxs_FlashCtx.SectorToAdd    = sxs_FlashCtx.Desc -> FlhSectorToAdd;
    sxs_FlashCtx.Lock           = sxs_FlashCtx.Desc -> FlhLock;
    sxs_FlashCtx.UnLock         = sxs_FlashCtx.Desc -> FlhUnLock;
    sxs_FlashCtx.EraseSector    = sxs_FlashCtx.Desc -> FlhEraseSector;
    sxs_FlashCtx.Write          = sxs_FlashCtx.Desc -> FlhWrite;

    if (((sxs_BlockFooter_t *)sxs_FlashCtx.Desc -> DynamicBnk [0].StartAdd) -> Id == SXS_FLH_BANK_READY)
        sxs_FlashCtx.DynamicIdx = 0;
    else if (((sxs_BlockFooter_t *)sxs_FlashCtx.Desc -> DynamicBnk [1].StartAdd) -> Id == SXS_FLH_BANK_READY)
        sxs_FlashCtx.DynamicIdx = 1;
    else
    {
        sxs_BankErase (&sxs_FlashCtx.Desc -> DynamicBnk [0]);
        sxs_BankErase (&sxs_FlashCtx.Desc -> DynamicBnk [1]);
        sxs_FlashCtx.DynamicIdx = 0;
    }

    {
        u16 *BankAdd;
        u16 *BankLastAdd;
        u8   Corruption;
        sxs_BlockFooter_t *Footer;

        /* Check Dynamic bank. */
        do
        {
            BankAdd = sxs_FlashCtx.Desc -> DynamicBnk [sxs_FlashCtx.DynamicIdx].StartAdd;
            BankLastAdd = sxs_FlashCtx.Desc -> DynamicBnk [sxs_FlashCtx.DynamicIdx].LastAdd;
            Corruption = 0;

            while ((BankLastAdd > BankAdd)
                    &&     (*BankLastAdd == SXS_FLH_BANK_FREE))
                BankLastAdd--;

            sxs_FlashCtx.DynamicAdd = BankLastAdd + 1;

            Footer = (sxs_BlockFooter_t *)(sxs_FlashCtx.DynamicAdd - (sizeof (sxs_BlockFooter_t) / sizeof (u16)));; // 2);

            while (((u16 *)Footer > BankAdd) && (Footer -> Length))
            {
                if (Footer -> Length != *((u16 *)((u16 *)Footer) - Footer -> Length + (sizeof (sxs_BlockFooter_t) / sizeof (u16))))
                    break;
                Footer = (sxs_BlockFooter_t *)(((u16 *)Footer) - Footer -> Length);
            }
//    BankLastAdd -= *BankLastAdd;

            if ((u16 *)Footer != BankAdd)
            {
                if (!Corruption)
                {
                    sxs_FlashCtx.PurgeRequired = TRUE;
                    sxs_FlashPurge (TRUE, FALSE);
                }
                else
                    sxs_BankErase (&sxs_FlashCtx.Desc -> DynamicBnk [sxs_FlashCtx.DynamicIdx]);

                if (Corruption++ > 2)
                    return FALSE;
            }
        }
        while (Corruption != 0);

        /* Check Stacked bank. */

        BankAdd = sxs_FlashCtx.Desc -> StackedBnk.StartAdd;
        BankLastAdd = sxs_FlashCtx.Desc -> StackedBnk.LastAdd;

        if (((sxs_BlockFooter_t *)BankAdd) -> Id == SXS_FLH_BANK_READY)
        {
            while ((BankLastAdd > BankAdd)
                    &&     (*BankLastAdd == SXS_FLH_BANK_FREE))
                BankLastAdd--;

            sxs_FlashCtx.StackedAdd = BankLastAdd + 1;

            Footer = (sxs_BlockFooter_t *)(sxs_FlashCtx.StackedAdd - (sizeof (sxs_BlockFooter_t) / sizeof (u16)));; // 2);

            while (((u16 *)Footer > BankAdd) && (Footer -> Length))
            {
                if (Footer -> Length != *((u16 *)((u16 *)Footer) - Footer -> Length + (sizeof (sxs_BlockFooter_t) / sizeof (u16))))
                    break;

                Footer = (sxs_BlockFooter_t *)(((u16 *)Footer) - Footer -> Length);
            }
        }

        if ((u16 *)Footer != BankAdd)
        {
            sxs_BankErase (&sxs_FlashCtx.Desc -> StackedBnk);
            sxs_FlashCtx.StackedAdd = BankAdd + (sizeof (sxs_BlockFooter_t) / sizeof (u16));
        }


#ifndef __SXS_FLH_NO_STATIC__
        /* Check Static bank. */

        BankAdd = sxs_FlashCtx.Desc -> StaticBnk.StartAdd;
        BankLastAdd = sxs_FlashCtx.Desc -> StaticBnk.LastAdd;

        if (((sxs_BlockFooter_t *)BankAdd) -> Id == SXS_FLH_BANK_READY)
        {
            while ((BankLastAdd > BankAdd)
                    &&     (*BankLastAdd == SXS_FLH_BANK_FREE))
                BankLastAdd--;

            sxs_FlashCtx.StaticAdd = BankLastAdd + 1;

            Footer = (sxs_BlockFooter_t *)(sxs_FlashCtx.StaticAdd - (sizeof (sxs_BlockFooter_t) / sizeof (u16)));; // 2);

            while (((u16 *)Footer > BankAdd) && (Footer -> Length))
            {
                if (Footer -> Length != *((u16 *)((u16 *)Footer) - Footer -> Length + (sizeof (sxs_BlockFooter_t) / sizeof (u16))))
                    break;

                Footer = (sxs_BlockFooter_t *)(((u16 *)Footer) - Footer -> Length);
            }
        }

        if ((u16 *)Footer != BankAdd)
        {
            sxs_BankErase (&sxs_FlashCtx.Desc -> StaticBnk);
            sxs_FlashCtx.StaticAdd = BankAdd + (sizeof (sxs_BlockFooter_t) / sizeof (u16));
        }
#endif
    }

    sxs_FlashCtx.PurgeRequired = FALSE;

    return TRUE;
}
#endif

/*
==============================================================================
   Function   : sxs_FlashHandlerInit
 -----------------------------------------------------------------------------
   Scope      : Flash handler initialization.
   Parameters : none
   Return     : none
==============================================================================
*/
u8 sxs_FlashHandlerInit (void)
{
#ifndef __SXS_FILE_IN_RAM__
    sxs_FlashCtx.SemIdx = sxr_NewSemaphore (1);

    return sxs_FlashInit ();
#else
    u32 FileTotalSize = 0, i;


    for (i=0; i<SXS_FLH_NB_FILES_MAX; i++)
    {
        sxs_FileOffset [i] = FileTotalSize;
        FileTotalSize += sxs_FileMaxSize [i];
    }

    if (sxs_RamFile == NIL)
    {
        sxs_RamFile = sxr_HMalloc (FileTotalSize);
        memset (sxs_RamFile, 0, FileTotalSize);  // Or take content from the right location.
    }

    return TRUE;
#endif
}

#ifndef __SXS_FILE_IN_RAM__
/*
==============================================================================
   Function   : sxs_BankErase
 -----------------------------------------------------------------------------
   Scope      : Erase all the sectors of the identified bank.
   Parameters : Pointer on bank descriptor.
   Return     : none
==============================================================================
*/
void sxs_BankErase (const sxs_BankDesc_t *BankDesc)
{
    u8 i;

    for (i=0; i<BankDesc -> NbSector; i++)
    {
//  SXS_TRACE (_SXR|TSTDOUT|TDB|TNB_ARG(1),TSTR("Erase 0x%x\n",0x06b90002), sxs_FlashCtx.SectorToAdd ((u8)(BankDesc -> FirstSector + i)));
        sxs_FlashCtx.EraseSector (sxs_FlashCtx.SectorToAdd ((u8)(BankDesc -> FirstSector + i)));
    }

    sxs_FlashWrite (BankDesc -> StartAdd, (u16 *)&sxs_FlhBankReady, sizeof (sxs_BlockFooter_t));

}
#endif

#ifndef __SXS_FILE_IN_RAM__
/*
==============================================================================
   Function   : sxs_FlashPurge
 -----------------------------------------------------------------------------
   Scope      : Check status of the dynamic bank and of the stacked bank and
                purge if necessary.
   Parameters : Boolean Protect indicates when protection against concurrent
                access must be set.
   Return     : none
==============================================================================
*/
void sxs_FlashPurge (u8 Protect, u8 UpdtStatic)
{
    if (((u16)((u8 *)sxs_FlashCtx.Desc -> DynamicBnk [sxs_FlashCtx.DynamicIdx].LastAdd - (u8 *)sxs_FlashCtx.DynamicAdd)
            < sxs_FlashCtx.Desc -> DynamicBnk [sxs_FlashCtx.DynamicIdx].PurgeThres)
            || (sxs_FlashCtx.PurgeRequired))
    {
        u32 FilesStart [SXS_FLH_NB_FILES_MAX];
        u32 FilesEnd [SXS_FLH_NB_FILES_MAX];
        u32 i,j;
        u16 *BankStart [3];
        u8  NbBank = 1;
        sxs_BankDesc_t const *BankDesc = &sxs_FlashCtx.Desc -> DynamicBnk [(sxs_FlashCtx.DynamicIdx + 1) & 0x01];
        u16 *NewBank = BankDesc -> StartAdd + (sizeof (sxs_BlockFooter_t) / sizeof (u16));

        u32  TotalLength = 0;

        BankStart [0] = sxs_FlashCtx.Desc -> DynamicBnk [sxs_FlashCtx.DynamicIdx].StartAdd + (sizeof (sxs_BlockFooter_t) / sizeof (u16));
        BankStart [1] = sxs_FlashCtx.Desc -> StackedBnk.StartAdd + (sizeof (sxs_BlockFooter_t) / sizeof (u16));

#ifndef __SXS_FLH_NO_STATIC__
        BankStart [2] = sxs_FlashCtx.Desc -> StaticBnk.StartAdd + (sizeof (sxs_BlockFooter_t) / sizeof (u16));
#endif

        if (Protect)
            sxr_TakeSemaphore (sxs_FlashCtx.SemIdx);

        memset ((u8 *)FilesStart, 0xFF, sizeof (u32) * SXS_FLH_NB_FILES_MAX);
        memset ((u8 *)FilesEnd, 0, sizeof (u32) * SXS_FLH_NB_FILES_MAX);

        sxs_BankErase (BankDesc);

        sxs_GetFileBankLimit (FilesStart, FilesEnd, BankStart [0]);

        if (sxs_FlashCtx.PurgeRequired) // Some blocks may be stacked
        {
            NbBank++;
            sxs_GetFileBankLimit (FilesStart, FilesEnd, BankStart [1]);
        }

#ifndef __SXS_FLH_NO_STATIC__
        if (UpdtStatic)
            sxs_GetFileBankLimit (FilesStart, FilesEnd, BankStart [2]);
#endif

        for (i=0; i<SXS_FLH_NB_FILES_MAX; i++)
        {
            if (FilesEnd [i] != 0) // File length is not 0
            {
                sxs_BlockFooter_t NewFooter;

                u16 FileLen = (u16)(FilesEnd [i] - FilesStart [i]);
                u8 *Data = (u8 *)sxr_HMalloc ((u16)FileLen);

                NewFooter.Id = 0;

                /* Don't leave holes in static files. Could be skip for pure stacked files... */
                sxs_BankBlockRead (SXS_SET_FILE_IDX(i) | FilesStart [i], Data, FileLen, BankStart [2]);

                for (j=0; j<NbBank; j++)
                {
//     sxs_BankBlockRead (SXS_SET_FILE_IDX(i) | FilesStart [i], Data, FileLen, BankStart [j]);
#if 1
                    u16 *BankAdd = BankStart [j];
                    sxs_BlockFooter_t *Footer = (sxs_BlockFooter_t *)(BankAdd + *BankAdd - (sizeof (sxs_BlockFooter_t) / sizeof (u16)));

                    while ((*BankAdd != 0) && (*BankAdd != 0xFFFF) && (Footer -> Length == *BankAdd))
                    {
                        if ((Footer -> Id & SXS_FLH_VALID_ID)
                                &&  (SXS_GET_FILE_IDX(Footer -> Id) == i))
                        {
                            u32 RcdOffset = Footer -> Id & SXS_OFFSET_MSK;
//       u16 RcdLen    = (2*Footer -> Length) - (Footer -> Id & SXS_FLH_EXTRA_BYTE ? 1 : 0) - sizeof (sxs_BlockFooter_t) - 2;
                            u16 RcdLen    = (2*Footer -> Length) - (SXS_FLH_GET_EXTRA_BYTE(Footer -> Id)) - sizeof (sxs_BlockFooter_t) - ZOB(2);
                            memcpy (Data + RcdOffset - FilesStart [i], (u8 *)(BankAdd + 1), RcdLen);
                            FileLen -= RcdLen;
                            NewFooter.Id = Footer -> Id & SXS_FLH_PROTECTED_BLOCK;
//       SXS_TRACE (_SXR|TSTDOUT|TNB_ARG(3)|TDB,TSTR("Purge: Footer.Id %lx Footer.Len %i (Extra %i)\n",0x06b90003), Footer -> Id, 2*Footer -> Length, SXS_FLH_GET_EXTRA_BYTE(Footer -> Id));
                        }

                        BankAdd += Footer -> Length;

                        Footer = (sxs_BlockFooter_t *)(BankAdd + *BankAdd - (sizeof (sxs_BlockFooter_t) / sizeof (u16)));
                    }
#endif
                }

                FileLen = (u16)(FilesEnd [i] - FilesStart [i]);

                TotalLength += FileLen;

                NewFooter.Id |= FilesStart [i];

                {
                    u16 ExtraLen = FileLen + ZOB(2) + sizeof (sxs_BlockFooter_t);
                    u16 WdLen =  ((ExtraLen + 3) & 0xFFFC) >> 1;
                    ExtraLen  = (WdLen << 1) - ExtraLen;

                    FileLen += FileLen & 1;
                    NewFooter.Id |= SXS_SET_FILE_IDX (i) | SXS_FLH_VALID_ID | SXS_FLH_SET_EXTRA_BYTE(ExtraLen);

                    NewFooter.Length = WdLen;
                }

//    SXS_TRACE (_SXR|TSTDOUT|TNB_ARG(3)|TDB,TSTR("Purge New rcd: Footer.Id %lx Footer.Len %i (Extra %i)\n",0x06b90004), NewFooter.Id, 2*NewFooter.Length, SXS_FLH_GET_EXTRA_BYTE(NewFooter.Id));

                if ((NewBank + NewFooter.Length) > BankDesc -> LastAdd)
                    SXS_RAISE ((_SXR|TABORT|TDB,TSTR("Unable to make a purge, Bank too small",0x06b90005)));

                sxs_FlashWrite (NewBank + NewFooter.Length - (sizeof (sxs_BlockFooter_t) / sizeof (u16)), (u16 *)&NewFooter, sizeof (sxs_BlockFooter_t));
                sxs_FlashWrite (NewBank + 1, (u16 *)Data, FileLen);
                sxs_FlashWrite (NewBank, (u16 *)&NewFooter.Length, sizeof (NewFooter.Length));
                NewBank += NewFooter.Length;

                sxr_Free (Data);
            }
        }

        if (sxs_FlashCtx.PurgeRequired)
            sxs_FlashBlockDelete (0, SXS_FLH_STACKED_BLOCK, FALSE);

        sxs_FlashCtx.DynamicAdd = NewBank;

        sxs_FlashWrite (sxs_FlashCtx.Desc -> DynamicBnk [sxs_FlashCtx.DynamicIdx].StartAdd,
                        (u16 *)&sxs_FlhBankFull,
                        sizeof (sxs_BlockFooter_t));

        sxs_FlashCtx.DynamicIdx = (sxs_FlashCtx.DynamicIdx + 1) & 0x01;

        sxs_FlashCtx.PurgeRequired = FALSE;

        if (Protect)
            sxr_ReleaseSemaphore (sxs_FlashCtx.SemIdx);
    }
    else if ((u16)((u8 *)sxs_FlashCtx.Desc -> StackedBnk.LastAdd - (u8 *)sxs_FlashCtx.StackedAdd)
             < sxs_FlashCtx.Desc -> StackedBnk.PurgeThres)
    {
        if (Protect)
            sxr_TakeSemaphore (sxs_FlashCtx.SemIdx);

        sxs_BankErase (&sxs_FlashCtx.Desc -> StackedBnk);

        if (Protect)
            sxr_ReleaseSemaphore (sxs_FlashCtx.SemIdx);
    }
}
#endif

#ifndef __SXS_FILE_IN_RAM__
/*
==============================================================================
   Function   : sxs_AddressToSector
 -----------------------------------------------------------------------------
   Scope      : Convert an address into correspoding sector address.
   Parameters : Address to be converted.
   Return     : Sector index.
==============================================================================
*/
u8 sxs_AddressToSector (void *Address)
{
    u8 i = 0;

    while ((i < sxs_FlashCtx.Desc -> NbSector)
            &&     ((u16 *)Address >= sxs_FlashCtx.SectorToAdd ((u8)(i+1))))
        i++;

    return i;
}
#endif

#ifndef __SXS_FILE_IN_RAM__
/*
==============================================================================
   Function   : sxs_SectorToAddress
 -----------------------------------------------------------------------------
   Scope      : Convert a sector number to the sector start address.
   Parameters : Sector index.
   Return     : Address.
==============================================================================
*/
void *sxs_SectorToAddress (u8 SectIdx)
{
    return (void *)sxs_FlashCtx.SectorToAdd (SectIdx);
}
#endif

#ifndef __SXS_FILE_IN_RAM__
/*
==============================================================================
   Function   : sxs_SectorErase
 -----------------------------------------------------------------------------
   Scope      : Clear the sector identified by its index.
   Parameters : Sector index.
   Return     : none
==============================================================================
*/
void sxs_SectorErase (u8 SectorIdx)
{
    SXS_FPRINTF ((_SXR|TDB|TNB_ARG(1),TSTR("Erase sector %i\n",0x06b90006), SectorIdx));
    sxr_TakeSemaphore (sxs_FlashCtx.SemIdx);
    sxs_FlashCtx.EraseSector (sxs_FlashCtx.SectorToAdd (SectorIdx));
    sxr_ReleaseSemaphore (sxs_FlashCtx.SemIdx);
}
#endif

#ifndef __SXS_FILE_IN_RAM__
/*
==============================================================================
   Function   : sxs_FlashWrite
 -----------------------------------------------------------------------------
   Scope      : Write generic data in flash.
   Parameters : Pointer to destination address in flash.
                Pointer to data source to be copied.
                Length of the data expressed in bytes.
   Return     : none
==============================================================================
*/
void sxs_FlashWrite (u16 *Dest, u16 *Src, u16 Length)
{
    u16 i;
    u32 Status;
    u16 Data;

// SXS_TRACE (_SXR|TSTDOUT|TDB|TNB_ARG(3),TSTR("Flash Write %i bytes from 0x%x at 0x%x\n",0x06b90007), Length, Src, Dest);
// SXS_DUMP (_SXR, 0, (u8 *)Src, Length);

    for (i=0; i<Length / 2; i++)
    {
        Data = *(((u8 *)Src) + 2*i) | (*(((u8 *)Src) + 2*i + 1) << 8);
        Status = sxr_EnterSc ();
        sxs_FlashCtx.Write (Dest + i, Data);
        sxr_ExitSc (Status);
    }
}
#endif

/*
==============================================================================
   Function   : sxs_FlashBlockWrite
 -----------------------------------------------------------------------------
   Scope      : Write a block of data in flash. The block is identified thanks
                to an Id. A footer is added at the end of the block.
   Parameters : Block Id
                Pointer on data to be copied.
                Length of data to be copied expressed in bytes.
   Return     : TRUE when the copy occured, FALSE otherwise.
==============================================================================
*/
u8 sxs_FlashBlockWrite (u32 Id, u8 *Data, u16 Length)
{
#ifndef __SXS_FILE_IN_RAM__
    u16 WdSize; // = ((Length + 1) >> 1) + 1 + (sizeof (sxs_BlockFooter_t) / sizeof (u16)); //2;
    sxs_BankDesc_t const *Bank;
    sxs_BlockFooter_t Footer;
    u16 **BankAdd;
    u16  *FlhAdd;
    u16  ExtraLen = Length + ZOB(2) + sizeof (sxs_BlockFooter_t);
    WdSize = ((ExtraLen + 3) & 0xFFFC) >> 1;
    ExtraLen = (WdSize << 1) - ExtraLen;

    sxr_TakeSemaphore (sxs_FlashCtx.SemIdx);

// SXS_TRACE (_SXR|TSMAP(4)|TSTDOUT|TNB_ARG(3)|TDB,TSTR("Id %lx Len %i (%s)\n",0x06b90008), Id, Length, Data);
// SXS_DUMP (_SXR|TSTDOUT, 0, Data, Length);

    Id |= SXS_FLH_VALID_ID;

#if 0
    if (Id & SXS_FLH_STATIC_BLOCK)
    {
        Bank = &sxs_FlashCtx.Desc -> StaticBnk;
        BankAdd = &sxs_FlashCtx.StaticAdd;
        FlhAdd  = sxs_FlashCtx.StaticAdd;
    }
    else
#endif

        Bank = &sxs_FlashCtx.Desc -> DynamicBnk [sxs_FlashCtx.DynamicIdx];
    BankAdd = &sxs_FlashCtx.DynamicAdd;
    FlhAdd  = sxs_FlashCtx.DynamicAdd;

    if ((Id & SXS_FLH_STACKED_BLOCK)
            ||  (FlhAdd + WdSize >= Bank -> LastAdd)) // >= Bank -> Size))
    {
        Bank = &sxs_FlashCtx.Desc -> StackedBnk;
        BankAdd = &sxs_FlashCtx.StackedAdd;
        FlhAdd  = sxs_FlashCtx.StackedAdd;
        sxs_FlashCtx.PurgeRequired = !(Id & SXS_FLH_STACKED_BLOCK);
    }

    if (WdSize > Bank -> Size)
        SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(2),TSTR("Bank %lx too small for write %i\n",0x06b90009), Bank, Length));

    if (FlhAdd + WdSize < Bank -> LastAdd) // < Bank -> Size)
    {
        FlhAdd = *BankAdd;
        *BankAdd += WdSize;

//  Footer.Id = Length & 1 ? Id | SXS_FLH_EXTRA_BYTE : Id;
        Footer.Id = Id | SXS_FLH_SET_EXTRA_BYTE (ExtraLen);
        Footer.Length = WdSize;

//  SXS_TRACE (_SXR|TSTDOUT|TNB_ARG(3)|TDB,TSTR("Footer.Id %lx Footer.Len %i (Extra %i)\n",0x06b9000a), Footer.Id, 2*Footer.Length, ExtraLen);

        sxs_FlashWrite (FlhAdd + WdSize - (sizeof (sxs_BlockFooter_t) / sizeof (u16)), (u16 *)&Footer, sizeof (sxs_BlockFooter_t));
        sxs_FlashWrite (FlhAdd + 1, (u16 *)Data, (u16)((Length + 1) & 0xFFFE));
        sxs_FlashWrite (FlhAdd, (u16 *)&Footer.Length, sizeof (Footer.Length));
    }
    else
    {
        sxr_ReleaseSemaphore (sxs_FlashCtx.SemIdx);
        return FALSE;
    }

    sxr_ReleaseSemaphore (sxs_FlashCtx.SemIdx);
    return TRUE;
#else
    u32 Offset  = Id & SXS_OFFSET_MSK;
    u8  FileIdx = SXS_GET_FILE_IDX(Id);
    u8 *Add = sxs_RamFile + sxs_FileOffset [FileIdx] + 8;
    u32 *End = (u32 *)(Add - 4);

    if (*End < (Offset + Length))
    {
        if ((Offset + Length + 8) > sxs_FileMaxSize [FileIdx])
            return FALSE;

        *End = Offset + Length;
    }

    memcpy (Add + Offset, Data, Length);

    return TRUE;

#endif
}

#ifndef __SXS_FILE_IN_RAM__
/*
==============================================================================
   Function   : sxs_BankBlockRead
 -----------------------------------------------------------------------------
   Scope      : Read a block of data identified by its Id in the required bank.
   Parameters : Block Id
                Pointer to a buffer where data may be copied.
                Length of data to be retreived.
                Bank address.
   Return     : TRUE when part or whole data found, FALSE otherwise.
==============================================================================
*/
u8 sxs_BankBlockRead (u32 Id, u8 *Data, u16 Length, u16 *BankAdd)
{
    sxs_BlockFooter_t *Footer = (sxs_BlockFooter_t *)(BankAdd + *BankAdd - (sizeof (sxs_BlockFooter_t) / sizeof (u16)));
    u8 Cnt = 0;

    while ((*BankAdd != 0) && (*BankAdd != 0xFFFF) && (Footer -> Length == *BankAdd))
    {
        if (((Id & SXS_FILE_IDX_MSK) | SXS_FLH_VALID_ID) == (Footer -> Id & (SXS_FILE_IDX_MSK | SXS_FLH_VALID_ID)))
        {
            u32 RcdOffset = Footer -> Id & SXS_OFFSET_MSK;
            u32 RcdLen    = (2*Footer -> Length) - sizeof (sxs_BlockFooter_t) - ZOB(2);
            u32 ReqOffset = Id & SXS_OFFSET_MSK;

//   if (Footer -> Id & SXS_FLH_EXTRA_BYTE)
//    RcdLen -= 1;
            RcdLen -= SXS_FLH_GET_EXTRA_BYTE(Footer -> Id);

            if (((RcdOffset + RcdLen) > ReqOffset)
                    &&  (RcdOffset <= ReqOffset))
            {
                memcpy (Data,
                        ((u8 *)BankAdd) + 2 + ReqOffset - RcdOffset,
                        MIN (Length, ReqOffset + RcdLen - RcdOffset));
                Cnt ++;
            }
            else if (((ReqOffset + Length) > RcdOffset)
                     &&  (ReqOffset <= RcdOffset))
            {
                memcpy (Data + RcdOffset - ReqOffset,
                        ((u8 *)BankAdd) + 2,
                        MIN (RcdLen, ReqOffset + Length - RcdOffset));
                Cnt ++;
            }
        }
        BankAdd += *BankAdd;

        Footer = (sxs_BlockFooter_t *)(BankAdd + *BankAdd - (sizeof (sxs_BlockFooter_t) / sizeof (u16)));
    }
    return Cnt;
}
#endif


/*
==============================================================================
   Function   : sxs_FlashBlockRead
 -----------------------------------------------------------------------------
   Scope      : Read a block of data identified by its Id.
   Parameters : Block Id
                Pointer to a buffer where data may be copied.
                Length of data to be retreived.
   Return     : Pointer to the address where required data block starts or
                NIL when no data found.
==============================================================================
*/
u8 *sxs_FlashBlockRead (u32 Id, u8 *Data, u16 Length)
{
#ifndef __SXS_FILE_IN_RAM__
    u8 Cnt = 0;
    sxr_TakeSemaphore (sxs_FlashCtx.SemIdx);

    if (Id & SXS_FLH_STACKED_BLOCK)
    {
        Cnt += sxs_BankBlockRead (Id, Data, Length,
                                  sxs_FlashCtx.Desc -> StackedBnk.StartAdd + (sizeof (sxs_BlockFooter_t) / sizeof (u16)));
    }
    else
    {
#ifndef __SXS_FLH_NO_STATIC__
        Cnt += sxs_BankBlockRead (Id, Data, Length,
                                  sxs_FlashCtx.Desc -> StaticBnk.StartAdd + (sizeof (sxs_BlockFooter_t) / sizeof (u16)));
#endif
        Cnt += sxs_BankBlockRead (Id, Data, Length,
                                  sxs_FlashCtx.Desc -> DynamicBnk [sxs_FlashCtx.DynamicIdx].StartAdd + (sizeof (sxs_BlockFooter_t) / sizeof (u16)));

        if (sxs_FlashCtx.PurgeRequired)
            Cnt += sxs_BankBlockRead (Id, Data, Length,
                                      sxs_FlashCtx.Desc -> StackedBnk.StartAdd + (sizeof (sxs_BlockFooter_t) / sizeof (u16)));
    }

    sxr_ReleaseSemaphore (sxs_FlashCtx.SemIdx);

    if (Cnt)
        return Data;
    else
        return NIL;
#else
    u32 Offset  = Id & SXS_OFFSET_MSK;
    u8  FileIdx = SXS_GET_FILE_IDX(Id);
    u8 *Ret = sxs_RamFile + sxs_FileOffset [FileIdx] + Offset + 8;

    memcpy (Data, Ret, Length);

    return Ret;
#endif
}



#ifndef __SXS_FILE_IN_RAM__
/*
==============================================================================
   Function   : sxs_GetFileBankLimit
 -----------------------------------------------------------------------------
   Scope      : Get all the existing files limit in the specified bank.
              : Found start offset and end offset are store in provided buffers.
   Parameters : Pointer on buffer where Start offset are to be stored.
                Pointer on buffer where End offset are to be stored.
   Parameters : none
   Return     : none
==============================================================================
*/
void sxs_GetFileBankLimit (u32 *Start, u32 *End, u16 *BankAdd)
{
    sxs_BlockFooter_t *Footer = (sxs_BlockFooter_t *)(BankAdd + *BankAdd - (sizeof (sxs_BlockFooter_t) / sizeof (u16)));

    while ((*BankAdd != 0) && (*BankAdd != 0xFFFF) && (Footer -> Length == *BankAdd))
    {
        if (Footer -> Id & SXS_FLH_VALID_ID)
        {
            if (Start [SXS_GET_FILE_IDX(Footer -> Id)] > (Footer -> Id & SXS_OFFSET_MSK))
                Start [SXS_GET_FILE_IDX(Footer -> Id)] = Footer -> Id & SXS_OFFSET_MSK;

//   if (End [SXS_GET_FILE_IDX(Footer -> Id)] < ((Footer -> Id & SXS_OFFSET_MSK) + (2*Footer -> Length - (Footer -> Id & SXS_FLH_EXTRA_BYTE ? 1 : 0) - sizeof (sxs_BlockFooter_t) - 2)))
//    End [SXS_GET_FILE_IDX(Footer -> Id)] = (Footer -> Id & SXS_OFFSET_MSK) + 2*Footer -> Length - (Footer -> Id & SXS_FLH_EXTRA_BYTE ? 1 : 0) - sizeof (sxs_BlockFooter_t) - 2;
            if (End [SXS_GET_FILE_IDX(Footer -> Id)] < ((Footer -> Id & SXS_OFFSET_MSK) + (2*Footer -> Length - (SXS_FLH_GET_EXTRA_BYTE (Footer -> Id)) - sizeof (sxs_BlockFooter_t) - ZOB(2))))
                End [SXS_GET_FILE_IDX(Footer -> Id)] = (Footer -> Id & SXS_OFFSET_MSK) + 2*Footer -> Length - (SXS_FLH_GET_EXTRA_BYTE (Footer -> Id)) - sizeof (sxs_BlockFooter_t) - ZOB(2);
        }

        BankAdd += *BankAdd;

        Footer = (sxs_BlockFooter_t *)(BankAdd + *BankAdd - (sizeof (sxs_BlockFooter_t) / sizeof (u16)));
    }
}
#endif

/*
==============================================================================
   Function   : sxs_GetFilesLimit
 -----------------------------------------------------------------------------
   Scope      : Retreive all the existing files limits. Found start offset and
                end offset are store in provided buffers.
   Parameters : Pointer on buffer where Start offset are to be stored.
                Pointer on buffer where End offset are to be stored.
   Return     : none
==============================================================================
*/
void sxs_GetFilesLimit (u32 *Start, u32 *End)
{
    memset ((u8 *)Start, 0xFF, sizeof (u32) * SXS_FLH_NB_FILES_MAX);
    memset ((u8 *)End, 0, sizeof (u32) * SXS_FLH_NB_FILES_MAX);

#ifndef __SXS_FILE_IN_RAM__
#ifndef __SXS_FLH_NO_STATIC__
    sxs_GetFileBankLimit (Start, End, sxs_FlashCtx.Desc -> StaticBnk.StartAdd + (sizeof (sxs_BlockFooter_t) / sizeof (u16)));
#endif
    sxs_GetFileBankLimit (Start, End, sxs_FlashCtx.Desc -> DynamicBnk [sxs_FlashCtx.DynamicIdx].StartAdd + (sizeof (sxs_BlockFooter_t) / sizeof (u16)));
    sxs_GetFileBankLimit (Start, End, sxs_FlashCtx.Desc -> StackedBnk.StartAdd + (sizeof (sxs_BlockFooter_t) / sizeof (u16)));
#else
    {
        u32 i;
        u32 *Limit;

        for (i=0; i<SXS_FLH_NB_FILES_MAX; i++)
        {
            Limit = (u32 *)(sxs_RamFile + sxs_FileOffset [i]);
            Start [i] = *Limit;
            Limit++;
            End [i] = *Limit;
        }
    }
#endif
}


/*
==============================================================================
   Function   : sxs_FlashBlockDelete
 -----------------------------------------------------------------------------
   Scope      : Delete the blocks identified by the couple (Id, Msk)
   Parameters : Id, mask
                Boolean Protect: when FALSE means that access to the flash
                is already protected against concurent access.
   Return     : none
==============================================================================
*/
void sxs_FlashBlockDelete (u32 Id, u32 Msk, u8 Protect)
{
#ifndef __SXS_FILE_IN_RAM__
    u16 *BankAdd [3];
    sxs_BlockFooter_t *Footer;
    u32 i, FirstBnk, LastBnk;
    u32  IdReset = 0;

    if (Protect)
        sxr_TakeSemaphore (sxs_FlashCtx.SemIdx);

    Id  |= SXS_FLH_VALID_ID;
    Msk |= SXS_FLH_VALID_ID;
    Msk &= ~SXS_FLH_EXTRA_BYTE_MSK;

#ifndef __SXS_FLH_NO_STATIC__
    BankAdd [0] = sxs_FlashCtx.Desc -> StaticBnk.StartAdd + (sizeof (sxs_BlockFooter_t) / sizeof (u16));
#endif
    BankAdd [1] = sxs_FlashCtx.Desc -> DynamicBnk [sxs_FlashCtx.DynamicIdx].StartAdd + (sizeof (sxs_BlockFooter_t) / sizeof (u16));
    BankAdd [2] = sxs_FlashCtx.Desc -> StackedBnk.StartAdd + (sizeof (sxs_BlockFooter_t) / sizeof (u16));

    FirstBnk = Id & SXS_FLH_STATIC_BLOCK ? 0 : Id & SXS_FLH_STACKED_BLOCK ? 2 : 1;
    LastBnk  = Id & SXS_FLH_STACKED_BLOCK ? 2 : sxs_FlashCtx.PurgeRequired ? 2 : 1;


    for (i=FirstBnk; i<=LastBnk; i++)
    {
        Footer = (sxs_BlockFooter_t *)(BankAdd [i] + *BankAdd [i] - (sizeof (sxs_BlockFooter_t) / sizeof (u16)));

        while ((*BankAdd [i] != 0) && (*BankAdd [i] != 0xFFFF) && (Footer -> Length == *BankAdd [i]))
        {
            if ((Footer -> Id & Msk) == Id)
                sxs_FlashWrite ((void *)(&Footer -> Id), (void *)&IdReset, sizeof (Footer -> Id));

            BankAdd [i] += *BankAdd [i];

            Footer = (sxs_BlockFooter_t *)(BankAdd [i] + *BankAdd [i] - (sizeof (sxs_BlockFooter_t) / sizeof (u16)));
        }
    }

    if (Protect)
        sxr_ReleaseSemaphore (sxs_FlashCtx.SemIdx);
#else
    u8 FileIdx = SXS_GET_FILE_IDX(Id);

    memset (sxs_RamFile + sxs_FileOffset [FileIdx], 0, sxs_FileMaxSize [FileIdx]);
#endif
}


#ifndef __SXS_FILE_IN_RAM__
/*
==============================================================================
   Function   :
 -----------------------------------------------------------------------------
   Scope      :
   Parameters : none
   Return     : none
==============================================================================
*/
#ifndef __SXS_FLH_NO_STATIC__
void sxs_UpdateStaticBank (void)
{
    sxr_TakeSemaphore (sxs_FlashCtx.SemIdx);

    sxs_FlashCtx.PurgeRequired = TRUE;
    sxs_FlashPurge (FALSE, TRUE);

    sxs_BankErase (&sxs_FlashCtx.Desc -> StaticBnk);
    sxs_FlashCtx.StaticAdd = sxs_FlashCtx.Desc -> StaticBnk.StartAdd + (sizeof (sxs_BlockFooter_t) / sizeof (u16));

    {
        u16 *BankAdd = sxs_FlashCtx.Desc -> DynamicBnk [sxs_FlashCtx.DynamicIdx].StartAdd + (sizeof (sxs_BlockFooter_t) / sizeof (u16));
        sxs_BlockFooter_t *Footer = (sxs_BlockFooter_t *)(BankAdd + *BankAdd - (sizeof (sxs_BlockFooter_t) / sizeof (u16)));

        while ((*BankAdd != 0) && (*BankAdd != 0xFFFF) && (Footer -> Length == *BankAdd))
        {
            if ((Footer -> Id & (SXS_FLH_VALID_ID | SXS_FLH_PROTECTED_BLOCK)) == (SXS_FLH_VALID_ID | SXS_FLH_PROTECTED_BLOCK))
            {
                sxs_FlashWrite (sxs_FlashCtx.StaticAdd, BankAdd, (u16)(Footer -> Length * 2));
                sxs_FlashCtx.StaticAdd += Footer -> Length;
                sxs_FlashBlockDelete (Footer -> Id, 0xFFFFFFFF, FALSE);
            }
            BankAdd += *BankAdd;

            Footer = (sxs_BlockFooter_t *)(BankAdd + *BankAdd - (sizeof (sxs_BlockFooter_t) / sizeof (u16)));
        }
    }

    sxr_ReleaseSemaphore (sxs_FlashCtx.SemIdx);
}
#endif
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
