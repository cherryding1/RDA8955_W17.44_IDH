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
#include "mcdp_debug.h"
#include "mcdp_sdmmc.h"

#include "hal_sdmmc.h"
#include "hal_sys.h"

#include "stdlib.h"
#include "stdio.h"
#include "sxr_tls.h"

/*
#include "ifc_service.h"
#include "lp_timer_service.h"
*/
#include "pmd_m.h"

#include "sxs_io.h"
#include "sxr_sbx.h"


// =============================================================================
//  MACROS
// =============================================================================


#define BROAD_ADDR  0
#define NOT_SDMMC_OVER  (1<<0)

#define MCD_MAX_BLOCK_NUM       128


#define MCD_SDMMC_OCR_TIMEOUT    (1 SECOND)  // the card is supposed to answer within 1s
//  - Max wait is 128 * 10ms=1,28s
//  TODO: where those 10ms come from ??

// Command 8 things: cf spec Vers.2 section 4.3.13
#define MCD_CMD8_CHECK_PATTERN      0xaa
#define MCD_CMD8_VOLTAGE_SEL        (0x1<<8)
#define MCD_OCR_HCS                 (1<<30)
#define MCD_OCR_CCS_MASK            (1<<30)


// Timeouts for V1
#define MCD_CMD_TIMEOUT_V1        ( 1 SECOND / 2 )
#define MCD_RESP_TIMEOUT_V1       ( 1 SECOND / 2 )
#define MCD_TRAN_TIMEOUT_V1       ( 1 SECOND / 2 )
#define MCD_READ_TIMEOUT_V1       ( 1 SECOND )
#define MCD_WRITE_TIMEOUT_V1      ( 1 SECOND )

// Timeouts for V2
#define MCD_CMD_TIMEOUT_V2        ( 1 SECOND / 10 )
#define MCD_RESP_TIMEOUT_V2       ( 1 SECOND / 10 )
#define MCD_TRAN_TIMEOUT_V2       ( 1 SECOND / 10 )
#define MCD_READ_TIMEOUT_V2       ( 1 SECOND / 2 )
#define MCD_WRITE_TIMEOUT_V2      (1 SECOND / 2 )
#define MCD_ERASE_TIMEOUT         ( 2 SECOND )



// =============================================================================
// TYPES
// =============================================================================

typedef struct
{
    UINT8   mid;
    UINT32  oid;
    UINT32  pnm1;
    UINT8   pnm2;
    UINT8   prv;
    UINT32  psn;
    UINT32  mdt;
    UINT8   crc;
} MCD_CID_FORMAT_T;


PRIVATE BOOL g_small_card = FALSE;
// =============================================================================
// Global variables
// =============================================================================

// Spec Ver2 p96
#define MCD_SDMMC_OCR_VOLT_PROF_MASK  0x00ff8000

PRIVATE UINT32 g_mcdOcrReg = MCD_SDMMC_OCR_VOLT_PROF_MASK;

/// Relative Card Address Register
/// Nota RCA is sixteen bit long, but is always used
/// as the 16 upper bits of a 32 bits word. Therefore
/// is variable is in fact (RCA<<16), to avoid operation
/// (shift, unshift), to always place the RCA value as the
/// upper bits of a data word.
PRIVATE UINT32 g_mcdRcaReg = 0x00000000;

// Driver Stage Register p.118
// (To adjust bus capacitance)
// TODO Tune and put in calibration ?
PRIVATE UINT32 g_mcdSdmmcDsr = 0x04040000;


PRIVATE UINT32 g_mcdSdmmcFreq = 200000;

PRIVATE MCD_CID_FORMAT_T  g_mcdCidReg;
PRIVATE UINT32            g_mcdBlockLen   = 0;
PRIVATE UINT32            g_mcdNbBlock    = 0;
PRIVATE BOOL              g_mcdCardIsSdhc = FALSE;

PRIVATE MCD_STATUS_T      g_mcdStatus = MCD_STATUS_NOTOPEN_PRESENT;

PRIVATE CONST TGT_MCD_CONFIG_T*     g_mcdConfig=NULL;
PRIVATE MCD_CARD_DETECT_HANDLER_T   g_mcdCardDetectHandler=NULL;

/// Semaphore to ensure proper concurrency of the MCD accesses
/// among all tasks.
PRIVATE UINT32           g_mcdSemaphore   = 0xFF;

/// Current in-progress transfer, if any.
PRIVATE HAL_SDMMC_TRANSFER_T g_mcdSdmmcTransfer =
{
    .sysMemAddr = 0,
    .sdCardAddr = 0,
    .blockNum   = 0,
    .blockSize  = 0,
    .direction  = HAL_SDMMC_DIRECTION_WRITE,
};

PRIVATE MCD_CSD_T g_mcdLatestCsd;

PRIVATE MCD_CARD_VER g_mcdVer = MCD_CARD_V2;
// =============================================================================
// Functions
// =============================================================================

// =============================================================================
// Functions for the HAL Driver ?
// =============================================================================

/// Macro to easily implement concurrency in the MCD driver.
/// Enter in the 'critical section'.
#define MCD_CS_ENTER \
    if (g_mcdSemaphore != 0xFF)             \
    {                                       \
        sxr_TakeSemaphore(g_mcdSemaphore);  \
    }                                       \
    else                                    \
    {                                       \
        return MCD_ERR;                     \
    }


/// Macro to easily implement concurrency in the MCD driver.
/// Exit in the 'critical section'.
#define MCD_CS_EXIT \
    {                                       \
    sxr_ReleaseSemaphore(g_mcdSemaphore);   \
    }

MCD_ERR_T  mcd_SdmmcModeSwitch(UINT32 arg);


PUBLIC void mcd_delay(UINT32 tktnum)
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


PUBLIC void mcd_hal_delay(UINT32 tktnum)
{
#if 1
    UINT32 wait_time = 0;

    wait_time = hal_TimGetUpTime();
    while(1)
    {
        if((hal_TimGetUpTime() - wait_time) > tktnum)
        {
            return;
        }
    }
#else
    sxr_Sleep(tktnum);
#endif
}


PUBLIC MCD_ERR_T mcd_GetCardSize(MCD_CARD_SIZE_T* size)
{
    MCD_CS_ENTER;

    size->blockLen = g_mcdBlockLen;
    size->nbBlock  = g_mcdNbBlock;

    MCD_CS_EXIT;
    return MCD_ERR_NO;
}

// Wait for a command to be done or timeouts
PRIVATE MCD_ERR_T mcd_SdmmcWaitCmdOver(VOID)
{
    UINT32 startTime = hal_TimGetUpTime();
    UINT32 time_out;

    time_out =  (MCD_CARD_V1 == g_mcdVer) ? MCD_CMD_TIMEOUT_V1:MCD_CMD_TIMEOUT_V2;

    while(hal_TimGetUpTime() - startTime < time_out && !hal_SdmmcCmdDone());

    if (!hal_SdmmcCmdDone())
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "SDC Waiting is Time out");
        return MCD_ERR_CARD_TIMEOUT;
    }
    else
    {
        return MCD_ERR_NO;
    }
}

/// Update g_mcdStatus
/// @return TRUE is card present (only exact when GPIO is used for card detect.)
PRIVATE BOOL mcd_CardDetectUpdateStatus(VOID)
{
    if(NULL == g_mcdConfig)
    {
        g_mcdConfig = tgt_GetMcdConfig();
    }

    if(g_mcdConfig->cardDetectGpio != HAL_GPIO_NONE)
    {
        BOOL gpioState = !!hal_GpioGet(g_mcdConfig->cardDetectGpio);
        // CARD ?
        if(gpioState == g_mcdConfig->gpioCardDetectHigh)
        {
            // card detected
            switch(g_mcdStatus)
            {
                case MCD_STATUS_OPEN_NOTPRESENT: // wait for the close !
                case MCD_STATUS_OPEN:
                    // No change
                    break;
                default:
                    g_mcdStatus = MCD_STATUS_NOTOPEN_PRESENT;
            }
            return TRUE;
        }
        else
        {
            // no card
            switch(g_mcdStatus)
            {
                case MCD_STATUS_OPEN_NOTPRESENT:
                case MCD_STATUS_OPEN:
                    g_mcdStatus = MCD_STATUS_OPEN_NOTPRESENT;
                    break;
                default:
                    g_mcdStatus = MCD_STATUS_NOTPRESENT;
            }
            return FALSE;
        }
    }
    // estimated
    switch(g_mcdStatus)
    {
        case MCD_STATUS_OPEN:
        case MCD_STATUS_NOTOPEN_PRESENT:
            return TRUE;
        default:
            return FALSE;
    }
}

PRIVATE VOID mcd_CardDetectHandler(VOID)
{
    BOOL CardPresent = mcd_CardDetectUpdateStatus();

    g_mcdCardDetectHandler(CardPresent);
}

// =============================================================================
// mcd_SetCardDetectHandler
// -----------------------------------------------------------------------------
/// Register a handler for card detection
///
/// @param handler function called when insertion/removal is detected.
// =============================================================================
PUBLIC MCD_ERR_T mcd_SetCardDetectHandler(MCD_CARD_DETECT_HANDLER_T handler)
{
    if(NULL == g_mcdConfig)
    {
        g_mcdConfig = tgt_GetMcdConfig();
    }

    if(g_mcdConfig->cardDetectGpio == HAL_GPIO_NONE)
    {
        return MCD_ERR_NO_HOTPLUG;
    }

    if(NULL != handler)
    {
        HAL_GPIO_CFG_T cfg  =
        {
            .direction      = HAL_GPIO_DIRECTION_INPUT,
            .irqMask        =
            {
                .rising     = TRUE,
                .falling    = TRUE,
                .debounce   = TRUE,
                .level      = FALSE
            },
            .irqHandler     = mcd_CardDetectHandler
        };

        hal_GpioOpen(g_mcdConfig->cardDetectGpio, &cfg);
        g_mcdCardDetectHandler = handler;
    }
    else
    {
        hal_GpioClose(g_mcdConfig->cardDetectGpio);
        g_mcdCardDetectHandler = NULL;
    }

    return MCD_ERR_NO;
}

