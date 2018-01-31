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

#include "hal_sys.h"
#include "hal_timers.h"
#include "atvd_m.h"
#include "rda5888drv.h"

#include "pmd_m.h"
#include "hal_gpio.h"

#include "atvd_config.h"
#include "tgt_atvd_cfg.h"
#include "dbg.h"
#include "sxs_io.h"
#include "chip_id.h"

#ifdef I2C_BASED_ON_GPIO
#include "gpio_i2c.h"
#else
extern HAL_I2C_BUS_ID_T g_atvdI2cBusId;
#endif

#define RDA5888_HSYNCHOLD_ENABLE
/** Default Base Address for TLG1100 */
#define RDA5888_DEVICE_ADDR            (0x0062)


#define RDA5888_CHIPID             (0x31E2)
// RDA5888 page definition.
#define RDA5888_PAGE_0            (0x0000)
#define RDA5888_PAGE_1            (0x0100)

#define RDA5888_PAGE_UNDEFINED    (0xffff)

#define IIC_ERR             1
#define IIC_DONE            0


#define CHECK_SEEK_READY_TIMES    (10)

// fm softmute threshold value setting.
#define FM_SOFTMUTE_THRESHOLD     (46) // RANGE: 0~63, default: 44

// for RDA5888H, RDA5888HE softmute setting.
#define FM_SOFTMUTE_THRESHOLD2     (8) //  RANGE: 0~63, default: 8

typedef enum
{
    ATV_SYS_RESET_MODE0,
    ATV_SYS_RESET_MODE1,
    ATV_SYS_RESET_MODE2,
    ATV_SYS_RESET_INVALID
} eSysMode;

#ifdef RDA5888_DSPINTCOEF_ENABLE
#define ATV_DSP_INT_COEF_LEVEL_1          (1)
#define ATV_DSP_INT_COEF_LEVEL_2          (2)
#define ATV_DSP_INT_COEF_LEVEL_3          (3)
#define ATV_DSP_INT_COEF_LEVEL_4          (4)
#define ATV_DSP_INT_COEF_LEVEL_5          (5)
#define ATV_DSP_INT_COEF_LEVEL_INVALID    (0xff)

#define RSSI_STAGE_LEVEL          (8)

// customize specific level for lumminace/chroma/frame buffer/image sharp
#define DSP_LUMMINANCE_LEVEL_CUR  (ATV_DSP_INT_COEF_LEVEL_2)
#define DSP_CHROMA_LEVEL_CUR      (ATV_DSP_INT_COEF_LEVEL_3)
#define DSP_FRAMEB_LEVEL_CUR      (ATV_DSP_INT_COEF_LEVEL_4)
#define DSP_SHARP_LEVEL_CUR       (ATV_DSP_INT_COEF_LEVEL_1)


#if 1//def DSP_LUMMINANCE_LEVEL_CUR
// coefficients table of luminance.
static uint16 m_wLumCoefTable[RSSI_STAGE_LEVEL] =
{
#if (DSP_LUMMINANCE_LEVEL_CUR == ATV_DSP_INT_COEF_LEVEL_1)
//  index_0  index_1  index_2  index_3  index_4  index_5  index_6  index_7
    0x000f,  0x000f,  0x000f,  0x0000, 0x0001,  0x0001,  0x0002,  0x0003
#elif (DSP_LUMMINANCE_LEVEL_CUR == ATV_DSP_INT_COEF_LEVEL_2)
    0x0000,  0x0000,  0x0000,  0x0001,  0x0001,  0x0002,  0x0003,  0x0004  // 0x1c2[15:12], 0x1ce[15:12]
#elif (DSP_LUMMINANCE_LEVEL_CUR == ATV_DSP_INT_COEF_LEVEL_3)
    0x0003,  0x0003,  0x0003,  0x0004, 0x0004,  0x0005,  0x0006,  0x0006
#elif (DSP_LUMMINANCE_LEVEL_CUR == ATV_DSP_INT_COEF_LEVEL_4)
    0x0004,  0x0004,  0x0004,  0x0005, 0x0005,  0x0005,  0x0006,  0x0007
#elif (DSP_LUMMINANCE_LEVEL_CUR == ATV_DSP_INT_COEF_LEVEL_3)
    0x0005,  0x0005,  0x0005,  0x0006, 0x0006,  0x0007,  0x0007,  0x0007
#else
    0x0003,  0x0003,  0x0003,  0x0004, 0x0004,  0x0005,  0x0006,  0x0006
#endif
};
#endif

#if 1//def DSP_CHROMA_LEVEL_CUR
// coefficients table of chroma.
static uint16 m_wChromaCoefTable[RSSI_STAGE_LEVEL] =
{
#if (DSP_CHROMA_LEVEL_CUR == ATV_DSP_INT_COEF_LEVEL_1)
//  index_0  index_1  index_2  index_3  index_4  index_5  index_6  index_7
    0x0002,  0x0002,  0x0002,  0x000f, 0x000e,  0x000d,  0x000a,  0x0008   // 0x165[15:12]...0x16c[15:12]
#elif (DSP_CHROMA_LEVEL_CUR == ATV_DSP_INT_COEF_LEVEL_2)
    0x0003,  0x0003,  0x0003,  0x0002, 0x000f,  0x000e,  0x000d,  0x000a
#elif (DSP_CHROMA_LEVEL_CUR == ATV_DSP_INT_COEF_LEVEL_3)
    0x0004,  0x0004,  0x0004,  0x0003, 0x0002,  0x000f,  0x000e,  0x000d
#elif (DSP_CHROMA_LEVEL_CUR == ATV_DSP_INT_COEF_LEVEL_4)
    0x0005,  0x0005,  0x0005,  0x0004, 0x0003,  0x0002,  0x000f,  0x000e
#elif (DSP_CHROMA_LEVEL_CUR == ATV_DSP_INT_COEF_LEVEL_5)
    0x0007,  0x0007,  0x0007,  0x0007, 0x0007,  0x0007,  0x0007,  0x000b
#else
    0x0004,  0x0004,  0x0004,  0x0003, 0x0002,  0x000f,  0x000e,  0x000d
#endif
};
#endif

#if 1//def DSP_FRAMEB_LEVEL_CUR
// coefficients table of framebuffer.
static uint16 m_wFramebCoefTable[RSSI_STAGE_LEVEL] =
{
#if (DSP_FRAMEB_LEVEL_CUR == ATV_DSP_INT_COEF_LEVEL_1)
//  index_0  index_1  index_2  index_3  index_4  index_5  index_6  index_7
    0x02f4,  0x02f4,  0x02f4,  0x0430, 0x0430,  0x0820,  0x0c10,  0x0c10   //  0x165[11:0]...0x16c[11:0]
#elif (DSP_FRAMEB_LEVEL_CUR == ATV_DSP_INT_COEF_LEVEL_2)
    0x0430,  0x0430,  0x0430,  0x0820, 0x0820,  0x0c10,  0x0e09,  0x0e09
#elif (DSP_FRAMEB_LEVEL_CUR == ATV_DSP_INT_COEF_LEVEL_3)
    0x0820,  0x0820,  0x0820,  0x0a96, 0x0a96,  0x0a96,  0x0c11,  0x0c11
#elif (DSP_FRAMEB_LEVEL_CUR == ATV_DSP_INT_COEF_LEVEL_4)
    0x0820,  0x0820,  0x0820,  0x0c10, 0x0c10,  0x0e08,  0x0e86,  0x0e86
#elif (DSP_FRAMEB_LEVEL_CUR == ATV_DSP_INT_COEF_LEVEL_5)
    0xc10,  0x0c10,  0x0c10,  0x0c10, 0x0c10,  0x0e08,  0x0e86,  0x0e86
#else
    0x0820,  0x0820,  0x0820,  0x0a96, 0x0a96,  0x0a96,  0x0c11,  0x0c11
#endif
};
#endif

#if 1//def DSP_SHARP_LEVEL_CUR
// coefficients table of image sharp.
static uint16 m_wSharpCoefTable[RSSI_STAGE_LEVEL] =
{
#if (DSP_SHARP_LEVEL_CUR == ATV_DSP_INT_COEF_LEVEL_1)
//  index_0  index_1  index_2  index_3  index_4  index_5  index_6  index_7
    0x0002,  0x0002,  0x0002,  0x0002,  0x0002,  0x0002,  0x0002,  0x0002
#elif (DSP_SHARP_LEVEL_CUR == ATV_DSP_INT_COEF_LEVEL_2)
    0x0004,  0x0004,  0x0004,  0x0004,  0x0004,  0x0004,  0x0004,  0x0004
#elif (DSP_SHARP_LEVEL_CUR == ATV_DSP_INT_COEF_LEVEL_3)
    0x0008,  0x0008,  0x0008,  0x0008,  0x0008,  0x0008,  0x0008,  0x0008   // 0x12d[15:12], 0x12e[15:12]
#elif (DSP_SHARP_LEVEL_CUR == ATV_DSP_INT_COEF_LEVEL_4)
    0x000b,  0x000b,  0x000b,  0x000b,  0x000b,  0x000b,  0x000b,  0x000b
#elif (DSP_SHARP_LEVEL_CUR == ATV_DSP_INT_COEF_LEVEL_5)
    0x000f,  0x000f,  0x000f,  0x000f,  0x000f,  0x000f,  0x000f,  0x000f
#else
    0x0008,  0x0008,  0x0008,  0x0008,  0x0008,  0x0008,  0x0008,  0x0008
#endif
};
#endif
#endif

uint8 g_nBlendMode = 0;

static uint32 m_nFreqOffset = 0;
static unsigned char m_bIs625Lines = 1;
static uint16 m_cReqMap = RDA5888_PAGE_UNDEFINED;

void RDAEXT_DelayMs(uint32 ms)
{
    hal_TimDelay(ms MS_WAITING);
}


//*****************************************************************************
//**************** START OF PUBLIC PROCEDURE IMPLEMENTATIONS ******************
//*****************************************************************************

// ----------------------------------------------------------------------------
// RDA5888_WriteReg
//
// dwRegAddr:       register to write
// wData:           data to write
//
// Return:
//  RDAMTV_RT_ERROR if an error occurred communicating with the device.
//  RDAMTV_RT_SUCCESS if the operation was successful.
// ----------------------------------------------------------------------------
uint32 RDA5888_WriteReg(uint32 dwRegAddr, uint16 wData)
{
    uint16 tempdata=0;
    int i;
    uint16 val1=0;
    uint32 reg_addr1;
    unsigned char addr =0x02;

#ifndef I2C_BASED_ON_GPIO
    //xiaoyifeng for atv
    HAL_ERR_T halErr = HAL_ERR_RESOURCE_BUSY;
#endif


#if (RDA5888_CHIP_VER == 5)
    addr =0x02;
#else
    addr =0xff;
#endif

    reg_addr1=dwRegAddr;
    if (dwRegAddr<0x100)
    {

        if (m_cReqMap != RDA5888_PAGE_0)
        {
            m_cReqMap = RDA5888_PAGE_0;

            //tempdata=0;
#ifdef I2C_BASED_ON_GPIO
            gpio_i2c_write_data(RDA5888_DEVICE_ADDR,&addr,1, (UINT8*)&m_cReqMap, 2);
#else
            halErr=hal_I2cSendData(g_atvdI2cBusId,RDA5888_DEVICE_ADDR,addr,(UINT8*)&m_cReqMap,2);
#endif

        }
    }
    else
    {
        //tempdata=0x0100;
        if (m_cReqMap != RDA5888_PAGE_1)
        {
            m_cReqMap = RDA5888_PAGE_1;
#ifdef I2C_BASED_ON_GPIO
            gpio_i2c_write_data(RDA5888_DEVICE_ADDR,&addr,1, (UINT8*)&m_cReqMap,2);
#else
            halErr=hal_I2cSendData(g_atvdI2cBusId,RDA5888_DEVICE_ADDR,addr,(UINT8*)&m_cReqMap,2);
#endif

            // gpio_i2c_read_data(RDA5888_DEVICE_ADDR,(UINT8* )&addr,1, (UINT8*)&tempdata, 2);
            // SXS_TRACE(TSTDOUT,"~~~~~~~~~~read 0XFF REG: tempdata= 0x%x ", tempdata);

        }
        dwRegAddr=dwRegAddr&0x0ff;
    }

    tempdata=((wData&0xff)<<8)|(wData>>8);


#ifdef I2C_BASED_ON_GPIO
    gpio_i2c_write_data(RDA5888_DEVICE_ADDR,   (UINT8*) &dwRegAddr,1, (UINT8*)&tempdata, 2);
#else
    halErr=hal_I2cSendData(g_atvdI2cBusId,RDA5888_DEVICE_ADDR,dwRegAddr,(UINT8*)&tempdata,2);
#endif

    for(i=0; i<2; i++)
    {
        RDA5888_ReadReg(reg_addr1, &val1);
        if((wData!=val1)&&(reg_addr1!=0x150)&&(reg_addr1!=0x151)&&(reg_addr1!=0x152)&&(reg_addr1!=0x153)&&(reg_addr1!=0x154))
        {
            //    SXS_TRACE(TSTDOUT,"==reg_add:%x var:%x  var_read:%x",reg_addr1,wData,val1);

#ifdef I2C_BASED_ON_GPIO
            gpio_i2c_write_data(RDA5888_DEVICE_ADDR,(UINT8*)&dwRegAddr,1, (UINT8*)&tempdata, 2);
#else
            halErr=hal_I2cSendData(g_atvdI2cBusId,RDA5888_DEVICE_ADDR,dwRegAddr,(UINT8*)&tempdata,2);
#endif

        }
        else
        {
            break;
        }
    }



#ifdef I2C_BASED_ON_GPIO
    return IIC_DONE;
#else


    if (HAL_ERR_NO==halErr)
        return IIC_DONE;
    else
    {
        SXS_TRACE(TSTDOUT,"RDA5888_WriteReg IIC_HAL_ERR= %x ", halErr);
        return IIC_ERR;
    }
#endif

}



// ----------------------------------------------------------------------------
// RDA5888_ReadReg
//
// dwRegAddr:        Register to read
// pwData:           Pointer for register data
//
// Return:
//  RDAMTV_RT_ERROR if an error occurred communicating with the device.
//  RDAMTV_RT_SUCCESS if the operation was successful.
// ----------------------------------------------------------------------------
uint32 RDA5888_ReadReg(uint32 dwRegAddr, uint16 *pwData)
{
    uint16 tempdata=0;
    uint16 tempdata1=0;

    unsigned char addr =0x02;

#ifndef I2C_BASED_ON_GPIO
//xiaoyifeng for atv
    HAL_ERR_T halErr = HAL_ERR_RESOURCE_BUSY;
#endif

#if (RDA5888_CHIP_VER == 5)
    addr =0x02;
#else
    addr =0xff;
#endif

    if (dwRegAddr<0x100)
    {
        if (m_cReqMap != RDA5888_PAGE_0)
        {
            m_cReqMap = RDA5888_PAGE_0;
            //tempdata=0;

#ifdef I2C_BASED_ON_GPIO
            gpio_i2c_write_data(RDA5888_DEVICE_ADDR, &addr,1, (UINT8*)&m_cReqMap, 2);
#else
            halErr=hal_I2cSendData(g_atvdI2cBusId,RDA5888_DEVICE_ADDR,addr,(UINT8*)&m_cReqMap,2);
#endif
        }
    }
    else
    {
        if (m_cReqMap != RDA5888_PAGE_1)
        {
            m_cReqMap = RDA5888_PAGE_1;
            //tempdata=0x0100;

#ifdef I2C_BASED_ON_GPIO
            gpio_i2c_write_data(RDA5888_DEVICE_ADDR, &addr,1, (UINT8*)&m_cReqMap, 2);
#else
            halErr=hal_I2cSendData(g_atvdI2cBusId,RDA5888_DEVICE_ADDR,addr,(UINT8*)&m_cReqMap,2);
#endif
        }

        dwRegAddr=dwRegAddr&0x0ff;
    }

//  hal_I2cGetData(RDA5888_DEVICE_ADDR,dwRegAddr,(UINT8*)val,2);



#ifdef I2C_BASED_ON_GPIO
    gpio_i2c_read_data(RDA5888_DEVICE_ADDR,(UINT8* )&dwRegAddr,1, (UINT8*)&tempdata, 2);
#else
    halErr=hal_I2cGetData(g_atvdI2cBusId,RDA5888_DEVICE_ADDR,dwRegAddr,(UINT8*)&tempdata,2);

#endif


//  SXS_TRACE(TSTDOUT,"dwRegAddr= %x ", dwRegAddr);
//  SXS_TRACE(TSTDOUT,"tempdata= %x ", tempdata);


    tempdata1=((tempdata&0xff)<<8)|(tempdata>>8);
    //   SXS_TRACE(TSTDOUT,"tempdata1= %x ", tempdata1);
    *pwData=tempdata1;

#ifdef I2C_BASED_ON_GPIO
    return IIC_DONE;
#else


    if (HAL_ERR_NO==halErr)
        return IIC_DONE;
    else
    {
        SXS_TRACE(TSTDOUT,"RDA5888_ReadReg IIC_HAL_ERR= %x ", halErr);
        return IIC_ERR;
    }
#endif


}



uint32 RDA5888_FieldWriteReg(uint32 dwRegAddr, uint16 wFieldMask, uint16 wFieldData)
{
    uint32 rt = RDAMTV_RT_SUCCESS;
    uint16 wReadData;
    uint16 i = 0;
    uint16 val = wFieldMask;

    while ((!(val&0x0001))&&(i < 15))
    {
        i++;
        val = (val >> 1);
    }

    RDA5888_ReadReg(dwRegAddr, &wReadData);
    //wReadData = (wReadData & (~wFieldMask)) |(wFieldData);
    wReadData = (wReadData & (~wFieldMask)) |(wFieldData<<i);

    RDA5888_WriteReg(dwRegAddr, wReadData);

    return rt;
}

uint16 RDA5888_FieldReadReg(uint32 dwRegAddr, uint16 wFieldMask)
{
    uint16 wReadData;
    uint16 i = 0;
    uint16 val = wFieldMask;

    while ((!(val&0x0001))&&(i < 15))
    {
        i++;
        val = (val >> 1);
    }

    RDA5888_ReadReg(dwRegAddr, &wReadData);
    wReadData = (wReadData & wFieldMask);

    wReadData = (wReadData >> i);

    return wReadData;
}

static void ATVSetRxOn(void)
{
#if ((RDA5888_CHIP_VER == 6) || (RDA5888_CHIP_VER == 7))
    RDA5888_FieldWriteReg(0x030, 0x0008, 0x0);
    RDAEXT_DelayMs(5);
    RDA5888_FieldWriteReg(0x030, 0x0008, 0x1);
#else
    RDA5888_FieldWriteReg(0x09e, 0x0001, 0x0);
    RDAEXT_DelayMs(5);
    RDA5888_FieldWriteReg(0x09e, 0x0001, 0x1);
#endif
}

#if (RDA5888_CHIP_VER == 5)
static void ATVEnable(void)
{
    //RDA5888_WriteReg(0x01, 0x0911);
    //RDA5888_WriteReg(0x01, 0x0919);
    RDA5888_FieldWriteReg(0x01, 0x0008, 0x0);
    RDAEXT_DelayMs(5);
    RDA5888_FieldWriteReg(0x01, 0x0008, 0x1);
}
#endif

static void ATVSysReset(void)
{
    RDA5888_FieldWriteReg(0x130, 0x0800, 0x0);
    RDAEXT_DelayMs(5);
    RDA5888_FieldWriteReg(0x130, 0x0800, 0x1);
}

// Params:
//  dwVFreq    : Desired tuning frequency in hertz.   i.e. 57.75mhz  =
//              57750000.
static void ATVSetNotchFilter(uint16 wMode, uint32 dwVFreq)
{
    dwVFreq = dwVFreq/1000;

//  RDAEXT_DebugPrint(RDA5888_TRACE_MOD  "[rdamtv]ATVSetNotchFilter, dwVFreq = %d", dwVFreq);

    switch (dwVFreq)
    {
        case 49750: // 49.75 MHZ
#if ((RDA5888_CHIP_VER == 6) || (RDA5888_CHIP_VER == 7))
            RDA5888_WriteReg(0x1A0, 0x1743);
            RDA5888_WriteReg(0x1A1, 0x07e3);
            RDA5888_WriteReg(0x1A2, 0xe);
            RDA5888_WriteReg(0x1A3, 0xBA12);
            RDA5888_WriteReg(0x1BC, 0x03F1);
            RDA5888_WriteReg(0x1BF, 0x07);
            RDA5888_WriteReg(0x1AC, 0x9800);
#else
            RDA5888_WriteReg(0x1A0, 0x1748);
            RDA5888_WriteReg(0x1A1, 0x7DE);
            RDA5888_WriteReg(0x1A2, 0x10);
#endif
            break;

        case 55250: // 55.25 MHZ
            RDA5888_WriteReg(0x1A0, 0x10CC);
            RDA5888_WriteReg(0x1A1, 0x7DE);
            RDA5888_WriteReg(0x1A2, 0x10);
            break;

        case 57750: // 57.75 MHZ
            RDA5888_WriteReg(0x1A0, 0x15CD);
            RDA5888_WriteReg(0x1A1, 0x7DE);
            RDA5888_WriteReg(0x1A2, 0x10);
            break;

        case 61250: // 61.25 MHZ
            RDA5888_WriteReg(0x1A0, 0x1E29);
            RDA5888_WriteReg(0x1A1, 0x7DE);
            RDA5888_WriteReg(0x1A2, 0x10);
            break;

        case 62250: // 62.25 MHZ
            RDA5888_WriteReg(0x1A0, 0x1A90);
            RDA5888_WriteReg(0x1A1, 0x7E8);
            RDA5888_WriteReg(0x1A2, 0xB);
            break;

        case 65750: // 65.75 MHZ
            RDA5888_WriteReg(0x1A0, 0x116D);
            RDA5888_WriteReg(0x1A1, 0x7DE);
            RDA5888_WriteReg(0x1A2, 0x10);
            break;

        case 77250: // 77.25 MHZ
            RDA5888_WriteReg(0x1A0, 0x15CD);
            RDA5888_WriteReg(0x1A1, 0x7DE);
            RDA5888_WriteReg(0x1A2, 0x10);
#if 1 //csd need    
            RDA5888_WriteReg(0x1A3, 0x8242);
            RDA5888_WriteReg(0x1BC, 0x03FB);
            RDA5888_WriteReg(0x1BF, 0x02);
            RDA5888_WriteReg(0x1AC, 0x9800);
#endif
            break;

        case 77750: // 77.75 MHZ
            RDA5888_WriteReg(0x1A0, 0x1745);
            RDA5888_WriteReg(0x1A1, 0x7e3);
            RDA5888_WriteReg(0x1A2, 0xe);
            break;
        case 83250: // 83.25 MHZ
            RDA5888_WriteReg(0x1A0, 0x124E);
            RDA5888_WriteReg(0x1A1, 0x7D0);
            RDA5888_WriteReg(0x1A2, 0x17);
            break;

        case 85250: // 85.25 MHZ
            RDA5888_WriteReg(0x1A0, 0x1744);
            RDA5888_WriteReg(0x1A1, 0x7e3);
            RDA5888_WriteReg(0x1A2, 0xe);
            break;
        case 176250: //176.25 MHZ
            RDA5888_WriteReg(0x1A0, 0x1c57);
            //lib3.09 modify 0x7DE ->0x7de ???
            RDA5888_WriteReg(0x1A1, 0x7de);
            RDA5888_WriteReg(0x1A2, 0x10);
            break;

        case 183250: // 183.25 MHZ
            RDA5888_WriteReg(0x1A0, 0x1c57);
            RDA5888_WriteReg(0x1A1, 0x7de);
            RDA5888_WriteReg(0x1A2, 0x10);
            break;

        case 184250: // 184.25 MHZ
            RDA5888_WriteReg(0x1A0, 0x18df);
            RDA5888_WriteReg(0x1A1, 0x7e3);
            RDA5888_WriteReg(0x1A2, 0xe);
            break;

        case 187250: // 187.25 MHZ
            RDA5888_WriteReg(0x1A0, 0x116D);
            RDA5888_WriteReg(0x1A1, 0x7DE);
            RDA5888_WriteReg(0x1A2, 0x10);
            break;

        case 192250: // 192.25 MHZ
            RDA5888_WriteReg(0x1A0, 0x1475);
            RDA5888_WriteReg(0x1A1, 0x7DE);
            RDA5888_WriteReg(0x1A2, 0x10);
            break;

        case 199250: // 199.25 MHZ
            RDA5888_WriteReg(0x1A0, 0x1470);
            RDA5888_WriteReg(0x1A1, 0x7e3);
            RDA5888_WriteReg(0x1A2, 0xe);
            break;

        case 200250: // 200.25 MHZ
            RDA5888_WriteReg(0x1A3, 0xc022);
            RDA5888_WriteReg(0x1BC, 0x03FB);
            RDA5888_WriteReg(0x1BF, 0x02);
            RDA5888_WriteReg(0x1AC, 0x9800);
            break;

        case 210250: // 210.25 MHZ
            RDA5888_WriteReg(0x1A0, 0x1C54);
            RDA5888_WriteReg(0x1A1, 0x7ED);
            RDA5888_WriteReg(0x1A2, 0x9);
            break;

        case 211250: // 211.25 MHZ
            RDA5888_WriteReg(0x1A0, 0x18E1);
            RDA5888_WriteReg(0x1A1, 0x7DE);
            RDA5888_WriteReg(0x1A2, 0x10);
            break;

        case 215250: // 215.25 MHZ
            RDA5888_WriteReg(0x1A0, 0x1055);
            RDA5888_WriteReg(0x1A1, 0x7e8);
            RDA5888_WriteReg(0x1A2, 0xb);
            break;
        case 216250: // 216.25 MHZ
            RDA5888_WriteReg(0x1A0, 0x1023);
            RDA5888_WriteReg(0x1A1, 0x7e4);
            RDA5888_WriteReg(0x1A2, 0xe);
            break;
        case 217250: // 217.25 MHZ
            RDA5888_WriteReg(0x1A0, 0x10C3);
            RDA5888_WriteReg(0x1A1, 0x7E8);
            RDA5888_WriteReg(0x1A2, 0xB);
            break;

        case 535250: // 535.25 MHZ
            RDA5888_WriteReg(0x1A0, 0x18df);
            RDA5888_WriteReg(0x1A1, 0x7e3);
            RDA5888_WriteReg(0x1A2, 0xe);
            break;

        case 631250: // 631.25 MHZ
            RDA5888_WriteReg(0x1A0, 0x1471);
            RDA5888_WriteReg(0x1A1, 0x7e3);
            RDA5888_WriteReg(0x1A2, 0xe);
            break;

        case 671250: // 671.25 MHZ
            RDA5888_WriteReg(0x1A0, 0x15c8);
            RDA5888_WriteReg(0x1A1, 0x7e3);
            RDA5888_WriteReg(0x1A2, 0xe);
            break;

        case 711250: // 711.25 MHZ
            RDA5888_WriteReg(0x1A0, 0x1744);
            RDA5888_WriteReg(0x1A1, 0x7e3);
            RDA5888_WriteReg(0x1A2, 0xe);
            break;

        case 751250: // 751.25 MHZ
            RDA5888_WriteReg(0x1A0, 0x18df);
            RDA5888_WriteReg(0x1A1, 0x7e3);
            RDA5888_WriteReg(0x1A2, 0xe);
            break;
//lib3.09 add 775250,777250,781250
        case 775250: // 775.25 MHZ
            RDA5888_WriteReg(0x1A0, 0x18da);
            RDA5888_WriteReg(0x1A1, 0x7ed);
            RDA5888_WriteReg(0x1A2, 0x9);
            break;

        case 777250: // 777.25 MHZ
            RDA5888_WriteReg(0x1A0, 0x1341);
            RDA5888_WriteReg(0x1A1, 0x7e3);
            RDA5888_WriteReg(0x1A2, 0xe);
            break;

        case 781250: // 781.25 MHZ
            RDA5888_WriteReg(0x1A0, 0x10be);
            RDA5888_WriteReg(0x1A1, 0x7ed);
            RDA5888_WriteReg(0x1A2, 0x9);
            break;

        case 927250: // 927.25 MHZ
            RDA5888_WriteReg(0x1A0, 0x1744);
            RDA5888_WriteReg(0x1A1, 0x7e3);
            RDA5888_WriteReg(0x1A2, 0xe);
            break;
        default:
            RDA5888_WriteReg(0x1AC, 0x9800);
            return; // return before sys reset.
    }
    ATVSysReset();
}

