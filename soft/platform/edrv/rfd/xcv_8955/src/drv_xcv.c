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
#include "calib_m.h"


#if defined(_FLASH_PROGRAMMER) || defined(_T_UPGRADE_PROGRAMMER)
#undef SPI_REG_DEBUG
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
VOID xcvBuildSpiCmd (HAL_RFSPI_CMD_T *cmd, UINT16 address, UINT32 data);
VOID xcvSetArfcn (RFD_RFDIR_T dir, GSM_RFBAND_T band, UINT16 arfcn);
VOID xcvReadReg (UINT16 address, UINT8 *output);

PROTECTED VOID rfd_XcvRegForceWriteSingle(UINT32 addr, UINT32 data);

INLINE VOID xcvConfigure(VOID);
CONST XCV_CONFIG_T* RFD_DATA_INTERNAL XCV = NULL;
#ifdef XCV_RAMP_DEBUG
BOOL RFD_DATA_INTERNAL m_ParampDebugflag = FALSE;
#endif

#ifdef VOC_CLOCK_WORKAROUND
#define VOC_CLOCK_WORKROUND_FREQ 208000000
BOOL g_rfdVocClkFlag;
HAL_SYS_VOC_FREQ_T g_rfdOldVocFreq =HAL_SYS_VOC_FREQ_104M ;
#endif


//--------------------------------------------------------------------//
//                                                                    //
//                        Internal Structures                         //
//                                                                    //
//--------------------------------------------------------------------//

// GLOBAL variables
HAL_SYS_CHIP_METAL_ID_T RFD_DATA_INTERNAL g_xcvChipId = 0;
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

// RF Commands Context
XCV_COMMANDS_T RFD_DATA_INTERNAL g_xcvCmdCtx = {0, 0, 0};

BOOL RFD_DATA_INTERNAL g_xcvCalibSetPaRamp = FALSE;
BOOL RFD_DATA_INTERNAL g_xcvCalibPaOffset = TRUE;
UINT16 RFD_DATA_INTERNAL g_xcvCalibPaRampValue[4];

#ifdef TXPOWER_TEMPERATURE_OFFSET
UINT16 RFD_DATA_INTERNAL g_tempgpadcvalue[5] = {0,};
#endif

UINT32 RFD_DATA_INTERNAL g_RxPllFreqReg_05h = 0xa654; //0x3a654
VOLATILE BOOL g_xcvRfIniting = FALSE;

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

