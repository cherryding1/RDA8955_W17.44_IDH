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
#include "hal_rfspi.h"
#include "hal_tcu.h"
#include "hal_sys.h"
#include "hal_clk.h"
#include "hal_timers.h"
#include "hal_host.h"
#include "drv_xcv.h"
#include "drv_xcv_calib.h"
#include "sxs_io.h"
#include "sxr_tls.h"
#include "sxs_idle.h"
#include "rfd_cfg.h"
#include "rfd_xcv.h"
#include "rfd_defs.h"
#include "gsm.h"
#include "baseband_defs.h"
#include "calib_m.h"


// As the SPI signal from XCV to baseband is too weak, buffer is added.
// However, the buffer chip on the new RF board has a significant activation time,
// and the buffered signal is postphoned for 1 SPI clock cycle.
#define RFSPI_RX_WORKAROUND 1

#define XCV_AFC_COMPATIBLE_WITH_OLD_CALIB_DATA 1


#define XCV_DIGRF_RX_CLK_POL                      RFD_DIGRF_NORM_CLK_POL
#define XCV_SAMPLE_WIDTH                                             12
#define XCV_ADC2DBM_GAIN                                             55

#define XCV_SET_PDN                                   SET_TCO(XCV->PDN)
#define XCV_CLR_PDN                                   CLR_TCO(XCV->PDN)

#define CDAC_REG_VALUE_BASE (0x1dd00)

// Forward declaration of internal functions
VOID xcvBuildSpiCmd (HAL_RFSPI_CMD_T *cmd, UINT8 address, UINT32 data);
VOID xcvSetArfcn (RFD_RFDIR_T dir, GSM_RFBAND_T band, UINT16 arfcn);
VOID xcvReadReg (UINT16 address, UINT8 *output);

INLINE VOID xcvConfigure(VOID);
CONST XCV_CONFIG_T* RFD_DATA_INTERNAL XCV = NULL;


#if defined(_FLASH_PROGRAMMER) || defined(_T_UPGRADE_PROGRAMMER)
#undef SPI_REG_DEBUG
#endif

#ifdef SPI_REG_DEBUG
// ======================================================
// Debug: Read and Write Registers via SPI

//#define XCV_REG_BUFFER_READ_WRITE

#define XCV_REG_VALUE_LOCATION __attribute__((section(".xcv_reg_value")))

#define SPI_REG_WRITE_FLAG (1<<31)
#define SPI_REG_READ_FLAG (1<<30)

#ifdef XCV_REG_BUFFER_READ_WRITE
#define SPI_REG_FORCE_WRITE_FLAG (1<<29)
#define SPI_REG_FORCE_READ_FLAG (1<<28)
#else // XCV_REG_BUFFER_READ_WRITE
#define SPI_REG_FORCE_WRITE_FLAG SPI_REG_WRITE_FLAG
#define SPI_REG_FORCE_READ_FLAG SPI_REG_READ_FLAG
#endif // ! XCV_REG_BUFFER_READ_WRITE

#define XCV_REG_DATA_MASK (RDA6220_DATA_MASK)

#define XCV_REG_PAGE_NUM (3)
#define XCV_REG_NUM_PER_PAGE (0x40)
UINT32 XCV_REG_VALUE_LOCATION g_rfdXcvRegValue[XCV_REG_PAGE_NUM][XCV_REG_NUM_PER_PAGE];

UINT32 g_rfdXcvRegForceWriteError = 0;
UINT32 g_rfdXcvRegForceReadError = 0;

extern PUBLIC BOOL hal_RfspiImmReadWriteAvail();
extern PUBLIC VOID hal_RfspiImmWrite(CONST UINT8 *Cmd, UINT32 CmdSize);
VOID xcvReadReg (UINT16 address, UINT8 *output);

#ifdef XCV_REG_BUFFER_READ_WRITE
UINT32 g_rfdXcvCurPage = 0;
UINT32 g_rfdXcvRegLastWriteValue[XCV_REG_PAGE_NUM][XCV_REG_NUM_PER_PAGE];
#define XCV_WRITE_LIST_LEN (0xa)
UINT32 RFD_DATA_INTERNAL g_rfdXcvRegWriteList[XCV_WRITE_LIST_LEN];

PROTECTED BOOL rfd_XcvRegWriteListInsert(UINT32 page, UINT32 addr, UINT32 data)
{
    int i;
    for (i=0; i<XCV_WRITE_LIST_LEN; i++)
    {
        if (g_rfdXcvRegWriteList[i] == 0) break;
    }

    if (i == XCV_WRITE_LIST_LEN) return FALSE;

    g_rfdXcvRegWriteList[i] = SPI_REG_WRITE_FLAG |
                              (addr&RDA6220_ADDR_MASK)<<24 |
                              (page&0x3)<<20 |
                              (data&RDA6220_DATA_MASK);
    return TRUE;
}
#endif // XCV_REG_BUFFER_READ_WRITE

PROTECTED VOID rfd_XcvRegForceWriteSingle(UINT32 addr, UINT32 data)
{
    UINT8 array[RDA6220_ONE_CMD_BYTE_QTY];
    UINT32 cmdWord = RDA6220_WRITE_FMT(addr, data);

    // Hardcoded
    array[0] = (cmdWord >> (3 * BYTE_SIZE)) & BYTE_MASK;
    array[1] = (cmdWord >> (2 * BYTE_SIZE)) & BYTE_MASK;
    array[2] = (cmdWord >> (BYTE_SIZE)) & BYTE_MASK;
    array[3] =  cmdWord & BYTE_MASK;

    hal_RfspiImmWrite(array, RDA6220_ONE_CMD_BYTE_QTY);
}

PROTECTED BOOL rfd_XcvRegForceWrite(UINT32 page, UINT32 addr, UINT32 data)
{
    BOOL result = FALSE;
    page &= 0x3;
    UINT32 status = hal_SysEnterCriticalSection();
    if (hal_RfspiImmReadWriteAvail())
    {
        if (page != 0) // switch to the page
        {
            rfd_XcvRegForceWriteSingle(0x3f, page);
        }
        rfd_XcvRegForceWriteSingle(addr, data);
        if (page != 0) // back to page 0
        {
            rfd_XcvRegForceWriteSingle(0x3f, 0x00000);
        }
        result = TRUE;
    }
    hal_SysExitCriticalSection(status);
    return result;
}

PROTECTED BOOL rfd_XcvRegForceRead(UINT32 page, UINT32 addr, UINT32 *pData)
{
    BOOL result = FALSE;
    page &= 0x3;
    UINT32 status = hal_SysEnterCriticalSection();
    if (hal_RfspiImmReadWriteAvail())
    {
        if (page != 0) // switch to the page
        {
            rfd_XcvRegForceWriteSingle(0x3f, page);
        }
        xcvReadReg(addr, (UINT8 *)pData);
        if (page != 0) // back to page 0
        {
            rfd_XcvRegForceWriteSingle(0x3f, 0x00000);
        }
        result = TRUE;
    }
    hal_SysExitCriticalSection(status);
    return result;
}

PROTECTED VOID rfd_XcvRegValueInit(VOID)
{
    for (int page=0; page<XCV_REG_PAGE_NUM; page++)
    {
        for (int i=0; i<XCV_REG_NUM_PER_PAGE; i++)
        {
            g_rfdXcvRegValue[page][i] = 0;
        }
    }
}

PROTECTED VOID rfd_XcvRegCheckSinglePage(UINT32 page)
{
    if (page >= XCV_REG_PAGE_NUM)
    {
        return;
    }

    for (int i=0; i<XCV_REG_NUM_PER_PAGE; i++)
    {
#ifdef XCV_REG_BUFFER_READ_WRITE
        if (g_rfdXcvRegValue[page][i] & SPI_REG_WRITE_FLAG)
        {
            if (!rfd_XcvRegWriteListInsert(page, i, g_rfdXcvRegValue[page][i]&XCV_REG_DATA_MASK))
            {
                continue;
            }
            g_rfdXcvRegValue[page][i] &= ~SPI_REG_WRITE_FLAG;
        }
        if (g_rfdXcvRegValue[page][i] & SPI_REG_READ_FLAG)
        {
            g_rfdXcvRegValue[page][i] &= ~(SPI_REG_READ_FLAG|XCV_REG_DATA_MASK);
            g_rfdXcvRegValue[page][i] |= g_rfdXcvRegLastWriteValue[page][i]&XCV_REG_DATA_MASK;
        }
#endif //XCV_REG_BUFFER_READ_WRITE

        if (g_rfdXcvRegValue[page][i] & SPI_REG_FORCE_WRITE_FLAG)
        {
            if (!rfd_XcvRegForceWrite(page, i, g_rfdXcvRegValue[page][i]&XCV_REG_DATA_MASK))
            {
                g_rfdXcvRegForceWriteError++;
                continue;
            }
#ifdef XCV_REG_BUFFER_READ_WRITE
            g_rfdXcvRegLastWriteValue[page][i] = g_rfdXcvRegValue[page][i]&XCV_REG_DATA_MASK;
#endif // XCV_REG_BUFFER_READ_WRITE
            g_rfdXcvRegValue[page][i] &= ~SPI_REG_FORCE_WRITE_FLAG;
        }
        if (g_rfdXcvRegValue[page][i] & SPI_REG_FORCE_READ_FLAG)
        {
            UINT32 data;
            if (!rfd_XcvRegForceRead(page, i, &data))
            {
                g_rfdXcvRegForceReadError++;
                continue;
            }
#ifdef XCV_REG_BUFFER_READ_WRITE
            g_rfdXcvRegLastWriteValue[page][i] = data&XCV_REG_DATA_MASK;
#endif // XCV_REG_BUFFER_READ_WRITE
            g_rfdXcvRegValue[page][i] &= ~(SPI_REG_FORCE_READ_FLAG|XCV_REG_DATA_MASK);
            g_rfdXcvRegValue[page][i] |= data&XCV_REG_DATA_MASK;
        }
    }
}

PUBLIC VOID rfd_XcvRegCheck(VOID)
{
    static BOOL initDone = FALSE;
    if (!initDone)
    {
        rfd_XcvRegValueInit();
        initDone = TRUE;
    }

#if 0
    for (UINT32 page=0; page<XCV_REG_PAGE_NUM; page++)
    {
        rfd_XcvRegCheckSinglePage(page);
    }
#else
    static UINT32 count = 0;
    UINT32 residual = count++ % 4;
    if (residual < 2)
    {
        // page 0 takes the precedence
        rfd_XcvRegCheckSinglePage(0);
    }
    else if (residual == 2)
    {
        rfd_XcvRegCheckSinglePage(1);
    }
    else // residual == 3
    {
        rfd_XcvRegCheckSinglePage(2);
    }
#endif
}

PUBLIC VOID rfd_RegisterXcvRegCheckFunc(VOID)
{
    static BOOL registerIdleHook = FALSE;
    if (!registerIdleHook)
    {
        registerIdleHook = TRUE;
        sxs_RegisterDebugIdleHookFunc(&rfd_XcvRegCheck);
    }
}
#endif // SPI_REG_DEBUG


//--------------------------------------------------------------------//
//                                                                    //
//                        Internal Structures                         //
//                                                                    //
//--------------------------------------------------------------------//

// GLOBAL variables
UINT32 RFD_DATA_INTERNAL g_xcvChipId = 0;
UINT16 g_xcvbattstablevolt = 0;
INT32 RFD_DATA_INTERNAL g_CDAC;
INT32 RFD_DATA_INTERNAL g_AfcOffset = 0;
UINT16 RFD_DATA_INTERNAL g_afcDacVal = RDA6220_AFC_MID;
INT16 RFD_DATA_INTERNAL g_CdacDelta = 0;
RFD_LP_MODE_T RFD_DATA_INTERNAL g_lastSleepMode = RFD_POWER_OFF;

PROTECTED CALIB_CALIBRATION_T* RFD_DATA_INTERNAL g_xcvCalibPtr     = 0 ;

CONST XCV_RX_GAIN_T AgcTab_EGSM[AGC_QTY]       = EGSM_AGC_DEFAULT;
CONST XCV_RX_GAIN_T AgcTab_DCS[AGC_QTY]        = DCS_AGC_DEFAULT;

#ifdef DIGRF_APC_TABLE
CONST XCV_APC_T ApcTab_EGSM_Default[APCLB_QTY]  = EGSM_APC_DEFAULT;
CONST XCV_APC_T ApcTab_DCS_Default[APCHB_QTY]   = DCS_APC_DEFAULT;
XCV_APC_T RFD_DATA_INTERNAL ApcTab_EGSM[APCLB_QTY];
XCV_APC_T RFD_DATA_INTERNAL ApcTab_DCS[APCHB_QTY];
#endif // DIGRF_APC_TABLE

// RF Commands Context
XCV_COMMANDS_T RFD_DATA_INTERNAL g_xcvCmdCtx = {0, 0, 0};

BOOL RFD_DATA_INTERNAL g_xcvCalibSetPaRamp = FALSE;
UINT16 RFD_DATA_INTERNAL g_xcvCalibPaRampValue[4];

PRIVATE BOOL RFD_DATA_INTERNAL g_XcvLimitMaxTxPower = FALSE;


// -------------------------------------------------------------------//
//                      Macros for SPI timings                        //
// -------------------------------------------------------------------//
#define RFSPI_CLOCK_13M

#if defined(RFSPI_CLOCK_26M)
#define RFSPI_CLOCK HAL_RFSPI_26M_CLK
#define CMD_DUR_FACTOR 2600
#elif defined(RFSPI_CLOCK_19_5M)
#define RFSPI_CLOCK HAL_RFSPI_19_5M_CLK
#define CMD_DUR_FACTOR 1950
#elif defined(RFSPI_CLOCK_13M)
#define RFSPI_CLOCK HAL_RFSPI_13M_CLK
#define CMD_DUR_FACTOR 1300
#elif defined(RFSPI_CLOCK_6_5M)
#define RFSPI_CLOCK HAL_RFSPI_6_5M_CLK
#define CMD_DUR_FACTOR 650
#elif defined(RFSPI_CLOCK_3_25M)
#define RFSPI_CLOCK HAL_RFSPI_3_25M_CLK
#define CMD_DUR_FACTOR 325
#else
#error "RFSPI clock is not specified"
#endif