// =============================================================================
// mcd_CardStatus
// -----------------------------------------------------------------------------
/// Return the card status
///
/// @return Card status see #MCD_STATUS_T
// =============================================================================
PUBLIC MCD_STATUS_T mcd_CardStatus(MCD_CARD_ID mcdId)
{
    mcd_CardDetectUpdateStatus();
    return g_mcdStatus;
}

// =============================================================================
// mcd_SdmmcWaitResp
// -----------------------------------------------------------------------------
/// Wait for a response for a time configured by MCD_RESP_TIMEOUT
/// @return MCD_ERR_NO if a response with a good crc was received,
///         MCD_ERR_CARD_NO_RESPONSE if no response was received within the
/// driver configured timeout.
//          MCD_ERR_CARD_RESPONSE_BAD_CRC if the received response presented
//  a bad CRC.
// =============================================================================
PRIVATE MCD_ERR_T mcd_SdmmcWaitResp(VOID)
{
    HAL_SDMMC_OP_STATUS_T status = hal_SdmmcGetOpStatus();
    UINT32 startTime = hal_TimGetUpTime();
    UINT32 rsp_time_out;

    rsp_time_out =  (MCD_CARD_V1 == g_mcdVer) ? MCD_RESP_TIMEOUT_V1:MCD_RESP_TIMEOUT_V2;

    while(hal_TimGetUpTime() - startTime < rsp_time_out &&status.fields.noResponseReceived)
    {
        status = hal_SdmmcGetOpStatus();
    }

    if (status.fields.noResponseReceived)
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "Response is Time out");
        return MCD_ERR_CARD_NO_RESPONSE;
    }

    if(status.fields.responseCrcError)
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "Response CRC is Error");
        return MCD_ERR_CARD_RESPONSE_BAD_CRC;
    }

    return MCD_ERR_NO;
}



//=============================================================================
// mcd_SdmmcReadCheckCrc
//-----------------------------------------------------------------------------
/// Check the read state of the sdmmc card.
/// @return MCD_ERR_NO if the Crc of read data was correct,
/// MCD_ERR_CARD_RESPONSE_BAD_CRC otherwise.
//=============================================================================
PRIVATE MCD_ERR_T mcd_SdmmcReadCheckCrc(VOID)
{
    HAL_SDMMC_OP_STATUS_T operationStatus;
    operationStatus = hal_SdmmcGetOpStatus();
    if (operationStatus.fields.dataError != 0) // 0 means no CRC error during transmission
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "sdC status:%0x", operationStatus.reg);
        return MCD_ERR_CARD_RESPONSE_BAD_CRC;
    }
    else
    {
        return MCD_ERR_NO;
    }
}


//=============================================================================
// mcd_SdmmcWriteCheckCrc
//-----------------------------------------------------------------------------
/// Check the crc state of the write operation of the sdmmc card.
/// @return MCD_ERR_NO if the Crc of read data was correct,
/// MCD_ERR_CARD_RESPONSE_BAD_CRC otherwise.
//=============================================================================
PRIVATE MCD_ERR_T mcd_SdmmcWriteCheckCrc(VOID)
{
    HAL_SDMMC_OP_STATUS_T operationStatus;
    operationStatus = hal_SdmmcGetOpStatus();

    if (operationStatus.fields.crcStatus != 2) // 0b010 = transmissionRight TODO a macro ?
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "sdC status:%0x", operationStatus.reg);
        return MCD_ERR_CARD_RESPONSE_BAD_CRC;
    }
    else
    {
        return MCD_ERR_NO;
    }
}




// =============================================================================
// mcd_SdmmcSendCmd
// -----------------------------------------------------------------------------
/// Send a command to the card, and fetch the response if one is expected.
/// @param cmd CMD to send
/// @param arg Argument of the ACMD.
/// @param resp Buffer to store card response.
/// @param suspend Not supported.
/// @return MCD_ERR_NO if a response with a good crc was received,
///         MCD_ERR_CARD_NO_RESPONSE if no reponse was received within the
/// driver configured timeout.
///          MCD_ERR_CARD_RESPONSE_BAD_CRC if the received response presented
///  a bad CRC.
///         MCD_ERR_CARD_TIMEOUT if the card timedout during procedure.
// =============================================================================
PRIVATE MCD_ERR_T mcd_SdmmcSendCmd(HAL_SDMMC_CMD_T cmd, UINT32 arg,
                                   UINT32* resp, BOOL suspend)
{
    MCD_ERR_T errStatus = MCD_ERR_NO;
    MCD_CARD_STATUS_T cardStatus = {0};
    UINT32  cmd55Response[4] = {0, 0, 0, 0};
    if (cmd & HAL_SDMMC_ACMD_SEL)
    {
        // This is an application specific command,
        // we send the CMD55 first
        hal_SdmmcSendCmd(HAL_SDMMC_CMD_APP_CMD, g_mcdRcaReg, FALSE);

        // Wait for command over
        if (MCD_ERR_CARD_TIMEOUT == mcd_SdmmcWaitCmdOver())
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "Cmd55 Send is Time out");
            return MCD_ERR_CARD_TIMEOUT;
        }

        // Wait for response
        if (hal_SdmmcNeedResponse(HAL_SDMMC_CMD_APP_CMD))
        {
            errStatus = mcd_SdmmcWaitResp();
        }
        if (MCD_ERR_NO != errStatus)
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "cmd55 response error");
            return errStatus;
        }

        // Fetch response
        hal_SdmmcGetResp(HAL_SDMMC_CMD_APP_CMD, cmd55Response, FALSE);

        cardStatus.reg = cmd55Response[0];
        if(HAL_SDMMC_CMD_SEND_OP_COND == cmd) // for some special card
        {
            //if (!(cardStatus.fields.readyForData) || !(cardStatus.fields.appCmd))
            if (!(cardStatus.fields.appCmd))
            {
                MCD_TRACE(MCD_INFO_TRC, 0, "cmd55(acmd411) status=%0x", cardStatus.reg);
                return MCD_ERR;
            }
        }
        else
        {
            if (!(cardStatus.fields.readyForData) || !(cardStatus.fields.appCmd))
            {
                MCD_TRACE(MCD_INFO_TRC, 0, "cmd55 status=%0x", cardStatus.reg);
                return MCD_ERR;
            }

        }
    }
    // Send proper command. If it was an ACMD, the CMD55 have just been sent.
    hal_SdmmcSendCmd(cmd, arg, suspend);

    // Wait for command to be sent
    errStatus = mcd_SdmmcWaitCmdOver();


    if (MCD_ERR_CARD_TIMEOUT == errStatus)
    {
        if (cmd & HAL_SDMMC_ACMD_SEL)
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "ACMD %d Sending Timed out", (cmd & HAL_SDMMC_CMD_MASK));
        }
        else
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "CMD %d Sending Timed out", (cmd & HAL_SDMMC_CMD_MASK));
        }
        return MCD_ERR_CARD_TIMEOUT;
    }

    // Wait for response and get its argument
    if (hal_SdmmcNeedResponse(cmd))
    {
        errStatus = mcd_SdmmcWaitResp();
    }

    if (MCD_ERR_NO != errStatus)
    {
        if (cmd & HAL_SDMMC_ACMD_SEL)
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "ACMD %d Response Error", (cmd & HAL_SDMMC_CMD_MASK));
        }
        else
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "CMD %d Response Error", (cmd & HAL_SDMMC_CMD_MASK));
            return errStatus;
        }
    }

    // Fetch response
    hal_SdmmcGetResp(cmd, resp, FALSE);

    //FOR DEBUG: MCD_TRACE(MCD_INFO_TRC, 0, "CMD %d Response is %#x", (cmd & HAL_SDMMC_CMD_MASK), resp[0]);

    return MCD_ERR_NO;
}




// =============================================================================
// mcd_SdmmcInitCid
// -----------------------------------------------------------------------------
/// Set the CID in the driver from the data read on the card.
/// @param cid 4 word array read from the card, holding the CID register value.
// =============================================================================
PRIVATE MCD_ERR_T mcd_SdmmcInitCid(UINT32* cid)
{
    // Fill the structure with the register bitfields value.
    g_mcdCidReg.mid     = (UINT8)((cid[3]&(0xff<<24))>>24);
    g_mcdCidReg.oid     = (cid[3]&(0xffff<<8))>>8;
    g_mcdCidReg.pnm2    = (UINT8)(cid[3]&0xff);
    g_mcdCidReg.pnm1    = cid[2];
    g_mcdCidReg.prv     = (UINT8)((cid[1]&(0xff<<24))>>24);
    g_mcdCidReg.psn     = (cid[1]&0xffffff)<<8;
    g_mcdCidReg.psn     = g_mcdCidReg.psn|((cid[0]&(0xff<<23))>>23);
    g_mcdCidReg.mdt     = (cid[0]&(0xfff<<7))>>7;
    g_mcdCidReg.crc     = (UINT8)(cid[0]&0x7f);

    return MCD_ERR_NO;
}


