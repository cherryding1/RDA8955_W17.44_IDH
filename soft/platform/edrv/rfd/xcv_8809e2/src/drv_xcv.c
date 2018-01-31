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
#include "hal_lps.h"
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
#ifdef RF_LDO_MODE
#include "pmd_m.h"
#endif
#include "calib_m.h"


#if defined(_FLASH_PROGRAMMER) || defined(_T_UPGRADE_PROGRAMMER)
#undef SPI_REG_DEBUG
#undef FM_LDO_WORKAROUND
#endif


#define XCV_AFC_COMPATIBLE_WITH_OLD_CALIB_DATA 1

#define XCV_624M_PLL_DITHER_ENABLE 0


#define XCV_DIGRF_RX_CLK_POL                     RFD_DIGRF_NORM_CLK_POL
#define XCV_SAMPLE_WIDTH                                             12
#define XCV_ADC2DBM_GAIN                                             50

#define XCV_SET_PDN                                   SET_TCO(XCV->PDN)
#define XCV_CLR_PDN                                   CLR_TCO(XCV->PDN)

#define CDAC_REG_VALUE_BASE (0x011)

#define MAX_PIGGYBACK_WAIT_COUNT 3
#define RX_PLL_FREQ_REG_DCDC_ON ((1<<17)|(1<<15))
#define RX_PLL_FREQ_REG_LDO_ON ((1<<16)|(1<<14))

// Forward declaration of internal functions
VOID xcvBuildSpiCmd (HAL_RFSPI_CMD_T *cmd, UINT8 address, UINT32 data);
VOID xcvSetArfcn (RFD_RFDIR_T dir, GSM_RFBAND_T band, UINT16 arfcn);
VOID xcvReadReg (UINT16 address, UINT8 *output);

PROTECTED VOID rfd_XcvRegForceWriteSingle(UINT32 addr, UINT32 data);

INLINE VOID xcvConfigure(VOID);
CONST XCV_CONFIG_T* RFD_DATA_INTERNAL XCV = NULL;
#ifdef XCV_RAMP_DEBUG
BOOL RFD_DATA_INTERNAL m_ParampDebugflag = FALSE;
#endif

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
#ifdef USE_BB_DC_CANCEL
INT32 RFD_DATA_INTERNAL g_dcoffsetflag = 0;
#endif
UINT16 RFD_DATA_INTERNAL g_afcDacVal = RDA6220_AFC_MID;
INT16 RFD_DATA_INTERNAL rxTxTimeOffset = 0;
INT16 RFD_DATA_INTERNAL g_CdacDelta = 0;
RFD_LP_MODE_T RFD_DATA_INTERNAL g_lastSleepMode = RFD_POWER_OFF;

PROTECTED CALIB_CALIBRATION_T* RFD_DATA_INTERNAL g_xcvCalibPtr     = 0 ;

CONST XCV_RX_GAIN_T AgcTab_EGSM[AGC_QTY]       = EGSM_AGC_DEFAULT;
CONST XCV_RX_GAIN_T AgcTab_DCS[AGC_QTY]        = DCS_AGC_DEFAULT;
#ifdef CHANNEL_FREQUENCY_MODE
CONST XCV_FREQUENCY_T Gsm850_Tx_Fre[GSM850_ARFCN_NUM] = GSM850_TX_FREQUENCY;
CONST XCV_FREQUENCY_T Gsm850_Rx_Fre[GSM850_ARFCN_NUM] = GSM850_RX_FREQUENCY;
CONST XCV_FREQUENCY_T Gsm900_Tx_Fre[GSM900_ARFCN_NUM] = GSM900_TX_FREQUENCY;
CONST XCV_FREQUENCY_T Gsm900_Rx_Fre[GSM900_ARFCN_NUM] = GSM900_RX_FREQUENCY;

CONST XCV_FREQUENCY_T Dcs_Tx_Fre[DCS_ARFCN_NUM] = DCS_TX_FREQUENCY;
CONST XCV_FREQUENCY_T Dcs_Rx_Fre[DCS_ARFCN_NUM] = DCS_RX_FREQUENCY;
CONST XCV_FREQUENCY_T Pcs_Tx_Fre[PCS_ARFCN_NUM] = PCS_TX_FREQUENCY;
CONST XCV_FREQUENCY_T Pcs_Rx_Fre[PCS_ARFCN_NUM] = PCS_RX_FREQUENCY;
#endif
#ifdef XCV_OOK_MODE
CONST XCV_OOKMODE_FREQUENCY_T GsmOokMode_Tx_Fre[GSMOOKMODE_ARFCN_NUM] = OOKMODE_TX_FREQUENCY;
#endif
// RF Commands Context
XCV_COMMANDS_T RFD_DATA_INTERNAL g_xcvCmdCtx = {0, 0, 0};

BOOL RFD_DATA_INTERNAL g_xcvCalibSetPaRamp = FALSE;
BOOL RFD_DATA_INTERNAL g_xcvCalibPaOffset = TRUE;
UINT16 RFD_DATA_INTERNAL g_xcvCalibPaRampValue[4];

#ifdef TXPOWER_TEMPERATURE_OFFSET
UINT16 RFD_DATA_INTERNAL g_tempgpadcvalue[7] = {0,};
#endif
UINT32 RFD_DATA_INTERNAL g_RxPllFreqReg_05h = 0x3a654;
VOLATILE BOOL g_xcvRfIniting = FALSE;
#ifdef RF_LDO_MODE
VOLATILE UINT32 g_xcvDcdcLdoModeReqStatus = 0;
#else
#ifdef FM_LDO_WORKAROUND
VOLATILE UINT32 g_xcvDcdcLdoModeReqStatus = 0;
#endif
#endif

#if (XCV_624M_PLL_DITHER_ENABLE)
BOOL RFD_DATA_INTERNAL g_Xcv624MPllDitherEnabled = FALSE;
#endif

PRIVATE BOOL RFD_DATA_INTERNAL g_XcvLimitMaxTxPower = FALSE;
#ifdef XCV_PLL_SDM_MODE
PRIVATE UINT8 RFD_DATA_INTERNAL g_XcvRxSdmMode = 0x20;
#endif

// -------------------------------------------------------------------//
//                      Macros for SPI timings                        //
// -------------------------------------------------------------------//
#define RFSPI_CLOCK_19_5M

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

#ifdef TXPOWER_TEMPERATURE_OFFSET
INT16 RFD_FUNC_INTERNAL PowerOffset(GSM_RFBAND_T band, UINT16 dacvalue,UINT8 upordown)
{
    if(g_xcvCalibPaOffset)
        return 0;
    INT16 ret_value = 0;
    if(upordown)
    {

#ifdef PASW_HS8292U
        static INT8 m_Rxleveloffset[4][11] =
        {
            {1,1,0,0,0,-1,-1,-1,0,0,-1}
            ,{1,1,0,0,0,-1,-1,-1,0,0,-1}
            ,{1,1,1,1,0,0,0,-1,1,0,0}
            ,{1,0,0,0,0,0,-1,-1,0,1,-1}
        };
#else
        static INT8 m_Rxleveloffset[4][11] =
        {
            {1,1,1,0,0,0,0,0,0,0,0}
            ,{1,1,1,0,0,0,0,0,0,0,0}
            ,{0,0,0,0,0,0,0,0,0,0,0}
            ,{0,0,0,0,0,0,0,0,0,0,0}
        };
#endif
        if(dacvalue > 991 && dacvalue < 1390)
        {
            ret_value = m_Rxleveloffset[band][10];
        }
        else if(dacvalue > 961 && dacvalue < 992)
        {
            ret_value = m_Rxleveloffset[band][10];
        }
        else if(dacvalue > 911 && dacvalue < 962)
        {
            ret_value = m_Rxleveloffset[band][9];
        }
        else if(dacvalue > 851 && dacvalue < 912)
        {
            ret_value = m_Rxleveloffset[band][8];
        }
        else if(dacvalue > 751 && dacvalue < 852)
        {
            ret_value = m_Rxleveloffset[band][7];
        }
        else if(dacvalue > 651 && dacvalue < 752)
        {
            ret_value = m_Rxleveloffset[band][6];
        }
        else if(dacvalue > 551 && dacvalue < 652)
        {
            ret_value = m_Rxleveloffset[band][5];
        }
        else if(dacvalue > 451 && dacvalue < 552)
        {
            ret_value = m_Rxleveloffset[band][4];
        }
        else if(dacvalue > 331 && dacvalue < 452)
        {
            ret_value = m_Rxleveloffset[band][3];
        }
        else if(dacvalue > 261 && dacvalue < 332)
        {
            ret_value = m_Rxleveloffset[band][2];
        }
        else if(dacvalue > 201 && dacvalue < 262)
        {
            ret_value = m_Rxleveloffset[band][1];
        }
        else if(dacvalue > 0 && dacvalue < 202)
        {
            ret_value = m_Rxleveloffset[band][0];
        }
        else
        {
            ret_value = 0;
        }
    }
    else
    {
#ifdef PASW_HS8292U
        static INT32 m_Poweroffset[4][2] =
        {
            {-1201,613021}
            ,{-1327,690708}
            ,{-3211,1713236}
            ,{-3312,2076586}
        };
#else
        static INT32 m_Poweroffset[4][2] =
        {
            {-1327,690708}
            ,{-1327,690708}
            ,{-1044,839164}
            ,{-1044,839164}
        };
#endif
        if(dacvalue > 71 && dacvalue < 1390)
        {
            ret_value = (m_Poweroffset[band][0] * dacvalue + m_Poweroffset[band][1])/10000;
        }
        else
            ret_value = 0;
    }
    return ret_value;
}
#endif
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
#ifdef CALIB_USE_0207
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
#else
    if (band == GSM_BAND_GSM850)
    {
        pcl2dbm_o_arfcn = (CALIB_ARFCN_T *)
                          g_xcvCalibPtr->pa->palcust.pcl2dbmArfcnG850;
        arfcn_s -= RFD_ARFCN_GSM850_MIN;
        nb_arfcn_in_band = RFD_ARFCN_GSM850_MAX - RFD_ARFCN_GSM850_MIN;
        if (Pcl < 5)
            Pcl = 5;
        else if (Pcl > 19)
            Pcl = 19;
        Pcl -= 5;
    }
    else if(band == GSM_BAND_GSM900)
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
        nb_arfcn_in_band = RFD_ARFCN_EGSM_RX_MAX1 - RFD_ARFCN_EGSM_RX_MIN1;

        if (Pcl < 5)
            Pcl = 5;
        else if (Pcl > 19)
            Pcl = 19;
        Pcl -= 5;
    }
#endif
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

    // Choose different MDLL divider number for some Arfcn at TX because of Modulation Sqectrum
    UINT32 freqReg = 0x18000; //divider=9
    if (dir == RFD_DIR_TX)
    {
        switch (band)
        {
            case GSM_BAND_GSM850:
                if (arfcn > 244 && arfcn < 252)
                {
                    freqReg = 0x14000;
                }
                break;
            case GSM_BAND_GSM900:
                if (arfcn > 79 && arfcn < 89)
                {
                    freqReg = 0x14000;
                }
                else if (arfcn == 996)
                {
                    freqReg = 0x14000;
                }
                break;
            case GSM_BAND_DCS1800:
                if (arfcn > 731 && arfcn < 748)
                {
                    freqReg = 0x14000;
                }
                break;
            case GSM_BAND_PCS1900:
                if (arfcn > 609 && arfcn < 626)
                {
                    freqReg = 0x1c000; //divider=10
                }
                break;
            default:
                break;
        }
    }
    else if (dir == RFD_DIR_RX)
    {
        freqReg = 0x10000; //divider=7
#ifdef XCV_PLL_SDM_MODE
        g_XcvRxSdmMode = 0;
#endif
        switch (band)
        {
            case GSM_BAND_GSM850:
                break;
            case GSM_BAND_GSM900:
                break;
            case GSM_BAND_DCS1800:
                break;
            case GSM_BAND_PCS1900:
                if (arfcn==611)
                {
                    freqReg = 0x14000; //divider=8
#ifdef XCV_PLL_SDM_MODE
                    g_XcvRxSdmMode = 1;
#endif
                }
                break;
            default:
                break;
        }
    }
    g_xcvCmdCtx.freqCtrl = (freqReg | selBand | rxTxMode | (arfcn & RDA6220_ARFCN_MASK));
}

INLINE
UINT8 RFD_FUNC_INTERNAL xcvSetRxGain (UINT8 gain, GSM_RFBAND_T band, UINT16 arfcn)
{
    UINT8 iLoss = 0;
    UINT8 XcvGain = 0;
    UINT8 GainIdx;
    CONST XCV_RX_GAIN_T* agcTable;
    agcTable=((band == GSM_BAND_GSM850)|| (band == GSM_BAND_GSM900))?
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
                iLoss = (XCV_PARAM(ILOSS(band)) >> 16)& 0xf;
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

#ifdef TXPOWER_TEMPERATURE_OFFSET
    iLoss += PowerOffset(band, g_tempgpadcvalue[6],1);
#endif
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

    g_xcvCmdCtx.gain = 0x20000 |
                       ( (agcTable[GainIdx].analogGain & 0x7)<<14) |
                       (agcTable[GainIdx].digGain & 0xff);

    return (XcvGain - iLoss);
}
#ifdef USE_BB_DC_CANCEL
UINT8 RFD_FUNC_INTERNAL xcvGetDigGain(UINT8 gainIndex)
{
    UINT8 diggain = 0;
    UINT8 gain[7] = {0, 1, 2, 4, 6, 12, 24};
    UINT8 index = 6;
    while(index)
    {
        if(gainIndex & (1 << index))
        {
            diggain += gain[index];
        }
        index--;
    }
    return (diggain);
}
#endif
//--------------------------------------------------------------------//
//                                                                    //
//                    Internal Functions                              //
//                                                                    //
//--------------------------------------------------------------------//

