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



#if 1


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
typedef enum
{
    SDMMC_NORMAL_VER_1,
    SDMMC_STANDARD_CAPACITY_VER_2,
    SDMMC_HIGH_CAPACITY_VER_2,
    SDMMC_NORMAL_VER_QTY
} SDMMC_VER_T;
BOOL IsMMC=FALSE;

// =============================================================================
//  MACROS
// =============================================================================

// Timeouts
#define HAL_TICK1S 16384

#define SECOND        * HAL_TICK1S

#define MCD_SPI_CMD_TIMEOUT        ( 1 SECOND / 10 )
#define MCD_SPI_RESP_TIMEOUT       ( 1 SECOND / 10 )
#define MCD_SPI_TRAN_TIMEOUT       ( 1 SECOND / 10 )
#define MCD_SPI_READ_TIMEOUT       ( 1 SECOND )
#define MCD_SPI_WRITE_TIMEOUT      ( 1 SECOND )

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
#define MCD_WRITE_TIMEOUT   2000             // max timeout for a write is
#define MCD_MULTI_WRITE_LEN  515 //512+2crc+1respone

//  5s = 5000 *SLEEP_DURATION_WR_POLL

// The number of retry does not depend on the clock divider.
#define CMD1_RETRY_NB       128   // the card is supposed to answer within 1s
//  - Max wait is 128 * 10ms=1,28s
#define CMD0_RETRY_NB       5     // Retry 5 times max
#define ACMD41_RETRY_NB     128   // the card is supposed to answer within 1s
//  - Max wait is 128 * 10ms=1,28s

#define CMDFF_RETRY_NB                           10
#define MCD_COMMAND_BUFFER_SIZE        16

#define MCD_DEV_NUMBER                           2
#define MCD_DEFUALT_BLOCK_SIZE            512

BOOL g_mcdCardIsSdhc = FALSE;
BOOL g_ver2_high_capcity_card = FALSE;
BOOL g_ver2_Standard_card = FALSE;
BOOL g_ver1_card = FALSE;
SDMMC_VER_T g_card_ver = SDMMC_NORMAL_VER_QTY;
UINT32 g_delay_time_for_the_card = 16384/1000*2;
// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================
#define RD_SINGLE_BLOCK_LEN    512+64

#if 0
PRIVATE VOLATILE UINT8       g_SpiMcdRxBuf[RDCMD_BUFFER_LEN]
__attribute__((section(".ucdata")));
PRIVATE VOLATILE UINT8       g_SpiMcdRdCmdBuf[RDCMD_BUFFER_LEN]
__attribute__((section(".ucdata")));
PRIVATE VOLATILE UINT8       g_SpiMcdWrCmdBuf[WRCMD_BUFFER_LEN]
__attribute__((section(".ucdata")));
#else
#define BUFFER_SPI_CARD ((512+64)*4)
#define CMD_BUFFER_LEN   MCD_COMMAND_BUFFER_SIZE*4
PRIVATE VOLATILE UINT8       g_SpiMcdRxBuf[BUFFER_SPI_CARD] __attribute__((section(".ucdata")));
PRIVATE VOLATILE UINT8       g_SpiMcdRdCmdBuf[CMD_BUFFER_LEN] __attribute__((section(".ucdata")));

PRIVATE VOLATILE UINT8       g_SpiMcdWrCmdBuf[CMD_BUFFER_LEN]  __attribute__((section(".ucdata")));


#endif

PRIVATE HAL_SPI_CS_T         g_mcdSpiCs      = HAL_SPI_CS0;
PRIVATE HAL_SPI_ID_T         g_mcdSpiId      = HAL_SPI;
PRIVATE HAL_SPI_PIN_T        g_mcdSpiPinId   = HAL_SPI_PIN_CS0;
PRIVATE UINT16               g_dmaBusy       = 0;
PRIVATE UINT32               g_transferSize  = 0;
PRIVATE UINT16               g_readBlockLen[MCD_DEV_NUMBER]  = {0,0};
PRIVATE UINT16               g_writeBlockLen[MCD_DEV_NUMBER] = {0,0};
PRIVATE UINT16               g_blockLen[MCD_DEV_NUMBER]  = {0,0};
UINT32                       g_blockNumber[MCD_DEV_NUMBER]  = {0,0};
PRIVATE BOOL                 g_mcdSpiFree    = TRUE;
PRIVATE UINT32               g_setBlockSize  = 0xffffffff;

/// HAL SPI CFG
PRIVATE HAL_SPI_CFG_T        g_spiConfig;
/// MCD config
//PRIVATE CONST TGT_MCD_CONFIG_T*  g_mcdConfig;
VOID mcd_TimeDelaly(UINT32 timeout_tick);

// =============================================================================
//  mcd_CalculateMult
// -----------------------------------------------------------------------------
/// Convert the C_SIZE_MULT from CSD register to 2^(csdSizeMult+2).
/// @param csdCSizeMult C_SIZE_MULT from CSD register.
/// @return 2^(csdSizeMult+2)
// =============================================================================
PROTECTED UINT16  mcd_CalculateMult(UINT8 csdCSizeMult);

UINT8 SD_GetCmdByte6(UINT8 cmd, UINT8 *param)
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
// =============================================================================
// mcd_ConvertBlockLength
// -----------------------------------------------------------------------------
/// Calculate W/R block length.
/// @ param blockLen: WRITE_BL_LEN or READ_BL_LEN from CSDregister.
/// @return 2^WRITE_BL_LEN or 2^READ_BL_LEN
// =============================================================================
PRIVATE UINT16 mcd_ConvertBlockLength(UINT8 blockLen);


// =============================================================================
//  mcd_SendCommand mcd_SendCommand(0xff, 0, &cmdStatus,0,NULL);
// -----------------------------------------------------------------------------
/// @brief To send command to the MMC
/// Only valid for commands returning no data and response R1 and R2
// =============================================================================
PRIVATE MCD_ERR_T mcd_SendCommand(UINT8 cmdId, UINT32 argument,UINT16* status,UINT16 dataSize,UINT8** pdata_array)
{
    UINT32 i,retry = 0;
    UINT8 array[5];
    UINT32 startTime = 0;
    if(cmdId == 16)
    {
        if(g_setBlockSize == argument)
        {
            return  MCD_ERR_NO;
        }

    }
    //MCD_TRACE(TSTDOUT, 0, "mcd_SendCommand %d", cmdId);

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
        MCD_TRACE(TSTDOUT, 0, "busy 2");
        return MCD_ERR_DMA_BUSY;
    }

    g_dmaBusy=hal_SpiSendData(g_mcdSpiId, g_mcdSpiCs, (UINT8*) g_SpiMcdRdCmdBuf, g_transferSize);
    mcd_TimeDelaly(SLEEP_DURATION_STD);
    if (g_dmaBusy == 0)
    {
        // Stop the Rx transfer.
        //hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        // Indicate that SPI is free
        MCD_TRACE(TSTDOUT, 0, "busy 1");
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
            MCD_TRACE(TSTDOUT, 0, "busy 7");
            retry ++;
            break;
        }

    }

    if(retry == 1)
    {
        g_dmaBusy=hal_SpiGetData(g_mcdSpiId, g_mcdSpiCs, (UINT8*) g_SpiMcdRxBuf, g_transferSize);
        if (g_dmaBusy==0)
        {
            return MCD_ERR_DMA_BUSY;
        }

        g_dmaBusy=hal_SpiSendData(g_mcdSpiId, g_mcdSpiCs, (UINT8*) g_SpiMcdRdCmdBuf, g_transferSize);

        if (g_dmaBusy == 0)
        {
            // Stop the Rx transfer.
            //hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
            // Indicate that SPI is free
            MCD_TRACE(TSTDOUT, 0, "busy 3");
            return MCD_ERR_DMA_BUSY;
        }
        //mcd_TimeDelaly(SLEEP_DURATION_STD);

        startTime = hal_TimGetUpTime();
        // Wait until the end of both transfers.
        while ((!hal_SpiRxDmaDone(g_mcdSpiId,g_mcdSpiCs))||(!hal_SpiTxDmaDone(g_mcdSpiId,g_mcdSpiCs)))
        {
            if (hal_TimGetUpTime() - startTime >  (MCD_SPI_CMD_TIMEOUT*1))
            {
                hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
                hal_SpiFlushFifos(g_mcdSpiId);
                hal_SpiClearRxDmaDone(g_mcdSpiId);
                hal_SpiClearTxDmaDone(g_mcdSpiId);
                hal_SpiTxIfcChannelRelease(g_mcdSpiId);
                hal_SpiRxIfcChannelRelease(g_mcdSpiId);
                // hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
                hal_HstSendEvent(0x11112227);
                MCD_TRACE(TSTDOUT, 0, "busy 5");
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
                MCD_TRACE(TSTDOUT, 0, "busy 6");
                return MCD_ERR_CARD_TIMEOUT;
            }
            i++;
        }
    }

    *status=g_SpiMcdRxBuf[i] | (g_SpiMcdRxBuf[i+1]<<8);