#define MCD_CSD_VERSION_1       0
#define MCD_CSD_VERSION_2       1
// =============================================================================
// mcd_SdmmcInitCsd
// -----------------------------------------------------------------------------
/// Fill MCD_CSD_T structure from the array of data read from the card
///
/// @param csd Pointer to the structure
/// @param csdRaw Pointer to the raw data.
/// @return MCD_ERR_NO
// =============================================================================
PRIVATE MCD_ERR_T mcd_SdmmcInitCsd(MCD_CSD_T* csd, UINT32* csdRaw)
{
    // CF SD spec version2, CSD version 1 ?
    csd->csdStructure       = (UINT8)((csdRaw[3]&(0x3<<30))>>30);

    // Byte 47 to 75 are different depending on the version
    // of the CSD srtucture.
    csd->specVers           = (UINT8)((csdRaw[3]&(0xf<26))>>26);
    csd->taac               = (UINT8)((csdRaw[3]&(0xff<<16))>>16);
    csd->nsac               = (UINT8)((csdRaw[3]&(0xff<<8))>>8);
    csd->tranSpeed          = (UINT8)(csdRaw[3]&0xff);

    csd->ccc                = (csdRaw[2]&(0xfff<<20))>>20;
    csd->readBlLen          = (UINT8)((csdRaw[2]&(0xf<<16))>>16);
    csd->readBlPartial      = (UINT8)((csdRaw[2]&(0x1<<15))>>15);
    csd->writeBlkMisalign   = (UINT8)((csdRaw[2]&(0x1<<14))>>14);
    csd->readBlkMisalign    = (UINT8)((csdRaw[2]&(0x1<<13))>>13);
    csd->dsrImp             = (UINT8)((csdRaw[2]&(0x1<<12))>>12);

    if (csd->csdStructure == MCD_CSD_VERSION_1)
    {
        csd->cSize              = (csdRaw[2]&0x3ff)<<2;

        csd->cSize              = csd->cSize|((csdRaw[1]&(0x3<<30))>>30);
        csd->vddRCurrMin        = (UINT8)((csdRaw[1]&(0x7<<27))>>27);
        csd->vddRCurrMax        = (UINT8)((csdRaw[1]&(0x7<<24))>>24);
        csd->vddWCurrMin        = (UINT8)((csdRaw[1]&(0x7<<21))>>21);
        csd->vddWCurrMax        = (UINT8)((csdRaw[1]&(0x7<<18))>>18);
        csd->cSizeMult          = (UINT8)((csdRaw[1]&(0x7<<15))>>15);

        // Block number: cf Spec Version 2 page 103 (116).
        csd->blockNumber        = (csd->cSize + 1)<<(csd->cSizeMult + 2);
    }
    else
    {
        // csd->csdStructure == MCD_CSD_VERSION_2
        csd->cSize = (((csdRaw[2]&0x3f))<<16)|((csdRaw[1]&(0xffff<<16))>>16);

        // Other fields are undefined --> zeroed
        csd->vddRCurrMin = 0;
        csd->vddRCurrMax = 0;
        csd->vddWCurrMin = 0;
        csd->vddWCurrMax = 0;
        csd->cSizeMult   = 0;

        // Block number: cf Spec Version 2 page 109 (122).
        csd->blockNumber        = (csd->cSize + 1) * 1024;
        //should check incompatible size and return MCD_ERR_UNUSABLE_CARD;
    }

    csd->eraseBlkEnable     = (UINT8)((csdRaw[1]&(0x1<<14))>>14);
    csd->sectorSize         = (UINT8)((csdRaw[1]&(0x7f<<7))>>7);
    csd->wpGrpSize          = (UINT8)(csdRaw[1]&0x7f);

    csd->wpGrpEnable        = (UINT8)((csdRaw[0]&(0x1<<31))>>31);
    csd->r2wFactor          = (UINT8)((csdRaw[0]&(0x7<<26))>>26);
    csd->writeBlLen         = (UINT8)((csdRaw[0]&(0xf<<22))>>22);
    csd->writeBlPartial     = (UINT8)((csdRaw[0]&(0x1<<21))>>21);
    csd->fileFormatGrp      = (UINT8)((csdRaw[0]&(0x1<<15))>>15);
    csd->copy               = (UINT8)((csdRaw[0]&(0x1<<14))>>14);
    csd->permWriteProtect   = (UINT8)((csdRaw[0]&(0x1<<13))>>13);
    csd->tmpWriteProtect    = (UINT8)((csdRaw[0]&(0x1<<12))>>12);
    csd->fileFormat         = (UINT8)((csdRaw[0]&(0x3<<10))>>10);
    csd->crc                = (UINT8)((csdRaw[0]&(0x7f<<1))>>1);

#if 0
    MCD_TRACE(MCD_INFO_TRC, 0, "CSD:csdStructure = %d", csd->csdStructure        );
    MCD_TRACE(MCD_INFO_TRC, 0, "CSD:specVers     = %d", csd->specVers           );
    MCD_TRACE(MCD_INFO_TRC, 0, "CSD:taac         = %d", csd->taac                );
    MCD_TRACE(MCD_INFO_TRC, 0, "CSD:nsac         = %d", csd->nsac                );
    MCD_TRACE(MCD_INFO_TRC, 0, "CSD:tranSpeed    = %d", csd->tranSpeed           );
    MCD_TRACE(MCD_INFO_TRC, 0, "CSD:ccc          = %d", csd->ccc             );
    MCD_TRACE(MCD_INFO_TRC, 0, "CSD:readBlLen    = %d", csd->readBlLen           );
    MCD_TRACE(MCD_INFO_TRC, 0, "CSD:readBlPartial = %d", csd->readBlPartial      );
    MCD_TRACE(MCD_INFO_TRC, 0, "CSD:writeBlkMisalign = %d", csd->writeBlkMisalign    );
    MCD_TRACE(MCD_INFO_TRC, 0, "CSD:readBlkMisalign  = %d", csd->readBlkMisalign    );
    MCD_TRACE(MCD_INFO_TRC, 0, "CSD:dsrImp       = %d", csd->dsrImp              );
    MCD_TRACE(MCD_INFO_TRC, 0, "CSD:cSize        = %d", csd->cSize               );
    MCD_TRACE(MCD_INFO_TRC, 0, "CSD:vddRCurrMin  = %d", csd->vddRCurrMin     );
    MCD_TRACE(MCD_INFO_TRC, 0, "CSD:vddRCurrMax  = %d", csd->vddRCurrMax     );
    MCD_TRACE(MCD_INFO_TRC, 0, "CSD:vddWCurrMin  = %d", csd->vddWCurrMin     );
    MCD_TRACE(MCD_INFO_TRC, 0, "CSD:vddWCurrMax  = %d", csd->vddWCurrMax     );
    MCD_TRACE(MCD_INFO_TRC, 0, "CSD:cSizeMult    = %d", csd->cSizeMult           );
    MCD_TRACE(MCD_INFO_TRC, 0, "CSD:eraseBlkEnable = %d", csd->eraseBlkEnable        );
    MCD_TRACE(MCD_INFO_TRC, 0, "CSD:sectorSize   = %d", csd->sectorSize          );
    MCD_TRACE(MCD_INFO_TRC, 0, "CSD:wpGrpSize    = %d", csd->wpGrpSize           );
    MCD_TRACE(MCD_INFO_TRC, 0, "CSD:wpGrpEnable  = %d", csd->wpGrpEnable     );
    MCD_TRACE(MCD_INFO_TRC, 0, "CSD:r2wFactor    = %d", csd->r2wFactor           );
    MCD_TRACE(MCD_INFO_TRC, 0, "CSD:writeBlLen   = %d", csd->writeBlLen          );
#endif
    MCD_TRACE(MCD_INFO_TRC, 0, "CSD:writeBlPartial = %d", csd->writeBlPartial        );
    MCD_TRACE(MCD_INFO_TRC, 0, "CSD:fileFormatGrp = %d", csd->fileFormatGrp       );
    MCD_TRACE(MCD_INFO_TRC, 0, "CSD:copy  = %d", csd->copy                 );
    MCD_TRACE(MCD_INFO_TRC, 0, "CSD:permWriteProtect = %d", csd->permWriteProtect      );
    MCD_TRACE(MCD_INFO_TRC, 0, "CSD:tmpWriteProtect  = %d", csd->tmpWriteProtect );
    MCD_TRACE(MCD_INFO_TRC, 0, "CSD:fileFormat       = %d", csd->fileFormat          );
    MCD_TRACE(MCD_INFO_TRC, 0, "CSD:crc              = %d", csd->crc                );
    MCD_TRACE(MCD_INFO_TRC, 0, "CSD:block number     = %d", csd->blockNumber        );

    if(csd->blockNumber < 300000 ) //51200
    {
        if(g_mcdCardIsSdhc == FALSE)
        {
            g_small_card = TRUE; hal_HstSendEvent(0xbbaa0001);
        }
        //return MCD_ERR_CSD;

    }
    return MCD_ERR_NO;
}



// =============================================================================
//  FUNCTIONS (public)
// =============================================================================



// =============================================================================
// mcd_ReadCsd
// -----------------------------------------------------------------------------
/// @brief Read the MMC CSD register
/// @param mcdCsd Pointer to the structure where the MMC CSD register info
/// are going to be written.
// =============================================================================
PRIVATE MCD_ERR_T mcd_ReadCsd(MCD_CSD_T* mcdCsd)
{
    MCD_ERR_T errStatus = MCD_ERR_NO;
    UINT32 response[4];

    // Get card CSD
    errStatus = mcd_SdmmcSendCmd(HAL_SDMMC_CMD_SEND_CSD, g_mcdRcaReg, response, FALSE);
    if (errStatus == MCD_ERR_NO)
    {
        errStatus = mcd_SdmmcInitCsd(mcdCsd, response);
    }

    // Store it localy
    // FIXME: Is this real ? cf Physical Spec version 2
    // page 59 (72) about CMD16 : default block length
    // is 512 bytes. Isn't 512 bytes a good block
    // length ? And I quote : "In both cases, if block length is set larger
    // than 512Bytes, the card sets the BLOCK_LEN_ERROR bit."
    g_mcdBlockLen = (1 << mcdCsd->readBlLen);
    if (g_mcdBlockLen > 512)
    {
        g_mcdBlockLen = 512;
    }
    g_mcdNbBlock  = mcdCsd->blockNumber * ((1 << mcdCsd->readBlLen)/g_mcdBlockLen);

    return errStatus;
}


