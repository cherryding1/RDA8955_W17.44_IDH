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

#include "lcdd_config.h"
#include "lcdd_private.h"
#include "hal_timers.h"
#include "hal_gouda.h"
#include "lcddp_debug.h"


#include "pmd_m.h"
#include "sxr_tls.h"

// To get config
#include "lcdd_tgt_params_gallite.h"
#include "lcdd.h"

PRIVATE LCDD_ERR_T ST7735S_Sleep(VOID);
PRIVATE LCDD_ERR_T ST7735S_WakeUp(VOID);

// =============================================================================
//
// -----------------------------------------------------------------------------
// =============================================================================

// =============================================================================
//  MACROS
// =============================================================================

#define   LCD_DataWrite_ST7735S_8( Data )    \
    while(hal_GoudaWriteData(Data)  != HAL_ERR_NO);

#define   LCD_CtrlWrite_ST7735S_8( Cmd )     \
    while(hal_GoudaWriteCmd(Cmd)    != HAL_ERR_NO);



#define   LCD_CtrlData_ST7735S_8( Cmd, Data)             \
{                                                        \
    LCD_CtrlWrite_ST7735S_8(Cmd);                         \
    LCD_DataWrite_ST7735S_8(Data);                        \
}


//low 8bit

#define  LCD_DataWrite_ST7735S_16( Data)                      \
{                                                              \
        LCD_DataWrite_ST7735S_8 (   ((Data & 0xFF00) >> 8) );   \
        LCD_DataWrite_ST7735S_8 (   (Data & 0x00FF) );              \
}

#define  LCD_CtrlWrite_ST7735S_16( Cmd)                         \
{                                                               \
        LCD_CtrlWrite_ST7735S_8 (  ((Cmd & 0xFF00) >> 8) );      \
        LCD_CtrlWrite_ST7735S_8 (  (Cmd & 0x00FF) );              \
}


#define   LCD_CtrlData_ST7735S_16( Cmd, Data)               \
{                                                           \
    LCD_CtrlWrite_ST7735S_16(Cmd);                          \
    LCD_DataWrite_ST7735S_16(Data);                         \
}

#define MS_WAITING      *  HAL_TICK1S / 1000

#define LCD_Delayms(x)  sxr_Sleep(x MS_WAITING)


#define LCD_Reset() hal_GoudaInitResetPin()



#define ST7735S_BUILD_CMD_WR_CMD(c,i,r) do{c[i].isData=FALSE; c[i].value=r;}while(0)
#define ST7735S_BUILD_CMD_WR_DAT(c,i,d) do{c[i].isData=TRUE; c[i].value=d;}while(0)
#define ST7735S_BUILD_CMD_WR_REG(c,i,r,d) do{ST7735S_BUILD_CMD_WR_CMD(c,i,r); ST7735S_BUILD_CMD_WR_DAT(c,i+1,d);}while(0)

#define ST7735S_TIME_MUTEX_RETRY 50

// =============================================================================
// Screen properties
// =============================================================================

// Number of actual pixels in the display width
// Number of pixels in the display height   must modify

#ifdef LCDSIZE_160_128
#define ST7735S_LCD_WIDTH		160 
#define ST7735S_LCD_HEIGHT		128 
#else
#define ST7735S_LCD_WIDTH       128
#define ST7735S_LCD_HEIGHT      160
#endif



// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

// LCDD screen config.               hal_gouda.h
PRIVATE CONST LCDD_CONFIG_T g_tgtLcddCfg = TGT_LCDD_CONFIG;

// true  when video player working with full screen
PRIVATE BOOL g_lcddRotate = FALSE;

// wheter ST7735S_GoudaBlitHandler() has to close ovl layer 0
PRIVATE BOOL g_lcddAutoCloseLayer = FALSE;

// Sleep status of the LCD( TRUE :sleep  FALSE: normal )
PRIVATE BOOL g_lcddInSleep = FALSE;

// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// ST7735S_GoudaBlitHandler
// -----------------------------------------------------------------------------
/// This function frees the lock to access the screen. It is set as the user
/// handler called by the DMA driver at the end of the writings on the screen.
// =============================================================================
PRIVATE VOID ST7735S_GoudaBlitHandler(VOID)
{
    lcdd_MutexFree();
    if(g_lcddAutoCloseLayer)
    {
        hal_GoudaOvlLayerClose(HAL_GOUDA_OVL_LAYER_ID0);
        g_lcddAutoCloseLayer = FALSE;
    }
}