static void ATVDspSystemReset(eSysMode sys_mode)
{
#if (RDA5888_CHIP_VER == 4)
    switch (sys_mode)
    {
        case ATV_SYS_RESET_MODE0:
            RDA5888_WriteReg(0x130, 0x0014);
            RDAEXT_DelayMs(5);
            RDA5888_WriteReg(0x130, 0x0814);
            break;

        case ATV_SYS_RESET_MODE1:
            RDA5888_WriteReg(0x130, 0x0015);
            RDAEXT_DelayMs(5);
            RDA5888_WriteReg(0x130, 0x0815);
            break;

        case ATV_SYS_RESET_MODE2:
            RDA5888_WriteReg(0x130, 0x0016);
            RDAEXT_DelayMs(5);
            RDA5888_WriteReg(0x130, 0x0816);
            break;

        default:
            break;
    }
#elif (RDA5888_CHIP_VER == 5)
    switch (sys_mode)
    {
        case ATV_SYS_RESET_MODE0:
            RDA5888_WriteReg(0x130, 0x0210);
            RDAEXT_DelayMs(5);
            RDA5888_WriteReg(0x130, 0x0a10);
            break;

        case ATV_SYS_RESET_MODE1:
            RDA5888_WriteReg(0x130, 0x0211);
            RDAEXT_DelayMs(5);
            RDA5888_WriteReg(0x130, 0x0a11);
            break;

        case ATV_SYS_RESET_MODE2:
            RDA5888_WriteReg(0x130, 0x0212);
            RDAEXT_DelayMs(5);
            RDA5888_WriteReg(0x130, 0x0a12);
            break;

        default:
            break;
    }
#elif ((RDA5888_CHIP_VER == 6) || (RDA5888_CHIP_VER == 7))
    switch (sys_mode)
    {
        case ATV_SYS_RESET_MODE0:
            RDA5888_WriteReg(0x130, 0x0010);
            RDAEXT_DelayMs(5);
            RDA5888_WriteReg(0x130, 0x0810);
            break;

        case ATV_SYS_RESET_MODE1:
            RDA5888_WriteReg(0x130, 0x0011);
            RDAEXT_DelayMs(5);
            RDA5888_WriteReg(0x130, 0x0811);
            break;

        case ATV_SYS_RESET_MODE2:
            RDA5888_WriteReg(0x130, 0x0012);
            RDAEXT_DelayMs(5);
            RDA5888_WriteReg(0x130, 0x0812);
            break;

        default:
            break;
    }
#endif
}

static void ATVCalcFreqOffset(rdamtv_vstd_t eVideoStd)
{
    switch (eVideoStd)
    {
        case RDAMTV_VSTD_NTSC_M:
        case RDAMTV_VSTD_PAL_M:
        case RDAMTV_VSTD_PAL_N:
        case RDAMTV_VSTD_PAL_NC:
            m_nFreqOffset = 1725;
            break;

        case RDAMTV_VSTD_PAL_B:
        case RDAMTV_VSTD_PAL_B1:
        case RDAMTV_VSTD_PAL_G:
        case RDAMTV_VSTD_PAL_I:
        case RDAMTV_VSTD_SECAM_B:
        case RDAMTV_VSTD_SECAM_B1:
        case RDAMTV_VSTD_SECAM_G:
        case RDAMTV_VSTD_NTSC_B_G:
        case RDAMTV_VSTD_NTSC_I:
            m_nFreqOffset = 2125;
            break;

        case RDAMTV_VSTD_PAL_D:
        case RDAMTV_VSTD_PAL_D1:
        case RDAMTV_VSTD_PAL_K:
        case RDAMTV_VSTD_PAL_H:
        case RDAMTV_VSTD_SECAM_D:
        case RDAMTV_VSTD_SECAM_D1:
        case RDAMTV_VSTD_SECAM_K:
        case RDAMTV_VSTD_NTSC_D_K:
            m_nFreqOffset = 2625;
            break;

        case RDAMTV_VSTD_SECAM_L:
            break;

        default:
            break;
    }

    RDAEXT_DebugPrint(RDA5888_TRACE_MOD  "[rdamtv]ATVCalcFreqOffset, eVideoStd = %d, m_nFreqOffset = %d\n", eVideoStd, m_nFreqOffset);
}

#ifdef RDA5888_DSPINTCOEF_ENABLE
static void ATVDSPCoefSet(void)
{
    uint16 temp, i;
//lib3.09 modify
#if 1//def DSP_LUMMINANCE_LEVEL_CUR
    // set coefficients of luminance.
    temp = ((m_wLumCoefTable[0]<<12) | (m_wLumCoefTable[1]<<8) | (m_wLumCoefTable[2]<<4) | (m_wLumCoefTable[3]));
    RDA5888_WriteReg(0x1c2, temp);
    temp = ((m_wLumCoefTable[4]<<12) | (m_wLumCoefTable[5]<<8) | (m_wLumCoefTable[6]<<4) | (m_wLumCoefTable[7]));
    RDA5888_WriteReg(0x1ce, temp);
#endif

#if 1// (defined(DSP_CHROMA_LEVEL_CUR) || defined(DSP_FRAMEB_LEVEL_CUR))
    //  set coefficients of chroma&framebuffer.
    for (i = 0; i < 8; i++)
    {
        temp = 0x0;
#if 1//defined(DSP_CHROMA_LEVEL_CUR)
        temp = (m_wChromaCoefTable[i]<<12);
#endif
#if 1//defined(DSP_FRAMEB_LEVEL_CUR)
        temp |= (m_wFramebCoefTable[i]);
#endif
        RDA5888_WriteReg((0x165+i), temp); // 0x165 ~ 0x16c
    }
#endif

#if 1//def DSP_SHARP_LEVEL_CUR
    // set coefficients of image sharp.
    temp = ((m_wSharpCoefTable[0]<<12) | (m_wSharpCoefTable[1]<<8) | (m_wSharpCoefTable[2]<<4) | (m_wSharpCoefTable[3]));
    RDA5888_WriteReg(0x12d, temp);
    temp = ((m_wSharpCoefTable[4]<<12) | (m_wSharpCoefTable[5]<<8) | (m_wSharpCoefTable[6]<<4) | (m_wSharpCoefTable[7]));
    RDA5888_WriteReg(0x12e, temp);
#endif
}
#endif

static void  RDA5888_CoolSandAtvPatch(void)
{
    RDA5888_WriteReg(0x120,0x423a);

    if(m_bIs625Lines)
    {
        RDA5888_WriteReg(0x15E,0x0018);
        RDA5888_WriteReg(0x15F,0x0147);
        RDA5888_WriteReg(0x15A,0x0100);
        RDA5888_FieldWriteReg(0x129, 0x0010, 1);
    }
    else
    {
        RDA5888_FieldWriteReg(0x129, 0x0010, 0);
    }

#if (RDA5888_CHIP_VER == 6)
    //open I2S slave
    RDA5888_WriteReg(0x1eb,0x3104);
    RDA5888_WriteReg(0x150,0x0410);
#endif

#if (RDA5888_CHIP_VER == 7)
    RDA5888_WriteReg(0x134, 0x02c6);
    RDA5888_WriteReg(0x135, 0x03cc); // 9'h135,16'h03cc; h_blank_end, cell without shell
    RDA5888_WriteReg(0x1d1, 0x0311); // 9'h1d1,16'h0311; added by xu 12152009, for scaler
    RDA5888_WriteReg(0x1d2, 0x05a0); // 9'h1d2,16'h05a0; added by xu 01222010, for frame buf
#endif
}

// ---------------------------------------------------------------------------
// Sets the internal registers to tune the specified frequency.
//
// Params:
//  dwVFreq    : Desired tuning frequency in hertz.   i.e. 57.75mhz  =
//              57750000.
//
// Return:
//  RDAMTV_RT_ERROR if an error occurred communicating with the device.
//  RDAMTV_RT_SUCCESS if the operation was successful.
// ---------------------------------------------------------------------------
uint32 RDA5888_FreqSet(uint32 dwVFreq)
{
    uint32 rt = RDAMTV_RT_SUCCESS;
    uint16 rega, regb;
    uint32 dwCFreq;

    dwCFreq = dwVFreq/1000 + m_nFreqOffset; // convert to center carrier freq.

    RDAEXT_DebugPrint(RDA5888_TRACE_MOD "[rdamtv]RDA5888_FreqSet, c_freq = %d KHZ", dwCFreq);

#if ((RDA5888_CHIP_VER == 6) || (RDA5888_CHIP_VER == 7))
    //set center freq
    dwCFreq = (dwCFreq << 10);
    regb = (uint16)((dwCFreq&0xffff0000)>>16);
    rega = (uint16)((dwCFreq&0x0000ffff));

    RDAEXT_DebugPrint(RDA5888_TRACE_MOD "[rdamtv]RDA5888_FreqSet, 0x32 = %x, 0x33 = %x", regb, rega);

#if (RDA5888_CHIP_VER == 6)
    RDA5888_WriteReg(0x033, (rega|0x0004));
#else
    RDA5888_WriteReg(0x033, (rega|0x0001));
#endif
    RDA5888_WriteReg(0x032, regb);
#else
#ifdef RDA5888_26MCRYSTAL_ENABLE
    //set center freq
    regb = (uint16)(((unsigned long long)dwCFreq<<8)/65000);
    rega = (uint16)((((unsigned long long)((dwCFreq<<8) - (regb * 65000))) << 16) /65000);
#else
    //set center freq
    regb = (uint16)(((unsigned long long)dwCFreq<<8)/67500);
    rega = (uint16)((((unsigned long long)((dwCFreq<<8) - (regb * 67500))) << 16) /67500);
#endif

    RDA5888_WriteReg(0x0a, rega);
    RDA5888_WriteReg(0x0b, regb);
#endif

    return rt;
}