// 1 Qb = 12/13 us
// 1 Cmd = 35 clocks
//       = (3500/CMD_DUR_FACTOR) / (12/13) Qb
#define CMD_DURATION(n) (((n) * 3500 * 13) / ((CMD_DUR_FACTOR) * 12) + 1)


//--------------------------------------------------------------------//
//                                                                    //
//                RF transceiver Serial link management               //
//                                                                    //
//--------------------------------------------------------------------//

// Generates commands according to the Xcver SPI serial format.
//
// Rda6220 RF Transceiver:
//  1 write command is:
//  1 bit equal to 0 for write + 18 bits for data + 6 bits for address = 25 bits
//           0AAAAAAD | DDDDDDDD | DDDDDDDD | D0000000
//      MSB    Byte0     Byte 1     Byte 2      Byte 3  LSB
//
//  1 read command is:
//  1 bit equal to 1 for read + 6 bits for address + 2 bits for clk turnaround + 18 bits for data = 27 bits
//           1AAAAAAD | DDDDDDDD | DDDDDDDD | D0000000
//      MSB    Byte0     Byte 1       Byte 2     Byte 3  LSB

VOID RFD_FUNC_INTERNAL xcvBuildSpiCmd (HAL_RFSPI_CMD_T *cmd, UINT8 address, UINT32 data)
{
    UINT8 array[RDA6220_ONE_CMD_BYTE_QTY];
    UINT32 cmdWord = RDA6220_WRITE_FMT(address, data);

#ifdef SPI_REG_DEBUG
#ifdef XCV_REG_BUFFER_READ_WRITE
    if (address == 0x3f)
    {
        g_rfdXcvCurPage = data&0x3;
    }
    if (address < XCV_REG_NUM_PER_PAGE)
    {
        g_rfdXcvRegLastWriteValue[g_rfdXcvCurPage][address] = data&XCV_REG_DATA_MASK;
    }
#endif // XCV_REG_BUFFER_READ_WRITE
#endif // SPI_REG_DEBUG

    // Hardcoded
    array[0] = (cmdWord >> (3 * BYTE_SIZE)) & BYTE_MASK;
    array[1] = (cmdWord >> (2 * BYTE_SIZE)) & BYTE_MASK;
    array[2] = (cmdWord >> (BYTE_SIZE)) & BYTE_MASK;
    array[3] =  cmdWord & BYTE_MASK;

    hal_RfspiPushData(cmd,array,RDA6220_ONE_CMD_BYTE_QTY);
}

VOID RFD_FUNC_INTERNAL xcvReadReg (UINT16 address, UINT8 *output)
{
    UINT8  array[RDA6220_ONE_CMD_BYTE_QTY];
    UINT32 cmdWord;

    cmdWord = RDA6220_READ_FMT(address, 0);

    array[0] = (cmdWord >> (3 * BYTE_SIZE)) & BYTE_MASK;
    array[1] = (cmdWord >> (2 * BYTE_SIZE)) & BYTE_MASK;
    array[2] = (cmdWord >> (BYTE_SIZE)) & BYTE_MASK;
    array[3] =  cmdWord & BYTE_MASK;
    hal_RfspiDigRfRead(array,output);
}


//--------------------------------------------------------------------//
//                                                                    //
//                        RF Commands Generation                      //
//                                                                    //
//--------------------------------------------------------------------//
INT16 RFD_FUNC_INTERNAL Pcl2dBm (GSM_RFBAND_T band,UINT16 arfcn,UINT8 Pcl)
{
    INT16 dBm_level;
    UINT16 nb_arfcn_in_band;
    CALIB_ARFCN_T *pcl2dbm_o_arfcn;
    INT16 arfcn_s = arfcn;

    if ((band == GSM_BAND_GSM900)||(band == GSM_BAND_GSM850))
    {
        // GSM
        pcl2dbm_o_arfcn = (CALIB_ARFCN_T *)
                          g_xcvCalibPtr->pa->palcust.pcl2dbmArfcnG;
        // GSM-P band (0->124)
        if (    (arfcn_s >= RFD_ARFCN_EGSM_RX_MIN1) &&
                (arfcn_s <= RFD_ARFCN_EGSM_RX_MAX1))
        {
            arfcn_s -= RFD_ARFCN_EGSM_RX_MIN1;
        }
        // GSM-E band (975->1023)
        else if (   (arfcn >= RFD_ARFCN_EGSM_TX_MIN3) &&
                    (arfcn <= RFD_ARFCN_EGSM_TX_MAX3))
        {
            arfcn_s -= (RFD_ARFCN_EGSM_TX_MAX3 + 1);
        }
        // TODO: Tx offset not handled for now in GSM850 (set to the min)
        // GSM850 (128->251)
        else if((arfcn_s >= RFD_ARFCN_GSM850_MIN) &&
                (arfcn_s <= RFD_ARFCN_GSM850_MAX))
        {
            arfcn_s = RFD_ARFCN_EGSM_RX_MAX1;
        }
        nb_arfcn_in_band = RFD_ARFCN_EGSM_RX_MAX1 - RFD_ARFCN_EGSM_RX_MIN1;

        if (Pcl < 5)
            Pcl = 5;
        else if (Pcl > 19)
            Pcl = 19;
        Pcl -= 5;
    }
    else if (band == GSM_BAND_DCS1800)
    {
        // DCS
        pcl2dbm_o_arfcn = (CALIB_ARFCN_T *)
                          g_xcvCalibPtr->pa->palcust.pcl2dbmArfcnD;
        arfcn_s -= RFD_ARFCN_DCS_MIN;
        nb_arfcn_in_band = RFD_ARFCN_DCS_MAX - RFD_ARFCN_DCS_MIN;

        if (Pcl >= 29)
        {
            Pcl = CALIB_DCS_PCL_QTY - 1;
        }
        else
        {
            if (Pcl > 15) Pcl = 15;
        }
    }
    else
    {
        // PCS
        pcl2dbm_o_arfcn = (CALIB_ARFCN_T *)
                          g_xcvCalibPtr->pa->palcust.pcl2dbmArfcnP;
        arfcn_s -= RFD_ARFCN_PCS_MIN;
        nb_arfcn_in_band = RFD_ARFCN_PCS_MAX - RFD_ARFCN_PCS_MIN;

        if ((Pcl >=22) && (Pcl <= 30))
        {
            Pcl = CALIB_PCS_PCL_QTY - 2;
        }
        else if (Pcl == 31)
        {
            Pcl = CALIB_PCS_PCL_QTY - 1;
        }
        else
        {
            if (Pcl > 15) Pcl = 15;
        }
    }

    if (g_XcvLimitMaxTxPower && Pcl < 5)
    {
        Pcl = 5;
    }

    dBm_level = pcl2dbm_o_arfcn[Pcl][0] + // Min level for lower PCL
                ((arfcn_s * (pcl2dbm_o_arfcn[Pcl][1] - pcl2dbm_o_arfcn[Pcl][0]) /
                  nb_arfcn_in_band)); // Linear interpolation upon arfcn

    return dBm_level;
}

//======================================================================
// rfd_XcvLimitMaxTxPower
// This function used to inform XCV driver to limit the maximum Tx power
//----------------------------------------------------------------------
PUBLIC VOID rfd_XcvLimitMaxTxPower(BOOL on)
{
    g_XcvLimitMaxTxPower = on;
}

VOID RFD_FUNC_INTERNAL xcvSetArfcn (RFD_RFDIR_T dir, GSM_RFBAND_T band, UINT16 arfcn)
{
    // According to the band set the corresponding bit
    UINT32 selBand  = ( (band == GSM_BAND_GSM850)  ? 0 :
                        (band == GSM_BAND_GSM900)  ? 1 :
                        (band == GSM_BAND_DCS1800) ? 2 :
                        3 ) << 12; // GSM_BAND_PCS1900

    // If rx set the 11th bit, if tx the 10th bit
    UINT32 rxTxMode = ( (dir == RFD_DIR_RX) ? 2 : 1 ) << 10;

    // g_xcvCmdCtx.freqCtrl = (0x18000 | selBand | rxTxMode | (arfcn & RDA6220_ARFCN_MASK));
    g_xcvCmdCtx.freqCtrl = (0x38000 | selBand | rxTxMode | (arfcn & RDA6220_ARFCN_MASK));

}

INLINE
UINT8 RFD_FUNC_INTERNAL xcvSetRxGain (UINT8 gain, GSM_RFBAND_T band, UINT16 arfcn)
{
    UINT8 iLoss = 0;
    UINT8 XcvGain = 0;
    UINT8 GainIdx;
    CONST XCV_RX_GAIN_T* agcTable=  ((band == GSM_BAND_GSM850)|| (band == GSM_BAND_GSM900))?
                                    AgcTab_EGSM:
                                    AgcTab_DCS;



    switch (band)
    {
        // GSM850 uses 5 points          128  -- 251

        case GSM_BAND_GSM850:
            if (arfcn > 128 && arfcn < 141 ) // 128 - 140
                iLoss = XCV_PARAM(ILOSS(band))& 0xf;
            else if (arfcn < 166 && arfcn > 140) // 141 - 165
                iLoss = (XCV_PARAM(ILOSS(band)) >> 4)& 0xf;
            else if (arfcn < 191 && arfcn > 165) // 166 - 190
                iLoss = (XCV_PARAM(ILOSS(band)) >> 8)& 0xf;
            else if (arfcn < 226 && arfcn > 190) // 191 - 225
                iLoss = (XCV_PARAM(ILOSS(band)) >> 12)& 0xf;
            else
                iLoss = (XCV_PARAM(ILOSS(band)) >> 16)& 0xf;;
            break;
        // GSM uses 5 points
        case GSM_BAND_GSM900:
            if (arfcn < 42) // 0-40
                iLoss = XCV_PARAM(ILOSS(band))& 0xf;
            else if (arfcn < 107 && arfcn > 41) // 41-82
                iLoss = (XCV_PARAM(ILOSS(band)) >> 4)& 0xf;
            else if (arfcn < 125 && arfcn > 106) // 83-124
                iLoss = (XCV_PARAM(ILOSS(band)) >> 8)& 0xf;
            else if (arfcn < 1006 && arfcn > 974) // 975-999
                iLoss = (XCV_PARAM(ILOSS(band)) >> 12)& 0xf;
            else // 1000-1023
                iLoss = (XCV_PARAM(ILOSS(band)) >> 16)& 0xf;;
            break;
        // DCS uses 8 points
        case GSM_BAND_DCS1800:
            if (arfcn < 553) // 512-558
                iLoss = XCV_PARAM(ILOSS(band))& 0xf;
            else if (arfcn < 583 && arfcn > 552) // 559-604
                iLoss = (XCV_PARAM(ILOSS(band)) >> 4)& 0xf;
            else if (arfcn < 663 && arfcn > 582) // 605-650
                iLoss = (XCV_PARAM(ILOSS(band)) >> 8)& 0xf;
            else if (arfcn < 693 && arfcn > 662) // 651-696
                iLoss = (XCV_PARAM(ILOSS(band)) >> 12)& 0xf;
            else if (arfcn < 718 && arfcn > 692) // 697-742
                iLoss = (XCV_PARAM(ILOSS(band)) >> 16)& 0xf;
            else if (arfcn < 735 && arfcn > 717) // 743-788
                iLoss = (XCV_PARAM(ILOSS(band)) >> 20)& 0xf;
            else if (arfcn < 848 && arfcn > 734) // 789-834
                iLoss = (XCV_PARAM(ILOSS(band)) >> 24)& 0xf;
            else // 835-885
                iLoss = (XCV_PARAM(ILOSS(band)) >> 28)& 0xf;
            break;
        // PCS uses 8 points
        case GSM_BAND_PCS1900:
            if (arfcn < 550) // 512-549
                iLoss = XCV_PARAM(ILOSS(band))& 0xf;
            else if (arfcn < 587 && arfcn > 549) // 550-586
                iLoss = (XCV_PARAM(ILOSS(band)) >> 4)& 0xf;
            else if (arfcn < 623 && arfcn > 586) // 587-622
                iLoss = (XCV_PARAM(ILOSS(band)) >> 8)& 0xf;
            else if (arfcn < 660 && arfcn > 622) // 623-659
                iLoss = (XCV_PARAM(ILOSS(band)) >> 12)& 0xf;
            else if (arfcn < 697 && arfcn > 659) // 660-696
                iLoss = (XCV_PARAM(ILOSS(band)) >> 16)& 0xf;
            else if (arfcn < 734 && arfcn > 696) // 697-733
                iLoss = (XCV_PARAM(ILOSS(band)) >> 20)& 0xf;
            else if (arfcn < 771 && arfcn > 733) // 734-770
                iLoss = (XCV_PARAM(ILOSS(band)) >> 24)& 0xf;
            else // 771-810
                iLoss = (XCV_PARAM(ILOSS(band)) >> 28)& 0xf;
            break;

        default:
            break;
    }

    if (gain == RFD_AGC)
    {
        // FIXE ME: What is XCVER_AGC_TARGET
        gain = XCVER_AGC_TARGET + iLoss;
    }
    else
    {
        gain += iLoss;
        // Boundaries check
        gain = ((gain>RDA6220_MIN_IN_POWER)?RDA6220_MIN_IN_POWER:gain);
        gain = ((gain<RDA6220_MAX_IN_POWER)?RDA6220_MAX_IN_POWER:gain);
    }

    GainIdx = (gain-15);
    XcvGain = agcTable[GainIdx].totGain;

    g_xcvCmdCtx.gain = 0x01000 |
                       ( (agcTable[GainIdx].analogGain & 0x7)<<14) |
                       (agcTable[GainIdx].digGain & 0xff);

    return (XcvGain - iLoss);
}

