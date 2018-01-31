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

#include "mcd_config.h"
#include "tgt_mcd_cfg.h"

#include "mcd_m.h"
#include "mcd_sdmmc.h"
#include "mcd_spi.h"
#include "mcdp_debug.h"
#include "mcdp_sdmmc.h"
#include "pmd_m.h"
#include "hal_sdmmc.h"
#include "hal_sys.h"

#include "stdlib.h"
#include "stdio.h"
#include "sxr_tls.h"

/*
#include "ifc_service.h"
#include "lp_timer_service.h"
*/


#include "sxs_io.h"
#include "sxr_sbx.h"

extern BOOL hal_HstSendEvent(UINT32 ch);
extern u8 sxr_NewSemaphore (u8 InitValue);
extern void sxr_TakeSemaphore (u8 Id);
extern void sxr_ReleaseSemaphore (u8 Id);

PRIVATE MCD_CARD_ID g_mcd_id = MCD_CARD_ID_0;
#ifdef DUAL_TFLASH_SUPPORT
PRIVATE UINT8 g_IsOpen[2] = {0,0};
#else
PRIVATE UINT8 g_IsOpen = 0;
#endif

PRIVATE UINT32 g_hMcdSem = 0;

PUBLIC MCD_CARD_DETECT_HANDLER_T   g_mcdCardDetectHandler=NULL;

PRIVATE VOID mcd_WaitSemTFlash(VOID)
{
    if (0 == g_hMcdSem)
    {
        g_hMcdSem = sxr_NewSemaphore(1);
    }
    if(g_hMcdSem > 0)
    {
        sxr_TakeSemaphore(g_hMcdSem);
    }

}


PRIVATE VOID mcd_ReleaseSemTFlash(VOID)
{
    if(g_hMcdSem > 0)
    {
        sxr_ReleaseSemaphore(g_hMcdSem);
    }

}

// =============================================================================
//  Functions
// =============================================================================

// =============================================================================
// mcd_SetCardDetectHandler
// -----------------------------------------------------------------------------
/// Register a handler for card detection
///
/// @param handler function called when insertion/removal is detected.
// =============================================================================
PUBLIC MCD_ERR_T mcd_SetCardDetectHandler(MCD_CARD_DETECT_HANDLER_T handler)
{
    MCD_ERR_T mcd_err;

    mcd_WaitSemTFlash();
    mcd_err = mcd_sdmmc_SetCardDetectHandler(handler);
    mcd_ReleaseSemTFlash();
    return mcd_err;
}

// =============================================================================
// mcd_Open
// -----------------------------------------------------------------------------
/// Open the MMC/SD chip
/// This function does the init process of the MMC chip, including reseting
/// the chip, sending a command of init to MMC, and reading the CSD
/// configurations.
///
/// @param mcdId is mcd device identification under dual t-flash mode.value is 0 or 1.
///
/// @param mcdCsd Pointer to the structure where the MMC CSD register info
/// are going to be written.
///
/// @param mcdVer is t card version.
// =============================================================================
#ifdef DUAL_TFLASH_SUPPORT
PUBLIC MCD_ERR_T mcd_Open(MCD_CARD_ID mcdId,MCD_CSD_T* mcdCsd, MCD_CARD_VER mcdVer)
{
    MCD_CONFIG_STRUCT_T* mcd_config = NULL;
    UINT32 config_id = 0;
    MCD_ERR_T mcd_err = MCD_ERR_NO;
    hal_HstSendEvent(0xa30000a0);
    mcd_config = (MCD_CONFIG_STRUCT_T*)tgt_GetMcdConfig();
    config_id = (mcdId == MCD_CARD_ID_0) ? 0:1;
    mcd_WaitSemTFlash();
    g_mcd_id = mcdId;
    hal_HstSendEvent(0xa40000a0);
    if(MCD_CONFIG_CARD_TYPE_SDMMC == mcd_config->mcd_if[config_id].type)
    {
        if(0 == g_IsOpen[config_id])
        {
            hal_HstSendEvent(0xa10000a0);
            mcd_err = mcd_sdmmc_Open(mcdCsd,mcdVer,&mcd_config->mcd_if[config_id]);
            //    hal_HstSendEvent(0xa100a000 | mcd_err);
            MCD_TRACE(MCD_INFO_TRC, 0, "mcd_Open(%d):(sdmmc)mcd_err =  %d.",mcdId,mcd_err);
        }

    }
    else if(MCD_CONFIG_CARD_TYPE_SPI == mcd_config->mcd_if[config_id].type)
    {
        if(0 == g_IsOpen[config_id])
        {
            hal_HstSendEvent(0xa20000a0);
            mcd_err = mcd_spi_Open(mcdCsd,mcdVer,&mcd_config->mcd_if[config_id]);

            //hal_HstSendEvent(0xa200a000 | mcd_err);
            MCD_TRACE(MCD_INFO_TRC, 0, "mcd_Open(%d):(spi)mcd_err =  %d.",mcdId,mcd_err);
        }
    }
    else
    {
        mcd_err = MCD_ERR_NO_CARD;
    }

    if(MCD_ERR_NO == mcd_err)
    {
        g_IsOpen[config_id] = 1;
    }

    MCD_TRACE(MCD_INFO_TRC, 0, "mcd_Open():config_id = %d,mcd_err =  %d,mcdVer = %d.",config_id,mcd_err,mcdVer);
    mcd_ReleaseSemTFlash();

    return mcd_err;
}