// ----------------------------------------------------------------------------
// This function sets the video decoder to a specified video format.
//
// Params:
//  eVideoStd        Video standard to set
//
// Return:
//  RDAMTV_RT_ERROR if an error occurred communicating with the device.
//  RDAMTV_RT_SUCCESS if the operation was successful.
// ----------------------------------------------------------------------------
uint32 RDA5888_VideoStdSet(rdamtv_vstd_t eVideoStd)
{
    RDAEXT_DebugPrint(RDA5888_TRACE_MOD  "[rdamtv]RDA5888_VideoStdSet, mode = %d\n", eVideoStd);

    switch(eVideoStd)
    {
        case RDAMTV_VSTD_PAL_B:
        case RDAMTV_VSTD_PAL_B1:
        case RDAMTV_VSTD_PAL_G:
#if (RDA5888_CHIP_VER == 4)
            RDA5888_WriteReg(0x140, 0x4009);//9'h140,16'h4009; angle_mode_if, angle_mode_fm 001
            RDA5888_WriteReg(0x143, 0x1bd5);//9'h143,16'h1bd5; notch coefficients
            RDA5888_WriteReg(0x104, 0x284c);//9'h104,16'h284c; angle_in_if
            RDA5888_WriteReg(0x111, 0x5413);//9'h111,16'h5413; V2.3
            RDA5888_WriteReg(0x1a0, 0x1bc3);//9'h1a0,16'h1bc3;Notch filter for audio
            RDA5888_WriteReg(0x1a1, 0x071e);//9'h1a1,16'h071e;
            RDA5888_WriteReg(0x1a2, 0x0070);//9'h1a2,16'h0070;
#elif (RDA5888_CHIP_VER == 5)
            RDA5888_WriteReg(0x140, 0x4209);//9'h140,16'h4209; angle_mode_if, angle_mode_fm 001
            RDA5888_WriteReg(0x111, 0x5413);//9'h111,16'h5413; V2.3
            RDA5888_WriteReg(0x1a0, 0x1bc3);//9'h1a0,16'h1bc3;Notch filter for audio
            RDA5888_WriteReg(0x1a1, 0x071e);//9'h1a1,16'h071e;
            RDA5888_WriteReg(0x1a2, 0x0070);//9'h1a2,16'h0070;
            RDA5888_WriteReg(0x1e0, 0x010a);//9'h1e0,16'h018a;angle_mode_fm
#elif (RDA5888_CHIP_VER == 6)
            RDA5888_WriteReg(0x104, 0x284c);//9'h104,16'h284c; angle_in_if
            RDA5888_WriteReg(0x111, 0x5413);//9'h111,16'h5413; angle_in_sub
            RDA5888_WriteReg(0x140, 0x4009);//9'h140,16'h4009; angle_mode_if, angle_mode_fm, angle_mode_sub, line625_flag
            RDA5888_WriteReg(0x1a0, 0x1bc3);//9'h1a0,16'h1bc3;Notch filter for audio
            RDA5888_WriteReg(0x1a1, 0x071e);//9'h1a1,16'h071e;Notch filter for audio
            RDA5888_WriteReg(0x1a2, 0x0070);//9'h1a2,16'h0070;Notch filter for audio
            RDA5888_WriteReg(0x1e0, 0x010a);//9'h1e0,16'h010A;by 20100727
            RDA5888_WriteReg(0x146, 0x0f1d);//9'h146,16'h0f1d; softmute added 10182010 by xu
            RDA5888_WriteReg(0x147, 0x293e);//9'h147,16'h293e;
            RDA5888_WriteReg(0x148, 0x5b73);//9'h148,16'h5b73;
            RDA5888_WriteReg(0x149, 0x8e08);//9'h149,16'h8e08;
#elif (RDA5888_CHIP_VER == 7)
            RDA5888_WriteReg(0x104, 0x284c);//9'h104,16'h284c; angle_in_if
            RDA5888_WriteReg(0x111, 0x5413);//9'h111,16'h5413; angle_in_sub
            RDA5888_WriteReg(0x140, 0x4009);//9'h140,16'h4009; angle_mode_if, angle_mode_fm, angle_mode_sub, line625_flag
            RDA5888_WriteReg(0x1a0, 0x1bc3);//9'h1a0,16'h1bc3;Notch filter for audio
            RDA5888_WriteReg(0x1a1, 0x071e);//9'h1a1,16'h071e;Notch filter for audio
            RDA5888_WriteReg(0x1a2, 0x0070);//9'h1a2,16'h0070;Notch filter for audio
            RDA5888_WriteReg(0x082, 0x8541);//9'h082,16'h8541;by 20101230 FM

            RDA5888_WriteReg(0x146, 0x0f1d);//9'h146,16'h0f1d; softmute added 10182010 by xu
            RDA5888_WriteReg(0x147, 0x293e);//9'h147,16'h293e;
            RDA5888_WriteReg(0x148, 0x5b73);//9'h148,16'h5b73;
            RDA5888_WriteReg(0x149, 0x8508);//9'h149,16'h8508;
            // add for audio notch-filter.
            RDA5888_WriteReg(0x1A3, 0xdd52);
            RDA5888_WriteReg(0x1bc, 0x038f);
            RDA5888_WriteReg(0x1bf, 0x0038);
#endif
            ATVDspSystemReset(ATV_SYS_RESET_MODE0);
            m_bIs625Lines = 1;
            g_nBlendMode = BLENDMODE_PALBG;
            break;

        case RDAMTV_VSTD_PAL_D:
        case RDAMTV_VSTD_PAL_D1:
        case RDAMTV_VSTD_PAL_K:
#if (RDA5888_CHIP_VER == 4)
            RDA5888_WriteReg(0x140, 0x402d);//9'h140,16'h402d; angle_mode_if, angle_mode_fm 101
            RDA5888_WriteReg(0x143, 0x1f4b);//9'h143,16'h1f4b; notch coefficients

            RDA5888_WriteReg(0x104, 0x31c7);// 9'h104,16'h31c7; angle_in_if
            RDA5888_WriteReg(0x111, 0x5413);//9'h111,16'h5413; V2.3

            RDA5888_WriteReg(0x1a0, 0x1f39);//9'h1a0,16'h1f39;Notch filter for audio
            RDA5888_WriteReg(0x1a1, 0x071e);//9'h1a1,16'h071e;
            RDA5888_WriteReg(0x1a2, 0x0070);//9'h1a2,16'h0070;
#elif (RDA5888_CHIP_VER == 5)
            RDA5888_WriteReg(0x140, 0x422d);//9'h140,16'h422d; angle_mode_if, angle_mode_fm 101
            RDA5888_WriteReg(0x111, 0x5413);//9'h111,16'h5413; V2.3

            RDA5888_WriteReg(0x1a0, 0x1f39);//9'h1a0,16'h1f39;Notch filter for audio
            RDA5888_WriteReg(0x1a1, 0x071e);//9'h1a1,16'h071e;
            RDA5888_WriteReg(0x1a2, 0x0070);//9'h1a2,16'h0070;
            RDA5888_WriteReg(0x1e0, 0x0116);//9'h1e0,16'h0196;angle_mode_fm
#elif (RDA5888_CHIP_VER == 6)
            RDA5888_WriteReg(0x140, 0x402d);//9'h140,16'h402d; angle_mode_if, angle_mode_fm, angle_mode_sub, line625_flag
            RDA5888_WriteReg(0x111, 0x5413);//9'h111,16'h5413; angle_in_sub

            RDA5888_WriteReg(0x1a0, 0x1f39);//9'h1a0,16'h1f39;Notch filter for audio
            RDA5888_WriteReg(0x1a1, 0x071e);//9'h1a1,16'h071e;Notch filter for audio
            RDA5888_WriteReg(0x1a2, 0x0070);//9'h1a2,16'h0070;Notch filter for audio
            RDA5888_WriteReg(0x1e0, 0x0116);//9'h1e0,16'h0116;by 20100727
#elif (RDA5888_CHIP_VER == 7)
            RDA5888_WriteReg(0x140, 0x402d);//9'h140,16'h402d; angle_mode_if, angle_mode_fm, angle_mode_sub, line625_flag
            RDA5888_WriteReg(0x146, 0x1220);//9'h146,16'h1220;  edited by xu 08062010
            RDA5888_WriteReg(0x147, 0x3046);//9'h147,16'h3046;
            RDA5888_WriteReg(0x148, 0x647a);//9'h148,16'h647a;
            RDA5888_WriteReg(0x149, 0x8808);//9'h149,16'h8808;
            RDA5888_WriteReg(0x111, 0x5413);//9'h111,16'h5413; angle_in_sub
            RDA5888_WriteReg(0x1a0, 0x1f39);//9'h1a0,16'h1f39;Notch filter for audio
            RDA5888_WriteReg(0x1a1, 0x071e);//9'h1a1,16'h071e;Notch filter for audio
            RDA5888_WriteReg(0x1a2, 0x0070);//9'h1a2,16'h0070;Notch filter for audio
            RDA5888_WriteReg(0x082, 0x86c1);//9'h082,16'h86c1;by 20101230 FM
            // add for audio notch-filter.
            RDA5888_WriteReg(0x1A3, 0xf8f2);
            RDA5888_WriteReg(0x1bc, 0x038f);
            RDA5888_WriteReg(0x1bf, 0x0038);
#endif
            ATVDspSystemReset(ATV_SYS_RESET_MODE0);
            m_bIs625Lines = 1;
            g_nBlendMode = BLENDMODE_PALDK;
            break;

        case RDAMTV_VSTD_PAL_I:
#if (RDA5888_CHIP_VER == 4)
            RDA5888_WriteReg(0x140, 0x4024);//9'h140,16'h4024; angle_mode_if, angle_mode_fm 100
            RDA5888_WriteReg(0x143, 0x1d8c);//9'h143,16'h1d8c; notch coefficients
            RDA5888_WriteReg(0x104, 0x284c);//9'h104,16'h284c; angle_in_if
            RDA5888_WriteReg(0x111, 0x5413);//9'h111,16'h5413; V2.3

            RDA5888_WriteReg(0x1a0, 0x1d7a);//9'h1a0,16'h1d7a;Notch filter for audio
            RDA5888_WriteReg(0x1a1, 0x071e);//9'h1a1,16'h071e;
            RDA5888_WriteReg(0x1a2, 0x0070);//9'h1a2,16'h0070;
#elif (RDA5888_CHIP_VER == 5)
            RDA5888_WriteReg(0x140, 0x4224);//9'h140,16'h4224; angle_mode_if, angle_mode_fm 100
            RDA5888_WriteReg(0x111, 0x5413);//9'h111,16'h5413; V2.3

            RDA5888_WriteReg(0x1a0, 0x1d7a);//9'h1a0,16'h1d7a;Notch filter for audio
            RDA5888_WriteReg(0x1a1, 0x071e);//9'h1a1,16'h071e;
            RDA5888_WriteReg(0x1a2, 0x0070);//9'h1a2,16'h0070;
            RDA5888_WriteReg(0x1e0, 0x0112);//9'h1e0,16'h0192;angle_mode_fm
#elif (RDA5888_CHIP_VER == 6)
            RDA5888_WriteReg(0x111, 0x5413);//9'h111,16'h5413; angle_in_sub
            RDA5888_WriteReg(0x140, 0x4024);//9'h140,16'h4024; angle_mode_if, angle_mode_fm, angle_mode_sub, line625_flag

            RDA5888_WriteReg(0x1a0, 0x1d7a);//9'h1a0,16'h1d7a;Notch filter for audio
            RDA5888_WriteReg(0x1a1, 0x071e);//9'h1a1,16'h071e;Notch filter for audio
            RDA5888_WriteReg(0x1a2, 0x0070);//9'h1a2,16'h0070;Notch filter for audio
            RDA5888_WriteReg(0x1e0, 0x0112);//9'h1e0,16'h0112;by 20100727
#elif (RDA5888_CHIP_VER == 7)
            RDA5888_WriteReg(0x111, 0x5413);//9'h111,16'h5413; angle_in_sub
            RDA5888_WriteReg(0x140, 0x4024);//9'h140,16'h4024; angle_mode_if, angle_mode_fm, angle_mode_sub, line625_flag
            RDA5888_WriteReg(0x146, 0x1220);//9'h146,16'h1220;  edited by xu 08062010
            RDA5888_WriteReg(0x147, 0x3046);//9'h147,16'h3046;
            RDA5888_WriteReg(0x148, 0x647a);//9'h148,16'h647a;
            RDA5888_WriteReg(0x149, 0x8808);//9'h149,16'h8808;
            RDA5888_WriteReg(0x1a0, 0x1d7a);//9'h1a0,16'h1d7a;Notch filter for audio
            RDA5888_WriteReg(0x1a1, 0x071e);//9'h1a1,16'h071e;Notch filter for audio
            RDA5888_WriteReg(0x1a2, 0x0070);//9'h1a2,16'h0070;Notch filter for audio
            RDA5888_WriteReg(0x082, 0x8641);//9'h082,16'h8641;by 20101230 FM
            // add for audio notch-filter.
            RDA5888_WriteReg(0x1A3, 0xeb02);
            RDA5888_WriteReg(0x1bc, 0x038f);
            RDA5888_WriteReg(0x1bf, 0x0038);
#endif
            ATVDspSystemReset(ATV_SYS_RESET_MODE0);
            m_bIs625Lines = 1;
            g_nBlendMode = BLENDMODE_PALI;
            break;

        case RDAMTV_VSTD_PAL_M:
#if (RDA5888_CHIP_VER == 4)
            RDA5888_WriteReg(0x140, 0x0040);//9'h140,16'h0040; angle_mode_if, angle_mode_fm 000, angle_mode_sub 001, line625_flag 0
            RDA5888_WriteReg(0x143, 0x1897);//9'h143,16'h1897; notch coefficients
            RDA5888_WriteReg(0x104, 0x20b6);//9'h104,16'h20b6; angle_in_if
            RDA5888_WriteReg(0x111, 0x43ce);//9'h111,16'h5413; V2.3

            RDA5888_WriteReg(0x1a0, 0x1872);//9'h1a0,16'h1872;Notch filter for audio
            RDA5888_WriteReg(0x1a1, 0x0749);//9'h1a1,16'h0749;
            RDA5888_WriteReg(0x1a2, 0x005b);//9'h1a2,16'h005b;
#elif (RDA5888_CHIP_VER == 5)
            RDA5888_WriteReg(0x140, 0x0240);//9'h140,16'h0240; angle_mode_if, angle_mode_fm 000, angle_mode_sub 001, line625_flag 0
            RDA5888_WriteReg(0x111, 0x43ce);//9'h111,16'h5413; V2.3

            RDA5888_WriteReg(0x1a0, 0x1872);//9'h1a0,16'h1872;Notch filter for audio
            RDA5888_WriteReg(0x1a1, 0x0749);//9'h1a1,16'h0749;
            RDA5888_WriteReg(0x1a2, 0x005b);//9'h1a2,16'h005b;
            RDA5888_WriteReg(0x1e0, 0x0102);//9'h1e0,16'h0182;angle_mode_fm
#elif (RDA5888_CHIP_VER == 6)
            RDA5888_WriteReg(0x111, 0x43ce);//9'h111,16'h43ce; angle_in_sub
            RDA5888_WriteReg(0x140, 0x0040);//9'h140,16'h0040; angle_mode_if, angle_mode_fm, angle_mode_sub, line625_flag
            RDA5888_WriteReg(0x1a0, 0x1872);//9'h1a0,16'h1872; Notch filter for audio
            RDA5888_WriteReg(0x1a1, 0x0749);//9'h1a1,16'h0749; Notch filter for audio
            RDA5888_WriteReg(0x1a2, 0x005b);//9'h1a2,16'h005b; Notch filter for audio
            RDA5888_WriteReg(0x1a9, 0x1c00);//9'h1a9,16'h1c00; lum_path_delay
            RDA5888_WriteReg(0x1ba, 0x06b3);//9'h1ba,16'h06b3; comb_constant *
            RDA5888_WriteReg(0x1e0, 0x0102);//9'h1e0,16'h0102;by 20100727
            RDA5888_WriteReg(0x15a, 0x010c);//9'h15A,16'h010c; added by xu 09272010 for snow
            RDA5888_WriteReg(0x15b, 0x06b3);//9'h15B,16'h06B3;
            RDA5888_WriteReg(0x1ae, 0x420c);//9'h1AE,16'h420c;
            RDA5888_WriteReg(0x146, 0x0f1c);//9'h146,16'h0f1c; softmute added 10182010 by xu
            RDA5888_WriteReg(0x147, 0x283a);//9'h147,16'h283a;
            RDA5888_WriteReg(0x148, 0x5367);//9'h148,16'h5367;
            RDA5888_WriteReg(0x149, 0x7708);//9'h149,16'h7708;
#elif (RDA5888_CHIP_VER == 7)
            RDA5888_WriteReg(0x111, 0x43ce);//9'h111,16'h43ce; angle_in_sub
            RDA5888_WriteReg(0x140, 0x0040);//9'h140,16'h0040; angle_mode_if, angle_mode_fm, angle_mode_sub, line625_flag
            RDA5888_WriteReg(0x1a0, 0x1872);//9'h1a0,16'h1872; Notch filter for audio
            RDA5888_WriteReg(0x1a1, 0x0749);//9'h1a1,16'h0749; Notch filter for audio
            RDA5888_WriteReg(0x1a2, 0x005b);//9'h1a2,16'h005b; Notch filter for audio
            RDA5888_WriteReg(0x1a9, 0x1c00);//9'h1a9,16'h1c00; lum_path_delay
            RDA5888_WriteReg(0x1ba, 0x06b3);//9'h1ba,16'h06b3; comb_constant *
            RDA5888_WriteReg(0x082, 0x8441);//9'h082,16'h8441;by 20101230 FM

            RDA5888_WriteReg(0x15a, 0x010c);//9'h15A,16'h010c; added by xu 09272010 for snow
            RDA5888_WriteReg(0x15b, 0x06b3);//9'h15B,16'h06B3;
            RDA5888_WriteReg(0x1ae, 0x420c);//9'h1AE,16'h420c;

            RDA5888_WriteReg(0x146, 0x0f1c);//9'h146,16'h0f1c; softmute added 10182010 by xu
            RDA5888_WriteReg(0x147, 0x283a);//9'h147,16'h283a;
            RDA5888_WriteReg(0x148, 0x5367);//9'h148,16'h5367;
            RDA5888_WriteReg(0x149, 0x7007);//9'h149,16'h7007;
            // add for audio notch-filter.
            RDA5888_WriteReg(0x1A3, 0xc382);
            RDA5888_WriteReg(0x1bc, 0x038f);
            RDA5888_WriteReg(0x1bf, 0x0038);
#endif
            ATVDspSystemReset(ATV_SYS_RESET_MODE0);
            m_bIs625Lines = 0;
            g_nBlendMode = BLENDMODE_PALM;
            break;

        case RDAMTV_VSTD_PAL_H:
            break;

        case RDAMTV_VSTD_PAL_N:
        case RDAMTV_VSTD_PAL_NC:
#if (RDA5888_CHIP_VER == 4)
            RDA5888_WriteReg(0x140, 0x4100);//9'h140,16'h4100; angle_mode_if, angle_mode_fm 000, angle_mode_sub 100, line625_flag 1
            RDA5888_WriteReg(0x143, 0x1897);//9'h143,16'h1897; notch coefficients
            RDA5888_WriteReg(0x104, 0x20b6);//9'h104,16'h20b6; angle_in_if
            RDA5888_WriteReg(0x111, 0x43ed);//9'h111,16'h5413; V2.3
            RDA5888_WriteReg(0x1a0, 0x1872);//9'h1a0,16'h1872;Notch filter for audio
            RDA5888_WriteReg(0x1a1, 0x0749);//9'h1a1,16'h0749;
            RDA5888_WriteReg(0x1a2, 0x005b);//9'h1a2,16'h005b;
#elif (RDA5888_CHIP_VER == 5)
            RDA5888_WriteReg(0x140, 0x4300);//9'h140,16'h4300; angle_mode_if, angle_mode_fm 000, angle_mode_sub 100, line625_flag 1
            RDA5888_WriteReg(0x111, 0x43ed);//9'h111,16'h5413; V2.3
            RDA5888_WriteReg(0x1a0, 0x1872);//9'h1a0,16'h1872;Notch filter for audio
            RDA5888_WriteReg(0x1a1, 0x0749);//9'h1a1,16'h0749;
            RDA5888_WriteReg(0x1a2, 0x005b);//9'h1a2,16'h005b;
            RDA5888_WriteReg(0x1e0, 0x0102);//9'h1e0,16'h0182;angle_mode_fm
#elif (RDA5888_CHIP_VER == 6)
            RDA5888_WriteReg(0x140, 0x4100);//9'h140,16'h4100; angle_mode_if, angle_mode_fm, angle_mode_sub, line625_flag
            RDA5888_WriteReg(0x111, 0x43ed);//9'h111,16'h43ed; angle_in_sub
            RDA5888_WriteReg(0x1a0, 0x1872);//9'h1a0,16'h1872;Notch filter for audio
            RDA5888_WriteReg(0x1a1, 0x0749);//9'h1a1,16'h0749;Notch filter for audio
            RDA5888_WriteReg(0x1a2, 0x005b);//9'h1a2,16'h005b;Notch filter for audio
            RDA5888_WriteReg(0x1e0, 0x0102);//9'h1e0,16'h0102;by 20100727
            RDA5888_WriteReg(0x146, 0x0f1c);//9'h146,16'h0f1c; softmute added 10182010 by xu
            RDA5888_WriteReg(0x147, 0x283a);//9'h147,16'h283a;
            RDA5888_WriteReg(0x148, 0x5367);//9'h148,16'h5367;
            RDA5888_WriteReg(0x149, 0x7708);//9'h149,16'h7708;
#elif (RDA5888_CHIP_VER == 7)
            RDA5888_WriteReg(0x140, 0x4100);//9'h140,16'h4100; angle_mode_if, angle_mode_fm, angle_mode_sub, line625_flag
            RDA5888_WriteReg(0x111, 0x43ed);//9'h111,16'h43ed; angle_in_sub
            RDA5888_WriteReg(0x1a0, 0x1872);//9'h1a0,16'h1872;Notch filter for audio
            RDA5888_WriteReg(0x1a1, 0x0749);//9'h1a1,16'h0749;Notch filter for audio
            RDA5888_WriteReg(0x1a2, 0x005b);//9'h1a2,16'h005b;Notch filter for audio
            RDA5888_WriteReg(0x082, 0x8441);//9'h082,16'h8441;by 20101230 FM
            RDA5888_WriteReg(0x146, 0x0f1c);//9'h146,16'h0f1c
            RDA5888_WriteReg(0x147, 0x283a);//9'h147,16'h283a
            RDA5888_WriteReg(0x148, 0x5367);//9'h148,16'h5367
            RDA5888_WriteReg(0x149, 0x7007);//9'h149,16'h7007
            // add for audio notch-filter.
            RDA5888_WriteReg(0x1A3, 0xc382);
            RDA5888_WriteReg(0x1bc, 0x038f);
            RDA5888_WriteReg(0x1bf, 0x0038);
#endif
            ATVDspSystemReset(ATV_SYS_RESET_MODE0);
            m_bIs625Lines = 1;
            g_nBlendMode = BLENDMODE_PALN;
            break;

        case RDAMTV_VSTD_SECAM_B:
        case RDAMTV_VSTD_SECAM_B1:
        case RDAMTV_VSTD_SECAM_G:
#if (RDA5888_CHIP_VER == 4)
            RDA5888_WriteReg(0x140, 0x40c9);//angle_mode_if, angle_mode_fm
            RDA5888_WriteReg(0x143, 0x1bd5);//notch coefficients
            RDA5888_WriteReg(0x104, 0x284c);//9'h104,16'h284c; angle_in_if
            RDA5888_WriteReg(0x111, 0x5142);//9'h111,16'h5413; V2.3
            RDA5888_WriteReg(0x116, 0x008c);//u_cb_dis, k1a
            RDA5888_WriteReg(0x117, 0x0fd2);//k1b
            RDA5888_WriteReg(0x118, 0x3fbf);//auto black
            RDA5888_WriteReg(0x1a0, 0x1bc3);//9'h1a0,16'h1bc3;Notch filter for audio
            RDA5888_WriteReg(0x1a1, 0x071e);//9'h1a1,16'h071e;
            RDA5888_WriteReg(0x1a2, 0x0070);//9'h1a2,16'h0070;
#elif (RDA5888_CHIP_VER == 5)
            RDA5888_WriteReg(0x140, 0x42c9);//angle_mode_if, angle_mode_fm
            RDA5888_WriteReg(0x111, 0x5142);//9'h111,16'h5413; V2.3
            RDA5888_WriteReg(0x116, 0x008c);//u_cb_dis, k1a
            RDA5888_WriteReg(0x117, 0x0fd2);//k1b
            RDA5888_WriteReg(0x118, 0x3fbf);//auto black
            RDA5888_WriteReg(0x1a0, 0x1bc3);//9'h1a0,16'h1bc3;Notch filter for audio
            RDA5888_WriteReg(0x1a1, 0x071e);//9'h1a1,16'h071e;
            RDA5888_WriteReg(0x1a2, 0x0070);//9'h1a2,16'h0070;
            RDA5888_WriteReg(0x1e0, 0x010a);//9'h1e0,16'h018a;angle_mode_fm
#elif (RDA5888_CHIP_VER == 6)
            RDA5888_WriteReg(0x111, 0x5142);//9'h111,16'h5142;
            RDA5888_WriteReg(0x116, 0x008c);//9'h116,16'h008c; u_cb_dis, k1a
            RDA5888_WriteReg(0x117, 0x0fd2);//9'h117,16'h0fd2; k1b
            RDA5888_WriteReg(0x118, 0x3fbf);//9'h118,16'h3fbf;
            RDA5888_WriteReg(0x140, 0x40c9);//9'h140,16'h40c9; angle_mode_if, angle_mode_fm, angle_mode_sub, line625_flag
            RDA5888_WriteReg(0x1a0, 0x1bc3);//9'h1a0,16'h1bc3;
            RDA5888_WriteReg(0x1a1, 0x071e);//9'h1a1,16'h071e;
            RDA5888_WriteReg(0x1a2, 0x0070);//9'h1a2,16'h0070;
            RDA5888_WriteReg(0x1e0, 0x010a);//9'h1e0,16'h010A;by 20100727
#elif (RDA5888_CHIP_VER == 7)
            RDA5888_WriteReg(0x111, 0x5142);//9'h111,16'h5142;
            RDA5888_WriteReg(0x116, 0x008c);//9'h116,16'h008c; u_cb_dis, k1a
            RDA5888_WriteReg(0x117, 0x0fd2);//9'h117,16'h0fd2; k1b
            RDA5888_WriteReg(0x118, 0x3fbf);//9'h118,16'h3fbf;
            RDA5888_WriteReg(0x140, 0x40c9);//9'h140,16'h40c9; angle_mode_if, angle_mode_fm, angle_mode_sub, line625_flag
            RDA5888_WriteReg(0x145, 0x00d0);//9'h145,16'h00d0;
            RDA5888_WriteReg(0x146, 0x4550);//9'h146,16'h4550;
            RDA5888_WriteReg(0x147, 0x5e67);//9'h147,16'h5e67;
            RDA5888_WriteReg(0x148, 0x7178);//9'h148,16'h7178;
            RDA5888_WriteReg(0x149, 0x7c02);//9'h149,16'h7c02;
            RDA5888_WriteReg(0x1a0, 0x1bc3);//9'h1a0,16'h1bc3;
            RDA5888_WriteReg(0x1a1, 0x071e);//9'h1a1,16'h071e;
            RDA5888_WriteReg(0x1a2, 0x0070);//9'h1a2,16'h0070;
            RDA5888_WriteReg(0x082, 0x8541);//9'h082,16'h8541;by 20101230 FM

            RDA5888_WriteReg(0x1e8, 0x4000);//9'h1e8,16'h4000;by 20101214

            // add for audio notch-filter.
            RDA5888_WriteReg(0x1A3, 0xdd52);
            RDA5888_WriteReg(0x1bc, 0x038f);
            RDA5888_WriteReg(0x1bf, 0x0038);
#endif
            ATVDspSystemReset(ATV_SYS_RESET_MODE2);
            m_bIs625Lines = 1;
            g_nBlendMode = BLENDMODE_INVALID;
            break;

        case RDAMTV_VSTD_SECAM_D:
        case RDAMTV_VSTD_SECAM_D1:
        case RDAMTV_VSTD_SECAM_K:
#if (RDA5888_CHIP_VER == 4)
            RDA5888_WriteReg(0x140,0x40ed);//angle_mode_if, angle_mode_fm
            RDA5888_WriteReg(0x143,0x1f4b);//notch coefficients
            RDA5888_WriteReg(0x104,0x31c7);//9'h104,16'h31c7; angle_in_if
            RDA5888_WriteReg(0x111, 0x5142);//9'h111,16'h5413; V2.3
            RDA5888_WriteReg(0x116,0x008c);//u_cb_dis, k1a
            RDA5888_WriteReg(0x117,0x0fd2);//k1b
            RDA5888_WriteReg(0x118,0x3fbf);//auto black
            RDA5888_WriteReg(0x1a0, 0x1f39);//9'h1a0,16'h1f39;Notch filter for audio
            RDA5888_WriteReg(0x1a1, 0x071e);//9'h1a1,16'h071e;
            RDA5888_WriteReg(0x1a2, 0x0070);//9'h1a2,16'h0070;
#elif (RDA5888_CHIP_VER == 5)
            RDA5888_WriteReg(0x140,0x42ed);//angle_mode_if, angle_mode_fm
            RDA5888_WriteReg(0x111, 0x5142);//9'h111,16'h5413; V2.3
            RDA5888_WriteReg(0x116,0x008c);//u_cb_dis, k1a
            RDA5888_WriteReg(0x117,0x0fd2);//k1b
            RDA5888_WriteReg(0x118,0x3fbf);//auto black
            RDA5888_WriteReg(0x1a0, 0x1f39);//9'h1a0,16'h1f39;Notch filter for audio
            RDA5888_WriteReg(0x1a1, 0x071e);//9'h1a1,16'h071e;
            RDA5888_WriteReg(0x1a2, 0x0070);//9'h1a2,16'h0070;
            RDA5888_WriteReg(0x1e0, 0x0116);//9'h1e0,16'h0196;angle_mode_fm
#elif (RDA5888_CHIP_VER == 6)
            RDA5888_WriteReg(0x10b, 0x8708);//9'h10b,16'h8708;
            RDA5888_WriteReg(0x111, 0x5142);//9'h111,16'h5142;
            RDA5888_WriteReg(0x116, 0x008c);//9'h116,16'h008c; u_cb_dis, k1a
            RDA5888_WriteReg(0x117, 0x0fd2);//9'h117,16'h0fd2; k1b
            RDA5888_WriteReg(0x118, 0x3fbf);//9'h118,16'h3fbf;
            RDA5888_WriteReg(0x140, 0x40ed);//9'h140,16'h40ed; angle_mode_if, angle_mode_fm, angle_mode_sub, line625_flag
            RDA5888_WriteReg(0x1a0, 0x1f39);//9'h1a0,16'h1f39;
            RDA5888_WriteReg(0x1a1, 0x071e);//9'h1a1,16'h071e;
            RDA5888_WriteReg(0x1a2, 0x0070);//9'h1a2,16'h0070;
            RDA5888_WriteReg(0x1e0, 0x0116);//9'h1e0,16'h0116;by 20100727
            //RDA5888_WriteReg(0x1ae, 0x10a8);//9'h1ae,16'h10a8;
#elif (RDA5888_CHIP_VER == 7)
            RDA5888_WriteReg(0x10b, 0x8708);//9'h10b,16'h8708;
            RDA5888_WriteReg(0x111, 0x5142);//9'h111,16'h5142;
            RDA5888_WriteReg(0x112, 0x3000);//9'h112,16'h3000;
            RDA5888_WriteReg(0x116, 0x008c);//9'h116,16'h008c; u_cb_dis, k1a
            RDA5888_WriteReg(0x117, 0x0fd2);//9'h117,16'h0fd2; k1b
            RDA5888_WriteReg(0x118, 0x3fbf);//9'h118,16'h3fbf;
            RDA5888_WriteReg(0x140, 0x40ed);//9'h140,16'h40ed; angle_mode_if, angle_mode_fm, angle_mode_sub, line625_flag
            RDA5888_WriteReg(0x145, 0x00d0);//9'h145,16'h00d0;

            RDA5888_WriteReg(0x146, 0x4550);//9'h146,16'h4550;
            RDA5888_WriteReg(0x147, 0x5e67);//9'h147,16'h5e67;
            RDA5888_WriteReg(0x148, 0x7178);//9'h148,16'h7178;
            RDA5888_WriteReg(0x149, 0x7c01);//9'h149,16'h7c01;

            RDA5888_WriteReg(0x1a0, 0x1f39);//9'h1a0,16'h1f39;
            RDA5888_WriteReg(0x1a1, 0x071e);//9'h1a1,16'h071e;
            RDA5888_WriteReg(0x1a2, 0x0070);//9'h1a2,16'h0070;
            RDA5888_WriteReg(0x082, 0x86c1);//9'h082,16'h86c1;by 20101230 FM

            RDA5888_WriteReg(0x1e8, 0x4000);//9'h1e8,16'h4000;by 20101214
            // add for audio notch-filter.
            RDA5888_WriteReg(0x1A3, 0xf8f2);
            RDA5888_WriteReg(0x1bc, 0x038f);
            RDA5888_WriteReg(0x1bf, 0x0038);
#endif
            ATVDspSystemReset(ATV_SYS_RESET_MODE2);
            m_bIs625Lines = 1;
            g_nBlendMode = BLENDMODE_INVALID;
            break;

        case RDAMTV_VSTD_SECAM_L:
            break;

        case RDAMTV_VSTD_NTSC_B_G:
#if (RDA5888_CHIP_VER == 4)
            RDA5888_WriteReg(0x140,0x0009);//angle_mode_if, angle_mode_fm
            RDA5888_WriteReg(0x143,0x1bd5);//notch coefficients
            RDA5888_WriteReg(0x104, 0x284c);//9'h111,16'h5413; V2.3
            RDA5888_WriteReg(0x111, 0x5413);//9'h111,16'h5413; V2.3
            RDA5888_WriteReg(0x137,0x034c);//h_sync_win_end
            RDA5888_WriteReg(0x139,0x03ac);//h_blank_win_end
            RDA5888_WriteReg(0x1a0, 0x1bc3);//9'h1a0,16'h1bc3;Notch filter for audio
            RDA5888_WriteReg(0x1a1, 0x071e);//9'h1a1,16'h071e;
            RDA5888_WriteReg(0x1a2, 0x0070);//9'h1a2,16'h0070;
#elif (RDA5888_CHIP_VER == 5)
            RDA5888_WriteReg(0x140,0x0209);//angle_mode_if, angle_mode_fm
            RDA5888_WriteReg(0x111, 0x5413);//9'h111,16'h5413; V2.3

            RDA5888_WriteReg(0x137,0x034c);//h_sync_win_end
            RDA5888_WriteReg(0x139,0x03ac);//h_blank_win_end

            RDA5888_WriteReg(0x1a0, 0x1bc3);//9'h1a0,16'h1bc3;Notch filter for audio
            RDA5888_WriteReg(0x1a1, 0x071e);//9'h1a1,16'h071e;
            RDA5888_WriteReg(0x1a2, 0x0070);//9'h1a2,16'h0070;
            RDA5888_WriteReg(0x1e0, 0x010a);//9'h1e0,16'h018a;angle_mode_fm
#endif
            ATVDspSystemReset(ATV_SYS_RESET_MODE1);
            m_bIs625Lines = 0;
            g_nBlendMode = BLENDMODE_NTSCBG;
            break;

        case RDAMTV_VSTD_NTSC_D_K:
#if (RDA5888_CHIP_VER == 4)
            RDA5888_WriteReg(0x140,0x002d);//angle_mode_if, angle_mode_fm
            RDA5888_WriteReg(0x143,0x1f4b);//notch coefficients
            RDA5888_WriteReg(0x104, 0x31c7);//9'h104,16'h31c7; angle_in_if
            RDA5888_WriteReg(0x111, 0x5413);//9'h111,16'h5413; V2.3
            RDA5888_WriteReg(0x137,0x034c);//h_sync_win_end
            RDA5888_WriteReg(0x139,0x03ac);//h_blank_win_end
            RDA5888_WriteReg(0x1a0, 0x1f39);//9'h1a0,16'h1f39;Notch filter for audio
            RDA5888_WriteReg(0x1a1, 0x071e);//9'h1a1,16'h071e;
            RDA5888_WriteReg(0x1a2, 0x0070);//9'h1a2,16'h0070;
#elif (RDA5888_CHIP_VER == 5)
            RDA5888_WriteReg(0x140,0x022d);//angle_mode_if, angle_mode_fm
            RDA5888_WriteReg(0x111, 0x5413);//9'h111,16'h5413; V2.3
            RDA5888_WriteReg(0x137,0x034c);//h_sync_win_end
            RDA5888_WriteReg(0x139,0x03ac);//h_blank_win_end
            RDA5888_WriteReg(0x1a0, 0x1f39);//9'h1a0,16'h1f39;Notch filter for audio
            RDA5888_WriteReg(0x1a1, 0x071e);//9'h1a1,16'h071e;
            RDA5888_WriteReg(0x1a2, 0x0070);//9'h1a2,16'h0070;
            RDA5888_WriteReg(0x1e0, 0x0116);//9'h1e0,16'h0196;angle_mode_fm
#endif
            ATVDspSystemReset(ATV_SYS_RESET_MODE1);
            m_bIs625Lines = 0;
            g_nBlendMode = BLENDMODE_NTSCDK;
            break;

        case RDAMTV_VSTD_NTSC_I:
#if (RDA5888_CHIP_VER == 4)
            RDA5888_WriteReg(0x140,0x0024);//angle_mode_if, angle_mode_fm
            RDA5888_WriteReg(0x143,0x1d8c);//notch coefficients
            RDA5888_WriteReg(0x104,0x284c);//9'h104,16'h284c; angle_in_if
            RDA5888_WriteReg(0x111,0x5413);//9'h111,16'h5413; V2.3
            RDA5888_WriteReg(0x137,0x034c);//h_sync_win_end
            RDA5888_WriteReg(0x139,0x03ac);//h_blank_win_end
            RDA5888_WriteReg(0x1a0, 0x1d7a);//9'h1a0,16'h1f39;Notch filter for audio
            RDA5888_WriteReg(0x1a1, 0x071e);//9'h1a1,16'h071e;
            RDA5888_WriteReg(0x1a2, 0x0070);//9'h1a2,16'h0070;
#elif (RDA5888_CHIP_VER == 5)
            RDA5888_WriteReg(0x140,0x0224);//angle_mode_if, angle_mode_fm
            RDA5888_WriteReg(0x111,0x5413);//9'h111,16'h5413; V2.3
            RDA5888_WriteReg(0x137,0x034c);//h_sync_win_end
            RDA5888_WriteReg(0x139,0x03ac);//h_blank_win_end
            RDA5888_WriteReg(0x1a0, 0x1d7a);//9'h1a0,16'h1f39;Notch filter for audio
            RDA5888_WriteReg(0x1a1, 0x071e);//9'h1a1,16'h071e;
            RDA5888_WriteReg(0x1a2, 0x0070);//9'h1a2,16'h0070;
            RDA5888_WriteReg(0x1e0, 0x0112);//9'h1e0,16'h0192;angle_mode_fm
#endif
            ATVDspSystemReset(ATV_SYS_RESET_MODE1);
            m_bIs625Lines = 0;
            g_nBlendMode = BLENDMODE_NTSCI;
            break;

        case RDAMTV_VSTD_NTSC_M:
#if (RDA5888_CHIP_VER == 4)
            RDA5888_WriteReg(0x140, 0x0080);//9'h140,16'h0040; angle_mode_if, angle_mode_fm
            RDA5888_WriteReg(0x143, 0x1897);//9'h143,16'h1897; notch coefficients
            RDA5888_WriteReg(0x104, 0x20b6);//9'h104,16'h20b6; angle_in_if
            RDA5888_WriteReg(0x111, 0x43e1);//9'h111,16'h5413; V2.3
            RDA5888_WriteReg(0x137, 0x034c);//9'h137,16'h034c; h_sync_win_end
            RDA5888_WriteReg(0x139, 0x03ac);//9'h139,16'h03ac; h_blank_win_end
            RDA5888_WriteReg(0x1a0, 0x1872);//9'h1a0,16'h1872;Notch filter for audio
            RDA5888_WriteReg(0x1a1, 0x0749);//9'h1a1,16'h0749;
            RDA5888_WriteReg(0x1a2, 0x005b);//9'h1a2,16'h005b;
            RDA5888_WriteReg(0x1ac, 0x0030);
#elif (RDA5888_CHIP_VER == 5)
            RDA5888_WriteReg(0x140, 0x0280);//9'h140,16'h0040; angle_mode_if, angle_mode_fm
            RDA5888_WriteReg(0x111, 0x43e1);//9'h111,16'h5413; V2.3
            RDA5888_WriteReg(0x137, 0x034c);//9'h137,16'h034c; h_sync_win_end
            RDA5888_WriteReg(0x139, 0x03ac);//9'h139,16'h03ac; h_blank_win_end
            RDA5888_WriteReg(0x1a0, 0x1872);//9'h1a0,16'h1872;Notch filter for audio
            RDA5888_WriteReg(0x1a1, 0x0749);//9'h1a1,16'h0749;
            RDA5888_WriteReg(0x1a2, 0x005b);//9'h1a2,16'h005b;
            //RDA5888_WriteReg(0x1ac, 0x2810);
            RDA5888_WriteReg(0x1e0, 0x0102);//9'h1e0,16'h0182;angle_mode_fm
#elif (RDA5888_CHIP_VER == 6)
            RDA5888_WriteReg(0x10b, 0x42f9);//9'h10B,16'h42f9;
            RDA5888_WriteReg(0x111, 0x43e1);//9'h111,16'h43e1; angle_in_sub
            RDA5888_WriteReg(0x137, 0x034c);//9'h137,16'h034c; h_sync_win_end
            RDA5888_WriteReg(0x139, 0x03ac);//9'h139,16'h03ac; h_blank_win_end
            RDA5888_WriteReg(0x140, 0x0080);//9'h140,16'h0080; angle_mode_if, angle_mode_fm, angle_mode_sub, line625_flag
            RDA5888_WriteReg(0x1a0, 0x1872);//9'h1a0,16'h1872; Notch filter for audio
            RDA5888_WriteReg(0x1a1, 0x0749);//9'h1a1,16'h0749; Notch filter for audio
            RDA5888_WriteReg(0x1a2, 0x005b);//9'h1a2,16'h005b; Notch filter for audio
            RDA5888_WriteReg(0x1ba, 0x06b3);//9'h1ba,16'h06b3; comb_constant *
            RDA5888_WriteReg(0x1e0, 0x0102);//9'h1e0,16'h0102;by 20100727
            RDA5888_WriteReg(0x15a, 0x010c);//9'h15A,16'h010c; added by xu 09272010 for snow
            RDA5888_WriteReg(0x15b, 0x06b3);//9'h15B,16'h06B3;
            RDA5888_WriteReg(0x1ae, 0x420c);//9'h1AE,16'h420c;
            RDA5888_WriteReg(0x146, 0x0f16);//9'h146,16'h0f16;  softmute added 10182010 by xu
            RDA5888_WriteReg(0x147, 0x2634);//9'h147,16'h2634;
            RDA5888_WriteReg(0x148, 0x4b61);//9'h148,16'h4b61;
            RDA5888_WriteReg(0x149, 0x7108);//9'h149,16'h7108;
#elif (RDA5888_CHIP_VER == 7)
            RDA5888_WriteReg(0x10b, 0x42f9);//9'h10B,16'h42f9;
            RDA5888_WriteReg(0x111, 0x43e1);//9'h111,16'h43e1; angle_in_sub
            RDA5888_WriteReg(0x137, 0x034c);//9'h137,16'h034c; h_sync_win_end
            RDA5888_WriteReg(0x139, 0x03ac);//9'h139,16'h03ac; h_blank_win_end
            RDA5888_WriteReg(0x140, 0x0080);//9'h140,16'h0080; angle_mode_if, angle_mode_fm, angle_mode_sub, line625_flag
            RDA5888_WriteReg(0x1a0, 0x1872);//9'h1a0,16'h1872; Notch filter for audio
            RDA5888_WriteReg(0x1a1, 0x0749);//9'h1a1,16'h0749; Notch filter for audio
            RDA5888_WriteReg(0x1a2, 0x005b);//9'h1a2,16'h005b; Notch filter for audio
            RDA5888_WriteReg(0x1ba, 0x06b3);//9'h1ba,16'h06b3; comb_constant *
            RDA5888_WriteReg(0x082, 0x8441);//9'h082,16'h8441;by 20101230 FM

            RDA5888_WriteReg(0x15a, 0x010c);//9'h15A,16'h010c; added by xu 09272010 for snow
            RDA5888_WriteReg(0x15b, 0x06b3);//9'h15B,16'h06B3;
            RDA5888_WriteReg(0x1ae, 0x420c);//9'h1AE,16'h420c;

            RDA5888_WriteReg(0x146, 0x0f16);//9'h146,16'h0f16;  softmute added 10182010 by xu
            RDA5888_WriteReg(0x147, 0x2634);//9'h147,16'h2634;
            RDA5888_WriteReg(0x148, 0x4b61);//9'h148,16'h4b61;
            RDA5888_WriteReg(0x149, 0x6e07);//9'h149,16'h6e07;
            // add for audio notch-filter.
            RDA5888_WriteReg(0x1A3, 0xc382);
            RDA5888_WriteReg(0x1bc, 0x038f);
            RDA5888_WriteReg(0x1bf, 0x0038);
#endif
            ATVDspSystemReset(ATV_SYS_RESET_MODE1);
            m_bIs625Lines = 0;
            g_nBlendMode = BLENDMODE_NTSCM;
            break;

        default:
            break;
    }

    return (RDAMTV_RT_SUCCESS);
}