//--------------------------------------------------------------------//
//                                                                    //
//                    Internal Functions                              //
//                                                                    //
//--------------------------------------------------------------------//

// *** xcvBuildRxSpiCmd (UINT16 arfcn, UINT8 gain) ***
// Prepare the transceiver commands for reception
INLINE
UINT8 RFD_FUNC_INTERNAL xcvBuildRxSpiCmd(UINT16 arfcn, GSM_RFBAND_T band, UINT8 gain, HAL_RFSPI_CMD_T *spiCmd)
{

    UINT8 XcvGain;

    // Set arfcn & band
    xcvSetArfcn (RFD_DIR_RX, band, arfcn);

    // Set Gain
    XcvGain = xcvSetRxGain (gain, band, arfcn);

    // Set DigitalAfc, 14 bit is set for digital AFC
    g_xcvCmdCtx.digAfc = 0x04000 | (g_afcDacVal & RDA6220_AFC_MASK);

    UINT16 adjustedCdac = g_CDAC + g_CdacDelta;
    xcvBuildSpiCmd (spiCmd, RDA6220_CDAC_REG, CDAC_REG_VALUE_BASE | ((adjustedCdac&0xff) << 0));

#if 0
    xcvBuildSpiCmd (spiCmd, 0x08, 0x1583c); // Band=GSM900;rxon=0;ARFCN=60(947MHz)
    xcvBuildSpiCmd (spiCmd, 0x01, 0x3c046); //
#else
    xcvBuildSpiCmd (spiCmd, RDA6220_FREQ_CTRL_REG, g_xcvCmdCtx.freqCtrl);
    xcvBuildSpiCmd (spiCmd, RDA6220_RX_GAIN_REG, g_xcvCmdCtx.gain);
    xcvBuildSpiCmd (spiCmd, RDA6220_AFC_REG, g_xcvCmdCtx.digAfc);
#endif

#ifdef SPI_REG_DEBUG
#ifdef XCV_REG_BUFFER_READ_WRITE
    {
        for (int i=0; i<XCV_WRITE_LIST_LEN; i++)
        {
            if (g_rfdXcvRegWriteList[i] != 0)
            {
                UINT32 page = (g_rfdXcvRegWriteList[i]>>20)&0x3;
                if (page != 0) // switch to the page
                {
                    xcvBuildSpiCmd (spiCmd, 0x3f, page);
                }
                xcvBuildSpiCmd (spiCmd,
                                (g_rfdXcvRegWriteList[i]>>24)&RDA6220_ADDR_MASK,
                                g_rfdXcvRegWriteList[i]&RDA6220_DATA_MASK);
                if (page != 0) // back to page 0
                {
                    xcvBuildSpiCmd (spiCmd, 0x3f, 0);
                }
                g_rfdXcvRegWriteList[i] = 0;
                break;  // One write cmd for one Rx
            }
        }
    }
#endif
#endif

    return XcvGain;

}

VOID xcv_SetStableVolt(UINT16 volt)
{
    g_xcvbattstablevolt = volt;
}
// *** xcvBuildTxSpiCmd  ***
// Prepare the transceiver commands for emission
INLINE
VOID RFD_FUNC_INTERNAL xcvBuildTxSpiCmd (RFD_WIN_T *Win, UINT16 arfcn, GSM_RFBAND_T band, HAL_RFSPI_CMD_T *spiCmd)
{
    UINT32 rampFactor_curve=0;
    UINT32 rampfactor_1= 0x20080;
    UINT32 rampfactor_2= 0x08020;
    UINT32 rampfactor_3= 0x00000;
    UINT32 temp=0;
    UINT8 WinBitMap = Win->bitmap;
    int count = 0;
    int curvernum = 0;
    int pclValue;

    // Set DigitalAfc, 14 bit is set for digital AFC
    g_xcvCmdCtx.digAfc = 0x04000 | (g_afcDacVal & RDA6220_AFC_MASK);

    UINT16 adjustedCdac = g_CDAC + g_CdacDelta;
    xcvBuildSpiCmd (spiCmd, RDA6220_CDAC_REG, CDAC_REG_VALUE_BASE | ((adjustedCdac&0xff) << 0));

    // Set Band & arfcn
    xcvSetArfcn (RFD_DIR_TX, band, arfcn);

    xcvBuildSpiCmd (spiCmd, RDA6220_FREQ_CTRL_REG,  g_xcvCmdCtx.freqCtrl);
    xcvBuildSpiCmd (spiCmd, RDA6220_AFC_REG, g_xcvCmdCtx.digAfc);

    if (g_xcvCalibSetPaRamp)
    {
        RFD_TRACE(RFD_INFO_TRC,0,"WinBitMap = %d, %d, %d, %d,%d",WinBitMap,
                  g_xcvCalibPaRampValue[0],
                  g_xcvCalibPaRampValue[1],
                  g_xcvCalibPaRampValue[2],
                  g_xcvCalibPaRampValue[3]);
        g_xcvCalibSetPaRamp = FALSE;
        while(WinBitMap)
        {
            // If active slot
            if (WinBitMap & 0x1)
            {
                temp=0;
                if(count==0)
                {
                    rampfactor_1 = rampfactor_1 & 0xff;   //00 0000 0000 1111 1111
                    rampfactor_1 |= (g_xcvCalibPaRampValue[count] & 0x3ff)<<8;
                }
                if(count==1)
                {
                    temp = 0x3 & g_xcvCalibPaRampValue[count];  // get lastest two bit
                    rampfactor_1 = rampfactor_1 & 0x3ff00;   // 11 1111 1111 0000 0000
                    rampfactor_1 |= (g_xcvCalibPaRampValue[count] & 0x3ff)>>2;

                    rampfactor_2 = rampfactor_2 & 0xffff;    // 00 1111 1111 1111 1111
                    rampfactor_2|= (temp & 0x3)<<16;
                }
                if(count==2)
                {
                    rampfactor_2 = rampfactor_2 & 0x3003f;   //11 0000 0000 0011 1111
                    rampfactor_2|= (g_xcvCalibPaRampValue[count] & 0x3ff)<<6;
                }
                if(count==3)
                {
                    temp = 0xf & g_xcvCalibPaRampValue[count];  // get lastest fore bit
                    rampfactor_2 = rampfactor_2 & 0x3fc00;   // 11 1111 1111 1100 0000
                    rampfactor_2|= (g_xcvCalibPaRampValue[count] & 0x3ff)>>4;

                    rampfactor_3 = rampfactor_3 & 0x3c000;    // 11 1100 0000 0000
                    rampfactor_3|= (temp & 0xf)<<14;
                }

                //******************************************************//
                // select ramp curver num                               //
                //******************************************************//

                if(band == GSM_BAND_GSM850 || band == GSM_BAND_GSM900)
                {

                    if(pclValue < 9)
                        curvernum = (0x0 & RDA6220_RAMP_CURVE_MASK) << RDA6220_RAMP_CURVE_OFFSET;
                    else if((8 < pclValue) && (pclValue < 12))
                        curvernum = (0x1 & RDA6220_RAMP_CURVE_MASK) << RDA6220_RAMP_CURVE_OFFSET;
                    else if((11 < pclValue) &&(pclValue < 16))
                        curvernum = (0x2 & RDA6220_RAMP_CURVE_MASK) << RDA6220_RAMP_CURVE_OFFSET;
                    else if(15 < pclValue)
                        curvernum = (0x3 & RDA6220_RAMP_CURVE_MASK) << RDA6220_RAMP_CURVE_OFFSET;
                }
                else
                {
                    if(pclValue < 4)
                        curvernum = (0x0 & RDA6220_RAMP_CURVE_MASK) << RDA6220_RAMP_CURVE_OFFSET;
                    else if((3 < pclValue) && (pclValue < 8))
                        curvernum = (0x1 & RDA6220_RAMP_CURVE_MASK) << RDA6220_RAMP_CURVE_OFFSET;
                    else if((7 < pclValue) &&(pclValue < 12))
                        curvernum = (0x2 & RDA6220_RAMP_CURVE_MASK) << RDA6220_RAMP_CURVE_OFFSET;
                    else if(11 < pclValue)
                        curvernum = (0x3 & RDA6220_RAMP_CURVE_MASK) << RDA6220_RAMP_CURVE_OFFSET;
                }

                rampFactor_curve = rampFactor_curve
                                   | (curvernum & RDA6220_RAMP_CURVE_MASK)<<((3-count)*3+3);// 8808e foure curve have been to one register,differen to 8808s
            }

            WinBitMap >>= 1;
            count++;
        }

        //ramp_slot_select ----change by like
        rampFactor_curve |= (((count-1) & 0x3) << 15);// Multi slot select,to select burst number
        rampFactor_curve |= 0x7; // gsm edge mode selcet ---> now is gsm slot 1,2,3
        xcvBuildSpiCmd (spiCmd, 0x03,  rampFactor_curve);

        xcvBuildSpiCmd (spiCmd, RDA6220_GSM_RAMP_1_REG,  rampfactor_1);
        xcvBuildSpiCmd (spiCmd, RDA6220_GSM_RAMP_2_REG,  rampfactor_2);
        xcvBuildSpiCmd (spiCmd, RDA6220_GSM_RAMP_3_REG,  rampfactor_3);

        return;
    }

#if 0
    xcvBuildSpiCmd (spiCmd, RDA6220_TX_RAMP_0_REG, 0x000c6); // PCL=19
    return;
#endif

#ifdef DIGRF_APC_TABLE
    CONST XCV_APC_T* pApcTable;
    UINT32 pclMin, pclMax;

    if (band == GSM_BAND_GSM850 || band == GSM_BAND_GSM900)
    {
        pApcTable = ApcTab_EGSM;
        pclMin = 5;
        pclMax = 29;
    }
    else
    {
        pApcTable = ApcTab_DCS;
        pclMin = 0;
        pclMax = 15;
    }

#else // ! DIGRF_APC_TABLE
    CONST UINT16 *pDacTable;
    INT16 dBmMax, dBmMin, dBmValue;
    int dBmOffset;
    UINT32 dacValue;

    CONST UINT16 DAC_MAX = RDA6220_RAMP_FACTOR_MASK;

    if (band == GSM_BAND_GSM850 || band == GSM_BAND_GSM900)
    {
        pDacTable = (UINT16 *)g_xcvCalibPtr->pa->palcust.profileG;
        dBmMax = g_xcvCalibPtr->pa->palcust.profileDbmMaxG;
        dBmMin = g_xcvCalibPtr->pa->palcust.profileDbmMinG;
    }
    else
    {
        pDacTable = (UINT16 *)g_xcvCalibPtr->pa->palcust.profileDp;
        dBmMax = g_xcvCalibPtr->pa->palcust.profileDbmMaxDp;
        dBmMin = g_xcvCalibPtr->pa->palcust.profileDbmMinDp;
    }

#endif // ! DIGRF_APC_TABLE

    while(WinBitMap)
    {
        // If active slot
        if (WinBitMap & 0x1)
        {
            pclValue = Win->gain[count];

#ifdef DIGRF_APC_TABLE
            if (pclValue < pclMin)
            {
                pclValue = pclMin;
            }
            else if (pclValue > pclMax)
            {
                // Copied from Pcl2dBm(). But why this way?
                if (band == GSM_BAND_GSM900 ||band == GSM_BAND_GSM850)
                {
                    pclValue = pclMax;
                }
                else if (band == GSM_BAND_DCS1800)
                {
                    if (pclValue >= 29)
                    {
                        pclValue = pclMin;
                    }
                    else
                    {
                        pclValue = pclMax;
                    }
                }
                else // PCS
                {
                    if (pclValue >=22 && pclValue <= 30)
                    {
                        pclValue = pclMin;
                    }
                    else if (pclValue >= 31)
                    {
                        pclValue = pclMin;
                    }
                    else
                    {
                        pclValue = pclMax;
                    }
                }
            }

            rampFactor[count] = (pApcTable[pclValue -pclMin].rampSet & RDA6220_RAMP_FACTOR_MASK)
                                << RDA6220_RAMP_FACTOR_OFFSET;

#else // ! DIGRF_APC_TABLE
            dBmValue = Pcl2dBm(band, arfcn, pclValue);
            if (dBmValue < dBmMin) dBmValue = dBmMin;
            else if (dBmValue > dBmMax) dBmValue = dBmMax;

            dBmOffset = 1023 * (dBmValue-dBmMin) / (dBmMax - dBmMin);

            dacValue = (pDacTable[dBmOffset]  > DAC_MAX) ? DAC_MAX :  pDacTable[dBmOffset];

            //***************************************************************//
            // prepare writing ramp multi factor to reg 10H,11H,12H          //
            //************************************************************** //

            temp=0;
            if(count==0)
            {
                rampfactor_1 = rampfactor_1 & 0xff;   //00 0000 0000 1111 1111
                rampfactor_1 |= (dacValue & 0x3ff)<<8;
            }
            if(count==1)
            {
                temp = 0x3 & dacValue;  // get lastest two bit
                rampfactor_1 = rampfactor_1 & 0x3ff00;   // 11 1111 1111 0000 0000
                rampfactor_1 |= (dacValue & 0x3ff)>>2;

                rampfactor_2 = rampfactor_2 & 0xffff;    // 00 1111 1111 1111 1111
                rampfactor_2|= (temp & 0x3)<<16;
            }
            if(count==2)
            {
                rampfactor_2 = rampfactor_2 & 0x3003f;   //11 0000 0000 0011 1111
                rampfactor_2|= (dacValue & 0x3ff)<<6;
            }
            if(count==3)
            {
                temp = 0xf & dacValue;  // get lastest fore bit
                rampfactor_2 = rampfactor_2 & 0x3fc00;   // 11 1111 1111 1100 0000
                rampfactor_2|= (dacValue & 0x3ff)>>4;

                rampfactor_3 = rampfactor_3 & 0x3c000;    // 11 1100 0000 0000
                rampfactor_3|= (temp & 0xf)<<14;
            }

            //******************************************************//
            // select ramp curver num                               //
            //******************************************************//

            if(band == GSM_BAND_GSM850 || band == GSM_BAND_GSM900)
            {

                if(pclValue < 9)
                    curvernum = (0x0 & RDA6220_RAMP_CURVE_MASK) << RDA6220_RAMP_CURVE_OFFSET;
                else if((8 < pclValue) && (pclValue < 12))
                    curvernum = (0x1 & RDA6220_RAMP_CURVE_MASK) << RDA6220_RAMP_CURVE_OFFSET;
                else if((11 < pclValue) &&(pclValue < 16))
                    curvernum = (0x2 & RDA6220_RAMP_CURVE_MASK) << RDA6220_RAMP_CURVE_OFFSET;
                else if(15 < pclValue)
                    curvernum = (0x3 & RDA6220_RAMP_CURVE_MASK) << RDA6220_RAMP_CURVE_OFFSET;
            }
            else
            {
                if(pclValue < 4)
                    curvernum = (0x0 & RDA6220_RAMP_CURVE_MASK) << RDA6220_RAMP_CURVE_OFFSET;
                else if((3 < pclValue) && (pclValue < 8))
                    curvernum = (0x1 & RDA6220_RAMP_CURVE_MASK) << RDA6220_RAMP_CURVE_OFFSET;
                else if((7 < pclValue) &&(pclValue < 12))
                    curvernum = (0x2 & RDA6220_RAMP_CURVE_MASK) << RDA6220_RAMP_CURVE_OFFSET;
                else if(11 < pclValue)
                    curvernum = (0x3 & RDA6220_RAMP_CURVE_MASK) << RDA6220_RAMP_CURVE_OFFSET;
            }

            rampFactor_curve = rampFactor_curve
                               | (curvernum & RDA6220_RAMP_CURVE_MASK)<<((3-count)*3+3);// 8808e foure curve have been to one register,differen to 8808s
#endif // ! DIGRF_APC_TABLE
        }

        WinBitMap >>= 1;
        count++;
    }

    //ramp_slot_select ----change by like
    rampFactor_curve |= (((count-1) & 0x3) << 15);// Multi slot select,to select burst number
    rampFactor_curve |= 0x7; // gsm edge mode selcet ---> now is gsm slot 1,2,3
    xcvBuildSpiCmd (spiCmd, 0x03,  rampFactor_curve);

    xcvBuildSpiCmd (spiCmd, RDA6220_GSM_RAMP_1_REG,  rampfactor_1);
    xcvBuildSpiCmd (spiCmd, RDA6220_GSM_RAMP_2_REG,  rampfactor_2);
    xcvBuildSpiCmd (spiCmd, RDA6220_GSM_RAMP_3_REG,  rampfactor_3);
}


