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


#include "string.h"
#include "cs_types.h"
#include "sxr_tls.h"
#include "mcdp_debug.h"
#include "mcd_m.h"
#include "hal_spi.h"
#include "mcd_config.h"
#include "tgt_mcd_cfg.h"

#include "hal_host.h"

#include "sxr_mem.h"


// =============================================================================
//
// -----------------------------------------------------------------------------
// =============================================================================

#error "MCD SPI is deprecated and not really updated anymore... don't use it."

// =============================================================================
//  MACROS
// =============================================================================

#define MAX_BLOCK_SIZE      2048
#define RDCMD_BUFFER_LEN    1024
#define WRCMD_BUFFER_LEN    (MAX_BLOCK_SIZE + 64)
#define SLEEP_DURATION_OP   20000
#define SLEEP_DURATION_RD   20
#define SLEEP_DURATION_READ 5000
#define SLEEP_DURATION_WR   5000
#define SLEEP_DURATION_STD  5
#define SLEEP_DURATION      0
#define SLEEP_DURATION_WR_POLL   16384/1000  // (1 millisecond)
#define SLEEP_DURATION_CMD  16384/100        // (10 millisecond)
#define MCD_WRITE_TIMEOUT   5000             // max timeout for a write is
//  5s = 5000 *SLEEP_DURATION_WR_POLL

// The number of retry does not depend on the clock divider.
#define CMD1_RETRY_NB       128   // the card is supposed to answer within 1s
//  - Max wait is 128 * 10ms=1,28s
#define CMD0_RETRY_NB       5     // Retry 5 times max
#define ACMD41_RETRY_NB     128   // the card is supposed to answer within 1s
//  - Max wait is 128 * 10ms=1,28s

#define CMDFF_RETRY_NB      10
#define MCD_COMMAND_BUFFER_SIZE 16


// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================
PRIVATE VOLATILE UINT8       g_SpiMcdRxBuf[RDCMD_BUFFER_LEN]
__attribute__((section(".ucbss")));
PRIVATE VOLATILE UINT8       g_SpiMcdRdCmdBuf[RDCMD_BUFFER_LEN]
__attribute__((section(".ucbss")));
PRIVATE VOLATILE UINT8       g_SpiMcdWrCmdBuf[WRCMD_BUFFER_LEN]
__attribute__((section(".ucbss")));

PRIVATE UINT8                g_mcdSpiCs      = 0;
PRIVATE UINT16               g_dmaBusy       = 0;
PRIVATE UINT32               g_transferSize  = 0;
PRIVATE UINT16               g_readBlockLen  = 0;
PRIVATE UINT16               g_writeBlockLen = 0;
PRIVATE UINT16               g_blockLen      = 0;
PRIVATE BOOL                 g_mcdSpiFree    = TRUE;

/// HAL SPI CFG
PRIVATE HAL_SPI_CFG_T        g_spiConfig;
/// MCD config
PRIVATE CONST TGT_MCD_CONFIG_T*  g_mcdConfig;


// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
//  mcd_CalculateMult
// -----------------------------------------------------------------------------
/// Convert the C_SIZE_MULT from CSD register to 2^(csdSizeMult+2).
/// @param csdCSizeMult C_SIZE_MULT from CSD register.
/// @return 2^(csdSizeMult+2)
// =============================================================================
PROTECTED UINT16  mcd_CalculateMult(UINT8 csdCSizeMult);


// =============================================================================
// mcd_ConvertBlockLength
// -----------------------------------------------------------------------------
/// Calculate W/R block length.
/// @ param blockLen: WRITE_BL_LEN or READ_BL_LEN from CSDregister.
/// @return 2^WRITE_BL_LEN or 2^READ_BL_LEN
// =============================================================================
PRIVATE UINT16 mcd_ConvertBlockLength(UINT8 blockLen);