#if 0
    MCD_TRACE(TSTDOUT, 0, "mcd_SendCommand  status 0x%x cmdId   %d", *status,cmdId);
    MCD_TRACE(TSTDOUT, 0, "mcd_SendCommand  status 0x%x ,0x%x  ,0x%x ,0x%x 0x%x ", g_SpiMcdRxBuf[i],g_SpiMcdRxBuf[i+1],g_SpiMcdRxBuf[i+2],g_SpiMcdRxBuf[i+3],g_SpiMcdRxBuf[i+4]);
    MCD_TRACE(TSTDOUT, 0, "mcd_SendCommand  status 0x%x ,0x%x  ,0x%x ,0x%x 0x%x  ", g_SpiMcdRxBuf[i],g_SpiMcdRxBuf[i-1],g_SpiMcdRxBuf[i-2],g_SpiMcdRxBuf[i-3],g_SpiMcdRxBuf[i+4]);
#endif
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
    }
    if(cmdId == 16) g_setBlockSize = argument;

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
        MCD_TRACE(TSTDOUT, 0, "v1 g_blockLen =%d",g_blockLen[0]);
        if(g_blockLen[0]==512)
        {
            if(g_blockNumber[0] < 4194304)
                g_delay_time_for_the_card =  16384/1000*4 ;
        }
        if(g_blockLen[0]==1024)
        {
            if(g_blockNumber[0] < 4194304/2)
                g_delay_time_for_the_card =  16384/1000*4 ;
        }
    }
    else if(g_card_ver == SDMMC_STANDARD_CAPACITY_VER_2)
    {
        g_blockNumber[0] = mcdCsd->blockNumber;
        g_blockLen[0] = 512;
        MCD_TRACE(TSTDOUT, 0, "v2block number = %d.",mcdCsd->blockNumber);
        MCD_TRACE(TSTDOUT, 0, "v2 g_blockLen =%d",g_blockLen[0]);


    }
    if (mcdCsd->taac > 100) g_delay_time_for_the_card = 16384/1000*30;

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


PUBLIC MCD_ERR_T mcd_GetCardSize(MCD_CARD_SIZE_T* size)
{
    size->blockLen = g_blockLen[0];
    size->nbBlock  = g_blockNumber[0];

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
static bool gMcd_opened=FALSE;

PUBLIC MCD_ERR_T mcd_Open(MCD_CSD_T* mcdCsd )
{
    MCD_ERR_T errorStatus = MCD_ERR_NO;
    UINT16 cmdStatus;
    UINT8 csdRdBlLen,csdWrBlLen = 0;
    UINT16 i, j;
    bool End_Init;
    UINT16 time55 = 0;
    MCD_TRACE(TSTDOUT, 0, "************ MCD_OPEN ************");
    //g_mcdConfig = tgt_GetMcdConfig();
    //g_mcdSpiCs = //g_mcdConfig->usedSpiCs;

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
    MCD_TRACE( TSTDOUT, 0, "Opening MMC/SD");
    hal_SpiOpen(g_mcdSpiId, g_mcdSpiCs, &g_spiConfig);
    // mcd_TimeDelaly(SLEEP_DURATION_OP);
    // Init the Cmd buffer, only used to send command to the MMC.
    // after the command, it is required to send only 0xff to the MMC.
    // For all commands, only the 7 first bytes are modified, for that
    // reason, it is never needed to reinit this buffer with 0xff.
    for (i = 0; i < CMD_BUFFER_LEN; i++)
    {
        g_SpiMcdWrCmdBuf[i] = 0xff;
    }
    //Used to send the data (for the write cmd) to the MMC.
    // Needed to reinit this buffer only when writing data to the MMC,
    // but the part to reinit is very small (see the write function).
    for (i = 0; i < CMD_BUFFER_LEN; i++)
    {
        g_SpiMcdRdCmdBuf[i] = 0xff;
    }

    //-----------------------------//
    // Activate the SPI CS
    // ---------------------------//
    if (hal_SpiActivateCs(g_mcdSpiId, g_mcdSpiCs)==FALSE)
    {
        hal_SpiClose(g_mcdSpiId, g_mcdSpiCs);
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
    if(errorStatus != MCD_ERR_NO)
    {
        return errorStatus;
    }
    //--------------------------------------//
    //          Init Process  CMD0          //
    //--------------------------------------//
    End_Init = FALSE;
    MCD_TRACE(TSTDOUT, 0, "cmdStatus before sending CMD0: %x ", cmdStatus);
    for(j = 0; j < CMD0_RETRY_NB; j++)
    {
        errorStatus = mcd_SendCommand(0,0,&cmdStatus,0,NULL);
        if (errorStatus == MCD_ERR_NO)
        {
            // CMD successfully sent
            MCD_TRACE(TSTDOUT, 0, "cmdStatus: 0x%x ", cmdStatus);

            mcd_TimeDelaly(SLEEP_DURATION_CMD);
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
                MCD_TRACE(TSTDOUT, 0, "MMC/SD in IDLE State. CMD0 sent %d times", j+1);
                End_Init = TRUE;
                break;
            }
            else
            {
                // The status is not Idle
                // Retry but set the status to timeout and indicate the failure
                mcd_TimeDelaly(SLEEP_DURATION);
                errorStatus = MCD_ERR_CARD_TIMEOUT;
                End_Init = FALSE;
            }
        }
        // Retry until CMD0_RETRY_NB is reached

        // Sleep a bit to leave some time to the card to init itself.
        mcd_TimeDelaly(SLEEP_DURATION_CMD);
    }
    MCD_TRACE(TSTDOUT,0,"End_Init: %i", End_Init);
    // We did not manage to set the card in Idle state - Exit
    if (End_Init == FALSE)
    {
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        // Indicate that SPI is free
        // g_mcdSpiFree = TRUE;
        hal_SpiClose(g_mcdSpiId, g_mcdSpiCs);

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
            if  (((cmdStatus & 0xff) == 0x1)||(time55>100))
            {
                // CMD 55 successfully sent
                // Send ACMD 41
                errorStatus = mcd_SendCommand(41,0x40000000,&cmdStatus,0,NULL);
                if (errorStatus == MCD_ERR_NO)
                {
                    // CMD successfully sent
                    if ((cmdStatus & 0xff) == 0x0)
                    {
                        MCD_TRACE(TSTDOUT, 0, "Send ACMD41 OK & cmdStatus&ff=0. cmdStatus: 0x %x", cmdStatus);
                        MCD_TRACE(TSTDOUT, 0, "SD in READY State. ACMD41 sent %d times", j+1);
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
            return errorStatus;
        }
        if (End_Init == TRUE) break;

        // Sleep a bit to leave some time to the card to init itself.
        mcd_TimeDelaly(SLEEP_DURATION_CMD);
    }

    if (End_Init == FALSE)
    {
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        // Indicate that SPI is free
        // g_mcdSpiFree = TRUE;
        hal_SpiClose(g_mcdSpiId, g_mcdSpiCs);
        MCD_TRACE(TSTDOUT, 0, " here");
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
                    MCD_TRACE(TSTDOUT, 0, "MMC in READY State. CMD1 sent %d times", j+1);
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
                return errorStatus;
            }
            // Sleep a bit to leave some time to the card to init itself.
            mcd_TimeDelaly(SLEEP_DURATION_CMD);
        }

        if (End_Init == FALSE)
        {
            hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
            // Indicate that SPI is free
            // g_mcdSpiFree = TRUE;
            hal_SpiClose(g_mcdSpiId, g_mcdSpiCs);
            return MCD_ERR_CARD_TIMEOUT;
        }
    }

    errorStatus = mcd_ReadCsd(mcdCsd);
    if (errorStatus != MCD_ERR_NO)
    {
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        // Indicate that SPI is free
        // g_mcdSpiFree = TRUE;
        hal_SpiClose(g_mcdSpiId, g_mcdSpiCs);
        return errorStatus;
    }

    // Set block lentgh
    csdWrBlLen = mcdCsd->writeBlLen;
    csdRdBlLen = mcdCsd->readBlLen;

    if (csdWrBlLen == csdRdBlLen)
    {
        g_blockLen[g_mcdSpiCs] = csdWrBlLen;
    }
    else if (csdWrBlLen < csdRdBlLen && mcdCsd->readBlPartial)
    {
        // If Write block length < Read block length and read partial block allowed
        g_blockLen[g_mcdSpiCs] = csdWrBlLen;
    }
    else if (csdWrBlLen > csdRdBlLen && mcdCsd->writeBlPartial)
    {
        // If Write block length > Read block length and read partial block allowed
        g_blockLen[g_mcdSpiCs] = csdRdBlLen;
    }
    else
    {
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        hal_SpiClose(g_mcdSpiId, g_mcdSpiCs);
        return MCD_ERR_BLOCK_LEN;
    }

    g_writeBlockLen[g_mcdSpiCs] = mcd_ConvertBlockLength(g_blockLen[g_mcdSpiCs]);
    g_readBlockLen[g_mcdSpiCs] = g_writeBlockLen[g_mcdSpiCs];
    MCD_TRACE(TSTDOUT, 0, "Wr bl len: %d, Rd bl len: %d", mcdCsd->writeBlLen, mcdCsd->readBlLen);

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
                hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
                // Indicate that SPI is free
                // g_mcdSpiFree = TRUE;
                return MCD_ERR_CSD;
        }
        rate = rate_val*rate_unit;
        MCD_TRACE(TSTDOUT, 0, "MMC/SD Speed Rating: %d kHz", rate);
        clkdivider = (78000/rate)/2;
        // MCD_TRACE(TSTDOUT, 0, " Clk divider: %d", clkdivider);
        // SD max frequency: 1.1Mhz: 1100000
        //g_spiConfig.spiFreq = 1100000;  // can be 20000000 20mhz
        g_spiConfig.spiFreq = 20000000;
        // MMC and Tflash work at minimal frequency of 1.5Mhz-TFlash=>4Mhz
        if (IsMMC == TRUE)
        {
            // g_spiConfig.spiFreq = 2000000;
        }
        //MCD_TRACE(TSTDOUT, 0, "MMC/SD Used Speed @52MHz: %d kHz", 52000/(g_spiConfig.spiFreq+1)/2);
        //MCD_TRACE(TSTDOUT, 0,"MMC/SD Used Speed @78MHz: %d kHz",78000/(g_spiConfig.spiFreq+1)/2);
        MCD_TRACE(TSTDOUT, 0,"MMC/SD Sector size : %d bytes",1<<mcdCsd->readBlLen);
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
            MCD_PROFILE_FUNCTION_EXIT(mcd_Open);
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
        MCD_TRACE(TSTDOUT, 0,"MMC/SD TAAC time : %d us",acc_time/10000);
        MCD_TRACE(TSTDOUT, 0,"MMC/SD NSAC time : %d clocks",mcdCsd->nsac*100);
        MCD_TRACE(TSTDOUT, 0,"MMC/SD Read Access time : %d bytes",nb_clk>>3);
        MCD_TRACE(TSTDOUT, 0,"MMC/SD Write Access time : %d bytes",(nb_clk>>3)*(1<<mcdCsd->r2wFactor));
    }
