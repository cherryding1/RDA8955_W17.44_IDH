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

#include "sxr_tls.h"
#include "sxs_idle.h"

#include "halp_rda_abb.h"
#include "halp_debug.h"
#include "hal_ispi.h"
#include "hal_rda_abb.h"
#include "hal_rda_audio.h"
#include "rfd_xcv.h"


//=============================================================================
// hal_AbbIspiRead
//-----------------------------------------------------------------------------
/// Read codec register on ISPI, the CS must already be activated
/// @param regIdx register to read
/// @return register value                 0 write 1 read
//=============================================================================
PROTECTED UINT32 hal_AbbIspiRead(UINT32 regIdx)
{
    return hal_IspiRegRead(HAL_ISPI_CS_ABB, regIdx);
}

//=============================================================================
// hal_AbbIspiWrite
//-----------------------------------------------------------------------------
/// Write codec register on ISPI, the CS must already be activated
/// @param regIdx register to read
/// @param value register value
//=============================================================================
PROTECTED VOID HAL_BOOT_FUNC hal_AbbIspiWrite(UINT32 regIdx, UINT32 value)
{
    hal_IspiRegWrite(HAL_ISPI_CS_ABB, regIdx, value);
}

PUBLIC BOOL hal_AbbRegRead(UINT32 addr, UINT32* pData)
{
#ifdef CHIP_DIE_8955
    hal_AbbIspiReadOutputSelect(addr);
#endif // CHIP_DIE_8955
    *pData = hal_IspiRegRead(HAL_ISPI_CS_ABB, addr);

    return TRUE;
}

PUBLIC BOOL hal_AbbRegWrite(UINT32 addr, UINT32 data)
{
    hal_IspiRegWrite(HAL_ISPI_CS_ABB, addr, data);
    return TRUE;
}


#ifdef SPI_REG_DEBUG
// ======================================================
// Debug: Read and Write Registers via SPI

#define ABB_SPI_REG_VALUE_LOCATION __attribute__((section(".abb_reg_value")))

#define SPI_REG_WRITE_FLAG (1<<31)
#define SPI_REG_READ_FLAG (1<<30)
#define SPI_REG_READ_ALL_FLAG (1<<29)
#define SPI_REG_DATA_MASK (0xffff)

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE || \
     CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
#define ABB_REG_NUM (0x36)
#elif (CHIP_ASIC_ID == CHIP_ASIC_ID_8809 || \
       CHIP_ASIC_ID == CHIP_ASIC_ID_8809P)
#define ABB_REG_NUM (0x50)
#elif (CHIP_ASIC_ID == CHIP_ASIC_ID_8810 || \
       CHIP_ASIC_ID == CHIP_ASIC_ID_8850 || \
       CHIP_ASIC_ID == CHIP_ASIC_ID_8820 || \
       CHIP_ASIC_ID == CHIP_ASIC_ID_8850E)
#define ABB_REG_NUM (0x80)
#else
#error "Unsupported CHIP_ASIC_ID"
#endif

UINT32 ABB_SPI_REG_VALUE_LOCATION g_abbSpiRegValue[ABB_REG_NUM];

PROTECTED VOID abb_SpiRegValueInit(VOID)
{
    for (int i=0; i<ABB_REG_NUM; i++)
    {
        g_abbSpiRegValue[i] = 0;
    }
}

#if defined( CHIP_DIE_8809E) || defined(CHIP_DIE_8955)
PUBLIC VOID abb_SpiRegCheck(VOID)
{
    BOOL readAll = (g_abbSpiRegValue[0] & SPI_REG_READ_ALL_FLAG)? TRUE : FALSE;

    g_abbSpiRegValue[0] &= ~SPI_REG_READ_ALL_FLAG;
    for (int i=0; i<ABB_REG_NUM; i++)
    {
        UINT16 idx;
        if ((i>=0x20)&&(i<=0x3f))
        {
            idx = i+0x100;  // [0x20, 0x3f] -> [0x120, 0x13f]
        }
        else if(i>0x3f)
        {
            idx = i-0x40 + 0x110; // [0x40, 0x4f] -> [0x110, 0x11f]
        }
        else
        {
            idx = i;
        }

        if (g_abbSpiRegValue[i] & SPI_REG_WRITE_FLAG)
        {
            hal_AbbIspiWrite(idx, g_abbSpiRegValue[i]&SPI_REG_DATA_MASK);
            g_abbSpiRegValue[i] &= ~SPI_REG_WRITE_FLAG;
        }
        if (readAll || (g_abbSpiRegValue[i] & SPI_REG_READ_FLAG))
        {
            UINT16 data = hal_AbbIspiRead(idx);
            g_abbSpiRegValue[i] &= ~(SPI_REG_READ_FLAG|SPI_REG_DATA_MASK);
            g_abbSpiRegValue[i] |= data&SPI_REG_DATA_MASK;
        }
    }
}

#else
PUBLIC VOID abb_SpiRegCheck(VOID)
{
    UINT32 idx;
    for (int i=0; i<ABB_REG_NUM; i++)
    {
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE || \
     CHIP_ASIC_ID == CHIP_ASIC_ID_8808 || \
     CHIP_ASIC_ID == CHIP_ASIC_ID_8809 || \
     CHIP_ASIC_ID == CHIP_ASIC_ID_8809P)
        idx = i;
#else // (CHIP_ASIC_ID == CHIP_ASIC_ID_8810)
        if (i < 0x10)
        {
            idx = i;
            // 0x00 ~ 0x0A
            if (idx > 0x0A)
                continue;
        }
        else if (i < 0x40)
        {
            idx = i - 0x10 + 0x40;
            // 0x40 ~ 0x6C, 0x70
            if (idx == 0x6D)
                idx = 0x70; // for Reset register
            else if (idx > 0x6D)
                continue;
        }
        else // if (i >= 0x40)
        {
            // 0x120 ~ 0x147
            idx = i - 0x40 + 0x120;
            if (idx > 0x147)
                continue;
        }
#endif
        if (g_abbSpiRegValue[i] & SPI_REG_WRITE_FLAG)
        {
            if (!hal_AbbRegWrite(idx, g_abbSpiRegValue[i]&SPI_REG_DATA_MASK))
            {
                continue;
            }
            g_abbSpiRegValue[i] &= ~SPI_REG_WRITE_FLAG;
        }
        if (g_abbSpiRegValue[i] & SPI_REG_READ_FLAG)
        {
            UINT32 data;
            if(!hal_AbbRegRead(idx, &data))
            {
                continue;
            }
            g_abbSpiRegValue[i] &= ~(SPI_REG_READ_FLAG|SPI_REG_DATA_MASK);
            g_abbSpiRegValue[i] |= data&SPI_REG_DATA_MASK;
        }
    }
}
#endif
PUBLIC VOID abb_RegisterSpiRegCheckFunc(VOID)
{
    static BOOL registerIdleHook = FALSE;
    if (!registerIdleHook)
    {
        registerIdleHook = TRUE;
        abb_SpiRegValueInit();
        sxs_RegisterDebugIdleHookFunc(&abb_SpiRegCheck);
    }
}
#endif // SPI_REG_DEBUG