VOID RFD_FUNC_INTERNAL xcvBuildSpiCmd (HAL_RFSPI_CMD_T *cmd, UINT16 address, UINT32 data)
{
    UINT8 array[RDA6220_ONE_CMD_BYTE_QTY];
    UINT32 cmdWord = RDA6220_WRITE_FMT(address, data);

#ifdef SPI_REG_DEBUG
#ifdef XCV_REG_BUFFER_READ_WRITE
    if (address < XCV_REG_NUMBER)
    {
        g_rfdXcvRegLastWriteValue[address] = data&XCV_REG_DATA_MASK;
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
    if(upordown) {
          static INT8 m_Rxleveloffset[4][10] =
          {
               {0,1,0,0,0,0,0,2,2,1},
               {0,1,0,0,0,0,0,2,2,2},
               {0,-1,0,0,0,0,0,2,1,1},
               {1,1,0,0,0,0,0,2,1,1}
           };
         if(dacvalue > -1 && dacvalue <11)
                  ret_value = m_Rxleveloffset[band][0];
         else if(dacvalue > 10 && dacvalue < 19)
                  ret_value = m_Rxleveloffset[band][1];
         else if(dacvalue > 18 && dacvalue <25)
                  ret_value = m_Rxleveloffset[band][2];
         else if(dacvalue > 24 && dacvalue < 33)
                  ret_value = m_Rxleveloffset[band][3];
         else if(dacvalue > 32 && dacvalue < 39)
                 ret_value = m_Rxleveloffset[band][4];
         else if(dacvalue > 38 && dacvalue < 45)
                 ret_value = m_Rxleveloffset[band][5];
         else if(dacvalue > 44 && dacvalue < 51)
                ret_value = m_Rxleveloffset[band][6];
         else if(dacvalue > 50 && dacvalue <55)
                ret_value = m_Rxleveloffset[band][7];
         else if(dacvalue > 54 && dacvalue < 61)
                ret_value = m_Rxleveloffset[band][8];
         else if(dacvalue > 60&& dacvalue < 64)
                 ret_value = m_Rxleveloffset[band][9];
         else
                ret_value = 0;
    }
    else {
         static INT32 m_Poweroffset[4][3] =
        {
               {56, 1432, -120},
               {56, 1432, -120},
               {30, 1228, -60},
               {30, 1228, -60}
         };
        if(dacvalue >-1 && dacvalue < 64) 
                ret_value = ((m_Poweroffset[band][0] * dacvalue *dacvalue + m_Poweroffset[band][1] * dacvalue )>>10)+ m_Poweroffset[band][2];
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
    UINT32 freqReg =0x0;
    if (g_xcvChipId >= HAL_SYS_CHIP_METAL_ID_U02)
    {
        freqReg = 0x18000; //divider=9
    }
    else
    {
        freqReg = 0x10000; //divider=7
    }

    if (dir == RFD_DIR_TX)
    {
        if (g_xcvChipId >= HAL_SYS_CHIP_METAL_ID_U02)
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
                    if (arfcn > 604 && arfcn < 631)
                    {
                        freqReg = 0x1c000; //divider=10
                    }
                    break;
                default:
                    break;
            }
        }
        else
        {
            switch (band)
            {
                case GSM_BAND_GSM850:
                    if (arfcn > 211 && arfcn < 221)
                    {
                        freqReg = 0x14000; //divider=8
                    }
                    break;
                case GSM_BAND_GSM900:
                    if (arfcn > 95 && arfcn < 105)
                    {
                        freqReg = 0x18000;//divider=9
                    }
                    else if (arfcn > 1005 && arfcn < 1015)
                    {
                        freqReg = 0x14000;
                    }
                    break;
                case GSM_BAND_DCS1800:
                    if (arfcn > 601 && arfcn < 611)
                    {
                        freqReg = 0x14000;
                    }
                    break;
                default:
                    break;
            }
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
       #ifdef CHIP_XTAL_CLK_IS_52M
             if(hal_SysDdrIsHighFreq())
	#endif
             {
                   iLoss += PowerOffset(band, g_tempgpadcvalue[4], 1);
              }
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
    xcvBuildSpiCmd (spiCmd, 0x17, 0x033a8);

    xcvBuildSpiCmd (spiCmd, RDA6220_FREQ_CTRL_REG, g_xcvCmdCtx.freqCtrl);

    xcvBuildSpiCmd (spiCmd, RDA6220_AFC_REG, g_xcvCmdCtx.digAfc);
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
    //  xcvBuildSpiCmd (spiCmd, 0x3f, 0x00001); //
    xcvBuildSpiCmd (spiCmd, 0x7b, (((i_dctemp&0x7f)<<10)|(Q_Dcoffset[anagain]&0x3ff))|0x20000);
    xcvBuildSpiCmd (spiCmd, 0x7a, ((I_Dcoffset[anagain]>>7)&0x7)|0x38);

//    xcvBuildSpiCmd (spiCmd, 0x3f, 0x00000); //
#else
    xcvBuildSpiCmd (spiCmd, RDA6220_RX_GAIN_REG, g_xcvCmdCtx.gain);
#endif
#if (XCV_624M_PLL_DITHER_ENABLE)
    if (Win->band == GSM_BAND_GSM900 && (Win->arfcn == 5 || Win->arfcn == 70))
    {
        if (!g_Xcv624MPllDitherEnabled)
        {
            xcvBuildSpiCmd (spiCmd, 0x107, 0x00c01); // ss_en=1;ss_squre=1
            xcvBuildSpiCmd (spiCmd, 0x10b, 0x00c01); // ss_en=1;ss_squre=1
            g_Xcv624MPllDitherEnabled = TRUE;
        }
    }
    else
    {
        if (g_Xcv624MPllDitherEnabled)
        {
            xcvBuildSpiCmd (spiCmd, 0x107, 0x00001); // ss_en=0;ss_squre=0
            xcvBuildSpiCmd (spiCmd, 0x10b, 0x00001); // ss_en=0;ss_squre=0
            g_Xcv624MPllDitherEnabled = FALSE;
        }
    }
#endif // XCV_624M_PLL_DITHER_ENABLE


#ifdef USE_BB_DC_CANCEL
    return XcvGain -(m_getdiggain - *xcvDigLevel );
#else
    return XcvGain;
#endif

}

// *** xcvBuildTxSpiCmd  ***
// Prepare the transceiver commands for emission
INLINE
VOID RFD_FUNC_INTERNAL xcvBuildTxSpiCmd (RFD_WIN_T *Win, UINT16 arfcn, GSM_RFBAND_T band, HAL_RFSPI_CMD_T *spiCmd)
{
    UINT8 Winbitmap = (Win->bitmap & 0xf);
    UINT16 nbSlots = (Winbitmap & 0x08)? 4:
                     (Winbitmap & 0x4)? 3:
                     (Winbitmap & 0x2)? 2: 1;
    int count = 0;
    int pclValue = Win->gain[count];

    UINT8  TabRampReg[4] = {RDA6220_TX_RAMP_0_REG, RDA6220_TX_RAMP_1_REG,
                            RDA6220_TX_RAMP_2_REG, RDA6220_TX_RAMP_3_REG
                           };
    UINT16 rampFactor[4] = {0};
    UINT8 WinBitMap = Win->bitmap;

 #ifdef TXPOWER_TEMPERATURE_OFFSET   
 if(band == GSM_BAND_PCS1900){
       if(g_tempgpadcvalue[4]>55){
	     xcvBuildSpiCmd (spiCmd, 0x10, 0x88fc);
	     xcvBuildSpiCmd (spiCmd, 0x56, 0x880);
     	 }
        else{
	     xcvBuildSpiCmd (spiCmd, 0x10, 0x88D8);
	     xcvBuildSpiCmd (spiCmd, 0x56, 0x2200);
     	  }
  }
#endif

    // Set DigitalAfc, 14 bit is set for digital AFC
    g_xcvCmdCtx.digAfc = 0x14000 | (g_afcDacVal & RDA6220_AFC_MASK);
    UINT16 adjustedCdac = g_CDAC + g_CdacDelta;
    xcvBuildSpiCmd (spiCmd, RDA6220_CDAC_REG, CDAC_REG_VALUE_BASE | ((adjustedCdac&RDA6220_CDAC_REG_MASK) << RDA6220_CDAC_REG_OFFSET));

    // Set Band & arfcn
    xcvSetArfcn (RFD_DIR_TX, band, arfcn);
    xcvBuildSpiCmd (spiCmd, RDA6220_FREQ_CTRL_REG, g_xcvCmdCtx.freqCtrl);
    xcvBuildSpiCmd (spiCmd, RDA6220_AFC_REG, g_xcvCmdCtx.digAfc);

#ifdef XCV_PLL_SDM_MODE
    xcvBuildSpiCmd (spiCmd, 0x35, 0x30000);
#endif
    g_xcvCmdCtx.digAfc = 0x14000 | (g_afcDacVal & RDA6220_AFC_MASK);
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


    int curvernum = 0;
    int curvernum_final = 0;
    CONST UINT16 *pDacTable;
    INT16 dBmMax, dBmMin, dBmValue;
    int dBmOffset;
    INT32 dacValue;

    CONST UINT16 DAC_MAX = RDA6220_RAMP_FACTOR_MASK;
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

            dBmValue = Pcl2dBm(band, arfcn, pclValue);

    #ifdef TXPOWER_TEMPERATURE_OFFSET   
	   if(!g_xcvCalibPaOffset)
	   { 
	        #ifdef CHIP_XTAL_CLK_IS_52M
		       if(hal_SysDdrIsHighFreq())
               #endif
		      {
	                   if(((band < 2) && (pclValue > 5)) || ((band > 1) && (pclValue > 0)))
                                dBmValue += PowerOffset(band, g_tempgpadcvalue[4], 0);
		      }
	   }
      #endif 
	  
            if (dBmValue < dBmMin) dBmValue = dBmMin;
            else if (dBmValue > dBmMax) dBmValue = dBmMax;
            dBmOffset = 463 * (dBmValue-dBmMin) / (dBmMax - dBmMin);
            dacValue = (pDacTable[dBmOffset]  > DAC_MAX) ? DAC_MAX :  pDacTable[dBmOffset];
		   
    #ifdef TXPOWER_TEMPERATURE_OFFSET
            if(!g_xcvCalibPaOffset)
            {
                #ifdef CHIP_XTAL_CLK_IS_52M
		        if(hal_SysDdrIsHighFreq())
		  #endif 
		       {
                             if(((band < 2) && (5 == pclValue)) || ((band > 1) && (0 ==pclValue )))
                            {
                                     INT16 dBmValue1 = Pcl2dBm(band, arfcn, pclValue +1);
                                     dBmOffset = 463 * (dBmValue1-dBmMin) / (dBmMax - dBmMin);
                                     INT16 dacValue1 = (pDacTable[dBmOffset]  > DAC_MAX) ? DAC_MAX :  pDacTable[dBmOffset];
                                     dacValue += ((dacValue - dacValue1)*(INT32)PowerOffset(band, g_tempgpadcvalue[4], 0)/200);
                                    dacValue = (dacValue  > DAC_MAX) ? DAC_MAX :  dacValue;
                            }
		      }
            }
     #endif
			
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


    if (count > 0)
    {
        // Ramp_mode_sel_gsm=0
        rampFactor[0] |= 0x00000 | (((count-1) & 0x3) << 13);
        curvernum = 0x0ffff & curvernum_final;
        xcvBuildSpiCmd (spiCmd, 0x92, curvernum);
        RFD_TRACE(RFD_INFO_TRC,0,"curvernum @ %d",curvernum);
        for(; count>0; count--)
        {
            xcvBuildSpiCmd (spiCmd, TabRampReg[count-1], rampFactor[count-1]);
        }
    }

#if (XCV_624M_PLL_DITHER_ENABLE)
    if (g_Xcv624MPllDitherEnabled)
    {
        xcvBuildSpiCmd (spiCmd, 0x107, 0x00001); // ss_en=0;ss_squre=0
        xcvBuildSpiCmd (spiCmd, 0x10b, 0x00001); // ss_en=0;ss_squre=0
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
#ifdef XCV_WAKEUP_FRAME_NB
    .wakeUpFrameNbr = 0, // activating low power optimisation
#else
    .wakeUpFrameNbr = 1, // activating low power optimisation
#endif
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

const HAL_RFSPI_CFG_T HAL_BOOT_DATA_INTERNAL g_spiCfg =
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
    27, // frameSize
    17, // inputFrameSize
};

#ifdef VOC_CLOCK_WORKAROUND
PRIVATE BOOL rfd_GetVocClkWorkroundFlag(GSM_RFBAND_T band, UINT16 arfcn)
{
    if(!hal_VocStateActive())
        return FALSE;

    if(hal_SysGetVocClock() > HAL_SYS_VOC_FREQ_208M)
        return FALSE;


    if (band == GSM_BAND_GSM850)
    {
        if( ((arfcn > 195)&&(arfcn < 201 )) ||((arfcn > 211)&&(arfcn < 217 )) ||((arfcn > 236)&&(arfcn < 242 )))
        {
            return TRUE;
        }
    }
    else if (band == GSM_BAND_GSM900)
    {
        if(arfcn == 5)
        {
            return TRUE;
        }
    }
    else if (band == GSM_BAND_DCS1800)
    {
        if(arfcn == 846)
        {
            return TRUE;
        }
    }
    else if (band == GSM_BAND_PCS1900)
    {
        if(arfcn == 711)
        {
            return TRUE;
        }
    }


    return FALSE;
}
#endif

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

#ifdef VOC_CLOCK_WORKAROUND
    g_rfdVocClkFlag= rfd_GetVocClkWorkroundFlag(Win->band, Win->arfcn);
    if(g_rfdVocClkFlag)
    {
        g_rfdOldVocFreq = hal_SysSetVocClock(VOC_CLOCK_WORKROUND_FREQ);
    }

#endif

    RFD_TRACE(RFD_INFO_TRC,0,"xcvRxOn @ %d Qb",start);

    return XcvGain;
}


VOID RFD_FUNC_INTERNAL rfd_XcvRxOff(RFD_WIN_T *Win,INT16 stop)
{
    // Transceiver in PowerDown
    hal_TcuSetEvent (XCV_CLR_PDN, stop + XCV_TIME(PDN_RX_DN));
    // stop the recording
    hal_TcuSetEvent (HAL_TCU_STOP_RFIN_RECORD, stop+ XCV_TIME(PDN_RX_DN));

#ifdef VOC_CLOCK_WORKAROUND
    if(g_rfdVocClkFlag)
    {
        hal_SysSetVocClock(g_rfdOldVocFreq);
        g_rfdVocClkFlag = FALSE;
    }
#endif

    RFD_TRACE(RFD_INFO_TRC,0,"xcvRxOff @ %d Qb",stop);
}

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
#ifdef PASW_MODEM
    INT8 m_strobtime[4][2][16]=
    {
        {
            {0,0,0,-1,-1,-1,0,0,0,0,0,-1,-1,-1,-3,-1},
            {1,1,2,1,1,1,1,1,1,1,1,1,2,3,3,0}
        },
        {
            {0,0,0,-1,-1,-1,1,0,0,1,0,0,-1,-1,-2,-2},
            {0,1,1,0,1,1,1,1,1,1,1,2,2,2,2,-1}
        },
        {
            {1,0,-1,-2,-1,-1,-1,1,1,1,0,-1,-1,-2,-2,-2},
            {2,2,2,2,0,0,0,0,-1,0,0,0,1,1,1,1}
        },
        {
            {0,-1,-1,-2,0,-1,-1,1,1,1,1,0,-1,-1,-2,-2},
            {2,2,2,2,0,0,1,-1,-1,0,-1,2,1,2,2,2}
        }
    };
#else
    INT8 m_strobtime[4][2][16]=
    {
        {
            {1,0,0,-1,-1,-1,0,0,0,0,0,0,-2,-3,-4,-1},
            {1,0,1,1,1,0,1,1,1,1,1,1,2,3,3,0}
        },
        {
            {1,0,0,-1,-1,-1,0,0,0,0,1,0,-2,-2,-3,-2},
            {0,1,1,0,1,0,1,1,1,1,1,2,2,2,2,-1}
        },
        {
            {0,-1,-2,-2,-1,-1,-1,1,1,1,0,-1,-2,-3,-2,-2},
            {1,2,2,2,0,0,0,0,-1,0,0,0,1,1,1,1}
        },
        {
            {0,-1,-1,-2,0,-1,-1,1,1,0,0,-1,-2,-1,-2,-2},
            {0,1,1,1,0,0,1,-1,-1,0,-1,2,1,2,2,2}
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
                    hal_TcuSetEvent (HAL_TCU_DIGRF_STB, start + XCV_TIME(TX_STROBE_ON) -6);
                else if(nbSlots == 4 && i == 2)
                    hal_TcuSetEvent (HAL_TCU_DIGRF_STB, start + XCV_TIME(TX_STROBE_ON) -6);
                else if(nbSlots == 4 && i == 1)
                    hal_TcuSetEvent (HAL_TCU_DIGRF_STB, start + XCV_TIME(TX_STROBE_ON) -6);
                else if(nbSlots == 3 && i == 2)
                    hal_TcuSetEvent (HAL_TCU_DIGRF_STB, start + XCV_TIME(TX_STROBE_ON) -6);
                else if(nbSlots == 3 && i == 1)
                    hal_TcuSetEvent (HAL_TCU_DIGRF_STB, start + XCV_TIME(TX_STROBE_ON) -4);
                else if(nbSlots == 2 && i == 1)
                    hal_TcuSetEvent (HAL_TCU_DIGRF_STB, start + XCV_TIME(TX_STROBE_ON) -4);
                else if(nbSlots == 1)
                    hal_TcuSetEvent (HAL_TCU_DIGRF_STB, start + XCV_TIME(TX_STROBE_ON) + m_strobtime[Win->band][0][(Win->band < 2)?(Win->gain[i]-5):Win->gain[i]]);
                else
                    hal_TcuSetEvent (HAL_TCU_DIGRF_STB, start + XCV_TIME(TX_STROBE_ON));
            }
            else
            {
                hal_TcuSetEvent (HAL_TCU_DIGRF_STB, start + XCV_TIME(TX_STROBE_ON) + m_strobtime[Win->band][0][0]);
            }
            stop = ((Win->type == GSM_WIN_TYPE_TXABURST)?
                    (start + GSM_ABURST_ACTIVE_QS_DURATION + 4*g_xcvParams.txBstMap.postamble):
                    (start + GSM_NBURST_ACTIVE_QS_DURATION + 4*g_xcvParams.txBstMap.postamble));
            if (!ramp_flag)
            {
                if(nbSlots == 4 && i == 3)
                    hal_TcuSetEvent (HAL_TCU_DIGRF_STB, stop + XCV_TIME(TX_STROBE_OFF) -2);
                else if(nbSlots == 4 && i == 2)
                    hal_TcuSetEvent (HAL_TCU_DIGRF_STB, stop + XCV_TIME(TX_STROBE_ON)-3 );
                else if(nbSlots == 4 && i == 1)
                    hal_TcuSetEvent (HAL_TCU_DIGRF_STB, stop + XCV_TIME(TX_STROBE_ON) -2);
                else if(nbSlots == 3 && i == 2)
                    hal_TcuSetEvent (HAL_TCU_DIGRF_STB, stop + XCV_TIME(TX_STROBE_OFF) -3);
                else if(nbSlots == 3 && i == 1)
                    hal_TcuSetEvent (HAL_TCU_DIGRF_STB, stop + XCV_TIME(TX_STROBE_OFF) -1);
                else if(nbSlots == 2 && i ==1)
                    hal_TcuSetEvent (HAL_TCU_DIGRF_STB, stop + XCV_TIME(TX_STROBE_OFF) -1);
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
     if(!(tempertureloop++ & 31))
     {
            #define GPADC_BAD_VALUE    0xFF
              UINT16 dacvalue =hal_SysGetThermal_cal();
             if(GPADC_BAD_VALUE != dacvalue)
            {
                   static UINT8 loop = 0;
                   g_tempgpadcvalue[loop & 3] = dacvalue;
                   if(++loop == 0xff)
                          loop = 4;
                   if(loop > 3)  
                          g_tempgpadcvalue[4] = (g_tempgpadcvalue[0]+g_tempgpadcvalue[1]+g_tempgpadcvalue[2]+g_tempgpadcvalue[3])>>2;
                   else
                          g_tempgpadcvalue[4] = dacvalue;
             }
     	}
#endif

    INT32 DacOffset = (1000*FOf) / XCV_PARAM(AFC_GAIN(band));
    g_afcDacVal += DacOffset;
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
        g_xcvChipId = hal_SysGetMetalId();
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
INLINE VOID xcvConfigure(VOID)
{
    HAL_RFSPI_CMD_T* confCmd = hal_RfspiNewSingleCmd();

    // Most of initialization are moved to boot_XcvConfigure

    if (g_xcvCalibPtr != NULL)
    {
#ifdef MULTI_BANDWIDE
        rxTxTimeOffset = g_xcvCalibPtr->xcv->palcust.rxTxTimeOffset;
#endif
        INT16 lp_CDAC = g_CDAC + g_CdacDelta;
        if(1)
        {
            lp_CDAC+=0;
        }
        else
            lp_CDAC-=8;
        //lp_CDAC -= 3;

        if (lp_CDAC < 0)
            lp_CDAC = 0;
        else if(lp_CDAC > 64)
            lp_CDAC = 64;

        // LP current and LP CDAC
        xcvBuildSpiCmd (confCmd, 0xc0, 0x04| ((lp_CDAC&RDA6220_CDAC_REG_MASK) << RDA6220_CDAC_REG_OFFSET));   //c2
        hal_RfspiSendCmd(confCmd);
    }
}

VOID xcv_SetStableVolt(UINT16 volt)
{
    return;
}
PUBLIC VOID rfd_XcvLimitMaxTxPower(BOOL on)
{
    return;
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
    if (g_xcvCalibPtr != NULL)
    {
        g_CDAC = RDA6220_CDAC_DEFAULT;
        g_AfcOffset = AfcOffset;
        if (g_AfcOffset > AGING_DAC_LIMIT)
        {
            g_CDAC += (g_AfcOffset/AGING_DAC_LIMIT);
        }
        else if (g_AfcOffset < AGING_DAC_LIMIT)
        {
            g_CDAC -= (g_AfcOffset/AGING_DAC_LIMIT);
        }

        g_afcDacVal = RDA6220_AFC_MID;
        g_CdacDelta = 0;
    }

    // Open RF Spi, ask for a 19.5MHz spi clock (Max speed)
    hal_RfspiOpen((HAL_RFSPI_CFG_T*)&g_spiCfg, RFSPI_CLOCK);

    xcvConfigure();
}
//-----------------------------------//
// Open/Close & Low Power Functions  //
//-----------------------------------//


INLINE VOID rfd_xcvRampSet(HAL_RFSPI_CMD_T* confCmd ,UINT16 ramp_num)
{
    for(int ramp_point = 0; ramp_point < 16; ramp_point++)
        xcvBuildSpiCmd (confCmd, 0x3d, ((15-ramp_point) | (0x01f0&(ramp_num << 4)) | (g_xcvCalibPtr->pa->palcust.rampCurve[ramp_num][ramp_point]<<9)));
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

    // ;=====DCXO Setting=====
    UINT16 adjustedCdac = g_CDAC + g_CdacDelta;
    xcvBuildSpiCmd (confCmd, RDA6220_CDAC_REG, CDAC_REG_VALUE_BASE| ((adjustedCdac&RDA6220_CDAC_REG_MASK) << RDA6220_CDAC_REG_OFFSET));

    // ;=====IC Debug=====
    xcvBuildSpiCmd (confCmd, RDA6220_FREQ_CTRL_MDLL_REG, 0x12464); // mdll band sel=1,mdll_band_reg_dr=0
    xcvBuildSpiCmd (confCmd, 0x14, 0x081e4); //18'b10_1000_0001_1110_0100; dac_bias=110,Dac_range=11,dac_cm=00  //edit 120314
    xcvBuildSpiCmd (confCmd, 0x17, 0x033a8); //18'b00_0011_0011_1010_1000; dac_rst_time=11
#ifdef USE_BB_DC_CANCEL
    xcvBuildSpiCmd (confCmd, 0x18, 0x0314c); //18'b00_0011_0101_0100_1100; adc_reset_delay=14CH(filter reset)
#else
    xcvBuildSpiCmd (confCmd, 0x18, 0x0354c); //18'b00_0011_0101_0100_1100; adc_reset_delay=14CH(filter reset)
#endif
    xcvBuildSpiCmd (confCmd, 0x1B, 0x29155); //18'b10_1001_0001_0101_0101; thmt_dac_ref
    xcvBuildSpiCmd (confCmd, 0x1C, 0x39122); //18'b11_1001_0001_0010_0010; Thmt_ptat_var=1100
    xcvBuildSpiCmd (confCmd, 0x0f, 0x28284); //18'b10_1000_0010_1000_0100; for phase error
    hal_RfspiSendCmd(confCmd);

    // ;=====Tx Setting=====
    xcvBuildSpiCmd (confCmd, 0x36, 0x10f02);
    xcvBuildSpiCmd (confCmd, 0x02, 0x2ab55); //18'b10_1010_1011_0101_0101; Digrf_stream_mode_enable=1
    xcvBuildSpiCmd (confCmd, 0x03, 0x00320); //18'b00_0000_0011_0010_0000; Ramp_curve,Ramp_mult_factor
    xcvBuildSpiCmd (confCmd, 0x28, 0x24602); //18'b10_0100_0110_0000_0010; equfil_tx_bypass=1,TX_diff_en=1

    // Tx timing control
    xcvBuildSpiCmd (confCmd, 0x24, 0x20740); // 18'b10_0000_0111_0100_0000; 17:16 txpower start delay 2

    hal_RfspiSendCmd(confCmd);
//#if 0
    int ramp_num = 0;
    for(ramp_num = 0; ramp_num < 24; ramp_num++)
    {
        rfd_xcvRampSet(confCmd,ramp_num);
    }
//#endif
    // ;=====RX Setting=====
    xcvBuildSpiCmd (confCmd, 0x01, 0x3c036); //18'b11_1100_0000_0011_0110; rx gain set
    xcvBuildSpiCmd (confCmd, 0x12, 0x110f0); //18'b01_0001_0000_1111_0000; filter tuning_bit=1000
    xcvBuildSpiCmd (confCmd, 0x13, 0x01241); //18'b00_0001_0010_0100_0001;
    xcvBuildSpiCmd (confCmd, 0x1a, 0x26e5c); //18'b10_0110_1110_0101_1100; Enlarge auxclk amp
    xcvBuildSpiCmd (confCmd, 0x29, 0x0a400); //18'b00_1010_0100_0000_0000; Bw_sel=100
    xcvBuildSpiCmd (confCmd, 0x10, 0x088d8); //18'b00_1000_1000_1100_1000; rxpll_reg_div2_bit=1101
#ifdef USE_BB_DC_CANCEL
    xcvBuildSpiCmd (confCmd, 0x2b, 0x0783f);
#endif
    xcvBuildSpiCmd (confCmd, 0x2d, 0x27004); //18'b10_0111_0000_0000_0100; LNA
    xcvBuildSpiCmd (confCmd, 0x3B, 0x00685); //18'b00_0000_0110_1000_0101; mode=slow,time=10(80us),bw=101(60Hz)
    hal_RfspiSendCmd(confCmd);

    xcvBuildSpiCmd (confCmd, 0x94, 0x0e400);
    xcvBuildSpiCmd (confCmd, 0x4C, 0x24ffa); //18'b00_0000_1111_0111_1010; gain table7
    xcvBuildSpiCmd (confCmd, 0x4B, 0x04B6a); //18'b00_0000_1011_0110_0110; gain table6
    xcvBuildSpiCmd (confCmd, 0x4A, 0x04B66); //18'b00_0000_1011_0110_0110; gain table5
    xcvBuildSpiCmd (confCmd, 0x49, 0x02666); //18'b00_0000_1010_0101_0110; gain table4
    xcvBuildSpiCmd (confCmd, 0x48, 0x01256); //18'b00_0000_0100_0101_0110; gain table3 //0x00066
    xcvBuildSpiCmd (confCmd, 0x47, 0x01202); //18'b00_0000_0000_0000_0110; gain table2 // 0x00006
    xcvBuildSpiCmd (confCmd, 0x46, 0x01002); //18'b00_0000_0000_0000_0001; gain table1  //0x00001
    xcvBuildSpiCmd (confCmd, 0x45, 0x01000); //18'b00_0000_0000_0000_0000; gain table0

    hal_RfspiSendCmd(confCmd);

    xcvBuildSpiCmd (confCmd, 0x53, 0x00e03);
    xcvBuildSpiCmd (confCmd, 0x54, 0x01002);
    xcvBuildSpiCmd (confCmd, 0x55, 0x02200);
    xcvBuildSpiCmd (confCmd, 0x60, 0x1e927); //18'b01_1110_1001_0010_0111; Lna_1800_i=111
    xcvBuildSpiCmd (confCmd, 0x70, 0x14c40); //18'b01_0100_1100_0100_0000; Thermo_diff(0111~0000,1111~1000)
    xcvBuildSpiCmd (confCmd, 0x71, 0x000fd);//18'b00_0000_0000_1111_1101; Thermo_diff temperature control by like 0914
    xcvBuildSpiCmd (confCmd, 0x77, 0x00020); //18'b00_0000_0000_0010_0000; Thermo_diff_en=0

    // ;=====PLL Setting=====
    xcvBuildSpiCmd (confCmd, 0x31, 0x02aac); //18'b00_0010_1010_1010_1100; Equfil_tx_g_gsm_d8
    xcvBuildSpiCmd (confCmd, 0x41, 0x0018b); // 18'b00_0000_0001_1000_1011; PLL_850
    xcvBuildSpiCmd (confCmd, 0x42, 0x0017b); // 18'b00_0000_0001_0111_1011; PLL_900
    hal_RfspiSendCmd(confCmd);
    xcvBuildSpiCmd (confCmd, 0x43, 0x0027b); // 18'b00_0000_0001_0111_1011; PLL_1800
    xcvBuildSpiCmd (confCmd, 0x44, 0x0025b); // 18'b00_0000_0001_0110_1011; PLL_1900
    xcvBuildSpiCmd (confCmd, 0x56, 0x02200); // VCO_ibit
    xcvBuildSpiCmd (confCmd, 0x67, 0x030c5); //18'b00_0011_0000_1100_0101; Equfil_tx_g_gsm_d7
    xcvBuildSpiCmd (confCmd, 0x68, 0x025ee); //18'b00_0010_0101_1110_1110; Equfil_tx_g_gsm_d9
    xcvBuildSpiCmd (confCmd, 0x69, 0x02223); //18'b00_0010_0010_0010_0011; Equfil_tx_g_gsm_d10

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

    hal_RfspiSendCmd(confCmd);

    // ;=====IC Self-Cal=====
    xcvBuildSpiCmd (confCmd, 0x63, 0x20a35); //18'b10_0000_1010_0011_0101; Dc_cal_ mode=10
    xcvBuildSpiCmd (confCmd, 0x6A, 0x38072); //18'b11_1000_0000_0111_0010; Thermo_cal_polarity=0

    xcvBuildSpiCmd (confCmd, 0x30, 0x20f57); //18'b10_0000_0101_0001_0111; DigRF=0,Freq_mode=0,Mdll_dr=1,Self-Cal=1
    xcvBuildSpiCmd (confCmd, 0x08, 0x00800); //18'b01_0100_0000_1010_1111; Mdll_div=01,band_sel =00,arfcn=175
    xcvBuildSpiCmd (confCmd, 0x05, 0xa625);  //0x2a625
    xcvBuildSpiCmd (confCmd, 0x06, 0x08948);
    xcvBuildSpiCmd (confCmd, 0x0a, 0x1); //18'b01_1000_0000_0000_0001; pdn =1

    hal_RfspiSendCmd(confCmd);
    sxr_Sleep(200 MS_WAITING);

    xcvBuildSpiCmd (confCmd, 0x0a, 0x0); //18'b01_1000_0000_0000_0000; pdn =0

    hal_RfspiSendCmd(confCmd);
    sxr_Sleep(3 MS_WAITING);

    xcvBuildSpiCmd (confCmd, 0x08, 0x01800); //18'b01_0001_0000_0010_0101; Mdll_div=00,band_sel =01,arfcn=37
    xcvBuildSpiCmd (confCmd, 0x05, 0xa89b);  //0x2a89b
    xcvBuildSpiCmd (confCmd, 0x06, 0x126d2);
    xcvBuildSpiCmd (confCmd, 0x0a, 0x1); //18'b01_1000_0000_0000_0001; pdn =1

    hal_RfspiSendCmd(confCmd);
    sxr_Sleep(200 MS_WAITING);

    xcvBuildSpiCmd (confCmd, 0x0a, 0x0); //18'b01_1000_0000_0000_0000; pdn =0

    hal_RfspiSendCmd(confCmd);
    sxr_Sleep(3 MS_WAITING);

    xcvBuildSpiCmd (confCmd, 0x08, 0x02800); //18'b01_0010_0010_1001_1101; Mdll_div=00,band_sel =10,arfcn=669
    xcvBuildSpiCmd (confCmd, 0x05, 0xa7a4);  //0x2a7a4
    xcvBuildSpiCmd (confCmd, 0x06, 0x36936);
    xcvBuildSpiCmd (confCmd, 0x0a, 0x1); //18'b01_1000_0000_0000_0001; pdn =1

    hal_RfspiSendCmd(confCmd);
    sxr_Sleep(200 MS_WAITING);

    xcvBuildSpiCmd (confCmd, 0x0a, 0x0); //18'b01_1000_0000_0000_0000; pdn =0

    hal_RfspiSendCmd(confCmd);
    sxr_Sleep(3 MS_WAITING);

    xcvBuildSpiCmd (confCmd, 0x08, 0x03800); //18'b01_0011_0010_0111_0110; Mdll_div=00,band_sel =11,arfcn=630
    xcvBuildSpiCmd (confCmd, 0x05, 0xaa64);  //0x2aa64
    xcvBuildSpiCmd (confCmd, 0x06, 0x09909);
    xcvBuildSpiCmd (confCmd, 0x0a, 0x1); //18'b01_1000_0000_0000_0001; pdn =1

    hal_RfspiSendCmd(confCmd);
    sxr_Sleep(200 MS_WAITING);

    xcvBuildSpiCmd (confCmd, 0x0a, 0x0); //18'b01_1000_0000_0000_0000; pdn =0

    hal_RfspiSendCmd(confCmd);
    sxr_Sleep(3 MS_WAITING);

    xcvBuildSpiCmd (confCmd, 0x30, 0x20715); //18'b10_0000_0111_0001_0101; DigRF=1,Freq_mode=0,Mdll_dr=1,Self-Cal=0

    // ;=====IC Debug2=====
    xcvBuildSpiCmd (confCmd, 0x111, 0x0c880); //18'b00_1100_1000_1000_0000; Ldo_bypass=1;

    // ;=====Mode Setting=====
    xcvBuildSpiCmd (confCmd, 0x1F, 0x3e014); //18'b11_1110_0000_0001_0100;
    xcvBuildSpiCmd (confCmd, 0x3B, 0x00390); // mode=fast-hold,time=10,bw=010
    xcvBuildSpiCmd (confCmd, 0x38, 0x10002); // Rx_Sample_mode=00: 16Bit/sample and 2sample/symbol
    xcvBuildSpiCmd (confCmd, 0x3A, 0x0049b); // 18'b00_0000_0100_1011_0110; bit11~9 means gsm850 BS0~2, GPIO_BS_TX //now works

#ifdef   USE_NOTCH
#ifdef USE_BB_DC_CANCEL
    xcvBuildSpiCmd (confCmd, 0x30, 0x20705);
#endif
#else
#ifdef USE_BB_DC_CANCEL
    xcvBuildSpiCmd (confCmd, 0x30, 0x30705);
#endif
#endif

    hal_RfspiSendCmd(confCmd);
    hal_ClkDisable(xcvClk);
    g_xcvRfIniting = FALSE;
}

VOID rfd_XcvClose(VOID)
{
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
        // pmd_SetEfuseFor_LP(0);
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
       // afc_offset_ppm += (1000*((g_afcDacVal) * XCV_PARAM(AFC_GAIN(GSM_BAND_GSM900)) / 1000)) /( 900*1300);
	 afc_offset_ppm += (240*((g_afcDacVal) * XCV_PARAM(AFC_GAIN(GSM_BAND_GSM900)) / 1000)) /( 900*1400);
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
        xcvBuildSpiCmd (confCmd, 0xc3, 0x0);
        xcvBuildSpiCmd (confCmd, 0xc2, (step_offset_lp)&0xff);
        xcvBuildSpiCmd (confCmd, 0xc3, 0x1);    //Step_offset_update
        hal_RfspiSendCmd(confCmd);
        //pmd_SetEfuseFor_LP(3);
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

PUBLIC VOID rfd_XcvFrameInit(VOID)
{
    UINT32 status = hal_SysEnterCriticalSection();

    hal_SysExitCriticalSection(status);
}

//======================================================================
// rfd_XcvFrameStart
// This function used to inform XCV driver that an FINT just starts
//----------------------------------------------------------------------

PUBLIC VOID RFD_FUNC_INTERNAL rfd_XcvFrameStart(VOID)
{
    //UINT32 status = hal_SysEnterCriticalSection();
    //hal_SysExitCriticalSection(status);
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
        UINT32 bitfield = CDAC_REG_VALUE_BASE | ((g_CDAC&RDA6220_CDAC_REG_MASK) << RDA6220_CDAC_REG_OFFSET);
        xcvBuildSpiCmd (spiCmd, RDA6220_CDAC_REG, bitfield);
    }
    int ramp_num = 0;
    for(ramp_num = 0; ramp_num < 24; ramp_num++)
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

#define XCV_REG_NUMBER (0x140)
UINT32 XCV_REG_VALUE_LOCATION g_rfdXcvRegValue[XCV_REG_NUMBER];

UINT32 g_rfdXcvRegForceWriteError = 0;
UINT32 g_rfdXcvRegForceReadError = 0;

#ifdef XCV_REG_BUFFER_READ_WRITE
UINT32 g_rfdXcvRegLastWriteValue[XCV_REG_NUMBER];
#define XCV_WRITE_LIST_LEN (0xa)
UINT32 RFD_DATA_INTERNAL g_rfdXcvRegWriteList[XCV_WRITE_LIST_LEN];

PROTECTED BOOL rfd_XcvRegWriteListInsert(UINT32 addr, UINT32 data)
{
    int i;
    for (i=0; i<XCV_WRITE_LIST_LEN; i++)
    {
        if (g_rfdXcvRegWriteList[i] == 0) break;
    }

    if (i == XCV_WRITE_LIST_LEN) return FALSE;

    g_rfdXcvRegWriteList[i] = SPI_REG_WRITE_FLAG |
                              (addr&RDA6220_ADDR_MASK)<<27 |
                              (data&RDA6220_DATA_MASK);
    return TRUE;
}
#endif // XCV_REG_BUFFER_READ_WRITE

PROTECTED BOOL rfd_XcvRegForceWrite(UINT32 addr, UINT32 data)
{
    BOOL result = FALSE;
    UINT32 status = hal_SysEnterCriticalSection();
    if (hal_RfspiImmReadWriteAvail())
    {
        rfd_XcvRegForceWriteSingle(addr, data);
        result = TRUE;
    }
    hal_SysExitCriticalSection(status);
    return result;
}

PROTECTED BOOL rfd_XcvRegForceRead(UINT32 addr, UINT32 *pData)
{
    BOOL result = FALSE;
    UINT32 status = hal_SysEnterCriticalSection();
    if (hal_RfspiImmReadWriteAvail())
    {
        xcvReadReg(addr, (UINT8 *)pData);
        result = TRUE;
    }
    hal_SysExitCriticalSection(status);
    return result;
}

PROTECTED VOID rfd_XcvRegValueInit(VOID)
{
    for (int i=0; i<XCV_REG_NUMBER; i++)
    {
        g_rfdXcvRegValue[i] = 0;
    }
}

PROTECTED VOID rfd_XcvRegCheckAll()
{

    for (int i=0; i<XCV_REG_NUMBER; i++)
    {
        if (g_rfdXcvRegValue[i] & SPI_REG_FORCE_WRITE_FLAG)
        {
            if (!rfd_XcvRegForceWrite(i, g_rfdXcvRegValue[i]&XCV_REG_DATA_MASK))
            {
                g_rfdXcvRegForceWriteError++;
                continue;
            }
            g_rfdXcvRegValue[i] &= ~SPI_REG_FORCE_WRITE_FLAG;
        }
        if (g_rfdXcvRegValue[i] & SPI_REG_FORCE_READ_FLAG)
        {
            UINT32 data;
            if (!rfd_XcvRegForceRead(i, &data))
            {
                g_rfdXcvRegForceReadError++;
                continue;
            }
            g_rfdXcvRegValue[i] &= ~(SPI_REG_FORCE_READ_FLAG|XCV_REG_DATA_MASK);
            g_rfdXcvRegValue[i] |= data&XCV_REG_DATA_MASK;
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
    rfd_XcvRegCheckAll();
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