#else

PUBLIC MCD_ERR_T mcd_Open(MCD_CSD_T* mcdCsd, MCD_CARD_VER mcdVer)
{
    MCD_CONFIG_STRUCT_T* mcd_config = NULL;
    UINT32 config_id = 0;
    MCD_ERR_T mcd_err = MCD_ERR_NO;

    mcd_config = (MCD_CONFIG_STRUCT_T*)tgt_GetMcdConfig();

    mcd_WaitSemTFlash();
    if(MCD_CONFIG_CARD_TYPE_SDMMC == mcd_config->mcd_if[config_id].type)
    {
        if(0 == g_IsOpen)
        {
            //     hal_HstSendEvent(0xa10000a0);
            mcd_err = mcd_sdmmc_Open(mcdCsd,mcdVer,&mcd_config->mcd_if[config_id]);
            //      hal_HstSendEvent(0xa100a000 | mcd_err);
        }
    }
    else if(MCD_CONFIG_CARD_TYPE_SPI == mcd_config->mcd_if[config_id].type)
    {
        if(0 == g_IsOpen)
        {
            //         hal_HstSendEvent(0xa20000a0);
            mcd_err = mcd_spi_Open(mcdCsd,mcdVer,&mcd_config->mcd_if[config_id]);
            //         hal_HstSendEvent(0xa200a000 | mcd_err);
        }
    }
    else
    {
        mcd_err = MCD_ERR_NO_CARD;
    }

    if(MCD_ERR_NO == mcd_err)
    {
        g_IsOpen = 1;
    }
    MCD_TRACE(MCD_INFO_TRC, 0, "mcd_Open():config_id = %d,mcd_err =  %d.",config_id,mcd_err);
    mcd_ReleaseSemTFlash();
    return mcd_err;
}

#endif
// =============================================================================
// mcd_Close
// -----------------------------------------------------------------------------
/// Close MCD.
///
/// To be called at the end of the operations
/// @param mcdId is mcd device identification under dual t-flash mode.value is 0 or 1.
///
/// @return MCD_ERR_NO if a response with a good crc was received,
///         MCD_ERR_CARD_NO_RESPONSE if no reponse was received within the
/// driver configured timeout.
///          MCD_ERR_CARD_RESPONSE_BAD_CRC if the received response presented
///  a bad CRC.
///         MCD_ERR_CARD_TIMEOUT if the card timedout during procedure.
// =============================================================================

#ifdef DUAL_TFLASH_SUPPORT