// =============================================================================
//  mcd_SendCommand
// -----------------------------------------------------------------------------
/// @brief To send command to the MMC
/// Only valid for commands returning no data and response R1 and R2
// =============================================================================
PRIVATE MCD_ERR_T mcd_SendCommand(UINT8 cmdId, UINT32 argument,UINT16* status,UINT16 dataSize,UINT8** pdata_array)
{
    UINT32 i;

    if (dataSize==0)
    {
        // --------------------------------
        // Transfer is
        // --------------------------------
        // 6 bytes command
        // 1 CRC byte
        // max 8 bytes stuffing
        // max 2 bytes status
        // --------------------------------
        g_transferSize=6+1+8+2;
    }
    else
    {
        // --------------------------------
        // Transfer is
        // --------------------------------
        // 6 bytes command
        // 1 CRC byte
        // max 8 bytes stuffing
        // max 2 bytes status
        // max 8 bytes stuffing
        // data header 1 byte
        // dataSize bytes
        // 2 bytes CRC
        // --------------------------------
        g_transferSize=6+1+8+2+8+1+dataSize+2;
    }

    hal_SpiFlushFifos(HAL_SPI);
    hal_SpiClearRxDmaDone(HAL_SPI);
    hal_SpiClearTxDmaDone(HAL_SPI);

    if (cmdId == 0)
    {
        // CMD0 requires a valid CRC
        // Write 0x40 00 00 00 00 95(CMD0)
        g_SpiMcdRdCmdBuf[0] = 0xff;
        g_SpiMcdRdCmdBuf[1] = 0x40;
        g_SpiMcdRdCmdBuf[2] = 0;
        g_SpiMcdRdCmdBuf[3] = 0;
        g_SpiMcdRdCmdBuf[4] = 0;
        g_SpiMcdRdCmdBuf[5] = 0;
        g_SpiMcdRdCmdBuf[6] = 0x95;
    }
    else if (cmdId == 0xff)
    {
        // Fake FF command, only sending 1s
        // To check the busy state
        g_SpiMcdRdCmdBuf[0] = 0xff;
        g_SpiMcdRdCmdBuf[1] = 0xff;
        g_SpiMcdRdCmdBuf[2] = 0xff;
        g_SpiMcdRdCmdBuf[3] = 0xff;
        g_SpiMcdRdCmdBuf[4] = 0xff;
        g_SpiMcdRdCmdBuf[5] = 0xff;
        g_SpiMcdRdCmdBuf[6] = 0xff;
    }
    else
    {
        // Write 0x40 00 00 00 00 01
        g_SpiMcdRdCmdBuf[0] = 0xff;
        g_SpiMcdRdCmdBuf[1] = 0x40 | cmdId;
        g_SpiMcdRdCmdBuf[2] = (argument & 0xff000000)>>24;
        g_SpiMcdRdCmdBuf[3] = (argument & 0x00ff0000)>>16;
        g_SpiMcdRdCmdBuf[4] = (argument & 0x0000ff00)>>8;
        g_SpiMcdRdCmdBuf[5] = (argument & 0x00);
        // other CMD CRC is ignored - set CRC to 0
        g_SpiMcdRdCmdBuf[6] = 0x01;
    }

    g_dmaBusy=hal_SpiGetData(HAL_SPI, g_mcdSpiCs, (UINT8*) g_SpiMcdRxBuf, g_transferSize);
    if (g_dmaBusy==0)
    {
        return MCD_ERR_DMA_BUSY;
    }

    g_dmaBusy=hal_SpiSendData(HAL_SPI, g_mcdSpiCs, (UINT8*) g_SpiMcdRdCmdBuf, g_transferSize);
    sxr_Sleep(SLEEP_DURATION_STD);
    if (g_dmaBusy == 0)
    {
        // Stop the Rx transfer.
        hal_SpiDeActivateCs(HAL_SPI, g_mcdSpiCs);
        // Indicate that SPI is free
        // g_mcdSpiFree = TRUE;
        return MCD_ERR_DMA_BUSY;
    }

    // Wait until the end of both transfers.
    MCD_PROFILE_FUNCTION_ENTER(mcd_Xxx_WaitTc);
    while (!hal_SpiTxDmaDone(HAL_SPI, g_spiConfig.enabledCS));

    MCD_PROFILE_FUNCTION_EXIT(mcd_Xxx_WaitTc);

    // search for CMD status start bit
    // search for non zero value for busy command (0xff)
    i = 0;
    if ( cmdId == 0xff )
    {
        while (g_SpiMcdRxBuf[i] == 0x00)
        {
            if (i == g_transferSize-1)
            {
                *status = 0x0000;
                return MCD_ERR_CARD_TIMEOUT;
            }
            i++;
        }
    }
    else
    {
        while (g_SpiMcdRxBuf[i] & 0x80)
        {
            if (i == g_transferSize-1)
            {
                *status = 0xffff;
                return MCD_ERR_CARD_TIMEOUT;
            }
            i++;
        }
    }

    *status=g_SpiMcdRxBuf[i] | (g_SpiMcdRxBuf[i+1]<<8);

    if (dataSize==0)
    {
        if(pdata_array != NULL)
            *pdata_array = NULL;
        return MCD_ERR_NO;
    }

    i++;

    // search for data block start bit
    while (g_SpiMcdRxBuf[i++] != 0xfe)
    {
        if (i > g_transferSize-dataSize)
        {
            *pdata_array = NULL;
            return MCD_ERR_CARD_TIMEOUT;
        }
    }

    *pdata_array = (UINT8*) (g_SpiMcdRxBuf+i);
    sxr_Sleep(SLEEP_DURATION);

    return MCD_ERR_NO;
}