// *** xcvBuildRxSpiCmd (UINT16 arfcn, UINT8 gain) ***
// Prepare the transceiver commands for reception
INLINE
#ifdef USE_BB_DC_CANCEL
UINT8 RFD_FUNC_INTERNAL xcvBuildRxSpiCmd(RFD_WIN_T *Win, HAL_RFSPI_CMD_T *spiCmd, INT16* I_Dcoffset,INT16* Q_Dcoffset,UINT8* xcvAnaLevel,UINT8* xcvDigLevel)
#else
UINT8 RFD_FUNC_INTERNAL xcvBuildRxSpiCmd(RFD_WIN_T *Win, HAL_RFSPI_CMD_T *spiCmd)
#endif
{
    UINT8 XcvGain;
#ifdef XCV_PLL_SDM_MODE
    INT32 m_mdllparam[4] = {339,387,436,484};
#endif
    // Set Gain
    XcvGain = xcvSetRxGain (Win->gain[0], Win->band, Win->arfcn);
#ifdef CHANNEL_FREQUENCY_MODE
    UINT32 reg_05h = 0;
    UINT32 reg_06h = 0;
    switch(band)
    {
        case GSM_BAND_GSM850:
            reg_05h = Gsm850_Rx_Fre[Win->arfcn-128].fiveregvalue & 0x3fff;
            reg_06h = Gsm850_Rx_Fre[Win->arfcn-128].sixregvalue & 0x3ffff;
            band = GSM_BAND_GSM900;
            break;
        // GSM uses 5 points
        case GSM_BAND_GSM900:
            if(arfcn <=124)
            {
                reg_05h = Gsm900_Rx_Fre[Win->arfcn].fiveregvalue & 0x3fff;
                reg_06h = Gsm900_Rx_Fre[Win->arfcn].sixregvalue & 0x3ffff;
            }
            else
            {
                reg_05h = Gsm900_Rx_Fre[Win->arfcn-850].fiveregvalue & 0x3fff;
                reg_06h = Gsm900_Rx_Fre[Win->arfcn-850].sixregvalue & 0x3ffff;
            }
            break;
        // DCS uses 8 points
        case GSM_BAND_DCS1800:
            reg_05h = Dcs_Rx_Fre[Win->arfcn-512].fiveregvalue & 0x3fff;
            reg_06h = Dcs_Rx_Fre[Win->arfcn-512].sixregvalue & 0x3ffff;
            break;
        // PCS uses 8 points
        case GSM_BAND_PCS1900:
        default:
            reg_05h = Pcs_Rx_Fre[Win->arfcn-512].fiveregvalue & 0x3fff;
            reg_06h = Pcs_Rx_Fre[Win->arfcn-512].sixregvalue & 0x3ffff;
            band = GSM_BAND_DCS1800;
            break;
    }
    g_RxPllFreqReg_05h = (g_RxPllFreqReg_05h & ~0x3fff) | reg_05h;
    xcvBuildSpiCmd (spiCmd, 0x05, g_RxPllFreqReg_05h);
    xcvBuildSpiCmd (spiCmd, 0x06, reg_06h);
#elif defined(FM_LDO_WORKAROUND)
    if (g_xcvDcdcLdoModeReqStatus == 1)
    {
        // Add dcdc piggyback commands
        xcvBuildSpiCmd (spiCmd, 0x05, g_RxPllFreqReg_05h);
        // Dcdc piggyback commands have been saved in queue
        g_xcvDcdcLdoModeReqStatus = 2;
    }
#endif

    // Set arfcn & band
    xcvSetArfcn (RFD_DIR_RX, Win->band, Win->arfcn);

    // Set DigitalAfc, 14 bit is set for digital AFC
    g_xcvCmdCtx.digAfc = 0x14000 | (g_afcDacVal & RDA6220_AFC_MASK);

#ifdef MULTI_BANDWIDE
    UINT32 reg_12h, reg_13h, reg_29h;

    if(Win->bw == 1)
    {
        g_xcvCalibPtr->xcv->palcust.rxTxTimeOffset = rxTxTimeOffset - 1;
        reg_12h = 0x110f0;
        reg_13h = 0x01241;
#ifdef USE_BB_DC_CANCEL
        reg_29h = 0x0a400|g_dcoffsetflag << 16;
#else
        reg_29h = 0x0a400;
#endif
    }
    else if(Win->bw == 2)
    {
        g_xcvCalibPtr->xcv->palcust.rxTxTimeOffset = rxTxTimeOffset - 4;
        reg_12h = 0x010f0;
        reg_13h = 0x01241;
#ifdef USE_BB_DC_CANCEL
        reg_29h = 0x0bc00|g_dcoffsetflag << 16;
#else
        reg_29h = 0x0bc00;
#endif
    }
    else
    {
        g_xcvCalibPtr->xcv->palcust.rxTxTimeOffset = rxTxTimeOffset - 5;
        reg_12h = 0x1f0f0;
        reg_13h = 0x07241;
#ifdef USE_BB_DC_CANCEL
        reg_29h = 0x08c00|g_dcoffsetflag << 16;
#else
        reg_29h = 0x08c00;
#endif
    }
    xcvBuildSpiCmd (spiCmd, 0x12, reg_12h);
    xcvBuildSpiCmd (spiCmd, 0x13, reg_13h);
    xcvBuildSpiCmd (spiCmd, 0x29, reg_29h);
#endif

    UINT16 adjustedCdac = g_CDAC + g_CdacDelta;
    xcvBuildSpiCmd (spiCmd, RDA6220_CDAC_REG, CDAC_REG_VALUE_BASE | ((adjustedCdac&RDA6220_CDAC_REG_MASK) << RDA6220_CDAC_REG_OFFSET));

    xcvBuildSpiCmd (spiCmd, 0x17, 0x03328); //
    xcvBuildSpiCmd (spiCmd, RDA6220_FREQ_CTRL_REG, g_xcvCmdCtx.freqCtrl);

    xcvBuildSpiCmd (spiCmd, RDA6220_AFC_REG, g_xcvCmdCtx.digAfc);
    //xcvBuildSpiCmd (spiCmd, 0x24, 0x20748); // PAON dr=1, PAON value=0
    //xcvBuildSpiCmd (spiCmd, RDA6220_FREQ_CTRL_MDLL_REG, 0x11f54); // MDLL=7/8
#ifdef XCV_PLL_SDM_MODE
    INT32 fof = Win->fof;
    if(fof < -8000)
        fof = -8000;
    else if(fof > 8000)
        fof = 8000;
    INT32 m_mdllFreq2dac = (INT32)(Win->band/2+1)*(fof* 1000)/(m_mdllparam[g_XcvRxSdmMode]);
    xcvBuildSpiCmd (spiCmd, 0x35, 0x30000|(m_mdllFreq2dac&0xffff));
#endif
#ifdef USE_BB_DC_CANCEL
#define MONDIGGAINVALUEMAX 10
    UINT8 anagain = ((g_xcvCmdCtx.gain)>>14)&0x7;
    UINT8 diggain = g_xcvCmdCtx.gain&0xff;
    UINT8 m_getdiggain = 0;
    *xcvAnaLevel = anagain;
    m_getdiggain = xcvGetDigGain(diggain);
    *xcvDigLevel = m_getdiggain;
    if(Win->type == GSM_WIN_TYPE_MONIT || Win->type == GSM_WIN_TYPE_MONIT_EXT)
    {
        if(m_getdiggain > MONDIGGAINVALUEMAX)
        {
            *xcvDigLevel = MONDIGGAINVALUEMAX;
            g_xcvCmdCtx.gain = (g_xcvCmdCtx.gain &0xffffff00)|0x17;
        }
    }
    xcvBuildSpiCmd (spiCmd, RDA6220_RX_GAIN_REG, g_xcvCmdCtx.gain);
    //*xcvDigLevel = xcvGetDigGain(diggain);
    if(I_Dcoffset[anagain] < -1023)
        I_Dcoffset[anagain] = -1023;
    else if(I_Dcoffset[anagain] > 1023)
        I_Dcoffset[anagain] = 1023;

    if(Q_Dcoffset[anagain] < -1023)
        Q_Dcoffset[anagain] = -1023;
    else if(Q_Dcoffset[anagain] > 1023)
        Q_Dcoffset[anagain] = 1023;
    UINT32 i_dctemp = I_Dcoffset[anagain];
    xcvBuildSpiCmd (spiCmd, 0x3f, 0x00001); //
    xcvBuildSpiCmd (spiCmd, 0x3b, (((i_dctemp&0x7f)<<10)|(Q_Dcoffset[anagain]&0x3ff))|0x20000);
    xcvBuildSpiCmd (spiCmd, 0x3a, ((I_Dcoffset[anagain]>>7)&0x7)|0x38);

    xcvBuildSpiCmd (spiCmd, 0x3f, 0x00000); //
#else
    xcvBuildSpiCmd (spiCmd, RDA6220_RX_GAIN_REG, g_xcvCmdCtx.gain);
#endif
#if (XCV_624M_PLL_DITHER_ENABLE)
    if (Win->band == GSM_BAND_GSM900 && (Win->arfcn == 5 || Win->arfcn == 70))
    {
        if (!g_Xcv624MPllDitherEnabled)
        {
            // switch to the page 4
            xcvBuildSpiCmd (spiCmd, 0x3f, 0x00004);
            xcvBuildSpiCmd (spiCmd, 0x07, 0x00c01); // ss_en=1;ss_squre=1
            xcvBuildSpiCmd (spiCmd, 0x0b, 0x00c01); // ss_en=1;ss_squre=1
            // back to page 0
            xcvBuildSpiCmd (spiCmd, 0x3f, 0x00000);
            g_Xcv624MPllDitherEnabled = TRUE;
        }
    }
    else
    {
        if (g_Xcv624MPllDitherEnabled)
        {
            // switch to the page 4
            xcvBuildSpiCmd (spiCmd, 0x3f, 0x00004);
            xcvBuildSpiCmd (spiCmd, 0x07, 0x00001); // ss_en=0;ss_squre=0
            xcvBuildSpiCmd (spiCmd, 0x0b, 0x00001); // ss_en=0;ss_squre=0
            // back to page 0
            xcvBuildSpiCmd (spiCmd, 0x3f, 0x00000);
            g_Xcv624MPllDitherEnabled = FALSE;
        }
    }
#endif // XCV_624M_PLL_DITHER_ENABLE

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

#ifdef USE_BB_DC_CANCEL
    return XcvGain -(m_getdiggain - *xcvDigLevel );
#else
    return XcvGain;
#endif

}