#endif
    gMcd_opened=TRUE;       //
    hal_HstSendEvent(0x22220000);
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
    UINT32 startTime = 0;

    MCD_ASSERT(blockLength == 512, "fixed 512");
    if(g_mcdCardIsSdhc == TRUE)
    {
        MCD_ASSERT(blockStartAddr%512 == 0, "must is 512*n");

        blockStartAddr = blockStartAddr/512;
    }
    UINT32 now = hal_TimGetUpTime();
    if (hal_SpiActivateCs(g_mcdSpiId, g_mcdSpiCs)==FALSE)
    {
        return MCD_ERR_SPI_BUSY;
    }
    //--------------------------------------//
    //       Set block Length       //
    //--------------------------------------//
    // Write CMD16
    // is defined in CSD register field WRITE_BL_LEN)
    errorStatus = mcd_SendCommand(16, 512, &cmdStatus,0,NULL);
    if(MCD_ERR_NO != errorStatus)
    {
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        return errorStatus;
    }
    MCD_TRACE(TSTDOUT, 0, " here2");
    //--------------------------------------//
    //           Write a block              //
    //--------------------------------------//
    // Empties the SPI Fifo and clear the DMA Done bits.
    hal_SpiFlushFifos(g_mcdSpiId);
    hal_SpiClearRxDmaDone(g_mcdSpiId);
    hal_SpiClearTxDmaDone(g_mcdSpiId);

    MCD_TRACE(TSTDOUT, 0, "SD destination block startAddr : 0x %x", blockStartAddr);
    // Write a single block (CMD24: 0x58)
    g_SpiMcdWrCmdBuf[0] = 0xff;
    g_SpiMcdWrCmdBuf[1] = 0x58;
    g_SpiMcdWrCmdBuf[2] = (blockStartAddr >> 24) & 0xff;
    g_SpiMcdWrCmdBuf[3] = (blockStartAddr >> 16) & 0xff;
    g_SpiMcdWrCmdBuf[4] = (blockStartAddr >> 8) & 0xff;
    g_SpiMcdWrCmdBuf[5] = blockStartAddr & 0xff;
    g_SpiMcdWrCmdBuf[6] = 0x1;

    // Start byte of data token.
    g_SpiMcdWrCmdBuf[MCD_COMMAND_BUFFER_SIZE-1] = 0xfe;

    for (i = 7; i < MCD_COMMAND_BUFFER_SIZE-1; i++)
    {
        g_SpiMcdWrCmdBuf[i] = 0xff;
    }
    g_dmaBusy=hal_SpiGetData(g_mcdSpiId, g_mcdSpiCs, (UINT8*) g_SpiMcdRxBuf,MCD_MULTI_WRITE_LEN+MCD_COMMAND_BUFFER_SIZE);
    // Get buffer
    // g_dmaBusy=hal_SpiGetData(g_mcdSpiId, g_mcdSpiCs, (UINT8*) g_SpiMcdRxBuf,RD_SINGLE_BLOCK_LEN);
    if (g_dmaBusy == 0)
    {
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        // Indicate that SPI is free
        // g_mcdSpiFree = TRUE;
        return MCD_ERR_DMA_BUSY;
    }
    // Send buffer
    // Reinit the end of the Tx data buffer.
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
    hal_SpiForcePin(g_mcdSpiId, HAL_SPI_PIN_CS0, HAL_SPI_PIN_STATE_0);
    g_dmaBusy=hal_SpiSendData(g_mcdSpiId, g_mcdSpiCs, (UINT8*) &g_SpiMcdWrCmdBuf[0], MCD_COMMAND_BUFFER_SIZE);
    if (g_dmaBusy == 0)
    {
        // Stop the Rx transfer.
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        hal_SpiForcePin(g_mcdSpiId, HAL_SPI_PIN_CS0, HAL_SPI_PIN_STATE_SPI);

        return MCD_ERR_DMA_BUSY;
    }
    startTime = hal_TimGetUpTime();
    while (!hal_SpiTxDmaDone(g_mcdSpiId,g_mcdSpiCs))
    {
        if (hal_TimGetUpTime() - startTime >  (1638))
        {
            hal_HstSendEvent(0x11112229);
            hal_SpiForcePin(g_mcdSpiId, HAL_SPI_PIN_CS0, HAL_SPI_PIN_STATE_SPI);
            return MCD_ERR_CARD_TIMEOUT;
        }
    }

    MCD_TRACE(TSTDOUT, 0, " here3 ");
    // Command and data are sent during the same low value of the CS.
    // Any time could be waited here (from us to minutes)
    // TODO: try to use the size passed as parameter, or check/assert it, if the CSD
    // allow that (rd, wr partial thing)
    g_dmaBusy=hal_SpiSendData(g_mcdSpiId, g_mcdSpiCs, blockWr,MCD_MULTI_WRITE_LEN);
    if (g_dmaBusy == 0)
    {
        // Stop the Rx transfer.
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        // Indicate that SPI is free
        hal_SpiForcePin(g_mcdSpiId, HAL_SPI_PIN_CS0, HAL_SPI_PIN_STATE_SPI);

        return MCD_ERR_DMA_BUSY;
    }
    MCD_TRACE(TSTDOUT, 0, " here 44");

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
            hal_SpiForcePin(g_mcdSpiId, HAL_SPI_PIN_CS0, HAL_SPI_PIN_STATE_SPI);
            hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
            hal_SpiTxIfcChannelRelease(g_mcdSpiId);
            hal_SpiRxIfcChannelRelease(g_mcdSpiId);
            hal_HstSendEvent(0x11112224);
            return MCD_ERR_CARD_TIMEOUT;
        }

    }
    hal_SpiForcePin(g_mcdSpiId, HAL_SPI_PIN_CS0, HAL_SPI_PIN_STATE_SPI);
    MCD_TRACE(TSTDOUT, 0, " here 8");

    i=0;
    // Polling until Previous Write finishes
    while ( i < MCD_WRITE_TIMEOUT )
    {
        // Check busy signal
        // FIXME Get rid of that if {} ?
        if (hal_SpiActivateCs(g_mcdSpiId, g_mcdSpiCs)==FALSE)
        {
            return MCD_ERR_SPI_BUSY;
        }
        // ENd of get rid of ...
        errorStatus = mcd_SendCommand(0xff, 0, &cmdStatus,0,NULL);
        if ( errorStatus == MCD_ERR_NO )
        {
            break;
        }
        i++;
        mcd_TimeDelaly(SLEEP_DURATION_WR_POLL/2);
    }

    hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
    // Indicate that SPI is free
    if ( i == MCD_WRITE_TIMEOUT )
    {
        return MCD_ERR_CARD_TIMEOUT;
    }

    UINT32 now1 = hal_TimGetUpTime();
    MCD_TRACE(TSTDOUT, 0, " spi mcd_Write ok 0x%x  %dms ",cmdStatus,((now1-now)*1000)/16384);
    return MCD_ERR_NO;
}