// =============================================================================
// ST7735S_Init
// -----------------------------------------------------------------------------
/// This function initializes LCD registers after powering on or waking up.
// =============================================================================
PRIVATE VOID ST7735S_Init(VOID)
{


#if defined(LCD_ST7735S_HLT_HSD)
////合力泰  翰彩 128 160 1.77 for for  盟博S185（F700   20140717

    LCD_CtrlWrite_ST7735S_8(0x11); //Sleep out
    LCD_Delayms(120);               // Must Delay At Least 100 ms
    //------------------------------------ST7735S Frame rate-----------------------------------------//
    LCD_CtrlWrite_ST7735S_8(0xB1); //Frame rate 80Hz
    LCD_DataWrite_ST7735S_8(0x08);
    LCD_DataWrite_ST7735S_8(0x3b);
    LCD_DataWrite_ST7735S_8(0x3b);
    LCD_CtrlWrite_ST7735S_8(0xB2); //Frame rate 80Hz
    LCD_DataWrite_ST7735S_8(0x08);
    LCD_DataWrite_ST7735S_8(0x3b);
    LCD_DataWrite_ST7735S_8(0x3b);
    LCD_CtrlWrite_ST7735S_8(0xB3); //Frame rate 80Hz
    LCD_DataWrite_ST7735S_8(0x08);
    LCD_DataWrite_ST7735S_8(0x3b);
    LCD_DataWrite_ST7735S_8(0x3b);
    LCD_DataWrite_ST7735S_8(0x08);
    LCD_DataWrite_ST7735S_8(0x3b);
    LCD_DataWrite_ST7735S_8(0x3b);
    //------------------------------------End ST7735S Frame rate-----------------------------------------//
    LCD_CtrlWrite_ST7735S_8(0xB4); //Dot inversion
    LCD_DataWrite_ST7735S_8(0x03);

    //------------------------------------ST7735S Power Sequence-----------------------------------------//
    LCD_CtrlWrite_ST7735S_8(0xC0);
    LCD_DataWrite_ST7735S_8(0x62);
    LCD_DataWrite_ST7735S_8(0x02);
    LCD_DataWrite_ST7735S_8(0x04);
    LCD_CtrlWrite_ST7735S_8(0xC1);
    LCD_DataWrite_ST7735S_8(0xC0);
    LCD_CtrlWrite_ST7735S_8(0xC2);
    LCD_DataWrite_ST7735S_8(0x0D);
    LCD_DataWrite_ST7735S_8(0x00);
    LCD_CtrlWrite_ST7735S_8(0xC3);
    LCD_DataWrite_ST7735S_8(0x8D);
    LCD_DataWrite_ST7735S_8(0x6A);
    LCD_CtrlWrite_ST7735S_8(0xC4);
    LCD_DataWrite_ST7735S_8(0x8D);
    LCD_DataWrite_ST7735S_8(0xEE);
    //---------------------------------End ST7735S Power Sequence-------------------------------------//
    LCD_CtrlWrite_ST7735S_8(0xC5); //VCOM
    LCD_DataWrite_ST7735S_8(0x12);//07 12
    LCD_CtrlWrite_ST7735S_8(0x36); //MX, MY, RGB mode
    LCD_DataWrite_ST7735S_8(0xC8);


    //------------------------------------ST7735S Gamma Sequence-----------------------------------------//
    LCD_CtrlWrite_ST7735S_8(0XE0);
    LCD_DataWrite_ST7735S_8(0x03);//10
    LCD_DataWrite_ST7735S_8(0x18);//1B 18
    LCD_DataWrite_ST7735S_8(0x12);
    LCD_DataWrite_ST7735S_8(0x11);
    LCD_DataWrite_ST7735S_8(0x3F);
    LCD_DataWrite_ST7735S_8(0x3A);
    LCD_DataWrite_ST7735S_8(0x32);
    LCD_DataWrite_ST7735S_8(0x34);
    LCD_DataWrite_ST7735S_8(0x2F);
    LCD_DataWrite_ST7735S_8(0x2B);
    LCD_DataWrite_ST7735S_8(0x30);
    LCD_DataWrite_ST7735S_8(0x3A);
    LCD_DataWrite_ST7735S_8(0x00);
    LCD_DataWrite_ST7735S_8(0x01);
    LCD_DataWrite_ST7735S_8(0x02);
    LCD_DataWrite_ST7735S_8(0x05);

    LCD_CtrlWrite_ST7735S_8(0XE1);
    LCD_DataWrite_ST7735S_8(0x03);//10
    LCD_DataWrite_ST7735S_8(0x18);//1B 18
    LCD_DataWrite_ST7735S_8(0x12);
    LCD_DataWrite_ST7735S_8(0x11);
    LCD_DataWrite_ST7735S_8(0x32);
    LCD_DataWrite_ST7735S_8(0x2F);
    LCD_DataWrite_ST7735S_8(0x2A);
    LCD_DataWrite_ST7735S_8(0x2F);
    LCD_DataWrite_ST7735S_8(0x2E);
    LCD_DataWrite_ST7735S_8(0x2C);
    LCD_DataWrite_ST7735S_8(0x35);
    LCD_DataWrite_ST7735S_8(0x3F);
    LCD_DataWrite_ST7735S_8(0x00);
    LCD_DataWrite_ST7735S_8(0x00);
    LCD_DataWrite_ST7735S_8(0x01);
    LCD_DataWrite_ST7735S_8(0x05);
    //------------------------------------End ST7735S Gamma Sequence-----------------------------------------//
    LCD_CtrlWrite_ST7735S_8(0xFC); //
    LCD_DataWrite_ST7735S_8(0x8C);

    LCD_CtrlWrite_ST7735S_8(0x3A); //65k mode
    LCD_DataWrite_ST7735S_8(0x05);
    LCD_CtrlWrite_ST7735S_8(0x29); //Display on
    LCD_Delayms(100);

#elif defined(LCD_ST7735S_ZGD_SC)

    LCD_CtrlWrite_ST7735S_8(0x11); //Sleep out
    sxr_Sleep(120 MS_WAITING);
    //LCD_Delayms(120);             // Must Delay At Least 100 ms
    //------------------------------------ST7735S Frame rate-----------------------------------------//

    LCD_CtrlWrite_ST7735S_8(0xB1);
    LCD_DataWrite_ST7735S_8(0x05); //0x01
    LCD_DataWrite_ST7735S_8(0x08); //0x08
    LCD_DataWrite_ST7735S_8(0x05); //0x05

    LCD_CtrlWrite_ST7735S_8(0xB2);
    LCD_DataWrite_ST7735S_8(0x05); //0x01
    LCD_DataWrite_ST7735S_8(0x08); //0x2c
    LCD_DataWrite_ST7735S_8(0x05); //0x2d


    LCD_CtrlWrite_ST7735S_8(0xB3);
    LCD_DataWrite_ST7735S_8(0x05); //0x01
    LCD_DataWrite_ST7735S_8(0x08); //0x2c
    LCD_DataWrite_ST7735S_8(0x05); //0x2d

    LCD_DataWrite_ST7735S_8(0x00); //0x01
    LCD_DataWrite_ST7735S_8(0x08); //0x2c
    LCD_DataWrite_ST7735S_8(0x05); //0x2d

    //--------End frame Rate--------//
    LCD_CtrlWrite_ST7735S_8(0xB4);    //column inversion
    LCD_DataWrite_ST7735S_8(0x00); //0x03

    //LCD_CtrlWrite_ST7735S_8(0xB6);    //column inversion
    //LCD_DataWrite_ST7735S_8(0xB4);
    //LCD_DataWrite_ST7735S_8(0xF0);

    LCD_CtrlWrite_ST7735S_8(0xC0);
    LCD_DataWrite_ST7735S_8(0x08);//a8
    LCD_DataWrite_ST7735S_8(0x02);//02  08
    LCD_DataWrite_ST7735S_8(0x84);//

    LCD_CtrlWrite_ST7735S_8(0xC1);
    LCD_DataWrite_ST7735S_8(0xC0); //c5

    LCD_CtrlWrite_ST7735S_8(0xC2);
    LCD_DataWrite_ST7735S_8(0x0D); //0a
    LCD_DataWrite_ST7735S_8(0x00);

    LCD_CtrlWrite_ST7735S_8(0xC3);
    LCD_DataWrite_ST7735S_8(0x8D); //8A
    LCD_DataWrite_ST7735S_8(0x2A);

    LCD_CtrlWrite_ST7735S_8(0xC4);
    LCD_DataWrite_ST7735S_8(0x8D); //8A
    LCD_DataWrite_ST7735S_8(0xEE);

    LCD_CtrlWrite_ST7735S_8(0xC5); //VCOM,若有水波纹,可适当调这个值
    LCD_DataWrite_ST7735S_8(0x19); //0A//0x14//0x18//0x0a

    LCD_CtrlWrite_ST7735S_8(0x36); //MX,MY,RGB mode
    LCD_DataWrite_ST7735S_8(0xc0); //c0  //my mx ml MV,rgb,000; =1,=MH=MX=MY=ML=0 and RGB filter panel
    //---------gamma--------------//

    LCD_CtrlWrite_ST7735S_8(0xE0); //Enable Gamma bit
    LCD_DataWrite_ST7735S_8(0x07);
    LCD_DataWrite_ST7735S_8(0x18);//0x1A     0x1c
    LCD_DataWrite_ST7735S_8(0x0C);//p1
    LCD_DataWrite_ST7735S_8(0x15);//p2
    LCD_DataWrite_ST7735S_8(0x2E);//p3
    LCD_DataWrite_ST7735S_8(0x2A);//p4
    LCD_DataWrite_ST7735S_8(0x23);//p5
    LCD_DataWrite_ST7735S_8(0x28);//p6
    LCD_DataWrite_ST7735S_8(0x28);//p7
    LCD_DataWrite_ST7735S_8(0x28);//p8
    LCD_DataWrite_ST7735S_8(0x2E);//p9
    LCD_DataWrite_ST7735S_8(0x39);//p10
    LCD_DataWrite_ST7735S_8(0x00);//p11
    LCD_DataWrite_ST7735S_8(0x03);//p12
    LCD_DataWrite_ST7735S_8(0x02);//p13
    LCD_DataWrite_ST7735S_8(0x10);//p14

    LCD_CtrlWrite_ST7735S_8(0xE1);
    LCD_DataWrite_ST7735S_8(0x06);//p1
    LCD_DataWrite_ST7735S_8(0x23);//p2
    LCD_DataWrite_ST7735S_8(0x0D);//p3
    LCD_DataWrite_ST7735S_8(0x17);//p4
    LCD_DataWrite_ST7735S_8(0x35);//p5
    LCD_DataWrite_ST7735S_8(0x30);//p6
    LCD_DataWrite_ST7735S_8(0x2A);//p7
    LCD_DataWrite_ST7735S_8(0x2D);//p8
    LCD_DataWrite_ST7735S_8(0x2C);//p9
    LCD_DataWrite_ST7735S_8(0x29);//p10
    LCD_DataWrite_ST7735S_8(0x31);//p11
    LCD_DataWrite_ST7735S_8(0x3B);//p12
    LCD_DataWrite_ST7735S_8(0x00);//p13
    LCD_DataWrite_ST7735S_8(0x02);//p14
    LCD_DataWrite_ST7735S_8(0x03);//p15
    LCD_DataWrite_ST7735S_8(0x12);//p15


    //LCD_CtrlWrite_ST7735S_8(0xF0); //enable test command
    //LCD_DataWrite_ST7735S_8(0x01);

    //LCD_CtrlWrite_ST7735S_8(0xF6); //disable ram power save mode
    // LCD_DataWrite_ST7735S_8(0x00);

    //LCD_Delayms(120);

    LCD_CtrlWrite_ST7735S_8(0x3a);
    LCD_DataWrite_ST7735S_8(0x05);

    //DISPLAY ON
    LCD_CtrlWrite_ST7735S_8(0x29);

    sxr_Sleep(120 MS_WAITING);
    //LCD_Delayms(120);


#elif defined(LCD_ST7735S_TYF_CMO)

    //LCD_Reset();//reset lcd
    //LCD_Delayms(120);

    //SCI_TRACE_LOW("LCD_Init_ST7735S ...");

    //LCD_Delayms(20);
    LCD_CtrlWrite_ST7735S_8(0x11); //Sleep out
    LCD_Delayms(120);
    LCD_CtrlWrite_ST7735S_8(0xB1);
    LCD_DataWrite_ST7735S_8(0x01);
    LCD_DataWrite_ST7735S_8(0x08);
    LCD_DataWrite_ST7735S_8(0x05);
    LCD_CtrlWrite_ST7735S_8(0xB2);
    LCD_DataWrite_ST7735S_8(0x01);
    LCD_DataWrite_ST7735S_8(0x2C);
    LCD_DataWrite_ST7735S_8(0x2D);
    LCD_CtrlWrite_ST7735S_8(0xB3);
    LCD_DataWrite_ST7735S_8(0x01);
    LCD_DataWrite_ST7735S_8(0x2C);
    LCD_DataWrite_ST7735S_8(0x2D);
    LCD_DataWrite_ST7735S_8(0x01);
    LCD_DataWrite_ST7735S_8(0x2C);
    LCD_DataWrite_ST7735S_8(0x2D);
    LCD_CtrlWrite_ST7735S_8(0xB4); //Column inversion
    LCD_DataWrite_ST7735S_8(0x00);
    LCD_CtrlWrite_ST7735S_8(0xC0);
    LCD_DataWrite_ST7735S_8(0xA2);
    LCD_DataWrite_ST7735S_8(0x02);
    LCD_DataWrite_ST7735S_8(0x84);
    LCD_CtrlWrite_ST7735S_8(0xC1);
    LCD_DataWrite_ST7735S_8(0xCF);
    LCD_CtrlWrite_ST7735S_8(0xC2);
    LCD_DataWrite_ST7735S_8(0x0D);
    LCD_DataWrite_ST7735S_8(0x00);
    LCD_CtrlWrite_ST7735S_8(0xC3);
    LCD_DataWrite_ST7735S_8(0x8A);
    LCD_DataWrite_ST7735S_8(0x2A);
    LCD_CtrlWrite_ST7735S_8(0xC4);
    LCD_DataWrite_ST7735S_8(0x8A);
    LCD_DataWrite_ST7735S_8(0xEE);
    LCD_CtrlWrite_ST7735S_8(0xC5); //VCOM
    LCD_DataWrite_ST7735S_8(0x2c);
    LCD_CtrlWrite_ST7735S_8(0x36); //MX, MY, RGB mode
    LCD_DataWrite_ST7735S_8(0xC8);
    LCD_CtrlWrite_ST7735S_8(0xe0);
    LCD_DataWrite_ST7735S_8(0x05);
    LCD_DataWrite_ST7735S_8(0x11);
    LCD_DataWrite_ST7735S_8(0x0e);
    LCD_DataWrite_ST7735S_8(0x19);
    LCD_DataWrite_ST7735S_8(0x3a);
    LCD_DataWrite_ST7735S_8(0x33);
    LCD_DataWrite_ST7735S_8(0x29);
    LCD_DataWrite_ST7735S_8(0x2a);
    LCD_DataWrite_ST7735S_8(0x26);
    LCD_DataWrite_ST7735S_8(0x21);
    LCD_DataWrite_ST7735S_8(0x26);
    LCD_DataWrite_ST7735S_8(0x3b);
    LCD_DataWrite_ST7735S_8(0x00);
    LCD_DataWrite_ST7735S_8(0x01);
    LCD_DataWrite_ST7735S_8(0x03);
    LCD_DataWrite_ST7735S_8(0x10);
    LCD_CtrlWrite_ST7735S_8(0xe1);
    LCD_DataWrite_ST7735S_8(0x05);
    LCD_DataWrite_ST7735S_8(0x11);
    LCD_DataWrite_ST7735S_8(0x0e);
    LCD_DataWrite_ST7735S_8(0x19);
    LCD_DataWrite_ST7735S_8(0x36);
    LCD_DataWrite_ST7735S_8(0x30);
    LCD_DataWrite_ST7735S_8(0x2b);
    LCD_DataWrite_ST7735S_8(0x30);
    LCD_DataWrite_ST7735S_8(0x31);
    LCD_DataWrite_ST7735S_8(0x31);
    LCD_DataWrite_ST7735S_8(0x3b);
    LCD_DataWrite_ST7735S_8(0x3f);
    LCD_DataWrite_ST7735S_8(0x00);
    LCD_DataWrite_ST7735S_8(0x02);
    LCD_DataWrite_ST7735S_8(0x03);
    LCD_DataWrite_ST7735S_8(0x10);
    LCD_CtrlWrite_ST7735S_8(0x3A); //65k mode
    LCD_DataWrite_ST7735S_8(0x05);
    LCD_CtrlWrite_ST7735S_8(0x29); //Display on

    LCD_Delayms(120);
    //读取 09h ，得到各种控制信息
    //ST7735S_GetStatus();
    //LCD_CtrlWrite_ST7735S_8(0x2c);


    //ST7735S_InitRAM();
    //LCD_Delayms(80);

    //DISPLAY ON
    //LCD_CtrlWrite_ST7735S_8(0x29);
    //LCD_Delayms(100);

    //读取 09h ，得到各种控制信息
    //ST7735S_GetStatus();
    //
#elif defined(LCD_ST7735S_SL_BOE)
    //LCD_Reset();//reset lcd
    //LCD_Delayms(120);

    LCD_CtrlWrite_ST7735S_8(0x11); //Sleep out
    LCD_Delayms (120); //Delay 120ms
    //------------------------------------ST7735S Frame Rate-----------------------------------------//
    LCD_CtrlWrite_ST7735S_8(0xB1);
    LCD_DataWrite_ST7735S_8(0x01);
    LCD_DataWrite_ST7735S_8(0x08);
    LCD_DataWrite_ST7735S_8(0x05);
    LCD_CtrlWrite_ST7735S_8(0xB2);
    LCD_DataWrite_ST7735S_8(0x05);
    LCD_DataWrite_ST7735S_8(0x3C);
    LCD_DataWrite_ST7735S_8(0x3C);
    LCD_CtrlWrite_ST7735S_8(0xB3);
    LCD_DataWrite_ST7735S_8(0x05);
    LCD_DataWrite_ST7735S_8(0x3C);
    LCD_DataWrite_ST7735S_8(0x3C);
    LCD_DataWrite_ST7735S_8(0x05);
    LCD_DataWrite_ST7735S_8(0x3C);
    LCD_DataWrite_ST7735S_8(0x3C);
    //------------------------------------End ST7735S Frame Rate-----------------------------------------//
    LCD_CtrlWrite_ST7735S_8(0xB4); //Dot inversion
    LCD_DataWrite_ST7735S_8(0x03);

    //------------------------------------ST7735S Power Sequence-----------------------------------------//
    LCD_CtrlWrite_ST7735S_8(0xC0);
    LCD_DataWrite_ST7735S_8(0x2E);
    LCD_DataWrite_ST7735S_8(0x06);
    LCD_DataWrite_ST7735S_8(0x04);
    LCD_CtrlWrite_ST7735S_8(0xC1);
    LCD_DataWrite_ST7735S_8(0XC0);
    LCD_CtrlWrite_ST7735S_8(0xC2);
    LCD_DataWrite_ST7735S_8(0x0D);
    LCD_DataWrite_ST7735S_8(0x00);
    LCD_CtrlWrite_ST7735S_8(0xC3);
    LCD_DataWrite_ST7735S_8(0x8D);
    LCD_DataWrite_ST7735S_8(0xEA);
    LCD_CtrlWrite_ST7735S_8(0xC4);
    LCD_DataWrite_ST7735S_8(0x8D);
    LCD_DataWrite_ST7735S_8(0xEE);
    //---------------------------------End ST7735S Power Sequence-------------------------------------//
    LCD_CtrlWrite_ST7735S_8(0xC5); //VCOM
    LCD_DataWrite_ST7735S_8(0x02);
    LCD_CtrlWrite_ST7735S_8(0x36); //MX, MY, RGB mode
    LCD_DataWrite_ST7735S_8(0xC0);//c0
    //------------------------------------ST7735S Gamma Sequence-----------------------------------------//
    LCD_CtrlWrite_ST7735S_8(0xE0);
    LCD_DataWrite_ST7735S_8(0x03);
    LCD_DataWrite_ST7735S_8(0x1A);
    LCD_DataWrite_ST7735S_8(0x06);
    LCD_DataWrite_ST7735S_8(0x0B);
    LCD_DataWrite_ST7735S_8(0x35);
    LCD_DataWrite_ST7735S_8(0x35);
    LCD_DataWrite_ST7735S_8(0x30);
    LCD_DataWrite_ST7735S_8(0x33);
    LCD_DataWrite_ST7735S_8(0x31);
    LCD_DataWrite_ST7735S_8(0x2E);
    LCD_DataWrite_ST7735S_8(0x34);
    LCD_DataWrite_ST7735S_8(0x3E);
    LCD_DataWrite_ST7735S_8(0x00);
    LCD_DataWrite_ST7735S_8(0x00);
    LCD_DataWrite_ST7735S_8(0x01);
    LCD_DataWrite_ST7735S_8(0x03);

    LCD_CtrlWrite_ST7735S_8(0xE1);
    LCD_DataWrite_ST7735S_8(0x03);
    LCD_DataWrite_ST7735S_8(0x19);
    LCD_DataWrite_ST7735S_8(0x06);
    LCD_DataWrite_ST7735S_8(0x0B);
    LCD_DataWrite_ST7735S_8(0x35);
    LCD_DataWrite_ST7735S_8(0x34);
    LCD_DataWrite_ST7735S_8(0x2F);
    LCD_DataWrite_ST7735S_8(0x33);
    LCD_DataWrite_ST7735S_8(0x32);
    LCD_DataWrite_ST7735S_8(0x2E);
    LCD_DataWrite_ST7735S_8(0x35);
    LCD_DataWrite_ST7735S_8(0x3E);
    LCD_DataWrite_ST7735S_8(0x00);
    LCD_DataWrite_ST7735S_8(0x01);
    LCD_DataWrite_ST7735S_8(0x02);
    LCD_DataWrite_ST7735S_8(0x14);
    //------------------------------------End ST7735S Gamma Sequence-----------------------------------------//
    LCD_CtrlWrite_ST7735S_8(0x3A); //65k mode
    LCD_DataWrite_ST7735S_8(0x05);
    LCD_CtrlWrite_ST7735S_8(0x21);
    LCD_CtrlWrite_ST7735S_8(0x29); //Display on
#elif defined(LCD_ST7735S_ZGD_BOE)
    LCD_CtrlWrite_ST7735S_8(0x11); //Sleep out
    LCD_Delayms(120);

    LCD_CtrlWrite_ST7735S_8(0xB1);
    LCD_DataWrite_ST7735S_8(0x05);
    LCD_DataWrite_ST7735S_8(0x3c);
    LCD_DataWrite_ST7735S_8(0x3c);

    LCD_CtrlWrite_ST7735S_8(0xB2);
    LCD_DataWrite_ST7735S_8(0x05);
    LCD_DataWrite_ST7735S_8(0x3c);
    LCD_DataWrite_ST7735S_8(0x3c);

    LCD_CtrlWrite_ST7735S_8(0xB3);
    LCD_DataWrite_ST7735S_8(0x05);
    LCD_DataWrite_ST7735S_8(0x3c);
    LCD_DataWrite_ST7735S_8(0x3c);
    LCD_DataWrite_ST7735S_8(0x05);
    LCD_DataWrite_ST7735S_8(0x3c);
    LCD_DataWrite_ST7735S_8(0x3c);
    LCD_CtrlWrite_ST7735S_8(0xB4);
    LCD_DataWrite_ST7735S_8(0x07);

    LCD_CtrlWrite_ST7735S_8(0xB6);
    LCD_DataWrite_ST7735S_8(0xB4);
    LCD_DataWrite_ST7735S_8(0xF0);
    LCD_CtrlWrite_ST7735S_8(0xC0);
    LCD_DataWrite_ST7735S_8(0xb4);
    LCD_DataWrite_ST7735S_8(0x14);
    LCD_DataWrite_ST7735S_8(0x04);
    LCD_CtrlWrite_ST7735S_8(0xC1);
    LCD_DataWrite_ST7735S_8(0xc0);
    LCD_CtrlWrite_ST7735S_8(0xC2);
    LCD_DataWrite_ST7735S_8(0x0a);
    LCD_DataWrite_ST7735S_8(0x00);

    LCD_CtrlWrite_ST7735S_8(0xC3);
    LCD_DataWrite_ST7735S_8(0x8a);
    LCD_DataWrite_ST7735S_8(0x2a);
    LCD_CtrlWrite_ST7735S_8(0xC4);
    LCD_DataWrite_ST7735S_8(0x8a);
    LCD_DataWrite_ST7735S_8(0xaa);
    LCD_CtrlWrite_ST7735S_8(0xC5);
    LCD_DataWrite_ST7735S_8(0x00);

    LCD_CtrlWrite_ST7735S_8(0xE0);
    LCD_DataWrite_ST7735S_8(0x03);
    LCD_DataWrite_ST7735S_8(0x22);
    LCD_DataWrite_ST7735S_8(0x0a);
    LCD_DataWrite_ST7735S_8(0x12);
    LCD_DataWrite_ST7735S_8(0x3a);
    LCD_DataWrite_ST7735S_8(0x35);
    LCD_DataWrite_ST7735S_8(0x2e);
    LCD_DataWrite_ST7735S_8(0x31);
    LCD_DataWrite_ST7735S_8(0x30);
    LCD_DataWrite_ST7735S_8(0x2d);
    LCD_DataWrite_ST7735S_8(0x34);
    LCD_DataWrite_ST7735S_8(0x3d);
    LCD_DataWrite_ST7735S_8(0x00);
    LCD_DataWrite_ST7735S_8(0x00);
    LCD_DataWrite_ST7735S_8(0x01);
    LCD_DataWrite_ST7735S_8(0x03);

    LCD_CtrlWrite_ST7735S_8(0xE1);
    LCD_DataWrite_ST7735S_8(0x03);
    LCD_DataWrite_ST7735S_8(0x20);
    LCD_DataWrite_ST7735S_8(0x0a);
    LCD_DataWrite_ST7735S_8(0x12);
    LCD_DataWrite_ST7735S_8(0x2e);
    LCD_DataWrite_ST7735S_8(0x2a);
    LCD_DataWrite_ST7735S_8(0x26);
    LCD_DataWrite_ST7735S_8(0x2b);
    LCD_DataWrite_ST7735S_8(0x2b);
    LCD_DataWrite_ST7735S_8(0x2a);
    LCD_DataWrite_ST7735S_8(0x32);
    LCD_DataWrite_ST7735S_8(0x3d);
    LCD_DataWrite_ST7735S_8(0x00);
    LCD_DataWrite_ST7735S_8(0x01);
    LCD_DataWrite_ST7735S_8(0x02);
    LCD_DataWrite_ST7735S_8(0x04);

    LCD_CtrlWrite_ST7735S_8(0x36);
    LCD_DataWrite_ST7735S_8(0xC0);
    LCD_CtrlWrite_ST7735S_8(0x2A);
    LCD_DataWrite_ST7735S_8(0x00);
    LCD_DataWrite_ST7735S_8(0x00);
    LCD_DataWrite_ST7735S_8(0x00);
    LCD_DataWrite_ST7735S_8(0x7F);

    LCD_CtrlWrite_ST7735S_8(0x2B);
    LCD_DataWrite_ST7735S_8(0x00);
    LCD_DataWrite_ST7735S_8(0x00);
    LCD_DataWrite_ST7735S_8(0x00);
    LCD_DataWrite_ST7735S_8(0x9F);
    LCD_CtrlWrite_ST7735S_8(0x21);   //for 0012  boe1.77
    LCD_CtrlWrite_ST7735S_8(0x3A);
    LCD_DataWrite_ST7735S_8(0x05);
    LCD_CtrlWrite_ST7735S_8(0x29);
    LCD_Delayms(120);
#else
    LCD_Reset();//reset lcd
    LCD_Delayms(100);

    //SCI_TRACE_LOW("LCD_Init_ST7735S ...");
    //LCD_Delayms(20);
    LCD_CtrlWrite_ST7735S_8(0x11); //Sleep out
    LCD_Delayms(100);
    LCD_CtrlWrite_ST7735S_8(0xB1);
    LCD_DataWrite_ST7735S_8(0x01);
    LCD_DataWrite_ST7735S_8(0x08);
    LCD_DataWrite_ST7735S_8(0x05);
    LCD_CtrlWrite_ST7735S_8(0xB2);
    LCD_DataWrite_ST7735S_8(0x01);
    LCD_DataWrite_ST7735S_8(0x2C);
    LCD_DataWrite_ST7735S_8(0x2D);
    LCD_CtrlWrite_ST7735S_8(0xB3);
    LCD_DataWrite_ST7735S_8(0x01);
    LCD_DataWrite_ST7735S_8(0x2C);
    LCD_DataWrite_ST7735S_8(0x2D);
    LCD_DataWrite_ST7735S_8(0x01);
    LCD_DataWrite_ST7735S_8(0x2C);
    LCD_DataWrite_ST7735S_8(0x2D);
    LCD_CtrlWrite_ST7735S_8(0xB4); //Column inversion
    LCD_DataWrite_ST7735S_8(0x00);
    LCD_CtrlWrite_ST7735S_8(0xC0);
    LCD_DataWrite_ST7735S_8(0xA2);
    LCD_DataWrite_ST7735S_8(0x02);
    LCD_DataWrite_ST7735S_8(0x84);
    LCD_CtrlWrite_ST7735S_8(0xC1);
    LCD_DataWrite_ST7735S_8(0xCF);
    LCD_CtrlWrite_ST7735S_8(0xC2);
    LCD_DataWrite_ST7735S_8(0x0D);
    LCD_DataWrite_ST7735S_8(0x00);
    LCD_CtrlWrite_ST7735S_8(0xC3);
    LCD_DataWrite_ST7735S_8(0x8A);
    LCD_DataWrite_ST7735S_8(0x2A);
    LCD_CtrlWrite_ST7735S_8(0xC4);
    LCD_DataWrite_ST7735S_8(0x8A);
    LCD_DataWrite_ST7735S_8(0xEE);
    LCD_CtrlWrite_ST7735S_8(0xC5); //VCOM
    LCD_DataWrite_ST7735S_8(0x2c);
#ifdef LCDSIZE_160_128
    LCD_CtrlWrite_ST7735S_8(0x36); //65k mode
    LCD_DataWrite_ST7735S_8(0xA8);
#else
    LCD_CtrlWrite_ST7735S_8(0x36); //MX, MY, RGB mode
    LCD_DataWrite_ST7735S_8(0xC8);
#endif
    LCD_CtrlWrite_ST7735S_8(0xe0);
    LCD_DataWrite_ST7735S_8(0x05);
    LCD_DataWrite_ST7735S_8(0x11);
    LCD_DataWrite_ST7735S_8(0x0e);
    LCD_DataWrite_ST7735S_8(0x19);
    LCD_DataWrite_ST7735S_8(0x3a);
    LCD_DataWrite_ST7735S_8(0x33);
    LCD_DataWrite_ST7735S_8(0x29);
    LCD_DataWrite_ST7735S_8(0x2a);
    LCD_DataWrite_ST7735S_8(0x26);
    LCD_DataWrite_ST7735S_8(0x21);
    LCD_DataWrite_ST7735S_8(0x26);
    LCD_DataWrite_ST7735S_8(0x3b);
    LCD_DataWrite_ST7735S_8(0x00);
    LCD_DataWrite_ST7735S_8(0x01);
    LCD_DataWrite_ST7735S_8(0x03);
    LCD_DataWrite_ST7735S_8(0x10);
    LCD_CtrlWrite_ST7735S_8(0xe1);
    LCD_DataWrite_ST7735S_8(0x05);
    LCD_DataWrite_ST7735S_8(0x11);
    LCD_DataWrite_ST7735S_8(0x0e);
    LCD_DataWrite_ST7735S_8(0x19);
    LCD_DataWrite_ST7735S_8(0x36);
    LCD_DataWrite_ST7735S_8(0x30);
    LCD_DataWrite_ST7735S_8(0x2b);
    LCD_DataWrite_ST7735S_8(0x30);
    LCD_DataWrite_ST7735S_8(0x31);
    LCD_DataWrite_ST7735S_8(0x31);
    LCD_DataWrite_ST7735S_8(0x3b);
    LCD_DataWrite_ST7735S_8(0x3f);
    LCD_DataWrite_ST7735S_8(0x00);
    LCD_DataWrite_ST7735S_8(0x02);
    LCD_DataWrite_ST7735S_8(0x03);
    LCD_DataWrite_ST7735S_8(0x10);
    LCD_CtrlWrite_ST7735S_8(0x3A); //65k mode
    LCD_DataWrite_ST7735S_8(0x05);
    LCD_CtrlWrite_ST7735S_8(0x29); //Display on

    LCD_Delayms(120);
    //读取 09h ，得到各种控制信息
    //ST7735S_GetStatus();
    //LCD_CtrlWrite_ST7735S_8(0x2c);


    //ST7735S_InitRAM();
    //LCD_Delayms(80);

    //DISPLAY ON
    //LCD_CtrlWrite_ST7735S_8(0x29);
    //LCD_Delayms(100);

    //读取 09h ，得到各种控制信息
    //ST7735S_GetStatus();
#endif

}


