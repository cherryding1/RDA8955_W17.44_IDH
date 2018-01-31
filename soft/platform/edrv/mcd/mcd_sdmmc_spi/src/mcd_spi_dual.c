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
#include "hal_spi.h"
#include "hal_gpio.h"
#include "sxr_tls.h"
#include "mcdp_debug.h"
#include "mcd_m.h"
#include "hal_spi.h"
#include "pmd_m.h"
#include "mcd_config.h"
#include "mcd_spi.h"
#include "mcd_config.h"
#include "tgt_mcd_cfg.h"

#include "hal_host.h"

#include "sxr_mem.h"

BOOL g_special_card = FALSE;
BOOL g_special_card2 = FALSE;

// =============================================================================
//
// -----------------------------------------------------------------------------
// =============================================================================
typedef enum
{
    SDMMC_NORMAL_VER_1,
    SDMMC_STANDARD_CAPACITY_VER_2,
    SDMMC_HIGH_CAPACITY_VER_2,
    SDMMC_NORMAL_VER_QTY
} SDMMC_VER_T;


// =============================================================================
//  MACROS
// =============================================================================

#define HAL_TICK1S 16384

#define SECOND        * HAL_TICK1S

#define MCD_SPI_CMD_TIMEOUT        ( 1 SECOND / 10 )
#define MCD_SPI_RESP_TIMEOUT       ( 1 SECOND / 10 )
#define MCD_SPI_TRAN_TIMEOUT       ( 1 SECOND / 10 )
#define MCD_SPI_READ_TIMEOUT       ( 1 SECOND / 10 )
#define MCD_SPI_WRITE_TIMEOUT      ( 1 SECOND / 10 )





#define COMMAND_TIMEOUT     100   //  ms
#define BLOCK_DATA_TIMEOUT     1000   // ms


#define MAX_BLOCK_SIZE      2048
//#define RDCMD_BUFFER_LEN    16380   //  1024 // in jade 16380
#define RDCMD_BUFFER_LEN    1638   // may be 800
#define WRCMD_BUFFER_LEN    (MAX_BLOCK_SIZE + 64)
#define SLEEP_DURATION_OP   2000
#define SLEEP_DURATION_RD   20
#define SLEEP_DURATION_READ 5000
#define SLEEP_DURATION_WR   5000
#define SLEEP_DURATION_STD  5
#define SLEEP_DURATION      0
#define SLEEP_DURATION_WR_POLL   16384/1000  // (1 millisecond)
#define SLEEP_DURATION_CMD  16384/100        // (10 millisecond)
#define MCD_WRITE_TIMEOUT   1000             // max timeout for a write is
//  5s = 5000 *SLEEP_DURATION_WR_POLL

// The number of retry does not depend on the clock divider.
#define CMD1_RETRY_NB       128   // the card is supposed to answer within 1s
//  - Max wait is 128 * 10ms=1,28s
#define CMD0_RETRY_NB       5     // Retry 5 times max
#define ACMD41_RETRY_NB     128   // the card is supposed to answer within 1s
//  - Max wait is 128 * 10ms=1,28s
#define RD_SINGLE_BLOCK_LEN    512+64

#define CMDFF_RETRY_NB                           10
#define MCD_COMMAND_BUFFER_SIZE        16

#define MCD_DEV_NUMBER                           2
#define MCD_DEFUALT_BLOCK_SIZE            512

UINT32 g_mcd_spi_delay = 0;

#define mcd_spi_delay(x)  mcd_delay(x)
// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================
PRIVATE VOLATILE UINT8       g_SpiMcdRxBuf[RDCMD_BUFFER_LEN]
__attribute__((section(".ucdata")));
PRIVATE VOLATILE UINT8       g_SpiMcdRdCmdBuf[RDCMD_BUFFER_LEN]
__attribute__((section(".ucdata")));
PRIVATE VOLATILE UINT8       g_SpiMcdWrCmdBuf[WRCMD_BUFFER_LEN]
__attribute__((section(".ucdata")));

PRIVATE  UINT8       g_SpiMcdtmpRdBuff[512+16+64];
PRIVATE HAL_SPI_CS_T         g_mcdSpiCs      = HAL_SPI_CS0;
PRIVATE HAL_SPI_ID_T         g_mcdSpiId      = HAL_SPI;
PRIVATE HAL_SPI_PIN_T        g_mcdSpiPinId   = HAL_SPI_PIN_CS0;
PRIVATE UINT16               g_dmaBusy       = 0;
PRIVATE UINT32               g_transferSize  = 0;
PRIVATE UINT16               g_readBlockLen[MCD_DEV_NUMBER]  = {0,0};
PRIVATE UINT16               g_writeBlockLen[MCD_DEV_NUMBER] = {0,0};
PRIVATE UINT16               g_blockLen[MCD_DEV_NUMBER]  = {0,0};
PRIVATE UINT32                       g_blockNumber[MCD_DEV_NUMBER]  = {0,0};
PRIVATE BOOL                 g_mcdSpiFree    = TRUE;

/// HAL SPI CFG
PRIVATE HAL_SPI_CFG_T        g_spiConfig;
/// MCD config

PRIVATE MCD_SDMMC_SPI_IF*  g_mcdConfig = NULL;
PRIVATE MCD_STATUS_T      g_mcdStatus = MCD_STATUS_NOTOPEN_PRESENT;

PRIVATE BOOL g_mcdCardIsSdhc = FALSE;
PRIVATE SDMMC_VER_T g_card_ver = SDMMC_NORMAL_VER_QTY;
PRIVATE BOOL IsMMC=FALSE;
PRIVATE BOOL g_mmcsd_open = FALSE;

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
// =============================================================================
//  FUNCTIONS
// =============================================================================

#if 0
// =============================================================================
//  mcd_CalculateMult
// -----------------------------------------------------------------------------
/// Convert the C_SIZE_MULT from CSD register to 2^(csdSizeMult+2).
/// @param csdCSizeMult C_SIZE_MULT from CSD register.
/// @return 2^(csdSizeMult+2)
// =============================================================================
PRIVATE UINT16  mcd_CalculateMult(UINT8 csdCSizeMult);