VOID xcv_SetStableVolt(UINT16 volt)
{
    g_xcvbattstablevolt = volt;
}
// *** xcvBuildTxSpiCmd  ***
// Prepare the transceiver commands for emission
//volatile int rampvalue = 0x00400;
INLINE
VOID RFD_FUNC_INTERNAL xcvBuildTxSpiCmd (RFD_WIN_T *Win, UINT16 arfcn, GSM_RFBAND_T band, HAL_RFSPI_CMD_T *spiCmd)
{

    UINT8 Winbitmap = (Win->bitmap & 0xf);
    UINT16 nbSlots = (Winbitmap & 0x08)? 4:
                     (Winbitmap & 0x4)? 3:
                     (Winbitmap & 0x2)? 2: 1;
    int count = 0;
    int pclValue = Win->gain[count];

#ifdef CHANNEL_FREQUENCY_MODE
    UINT32 reg_05h = 0;
    UINT32 reg_06h = 0;
    switch(band)
    {
        case GSM_BAND_GSM850:
            reg_05h = Gsm850_Tx_Fre[arfcn-128].fiveregvalue & 0x3fff;
            reg_06h = Gsm850_Tx_Fre[arfcn-128].sixregvalue & 0x3ffff;
            band = GSM_BAND_GSM900;
            break;
        // GSM uses 5 points
        case GSM_BAND_GSM900:
            if(arfcn <=124)
            {
                reg_05h = Gsm900_Tx_Fre[arfcn].fiveregvalue & 0x3fff;
                reg_06h = Gsm900_Tx_Fre[arfcn].sixregvalue & 0x3ffff;
            }
            else
            {
                reg_05h = Gsm900_Tx_Fre[arfcn-850].fiveregvalue & 0x3fff;
                reg_06h = Gsm900_Tx_Fre[arfcn-850].sixregvalue & 0x3ffff;
            }

            break;
        // DCS uses 8 points
        case GSM_BAND_DCS1800:
            reg_05h = Dcs_Tx_Fre[arfcn-512].fiveregvalue & 0x3fff;
            reg_06h = Dcs_Tx_Fre[arfcn-512].sixregvalue & 0x3ffff;
            break;
        // PCS uses 8 points
        case GSM_BAND_PCS1900:
        default:
            reg_05h = Pcs_Tx_Fre[arfcn-512].fiveregvalue & 0x3fff;
            reg_06h = Pcs_Tx_Fre[arfcn-512].sixregvalue & 0x3ffff;
            band = GSM_BAND_DCS1800;
            break;
    }
    g_RxPllFreqReg_05h = (g_RxPllFreqReg_05h & ~0x3fff) | reg_05h;
    xcvBuildSpiCmd (spiCmd, 0x05, g_RxPllFreqReg_05h);
    xcvBuildSpiCmd (spiCmd, 0x06, reg_06h);
#endif
    UINT8  TabRampReg[4] = {RDA6220_TX_RAMP_0_REG, RDA6220_TX_RAMP_1_REG,
                            RDA6220_TX_RAMP_2_REG, RDA6220_TX_RAMP_3_REG
                           };
    UINT16 rampFactor[4] = {0};
    UINT8 WinBitMap = Win->bitmap;

    // Set DigitalAfc, 14 bit is set for digital AFC
    g_xcvCmdCtx.digAfc = 0x14000 | (g_afcDacVal & RDA6220_AFC_MASK);
    if(nbSlots == 1)
    {
        switch(band)
        {
            case GSM_BAND_GSM850:
                g_xcvCmdCtx.digAfc = g_xcvCmdCtx.digAfc - 8;
                xcvBuildSpiCmd (spiCmd, 0x0f, 0x28284); //for phase error
                break;
            case GSM_BAND_GSM900:
                g_xcvCmdCtx.digAfc = g_xcvCmdCtx.digAfc - 8;
                xcvBuildSpiCmd (spiCmd, 0x0f, 0x28284); //for phase error
                break;
            case GSM_BAND_DCS1800:
                g_xcvCmdCtx.digAfc = g_xcvCmdCtx.digAfc - 10;
                xcvBuildSpiCmd (spiCmd, 0x0f, 0x2b284); //for phase error
                break;
            case GSM_BAND_PCS1900:
                g_xcvCmdCtx.digAfc = g_xcvCmdCtx.digAfc - 14;
                xcvBuildSpiCmd (spiCmd, 0x0f, 0x2a284); //for phase error
                break;
            default:
                g_xcvCmdCtx.digAfc = g_xcvCmdCtx.digAfc;
                break;
        }
    }
    else if(nbSlots == 2)
    {
        xcvBuildSpiCmd (spiCmd, 0x0f, 0x25284); //for phase error
        switch(band)
        {
            case GSM_BAND_GSM850:
                g_xcvCmdCtx.digAfc = g_xcvCmdCtx.digAfc - 9; //9
                break;
            case GSM_BAND_GSM900:
                g_xcvCmdCtx.digAfc = g_xcvCmdCtx.digAfc - 8; //8
                break;
            case GSM_BAND_DCS1800:
                g_xcvCmdCtx.digAfc = g_xcvCmdCtx.digAfc - 8; //8
                break;
            case GSM_BAND_PCS1900:
                g_xcvCmdCtx.digAfc = g_xcvCmdCtx.digAfc - 10; //10
                break;
            default:
                g_xcvCmdCtx.digAfc = g_xcvCmdCtx.digAfc;
                break;
        }
    }
    else if(nbSlots == 3)
    {
        xcvBuildSpiCmd (spiCmd, 0x0f, 0x24284); //for phase error
        switch(band)
        {
            case GSM_BAND_GSM850:
                g_xcvCmdCtx.digAfc = g_xcvCmdCtx.digAfc - 6; //6
                break;
            case GSM_BAND_GSM900:
                g_xcvCmdCtx.digAfc = g_xcvCmdCtx.digAfc - 5; //5
                break;
            case GSM_BAND_DCS1800:
                g_xcvCmdCtx.digAfc = g_xcvCmdCtx.digAfc - 5; //5
                break;
            case GSM_BAND_PCS1900:
                g_xcvCmdCtx.digAfc = g_xcvCmdCtx.digAfc - 6; //6
                break;
            default:
                g_xcvCmdCtx.digAfc = g_xcvCmdCtx.digAfc;
                break;
        }
    }
    else if(nbSlots == 4)
    {
        xcvBuildSpiCmd (spiCmd, 0x0f, 0x24284); //for phase error
        switch(band)
        {
            case GSM_BAND_GSM850:
                g_xcvCmdCtx.digAfc = g_xcvCmdCtx.digAfc - 6; //6
                break;
            case GSM_BAND_GSM900:
                g_xcvCmdCtx.digAfc = g_xcvCmdCtx.digAfc - 5; //5
                break;
            case GSM_BAND_DCS1800:
                g_xcvCmdCtx.digAfc = g_xcvCmdCtx.digAfc - 8; //4
                break;
            case GSM_BAND_PCS1900:
                g_xcvCmdCtx.digAfc = g_xcvCmdCtx.digAfc - 10; //5
                if(pclValue == 15)
                {
                    g_xcvCmdCtx.digAfc = g_xcvCmdCtx.digAfc - 2;;
                }
                break;
            default:
                g_xcvCmdCtx.digAfc = g_xcvCmdCtx.digAfc;
                break;
        }
    }

    UINT16 adjustedCdac = g_CDAC + g_CdacDelta;
    xcvBuildSpiCmd (spiCmd, RDA6220_CDAC_REG, CDAC_REG_VALUE_BASE | ((adjustedCdac&RDA6220_CDAC_REG_MASK) << RDA6220_CDAC_REG_OFFSET));

    // Set Band & arfcn
    xcvSetArfcn (RFD_DIR_TX, band, arfcn);
  //  xcvBuildSpiCmd (spiCmd, 0x3d, rampvalue);
    xcvBuildSpiCmd (spiCmd, RDA6220_FREQ_CTRL_REG, g_xcvCmdCtx.freqCtrl);
    xcvBuildSpiCmd (spiCmd, RDA6220_AFC_REG, g_xcvCmdCtx.digAfc);

#ifdef XCV_PLL_SDM_MODE
    xcvBuildSpiCmd (spiCmd, 0x35, 0x30000);
#endif
    g_xcvCmdCtx.digAfc = 0x14000 | (g_afcDacVal & RDA6220_AFC_MASK);
    //UINT32 freqMdllReg;
    //if ((g_xcvCmdCtx.freqCtrl & 0x08000) == 0)
    //{
    //    freqMdllReg = 0x11f54; // MDLL=7/8
    //}
    //else
    //{
    //    freqMdllReg = 0x11f5a; // MDLL=9
    //}
    //xcvBuildSpiCmd (spiCmd, RDA6220_FREQ_CTRL_MDLL_REG, freqMdllReg);

    //xcvBuildSpiCmd (spiCmd, 0x24, 0x20740); //PAON dr clear
    xcvBuildSpiCmd (spiCmd, 0x24, 0x2074a); // PAON dr=1, PAON value=1
#ifdef USE_BB_DC_CANCEL
    xcvBuildSpiCmd (spiCmd, 0x29, 0x1a400);
#endif
    if (g_xcvCalibSetPaRamp)
    {

        RFD_TRACE(RFD_INFO_TRC,0,"WinBitMap = %d, %d, %d, %d,%d",WinBitMap,
                  g_xcvCalibPaRampValue[0],
                  g_xcvCalibPaRampValue[1],
                  g_xcvCalibPaRampValue[2],
                  g_xcvCalibPaRampValue[3]);
        g_xcvCalibSetPaRamp = FALSE;
        g_xcvCalibPaOffset = TRUE;
        while(WinBitMap)
        {
            // If active slot
            if (WinBitMap & 0x1)
            {
                rampFactor[count] = (((g_xcvCalibPaRampValue[count] & RDA6220_RAMP_FACTOR_MASK)
                                      << RDA6220_RAMP_FACTOR_OFFSET));
            }

            WinBitMap >>= 1;
            count++;
        }
        if (count > 0)
        {
            // Ramp_mode_sel_gsm=0
            rampFactor[0] |= 0x00000 | (((count-1) & 0x3) << 13);
            for(; count>0; count--)
            {
                xcvBuildSpiCmd (spiCmd, TabRampReg[count-1], rampFactor[count-1]);
            }
        }
        return;
    }

#if 0
    xcvBuildSpiCmd (spiCmd, RDA6220_TX_RAMP_0_REG, 0x000c6); // PCL=19
    return;
#endif

    int curvernum = 0;
    int curvernum_final = 0;
    CONST UINT16 *pDacTable;
    INT16 dBmMax, dBmMin, dBmValue;
    int dBmOffset;
    INT32 dacValue;

    CONST UINT16 DAC_MAX = RDA6220_RAMP_FACTOR_MASK;
#ifdef CALIB_USE_0207
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

#else
    if (band == GSM_BAND_GSM850)
    {
        pDacTable = (UINT16 *)g_xcvCalibPtr->pa->palcust.profileG850;
        dBmMax = g_xcvCalibPtr->pa->palcust.profileDbmMaxG850;
        dBmMin = g_xcvCalibPtr->pa->palcust.profileDbmMinG850;
    }
    else if (band == GSM_BAND_GSM900)
    {
        pDacTable = (UINT16 *)g_xcvCalibPtr->pa->palcust.profileG;
        dBmMax = g_xcvCalibPtr->pa->palcust.profileDbmMaxG;
        dBmMin = g_xcvCalibPtr->pa->palcust.profileDbmMinG;
    }
    else if (band == GSM_BAND_DCS1800)
    {
        pDacTable = (UINT16 *)g_xcvCalibPtr->pa->palcust.profileD;
        dBmMax = g_xcvCalibPtr->pa->palcust.profileDbmMaxD;
        dBmMin = g_xcvCalibPtr->pa->palcust.profileDbmMinD;
    }
    else
    {
        pDacTable = (UINT16 *)g_xcvCalibPtr->pa->palcust.profileP;
        dBmMax = g_xcvCalibPtr->pa->palcust.profileDbmMaxP;
        dBmMin = g_xcvCalibPtr->pa->palcust.profileDbmMinP;
    }
#endif
    while(WinBitMap)
    {
        // If active slot
        if (WinBitMap & 0x1)
        {
            pclValue = Win->gain[count];

            if(!g_xcvCalibPaOffset)
            {
                if(nbSlots == 4)
                {

                    if(band == GSM_BAND_GSM850 || band == GSM_BAND_GSM900)
                    {
                        if(pclValue < 9)
                            pclValue = 0x8;
                    }
                    else if(band == GSM_BAND_DCS1800 || band == GSM_BAND_PCS1900)
                    {
                        if(pclValue < 4)
                            pclValue = 0x3;
                    }
                }

                else if(nbSlots == 3)
                {

                    if(band == GSM_BAND_GSM850 || band == GSM_BAND_GSM900)
                    {
                        if(pclValue < 8)
                            pclValue = 0x7;
                    }
                    else if(band == GSM_BAND_DCS1800 || band == GSM_BAND_PCS1900)
                    {
                        if(pclValue < 3)
                            pclValue = 0x2;
                    }
                }

                else if(nbSlots == 2)
                {
                    if(band == GSM_BAND_GSM850 || band == GSM_BAND_GSM900)
                    {
                        if(pclValue < 7)
                            pclValue = 0x6;
                    }
                    else if(band == GSM_BAND_DCS1800 || band == GSM_BAND_PCS1900)
                    {
                        if(pclValue < 2)
                            pclValue = 0x1;
                    }
                }
            }
#ifdef TXPOWER_TEMPERATURE_OFFSET
#ifdef PASW_RDAPA
            if((nbSlots > 2) && (g_tempgpadcvalue[6] < 332))
            {
                pclValue -= 1;
            }
            else
            {
            }
#endif
#endif
            dBmValue = Pcl2dBm(band, arfcn, pclValue);

#ifdef TXPOWER_TEMPERATURE_OFFSET
            if(!g_xcvCalibPaOffset)
            {
                if(band < 2)
                {
                    if(pclValue > 5)
                        dBmValue += PowerOffset(band, g_tempgpadcvalue[6],0);
                }
                else
                {
                    if(pclValue > 0)
                        dBmValue += PowerOffset(band, g_tempgpadcvalue[6],0);
                }
            }
#endif
            if (dBmValue < dBmMin) dBmValue = dBmMin;
            else if (dBmValue > dBmMax) dBmValue = dBmMax;
#ifdef CALIB_USE_0207
            dBmOffset = 1023 * (dBmValue-dBmMin) / (dBmMax - dBmMin);
#else
            dBmOffset = 463 * (dBmValue-dBmMin) / (dBmMax - dBmMin);
#endif
            dacValue = (pDacTable[dBmOffset]  > DAC_MAX) ? DAC_MAX :  pDacTable[dBmOffset];

#ifdef TXPOWER_TEMPERATURE_OFFSET
            if(!g_xcvCalibPaOffset)
            {
                if(band < 2)
                {
                    if(pclValue == 5)
                    {
                        INT16 dBmValue1 = Pcl2dBm(band, arfcn, pclValue +1);
#ifdef CALIB_USE_0207
                        dBmOffset = 1023 * (dBmValue1-dBmMin) / (dBmMax - dBmMin);
#else
                        dBmOffset = 463 * (dBmValue1-dBmMin) / (dBmMax - dBmMin);
#endif
                        INT16 dacValue1 = (pDacTable[dBmOffset]  > DAC_MAX) ? DAC_MAX :  pDacTable[dBmOffset];
                        dacValue += ((dacValue - dacValue1)*(INT32)PowerOffset(band, g_tempgpadcvalue[6],0)/200);
                        dacValue = (dacValue  > DAC_MAX) ? DAC_MAX :  dacValue;
                    }
                }
                else
                {
                    if(pclValue == 0)
                    {
                        INT16 dBmValue1 = Pcl2dBm(band, arfcn, pclValue +1);
#ifdef CALIB_USE_0207
                        dBmOffset = 1023 * (dBmValue1-dBmMin) / (dBmMax - dBmMin);
#else
                        dBmOffset = 463 * (dBmValue1-dBmMin) / (dBmMax - dBmMin);
#endif
                        INT16 dacValue1 = (pDacTable[dBmOffset]  > DAC_MAX) ? DAC_MAX :  pDacTable[dBmOffset];
                        hal_DbgTrace(RFD_INFO_TRC,0,"dacvalue @ %d %d %d",dacValue ,dacValue1,PowerOffset(band, g_tempgpadcvalue[6],0) );
                        dacValue += ((dacValue - dacValue1)*(INT32)PowerOffset(band, g_tempgpadcvalue[6],0)/200);
                        dacValue = (dacValue  > DAC_MAX) ? DAC_MAX :  dacValue;
                        hal_DbgTrace(RFD_INFO_TRC,0,"dacvalue1 @ %d",dacValue);
                    }
                }
            }
#endif
            //if(g_xcvbattstablevolt > 3600)
            //{
            //    if((band == GSM_BAND_GSM850 || band == GSM_BAND_GSM900)&&(pclValue == 5))
            //    {
            //        UINT32 pcl6dacvalue;
            //        INT16 Pcl6dBmValue = Pcl2dBm(band, arfcn, 6);
            //        if (Pcl6dBmValue < dBmMin) Pcl6dBmValue = dBmMin;
            //        else if (Pcl6dBmValue > dBmMax) Pcl6dBmValue = dBmMax;
            //
            //        dBmOffset = 1023 * (Pcl6dBmValue-dBmMin) / (dBmMax - dBmMin);
            //
            //        pcl6dacvalue = (pDacTable[dBmOffset]  > DAC_MAX) ? DAC_MAX :  pDacTable[dBmOffset];
            //        if(g_xcvbattstablevolt > 4000)
            //        {
            //            dacValue -=((dacValue - pcl6dacvalue)*800)/1200;  // 1db   -> 4.0-4.2
            //        }
            //        else if((g_xcvbattstablevolt > 3800)&&(g_xcvbattstablevolt < 4001))
            //        {
            //            dacValue -=((dacValue - pcl6dacvalue)*650)/1200;  // 0.8db -> 3.8-4.0
            //        }
            //        else if((g_xcvbattstablevolt > 3600)&&(g_xcvbattstablevolt < 3801))
            //        {
            //            dacValue -=((dacValue - pcl6dacvalue)*370)/1200;  // 0.5db -> 3.6-3.8
            //        }
            //    }
            //}

            if(band == GSM_BAND_GSM850 || band == GSM_BAND_GSM900)
            {
                if(pclValue < 6)
                    curvernum = (0x0 & RDA6220_RAMP_CURVE_MASK);
                else if((5 < pclValue) && (pclValue < 8))
                    curvernum = (0x1 & RDA6220_RAMP_CURVE_MASK);
                else if((7 < pclValue) && (pclValue < 11))
                    curvernum = (0x2 & RDA6220_RAMP_CURVE_MASK);
                else if((10 < pclValue) && (pclValue < 14))
                    curvernum = (0x3 & RDA6220_RAMP_CURVE_MASK);
                else if((13 < pclValue) && (pclValue < 17))
                    curvernum = (0x4 & RDA6220_RAMP_CURVE_MASK);
                else if(16 < pclValue)
                    curvernum = (0x5 & RDA6220_RAMP_CURVE_MASK);
            }
            else
            {
                if(pclValue < 1)
                    curvernum = (0x6 & RDA6220_RAMP_CURVE_MASK);
                else if((0 < pclValue) && (pclValue < 4))
                    curvernum = (0x7 & RDA6220_RAMP_CURVE_MASK);
                else if((3 < pclValue) && (pclValue < 7))
                    curvernum = (0x8 & RDA6220_RAMP_CURVE_MASK);
                else if((6 < pclValue) && (pclValue < 10))
                    curvernum = (0x9 & RDA6220_RAMP_CURVE_MASK);
                else if((9 < pclValue) && (pclValue < 13))
                    curvernum = (0xa & RDA6220_RAMP_CURVE_MASK);
                else if(12 < pclValue)
                    curvernum = (0xb & RDA6220_RAMP_CURVE_MASK);

            }
            rampFactor[count] = (((dacValue & RDA6220_RAMP_FACTOR_MASK)
                                  << RDA6220_RAMP_FACTOR_OFFSET));
            curvernum_final |= 0x00000 | ((0xf & curvernum) << (4*count));
        }

        WinBitMap >>= 1;
        count++;
    }
#ifdef XCV_RAMP_DEBUG
#ifdef CALIB_USE_0207
    if(m_ParampDebugflag)
    {
        for(int j = 0; j < 4; j++)
        {
            for(int i = 0; i < 16; i++)
            {
                if(j < 2)
                { 
                    if(g_xcvCalibPtr->hstPa->palcust.rampLow[j*16+i] !=
                            g_xcvCalibPtr->pa->palcust.rampLow[j*16+i])
                    {
                        g_xcvCalibPtr->pa->palcust.rampLow[j*16+i] = g_xcvCalibPtr->hstPa->palcust.rampLow[j*16+i];
			   xcvBuildSpiCmd (spiCmd, 0x3d, ((15-i) | (0x01f0&(j << 4)) | (g_xcvCalibPtr->hstPa->palcust.rampLow[j*16+i]<<9)));
                        break;
                    }
                }
                else
                {
                    if(g_xcvCalibPtr->hstPa->palcust.rampHigh[(j-2)*16+i] !=
                            g_xcvCalibPtr->pa->palcust.rampHigh[(j-2)*16+i])
                    {
                        g_xcvCalibPtr->pa->palcust.rampHigh[(j-2)*16+i] = g_xcvCalibPtr->hstPa->palcust.rampHigh[(j-2)*16+i];
			   xcvBuildSpiCmd (spiCmd, 0x3d, ((15-i) | (0x01f0&(j << 4)) | (g_xcvCalibPtr->hstPa->palcust.rampHigh[(j-2)*16+i]<<9)));
                        break;
                    }
                }

            }
        }
    }
#else
    if(m_ParampDebugflag)
    {
        for(int j = 0; j < 24; j++)
        {
            for(int i = 0; i < 16; i++)
            {
                if(g_xcvCalibPtr->hstPa->palcust.rampCurve[j][i] !=
                        g_xcvCalibPtr->pa->palcust.rampCurve[j][i])
                {
                    g_xcvCalibPtr->pa->palcust.rampCurve[j][i] = g_xcvCalibPtr->hstPa->palcust.rampCurve[j][i];
                    xcvBuildSpiCmd (spiCmd, 0x3d, ((15-i) | (0x01f0&(j<< 4)) | (g_xcvCalibPtr->pa->palcust.rampCurve[j][i]<<9)));
		      break;
                }
            }
        }
    }
#endif
#endif
    if (count > 0)
    {
        // Ramp_mode_sel_gsm=0
        rampFactor[0] |= 0x00000 | (((count-1) & 0x3) << 13);
        curvernum = 0x0ffff & curvernum_final;
        xcvBuildSpiCmd (spiCmd, 0x3f, 0x00002);
        xcvBuildSpiCmd (spiCmd, 0x12, curvernum);
        xcvBuildSpiCmd (spiCmd, 0x3f, 0x00000);
        RFD_TRACE(RFD_INFO_TRC,0,"curvernum @ %d",curvernum);
        for(; count>0; count--)
        {
            xcvBuildSpiCmd (spiCmd, TabRampReg[count-1], rampFactor[count-1]);
        }
    }

#if (XCV_624M_PLL_DITHER_ENABLE)
    if (g_Xcv624MPllDitherEnabled)
    {
        // switch to the page 4
        xcvBuildSpiCmd (spiCmd, 0x3f, 0x00004);
        xcvBuildSpiCmd (spiCmd, 0x07, 0x00001); // ss_en=0;ss_squre=0
        xcvBuildSpiCmd (spiCmd, 0x0b, 0x00001); // ss_en=0;ss_squre=0
        // back to page 0
        xcvBuildSpiCmd (spiCmd, 0x3f, 0x00000);
        g_Xcv624MPllDitherEnabled = FALSE;
    }
#endif
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
    .lowRla = 108,
    .dtxTcoSettings = 0,
    .rxOnTcoSettings = 0,

    // PowerUp params
#ifdef MODULE_LOCK_PLL
    .wakeUpFrameNbr = 1, //1, // activating low power optimisation
#else
    .wakeUpFrameNbr = 1, //1, // activating low power optimisation
#endif
    .pu2VcoEnDelay = 1,
    .vcoEn2VcoStableDelay = 2,
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
        .rxSampleAlign  = RFD_DIGRF_SAMPLE_ALIGN_MSB,
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
#ifdef MULTI_BANDWIDE
VOLATILE u8 gbw= 0;
#endif

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
    HAL_RFSPI_HALF_CLK_PERIOD_3, // csEndPulse
    2,  // turnAroundCycles
    24, // frameSize
    17, // inputFrameSize
};