#if 0
// g_SpiMcdRxBuf[BUFFER_SPI_CARD]
// g_SpiMcdTxBuf[BUFFER_SPI_CARD]
PRIVATE VOLATILE UINT8       g1_SpiMcdRxBuf[BUFFER_SPI_CARD] __attribute__((section(".ucdata")));
PRIVATE VOLATILE UINT8       g2_SpiMcdRxBuf[BUFFER_SPI_CARD] __attribute__((section(".ucdata")));
PRIVATE VOLATILE UINT8       g3_SpiMcdRxBuf[BUFFER_SPI_CARD] __attribute__((section(".ucdata")));
PRIVATE VOLATILE UINT8       g4_SpiMcdRxBuf[BUFFER_SPI_CARD] __attribute__((section(".ucdata")));
PRIVATE VOLATILE UINT8       g5_SpiMcdRxBuf[BUFFER_SPI_CARD] __attribute__((section(".ucdata")));
PRIVATE VOLATILE UINT8       g6_SpiMcdRxBuf[BUFFER_SPI_CARD] __attribute__((section(".ucdata")));
#endif
MCD_ERR_T mcd_TimeOut(UINT32 timeout_tick)
{
    UINT32 startTime = 0;
    startTime = hal_TimGetUpTime();
    while ((!hal_SpiRxDmaDone(g_mcdSpiId,g_mcdSpiCs))||(!hal_SpiTxDmaDone(g_mcdSpiId,g_mcdSpiCs)))
    {
        if (hal_TimGetUpTime() - startTime >  (timeout_tick))
        {
            hal_SpiFlushFifos(g_mcdSpiId);
            hal_SpiClearRxDmaDone(g_mcdSpiId);
            hal_SpiClearTxDmaDone(g_mcdSpiId);
            hal_SpiTxIfcChannelRelease(g_mcdSpiId);
            hal_SpiRxIfcChannelRelease(g_mcdSpiId);
            hal_HstSendEvent(0x11112224);
            return MCD_ERR_CARD_TIMEOUT;
        }

    }
    return MCD_ERR_NO;

}
VOID mcd_TimeDelaly(UINT32 timeout_tick)
{
#if 0
    UINT32 startTime = 0;
    startTime = hal_TimGetUpTime();
    while (1)
    {
        if (hal_TimGetUpTime() - startTime >  (timeout_tick))
        {
            break;
        }

    }
#else
    sxr_Sleep(timeout_tick);
#endif

}
MCD_ERR_T mcd_WaitBusy(UINT32 time_count)//MCD_WRITE_TIMEOUT
{
    UINT32 i =0;    UINT16    cmdStatus;

    MCD_ERR_T errorStatus;
    // Polling until Previous Write finishes
    while ( i < time_count )
    {
        if (hal_SpiActivateCs(g_mcdSpiId, g_mcdSpiCs)==FALSE)
        {
            return MCD_ERR_SPI_BUSY;
        }
        errorStatus = mcd_SendCommand(0xff, 0, &cmdStatus,0,NULL);
        if ( errorStatus == MCD_ERR_NO )
        {
            break;
        }
        i++;
        mcd_TimeDelaly(SLEEP_DURATION_WR_POLL/2);
    }
    if ( i == time_count )
    {
        return MCD_ERR_CARD_TIMEOUT;
    }
    return MCD_ERR_NO;


}