PRIVATE UINT8 SD_GetCmdByte6(UINT8 cmd, UINT8 *param)
{
    UINT8 i, j;
    UINT8 reg = 0;
    UINT8 array[5];

    array[0] = cmd;

    for (i = 0; i < 5; i++)                     /* 计算5个字节的CRC7 */
    {
        for (j = 0; j < 8; j++)
        {
            reg <<= 1;
            reg ^= ((((array[i] << j) ^ reg) & 0x80) ? 0x9 : 0);
        }
    }

    return ((reg << 1) + 0x01) ;                /* 计算结果的CRC7左移一位,并将最低位置1 */
}
#endif

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
    UINT32 i,retry = 0;
    MCD_TRACE(MCD_INFO_TRC, 0, "mcd_SendCommand %d", cmdId);
    UINT8 array[5];
    UINT32 startTime = 0;
    hal_SpiTxIfcChannelRelease(g_mcdSpiId);
    hal_SpiRxIfcChannelRelease(g_mcdSpiId);

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
    hal_SpiFlushFifos(g_mcdSpiId);
    hal_SpiClearRxDmaDone(g_mcdSpiId);
    hal_SpiClearTxDmaDone(g_mcdSpiId);
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
    else if(cmdId == 8)
    {
        g_SpiMcdRdCmdBuf[0] = 0xff;
        g_SpiMcdRdCmdBuf[1] = 0x40 | cmdId;
        g_SpiMcdRdCmdBuf[2] = (argument & 0xff000000)>>24;
        g_SpiMcdRdCmdBuf[3] = (argument & 0x00ff0000)>>16;
        g_SpiMcdRdCmdBuf[4] = (argument & 0x0000ff00)>>8;
        g_SpiMcdRdCmdBuf[5] = (argument & 0x00);
        // other CMD CRC is ignored - set CRC to 0
        array[0] = cmdId;
        array[1] = g_SpiMcdRdCmdBuf[5];
        array[2] = g_SpiMcdRdCmdBuf[4];
        array[3] = g_SpiMcdRdCmdBuf[3];
        array[4] = g_SpiMcdRdCmdBuf[2];

        g_SpiMcdRdCmdBuf[6] = 0xd4|0x1;// ;

        // MCD_TRACE(MCD_INFO_TRC, 0, "cmdId cmd 8 crc 0x%x",SD_GetCmdByte6(8,array));


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

    g_dmaBusy=hal_SpiGetData(g_mcdSpiId, g_mcdSpiCs, (UINT8*) g_SpiMcdRxBuf, g_transferSize);
    if (g_dmaBusy==0)
    {
        return MCD_ERR_DMA_BUSY;
    }

    g_dmaBusy=hal_SpiSendData(g_mcdSpiId, g_mcdSpiCs, (UINT8*) g_SpiMcdRdCmdBuf, g_transferSize);
    mcd_spi_delay(SLEEP_DURATION_STD);
    if (g_dmaBusy == 0)
    {
        // Stop the Rx transfer.
        //hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        // Indicate that SPI is free
        MCD_TRACE(MCD_INFO_TRC, 0, "mcd_SendCommand busy1");

        return MCD_ERR_DMA_BUSY;
    }

    // Wait
    startTime = hal_TimGetUpTime();

    // Wait until the end of both transfers.
    while ((!hal_SpiRxDmaDone(g_mcdSpiId,g_mcdSpiCs))||(!hal_SpiTxDmaDone(g_mcdSpiId,g_mcdSpiCs)))
    {
        if (hal_TimGetUpTime() - startTime >  (MCD_SPI_CMD_TIMEOUT*1))
        {

            hal_SpiFlushFifos(g_mcdSpiId);
            hal_SpiClearRxDmaDone(g_mcdSpiId);
            hal_SpiClearTxDmaDone(g_mcdSpiId);
            hal_SpiTxIfcChannelRelease(g_mcdSpiId);
            hal_SpiRxIfcChannelRelease(g_mcdSpiId);
            hal_HstSendEvent(0x11112226);
            retry ++;
            break;
        }

    }

    if(retry == 1)
    {
        g_dmaBusy=hal_SpiGetData(g_mcdSpiId, g_mcdSpiCs, (UINT8*) g_SpiMcdRxBuf, g_transferSize);
        if (g_dmaBusy==0)
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "mcd_SendCommand busy2");
            return MCD_ERR_DMA_BUSY;
        }

        g_dmaBusy=hal_SpiSendData(g_mcdSpiId, g_mcdSpiCs, (UINT8*) g_SpiMcdRdCmdBuf, g_transferSize);

        if (g_dmaBusy == 0)
        {
            // Stop the Rx transfer.
            //hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
            MCD_TRACE(MCD_INFO_TRC, 0, "mcd_SendCommand busy3");
            // g_mcdSpiFree = TRUE;
            return MCD_ERR_DMA_BUSY;
        }
        mcd_spi_delay(SLEEP_DURATION_STD);

        startTime = hal_TimGetUpTime();
        // Wait until the end of both transfers.
        while (!hal_SpiRxDmaDone(g_mcdSpiId,g_mcdSpiCs))
        {
            if (hal_TimGetUpTime() - startTime >  (MCD_SPI_CMD_TIMEOUT*1))
            {

                hal_SpiFlushFifos(g_mcdSpiId);
                hal_SpiClearRxDmaDone(g_mcdSpiId);
                hal_SpiClearTxDmaDone(g_mcdSpiId);
                hal_SpiTxIfcChannelRelease(g_mcdSpiId);
                hal_SpiRxIfcChannelRelease(g_mcdSpiId);
                //hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
                MCD_TRACE(MCD_INFO_TRC, 0, "mcd_SendCommand busy4");
                return MCD_ERR_CARD_TIMEOUT;

            }

        }


    }
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
    //MCD_TRACE(MCD_INFO_TRC, 0, "mcd_SendCommand  status 0x%x cmdId   %d", *status,cmdId);
    // MCD_TRACE(MCD_INFO_TRC, 0, "mcd_SendCommand ytt  status 0x%x ,0x%x  ,0x%x ,0x%x 0x%x ", g_SpiMcdRxBuf[i],g_SpiMcdRxBuf[i+1],g_SpiMcdRxBuf[i+2],g_SpiMcdRxBuf[i+3],g_SpiMcdRxBuf[i+4]);
    //MCD_TRACE(MCD_INFO_TRC, 0, "mcd_SendCommand ytt  status 0x%x ,0x%x  ,0x%x ,0x%x 0x%x  ", g_SpiMcdRxBuf[i],g_SpiMcdRxBuf[i-1],g_SpiMcdRxBuf[i-2],g_SpiMcdRxBuf[i-3],g_SpiMcdRxBuf[i+4]);
    if(cmdId == 58)
    {
        if(g_SpiMcdRxBuf[i+1] == 0xc0)
        {
            g_mcdCardIsSdhc = TRUE;
            MCD_TRACE(TSTDOUT, 0, "Card is SDHC");
        }
        else
        {
            g_mcdCardIsSdhc = FALSE;
            MCD_TRACE(TSTDOUT, 0, "Card is standard capacity SD");
        }
        MCD_TRACE(TSTDOUT, 0, " error ");
    }

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

    return MCD_ERR_NO;
}