void mcd_hal_delay_sd1(UINT32 tktnum)
{
#if 1
    UINT32 wait_time = 0;

    wait_time = hal_TimGetUpTime();
    while(1)
    {
        if((hal_TimGetUpTime() - wait_time) > tktnum)
        {
            return;
        }
    }
#else
    sxr_Sleep(tktnum);
#endif
}




// =============================================================================
// mcd_Open
// -----------------------------------------------------------------------------
/// @brief Open the SD-MMC chip
/// This function does the init process of the MMC chip, including reseting
/// the chip, sending a command of init to MMC, and reading the CSD
/// configurations.
///
/// @param mcdCsd Pointer to the structure where the MMC CSD register info
/// are going to be written.
///
/// @param mcdVer is t card version.
// =============================================================================



PUBLIC MCD_ERR_T mcd_Open(MCD_CARD_ID mcdId,MCD_CSD_T* mcdCsd, MCD_CARD_VER mcdVer)
{
    MCD_ERR_T                  errStatus   = MCD_ERR_NO;
    UINT32                     response[4] = {0, 0, 0, 0};
    MCD_CARD_STATUS_T          cardStatus  = {0};
    BOOL                       isMmc       = FALSE;
    HAL_SDMMC_DATA_BUS_WIDTH_T dataBusWidth;
    UINT32 startTime = 0;
    MCD_PROFILE_FUNCTION_ENTER(mcd_Open);

    mcd_sdmmc_rst_volt();
    MCD_TRACE(TSTDOUT, 0, "mcd_Open starts ... mcdVer %d",mcdVer);

    // Check concurrency. Only one mcd_Open.
    UINT32 cs = hal_SysEnterCriticalSection();
    if (g_mcdSemaphore == 0xFF)
    {
        // Create semaphore and go on with the driver.

        // NOTE: This semaphore is never deleted for now, as
        // 1. sema deletion while other task is waiting for it will cause an error;
        // 2. sema deletion will make trouble if already-open state is considered OK.
        g_mcdSemaphore = sxr_NewSemaphore(1);
    }
    hal_SysExitCriticalSection(cs);

    // Following operation should be kept protected
    MCD_CS_ENTER;

    if(NULL == g_mcdConfig)
    {
        g_mcdConfig = tgt_GetMcdConfig();
    }

    if(g_mcdConfig->cardDetectGpio != HAL_GPIO_NONE)
    {
        // Only if GPIO for detection exists, else we try to open anyway.
        if(!mcd_CardDetectUpdateStatus())
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "mcd_Open: GPIO detection - no card");
            MCD_CS_EXIT;
            MCD_PROFILE_FUNCTION_EXIT(mcd_Open);
            return MCD_ERR_NO_CARD;
        }
    }

    if (g_mcdStatus == MCD_STATUS_OPEN)
    {
        // Already opened, just return OK
        MCD_TRACE(MCD_INFO_TRC, 0, "mcd_Open: Already opened");
        *mcdCsd = g_mcdLatestCsd;

        MCD_CS_EXIT;
        MCD_PROFILE_FUNCTION_EXIT(mcd_Open);
        return MCD_ERR_NO;
    }
    if(MCD_CARD_V1 == mcdVer)
    {
        hal_SdmmcOpen(0x05);
    }
    else
    {
        hal_SdmmcOpen(0x1); // fprd electrical specification
    }


    ///@todo: should use g_mcdConfig->dat3HasPullDown if true, we can handle
    /// basic card detection as follows:
    /// call hal_SdmmcGetCardDetectPinLevel() to check if card as changed (back 1)
    /// send command ACMD42 to disable internal pull up (so pin goes to 0)
    /// meaning if pin is 1, there was a removal, file system should be told to
    /// remount the card as it might be a different one (or the content might have
    /// been changed externally).

    // RCA = 0x0000 for card identification phase.
    g_mcdRcaReg = 0;

    g_mcdSdmmcFreq = 200000;
    hal_SdmmcSetClk(g_mcdSdmmcFreq);

    // assume it's not an SDHC card
    g_mcdCardIsSdhc = FALSE;
    g_mcdOcrReg = MCD_SDMMC_OCR_VOLT_PROF_MASK;

    // Send Power On command
    hal_SdmmcSetClkMode(FALSE); // make sure the clk number  much than 74 cycle after the first time.
    mcd_hal_delay_sd1(16);
    errStatus = mcd_SdmmcSendCmd(HAL_SDMMC_CMD_GO_IDLE_STATE, BROAD_ADDR, response, FALSE);
    hal_SdmmcSetClkMode(TRUE);
    if (MCD_ERR_NO != errStatus)
    {
        // some card need the clk lever is high before send this command ,so try again . becaus the first call can set the clk high.
        errStatus = mcd_SdmmcSendCmd(HAL_SDMMC_CMD_SEND_IF_COND, (MCD_CMD8_VOLTAGE_SEL | MCD_CMD8_CHECK_PATTERN), response, FALSE);
    }
    if (MCD_ERR_NO != errStatus)
    {
        g_mcdStatus = MCD_STATUS_NOTPRESENT;
        MCD_TRACE(MCD_INFO_TRC, 0, "Because Power on, Initialize Failed");
        MCD_TRACE(MCD_INFO_TRC, 0, "Error Status = %d", errStatus);
        hal_SdmmcClose(); pmd_EnablePower(PMD_POWER_SDMMC, FALSE);

        MCD_CS_EXIT;
        MCD_PROFILE_FUNCTION_EXIT(mcd_Open);
        return errStatus;
    }
    else
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "Card successfully in Idle state");
    }

    // Check if the card is a spec vers.2 one
    errStatus = mcd_SdmmcSendCmd(HAL_SDMMC_CMD_SEND_IF_COND, (MCD_CMD8_VOLTAGE_SEL | MCD_CMD8_CHECK_PATTERN), response, FALSE);
    if (MCD_ERR_NO != errStatus)
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "This card doesn't comply to the spec version 2.0");
    }
    else
    {
        // This card comply to the SD spec version 2. Is it compatible with the
        // proposed voltage, and is it an high capacity one ?
        if (response[0] != (MCD_CMD8_VOLTAGE_SEL | MCD_CMD8_CHECK_PATTERN))
        {
            // Bad pattern or unsupported voltage.
            MCD_TRACE(MCD_INFO_TRC, 0, "Bad pattern or unsupported voltage");
            hal_SdmmcClose();        pmd_EnablePower(PMD_POWER_SDMMC, FALSE);

            g_mcdStatus = MCD_STATUS_NOTPRESENT;
            //g_mcdVer = MCD_CARD_V1;
            MCD_CS_EXIT;
            MCD_PROFILE_FUNCTION_EXIT(mcd_Open);
            return MCD_ERR_UNUSABLE_CARD;
        }
        else
        {
            g_mcdOcrReg |= MCD_OCR_HCS;
        }
    }

    // TODO HCS mask bit to ACMD 41 if high capacity
    // Send OCR, as long as the card return busy
    mcd_hal_delay_sd1(100);
    startTime = hal_TimGetUpTime();

    while(1)
    {
        if(hal_TimGetUpTime() - startTime > MCD_SDMMC_OCR_TIMEOUT )
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "SD OCR timeout");
            hal_SdmmcClose(); pmd_EnablePower(PMD_POWER_SDMMC, FALSE);
            g_mcdStatus = MCD_STATUS_NOTPRESENT;

            MCD_CS_EXIT;
            MCD_PROFILE_FUNCTION_EXIT(mcd_Open);
            return MCD_ERR;
        }

        errStatus = mcd_SdmmcSendCmd(HAL_SDMMC_CMD_SEND_OP_COND, g_mcdOcrReg, response, FALSE);
        if (errStatus == MCD_ERR_CARD_NO_RESPONSE)
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "Inserted Card is a MMC");
            // CMD 55 was not recognised: this is not an SD card !
            isMmc = TRUE;
            break;
        }

        // Bit 31 is power up busy status bit(pdf spec p. 109)
        g_mcdOcrReg = (response[0] & 0x7fffffff);

        // Power up busy bit is set to 1,
        // we can continue ...
        if (response[0] & 0x80000000)
        {
            // Version 2?
            if((g_mcdOcrReg & MCD_OCR_HCS) == MCD_OCR_HCS)
            {
                // Card is V2: check for high capacity
                if (response[0] & MCD_OCR_CCS_MASK)
                {
                    g_mcdCardIsSdhc = TRUE;
                    MCD_TRACE(MCD_INFO_TRC, 0, "Card is SDHC");
                }
                else
                {
                    g_mcdCardIsSdhc = FALSE;
                    MCD_TRACE(MCD_INFO_TRC, 0, "Card is standard capacity SD");
                }
            }
            MCD_TRACE(MCD_INFO_TRC, 0, "Inserted Card is a SD card");
            break;
        }
    }

    if (isMmc)
    {
        while(1)
        {
            if(hal_TimGetUpTime() - startTime > MCD_SDMMC_OCR_TIMEOUT )
            {
                MCD_TRACE(MCD_INFO_TRC, 0, "MMC OCR timeout");
                hal_SdmmcClose();        pmd_EnablePower(PMD_POWER_SDMMC, FALSE);

                g_mcdStatus = MCD_STATUS_NOTPRESENT;

                MCD_CS_EXIT;
                MCD_PROFILE_FUNCTION_EXIT(mcd_Open);
                return MCD_ERR;
            }

            errStatus = mcd_SdmmcSendCmd(HAL_SDMMC_CMD_MMC_SEND_OP_COND, g_mcdOcrReg, response, FALSE);
            if (errStatus == MCD_ERR_CARD_NO_RESPONSE)
            {
                MCD_TRACE(MCD_INFO_TRC, 0, "MMC OCR didn't answer");
                hal_SdmmcClose(); pmd_EnablePower(PMD_POWER_SDMMC, FALSE);
                g_mcdStatus = MCD_STATUS_NOTPRESENT;

                MCD_CS_EXIT;
                MCD_PROFILE_FUNCTION_EXIT(mcd_Open);
                return MCD_ERR;
            }

            // Bit 31 is power up busy status bit(pdf spec p. 109)
            g_mcdOcrReg = (response[0] & 0x7fffffff);

            // Power up busy bit is set to 1,
            // we can continue ...
            if (response[0] & 0x80000000)
            {
                break;
            }
        }

    }

    // Get the CID of the card.
    errStatus = mcd_SdmmcSendCmd(HAL_SDMMC_CMD_ALL_SEND_CID, BROAD_ADDR, response, FALSE);
    if(MCD_ERR_NO != errStatus)
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "Because Get CID, Initialize Failed");
        hal_SdmmcClose(); pmd_EnablePower(PMD_POWER_SDMMC, FALSE);
        g_mcdStatus = MCD_STATUS_NOTPRESENT;

        MCD_CS_EXIT;
        MCD_PROFILE_FUNCTION_EXIT(mcd_Open);
        return errStatus;
    }
    mcd_SdmmcInitCid(response);

    // Get card RCA
    errStatus = mcd_SdmmcSendCmd(HAL_SDMMC_CMD_SEND_RELATIVE_ADDR, BROAD_ADDR, response, FALSE);
    if (MCD_ERR_NO != errStatus)
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "Because Get Relative Address, Initialize Failed");
        hal_SdmmcClose(); pmd_EnablePower(PMD_POWER_SDMMC, FALSE);
        g_mcdStatus = MCD_STATUS_NOTPRESENT;

        MCD_CS_EXIT;
        MCD_PROFILE_FUNCTION_EXIT(mcd_Open);
        return errStatus;
    }

    // Spec Ver 2 pdf p. 81 - rca are the 16 upper bit of this
    // R6 answer. (lower bits are stuff bits)
    g_mcdRcaReg = response[0] & 0xffff0000;

    // Get card CSD
    errStatus = mcd_ReadCsd(mcdCsd);
    if (errStatus != MCD_ERR_NO)
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "Because Get CSD, Initialize Failed");
        hal_SdmmcClose(); pmd_EnablePower(PMD_POWER_SDMMC, FALSE);
        g_mcdStatus = MCD_STATUS_NOTPRESENT;

        MCD_CS_EXIT;
        MCD_PROFILE_FUNCTION_EXIT(mcd_Open);
        return errStatus;
    }

    // If possible, set the DSR
    if (mcdCsd->dsrImp)
    {
        errStatus = mcd_SdmmcSendCmd(HAL_SDMMC_CMD_SET_DSR, g_mcdSdmmcDsr, response, FALSE);
        if (errStatus != MCD_ERR_NO)
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "Because Set DSR, Initialize Failed");
            hal_SdmmcClose(); pmd_EnablePower(PMD_POWER_SDMMC, FALSE);
            g_mcdStatus = MCD_STATUS_NOTPRESENT;

            MCD_CS_EXIT;
            MCD_PROFILE_FUNCTION_EXIT(mcd_Open);
            return errStatus;
        }
    }

    // Select the card
    errStatus = mcd_SdmmcSendCmd(HAL_SDMMC_CMD_SELECT_CARD, g_mcdRcaReg, response, FALSE);
    if (errStatus != MCD_ERR_NO)
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "Because Select Card, Initialize Failed");
        MCD_TRACE(MCD_INFO_TRC, 0, "errStatus = %d", errStatus);
        hal_SdmmcClose(); pmd_EnablePower(PMD_POWER_SDMMC, FALSE);
        g_mcdStatus = MCD_STATUS_NOTPRESENT;

        MCD_CS_EXIT;
        MCD_PROFILE_FUNCTION_EXIT(mcd_Open);
        return errStatus;
    }
    // That command returns the card status, we check we're in data mode.
    cardStatus.reg = response[0];
    if(!(cardStatus.fields.readyForData))
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "CMD7 status=%0x", cardStatus.reg);
        hal_SdmmcClose(); pmd_EnablePower(PMD_POWER_SDMMC, FALSE);
        g_mcdStatus = MCD_STATUS_NOTPRESENT;

        MCD_CS_EXIT;
        MCD_PROFILE_FUNCTION_EXIT(mcd_Open);
        return MCD_ERR;
    }

    // Set the bus width (use 4 data lines for SD cards only)
    if (isMmc)
    {
        dataBusWidth = HAL_SDMMC_DATA_BUS_WIDTH_1;
    }
    else
    {

        dataBusWidth = MCD_CARD_V1 == mcdVer ? HAL_SDMMC_DATA_BUS_WIDTH_1 : HAL_SDMMC_DATA_BUS_WIDTH_4;

    }

    errStatus = mcd_SdmmcSendCmd(HAL_SDMMC_CMD_SET_BUS_WIDTH, dataBusWidth,
                                 response, FALSE);
    if (errStatus != MCD_ERR_NO)
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "Because Set Bus, Initialize Failed");
        hal_SdmmcClose(); pmd_EnablePower(PMD_POWER_SDMMC, FALSE);
        g_mcdStatus = MCD_STATUS_NOTPRESENT;

        MCD_CS_EXIT;
        MCD_PROFILE_FUNCTION_EXIT(mcd_Open);
        return errStatus;
    }

    // That command returns the card status, in tran state ?
    cardStatus.reg = response[0];
    if (   !(cardStatus.fields.appCmd)
            || !(cardStatus.fields.readyForData)
            || (cardStatus.fields.currentState != MCD_CARD_STATE_TRAN))
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "ACMD6 status=%0x", cardStatus.reg);
        hal_SdmmcClose(); pmd_EnablePower(PMD_POWER_SDMMC, FALSE);
        g_mcdStatus = MCD_STATUS_NOTPRESENT;

        MCD_CS_EXIT;
        MCD_PROFILE_FUNCTION_EXIT(mcd_Open);
        return MCD_ERR;
    }

    // Configure the controller to use that many lines.
    hal_SdmmcSetDataWidth(dataBusWidth);