// ============================================================================
// ST7735S_Open
// ----------------------------------------------------------------------------
/// Open the LCDD driver.
/// It must be called before any call to any other function of this driver.
/// This function is to be called only once.
/// @return #LCDD_ERR_NO or #LCDD_ERR_DEVICE_NOT_FOUND.
//   include reset options
// ============================================================================
PRIVATE LCDD_ERR_T ST7735S_Open(VOID)
{
#ifdef USE_SPI_LCD
    hal_GoudaSerialOpen(LCDD_SPI_LINE_TYPE, LCDD_SPI_FREQ, &g_tgtLcddCfg.config, 0);
#else
    hal_GoudaOpen(&g_tgtLcddCfg.config, g_tgtLcddCfg.timings, 0);
#endif

    // Init code
    LCD_Delayms(50 );                         // Delay 50 ms

    ST7735S_Init();

    g_lcddInSleep = FALSE;

    return LCDD_ERR_NO;
}


// ============================================================================
// ST7735S_Close
// ----------------------------------------------------------------------------
/// Close the LCDD driver
/// No other functions of this driver should be called after a call to
/// #ST7735S_Close.
/// @return #LCDD_ERR_NO or #LCDD_ERR_DEVICE_NOT_FOUND.
// ============================================================================
PRIVATE LCDD_ERR_T ST7735S_Close(VOID)
{

    hal_GoudaClose();

    return LCDD_ERR_NO;
}