// =============================================================================
// mcd_ReadCsd
// -----------------------------------------------------------------------------
/// @brief Read the MMC CSD register
/// @param mcdCsd Pointer to the structure where the MMC CSD register info
/// are going to be written.
// =============================================================================
PRIVATE MCD_ERR_T mcd_ReadCsd(MCD_CSD_T* mcdCsd)
{
    UINT16 i = 0;
    MCD_ERR_T cmd_error;
    UINT16 cmdStatus;
    UINT8  temp[16];
    UINT8* data_array = temp;

    cmd_error = mcd_SendCommand(9,0,&cmdStatus,16,&data_array);

    // bit 127:120
    mcdCsd->csdStructure = (data_array[i]>>6)&0x3;           //  data_array[0]= 127:120  i = 0
    mcdCsd->specVers = (data_array[i++]>>2)&0xf;             //  data_array[0]= 127:120  i = 1
    // bit 119:112
    mcdCsd->taac = data_array[i++];                          //  data_array[1]= 119:112  i = 2   //ver2.0 taac=0x0e.
    // bit 111:104
    mcdCsd->nsac = data_array[i++];                          //  data_array[2]= 111:104  i = 3
    // bit 103:96
    mcdCsd->tranSpeed = data_array[i++];                     //  data_array[3]= 103: 96  i = 4    0x32 or 0x5a
    // bit 95:88
    mcdCsd->ccc = (UINT16)(data_array[i++])<<4;              //  data_array[4]= 95 : 88  i = 5
    // bit 87:80
    mcdCsd->ccc |= ((data_array[i]>>4)&0xf);                 //  data_array[5]= 87 : 80  i = 5
    mcdCsd->readBlLen = data_array[i++]&0xf;                 //  data_array[5]= 87 : 80  i = 6   //ver2.0 rl=9  512bytes
    // bit 79:72
    mcdCsd->readBlPartial = (data_array[i]>>7)&0x1;          //  data_array[6]= 79 : 72  i = 6
    mcdCsd->writeBlkMisalign = (data_array[i]>>6)&0x1;       //  data_array[6]= 79 : 72  i = 6
    mcdCsd->readBlkMisalign = (data_array[i]>>5)&0x1;        //  data_array[6]= 79 : 72  i = 6
    mcdCsd->dsrImp = (data_array[i]>>4)&0x1;                 //  data_array[6]= 79 : 72  i = 6
    if(IsMMC == TRUE)
    {
        mcdCsd->cSize = (UINT32)((data_array[i++]&0x3))<<10; //  data_array[6]= 79 : 72  i = 7
        // bit 71:64
        mcdCsd->cSize |= (UINT32)(data_array[i++])<<2;       //  data_array[7]= 71 : 64  i = 8
        // bit 63:56
        mcdCsd->cSize |= ((data_array[i]>>6)&0x3);           //  data_array[8]= 63 : 56  i = 8

        mcdCsd->vddRCurrMin = (data_array[i]>>3)&0x7;        //  data_array[8]= 63 : 56  i = 8
        mcdCsd->vddRCurrMax = data_array[i++]&0x7;           //  data_array[8]= 63 : 56  i = 9
        // bit 55:48
        mcdCsd->vddWCurrMin = (data_array[i]>>5)&0x7;        //  data_array[9]= 55 : 48  i = 9
        mcdCsd->vddWCurrMax = (data_array[i]>>2)&0x7;        //  data_array[9]= 55 : 48  i = 9
        mcdCsd->cSizeMult = (data_array[i++]&0x3)<<1;         // data_array[9]= 55 : 48  i = 10
        // bit 47:40
        mcdCsd->cSizeMult |= ((data_array[i]>>7)&0x1);        // data_array[10]= 47 : 40 i = 10
        mcdCsd->eraseGrpSize = (data_array[i]>>2)&0x1f;        // data_array[10]= 47 : 40 i = 10
        mcdCsd->eraseGrpMult = (data_array[i++] &0x3)<<3;      // data_array[10]= 47 : 40 i = 11
        // bit 39:32
        mcdCsd->eraseGrpMult |= ((data_array[i]>>5)&0x7);      // data_array[11]= 39 : 32 i = 11
        mcdCsd->wpGrpSize = data_array[i++]&0x1f;              // data_array[11]= 39 : 32 i = 12
        // bit 31:24
        mcdCsd->wpGrpEnable = ((data_array[i]>>7)&0x1);        // data_array[12]= 31 : 24 i = 12
        mcdCsd->defaultEcc = ((data_array[i]>>5)&0x3);         // data_array[12]= 31 : 24 i = 12
        mcdCsd->r2wFactor = ((data_array[i]>>2)&0x7);          // data_array[12]= 31 : 24 i = 12
        mcdCsd->writeBlLen = (data_array[i++]&0x3)<<2;         // data_array[12]= 31 : 24 i = 13
        // bit 23:16
        mcdCsd->writeBlLen |= ((data_array[i]>>6)&0x3);        // data_array[13]= 23 : 16 i = 13
        mcdCsd->writeBlPartial = ((data_array[i]>>5)&0x1);     // data_array[13]= 23 : 16 i = 13
        mcdCsd->contentProtApp = data_array[i++]&0x1;          // data_array[13]= 23 : 16 i = 14
        // bit 15:8
        mcdCsd->fileFormatGrp = (data_array[i]>>7)&0x1;        // data_array[14]= 15 : 8 i = 14
        mcdCsd->copy = (data_array[i]>>6)&0x1;                 // data_array[14]= 15 : 8 i = 14
        mcdCsd->permWriteProtect = (data_array[i]>>5)&0x1;     // data_array[14]= 15 : 8 i = 14
        mcdCsd->tmpWriteProtect = (data_array[i]>>4)&0x1;       // data_array[14]= 15 : 8 i = 14
        mcdCsd->fileFormat = (data_array[i]>>2)&0x3;           // data_array[14]= 15 : 8 i = 14
        mcdCsd->ecc = data_array[i++]&0x3;                     // data_array[14]= 15 : 8 i = 15
        // bit 7:0
        mcdCsd->crc = (data_array[i]>>1)&0x7f;                 // data_array[15]= 7: 0 i = 14
        mcdCsd->blockNumber = (mcdCsd->cSize + 1)<<(mcdCsd->cSizeMult + 2);

    }
    else
    {
        if(mcdCsd->csdStructure == 0x0)
        {
            mcdCsd->cSize = (UINT32)((data_array[i++]&0x3))<<10; //  data_array[6]= 79 : 72  i = 7
            // bit 71:64
            mcdCsd->cSize |= (UINT32)(data_array[i++])<<2;       //  data_array[7]= 71 : 64  i = 8
            // bit 63:56
            mcdCsd->cSize |= ((data_array[i]>>6)&0x3);           //  data_array[8]= 63 : 56  i = 8

            mcdCsd->vddRCurrMin = (data_array[i]>>3)&0x7;        //  data_array[8]= 63 : 56  i = 8
            mcdCsd->vddRCurrMax = data_array[i++]&0x7;           //  data_array[8]= 63 : 56  i = 9
            // bit 55:48
            mcdCsd->vddWCurrMin = (data_array[i]>>5)&0x7;        //  data_array[9]= 55 : 48  i = 10
            mcdCsd->vddWCurrMax = (data_array[i]>>2)&0x7;
            mcdCsd->cSizeMult = (data_array[i++]&0x3)<<1;         // data_array[9]= 55 : 48  i = 10
            // bit 47:40
            mcdCsd->cSizeMult |= ((data_array[i]>>7)&0x1);        // data_array[10]= 47 : 40 i = 10
            mcdCsd->eraseBlkEnable = ((data_array[i]>>6)&0x1);    // data_array[10]= 47 : 40 i = 10
            mcdCsd->sectorSize = ((data_array[i++])&0x3f)<<1;     // data_array[10]= 47 : 40 i = 11
            mcdCsd->sectorSize |= ((data_array[i])>>7);           // data_array[11]= 39 : 32 i = 11
            // bit  38 :32
            mcdCsd->wpGrpSize= (data_array[i++])&0x7f;            // data_array[11]= 39 : 32 i = 12
            mcdCsd->wpGrpEnable= (data_array[i]>>7);              // data_array[12]= 31 : 24 i = 11
            // bit 31:24
            mcdCsd->defaultEcc = ((data_array[i]>>5)&0x3);         // data_array[12]= 31 : 24 i = 12
            mcdCsd->r2wFactor = ((data_array[i]>>2)&0x7);          // data_array[12]= 31 : 24 i = 12
            MCD_TRACE(TSTDOUT, 0, " 1writeBlLen i = %d",i);
            mcdCsd->writeBlLen = (data_array[i++]&0x3)<<2;         // data_array[12]= 31 : 24 i = 13
            // bit 23:16
            mcdCsd->writeBlLen |= ((data_array[i]>>6)&0x3);        // data_array[13]= 23 : 16 i = 13
            MCD_TRACE(TSTDOUT, 0, " 1writeBlLen i = %d",i);

            mcdCsd->writeBlPartial = ((data_array[i]>>5)&0x1);     // data_array[13]= 23 : 16 i = 13
            mcdCsd->contentProtApp = data_array[i++]&0x1;          // data_array[13]= 23 : 16 i = 14
            // bit 15:8
            mcdCsd->fileFormatGrp = (data_array[i]>>7)&0x1;        // data_array[14]= 15 : 8 i = 14
            mcdCsd->copy = (data_array[i]>>6)&0x1;                 // data_array[14]= 15 : 8 i = 14
            mcdCsd->permWriteProtect = (data_array[i]>>5)&0x1;     // data_array[14]= 15 : 8 i = 14
            mcdCsd->tmpWriteProtect = (data_array[i]>>4)&0x1;       // data_array[14]= 15 : 8 i = 14
            mcdCsd->fileFormat = (data_array[i]>>2)&0x3;           // data_array[14]= 15 : 8 i = 14
            mcdCsd->ecc = data_array[i++]&0x3;                     // data_array[14]= 15 : 8 i = 15
            // bit 7:0
            mcdCsd->crc = (data_array[i]>>1)&0x7f;                 // data_array[15]= 7: 0 i = 14
            mcdCsd->blockNumber = (mcdCsd->cSize + 1)<<(mcdCsd->cSizeMult + 2);
            //g_blockNumber[g_mcdSpiCs] = ((mcdCsd->cSize)+1) * mcd_CalculateMult(mcdCsd->cSizeMult) * (mcd_ConvertBlockLength(mcdCsd->readBlLen)/MCD_DEFUALT_BLOCK_SIZE);

        }
        else // c size  69:48
        {
            i++;                                                  //  data_array[6]= 79 : 72  i = 7
            // bit 71:64
            mcdCsd->cSize = ((UINT32)((data_array[i++])&0x3f))<<16; //  data_array[7]= 71 : 64  i = 8
            // bit 63:56
            mcdCsd->cSize |= ((UINT32)((data_array[i++])))<<8;        //  data_array[8]= 63 : 56  i = 9
            mcdCsd->cSize |= ((UINT32)((data_array[i++])));           //  data_array[9]= 55 : 48  i = 10
            // bit 47:40
            mcdCsd->eraseBlkEnable = ((data_array[i]>>6)&0x1);    // data_array[10]= 47 : 40 i = 10
            mcdCsd->sectorSize = ((data_array[i++])&0x3f)<<1;     // data_array[10]= 47 : 40 i = 11
            mcdCsd->sectorSize |= ((data_array[i])>>7);           // data_array[11]= 39 : 32 i = 11
            // bit  38 :32
            mcdCsd->wpGrpSize= (data_array[i++])&0x7f;            // data_array[11]= 39 : 32 i = 12
            mcdCsd->wpGrpEnable= (data_array[i]>>7);              // data_array[12]= 31 : 24 i = 11
            // bit 31:24
            mcdCsd->defaultEcc = ((data_array[i]>>5)&0x3);         // data_array[12]= 31 : 24 i = 12
            mcdCsd->r2wFactor = ((data_array[i]>>2)&0x7);          // data_array[12]= 31 : 24 i = 12
            MCD_TRACE(TSTDOUT, 0, " 1writeBlLen i = %d",i);

            mcdCsd->writeBlLen = (data_array[i++]&0x3)<<2;         // data_array[12]= 31 : 24 i = 13

            // bit 23:16
            mcdCsd->writeBlLen |= ((data_array[i]>>6)&0x3);        // data_array[13]= 23 : 16 i = 13
            MCD_TRACE(TSTDOUT, 0, " writeBlLen i = %d",i);

            mcdCsd->writeBlPartial = ((data_array[i]>>5)&0x1);     // data_array[13]= 23 : 16 i = 13
            mcdCsd->contentProtApp = data_array[i++]&0x1;          // data_array[13]= 23 : 16 i = 14
            // bit 15:8
            mcdCsd->fileFormatGrp = (data_array[i]>>7)&0x1;        // data_array[14]= 15 : 8 i = 14
            mcdCsd->copy = (data_array[i]>>6)&0x1;                 // data_array[14]= 15 : 8 i = 14
            mcdCsd->permWriteProtect = (data_array[i]>>5)&0x1;     // data_array[14]= 15 : 8 i = 14
            mcdCsd->tmpWriteProtect = (data_array[i]>>4)&0x1;       // data_array[14]= 15 : 8 i = 14
            mcdCsd->fileFormat = (data_array[i]>>2)&0x3;           // data_array[14]= 15 : 8 i = 14
            mcdCsd->ecc = data_array[i++]&0x3;                     // data_array[14]= 15 : 8 i = 15
            // bit 7:0
            mcdCsd->crc = (data_array[i]>>1)&0x7f;                 // data_array[15]= 7: 0 i = 14
            mcdCsd->blockNumber = (mcdCsd->cSize + 1)*1024;


        }
    }
    MCD_TRACE(TSTDOUT, 0, " spi sd card test... ");

    MCD_TRACE(TSTDOUT, 0, "Read block partial: %d",  mcdCsd->readBlPartial);
    MCD_TRACE(TSTDOUT, 0, "Write block partial: %d",  mcdCsd->writeBlPartial);
    MCD_TRACE(TSTDOUT, 0, "Max read block lentgh: %x",  mcdCsd->readBlLen);
    MCD_TRACE(TSTDOUT, 0, "Max write block length: %x",  mcdCsd->writeBlLen);
    MCD_TRACE(TSTDOUT, 0, "cSizeMult: %d",  mcdCsd->cSizeMult);
    // MCD_TRACE(TSTDOUT, 0, "C size: %d. Memory capacity: %d Kbytes", mcdCsd->cSize, ((mcdCsd->cSize)+1)*512);
    if(mcdCsd->csdStructure == 0x0)
    {
        g_card_ver = SDMMC_NORMAL_VER_1;

    }
    else if(mcdCsd->csdStructure == 0x1)
    {
        g_card_ver = SDMMC_STANDARD_CAPACITY_VER_2;

    }
    else if(mcdCsd->csdStructure == 0x2)
    {
        g_card_ver = SDMMC_HIGH_CAPACITY_VER_2;

    }

    if(g_card_ver == SDMMC_NORMAL_VER_1)
    {
        // this block number not the really block number,is a logical and just for FS system .
        g_blockNumber[0] = mcdCsd->blockNumber;
        MCD_TRACE(TSTDOUT, 0, "v1 block number = %d.",mcdCsd->blockNumber);
        g_blockLen[0] = mcd_ConvertBlockLength(mcdCsd->readBlLen);
        MCD_TRACE(TSTDOUT, 0, "v1 g_blockLen =%d",g_blockLen);
        if(g_card_ver == SDMMC_NORMAL_VER_1)
        {
            if(g_blockLen[0] == 512)
            {
                if(g_blockNumber[0] < 409600)
                {
                    g_special_card=TRUE;
                    if(mcdCsd->copy == 1)
                    {
                        if((mcdCsd->taac > 105)&&(mcdCsd->taac < 120))
                            g_special_card2  = TRUE; hal_HstSendEvent(0xddddaabb);
                    }

                    MCD_TRACE(TSTDOUT, 0, " some special card for temp",g_blockLen);

                }
            }
            else if(g_blockLen[0] == 1024)
            {
                if(g_blockNumber[0] < 208320)
                {
                    g_special_card=TRUE;
                    if(mcdCsd->copy == 1)
                    {
                        if((mcdCsd->taac > 105)&&(mcdCsd->taac < 120))
                            g_special_card2  = TRUE; hal_HstSendEvent(0xddddaabb);
                    }
                    MCD_TRACE(TSTDOUT, 0, " some special card for temp",g_blockLen);

                }
            }

        }
    }
    else if(g_card_ver == SDMMC_STANDARD_CAPACITY_VER_2)
    {
        g_blockNumber[0] = mcdCsd->blockNumber;
        g_blockLen[0] = 512;
        MCD_TRACE(TSTDOUT, 0, "v2block number = %d.",mcdCsd->blockNumber);
        MCD_TRACE(TSTDOUT, 0, "v2 g_blockLen =%d",g_blockLen);
        // hal_HstSendEvent(0x115501);
        // hal_HstSendEvent((UINT32)g_blockNumber[0]);
        // hal_HstSendEvent((UINT32)g_blockLen[0]);

    }
    MCD_TRACE(TSTDOUT, 0, "CSD:csdStructure = %d", mcdCsd->csdStructure        );
    MCD_TRACE(TSTDOUT, 0, "CSD:specVers     = %d", mcdCsd->specVers           );
    MCD_TRACE(TSTDOUT, 0, "CSD:taac         = %d", mcdCsd->taac                );
    MCD_TRACE(TSTDOUT, 0, "CSD:nsac         = %d", mcdCsd->nsac                );
    MCD_TRACE(TSTDOUT, 0, "CSD:tranSpeed    = %d", mcdCsd->tranSpeed           );
    MCD_TRACE(TSTDOUT, 0, "CSD:ccc          = %d", mcdCsd->ccc             );
    MCD_TRACE(TSTDOUT, 0, "CSD:readBlLen    = %d", mcdCsd->readBlLen           );
    MCD_TRACE(TSTDOUT, 0, "CSD:readBlPartial = %d", mcdCsd->readBlPartial      );
    MCD_TRACE(TSTDOUT, 0, "CSD:writeBlkMisalign = %d", mcdCsd->writeBlkMisalign    );
    MCD_TRACE(TSTDOUT, 0, "CSD:readBlkMisalign  = %d", mcdCsd->readBlkMisalign    );
    MCD_TRACE(TSTDOUT, 0, "CSD:dsrImp       = %d", mcdCsd->dsrImp              );
    MCD_TRACE(TSTDOUT, 0, "CSD:cSize        = %d", mcdCsd->cSize               );
    MCD_TRACE(TSTDOUT, 0, "CSD:vddRCurrMin  = %d", mcdCsd->vddRCurrMin     );
    MCD_TRACE(TSTDOUT, 0, "CSD:vddRCurrMax  = %d", mcdCsd->vddRCurrMax     );
    MCD_TRACE(TSTDOUT, 0, "CSD:vddWCurrMin  = %d", mcdCsd->vddWCurrMin     );
    MCD_TRACE(TSTDOUT, 0, "CSD:vddWCurrMax  = %d", mcdCsd->vddWCurrMax     );
    MCD_TRACE(TSTDOUT, 0, "CSD:cSizeMult    = %d", mcdCsd->cSizeMult           );
    MCD_TRACE(TSTDOUT, 0, "CSD:eraseBlkEnable = %d", mcdCsd->eraseBlkEnable        );
    MCD_TRACE(TSTDOUT, 0, "CSD:sectorSize   = %d", mcdCsd->sectorSize          );
    MCD_TRACE(TSTDOUT, 0, "CSD:wpGrpSize    = %d", mcdCsd->wpGrpSize           );
    MCD_TRACE(TSTDOUT, 0, "CSD:wpGrpEnable  = %d", mcdCsd->wpGrpEnable     );
    MCD_TRACE(TSTDOUT, 0, "CSD:r2wFactor    = %d", mcdCsd->r2wFactor           );
    MCD_TRACE(TSTDOUT, 0, "CSD:writeBlLen   = %d", mcdCsd->writeBlLen          );
    MCD_TRACE(TSTDOUT, 0, "CSD:writeBlPartial = %d", mcdCsd->writeBlPartial        );
    MCD_TRACE(TSTDOUT, 0, "CSD:fileFormatGrp = %d", mcdCsd->fileFormatGrp       );
    MCD_TRACE(TSTDOUT, 0, "CSD:copy  = %d", mcdCsd->copy                 );
    MCD_TRACE(TSTDOUT, 0, "CSD:permWriteProtect = %d", mcdCsd->permWriteProtect      );
    MCD_TRACE(TSTDOUT, 0, "CSD:tmpWriteProtect  = %d", mcdCsd->tmpWriteProtect );
    MCD_TRACE(TSTDOUT, 0, "CSD:fileFormat       = %d", mcdCsd->fileFormat          );
    MCD_TRACE(TSTDOUT, 0, "CSD:crc              = %d", mcdCsd->crc                );
    MCD_TRACE(TSTDOUT, 0, "CSD:block number     = %d", mcdCsd->blockNumber        );

    return MCD_ERR_NO;
}