//--------------------------------------------------------------------//
//                                                                    //
//               Functions for GLOBAL PAL API                         //
//                                                                    //
//--------------------------------------------------------------------//

RFD_XCV_PARAMS_T RFD_DATA_INTERNAL g_xcvParams =
{
    // Misc Modem params
    .dcocMode= RFD_SW_DCOC_ENABLED,
    //.dcocMode= RFD_NO_DCOC,
    .lowRla = 104,
    .dtxTcoSettings = 0,
    .rxOnTcoSettings = 0,

    // PowerUp params
    .wakeUpFrameNbr = 0, //1, // activating low power optimisation
    .pu2VcoEnDelay = 1,
    .vcoEn2VcoStableDelay = 60,
    .vrfBehaviour = RFD_LDO_BEHAVIOR_DONT_USE,

    // Window bounds
    // NOTE : these timings will be wrong if changed by a calibration
    // file, they should be updated at execution time
    .winBounds = {
        .rxSetup = DEFAULT_CALIB_SPI_CMD - CMD_DURATION(7),
        .rxHold  = DEFAULT_CALIB_PDN_RX_DN,
        .txSetup = DEFAULT_CALIB_SPI_CMD - CMD_DURATION(11),
        .txHold  = DEFAULT_CALIB_PDN_TX_DN
    },

    // DigRf params
    .digRf = {
        .used           = TRUE,
        .rxRate         = RFD_DIGRF_1_SAMPLE_PER_SYMB,
        .rxSampleWidth  = XCV_SAMPLE_WIDTH,
        .rxSampleAlign  = RFD_DIGRF_SAMPLE_ALIGN_LSB,
        .rxAdc2DbmGain  = XCV_ADC2DBM_GAIN,
        .rxClkPol       = XCV_DIGRF_RX_CLK_POL,
        .txClkPol       = RFD_DIGRF_INV_CLK_POL,
#ifdef __TX_BLOCK_MODE__
        .txMode         = RFD_DIGRF_TX_BLOCK
#else
        .txMode         = RFD_DIGRF_TX_STREAM
#endif
    },

    .txBstMap = {
        .preamble  = 4,
        .guardStart= 4,
        .postamble = 4
    }

};

const RFD_XCV_PARAMS_T* rfd_XcvGetParameters(VOID)
{

    // Get the calibration pointer
    g_xcvCalibPtr = calib_GetPointers();

    g_xcvParams.dtxTcoSettings    = MASK_TCO(XCV->PDN) | HAL_TCU_MASK_TCO_GMSK_ON | HAL_TCU_MASK_DIGRF_STROBE | HAL_TCU_MASK_RF_SPI_MARKED_CMD;
    g_xcvParams.rxOnTcoSettings = MASK_TCO(XCV->PDN) | HAL_TCU_MASK_TCO_RX_DEC_ON;
    g_xcvParams.winBounds.rxSetup = XCV_TIME(SPI_CMD) - CMD_DURATION(7);
    g_xcvParams.winBounds.rxHold  = XCV_TIME(PDN_RX_DN);
    g_xcvParams.winBounds.txSetup = XCV_TIME(SPI_CMD) - CMD_DURATION(11);
    g_xcvParams.winBounds.txHold  = XCV_TIME(PDN_TX_DN);
    return &g_xcvParams;
}

const HAL_RFSPI_CFG_T g_spiCfg =
{
    TRUE,  // Enable
    TRUE, // csActiveHi
    TRUE,  // modeDigRfRead
    FALSE, // modeClockBack2Back
    FALSE, // inputEn
    FALSE, // clkFallEdge
    HAL_RFSPI_HALF_CLK_PERIOD_1, // clkDelay
    HAL_RFSPI_HALF_CLK_PERIOD_2, // doDelay
    HAL_RFSPI_HALF_CLK_PERIOD_3, // diDelay
    HAL_RFSPI_HALF_CLK_PERIOD_2, // csDelay
    HAL_RFSPI_HALF_CLK_PERIOD_0, // csEndHold
    HAL_RFSPI_HALF_CLK_PERIOD_2, // csEndPulse
    2,  // turnAroundCycles
#if (RFSPI_RX_WORKAROUND)
    25, // frameSize
    18, // inputFrameSize
#else
    24, // frameSize
    17, // inputFrameSize
#endif
};



//-----------------------------//
// Window Open/Close Functions //
//-----------------------------//
UINT8 RFD_FUNC_INTERNAL rfd_XcvRxOn(RFD_WIN_T *Win, INT16 start)
{
    UINT8 XcvGain;
    HAL_RFSPI_CMD_T *spiCmd;
    UINT8 i =0;
    UINT8 Winbitmap = (Win->bitmap & 0xf) << 1;
    UINT8 nbSlots = (Winbitmap & 0x10)? 4:
                    (Winbitmap & 0x8)? 3:
                    (Winbitmap & 0x4)? 2: 1;


    // Allocate the spi command
    spiCmd = hal_RfspiNewCmd(  (start + XCV_TIME(SPI_CMD) - CMD_DURATION(7)),
                               Win->winIdx );

    // Build the command
    XcvGain = xcvBuildRxSpiCmd (    Win->arfcn,
                                    Win->band,
                                    Win->gain[0],
                                    spiCmd);

    for(i =0; i<nbSlots; i++)
    {
        UINT32 prev_slot = Winbitmap & 0x1;
        UINT32 curr_slot = Winbitmap & 0x2;
        // If current slot active
        if (curr_slot)
        {
            //If previous slot not active
            if(!prev_slot)
            {
                hal_TcuSetEvent(HAL_TCU_START_RFIN_RECORD, start + XCV_TIME(PDN_RX_UP));
                hal_TcuSetEvent (XCV_SET_PDN, start + XCV_TIME(PDN_RX_UP));

                if(Win->type == GSM_WIN_TYPE_FCCH)
                {
                    // FCCH contains consecutive multiple slots only. No need to stop here.
                    break;
                }
            }
            // The last active slot will be stopped by rfd_XcvRxOff()
        }
        else
        {
            //If previous slot active
            if(prev_slot)
            {
                // BCPU does NOT expect to receive the last Qb
                hal_TcuSetEvent(XCV_CLR_PDN, start + XCV_TIME(PDN_RX_DN));
                hal_TcuSetEvent(HAL_TCU_STOP_RFIN_RECORD, start + XCV_TIME(PDN_RX_DN));
            }
        }

        Winbitmap >>= 1;
        start += GSM_SLOT_QS_DURATION;
    }

    Win->swapIQ = FALSE;

    RFD_TRACE(RFD_INFO_TRC,0,"xcvRxOn @ %d Qb",start);

    return XcvGain;
}

VOID RFD_FUNC_INTERNAL rfd_XcvRxOff(RFD_WIN_T *Win,INT16 stop)
{
    // Transceiver in PowerDown
    hal_TcuSetEvent (XCV_CLR_PDN, stop + XCV_TIME(PDN_RX_DN));
    // stop the recording
    hal_TcuSetEvent (HAL_TCU_STOP_RFIN_RECORD, stop+ XCV_TIME(PDN_RX_DN));

    RFD_TRACE(RFD_INFO_TRC,0,"xcvRxOff @ %d Qb",stop);
}

VOID RFD_FUNC_INTERNAL rfd_XcvTxOn(RFD_WIN_T *Win, INT16 start)
{
    HAL_RFSPI_CMD_T *spiCmd;
    UINT8 i =0;
    UINT8 Winbitmap = (Win->bitmap & 0xf) << 1;
    UINT8 nbSlots = (Winbitmap & 0x10)? 4:
                    (Winbitmap & 0x8)? 3:
                    (Winbitmap & 0x4)? 2: 1;

#ifdef DIGRF_TXEN_V2_WORKAROUND
    hal_TcuSetEvent(HAL_TCU_START_GMSK, start + XCV_TIME(PDN_TX_UP));
    hal_TcuSetEvent(HAL_TCU_STOP_GMSK, start + XCV_TIME(PDN_TX_UP)+20);
#endif // DIGRF_TXEN_V2_WORKAROUND

    // Needs an Spi Command
    spiCmd = hal_RfspiNewCmd(  (start + XCV_TIME(SPI_CMD)- CMD_DURATION(11)),
                               Win->winIdx);

    // mark for DTX
    spiCmd->maskable = TRUE;

    for(i =0; i<nbSlots; i++)
    {
        UINT32 prev_slot = Winbitmap & 0x1;
        UINT32 curr_slot = Winbitmap & 0x2;
        UINT32 next_slot = Winbitmap & 0x4;
        // If current slot active
        if (curr_slot)
        {
            // If prev slot not active
            if(!prev_slot)
            {
                hal_TcuSetEvent (HAL_TCU_DIGRF_STB, start + XCV_TIME(TX_STROBE_ON));
#ifndef _TX_BLOCK_MODE
                hal_TcuSetEvent(HAL_TCU_START_GMSK, start + XCV_TIME(TX_GMSK_ON));
#endif
                hal_TcuSetEvent(XCV_SET_PDN, start + XCV_TIME(PDN_TX_UP));
            }
            // If next slot not active
            if(!next_slot)
            {
                UINT16 stop = ((Win->type == GSM_WIN_TYPE_TXABURST)?
                               (start + GSM_ABURST_ACTIVE_QS_DURATION + 4*g_xcvParams.txBstMap.postamble):
                               (start + GSM_NBURST_ACTIVE_QS_DURATION + 4*g_xcvParams.txBstMap.postamble));

                hal_TcuSetEvent (HAL_TCU_DIGRF_STB, stop + XCV_TIME(TX_STROBE_OFF));
#ifndef _TX_BLOCK_MODE
                hal_TcuSetEvent(HAL_TCU_STOP_GMSK, stop + XCV_TIME(TX_GMSK_OFF));
#endif
                hal_TcuSetEvent (XCV_CLR_PDN, stop + XCV_TIME(PDN_TX_DN));
            }
        }

        start += (Win->type == GSM_WIN_TYPE_TXABURST)?
                 (GSM_ABURST_ACTIVE_QS_DURATION + 4*8):  // active access burst + 8 guard bits.
                 GSM_SLOT_QS_DURATION;
        Winbitmap >>= 1;
    }

    // Build the command
    xcvBuildTxSpiCmd (Win,
                      Win->arfcn,
                      Win->band,
                      spiCmd);

    Win->swapIQ = FALSE;

    RFD_TRACE(RFD_INFO_TRC,0,"xcvTxOn @ %d Qb",start);
}

VOID RFD_FUNC_INTERNAL rfd_XcvTxOff(RFD_WIN_T *Win,INT16 stop)
{
    // Put Transceiver in Powerdown
    if (Win->type==GSM_WIN_TYPE_TXABURST)
        stop+=16;

    // Do nothing since Tx stop operations have been programmed in rfd_XcvTxOn()

    RFD_TRACE(RFD_INFO_TRC,0,"xcvTxOff @ %d Qb",stop);
}