// ============================================================================
// ST7735S_SetContrast
// ----------------------------------------------------------------------------
/// Set the contrast of the 'main'LCD screen.
/// @param contrast Value to set the contrast to.
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED or
/// #LCDD_ERR_INVALID_PARAMETER.
// ============================================================================
PRIVATE LCDD_ERR_T ST7735S_SetContrast(UINT32 contrast)
{
    //#warning This function is not implemented yet
    return LCDD_ERR_NO;
}





// ============================================================================
// ST7735S_Sleep
// ----------------------------------------------------------------------------
/// Set the main LCD screen in sleep mode.
/// @return #LCDD_ERR_NO or #LCDD_ERR_NOT_OPENED
// ============================================================================

PRIVATE LCDD_ERR_T ST7735S_Sleep(VOID)
{
    while (0 == lcdd_MutexGet())
    {
        LCD_Delayms(ST7735S_TIME_MUTEX_RETRY);
        LCDD_TRACE(TSTDOUT, 0, "LCDD: Sleep while another LCD operation in progress. Sleep %d ticks",
                   ST7735S_TIME_MUTEX_RETRY);
    }

    if (g_lcddInSleep)
    {
        lcdd_MutexFree();
        return LCDD_ERR_NO;
    }


//enter sleep mode code must modify

    LCD_CtrlWrite_ST7735S_8(0x28);
    LCD_Delayms(30);
    LCD_CtrlWrite_ST7735S_8(0x10);
    LCD_Delayms(120);
//end

    LCDD_TRACE(TSTDOUT, 0, "ST7735S_Sleep: calling hal_GoudaClose");
    hal_GoudaClose();

    g_lcddInSleep = TRUE;

    lcdd_MutexFree();

    return LCDD_ERR_NO;
}