#ifdef _USE_HIGH_SPEED_MODE_
    if(g_mcdCardIsSdhc == TRUE)
    {
	errStatus = mcd_SdmmcModeSwitch(0x80fffff1);
    if (errStatus != MCD_ERR_NO)
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "Switch to High Speed Failed");
    }
	else
	{
	   // extern int hal_cnt_divider;
		//hal_cnt_divider = 0;
	}
    }
#endif
    // Configure the block lenght
    errStatus = mcd_SdmmcSendCmd(HAL_SDMMC_CMD_SET_BLOCKLEN, g_mcdBlockLen, response, FALSE);
    if (errStatus != MCD_ERR_NO)
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "Because Set Block Length, Initialize Failed");
        hal_SdmmcClose(); pmd_EnablePower(PMD_POWER_SDMMC, FALSE);
        g_mcdStatus = MCD_STATUS_NOTPRESENT;

        MCD_CS_EXIT;
        MCD_PROFILE_FUNCTION_EXIT(mcd_Open);
        return errStatus;
    }

    // That command returns the card status, in tran state ?
    cardStatus.reg = response[0];
    {
        MCD_CARD_STATUS_T expectedCardStatus;
        expectedCardStatus.reg  = 0;
        expectedCardStatus.fields.readyForData = 1;
        expectedCardStatus.fields.currentState = MCD_CARD_STATE_TRAN;

        if (cardStatus.reg != expectedCardStatus.reg)
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "CMD16 status=%0x", cardStatus.reg);
            hal_SdmmcClose(); pmd_EnablePower(PMD_POWER_SDMMC, FALSE);
            g_mcdStatus = MCD_STATUS_NOTPRESENT;

            MCD_CS_EXIT;
            MCD_PROFILE_FUNCTION_EXIT(mcd_Open);
            return MCD_ERR;
        }
    }

    hal_SdmmcEnterDataTransferMode();


    // Set the clock of the SD bus for the fastest possible rate.

    g_mcdSdmmcFreq = MCD_CARD_V1 == mcdVer ? 6000000 : 20000000;

    // if(g_mcdCardIsSdhc == TRUE) g_mcdSdmmcFreq = 40000000; // max 50M to ver 2.0 card

    if(g_small_card == TRUE)    g_mcdSdmmcFreq = 10000000;
    hal_SdmmcSetClk(g_mcdSdmmcFreq);

    g_mcdLatestCsd = *mcdCsd;
    g_mcdStatus = MCD_STATUS_OPEN;
    hal_SdmmcSleep();
    g_mcdVer = mcdVer;
    MCD_CS_EXIT;
    MCD_PROFILE_FUNCTION_EXIT(mcd_Open);
    return MCD_ERR_NO;
}