// =============================================================================
// mcd_ReadCsd
// -----------------------------------------------------------------------------
/// @brief Read the MMC CSD register
/// @param mcdCsd Pointer to the structure where the MMC CSD register info
/// are going to be written.
// =============================================================================
PUBLIC MCD_ERR_T mcd_ReadCsd(MCD_CSD_T* mcdCsd)
{
    UINT16 i = 0;
    MCD_ERR_T cmd_error;
    UINT16 cmdStatus;
    UINT8* data_array;

    MCD_PROFILE_FUNCTION_ENTER(mcd_ReadCsd);

    cmd_error = mcd_SendCommand(9,0,&cmdStatus,16,&data_array);

    // bit 127:120
    mcdCsd->csdStructure = (data_array[i]>>6)&0x3;
    mcdCsd->specVers = (data_array[i++]>>2)&0xf;
    // bit 119:112
    mcdCsd->taac = data_array[i++];
    // bit 111:104
    mcdCsd->nsac = data_array[i++];
    // bit 103:96
    mcdCsd->tranSpeed = data_array[i++];
    // bit 95:88
    mcdCsd->ccc = (UINT16)(data_array[i++])<<4;
    // bit 87:80
    mcdCsd->ccc |= ((data_array[i]>>4)&0xf);
    mcdCsd->readBlLen = data_array[i++]&0xf;
    // bit 79:72
    mcdCsd->readBlPartial = (data_array[i]>>7)&0x1;
    mcdCsd->writeBlkMisalign = (data_array[i]>>6)&0x1;
    mcdCsd->readBlkMisalign = (data_array[i]>>5)&0x1;
    mcdCsd->dsrImp = (data_array[i]>>4)&0x1;
    mcdCsd->cSize = (UINT16)((data_array[i++]&0x3))<<10;
    // bit 71:64
    mcdCsd->cSize |= (UINT16)(data_array[i++])<<2;
    // bit 63:56
    mcdCsd->cSize |= ((data_array[i]>>6)&0x3);
    mcdCsd->vddRCurrMin = (data_array[i]>>3)&0x7;
    mcdCsd->vddRCurrMax = data_array[i++]&0x7;
    // bit 55:48
    mcdCsd->vddWCurrMin = (data_array[i]>>5)&0x7;
    mcdCsd->vddWCurrMax = (data_array[i]>>2)&0x7;
    mcdCsd->cSizeMult = (data_array[i++]&0x3)<<1;
    // bit 47:40
    mcdCsd->cSizeMult |= ((data_array[i]>>7)&0x1);
    mcdCsd->eraseGrpSize = (data_array[i]>>2)&0x1f;
    mcdCsd->eraseGrpMult = (data_array[i++] &0x3)<<3;
    // bit 39:32
    mcdCsd->eraseGrpMult |= ((data_array[i]>>5)&0x7);
    mcdCsd->wpGrpSize = data_array[i++]&0x1f;
    // bit 31:24
    mcdCsd->wpGrpEnable = ((data_array[i]>>7)&0x1);
    mcdCsd->defaultEcc = ((data_array[i]>>5)&0x3);
    mcdCsd->r2wFactor = ((data_array[i]>>2)&0x7);
    mcdCsd->writeBlLen = (data_array[i++]&0x3)<<2;
    // bit 23:16
    mcdCsd->writeBlLen |= ((data_array[i]>>6)&0x3);
    mcdCsd->writeBlPartial = ((data_array[i]>>5)&0x1);
    mcdCsd->contentProtApp = data_array[i++]&0x1;
    // bit 15:8
    mcdCsd->fileFormatGrp = (data_array[i]>>7)&0x1;
    mcdCsd->copy = (data_array[i]>>6)&0x1;
    mcdCsd->permWriteProtect = (data_array[i]>>5)&0x1;
    mcdCsd->tmpWriteProtect = (data_array[i]>>4)&0x1;
    mcdCsd->fileFormat = (data_array[i]>>2)&0x3;
    mcdCsd->ecc = data_array[i++]&0x3;
    // bit 7:0
    mcdCsd->crc = (data_array[i]>>1)&0x7f;

    MCD_PROFILE_FUNCTION_EXIT(mcd_ReadCsd);

    MCD_TRACE(MCD_INFO_TRC, 0, "Read block partial: %d",  mcdCsd->readBlPartial);
    MCD_TRACE(MCD_INFO_TRC, 0, "Write block partial: %d",  mcdCsd->writeBlPartial);
    MCD_TRACE(MCD_INFO_TRC, 0, "Max read block lentgh: %x",  mcdCsd->readBlLen);
    MCD_TRACE(MCD_INFO_TRC, 0, "Max write block length: %x",  mcdCsd->writeBlLen);
    MCD_TRACE(MCD_INFO_TRC, 0, "cSizeMult: %d",  mcdCsd->cSizeMult);
    // MCD_TRACE(MCD_INFO_TRC, 0, "C size: %d. Memory capacity: %d Kbytes", mcdCsd->cSize, ((mcdCsd->cSize)+1)*512);
    MCD_TRACE(MCD_INFO_TRC, 0, "C size: %d. Memory capacity: %d bytes", mcdCsd->cSize, ((mcdCsd->cSize)+1) * mcd_CalculateMult(mcdCsd->cSizeMult) * mcd_ConvertBlockLength(mcdCsd->readBlLen));

    return MCD_ERR_NO;
}

// =============================================================================
//  hal_CalculateVal
// =============================================================================
PUBLIC UINT32 hal_CalculateVal(UINT8 code)
{
    UINT32 result;

    switch (code)
    {
        case 1: // Val 1.0
            result = 10;
            break;
        case 2: // Val 1.2
            result = 12;
            break;
        case 3: // Val 1.3
            result = 13;
            break;
        case 4: // Val 1.5
            result = 15;
            break;
        case 5: // Val 2.0
            result = 20;
            break;
        case 6: // Val 2.5
            result = 25;
            break;
        case 7: // Val 3.0
            result = 30;
            break;
        case 8: // Val 3.5
            result = 35;
            break;
        case 9: // Val 4.0
            result = 40;
            break;
        case 10: // Val 4.5
            result = 45;
            break;
        case 11: // Val 5.0
            result = 50;
            break;
        case 12: // Val 5.5
            result = 55;
            break;
        case 13: // Val 6.0
            result = 60;
            break;
        case 14: // Val 7.0
            result = 70;
            break;
        case 15: // Val 8.0
            result = 80;
            break;
        default: // Ohters are reserved
            result = 0;
    }
    return result;
}