// ============================================================================
// ST7735S_PartialOn
// ----------------------------------------------------------------------------
/// Set the Partial mode of the LCD
/// @param vsa : Vertical Start Active
/// @param vea : Vertical End Active
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED
// ============================================================================
PRIVATE LCDD_ERR_T ST7735S_PartialOn(UINT16 vsa, UINT16 vea)
{
    return LCDD_ERR_NO;
}


// ============================================================================
// ST7735S_PartialOff
// ----------------------------------------------------------------------------
/// return to Normal Mode
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED
// ============================================================================
PRIVATE LCDD_ERR_T ST7735S_PartialOff(VOID)
{
    return LCDD_ERR_NO;
}

// ============================================================================
// ST7735S_WakeUp
// ----------------------------------------------------------------------------
/// Wake the main LCD screen out of sleep mode
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED
// ============================================================================
PRIVATE LCDD_ERR_T ST7735S_WakeUp(VOID)
{
    while (0 == lcdd_MutexGet())
    {
        LCD_Delayms(ST7735S_TIME_MUTEX_RETRY);
        LCDD_TRACE(TSTDOUT, 0, "LCDD: Wakeup while another LCD operation in progress. Sleep %d ticks",
                   ST7735S_TIME_MUTEX_RETRY);
    }

    if (!g_lcddInSleep)
    {
        lcdd_MutexFree();
        return LCDD_ERR_NO;
    }

    LCDD_TRACE(TSTDOUT, 0, "ST7735S_WakeUp: calling hal_GoudaOpen");

#ifdef USE_SPI_LCD
    hal_GoudaSerialOpen(LCDD_SPI_LINE_TYPE, LCDD_SPI_FREQ, &g_tgtLcddCfg.config, 0);
#else
    hal_GoudaOpen(&g_tgtLcddCfg.config, g_tgtLcddCfg.timings, 0);
#endif

//wake up start  Init code must modify
    LCD_Delayms(100 );                         // Delay 50 ms

//   LCD_CtrlWrite_ST7735S_8(0x11); //Exit Sleep
//   LCD_Reset();
//   LCD_Delayms(50 );
    ST7735S_Init();
//wake up end

    g_lcddInSleep = FALSE;

    lcdd_MutexFree();

    // Set a comfortable background color to avoid screen flash
    LCDD_FBW_T frameBufferWin;
    frameBufferWin.fb.buffer = NULL;
    frameBufferWin.fb.colorFormat = LCDD_COLOR_FORMAT_RGB_565; //LCDD_COLOR_FORMAT_RGB_565
    frameBufferWin.roi.x=0;
    frameBufferWin.roi.y=0;

    if (g_lcddRotate)
    {
        frameBufferWin.roi.width = ST7735S_LCD_HEIGHT;
        frameBufferWin.roi.height = ST7735S_LCD_WIDTH;
        frameBufferWin.fb.width = ST7735S_LCD_HEIGHT;
        frameBufferWin.fb.height = ST7735S_LCD_WIDTH;
    }
    else
    {
        frameBufferWin.roi.width = ST7735S_LCD_WIDTH;
        frameBufferWin.roi.height = ST7735S_LCD_HEIGHT;
        frameBufferWin.fb.width = ST7735S_LCD_WIDTH;
        frameBufferWin.fb.height = ST7735S_LCD_HEIGHT;
    }
    lcddp_Blit16(&frameBufferWin,frameBufferWin.roi.x,frameBufferWin.roi.y);

    return LCDD_ERR_NO;
}

// ============================================================================
// ST7735S_SetStandbyMode
// ----------------------------------------------------------------------------
/// Set the main LCD in standby mode or exit from it
/// @param standbyMode If \c TRUE, go in standby mode.
///                    If \c FALSE, cancel standby mode.
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED or
/// #LCDD_ERR_INVALID_PARAMETER.
// ============================================================================
PRIVATE LCDD_ERR_T ST7735S_SetStandbyMode(BOOL standbyMode)
{
    if (standbyMode)  // enter standby
    {
        ST7735S_Sleep();
    }
    else            // exit standby
    {
        ST7735S_WakeUp();
    }
    return LCDD_ERR_NO;
}