//-----------------------------//
// Window Open/Close Functions //
//-----------------------------//
#ifdef USE_BB_DC_CANCEL
UINT8 RFD_FUNC_INTERNAL rfd_XcvRxOn(RFD_WIN_T *Win, INT16 start, INT16* I_Dcoffset,INT16* Q_Dcoffset,UINT8* xcvAnaLevel,UINT8* xcvDigLevel)
#else
UINT8 RFD_FUNC_INTERNAL rfd_XcvRxOn(RFD_WIN_T *Win, INT16 start)
#endif
{
    UINT8 XcvGain;
    HAL_RFSPI_CMD_T *spiCmd = NULL;
    //HAL_RFSPI_CMD_T *filterResetCmd = NULL;
    UINT8 i =0;
    UINT8 Winbitmap = (Win->bitmap & 0xf) << 1;
    UINT8 nbSlots = (Winbitmap & 0x10)? 4:
                    (Winbitmap & 0x8)? 3:
                    (Winbitmap & 0x4)? 2: 1;

    // Allocate the spi command
    spiCmd = hal_RfspiNewCmd(  (start + XCV_TIME(SPI_CMD) - CMD_DURATION(12)),
                               Win->winIdx );
    // Not to direct-reg filter-resetn
    // (should be done before PDN becomes valid)
    //xcvBuildSpiCmd (spiCmd, 0x16, 0x04000);
    // Build the command

#ifdef MULTI_BANDWIDE
    if(gbw == 1)
        Win->bw = 1;
#endif
#ifdef USE_BB_DC_CANCEL
    XcvGain = xcvBuildRxSpiCmd (Win,
                                spiCmd,
                                I_Dcoffset,
                                Q_Dcoffset,
                                xcvAnaLevel,
                                xcvDigLevel);
#else
    XcvGain = xcvBuildRxSpiCmd (Win,
                                spiCmd);
#endif

    spiCmd = NULL;

    for(i = 0; i<nbSlots; i++)
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

                // Allocate a filter reset command
                //filterResetCmd = hal_RfspiNewCmd(  (start + XCV_TIME(PDN_RX_UP) + 1),
                //                                   Win->winIdx );
                // Direct-reg filter-resetn to 1
                //xcvBuildSpiCmd (filterResetCmd, 0x16, 0x04210);
                //filterResetCmd = NULL;

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

                // Allocate a filter reset command
                //filterResetCmd = hal_RfspiNewCmd(  (start + XCV_TIME(PDN_RX_DN)),
                //                                   Win->winIdx );
                // Not to direct-reg filter-resetn
                // (should be done before next PDN becomes valid)
                //xcvBuildSpiCmd (filterResetCmd, 0x16, 0x04000);
                //filterResetCmd = NULL;
            }
        }

        Winbitmap >>= 1;
        start += GSM_SLOT_QS_DURATION;
    }

    Win->swapIQ = FALSE;

    RFD_TRACE(RFD_INFO_TRC,0,"xcvRxOn @ %d Qb",start);

    return XcvGain;
}

#ifdef XCV_OOK_MODE

VOID rfd_XcvSetXcvMode(bool chorfremod)  // 0 channel mode , 1 frequency mode
{

    HAL_RFSPI_CMD_T* confCmd = hal_RfspiNewSingleCmd();
    if(chorfremod)
    {
        xcvBuildSpiCmd (confCmd, 0x30, 0x20555);
    }
    else
    {
        xcvBuildSpiCmd (confCmd, 0x30, 0x20715);
        xcvBuildSpiCmd (confCmd, 0x07, 0x18200);   //ramp_din_dr 1,ramp_din_reg dac value
        xcvBuildSpiCmd (confCmd, 0x24, 0x1d741);
    }
    hal_RfspiSendCmd(confCmd);
    confCmd = NULL;
}


VOID rfd_XcvSetOokPower(char power)  //4 level, 0 level power is the lowest, level 3 power is the highest
{
    HAL_RFSPI_CMD_T* confCmd = hal_RfspiNewSingleCmd();
    if(power > 3)
        power = 3;
    if(power == 0)
        xcvBuildSpiCmd (confCmd, 0x07, 0x18880);   //12bit pd_dac_ramp_dr 1, 11bit ramp_din_dr 1,ramp_din_reg[0-9] dac value ,0x18880 ramp 200mv,0x18900 450mv,0x18980 720mv, 0x18a00 960mv
    else if(power == 1)
        xcvBuildSpiCmd (confCmd, 0x07, 0x18900);
    else if(power == 2)
        xcvBuildSpiCmd (confCmd, 0x07, 0x18980);
    else
        xcvBuildSpiCmd (confCmd, 0x07, 0x18a00);
    hal_RfspiSendCmd(confCmd);
    confCmd = NULL;
}

VOID rfd_XcvSetOokChannel(INT16 channel)  // [0-163] max channel is 163
{
    UINT32 reg_05h = 0;
    UINT32 reg_06h = 0;
    if(channel > 163)
        channel = 163;
    HAL_RFSPI_CMD_T* confCmd = hal_RfspiNewSingleCmd();

    hal_TcuClrTco(XCV->PDN);

    // hal_SysWaitMicrosecond(1);
    reg_05h = GsmOokMode_Tx_Fre[channel].fiveregvalue & 0x3fff;
    reg_06h = GsmOokMode_Tx_Fre[channel].sixregvalue & 0x3ffff;
    xcvBuildSpiCmd (confCmd, 0x08, 0x18000);
    xcvBuildSpiCmd (confCmd, 0x05, 0x28000|reg_05h);
    xcvBuildSpiCmd (confCmd, 0x06, reg_06h);
    xcvBuildSpiCmd (confCmd, RDA6220_FREQ_CTRL_REG, (0x18000 | (1 << 12) | (1 << 10)));
    hal_RfspiSendCmd(confCmd);

    hal_TcuSetTco(XCV->PDN);
    confCmd = NULL;
}

VOID rfd_XcvSetOokData(bool Ookdata)  //Ookdata
{
    HAL_RFSPI_CMD_T* confCmd = hal_RfspiNewSingleCmd();

    if(Ookdata)
    {
        xcvBuildSpiCmd (confCmd, 0x24, 0x1d74b);
        hal_RfspiSendCmd(confCmd);
        //     hal_SysWaitMicrosecond(200);  //
    }
    else
    {
        xcvBuildSpiCmd (confCmd, 0x24, 0x1d749);
        hal_RfspiSendCmd(confCmd);
        //  hal_SysWaitMicrosecond(200);  // 239 us

    }

    confCmd = NULL;
}

// 0x26 default value 0x2a020
VOID rfd_XcvSetXcvTestMode(bool testmod)  // 1 testmode, 0 normal mode
{

    HAL_RFSPI_CMD_T* confCmd = hal_RfspiNewSingleCmd();

    if(testmod)
    {
        xcvBuildSpiCmd (confCmd, 0x26, 0x2a43c); //fomer_bypass_dr 1,fomer_bypass_dr 1
    }
    else
    {
        xcvBuildSpiCmd (confCmd, 0x26, 0x2a020);
    }
    hal_RfspiSendCmd(confCmd);

    confCmd = NULL;
}