PUBLIC MCD_ERR_T mcd_Write_mutil(UINT32 blockStartAddr,CONST UINT8* blockWr, UINT32 blockLength)
{
    UINT16    i;
    UINT16    cmdStatus;
    MCD_ERR_T errorStatus;
    UINT32 kk = 0;

    if(g_mcdCardIsSdhc == TRUE)
    {
        MCD_ASSERT(blockStartAddr%512 == 0, "must is 512*n");
        blockStartAddr = blockStartAddr/512;
    }
    MCD_ASSERT(blockLength == 512*4, "fixed 2k");

    UINT32 now = hal_TimGetUpTime();
    //MCD_TRACE(TSTDOUT, 0, "************ mcd_Write_mutil ***blockStartAddr=0x%d",blockStartAddr);
    if (hal_SpiActivateCs(g_mcdSpiId, g_mcdSpiCs)==FALSE)
    {
        return MCD_ERR_SPI_BUSY;
    }
    errorStatus = mcd_SendCommand(16, 512, &cmdStatus,0,NULL);
    if(MCD_ERR_NO != errorStatus)
    {
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        return errorStatus;
    }
    hal_SpiFlushFifos(g_mcdSpiId);
    hal_SpiClearRxDmaDone(g_mcdSpiId);
    hal_SpiClearTxDmaDone(g_mcdSpiId);
    // Write a single block (CMD25: 0x59)
    g_SpiMcdWrCmdBuf[0] = 0xff;
    g_SpiMcdWrCmdBuf[1] = 0x59;
    g_SpiMcdWrCmdBuf[2] = (blockStartAddr >> 24) & 0xff;
    g_SpiMcdWrCmdBuf[3] = (blockStartAddr >> 16) & 0xff;
    g_SpiMcdWrCmdBuf[4] = (blockStartAddr >> 8) & 0xff;
    g_SpiMcdWrCmdBuf[5] = blockStartAddr & 0xff;
    g_SpiMcdWrCmdBuf[6] = 0x1;
    // Start byte of data token.
    g_SpiMcdWrCmdBuf[MCD_COMMAND_BUFFER_SIZE-1] = 0xfc;

    for (i = 7; i < MCD_COMMAND_BUFFER_SIZE-1; i++)
    {
        g_SpiMcdWrCmdBuf[i] = 0xff;
    }
    g_dmaBusy=hal_SpiGetData(g_mcdSpiId, g_mcdSpiCs, (UINT8*) g_SpiMcdRxBuf,MCD_COMMAND_BUFFER_SIZE);//
    if (g_dmaBusy == 0)
    {
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        return MCD_ERR_DMA_BUSY;
    }
    hal_SpiForcePin(g_mcdSpiId, HAL_SPI_PIN_CS0, HAL_SPI_PIN_STATE_0);

    g_dmaBusy=hal_SpiSendData(g_mcdSpiId, g_mcdSpiCs, (UINT8*) &g_SpiMcdWrCmdBuf[0], MCD_COMMAND_BUFFER_SIZE);
    if (g_dmaBusy == 0)
    {
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        return MCD_ERR_DMA_BUSY;
    }
    if(MCD_ERR_CARD_TIMEOUT == mcd_TimeOut(1638))
    {
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        hal_SpiForcePin(g_mcdSpiId, HAL_SPI_PIN_CS0, HAL_SPI_PIN_STATE_SPI);
        return MCD_ERR_CARD_TIMEOUT;
    }
    g_dmaBusy=hal_SpiGetData(g_mcdSpiId, g_mcdSpiCs, (UINT8*) g_SpiMcdRxBuf,MCD_MULTI_WRITE_LEN);//
    if (g_dmaBusy == 0)
    {
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        hal_SpiForcePin(g_mcdSpiId, HAL_SPI_PIN_CS0, HAL_SPI_PIN_STATE_SPI);
        return MCD_ERR_DMA_BUSY;
    }
    g_dmaBusy=hal_SpiSendData(g_mcdSpiId, g_mcdSpiCs, blockWr, MCD_MULTI_WRITE_LEN);
    if (g_dmaBusy == 0)
    {
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        hal_SpiForcePin(g_mcdSpiId, HAL_SPI_PIN_CS0, HAL_SPI_PIN_STATE_SPI);
        return MCD_ERR_DMA_BUSY;
    }
    if(MCD_ERR_CARD_TIMEOUT == mcd_TimeOut(MCD_SPI_WRITE_TIMEOUT))
    {
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        hal_SpiForcePin(g_mcdSpiId, HAL_SPI_PIN_CS0, HAL_SPI_PIN_STATE_SPI);
        return MCD_ERR_CARD_TIMEOUT;
    }
    hal_SpiForcePin(g_mcdSpiId, HAL_SPI_PIN_CS0, HAL_SPI_PIN_STATE_SPI);

    if(MCD_ERR_NO!= mcd_WaitBusy(MCD_WRITE_TIMEOUT))
    {
        return MCD_ERR_CARD_TIMEOUT;
    }
    g_SpiMcdWrCmdBuf[0] = 0xff;
    g_SpiMcdWrCmdBuf[1] = 0xff;
    g_SpiMcdWrCmdBuf[2] = 0xff;
    g_SpiMcdWrCmdBuf[3] = 0xff;
    g_SpiMcdWrCmdBuf[4] = 0xfc;

    for(kk = 1; kk<4; kk++)
    {
        hal_SpiFlushFifos(g_mcdSpiId);
        hal_SpiClearRxDmaDone(g_mcdSpiId);
        hal_SpiClearTxDmaDone(g_mcdSpiId);
        hal_SpiTxIfcChannelRelease(g_mcdSpiId);
        hal_SpiRxIfcChannelRelease(g_mcdSpiId);

        g_dmaBusy=hal_SpiGetData(g_mcdSpiId, g_mcdSpiCs, (UINT8*) g_SpiMcdRxBuf,5);//
        if (g_dmaBusy == 0)
        {
            hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
            return MCD_ERR_DMA_BUSY;
        }
        hal_SpiForcePin(g_mcdSpiId, HAL_SPI_PIN_CS0, HAL_SPI_PIN_STATE_0);
        g_dmaBusy=hal_SpiSendData(g_mcdSpiId, g_mcdSpiCs, (UINT8*) &g_SpiMcdWrCmdBuf[0], 5);
        if (g_dmaBusy == 0)
        {
            hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
            hal_SpiForcePin(g_mcdSpiId, HAL_SPI_PIN_CS0, HAL_SPI_PIN_STATE_SPI);
            return MCD_ERR_DMA_BUSY;
        }
        if(MCD_ERR_CARD_TIMEOUT == mcd_TimeOut(1638))
        {
            hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs); hal_SpiForcePin(g_mcdSpiId, HAL_SPI_PIN_CS0, HAL_SPI_PIN_STATE_SPI);
            return MCD_ERR_CARD_TIMEOUT;
        }
        g_dmaBusy=hal_SpiGetData(g_mcdSpiId, g_mcdSpiCs, (UINT8*) g_SpiMcdRxBuf,MCD_MULTI_WRITE_LEN);//
        if (g_dmaBusy == 0)
        {
            hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
            return MCD_ERR_DMA_BUSY;
        }

        g_dmaBusy=hal_SpiSendData(g_mcdSpiId, g_mcdSpiCs,(UINT8*)&blockWr[kk*512], MCD_MULTI_WRITE_LEN );
        if (g_dmaBusy == 0)
        {
            hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
            return MCD_ERR_DMA_BUSY;
        }

        if(MCD_ERR_CARD_TIMEOUT == mcd_TimeOut(MCD_SPI_WRITE_TIMEOUT))
        {
            hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
            hal_SpiForcePin(g_mcdSpiId, HAL_SPI_PIN_CS0, HAL_SPI_PIN_STATE_SPI);
            return MCD_ERR_CARD_TIMEOUT;
        }
        hal_SpiForcePin(g_mcdSpiId, HAL_SPI_PIN_CS0, HAL_SPI_PIN_STATE_SPI);

        if(MCD_ERR_NO!= mcd_WaitBusy(MCD_WRITE_TIMEOUT))
        {
            return MCD_ERR_CARD_TIMEOUT;
        }
    }
    for (i = 0; i < MCD_COMMAND_BUFFER_SIZE-1; i++)
    {
        g_SpiMcdWrCmdBuf[i] = 0xff;
    }
    g_SpiMcdWrCmdBuf[MCD_COMMAND_BUFFER_SIZE-1] = 0xfd;
    g_dmaBusy=hal_SpiGetData(g_mcdSpiId, g_mcdSpiCs, (UINT8*) g_SpiMcdRxBuf,MCD_COMMAND_BUFFER_SIZE);//
    if (g_dmaBusy == 0)
    {
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        return MCD_ERR_DMA_BUSY;
    }
    g_dmaBusy=hal_SpiSendData(g_mcdSpiId, g_mcdSpiCs, (UINT8*) &g_SpiMcdWrCmdBuf[0], MCD_COMMAND_BUFFER_SIZE);
    if (g_dmaBusy == 0)
    {
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        return MCD_ERR_DMA_BUSY;
    }
    if(MCD_ERR_CARD_TIMEOUT == mcd_TimeOut(1638))
    {
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs); hal_SpiForcePin(g_mcdSpiId, HAL_SPI_PIN_CS0, HAL_SPI_PIN_STATE_SPI);
        return MCD_ERR_CARD_TIMEOUT;
    }
    if(MCD_ERR_NO!= mcd_WaitBusy(MCD_WRITE_TIMEOUT))
    {
        return MCD_ERR_CARD_TIMEOUT;
    }
    hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
    mcd_TimeDelaly(g_delay_time_for_the_card);
    UINT32 now1 = hal_TimGetUpTime();
    MCD_TRACE(TSTDOUT, 0, "mcd_multi_Write ok %dms ",((now1-now)*1000)/16384);
    return MCD_ERR_NO;
}
PUBLIC MCD_ERR_T mcd_Write_MutilBlock(UINT32 blockStartAddr,CONST UINT8* blockWr, UINT32 blockLength)
{
    UINT16    cmdStatus;
    if(MCD_ERR_NO != mcd_Write_mutil(blockStartAddr,blockWr,blockLength))
    {
        mcd_SendCommand(12, 0, &cmdStatus,0,NULL);
        return MCD_ERR_CARD_NO_RESPONSE;

    }
    else
    {
        return MCD_ERR_NO;
    }
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

MCD_ERR_T mcd_Read_OneBlock(UINT32 startAddr, UINT8* rdDataBlock, UINT32 blockLength)
{
    HAL_SPI_INFINITE_TX_T infTx;
    MCD_ERR_T errorStatus = MCD_ERR_NO;
    UINT16    cmdStatus,Index_flag_data = 0;
    UINT32 i,startTime=0;
    //sxr_Sleep(SLEEP_DURATION_READ);
    MCD_ASSERT(blockLength == 512, "fixed 512");
    if(g_mcdCardIsSdhc == TRUE)
    {
        MCD_ASSERT(startAddr%512 == 0, "must is 512*n");
        startAddr = startAddr/512;
    }
    UINT32 now = hal_TimGetUpTime();
    // Config SPI Infinite
    infTx.sendOne     = TRUE;
    infTx.autoStop    = TRUE;
    infTx.patternMode = HAL_SPI_PATTERN_UNTIL;
    infTx.pattern     = 0xfe;
    if (hal_SpiActivateCs(g_mcdSpiId, g_mcdSpiCs)==FALSE)
    {
        return MCD_ERR_SPI_BUSY;
    }
    errorStatus = mcd_SendCommand(16, 512, &cmdStatus,0,NULL);
    if(MCD_ERR_NO != errorStatus)
    {
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        return errorStatus;
    }
    // Read a block
    hal_SpiFlushFifos(g_mcdSpiId);
    hal_SpiClearRxDmaDone(g_mcdSpiId);
    hal_SpiClearTxDmaDone(g_mcdSpiId);
    hal_SpiTxIfcChannelRelease(g_mcdSpiId);
    hal_SpiRxIfcChannelRelease(g_mcdSpiId);

    g_SpiMcdRdCmdBuf[0] = 0xff;
    g_SpiMcdRdCmdBuf[1] = 0x51;
    g_SpiMcdRdCmdBuf[2] = (startAddr >> 24) & 0xff;
    g_SpiMcdRdCmdBuf[3] = (startAddr >> 16) & 0xff;
    g_SpiMcdRdCmdBuf[4] = (startAddr >> 8) & 0xff;
    g_SpiMcdRdCmdBuf[5] = startAddr & 0xff;
    g_SpiMcdRdCmdBuf[6] = 0x1;
    for (i = 7; i < MCD_COMMAND_BUFFER_SIZE; i++)
    {
        g_SpiMcdRdCmdBuf[i] = 0xff;
    }
    hal_SpiStartInfiniteWriteMode(g_mcdSpiId, g_mcdSpiCs, &infTx);
    //  RDCMD_BUFFER_LEN
    g_dmaBusy = hal_SpiGetData(g_mcdSpiId, g_mcdSpiCs, (UINT8*)g_SpiMcdRxBuf,RD_SINGLE_BLOCK_LEN);
    if (g_dmaBusy==0)
    {
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        return MCD_ERR_DMA_BUSY;
    }
    hal_SpiForcePin(g_mcdSpiId, HAL_SPI_PIN_CS0, HAL_SPI_PIN_STATE_0);
    g_dmaBusy = hal_SpiSendData(g_mcdSpiId, g_mcdSpiCs, (UINT8*)g_SpiMcdRdCmdBuf,MCD_COMMAND_BUFFER_SIZE);
    if (g_dmaBusy == 0)
    {
        // Stop the Rx transfer.
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        hal_SpiForcePin(g_mcdSpiId, HAL_SPI_PIN_CS0, HAL_SPI_PIN_STATE_SPI);
        return MCD_ERR_DMA_BUSY;
    }


    hal_SpiForcePin(g_mcdSpiId, HAL_SPI_PIN_DO, HAL_SPI_PIN_STATE_1);
    g_dmaBusy = hal_SpiSendData(g_mcdSpiId, g_mcdSpiCs, (UINT8*)&g_SpiMcdRdCmdBuf[MCD_COMMAND_BUFFER_SIZE],RD_SINGLE_BLOCK_LEN-MCD_COMMAND_BUFFER_SIZE);
    if (g_dmaBusy == 0)
    {
        // Stop the Rx transfer.
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        hal_SpiForcePin(g_mcdSpiId, HAL_SPI_PIN_CS0, HAL_SPI_PIN_STATE_SPI);
        hal_SpiForcePin(g_mcdSpiId, HAL_SPI_PIN_DO, HAL_SPI_PIN_STATE_SPI);

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
            hal_SpiSendData(g_mcdSpiId, g_mcdSpiCs, (UINT8*)&g_SpiMcdRdCmdBuf[MCD_COMMAND_BUFFER_SIZE],RD_SINGLE_BLOCK_LEN-MCD_COMMAND_BUFFER_SIZE);
            sxr_Sleep(1638);

            hal_SpiFlushFifos(g_mcdSpiId);
            hal_SpiClearRxDmaDone(g_mcdSpiId);
            hal_SpiClearTxDmaDone(g_mcdSpiId);
            hal_SpiForcePin(g_mcdSpiId, HAL_SPI_PIN_CS0, HAL_SPI_PIN_STATE_SPI);
            hal_SpiForcePin(g_mcdSpiId, HAL_SPI_PIN_DO, HAL_SPI_PIN_STATE_SPI);
            hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
            hal_SpiTxIfcChannelRelease(g_mcdSpiId);
            hal_SpiRxIfcChannelRelease(g_mcdSpiId);
            hal_HstSendEvent(0x11112223);
            MCD_TRACE(TSTDOUT, 0, " read timeout ");
            return MCD_ERR_CARD_TIMEOUT;
        }

    }
    hal_SpiForcePin(g_mcdSpiId, HAL_SPI_PIN_CS0, HAL_SPI_PIN_STATE_SPI);
    hal_SpiForcePin(g_mcdSpiId, HAL_SPI_PIN_DO, HAL_SPI_PIN_STATE_SPI);
    hal_SpiStopInfiniteWriteMode(g_mcdSpiId, g_mcdSpiCs);
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
    //  for (i = 0; i < 512; i++)
    {
        //   rdDataBlock[i] = g_SpiMcdRxBuf[i];
    }
    memcpy((UINT8*)rdDataBlock, (UINT8*)(g_SpiMcdRxBuf), 512);
    // memcpy((UINT8*)rdDataBlock, (UINT8*)(g_SpiMcdRdCmdBuf), g_readBlockLen);
    UINT32 now1 = hal_TimGetUpTime();
    MCD_TRACE(TSTDOUT, 0, "mcd_Read ok %dms",((now1-now)*1000)/16384);
    return MCD_ERR_NO;
}