// ----------------------------------------------------------------------------
// This function sets the video decoder to the test pattern.
//
// Params:
//  None
//
// Return:
//  RDAMTV_RT_ERROR if an error occurred communicating with the device.
//  RDAMTV_RT_SUCCESS if the operation was successful.
// ----------------------------------------------------------------------------
uint32 RDA5888_SetTestPattern(void)
{
    uint32 rt = RDAMTV_RT_SUCCESS;

#if ((RDA5888_CHIP_VER == 4) ||(RDA5888_CHIP_VER == 5))
    RDA5888_DCDCInit();
    RDA5888_ScanChannel(503000000, RDAMTV_VSTD_PAL_D, 0);
    RDA5888_WriteReg(0x130, 0x0803);
    RDA5888_WriteReg(0x116, 0x0271);
    //RDA5888_WriteReg(0x1ae, 0x4271); // for 5888s colorbar.
    //RDA5888_WriteReg(0x1b0, 0xc000); // for 5888e colorbar.
#endif

    return rt;
}

// ----------------------------------------------------------------------------
// This function initializes the rda5888 analog module.
//
// Params:
//  none.
//
// Return:
//  RDAMTV_RT_ERROR if an error occurred communicating with the device.
//  RDAMTV_RT_SUCCESS if the operation was successful.
// ----------------------------------------------------------------------------
uint32 RDA5888_DCDCInit(void)
{
    uint32 rt = RDAMTV_RT_SUCCESS;
    unsigned short reg;

    // check chipid
    RDA5888_ReadReg(0x0, &reg);
    RDAEXT_DebugPrint(RDA5888_TRACE_MOD  "[rdamtv]chipid = %x, chip_ver = %d\n", reg, RDA5888_CHIP_VER);

#if (RDA5888_CHIP_VER == 4)
    RDA5888_WriteReg(0x001, 0x0000);//9'0x01,16'h0000;
    RDA5888_WriteReg(0x001, 0x0115);//9'h01,16'h0115;soft reset
    RDAEXT_DelayMs((uint16)10);

    RDA5888_WriteReg(0x001, 0x091B);//9'h001,16'h091B; agc_en_b
    RDA5888_WriteReg(0x053, 0x0e50);//9'h053,16'h0e50; lna current
    RDA5888_WriteReg(0x05a, 0x0100);//9'h05a,16'h0100; lna_th0
    RDA5888_WriteReg(0x05b, 0x0280);//9'h05b,16'h0280; lna_th1
    RDA5888_WriteReg(0x062, 0x053f);//9'h62,16'h053f; vco high low th 705M
    RDA5888_WriteReg(0x066, 0x0470);//9'h66,16'h0470; cfc th0
    RDA5888_WriteReg(0x067, 0x0538);//9'h67,16'h0538; cfc th1
    RDA5888_WriteReg(0x068, 0x0648);//9'h68,16'h0648; cfc th2
    RDA5888_WriteReg(0x06a, 0xcccc);//9'h6a,16'hcccc; rfpll_gain0~3
    RDA5888_WriteReg(0x072, 0xaaaa);//9'h072,16'hAAAA; vco ibit 1010
    RDA5888_WriteReg(0x084, 0x000f);//9'h084,16'h000F; Analog Filter
    RDA5888_WriteReg(0x08e, 0x3400);//9'h8e,16'h3400; int_dec_sel 00
    RDA5888_WriteReg(0x086, 0x1045);//9'h86,16'h1045; mdll_test 1
    RDA5888_WriteReg(0x087, 0x0c12);//9'h087,16'h0c12;
    RDA5888_WriteReg(0x080, 0x004d);//9'h80,16'h004d; adc_sel_clk 1
    RDA5888_WriteReg(0x088, 0x6524);//9'h088,16'h6524; xtal cap

    RDA5888_WriteReg(0x098, 0x0400);//9'h098,16'h0400; os_en, dc_i
    RDA5888_WriteReg(0x099, 0x0000);//9'h099,16'h0000; dc_q
    RDA5888_WriteReg(0x0b0, 0x0140);//9'h0b0,16'h0140; pllbb

    RDA5888_WriteReg(0x042, 0x0924);//9'h42,16'h0FF4; analog gain
    RDA5888_WriteReg(0x043, 0x0FFC);//9'0x43,16'0x0FFF;
    RDA5888_WriteReg(0x044, 0x0000);//9'0x44,16'0x1000;
    RDA5888_WriteReg(0x045, 0xF700);//9'0x45,16'0xFF73;

    RDA5888_WriteReg(0x046, 0x0000);//9'0x46,16'0x0000;
    RDA5888_WriteReg(0x047, 0x0000);//9'0x47,16'0x0000;
    RDA5888_WriteReg(0x048, 0x0000);//9'0x48,16'0x0000;
    RDA5888_WriteReg(0x049, 0x0000);//9'0x49,16'0x0000;
    RDA5888_WriteReg(0x04A, 0x0000);//9'0x4A,16'0x0000;
    RDA5888_WriteReg(0x04B, 0x0000);//9'0x4B,16'0x0000;
    RDA5888_WriteReg(0x04C, 0x28FF);//9'H04C,16'H28F3; // inc adac gain 0x28F6 0x28FF
    RDA5888_WriteReg(0x04D, 0x00F0);//9'0x4D,16'0x00F0;
    RDA5888_WriteReg(0x04E, 0x0000);//9'0x4E,16'0x0000;
    RDA5888_WriteReg(0x04F, 0xC000);//9'0x4F,16'0xC000;
    RDA5888_WriteReg(0x050, 0x0000);//9'0x50,16'0x0000;
    RDA5888_WriteReg(0x051, 0x0000);//9'0x51,16'0x0000;
    RDA5888_WriteReg(0x052, 0x0000);//9'0x52,16'0x0000;

#ifdef RDA5888_LDOMODE_ENABLE
    RDA5888_WriteReg(0x0a1, 0x018b);// 9'h0a1,16'h008B; LDO mode
    RDA5888_WriteReg(0x0a0, 0x0016);// set ldo vol=1.27
#else
    RDA5888_WriteReg(0x0a1, 0x008B);//9'h0a1,16'h008B; bit_8: dsp_pd_mode
#endif
#elif (RDA5888_CHIP_VER == 5)
    RDA5888_WriteReg(0x001, 0x0000);//9'0x01,16'h0000;
    RDA5888_WriteReg(0x001, 0x0115);//9'h01,16'h0115;soft reset
    RDAEXT_DelayMs((uint16)10);

    RDA5888_WriteReg(0x001, 0x091B);//9'h001,16'h091B; agc_en_b
    RDA5888_WriteReg(0x011, 0x001f);//9'h011,16'h001F; rda5888S2
    RDA5888_WriteReg(0x053, 0x0e50);//9'h053,16'h0e50; lna current
    RDA5888_WriteReg(0x05a, 0x0100);//9'h05a,16'h0100; lna_th0
    RDA5888_WriteReg(0x05b, 0x0280);//9'h05b,16'h0280; lna_th1
    RDA5888_WriteReg(0x062, 0x053f);//9'h62,16'h053f; vco high low th 705M
    RDA5888_WriteReg(0x066, 0x0470);//9'h66,16'h0470; cfc th0
    RDA5888_WriteReg(0x067, 0x0538);//9'h67,16'h0538; cfc th1
    RDA5888_WriteReg(0x068, 0x0648);//9'h68,16'h0648; cfc th2
    RDA5888_WriteReg(0x06a, 0xcccc);//9'h6a,16'hcccc; rfpll_gain0~3
    RDA5888_WriteReg(0x072, 0xaaaa);//9'h072,16'hAAAA; vco ibit 1010
    RDA5888_WriteReg(0x084, 0x000f);//9'h084,16'h000F; Analog Filter
    RDA5888_WriteReg(0x08e, 0x3400);//9'h8e,16'h3400; int_dec_sel 00
    RDA5888_WriteReg(0x086, 0x1045);//9'h86,16'h1045; mdll_test 1
    RDA5888_WriteReg(0x087, 0x0c12);//9'h087,16'h0c12;
    RDA5888_WriteReg(0x080, 0x004d);//9'h80,16'h004d; adc_sel_clk 1
    RDA5888_WriteReg(0x088, 0x6a24);//9'h088,16'h6A24; xtal cap
    RDA5888_WriteReg(0x089, 0x050e);//9'h089,16'h050e; edit 20100221
    RDA5888_WriteReg(0x098, 0x0400);//9'h098,16'h0400; os_en, dc_i
    RDA5888_WriteReg(0x099, 0x0000);//9'h099,16'h0000; dc_q
    RDA5888_WriteReg(0x0b0, 0x0140);//9'h0b0,16'h0140; pllbb

    RDA5888_WriteReg(0x042, 0x0924);//9'h42,16'h0FF4; analog gain
    RDA5888_WriteReg(0x043, 0x0FFC);//9'0x43,16'0x0FFF;
    RDA5888_WriteReg(0x044, 0x0000);//9'0x44,16'0x1000;
    RDA5888_WriteReg(0x045, 0xF700);//9'0x45,16'0xFF73;

    RDA5888_WriteReg(0x046, 0x0000);//9'0x46,16'0x0000;
    RDA5888_WriteReg(0x047, 0x0000);//9'0x47,16'0x0000;
    RDA5888_WriteReg(0x048, 0x0000);//9'0x48,16'0x0000;
    RDA5888_WriteReg(0x049, 0x0000);//9'0x49,16'0x0000;
    RDA5888_WriteReg(0x04A, 0x0000);//9'0x4A,16'0x0000;
    RDA5888_WriteReg(0x04B, 0x0000);//9'0x4B,16'0x0000;
    RDA5888_WriteReg(0x04C, 0x28FF);//9'H04C,16'H28F3; // inc adac gain 0x28F6 0x28FF
    RDA5888_WriteReg(0x04D, 0x00F0);//9'0x4D,16'0x00F0;
    RDA5888_WriteReg(0x04E, 0x0000);//9'0x4E,16'0x0000;
    RDA5888_WriteReg(0x04F, 0xC000);//9'0x4F,16'0xC000;
    RDA5888_WriteReg(0x050, 0x0000);//9'0x50,16'0x0000;
    RDA5888_WriteReg(0x051, 0x0000);//9'0x51,16'0x0000;
    RDA5888_WriteReg(0x052, 0x0000);//9'0x52,16'0x0000;

    //;9'h0a1,16'h0089; dcdc enable
    RDA5888_WriteReg(0x0a1, 0x009B);//9'h0a1,16'h009B; dcdc enable edited 2010 0425
#elif (RDA5888_CHIP_VER == 6)
#ifdef RDA5888_LDOMODE_ENABLE
    // item0:LDO_5888E
    RDA5888_WriteReg(0x002, 0x0002);//9'h002,16'h0002; soft reset
    RDA5888_WriteReg(0x002, 0x0001);//9'h002,16'h0001;
    RDA5888_WriteReg(0x003, 0x0920);//9'h003,16'h0920;
    RDA5888_WriteReg(0x004, 0xbe40);//9'h004,16'hbe40;
    RDA5888_WriteReg(0x005, 0x01aa);//9'h005,16'h01aa;
    RDA5888_WriteReg(0x002, 0x000d);//9'h002,16'h000d;
    //wait 5ms;
    RDAEXT_DelayMs((uint16)5);
    RDA5888_WriteReg(0x030, 0x0001);//9'h030,16'h0001; soft reset interface
    RDA5888_WriteReg(0x100, 0x0000);//9'h100,16'h0000; soft reset dsp
    RDA5888_WriteReg(0x100, 0x0001);//9'h100,16'h0001;
    RDA5888_WriteReg(0x008, 0x033f);//9'h008,16'h033f;
    RDA5888_WriteReg(0x009, 0x047f);//9'h009,16'h047f;
    RDA5888_WriteReg(0x00a, 0x24ff);//9'h00a,16'h24ff;
    RDA5888_WriteReg(0x00d, 0x01e1);//9'h00d,16'h01e1;
    RDA5888_WriteReg(0x010, 0x0044);//9'h010,16'h0044;
    RDA5888_WriteReg(0x011, 0x0c85);//9'h011,16'h0c85;
    RDA5888_WriteReg(0x013, 0x8248);//9'h013,16'h8248;
    RDA5888_WriteReg(0x015, 0x2097);//9'h015,16'h2097;
    RDA5888_WriteReg(0x01a, 0x1ce7);//9'h01a,16'h1ce7;
    RDA5888_WriteReg(0x01b, 0x1ce7);//9'h01b,16'h1ce7;
    RDA5888_WriteReg(0x01d, 0x4f10);//9'h01d,16'h4f10;
    RDA5888_WriteReg(0x01e, 0xa234);//9'h01e,16'ha234;
    RDA5888_WriteReg(0x021, 0x8600);//9'h021,16'h8600; for 27MHz DCXO by 100902
    RDA5888_WriteReg(0x022, 0xffff);//9'h022,16'hffff;

    RDA5888_WriteReg(0x023, 0x3fff);//9'h023,16'h3fff;
    RDA5888_WriteReg(0x024, 0x00f0);//9'h024,16'h00f0;
    RDA5888_WriteReg(0x025, 0x0130);//9'h025,16'h0130;
    RDA5888_WriteReg(0x026, 0x0f30);//9'h026,16'h0f30;
    RDA5888_WriteReg(0x027, 0x0f00);//9'h027,16'h0f00;
    RDA5888_WriteReg(0x028, 0x3f00);//9'h028,16'h3f00;
    RDA5888_WriteReg(0x029, 0x3f00);//9'h029,16'h3f00;
    RDA5888_WriteReg(0x02a, 0x3f00);//9'h02a,16'h3f00;
    RDA5888_WriteReg(0x02b, 0x3f00);//9'h02b,16'h3f00;
    RDA5888_WriteReg(0x02c, 0x3333);//9'h02c,16'h3333;
    RDA5888_WriteReg(0x02d, 0x3333);//9'h02d,16'h3333;

    RDA5888_WriteReg(0x02e, 0xab04);//9'h02e,16'hab04;
    RDA5888_WriteReg(0x036, 0x6a85);//9'h036,16'h6a85;
    RDA5888_WriteReg(0x038, 0x4151);//9'h038,16'h4151;
    RDA5888_WriteReg(0x03b, 0x07ef);//9'h03b,16'h07ef; 130MHz  LNA100 ->  LNA200
    RDA5888_WriteReg(0x03c, 0x1424);//9'h03c,16'h1424;  330MHz  LNA200 ->  LNA800
    RDA5888_WriteReg(0x045, 0x00e2);//9'h045,16'h00e2;
    RDA5888_WriteReg(0x030, 0x0005);//9'h030,16'h0005;//calibration
    //wait 5ms;
    RDAEXT_DelayMs((uint16)5);
    RDA5888_WriteReg(0x030, 0x0007);//9'h030,16'h0007;
#else // rda5888e dcdc mode.
    //item1:DCDC_5888E
    RDA5888_WriteReg(0x002, 0x0002);//9'h002,16'h0002
    RDA5888_WriteReg(0x002, 0x0001);//9'h002,16'h0001
    RDA5888_WriteReg(0x005, 0x01af);//9'h005,16'h01aF
    RDA5888_WriteReg(0x003, 0x0e56);//9'h003,16'h0E56
    RDA5888_WriteReg(0x004, 0xde40);//9'h004,16'hde40
    RDA5888_WriteReg(0x002, 0x000d);//9'h002,16'h000d
    //wait 5ms
    RDAEXT_DelayMs((uint16)5);
    RDA5888_WriteReg(0x030, 0x0001);//9'h030,16'h0001
    RDA5888_WriteReg(0x100, 0x0000);//9'h100,16'h0000
    RDA5888_WriteReg(0x100, 0x0001);//9'h100,16'h0001
    RDA5888_WriteReg(0x10e, 0xff80);//9'h10e,16'hff80
    RDA5888_WriteReg(0x008, 0x033f);//9'h008,16'h033f
    RDA5888_WriteReg(0x009, 0x047f);//9'h009,16'h047f
    RDA5888_WriteReg(0x00a, 0x24ff);//9'h00a,16'h24ff
    RDA5888_WriteReg(0x00d, 0x01e1);//9'h00d,16'h01e1
    RDA5888_WriteReg(0x010, 0x0044);//9'h010,16'h0044
    RDA5888_WriteReg(0x011, 0x0c85);//9'h011,16'h0c85
    RDA5888_WriteReg(0x013, 0x8248);//9'h013,16'h8248
    RDA5888_WriteReg(0x015, 0x2097);//9'h015,16'h2097
    RDA5888_WriteReg(0x01a, 0x1ce7);//9'h01a,16'h1ce7
    RDA5888_WriteReg(0x01b, 0x1ce7);//9'h01b,16'h1ce7
    RDA5888_WriteReg(0x01d, 0x4f10);//9'h01d,16'h4f10
    RDA5888_WriteReg(0x01e, 0xa234);//9'h01e,16'ha234
    RDA5888_WriteReg(0x021, 0x8600);//9'h021,16'h8600
    RDA5888_WriteReg(0x022, 0xffff);//9'h022,16'hffff
    RDA5888_WriteReg(0x023, 0x3fff);//9'h023,16'h3fff
    RDA5888_WriteReg(0x024, 0x00f0);//9'h024,16'h00f0
    RDA5888_WriteReg(0x025, 0x0130);//9'h025,16'h0130;
    RDA5888_WriteReg(0x026, 0x0f30);//9'h026,16'h0f30
    RDA5888_WriteReg(0x027, 0x0f00);//9'h027,16'h0f00
    RDA5888_WriteReg(0x028, 0x3f00);//9'h028,16'h3f00
    RDA5888_WriteReg(0x029, 0x3f00);//9'h029,16'h3f00
    RDA5888_WriteReg(0x02a, 0x3f00);//9'h02a,16'h3f00
    RDA5888_WriteReg(0x02b, 0x3f00);//9'h02b,16'h3f00
    RDA5888_WriteReg(0x02c, 0x3333);//9'h02c,16'h3333
    RDA5888_WriteReg(0x02d, 0x3333);//9'h02d,16'h3333
    RDA5888_WriteReg(0x02e, 0xab04);//9'h02e,16'hab04
    RDA5888_WriteReg(0x036, 0x6a85);//9'h036,16'h6a85
    RDA5888_WriteReg(0x038, 0x4151);//9'h038,16'h4151
    RDA5888_WriteReg(0x03b, 0x07ef);//9'h03b,16'h07ef
    RDA5888_WriteReg(0x03c, 0x1424);//9'h03c,16'h1424
    RDA5888_WriteReg(0x045, 0x00e2);//9'h045,16'h00e2
    RDA5888_WriteReg(0x030, 0x0005);//9'h030,16'h0005
    //wait 5ms
    RDAEXT_DelayMs((uint16)5);
    RDA5888_WriteReg(0x030, 0x0007);//9'h030,16'h0007
#endif // end RDA5888_LDOMODE_ENABLE 
    RDAEXT_DelayMs((uint16)200);
#elif (RDA5888_CHIP_VER == 7) // RDA5888H / RDA5888HE
#ifdef RDA5888_LDOMODE_ENABLE
    RDA5888_WriteReg(0x002, 0x0002);//9'h002,16'h0002; soft reset
    RDA5888_WriteReg(0x002, 0x0001);//9'h002,16'h0001;
    RDA5888_WriteReg(0x003, 0x0920);//9'h003,16'h0920;
    RDA5888_WriteReg(0x004, 0xbf40);//9'h004,16'hbf40;
    RDA5888_WriteReg(0x005, 0x01aa);//  9'h005,16'h01aa; by 110115 for ECO1
    RDA5888_WriteReg(0x002, 0x000d);//9'h002,16'h000d;
    //wait 5ms;
    RDAEXT_DelayMs((uint16)5);
    RDA5888_WriteReg(0x030, 0x0001);//9'h030,16'h0001; soft reset interface
    RDA5888_WriteReg(0x100, 0x0000);//9'h100,16'h0000; soft reset dsp
    RDA5888_WriteReg(0x100, 0x0001);//9'h100,16'h0001;
    RDA5888_WriteReg(0x008, 0x033f);//9'h008,16'h033f;
    RDA5888_WriteReg(0x009, 0x447f);//9'h009,16'h447f; by 101215
    RDA5888_WriteReg(0x00a, 0x2924);//  9'h00a,16'h2924; by 110115 for ECO1
    RDA5888_WriteReg(0x00d, 0x01e1);//9'h00d,16'h01e1;
    RDA5888_WriteReg(0x010, 0x0044);//9'h010,16'h0044;
    RDA5888_WriteReg(0x011, 0x0c85);//9'h011,16'h0c85;
    RDA5888_WriteReg(0x013, 0x8248);//9'h013,16'h8248;
    RDA5888_WriteReg(0x014, 0x5018);//9'h014,16'h5018; by 101218
    RDA5888_WriteReg(0x015, 0x102b);//9'h015,16'h102b; edit  20101207
    RDA5888_WriteReg(0x017, 0x1112);//9'h017,16'h1112; by 101216
    RDA5888_WriteReg(0x01a, 0x1ce7);//9'h01a,16'h1ce7;
    RDA5888_WriteReg(0x01b, 0x1ce7);//9'h01b,16'h1ce7;
    RDA5888_WriteReg(0x01d, 0x4e10);//9'h01d,16'h4e10;
    RDA5888_WriteReg(0x01e, 0xe33c);//9'h01e,16'he33c; dac I/Q all on
    RDA5888_WriteReg(0x01f, 0x2042);//9'h01f,16'h2042;
    RDA5888_WriteReg(0x020, 0x6420);//9'h020,16'h6420;
    RDA5888_WriteReg(0x021, 0x96a0);//9'h021,16'h96a0; for 27MHz DCXO by 101207
    RDA5888_WriteReg(0x022, 0xffff);//9'h022,16'hffff;
    RDA5888_WriteReg(0x023, 0x3fff);//9'h023,16'h3fff;

    RDA5888_WriteReg(0x024, 0x00f1);//  9'h024,16'h00f1; by zm 101222
    RDA5888_WriteReg(0x025, 0x0070);//9'h025,16'h0070;
    RDA5888_WriteReg(0x026, 0x0010);//9'h026,16'h0010;
    RDA5888_WriteReg(0x027, 0x2010);//9'h027,16'h2010;
    RDA5888_WriteReg(0x028, 0x3c00);//9'h028,16'h3c00;
    RDA5888_WriteReg(0x029, 0x3c00);//9'h029,16'h3c00;
    RDA5888_WriteReg(0x02a, 0x3c00);//9'h02a,16'h3c00;
    RDA5888_WriteReg(0x02b, 0x3c00);//9'h02b,16'h3c00;
    RDA5888_WriteReg(0x02c, 0x7777);//9'h02c,16'h7777;
    RDA5888_WriteReg(0x02d, 0x3333);//  9'h02d,16'h3333; by 110115 for ECO1

    RDA5888_WriteReg(0x02e, 0x090d);//  9'h02e,16'h090d;  by 110115 for ECO1
    RDA5888_WriteReg(0x036, 0x6a85);//9'h036,16'h6a85;
    RDA5888_WriteReg(0x037, 0x8720);//9'h037,16'h8720; by 101218
    RDA5888_WriteReg(0x038, 0x5155);//9'h038,16'h5155;
    RDA5888_WriteReg(0x03b, 0x07ef);//9'h03b,16'h07ef; 130MHz  LNA100 ->  LNA200
    RDA5888_WriteReg(0x03c, 0x1424);//9'h03c,16'h1424;  330MHz  LNA200 ->  LNA800
    RDA5888_WriteReg(0x045, 0x00e2);//9'h045,16'h00e2;

    RDA5888_WriteReg(0x085, 0x0a88);//9'h085,16'h0a88; by 101215
    //lib3.09 0x19a3->0x09a3
    RDA5888_WriteReg(0x099, 0x09A3);//9'h099,16'h19a3;
    RDA5888_WriteReg(0x09b, 0xd488);//9'h09b,16'hd488;
    RDA5888_WriteReg(0x09e, 0x460c);//9'h09e,16'h460c;
    RDA5888_WriteReg(0x09f, 0x0086);//  9'h09f,16'h0086; FM VOL
    //lib3.09 0x06d9->0x001d
    RDA5888_WriteReg(0x0a0, 0x001D);//9'h0a0,16'h06d9;
    RDA5888_WriteReg(0x0a3, 0x2346);//9'h0a3,16'h2346;
    RDA5888_WriteReg(0x0a4, 0x0406);//9'h0a4,16'h0406;
    RDA5888_WriteReg(0x0a5, 0x060b);//9'h0a5,16'h060b;
    RDA5888_WriteReg(0x0a9, 0x080f);//9'h0a9,16'h080f;
    RDA5888_WriteReg(0x0aa, 0x0010);//9'h0aa,16'h0010;
    RDA5888_WriteReg(0x0b3, 0x2822);//9'h0b3,16'h2822;
    RDA5888_WriteReg(0x0b4, 0x1280);//9'h0b4,16'h1280;

    RDA5888_WriteReg(0x082, 0x86c1);//9'h082,16'h86c1; FM On

    RDA5888_WriteReg(0x030, 0x0005);//9'h030,16'h0005;//calibration
    //wait 5ms;
    RDAEXT_DelayMs((uint16)5);

    RDA5888_WriteReg(0x02e, 0xe90d);//9'h02e,16'he90d;  by 110115 for ECO1
    RDA5888_WriteReg(0x030, 0x0007);//9'h030,16'h0007;
#else // RDA5888H DCDC MODE.
    // item0:dcdc_5888H
    RDA5888_WriteReg(0x002, 0x0002);//9'h002,16'h0002; soft reset
    RDA5888_WriteReg(0x002, 0x0001);//9'h002,16'h0001;
    RDA5888_WriteReg(0x003, 0x0b18);//9'h003,16'h0B18;
    RDA5888_WriteReg(0x004, 0xff40);//9'h004,16'hff40;
    RDA5888_WriteReg(0x005, 0x81aa);//9'h005,16'h81aa; by 101217
    RDA5888_WriteReg(0x002, 0x000d);//9'h002,16'h000d;
    //wait 5ms;
    RDAEXT_DelayMs((uint16)5);
    RDA5888_WriteReg(0x030, 0x0001);//9'h030,16'h0001; soft reset interface
    RDA5888_WriteReg(0x100, 0x0000);//9'h100,16'h0000; soft reset dsp
    RDA5888_WriteReg(0x100, 0x0001);//9'h100,16'h0001;
    RDA5888_WriteReg(0x008, 0x033f);//9'h008,16'h033f;
    RDA5888_WriteReg(0x009, 0x447f);//9'h009,16'h447f; by 101215
    RDA5888_WriteReg(0x00a, 0x7000);//9'h00a,16'h7000;by 101215
    RDA5888_WriteReg(0x00d, 0x01e1);//9'h00d,16'h01e1;
    RDA5888_WriteReg(0x010, 0x0044);//9'h010,16'h0044;
    RDA5888_WriteReg(0x011, 0x0c85);//9'h011,16'h0c85;
    RDA5888_WriteReg(0x013, 0x8248);//9'h013,16'h8248;
    RDA5888_WriteReg(0x014, 0x5018);//9'h014,16'h5018; by 101218
    RDA5888_WriteReg(0x015, 0x102b);//9'h015,16'h102b; edit  20101207
    RDA5888_WriteReg(0x017, 0x1112);//9'h017,16'h1112; by 101216
    RDA5888_WriteReg(0x01a, 0x1ce7);//9'h01a,16'h1ce7;
    RDA5888_WriteReg(0x01b, 0x1ce7);//9'h01b,16'h1ce7;
    RDA5888_WriteReg(0x01d, 0x4e10);//9'h01d,16'h4e10;
    RDA5888_WriteReg(0x01e, 0xe33c);//9'h01e,16'he33c; dac I/Q all on
    RDA5888_WriteReg(0x01f, 0x2042);//9'h01f,16'h2042;
    RDA5888_WriteReg(0x020, 0x6420);//9'h020,16'h6420;
    RDA5888_WriteReg(0x021, 0x96a0);//9'h021,16'h96a0; for 27MHz DCXO by 101207
    RDA5888_WriteReg(0x022, 0xffff);//9'h022,16'hffff;
    RDA5888_WriteReg(0x023, 0x3fff);//9'h023,16'h3fff;

    RDA5888_WriteReg(0x024, 0x40ff);//9'h024,16'h40ff;
    RDA5888_WriteReg(0x025, 0x0070);//9'h025,16'h0070;
    RDA5888_WriteReg(0x026, 0x0010);//9'h026,16'h0010;
    RDA5888_WriteReg(0x027, 0x2010);//9'h027,16'h2010;
    RDA5888_WriteReg(0x028, 0x3c00);//9'h028,16'h3c00;
    RDA5888_WriteReg(0x029, 0x3c00);//9'h029,16'h3c00;
    RDA5888_WriteReg(0x02a, 0x3c00);//9'h02a,16'h3c00;
    RDA5888_WriteReg(0x02b, 0x3c00);//9'h02b,16'h3c00;
    RDA5888_WriteReg(0x02c, 0x7777);//9'h02c,16'h7777;
    RDA5888_WriteReg(0x02d, 0x7777);//9'h02d,16'h7777;

    RDA5888_WriteReg(0x02e, 0xa90d);//9'h02e,16'ha90d;
    RDA5888_WriteReg(0x036, 0x6a85);//9'h036,16'h6a85;
    RDA5888_WriteReg(0x037, 0x8720);//9'h037,16'h8720; by 101218
    RDA5888_WriteReg(0x038, 0x5155);//9'h038,16'h5155;
    RDA5888_WriteReg(0x03b, 0x07ef);//9'h03b,16'h07ef; 130MHz  LNA100 ->    LNA200
    RDA5888_WriteReg(0x03c, 0x1424);//9'h03c,16'h1424;  330MHz  LNA200 ->  LNA800
    RDA5888_WriteReg(0x045, 0x00e2);//9'h045,16'h00e2;

    RDA5888_WriteReg(0x085, 0x0a88);//9'h085,16'h0a88; by 101215
    RDA5888_WriteReg(0x099, 019a3);//9'h099,16'h19a3;
    RDA5888_WriteReg(0x09b, 0xd488);//9'h09b,16'hd488;
    RDA5888_WriteReg(0x09e, 0x460c);//9'h09e,16'h460c;
    RDA5888_WriteReg(0x09f, 0x0090);//9'h09f,16'h0090; FM VOL
    RDA5888_WriteReg(0x0a0, 0x06d9);//9'h0a0,16'h06d9;
    RDA5888_WriteReg(0x0a3, 0x2346);//9'h0a3,16'h2346;
    RDA5888_WriteReg(0x0a4, 0x0406);//9'h0a4,16'h0406;
    RDA5888_WriteReg(0x0a5, 0x060b);//9'h0a5,16'h060b;
    RDA5888_WriteReg(0x0a9, 0x080f);//9'h0a9,16'h080f;
    RDA5888_WriteReg(0x0aa, 0x0010);//9'h0aa,16'h0010;
    RDA5888_WriteReg(0x0b3, 0x2822);//9'h0b3,16'h2822;
    RDA5888_WriteReg(0x0b4, 0x1280);//9'h0b4,16'h1280;

    RDA5888_WriteReg(0x082, 0x86c1);//9'h082,16'h86c1; FM On

    RDA5888_WriteReg(0x030, 0x0005);//9'h030,16'h0005;//calibration
    //wait 5ms;
    RDAEXT_DelayMs((uint16)5);
    RDA5888_WriteReg(0x030, 0x0007);//9'h030,16'h0007;
#endif // end RDA5888_LDOMODE_ENABLE 
    RDAEXT_DelayMs((uint16)200);
#endif // end RDA5888_CHIP_VER

    RDAEXT_DebugPrint(RDA5888_TRACE_MOD  "[rdamtv]RDA5888_DCDCInit End\n");

    return rt;
}