PUBLIC MCD_ERR_T mcd_Close(MCD_CARD_ID mcdId)
{
    MCD_CONFIG_STRUCT_T* mcd_config = NULL;
    UINT32 config_id = 0;
    //MCD_ERR_T mcd_err[2]= {MCD_ERR_NO,MCD_ERR_NO};
    MCD_ERR_T mcd_err = MCD_ERR_NO;

    mcd_config = (MCD_CONFIG_STRUCT_T*)tgt_GetMcdConfig();
    config_id = mcdId == MCD_CARD_ID_0 ? 0:1;

    mcd_WaitSemTFlash();

    if(MCD_CONFIG_CARD_TYPE_SDMMC == mcd_config->mcd_if[config_id].type)
    {
        if(1 == g_IsOpen[config_id])
        {
            // hal_HstSendEvent(0xb10000b0);
            mcd_err = mcd_sdmmc_Close();
            // hal_HstSendEvent(0xb100b000 | mcd_err);
            MCD_TRACE(MCD_INFO_TRC, 0, "mcd_Close(%d):(sdmmc)mcd_err =  %d.",mcdId,mcd_err);
        }
        else
        {
            // hal_HstSendEvent(0xb10000b1);
            MCD_TRACE(MCD_INFO_TRC, 0, "mcd_Close(%d):g_IsOpen[%d] == 0 ",mcdId,config_id);
        }
    }
    else if(MCD_CONFIG_CARD_TYPE_SPI == mcd_config->mcd_if[config_id].type)
    {
        if(1 == g_IsOpen[config_id])
        {
            // hal_HstSendEvent(0xb20000b0);
            mcd_err =  mcd_spi_Close();
            // hal_HstSendEvent(0xb200b000 | mcd_err);
            MCD_TRACE(MCD_INFO_TRC, 0, "mcd_Close(%d):(spi)mcd_err =  %d.",mcdId,mcd_err);
        }
        else
        {
            // hal_HstSendEvent(0xb20000b1);
            MCD_TRACE(MCD_INFO_TRC, 0, "mcd_Close(%d):g_IsOpen[%d] == 0 ",mcdId,config_id);
        }
    }
    else
    {
        mcd_err = MCD_ERR_NO_CARD;
    }
    if(mcd_err == MCD_ERR_NO)
    {
        g_IsOpen[config_id] = 0;
    }
    mcd_ReleaseSemTFlash();

    return mcd_err;//mcd_err[config_id];

}

#else

PUBLIC MCD_ERR_T mcd_Close(VOID)
{
    MCD_CONFIG_STRUCT_T* mcd_config = NULL;
    UINT32 config_id = 0;
    MCD_ERR_T mcd_err = MCD_ERR_NO;

    mcd_config = (MCD_CONFIG_STRUCT_T*)tgt_GetMcdConfig();

    mcd_WaitSemTFlash();
    if(MCD_CONFIG_CARD_TYPE_SDMMC == mcd_config->mcd_if[config_id].type)
    {
        if(1 == g_IsOpen)
        {
            mcd_err = mcd_sdmmc_Close();
        }
    }
    else if(MCD_CONFIG_CARD_TYPE_SPI == mcd_config->mcd_if[config_id].type)
    {
        if(1 == g_IsOpen)
        {
            mcd_err = mcd_spi_Close();
        }
    }
    else
    {
        mcd_err = MCD_ERR_NO_CARD;
    }

    if(MCD_ERR_NO == mcd_err)
    {
        g_IsOpen = 0;
    }
    MCD_TRACE(MCD_INFO_TRC, 0, "mcd_Close():config_id = %d,mcd_err =  %d.",config_id,mcd_err);
    mcd_ReleaseSemTFlash();
    return mcd_err;
}
#endif


// =============================================================================
// mcd_CardStatus
// -----------------------------------------------------------------------------
/// Return the card status
///
/// @return Card status see #MCD_STATUS_T
// =============================================================================
#ifdef DUAL_TFLASH_SUPPORT
PUBLIC MCD_STATUS_T mcd_CardStatus(MCD_CARD_ID mcdId)
{
    MCD_CONFIG_STRUCT_T* mcd_config = NULL;
    UINT32 config_id = 0;
    MCD_STATUS_T mcd_status;

    mcd_config = (MCD_CONFIG_STRUCT_T*)tgt_GetMcdConfig();
    mcd_WaitSemTFlash();
    config_id = mcdId == MCD_CARD_ID_0 ? 0:1;

    if(MCD_CONFIG_CARD_TYPE_SDMMC == mcd_config->mcd_if[config_id].type)
    {
        mcd_status =  mcd_sdmmc_CardStatus();
    }
    else if(MCD_CONFIG_CARD_TYPE_SPI == mcd_config->mcd_if[config_id].type)
    {
        mcd_status = mcd_spi_CardStatus();
    }
    else
    {
        mcd_status = MCD_STATUS_NOTPRESENT;
    }
    mcd_ReleaseSemTFlash();

    return mcd_status;
}
#else
PUBLIC MCD_STATUS_T mcd_CardStatus(MCD_CARD_ID mcdId)
{
    MCD_CONFIG_STRUCT_T* mcd_config = NULL;
    UINT32 config_id = 0;
    MCD_STATUS_T mcd_status;

    mcd_config = (MCD_CONFIG_STRUCT_T*)tgt_GetMcdConfig();
    mcd_WaitSemTFlash();
    config_id = g_mcd_id == MCD_CARD_ID_0 ? 0:1;

    if(MCD_CONFIG_CARD_TYPE_SDMMC == mcd_config->mcd_if[config_id].type)
    {
        mcd_status =  mcd_sdmmc_CardStatus();
    }
    else if(MCD_CONFIG_CARD_TYPE_SPI == mcd_config->mcd_if[config_id].type)
    {
        mcd_status = mcd_spi_CardStatus();
    }
    else
    {
        mcd_status = MCD_ERR_NO_CARD;
    }
    mcd_ReleaseSemTFlash();

    return mcd_status;
}
#endif