// =============================================================================
// mcd_Open
// -----------------------------------------------------------------------------
/// @brief Open the MMC chip
/// This function does the init process of the MMC chip, including reseting
/// the chip, sending a command of init to MMC, and reading the CSD
/// configurations.
///
/// This function will force the SPI to be used instead of the GPIOs 23, 24,
/// 25 and 26 (if CS_0 is used) or 27 (if CS_1 is used). It also force the
/// SPI to be used, bypassing the configuration of the board config.
///
/// @param enabledCS #HAL_SPI_CS0 or #HAL_SPI_CS1, depending on which
/// Chip Select pin (CS0 or CS1) of Granite SPI is connected to MMC
/// @param mcdCsd Pointer to the structure where the MMC CSD register info
/// are going to be written.
// =============================================================================
PUBLIC MCD_ERR_T mcd_Open(MCD_CSD_T* mcdCsd)
{
    bool IsMMC=FALSE;
    MCD_ERR_T errorStatus;
    UINT16 cmdStatus;
    UINT8 csdRdBlLen,csdWrBlLen = 0;
    UINT16 i, j;
    bool End_Init;

    MCD_TRACE(MCD_INFO_TRC, 0, "************ MCD_OPEN ************");
    MCD_PROFILE_FUNCTION_ENTER(mcd_Open);
    g_mcdConfig = tgt_GetMcdConfig();
    g_mcdSpiCs = g_mcdConfig->usedSpiCs;

    // Configure SPI
    /// ctrl
    g_spiConfig.enabledCS = g_mcdSpiCs;
    g_spiConfig.inputEn = TRUE;
	
	g_spiConfig.inputSel = TRUE;
    g_spiConfig.clkFallEdge = TRUE;
    g_spiConfig.clkDelay = HAL_SPI_HALF_CLK_PERIOD_0;
    g_spiConfig.doDelay = HAL_SPI_HALF_CLK_PERIOD_0;
    g_spiConfig.diDelay = HAL_SPI_HALF_CLK_PERIOD_1;
    g_spiConfig.csDelay = HAL_SPI_HALF_CLK_PERIOD_0;
    g_spiConfig.csPulse = HAL_SPI_HALF_CLK_PERIOD_0;
    g_spiConfig.frameSize = 8;
    // Clock Divider for 400kHz
    // Max rate == 78MHz / ((clkDivider+1)*2)
    // for clkDivider = 97 => 398kHz
    g_spiConfig.spiFreq = 600000;
    g_spiConfig.rxTrigger = HAL_SPI_RX_TRIGGER_4_BYTE;
    g_spiConfig.txTrigger = HAL_SPI_TX_TRIGGER_8_EMPTY;
    // Use IFC
    g_spiConfig.rxMode = HAL_SPI_DMA_POLLING;
    g_spiConfig.txMode = HAL_SPI_DMA_POLLING;
    g_spiConfig.mask.rxOvf = 0;
    g_spiConfig.mask.txTh = 0;
    g_spiConfig.mask.txDmaDone = 0;
    g_spiConfig.mask.rxTh = 0;
    g_spiConfig.mask.rxDmaDone = 0;
    g_spiConfig.handler = NULL;
    /// Cfg
    g_spiConfig.csActiveLow = FALSE;
    MCD_TRACE( TSTDOUT, 0, "Opening MMC/SD");
    hal_SpiOpen(HAL_SPI, g_mcdSpiCs, &g_spiConfig);
    sxr_Sleep(SLEEP_DURATION_OP);
    // Init the Cmd buffer, only used to send command to the MMC.
    // after the command, it is required to send only 0xff to the MMC.
    // For all commands, only the 7 first bytes are modified, for that
    // reason, it is never needed to reinit this buffer with 0xff.
    for (i = 0; i < WRCMD_BUFFER_LEN; i++)
    {
        g_SpiMcdWrCmdBuf[i] = 0xff;
    }
    //Used to send the data (for the write cmd) to the MMC.
    // Needed to reinit this buffer only when writing data to the MMC,
    // but the part to reinit is very small (see the write function).
    for (i = 0; i < RDCMD_BUFFER_LEN; i++)
    {
        g_SpiMcdRdCmdBuf[i] = 0xff;
    }

    //-----------------------------//
    // Activate the SPI CS
    // ---------------------------//
    if (hal_SpiActivateCs(HAL_SPI, g_mcdSpiCs)==FALSE)
    {
        hal_SpiClose(HAL_SPI, g_mcdSpiCs);
        MCD_PROFILE_FUNCTION_EXIT(mcd_Open);
        return MCD_ERR_SPI_BUSY;
    }

    //-----------------------------------------------------//
    //          Init Process  - Send more than 74 clocks   //
    //          Data Line must be 1
    //          CS must be 1
    //-----------------------------------------------------//

    hal_SpiForcePin(HAL_SPI, HAL_SPI_PIN_CS1, HAL_SPI_PIN_STATE_1);
    // Send check busy command which only sends 0xff
    // 17*8 = 136 clocks
    errorStatus = mcd_SendCommand(0xff, 0, &cmdStatus,0,NULL);
    hal_SpiForcePin(HAL_SPI, HAL_SPI_PIN_CS1, HAL_SPI_PIN_STATE_SPI);

    //--------------------------------------//
    //          Init Process  CMD0          //
    //--------------------------------------//
    End_Init = FALSE;
    MCD_TRACE(MCD_INFO_TRC, 0, "cmdStatus before sending CMD0: %x ", cmdStatus);
    for(j = 0; j < CMD0_RETRY_NB; j++)
    {
        errorStatus = mcd_SendCommand(0,0,&cmdStatus,0,NULL);
        if (errorStatus == MCD_ERR_NO)
        {
            // CMD successfully sent
            MCD_TRACE(MCD_INFO_TRC, 0, "cmdStatus: %x ", cmdStatus);
            MCD_TRACE(MCD_INFO_TRC, 0, "cmdStatus & 0xff: %x ", (cmdStatus & 0xff));
            if ((cmdStatus & 0xff) == 0x1)
            {
                // If in Idle state, continue procedure
                MCD_TRACE(MCD_INFO_TRC, 0, "MMC/SD in IDLE State. CMD0 sent %d times", j+1);
                End_Init = TRUE;
                break;
            }
            else
            {
                // The status is not Idle
                // Retry but set the status to timeout and indicate the failure
                sxr_Sleep(SLEEP_DURATION);

                errorStatus = MCD_ERR_CARD_TIMEOUT;
                End_Init = FALSE;
            }
        }
        // Retry until CMD0_RETRY_NB is reached

        // Sleep a bit to leave some time to the card to init itself.
        sxr_Sleep(SLEEP_DURATION_CMD);
    }
    MCD_TRACE(MCD_INFO_TRC,0,"End_Init: %i", End_Init);
    // We did not manage to set the card in Idle state - Exit
    if (End_Init == FALSE)
    {
        hal_SpiDeActivateCs(HAL_SPI, g_mcdSpiCs);
        // Indicate that SPI is free
        // g_mcdSpiFree = TRUE;
        hal_SpiClose(HAL_SPI, g_mcdSpiCs);

        MCD_PROFILE_FUNCTION_EXIT(mcd_Open);

        return errorStatus;
    }

    //--------------------------------------//
    //          FOR SD Cards
    //          Init Process CMD55+ACMD41   //
    //--------------------------------------//
    End_Init = FALSE;
    for(j = 0; j < ACMD41_RETRY_NB; j++)
    {
        errorStatus = mcd_SendCommand(55,0,&cmdStatus,0,NULL);
        if (errorStatus == MCD_ERR_NO)
        {
            // CMD successfully sent
            if ((cmdStatus & 0xff) == 0x1)
            {
                // CMD 55 successfully sent
                // Send ACMD 41
                errorStatus = mcd_SendCommand(41,0,&cmdStatus,0,NULL);
                if (errorStatus == MCD_ERR_NO)
                {
                    // CMD successfully sent
                    if ((cmdStatus & 0xff) == 0x0)
                    {
                        MCD_TRACE(MCD_INFO_TRC, 0, "Send ACMD41 OK & cmdStatus&ff=0. cmdStatus: 0x %x", cmdStatus);
                        MCD_TRACE(MCD_INFO_TRC, 0, "SD in READY State. ACMD41 sent %d times", j+1);
                        // If out of Idle state, continue procedure
                        End_Init = TRUE;
                    }
                }
                else
                {
                    hal_SpiDeActivateCs(HAL_SPI, g_mcdSpiCs);
                    // Indicate that SPI is free
                    // g_mcdSpiFree = TRUE;
                    MCD_PROFILE_FUNCTION_EXIT(mcd_Open);
                    return errorStatus;
                }
            }
            else if ((cmdStatus & 0xff) == 0x5)
            {
                // the command is illegal, it must be a MMC
                IsMMC = TRUE;
                // Continue procedure and try CMD1
                End_Init = TRUE;
            }
        }
        else
        {
            hal_SpiDeActivateCs(HAL_SPI, g_mcdSpiCs);
            // Indicate that SPI is free
            // g_mcdSpiFree = TRUE;
            hal_SpiClose(HAL_SPI, g_mcdSpiCs);
            MCD_PROFILE_FUNCTION_EXIT(mcd_Open);
            return errorStatus;
        }
        if (End_Init == TRUE) break;

        // Sleep a bit to leave some time to the card to init itself.
        sxr_Sleep(SLEEP_DURATION_CMD);
    }

    if (End_Init == FALSE)
    {
        MCD_PROFILE_FUNCTION_EXIT(mcd_Open);
        hal_SpiDeActivateCs(HAL_SPI, g_mcdSpiCs);
        // Indicate that SPI is free
        // g_mcdSpiFree = TRUE;
        hal_SpiClose(HAL_SPI, g_mcdSpiCs);
        return MCD_ERR_CARD_TIMEOUT;
    }

    if (IsMMC == TRUE)
    {
//hal_HstSendEvent(0x33330001);

        //--------------------------------------//
        //          FOR MMC Cards
        //          Init Process CMD1           //
        //--------------------------------------//
        End_Init = FALSE;
        for(j = 0; j < CMD1_RETRY_NB; j++)
        {
            errorStatus = mcd_SendCommand(1,0,&cmdStatus,0,NULL);
            if (errorStatus == MCD_ERR_NO)
            {
                // CMD successfully sent
                if ((cmdStatus & 0xff) == 0x0)
                {
                    MCD_TRACE(MCD_INFO_TRC, 0, "MMC in READY State. CMD1 sent %d times", j+1);
                    // If out of Idle state, continue procedure
                    End_Init = TRUE;
                    break;
                }
            }
            else
            {
                hal_SpiDeActivateCs(HAL_SPI, g_mcdSpiCs);
                // Indicate that SPI is free
                // g_mcdSpiFree = TRUE;
                hal_SpiClose(HAL_SPI, g_mcdSpiCs);
                MCD_PROFILE_FUNCTION_EXIT(mcd_Open);
                return errorStatus;
            }
            // Sleep a bit to leave some time to the card to init itself.
            sxr_Sleep(SLEEP_DURATION_CMD);
        }

        if (End_Init == FALSE)
        {
            MCD_PROFILE_FUNCTION_EXIT(mcd_Open);
            hal_SpiDeActivateCs(HAL_SPI, g_mcdSpiCs);
            // Indicate that SPI is free
            // g_mcdSpiFree = TRUE;
//hal_HstSendEvent(0x11110003);
            hal_SpiClose(HAL_SPI, g_mcdSpiCs);
            return MCD_ERR_CARD_TIMEOUT;
        }
    }

    errorStatus = mcd_ReadCsd(mcdCsd);
    if (errorStatus != MCD_ERR_NO)
    {
        MCD_PROFILE_FUNCTION_EXIT(mcd_Open);
        hal_SpiDeActivateCs(HAL_SPI, g_mcdSpiCs);
        // Indicate that SPI is free
        // g_mcdSpiFree = TRUE;
        hal_SpiClose(HAL_SPI, g_mcdSpiCs);
        return errorStatus;
    }

    // Set block lentgh
    csdWrBlLen = mcdCsd->writeBlLen;
    csdRdBlLen = mcdCsd->readBlLen;

    if (csdWrBlLen == csdRdBlLen)
    {
        g_blockLen = csdWrBlLen;
    }
    else if (csdWrBlLen < csdRdBlLen && mcdCsd->readBlPartial)
    {
        // If Write block length < Read block length and read partial block allowed
        g_blockLen = csdWrBlLen;
    }
    else if (csdWrBlLen > csdRdBlLen && mcdCsd->writeBlPartial)
    {
        // If Write block length > Read block length and read partial block allowed
        g_blockLen = csdRdBlLen;
    }
    else
    {
        hal_SpiClose(HAL_SPI, g_mcdSpiCs);
        return MCD_ERR_BLOCK_LEN;
    }

    g_writeBlockLen = mcd_ConvertBlockLength(g_blockLen);
    g_readBlockLen = g_writeBlockLen;
    MCD_TRACE(MCD_INFO_TRC, 0, "Wr bl len: %d, Rd bl len: %d", mcdCsd->writeBlLen, mcdCsd->readBlLen);

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
    // Close the SPI and Re-open with the new clock settings
    hal_SpiDeActivateCs(HAL_SPI, g_mcdSpiCs);
    hal_SpiClose(HAL_SPI, g_mcdSpiCs);
    hal_SpiOpen(HAL_SPI, g_mcdSpiCs, &g_spiConfig);

#if 0
    // Calculate access Time
    {
        UINT32 acc_val = (mcdCsd->taac >> 3) & 0xf;
        UINT32 acc_unit = (mcdCsd->taac) & 0x7;
        UINT32 acc_time;
        UINT32 nb_clk;
        acc_val = hal_CalculateVal(acc_val);
        if ( acc_val == 0 )
        {
            MCD_PROFILE_FUNCTION_EXIT(mcd_Open);
            hal_SpiDeActivateCs(HAL_SPI, g_mcdSpiCs);
            // Indicate that SPI is free
            // g_mcdSpiFree = TRUE;
            hal_SpiClose(HAL_SPI, g_mcdSpiCs);
            return MCD_ERR_CSD;
        }
        acc_unit = 1;
        for (i=0; i < ((mcdCsd->taac) & 0x7); i++)
        {
            acc_unit=acc_unit*10;
        }
        acc_time = acc_val*acc_unit;    // in 0.1ns unit
        nb_clk = mcdCsd->nsac*100 + (acc_time * (78/(g_spiConfig.spiFreq+1)/2)/10000);
        MCD_TRACE(MCD_INFO_TRC, 0,"MMC/SD TAAC time : %d us",acc_time/10000);
        MCD_TRACE(MCD_INFO_TRC, 0,"MMC/SD NSAC time : %d clocks",mcdCsd->nsac*100);
        MCD_TRACE(MCD_INFO_TRC, 0,"MMC/SD Read Access time : %d bytes",nb_clk>>3);
        MCD_TRACE(MCD_INFO_TRC, 0,"MMC/SD Write Access time : %d bytes",(nb_clk>>3)*(1<<mcdCsd->r2wFactor));
    }
#endif
    MCD_PROFILE_FUNCTION_EXIT(mcd_Open);
//hal_HstSendEvent(0x22220000);

    return MCD_ERR_NO;
}