PUBLIC MCD_ERR_T mcd_spi_GetCardSize(MCD_CARD_SIZE_T* size)
{
    size->blockLen = g_blockLen[0];
    size->nbBlock  = g_blockNumber[0];

    return MCD_ERR_NO;
}


// =============================================================================
//  hal_CalculateVal
// =============================================================================
PRIVATE UINT32 hal_CalculateVal(UINT8 code)
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
// mcd_spi_Open
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

PUBLIC MCD_ERR_T mcd_spi_Open(MCD_CSD_T* mcdCsd, MCD_CARD_VER mcdVer,MCD_SDMMC_SPI_IF* mcdConfig)
{
    MCD_ERR_T errorStatus = MCD_ERR_NO;
    UINT16 cmdStatus;
    UINT8 csdRdBlLen,csdWrBlLen = 0;
    UINT16 i, j;
    bool End_Init;
    UINT16 time55 = 0;
    //hal_HstSendEvent(0x33330008);
    pmd_EnablePower(PMD_POWER_SDMMC, TRUE);
    mcd_spi_delay(1638);

    MCD_TRACE(MCD_INFO_TRC, 0, "* MCD_SPI_OPEN %d",g_mmcsd_open);
    MCD_PROFILE_FUNCTION_ENTER(mcd_spi_Open);
    if(g_mmcsd_open == TRUE)
    {
        return MCD_ERR_NO;
    }
    hal_SpiForcePin(HAL_SPI,HAL_SPI_PIN_CS0, HAL_SPI_PIN_STATE_SPI);
    hal_SpiForcePin(HAL_SPI,HAL_SPI_PIN_CLK, HAL_SPI_PIN_STATE_SPI);
    hal_SpiForcePin(HAL_SPI,HAL_SPI_PIN_DO, HAL_SPI_PIN_STATE_SPI);
    hal_SpiForcePin(HAL_SPI,HAL_SPI_PIN_DI, HAL_SPI_PIN_STATE_SPI);
    sxr_Sleep(1638);
    g_mcdConfig = mcdConfig;
    g_mcdSpiCs = g_mcdConfig->usedSpiCs0;
    g_mcdSpiId = g_mcdConfig->usedSpiId;
    if(g_mcdSpiCs == HAL_SPI_CS0)
        g_mcdSpiPinId   = HAL_SPI_PIN_CS0;
    else
        g_mcdSpiPinId   = HAL_SPI_PIN_CS1;
    // Configure SPI
    /// ctrl
    g_spiConfig.enabledCS = g_mcdSpiCs;
    g_spiConfig.inputEn = TRUE;
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
    g_spiConfig.spiFreq = 300000;
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
    g_spiConfig.csActiveLow = TRUE;
    hal_SpiOpen(g_mcdSpiId, g_mcdSpiCs, &g_spiConfig);

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
    if (hal_SpiActivateCs(g_mcdSpiId, g_mcdSpiCs)==FALSE)
    {
        hal_SpiClose(g_mcdSpiId, g_mcdSpiCs);
        MCD_PROFILE_FUNCTION_EXIT(mcd_spi_Open);
        hal_HstSendEvent(0x3333001e);
        return MCD_ERR_SPI_BUSY;
    }

    //-----------------------------------------------------//
    //          Init Process  - Send more than 74 clocks   //
    //          Data Line must be 1
    //          CS must be 1
    //-----------------------------------------------------//

    hal_SpiForcePin(g_mcdSpiId, g_mcdSpiPinId, HAL_SPI_PIN_STATE_1);
    // Send check busy command which only sends 0xff
    // 17*8 = 136 clocks
    errorStatus = mcd_SendCommand(0xff, 0, &cmdStatus,0,NULL);
    hal_SpiForcePin(g_mcdSpiId, g_mcdSpiPinId, HAL_SPI_PIN_STATE_SPI);
    if(errorStatus != MCD_ERR_NO )
    {
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        hal_SpiClose(g_mcdSpiId, g_mcdSpiCs);
        MCD_TRACE(MCD_INFO_TRC, 0, " MCD_SPI_OPEN error %d",errorStatus);
        return errorStatus;
    }
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
            mcd_spi_delay(SLEEP_DURATION_CMD);
            errorStatus = mcd_SendCommand(8,0x1aa,&cmdStatus,0,NULL);

            if ((cmdStatus & 0xFB) == 0x1)
            {
                if((cmdStatus & 0xFF) == 0x1)
                {
                    MCD_TRACE(TSTDOUT, 0, "This card doesn't comply to the spec version 2.0");
                }
                else
                {
                    MCD_TRACE(TSTDOUT, 0, "This card   comply to the spec version 2.0");
                }
                // If in Idle state, continue procedure
                MCD_TRACE(MCD_INFO_TRC, 0, "MMC/SD in IDLE State. CMD0 sent %d times", j+1);
                End_Init = TRUE;
                break;
            }
            else
            {
                // The status is not Idle
                // Retry but set the status to timeout and indicate the failure
                mcd_spi_delay(SLEEP_DURATION);
                errorStatus = MCD_ERR_CARD_TIMEOUT;
                End_Init = FALSE;
            }
        }
        // Retry until CMD0_RETRY_NB is reached

        // Sleep a bit to leave some time to the card to init itself.
        mcd_spi_delay(SLEEP_DURATION_CMD);
    }
    MCD_TRACE(MCD_INFO_TRC,0,"End_Init: %i", End_Init);
    // We did not manage to set the card in Idle state - Exit
    if (End_Init == FALSE)
    {
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        // Indicate that SPI is free
        // g_mcdSpiFree = TRUE;
        hal_SpiClose(g_mcdSpiId, g_mcdSpiCs);

        MCD_PROFILE_FUNCTION_EXIT(mcd_spi_Open);
        hal_HstSendEvent(0x3333003e);
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
        time55 ++;
        if (errorStatus == MCD_ERR_NO)
        {
            // CMD successfully sent
            if (((cmdStatus & 0xff) == 0x1)||(time55>100))
            {
                // CMD 55 successfully sent
                // Send ACMD 41
                errorStatus = mcd_SendCommand(41,0x40000000,&cmdStatus,0,NULL);
                if (errorStatus == MCD_ERR_NO)
                {
                    // CMD successfully sent
                    if ((cmdStatus & 0xff) == 0x0)
                    {
                        MCD_TRACE(MCD_INFO_TRC, 0, "Send ACMD41 OK & cmdStatus&ff=0. cmdStatus: 0x %x", cmdStatus);
                        MCD_TRACE(MCD_INFO_TRC, 0, "SD in READY State. ACMD41 sent %d times", j+1);
                        // If out of Idle state, continue procedure
                        End_Init = TRUE;
                        errorStatus = mcd_SendCommand(58,0,&cmdStatus,0,NULL);
                    }
                }
                else
                {
                    hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
                    // Indicate that SPI is free
                    // g_mcdSpiFree = TRUE;
                    hal_SpiClose(g_mcdSpiId, g_mcdSpiCs);
                    MCD_PROFILE_FUNCTION_EXIT(mcd_spi_Open);
                    hal_HstSendEvent(0x3333004e);
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
            hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
            // Indicate that SPI is free
            // g_mcdSpiFree = TRUE;
            hal_SpiClose(g_mcdSpiId, g_mcdSpiCs);
            MCD_PROFILE_FUNCTION_EXIT(mcd_spi_Open);
            return errorStatus;
        }
        if (End_Init == TRUE) break;

        // Sleep a bit to leave some time to the card to init itself.
        mcd_spi_delay(SLEEP_DURATION_CMD);
    }

    if (End_Init == FALSE)
    {
        MCD_PROFILE_FUNCTION_EXIT(mcd_spi_Open);
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        // Indicate that SPI is free
        // g_mcdSpiFree = TRUE;
        hal_SpiClose(g_mcdSpiId, g_mcdSpiCs);
        //MCD_TRACE(MCD_INFO_TRC, 0, " here");
        hal_HstSendEvent(0x3333005e);
        return MCD_ERR_CARD_TIMEOUT;
    }

    if (IsMMC == TRUE)
    {

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
                hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
                // Indicate that SPI is free
                // g_mcdSpiFree = TRUE;
                hal_SpiClose(g_mcdSpiId, g_mcdSpiCs);
                MCD_PROFILE_FUNCTION_EXIT(mcd_spi_Open);
                return errorStatus;
            }
            // Sleep a bit to leave some time to the card to init itself.
            mcd_spi_delay(SLEEP_DURATION_CMD);
        }

        if (End_Init == FALSE)
        {
            MCD_PROFILE_FUNCTION_EXIT(mcd_spi_Open);
            hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
            // Indicate that SPI is free
            // g_mcdSpiFree = TRUE;
            hal_SpiClose(g_mcdSpiId, g_mcdSpiCs);
            hal_HstSendEvent(0x3333006e);
            return MCD_ERR_CARD_TIMEOUT;
        }
    }

    errorStatus = mcd_ReadCsd(mcdCsd);
    if (errorStatus != MCD_ERR_NO)
    {
        MCD_PROFILE_FUNCTION_EXIT(mcd_spi_Open);
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        // Indicate that SPI is free
        // g_mcdSpiFree = TRUE;
        hal_SpiClose(g_mcdSpiId, g_mcdSpiCs);
        hal_HstSendEvent(0x3333007e);
        return errorStatus;
    }

    // Set block lentgh
    csdWrBlLen = mcdCsd->writeBlLen;
    csdRdBlLen = mcdCsd->readBlLen;

    if (csdWrBlLen == csdRdBlLen)
    {
        // g_blockLen[g_mcdSpiCs] = csdWrBlLen;
    }
    else if (csdWrBlLen < csdRdBlLen && mcdCsd->readBlPartial)
    {
        // If Write block length < Read block length and read partial block allowed
        // g_blockLen[g_mcdSpiCs] = csdWrBlLen;
    }
    else if (csdWrBlLen > csdRdBlLen && mcdCsd->writeBlPartial)
    {
        // If Write block length > Read block length and read partial block allowed
        // g_blockLen[g_mcdSpiCs] = csdRdBlLen;
    }
    else
    {
        hal_SpiClose(g_mcdSpiId, g_mcdSpiCs);
        return MCD_ERR_BLOCK_LEN;
    }

    g_writeBlockLen[g_mcdSpiCs] = 512;//mcd_ConvertBlockLength(g_blockLen[g_mcdSpiCs]);
    g_readBlockLen[g_mcdSpiCs] = 512;//g_writeBlockLen[g_mcdSpiCs];
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
            MCD_PROFILE_FUNCTION_EXIT(mcd_spi_Open);
            hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
            // Indicate that SPI is free
            // g_mcdSpiFree = TRUE;
            hal_SpiClose(g_mcdSpiId, g_mcdSpiCs);
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
                MCD_PROFILE_FUNCTION_EXIT(mcd_spi_Open);
                hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
                // Indicate that SPI is free
                // g_mcdSpiFree = TRUE;
                hal_SpiClose(g_mcdSpiId, g_mcdSpiCs);
                hal_HstSendEvent(0x3333008e);
                return MCD_ERR_CSD;
        }
        rate = rate_val*rate_unit;
        MCD_TRACE(MCD_INFO_TRC, 0, "MMC/SD Speed Rating: %d kHz", rate);
        clkdivider = (78000/rate)/2;
        // MCD_TRACE(MCD_INFO_TRC, 0, " Clk divider: %d", clkdivider);
        // SD max frequency: 1.1Mhz: 1100000
        //g_spiConfig.spiFreq = 1100000;  // can be 20000000 20mhz
        g_spiConfig.spiFreq = 20000000;
        // MMC and Tflash work at minimal frequency of 1.5Mhz-TFlash=>4Mhz
        if (IsMMC == TRUE)
        {
            // g_spiConfig.spiFreq = 2000000;
        }
        //MCD_TRACE(MCD_INFO_TRC, 0, "MMC/SD Used Speed @52MHz: %d kHz", 52000/(g_spiConfig.spiFreq+1)/2);
        //MCD_TRACE(MCD_INFO_TRC, 0,"MMC/SD Used Speed @78MHz: %d kHz",78000/(g_spiConfig.spiFreq+1)/2);
        MCD_TRACE(MCD_INFO_TRC, 0,"MMC/SD Sector size : %d bytes",1<<mcdCsd->readBlLen);
    }
    // Close the SPI and Re-open with the new clock settings
    hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
    hal_SpiClose(g_mcdSpiId, g_mcdSpiCs);
    hal_SpiOpen(g_mcdSpiId, g_mcdSpiCs, &g_spiConfig);

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
            MCD_PROFILE_FUNCTION_EXIT(mcd_spi_Open);
            hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
            // Indicate that SPI is free
            // g_mcdSpiFree = TRUE;
            hal_SpiClose(g_mcdSpiId, g_mcdSpiCs);
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
    g_mmcsd_open = TRUE;

    MCD_PROFILE_FUNCTION_EXIT(mcd_spi_Open);
    return MCD_ERR_NO;
}