// =============================================================================
// mcd_GetCardSize
// -----------------------------------------------------------------------------
/// Get card size
///
/// @param size Structure use to store size of memory card
// =============================================================================
PUBLIC MCD_ERR_T mcd_GetCardSize(MCD_CARD_SIZE_T* size)
{
    MCD_CONFIG_STRUCT_T* mcd_config = NULL;
    MCD_CARD_SIZE_T mcd_size = {0,0};
    UINT32 config_id = 0;
    MCD_ERR_T mcd_err = MCD_ERR_NO;

    mcd_config = (MCD_CONFIG_STRUCT_T*)tgt_GetMcdConfig();

    mcd_WaitSemTFlash();
    config_id = g_mcd_id == MCD_CARD_ID_0 ? 0:1;

    if(MCD_CONFIG_CARD_TYPE_SDMMC == mcd_config->mcd_if[config_id].type)
    {
        mcd_err = mcd_sdmmc_GetCardSize(&mcd_size);
        // hal_HstSendEvent(0xba000000 | mcd_err);
        // hal_HstSendEvent(size->nbBlock);
    }
    else if(MCD_CONFIG_CARD_TYPE_SPI == mcd_config->mcd_if[config_id].type)
    {
        mcd_err = mcd_spi_GetCardSize(&mcd_size);

        // hal_HstSendEvent(0xbb000000 | mcd_err);
        // hal_HstSendEvent(size->nbBlock);
    }
    else
    {
        mcd_err = MCD_ERR_NO_CARD;
    }
    size->nbBlock = ((mcd_size.nbBlock/512)*mcd_size.blockLen);
    size->blockLen = mcd_size.blockLen > 0 ?512:0;
    MCD_TRACE(MCD_INFO_TRC, 0, "mcd_GetCardSize():config_id = %d,mcd_err =  %d,blk_nr = 0x%x,blk_sz = 0x%x.",config_id,mcd_err,size->nbBlock,size->blockLen);
    mcd_ReleaseSemTFlash();

    return mcd_err;
}