// ============================================================================
// ST7735S_GetScreenInfo
// ----------------------------------------------------------------------------
/// Get information about the main LCD device.
/// @param screenInfo Pointer to the structure where the information
/// obtained will be stored
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED or
/// #LCDD_ERR_INVALID_PARAMETER.
// ============================================================================
PRIVATE LCDD_ERR_T ST7735S_GetScreenInfo(LCDD_SCREEN_INFO_T* screenInfo)
{
    {
        screenInfo->width = ST7735S_LCD_WIDTH;
        screenInfo->height = ST7735S_LCD_HEIGHT;
        screenInfo->bitdepth = LCDD_COLOR_FORMAT_RGB_565;
        screenInfo->nReserved = 0;
        return LCDD_ERR_NO;
    }
}


// ============================================================================
// ST7735S_SetPixel16
// ----------------------------------------------------------------------------
/// Draw a 16-bit pixel a the specified position.
/// @param x X coordinate of the point to set.
/// @param y Y coordinate of the point to set.
/// @param pixelData 16-bit pixel data to draw.
/// @return #LCDD_ERR_NO, #LCDD_ERR_RESOURCE_BUSY or #LCDD_ERR_NOT_OPENED.
// ============================================================================
PRIVATE LCDD_ERR_T ST7735S_SetPixel16(UINT16 x, UINT16 y, UINT16 pixelData)
{
    if (0 == lcdd_MutexGet())
    {
        return LCDD_ERR_RESOURCE_BUSY;
    }
    else
    {
        if (g_lcddInSleep)
        {
            lcdd_MutexFree();
            return LCDD_ERR_NO;
        }


        /*

            style00:

                    LCD_CtrlWrite_ST7735S_8(0x2a);
                    LCD_DataWrite_ST7735S_8(0x00);   // start column [15:8]
                    LCD_DataWrite_ST7735S_8(x);     // start column [7:0]
                    LCD_DataWrite_ST7735S_8(0x00);  // end column [15:8]
                    LCD_DataWrite_ST7735S_8(x);     // endcolumn [15:8]

                    LCD_CtrlWrite_ST7735S_8(0x2b);  // start row [15:8]
                    LCD_DataWrite_ST7735S_8(0x00);  // start row [7:0]
                    LCD_DataWrite_ST7735S_8(y);     // end row [15:8]
                    LCD_DataWrite_ST7735S_8(0x00);  // end row [7:0]
                    LCD_DataWrite_ST7735S_8(y);

                    LCD_CtrlWrite_ST7735S_8(0x2c);      // memory write
                    LCD_DataWrite_NV3021_16(pixelData); //0xffff  fffff
            style01:

                    LCD_CtrlData_ST7735S_16(0x0050, 0x0000); //Set Column Address must modify
                    LCD_CtrlData_ST7735S_16(0x0051, x);
                    LCD_CtrlData_ST7735S_16(0x0052, 0x0000);
                    LCD_CtrlData_ST7735S_16(0x0053, y);

                    LCD_CtrlWrite_ST7735S_8(0x22); //WRITE ram Data display    // memory write
                    LCD_DataWrite_ST7735S_16(pixelData); //0xffff  fffff

        **/

        LCD_CtrlWrite_ST7735S_8(0x2a);
        LCD_DataWrite_ST7735S_8(0x00);   // start column [15:8]
        LCD_DataWrite_ST7735S_8(x);     // start column [7:0]
        LCD_DataWrite_ST7735S_8(0x00);  // end column [15:8]
        LCD_DataWrite_ST7735S_8(x);     // endcolumn [15:8]

        LCD_CtrlWrite_ST7735S_8(0x2b);  // start row [15:8]
        LCD_DataWrite_ST7735S_8(0x00);  // start row [7:0]
        LCD_DataWrite_ST7735S_8(y);     // end row [15:8]
        LCD_DataWrite_ST7735S_8(0x00);  // end row [7:0]
        LCD_DataWrite_ST7735S_8(y);

        LCD_CtrlWrite_ST7735S_8(0x2c);      // memory write
        LCD_DataWrite_ST7735S_16(pixelData); //0xffff  fffff




        lcdd_MutexFree();
        return LCDD_ERR_NO;
    }
}


// ============================================================================
// lcddp_BlitRoi2Win
// ----------------------------------------------------------------------------
// Private function to transfer data to the LCD
//          display window (0x50   0x51  0x52  0x53 0x22 )
// ============================================================================
PRIVATE VOID lcddp_BlitRoi2Win(CONST HAL_GOUDA_WINDOW_T* pRoi, CONST HAL_GOUDA_WINDOW_T* pActiveWin)
{
    HAL_GOUDA_LCD_CMD_T cmd[13];

    if(!((pRoi->tlPX < pRoi->brPX) && (pRoi->tlPY < pRoi->brPY)))
    {
        LCDD_TRACE(LCDD_WARN_TRC,0,"lcddp_BlitRoi2Win: Invalid Roi x:%d < %d, y:%d < %d",pRoi->tlPX, pRoi->brPX, pRoi->tlPY, pRoi->brPY);
        ST7735S_GoudaBlitHandler();
        return;
    }

    // building set roi sequence:


#if defined(LCD_ST7735S_HLT_HSD)
// 翰彩玻璃 常用
// 屏幕边缘 花屏 更改 窗口区域
    uint16 left = pActiveWin->tlPX + 2 ;
    uint16 top =  pActiveWin->tlPY + 1;
    uint16 right = pActiveWin->brPX + 2;
    uint16 bottom = pActiveWin->brPY + 1;
#else
    uint16 left = pActiveWin->tlPX;
    uint16 top =  pActiveWin->tlPY;
    uint16 right = pActiveWin->brPX ;
    uint16 bottom = pActiveWin->brPY ;
#endif

#if 0
    if(g_lcddRotate)
    {
        //Window Horizontal RAM Address Start
        ST7735S_BUILD_CMD_WR_REG(cmd,0,0x0037,pActiveWin->tlPY);
        //Window Horizontal RAM Address End
        ST7735S_BUILD_CMD_WR_REG(cmd,2,0x0036,pActiveWin->brPY);
        //Window Vertical RAM Address Start
        ST7735S_BUILD_CMD_WR_REG(cmd,4,0x0039,ST7735S_LCD_HEIGHT-1-pActiveWin->brPX);
        //Window Vertical RAM Address End
        ST7735S_BUILD_CMD_WR_REG(cmd,6,0x0038,ST7735S_LCD_HEIGHT-1-pActiveWin->tlPX);

        //Start point
        ST7735S_BUILD_CMD_WR_REG(cmd,8,0x0020,pActiveWin->tlPY);
        ST7735S_BUILD_CMD_WR_REG(cmd,10,0x0021,ST7735S_LCD_HEIGHT-1-pActiveWin->tlPX);
    }
    else
    {
        //Window Horizontal RAM Address Start
        ST7735S_BUILD_CMD_WR_REG(cmd,0,0x0037,ST7735S_LCD_WIDTH-1-pActiveWin->brPX);
        //Window Horizontal RAM Address End
        ST7735S_BUILD_CMD_WR_REG(cmd,2,0x0036,ST7735S_LCD_WIDTH-1-pActiveWin->tlPX);
        //Window Vertical RAM Address Start
        ST7735S_BUILD_CMD_WR_REG(cmd,4,0x0039,ST7735S_LCD_HEIGHT-1-pActiveWin->brPY);
        //Window Vertical RAM Address End
        ST7735S_BUILD_CMD_WR_REG(cmd,6,0x0038,ST7735S_LCD_HEIGHT-1-pActiveWin->tlPY);

        //Start point
        ST7735S_BUILD_CMD_WR_REG(cmd,8,0x0020,ST7735S_LCD_WIDTH-1-pActiveWin->tlPX);
        ST7735S_BUILD_CMD_WR_REG(cmd,10,0x0021,ST7735S_LCD_HEIGHT-1-pActiveWin->tlPY);


    }

    // Send command after which the data we sent
    // are recognized as pixels.
    ST7735S_BUILD_CMD_WR_CMD(cmd,12,0x0022);
    while(HAL_ERR_NO != hal_GoudaBlitRoi(pRoi, 13, cmd, ST7735S_GoudaBlitHandler));

#endif

    if(g_lcddRotate)
    {
        /*
                   LCD_CtrlWrite_ST7735S_8(0x2A);    //Set Column Address
                LCD_DataWrite_ST7735S_8(0x00);
                LCD_DataWrite_ST7735S_8(pActiveWin->tlPY);
                LCD_DataWrite_ST7735S_8(0x00);
                LCD_DataWrite_ST7735S_8(pActiveWin->brPY);

                LCD_CtrlWrite_ST7735S_8(0x2B);   //Set Page Address
                LCD_DataWrite_ST7735S_8(0x00);
                LCD_DataWrite_ST7735S_8(ST7735S_LCD_HEIGHT-1- pActiveWin->brPX);
                LCD_DataWrite_ST7735S_8(0x00);
                LCD_DataWrite_ST7735S_8(ST7735S_LCD_HEIGHT-1- pActiveWin->tlPX);


        style00:
                LCD_CtrlWrite_ST7735S_8(0x2A);   //Set Column Address msut modify
                LCD_DataWrite_ST7735S_8(pActiveWin->tlPX>>8);
                LCD_DataWrite_ST7735S_8(pActiveWin->tlPX&0x00ff);

                LCD_DataWrite_ST7735S_8(pActiveWin->brPX>>8);
                LCD_DataWrite_ST7735S_8(pActiveWin->brPX&0x00ff);

                LCD_CtrlWrite_ST7735S_8(0x2B);   //Set Page Address
                LCD_DataWrite_ST7735S_8(pActiveWin->tlPY>>8);
                LCD_DataWrite_ST7735S_8(pActiveWin->tlPY&0x00ff);

                LCD_DataWrite_ST7735S_8(pActiveWin->brPY>>8);
                LCD_DataWrite_ST7735S_8(pActiveWin->brPY&0x00ff);
                LCD_CtrlWrite_ST7735S_8(0x2c); //WRITE ram Data display

        style01:


               LCD_CtrlData_ST7735S_16(0x0050, pActiveWin->tlPX); //Set Column Address msut modify
               LCD_CtrlData_ST7735S_16(0x0051, pActiveWin->brPX);
               LCD_CtrlData_ST7735S_16(0x0052, pActiveWin->tlPY);
               LCD_CtrlData_ST7735S_16(0x0053, pActiveWin->brPY);

               LCD_CtrlData_ST7735S_16(0x0020, pActiveWin->tlPX);
               LCD_CtrlData_ST7735S_16(0x0021, pActiveWin->tlPY);

               LCD_CtrlWrite_ST7735S_8(0x22); //WRITE ram Data display
        style02:
               LCD_CtrlData_ST7735S_16(0x0036, pActiveWin->tlPX); //Set Column Address msut modify
               LCD_CtrlData_ST7735S_16(0x0037, pActiveWin->brPX);
               LCD_CtrlData_ST7735S_16(0x0038, pActiveWin->tlPY);
               LCD_CtrlData_ST7735S_16(0x0039, pActiveWin->brPY);

               LCD_CtrlData_ST7735S_16(0x0020, pActiveWin->tlPX);
               LCD_CtrlData_ST7735S_16(0x0021, pActiveWin->tlPY);

               LCD_CtrlWrite_ST7735S_8(0x22); //WRITE ram Data display

        */

        LCD_CtrlWrite_ST7735S_8(0x2A);   //Set Column Address msut modify
        LCD_DataWrite_ST7735S_8(left>>8);
        LCD_DataWrite_ST7735S_8(left&0x00ff);

        LCD_DataWrite_ST7735S_8(right>>8);
        LCD_DataWrite_ST7735S_8(right&0x00ff);

        LCD_CtrlWrite_ST7735S_8(0x2B);   //Set Page Address
        LCD_DataWrite_ST7735S_8(top>>8);
        LCD_DataWrite_ST7735S_8(top&0x00ff);

        LCD_DataWrite_ST7735S_8(bottom>>8);
        LCD_DataWrite_ST7735S_8(bottom&0x00ff);
        LCD_CtrlWrite_ST7735S_8(0x2c); //WRITE ram Data display

    }
    else
    {

        // this will allow to keep LCDD interface for blit while using gouda
        // directly for configuring layers
        LCD_CtrlWrite_ST7735S_8(0x2A);    //Set Column Address msut modify
        LCD_DataWrite_ST7735S_8(left>>8);
        LCD_DataWrite_ST7735S_8(left&0x00ff);

        LCD_DataWrite_ST7735S_8(right>>8);
        LCD_DataWrite_ST7735S_8(right&0x00ff);

        LCD_CtrlWrite_ST7735S_8(0x2B);   //Set Page Address
        LCD_DataWrite_ST7735S_8(top>>8);
        LCD_DataWrite_ST7735S_8(top&0x00ff);

        LCD_DataWrite_ST7735S_8(bottom>>8);
        LCD_DataWrite_ST7735S_8(bottom&0x00ff);
        LCD_CtrlWrite_ST7735S_8(0x2c); //WRITE ram Data display


    }

    // Send command after which the data we sent
    // are recognized as pixels.


    // Send command after which the data we sent
    // are recognized as pixels.
    while(HAL_ERR_NO != hal_GoudaBlitRoi(pRoi, 0, cmd, ST7735S_GoudaBlitHandler));
}