// =============================================================================
// mcd_Write
// -----------------------------------------------------------------------------
/// @brief Write a block of data to MMC.
///
/// This function is used to write a block of data on the MMC.
/// @param startAddr Start Adress  of the MMC memory block where the
/// data will be written
/// @param blockWr Pointer to the block of data to write
/// @param blockLength length of the block, in bytes (The max block length
/// is defined in CSD register field WRITE_BL_LEN)
// =============================================================================
UINT8 temp_rdDataBlock[1024];
PUBLIC MCD_ERR_T mcd_spi_Read_S(UINT32 startAddr, UINT8* rdDataBlock, UINT32 blockLength);



PUBLIC MCD_ERR_T mcd_spi_Write_s(UINT32 startAddr, CONST UINT8* blockWr, UINT32 blockLength)
{
    UINT16    i;
    UINT16    cmdStatus;
    MCD_ERR_T errorStatus;
    UINT8     commandBuffer[MCD_COMMAND_BUFFER_SIZE];
    UINT32 startTime = 0;
    UINT32 now1,now;
    if(g_mmcsd_open == FALSE)
    {
        // hal_HstSendEvent(0x22220005);
        MCD_ASSERT(0, "mcd_spi_Write when open fail");

        return MCD_ERR_UNUSABLE_CARD;
    }

    MCD_ASSERT(blockLength == 512, "fixed 512");
    if(g_mcdCardIsSdhc == TRUE)
    {
        //MCD_ASSERT(startAddr%512 == 0, "must is 512*n");

        startAddr = startAddr;
    }
    else
    {
        startAddr = startAddr*512;
    }
    now = hal_TimGetUpTime();

    // tmpRdBuff = sxr_Malloc(512 + 64 +16);

    //MCD_TRACE(TSTDOUT, 0, "************ MCD_WRITE ************");

    MCD_PROFILE_FUNCTION_ENTER(mcd_Write);

    if (hal_SpiActivateCs(g_mcdSpiId, g_mcdSpiCs)==FALSE)
    {
        MCD_PROFILE_FUNCTION_EXIT(mcd_Write);

        return MCD_ERR_SPI_BUSY;
    }

    //MCD_TRACE(TSTDOUT, 0, "blockWr mcd_write: 0x %x ,blockStartAddr 0x%x", blockWr,startAddr);

    //--------------------------------------//
    //       Set block Length       //
    //--------------------------------------//
    // Write CMD16
    //MCD_TRACE(TSTDOUT, 0, "Block len g_writeBlockLen: %d", g_writeBlockLen[g_mcdSpiCs]);

    // g_writeBlockLen length of the block, in bytes (The max block length
    // is defined in CSD register field WRITE_BL_LEN)
    if(g_mcdCardIsSdhc == FALSE)
    {
        // errorStatus = mcd_SendCommand(16, 512, &cmdStatus,0,NULL);

        errorStatus = MCD_ERR_NO;
        if (errorStatus != MCD_ERR_NO)
        {
            MCD_PROFILE_FUNCTION_EXIT(mcd_Write);
            MCD_TRACE(TSTDOUT, 0, "Write deactivate0");
            hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
            // Indicate that SPI is free
            // g_mcdSpiFree = TRUE;

            return errorStatus;
        }

    }
    //--------------------------------------//
    //           Write a block              //
    //--------------------------------------//

    // Empties the SPI Fifo and clear the DMA Done bits.

    hal_SpiFlushFifos(g_mcdSpiId);
    hal_SpiClearRxDmaDone(g_mcdSpiId);
    hal_SpiClearTxDmaDone(g_mcdSpiId);

    MCD_TRACE(TSTDOUT, 0, "SD destination block startAddr : 0x %x", startAddr);
    // Write a single block (CMD24: 0x58)
    commandBuffer[0] = 0xff;
    commandBuffer[1] = 0x58;
    commandBuffer[2] = (startAddr >> 24) & 0xff;
    commandBuffer[3] = (startAddr >> 16) & 0xff;
    commandBuffer[4] = (startAddr >> 8) & 0xff;
    commandBuffer[5] = startAddr & 0xff;
    commandBuffer[6] = 0x1;

    // Start byte of data token.
    commandBuffer[MCD_COMMAND_BUFFER_SIZE-1] = 0xfe;

    // Get buffer
    // g_dmaBusy=hal_SpiGetData(g_mcdSpiId, g_mcdSpiCs, (UINT8*) tmpRdBuff, g_writeBlockLen[g_mcdSpiCs] + 64);
    if(g_special_card2 == TRUE)
    {
        g_dmaBusy=  hal_SpiGetData(g_mcdSpiId, g_mcdSpiCs, (UINT8*) g_SpiMcdtmpRdBuff,512+16+64);
    }
    else
    {
        g_dmaBusy= hal_SpiGetData(g_mcdSpiId, g_mcdSpiCs, (UINT8*) g_SpiMcdtmpRdBuff,515+16);
    }
    if (g_dmaBusy == 0)
    {
        MCD_PROFILE_FUNCTION_EXIT(mcd_Write);
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        // Indicate that SPI is free
        // g_mcdSpiFree = TRUE;

        return MCD_ERR_DMA_BUSY;
    }

    // Send buffer
    // Reinit the end of the Tx data buffer.
    for (i = 7; i < MCD_COMMAND_BUFFER_SIZE-1; i++)
    {
        commandBuffer[i] = 0xff;
    }

#if 0
    g_dmaBusy=hal_SpiSendData(g_mcdSpiId, g_mcdSpiCs, (UINT8*) blockWr, g_writeBlockLen + 64);
    if (g_dmaBusy == 0)
    {
        // Stop the Rx transfer.
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        // Indicate that SPI is free
        // g_mcdSpiFree = TRUE;
        MCD_PROFILE_FUNCTION_EXIT(mcd_Write);
        //hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        sxr_Free(tmpRdBuff);
        return MCD_ERR_DMA_BUSY;
    }
#else
    hal_SpiForcePin(g_mcdSpiId, g_mcdSpiPinId, HAL_SPI_PIN_STATE_0);
    g_dmaBusy=hal_SpiSendData(g_mcdSpiId, g_mcdSpiCs, (UINT8*) &commandBuffer[0], 16);
    if (g_dmaBusy == 0)
    {
        // Stop the Rx transfer.
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        // hal_HstSendEvent(0x11112225);

        // Indicate that SPI is free
        // g_mcdSpiFree = TRUE;
        MCD_PROFILE_FUNCTION_EXIT(mcd_Write);
        //hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);

        return MCD_ERR_DMA_BUSY;
    }
    mcd_spi_delay(SLEEP_DURATION_WR_POLL);
    // Command and data are sent during the same low value of the CS.
    // Any time could be waited here (from us to minutes)

    // TODO: try to use the size passed as parameter, or check/assert it, if the CSD
    // allow that (rd, wr partial thing)
    if(g_special_card2 == TRUE)
    {
#ifdef UCACHE_MMC
        g_dmaBusy=hal_SpiSendData(g_mcdSpiId, g_mcdSpiCs, blockWr|0xa0000000,576);
#else
        g_dmaBusy=hal_SpiSendData(g_mcdSpiId, g_mcdSpiCs, blockWr,576);
#endif

    }
    else
    {
#ifdef UCACHE_MMC
        g_dmaBusy=hal_SpiSendData(g_mcdSpiId, g_mcdSpiCs, blockWr|0xa0000000,515);
#else
        g_dmaBusy=hal_SpiSendData(g_mcdSpiId, g_mcdSpiCs, blockWr,515);
#endif
    }
    if (g_dmaBusy == 0)
    {
        // Stop the Rx transfer.
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        // Indicate that SPI is free
        // g_mcdSpiFree = TRUE;
        MCD_PROFILE_FUNCTION_EXIT(mcd_Write);
        //hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);

        return MCD_ERR_DMA_BUSY;
    }

    hal_SpiForcePin(g_mcdSpiId, g_mcdSpiPinId, HAL_SPI_PIN_STATE_SPI);
#endif

    // Wait
    startTime = hal_TimGetUpTime();
    while ((!hal_SpiRxDmaDone(g_mcdSpiId,g_mcdSpiCs))||(!hal_SpiTxDmaDone(g_mcdSpiId,g_mcdSpiCs)))
    {
        if (hal_TimGetUpTime() - startTime >  (MCD_SPI_WRITE_TIMEOUT*1))
        {
            hal_SpiFlushFifos(g_mcdSpiId);
            hal_SpiClearRxDmaDone(g_mcdSpiId);
            hal_SpiClearTxDmaDone(g_mcdSpiId);
            hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
            hal_SpiTxIfcChannelRelease(g_mcdSpiId);
            hal_SpiRxIfcChannelRelease(g_mcdSpiId);
            // hal_HstSendEvent(0x11112224);
            return MCD_ERR_CARD_TIMEOUT;

        }

    }
    i=0;
    if(g_special_card == TRUE)
    {
        if(g_special_card2 == TRUE)
        {
            mcd_spi_delay(16384/35);
        }
        else
        {
            mcd_spi_delay(16384/30);
        }
    }
    else
    {
        if(g_special_card2 == TRUE)
        {
            mcd_spi_delay(16384/35);
        }

    }
    // Polling until Previous Write finishes
    while ( i < MCD_WRITE_TIMEOUT )
    {
        // Check busy signal
        // FIXME Get rid of that if {} ?
        if (hal_SpiActivateCs(g_mcdSpiId, g_mcdSpiCs)==FALSE)
        {
            MCD_PROFILE_FUNCTION_EXIT(mcd_Write);

            return MCD_ERR_SPI_BUSY;
        }
        // ENd of get rid of ...

        errorStatus = mcd_SendCommand(0xff, 0, &cmdStatus,0,NULL);
        if ( errorStatus == MCD_ERR_NO )
        {
            break;
        }
        i++;
        mcd_spi_delay(SLEEP_DURATION_WR_POLL);
    }
    hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
    // Indicate that SPI is free
    // g_mcdSpiFree = TRUE;
    MCD_PROFILE_FUNCTION_EXIT(mcd_Write);

    if ( i == MCD_WRITE_TIMEOUT )
    {

        return MCD_ERR_CARD_TIMEOUT;
    }

    now1 = hal_TimGetUpTime();
    //MCD_TRACE(TSTDOUT, 0, " spi mcd_Write ok 0x%x   %dms ",cmdStatus,((now1-now)*1000)/16384);

    return MCD_ERR_NO;
}