// =============================================================================
// mcd_Write
// -----------------------------------------------------------------------------
/// Write a block of data to MMC.
///
/// This function is used to write blocks of data on the MMC.
/// @param startAddr Start Adress  of the MMC memory block where the
/// data will be written
/// @param blockWr Pointer to the block of data to write. Must be aligned
/// on a 32 bits boundary.
/// @param size Number of bytes to write. Must be an interger multiple of the
/// sector size of the card.
// =============================================================================
PUBLIC MCD_ERR_T mcd_Write(UINT32 startAddr, CONST UINT8* blockWr, UINT32 size)
{
    MCD_CONFIG_STRUCT_T* mcd_config = NULL;
    UINT32 config_id = 0;
    UINT32 i;
    MCD_ERR_T mcd_err = MCD_ERR_NO;

    mcd_config = (MCD_CONFIG_STRUCT_T*)tgt_GetMcdConfig();
    mcd_WaitSemTFlash();
    config_id = g_mcd_id == MCD_CARD_ID_0 ? 0:1;

    if(0 ==  g_IsOpen[config_id] )
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "mcd_Write:g_IsOpen is 0,addr = 0x%x.",startAddr);
        mcd_ReleaseSemTFlash();
        return MCD_ERR_NO_CARD;
    }

    if(MCD_CONFIG_CARD_TYPE_SDMMC == mcd_config->mcd_if[config_id].type)
    {
        mcd_err = mcd_sdmmc_Write(startAddr, blockWr, size);
        MCD_TRACE(MCD_INFO_TRC, 0, "mcd_Write:mult-block,config_id = %d,mcd_err =  %d,startAddr = 0x%x,size = 0x%x.",config_id,mcd_err,startAddr,size);
    }
    else if(MCD_CONFIG_CARD_TYPE_SPI == mcd_config->mcd_if[config_id].type)
    {
#if 0
        for(i = 0;  i < (size/MCD_DEFUALT_BLOCK_SIZE); i++)
        {
            hal_HstSendEvent(0xb0000000 | (startAddr + i*MCD_DEFUALT_BLOCK_SIZE));
            //  mcd_err = mcd_sdmmc_Write(startAddr, blockWr, size);
            mcd_err = mcd_spi_Write((startAddr + i), (blockWr + i*MCD_DEFUALT_BLOCK_SIZE), MCD_DEFUALT_BLOCK_SIZE);
            if(MCD_ERR_NO != mcd_err)
            {
                break;

            }
            MCD_TRACE(MCD_INFO_TRC, 0, "mcd_Write:sigle-block,config_id = %d,mcd_err =  %d,startAddr = 0x%x,512.",config_id,mcd_err,(startAddr + i*MCD_DEFUALT_BLOCK_SIZE));
        }
#else
        mcd_err = mcd_spi_Write(startAddr, blockWr, size);
        MCD_TRACE(MCD_INFO_TRC, 0, "mcd_spi_Write:mult-block,config_id = %d,mcd_err =  %d,startAddr = 0x%x,size = 0x%x.",config_id,mcd_err,startAddr,size);

#endif
    }
    else
    {
        mcd_err = MCD_ERR_NO_CARD;
    }

    if(MCD_ERR_NO != mcd_err)
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "mcd_Write:sigle-block,config_id = %d,mcd_err =  %d,startAddr = 0x%x,size = 0x%x.",config_id,mcd_err,startAddr,size);
        // hal_HstSendEvent(0xb00000e1);
        // hal_HstSendEvent(mcd_err);
    }
    mcd_ReleaseSemTFlash();

    return mcd_err;
}


// =============================================================================
// mcd_Read
// -----------------------------------------------------------------------------
/// Read using pattern mode.
/// @param startAddr: of the MMC memory block where the data
/// will be read
/// @param blockRd Pointer to the buffer where the data will be stored. Must be aligned
/// on a 32 bits boundary.
/// @param size Number of bytes to read. Must be an interger multiple of the
/// sector size of the card.
// =============================================================================
PUBLIC MCD_ERR_T mcd_Read(UINT32 startAddr, UINT8* blockRd, UINT32 size)
{
    MCD_CONFIG_STRUCT_T* mcd_config = NULL;
    UINT32 config_id = 0;
    UINT32 i;
    MCD_ERR_T mcd_err = MCD_ERR_NO;

    mcd_config = (MCD_CONFIG_STRUCT_T*)tgt_GetMcdConfig();

    mcd_WaitSemTFlash();
    config_id = g_mcd_id == MCD_CARD_ID_0 ? 0:1;

    if(0 ==  g_IsOpen[config_id] )
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "mcd_Read:g_IsOpen is 0,addr = 0x%x.",startAddr);
        mcd_ReleaseSemTFlash();
        return MCD_ERR_NO_CARD;
    }

    if(MCD_CONFIG_CARD_TYPE_SDMMC == mcd_config->mcd_if[config_id].type)
    {

        mcd_err = mcd_sdmmc_Read(startAddr, blockRd, size);
        MCD_TRACE(MCD_INFO_TRC, 0, "mcd_Read:mult-block,config_id = %d,mcd_err =  %d,startAddr = 0x%x,size = 0x%x.",config_id,mcd_err,startAddr,size);

    }
    else if(MCD_CONFIG_CARD_TYPE_SPI == mcd_config->mcd_if[config_id].type)
    {
        // for(i = 0;  i < (size/MCD_DEFUALT_BLOCK_SIZE); i++)
        // {
        // hal_HstSendEvent(0xc0000000 | (startAddr + i*MCD_DEFUALT_BLOCK_SIZE));
        //  mcd_err = mcd_spi_Read((startAddr + i), (blockRd + i*MCD_DEFUALT_BLOCK_SIZE), MCD_DEFUALT_BLOCK_SIZE);
        mcd_err = mcd_spi_Read(startAddr, blockRd, size);
        // if(MCD_ERR_NO != mcd_err)
        // {
        //   break;
        //  }
        MCD_TRACE(MCD_INFO_TRC, 0, "mcd_Read:sigle-block,config_id = %d,mcd_err =  %d,startAddr = 0x%x,512.",config_id,mcd_err,(startAddr + i*MCD_DEFUALT_BLOCK_SIZE));
        // }
    }
    else
    {
        mcd_err = MCD_ERR_NO_CARD;
    }

    if(MCD_ERR_NO != mcd_err)
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "mcd_Read:sigle-block,config_id = %d,mcd_err =  %d,startAddr = 0x%x,size = 0x%x.",config_id,mcd_err,startAddr,size);
        // hal_HstSendEvent(0xc00000e1);
        // hal_HstSendEvent(mcd_err);
    }
    mcd_ReleaseSemTFlash();

    return mcd_err;
}