PUBLIC VOID mcd_sdmmc_Close_all_volt(VOID)
{
    MCD_TRACE(TSTDOUT, 0, "mcd: close all");
    hal_SdmmcClose();
    pmd_EnablePower(PMD_POWER_SDMMC, FALSE);
    mcd_hal_delay_sd1(1800);
}
PUBLIC VOID mcd_sdmmc_rst_volt(VOID)
{
    // hal_HstSendEvent(0x7788aaa0);
    mcd_sdmmc_Close_all_volt();
    pmd_EnablePower(PMD_POWER_SDMMC, TRUE);
    mcd_hal_delay_sd1(580);
}

// =============================================================================
// mcd_Close
// -----------------------------------------------------------------------------
/// Close MCD.
///
/// To be called at the end of the operations
/// @return MCD_ERR_NO if a response with a good crc was received,
///         MCD_ERR_CARD_NO_RESPONSE if no reponse was received within the
/// driver configured timeout.
///          MCD_ERR_CARD_RESPONSE_BAD_CRC if the received response presented
///  a bad CRC.
///         MCD_ERR_CARD_TIMEOUT if the card timedout during procedure.
// =============================================================================
PUBLIC MCD_ERR_T mcd_Close(MCD_CARD_ID mcdId)
{
    MCD_TRACE(TSTDOUT, 0, "mcd_Close");
    g_small_card = FALSE;
    if (g_mcdSemaphore == 0xFF)
    {
        MCD_TRACE(TSTDOUT, 0, "mcd_Close: Never opened before");
        return MCD_ERR_NO;
    }

    MCD_CS_ENTER;

    MCD_ERR_T errStatus = MCD_ERR_NO;

    MCD_PROFILE_FUNCTION_ENTER(mcd_Close);

    // Don't close the MCD driver if a transfer is in progress,
    // and be definitive about it:
    if (hal_SdmmcIsBusy() == TRUE)
    {
        MCD_TRACE(TSTDOUT, 0, "MCD: Attempt to close MCD while a transfer is in progress");
    }

    hal_SdmmcWakeUp();

    // Brutal force stop current transfer, if any.
    hal_SdmmcStopTransfer(&g_mcdSdmmcTransfer);

    // Close the SDMMC module
    hal_SdmmcClose();

    g_mcdStatus = MCD_STATUS_NOTOPEN_PRESENT; // without GPIO
    mcd_CardDetectUpdateStatus(); // Test GPIO for card detect

    pmd_EnablePower(PMD_POWER_SDMMC, FALSE);
    //mcd_hal_delay(800);

    MCD_CS_EXIT;
    MCD_PROFILE_FUNCTION_EXIT(mcd_Close);
    return errStatus;
}

//=============================================================================
// mcd_SdmmcTranState
//-----------------------------------------------------------------------------
/// Blocking function checking that the card is in the transfer state,
/// acknowledging thus that, for example, end of transmission.
/// @param iter Number of attempt to be made.
/// @param duration Sleeping time before the next attempt (in sys ticks).
//=============================================================================
PRIVATE MCD_ERR_T mcd_SdmmcTranState(UINT32 iter)
{
    UINT32 cardResponse[4] = {0, 0, 0, 0};
    MCD_ERR_T errStatus = MCD_ERR_NO;
    MCD_CARD_STATUS_T cardStatusTranState;
    // Using reg to clear all bit of the bitfields that are not
    // explicitly set.
    cardStatusTranState.reg = 0;
    cardStatusTranState.fields.readyForData = 1;
    cardStatusTranState.fields.currentState = MCD_CARD_STATE_TRAN;
    UINT32 startTime = hal_TimGetUpTime();
    UINT32 time_out;

    while(1)
    {
        errStatus = mcd_SdmmcSendCmd(HAL_SDMMC_CMD_SEND_STATUS, g_mcdRcaReg, cardResponse, FALSE);
        if (errStatus != MCD_ERR_NO)
        {
            // error while sending the command
            MCD_TRACE(MCD_INFO_TRC, 0, "Sd Tran Read Aft State error! err nb:%d", errStatus);
            return MCD_ERR;
        }
        else if (cardResponse[0] == cardStatusTranState.reg)
        {
            // the status is the expected one - success
            return MCD_ERR_NO;
        }
        else
        {
            // try again
            // check for timeout
            time_out =  (MCD_CARD_V1 == g_mcdVer) ? MCD_CMD_TIMEOUT_V1:MCD_CMD_TIMEOUT_V2;
            if (hal_TimGetUpTime() - startTime >  time_out )
            {
                MCD_TRACE(MCD_INFO_TRC, 0, "Sd Tran don't finished");
                MCD_TRACE(MCD_INFO_TRC, 0, "current state =%0x", cardResponse[0]);
                return MCD_ERR;
            }
        }
		#ifdef _USE_HIGH_SPEED_MODE_
        sxr_Sleep(2);
		#endif
    }
}

PUBLIC MCD_ERR_T mcd_SdmmcMultBlockErase(UINT32 blockAddr,UINT32 blockSize)
{
    UINT32 cardResponse[4] = {0, 0, 0, 0};
	UINT32 cmd_time_out;

	cmd_time_out =  (MCD_CARD_V1 == g_mcdVer) ? MCD_CMD_TIMEOUT_V1 : MCD_CMD_TIMEOUT_V2;

	if (MCD_ERR_NO != mcd_SdmmcTranState(cmd_time_out))
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "Erase on Sdmmc while not in Tran state");
        return MCD_ERR_CARD_TIMEOUT;
    }

	mcd_SdmmcSendCmd(HAL_SDMMC_CMD_ERASE_BLK_START, blockAddr, cardResponse, FALSE);
	mcd_SdmmcSendCmd(HAL_SDMMC_CMD_ERASE_BLK_END, blockAddr+blockSize, cardResponse, FALSE);
	mcd_SdmmcSendCmd(HAL_SDMMC_CMD_MULTI_BLK_ERASE, 0, cardResponse, FALSE);

	if (MCD_ERR_NO != mcd_SdmmcTranState(MCD_ERASE_TIMEOUT))
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "Erase timeout");
        return MCD_ERR_CARD_TIMEOUT;
    }

    return MCD_ERR_NO;
}


// send to high mode
MCD_ERR_T  mcd_SdmmcModeSwitch(UINT32 arg)
{
    int i;
	UINT32 startTime,write_time_out;
	UINT8 cardResponse[64] = {0};

	write_time_out =  (MCD_CARD_V1 == g_mcdVer) ? MCD_WRITE_TIMEOUT_V1:MCD_WRITE_TIMEOUT_V2;
	if (MCD_ERR_NO != mcd_SdmmcTranState(write_time_out))
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "Write on Sdmmc while not in Tran state");
        return MCD_ERR_CARD_TIMEOUT;
    }

	mcd_SdmmcSendCmd(HAL_SDMMC_CMD_SWITCH,arg, cardResponse, FALSE);

    g_mcdSdmmcTransfer.sysMemAddr = cardResponse;
    g_mcdSdmmcTransfer.blockNum   = 1;
    g_mcdSdmmcTransfer.blockSize  = 64;
    g_mcdSdmmcTransfer.direction  = HAL_SDMMC_DIRECTION_READ;

	hal_SdmmcTransfer(&g_mcdSdmmcTransfer);

    startTime = hal_TimGetUpTime();
    while(!hal_SdmmcTransferDone())
    {
        if (hal_TimGetUpTime() - startTime >  (write_time_out))
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "Switch on Sdmmc timeout");
            hal_SdmmcStopTransfer(&g_mcdSdmmcTransfer);
        }
    }

    return MCD_ERR_NO;
}