// =============================================================================
// mcd_Write
// -----------------------------------------------------------------------------
/// @brief Write a block of data to MMC.
///
/// This function is used to write a block of data on the MMC.
/// @param blockStartAddr Start Adress  of the MMC memory block where the
/// data will be written
/// @param blockWr Pointer to the block of data to write
/// @param blockLength length of the block, in bytes (The max block length
/// is defined in CSD register field WRITE_BL_LEN)
// =============================================================================
PUBLIC MCD_ERR_T mcd_Write(UINT32 blockStartAddr, CONST UINT8* blockWr, UINT32 blockLength)
{
    UINT16    i;
    UINT16    cmdStatus;
    MCD_ERR_T errorStatus;
    UINT8     commandBuffer[MCD_COMMAND_BUFFER_SIZE];
    UINT8*    tmpRdBuff;

    tmpRdBuff = sxr_Malloc(g_writeBlockLen + 64);

    MCD_TRACE(MCD_INFO_TRC, 0, "************ MCD_WRITE ************");

    MCD_PROFILE_FUNCTION_ENTER(mcd_Write);

    if (hal_SpiActivateCs(HAL_SPI, g_mcdSpiCs)==FALSE)
    {
        MCD_PROFILE_FUNCTION_EXIT(mcd_Write);
        sxr_Free(tmpRdBuff);
        return MCD_ERR_SPI_BUSY;
    }

    MCD_TRACE(MCD_INFO_TRC, 0, "blockWr mcd_write: 0x %x", blockWr);

    //--------------------------------------//
    //       Set block Length       //
    //--------------------------------------//
    // Write CMD16
    MCD_TRACE(MCD_INFO_TRC, 0, "Block len g_writeBlockLen: %d", g_writeBlockLen);

    // g_writeBlockLen length of the block, in bytes (The max block length
    // is defined in CSD register field WRITE_BL_LEN)
    errorStatus = mcd_SendCommand(16, g_writeBlockLen, &cmdStatus,0,NULL);
    if (errorStatus != MCD_ERR_NO)
    {
        MCD_PROFILE_FUNCTION_EXIT(mcd_Write);
        MCD_TRACE(MCD_INFO_TRC, 0, "Write deactivate0");
        hal_SpiDeActivateCs(HAL_SPI, g_mcdSpiCs);
        // Indicate that SPI is free
        // g_mcdSpiFree = TRUE;
        sxr_Free(tmpRdBuff);
        return errorStatus;
    }


    //--------------------------------------//
    //           Write a block              //
    //--------------------------------------//

    // Empties the SPI Fifo and clear the DMA Done bits.

    hal_SpiFlushFifos(HAL_SPI);
    hal_SpiClearRxDmaDone(HAL_SPI);
    hal_SpiClearTxDmaDone(HAL_SPI);

    MCD_TRACE(MCD_INFO_TRC, 0, "SD destination block startAddr : 0x %x", blockStartAddr);
    // Write a single block (CMD24: 0x58)
    commandBuffer[0] = 0xff;
    commandBuffer[1] = 0x58;
    commandBuffer[2] = (blockStartAddr >> 24) & 0xff;
    commandBuffer[3] = (blockStartAddr >> 16) & 0xff;
    commandBuffer[4] = (blockStartAddr >> 8) & 0xff;
    commandBuffer[5] = blockStartAddr & 0xff;
    commandBuffer[6] = 0x1;

    // Start byte of data token.
    commandBuffer[MCD_COMMAND_BUFFER_SIZE-1] = 0xfe;

    // Get buffer
    g_dmaBusy=hal_SpiGetData(HAL_SPI, g_mcdSpiCs, (UINT8*) tmpRdBuff, g_writeBlockLen + 64);
    if (g_dmaBusy == 0)
    {
        MCD_PROFILE_FUNCTION_EXIT(mcd_Write);
        hal_SpiDeActivateCs(HAL_SPI, g_mcdSpiCs);
        // Indicate that SPI is free
        // g_mcdSpiFree = TRUE;
        sxr_Free(tmpRdBuff);
        return MCD_ERR_DMA_BUSY;
    }

    // Send buffer
    // Reinit the end of the Tx data buffer.
    for (i = 7; i < MCD_COMMAND_BUFFER_SIZE-1; i++)
    {
        commandBuffer[i] = 0xff;
    }

#if 0
    g_dmaBusy=hal_SpiSendData(HAL_SPI, g_mcdSpiCs, (UINT8*) blockWr, g_writeBlockLen + 64);
    if (g_dmaBusy == 0)
    {
        // Stop the Rx transfer.
        hal_SpiDeActivateCs(HAL_SPI, g_mcdSpiCs);
        // Indicate that SPI is free
        // g_mcdSpiFree = TRUE;
        MCD_PROFILE_FUNCTION_EXIT(mcd_Write);
        //hal_SpiDeActivateCs(HAL_SPI, g_mcdSpiCs);
        sxr_Free(tmpRdBuff);
        return MCD_ERR_DMA_BUSY;
    }
#else
    hal_SpiForcePin(HAL_SPI, g_mcdSpiCs, HAL_SPI_PIN_STATE_0);
    g_dmaBusy=hal_SpiSendData(HAL_SPI, g_mcdSpiCs, (UINT8*) &commandBuffer[0], MCD_COMMAND_BUFFER_SIZE);
    if (g_dmaBusy == 0)
    {
        // Stop the Rx transfer.
        hal_SpiDeActivateCs(HAL_SPI, g_mcdSpiCs);
        // Indicate that SPI is free
        // g_mcdSpiFree = TRUE;
        MCD_PROFILE_FUNCTION_EXIT(mcd_Write);
        //hal_SpiDeActivateCs(HAL_SPI, g_mcdSpiCs);
        sxr_Free(tmpRdBuff);
        return MCD_ERR_DMA_BUSY;
    }

    // Command and data are sent during the same low value of the CS.
    // Any time could be waited here (from us to minutes)

    // TODO: try to use the size passed as parameter, or check/assert it, if the CSD
    // allow that (rd, wr partial thing)
    g_dmaBusy=hal_SpiSendData(HAL_SPI, g_mcdSpiCs, blockWr, g_writeBlockLen + 64 - 16);
    if (g_dmaBusy == 0)
    {
        // Stop the Rx transfer.
        hal_SpiDeActivateCs(HAL_SPI, g_mcdSpiCs);
        // Indicate that SPI is free
        // g_mcdSpiFree = TRUE;
        MCD_PROFILE_FUNCTION_EXIT(mcd_Write);
        //hal_SpiDeActivateCs(HAL_SPI, g_mcdSpiCs);
        sxr_Free(tmpRdBuff);
        return MCD_ERR_DMA_BUSY;
    }
    hal_SpiForcePin(HAL_SPI, g_mcdSpiCs, HAL_SPI_PIN_STATE_SPI);
#endif



    // Wait until the end of both transfers (Tx finishes allways before Rx).
    MCD_PROFILE_FUNCTION_ENTER(mcd_Xxx_WaitTc);

    while (!hal_SpiRxDmaDone(HAL_SPI, g_spiConfig.enabledCS))
    {
        sxr_Sleep(SLEEP_DURATION);
    }

    MCD_PROFILE_FUNCTION_EXIT(mcd_Xxx_WaitTc);

    i=0;
    // Polling until Previous Write finishes
    while ( i < MCD_WRITE_TIMEOUT )
    {
        // Check busy signal
        // FIXME Get rid of that if {} ?
        if (hal_SpiActivateCs(HAL_SPI, g_mcdSpiCs)==FALSE)
        {
            MCD_PROFILE_FUNCTION_EXIT(mcd_Write);
            sxr_Free(tmpRdBuff);
            return MCD_ERR_SPI_BUSY;
        }
        // ENd of get rid of ...

        errorStatus = mcd_SendCommand(0xff, 0, &cmdStatus,0,NULL);
        if ( errorStatus == MCD_ERR_NO )
        {
            break;
        }
        i++;
        sxr_Sleep(SLEEP_DURATION_WR_POLL);
    }

    hal_SpiDeActivateCs(HAL_SPI, g_mcdSpiCs);
    // Indicate that SPI is free
    // g_mcdSpiFree = TRUE;
    MCD_PROFILE_FUNCTION_EXIT(mcd_Write);

    if ( i == MCD_WRITE_TIMEOUT )
    {
        sxr_Free(tmpRdBuff);
        return MCD_ERR_CARD_TIMEOUT;
    }

    sxr_Free(tmpRdBuff);
    return MCD_ERR_NO;
}