// *** UINT8 xcvGetAgcGain
UINT8 RFD_FUNC_INTERNAL rfd_XcvGetAgcGain(UINT8 step, GSM_RFBAND_T band, UINT16 arfcn)
{
    UINT8 xcver_gain = 0;
    UINT8 gain = XCVER_AGC_TARGET + XCV_PARAM(ILOSS(band));
    UINT8 index = (gain - 15);

    CONST XCV_RX_GAIN_T* agcTable =  ((band == GSM_BAND_GSM850)|| (band == GSM_BAND_GSM900))?
                                     AgcTab_EGSM:
                                     AgcTab_DCS;

    xcver_gain = agcTable[index].totGain - (XCV_PARAM(ILOSS(band))& 0xf);

    return xcver_gain;
}

VOID RFD_FUNC_INTERNAL rfd_XcvFrequencyTuning(INT32 FOf, GSM_RFBAND_T band)
{
    // Frequency(Hz) of CDAC Offset * DacOffset_1Hz(900MHzBand)
    // DacOffset_1Hz(900MHzBand) = 1000*1Hz/XCV_PARAM(AFC_GAIN(GSM_BAND_GSM900))
    //#define DELTA_AFC_VALUE ((XCV_PARAM(AFC_FREQ(GSM_BAND_GSM900))-XCV_PARAM(CDAC_FREQ))*1000/XCV_PARAM(AFC_GAIN2(GSM_BAND_GSM900)))
    extern UINT8 g_Calib_OnSite;
    if(g_Calib_OnSite)
    {
        if (band==GSM_BAND_GSM850 || band==GSM_BAND_GSM900 )
        {
            if(ABS(FOf) >= 5000)
            {
                g_CdacDelta += FOf/1200;
                if((g_CDAC + g_CdacDelta) >  RDA6220_CDAC_MAX)
                    g_CdacDelta = RDA6220_CDAC_MAX - g_CDAC;
                else if((g_CDAC + g_CdacDelta) <  RDA6220_CDAC_MIN)
                    g_CdacDelta = g_CDAC - RDA6220_CDAC_MIN;
                if (g_xcvCalibPtr->hstXcv != NULL)
                    HST_XCV_PARAM(XTAL) = g_CDAC + g_CdacDelta;

                return;
            }

        }
        else
        {
            if(ABS(FOf) >= 10000)
            {

                g_CdacDelta += FOf/2400;
                if((g_CDAC + g_CdacDelta) >  RDA6220_CDAC_MAX)
                    g_CdacDelta = RDA6220_CDAC_MAX - g_CDAC;
                else if((g_CDAC + g_CdacDelta) <  RDA6220_CDAC_MIN)
                    g_CdacDelta = g_CDAC - RDA6220_CDAC_MIN;
                if (g_xcvCalibPtr->hstXcv != NULL)
                    HST_XCV_PARAM(XTAL) = g_CDAC + g_CdacDelta;

                return;
            }
        }

    }
#if 1
    INT32 DacOffset = (1000*FOf) / XCV_PARAM(AFC_GAIN(band));
    g_afcDacVal += DacOffset;
#else
    UINT16 afcDacVal = g_afcDacVal;
    INT16 CdacDelta = g_CdacDelta;

    INT16 AfcGain = 0;
    if (CdacDelta == 0)
    {
        AfcGain = XCV_PARAM(AFC_GAIN(band));
        if(AfcGain < 150)
            AfcGain = AfcGain * 100;
    }
    else
    {
        AfcGain = XCV_PARAM(AFC_GAIN2(band));
#if (XCV_AFC_COMPATIBLE_WITH_OLD_CALIB_DATA)
        if(AfcGain == 0)
        {
            if (band==GSM_BAND_GSM850 || band==GSM_BAND_GSM900 )
            {
                AfcGain = XCV_PARAM(AFC_GAIN(band))*100;
            }
            else
            {
                AfcGain =  XCV_PARAM(AFC_GAIN(band))*100;
            }
        }
#endif // XCV_AFC_COMPATIBLE_WITH_OLD_CALIB_DATA
    }
    // AFC DAC gain is in thousend's of Hertz
    INT32 DacOffset = (1000*FOf) / AfcGain;
    INT32 DeltaAfcValue = 0;
    if (band==GSM_BAND_GSM850 || band==GSM_BAND_GSM900 )
    {
#if (XCV_AFC_COMPATIBLE_WITH_OLD_CALIB_DATA)
        if(!XCV_PARAM(AFC_GAIN2(band)))
            XCV_PARAM(AFC_GAIN2(band)) =  XCV_PARAM(AFC_GAIN(band))*100;
#endif // XCV_AFC_COMPATIBLE_WITH_OLD_CALIB_DATA
        DeltaAfcValue = ((XCV_PARAM(AFC_FREQ(band))-XCV_PARAM(CDAC_FREQ))*1000/XCV_PARAM(AFC_GAIN2(band)));
    }
    else
    {
#if (XCV_AFC_COMPATIBLE_WITH_OLD_CALIB_DATA)
        if(!XCV_PARAM(AFC_GAIN2(band)))
            XCV_PARAM(AFC_GAIN2(band)) =  XCV_PARAM(AFC_GAIN(band))*100;
#endif // XCV_AFC_COMPATIBLE_WITH_OLD_CALIB_DATA
        DeltaAfcValue = ((XCV_PARAM(AFC_FREQ(band))-2*XCV_PARAM(CDAC_FREQ))*1000/XCV_PARAM(AFC_GAIN2(band)));
    }

    INT32 afcValue = afcDacVal + DacOffset;
    if (CdacDelta == 0)
    {
        if (afcValue < RDA6220_AFC_MIN)
        {
            afcValue += DeltaAfcValue;
            CdacDelta -= RDA6220_CDAC_OFFSET;
        }
        else if (afcValue > RDA6220_AFC_MAX)
        {
            afcValue -= DeltaAfcValue;
            CdacDelta = RDA6220_CDAC_OFFSET;
        }
    }
    else if (CdacDelta > 0)
    {
        if (afcValue < RDA6220_AFC_MIN)
        {
            afcValue += DeltaAfcValue;
            CdacDelta = 0;
        }
        else if (afcValue > RDA6220_AFC_MAX)
        {
            afcValue = RDA6220_AFC_MAX;
        }
    }
    else // (CdacDelta < 0)
    {
        if (afcValue > RDA6220_AFC_MAX)
        {
            afcValue -= DeltaAfcValue;
            CdacDelta = 0;
        }
        else if (afcValue < RDA6220_AFC_MIN)
        {
            afcValue = RDA6220_AFC_MIN;
        }
    }

    g_afcDacVal = afcValue;
    g_CdacDelta = CdacDelta;
    //RFD_TRACE(RFD_INFO_TRC|TSTDOUT,0,"g_afcDacVal: %#x ", g_afcDacVal);
#endif
}

//-----------------------------------//
// Crystal Aging detection stuff     //
//-----------------------------------//
#define AGING_DAC_LIMIT     0x9F // 1.5 CDAC unit ~= 159 Dac units
#define DAC_MID_RANGE       RDA6220_AFC_MID
INT32 RFD_FUNC_INTERNAL rfd_XcvGetAfcOffset(VOID)
{
    //return (g_AfcOffset + (g_afcDacVal - DAC_MID_RANGE));
#ifdef __USE_DIGITAL_AFC__
#define AGING_LIMIT  0xff

    INT32 AfcOffset = 0xffff / AGING_LIMIT;

    if (AfcOffset != 0)
    {
        return (AfcOffset);
    }
    else
    {
        return (PAL_XTAL_NO_AGING);
    }
#endif
    return 0;
}

#ifdef DIGRF_APC_TABLE
VOID rfd_XcvBuildApcTable(VOID)
{
    INT16 dBmMax, dBmMin, dBmValue;
    UINT32 i, dBmOffset;
    UINT32 pclValue, pclMin, pclMax;
    UINT16 *padac;
    XCV_APC_T *pApcTable;

    GSM_RFBAND_T band;
    UINT16 arfcn;

    CONST UINT16 DAC_MAX  = RDA6220_RAMP_FACTOR_MASK;

    // Select PA profile
    for (i=0; i<2; i++)
    {
        if (i == 0) // GSM 850/900
        {
            band = GSM_BAND_GSM900;
            arfcn = RFD_ARFCN_EGSM_RX_MIN1;
            pclMin = 5;
            pclMax = 29;
            pApcTable = ApcTab_EGSM;
            padac = (UINT16 *)g_xcvCalibPtr->pa->palcust.profileG;
            dBmMax = g_xcvCalibPtr->pa->palcust.profileDbmMaxG;
            dBmMin = g_xcvCalibPtr->pa->palcust.profileDbmMinG;
        }
        else // DCS/PCS
        {
            band = GSM_BAND_DCS1800;
            arfcn = RFD_ARFCN_DCS_MIN;
            pclMin = 0;
            pclMax = 15;
            pApcTable = ApcTab_DCS;
            padac = (UINT16 *)g_xcvCalibPtr->pa->palcust.profileDp;
            dBmMax = g_xcvCalibPtr->pa->palcust.profileDbmMaxDp;
            dBmMin = g_xcvCalibPtr->pa->palcust.profileDbmMinDp;
        }

        for (pclValue = pclMin; pclValue <= pclMax; pclValue++)
        {
            dBmValue = Pcl2dBm(band, arfcn, pclValue);
            if (dBmValue < dBmMin) dBmValue = dBmMin;
            else if (dBmValue > dBmMax) dBmValue = dBmMax;

            dBmOffset = 1023 * (dBmValue-dBmMin) / (dBmMax - dBmMin);

            pApcTable[pclValue - pclMin].rampSet = (padac[dBmOffset] > DAC_MAX) ?
                                                   DAC_MAX : padac[dBmOffset];
        }
    }

}
#endif // DIGRF_APC_TABLE

//-----------------------------------//
// Read XCV Chip ID from the chip                  //
//-----------------------------------//
VOID rfd_XcvReadChipId(VOID)
{
    if (g_xcvChipId == 0)
    {
        // Open RF Spi, ask for a 19.5MHz spi clock (Max speed)
        hal_RfspiOpen((HAL_RFSPI_CFG_T*)&g_spiCfg, RFSPI_CLOCK);
        HAL_RFSPI_CMD_T* confCmd = hal_RfspiNewSingleCmd();
        xcvBuildSpiCmd (confCmd, 0x06, 0x3f333); // SPI-Mode Select
        hal_RfspiSendCmd(confCmd);
        xcvBuildSpiCmd (confCmd, 0x3F, 0x00000); // Page0
        hal_RfspiSendCmd(confCmd);
#ifdef FPGA
        g_xcvChipId = GALLITE_CHIP_ID_8805_V5;
#else
        UINT32 count = 0;
        while (!(g_xcvChipId == GALLITE_CHIP_ID_8805_V5
                 || g_xcvChipId == GALLITE_CHIP_ID_8805_V6
                 || g_xcvChipId == GALLITE_CHIP_ID_8805_V7
                 || g_xcvChipId == GALLITE_CHIP_ID_8805_V8))
        {
            if (count++ == 100)
            {
                hal_HstSendEvent(XCV_CHIP_ID_INVALID_EVENT);
                hal_HstSendEvent(g_xcvChipId);
            }
            xcvReadReg(0, (UINT8*)&g_xcvChipId);
        }
#endif
        hal_RfspiClose();
    }
}

//-----------------------------------//
// Get XCV Chip ID from the driver variable               //
//-----------------------------------//
UINT32 rfd_XcvGetChipId(VOID)
{
    return g_xcvChipId;
}

//-----------------------------------//
// Open/Close & Low Power Functions  //
//-----------------------------------//

INLINE VOID xcvConfigure(VOID)
{
    HAL_RFSPI_CMD_T* confCmd = hal_RfspiNewSingleCmd();

    // ;=====Reg Reset==============================================

    // SPI-Mode selection is done in pmd_Open, and it had better be
    // performed once only when XCV is reset by h/w
    //xcvBuildSpiCmd (confCmd, 0x06, 0x3f333); // SPI-Mode Select
    /*
        xcvBuildSpiCmd (confCmd, 0x30, 0x08a00); // Soft Reset
        hal_RfspiSendCmd(confCmd);
        // wait 1ms
        hal_TimDelay(1 MS_WAITING);
        xcvBuildSpiCmd (confCmd, 0x30, 0x18a01); // Soft Reset
        hal_RfspiSendCmd(confCmd);
    */

    xcvBuildSpiCmd (confCmd, 0x06, 0x3f332); // SPI-Mode Select
//    xcvBuildSpiCmd (confCmd, 0x30, 0x1c240); // Soft Reset
    // wait 10ms
//    hal_TimDelay(10 MS_WAITING);
//    xcvBuildSpiCmd (confCmd, 0x30, 0x1c241); // Soft Reset
    hal_RfspiSendCmd(confCmd);

}

VOID rfd_XcvOpen(INT32 AfcOffset, CONST XCV_CONFIG_T* xcvConfig)
{
#ifdef SPI_REG_DEBUG
    extern VOID rfd_RegisterXcvRegCheckFunc(VOID);
    rfd_RegisterXcvRegCheckFunc();
#endif // SPI_REG_DEBUG

    XCV = xcvConfig;
    // Get the calibration pointer
    g_xcvCalibPtr = calib_GetPointers();

#ifdef DIGRF_APC_TABLE
    rfd_XcvBuildApcTable();
#endif // DIGRF_APC_TABLE

    // Derive Aging compensated CDAC value
    // Xtal triming capacitor register to default (calibrated)
    g_CDAC = RDA6220_CDAC_DEFAULT;
    g_AfcOffset = AfcOffset;
    // Replace the default only if aging compensation happened
    // TODO: to be tested
    if (g_AfcOffset > AGING_DAC_LIMIT)
    {
        g_CDAC += (g_AfcOffset/AGING_DAC_LIMIT);
    }
    else if (g_AfcOffset < AGING_DAC_LIMIT)
    {
        g_CDAC -= (g_AfcOffset/AGING_DAC_LIMIT);
    }

    // turn on AFC DAC - mid range
    g_afcDacVal = RDA6220_AFC_MID;
    g_CdacDelta = 0;

    // Open RF Spi, ask for a 19.5MHz spi clock (Max speed)
    hal_RfspiOpen((HAL_RFSPI_CFG_T*)&g_spiCfg, RFSPI_CLOCK);
    xcvConfigure();
}