PUBLIC MCD_ERR_T mcd_spi_Write(UINT32 startAddr, CONST UINT8* blockWr, UINT32 blockLength)
{
    UINT32 kk = 0,flag = 1,conunt = 0;
    if(g_special_card2 == TRUE)
    {
        while(flag == 1)
        {
            conunt ++;
            flag = 0;
            if(conunt > 5) {hal_HstSendEvent(0xaabbaabb); break;}
            mcd_spi_Write_s(startAddr,blockWr,blockLength);
            mcd_spi_Read(startAddr,temp_rdDataBlock,blockLength);
            for(kk = 0; kk<512; kk++)
            {
                if(temp_rdDataBlock[kk]!=blockWr[kk])
                {
                    flag = 1; hal_HstSendEvent(0xccccaabb); break;
                }
            }


        }
        return MCD_ERR_NO;
    }
    else
    {
        return mcd_spi_Write_s( startAddr,   blockWr,  blockLength);
    }


}
// =============================================================================
// mcd_Read
// -----------------------------------------------------------------------------
/// @brief Read using pattern mode.
/// @ param startAddr: of the MMC memory block where the data
/// will be read
/// @param blockRd Pointer to the buffer where the data will be stored
/// @param blockLength length of the block, in bytes (The max block length
/// is defined in CSD register field WRITE_BL_LEN)
// =============================================================================