// ----------------------------------------------------------------------------
// This function initializes the rda5888 dsp module.
//
// Params:
//  none.
//
// Return:
//  none.
// ----------------------------------------------------------------------------
void RDA5888_DSPInit(void)
{
    RDAEXT_DebugPrint(RDA5888_TRACE_MOD  "[rdamtv]RDA5888_DSPInit Begin\n");

#if (RDA5888_CHIP_VER == 4)
    RDA5888_WriteReg(0x100,0x0000); //reset dig registers
    RDA5888_WriteReg(0x100,0x0001);
    RDA5888_WriteReg(0x130,0x0010);//9'h130,16'h0000; reset dsp

    RDA5888_WriteReg(0x104,0x31c7);//9'h104,16'h31c7; Pal D/k angle_in_if

    RDA5888_WriteReg(0x107,0x00ff); //sk_rssi_rate, sk_tmr_afc
    RDA5888_WriteReg(0x108,0x00a0); //sk_rssi_th

    RDA5888_WriteReg(0x10A,0x089f); //9'h10A,16'h089f;
    RDA5888_WriteReg(0x10B,0x4229); //9'h10b,16'h4229;
    RDA5888_WriteReg(0x10c,0xf047); //9'h10c,16'hf047;

    RDA5888_WriteReg(0x111,0x5413); //9'h111,16'h5413;
    RDA5888_WriteReg(0x112,0x3000); //9'h112,16'h3000; ECO2
    RDA5888_WriteReg(0x113,0x9158); //9'h113,16'hfd58;  0xfd58
    RDA5888_WriteReg(0x114,0xefe0); //9'h114,16'hefe0;
    RDA5888_WriteReg(0x115,0x048a); //9'h115,16'h048a;
    RDA5888_WriteReg(0x116,0x1201); //9'h116,16'h1201;
    RDA5888_WriteReg(0x118,0x3fbf); //9'h118,16'hBFBF; auto black

    RDA5888_WriteReg(0x11d,0x60ff); //ha_index_var_thd & ha_value_var_thd
    RDA5888_WriteReg(0x11f,0x082a); //9'h11f,16'h082a;
    RDA5888_WriteReg(0x120,0x4832); //9'h120,16'h4832;
    RDA5888_WriteReg(0x125,0x0870); //9'h125,16'h1070; gainct_fm 0x1070 0x0470

    RDA5888_WriteReg(0x129,0x08cb); //hsync_delay_sel & clk_inv
    //RDA5888_WriteReg(0x129,0x08ab); //hsync_delay_sel & clk_inv  for SPRD_CUSTOMER

    RDA5888_WriteReg(0x133,0x0186);// 9'h133,16'h0186;
    RDA5888_WriteReg(0x135,0x03cc); //h_blank_end, cell without shell

    RDA5888_WriteReg(0x140,0x402D); //line625_flag
    RDA5888_WriteReg(0x141,0x0001);
    RDA5888_WriteReg(0x142,0x078f); //notch coefficients
    RDA5888_WriteReg(0x143,0x1f4b);// notch coefficients
    RDA5888_WriteReg(0x144,0x071e); //notch coefficients
    RDA5888_WriteReg(0x145,0x0000);
    RDA5888_WriteReg(0x146,0x0000);
    RDA5888_WriteReg(0x147,0x0000);
    RDA5888_WriteReg(0x148,0x0000);
    RDA5888_WriteReg(0x149,0x0000);
    RDA5888_WriteReg(0x14a,0x0000);
    RDA5888_WriteReg(0x14b,0x0000);
    RDA5888_WriteReg(0x14c,0x0000);
    RDA5888_WriteReg(0x14d,0x0000);
    RDA5888_WriteReg(0x14e,0x0000);
    RDA5888_WriteReg(0x14f,0x0000);
    RDA5888_WriteReg(0x150,0x0000);

#ifdef RDA5888_IODRIVEREN_ENABLE
    RDA5888_WriteReg(0x151,0x0022);
    RDA5888_WriteReg(0x152,0x0880);
    RDA5888_WriteReg(0x153,0x0002);
    RDA5888_WriteReg(0x154,0x0000);
    RDA5888_WriteReg(0x155,0x0002);
#else
    RDA5888_WriteReg(0x151,0x0000);
    RDA5888_WriteReg(0x152,0x0000);
    RDA5888_WriteReg(0x153,0x0000);
    RDA5888_WriteReg(0x154,0x0000);
    RDA5888_WriteReg(0x155,0x0000);
#endif

    RDA5888_WriteReg(0x156,0x0000);
    RDA5888_WriteReg(0x157,0x0000);
    RDA5888_WriteReg(0x158,0x0000);
    RDA5888_WriteReg(0x159,0x0000);
    RDA5888_WriteReg(0x15a,0x0000);
    RDA5888_WriteReg(0x15b,0x0000);
    RDA5888_WriteReg(0x15c,0x0000);
    RDA5888_WriteReg(0x15d,0x0000);
    RDA5888_WriteReg(0x15e,0x0000);
    RDA5888_WriteReg(0x15f,0x0000);
    RDA5888_WriteReg(0x160,0x0000);
    RDA5888_WriteReg(0x161,0x0000);
    RDA5888_WriteReg(0x162,0x0000);
    RDA5888_WriteReg(0x163,0x0000);
    RDA5888_WriteReg(0x164,0x0000);
    RDA5888_WriteReg(0x165,0x0000);
    RDA5888_WriteReg(0x166,0x0000);
    RDA5888_WriteReg(0x167,0x0000);
    RDA5888_WriteReg(0x168,0x0000);
    RDA5888_WriteReg(0x169,0x0000);
    RDA5888_WriteReg(0x16a,0x0000);
    RDA5888_WriteReg(0x16b,0x0000);
    RDA5888_WriteReg(0x16c,0x0000);
    RDA5888_WriteReg(0x16d,0x0000);
    RDA5888_WriteReg(0x16e,0x0000);
    RDA5888_WriteReg(0x16f,0x0000);
    RDA5888_WriteReg(0x170,0x00bf);
    RDA5888_WriteReg(0x171,0x0006);
    RDA5888_WriteReg(0x172,0x4c1d);
    RDA5888_WriteReg(0x173,0x4c1d);
    RDA5888_WriteReg(0x174,0x008c);
    RDA5888_WriteReg(0x175,0x1020);
    RDA5888_WriteReg(0x176,0x0030);
    RDA5888_WriteReg(0x177,0x01cf); // 9'h177,16'h01cf;
    RDA5888_WriteReg(0x178,0x236d);
    RDA5888_WriteReg(0x179,0x0080);
    RDA5888_WriteReg(0x17a,0x1440);
    RDA5888_WriteReg(0x17b,0x0000);
    RDA5888_WriteReg(0x17c,0x0000);
    RDA5888_WriteReg(0x17d,0x0000);
    RDA5888_WriteReg(0x17e,0x0000);
    RDA5888_WriteReg(0x17f,0x0000);
    RDA5888_WriteReg(0x180,0x0004);
    RDA5888_WriteReg(0x181,0x2040);
    RDA5888_WriteReg(0x182,0x0138);
    RDA5888_WriteReg(0x183,0x9005); //afc_disable_fm 0x9037 0x9005
    RDA5888_WriteReg(0x184,0x0924); // 0x0924 0x0FFF
    RDA5888_WriteReg(0x185,0x0000);
    RDA5888_WriteReg(0x186,0x0000);
    RDA5888_WriteReg(0x187,0x0000);
    RDA5888_WriteReg(0x188,0x0000);
    RDA5888_WriteReg(0x189,0x0000);
    RDA5888_WriteReg(0x18a,0x0000);
    RDA5888_WriteReg(0x18b,0x0000);
    RDA5888_WriteReg(0x18c,0x0000);
    RDA5888_WriteReg(0x18d,0x0000);
    RDA5888_WriteReg(0x18e,0x0000);
    RDA5888_WriteReg(0x18f,0x051f);// tmr_agc1
    RDA5888_WriteReg(0x190,0x0008);

    RDA5888_WriteReg(0x191,0x19be);//16'b00_0110111_0111011); adv_th7=55, adv_th6=59
    RDA5888_WriteReg(0x192,0x275d);//16'b00_1000001_1000111); adv_th5=65, adv_th4=71
    RDA5888_WriteReg(0x193,0x3fff);//16'b00_1001100_1010000); adv_th3=76, adv_th2=80
    RDA5888_WriteReg(0x194,0x817f);//16'b100000010_1010110); adv_th1=86
    RDA5888_WriteReg(0x195,0x050a);//16'b00_0001101_0010011); ana_gain_index0=13,ana_gain_index1=19
    RDA5888_WriteReg(0x196,0x850a);//16'b10_0011001_0011110); ana_gain_index2=25, ana_gain_index3=30
    RDA5888_WriteReg(0x197,0x051c);//16'b00_0100011_0101000); ana_gain_index4=35, ana_gain_index5=40
    RDA5888_WriteReg(0x198,0x1637);//16'b00_0101111_0110100); ana_gain_index6=47, ana_gain_index7=52
    RDA5888_WriteReg(0x199,0x6000);//9'h199,16'b0_1010000_0_0000000; tagt_pwr=112 0x5000

    //;9'h19a,16'h0014; dgc see rssi before lpf
    //9'h19a,16'h0015; dgc see rssi after lpf
    RDA5888_WriteReg(0x19a,0x0014); // 0x0015
    RDA5888_WriteReg(0x19B,0x0D09);// bypass rate_conv & enable notch & sk_mode
    RDA5888_WriteReg(0x19C,0x00ff);// tmr_rssi1
    RDA5888_WriteReg(0x19d,0x14FF);// tmr_rssi3
    RDA5888_WriteReg(0x19e,0x0000);
    RDA5888_WriteReg(0x19f,0x0000);

    RDA5888_WriteReg(0x1a0,0x1f39); // notch filter for audio
    RDA5888_WriteReg(0x1a1,0x071e);
    RDA5888_WriteReg(0x1a2,0x0070);

    RDA5888_WriteReg(0x1a3,0x0002);
    RDA5888_WriteReg(0x1a4,0x0000);
    RDA5888_WriteReg(0x1a5,0x0080);
    RDA5888_WriteReg(0x1a6,0x0000);
    RDA5888_WriteReg(0x1a7,0x0000);
    RDA5888_WriteReg(0x1a8,0x0801); //9'h1a8,16'h0801;ECO2
    RDA5888_WriteReg(0x1a9,0x0000);
    RDA5888_WriteReg(0x1aa,0x0000);
    RDA5888_WriteReg(0x1ab,0x0007); // adc_clk_edge, dcdc_div_ct
    RDA5888_WriteReg(0x1ac,0x0000);
    RDA5888_WriteReg(0x1ad,0x3000); //seek_en
    RDA5888_WriteReg(0x1ae,0x7000); //9'h1ae,16'h7000;ECO2
    RDA5888_WriteReg(0x1af,0x0000);

    RDA5888_WriteReg(0x12f,0x007e); //9'h12f,16'h0076;
    RDA5888_WriteReg(0x130,0x0814); //9'h130,16'h0814;
#elif (RDA5888_CHIP_VER == 5)
    RDA5888_WriteReg(0x100,0x0000); //reset dig registers
    RDA5888_WriteReg(0x100,0x0001);
    RDA5888_WriteReg(0x130,0x0000); // reset dsp

    RDA5888_WriteReg(0x101,0x1da4); // 9'h101,16'h1DA4; add by tong 010514
    RDA5888_WriteReg(0x103,0x001c); // 9'h103,16'h001C; add by tong 010514
    RDA5888_WriteReg(0x105,0x3fff); // 9'h105,16'h3fff;
    RDA5888_WriteReg(0x106,0x003c); // 9'h106,16'h003c;
    RDA5888_WriteReg(0x107,0x00ff); // 9'h107,16'h00ff; sk_rssi_rate, sk_tmr_afc
    RDA5888_WriteReg(0x108,0x0028); // 9'h108,16'h0028; sk_rssi_th *
    RDA5888_WriteReg(0x10a,0x09df); // 9'h10a,16'h09df;
    RDA5888_WriteReg(0x10b,0x4228); // 9'h10b,16'h4228;   edited by xu 01192010

    RDA5888_WriteReg(0x10c,0x7bdf); // 9'h10c,16'h7bdf;
    RDA5888_WriteReg(0x10d,0x082d); // 9'h10d,16'h082d;
    RDA5888_WriteReg(0x10f,0x4444); // 9'h10f,16'h4444;
    RDA5888_WriteReg(0x111,0x5413); // 9'h111,16'h5413;
    RDA5888_WriteReg(0x112,0x1004); // 9'h112,16'h1000; 0x1004

    RDA5888_WriteReg(0x113,0x90c0); // 9'h113,16'h90c0;    edited by xu 02012010
    RDA5888_WriteReg(0x114,0xcfe0); // 9'h114,16'hcfe0;
    RDA5888_WriteReg(0x115,0x048a); // 9'h115,16'h048a;
    RDA5888_WriteReg(0x116,0x1000); // 9'h116,16'h1000;
    RDA5888_WriteReg(0x117,0x0000); // 9'h117,16'h0000;
    RDA5888_WriteReg(0x118,0x5fdf); // 9'h118,16'h5fdf; auto black

    RDA5888_WriteReg(0x119,0xA110); // 9'h119,16'hA110; *
    RDA5888_WriteReg(0x11b,0x0003); // 9'h11b,16'h0003; *
    RDA5888_WriteReg(0x11c,0xc200); // 9'h11c,16'hc200; *
    RDA5888_WriteReg(0x11d,0x6180); // 9'h11d,16'h6180; ha_index_var_thd & ha_value_var_thd
    RDA5888_WriteReg(0x11e,0x6192); // 9'h11e,16'h6192; *edit 20100322 tong
    RDA5888_WriteReg(0x11f,0x180e); // 9'h11f,16'h180e; *edit 20100310 tong

    RDA5888_WriteReg(0x120,0x403a); // 9'h120,16'h403a; *
    RDA5888_WriteReg(0x125,0x0870); // 9'h125,16'h0870; gainct_fm

    RDA5888_WriteReg(0x129,0x08cb); //hsync_delay_sel & clk_inv  for MTK_CUSTOMER
    //RDA5888_WriteReg(0x129,0x08ab); //hsync_delay_sel & clk_inv  for SPRD_CUSTOMER
    //RDA5888_WriteReg(0x129,0x088B); //hsync_delay_sel & clk_inv for SPRD_CUSTOMER_P4
    //RDA5888_WriteReg(0x133,0x0106);// 9'h133,16'h0186;

    RDA5888_WriteReg(0x12f,0x0c00); // 9'h12f,16'h0c00; by xu, iq swap = 1

    RDA5888_WriteReg(0x133,0x0586); // 9'h133,16'h0586; *
    RDA5888_WriteReg(0x135,0x03cc); // 9'h135,16'h03cc; h_blank_end, cell without shell
    RDA5888_WriteReg(0x137,0x035f); // 9'h137,16'h035f;
    RDA5888_WriteReg(0x13a,0x03aa); // 9'h13a,16'h03aa; h_chrom_win_start
    RDA5888_WriteReg(0x13b,0x03ff); // 9'h13b,16'h03ff; h_chrom_win_end
    RDA5888_WriteReg(0x140,0x422d); // 9'h140,16'h602d;
    RDA5888_WriteReg(0x141,0x0001); // 9'h141,16'h0001;
    RDA5888_WriteReg(0x142,0x002f); // 9'h142,16'h002f; notch coefficients  by 20100727
    RDA5888_WriteReg(0x143,0x129d); // 9'h143,16'h129d; notch coefficients  by 20100727
    RDA5888_WriteReg(0x144,0x05c1); // 9'h144,16'h05c1; notch coefficients  by 20100727
    RDA5888_WriteReg(0x145,0x0000);
    RDA5888_WriteReg(0x146,0x0000);
    RDA5888_WriteReg(0x147,0x0000);
    RDA5888_WriteReg(0x148,0x0000);
    RDA5888_WriteReg(0x149,0x0000);
    RDA5888_WriteReg(0x14a,0x0000);
    RDA5888_WriteReg(0x14b,0x0000);
    RDA5888_WriteReg(0x14c,0x0000);
    RDA5888_WriteReg(0x14d,0x0000);
    RDA5888_WriteReg(0x14e,0x0000);
    RDA5888_WriteReg(0x14f,0x0000);
    RDA5888_WriteReg(0x150,0x0000);

#ifdef RDA5888_IODRIVEREN_ENABLE
    RDA5888_WriteReg(0x151,0x0022);
    RDA5888_WriteReg(0x152,0x0880);
    RDA5888_WriteReg(0x153,0x0002);
    RDA5888_WriteReg(0x154,0x0000);
    RDA5888_WriteReg(0x155,0x0002);
#else
    RDA5888_WriteReg(0x151,0x0000);
    RDA5888_WriteReg(0x152,0x0000);
    RDA5888_WriteReg(0x153,0x0000);
    RDA5888_WriteReg(0x154,0x0000);
    RDA5888_WriteReg(0x155,0x0000);
#endif

    RDA5888_WriteReg(0x156,0x0000);
    RDA5888_WriteReg(0x157,0x0000);
    RDA5888_WriteReg(0x158,0x0000);
    RDA5888_WriteReg(0x159,0x0000);
    RDA5888_WriteReg(0x15a,0x0000);
    RDA5888_WriteReg(0x15b,0x0000);
    RDA5888_WriteReg(0x15c,0x0000);
    RDA5888_WriteReg(0x15d,0x0000);
    RDA5888_WriteReg(0x15e,0x0000);
    RDA5888_WriteReg(0x15f,0x0000);
    RDA5888_WriteReg(0x160,0x0000);
    RDA5888_WriteReg(0x161,0x0000);
    RDA5888_WriteReg(0x162,0x0000);
    RDA5888_WriteReg(0x163,0x0000);
    RDA5888_WriteReg(0x164,0x0000);
    RDA5888_WriteReg(0x165,0x0000);
    RDA5888_WriteReg(0x166,0x0000);
    RDA5888_WriteReg(0x167,0x0000);
    RDA5888_WriteReg(0x168,0x0000);
    RDA5888_WriteReg(0x169,0x0000);
    RDA5888_WriteReg(0x16a,0x0000);
    RDA5888_WriteReg(0x16b,0x0000);
    RDA5888_WriteReg(0x16c,0x0000);
    RDA5888_WriteReg(0x16d,0x0000);
    RDA5888_WriteReg(0x16e,0x0000);
    RDA5888_WriteReg(0x16f,0x0000);
    RDA5888_WriteReg(0x170,0x00bf);
    RDA5888_WriteReg(0x171,0x0006);
    RDA5888_WriteReg(0x172,0x4c1d);
    RDA5888_WriteReg(0x173,0x4c1d);
    RDA5888_WriteReg(0x174,0x008c);
    RDA5888_WriteReg(0x175,0x1020);
    RDA5888_WriteReg(0x176,0x0030);
    RDA5888_WriteReg(0x177,0x01cf); // 9'h177,16'h01cf;
    RDA5888_WriteReg(0x178,0x236d);
    RDA5888_WriteReg(0x179,0x0080);
    RDA5888_WriteReg(0x17a,0x1440);
    RDA5888_WriteReg(0x17b,0x0000);
    RDA5888_WriteReg(0x17c,0x0000);
    RDA5888_WriteReg(0x17d,0x0000);
    RDA5888_WriteReg(0x17e,0x0000);
    RDA5888_WriteReg(0x17f,0x0000);
    RDA5888_WriteReg(0x180,0x0004);
    RDA5888_WriteReg(0x181,0x2040);
    RDA5888_WriteReg(0x182,0x0138);
    RDA5888_WriteReg(0x183,0x8035); //9'h183,16'h8035; afc_disable_fm
    RDA5888_WriteReg(0x184,0x0924); // 0x0924 0x0FFF
    RDA5888_WriteReg(0x185,0x0000);
    RDA5888_WriteReg(0x186,0x0000);
    RDA5888_WriteReg(0x187,0x0000);
    RDA5888_WriteReg(0x188,0x0000);
    RDA5888_WriteReg(0x189,0x0000);
    RDA5888_WriteReg(0x18a,0x0000);
    RDA5888_WriteReg(0x18b,0x0000);
    RDA5888_WriteReg(0x18c,0x0000);
    RDA5888_WriteReg(0x18d,0x0000);
    RDA5888_WriteReg(0x18e,0x0000);
    RDA5888_WriteReg(0x18f,0x051f);// tmr_agc1
    RDA5888_WriteReg(0x190,0x0008);

    RDA5888_WriteReg(0x191,0x19be);//16'b00_0110111_0111011); adv_th7=55, adv_th6=59
    RDA5888_WriteReg(0x192,0x275d);//16'b00_1000001_1000111); adv_th5=65, adv_th4=71
    RDA5888_WriteReg(0x193,0x3fff);//16'b00_1001100_1010000); adv_th3=76, adv_th2=80
    RDA5888_WriteReg(0x194,0x817f);//16'b100000010_1010110); adv_th1=86
    RDA5888_WriteReg(0x195,0x050a);//16'b00_0001101_0010011); ana_gain_index0=13,ana_gain_index1=19
    RDA5888_WriteReg(0x196,0x850a);//16'b10_0011001_0011110); ana_gain_index2=25, ana_gain_index3=30
    RDA5888_WriteReg(0x197,0x051c);//16'b00_0100011_0101000); ana_gain_index4=35, ana_gain_index5=40
    RDA5888_WriteReg(0x198,0x1637);//16'b00_0101111_0110100); ana_gain_index6=47, ana_gain_index7=52
    RDA5888_WriteReg(0x199,0x7300);//9'h199,16'h6300; tagt_pwr * 0x6300 0x7300

    RDA5888_WriteReg(0x19a,0x0014); // 0x0015
    RDA5888_WriteReg(0x19B,0x0f08);// 9'h19B,16'h0f08; bypass rate_conv & enable notch & sk_mode=00
    RDA5888_WriteReg(0x19C,0x00ff);// tmr_rssi1
    RDA5888_WriteReg(0x19d,0x14fe);// tmr_rssi3
    RDA5888_WriteReg(0x19e,0x0000);
    RDA5888_WriteReg(0x19f,0x0000);

    RDA5888_WriteReg(0x1a0,0x1f39); // 9'h1a0,16'h1f39;Notch filter2 for audio
    RDA5888_WriteReg(0x1a1,0x071e); // 9'h1a1,16'h071e;Notch filter2 for audio
    RDA5888_WriteReg(0x1a2,0x0070); // 9'h1a2,16'h0070;Notch filter2 for audio

    RDA5888_WriteReg(0x1a3,0x0002); // 9'h1a3,16'h0002;
    RDA5888_WriteReg(0x1a4,0x0000); // 9'h1a4,16'h0000;
    RDA5888_WriteReg(0x1a5,0x0080); // 9'h1a5,16'h0080;
    RDA5888_WriteReg(0x1a6,0xa000); // 9'h1a6,16'ha000; *by 20100727
    RDA5888_WriteReg(0x1a7,0x0741); // 9'h1a7,16'h0741; *
    RDA5888_WriteReg(0x1a8,0x1800); // 9'h1a8,16'h1800;
    RDA5888_WriteReg(0x1a9,0x1500); // 9'h1a9,16'h0700; lum_path_delay *
    RDA5888_WriteReg(0x1aa,0x0000); // 9'h1aa,16'h0000;
    RDA5888_WriteReg(0x1ab,0x0004); // adc_clk_edge, dcdc_div_ct 0x0006 0x0004
    RDA5888_WriteReg(0x1ac,0xa800); // 9'h1ac,16'ha800;edit by 20100727
    RDA5888_WriteReg(0x1ad,0x3000); // 9'h1ad,16'h3000; seek_en
    RDA5888_WriteReg(0x1ae,0x4270); // 9'h1ae,16'h4066; Color or snow 0x4270(vsync = 25hz)
    RDA5888_WriteReg(0x1af,0x0000); // 9'h1af,16'h0000;
    RDA5888_WriteReg(0x1b1,0x8012); // 9'h1b1,16'h8012;
    RDA5888_WriteReg(0x1b2,0x0004); // 9'h1b2,16'h0004;
    RDA5888_WriteReg(0x1b3,0x3f20); // 9'h1b3,16'h3f20;
    RDA5888_WriteReg(0x1b5,0x0008); // 9'h1b5,16'h0008;
    RDA5888_WriteReg(0x1b8,0x0000); // 9'h1b8,16'h0000;
    RDA5888_WriteReg(0x1bb,0x202f); // 9'h1bb,16'h202f;
    RDA5888_WriteReg(0x1bd,0x129d); // 9'h1bd,16'h129d;
    RDA5888_WriteReg(0x1be,0x05c1); // 9'h1be,16'h05c1;
    RDA5888_WriteReg(0x1c0,0x3d08); // 9'h1c0,16'h3d08;
    RDA5888_WriteReg(0x1c1,0x0007); // 9'h1c1,16'h0007;
    RDA5888_WriteReg(0x1c3,0x00c2); // 9'h1c3,16'h00c2; added by xu 01152010
    RDA5888_WriteReg(0x1c4,0x0200); // 9'h1c4,16'h0200; added by 20100727, no   delay hvsync for green arise when swing ana
    RDA5888_WriteReg(0x1cf,0x00c2); // 9'h1cf,16'h00c2;
    RDA5888_WriteReg(0x1d0,0x00c2); // 9'h1d0,16'h00c2;
    RDA5888_WriteReg(0x1d1,0x0311); // 9'h1d1,16'h0311; added by xu 12152009, for scaler
    RDA5888_WriteReg(0x1d2,0x05a0); // 9'h1d2,16'h05a0; added by xu 01222010, for frame buf
    RDA5888_WriteReg(0x1d3,0x0000); // 9'h1d3,16'h0000; *
    RDA5888_WriteReg(0x1d4,0x0000); // 9'h1d4,16'h0000; *
    RDA5888_WriteReg(0x1d5,0x0000); // 9'h1d5,16'h0000; *
    RDA5888_WriteReg(0x1d6,0x0000); // 9'h1d6,16'h0000; *
    RDA5888_WriteReg(0x1d8,0x715e); // 9'h1d8,16'h715e;
    RDA5888_WriteReg(0x1dc,0x6800); // 9'h1dc,16'h6800;
    RDA5888_WriteReg(0x1dd,0x1200); // 9'h1dd,16'h1200;
    RDA5888_WriteReg(0x1de,0x0007); // 9'h1de,16'h0007;
    RDA5888_WriteReg(0x1e0,0x0116); // 9'h1e0,16'h0196; 0x0116, bit7=0 close stero_in
    RDA5888_WriteReg(0x1e3,0x0823); // 9'h1e3,16'h0823; 0x0823 0x0e23
    RDA5888_WriteReg(0x1e6,0x4204); // 9'h1e6,16'h4204;
    RDA5888_WriteReg(0x1e9,0x706c); // 9'h1e9,16'h706c;
    RDA5888_WriteReg(0x1ed,0x08CF); // 9'h1ed,16'h084f; 1ED=08CF, optimize fm seek effect.
    RDA5888_WriteReg(0x1ee,0x1708); // 9'h1ee,16'h1708;
    RDA5888_WriteReg(0x1f0,0x0480); // 9'h1f0,16'h0480;
    RDA5888_WriteReg(0x1f1,0x5c94); // 9'h1f1,16'h5c94;
    RDA5888_WriteReg(0x1f3,0x0228); // 9'h1f3,16'h0228;
    RDA5888_WriteReg(0x1f4,0x2244); // 9'h1f4,16'h1d2d; 1F4=2244, optimize fm seek effect.
    RDA5888_WriteReg(0x1f7,0x3348); // 9'h1f7,16'h3348; 0x3348  0x3188
    RDA5888_WriteReg(0x1f8,0x0015); // 9'h1f8,16'h0037; 1F8=0015, optimize fm seek effect.
    RDA5888_WriteReg(0x1f9,0xf9e9); // 9'h1f9,16'hf9e9;

    RDA5888_WriteReg(0x130,0x0210); // 9'h130,16'h0210;
    RDA5888_WriteReg(0x130,0x0a10); // 9'h130,16'h0A10;

    // customize feature.
    RDA5888_FieldWriteReg(0x1e9, 0x003F, FM_SOFTMUTE_THRESHOLD); // set softmute threshold.

#elif (RDA5888_CHIP_VER == 6)
    RDA5888_WriteReg(0x100,0x0000); // 9'h100,16'h0000; reset dig registers
    RDA5888_WriteReg(0x100,0x0001); // 9'h100,16'h0001;
    RDA5888_WriteReg(0x130,0x0000); // 9'h130,16'h0000; reset dsp
    RDA5888_WriteReg(0x102,0x141e); // 9'h102,16'h141e;
    RDA5888_WriteReg(0x105,0x3fff); // 9'h105,16'h3fff;
    RDA5888_WriteReg(0x106,0x003c); // 9'h106,16'h003c;
    RDA5888_WriteReg(0x107,0x00ff); // 9'h107,16'h00ff; sk_rssi_rate, sk_tmr_afc
    RDA5888_WriteReg(0x108,0x0028); // 9'h108,16'h0028; sk_rssi_th *
    RDA5888_WriteReg(0x10a,0x09df); // 9'h10a,16'h09df;
    RDA5888_WriteReg(0x10b,0x42f7); // 9'h10b,16'h42f7; added by xu 02012010
    RDA5888_WriteReg(0x10c,0x7bdf); // 9'h10c,16'h7bdf;
    RDA5888_WriteReg(0x10d,0x082d); // 9'h10d,16'h082d;
    RDA5888_WriteReg(0x10e,0xff80); // 9'h10e,16'hff80;
    RDA5888_WriteReg(0x10f,0x4444); // 9'h10f,16'h4444;
    RDA5888_WriteReg(0x111,0x5413); // 9'h111,16'h5413;
    RDA5888_WriteReg(0x112,0x3002); // 9'h112,16'h3002;
    RDA5888_WriteReg(0x113,0x90d0); // 9'h113,16'h90d0; edited by xu 02012016012010
    RDA5888_WriteReg(0x114,0xcfe0); // 9'h114,16'hcfe0;

    RDA5888_WriteReg(0x115,0x048a); // 9'h115,16'h048a;
    RDA5888_WriteReg(0x116,0x1000); // 9'h116,16'h1000;
    RDA5888_WriteReg(0x117,0x0000); // 9'h117,16'h0000;
    RDA5888_WriteReg(0x118,0x5fdf); // 9'h118,16'h5fdf; auto black
    RDA5888_WriteReg(0x119,0x2bfb); // 9'h119,16'h2110; *
    RDA5888_WriteReg(0x11b,0x1002); // 9'h11b,16'h1003; *
    RDA5888_WriteReg(0x11c,0xc200); // 9'h11c,16'hc200; *
    RDA5888_WriteReg(0x11d,0x6180); // 9'h11d,16'h6180; ha_index_var_thd & ha_value_var_thd
    RDA5888_WriteReg(0x11e,0x61c2); // 9'h11e,16'h61c2; *
    RDA5888_WriteReg(0x11f,0x980e); // 9'h11f,16'hb80e; *edit 20100127
    RDA5888_WriteReg(0x120,0x403a); // 9'h120,16'h403a; *
    RDA5888_WriteReg(0x125,0x0870); // 9'h125,16'h0870; gainct_fm
    RDA5888_WriteReg(0x129,0x09cb); // 9'h129,16'h09cb; tong 20100806 framebuffer anti interference
    RDA5888_WriteReg(0x12f,0x0c00); // 9'h12f,16'h0c00; by xu, iq swap = 1
    RDA5888_WriteReg(0x133,0x0586); // 9'h133,16'h0586; *
    RDA5888_WriteReg(0x135,0x03cc); // 9'h135,16'h03cc; h_blank_end, cell without shell
    RDA5888_WriteReg(0x137,0x035f); // 9'h137,16'h035f;
    RDA5888_WriteReg(0x13a,0x038a); // 9'h13a,16'h038a; h_chrom_win_start
    RDA5888_WriteReg(0x13b,0x03bc); // 9'h13b,16'h03bc; h_chrom_win_end
    RDA5888_WriteReg(0x140,0x402d); // 9'h140,16'h402d;

    RDA5888_WriteReg(0x141,0x0001); // 9'h141,16'h0001;
    RDA5888_WriteReg(0x142,0x002f); // 9'h142,16'h002f; notch coefficients
    RDA5888_WriteReg(0x143,0x129d); // 9'h143,16'h129d; notch coefficients
    RDA5888_WriteReg(0x144,0x05c1); // 9'h144,16'h05c1; notch coefficients
    RDA5888_WriteReg(0x145,0x00f0); // 9'h145,16'h00f0;
    RDA5888_WriteReg(0x146,0x1220); // 9'h146,16'h1220;  edited by xu 08062010
    RDA5888_WriteReg(0x147,0x3046); // 9'h147,16'h3046;
    RDA5888_WriteReg(0x148,0x647a); // 9'h148,16'h647a;
    RDA5888_WriteReg(0x149,0x9008); // 9'h149,16'h9008;  edited by xu 10182010
    RDA5888_WriteReg(0x14a,0x8000); // 9'h14a,16'h8000; rssi_check_timer
    RDA5888_WriteReg(0x14b,0x0000); // 9'h14b,16'h0000;
    RDA5888_WriteReg(0x14c,0x0000); // 9'h14c,16'h0000;
    RDA5888_WriteReg(0x14d,0x8888); // 9'h14d,16'h8888;
    RDA5888_WriteReg(0x14e,0x8888); // 9'h14e,16'h8888;
    RDA5888_WriteReg(0x14f,0x0000); // 9'h14f,16'h0000;
    RDA5888_WriteReg(0x150,0x0000); // 9'h150,16'h0000;

#ifdef RDA5888_IODRIVEREN_ENABLE
    RDA5888_WriteReg(0x151,0x0022);
    RDA5888_WriteReg(0x152,0x0880);
    RDA5888_WriteReg(0x153,0x0002);
    RDA5888_WriteReg(0x154,0x0000);
    RDA5888_WriteReg(0x155,0x0002);
#else
    RDA5888_WriteReg(0x151,0x0000); // 9'h151,16'h0000;
    RDA5888_WriteReg(0x152,0x0000); // 9'h152,16'h0000;
    RDA5888_WriteReg(0x153,0x0000); // 9'h153,16'h0000;
    RDA5888_WriteReg(0x154,0x0000); // 9'h154,16'h0000;
    RDA5888_WriteReg(0x155,0x0000); // 9'h155,16'h0000;
#endif

    RDA5888_WriteReg(0x156,0x0000); // 9'h156,16'h0000;
    RDA5888_WriteReg(0x157,0x0000); // 9'h157,16'h0000;
    RDA5888_WriteReg(0x158,0x0000); // 9'h158,16'h0000;
    RDA5888_WriteReg(0x159,0x0000); // 9'h159,16'h0000;
    RDA5888_WriteReg(0x15a,0x011c); // 9'h15a,16'h011c;
    RDA5888_WriteReg(0x15b,0x06bf); // 9'h15b,16'h06bf;
    RDA5888_WriteReg(0x15c,0x00b3); // 9'h15c,16'h00b3;
    RDA5888_WriteReg(0x15d,0x0018); // 9'h15d,16'h0025; 0x0018
    RDA5888_WriteReg(0x15e,0x0016); // 9'h15e,16'h0026;
    RDA5888_WriteReg(0x15f,0x0139); // 9'h15f,16'h0118;
    RDA5888_WriteReg(0x160,0x0000); // 9'h160,16'h0000;
    RDA5888_WriteReg(0x161,0x0000); // 9'h161,16'h0000;
    RDA5888_WriteReg(0x162,0x0000); // 9'h162,16'h0000;
    RDA5888_WriteReg(0x163,0x3b13); // 9'h163,16'h3b13;
    RDA5888_WriteReg(0x164,0x0531); // 9'h164,16'h0531;

    RDA5888_WriteReg(0x165,0xf820); // 9'h165,16'hf820; edited by xu 10182010
    RDA5888_WriteReg(0x166,0xf820); // 9'h166,16'hf820;
    RDA5888_WriteReg(0x167,0xf820); // 9'h167,16'hf820;
    RDA5888_WriteReg(0x168,0xec10); // 9'h168,16'hec10;
    RDA5888_WriteReg(0x169,0xdc10); // 9'h169,16'hdc10;
    RDA5888_WriteReg(0x16a,0xae09); // 9'h16a,16'hae09;
    RDA5888_WriteReg(0x16b,0x8e86); // 9'h16b,16'h8e86;

    RDA5888_WriteReg(0x16c,0x0e86); // 9'h16c,16'h0e86;
    RDA5888_WriteReg(0x1ca,0x1500); // 9'h1ca,16'h1500;
    RDA5888_WriteReg(0x16d,0x4924); // 9'h16d,16'h4924;
    RDA5888_WriteReg(0x16e,0x4800); // 9'h16e,16'h4800;
    RDA5888_WriteReg(0x16f,0x0000); // 9'h16f,16'h0000;
    RDA5888_WriteReg(0x170,0x00bf); // 9'h170,16'h00bf; dccancel
    RDA5888_WriteReg(0x171,0x0006); // 9'h171,16'h0006;
    RDA5888_WriteReg(0x172,0x4c1d); // 9'h172,16'h4c1d;
    RDA5888_WriteReg(0x173,0x4c1d); // 9'h173,16'h4c1d;
    RDA5888_WriteReg(0x174,0x008c); // 9'h174,16'h008c;
    RDA5888_WriteReg(0x175,0x1021); // 9'h175,16'h1021;

    RDA5888_WriteReg(0x176,0x0030); // 9'h176,16'h0030;
    RDA5888_WriteReg(0x177,0x01cf); // 9'h177,16'h01cf;
    RDA5888_WriteReg(0x178,0x236d); // 9'h178,16'h236d;
    RDA5888_WriteReg(0x179,0x0080); // 9'h179,16'h0080;
    RDA5888_WriteReg(0x17a,0x1441); // 9'h17a,16'h1441;
#ifdef RDA5888_HSYNCHOLD_ENABLE
    RDA5888_WriteReg(0x17b,0x4000); // 9'h17b,16'h4000; // open the it601 interlace mode.
#else
    RDA5888_WriteReg(0x17b,0x0000); // 9'h17b,16'h4000; // close the it601 interlace mode.
#endif
    RDA5888_WriteReg(0x17c,0x0000); // 9'h17c,16'h0000;
    RDA5888_WriteReg(0x17d,0x0000); // 9'h17d,16'h0000;
    RDA5888_WriteReg(0x17e,0x0000); // 9'h17e,16'h0000;
    RDA5888_WriteReg(0x17f,0x0000); // 9'h17f,16'h0000;
    RDA5888_WriteReg(0x180,0x0004); // 9'h180,16'h0004;
    RDA5888_WriteReg(0x181,0x2040); // 9'h181,16'h2040;
    RDA5888_WriteReg(0x182,0x0138); // 9'h182,16'h0338;
    RDA5888_WriteReg(0x183,0x8235); // 9'h183,16'h8235; afc_disable_fm
    RDA5888_WriteReg(0x184,0x4924); // 9'h184,16'h4924;

    RDA5888_WriteReg(0x185,0x0000); // 9'h185,16'h0000;
    RDA5888_WriteReg(0x186,0x0000); // 9'h186,16'h0000;
    RDA5888_WriteReg(0x187,0x0000); // 9'h187,16'h0000;
    RDA5888_WriteReg(0x188,0x0000); // 9'h188,16'h0000;
    RDA5888_WriteReg(0x189,0x0000); // 9'h189,16'h0000;
    RDA5888_WriteReg(0x18a,0x0000); // 9'h18a,16'h0000;
    RDA5888_WriteReg(0x18b,0x0000); // 9'h18b,16'h0000;
    RDA5888_WriteReg(0x18c,0x0000); // 9'h18c,16'h0000;
    RDA5888_WriteReg(0x18d,0x0000); // 9'h18d,16'h0000;
    RDA5888_WriteReg(0x18e,0x0000); // 9'h18e,16'h0000;
    RDA5888_WriteReg(0x18f,0x051f); // 9'h18f,16'h051f; tmr_agc1
    RDA5888_WriteReg(0x190,0x0008); // 9'h190,16'h0008;

    RDA5888_WriteReg(0x191,0x1a40); // 9'h191,16'h1a40;
    RDA5888_WriteReg(0x192,0x25d4); // 9'h192,16'h25d4;
    RDA5888_WriteReg(0x193,0x3468); // 9'h193,16'h3468;
    RDA5888_WriteReg(0x194,0x8168); // 9'h194,16'h8168;
    RDA5888_WriteReg(0x195,0x0000); // 9'h195,16'h0000;
    RDA5888_WriteReg(0x196,0x0000); // 9'h196,16'h0000;
    RDA5888_WriteReg(0x197,0x0513); // 9'h197,16'h0513;
    RDA5888_WriteReg(0x198,0x0f2a); // 9'h198,16'h0f2a;
    RDA5888_WriteReg(0x199,0x6300); // 9'h199,16'h6300; tagt_pwr *

    RDA5888_WriteReg(0x19a,0x0014); // 9'h19a,16'h0014;
    RDA5888_WriteReg(0x19b,0x1309); // 9'h19B,16'h1309; bypass rate_conv & enable notch & sk_mode
    RDA5888_WriteReg(0x19c,0x00ff); // 9'h19C,16'h00ff; tmr_rssi1
    RDA5888_WriteReg(0x19d,0x14fe); // 9'h19d,16'h14fe; tmr_rssi3
    RDA5888_WriteReg(0x19e,0x0000); // 9'h19e,16'h0000;
    RDA5888_WriteReg(0x19f,0x0000); // 9'h19f,16'h0000;
    RDA5888_WriteReg(0x1a0,0x1f39); // 9'h1a0,16'h1f39;Notch filter for audio
    RDA5888_WriteReg(0x1a1,0x071e); // 9'h1a1,16'h071e;Notch filter for audio
    RDA5888_WriteReg(0x1a2,0x0070); // 9'h1a2,16'h0070;Notch filter for audio
    RDA5888_WriteReg(0x1a3,0x0002); // 9'h1a3,16'h0002;
    RDA5888_WriteReg(0x1a4,0x0000); // 9'h1a4,16'h0000;
    RDA5888_WriteReg(0x1a5,0x0080); // 9'h1a5,16'h0080;
    RDA5888_WriteReg(0x1a6,0xfc00); // 9'h1a6,16'hfc00; *added by tong 2010 0801
    RDA5888_WriteReg(0x1a7,0x0741); // 9'h1a7,16'h0741; *
    RDA5888_WriteReg(0x1a8,0x1920); // 9'h1a8,16'h1920;
    RDA5888_WriteReg(0x1a9,0x1800); // 9'h1a9,16'h1800; lum_path_delay *
    RDA5888_WriteReg(0x1aa,0x0000); // 9'h1aa,16'h0000;
    RDA5888_WriteReg(0x1ab,0x18c0); // 9'h1ab,16'h18c0; clk_dcdc=1.25m, no dither
    RDA5888_WriteReg(0x1ac,0x8800); // 9'h1ac,16'h8800;edit 20100127
    RDA5888_WriteReg(0x1ad,0x3200); // 9'h1ad,16'h3200; seek_en
    RDA5888_WriteReg(0x1ae,0x4270); // 9'h1ae,16'h4270;
    RDA5888_WriteReg(0x1af,0x0000); // 9'h1af,16'h0000;
    RDA5888_WriteReg(0x1b0,0x8000); // 9'h1b0,16'h8000;
    RDA5888_WriteReg(0x1b3,0x3f20); // 9'h1b3,16'h3f20;
    RDA5888_WriteReg(0x1b5,0x0008); // 9'h1b5,16'h0008;
    RDA5888_WriteReg(0x1b8,0x0000); // 9'h1b8,16'h0000;
    RDA5888_WriteReg(0x1bb,0x202f); // 9'h1bb,16'h202f;
    RDA5888_WriteReg(0x1bd,0x129d); // 9'h1bd,16'h129d;
    RDA5888_WriteReg(0x1be,0x05c1); // 9'h1be,16'h05c1;
    RDA5888_WriteReg(0x1c0,0x3d08); // 9'h1c0,16'h3d08;
    RDA5888_WriteReg(0x1c1,0x0007); // 9'h1c1,16'h0007;
    RDA5888_WriteReg(0x1c2,0x0000); // 9'h1c2,16'h0000;
    RDA5888_WriteReg(0x1c3,0x20c2); // 9'h1c3,16'h20c2; added by xu 01152010
#ifdef RDA5888_HSYNCHOLD_ENABLE
    RDA5888_WriteReg(0x1c4,0x0248); // 9'h1c4,16'h0248;
#else
    RDA5888_WriteReg(0x1c4,0x0200); // 9'h1c4,16'h0248; added by xu 01152010r, no  delay hvsync for green arise when swing ana
#endif
    RDA5888_WriteReg(0x1c5,0x0001); // 9'h1c5,16'h0001; added by xu 07192010

    RDA5888_WriteReg(0x1ca,0x1500); // 9'h1ca,16'h1500;
    RDA5888_WriteReg(0x1ce,0x0000); // 9'h1ce,16'h0000;
    RDA5888_WriteReg(0x1cf,0x00c2); // 9'h1cf,16'h00c2;
    RDA5888_WriteReg(0x1d0,0x00c2); // 9'h1d0,16'h00c2;
    RDA5888_WriteReg(0x1d1,0x0311); // 9'h1d1,16'h0311; added by xu 12152009, for scaler
    RDA5888_WriteReg(0x1d3,0x0000); // 9'h1d3,16'h0000; *
    RDA5888_WriteReg(0x1d2,0x05a0); // 9'h1d2,16'h05a0; added by xu 01222010, for frame buf
    RDA5888_WriteReg(0x1d4,0x0000); // 9'h1d4,16'h0000; *
    RDA5888_WriteReg(0x1d5,0x0000); // 9'h1d5,16'h0000; *
    RDA5888_WriteReg(0x1d6,0x0000); // 9'h1d6,16'h0000; *

    RDA5888_WriteReg(0x1d8,0x71fe); // 9'h1d8,16'h71fe;
    RDA5888_WriteReg(0x1dc,0x6800); // 9'h1dc,16'h6800;
    RDA5888_WriteReg(0x1dd,0x1200); // 9'h1dd,16'h1200;
    RDA5888_WriteReg(0x1de,0x3800); // 9'h1de,16'h3800;
    RDA5888_WriteReg(0x1a9,0x1f00); // 9'h1a9,16'h1f00;
    RDA5888_WriteReg(0x1dd,0xd200); // 9'h1dd,16'hd200; added by xu 06022010, bypass framebuf when async,but not reset
    RDA5888_WriteReg(0x1e0,0x0116); // 9'h1e0,16'h0116;disable stero by 20100727
    RDA5888_WriteReg(0x1e3,0x0e1b); // 9'h1e3,16'h0823; 0x0823
    RDA5888_WriteReg(0x1e6,0x4204); // 9'h1e6,16'h4204;
    RDA5888_WriteReg(0x1e9,0x706c); // 9'h1e9,16'h706c;fm softmute by tong 04212010
    RDA5888_WriteReg(0x1ed,0x08cf); // 9'h1ed,16'h08cf;fm softmute by tong 04212010
    RDA5888_WriteReg(0x1ee,0x1708); // 9'h1ee,16'h1708;
    RDA5888_WriteReg(0x1f0,0x0480); // 9'h1f0,16'h0480;
    RDA5888_WriteReg(0x1f1,0x5c94); // 9'h1f1,16'h5c94;
    RDA5888_WriteReg(0x1f3,0x0228); // 9'h1f3,16'h0228;
    RDA5888_WriteReg(0x1f4,0x2244); // 9'h1f4,16'h2244;
    RDA5888_WriteReg(0x1f7,0xffff); // 9'h1f7,16'h3348; 0x3348
    RDA5888_WriteReg(0x1f8,0x0015); // 9'h1f8,16'h0015;
    RDA5888_WriteReg(0x1f9,0xf989); // 9'h1f9,16'hf989;
    //;wait 1m; *
    RDA5888_WriteReg(0x130,0x0010); // 9'h130,16'h0010;
    RDA5888_WriteReg(0x130,0x0810); // 9'h130,16'h0810;

    // customize feature.
    RDA5888_FieldWriteReg(0x1e9, 0x003F, FM_SOFTMUTE_THRESHOLD); // set softmute threshold.

#ifdef RDA5888_DSPINTCOEF_ENABLE
    ATVDSPCoefSet();
#endif

#elif (RDA5888_CHIP_VER == 7)
    // item2345235:inital_dig_5888h
    RDA5888_WriteReg(0x100, 0x0000); // 9'h100,16'h0000; reset dig registers
    RDA5888_WriteReg(0x100, 0x0001); // 9'h100,16'h0001;
    RDA5888_WriteReg(0x130, 0x0000); // 9'h130,16'h0000; reset dsp
    RDA5888_WriteReg(0x102, 0x141e); // 9'h102,16'h141e;
    RDA5888_WriteReg(0x105, 0xffff); // 9'h105,16'h3fff; 0x3fff afc_disable
    RDA5888_WriteReg(0x106, 0x003c); // 9'h106,16'h003c;
    RDA5888_WriteReg(0x107, 0x00ff); // 9'h107,16'h00ff; sk_rssi_rate, sk_tmr_afc
    RDA5888_WriteReg(0x108, 0x0028); // 9'h108,16'h0028; sk_rssi_th *
    RDA5888_WriteReg(0x10a, 0x09df); // 9'h10a,16'h09df;
    //lib3.09 0x42f7->0x42fc
    RDA5888_WriteReg(0x10b, 0x42FC); // 9'h10b,16'h42f7; added by xu 02012010
    RDA5888_WriteReg(0x10c, 0x7bdf); // 9'h10c,16'h7bdf;
    RDA5888_WriteReg(0x10d, 0x082d); // 9'h10d,16'h082d;
    RDA5888_WriteReg(0x10e, 0xff80); // 9'h10e,16'hff80;
    RDA5888_WriteReg(0x10f, 0x4444); // 9'h10f,16'h4444;
    RDA5888_WriteReg(0x111, 0x5413); // 9'h111,16'h5413;
    RDA5888_WriteReg(0x112, 0x3002); // 9'h112,16'h3002; edited by xu 09032010
    RDA5888_WriteReg(0x113, 0x90d0); // 9'h113,16'h90d0;    edited by xu 02012010
    RDA5888_WriteReg(0x114, 0xcfe0); // 9'h114,16'hcfe0;
    RDA5888_WriteReg(0x115, 0x048a); // 9'h115,16'h048a;
    RDA5888_WriteReg(0x116, 0x1000); // 9'h116,16'h1000;
    RDA5888_WriteReg(0x117, 0x0000); // 9'h117,16'h0000;
    RDA5888_WriteReg(0x118, 0x5fdf); // 9'h118,16'h5fdf; auto black
    RDA5888_WriteReg(0x119, 0x0c84); // 9'h119,16'h0c84; *edit 20101217
    RDA5888_WriteReg(0x11b, 0x1002); // 9'h11b,16'h1002; *
    RDA5888_WriteReg(0x11c, 0xc200); // 9'h11c,16'hc200; *
    RDA5888_WriteReg(0x11d, 0x6180); // 9'h11d,16'h6180; ha_index_var_thd & ha_value_var_thd
    RDA5888_WriteReg(0x11e, 0x61c2); // 9'h11e,16'h61c2; *
    RDA5888_WriteReg(0x11f, 0x980e); // 9'h11f,16'h980e; *edit 20100827
    RDA5888_WriteReg(0x120, 0x403a); // 9'h120,16'h403a; *
    RDA5888_WriteReg(0x125, 0x0870); // 9'h125,16'h0870; gainct_fm
    RDA5888_WriteReg(0x129, 0x09cb); // 9'h129,16'h09cb; tong 20100806 framebuffer anti interference
    RDA5888_WriteReg(0x12f, 0x0c00); // 9'h12f,16'h0c00; by xu, iq swap = 1
    RDA5888_WriteReg(0x133, 0x0586); // 9'h133,16'h0586; *
    RDA5888_WriteReg(0x134, 0x02f2); // 9'h134,16'h02f2
    RDA5888_WriteReg(0x135, 0x041c); // 9'h135,16'h041c ; by 110115 xu
    RDA5888_WriteReg(0x137, 0x035f); // 9'h137,16'h035f;
    RDA5888_WriteReg(0x13a, 0x038a); // 9'h13a,16'h038a; h_chrom_win_start
    RDA5888_WriteReg(0x13b, 0x03bc); // 9'h13b,16'h03bc; h_chrom_win_end
    RDA5888_WriteReg(0x140, 0x402d); // 9'h140,16'h402d;
    RDA5888_WriteReg(0x141, 0x0001); // 9'h141,16'h0001;
    RDA5888_WriteReg(0x142, 0x00a5); // 9'h142,16'h00a5; notch coefficients 2M
    RDA5888_WriteReg(0x143, 0x1525); // 9'h143,16'h1525; notch coefficients
    RDA5888_WriteReg(0x144, 0x0425); // 9'h144,16'h0425; notch coefficients
    RDA5888_WriteReg(0x145, 0x00f0); // 9'h145,16'h00f0;
    RDA5888_WriteReg(0x146, 0x1220); // 9'h146,16'h1220;  edited by xu 08062010
    RDA5888_WriteReg(0x147, 0x3046); // 9'h147,16'h3046;
    RDA5888_WriteReg(0x148, 0x647a); // 9'h148,16'h647a;
    RDA5888_WriteReg(0x149, 0x8808); // 9'h149,16'h8808;
    RDA5888_WriteReg(0x14a, 0x8000); // 9'h14a,16'h8000; rssi_check_timer
    RDA5888_WriteReg(0x14b, 0x0000); // 9'h14b,16'h0000;
    RDA5888_WriteReg(0x14c, 0x0000); // 9'h14c,16'h0000;
    RDA5888_WriteReg(0x14d, 0x8888); // 9'h14d,16'h8888;
    RDA5888_WriteReg(0x14e, 0x8888); // 9'h14e,16'h8888;
    RDA5888_WriteReg(0x14f, 0x0000); // 9'h14f,16'h0000;
    RDA5888_WriteReg(0x150, 0x0400); // 9'h150,16'h0400;  SCK pull-down by 101218

#ifdef RDA5888_IODRIVEREN_ENABLE
    RDA5888_WriteReg(0x151,0x0022);
    RDA5888_WriteReg(0x152,0x0880);
    RDA5888_WriteReg(0x153,0x0002);
    RDA5888_WriteReg(0x154,0x0000);
    RDA5888_WriteReg(0x155,0x0002);
#else
    RDA5888_WriteReg(0x151,0x0000);
    RDA5888_WriteReg(0x152,0x0000);
    RDA5888_WriteReg(0x153,0x0000);
    RDA5888_WriteReg(0x154,0x0000);
    RDA5888_WriteReg(0x155,0x0000);
#endif

    RDA5888_WriteReg(0x156, 0x0000); // 9'h156,16'h0000;
    RDA5888_WriteReg(0x157, 0x0000); // 9'h157,16'h0000;
    RDA5888_WriteReg(0x158, 0x0000); // 9'h158,16'h0000;
    RDA5888_WriteReg(0x159, 0x0000); // 9'h159,16'h0000;
    RDA5888_WriteReg(0x15a, 0x0124); // 9'h15a,16'h0124;by 110115 xu
    RDA5888_WriteReg(0x15b, 0x06bf); // 9'h15b,16'h06bf;
    RDA5888_WriteReg(0x15c, 0x00b3); // 9'h15c,16'h00b3;
    RDA5888_WriteReg(0x15d, 0x0017); // 9'h15d,16'h0017; by 110115 xu
    RDA5888_WriteReg(0x15e, 0x0016); // 9'h15e,16'h0016; change 100826
    RDA5888_WriteReg(0x15f, 0x0139); // 9'h15f,16'h0139;  change 100826

    RDA5888_WriteReg(0x160, 0x0fde); // 9'h160,16'h0fde; pllbb setting
    RDA5888_WriteReg(0x161, 0x0000); // 9'h161,16'h0000;
    RDA5888_WriteReg(0x162, 0x0000); // 9'h162,16'h0000;
    RDA5888_WriteReg(0x163, 0x0000); // 9'h163,16'h0000;
    RDA5888_WriteReg(0x164, 0x0500); // 9'h164,16'h0500;

    RDA5888_WriteReg(0x165, 0x7820); // 9'h165,16'h7820; edited by xu 10182010
    RDA5888_WriteReg(0x166, 0x7820); // 9'h166,16'h7820;
    RDA5888_WriteReg(0x167, 0x7820); // 9'h167,16'h7820;
    RDA5888_WriteReg(0x168, 0x7c10); // 9'h168,16'h7c10;
    RDA5888_WriteReg(0x169, 0x7c10); // 9'h169,16'h7c10;
    RDA5888_WriteReg(0x16a, 0x7e08); // 9'h16a,16'h7e08;
    RDA5888_WriteReg(0x16b, 0x7e86); // 9'h16b,16'h7e86;
    RDA5888_WriteReg(0x16c, 0xbe86); // 9'h16c,16'hbe86;

    RDA5888_WriteReg(0x1ca, 0x1500); // 9'h1ca,16'h1500;
    RDA5888_WriteReg(0x16d, 0x4924); // 9'h16d,16'h4924;
    RDA5888_WriteReg(0x16e, 0x4800); // 9'h16e,16'h4800;
    RDA5888_WriteReg(0x16f, 0x0000); // 9'h16f,16'h0000;
    RDA5888_WriteReg(0x170, 0x00bf); // 9'h170,16'h00bf; dccancel
    RDA5888_WriteReg(0x171, 0x0006); // 9'h171,16'h0006;
    //lib3.09 0x4c1d->0x4c14
    RDA5888_WriteReg(0x172, 0x4c14); // 9'h172,16'h4c1d;
    RDA5888_WriteReg(0x173, 0x4c14); // 9'h173,16'h4c1d;
    RDA5888_WriteReg(0x174, 0x008c); // 9'h174,16'h008c;
    RDA5888_WriteReg(0x175, 0x1021); // 9'h175,16'h1021;
    RDA5888_WriteReg(0x176, 0x0030); // 9'h176,16'h0030;
    RDA5888_WriteReg(0x177, 0x01cf); // 9'h177,16'h01cf;
    RDA5888_WriteReg(0x178, 0x236d); // 9'h178,16'h236d;
    RDA5888_WriteReg(0x179, 0x0080); // 9'h179,16'h0080;
    RDA5888_WriteReg(0x17a, 0x1441); // 9'h17a,16'h1441;

#ifdef RDA5888_HSYNCHOLD_ENABLE
    RDA5888_WriteReg(0x17b,0x4000); // 9'h17b,16'h4000; // open the it601 interlace mode.
#else
    RDA5888_WriteReg(0x17b,0x0000); // 9'h17b,16'h4000; // close the it601 interlace mode.
#endif

    RDA5888_WriteReg(0x17c, 0x0000); // 9'h17c,16'h0000;
    RDA5888_WriteReg(0x17d, 0x0000); // 9'h17d,16'h0000;
    RDA5888_WriteReg(0x17e, 0x0000); // 9'h17e,16'h0000;
    //lib3.09 0x0000->0x01c0
    RDA5888_WriteReg(0x17f, 0x01c0); // 9'h17f,16'h0000;
    RDA5888_WriteReg(0x180, 0x0004); // 9'h180,16'h0004;
    RDA5888_WriteReg(0x181, 0x2040); // 9'h181,16'h2040;
    RDA5888_WriteReg(0x182, 0x0138); // 9'h182,16'h0138;edit by tong 20100827
    RDA5888_WriteReg(0x183, 0xe235); // 9'h183,16'he235; afc_disable_fm
    RDA5888_WriteReg(0x184, 0x4924); // 9'h184,16'h4924;
    RDA5888_WriteReg(0x185, 0x0000); // 9'h185,16'h0000;
    RDA5888_WriteReg(0x186, 0x0000); // 9'h186,16'h0000;
    RDA5888_WriteReg(0x187, 0x0000); // 9'h187,16'h0000;
    RDA5888_WriteReg(0x188, 0x0000); // 9'h188,16'h0000;
    RDA5888_WriteReg(0x189, 0x0000); // 9'h189,16'h0000;
    RDA5888_WriteReg(0x18a, 0x0000); // 9'h18a,16'h0000;
    RDA5888_WriteReg(0x18b, 0x0000); // 9'h18b,16'h0000;
    RDA5888_WriteReg(0x18c, 0x0000); // 9'h18c,16'h0000;
    RDA5888_WriteReg(0x18d, 0x0000); // 9'h18d,16'h0000;
    RDA5888_WriteReg(0x18e, 0x0000); // 9'h18e,16'h0000;
    RDA5888_WriteReg(0x18f, 0x051f); // 9'h18f,16'h051f; tmr_agc1
    RDA5888_WriteReg(0x190, 0x0008); // 9'h190,16'h0008;

    RDA5888_WriteReg(0x191, 0x1ac1); // 9'h191,16'h1ac1; by zm 101222
    RDA5888_WriteReg(0x192, 0x25d4); // 9'h192,16'h25d4;
    RDA5888_WriteReg(0x193, 0x2edd); // 9'h193,16'h2edd;
    RDA5888_WriteReg(0x194, 0x815d); // 9'h194,16'h815d;
    RDA5888_WriteReg(0x195, 0x0306); // 9'h195,16'h0306;
    RDA5888_WriteReg(0x196, 0x0306); // 9'h196,16'h0306;
    //lib3.09 0x0798->0x0a9e
    RDA5888_WriteReg(0x197, 0x0a9e); // 9'h197,16'h0798;
    //lib3.09 0x112e->0x1434
    RDA5888_WriteReg(0x198, 0x1434); // 9'h198,16'h112e;
    RDA5888_WriteReg(0x199, 0x6300); // 9'h199,16'h6300; tagt_pwr *

    RDA5888_WriteReg(0x19a, 0x0014); // 9'h19a,16'h0014;
    RDA5888_WriteReg(0x19b, 0x0f09); // 9'h19B,16'h0f09; bypass rate_conv & enable notch & sk_mode
    RDA5888_WriteReg(0x19c, 0x00ff); // 9'h19C,16'h00ff; tmr_rssi1
    RDA5888_WriteReg(0x19d, 0x14fe); // 9'h19d,16'h14fe; tmr_rssi3
    RDA5888_WriteReg(0x19e, 0x0000); // 9'h19e,16'h0000;
    RDA5888_WriteReg(0x19f, 0x0000); // 9'h19f,16'h0000;
    RDA5888_WriteReg(0x1a0, 0x1f39); // 9'h1a0,16'h1f39;Notch filter for audio
    RDA5888_WriteReg(0x1a1, 0x071e); // 9'h1a1,16'h071e;Notch filter for audio
    RDA5888_WriteReg(0x1a2, 0x0070); // 9'h1a2,16'h0070;Notch filter for audio
    RDA5888_WriteReg(0x1a3, 0x0002); // 9'h1a3,16'h0002;
    RDA5888_WriteReg(0x1a4, 0x0000); // 9'h1a4,16'h0000;
    RDA5888_WriteReg(0x1a5, 0x0080); // 9'h1a5,16'h0080;
    RDA5888_WriteReg(0x1a6, 0xc120); // 9'h1a6,16'hc120; *added by tong 2010 0801
    RDA5888_WriteReg(0x1a7, 0x0741); // 9'h1a7,16'h0741; *
    RDA5888_WriteReg(0x1a8, 0x1920); // 9'h1a8,16'h1920;
    RDA5888_WriteReg(0x1a9, 0x1800); // 9'h1a9,16'h1800; lum_path_delay *added by tong 2010 0903
    RDA5888_WriteReg(0x1aa, 0x0000); // 9'h1aa,16'h0000;
    RDA5888_WriteReg(0x1ab, 0x18c8); // 9'h1ab,16'h18c8; clk_dcdc=1.25m, no dither
    RDA5888_WriteReg(0x1ac, 0x8800); // 9'h1ac,16'h8800;edit 20100127
    RDA5888_WriteReg(0x1ad, 0x3000); // 9'h1ad,16'h3000; seek_en
    RDA5888_WriteReg(0x1ae, 0x4270); // 9'h1ae,16'h4270; change 100826
    RDA5888_WriteReg(0x1af, 0x0000); // 9'h1af,16'h0000;
    RDA5888_WriteReg(0x1b0, 0x8000); // 9'h1b0,16'h8000;
    RDA5888_WriteReg(0x1b3, 0x3f20); // 9'h1b3,16'h3f20;
    RDA5888_WriteReg(0x1b5, 0x0008); // 9'h1b5,16'h0008;
    RDA5888_WriteReg(0x1b8, 0x0000); // 9'h1b8,16'h0000;
    RDA5888_WriteReg(0x1bb, 0x202f); // 9'h1bb,16'h202f;
    RDA5888_WriteReg(0x1bd, 0x129d); // 9'h1bd,16'h129d;
    RDA5888_WriteReg(0x1be, 0x05c1); // 9'h1be,16'h05c1;
    RDA5888_WriteReg(0x1c0, 0x3d08); // 9'h1c0,16'h3d08;
    RDA5888_WriteReg(0x1c1, 0x0007); // 9'h1c1,16'h0007;
    RDA5888_WriteReg(0x1c2, 0x0001); // 9'h1c2,16'h0001;
    RDA5888_WriteReg(0x1c3, 0x20c2); // 9'h1c3,16'h20c2; added by xu 01152010

#ifdef RDA5888_HSYNCHOLD_ENABLE
    RDA5888_WriteReg(0x1c4,0x0248); // 9'h1c4,16'h0248;
#else
    RDA5888_WriteReg(0x1c4,0x0200); // 9'h1c4,16'h0248; added by xu 01152010r, no  delay hvsync for green arise when swing ana
#endif

    RDA5888_WriteReg(0x1c5, 0x0001); // 9'h1c5,16'h0001; added by xu 07192010
    RDA5888_WriteReg(0x1ca, 0x1500); // 9'h1ca,16'h1500;
    RDA5888_WriteReg(0x1ce, 0x1234); // 9'h1ce,16'h1234;
    RDA5888_WriteReg(0x1cf, 0x00c2); // 9'h1cf,16'h00c2;
    RDA5888_WriteReg(0x1d0, 0x00c2); // 9'h1d0,16'h00c2;
    RDA5888_WriteReg(0x1d1, 0x0321); // 9'h1d1,16'h0321; by 110115 xu
    RDA5888_WriteReg(0x1d2, 0x057e); // 9'h135,16'h041c ;
    //lib3.09 0x00f0->0x0070
    RDA5888_WriteReg(0x1d3, 0x0070); // 9'h1d3,16'h00f0; *
    RDA5888_WriteReg(0x1d4, 0x0000); // 9'h1d4,16'h0000; *
    RDA5888_WriteReg(0x1d5, 0x0000); // 9'h1d5,16'h0000; *
    RDA5888_WriteReg(0x1d6, 0x0000); // 9'h1d6,16'h0000; *
    RDA5888_WriteReg(0x1d7, 0x0207); // 9'h1d7,16'h0207; added for framebuf coef sepe
    RDA5888_WriteReg(0x1d8, 0x71fe); // 9'h1d8,16'h71fe; edited by xutao 09032010 for eco1
    RDA5888_WriteReg(0x1dc, 0x6800); // 9'h1dc,16'h6800;
    RDA5888_WriteReg(0x1dd, 0x1200); // 9'h1dd,16'h1200;
    RDA5888_WriteReg(0x1de, 0x3800); // 9'h1de,16'h3800;dited by xutao 09032010
    RDA5888_WriteReg(0x1dd, 0xd200); // 9'h1dd,16'hd200; added by xu 06022010, bypass framebuf when async,but not reset
    RDA5888_WriteReg(0x1e0, 0x0c10); // 9'h1e0,16'h0c10;
    RDA5888_WriteReg(0x1e1, 0x0c10); // 9'h1e1,16'h0c10;
    RDA5888_WriteReg(0x1e2, 0x0c10); // 9'h1e2,16'h0c10;
    RDA5888_WriteReg(0x1e3, 0x0e08); // 9'h1e3,16'h0e08;
    RDA5888_WriteReg(0x1e4, 0x0e08); // 9'h1e4,16'h0e08;
    RDA5888_WriteReg(0x1e5, 0x0f04); // 9'h1e5,16'h0f04;
    RDA5888_WriteReg(0x1e6, 0x0f04); // 9'h1e6,16'h0f04;
    RDA5888_WriteReg(0x1e7, 0x0f04); // 9'h1e7,16'h0f04;
    RDA5888_WriteReg(0x1e9, 0x706c); // 9'h1e9,16'h706c;fm softmute by tong 04212010
    RDA5888_WriteReg(0x1ed, 0x08cf); // 9'h1ed,16'h08cf;fm softmute by tong 04212010
    RDA5888_WriteReg(0x1ee, 0x1708); // 9'h1ee,16'h1708;
    RDA5888_WriteReg(0x1f0, 0x0480); // 9'h1f0,16'h0480;
    RDA5888_WriteReg(0x1f1, 0x5c94); // 9'h1f1,16'h5c94;
    RDA5888_WriteReg(0x1f3, 0x0228); // 9'h1f3,16'h0228;
    RDA5888_WriteReg(0x1f4, 0x2244); // 9'h1f4,16'h2244;
    RDA5888_WriteReg(0x1f5, 0x3348); // 9'h1f7,16'h3348;
    RDA5888_WriteReg(0x1f8, 0x0015); // 9'h1f8,16'h0015;
    RDA5888_WriteReg(0x1f9, 0xf989); // 9'h1f9,16'hf989;

    RDA5888_WriteReg(0x1d8, 0x71f8); // 9'h1d8,16'h71f8;
    RDA5888_WriteReg(0x1a8, 0x1c40); // 9'h1a8,16'h1840;  0x1840
    RDA5888_WriteReg(0x114, 0xcfe0); // 9'h114,16'hcfe0;
    RDA5888_WriteReg(0x1a6, 0xc020); // 9'h1a6,16'hc020;
    RDA5888_WriteReg(0x11e, 0x6182); // 9'h11e,16'h6182;
    RDA5888_WriteReg(0x11b, 0x9002); // 9'h11b,16'h9002;

    RDA5888_WriteReg(0x1de, 0x463f); // 9'h1de,16'h463f;
    RDA5888_WriteReg(0x1b2, 0x0f0f); // 9'h1b2,16'h0f0f;
    RDA5888_WriteReg(0x1c3, 0x2000); // 9'h1c3,16'h2000;
    RDA5888_WriteReg(0x1dd, 0x9200); // 9'h1dd,16'h9200;
    RDA5888_WriteReg(0x1d7, 0x0007); // 9'h1d7,16'h0007;

    //;wait 1m; *
    RDA5888_WriteReg(0x130, 0x0010); // 9'h130,16'h0010;
    RDA5888_WriteReg(0x130, 0x0810); // 9'h130,16'h0810;

    RDA5888_WriteReg(0x1AC, 0x9800);

    // customize feature.
    RDA5888_FieldWriteReg(0x0b0, 0x3F00, FM_SOFTMUTE_THRESHOLD2); // set softmute threshold (default: 8).

#ifdef RDA5888_DSPINTCOEF_ENABLE
    ATVDSPCoefSet();
#endif

#endif

    //RDAEXT_DebugPrint(RDA5888_TRACE_MOD  "[rdamtv]RDA_Inital_Dig End!\n");
}