MCD_ERR_T mcd_Read(UINT32 startAddr, UINT8* rdDataBlock, UINT32 blockLength)
{
    if((mcd_Read_OneBlock(startAddr,rdDataBlock,blockLength))!=MCD_ERR_NO)
    {
        return mcd_Read_OneBlock(startAddr,rdDataBlock,blockLength);
    }
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
PUBLIC MCD_ERR_T mcd_Close(MCD_CARD_ID mcdId)
{
    if(gMcd_opened)
    {
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        hal_SpiClose(g_mcdSpiId, g_mcdSpiCs);
        gMcd_opened=FALSE;
    }
    g_setBlockSize = FALSE;
    hal_SpiTxIfcChannelRelease(g_mcdSpiId);
    hal_SpiRxIfcChannelRelease(g_mcdSpiId);

    return MCD_ERR_NO;
}

PUBLIC MCD_ERR_T mcd_SwitchOperationTo(UINT8 card_number)
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

PUBLIC MCD_STATUS_T mcd_CardStatus(MCD_CARD_ID mcdId)
{
    return MCD_STATUS_NOTPRESENT;
}


VOID test_open()
{
    MCD_CSD_T mcdCsd;
    mcd_Open(&mcdCsd);

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
    mcd_Write_mutil(blockStartAddr,  blockWr, blockLength);
}





#else
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
typedef enum
{
    SDMMC_NORMAL_VER_1,
    SDMMC_STANDARD_CAPACITY_VER_2,
    SDMMC_HIGH_CAPACITY_VER_2,
    SDMMC_NORMAL_VER_QTY
} SDMMC_VER_T;
BOOL IsMMC=FALSE;

// =============================================================================
//  MACROS
// =============================================================================

// Timeouts
#define HAL_TICK1S 16384

#define SECOND        * HAL_TICK1S

#define MCD_SPI_CMD_TIMEOUT        ( 1 SECOND / 10 )
#define MCD_SPI_RESP_TIMEOUT       ( 1 SECOND / 10 )
#define MCD_SPI_TRAN_TIMEOUT       ( 1 SECOND / 10 )
#define MCD_SPI_READ_TIMEOUT       ( 1 SECOND )
#define MCD_SPI_WRITE_TIMEOUT      ( 1 SECOND )





#define MAX_BLOCK_SIZE      2048
//#define RDCMD_BUFFER_LEN    16380   //  1024 // in jade 16380
#define RDCMD_BUFFER_LEN    1638   // may be 800
#define WRCMD_BUFFER_LEN    (MAX_BLOCK_SIZE + 64)
#define SLEEP_DURATION_OP   20000
#define SLEEP_DURATION_RD   20
#define SLEEP_DURATION_READ 5000
#define SLEEP_DURATION_WR   5000
#define SLEEP_DURATION_STD  5
#define SLEEP_DURATION      0
#define SLEEP_DURATION_WR_POLL   16384/1000  // (1 millisecond)
#define SLEEP_DURATION_CMD  16384/100        // (10 millisecond)
#define MCD_WRITE_TIMEOUT   2000             // max timeout for a write is
//  5s = 5000 *SLEEP_DURATION_WR_POLL

// The number of retry does not depend on the clock divider.
#define CMD1_RETRY_NB       128   // the card is supposed to answer within 1s
//  - Max wait is 128 * 10ms=1,28s
#define CMD0_RETRY_NB       5     // Retry 5 times max
#define ACMD41_RETRY_NB     128   // the card is supposed to answer within 1s
//  - Max wait is 128 * 10ms=1,28s

#define CMDFF_RETRY_NB                           10
#define MCD_COMMAND_BUFFER_SIZE        16

#define MCD_DEV_NUMBER                           2
#define MCD_DEFUALT_BLOCK_SIZE            512

BOOL g_mcdCardIsSdhc = FALSE;
BOOL g_ver2_high_capcity_card = FALSE;
BOOL g_ver2_Standard_card = FALSE;
BOOL g_ver1_card = FALSE;
SDMMC_VER_T g_card_ver = SDMMC_NORMAL_VER_QTY;
// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================
PRIVATE VOLATILE UINT8       g_SpiMcdRxBuf[RDCMD_BUFFER_LEN]
__attribute__((section(".ucdata")));
PRIVATE VOLATILE UINT8       g_SpiMcdRdCmdBuf[RDCMD_BUFFER_LEN]
__attribute__((section(".ucdata")));
PRIVATE VOLATILE UINT8       g_SpiMcdWrCmdBuf[WRCMD_BUFFER_LEN]
__attribute__((section(".ucdata")));

PRIVATE HAL_SPI_CS_T         g_mcdSpiCs      = HAL_SPI_CS0;
PRIVATE HAL_SPI_ID_T         g_mcdSpiId      = HAL_SPI;
PRIVATE HAL_SPI_PIN_T        g_mcdSpiPinId   = HAL_SPI_PIN_CS0;
PRIVATE UINT16               g_dmaBusy       = 0;
PRIVATE UINT32               g_transferSize  = 0;
PRIVATE UINT16               g_readBlockLen[MCD_DEV_NUMBER]  = {0,0};
PRIVATE UINT16               g_writeBlockLen[MCD_DEV_NUMBER] = {0,0};
PRIVATE UINT16               g_blockLen[MCD_DEV_NUMBER]  = {0,0};
UINT32                       g_blockNumber[MCD_DEV_NUMBER]  = {0,0};
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

UINT8 SD_GetCmdByte6(UINT8 cmd, UINT8 *param)
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
    MCD_TRACE(TSTDOUT, 0, "mcd_SendCommand %d", cmdId);
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
    sxr_Sleep(SLEEP_DURATION_STD);
    if (g_dmaBusy == 0)
    {
        // Stop the Rx transfer.
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        // Indicate that SPI is free
        // g_mcdSpiFree = TRUE;
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
            return MCD_ERR_DMA_BUSY;
        }

        g_dmaBusy=hal_SpiSendData(g_mcdSpiId, g_mcdSpiCs, (UINT8*) g_SpiMcdRdCmdBuf, g_transferSize);

        if (g_dmaBusy == 0)
        {
            // Stop the Rx transfer.
            hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
            // Indicate that SPI is free
            // g_mcdSpiFree = TRUE;
            return MCD_ERR_DMA_BUSY;
        }
        sxr_Sleep(SLEEP_DURATION_STD);

        startTime = hal_TimGetUpTime();
        // Wait until the end of both transfers.
        while (!hal_SpiRxDmaDone(g_mcdSpiId,g_mcdSpiCs))
        {
            if (hal_TimGetUpTime() - startTime >  (MCD_SPI_CMD_TIMEOUT*1))
            {
                hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
                hal_SpiFlushFifos(g_mcdSpiId);
                hal_SpiClearRxDmaDone(g_mcdSpiId);
                hal_SpiClearTxDmaDone(g_mcdSpiId);
                hal_SpiTxIfcChannelRelease(g_mcdSpiId);
                hal_SpiRxIfcChannelRelease(g_mcdSpiId);
                hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
                hal_HstSendEvent(0x11112226);
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
    MCD_TRACE(TSTDOUT, 0, "mcd_SendCommand  status 0x%x cmdId   %d", *status,cmdId);
    MCD_TRACE(TSTDOUT, 0, "mcd_SendCommand  status 0x%x ,0x%x  ,0x%x ,0x%x 0x%x ", g_SpiMcdRxBuf[i],g_SpiMcdRxBuf[i+1],g_SpiMcdRxBuf[i+2],g_SpiMcdRxBuf[i+3],g_SpiMcdRxBuf[i+4]);
    MCD_TRACE(TSTDOUT, 0, "mcd_SendCommand  status 0x%x ,0x%x  ,0x%x ,0x%x 0x%x  ", g_SpiMcdRxBuf[i],g_SpiMcdRxBuf[i-1],g_SpiMcdRxBuf[i-2],g_SpiMcdRxBuf[i-3],g_SpiMcdRxBuf[i+4]);
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

    MCD_TRACE(TSTDOUT, 0, " here 2  cmdId %d ",cmdId);


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
    }
    else if(g_card_ver == SDMMC_STANDARD_CAPACITY_VER_2)
    {
        g_blockNumber[0] = mcdCsd->blockNumber;
        g_blockLen[0] = 512;
        MCD_TRACE(TSTDOUT, 0, "v2block number = %d.",mcdCsd->blockNumber);
        MCD_TRACE(TSTDOUT, 0, "v2 g_blockLen =%d",g_blockLen);

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


PUBLIC MCD_ERR_T mcd_GetCardSize(MCD_CARD_SIZE_T* size)
{
    size->blockLen = g_blockLen[0];
    size->nbBlock  = g_blockNumber[0];

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
static bool gMcd_opened=FALSE;

PUBLIC MCD_ERR_T mcd_Open(MCD_CARD_ID mcdId,MCD_CSD_T* mcdCsd, MCD_CARD_VER mcdVer )
{
    MCD_ERR_T errorStatus = MCD_ERR_NO;
    UINT16 cmdStatus;
    UINT8 csdRdBlLen,csdWrBlLen = 0;
    UINT16 i, j;
    bool End_Init;
    MCD_TRACE(TSTDOUT, 0, "************ MCD_OPEN ************");

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
    MCD_TRACE( TSTDOUT, 0, "Opening MMC/SD");
    hal_SpiOpen(g_mcdSpiId, g_mcdSpiCs, &g_spiConfig);
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
    if (hal_SpiActivateCs(g_mcdSpiId, g_mcdSpiCs)==FALSE)
    {
        hal_SpiClose(g_mcdSpiId, g_mcdSpiCs);
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
    if(errorStatus != MCD_ERR_NO)
    {
        return errorStatus;
    }
    //--------------------------------------//
    //          Init Process  CMD0          //
    //--------------------------------------//
    End_Init = FALSE;
    MCD_TRACE(TSTDOUT, 0, "cmdStatus before sending CMD0: %x ", cmdStatus);
    for(j = 0; j < CMD0_RETRY_NB; j++)
    {
        errorStatus = mcd_SendCommand(0,0,&cmdStatus,0,NULL);
        if (errorStatus == MCD_ERR_NO)
        {
            // CMD successfully sent
            MCD_TRACE(TSTDOUT, 0, "cmdStatus: %x ", cmdStatus);
            MCD_TRACE(TSTDOUT, 0, "cmdStatus & 0xff: %x ", (cmdStatus & 0xff));
            sxr_Sleep(SLEEP_DURATION_CMD);
            errorStatus = mcd_SendCommand(8,0x1aa,&cmdStatus,0,NULL);
            if ((cmdStatus & 0xFB) == 0x1)
            {
                if(cmdStatus & 0xFF == 0x1)
                {
                    MCD_TRACE(TSTDOUT, 0, "This card doesn't comply to the spec version 2.0");
                }
                else
                {
                    MCD_TRACE(TSTDOUT, 0, "This card   comply to the spec version 2.0");
                }
                // If in Idle state, continue procedure
                MCD_TRACE(TSTDOUT, 0, "MMC/SD in IDLE State. CMD0 sent %d times", j+1);
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
    MCD_TRACE(TSTDOUT,0,"End_Init: %i", End_Init);
    // We did not manage to set the card in Idle state - Exit
    if (End_Init == FALSE)
    {
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        // Indicate that SPI is free
        // g_mcdSpiFree = TRUE;
        hal_SpiClose(g_mcdSpiId, g_mcdSpiCs);


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
                errorStatus = mcd_SendCommand(41,0x40000000,&cmdStatus,0,NULL);
                if (errorStatus == MCD_ERR_NO)
                {
                    // CMD successfully sent
                    if ((cmdStatus & 0xff) == 0x0)
                    {
                        MCD_TRACE(TSTDOUT, 0, "Send ACMD41 OK & cmdStatus&ff=0. cmdStatus: 0x %x", cmdStatus);
                        MCD_TRACE(TSTDOUT, 0, "SD in READY State. ACMD41 sent %d times", j+1);
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
            return errorStatus;
        }
        if (End_Init == TRUE) break;

        // Sleep a bit to leave some time to the card to init itself.
        sxr_Sleep(SLEEP_DURATION_CMD);
    }

    if (End_Init == FALSE)
    {
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        // Indicate that SPI is free
        // g_mcdSpiFree = TRUE;
        hal_SpiClose(g_mcdSpiId, g_mcdSpiCs);
        MCD_TRACE(TSTDOUT, 0, " here");
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
                    MCD_TRACE(TSTDOUT, 0, "MMC in READY State. CMD1 sent %d times", j+1);
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
                return errorStatus;
            }
            // Sleep a bit to leave some time to the card to init itself.
            sxr_Sleep(SLEEP_DURATION_CMD);
        }

        if (End_Init == FALSE)
        {
            hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
            // Indicate that SPI is free
            // g_mcdSpiFree = TRUE;
//hal_HstSendEvent(0x11110003);
            hal_SpiClose(g_mcdSpiId, g_mcdSpiCs);
            return MCD_ERR_CARD_TIMEOUT;
        }
    }

    errorStatus = mcd_ReadCsd(mcdCsd);
    if (errorStatus != MCD_ERR_NO)
    {
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        // Indicate that SPI is free
        // g_mcdSpiFree = TRUE;
        hal_SpiClose(g_mcdSpiId, g_mcdSpiCs);
        return errorStatus;
    }

    // Set block lentgh
    csdWrBlLen = mcdCsd->writeBlLen;
    csdRdBlLen = mcdCsd->readBlLen;

    if (csdWrBlLen == csdRdBlLen)
    {
        g_blockLen[g_mcdSpiCs] = csdWrBlLen;
    }
    else if (csdWrBlLen < csdRdBlLen && mcdCsd->readBlPartial)
    {
        // If Write block length < Read block length and read partial block allowed
        g_blockLen[g_mcdSpiCs] = csdWrBlLen;
    }
    else if (csdWrBlLen > csdRdBlLen && mcdCsd->writeBlPartial)
    {
        // If Write block length > Read block length and read partial block allowed
        g_blockLen[g_mcdSpiCs] = csdRdBlLen;
    }
    else
    {
        hal_SpiClose(g_mcdSpiId, g_mcdSpiCs);
        return MCD_ERR_BLOCK_LEN;
    }

    g_writeBlockLen[g_mcdSpiCs] = mcd_ConvertBlockLength(g_blockLen[g_mcdSpiCs]);
    g_readBlockLen[g_mcdSpiCs] = g_writeBlockLen[g_mcdSpiCs];
    MCD_TRACE(TSTDOUT, 0, "Wr bl len: %d, Rd bl len: %d", mcdCsd->writeBlLen, mcdCsd->readBlLen);

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
                hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
                // Indicate that SPI is free
                // g_mcdSpiFree = TRUE;
                return MCD_ERR_CSD;
        }
        rate = rate_val*rate_unit;
        MCD_TRACE(TSTDOUT, 0, "MMC/SD Speed Rating: %d kHz", rate);
        clkdivider = (78000/rate)/2;
        // MCD_TRACE(TSTDOUT, 0, " Clk divider: %d", clkdivider);
        // SD max frequency: 1.1Mhz: 1100000
        //g_spiConfig.spiFreq = 1100000;  // can be 20000000 20mhz
        g_spiConfig.spiFreq = 20000000;
        // MMC and Tflash work at minimal frequency of 1.5Mhz-TFlash=>4Mhz
        if (IsMMC == TRUE)
        {
            // g_spiConfig.spiFreq = 2000000;
        }
        //MCD_TRACE(TSTDOUT, 0, "MMC/SD Used Speed @52MHz: %d kHz", 52000/(g_spiConfig.spiFreq+1)/2);
        //MCD_TRACE(TSTDOUT, 0,"MMC/SD Used Speed @78MHz: %d kHz",78000/(g_spiConfig.spiFreq+1)/2);
        MCD_TRACE(TSTDOUT, 0,"MMC/SD Sector size : %d bytes",1<<mcdCsd->readBlLen);
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
            MCD_PROFILE_FUNCTION_EXIT(mcd_Open);
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
        MCD_TRACE(TSTDOUT, 0,"MMC/SD TAAC time : %d us",acc_time/10000);
        MCD_TRACE(TSTDOUT, 0,"MMC/SD NSAC time : %d clocks",mcdCsd->nsac*100);
        MCD_TRACE(TSTDOUT, 0,"MMC/SD Read Access time : %d bytes",nb_clk>>3);
        MCD_TRACE(TSTDOUT, 0,"MMC/SD Write Access time : %d bytes",(nb_clk>>3)*(1<<mcdCsd->r2wFactor));
    }
#endif
    gMcd_opened=TRUE;       //
    hal_HstSendEvent(0x22220000);
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
    UINT32 startTime = 0;

    MCD_ASSERT(blockLength == 512, "fixed 512");
    if(g_mcdCardIsSdhc == TRUE)
    {
        MCD_ASSERT(blockStartAddr%512 == 0, "must is 512*n");

        blockStartAddr = blockStartAddr/512;
    }
    UINT32 now = hal_TimGetUpTime();

    tmpRdBuff = sxr_Malloc(g_writeBlockLen[g_mcdSpiCs] + 64);

    MCD_TRACE(TSTDOUT, 0, "************ MCD_WRITE ************");


    if (hal_SpiActivateCs(g_mcdSpiId, g_mcdSpiCs)==FALSE)
    {
        sxr_Free(tmpRdBuff);
        return MCD_ERR_SPI_BUSY;
    }

    MCD_TRACE(TSTDOUT, 0, "blockWr mcd_write: 0x %x ,blockStartAddr 0x%x", blockWr,blockStartAddr);

    //--------------------------------------//
    //       Set block Length       //
    //--------------------------------------//
    // Write CMD16
    MCD_TRACE(TSTDOUT, 0, "Block len g_writeBlockLen: %d", g_writeBlockLen[g_mcdSpiCs]);

    // g_writeBlockLen length of the block, in bytes (The max block length
    // is defined in CSD register field WRITE_BL_LEN)
    if(g_mcdCardIsSdhc == FALSE)
    {
        errorStatus = mcd_SendCommand(16, 512, &cmdStatus,0,NULL);


        if (errorStatus != MCD_ERR_NO)
        {
            MCD_TRACE(TSTDOUT, 0, "Write deactivate0");
            hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
            // Indicate that SPI is free
            // g_mcdSpiFree = TRUE;
            sxr_Free(tmpRdBuff);
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

    MCD_TRACE(TSTDOUT, 0, "SD destination block startAddr : 0x %x", blockStartAddr);
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
    g_dmaBusy=hal_SpiGetData(g_mcdSpiId, g_mcdSpiCs, (UINT8*) tmpRdBuff, g_writeBlockLen[g_mcdSpiCs] + 64);
    if (g_dmaBusy == 0)
    {
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
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
    g_dmaBusy=hal_SpiSendData(g_mcdSpiId, g_mcdSpiCs, (UINT8*) &commandBuffer[0], MCD_COMMAND_BUFFER_SIZE);
    if (g_dmaBusy == 0)
    {
        // Stop the Rx transfer.
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        hal_HstSendEvent(0x11112225);

        // Indicate that SPI is free
        // g_mcdSpiFree = TRUE;
        //hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        sxr_Free(tmpRdBuff);
        return MCD_ERR_DMA_BUSY;
    }
    sxr_Sleep(SLEEP_DURATION_WR_POLL);
    // Command and data are sent during the same low value of the CS.
    // Any time could be waited here (from us to minutes)

    // TODO: try to use the size passed as parameter, or check/assert it, if the CSD
    // allow that (rd, wr partial thing)
    g_dmaBusy=hal_SpiSendData(g_mcdSpiId, g_mcdSpiCs, blockWr, g_writeBlockLen[g_mcdSpiCs] + 64 - 16);
    if (g_dmaBusy == 0)
    {
        // Stop the Rx transfer.
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        // Indicate that SPI is free
        // g_mcdSpiFree = TRUE;
        //hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        sxr_Free(tmpRdBuff);
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
            hal_HstSendEvent(0x11112224);
            return MCD_ERR_CARD_TIMEOUT;

        }

    }
    i=0;
    // Polling until Previous Write finishes
    while ( i < MCD_WRITE_TIMEOUT )
    {
        // Check busy signal
        // FIXME Get rid of that if {} ?
        if (hal_SpiActivateCs(g_mcdSpiId, g_mcdSpiCs)==FALSE)
        {
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
    hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
    // Indicate that SPI is free
    // g_mcdSpiFree = TRUE;

    if ( i == MCD_WRITE_TIMEOUT )
    {
        sxr_Free(tmpRdBuff);
        return MCD_ERR_CARD_TIMEOUT;
    }
    sxr_Free(tmpRdBuff);
    UINT32 now1 = hal_TimGetUpTime();
    MCD_TRACE(TSTDOUT, 0, " spi mcd_Write ok 0x%x   %dms ",cmdStatus,((now1-now)*1000)/16384);
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
    //UINT8 commandBuffer[MCD_COMMAND_BUFFER_SIZE];
    HAL_SPI_INFINITE_TX_T infTx;
    MCD_ERR_T errorStatus = MCD_ERR_NO;
    UINT16    cmdStatus,Index_flag_data = 0;
    UINT32 i;
    UINT32 startTime=0;

    MCD_ASSERT(blockLength == 512, "fixed 512");
    MCD_TRACE(TSTDOUT, 0, "************ MCD_READ ************");
    if(g_mcdCardIsSdhc == TRUE)
    {
        MCD_ASSERT(startAddr%512 == 0, "must is 512*n");

        startAddr = startAddr/512;
    }
    UINT32 now = hal_TimGetUpTime();

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

    MCD_TRACE(TSTDOUT, 0, "Rx buffer Rd_Data_Block->data: 0x%x", rdDataBlock);
    MCD_TRACE(TSTDOUT, 0, "Rx buffer g_SpiMcdRxBuf: 0x%x", g_SpiMcdRxBuf);

    g_dmaBusy = hal_SpiSendData(g_mcdSpiId, g_mcdSpiCs, (UINT8*)g_SpiMcdRdCmdBuf, RDCMD_BUFFER_LEN);

    if (g_dmaBusy == 0)
    {
        // Stop the Rx transfer.
        hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
        hal_HstSendEvent(0x11112222);
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
            hal_SpiFlushFifos(g_mcdSpiId);
            hal_SpiClearRxDmaDone(g_mcdSpiId);
            hal_SpiClearTxDmaDone(g_mcdSpiId);
            hal_SpiDeActivateCs(g_mcdSpiId, g_mcdSpiCs);
            hal_SpiTxIfcChannelRelease(g_mcdSpiId);
            hal_SpiRxIfcChannelRelease(g_mcdSpiId);
            hal_HstSendEvent(0x11112223);
            return MCD_ERR_CARD_TIMEOUT;
        }

    }
    sxr_Sleep(SLEEP_DURATION_RD);
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
    MCD_TRACE(TSTDOUT, 0, "Struct Rd_Data_Block: 0x%x", rdDataBlock);
    UINT32 now1 = hal_TimGetUpTime();
    MCD_TRACE(TSTDOUT, 0, " spi mcd_Read ok  now1 %d  ,%dms", now1,((now1-now)*1000)/16384);
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
PUBLIC MCD_ERR_T mcd_Close(MCD_CARD_ID mcdId)
{
    if(gMcd_opened)
    {
        hal_SpiClose(g_mcdSpiId, g_mcdSpiCs);
        gMcd_opened=FALSE;
    }

    hal_SpiTxIfcChannelRelease(g_mcdSpiId);
    hal_SpiRxIfcChannelRelease(g_mcdSpiId);

    return MCD_ERR_NO;
}


PUBLIC MCD_ERR_T mcd_SwitchOperationTo(UINT8 card_number)
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

PUBLIC MCD_STATUS_T mcd_CardStatus(MCD_CARD_ID mcdId)
{
    return MCD_STATUS_NOTPRESENT;
}


VOID test_open()
{
    MCD_CSD_T mcdCsd;
    mcd_Open(&mcdCsd);

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
    MCD_CSD_T mcdCsd;
    mcd_Open(&mcdCsd);

}
#endif