PUBLIC MCD_ERR_T mcd_spi_Read_S(UINT32 startAddr, UINT8* rdDataBlock, UINT32 blockLength)
{
    //UINT8 commandBuffer[MCD_COMMAND_BUFFER_SIZE];
    HAL_SPI_INFINITE_TX_T infTx;
    MCD_ERR_T errorStatus = MCD_ERR_NO;
    UINT16    cmdStatus,Index_flag_data = 0;
    UINT32 i;
    UINT32 startTime=0;
    UINT32 now,now1;
    if(g_mmcsd_open == FALSE)
    {
        // hal_HstSendEvent(0x22220004);
        MCD_ASSERT(0, "mcd_spi_Read when open fail");

        return MCD_ERR_UNUSABLE_CARD;
    }



    MCD_ASSERT(blockLength == 512, "fixed 512");
    //MCD_TRACE(TSTDOUT, 0, "************ MCD_READ ************");
    if(g_mcdCardIsSdhc == TRUE)
    {
        // MCD_ASSERT(startAddr%512 == 0, "must is 512*n");

        startAddr = startAddr;
    }
    else
    {
        startAddr = startAddr*512;
    }
    now = hal_TimGetUpTime();

    // Config SPI Infinite
    infTx.sendOne     = TRUE;
    infTx.autoStop    = TRUE;
    infTx.patternMode = HAL_SPI_PATTERN_UNTIL;
    infTx.pattern     = 0xfe;

    if (hal_SpiActivateCs(g_mcdSpiId, g_mcdSpiCs)==FALSE)
    {
        return MCD_ERR_SPI_BUSY;
    }
    if(g_mcdCardIsSdhc == FALSE)
    {
        errorStatus = mcd_SendCommand(16, 512, &cmdStatus,0,NULL);
    }
    if(MCD_ERR_NO != errorStatus)
    {
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        return errorStatus;
    }
    // Read a block
    hal_SpiFlushFifos(g_mcdSpiId);
    hal_SpiClearRxDmaDone(g_mcdSpiId);
    hal_SpiClearTxDmaDone(g_mcdSpiId);

    g_SpiMcdRdCmdBuf[0] = 0xff;
    g_SpiMcdRdCmdBuf[1] = 0x51;
    g_SpiMcdRdCmdBuf[2] = (startAddr >> 24) & 0xff;
    g_SpiMcdRdCmdBuf[3] = (startAddr >> 16) & 0xff;
    g_SpiMcdRdCmdBuf[4] = (startAddr >> 8) & 0xff;
    g_SpiMcdRdCmdBuf[5] = startAddr & 0xff;
    g_SpiMcdRdCmdBuf[6] = 0x1;

    hal_SpiStartInfiniteWriteMode(g_mcdSpiId, g_mcdSpiCs, &infTx);
    //  RDCMD_BUFFER_LEN
    // Receive Buffer
    g_dmaBusy = hal_SpiGetData(g_mcdSpiId, g_mcdSpiCs, (UINT8*)g_SpiMcdRxBuf, RDCMD_BUFFER_LEN);
//  g_dmaBusy = hal_SpiGetData(g_mcdSpiId, g_mcdSpiCs, (UINT8*)rdDataBlock, RDCMD_BUFFER_LEN);

    if (g_dmaBusy==0)
    {
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        // Indicate that SPI is free
        // g_mcdSpiFree = TRUE;
        return MCD_ERR_DMA_BUSY;
    }

    //MCD_TRACE(TSTDOUT, 0, "Rx buffer Rd_Data_Block->data: 0x%x", rdDataBlock);
    //MCD_TRACE(TSTDOUT, 0, "Rx buffer g_SpiMcdRxBuf: 0x%x", g_SpiMcdRxBuf);

    g_dmaBusy = hal_SpiSendData(g_mcdSpiId, g_mcdSpiCs, (UINT8*)g_SpiMcdRdCmdBuf, RDCMD_BUFFER_LEN);

    if (g_dmaBusy == 0)
    {
        // Stop the Rx transfer.
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        // hal_HstSendEvent(0x11112222);
        // Indicate that SPI is free
        // g_mcdSpiFree = TRUE;
        return MCD_ERR_DMA_BUSY;
    }

    // Wait
    startTime = hal_TimGetUpTime();
    while ((!hal_SpiRxDmaDone(g_mcdSpiId,g_mcdSpiCs))||(!hal_SpiTxDmaDone(g_mcdSpiId,g_mcdSpiCs)))
    {
        if (hal_TimGetUpTime() - startTime > (MCD_SPI_READ_TIMEOUT*1))
        {
            hal_SpiStopInfiniteWriteMode(g_mcdSpiId, g_mcdSpiCs);
            infTx.pattern     = 0xff;
            hal_SpiStartInfiniteWriteMode(g_mcdSpiId, g_mcdSpiCs, &infTx);
            g_dmaBusy = hal_SpiSendData(g_mcdSpiId, g_mcdSpiCs, (UINT8*)&g_SpiMcdRdCmdBuf[MCD_COMMAND_BUFFER_SIZE],RD_SINGLE_BLOCK_LEN-MCD_COMMAND_BUFFER_SIZE);
            sxr_Sleep(1638);
            hal_SpiForcePin(g_mcdSpiId, HAL_SPI_PIN_CS0, HAL_SPI_PIN_STATE_SPI);
            hal_SpiForcePin(g_mcdSpiId, HAL_SPI_PIN_DO, HAL_SPI_PIN_STATE_SPI);
            hal_SpiFlushFifos(g_mcdSpiId);
            hal_SpiClearRxDmaDone(g_mcdSpiId);
            hal_SpiClearTxDmaDone(g_mcdSpiId);
            hal_SpiTxIfcChannelRelease(g_mcdSpiId);
            hal_SpiRxIfcChannelRelease(g_mcdSpiId);
            hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);

            return MCD_ERR_CARD_TIMEOUT;
        }

    }
    mcd_spi_delay(SLEEP_DURATION_RD);
    //hal_SpiStopInfiniteWriteMode(g_mcdSpiId, g_mcdSpiCs);
    hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
    // To invalidate cache Index_flag

    Index_flag_data =1;