#ifdef RDA5888_26MCRYSTAL_ENABLE
void RDA5888_26MCrystalInit(void)
{
#if (RDA5888_CHIP_VER == 4)
    RDA5888_WriteReg(0x163, 0x3b14); // 9'h163,16'h3B14;
    RDA5888_WriteReg(0x164, 0x0531); // 9'h164,16'h0531;
    RDA5888_WriteReg(0x080, 0x0049); // 9'h080,16'h0049;
    RDA5888_WriteReg(0x106, 0x401c); // 9'h106,16'h401C;
    RDA5888_WriteReg(0x087, 0x0c0a); // 9'h087,16'h0C0A;
    RDA5888_WriteReg(0x088, 0x1524); // 9'h088,16'h1524; xtal cap for 8.5pF 26MHz
#ifdef RDA5888_LDOMODE_ENABLE
    RDA5888_WriteReg(0x0a1, 0x018a); // 9'h0A1,16'h018A;
    RDA5888_WriteReg(0x010, 0x8400); // 9'h010,16'h8400;
    RDA5888_WriteReg(0x011, 0x006f); // 9'h011,16'h006F;
    RDA5888_WriteReg(0x011, 0x004f); // 9'h011,16'h004F;
    //wait 1m;
    RDAEXT_DelayMs(2);
    RDA5888_WriteReg(0x011, 0x006f); // 9'h011,16'h006F;
    RDA5888_WriteReg(0x011, 0x004f); // 9'h011,16'h004F;
    RDA5888_WriteReg(0x010, 0x0500); // 9'h010,16'h0500;
#else
    RDA5888_WriteReg(0x010, 0x8400); // 9'h010,16'h8400;
    RDA5888_WriteReg(0x011, 0x002f); // 9'h011,16'h002F;
    RDA5888_WriteReg(0x011, 0x000f); // 9'h011,16'h000F;
    //wait 1m;
    RDAEXT_DelayMs(2);
    RDA5888_WriteReg(0x011, 0x002f); // 9'h011,16'h002F;
    RDA5888_WriteReg(0x011, 0x000f); // 9'h011,16'h000F;
    RDA5888_WriteReg(0x010, 0x0500); // 9'h010,16'h0500;
#endif
#elif (RDA5888_CHIP_VER == 5)
    RDA5888_WriteReg(0x163, 0x3b14); // 9'h163,16'h3B14;
    RDA5888_WriteReg(0x164, 0x0531); // 9'h164,16'h0531;
    RDA5888_WriteReg(0x080, 0x0049); // 9'h080,16'h0049;
    RDA5888_WriteReg(0x106, 0x401c); // 9'h106,16'h401C;
    RDA5888_WriteReg(0x087, 0x0c0a); // 9'h087,16'h0C0A;
    RDA5888_WriteReg(0x088, 0x1d24); // 9'h088,16'h1D24;
    RDA5888_WriteReg(0x010, 0x8400); // 9'h010,16'h8400;
    RDA5888_WriteReg(0x011, 0x002f); // 9'h011,16'h002F;
    RDA5888_WriteReg(0x011, 0x000f); // 9'h011,16'h000F;
    //wait 1m;
    RDAEXT_DelayMs(2);
    RDA5888_WriteReg(0x011, 0x002f); // 9'h011,16'h002F;
    RDA5888_WriteReg(0x011, 0x000f); // 9'h011,16'h000F;
    RDA5888_WriteReg(0x010, 0x0500); // 9'h010,16'h0500;
#elif (RDA5888_CHIP_VER == 6)
    // 26M crystal-less
    RDA5888_WriteReg(0x038, 0x5151); // 9'h038,16'h5151;
    RDA5888_WriteReg(0x01d, 0x4e10); // 9'h01d,16'h4e10;
    RDA5888_WriteReg(0x021, 0x869c); // 9'h021,16'h869c;
    RDA5888_WriteReg(0x02e, 0xab0c); // 9'h02e,16'hab0c;
    RDA5888_WriteReg(0x03a, 0x6590); // 9'h03a,16'h6590;
    RDA5888_WriteReg(0x030, 0x0005); // 9'h030,16'h0005;
    RDA5888_WriteReg(0x030, 0x0007); // 9'h030,16'h0007;
    RDAEXT_DelayMs(500); // wait 50m; *
    RDA5888_WriteReg(0x106, 0x403c); // 9'h106,16'h403c;
    RDA5888_WriteReg(0x164, 0x0531); // 9'h164,16'h0531;
    RDA5888_WriteReg(0x163, 0x3b13); // 9'h163,16'h3b13;
    RDA5888_WriteReg(0x038, 0x4151); // 9'h038,16'h4151;
    RDA5888_WriteReg(0x038, 0x5151); // 9'h038,16'h5151;
#elif (RDA5888_CHIP_VER == 7)
    RDA5888_WriteReg(0x021, 0x96e0); // 9'h021,16'h96E0; for 8.5pf 26M DCXO
    RDA5888_WriteReg(0x03a, 0x6590); // 9'h03a,16'h6590;
    RDA5888_WriteReg(0x163, 0x3b13); // 9'h163,16'h3b13;
    RDA5888_WriteReg(0x164, 0x0531); // 9'h164,16'h0531;
#endif
}
#endif