INLINE VOID rfd_xcvRamp0Set(HAL_RFSPI_CMD_T* confCmd)
{
    xcvBuildSpiCmd (confCmd, 0x3D, (0x0000f | (g_xcvCalibPtr->pa->palcust.rampLow[0]<<8))); //  Ramp Curve000; y=(sinx)^1.5
    xcvBuildSpiCmd (confCmd, 0x3D, (0x0000e | (g_xcvCalibPtr->pa->palcust.rampLow[1]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x0000d | (g_xcvCalibPtr->pa->palcust.rampLow[2]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x0000c | (g_xcvCalibPtr->pa->palcust.rampLow[3]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x0000b | (g_xcvCalibPtr->pa->palcust.rampLow[4]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x0000a | (g_xcvCalibPtr->pa->palcust.rampLow[5]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00009 | (g_xcvCalibPtr->pa->palcust.rampLow[6]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00008 | (g_xcvCalibPtr->pa->palcust.rampLow[7]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00007 | (g_xcvCalibPtr->pa->palcust.rampLow[8]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00006 | (g_xcvCalibPtr->pa->palcust.rampLow[9]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00005 | (g_xcvCalibPtr->pa->palcust.rampLow[10]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00004 | (g_xcvCalibPtr->pa->palcust.rampLow[11]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00003 | (g_xcvCalibPtr->pa->palcust.rampLow[12]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00002 | (g_xcvCalibPtr->pa->palcust.rampLow[13]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00001 | (g_xcvCalibPtr->pa->palcust.rampLow[14]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00000 | (g_xcvCalibPtr->pa->palcust.rampLow[15]<<8))); //
    hal_RfspiSendCmd(confCmd);
}

INLINE VOID rfd_xcvRamp1Set(HAL_RFSPI_CMD_T* confCmd)
{
    xcvBuildSpiCmd (confCmd, 0x3D, (0x0001f | (g_xcvCalibPtr->pa->palcust.rampLow[16]<<8))); //  Ramp Curve000; y=(sinx)^1.5
    xcvBuildSpiCmd (confCmd, 0x3D, (0x0001e | (g_xcvCalibPtr->pa->palcust.rampLow[17]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x0001d | (g_xcvCalibPtr->pa->palcust.rampLow[18]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x0001c | (g_xcvCalibPtr->pa->palcust.rampLow[19]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x0001b | (g_xcvCalibPtr->pa->palcust.rampLow[20]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x0001a | (g_xcvCalibPtr->pa->palcust.rampLow[21]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00019 | (g_xcvCalibPtr->pa->palcust.rampLow[22]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00018 | (g_xcvCalibPtr->pa->palcust.rampLow[23]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00017 | (g_xcvCalibPtr->pa->palcust.rampLow[24]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00016 | (g_xcvCalibPtr->pa->palcust.rampLow[25]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00015 | (g_xcvCalibPtr->pa->palcust.rampLow[26]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00014 | (g_xcvCalibPtr->pa->palcust.rampLow[27]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00013 | (g_xcvCalibPtr->pa->palcust.rampLow[28]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00012 | (g_xcvCalibPtr->pa->palcust.rampLow[29]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00011 | (g_xcvCalibPtr->pa->palcust.rampLow[30]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00010 | (g_xcvCalibPtr->pa->palcust.rampLow[31]<<8))); //
    hal_RfspiSendCmd(confCmd);
}

INLINE VOID rfd_xcvRamp2Set(HAL_RFSPI_CMD_T* confCmd)
{
    xcvBuildSpiCmd (confCmd, 0x3D, (0x0002f | (g_xcvCalibPtr->pa->palcust.rampHigh[0]<<8))); //  Ramp Curve010; y=(sinx)^2
    xcvBuildSpiCmd (confCmd, 0x3D, (0x0002e | (g_xcvCalibPtr->pa->palcust.rampHigh[1]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x0002d | (g_xcvCalibPtr->pa->palcust.rampHigh[2]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x0002c | (g_xcvCalibPtr->pa->palcust.rampHigh[3]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x0002b | (g_xcvCalibPtr->pa->palcust.rampHigh[4]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x0002a | (g_xcvCalibPtr->pa->palcust.rampHigh[5]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00029 | (g_xcvCalibPtr->pa->palcust.rampHigh[6]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00028 | (g_xcvCalibPtr->pa->palcust.rampHigh[7]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00027 | (g_xcvCalibPtr->pa->palcust.rampHigh[8]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00026 | (g_xcvCalibPtr->pa->palcust.rampHigh[9]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00025 | (g_xcvCalibPtr->pa->palcust.rampHigh[10]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00024 | (g_xcvCalibPtr->pa->palcust.rampHigh[11]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00023 | (g_xcvCalibPtr->pa->palcust.rampHigh[12]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00022 | (g_xcvCalibPtr->pa->palcust.rampHigh[13]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00021 | (g_xcvCalibPtr->pa->palcust.rampHigh[14]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00020 | (g_xcvCalibPtr->pa->palcust.rampHigh[15]<<8))); //
    hal_RfspiSendCmd(confCmd);
}

INLINE VOID rfd_xcvRamp3Set(HAL_RFSPI_CMD_T* confCmd)
{
    xcvBuildSpiCmd (confCmd, 0x3D, (0x0003f | (g_xcvCalibPtr->pa->palcust.rampHigh[16]<<8))); //  Ramp Curve010; y=(sinx)^2.5
    xcvBuildSpiCmd (confCmd, 0x3D, (0x0003e | (g_xcvCalibPtr->pa->palcust.rampHigh[17]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x0003d | (g_xcvCalibPtr->pa->palcust.rampHigh[18]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x0003c | (g_xcvCalibPtr->pa->palcust.rampHigh[19]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x0003b | (g_xcvCalibPtr->pa->palcust.rampHigh[20]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x0003a | (g_xcvCalibPtr->pa->palcust.rampHigh[21]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00039 | (g_xcvCalibPtr->pa->palcust.rampHigh[22]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00038 | (g_xcvCalibPtr->pa->palcust.rampHigh[23]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00037 | (g_xcvCalibPtr->pa->palcust.rampHigh[24]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00036 | (g_xcvCalibPtr->pa->palcust.rampHigh[25]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00035 | (g_xcvCalibPtr->pa->palcust.rampHigh[26]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00034 | (g_xcvCalibPtr->pa->palcust.rampHigh[27]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00033 | (g_xcvCalibPtr->pa->palcust.rampHigh[28]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00032 | (g_xcvCalibPtr->pa->palcust.rampHigh[29]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00031 | (g_xcvCalibPtr->pa->palcust.rampHigh[30]<<8))); //
    xcvBuildSpiCmd (confCmd, 0x3D, (0x00030 | (g_xcvCalibPtr->pa->palcust.rampHigh[31]<<8))); //
    hal_RfspiSendCmd(confCmd);
}

VOID rfd_XcvRfInit(VOID)
{
    HAL_CLK_T *xcvClk = hal_ClkGet(FOURCC_XCV);
    hal_ClkEnable(xcvClk);

    HAL_RFSPI_CMD_T* confCmd = hal_RfspiNewSingleCmd();
    /*
        //xcvBuildSpiCmd (confCmd, 0x3f, 0x00000); // page 0
        xcvBuildSpiCmd (confCmd, 0x06, 0x3f332); // SPI-Mode Select
        xcvBuildSpiCmd (confCmd, 0x30, 0x1c240); // Soft Reset
        // wait 10ms
       hal_TimDelay(10 MS_WAITING);
        xcvBuildSpiCmd (confCmd, 0x30, 0x1c241); // Soft Reset
        hal_RfspiSendCmd(confCmd);
    */
    // ;=====IC Debug=====
//#ifdef FPGA
//#else
    xcvBuildSpiCmd (confCmd, 0x1a, 0x14853);//set digRf interface to 2.8V
    xcvBuildSpiCmd (confCmd, 0x0d, 0x2e224);
    xcvBuildSpiCmd (confCmd, 0x15, 0x24250);
    xcvBuildSpiCmd (confCmd, 0x19, 0x3dd74);
    xcvBuildSpiCmd (confCmd, 0x3f, 0x00001);
    xcvBuildSpiCmd (confCmd, 0x00, 0x0ff90);
    xcvBuildSpiCmd (confCmd, 0x22, 0x03dc8);
    xcvBuildSpiCmd (confCmd, 0x2a, 0x000f0);
    xcvBuildSpiCmd (confCmd, 0x3F, 0x00000); // page=0

    hal_RfspiSendCmd(confCmd);
//#endif
    // ;=====DCXO Setting=====
    UINT16 adjustedCdac = g_CDAC + g_CdacDelta;
    xcvBuildSpiCmd (confCmd, RDA6220_CDAC_REG, CDAC_REG_VALUE_BASE | ((adjustedCdac&0xff) << 0));
    // ;=====PLL_GSM Setting=====
    xcvBuildSpiCmd (confCmd, 0x31, 0x0444a);
    xcvBuildSpiCmd (confCmd, 0x3f, 0x00001);
    xcvBuildSpiCmd (confCmd, 0x02, 0x05d69);
    xcvBuildSpiCmd (confCmd, 0x04, 0x004c9);
    xcvBuildSpiCmd (confCmd, 0x03, 0x14d68);
    xcvBuildSpiCmd (confCmd, 0x12, 0x004bb);
    //xcvBuildSpiCmd (confCmd, 0x14, 0x25dad);// last 6 bit si for GPIO out to control RF switch mode
    xcvBuildSpiCmd (confCmd, 0x14, 0x25d9b);
    xcvBuildSpiCmd (confCmd, 0x1a, 0x08013);
    xcvBuildSpiCmd (confCmd, 0x1b, 0x0420a);
    xcvBuildSpiCmd (confCmd, 0x1f, 0x004ba);
    xcvBuildSpiCmd (confCmd, 0x23, 0x25d30);
    xcvBuildSpiCmd (confCmd, 0x16, 0x0021c);
    xcvBuildSpiCmd (confCmd, 0x28, 0x03cb4);
    xcvBuildSpiCmd (confCmd, 0x29, 0x036a2);
    xcvBuildSpiCmd (confCmd, 0x3f, 0x00002);
    xcvBuildSpiCmd (confCmd, 0x23, 0x29cbb);
    xcvBuildSpiCmd (confCmd, 0x2a, 0x29cbb);
    xcvBuildSpiCmd (confCmd, 0x30, 0x0c104);
    xcvBuildSpiCmd (confCmd, 0x34, 0x22249);
    xcvBuildSpiCmd (confCmd, 0x36, 0x028a4);
    xcvBuildSpiCmd (confCmd, 0x3f, 0x00000);
    hal_RfspiSendCmd(confCmd);

    // ;=====Tx Setting=====
    xcvBuildSpiCmd (confCmd, 0x02, 0x0027f);//stream mode
    xcvBuildSpiCmd (confCmd, 0x04, 0x03d00);
    xcvBuildSpiCmd (confCmd, 0x05, 0x20ffa);
    xcvBuildSpiCmd (confCmd, 0x2b, 0x20ff0);
    xcvBuildSpiCmd (confCmd, 0x2d, 0x20ff0);
    xcvBuildSpiCmd (confCmd, 0x35, 0x20ff0);
    xcvBuildSpiCmd (confCmd, 0x14, 0x00885);
    xcvBuildSpiCmd (confCmd, 0x16, 0x22056);
    xcvBuildSpiCmd (confCmd, 0x28, 0x24202);
    xcvBuildSpiCmd (confCmd, 0x3f, 0x00001);

    hal_RfspiSendCmd(confCmd);

    xcvBuildSpiCmd (confCmd, 0x15, 0x006b1);
    xcvBuildSpiCmd (confCmd, 0x39, 0x00059);
    xcvBuildSpiCmd (confCmd, 0x3f, 0x00002);

    hal_RfspiSendCmd(confCmd);

    xcvBuildSpiCmd (confCmd, 0x01, 0x09800);
    xcvBuildSpiCmd (confCmd, 0x02, 0x0c005);
    xcvBuildSpiCmd (confCmd, 0x27, 0x00c98);
    xcvBuildSpiCmd (confCmd, 0x28, 0x00000);
    xcvBuildSpiCmd (confCmd, 0x29, 0x00fcd);
    xcvBuildSpiCmd (confCmd, 0x32, 0x2904c);
    xcvBuildSpiCmd (confCmd, 0x33, 0x000a2);
    xcvBuildSpiCmd (confCmd, 0x37, 0x3c524);
    xcvBuildSpiCmd (confCmd, 0x39, 0x24e01);
    xcvBuildSpiCmd (confCmd, 0x3F, 0x00000); // page=0

    hal_RfspiSendCmd(confCmd);

    // ;=====Tx GSM Ramping Setting=====

    if((g_xcvCalibPtr->pa->palcust.rampLow[14] > 65) || (g_xcvCalibPtr->pa->palcust.rampLow[15] > 65))
    {
        xcvBuildSpiCmd (confCmd, 0x3D, 0x03f0f); //  Ramp Curve000; y=(sinx)^1.5
        xcvBuildSpiCmd (confCmd, 0x3D, 0x03e0e); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x03c0d); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x0390c); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x0350b); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x02f0a); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x02909); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x02208); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x01c07); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x01706); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x01205); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x00d04); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x00903); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x00502); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x00301); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x00200); //

        hal_RfspiSendCmd(confCmd);

        xcvBuildSpiCmd (confCmd, 0x3D, 0x03f1f); //  Ramp Curve001; y=(sinx)^1.5
        xcvBuildSpiCmd (confCmd, 0x3D, 0x03e1e); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x03c1d); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x0391c); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x0351b); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x02f1a); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x02919); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x02218); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x01c17); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x01716); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x01215); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x00d14); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x00913); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x00512); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x00311); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x00210); //

        hal_RfspiSendCmd(confCmd);

        xcvBuildSpiCmd (confCmd, 0x3D, 0x03e2f); //  Ramp Curve010; y=(sinx)^2
        xcvBuildSpiCmd (confCmd, 0x3D, 0x03c2e); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x0392d); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x0342c); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x0302b); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x02a2a); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x02629); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x02028); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x01c27); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x01626); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x01025); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x00a24); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x00623); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x00222); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x00021); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x00020); //

        hal_RfspiSendCmd(confCmd);

        xcvBuildSpiCmd (confCmd, 0x3D, 0x03f3f); //  Ramp Curve010; y=(sinx)^2.5
        xcvBuildSpiCmd (confCmd, 0x3D, 0x03e3e); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x03d3d); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x0393c); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x0333b); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x02d3a); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x02539); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x01e38); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x01737); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x01136); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x00b35); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x00234); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x00333); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x00132); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x00031); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x00030); //

        hal_RfspiSendCmd(confCmd);

        xcvBuildSpiCmd (confCmd, 0x3D, 0x03f4f); //  Ramp Curve010; y=(sinx)^3
        xcvBuildSpiCmd (confCmd, 0x3D, 0x03f4e); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x03c4d); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x0374c); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x0314b); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x0294a); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x02249); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x01a48); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x01347); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x00d46); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x00845); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x00444); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x00143); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x00042); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x00041); //
        xcvBuildSpiCmd (confCmd, 0x3D, 0x00040); //

        hal_RfspiSendCmd(confCmd);
    }
    else
    {
        rfd_xcvRamp0Set(confCmd);
        rfd_xcvRamp1Set(confCmd);
        rfd_xcvRamp2Set(confCmd);
        rfd_xcvRamp3Set(confCmd);
    }

    // ;=====Rx Setting=====

    xcvBuildSpiCmd (confCmd, 0x01, 0x1e438); //
    xcvBuildSpiCmd (confCmd, 0x0c, 0x01000); //
    xcvBuildSpiCmd (confCmd, 0x38, 0x10002); //
    //xcvBuildSpiCmd (confCmd, 0x38, 0x30002); //rx_clk_revert
    //xcvBuildSpiCmd (confCmd, 0x38, 0x31002);//iq_swap_clk_revert
    xcvBuildSpiCmd (confCmd, 0x3b, 0x0a685); //
    xcvBuildSpiCmd (confCmd, 0x17, 0x0585e); //
    xcvBuildSpiCmd (confCmd, 0x3f, 0x000001);

    hal_RfspiSendCmd(confCmd);

    xcvBuildSpiCmd (confCmd, 0x0c, 0x0ffc6); //anolog gain table 7
    xcvBuildSpiCmd (confCmd, 0x0b, 0x0ffa5); //anolog gain table 6
    xcvBuildSpiCmd (confCmd, 0x0a, 0x093a5); //anolog gain table 5
    xcvBuildSpiCmd (confCmd, 0x09, 0x0938d); //anolog gain table 4
    xcvBuildSpiCmd (confCmd, 0x08, 0x0918d); //anolog gain table 3
    xcvBuildSpiCmd (confCmd, 0x07, 0x0908d); //anolog gain table 2
    xcvBuildSpiCmd (confCmd, 0x06, 0x0008d); //anolog gain table 1
    xcvBuildSpiCmd (confCmd, 0x05, 0x00005); //anolog gain table 0
    xcvBuildSpiCmd (confCmd, 0x3f, 0x00000); //page=0

    hal_RfspiSendCmd(confCmd);

    // ;=====Timing Setting=====

    xcvBuildSpiCmd (confCmd, 0x13, 0x07239);
    xcvBuildSpiCmd (confCmd, 0x20, 0x05201);
    xcvBuildSpiCmd (confCmd, 0x24, 0x00540);
    xcvBuildSpiCmd (confCmd, 0x29, 0x08ae0);
    xcvBuildSpiCmd (confCmd, 0x37, 0x00ca0);
    xcvBuildSpiCmd (confCmd, 0x3e, 0x28b0c);

    hal_RfspiSendCmd(confCmd);

    // ;=====IC Self-Cal=====

    xcvBuildSpiCmd (confCmd, 0x09, 0x128c0);
    xcvBuildSpiCmd (confCmd, 0x30, 0x1aa05);
    xcvBuildSpiCmd (confCmd, 0x08, 0x1903c);
    xcvBuildSpiCmd (confCmd, 0x3f, 0x00002);
    xcvBuildSpiCmd (confCmd, 0x2e, 0x20206);
    xcvBuildSpiCmd (confCmd, 0x2f, 0x0f18f);
    xcvBuildSpiCmd (confCmd, 0x3f, 0x00000);
    xcvBuildSpiCmd (confCmd, 0x30, 0x1aa07);
    xcvBuildSpiCmd (confCmd, 0x06, 0x00001);

    hal_RfspiSendCmd(confCmd);
    //wating 200ms
    hal_TimDelay(200 MS_WAITING);

    xcvBuildSpiCmd (confCmd, 0x06, 0x00000);
    xcvBuildSpiCmd (confCmd, 0x08, 0x1a2b8);
    xcvBuildSpiCmd (confCmd, 0x3f, 0x00002);
    xcvBuildSpiCmd (confCmd, 0x2e, 0x2020a);
    xcvBuildSpiCmd (confCmd, 0x2f, 0x19299);
    xcvBuildSpiCmd (confCmd, 0x3f, 0x00000);
    xcvBuildSpiCmd (confCmd, 0x06, 0x00001);

    hal_RfspiSendCmd(confCmd);
    //wating 200ms
    hal_TimDelay(200 MS_WAITING);

    xcvBuildSpiCmd (confCmd, 0x06, 0x00000);
    xcvBuildSpiCmd (confCmd, 0x30, 0x16a07);
    xcvBuildSpiCmd (confCmd, 0x08, 0x1943c);
    xcvBuildSpiCmd (confCmd, 0x3f, 0x00002);
    xcvBuildSpiCmd (confCmd, 0x2e, 0x20206);
    xcvBuildSpiCmd (confCmd, 0x2f, 0x0f18f);
    xcvBuildSpiCmd (confCmd, 0x3f, 0x00000);
    xcvBuildSpiCmd (confCmd, 0x06, 0x00001);

    hal_RfspiSendCmd(confCmd);
    //wating 200ms
    hal_TimDelay(200 MS_WAITING);

    xcvBuildSpiCmd (confCmd, 0x06, 0x00000);
    xcvBuildSpiCmd (confCmd, 0x30, 0x1e205);

    hal_RfspiSendCmd(confCmd);

    // ;=====Mode Setting=====

    xcvBuildSpiCmd (confCmd, 0x0c, 0x21000);
    xcvBuildSpiCmd (confCmd, 0x1d, 0x2ecae);
    xcvBuildSpiCmd (confCmd, 0x3b, 0x0a348);
    xcvBuildSpiCmd (confCmd, 0x3f, 0x00001);
    xcvBuildSpiCmd (confCmd, 0x21, 0x3f801);  //to increase the IO,gidio dirve capacibility
    //xcvBuildSpiCmd (confCmd, 0x13, 0x1a236); // XCV gpo rof tx and rx need to adjust with schematic
    xcvBuildSpiCmd (confCmd, 0x13, 0x22652);
    xcvBuildSpiCmd (confCmd, 0x3f, 0x00002);
    xcvBuildSpiCmd (confCmd, 0x05, 0x36017);
    xcvBuildSpiCmd (confCmd, 0x3f, 0x00000);
    xcvBuildSpiCmd (confCmd, 0x1b, 0x37ccf); //decrease auxclk amplitude

    hal_RfspiSendCmd(confCmd);
    /*
        // ;=====Rx Setting2=====
    #if 0
        xcvBuildSpiCmd (confCmd, 0x3B, 0x065df); // DC1=fast+bw011
    #else
    #ifdef XCV_DC_CANCEL_OFF
        xcvBuildSpiCmd (confCmd, 0x29, 0x18c00); //  DC1=bypass; DC2=bypass; BW_sel=001
        xcvBuildSpiCmd (confCmd, 0x3B, 0x1addf); //  DC1=fast+bw011; Edge TX Equfil Setting:Equfil_cheby_k2
    #else // !XCV_DC_CANCEL_OFF
        xcvBuildSpiCmd (confCmd, 0x29, 0x08c00); //  DC1=enabled; DC2=bypass; BW_sel=001
        xcvBuildSpiCmd (confCmd, 0x3B, 0x1ab90); //  DC1=fast->hold+bw010; Edge TX Equfil Setting:Equfil_cheby_k2
    #endif // !XCV_DC_CANCEL_OFF
    #endif
        xcvBuildSpiCmd (confCmd, 0x3F, 0x00001); // page=1
        xcvBuildSpiCmd (confCmd, 0x75, 0x01317); // DigRF_slew_bit=111
        xcvBuildSpiCmd (confCmd, 0x3F, 0x00000); // page=0
    #ifdef XCV_DC_CANCEL_OFF
        xcvBuildSpiCmd (confCmd, 0x13, 0x0761f); // 10bit=1; Rx_DigRF_rxtxen_Delay=00001111(15-symbol Delay)
    #else // !XCV_DC_CANCEL_OFF
        xcvBuildSpiCmd (confCmd, 0x13, 0x07633); // 10bit=1; Rx_DigRF_rxtxen_Delay=0x19(25-symbol Delay)
    #endif // !XCV_DC_CANCEL_OFF
        xcvBuildSpiCmd (confCmd, 0x38, 0x10002); // Rx_Sample_mode=00: 16Bit/sample and 2sample/symbol
        hal_RfspiSendCmd(confCmd);

        // add for High and low temperature testing.   Modify by xianmiao
        xcvBuildSpiCmd (confCmd, 0x11, 0x251c1); // edit110505 XM
        xcvBuildSpiCmd (confCmd, 0x3F, 0x00001); // edit110505 XM
        xcvBuildSpiCmd (confCmd, 0x37, 0x10088); // edit110505 XM: Thermo_diff_enable
        xcvBuildSpiCmd (confCmd, 0x30, 0x00000); // edit110505 XM: Thermo_diff_setting1
        xcvBuildSpiCmd (confCmd, 0x31, 0x00088); // edit110505 XM: Thermo_diff_setting2
        xcvBuildSpiCmd (confCmd, 0x3F, 0x00000); // edit110505 XM
        xcvBuildSpiCmd (confCmd, 0x1C, 0x1909C); // edit110505 XM: Thermo_dac_setting
        hal_RfspiSendCmd(confCmd);

    #ifdef FPGA
        // ;=====FPGA Clock Buffer=====
        xcvBuildSpiCmd (confCmd, 0x1B, 0x2fcc0); //
    #else
        //xcvBuildSpiCmd (confCmd, 0x1A, 0x146ff); //
        //xcvBuildSpiCmd (confCmd, 0x1B, 0x2e5c0); //
        xcvBuildSpiCmd (confCmd, 0x1A, 0x146f8); //
        xcvBuildSpiCmd (confCmd, 0x1B, 0x2e000); //
    #endif
        xcvBuildSpiCmd (confCmd, 0x0E, 0x14384); // mdll
        xcvBuildSpiCmd (confCmd, 0x11, 0x251c1); // mdll
        hal_RfspiSendCmd(confCmd);

    #if 0 // Debug: Read out settings
        xcvReadReg (0x0, (UINT8 *)&g_tmp5);
        RFD_TRACE(TSTDOUT,0,"First read Reg 0x00 = 0x%x",g_tmp5);
    #endif
    */

    hal_ClkDisable(xcvClk);
}