#ifdef DUAL_TFLASH_SUPPORT
// =============================================================================
// mcd_SwitchOperationTo
// -----------------------------------------------------------------------------
/// Switch MCD id.
///
/// To be called when appliction switch to another t-falsh.
/// @param  mcdId is mcd device identification under dual t-flash mode.value is 0 or 1.
///
// =============================================================================
PUBLIC MCD_ERR_T mcd_SwitchOperationTo(MCD_CARD_ID mcdId)
{
    MCD_CONFIG_STRUCT_T* mcd_config = NULL;
    MCD_ERR_T mcd_err = MCD_ERR_NO;
    UINT32 config_id;
    MCD_CARD_ID card_id;

    card_id = mcdId;
    mcd_config = (MCD_CONFIG_STRUCT_T*)tgt_GetMcdConfig();
    config_id = mcdId ==  MCD_CARD_ID_0 ?  0:1;

    mcd_WaitSemTFlash();
    if(MCD_CONFIG_CARD_TYPE_SPI == mcd_config->mcd_if[0].type &&
            MCD_CONFIG_CARD_TYPE_SPI == mcd_config->mcd_if[1].type)
    {
        mcd_err = mcd_spi_SwitchOperationTo(mcdId);
        MCD_TRACE(MCD_INFO_TRC, 0, "mcd_SwitchOperationTo:spi switch.mcdId = %d, mcd_err = %d.",mcdId,mcd_err);
    }
    else
    {
        mcd_err = MCD_ERR_NO;
    }

    if(MCD_ERR_NO == mcd_err)
    {
        g_mcd_id = mcdId;
        MCD_TRACE(MCD_INFO_TRC, 0, "mcd_SwitchOperationTo:g_mcd_id = %d.",g_mcd_id);
        // al_HstSendEvent(0xdada0000|g_mcd_id);
    }
    mcd_ReleaseSemTFlash();

    return mcd_err;
}
#endif


PUBLIC void mcd_delay_tick(UINT32 tktnum)
{
#if 0
    UINT32 wait_time = 0;

    wait_time = hal_TimGetUpTime();
    if((hal_TimGetUpTime() - wait_time) > tktnum)
    {
        return;
    }
#else
    sxr_Sleep(tktnum);
#endif
}


PUBLIC VOID mcd_LowPower(VOID)
{
    if((0 == g_IsOpen[0]) && (0 == g_IsOpen[1]))
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "mcd_LowPower ok.");
        pmd_EnablePower(PMD_POWER_SDMMC, FALSE);
        mcd_delay_tick(1638);
    }
    else
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "mcd_LowPower tflas card not close. t0 = %d, t1= %d.",g_IsOpen[0],g_IsOpen[1]);
    }

}


PUBLIC UINT8 mcd_IsOpened(MCD_CARD_ID mcdId)
{
    UINT32 config_id = 0;

    config_id = (mcdId == MCD_CARD_ID_0) ? 0:1;
    return g_IsOpen[config_id];
}