// ---------------------------------------------------------------------------
// RDA5888_ScanChannel.
//
// Params:
//  dwVFreq    : Desired tuning frequency in hertz.   i.e. 57.75mhz  =
//              57750000.
//  eVideoStd  : AnalogTV standard set.
//  nFlag      : 0: set channel, 1: scan channel.
//
// Return:
//  RDAMTV_RT_ERROR:     an error occurred communicating with the device.
//  RDAMTV_RT_SCAN_FAIL: not found channel.
//  RDAMTV_RT_SCAN_DONE: found channel.
// ---------------------------------------------------------------------------
#if ((RDA5888_CHIP_VER == 5) || (RDA5888_CHIP_VER == 6) || (RDA5888_CHIP_VER == 7))
#define SEEK_TV_THRESHOLD        65 // (defaul: 65)
#define K1                       1
#define K2                       2
uint32 RDA5888_ScanChannel(uint32 dwVFreq, rdamtv_vstd_t eVideoStd, uint32 nFlag)
{
    uint16 var,reg;
    uint32 M1, M2, M3, rt;

    //dwVFreq = 527250000; // set fixed freq, for test.
    ATVCalcFreqOffset(eVideoStd);
    RDA5888_FreqSet(dwVFreq);

#if (RDA5888_CHIP_VER == 5)
    ATVEnable();
    ATVSetRxOn();
    RDAEXT_DelayMs(30);
    RDA5888_DSPInit();
#ifdef RDA5888_26MCRYSTAL_ENABLE
    RDA5888_26MCrystalInit();
#endif
#elif ((RDA5888_CHIP_VER == 6)  || (RDA5888_CHIP_VER == 7))
    ATVSetRxOn();
    RDAEXT_DelayMs(20);
#endif

    RDA5888_VideoStdSet(eVideoStd);
    ATVSetNotchFilter(eVideoStd, dwVFreq);

    RDA5888_CoolSandAtvPatch();

    RDAEXT_DelayMs(20);

#if (RDA5888_CHIP_VER == 7)
    // vsb_dem reset
    RDA5888_WriteReg(0x103, 0x020d);
    RDA5888_WriteReg(0x103, 0x000d);
    RDAEXT_DelayMs(60);

    // fm enable
    //RDA5888_FieldWriteReg(0x82, 0x0001, 0x0);
    //RDA5888_FieldWriteReg(0x82, 0x0001, 0x1);
    //RDAEXT_DelayMs(100);

    // read noise
    M1 = RDA5888_FieldReadReg(0x086, 0x1fc0);
#else
    // read noise
    RDA5888_ReadReg(0x1ff, &reg);
    M1 = reg & 0x007f;
#endif

    RDA5888_WriteReg(0x12f, 0x0c35);
    RDA5888_ReadReg(0x117, &reg);
    M2 = (reg & 0x00fe) >> 1;
    M3 = (reg & 0x7f00) >> 8;

    var = K1*M1 + K2*(M3-M2);

    if (var < SEEK_TV_THRESHOLD)
    {
        rt = RDAMTV_RT_SCAN_DONE;
        RDAEXT_DebugPrint(RDA5888_TRACE_MOD  "[rdamtv]RDA5888_ScanChannel Done, var = %d\n", var);
    }
    else
    {
        rt = RDAMTV_RT_SCAN_FAIL;
    }

    //ATVSysReset();

    RDAEXT_DebugPrint(RDA5888_TRACE_MOD  "[rdamtv]RDA5888_ScanChannel End, seek_done = %d\n", rt);

    return rt;
}
#else
uint32 RDA5888_ScanChannel(uint32 dwVFreq, rdamtv_vstd_t eVideoStd, uint32 nFlag)
{
    uint16 var;
    uint8 cnt = 0;

    ATVCalcFreqOffset(eVideoStd);

    RDA5888_FreqSet(dwVFreq);
#if (RDA5888_CHIP_VER == 5)
    ATVEnable();
#endif
    ATVSetRxOn();
    RDAEXT_DelayMs(30);

    //if (!nFlag)
    {
        // digital inital
        RDA5888_DSPInit();
    }

#ifdef RDA5888_26MCRYSTAL_ENABLE
    RDA5888_26MCrystalInit();
#endif

    RDA5888_VideoStdSet(eVideoStd);
    ATVSetNotchFilter(eVideoStd, dwVFreq);
    RDAEXT_DelayMs(30);

    // wait seek_ready.
    cnt = 0;
    do
    {
        //get bit_done : reg( 0x100[1])
        var = RDA5888_FieldReadReg(0x0100, 0x0002);
        RDAEXT_DelayMs(20);
        cnt++;
    }
    while ((var == 0) && (cnt < CHECK_SEEK_READY_TIMES));

    if (cnt == CHECK_SEEK_READY_TIMES)
    {
        RDAEXT_DebugPrint(RDA5888_TRACE_MOD  "[rdamtv]scan channel timeout\n");
        return RDAMTV_RT_SCAN_FAIL; // timeout
    }

    // check seek_done.
    var = RDA5888_FieldReadReg(0x0100, 0x0004);

    if (var == 0)
    {
        RDAEXT_DebugPrint(RDA5888_TRACE_MOD  "[rdamtv]scan channel fail\n");
        return RDAMTV_RT_SCAN_FAIL; // seekdone fail
    }

    RDAEXT_DebugPrint(RDA5888_TRACE_MOD  "[rdamtv]scan channel successful\n");

    RDA5888_WriteReg(0x108,0x03ff);
    RDA5888_WriteReg(0x1ad,0x0);

#if 0 // for test pattern mode.
    RDA5888_WriteReg(0x130, 0x0803);
#endif

    ATVSysReset();

    return RDAMTV_RT_SCAN_DONE;
}
#endif