#endif
VOID RFD_FUNC_INTERNAL rfd_XcvRxOff(RFD_WIN_T *Win,INT16 stop)
{
    // Transceiver in PowerDown
    hal_TcuSetEvent (XCV_CLR_PDN, stop + XCV_TIME(PDN_RX_DN));
    // stop the recording
    hal_TcuSetEvent (HAL_TCU_STOP_RFIN_RECORD, stop+ XCV_TIME(PDN_RX_DN));

    RFD_TRACE(RFD_INFO_TRC,0,"xcvRxOff @ %d Qb",stop);
}
#ifdef RF_LDO_MODE
extern VOID pmd_DcdcPower_Control(BOOL on);
#endif
VOID RFD_FUNC_INTERNAL rfd_XcvTxOn(RFD_WIN_T *Win, INT16 start)
{
    HAL_RFSPI_CMD_T *spiCmd;
    UINT8 i =0;
    UINT16 stop = 0;
    UINT8 Winbitmap = (Win->bitmap & 0xf) << 1;
    UINT8 nbSlots = (Winbitmap & 0x10)? 4:
                    (Winbitmap & 0x8)? 3:
                    (Winbitmap & 0x4)? 2: 1;
    BOOL ramp_flag = g_xcvCalibSetPaRamp;
#ifdef RF_LDO_MODE
    pmd_DcdcPower_Control(FALSE);
#endif
#ifdef PASW_HS8292U
    INT8 m_strobtime[4][2][16]=
    {
        {   {0,0,-1,-1,-1,0,2,2,2,2,1,1,-1,-1,-1,-1}
            ,{1,2,2,1,1,0,-2,-2,-2,-1,-1,-2,-2,0,0,0}
        }
        ,{{1,1,1,1,1,1,3,2,2,2,2,0,-1,-1,-1,-1}
            ,{0,1,1,1,1,0,-2,-2,-2,-2,-1,0,0,0,-1,-1}
        }
        ,{{1,0,-1,-1,-1,0,0,2,2,2,2,2,-1,-1,-1,-1}
            ,{0,0,0,0,0,0,0,-2,-2,-2,-1,-1,1,1,1,1}
        }
        ,{{0,0,0,0,0,0,0,3,2,2,2,1,0,0,0,0}
            ,{0,1,1,2,1,1,1,-1,-1,-1,-1,-1,0,3,3,3}
        }
    };

#else
    INT8 m_strobtime[4][2][16]=
    {
        {   {0,-1,-2,-2,-2,1,1,1,1,1,1,1,-1,-1,-1,-1}
            ,{1,2,2,2,2,-1,-1,-1,-1,-1,-1,-2,-3,0,0,0}
        }
        ,{{1,0,1,1,1,1,1,1,1,1,1,0,-1,-2,-2,-2}
            ,{0,1,2,2,2,-1,-1,-1,-1,-1,-1,0,0,0,-1,-1}
        }
        ,{{1,0,-1,-1,-1,2,2,2,2,2,2,1,-1,-1,-1,-1}
            ,{0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,1,1,1}
        }
        ,{{0,0,0,0,1,0,0,2,1,1,2,1,-1,-1,-1,-1}
            ,{0,1,1,2,0,0,0,-1,-1,-1,-1,-1,0,3,3,3}
        }
    };
#endif
    if(Win->type == GSM_WIN_TYPE_TXABURST)
    {
        g_xcvCalibPaOffset = FALSE;
#ifdef USE_BB_DC_CANCEL
        g_dcoffsetflag = 1;
#endif
    }
#ifdef DIGRF_TXEN_V2_WORKAROUND
    hal_TcuSetEvent(HAL_TCU_START_GMSK, start + XCV_TIME(PDN_TX_UP));
    hal_TcuSetEvent(HAL_TCU_STOP_GMSK, start + XCV_TIME(PDN_TX_UP)+20);
#endif // DIGRF_TXEN_V2_WORKAROUND

    // Needs an Spi Command
    spiCmd = hal_RfspiNewCmd(  (start + XCV_TIME(SPI_CMD)- CMD_DURATION(11)),
                               Win->winIdx);

    // mark for DTX
    spiCmd->maskable = TRUE;

#ifndef _TX_BLOCK_MODE
    hal_TcuSetEvent(HAL_TCU_START_GMSK, start + XCV_TIME(TX_GMSK_ON));
#endif
    hal_TcuSetEvent(XCV_SET_PDN, start + XCV_TIME(PDN_TX_UP));
    for(i =0; i<nbSlots; i++)
    {
        UINT32 curr_slot = Winbitmap & 0x2;
        if (curr_slot)
        {
            if (!ramp_flag)
            {
                if(nbSlots == 4 && i == 3)
                    hal_TcuSetEvent (HAL_TCU_DIGRF_STB, start + XCV_TIME(TX_STROBE_ON) -4);
                else if(nbSlots == 4 && i == 2)
                    hal_TcuSetEvent (HAL_TCU_DIGRF_STB, start + XCV_TIME(TX_STROBE_ON) -4);
                else if(nbSlots == 4 && i == 1)
                    hal_TcuSetEvent (HAL_TCU_DIGRF_STB, start + XCV_TIME(TX_STROBE_ON) -3);
                else if(nbSlots == 3 && i == 2)
                    hal_TcuSetEvent (HAL_TCU_DIGRF_STB, start + XCV_TIME(TX_STROBE_ON) -4);
                else if(nbSlots == 3 && i == 1)
                    hal_TcuSetEvent (HAL_TCU_DIGRF_STB, start + XCV_TIME(TX_STROBE_ON) -3);
                else if(nbSlots == 2 && i == 1)
                    hal_TcuSetEvent (HAL_TCU_DIGRF_STB, start + XCV_TIME(TX_STROBE_ON) -4);
                else if(nbSlots == 1)
                    hal_TcuSetEvent (HAL_TCU_DIGRF_STB, start + XCV_TIME(TX_STROBE_ON) + m_strobtime[Win->band][0][(Win->band < 2)?(Win->gain[i]-5):Win->gain[i]]);
                else
                    hal_TcuSetEvent (HAL_TCU_DIGRF_STB, start + XCV_TIME(TX_STROBE_ON));
            }
            else
                hal_TcuSetEvent (HAL_TCU_DIGRF_STB, start + XCV_TIME(TX_STROBE_ON) + m_strobtime[Win->band][0][0]);;

            stop = ((Win->type == GSM_WIN_TYPE_TXABURST)?
                    (start + GSM_ABURST_ACTIVE_QS_DURATION + 4*g_xcvParams.txBstMap.postamble):
                    (start + GSM_NBURST_ACTIVE_QS_DURATION + 4*g_xcvParams.txBstMap.postamble));

            if (!ramp_flag)
            {
                if(nbSlots == 4 && i == 3)
                    hal_TcuSetEvent (HAL_TCU_DIGRF_STB, stop + XCV_TIME(TX_STROBE_OFF) -3);
                else if(nbSlots == 3 && i == 2)
                    hal_TcuSetEvent (HAL_TCU_DIGRF_STB, stop + XCV_TIME(TX_STROBE_OFF) -2);
                else if(nbSlots == 3 && i == 1)
                    hal_TcuSetEvent (HAL_TCU_DIGRF_STB, stop + XCV_TIME(TX_STROBE_OFF) +2);
                else if(nbSlots == 2 && i == 0)
                    hal_TcuSetEvent (HAL_TCU_DIGRF_STB, stop + XCV_TIME(TX_STROBE_OFF) +2);
                else if(nbSlots == 1)
                    hal_TcuSetEvent (HAL_TCU_DIGRF_STB, stop + XCV_TIME(TX_STROBE_OFF) + m_strobtime[Win->band][1][(Win->band < 2)?(Win->gain[i]-5):Win->gain[i]]);
                else
                    hal_TcuSetEvent (HAL_TCU_DIGRF_STB, stop + XCV_TIME(TX_STROBE_OFF));
            }
            else
            {
                hal_TcuSetEvent (HAL_TCU_DIGRF_STB, stop + XCV_TIME(TX_STROBE_OFF) + m_strobtime[Win->band][1][0]);
            }
        }

        start += (Win->type == GSM_WIN_TYPE_TXABURST)?
                 (GSM_ABURST_ACTIVE_QS_DURATION + 4*8):  // active access burst + 8 guard bits.
                 GSM_SLOT_QS_DURATION;
        Winbitmap >>= 1;
    }
#ifndef _TX_BLOCK_MODE
    hal_TcuSetEvent(HAL_TCU_STOP_GMSK, stop + XCV_TIME(TX_GMSK_OFF));
#endif
    hal_TcuSetEvent (XCV_CLR_PDN, stop + XCV_TIME(PDN_TX_DN));
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
    HAL_RFSPI_CMD_T *spiCmd;
    // Needs an Spi Command
    spiCmd = hal_RfspiNewCmd(  (stop + XCV_TIME(PDN_TX_DN) - CMD_DURATION(2) + 4),
                               Win->winIdx);

    // mark for DTX
    spiCmd->maskable = TRUE;
    xcvBuildSpiCmd (spiCmd, 0x24, 0x20748); // PAON dr=1, PAON value=0

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
    extern void calib_CheckCalibVersion();
    if(!g_xcvCalibPaOffset)
        calib_CheckCalibVersion();

#ifdef TXPOWER_TEMPERATURE_OFFSET
    static UINT8 tempertureloop = 0;
    if(!(tempertureloop++ %30))
    {
#define GPADC_BAD_VALUE  0xFFFF
        UINT16 dacvalue = pmd_GeTemperatureSW();
        if(GPADC_BAD_VALUE != dacvalue)
        {
            static UINT8 loop = 0;
            g_tempgpadcvalue[loop%6] = dacvalue;
            if(++loop == 0xff)
            {
                loop = 6;
            }
            if(loop > 5)
            {
                g_tempgpadcvalue[6] = (g_tempgpadcvalue[0]+g_tempgpadcvalue[5]+g_tempgpadcvalue[4]+g_tempgpadcvalue[3]+g_tempgpadcvalue[2]+g_tempgpadcvalue[1])/6;
            }
            else
            {
                g_tempgpadcvalue[6] = dacvalue;
            }
        }
    }
#endif
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
        xcvBuildSpiCmd (confCmd, 0x3F, 0x00000); // Page0
        hal_RfspiSendCmd(confCmd);
        // Read chip metal ID from HAL SYS
        g_xcvChipId = hal_SysGetChipId(HAL_SYS_CHIP_METAL_ID);
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

#if defined(ABB_HP_DETECT) && defined(ABB_HP_DETECT_IN_LP)
VOID rfd_XcvRegPuXtalEnable(BOOL enable)
{
    UINT32 status = hal_SysEnterCriticalSection();

    // Check why force-write takes 3 ms to finish in some case (when BT playing music?)
    while (!(hal_RfspiImmReadWriteAvail() && hal_LpsRemainingFramesReady(2)))
    {
        sxr_Sleep(10 MS_WAITING);
    }

    // Now we can directly write to XCV
    if(enable)
    {
        rfd_XcvRegForceWriteSingle(0x20, 0x310a1);// enabling 6p5m to avoid 26M clock burr
    }
    else
    {
        rfd_XcvRegForceWriteSingle(0x20,0x110a1);// disabling 6p5m to avoid 26M clock burr
    }

    hal_SysExitCriticalSection(status);
}
#endif

//-----------------------------------//
// Open/Close & Low Power Functions  //
//-----------------------------------//

INLINE VOID xcvConfigure(VOID)
{
    HAL_RFSPI_CMD_T* confCmd = hal_RfspiNewSingleCmd();
    xcvBuildSpiCmd (confCmd, 0x3F, 0x00000); // Page0

    // ;=====Reg Reset==============================================
    xcvBuildSpiCmd (confCmd, 0x30, 0x00a00); // Soft Resetn down
    hal_RfspiSendCmd(confCmd);
    // wait 1ms
    hal_TimDelay(1 MS_WAITING);

#ifdef CHANNEL_FREQUENCY_MODE
    xcvBuildSpiCmd (confCmd, 0x30, 0x00555); // Soft Resetn up
#else
    xcvBuildSpiCmd (confCmd, 0x30, 0x00515); // Soft Resetn up
#endif
    hal_RfspiSendCmd(confCmd);

#ifdef USE_EXT_XTAL_32K //EXT 32K enable
    xcvBuildSpiCmd (confCmd, 0x3F, 0x00003); // page=3
    xcvBuildSpiCmd (confCmd, 0x04, 0x163f0); // Pu_xtal_pmu=1
    hal_RfspiSendCmd(confCmd);
    hal_SysWaitMicrosecond(100); //Delay 100us
    xcvBuildSpiCmd (confCmd, 0x04, 0x063f0); //clk32K_lpo_en_pmu=0
    hal_RfspiSendCmd(confCmd);
    hal_SysWaitMicrosecond(100); //Delay 100us
    xcvBuildSpiCmd (confCmd, 0x04, 0x0e3f0); //clk32K_xtal_en_pmu=0
    hal_RfspiSendCmd(confCmd);
    hal_SysWaitMicrosecond(100); //Delay 100us
    xcvBuildSpiCmd (confCmd, 0x04, 0x0a3f0); //lpo_32K_pu=0
    hal_RfspiSendCmd(confCmd);
#else //26M Div 32k enable
    // Direct-reg pu_xtal=1 before enabling 6p5m to avoid 26M clock burr
    xcvBuildSpiCmd (confCmd, 0x20, 0x310a1);
    xcvBuildSpiCmd (confCmd, 0x3F, 0x00003); // page=3
    xcvBuildSpiCmd (confCmd, 0x01, 0x3e410); // Enable_clk_6p5m=1
    // Restore pu_xtal setting after enabling 6p5m
    xcvBuildSpiCmd (confCmd, 0x3F, 0x00000); // page=0
    xcvBuildSpiCmd (confCmd, 0x20, 0x110a1);
    xcvBuildSpiCmd (confCmd, 0x3F, 0x00003); // page=3
    hal_RfspiSendCmd(confCmd);
    xcvBuildSpiCmd (confCmd, 0x04, 0x043f0); // clk32k_lpo_en_pmu=0
    hal_RfspiSendCmd(confCmd);
    hal_SysWaitMicrosecond(100); //Delay 100us
    xcvBuildSpiCmd (confCmd, 0x04, 0x203f1); // clk32k_26mXtal_en_pmu=1 Xen_bt_enable=1
    hal_RfspiSendCmd(confCmd);
#endif
    //xcvBuildSpiCmd (confCmd, 0x3F, 0x00000); // page 0

    // ;=====Page 3=====
    xcvBuildSpiCmd (confCmd, 0x3F, 0x00003); // page=3
    INT16 lp_CDAC = g_CDAC + g_CdacDelta;

    if (1)
        lp_CDAC -= 3;
    else
        lp_CDAC -= 8;

    if (lp_CDAC < 0)
        lp_CDAC = 0;
    else if(lp_CDAC > 64)
        lp_CDAC = 64;

    // LP current and LP CDAC
    if(1)
        xcvBuildSpiCmd (confCmd, 0x00, 0x0c2| ((lp_CDAC&RDA6220_CDAC_REG_MASK) << RDA6220_CDAC_REG_OFFSET));
    else
        xcvBuildSpiCmd (confCmd, 0x00, 0x102| ((lp_CDAC&RDA6220_CDAC_REG_MASK) << RDA6220_CDAC_REG_OFFSET));
    //xcvBuildSpiCmd (confCmd, 0x3f, 0x00000); // page 0
    hal_RfspiSendCmd(confCmd);

    // ;=====Page 4=====
    xcvBuildSpiCmd (confCmd, 0x3f, 0x00004); // page 4
    xcvBuildSpiCmd (confCmd, 0x01, 0x04926); // pll_cpaux_bit=7  0x01f26

    //xcvBuildSpiCmd (confCmd, 0x0d, 0x0a041); // dcdc divider 0x093fd
    //xcvBuildSpiCmd (confCmd, 0x0e, 0x0a041); // dcdc divider 0x093fd
    xcvBuildSpiCmd (confCmd, 0x0d, 0x0a021); // dcdc1_Freq=26/9=2.9M -- edit by xianmiao 160303
    xcvBuildSpiCmd (confCmd, 0x0e, 0x0a021); // dcdc2_Freq=26/9=2.9M -- edit by xianmiao 160303

    xcvBuildSpiCmd (confCmd, 0x3f, 0x00000); // page 0
    xcvBuildSpiCmd (confCmd, 0x05,0x3a950);
    hal_RfspiSendCmd(confCmd);
    rxTxTimeOffset = g_xcvCalibPtr->xcv->palcust.rxTxTimeOffset;
#ifdef TXPOWER_TEMPERATURE_OFFSET
    hal_AnaGpadcOpen(0, 7);
#endif
#ifdef RF_LDO_MODE
    extern BOOL pmd_EnableDcdcPower(BOOL on);
    pmd_EnableDcdcPower(FALSE);
#endif
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

    // For the phones calibrated with the old xcv driver, this
    // ensures that the timing is correct without re-calibration.
    g_xcvCalibPtr->xcv->palcust.rxTxTimeOffset = -59;

    // XCV Chip ID has been read in pmd_Open()

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
#ifdef CALIB_USE_0207
#ifdef PASW_HS8292U
UINT16 m_rampvalue[24][16]=
{
    //-3.25  ,-3     ,-2.75  ,-2.5   ,-2.25  ,-2     ,-1.75  ,-1.5   ,-1.25  ,-1     ,-0.75  ,-0.5   ,-0.25  ,0      ,0.25   ,0.5
    {0x00a00,0x00a01,0x00e02,0x01403,0x01c04,0x02205,0x02e06,0x03e07,0x04608,0x05209,0x05c0a,0x0620b,0x06e0c,0x0740d,0x0780e,0x07a0f}
    ,{0x01a10,0x01a11,0x01c12,0x01e13,0x02414,0x02615,0x03016,0x03a17,0x04a18,0x05419,0x0601a,0x06a1b,0x0721c,0x0781d,0x07c1e,0x07e1f}
    ,{0x01e20,0x01e21,0x02022,0x02623,0x02a24,0x02c25,0x03426,0x03c27,0x04828,0x05429,0x0602a,0x06a2b,0x0722c,0x0782d,0x07c2e,0x07e2f}
    ,{0x03e30,0x04031,0x04032,0x04033,0x04e34,0x05835,0x06236,0x06e37,0x07438,0x07a39,0x07e3a,0x07e3b,0x07e3c,0x07e3d,0x07e3e,0x07e3f}
    ,{0x04040,0x04041,0x04842,0x04e43,0x05244,0x05e45,0x06246,0x06a47,0x07048,0x07449,0x07e4a,0x07e4b,0x07e4c,0x07e4d,0x07e4e,0x07e4f}
    ,{0x02c50,0x03651,0x03e52,0x04653,0x05654,0x06055,0x06456,0x06857,0x07258,0x07659,0x07e5a,0x07e5b,0x07e5c,0x07e5d,0x07e5e,0x07e5f}
    ,{0x01860,0x01861,0x01a62,0x01a63,0x02464,0x02465,0x02e66,0x03867,0x04468,0x05269,0x05e6a,0x06a6b,0x0726c,0x0786d,0x07c6e,0x07e6f}
    ,{0x01a70,0x01a71,0x01c72,0x02073,0x02474,0x02875,0x02e76,0x03877,0x04478,0x05279,0x05e7a,0x06a7b,0x0727c,0x0787d,0x07c7e,0x07e7f}
    ,{0x01a80,0x01a81,0x01c82,0x02083,0x02484,0x02e85,0x03286,0x03a87,0x04488,0x05289,0x05e8a,0x06a8b,0x0728c,0x07a8d,0x07e8e,0x07e8f}
    ,{0x03690,0x03a91,0x03e92,0x04c93,0x04e94,0x05895,0x06496,0x06e97,0x07498,0x07a99,0x07e9a,0x07e9b,0x07e9c,0x07e9d,0x07e9e,0x07e9f}
    ,{0x036a0,0x03aa1,0x03ea2,0x04ca3,0x04ea4,0x058a5,0x064a6,0x06ea7,0x074a8,0x07aa9,0x07eaa,0x07eab,0x07eac,0x07ead,0x07eae,0x07eaf}
    ,{0x02cb0,0x036b1,0x03eb2,0x046b3,0x04eb4,0x058b5,0x064b6,0x06eb7,0x074b8,0x07ab9,0x07eba,0x07ebb,0x07ebc,0x07ebd,0x07ebe,0x07ebf}
    ,{0x004c0,0x00ac1,0x012c2,0x01ac3,0x020c4,0x02ec5,0x036c6,0x03ec7,0x04ac8,0x056c9,0x062ca,0x068cb,0x070cc,0x078cd,0x07cce,0x07ecf}
    ,{0x004d0,0x006d1,0x00ad2,0x012d3,0x01ad4,0x024d5,0x02ed6,0x038d7,0x044d8,0x052d9,0x05eda,0x06adb,0x072dc,0x078dd,0x07cde,0x07edf}
    ,{0x004e0,0x006e1,0x00ae2,0x012e3,0x01ae4,0x028e5,0x03ae6,0x046e7,0x054e8,0x05ee9,0x072ea,0x07aeb,0x07eec,0x07eed,0x07eee,0x07eef}
    ,{0x02cf0,0x036f1,0x03ef2,0x046f3,0x04ef4,0x058f5,0x064f6,0x06ef7,0x074f8,0x07af9,0x07efa,0x07efb,0x07efc,0x07efd,0x07efe,0x07eff}
    ,{0x02d00,0x03701,0x03f02,0x04703,0x04f04,0x05905,0x06506,0x06f07,0x07508,0x07b09,0x07f0a,0x07f0b,0x07f0c,0x07f0d,0x07f0e,0x07f0f}
    ,{0x02d10,0x03711,0x03f12,0x04713,0x04f14,0x05915,0x06516,0x06f17,0x07518,0x07b19,0x07f1a,0x07f1b,0x07f1c,0x07f1d,0x07f1e,0x07f1f}
    ,{0x00520,0x00721,0x00b22,0x01323,0x01b24,0x02325,0x02f26,0x03927,0x04528,0x05329,0x05f2a,0x06b2b,0x0732c,0x0792d,0x07d2e,0x07f2f}
    ,{0x00530,0x00731,0x00b32,0x01333,0x01b34,0x02335,0x02f36,0x03937,0x04538,0x05339,0x05f3a,0x06b3b,0x0733c,0x0793d,0x07d3e,0x07f3f}
    ,{0x00540,0x00741,0x00b42,0x01343,0x01b44,0x02345,0x02f46,0x03947,0x04548,0x05349,0x05f4a,0x06b4b,0x0734c,0x0794d,0x07d4e,0x07f4f}
    ,{0x02d50,0x03751,0x03f52,0x04753,0x04f54,0x05955,0x06556,0x06f57,0x07558,0x07b59,0x07f5a,0x07f5b,0x07f5c,0x07f5d,0x07f5e,0x07f5f}
    ,{0x02d60,0x03761,0x03f62,0x04763,0x04f64,0x05965,0x06566,0x06f67,0x07568,0x07b69,0x07f6a,0x07f6b,0x07f6c,0x07f6d,0x07f6e,0x07f6f}
    ,{0x02d70,0x03771,0x03f72,0x04773,0x04f74,0x05975,0x06576,0x06f77,0x07578,0x07b79,0x07f7a,0x07f7b,0x07f7c,0x07f7d,0x07f7e,0x07f7f}
};
#else
UINT16 m_rampvalue[24][16]=
{
    {0x01800,0x01801,0x01c02,0x01c03,0x02404,0x02a05,0x03406,0x03e07,0x04a08,0x05409,0x0600a,0x06a0b,0x0720c,0x0780d,0x07c0e,0x07e0f}
    ,{0x01a10,0x01a11,0x01c12,0x02013,0x02414,0x02c15,0x03416,0x03e17,0x04a18,0x05419,0x0601a,0x06a1b,0x0721c,0x0781d,0x07c1e,0x07e1f}
    ,{0x00420,0x00621,0x00a22,0x01223,0x01a24,0x02825,0x03826,0x04627,0x05428,0x06429,0x0722a,0x07a2b,0x07e2c,0x07e2d,0x07e2e,0x07e2f}
    ,{0x01430,0x02231,0x02c32,0x03633,0x04234,0x05035,0x05e36,0x06837,0x07038,0x07639,0x07a3a,0x07e3b,0x07e3c,0x07e3d,0x07e3e,0x07e3f}
    ,{0x01440,0x02241,0x02c42,0x03643,0x04244,0x05045,0x06846,0x07047,0x07648,0x07a49,0x07e4a,0x07e4b,0x07e4c,0x07e4d,0x07e4e,0x07e4f}
    ,{0x02c50,0x03651,0x03e52,0x04653,0x04e54,0x05855,0x06456,0x06e57,0x07458,0x07a59,0x07e5a,0x07e5b,0x07e5c,0x07e5d,0x07e5e,0x07e5f}
    ,{0x01060,0x01061,0x01462,0x01863,0x01a64,0x02265,0x02e66,0x03a67,0x04468,0x05269,0x05e6a,0x06a6b,0x0726c,0x0786d,0x07c6e,0x07e6f}
    ,{0x01a70,0x01a71,0x01c72,0x02073,0x02474,0x02475,0x02e76,0x03877,0x04478,0x05279,0x05e7a,0x06a7b,0x0727c,0x0787d,0x07c7e,0x07e7f}
    ,{0x01280,0x01481,0x01c82,0x02483,0x03084,0x03885,0x04086,0x04687,0x05488,0x06489,0x0728a,0x07a8b,0x07e8c,0x07e8d,0x07e8e,0x07e8f}
    ,{0x01490,0x02291,0x02c92,0x03693,0x04294,0x05095,0x05e96,0x06897,0x07098,0x07699,0x07a9a,0x07e9b,0x07e9c,0x07e9d,0x07e9e,0x07e9f}
    ,{0x014a0,0x022a1,0x02ca2,0x036a3,0x042a4,0x050a5,0x05ea6,0x068a7,0x070a8,0x076a9,0x07aaa,0x07eab,0x07eac,0x07ead,0x07eae,0x07eaf}
    ,{0x02cb0,0x036b1,0x03eb2,0x046b3,0x04eb4,0x058b5,0x064b6,0x06eb7,0x074b8,0x07ab9,0x07eba,0x07ebb,0x07ebc,0x07ebd,0x07ebe,0x07ebf}
    ,{0x004c0,0x006c1,0x00ac2,0x018c3,0x01ac4,0x020c5,0x02ec6,0x03cc7,0x044c8,0x052c9,0x05eca,0x06acb,0x072cc,0x078cd,0x07cce,0x07ecf}
    ,{0x004d0,0x006d1,0x00ad2,0x012d3,0x01ad4,0x024d5,0x02ed6,0x038d7,0x044d8,0x052d9,0x05eda,0x06adb,0x072dc,0x078dd,0x07cde,0x07edf}
    ,{0x004e0,0x006e1,0x00ae2,0x012e3,0x01ae4,0x028e5,0x038e6,0x046e7,0x054e8,0x064e9,0x072ea,0x07aeb,0x07eec,0x07eed,0x07eee,0x07eef}
    ,{0x014f0,0x022f1,0x02cf2,0x036f3,0x042f4,0x050f5,0x05ef6,0x068f7,0x070f8,0x076f9,0x07afa,0x07efb,0x07efc,0x07efd,0x07efe,0x07eff}
    ,{0x01500,0x02301,0x02d02,0x03703,0x04304,0x05105,0x05f06,0x06907,0x07108,0x07709,0x07b0a,0x07f0b,0x07f0c,0x07f0d,0x07f0e,0x07f0f}
    ,{0x02d10,0x03711,0x03f12,0x04713,0x04f14,0x05915,0x06516,0x06f17,0x07518,0x07b19,0x07f1a,0x07f1b,0x07f1c,0x07f1d,0x07f1e,0x07f1f}
    ,{0x00520,0x00721,0x00b22,0x01323,0x01b24,0x02125,0x02f26,0x03927,0x04528,0x05329,0x05f2a,0x06b2b,0x0732c,0x0772d,0x07d2e,0x07f2f}
    ,{0x00530,0x00731,0x00b32,0x01333,0x01b34,0x02535,0x02f36,0x03937,0x04538,0x05339,0x05f3a,0x06b3b,0x0733c,0x0793d,0x07d3e,0x07f3f}
    ,{0x00540,0x00741,0x00b42,0x01343,0x01b44,0x02945,0x03946,0x04747,0x05548,0x06549,0x0734a,0x07b4b,0x07f4c,0x07f4d,0x07f4e,0x07f4f}
    ,{0x01550,0x02351,0x02d52,0x03753,0x04354,0x05155,0x05f56,0x06957,0x07158,0x07759,0x07b5a,0x07f5b,0x07f5c,0x07f5d,0x07f5e,0x07f5f}
    ,{0x01560,0x02361,0x02d62,0x03763,0x04364,0x05165,0x05f66,0x06967,0x07168,0x07769,0x07b6a,0x07f6b,0x07f6c,0x07f6d,0x07f6e,0x07f6f}
    ,{0x02d70,0x03771,0x03f72,0x04773,0x04f74,0x05975,0x06576,0x06f77,0x07578,0x07b79,0x07f7a,0x07f7b,0x07f7c,0x07f7d,0x07f7e,0x07f7f}
};
#endif
#endif
INLINE VOID rfd_xcvRampSet(HAL_RFSPI_CMD_T* confCmd ,UINT16 ramp_num)
{
    for(int ramp_point = 0; ramp_point < 16; ramp_point++)
#ifdef CALIB_USE_0207
        xcvBuildSpiCmd (confCmd, 0x3d, (m_rampvalue[ramp_num][ramp_point])|(ramp_num << 4));
#else
        xcvBuildSpiCmd (confCmd, 0x3d, ((15-ramp_point) | (0x01f0&(ramp_num << 4)) | (g_xcvCalibPtr->pa->palcust.rampCurve[ramp_num][ramp_point]<<9)));
#endif
    hal_RfspiSendCmd(confCmd);
}

VOID rfd_XcvRfInit(VOID)
{
    g_xcvRfIniting = TRUE;

    HAL_CLK_T *xcvClk = hal_ClkGet(FOURCC_XCV);
    if (xcvClk == NULL)
        asm volatile ("break 1");
    hal_ClkEnable(xcvClk);

    HAL_RFSPI_CMD_T* confCmd = hal_RfspiNewSingleCmd();

    xcvBuildSpiCmd (confCmd, 0x3f, 0x00000); // page 0
    // ;=====DCXO Setting=====
    UINT16 adjustedCdac = g_CDAC + g_CdacDelta;
    xcvBuildSpiCmd (confCmd, RDA6220_CDAC_REG, CDAC_REG_VALUE_BASE| ((adjustedCdac&RDA6220_CDAC_REG_MASK) << RDA6220_CDAC_REG_OFFSET));

    // ;=====IC Debug=====
    xcvBuildSpiCmd (confCmd, RDA6220_FREQ_CTRL_MDLL_REG, 0x12464); // mdll band sel=1,mdll_band_reg_dr=0
    xcvBuildSpiCmd (confCmd, 0x14, 0x281e4); //18'b10_1000_0001_1110_0100; dac_bias=110,Dac_range=11,dac_cm=00  //edit 120314
    xcvBuildSpiCmd (confCmd, 0x17, 0x033a8); //18'b00_0011_0011_1010_1000; dac_rst_time=11
    xcvBuildSpiCmd (confCmd, 0x18, 0x0354c); //18'b00_0011_0101_0100_1100; adc_reset_delay=14CH(filter reset)
    xcvBuildSpiCmd (confCmd, 0x1B, 0x29155); //18'b10_1001_0001_0101_0101; thmt_dac_ref
    xcvBuildSpiCmd (confCmd, 0x1C, 0x39122); //18'b11_1001_0001_0010_0010; Thmt_ptat_var=1100
    xcvBuildSpiCmd (confCmd, 0x0f, 0x28284); //18'b10_1000_0010_1000_0100; for phase error
    hal_RfspiSendCmd(confCmd);

    // ;=====Tx Setting=====
    xcvBuildSpiCmd (confCmd, 0x02, 0x2ab55); //18'b10_1010_1011_0101_0101; Digrf_stream_mode_enable=1
    xcvBuildSpiCmd (confCmd, 0x03, 0x00320); //18'b00_0000_0011_0010_0000; Ramp_curve,Ramp_mult_factor
    xcvBuildSpiCmd (confCmd, 0x28, 0x24602); //18'b10_0100_0110_0000_0010; equfil_tx_bypass=1,TX_diff_en=1

    // Tx timing control
    xcvBuildSpiCmd (confCmd, 0x24, 0x20740); // 18'b10_0000_0111_0100_0000; 17:16 txpower start delay 2
    //xcvBuildSpiCmd (confCmd, 0x36, 0x10f05);

    hal_RfspiSendCmd(confCmd);
    int ramp_num = 0;
    for(ramp_num = 0; ramp_num < 24; ramp_num++)
    {
        rfd_xcvRampSet(confCmd,ramp_num);
    }
    // ;=====RX Setting=====
    xcvBuildSpiCmd (confCmd, 0x01, 0x3c036); //18'b11_1100_0000_0011_0110; rx gain set
    xcvBuildSpiCmd (confCmd, 0x12, 0x110f0); //18'b01_0001_0000_1111_0000; filter tuning_bit=1000
    xcvBuildSpiCmd (confCmd, 0x13, 0x01241); //18'b00_0001_0010_0100_0001;
    xcvBuildSpiCmd (confCmd, 0x1a, 0x26e5c); //18'b10_0110_1110_0101_1100; Enlarge auxclk amp
    //xcvBuildSpiCmd (confCmd, 0x29, 0x08c00); //18'b00_1000_1100_0000_0000; Bw_sel=001
    xcvBuildSpiCmd (confCmd, 0x29, 0x0a400); //18'b00_1010_0100_0000_0000; Bw_sel=100
    xcvBuildSpiCmd (confCmd, 0x10, 0x088d8); //18'b00_1000_1000_1100_1000; rxpll_reg_div2_bit=1101
#ifdef USE_BB_DC_CANCEL
    xcvBuildSpiCmd (confCmd, 0x2b, 0x0783f);
#endif
    xcvBuildSpiCmd (confCmd, 0x2d, 0x27004); //18'b10_0111_0000_0000_0100; LNA
    xcvBuildSpiCmd (confCmd, 0x3B, 0x00685); //18'b00_0000_0110_1000_0101; mode=slow,time=10(80us),bw=101(60Hz)
    xcvBuildSpiCmd (confCmd, 0x3F, 0x00001); //18'b00_0000_0000_0000_0001; page 1
    hal_RfspiSendCmd(confCmd);

    xcvBuildSpiCmd (confCmd, 0x4C, 0x00ffa); //18'b00_0000_1111_0111_1010; gain table7
    xcvBuildSpiCmd (confCmd, 0x4B, 0x00B6a); //18'b00_0000_1011_0110_0110; gain table6
    xcvBuildSpiCmd (confCmd, 0x4A, 0x00B66); //18'b00_0000_1011_0110_0110; gain table5
    xcvBuildSpiCmd (confCmd, 0x49, 0x00A56); //18'b00_0000_1010_0101_0110; gain table4
    xcvBuildSpiCmd (confCmd, 0x48, 0x00456); //18'b00_0000_0100_0101_0110; gain table3
    xcvBuildSpiCmd (confCmd, 0x47, 0x00006); //18'b00_0000_0000_0000_0110; gain table2
    xcvBuildSpiCmd (confCmd, 0x46, 0x00001); //18'b00_0000_0000_0000_0001; gain table1
    xcvBuildSpiCmd (confCmd, 0x45, 0x00000); //18'b00_0000_0000_0000_0000; gain table0

    hal_RfspiSendCmd(confCmd);

    xcvBuildSpiCmd (confCmd, 0x60, 0x1e927); //18'b01_1110_1001_0010_0111; Lna_1800_i=111
    //xcvBuildSpiCmd (confCmd, 0x70, 0x3ffff); //18'b11_1111_1111_1111_1111; Thermo_diff(0111~0000,1111~1000)
    xcvBuildSpiCmd (confCmd, 0x70, 0x14c40); //18'b01_0100_1100_0100_0000; Thermo_diff(0111~0000,1111~1000)
    //xcvBuildSpiCmd (confCmd, 0x71, 0x3ffff); //18'b11_1111_1111_1111_1111; Thermo_diff
    xcvBuildSpiCmd (confCmd, 0x71, 0x000fd);//18'b00_0000_0000_1111_1101; Thermo_diff temperature control by like 0914
    //xcvBuildSpiCmd (confCmd, 0x77, 0x000f8); //18'b00_0000_0000_1111_1000; Thermo_diff_en=0
    xcvBuildSpiCmd (confCmd, 0x77, 0x00020); //18'b00_0000_0000_0010_0000; Thermo_diff_en=0
    //xcvBuildSpiCmd (confCmd, 0x76, 0x37288);
    xcvBuildSpiCmd (confCmd, 0x3F, 0x00000); //18'b00_0000_0000_0000_0000; page 0

    // ;=====PLL Setting=====
    xcvBuildSpiCmd (confCmd, 0x31, 0x02aac); //18'b00_0010_1010_1010_1100; Equfil_tx_g_gsm_d8
    xcvBuildSpiCmd (confCmd, 0x3F, 0x00001); //18'b00_0000_0000_0000_0001; page 1
    xcvBuildSpiCmd (confCmd, 0x41, 0x0018b); // 18'b00_0000_0001_1000_1011; PLL_850
    xcvBuildSpiCmd (confCmd, 0x42, 0x0017b); // 18'b00_0000_0001_0111_1011; PLL_900
    xcvBuildSpiCmd (confCmd, 0x43, 0x0027b); // 18'b00_0000_0001_0111_1011; PLL_1800
    xcvBuildSpiCmd (confCmd, 0x44, 0x0025b); // 18'b00_0000_0001_0110_1011; PLL_1900
    xcvBuildSpiCmd (confCmd, 0x56, 0x03300); // VCO_ibit
    //xcvBuildSpiCmd (confCmd, 0x62, 0x03fc0); // rfvco_vcobuf
    xcvBuildSpiCmd (confCmd, 0x67, 0x030c5); //18'b00_0011_0000_1100_0101; Equfil_tx_g_gsm_d7
    xcvBuildSpiCmd (confCmd, 0x68, 0x025ee); //18'b00_0010_0101_1110_1110; Equfil_tx_g_gsm_d9
    xcvBuildSpiCmd (confCmd, 0x69, 0x02223); //18'b00_0010_0010_0010_0011; Equfil_tx_g_gsm_d10
    xcvBuildSpiCmd (confCmd, 0x3F, 0x00000); //18'b00_0000_0000_0000_0000; page 0
    xcvBuildSpiCmd (confCmd, 0x39, 0x28000); //close sw
    hal_RfspiSendCmd(confCmd);

    // ;=====IC Self-Cal=====
    xcvBuildSpiCmd (confCmd, 0x3F, 0x00001); //18'b00_0000_0000_0000_0001; page 1
    xcvBuildSpiCmd (confCmd, 0x63, 0x20a35); //18'b10_0000_1010_0011_0101; Dc_cal_ mode=10
    xcvBuildSpiCmd (confCmd, 0x6A, 0x38072); //18'b11_1000_0000_0111_0010; Thermo_cal_polarity=0
    xcvBuildSpiCmd (confCmd, 0x3F, 0x00000); //18'b00_0000_0000_0000_0000; page 0
    xcvBuildSpiCmd (confCmd, 0x30, 0x20517); //18'b10_0000_0101_0001_0111; DigRF=0,Freq_mode=0,Mdll_dr=1,Self-Cal=1
    xcvBuildSpiCmd (confCmd, 0x08, 0x140af); //18'b01_0100_0000_1010_1111; Mdll_div=01,band_sel =00,arfcn=175
    xcvBuildSpiCmd (confCmd, 0x0a, 0x18001); //18'b01_1000_0000_0000_0001; pdn =1

    hal_RfspiSendCmd(confCmd);
    sxr_Sleep(200 MS_WAITING);

    xcvBuildSpiCmd (confCmd, 0x0a, 0x18000); //18'b01_1000_0000_0000_0000; pdn =0

    hal_RfspiSendCmd(confCmd);
    sxr_Sleep(3 MS_WAITING);

    xcvBuildSpiCmd (confCmd, 0x08, 0x11025); //18'b01_0001_0000_0010_0101; Mdll_div=00,band_sel =01,arfcn=37
    xcvBuildSpiCmd (confCmd, 0x0a, 0x18001); //18'b01_1000_0000_0000_0001; pdn =1

    hal_RfspiSendCmd(confCmd);
    sxr_Sleep(200 MS_WAITING);

    xcvBuildSpiCmd (confCmd, 0x0a, 0x18000); //18'b01_1000_0000_0000_0000; pdn =0

    hal_RfspiSendCmd(confCmd);
    sxr_Sleep(3 MS_WAITING);

    xcvBuildSpiCmd (confCmd, 0x08, 0x1229d); //18'b01_0010_0010_1001_1101; Mdll_div=00,band_sel =10,arfcn=669
    xcvBuildSpiCmd (confCmd, 0x0a, 0x18001); //18'b01_1000_0000_0000_0001; pdn =1

    hal_RfspiSendCmd(confCmd);
    sxr_Sleep(200 MS_WAITING);

    xcvBuildSpiCmd (confCmd, 0x0a, 0x18000); //18'b01_1000_0000_0000_0000; pdn =0

    hal_RfspiSendCmd(confCmd);
    sxr_Sleep(3 MS_WAITING);

    xcvBuildSpiCmd (confCmd, 0x08, 0x13276); //18'b01_0011_0010_0111_0110; Mdll_div=00,band_sel =11,arfcn=630
    xcvBuildSpiCmd (confCmd, 0x0a, 0x18001); //18'b01_1000_0000_0000_0001; pdn =1

    hal_RfspiSendCmd(confCmd);
    sxr_Sleep(200 MS_WAITING);

    xcvBuildSpiCmd (confCmd, 0x0a, 0x18000); //18'b01_1000_0000_0000_0000; pdn =0

    hal_RfspiSendCmd(confCmd);
    sxr_Sleep(3 MS_WAITING);

    xcvBuildSpiCmd (confCmd, 0x30, 0x20715); //18'b10_0000_0111_0001_0101; DigRF=1,Freq_mode=0,Mdll_dr=1,Self-Cal=0

    // ;=====IC Debug2=====
    xcvBuildSpiCmd (confCmd, 0x3F, 0x00004); //18'b00_0000_0000_0000_0100; page 4
    xcvBuildSpiCmd (confCmd, 0x11, 0x0c880); //18'b00_1100_1000_1000_0000; Ldo_bypass=1;
    xcvBuildSpiCmd (confCmd, 0x3F, 0x00000); //18'b00_0000_0000_0000_0000; page 0

    // ;=====Mode Setting=====
    xcvBuildSpiCmd (confCmd, 0x1F, 0x3e014); //18'b11_1110_0000_0001_0100;
    xcvBuildSpiCmd (confCmd, 0x3B, 0x00390); // mode=fast-hold,time=10,bw=010
    xcvBuildSpiCmd (confCmd, 0x38, 0x10002); // Rx_Sample_mode=00: 16Bit/sample and 2sample/symbol

#ifdef __RF_RDAPASW_TWOBAND_AS_FOURBAND__
#ifdef __RF_RX3322_RECFG__
    xcvBuildSpiCmd (confCmd, 0x39, 0x286d2); // GPIO_BS_RX RX2_RX2_RX3_RX3:1900~850
#else  //RX2233
    xcvBuildSpiCmd (confCmd, 0x39, 0x2849b); // GPIO_BS_RX RX2_RX2_RX3_RX3:850~1900 //now works
#endif
#else
#ifdef __RF_RX4321_RECFG__
    xcvBuildSpiCmd (confCmd, 0x39, 0x282d4); // GPIO_BS_RX RX1~4:1900~850
#else //RX1234
    xcvBuildSpiCmd (confCmd, 0x39, 0x28899); // GPIO_BS_RX RX1~4:850~1900
#endif
#endif
    //xcvBuildSpiCmd (confCmd, 0x3A, 0x3825b); // GPIO_BS_TX
    xcvBuildSpiCmd (confCmd, 0x3A, 0x0049b); // 18'b00_0000_0100_1011_0110; bit11~9 means gsm850 BS0~2, GPIO_BS_TX //now works
#ifdef USE_BB_DC_CANCEL
    xcvBuildSpiCmd (confCmd, 0x30, 0x30715);
#endif
    hal_RfspiSendCmd(confCmd);
    hal_ClkDisable(xcvClk);
    g_xcvRfIniting = FALSE;
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
        UINT32 bitfield = CDAC_REG_VALUE_BASE | (((g_CDAC + g_CdacDelta)&RDA6220_CDAC_REG_MASK) << RDA6220_CDAC_REG_OFFSET);
        xcvBuildSpiCmd  (confCmd, RDA6220_CDAC_REG, bitfield);
        hal_RfspiSendCmd(confCmd);
        g_lastSleepMode = RFD_POWER_ON;
        pmd_SetEfuseFor_LP(0);
    }
}

// *** xcvSleep (bool toDeepPD) ***
VOID RFD_FUNC_INTERNAL rfd_XcvSleep (RFD_LP_MODE_T lp_mode)
{
    if(RFD_LOW_POWER == lp_mode)
    {
        hal_TcuClrTco(HAL_TCU_TCO_RX_DEC_ON);
        hal_TcuClrTco(XCV->PDN);
    }
    else if(RFD_POWER_OFF== lp_mode)
    {
        INT32 afc_offset_ppm;
        UINT16 step_offset_lp = 0;

        INT16 lp_afcDacOffset = (1<<9)-(g_afcDacVal&0x1ff);
        afc_offset_ppm = ((lp_afcDacOffset) * XCV_PARAM(AFC_GAIN(GSM_BAND_GSM900))) / (1000 * 900);
        afc_offset_ppm += (140*((g_afcDacVal) * XCV_PARAM(AFC_GAIN(GSM_BAND_GSM900)) / 1000)) /( 900*1400);
        step_offset_lp = (afc_offset_ppm * 13) >> 4 ;//afc_offset_ppm *  6.5 / 8
        if (afc_offset_ppm<0)
        {
            step_offset_lp = 0;
        }
        else
        {
            step_offset_lp = (afc_offset_ppm * 13) >> 4 ;//afc_offset_ppm *  6.5 / 8
        }

        // It is safe to send RFSPI commands when ready to enter deep sleep mode
        HAL_RFSPI_CMD_T* confCmd = hal_RfspiNewSingleCmd();
        xcvBuildSpiCmd (confCmd, 0x3F, 0x00003); // page=3
        xcvBuildSpiCmd (confCmd, 0x03, 0x0);
        xcvBuildSpiCmd (confCmd, 0x02, (step_offset_lp)&0xff);
        xcvBuildSpiCmd (confCmd, 0x03, 0x1);    //Step_offset_update
        xcvBuildSpiCmd (confCmd, 0x3F, 0x00000); // page=0
        hal_RfspiSendCmd(confCmd);
        pmd_SetEfuseFor_LP(3);
    }

    g_lastSleepMode = lp_mode;
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
#ifdef RF_LDO_MODE
PUBLIC VOID rfd_XcvFrameInit(VOID)
{
#if 0
    UINT32 status = hal_SysEnterCriticalSection();

#ifdef FM_LDO_WORKAROUND
    // Dcdc piggyback cmds, if any, will be cleared as well
    g_xcvDcdcLdoModeReqStatus = 0;
#endif

    hal_SysExitCriticalSection(status);
#endif
}
#else
PUBLIC VOID rfd_XcvFrameInit(VOID)
{
    UINT32 status = hal_SysEnterCriticalSection();

#ifdef FM_LDO_WORKAROUND
    // Dcdc piggyback cmds, if any, will be cleared as well
    g_xcvDcdcLdoModeReqStatus = 0;
#endif

    hal_SysExitCriticalSection(status);
}
#endif
//======================================================================
// rfd_XcvFrameStart
// This function used to inform XCV driver that an FINT just starts
//----------------------------------------------------------------------
#ifdef RF_LDO_MODE
PUBLIC VOID RFD_FUNC_INTERNAL rfd_XcvFrameStart(VOID)
{
    //UINT32 status = hal_SysEnterCriticalSection();
#if 0
#ifdef FM_LDO_WORKAROUND
    if (g_xcvDcdcLdoModeReqStatus == 2)
    {
        g_xcvDcdcLdoModeReqStatus = 3; // Dcdc piggyback cmds will be sent in this frame
    }
    else if (g_xcvDcdcLdoModeReqStatus == 3)
    {
        g_xcvDcdcLdoModeReqStatus = 4; // Dcdc piggyback cmds have been sent in previous frame
    }
#endif
#endif
    //hal_SysExitCriticalSection(status);
}
#else
PUBLIC VOID RFD_FUNC_INTERNAL rfd_XcvFrameStart(VOID)
{
    //UINT32 status = hal_SysEnterCriticalSection();

#ifdef FM_LDO_WORKAROUND
    if (g_xcvDcdcLdoModeReqStatus == 2)
    {
        g_xcvDcdcLdoModeReqStatus = 3; // Dcdc piggyback cmds will be sent in this frame
    }
    else if (g_xcvDcdcLdoModeReqStatus == 3)
    {
        g_xcvDcdcLdoModeReqStatus = 4; // Dcdc piggyback cmds have been sent in previous frame
    }
#endif

    //hal_SysExitCriticalSection(status);
}
#endif

#ifdef RF_LDO_MODE
PUBLIC VOID rfd_XcvSetDcdcLdoMode(BOOL dcdcOn, BOOL ldoOn)
{
    UINT32 count = 0;
    UINT32 status = hal_SysEnterCriticalSection();

    while (g_xcvRfIniting)
    {
        sxr_Sleep(10 MS_WAITING);
    }

    if (dcdcOn)
    {
        g_RxPllFreqReg_05h |= RX_PLL_FREQ_REG_DCDC_ON;
    }
    else
    {
        g_RxPllFreqReg_05h &= ~RX_PLL_FREQ_REG_DCDC_ON;
    }
    if (ldoOn)
    {
        g_RxPllFreqReg_05h |= RX_PLL_FREQ_REG_LDO_ON;
    }
    else
    {
        g_RxPllFreqReg_05h &= ~RX_PLL_FREQ_REG_LDO_ON;
    }

    // Never directly write to XCV if PAL is working, otherwise it
    // might conflict with XCV Rx/Tx commands when CHANNEL_FREQUENCY_MODE
    // is defined
    while (!(hal_RfspiImmReadWriteAvail() && hal_LpsRemainingFramesReady(2)))
    {
        // Piggyback dcdc commands with XCV Rx commonds
        g_xcvDcdcLdoModeReqStatus = 1;
        count = 0;
        while (count++ < MAX_PIGGYBACK_WAIT_COUNT)
        {
            if (count == 1)
            {
                sxr_Sleep(10 MS_WAITING);
            }
            else
            {
                sxr_Sleep(5 MS_WAITING);
            }

            if (g_xcvDcdcLdoModeReqStatus == 4)
            {
                // Dcdc piggyback commands have been sent to XCV
                hal_SysExitCriticalSection(status);
                return;
            }
        }
        // Cancel dcdc piggyback commands
        g_xcvDcdcLdoModeReqStatus = 0;
    }

    // Now we can directly write to XCV
    rfd_XcvRegForceWriteSingle(0x05, g_RxPllFreqReg_05h);

    hal_SysExitCriticalSection(status);
}
#else
#ifdef FM_LDO_WORKAROUND
PUBLIC VOID rfd_XcvSetDcdcLdoMode(BOOL dcdcOn, BOOL ldoOn)
{
    UINT32 count = 0;
    UINT32 status = hal_SysEnterCriticalSection();

    while (g_xcvRfIniting)
    {
        sxr_Sleep(10 MS_WAITING);
    }

    if (dcdcOn)
    {
        g_RxPllFreqReg_05h |= RX_PLL_FREQ_REG_DCDC_ON;
    }
    else
    {
        g_RxPllFreqReg_05h &= ~RX_PLL_FREQ_REG_DCDC_ON;
    }
    if (ldoOn)
    {
        g_RxPllFreqReg_05h |= RX_PLL_FREQ_REG_LDO_ON;
    }
    else
    {
        g_RxPllFreqReg_05h &= ~RX_PLL_FREQ_REG_LDO_ON;
    }

    // Never directly write to XCV if PAL is working, otherwise it
    // might conflict with XCV Rx/Tx commands when CHANNEL_FREQUENCY_MODE
    // is defined
    while (!(hal_RfspiImmReadWriteAvail() && hal_LpsRemainingFramesReady(2)))
    {
        // Piggyback dcdc commands with XCV Rx commonds
        g_xcvDcdcLdoModeReqStatus = 1;
        count = 0;
        while (count++ < MAX_PIGGYBACK_WAIT_COUNT)
        {
            if (count == 1)
            {
                sxr_Sleep(10 MS_WAITING);
            }
            else
            {
                sxr_Sleep(5 MS_WAITING);
            }

            if (g_xcvDcdcLdoModeReqStatus == 4)
            {
                // Dcdc piggyback commands have been sent to XCV
                hal_SysExitCriticalSection(status);
                return;
            }
        }
        // Cancel dcdc piggyback commands
        g_xcvDcdcLdoModeReqStatus = 0;
    }

    // Now we can directly write to XCV
    rfd_XcvRegForceWriteSingle(0x05, g_RxPllFreqReg_05h);

    hal_SysExitCriticalSection(status);
}
#endif
#endif

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
        UINT32 bitfield = CDAC_REG_VALUE_BASE | ((g_CDAC&RDA6220_CDAC_REG_MASK) << RDA6220_CDAC_REG_OFFSET);
        xcvBuildSpiCmd (spiCmd, RDA6220_CDAC_REG, bitfield);
    }
    int ramp_num = 0;
    for(ramp_num = 0; ramp_num < 23; ramp_num++)
    {
        rfd_xcvRampSet(spiCmd,ramp_num);
    }
    g_xcvCalibPaOffset = TRUE;
    // Reset Xtal calib state
    xtal_state = GUESS_CDAC;
    // Reset iloss calib state
    g_rfdILossInit = TRUE;
    return;
}