// ============================================================================
// ST7735S_FillRect16
// ----------------------------------------------------------------------------
/// This function performs a fill of the active window  with some color.
/// @param bgColor Color with which to fill the rectangle. It is a 16-bit
/// data, as the one of #ST7735S_SetPixel16
/// @return #LCDD_ERR_NO, #LCDD_ERR_RESOURCE_BUSY or #LCDD_ERR_NOT_OPENED.
// ============================================================================
PRIVATE LCDD_ERR_T ST7735S_FillRect16(CONST LCDD_ROI_T* regionOfInterrest, UINT16 bgColor)
{
    // Active window coordinates.
    HAL_GOUDA_WINDOW_T activeWin;

    if (0 == lcdd_MutexGet())
    {
        return LCDD_ERR_RESOURCE_BUSY;
    }
    else
    {
        if (g_lcddInSleep)
        {
            lcdd_MutexFree();
            return LCDD_ERR_NO;
        }

        // Set Active window
        activeWin.tlPX = regionOfInterrest->x;
        activeWin.brPX = regionOfInterrest->x + regionOfInterrest->width - 1;
        activeWin.tlPY = regionOfInterrest->y;
        activeWin.brPY = regionOfInterrest->y + regionOfInterrest->height - 1;

        // Check parameters
        // ROI must be within the screen boundary
        BOOL badParam = FALSE;
        if (g_lcddRotate)
        {
            if (    (activeWin.tlPX >= ST7735S_LCD_HEIGHT) ||
                    (activeWin.brPX >= ST7735S_LCD_HEIGHT) ||
                    (activeWin.tlPY >= ST7735S_LCD_WIDTH) ||
                    (activeWin.brPY >= ST7735S_LCD_WIDTH)
               )
            {
                badParam = TRUE;
            }
        }
        else
        {
            if (    (activeWin.tlPX >= ST7735S_LCD_WIDTH) ||
                    (activeWin.brPX >= ST7735S_LCD_WIDTH) ||
                    (activeWin.tlPY >= ST7735S_LCD_HEIGHT) ||
                    (activeWin.brPY >= ST7735S_LCD_HEIGHT)
               )
            {
                badParam = TRUE;
            }
        }
        if (badParam)
        {
            lcdd_MutexFree();
            return LCDD_ERR_INVALID_PARAMETER;
        }

        hal_GoudaSetBgColor(bgColor);
        lcddp_BlitRoi2Win(&activeWin,&activeWin);

        return LCDD_ERR_NO;
    }
}

// ============================================================================
// lcddp_Blit16
// ----------------------------------------------------------------------------
/// This function provides the basic bit-block transfer capabilities.
/// This function copies the data (such as characters/bmp) on the LCD directly
/// as a (rectangular) block. The data is drawn in the active window.
/// The buffer has to be properly aligned (@todo define properly 'properly')
///
/// @param pPixelData Pointer to the buffer holding the data to be displayed
/// as a block. The dimension of this block are the one of the #pDestRect
/// parameter
/// @return #LCDD_ERR_NO, #LCDD_ERR_RESOURCE_BUSY or #LCDD_ERR_NOT_OPENED.
// ============================================================================
PRIVATE LCDD_ERR_T lcddp_Blit16(CONST LCDD_FBW_T* frameBufferWin, UINT16 startX, UINT16 startY)
{
    LCDD_ASSERT((frameBufferWin->fb.width&1) == 0, "LCDD: FBW must have an even number "
                "of pixels per line. Odd support is possible at the price of a huge "
                "performance lost");
    // Active window coordinates.
    HAL_GOUDA_WINDOW_T inputWin;
    HAL_GOUDA_WINDOW_T activeWin;

    if (0 == lcdd_MutexGet())
    {
        return LCDD_ERR_RESOURCE_BUSY;
    }
    else
    {
        if (g_lcddInSleep)
        {
            lcdd_MutexFree();
            return LCDD_ERR_NO;
        }

        // Set Input window
        inputWin.tlPX = frameBufferWin->roi.x;
        inputWin.brPX = frameBufferWin->roi.x + frameBufferWin->roi.width - 1;
        inputWin.tlPY = frameBufferWin->roi.y;
        inputWin.brPY = frameBufferWin->roi.y + frameBufferWin->roi.height - 1;

        // Set Active window
        activeWin.tlPX = startX;
        activeWin.brPX = startX + frameBufferWin->roi.width - 1;
        activeWin.tlPY = startY;
        activeWin.brPY = startY + frameBufferWin->roi.height - 1;

        // Check parameters
        // ROI must be within the screen boundary
        // ROI must be within the Frame buffer
        // Color format must be 16 bits
        BOOL badParam = FALSE;
        if (g_lcddRotate)
        {
            if (    (activeWin.tlPX >= ST7735S_LCD_HEIGHT) ||
                    (activeWin.brPX >= ST7735S_LCD_HEIGHT) ||
                    (activeWin.tlPY >= ST7735S_LCD_WIDTH) ||
                    (activeWin.brPY >= ST7735S_LCD_WIDTH)
               )
            {
                badParam = TRUE;
            }
        }
        else
        {
            if (    (activeWin.tlPX >= ST7735S_LCD_WIDTH) ||
                    (activeWin.brPX >= ST7735S_LCD_WIDTH) ||
                    (activeWin.tlPY >= ST7735S_LCD_HEIGHT) ||
                    (activeWin.brPY >= ST7735S_LCD_HEIGHT)
               )
            {
                badParam = TRUE;
            }
        }
        if (!badParam)
        {
            if (    (frameBufferWin->roi.width > frameBufferWin->fb.width) ||
                    (frameBufferWin->roi.height > frameBufferWin->fb.height) ||
                    (frameBufferWin->fb.colorFormat != LCDD_COLOR_FORMAT_RGB_565)
               )
            {
                badParam = TRUE;;
            }
        }
        if (badParam)
        {
            lcdd_MutexFree();
            return LCDD_ERR_INVALID_PARAMETER;
        }

        // this will allow to keep LCDD interface for blit while using gouda
        // directly for configuring layers
        if (frameBufferWin->fb.buffer != NULL)
        {
            HAL_GOUDA_OVL_LAYER_DEF_T def;
            // configure ovl layer 0 as buffer
            def.addr = (UINT32*)frameBufferWin->fb.buffer; // what about aligment ?
            def.fmt = HAL_GOUDA_IMG_FORMAT_RGB565; //TODO convert from .colorFormat
            //def.stride = frameBufferWin->fb.width * 2;
            def.stride = 0; // let hal gouda decide
            def.pos.tlPX = 0;
            def.pos.tlPY = 0;
            def.pos.brPX = frameBufferWin->fb.width - 1;
            def.pos.brPY = frameBufferWin->fb.height - 1;
            def.alpha = 255;
            def.cKeyEn = FALSE;

            // open the layer
            hal_GoudaOvlLayerClose(HAL_GOUDA_OVL_LAYER_ID0);
            hal_GoudaOvlLayerOpen(HAL_GOUDA_OVL_LAYER_ID0, &def);
            // tell the end handler not to close the layer when we are done
            g_lcddAutoCloseLayer = FALSE;
        }

        // gouda is doing everything ;)
        lcddp_BlitRoi2Win(&inputWin, &activeWin);

        return LCDD_ERR_NO;
    }
}