VOID rfd_XcvClose(VOID)
{
    // Nothing to be done for the Xcver...
    hal_RfspiClose();
}

VOID RFD_FUNC_INTERNAL rfd_XcvWakeUp (VOID)
{
    if (g_lastSleepMode == RFD_POWER_OFF)
    {
        HAL_RFSPI_CMD_T* confCmd = hal_RfspiNewSingleCmd();
        UINT16 adjustedCdac = g_CDAC + g_CdacDelta;
        xcvBuildSpiCmd (confCmd, RDA6220_CDAC_REG, CDAC_REG_VALUE_BASE | ((adjustedCdac&0xff) << 0));
        hal_RfspiSendCmd(confCmd);

        g_lastSleepMode = RFD_POWER_ON;
    }
}

// *** xcvSleep (bool toDeepPD) ***
VOID RFD_FUNC_INTERNAL rfd_XcvSleep (RFD_LP_MODE_T lp_mode)
{
    // HAL_RFSPI_CMD_T* confCmd = hal_RfspiNewSingleCmd();
    g_lastSleepMode = lp_mode;

    if(RFD_LOW_POWER == lp_mode)
    {
        hal_TcuClrTco(HAL_TCU_TCO_RX_DEC_ON);
        hal_TcuClrTco(XCV->PDN);
    }
    /*
        else if(RFD_POWER_OFF== lp_mode)
        {
            // Set DigitalAfc, 14 bit is set for digital AFC
           g_xcvCmdCtx.digAfc = 0x04000 | (g_afcDacVal & RDA6220_AFC_MASK);

           xcvBuildSpiCmd (confCmd, 0x3F, 0x00000);
        xcvBuildSpiCmd (confCmd, RDA6220_AFC_REG, g_xcvCmdCtx.digAfc);

        }
    */

}

//======================================================================
// rfd_XcvGetAfcDacOffset
// This function used to get the difference between current g_afcDacVal and AFC_MID
//----------------------------------------------------------------------
PUBLIC INT16 rfd_XcvGetAfcDacOffset(VOID)
{
    return g_afcDacVal - RDA6220_AFC_MID;
}