#if 0
    for (i = 0; i < RDCMD_BUFFER_LEN; i++)
    {
        if(g_SpiMcdRxBuf[i] == 0xfe)
        {

            Index_flag_data =i;
            break;
        }

    }

#endif
    for (i = 0; i < 512; i++)
    {
        // rdDataBlock[i] = g_SpiMcdRxBuf[i + Index_flag_data + 1];
        rdDataBlock[i] = g_SpiMcdRxBuf[i ];
    }

    // memcpy((UINT8*)rdDataBlock, (UINT8*)(g_SpiMcdRdCmdBuf), g_readBlockLen);
    // Indicate that SPI is free
    // g_mcdSpiFree = TRUE;
    //MCD_TRACE(TSTDOUT, 0, "Struct Rd_Data_Block: 0x%x", rdDataBlock);
    now1 = hal_TimGetUpTime();
    //MCD_TRACE(TSTDOUT, 0, " spi mcd_Read ok  now1 %d  ,%dms", now1,((now1-now)*1000)/16384);
    return MCD_ERR_NO;
}

PUBLIC MCD_ERR_T mcd_spi_Read(UINT32 startAddr, UINT8* rdDataBlock, UINT32 blockLength)
{

    if(g_special_card2 == TRUE)
    {
        if(MCD_ERR_NO != mcd_spi_Read_S(startAddr,rdDataBlock,blockLength))
        {
            return  mcd_spi_Read_S(startAddr,rdDataBlock,blockLength);
        }
        else
        {
            return MCD_ERR_NO;
        }

    }
    else
    {
        return mcd_spi_Read_S(startAddr,rdDataBlock,blockLength);
    }

}



#if 0
// =============================================================================
//  mcd_CalculateMult
// -----------------------------------------------------------------------------
/// @brief Convert the cSizeMult from CSD register to 2^(csdSizeMult+2).
/// @param csdCSizeMult cSizeMult from CSD register.
/// @return 2^(csdSizeMult+2)
// =============================================================================
PRIVATE UINT16 mcd_CalculateMult(UINT8 csdCSizeMult)
{
    return 1<<(csdCSizeMult+2);
}
#endif

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

#if 0
// =============================================================================
// mcd_SpiFree
// -----------------------------------------------------------------------------
/// @brief Indicate the the SPI is free. Used for SPI sharing
/// To be called after hal_SpiDeActivateCs
// =============================================================================

PRIVATE BOOL mcd_SpiFree(VOID)
{
    return g_mcdSpiFree;
}
#endif

PRIVATE BOOL mcd_CardDetectUpdateStatus(VOID)
{

    if(NULL == g_mcdConfig)
    {
        return FALSE;
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


// =============================================================================
// mcd_spi_Close
// -----------------------------------------------------------------------------
/// @brief Close the SPI-MMC
///
/// To be called at the end of the operations
// =============================================================================
PUBLIC MCD_ERR_T mcd_spi_Close(VOID)
{
    MCD_PROFILE_FUNCTION_ENTER(mcd_spi_Close);
    hal_SpiTxIfcChannelRelease(g_mcdSpiId);
    hal_SpiRxIfcChannelRelease(g_mcdSpiId);
    hal_SpiForcePin(HAL_SPI,HAL_SPI_PIN_CS0, HAL_SPI_PIN_STATE_0);
    hal_SpiForcePin(HAL_SPI,HAL_SPI_PIN_CLK, HAL_SPI_PIN_STATE_0);
    hal_SpiForcePin(HAL_SPI,HAL_SPI_PIN_DO, HAL_SPI_PIN_STATE_0);
    hal_SpiForcePin(HAL_SPI,HAL_SPI_PIN_DI, HAL_SPI_PIN_STATE_0);

    if(g_mmcsd_open == FALSE)
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "mcd_spi_Close() repeat!!!!!!!!");
        MCD_PROFILE_FUNCTION_EXIT(mcd_spi_Close);

        return MCD_ERR_NO;
    }
    mcd_spi_delay(1638);
    ///hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
    hal_SpiClose(g_mcdSpiId, g_mcdSpiCs);
    g_mmcsd_open = FALSE;
    MCD_TRACE(MCD_INFO_TRC, 0, "mcd_spi_Close() ok.");

    MCD_PROFILE_FUNCTION_EXIT(mcd_spi_Close);
    // hal_HstSendEvent(0x22220002);


    return MCD_ERR_NO;
}


PUBLIC MCD_ERR_T mcd_spi_SwitchOperationTo(UINT8 card_number)
{
    UINT32 scStatus = 0;


    if ( TRUE == g_mcdSpiFree)
    {
        if ( card_number == 0)
        {
            scStatus = hal_SysEnterCriticalSection();
            g_mcdSpiCs = HAL_SPI_CS0;
            hal_SysExitCriticalSection(scStatus);
            return MCD_ERR_NO;
        }
        if ( card_number == 1)
        {
            scStatus = hal_SysEnterCriticalSection();
            g_mcdSpiCs = HAL_SPI_CS1;
            hal_SysExitCriticalSection(scStatus);
            return MCD_ERR_NO;
        }
        return MCD_ERR_CARD_NO_RESPONSE;
    }
    return MCD_ERR_CARD_NO_RESPONSE;

}


PUBLIC MCD_STATUS_T mcd_spi_CardStatus(VOID)
{
    mcd_CardDetectUpdateStatus();
    return g_mcdStatus;
}