//=============================================================================
// mcd_SdmmcMultBlockWrite
//-----------------------------------------------------------------------------
/// Write one or a bunch of data blocks.
/// @param blockAddr Address on the card where to write data.
/// @param pWrite Pointer towards the buffer of data to write.
/// @param blockNum Number of blocks to write.
//=============================================================================
PRIVATE MCD_ERR_T mcd_SdmmcMultBlockWrite(UINT8* blockAddr, CONST UINT8* pWrite, UINT32 blockNum)
{
    UINT32 cardResponse[4] = {0, 0, 0, 0};
    MCD_ERR_T errStatus = MCD_ERR_NO;
    MCD_CARD_STATUS_T cardStatusTranState;
    // Using reg to clear all bit of the bitfields that are not
    // explicitly set.
    cardStatusTranState.reg = 0;
    cardStatusTranState.fields.readyForData = 1;
    cardStatusTranState.fields.currentState = MCD_CARD_STATE_TRAN;

    MCD_CARD_STATUS_T cardStatusPreErasedState;
    cardStatusPreErasedState.reg    = 0;
    cardStatusPreErasedState.fields.appCmd       = 1;
    cardStatusPreErasedState.fields.readyForData = 1;
    cardStatusPreErasedState.fields.currentState = MCD_CARD_STATE_TRAN;

    MCD_CARD_STATUS_T cardStatusResponse = {0,};

    UINT32 startTime = 0;

    HAL_SDMMC_CMD_T writeCmd;
    UINT32 tran_time_out;
    UINT32 write_time_out;

    g_mcdSdmmcTransfer.sysMemAddr = (UINT8*) pWrite;
    g_mcdSdmmcTransfer.sdCardAddr = blockAddr;
    g_mcdSdmmcTransfer.blockNum   = blockNum;
    g_mcdSdmmcTransfer.blockSize  = g_mcdBlockLen;
    g_mcdSdmmcTransfer.direction  = HAL_SDMMC_DIRECTION_WRITE;


// FIXME find what the heck is that !:
// program_right_num=0;

    // Check buffer.
    MCD_ASSERT(pWrite != NULL, "SDMMC write: Buffer is NULL");
    MCD_ASSERT(((UINT32)pWrite & 0x3) ==0,
               "SDMMC write: buffer is not aligned! addr=%08x", pWrite);
    MCD_ASSERT(blockNum>=1 && blockNum<= MCD_MAX_BLOCK_NUM,
               "Block Num is overflow");

    // Check that the card is in tran (Transmission) state.
    tran_time_out =  (MCD_CARD_V1 == g_mcdVer) ? MCD_TRAN_TIMEOUT_V1:MCD_TRAN_TIMEOUT_V2;
    if (MCD_ERR_NO != mcd_SdmmcTranState(tran_time_out))
        // 5200000, 0, initially, that is to say rougly 0.1 sec ?
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "Write on Sdmmc while not in Tran state");
        return MCD_ERR_CARD_TIMEOUT;
    }

    // The command for single block or multiple blocks are differents
    if (blockNum == 1)
    {
        writeCmd = HAL_SDMMC_CMD_WRITE_SINGLE_BLOCK;
    }
    else
    {
        writeCmd = HAL_SDMMC_CMD_WRITE_MULT_BLOCK;
    }

    // PreErasing, to accelerate the multi-block write operations
    if (blockNum >1)
    {
        if(MCD_ERR == mcd_SdmmcSendCmd(HAL_SDMMC_CMD_SET_WR_BLK_COUNT, blockNum, cardResponse, FALSE))
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "Set Pre-erase Failed");
            return MCD_ERR;
        }

        // Advance compatibility,to support 1.0 t-flash.
        if (cardResponse[0] != cardStatusPreErasedState.reg)
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "warning ACMD23 status=%0x", cardResponse[0]);
            // cardResponse[0] = cardStatusPreErasedState.reg;
            // return MCD_ERR;
        }
    }


    // Initiate data migration through Ifc.
    if (hal_SdmmcTransfer(&g_mcdSdmmcTransfer) != HAL_ERR_NO)
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "write sd no ifc channel");
        return MCD_ERR_DMA_BUSY;
    }

    // Initiate data migration of multiple blocks through SD bus.
    errStatus = mcd_SdmmcSendCmd(writeCmd,
                                 (UINT32)blockAddr,
                                 cardResponse,
                                 FALSE);
    if (errStatus != MCD_ERR_NO)
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "Set sd write had error");
        hal_SdmmcStopTransfer(&g_mcdSdmmcTransfer);
        return MCD_ERR_CMD;
    }

    cardStatusResponse.reg = cardResponse[0] ;
    // Check for error, by listing all valid states
    // TODO: FIXME, more states could be legal here
    // The sixteen uper bits are error bits: they all must be null
    // (Physical Spec V.2, p71)
    if ((cardResponse[0] != cardStatusTranState.reg)
            && !((cardStatusResponse.fields.readyForData == 1)
                 && (cardStatusResponse.fields.currentState == MCD_CARD_STATE_RCV)
                 && ((cardStatusResponse.reg&0xFFFF0000) == 0))
            && !(cardStatusResponse.fields.currentState == MCD_CARD_STATE_RCV
                 && ((cardStatusResponse.reg&0xFFFF0000) == 0))
            && !(cardStatusResponse.fields.currentState == MCD_CARD_STATE_PRG
                 && ((cardStatusResponse.reg&0xFFFF0000) == 0))
       )
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "Write block,Card Reponse: %x", cardResponse[0]);
        hal_SdmmcStopTransfer(&g_mcdSdmmcTransfer);
        return MCD_ERR;
    }


    // Wait
    startTime = hal_TimGetUpTime();
    write_time_out =  (MCD_CARD_V1 == g_mcdVer) ? MCD_WRITE_TIMEOUT_V1:MCD_WRITE_TIMEOUT_V2;
    while(!hal_SdmmcTransferDone())
    {
        if (hal_TimGetUpTime() - startTime >  (write_time_out*blockNum))
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "Write on Sdmmc timeout");
            // Abort the transfert.
            hal_SdmmcStopTransfer(&g_mcdSdmmcTransfer);
            return MCD_ERR_CARD_TIMEOUT;
        }
    }


    // Nota: CMD12 (stop transfer) is automatically
    // sent by the SDMMC controller.

    if (mcd_SdmmcWriteCheckCrc() != MCD_ERR_NO)
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "SDMMC Write error");
        return MCD_ERR;
    }

    // Check that the card is in tran (Transmission) state.
    if (MCD_ERR_NO != mcd_SdmmcTranState(tran_time_out))
        // 5200000, 0, initially, that is to say rougly 0.1 sec ?
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "Write on Sdmmc while not in Tran state");
        return MCD_ERR_CARD_TIMEOUT;
    }
    return MCD_ERR_NO;

}




//=============================================================================
// mcd_SdmmcMultBlockRead
//-----------------------------------------------------------------------------
/// Read one or a bunch of data blocks.
/// @param blockAddr Address on the card where to read data.
/// @param pRead Pointer towards the buffer of data to read.
/// @param blockNum Number of blocks to read.
//=============================================================================
PRIVATE MCD_ERR_T mcd_SdmmcMultBlockRead(UINT8* blockAddr, UINT8* pRead, UINT32 blockNum)
{
    UINT32                  cardResponse[4]     = {0, 0, 0, 0};
    MCD_ERR_T               errStatus           = MCD_ERR_NO;
    HAL_SDMMC_CMD_T         readCmd;
    MCD_CARD_STATUS_T cardStatusTranState;
    // Using reg to clear all bit of the bitfields that are not
    // explicitly set.
    cardStatusTranState.reg = 0;
    cardStatusTranState.fields.readyForData = 1;
    cardStatusTranState.fields.currentState = MCD_CARD_STATE_TRAN;

    UINT32 startTime=0;
    UINT32 tran_time_out;
    UINT32 read_time_out;

    g_mcdSdmmcTransfer.sysMemAddr = (UINT8*) pRead;
    g_mcdSdmmcTransfer.sdCardAddr = blockAddr;
    g_mcdSdmmcTransfer.blockNum   = blockNum;
    g_mcdSdmmcTransfer.blockSize  = g_mcdBlockLen;
    g_mcdSdmmcTransfer.direction  = HAL_SDMMC_DIRECTION_READ;

    // Check buffer.
    MCD_ASSERT(pRead != NULL, "SDMMC write: Buffer is NULL");
    MCD_ASSERT(((UINT32)pRead & 0x3) ==0, "SDMMC write: buffer is not aligned");
    MCD_ASSERT(blockNum>=1 && blockNum<= MCD_MAX_BLOCK_NUM,
               "Block Num is overflow");

    // Command are different for reading one or several blocks of data
    if (blockNum == 1)
    {
        readCmd = HAL_SDMMC_CMD_READ_SINGLE_BLOCK;
    }
    else
    {
        readCmd = HAL_SDMMC_CMD_READ_MULT_BLOCK;
    }

    // Initiate data migration through Ifc.
    if (hal_SdmmcTransfer(&g_mcdSdmmcTransfer) != HAL_ERR_NO)
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "write sd no ifc channel");
        return MCD_ERR_DMA_BUSY;
    }

    // Initiate data migration of multiple blocks through SD bus.
    errStatus = mcd_SdmmcSendCmd(readCmd,
                                 (UINT32)blockAddr,
                                 cardResponse,
                                 FALSE);
    if (errStatus != MCD_ERR_NO)
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "Set sd write had error");
        hal_SdmmcStopTransfer(&g_mcdSdmmcTransfer);
        return MCD_ERR_CMD;
    }

    if (cardResponse[0] != cardStatusTranState.reg)
    {
        MCD_TRACE(TSTDOUT, 0, "CMD%d status=0x%x",readCmd,cardResponse[0] );
        // hal_SdmmcStopTransfer(&g_mcdSdmmcTransfer);
        // return MCD_ERR;
    }

    // Wait
    startTime = hal_TimGetUpTime();
    read_time_out =  (MCD_CARD_V1 == g_mcdVer) ? MCD_READ_TIMEOUT_V1:MCD_READ_TIMEOUT_V2;
    while(!hal_SdmmcTransferDone())
    {
        if (hal_TimGetUpTime() - startTime > (read_time_out))
        {
            MCD_TRACE(TSTDOUT, 0, "Read on Sdmmc timeout");
            // Abort the transfert.
            hal_SdmmcStopTransfer(&g_mcdSdmmcTransfer);
            return MCD_ERR_CARD_TIMEOUT;
        }
    }

    // Nota: CMD12 (stop transfer) is automatically
    // sent by the SDMMC controller.

    if (mcd_SdmmcReadCheckCrc() != MCD_ERR_NO)
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "sdc read state error");
        return MCD_ERR;
    }

    tran_time_out = (MCD_CARD_V1 == g_mcdVer) ? MCD_TRAN_TIMEOUT_V1:MCD_TRAN_TIMEOUT_V2;
    // Check that the card is in tran (Transmission) state.
    if (MCD_ERR_NO != mcd_SdmmcTranState(tran_time_out))
        // 5200000, 0, initially, that is to say rougly 0.1 sec ?
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "Read on Sdmmc while not in Tran state");
        return MCD_ERR_CARD_TIMEOUT;
    }

    // Flush Cache
    hal_SysInvalidateCache(pRead, blockNum * g_mcdBlockLen);

    return MCD_ERR_NO;
}



