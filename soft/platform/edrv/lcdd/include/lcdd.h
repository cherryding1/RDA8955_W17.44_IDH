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

#ifndef LCDD_H
#define LCDD_H

#include "lcdd_types.h"

// =============================================================================
// LCDD_REG_T
// -----------------------------------------------------------------------------
/// This types describes all functions for lcd driver.
// =============================================================================
typedef struct
{
    LCDD_ERR_T (*lcdd_Open)(VOID);
    LCDD_ERR_T (*lcdd_Close)(VOID);
    LCDD_ERR_T (*lcdd_SetContrast)(UINT32 contrast);
    //LCDD_ERR_T (*lcdd_SetBrightness)(UINT32 brightness);
    LCDD_ERR_T (*lcdd_SetStandbyMode)(BOOL standbyMode);
    LCDD_ERR_T (*lcdd_Sleep)(VOID);
    LCDD_ERR_T (*lcdd_PartialOn)(UINT16 vsa, UINT16 vea);
    LCDD_ERR_T (*lcdd_PartialOff)(VOID);
    LCDD_ERR_T (*lcdd_WakeUp)(VOID);
    LCDD_ERR_T (*lcdd_GetScreenInfo)(LCDD_SCREEN_INFO_T *screenInfo);
    LCDD_ERR_T (*lcdd_SetPixel16)(UINT16 x, UINT16 y, UINT16 pixelData);
    LCDD_ERR_T (*lcdd_FillRect16)(CONST LCDD_ROI_T *regionOfInterrest, UINT16 bgColor);
    LCDD_ERR_T (*lcdd_Blit16)(CONST LCDD_FBW_T *frameBufferWin, UINT16 startX, UINT16 startY);
    BOOL (*lcdd_Busy)(VOID);
    BOOL (*lcdd_SetDirRotation)(VOID);
    BOOL (*lcdd_SetDirDefault)(VOID);
    char *(*lcdd_GetStringId)(VOID);
    UINT16 (*lcdd_GetLcdId)(VOID);
    VOID (*lcdd_ChangeLcdSpiFreq)(BOOL high);
    VOID (*lcdd_GoudaBltHdl)(VOID);
#ifdef CHECK_LCD_STATUS
    BOOL (*lcdd_CheckLCDStatus)(VOID);
    LCDD_ERR_T (*lcdd_Reset_InIdle)(VOID);
#endif
} LCDD_REG_T;

// =============================================================================
// LCDD_CONFIG_T
// -----------------------------------------------------------------------------
/// The type LCDD_CONFIG_STRUCT_T is defined in lcdd_config.h
// =============================================================================
typedef struct LCDD_CONFIG_STRUCT_T LCDD_CONFIG_T;

// ============================================================================
// lcdd_LoadSuccess
// ----------------------------------------------------------------------------
// This function return whether lcd driver load success or not.
// ============================================================================
extern BOOL lcdd_LoadSuccess(VOID);

// ============================================================================
// lcdd_**_RegInit
// ----------------------------------------------------------------------------
// This function register the right lcd driver
//  When added new lcd dirver,must add lcdd_**_RegInit function on here
// ============================================================================
extern BOOL lcdd_au_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_dc2200_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_fd54124bv_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_kc20a_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_lgd4525b_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_lgdp4532_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_ms1321_RegInit(LCDD_REG_T *pLcdDrv);

extern BOOL lcdd_optrex_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_otm2201h_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_otm2201h8b_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_otm4001_RegInit(LCDD_REG_T *pLcdDrv);

extern BOOL lcdd_R61503v_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_R61503vr_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_spfd5408b_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_ssd1297_RegInit(LCDD_REG_T *pLcdDrv);

extern BOOL lcdd_hx8340_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_hx8340b_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_hx8345a_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_hx8347_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_hx8347b_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_hx8347d_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_hx8347g_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_hx8353d_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_hx8352b_RegInit(LCDD_REG_T *pLcdDrv);

extern BOOL lcdd_ili9163_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_ili9163b_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_ili9163c_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_ili9163ch_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_ili9163czgd_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_ili9163c_sub_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_ili9163cds_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_ili9225_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_ili9225b_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_ili9225c_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_ili9225b8b_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_ili9225br_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_ili9225g_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_ili9225g8b_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_ili9320_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_ili9325_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_ili9325d_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_ili9327_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_ili9328_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_ili9338b_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_ili9340_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_ili9340_old_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_ili9341_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_ili93418b_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_ili9342_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_ili9481_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_ili9481ds_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_ili9486_RegInit(LCDD_REG_T *pLcdDrv);

extern BOOL lcdd_s6b33bf_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_s6d0144_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_s6d0154_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_s6d02a1_RegInit(LCDD_REG_T *pLcdDrv);

extern BOOL lcdd_st7669v_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_st7735r_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_st7735_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_st7735rsmall_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_st7775r_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_st7735rh_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_st7781_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_st7787_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_st7735rzgd_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_st7567_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_st7735s_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_st7735_boe_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_st7789v_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_st7789s_RegInit(LCDD_REG_T *pLcdDrv);

extern BOOL lcdd_R61509v_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_r61509v8bit_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_nt35601_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_r615268b_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_rm68090_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_ili93418b_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_ili93258bit_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_gc9301ds_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_rm68130_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_gc9312_RegInit(LCDD_REG_T *pLcdDrv);

extern BOOL lcdd_gc93018b_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_nt35702_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_nv3029_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_gc9101_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_rm68116_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_nv3021_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_gc9301_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_gc9303_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_gc9102_RegInit(LCDD_REG_T *pLcdDrv);

extern BOOL lcdd_ili9325c_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_r61520_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_st7781r_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_gc9102d_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_nv3029e_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_gc9304_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_gc9304lane2_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_nv3029f_RegInit(LCDD_REG_T *pLcdDrv);

extern BOOL lcdd_ili9163csmall_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_st7735ssmall_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_st7735ssmall2_RegInit(LCDD_REG_T *pLcdDrv);
extern BOOL lcdd_gc9305_RegInit(LCDD_REG_T *pLcdDrv);
#endif