// =============================================================================
// mcd_Read
// -----------------------------------------------------------------------------
/// @brief Read using pattern mode.
/// @ param blockStartAddr: of the MMC memory block where the data
/// will be read
/// @param blockRd Pointer to the buffer where the data will be stored
/// @param blockLength length of the block, in bytes (The max block length
/// is defined in CSD register field WRITE_BL_LEN)
// =============================================================================
MCD_ERR_T mcd_Read(UINT32 startAddr, UINT8* rdDataBlock, UINT32 blockLength)
{
    UINT8 commandBuffer[MCD_COMMAND_BUFFER_SIZE];
    HAL_SPI_INFINITE_TX_T infTx;

    sxr_Sleep(SLEEP_DURATION_READ);

    MCD_TRACE(MCD_INFO_TRC, 0, "************ MCD_READ ************");

    // Config SPI Infinite
    infTx.sendOne     = TRUE;
    infTx.autoStop    = TRUE;
    infTx.patternMode = HAL_SPI_PATTERN_UNTIL;
    infTx.pattern     = 0xfe;

    if (hal_SpiActivateCs(HAL_SPI, g_mcdSpiCs)==FALSE)
    {
        return MCD_ERR_SPI_BUSY;
    }

    // Read a block
    hal_SpiFlushFifos(HAL_SPI);
    hal_SpiClearRxDmaDone(HAL_SPI);
    hal_SpiClearTxDmaDone(HAL_SPI);

    commandBuffer[0] = 0xff;
    commandBuffer[1] = 0x51;
    commandBuffer[2] = (startAddr >> 24) & 0xff;
    commandBuffer[3] = (startAddr >> 16) & 0xff;
    commandBuffer[4] = (startAddr >> 8) & 0xff;
    commandBuffer[5] = startAddr & 0xff;
    commandBuffer[6] = 0x1;

    hal_SpiStartInfiniteWriteMode(HAL_SPI, g_mcdSpiCs, &infTx);

    // Receive Buffer
    g_dmaBusy = hal_SpiGetData(HAL_SPI, g_mcdSpiCs, (UINT8*)rdDataBlock, g_readBlockLen);
    if (g_dmaBusy==0)
    {
        hal_SpiDeActivateCs(HAL_SPI, g_mcdSpiCs);
        // Indicate that SPI is free
        // g_mcdSpiFree = TRUE;
        return MCD_ERR_DMA_BUSY;
    }

    MCD_TRACE(MCD_INFO_TRC, 0, "Rx buffer Rd_Data_Block->data: 0x %x", rdDataBlock);

    g_dmaBusy = hal_SpiSendData(HAL_SPI, g_mcdSpiCs, commandBuffer, g_readBlockLen + 64);

    if (g_dmaBusy == 0)
    {
        // Stop the Rx transfer.
        hal_SpiDeActivateCs(HAL_SPI, g_mcdSpiCs);
        // Indicate that SPI is free
        // g_mcdSpiFree = TRUE;
        return MCD_ERR_DMA_BUSY;
    }

    while (!hal_SpiTxDmaDone(HAL_SPI, g_spiConfig.enabledCS));
    sxr_Sleep(SLEEP_DURATION_RD);
    //hal_SpiStopInfiniteWriteMode(HAL_SPI, g_mcdSpiCs);
    hal_SpiDeActivateCs(HAL_SPI, g_mcdSpiCs);
    // To invalidate cache
    memcpy((UINT8*)rdDataBlock, (CONST UINT8*)HAL_SYS_GET_UNCACHED_ADDR(rdDataBlock), g_readBlockLen);
    // Indicate that SPI is free
    // g_mcdSpiFree = TRUE;
    MCD_TRACE(MCD_INFO_TRC, 0, "Struct Rd_Data_Block: 0x %x", rdDataBlock);
    return MCD_ERR_NO;
}