// =============================================================================
// mcd_Write
// -----------------------------------------------------------------------------
/// @brief Write a block of data to MMC.
///
/// This function is used to write blocks of data on the MMC.
/// @param startAddr Start Adress  of the SD memory block where the
/// data will be written
/// @param blockWr Pointer to the block of data to write. Must be aligned
/// on a 32 bits boundary.
/// @param size Number of bytes to write. Must be an interger multiple of the
/// sector size of the card
// =============================================================================
PUBLIC MCD_ERR_T mcd_Write(UINT32 startAddr, CONST UINT8* blockWr, UINT32 size)
{
    MCD_CS_ENTER;
    UINT8*      tmpAdd  = NULL;
    MCD_ERR_T   value   = MCD_ERR_NO;
    UINT32 i = 0;

    MCD_PROFILE_FUNCTION_ENTER(mcd_Write);

    MCD_ASSERT(g_mcdBlockLen != 0, "mcd_Write called before a successful mcd_Open");
    if (g_mcdCardIsSdhc == FALSE)
    {
        // MCD_ASSERT(startAddr % g_mcdBlockLen == 0, "write card address is not aligned");
    }
    MCD_ASSERT((size % g_mcdBlockLen) == 0, "mcd_Write size (%d) is not a multiple of"
               "the block size (%d)",
               size, g_mcdBlockLen);
    if(!mcd_CardDetectUpdateStatus())
    {
        MCD_PROFILE_FUNCTION_EXIT(mcd_Write);
        MCD_CS_EXIT;
        return MCD_ERR_NO_CARD;
    }

    hal_SdmmcWakeUp();
    // Addresses are block number for high capacity card
    if (g_mcdCardIsSdhc)
    {
        tmpAdd = (UINT8*) (startAddr);
    }
    else
    {
        tmpAdd = (UINT8*) (startAddr*g_mcdBlockLen);
    }
    if(MCD_CARD_V1 == g_mcdVer)
    {
        for(i = 0; i < size/g_mcdBlockLen; i++)
        {
            value = mcd_SdmmcMultBlockWrite(tmpAdd + i*g_mcdBlockLen, blockWr + i*g_mcdBlockLen, 1);
            if(value != MCD_ERR_NO)
            {
                break;
            }
        }
    }
    else
    {
        value = mcd_SdmmcMultBlockWrite(tmpAdd, blockWr, size/g_mcdBlockLen);
    }

    hal_SdmmcSleep();
    MCD_PROFILE_FUNCTION_EXIT(mcd_Write);
    MCD_CS_EXIT;
    return value;
}



// =============================================================================
// mcd_Read
// -----------------------------------------------------------------------------
/// @brief Read using pattern mode.
/// @ param startAddr: of the SD memory block where the data
/// will be read
/// @param blockRd Pointer to the buffer where the data will be stored. Must be aligned
/// on a 32 bits boundary.
/// @param size Number of bytes to read. Must be an interger multiple of the
/// sector size of the card.
// =============================================================================
PUBLIC MCD_ERR_T mcd_Read(UINT32 startAddr, UINT8* blockRd, UINT32 size)
{
    MCD_CS_ENTER;
    UINT8*      tmpAdd  = NULL;
    MCD_ERR_T   value   = MCD_ERR_NO;

    MCD_PROFILE_FUNCTION_ENTER(mcd_Read);
    MCD_ASSERT(g_mcdBlockLen != 0, "mcd_Read called before a successful mcd_Open_V1 or mcd_Open_V2");

    if (g_mcdCardIsSdhc == FALSE)
    {
        // MCD_ASSERT(startAddr % g_mcdBlockLen == 0,"read card address is not aligned");
    }


    MCD_ASSERT((size % g_mcdBlockLen) == 0, "mcd_Read size (%d) is not a multiple of"
               "the block size (%d)",
               size, g_mcdBlockLen);

    if(!mcd_CardDetectUpdateStatus())
    {
        MCD_PROFILE_FUNCTION_EXIT(mcd_Read);
        MCD_CS_EXIT;
        return MCD_ERR_NO_CARD;
    }

    hal_SdmmcWakeUp();
    // Addresses are block number for high capacity card
    if (g_mcdCardIsSdhc)
    {
        tmpAdd = (UINT8*) (startAddr);
    }
    else
    {
        tmpAdd = (UINT8*) (startAddr*g_mcdBlockLen);
    }

    value = mcd_SdmmcMultBlockRead(tmpAdd, blockRd, size/g_mcdBlockLen);

    hal_SdmmcSleep();

    MCD_PROFILE_FUNCTION_EXIT(mcd_Read);
    MCD_CS_EXIT;
    return value;

}


BOOL mcd_IsHighCapacityCard(VOID)
{
    if(g_mcdCardIsSdhc == TRUE)
    {
        return TRUE;
    }
    return FALSE;

}

#if 0
VOID mcd_get_MaxClk()
{


    // Set speed to max allowable speed
    // Calculate Divider for 78MHz
    // Rate = SysClk / ((clkDivider+1)*2)
    // clkDivider = (SysClk/Rate)/2-1
    {
        UINT32 rate_unit = (mcdCsd->tranSpeed & 0x7);
        UINT32 rate_val = (mcdCsd->tranSpeed >>3) & 0xf;
        UINT32 rate;
        UINT8 clkdivider=1;
        rate_val = hal_CalculateVal(rate_val);
        if ( rate_val == 0 )
        {
            MCD_PROFILE_FUNCTION_EXIT(mcd_Open);
            hal_SpiDeActivateCs(HAL_SPI, g_mcdSpiCs);
            // Indicate that SPI is free
            // g_mcdSpiFree = TRUE;
            hal_SpiClose(HAL_SPI, g_mcdSpiCs);
            return MCD_ERR_CSD;
        }
        switch (rate_unit)
        {
            case 0: // Unit 100kHz
                rate_unit = 10;
                break;
            case 1: // Unit 1MHz
                rate_unit = 100;
                break;
            case 2: // Unit 10MHz
                rate_unit = 1000;
                break;
            case 3: // Unit 100MHz
                rate_unit = 10000;
                break;
            default: // Ohters are reserved
                MCD_PROFILE_FUNCTION_EXIT(mcd_Open);
                hal_SpiDeActivateCs(HAL_SPI, g_mcdSpiCs);
                // Indicate that SPI is free
                // g_mcdSpiFree = TRUE;
                return MCD_ERR_CSD;
        }
        rate = rate_val*rate_unit;
        MCD_TRACE(MCD_INFO_TRC, 0, "MMC/SD Speed Rating: %d kHz", rate);
        clkdivider = (78000/rate)/2;
        // MCD_TRACE(MCD_INFO_TRC, 0, " Clk divider: %d", clkdivider);
        // SD max frequency: 1.1Mhz: 1100000
        g_spiConfig.spiFreq = 1100000;
        // MMC and Tflash work at minimal frequency of 1.5Mhz-TFlash=>4Mhz
        if (IsMMC == TRUE)
        {
            g_spiConfig.spiFreq = 2000000;
        }
        //MCD_TRACE(MCD_INFO_TRC, 0, "MMC/SD Used Speed @52MHz: %d kHz", 52000/(g_spiConfig.spiFreq+1)/2);
        //MCD_TRACE(MCD_INFO_TRC, 0,"MMC/SD Used Speed @78MHz: %d kHz",78000/(g_spiConfig.spiFreq+1)/2);
        MCD_TRACE(MCD_INFO_TRC, 0,"MMC/SD Sector size : %d bytes",1<<mcdCsd->readBlLen);
    }
}
#endif

#if 0
VOID test_open()
{
    hal_HstSendEvent(0xaabb1111);
    MCD_TRACE(TSTDOUT, 0, "open mcd");
    MCD_CSD_T mcdCsd;
    mcd_Open(&mcdCsd,MCD_CARD_V2);

}
VOID test_mcd_Read(UINT32 startAddr, UINT8* rdDataBlock, UINT32 blockLength)
{
    mcd_Read(startAddr,rdDataBlock,blockLength);
}

VOID test_mcd_Write(UINT32 blockStartAddr, CONST UINT8* blockWr, UINT32 blockLength)
{

    mcd_Write( blockStartAddr,  blockWr, blockLength);
}

VOID test_close()
{

    mcd_Close(0);

}

VOID test_reopenspi()
{


}
VOID test_mcd_Write_4block(UINT32 blockStartAddr, CONST UINT8* blockWr, UINT32 blockLength)
{
    //mcd_Write_mutil(blockStartAddr,  blockWr, blockLength);
}

#define HAL_UNCACHED_DATA __attribute__((section(".ucdata")))

UINT8 HAL_UNCACHED_DATA  test_m_w[1024];
UINT8  HAL_UNCACHED_DATA test_m_r[1024];

VOID test_mcd_card(VOID)
{
    UINT32 k;
    hal_HstSendEvent(0xcccccccc);
    for(k=0; k<1024; k++)
    {
        test_m_w[k]=k;
    }
    test_open();
    test_mcd_Write(0x1000,test_m_w,1024);
    test_mcd_Read(0x1000,test_m_r,1024);
    for(k=0; k<1024; k++)
    {
        if(test_m_w[k]!=test_m_r[k])
        {
            hal_HstSendEvent(0xffffcdcd);
			hal_HstSendEvent(k);
			hal_HstSendEvent(test_m_w[k]);
			hal_HstSendEvent(test_m_r[k]);
            break;
        }
    }
    hal_HstSendEvent(0xffffcd00);

    for(k=0; k<1024; k++)
    {
        test_m_w[k]=0x5a5a5a5a;
    }
    test_mcd_Write(0x2000,test_m_w,1024);
    test_mcd_Read(0x2000,test_m_r,1024);
    for(k=0; k<1024; k++)
    {
        if(test_m_w[k]!=test_m_r[k])
        {
            hal_HstSendEvent(0xfff0cdcd);
			hal_HstSendEvent(k);
			hal_HstSendEvent(test_m_w[k]);
			hal_HstSendEvent(test_m_r[k]);
            break;
        }
    }
    hal_HstSendEvent(0xffffcd01);
    mcd_Close(0);
    while(1) {};

}

#endif