// ----------------------------------------------------------------------------
// This function gets the rssi value for the current channel.
//
// Params:
//  None
//
// Return:
//  rssi value.
// ----------------------------------------------------------------------------
int16 RDA5888_GetRSSI(void)
{
    int16 rssi;
    uint16 reg;

#if ((RDA5888_CHIP_VER == 6)  || (RDA5888_CHIP_VER == 7))
    reg = RDA5888_FieldReadReg(0x0185, 0x1fc0);
    rssi = (short)reg - 110;
#else
    RDA5888_ReadReg(0x0185, &reg);
    rssi = ((short)(reg&0x03ff))/8 - 110;
#endif

    return rssi;
}

// ----------------------------------------------------------------------------
// This function sets luminance for the current channel being processed.
//
// Params:
//  nLevel: 0~15
//
// Return:
//  RDAMTV_RT_ERROR if an error occurred communicating with the device.
//  RDAMTV_RT_SUCCESS if the operation was successful.
// ----------------------------------------------------------------------------
uint32 RDA5888_SetLumaLevel(uint8 nLevel)
{
    uint32 rt = RDAMTV_RT_SUCCESS;

    if (nLevel > 15)
    {
        nLevel = 15;
    }

    // signed number.
    nLevel = ((nLevel < 8) ? (nLevel + 8) : (nLevel - 8));

#if ((RDA5888_CHIP_VER == 6)  || (RDA5888_CHIP_VER == 7))
    rt = RDA5888_FieldWriteReg(0x1c2, 0xf000, nLevel);
#else
    rt = RDA5888_FieldWriteReg(0x10b, 0x0f00, nLevel);
#endif

    return rt;
}

// ----------------------------------------------------------------------------
// This function sets chrominance for the current channel being processed.
//
// Params:
//  nLevel: 0~15
//
// Return:
//  RDAMTV_RT_ERROR if an error occurred communicating with the device.
//  RDAMTV_RT_SUCCESS if the operation was successful.
// ----------------------------------------------------------------------------
uint32 RDA5888_SetChromLevel(uint8 nLevel)
{
    uint32 rt = RDAMTV_RT_SUCCESS;

    if (nLevel > 15)
    {
        nLevel = 15;
    }

    // signed number.
    nLevel = ((nLevel < 8) ? (nLevel + 8) : (nLevel - 8));

#if ((RDA5888_CHIP_VER == 6)  || (RDA5888_CHIP_VER == 7))
    rt = RDA5888_FieldWriteReg(0x165, 0xf000, nLevel);
#else
    rt = RDA5888_FieldWriteReg(0x10b, 0x00f0, nLevel);
#endif

    return rt;
}

// ----------------------------------------------------------------------------
// This function sets volume value for audio output.
//
// Params:
//  nValue: volume to set.
//
// Return:
//  RDAMTV_RT_ERROR if an error occurred communicating with the device.
//  RDAMTV_RT_SUCCESS if the operation was successful.
// ----------------------------------------------------------------------------
uint32 RDA5888_SetVolume(uint16 nValue)
{
    uint32 rt = RDAMTV_RT_SUCCESS;

#if 0 // do not adjust volume by gainct[8:0]
#if (RDA5888_CHIP_VER == 4)
    rt = RDA5888_FieldWriteReg(0x125, 0xff80, nValue);
#elif ((RDA5888_CHIP_VER == 5) || (RDA5888_CHIP_VER == 6))
    rt = RDA5888_FieldWriteReg(0x1e4, 0x003f, nValue);
#elif (RDA5888_CHIP_VER == 7)
    rt = RDA5888_FieldWriteReg(0x09f, 0x003f, nValue);
#endif
#else
    rt = RDA5888_FieldWriteReg(0x0a8, 0x01e0, nValue); // range: 0~15.
#endif

    return rt;
}

uint8 RDA5888_Is625Lines(void)
{
    return m_bIs625Lines;
}

float RDA5888_GetAFCOut(void)
{
    float afc_value;
    uint16 reg;

    RDA5888_ReadReg(0x1FB, &reg);
    afc_value = (float)((reg - 32768) * 6.4); // hz

    return afc_value;
}

// ----------------------------------------------------------------------------
// This function gets the chip version.
//
// Params:
//  None
//
// Return:
//  current chip version.
// ----------------------------------------------------------------------------
uint8 RDA5888_GetChipVer(void)
{
    uint16 reg;
    uint8 chip_ver = 0xff;
    uint8 i = 0;

    for (i = 0; i < 5; i++)  // to try get it many times if it return fail of chip_ver.
    {
        RDA5888_ReadReg(0x0, &reg);
        if (0x31e2 == reg) // 5888D or 5888E
        {
            RDA5888_ReadReg(0x9f, &reg);
            if (0x0000 == reg)
            {
                chip_ver = RDA5888D;
                break;
            }
            else if (0x0001 == reg)
            {
                chip_ver = RDA5888S;
                break;
            }
        }
        else if (0x5888 == reg)
        {
            RDA5888_WriteReg(0x002, 0x0001);
            RDA5888_ReadReg(0x080, &reg);
            if (0x5804 == reg)
            {
                chip_ver = RDA5888H;
            }
            else
            {
                chip_ver = RDA5888E;
            }
            break;
        }
    }

    RDAEXT_DebugPrint(RDA5888_TRACE_MOD  "[rdamtv]RDA5888_GetChipVer, chip_ver = %d\n", chip_ver);

    return chip_ver;
}

// ----------------------------------------------------------------------------
// This function open the I2S interface.
//
// Params:
//  cfg: I2S configuration.
//
// Return:
//  none.
// ----------------------------------------------------------------------------
void  RDA5888_I2SOpen(uint8 cfg)
{
#if (RDA5888_CHIP_VER != 7)
    //I2S slave
    RDA5888_WriteReg(0x1eb,0x3104);
    RDA5888_WriteReg(0x150,0x0410);
#endif
}

// ----------------------------------------------------------------------------
// This function close the I2S interface.
//
// Params:
//  none.
//
// Return:
//  none.
// ----------------------------------------------------------------------------
void RDA5888_I2SClose(void)
{
#if (RDA5888_CHIP_VER != 7)
    RDA5888_WriteReg(0x1eb,0x1104); //13bit :1 open i2s,0 close i2s
    RDA5888_WriteReg(0x150,0x0410);
#endif
}

// ----------------------------------------------------------------------------
// This function power down the rda5888.
//
// Params:
//  none.
//
// Return:
//  none.
// ----------------------------------------------------------------------------
void RDA5888_PowerDown(void)
{
#if ((RDA5888_CHIP_VER == 6) || (RDA5888_CHIP_VER == 7))
    RDA5888_WriteReg(0x002, 0x10);
    RDA5888_WriteReg(0x002, 0x0); // power donw all
#endif
}