// =============================================================================
//  mcd_CalculateMult
// -----------------------------------------------------------------------------
/// @brief Convert the cSizeMult from CSD register to 2^(csdSizeMult+2).
/// @param csdCSizeMult cSizeMult from CSD register.
/// @return 2^(csdSizeMult+2)
// =============================================================================
PROTECTED UINT16 mcd_CalculateMult(UINT8 csdCSizeMult)
{
    return 1<<(csdCSizeMult+2);
}


// =============================================================================
// mcd_ConvertBlockLength
// -----------------------------------------------------------------------------
/// @brief Calculate W/R block length.
/// @ param blockLen: writeBlLen or readBlLen from CSDregister.
/// @return 2^writeBlLen or 2^readBlLen
// =============================================================================
PRIVATE UINT16 mcd_ConvertBlockLength(UINT8 blockLen)
{
    return 1<<blockLen;
}


// =============================================================================
// mcd_SpiFree
// -----------------------------------------------------------------------------
/// @brief Indicate the the SPI is free. Used for SPI sharing
/// To be called after hal_SpiDeActivateCs
// =============================================================================
PUBLIC BOOL mcd_SpiFree(VOID)
{
    return g_mcdSpiFree;
}


// =============================================================================
// mcd_Close
// -----------------------------------------------------------------------------
/// @brief Close the SPI-MMC
///
/// To be called at the end of the operations
// =============================================================================
PUBLIC MCD_ERR_T mcd_Close(VOID)
{
    MCD_PROFILE_FUNCTION_ENTER(mcd_Close);

    hal_SpiClose(HAL_SPI, g_mcdSpiCs);

    MCD_PROFILE_FUNCTION_EXIT(mcd_Close);

    return MCD_ERR_NO;
}