// ============================================================================
// ST7735S_Busy
// ----------------------------------------------------------------------------
/// This function is not implemented for the ebc version of the driver
// ============================================================================
PRIVATE BOOL ST7735S_Busy(VOID)
{
    return FALSE;
}


// ============================================================================
// ST7735S_SetDirRotation
// ----------------------------------------------------------------------------
///
// ============================================================================

#if  LCD_ST7735S_ZGD_SC//0//defined()   //must modify
PRIVATE UINT16 u_RGB =0x00 ;
#else
PRIVATE UINT16 u_RGB =0x08 ;
#endif



PRIVATE BOOL ST7735S_SetDirRotation(VOID)
{
    while (0 == lcdd_MutexGet())
    {
        LCD_Delayms(ST7735S_TIME_MUTEX_RETRY);
    }

    g_lcddRotate = TRUE;

    if (g_lcddInSleep)
    {
        lcdd_MutexFree();
        return TRUE;
    }


    LCD_CtrlWrite_ST7735S_8(0x36);   //Set Scanning Direction must modify
#ifdef LCDSIZE_160_128
	LCD_DataWrite_ST7735S_8(0xA8);    //0x40
#else
    LCD_DataWrite_ST7735S_8(0x60 |u_RGB);    //0x40
#endif

    //LCD_CtrlData_ST7735S_16(0x0003, 0x1030 |u_RGB);

    LCD_Delayms(10 );



    lcdd_MutexFree();

    return TRUE;
}

// ============================================================================
// ST7735S_SetDirection
// ----------------------------------------------------------------------------
///         MY    MX   MV  ML     RGB
// ============================================================================
PRIVATE BOOL ST7735S_SetDirection(VOID)
{
    while (0 == lcdd_MutexGet())
    {
        LCD_Delayms(ST7735S_TIME_MUTEX_RETRY);
    }

    g_lcddRotate = FALSE;

    if (g_lcddInSleep)
    {
        lcdd_MutexFree();
        return TRUE;
    }


    //normal
    LCD_CtrlWrite_ST7735S_8(0x36); //Set Scanning Direction
#ifdef LCDSIZE_160_128
		LCD_DataWrite_ST7735S_8(0xA8);  //0xc0  must modify 
#else
    LCD_DataWrite_ST7735S_8(0xc0| u_RGB);  //0xc0  must modify
#endif
    //    LCD_CtrlData_ST7735S_16(0x0003,0x1030 |u_RGB);

    LCD_Delayms(10 );

    lcdd_MutexFree();

    return TRUE;
}



/*
**  ST7735S   readID()
**
**  read(0x00) << 0xxx 0xxx 0x30  0X29
**

**
**
*/



//       ST7735S  ID most modify
#ifdef USE_SPI_LCD
#define  LCD_ST7735S_ID         0xf089
#else
#define  LCD_ST7735S_ID         0x7c89
#endif


//extern char *HX_lcd_string;  //must be end with \n
#if 0
PRIVATE BOOL ST7735S_ReadID()
{
    UINT16 productId=0;
    LCDD_CONFIG_T lcddReadConfig=LCDD_READ_CONFIG;


    hal_GoudaOpen(&lcddReadConfig.config, lcddReadConfig.timings, 0);
    sxr_Sleep(20 MS_WAITING);

    hal_GoudaReadReg(0x04, &productId);
    hal_GoudaClose();

    SXS_TRACE(TSTDOUT, "st7735s(0x%x): lcd read id is 0x%x ", LCD_ST7735S_ID, productId);

    return TRUE;

    if(productId == LCD_ST7735S_ID)
        return TRUE;
    else
        return FALSE;

}
#else

PRIVATE BOOL ST7735S_ReadID()
{
    UINT16 productId=0;


    UINT8 productIds[4];
    LCDD_CONFIG_T lcddReadConfig=LCDD_READ_CONFIG;

#ifdef USE_SPI_LCD
    hal_GoudaSerialOpen(LCDD_SPI_LINE_TYPE, LCDD_SPI_FREQ_READ, &lcddReadConfig.config, 0);
#else
    hal_GoudaOpen(&lcddReadConfig.config, lcddReadConfig.timings, 0);
#endif

    LCD_Delayms(20 );



    hal_GoudaReadData(0x04,productIds,4);

    productId= ((productIds[1]&0xff) << 8)  |(productIds[2]&0xff);

    hal_HstSendEvent(0x1cd77355);
    hal_HstSendEvent(productId);


    hal_GoudaClose();


    SXS_TRACE(TSTDOUT, "ST7735S(0x%x): lcd read id is 0x%x ", LCD_ST7735S_ID, productId);


    if (productId == LCD_ST7735S_ID)
    {
        //HX_lcd_string = "ST7735S\n";
        hal_HstSendEvent(0xdeb77355);

        return TRUE;
    }
    else

        return FALSE;

}

#endif


//ST7735S_string
PRIVATE char* ST7735S_get_id_string(void)
{
    static char ST7735S_id_str[] = "ST7735S\n";

    return ST7735S_id_str;
}

// ============================================================================
// lcdd_ST7735S_RegInit
// ----------------------------------------------------------------------------
/// register the right lcd driver, according to ST7735S_ReadID
/// @return #TRUE, #FALSE
// ============================================================================
PUBLIC BOOL  lcdd_st7735s_RegInit(LCDD_REG_T *pLcdDrv)
{
    hal_HstSendEvent(0x1cd7735f);

    if( ST7735S_ReadID())
    {
        pLcdDrv->lcdd_Open=ST7735S_Open;
        pLcdDrv->lcdd_Close=ST7735S_Close;
        pLcdDrv->lcdd_SetContrast=ST7735S_SetContrast;

        pLcdDrv->lcdd_SetStandbyMode=ST7735S_SetStandbyMode;
        pLcdDrv->lcdd_PartialOn=ST7735S_PartialOn;
        pLcdDrv->lcdd_PartialOff=ST7735S_PartialOff;
        pLcdDrv->lcdd_Blit16=lcddp_Blit16;
        pLcdDrv->lcdd_Busy=ST7735S_Busy;
        pLcdDrv->lcdd_FillRect16=ST7735S_FillRect16;
        pLcdDrv->lcdd_GetScreenInfo=ST7735S_GetScreenInfo;
        pLcdDrv->lcdd_WakeUp=ST7735S_WakeUp;
        pLcdDrv->lcdd_SetPixel16=ST7735S_SetPixel16;
        pLcdDrv->lcdd_Sleep=ST7735S_Sleep;
        pLcdDrv->lcdd_SetDirRotation=ST7735S_SetDirRotation;
        pLcdDrv->lcdd_SetDirDefault=ST7735S_SetDirection;
        pLcdDrv->lcdd_GetStringId=ST7735S_get_id_string;

        return TRUE;
    }

    return FALSE;
}