//======================================================================
// rfd_XcvGetCdacOffset
// This function used to get current CDAC offset
//----------------------------------------------------------------------
PUBLIC INT16 rfd_XcvGetCdacOffset(VOID)
{
    return g_CdacDelta;
}

//======================================================================
// rfd_XcvSetAfcDacOffset
// This function used to set g_afcDacVal
//----------------------------------------------------------------------
PUBLIC VOID rfd_XcvSetAfcDacOffset(INT16 offset)
{
    g_afcDacVal = RDA6220_AFC_MID + offset;
}

//======================================================================
// rfd_XcvSetCdacOffset
// This function used to set CDAC offset
//----------------------------------------------------------------------
PUBLIC VOID rfd_XcvSetCdacOffset(INT16 offset)
{
    g_CdacDelta = offset;
}

//======================================================================
// rfd_XcvFrameInit
// This function used to inform XCV driver that FINT is initialized
//----------------------------------------------------------------------
PUBLIC VOID rfd_XcvFrameInit(VOID)
{
}

//======================================================================
// rfd_XcvFrameStart
// This function used to inform XCV driver that an FINT just starts
//----------------------------------------------------------------------
PUBLIC VOID RFD_FUNC_INTERNAL rfd_XcvFrameStart(VOID)
{
}

#ifdef ENABLE_BT_CLK_REQ_ON_DEMAND
VOID rfd_XcvEnableBtClkReq(BOOL enable)
{

}
#endif

BOOL rfd_XcvCaluChipTemperature(INT8 *tp)
{
    BOOL result = FALSE;

    *tp = 20;
    return result;
}
//----------------------//
// Calibration Related  //
//----------------------//

#define CALIB_DONE      0
#define CALIB_NOTDONE   1
#define CALIB_ERROR     0xFF
// State for Xtal calibration
enum {GUESS_CDAC,SEARCH_CDAC,DONE_CDAC};
UINT8 xtal_state = GUESS_CDAC;
// State of the iloss calibration
BOOL g_rfdILossInit = TRUE;

const CALIB_XCV_T xcv_calib_default =
{
    DEFAULT_CALIB_TIMES,
    DEFAULT_CALIB_PARAM,
    DEFAULT_CALIB_PALCUST
};

const CALIB_XCV_T * rfd_XcvCalibGetDefault(VOID)
{
    return &xcv_calib_default;
}

VOID rfd_XcvCalibUpdateValues(VOID)
{
    return;
}

VOID rfd_XcvCalibResetValues(VOID)
{
    // When the current CDAC is different from the init CDAC value,
    // we reset the CDAC value.
    HAL_RFSPI_CMD_T* spiCmd = hal_RfspiNewSingleCmd();
    if (g_CDAC != RDA6220_CDAC_DEFAULT)
    {
        // In this case, we reset the CDAC to default value.

        // Xtal triming capacitor register to default (calibrated)
        g_CDAC = RDA6220_CDAC_DEFAULT;
        UINT32 bitfield = CDAC_REG_VALUE_BASE | ((g_CDAC&0xff) << 0);
        xcvBuildSpiCmd (spiCmd, RDA6220_CDAC_REG, bitfield);
    }
    rfd_xcvRamp0Set(spiCmd);
    rfd_xcvRamp1Set(spiCmd);
    rfd_xcvRamp2Set(spiCmd);
    rfd_xcvRamp3Set(spiCmd);

    // Reset Xtal calib state
    xtal_state = GUESS_CDAC;
    // Reset iloss calib state
    g_rfdILossInit = TRUE;
    return;
}

UINT8 rfd_XcvCalibXtalFreqOffset(INT32 FOf)
{

// 1 CDAC unit =~ 340Hz
#define CDAC_UNIT 300
#define FIRST_FOF 0x7FFF
#define OPPOSITE_SIGN(n,m) (((n>0)&&(m<0))||((m>0)&&(n<0)))
//#define ABS(x)   (((x)>0)?(x):-(x))

    static INT16 last_FOf = FIRST_FOF;
    static UINT8 last_Cdac = 0;

    HAL_RFSPI_CMD_T *spiCmd;

    switch (xtal_state)
    {
        case GUESS_CDAC:
            last_FOf = FIRST_FOF;
            last_Cdac = g_CDAC;

            g_CDAC -= (FOf / CDAC_UNIT);
            xtal_state = SEARCH_CDAC;

            // Check CDAC boundaries
            if ((g_CDAC > RDA6220_CDAC_MAX)||(g_CDAC < RDA6220_CDAC_MIN))
            {
                //xcv_fprintf((RFD_INFO_TRC,"xcvCDAC @ %d Qb",g_CDAC));
                return CALIB_ERROR;
            }
            break;

        case SEARCH_CDAC:
            // Check if we changed sign
            if (OPPOSITE_SIGN(FOf,last_FOf) && (last_FOf != FIRST_FOF))
            {
                // We have the good CDAC
                if (ABS(FOf) < ABS(last_FOf))
                {
                    XCV_PARAM(CDAC_FREQ) = (INT32)FOf;
                    if (g_xcvCalibPtr->hstXcv != NULL)
                        HST_XCV_PARAM(CDAC_FREQ) = (INT32)FOf;
                    return CALIB_DONE;
                }
                else // Last CDAC was the good one
                {
                    XCV_PARAM(CDAC_FREQ) = (INT32)last_FOf;
                    if (g_xcvCalibPtr->hstXcv != NULL)
                        HST_XCV_PARAM(CDAC_FREQ) = (INT32)last_FOf;
                    g_CDAC = last_Cdac;
                    xtal_state = DONE_CDAC;
                }
            }
            else // still searching...
            {
                last_Cdac = g_CDAC;
                if (FOf < 0) g_CDAC++;
                else g_CDAC--;

                // Check CDAC boundaries
                if ((g_CDAC > RDA6220_CDAC_MAX)||(g_CDAC < RDA6220_CDAC_MIN))
                    return CALIB_ERROR;
            }
            last_FOf = FOf;
            break;
    }

    // Program CDAC change early next frame
    //RFD_TRACE(TSTDOUT,0,"XTAL xcvCDAC @ %d Qb",g_CDAC);
    spiCmd = hal_RfspiNewCmd(0,0);
    UINT32 bitfield = CDAC_REG_VALUE_BASE | ((g_CDAC&0xff) << 0);
    xcvBuildSpiCmd (spiCmd, RDA6220_CDAC_REG, bitfield);

    // Update calibration structure
    XCV_PARAM(XTAL) = g_CDAC;
    if (g_xcvCalibPtr->hstXcv != NULL)
        HST_XCV_PARAM(XTAL) = g_CDAC;

    // We can stop right now are we are sure that at least one
    // FINT will happen util the stub goes to another task, so
    // the CDAC value will be changed.
    if (xtal_state == DONE_CDAC) return CALIB_DONE;

    return CALIB_NOTDONE;
}

VOID rfd_XcvCalibSetAfcBound(RFD_AFC_BOUND_T bound)
{
    HAL_RFSPI_CMD_T *spiCmd = hal_RfspiNewCmd(0,0);

    if (RFD_AFC_BOUND_CENTER == bound)
        g_afcDacVal = RDA6220_AFC_MID;
    else if (RFD_AFC_BOUND_POS == bound)
        g_afcDacVal = RDA6220_AFC_BOUND_HIGH;
    else
        g_afcDacVal = RDA6220_AFC_BOUND_LOW;

    // Set DigitalAfc, 14 bit is set for digital AFC
    g_xcvCmdCtx.digAfc = 0x04000 | (g_afcDacVal & RDA6220_AFC_MASK);
    xcvBuildSpiCmd (spiCmd, RDA6220_AFC_REG, g_xcvCmdCtx.digAfc);
    g_CdacDelta = 0;
//#endif
}

VOID rfd_XcvCalibCalcAfcGain(INT32 FreqOffset, GSM_RFBAND_T band)
{
    INT32 gain = (FreqOffset * 1000) / (2 * RDA6220_AFC_HRANGE);

    // Update calibration structure
    XCV_PARAM(AFC_GAIN(band)) = gain;
    if (g_xcvCalibPtr->hstXcv != NULL)
        HST_XCV_PARAM(AFC_GAIN(band)) = gain;
}

VOID rfd_XcvCalibSetChangeDAC(void)
{
    HAL_RFSPI_CMD_T *spiCmd = hal_RfspiNewCmd(0,0);
    //hal_HstSendEvent(g_CDAC);
    UINT32 bitfield = CDAC_REG_VALUE_BASE | (((g_CDAC+RDA6220_CDAC_OFFSET)&0xff) << 0);
    xcvBuildSpiCmd (spiCmd, RDA6220_CDAC_REG, bitfield);
    //hal_HstSendEvent(g_CDAC+RDA6220_CDAC_OFFSET);
    g_CdacDelta = RDA6220_CDAC_OFFSET;
}

VOID rfd_XcvCalibSetAfcBoundChangeDAC(RFD_AFC_BOUND_T bound)
{
    HAL_RFSPI_CMD_T *spiCmd = hal_RfspiNewCmd(0,0);
    rfd_XcvCalibSetChangeDAC();
    //hal_HstSendEvent(0xca11afc);

    if (RFD_AFC_BOUND_CENTER == bound)
        g_afcDacVal = RDA6220_AFC_MID;
    else if (RFD_AFC_BOUND_POS == bound)
        g_afcDacVal = RDA6220_AFC_BOUND_HIGH;
    else
        g_afcDacVal = RDA6220_AFC_BOUND_LOW;

    // Set DigitalAfc, 14 bit is set for digital AFC
    g_xcvCmdCtx.digAfc = 0x04000 | (g_afcDacVal & RDA6220_AFC_MASK);
    xcvBuildSpiCmd (spiCmd, RDA6220_AFC_REG, g_xcvCmdCtx.digAfc);
}

VOID rfd_XcvCalibCalcAfcGainChangeDAC(INT32 FreqOffset, GSM_RFBAND_T band)
{
    INT32 gain = (FreqOffset * 1000) / (2 * RDA6220_AFC_HRANGE);

    // Update calibration structure
    XCV_PARAM(AFC_GAIN2(band)) = gain;
    if (g_xcvCalibPtr->hstXcv != NULL)
        HST_XCV_PARAM(AFC_GAIN2(band)) = gain;
    g_CdacDelta = 0;
}

VOID rfd_XcvCalibCalcAfcFreqChangeDAC(INT32 FreqOffset, GSM_RFBAND_T band)
{
    // Update calibration structure
    XCV_PARAM(AFC_FREQ(band)) = FreqOffset;
    if (g_xcvCalibPtr->hstXcv != NULL)
        HST_XCV_PARAM(AFC_FREQ(band)) = FreqOffset;
}

// ILoss ARFCN Table
PRIVATE UINT16 g_rfdILossArfcnTab850[6] = RFD_DEFAULT_CALIB_ILOSS_ARFCN_850;
PRIVATE UINT16 g_rfdILossArfcnTabGSM[6] = RFD_DEFAULT_CALIB_ILOSS_ARFCN_GSM;
PRIVATE UINT16 g_rfdILossArfcnTabDCS[9] = RFD_DEFAULT_CALIB_ILOSS_ARFCN_DCS;
PRIVATE UINT16 g_rfdILossArfcnTabPCS[9] = RFD_DEFAULT_CALIB_ILOSS_ARFCN_PCS;

PRIVATE UINT8    g_rfdILossNum;
PRIVATE UINT16*  g_rfdTmpArfcn;

PRIVATE UINT32   g_rfdTmpOffset;

UINT8 rfd_XcvCalibSetILoss(INT8 offset, GSM_RFBAND_T band, UINT16* nextArfcn)
{
// First call of this function after the restart of the system or after
// after a call to the xcv_CalibResetValues function.


    if (TRUE == g_rfdILossInit)
    {
        // iloss calibration initialization. Choose the band which need calibration.
        g_rfdTmpArfcn = (band == GSM_BAND_GSM850 )? g_rfdILossArfcnTab850:
                        (band == GSM_BAND_DCS1800)? g_rfdILossArfcnTabDCS:
                        (band == GSM_BAND_PCS1900)? g_rfdILossArfcnTabPCS:
                        g_rfdILossArfcnTabGSM;

        if(GSM_BAND_DCS1800 == band)
            XCV_PARAM(ILOSS(band)) = 0x44444444;
        else if(GSM_BAND_PCS1900 == band)
            XCV_PARAM(ILOSS(band)) = 0x55555555;
        else
            XCV_PARAM(ILOSS(band)) = 0x33333;
        g_rfdTmpOffset = 0;
        g_rfdILossNum = 0;
        g_rfdILossInit = FALSE;
        *nextArfcn = g_rfdTmpArfcn[g_rfdILossNum];
        return CALIB_PROCESS_CONTINUE;
    }

    offset += ((XCV_PARAM(ILOSS(band)) >> (4*g_rfdILossNum)) & 0xF);
    g_rfdTmpOffset |= (offset << (4*g_rfdILossNum));
    *nextArfcn = g_rfdTmpArfcn[++g_rfdILossNum];

    // Judge the sign of iloss calibration.
    if(*nextArfcn == 0)
    {
        g_rfdILossInit = TRUE;

        XCV_PARAM(ILOSS(band)) = g_rfdTmpOffset;
        if (g_xcvCalibPtr->hstXcv != NULL)
            HST_XCV_PARAM(ILOSS(band)) = g_rfdTmpOffset;

        return CALIB_PROCESS_STOP;
    }

    return CALIB_PROCESS_CONTINUE;
}

PUBLIC VOID rfd_XcvCalibSetPaRamp(UINT16* dacValue)
{
    g_xcvCalibSetPaRamp = TRUE;
    for(int i = 0; i < 4; i++)
        g_xcvCalibPaRampValue[i] = dacValue[i];
}