UINT8 rfd_XcvCalibXtalFreqOffset(INT32 FOf)
{

    // 1 CDAC unit =~ 1900Hz
#define CDAC_UNIT 1900
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
    UINT32 bitfield = CDAC_REG_VALUE_BASE | ((g_CDAC&RDA6220_CDAC_REG_MASK) << RDA6220_CDAC_REG_OFFSET);
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
    g_xcvCmdCtx.digAfc = 0x14000 | (g_afcDacVal & RDA6220_AFC_MASK);
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
    UINT32 bitfield = CDAC_REG_VALUE_BASE | (((g_CDAC+RDA6220_CDAC_OFFSET)&RDA6220_CDAC_REG_MASK) << RDA6220_CDAC_REG_OFFSET);
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
    g_xcvCmdCtx.digAfc = 0x14000 | (g_afcDacVal & RDA6220_AFC_MASK);
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

PUBLIC VOID rfd_XcvCalibSetPaRamp (UINT16* dacValue)
{
    g_xcvCalibSetPaRamp = TRUE;
    int i = 0;
    for(i = 0; i < 4; i++)
        g_xcvCalibPaRampValue[i] = dacValue[i];
}


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

#define XCV_REG_PAGE_NUM (5)
#define XCV_REG_NUM_PER_PAGE (0x40)
UINT32 XCV_REG_VALUE_LOCATION g_rfdXcvRegValue[XCV_REG_PAGE_NUM][XCV_REG_NUM_PER_PAGE];

UINT32 g_rfdXcvRegForceWriteError = 0;
UINT32 g_rfdXcvRegForceReadError = 0;

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

PROTECTED BOOL rfd_XcvRegForceWrite(UINT32 page, UINT32 addr, UINT32 data)
{
    BOOL result = FALSE;
    page &= 0x7;
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
    page &= 0x7;
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
    UINT32 residual = count++ % (XCV_REG_PAGE_NUM+1);
    if (residual < 2)
    {
        // page 0 takes the precedence
        rfd_XcvRegCheckSinglePage(0);
    }
    else if (residual == 2)
    {
        rfd_XcvRegCheckSinglePage(1);
    }
    else if (residual == 3)
    {
        rfd_XcvRegCheckSinglePage(2);
    }
    else if (residual == 4)
    {
        rfd_XcvRegCheckSinglePage(3);
    }
    else // residual == 5
    {
        rfd_XcvRegCheckSinglePage(4);
    }
#endif
}

PUBLIC VOID rfd_RegisterXcvRegCheckFunc(VOID)
{
    static BOOL registerIdleHook = FALSE;
    if (!registerIdleHook)
    {
#ifdef XCV_RAMP_DEBUG
        m_ParampDebugflag = TRUE;
#endif
        registerIdleHook = TRUE;
        sxs_RegisterDebugIdleHookFunc(&rfd_XcvRegCheck);
    }
}
#endif // SPI_REG_DEBUG

