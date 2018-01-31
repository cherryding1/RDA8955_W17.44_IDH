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

#include "rda5888app.h"
#include "rda5888freqtbl.h"
#if defined(SPRD_CUSTOMER)
#include "rda5888sprd.h"
#elif defined(__MTK_TARGET__)
#include "rda5888mtk.h"
#include "TimerEvents.h"
#include "isp_if_622x_series_hw.h"
#endif

// ---------------------------------------------------------------------------
// global variables & definition
// ---------------------------------------------------------------------------
int g_nCurChCnt = 0;  // current channel counter.
uint8 g_nSignalLevel = 0;
rdamtv_ctx_t g_stRdamtvCtx;
const rdamtv_channel_t *p_rdamtv_cur_map = NULL; /* Current Default Channel Map*/

#ifdef RDA5888_VIDENHANCE_ENABLE
uint8 g_nVideoEnhanceEnable = 1; // enable/disable video enhance function.
uint8 g_nDiscardFrameNum = 2; // actual discard frame: g_nDiscardFrameNum - 1
uint32 g_nFrameBufSize = 0;
uint8 g_nFirstBufferFlag = 5;  // 1: first frame buffer, 0: other frame buffer.
#endif

// ---------------------------------------------------------------------------
// local variables & definition
// ---------------------------------------------------------------------------
static uint32 m_nCurTvArea = RDAMTV_AREA_CHINA; // defualt area: RDAMTV_AREA_CHINA
static uint16 m_nIsTPMode = 0;
#if ((RDA5888_CHIP_VER == 4) ||(RDA5888_CHIP_VER == 5))
static uint16 m_nTPRegBak = 0;
static uint16 g_n116RegBak = 0;
#endif

#ifdef RDA5888_VIDENHANCE_ENABLE
#define R_MASK         0xF800//0xF000//0xF800
#define G_MASK         0x07E0//0x07C0//0x07E0
#define B_MASK         0x001F//0x001E//0x001F
#define RGB_MASK       0xF7DEF7DE
#define R_SHIFT        11
#define G_SHIFT        5
#define B_SHIFT        0

#if (RDA5888_CHIP_VER == 4)
//#define RDA5888_SHARPENING_ENABLE
#endif

#define RDA5888_SNOWPATTERN_ENABLE

static uint8 m_nImageSharpEn = 0;  // image sharp enable
#endif

#define CHECK_SYNC_FAIL_TIMES           5
#define RDAAPP_TIMER_CYCLE            100

//*****************************************************************************
//**************** START OF PRIVATE PROCEDURE IMPLEMENTATIONS *****************
//*****************************************************************************

#ifdef RDA5888_SOFTBLENDING_ENABLE
#define AVERAGE_RATE         3  // 3

#if (4 == RDA5888_CHIP_VER)
#define BLENDTABLE_OFFSET    15  //20  // 15
#elif (5 == RDA5888_CHIP_VER)
#define BLENDTABLE_OFFSET    50  //35
#else
#define BLENDTABLE_OFFSET    15
#endif

#define BLENDTABLE_LEN      11

#define XTAL_117H_TH        75
#define XTAL_X_TH           500 //50

typedef struct
{
    uint16 Lim_Low;
    uint16 Lim_UP;
    uint8 yc_adjust;
    uint8 vt_fail_th;
    uint16 frame_coef;
} BlendStruct;

static BlendStruct BlendTable[BLENDMODE_INVALID][BLENDTABLE_LEN] =
{
    // BLENDMODE_PALBG
    {
        //       min                        max          Y/C   vt_f  coef
        {0,                     10 +BLENDTABLE_OFFSET, 0x22, 0x06, 0x046}, // index_0
        {10 +BLENDTABLE_OFFSET, 25 +BLENDTABLE_OFFSET, 0x22, 0x0f, 0x046}, // index_1
        {25 +BLENDTABLE_OFFSET, 35 +BLENDTABLE_OFFSET, 0x20, 0x15, 0x046}, // index_2
        {35 +BLENDTABLE_OFFSET, 45 +BLENDTABLE_OFFSET, 0x20, 0x1d, 0x0a3}, // index_3
        {45 +BLENDTABLE_OFFSET, 55 +BLENDTABLE_OFFSET, 0x2E, 0x25, 0x0a3}, // index_4
        {55 +BLENDTABLE_OFFSET, 65 +BLENDTABLE_OFFSET, 0x2E, 0x2f, 0x0a3}, // index_5
        {65 +BLENDTABLE_OFFSET, 75 +BLENDTABLE_OFFSET, 0x2C, 0x34, 0x0c2}, // index_6
        {75 +BLENDTABLE_OFFSET, 85 +BLENDTABLE_OFFSET, 0x3C, 0x3a, 0x0c2}, // index_7
        {85 +BLENDTABLE_OFFSET, 100+BLENDTABLE_OFFSET, 0x4A, 0x3f, 0x0c2}, // index_8
        {100+BLENDTABLE_OFFSET, 118+BLENDTABLE_OFFSET, 0x69, 0x3f, 0x0e1}, // index_9
        {118+BLENDTABLE_OFFSET, 4096,                  0x78, 0x3f, 0x0e1}  // index_10
    },

    // BLENDMODE_PALDK
    {
        {0,                     10 +BLENDTABLE_OFFSET, 0x22, 0x06, 0x046}, // index_0
        {10 +BLENDTABLE_OFFSET, 25 +BLENDTABLE_OFFSET, 0x22, 0x0f, 0x046}, // index_1
        {25 +BLENDTABLE_OFFSET, 35 +BLENDTABLE_OFFSET, 0x20, 0x15, 0x046}, // index_2
        {35 +BLENDTABLE_OFFSET, 45 +BLENDTABLE_OFFSET, 0x20, 0x1d, 0x0a3}, // index_3
        {45 +BLENDTABLE_OFFSET, 55 +BLENDTABLE_OFFSET, 0x2E, 0x25, 0x0a3}, // index_4
        {55 +BLENDTABLE_OFFSET, 65 +BLENDTABLE_OFFSET, 0x2E, 0x2f, 0x0a3}, // index_5
        {65 +BLENDTABLE_OFFSET, 75 +BLENDTABLE_OFFSET, 0x2C, 0x34, 0x0c2}, // index_6
        {75 +BLENDTABLE_OFFSET, 85 +BLENDTABLE_OFFSET, 0x3C, 0x3a, 0x0c2}, // index_7
        {85 +BLENDTABLE_OFFSET, 100+BLENDTABLE_OFFSET, 0x4A, 0x3f, 0x0c2}, // index_8
        {100+BLENDTABLE_OFFSET, 120+BLENDTABLE_OFFSET, 0x69, 0x3f, 0x0e1}, // index_9
        {120+BLENDTABLE_OFFSET, 4096,                  0x78, 0x3f, 0x0e1}  // index_10
    },

    // BLENDMODE_PALI
    {
        {0,                     10 +BLENDTABLE_OFFSET, 0x22, 0x06, 0x046}, // index_0
        {10 +BLENDTABLE_OFFSET, 25 +BLENDTABLE_OFFSET, 0x22, 0x0f, 0x046}, // index_1
        {25 +BLENDTABLE_OFFSET, 35 +BLENDTABLE_OFFSET, 0x20, 0x15, 0x046}, // index_2
        {35 +BLENDTABLE_OFFSET, 45 +BLENDTABLE_OFFSET, 0x20, 0x1d, 0x0a3}, // index_3
        {45 +BLENDTABLE_OFFSET, 55 +BLENDTABLE_OFFSET, 0x2E, 0x25, 0x0a3}, // index_4
        {55 +BLENDTABLE_OFFSET, 65 +BLENDTABLE_OFFSET, 0x2E, 0x2f, 0x0a3}, // index_5
        {65 +BLENDTABLE_OFFSET, 75 +BLENDTABLE_OFFSET, 0x2C, 0x34, 0x0c2}, // index_6
        {75 +BLENDTABLE_OFFSET, 85 +BLENDTABLE_OFFSET, 0x3C, 0x3a, 0x0c2}, // index_7
        {85 +BLENDTABLE_OFFSET, 100+BLENDTABLE_OFFSET, 0x4A, 0x3f, 0x0c2}, // index_8
        {100+BLENDTABLE_OFFSET, 118+BLENDTABLE_OFFSET, 0x69, 0x3f, 0x0e1}, // index_9
        {118+BLENDTABLE_OFFSET, 4096,                  0x78, 0x3f, 0x0e1}  // index_10
    },

    // BLENDMODE_PALM
    {
        {0,                     10 +BLENDTABLE_OFFSET, 0x22, 0x06, 0x046}, // index_0
        {10 +BLENDTABLE_OFFSET, 20 +BLENDTABLE_OFFSET, 0x22, 0x0f, 0x046}, // index_1
        {20 +BLENDTABLE_OFFSET, 30 +BLENDTABLE_OFFSET, 0x20, 0x15, 0x046}, // index_2
        {30 +BLENDTABLE_OFFSET, 35 +BLENDTABLE_OFFSET, 0x20, 0x1d, 0x0a3}, // index_3
        {35 +BLENDTABLE_OFFSET, 45 +BLENDTABLE_OFFSET, 0x2E, 0x25, 0x0a3}, // index_4
        {45 +BLENDTABLE_OFFSET, 55 +BLENDTABLE_OFFSET, 0x2E, 0x2f, 0x0a3}, // index_5
        {55 +BLENDTABLE_OFFSET, 65 +BLENDTABLE_OFFSET, 0x2C, 0x34, 0x0c2}, // index_6
        {65 +BLENDTABLE_OFFSET, 75 +BLENDTABLE_OFFSET, 0x3C, 0x3a, 0x0c2}, // index_7
        {75 +BLENDTABLE_OFFSET, 85 +BLENDTABLE_OFFSET, 0x4A, 0x3f, 0x0c2}, // index_8
        {85 +BLENDTABLE_OFFSET, 100+BLENDTABLE_OFFSET, 0x69, 0x3f, 0x0e1}, // index_9
        {100+BLENDTABLE_OFFSET, 4096,                  0x78, 0x3f, 0x0e1}  // index_10
    },

    // BLENDMODE_PALN
    {
        {0,                     10 +BLENDTABLE_OFFSET, 0x22, 0x06, 0x046}, // index_0
        {10 +BLENDTABLE_OFFSET, 25 +BLENDTABLE_OFFSET, 0x22, 0x0f, 0x046}, // index_1
        {25 +BLENDTABLE_OFFSET, 35 +BLENDTABLE_OFFSET, 0x20, 0x15, 0x046}, // index_2
        {35 +BLENDTABLE_OFFSET, 45 +BLENDTABLE_OFFSET, 0x20, 0x1d, 0x0a3}, // index_3
        {45 +BLENDTABLE_OFFSET, 55 +BLENDTABLE_OFFSET, 0x2E, 0x25, 0x0a3}, // index_4
        {55 +BLENDTABLE_OFFSET, 65 +BLENDTABLE_OFFSET, 0x2E, 0x2f, 0x0a3}, // index_5
        {65 +BLENDTABLE_OFFSET, 75 +BLENDTABLE_OFFSET, 0x2C, 0x34, 0x0c2}, // index_6
        {75 +BLENDTABLE_OFFSET, 85 +BLENDTABLE_OFFSET, 0x3C, 0x3a, 0x0c2}, // index_7
        {85 +BLENDTABLE_OFFSET, 100+BLENDTABLE_OFFSET, 0x4A, 0x3f, 0x0c2}, // index_8
        {100+BLENDTABLE_OFFSET, 115+BLENDTABLE_OFFSET, 0x69, 0x3f, 0x0e1}, // index_9
        {115+BLENDTABLE_OFFSET, 4096,                  0x78, 0x3f, 0x0e1}  // index_10
    },

    // BLENDMODE_NTSCBG
    {
        {0,                     10 +BLENDTABLE_OFFSET, 0x22, 0x06, 0x046}, // index_0
        {10 +BLENDTABLE_OFFSET, 25 +BLENDTABLE_OFFSET, 0x22, 0x0f, 0x046}, // index_1
        {25 +BLENDTABLE_OFFSET, 35 +BLENDTABLE_OFFSET, 0x20, 0x15, 0x046}, // index_2
        {35 +BLENDTABLE_OFFSET, 45 +BLENDTABLE_OFFSET, 0x20, 0x1d, 0x0a3}, // index_3
        {45 +BLENDTABLE_OFFSET, 55 +BLENDTABLE_OFFSET, 0x2E, 0x25, 0x0a3}, // index_4
        {55 +BLENDTABLE_OFFSET, 65 +BLENDTABLE_OFFSET, 0x2E, 0x2f, 0x0a3}, // index_5
        {65 +BLENDTABLE_OFFSET, 75 +BLENDTABLE_OFFSET, 0x2C, 0x34, 0x0c2}, // index_6
        {75 +BLENDTABLE_OFFSET, 85 +BLENDTABLE_OFFSET, 0x3C, 0x3a, 0x0c2}, // index_7
        {85 +BLENDTABLE_OFFSET, 100+BLENDTABLE_OFFSET, 0x4A, 0x3f, 0x0c2}, // index_8
        {100+BLENDTABLE_OFFSET, 115+BLENDTABLE_OFFSET, 0x69, 0x3f, 0x0e1}, // index_9
        {115+BLENDTABLE_OFFSET, 4096,                  0x78, 0x3f, 0x0e1}  // index_10
    },

    // BLENDMODE_NTSCDK
    {
        {0,                     10 +BLENDTABLE_OFFSET, 0x22, 0x06, 0x046}, // index_0
        {10 +BLENDTABLE_OFFSET, 25 +BLENDTABLE_OFFSET, 0x22, 0x0f, 0x046}, // index_1
        {25 +BLENDTABLE_OFFSET, 35 +BLENDTABLE_OFFSET, 0x20, 0x15, 0x046}, // index_2
        {35 +BLENDTABLE_OFFSET, 45 +BLENDTABLE_OFFSET, 0x20, 0x1d, 0x0a3}, // index_3
        {45 +BLENDTABLE_OFFSET, 55 +BLENDTABLE_OFFSET, 0x2E, 0x25, 0x0a3}, // index_4
        {55 +BLENDTABLE_OFFSET, 65 +BLENDTABLE_OFFSET, 0x2E, 0x2f, 0x0a3}, // index_5
        {65 +BLENDTABLE_OFFSET, 75 +BLENDTABLE_OFFSET, 0x2C, 0x34, 0x0c2}, // index_6
        {75 +BLENDTABLE_OFFSET, 85 +BLENDTABLE_OFFSET, 0x3C, 0x3a, 0x0c2}, // index_7
        {85 +BLENDTABLE_OFFSET, 100+BLENDTABLE_OFFSET, 0x4A, 0x3f, 0x0c2}, // index_8
        {100+BLENDTABLE_OFFSET, 115+BLENDTABLE_OFFSET, 0x69, 0x3f, 0x0e1}, // index_9
        {115+BLENDTABLE_OFFSET, 4096,                  0x78, 0x3f, 0x0e1}  // index_10
    },

    // BLENDMODE_NTSCI
    {
        {0,                     10 +BLENDTABLE_OFFSET, 0x22, 0x06, 0x046}, // index_0
        {10 +BLENDTABLE_OFFSET, 25 +BLENDTABLE_OFFSET, 0x22, 0x0f, 0x046}, // index_1
        {25 +BLENDTABLE_OFFSET, 35 +BLENDTABLE_OFFSET, 0x20, 0x15, 0x046}, // index_2
        {35 +BLENDTABLE_OFFSET, 45 +BLENDTABLE_OFFSET, 0x20, 0x1d, 0x0a3}, // index_3
        {45 +BLENDTABLE_OFFSET, 55 +BLENDTABLE_OFFSET, 0x2E, 0x25, 0x0a3}, // index_4
        {55 +BLENDTABLE_OFFSET, 65 +BLENDTABLE_OFFSET, 0x2E, 0x2f, 0x0a3}, // index_5
        {65 +BLENDTABLE_OFFSET, 75 +BLENDTABLE_OFFSET, 0x2C, 0x34, 0x0c2}, // index_6
        {75 +BLENDTABLE_OFFSET, 85 +BLENDTABLE_OFFSET, 0x3C, 0x3a, 0x0c2}, // index_7
        {85 +BLENDTABLE_OFFSET, 100+BLENDTABLE_OFFSET, 0x4A, 0x3f, 0x0c2}, // index_8
        {100+BLENDTABLE_OFFSET, 115+BLENDTABLE_OFFSET, 0x69, 0x3f, 0x0e1}, // index_9
        {115+BLENDTABLE_OFFSET, 4096,                  0x78, 0x3f, 0x0e1}  // index_10
    },

    // BLENDMODE_NTSCM
    {
        {0,                     10 +BLENDTABLE_OFFSET, 0x22, 0x06, 0x046}, // index_0
        {10 +BLENDTABLE_OFFSET, 20 +BLENDTABLE_OFFSET, 0x22, 0x0f, 0x046}, // index_1
        {20 +BLENDTABLE_OFFSET, 30 +BLENDTABLE_OFFSET, 0x20, 0x15, 0x046}, // index_2
        {30 +BLENDTABLE_OFFSET, 40 +BLENDTABLE_OFFSET, 0x20, 0x1d, 0x0a3}, // index_3
        {40 +BLENDTABLE_OFFSET, 50 +BLENDTABLE_OFFSET, 0x2E, 0x25, 0x0a3}, // index_4
        {50 +BLENDTABLE_OFFSET, 60 +BLENDTABLE_OFFSET, 0x2E, 0x2f, 0x0a3}, // index_5
        {60 +BLENDTABLE_OFFSET, 70 +BLENDTABLE_OFFSET, 0x2C, 0x34, 0x0c2}, // index_6
        {70 +BLENDTABLE_OFFSET, 80 +BLENDTABLE_OFFSET, 0x3C, 0x3a, 0x0c2}, // index_7
        {80 +BLENDTABLE_OFFSET, 89 +BLENDTABLE_OFFSET, 0x4A, 0x3f, 0x0c2}, // index_8
        {89 +BLENDTABLE_OFFSET, 98 +BLENDTABLE_OFFSET, 0x69, 0x3f, 0x0e1}, // index_9
        {98 +BLENDTABLE_OFFSET, 4096,                  0x78, 0x3f, 0x0e1}  // index_10
    }

};

static bool get_mid(uint16 DataIn,uint16* mid_var)
{
    static uint8 count = 1;
    static uint16 Mid_buff[3];

    Mid_buff[2] = Mid_buff[1];
    Mid_buff[1] = Mid_buff[0];
    Mid_buff[0] = DataIn;

    // wait get 3 Mid_buff.
    if (count < 3)
    {
        count++;
        return 0;
    }

    // get midle value from Mid_buff[2:0]
    *mid_var=(Mid_buff[0] < Mid_buff[1] ?
              (Mid_buff[1] < Mid_buff[2] ? Mid_buff[1] : Mid_buff[0] < Mid_buff[2] ? Mid_buff[2] : Mid_buff[0]) :
              (Mid_buff[1] > Mid_buff[2] ? Mid_buff[1] : Mid_buff[0] > Mid_buff[2] ? Mid_buff[2] : Mid_buff[0]));

    return 1;
}

static bool average(uint16 DataIn,uint16* AveVar)
{
    static char count = 0;
    static uint32 sum = 0;

    sum += DataIn;
    count++;
    if(count < AVERAGE_RATE)
    {
        return 0;
    }

    *AveVar = sum/AVERAGE_RATE;
    count = 0;
    sum = 0;

    return 1;
}

static uint8 search_blend_table(uint16 nAveVal)
{
    int32 i;
    static uint8 LastIndex = 0xff;
    uint16 Lim_UP, Lim_Low;

    if (LastIndex != 0xff)
    {
        for (i = LastIndex-1; (BlendTable[g_nBlendMode][LastIndex].yc_adjust == BlendTable[g_nBlendMode][i].yc_adjust) && (i >= 0); i--);
        Lim_Low = (BlendTable[g_nBlendMode][i+1].Lim_Low-3) < BlendTable[g_nBlendMode][i+1].Lim_Low ? BlendTable[g_nBlendMode][i+1].Lim_Low-3 : BlendTable[g_nBlendMode][i+1].Lim_Low;

        for (i = LastIndex+1; (BlendTable[g_nBlendMode][LastIndex].yc_adjust == BlendTable[g_nBlendMode][i].yc_adjust) && (i < BLENDTABLE_LEN); i++);
        Lim_UP = (BlendTable[g_nBlendMode][i-1].Lim_UP+3) > BlendTable[g_nBlendMode][i-1].Lim_UP ? BlendTable[g_nBlendMode][i-1].Lim_UP+3 : BlendTable[g_nBlendMode][i-1].Lim_UP;

        if ((nAveVal >= Lim_Low) && (nAveVal <= Lim_UP))
            return (0xff);
    }

    for(i = 0; i < BLENDTABLE_LEN; i++)
    {
        if (nAveVal < BlendTable[g_nBlendMode][i].Lim_UP)
        {
            LastIndex = i;
            return  i;
        }
    }

    return (BLENDTABLE_LEN-1);
}

#ifdef RDA5888_SOFTAFC_ENABLE

#define GET_ABS(x)          ((x)>0?(x):-(x))

static void xtal_bank_process(void)
{
    uint16 reg, Reg_88H, Reg_88HR;
    static int Pk = 0;
    static int Pk1 = 0;
    static int Yk = 0;
    static int Yk1 = 0;
    static int Countk = 0;
    static int Countk1 = 0;
    static int Sum = 0;
    int PK_D, PK_D_1, PK_D_2, X;

    // step3
    RDA5888_ReadReg(0x88, &reg);
    Reg_88H = Reg_88HR = reg;

    // step4
    Pk1 = Pk;
    RDA5888_ReadReg(0x1d7, &reg);
    Pk = (int)reg;
    PK_D = Pk - Pk1;

    // PK_D_1 limit process
    PK_D_1 = (PK_D > 32768) ? (PK_D - 65536) : PK_D;

    // PK_D_2 limit process
    PK_D_2 = (PK_D < -32768) ? (PK_D_1 + 65536) : PK_D_1;
    if (GET_ABS(PK_D_2) >= XTAL_X_TH)
    {
        PK_D_2 = (PK_D_2 > XTAL_X_TH) ? (XTAL_X_TH) : (-XTAL_X_TH);
    }

    X = PK_D_2;
    Yk = (1-0.1)*Yk1 + X*0.1;
    Yk1 = Yk;

    // step 5
    Countk = Yk*0.5 + Countk1;
    Countk1 = Countk;
    if (GET_ABS(Countk) > 256) // (a)
    {
        Sum = 0;
        if (Countk > 0)
        {
            Countk = Countk - 256;
            Countk1 = Countk;
            if (RDA_GET_MASK(Reg_88H, 6, 0x3fc0) > 0)
            {
                Reg_88H = (Reg_88H - 0x0040);
                RDA5888_WriteReg(0x88, Reg_88H);
                Reg_88HR = Reg_88H;
            }
        }
        else // (Countk <= 0)
        {
            Countk = Countk + 256;
            Countk1 = Countk;
            if (RDA_GET_MASK(Reg_88H, 6, 0x3fc0) < 0x3fc0)
            {
                Reg_88H = (Reg_88H + 0x0040);
                RDA5888_WriteReg(0x88, Reg_88H);
                Reg_88HR = Reg_88H;
            }
        }
    }
    else // (b) (GET_ABS(Countk) < 256)
    {
        Sum = Sum + Countk;
        if (GET_ABS(Sum) > 256)
        {
            if (Sum > 0)
            {
                Sum = Sum - 256;
                if (RDA_GET_MASK(Reg_88H, 6, 0x3fc0) > 0)
                {
                    Reg_88HR = (Reg_88H - 0x0040);
                    RDA5888_WriteReg(0x88, Reg_88HR);
                }
            }
            else
            {
                Sum = Sum + 256;
                if (RDA_GET_MASK(Reg_88H, 6, 0x3fc0) < 0x3fc0)
                {
                    Reg_88HR = (Reg_88H + 0x0040);
                    RDA5888_WriteReg(0x88, Reg_88HR);
                }
            }
        }
        else
        {
            Reg_88HR = Reg_88H;
            RDA5888_WriteReg(0x88, Reg_88HR);
        }
    }

    // debug printf.
    //RDA5888_ReadReg(0x88, &reg);
    //RDAEXT_DebugPrint(RDA5888_TRACE_MOD "[rdamtv]xtal_bank, REG88= 0x%x\n", reg);

#if 0 // for test xtal_bank adjust.
    {
        RDA5888_ReadReg(0x1d9, &reg);
        //f = (float)(r*6.436);
        RDAEXT_DebugPrint(RDA5888_TRACE_MOD "[rdamtv]xtal_bank, reg = 0x%x\n", reg);
    }
#endif
}
#endif

#ifdef RDA5888_MULTIPATH_ENABLE
static void multi_path_process(uint16 var)
{
    static uint8 set_flag = 1;

    if (var < XTAL_117H_TH)
    {
        if (set_flag)
        {
            RDA5888_WriteReg(0x115, 0x040a);
            set_flag = 0;
        }
    }
    else
    {
        if (!set_flag)
        {
            RDA5888_WriteReg(0x115, 0x048a);
            set_flag = 1;
        }
    }
}
#endif

#ifdef RDA5888_LPFSWITCH_ENABLE
static void LPF_switch_process(uint16 var)
{
    if (var > 83)
    {
        RDA5888_FieldWriteReg(0x19b, 0x0020, 0x0);
    }
    else if (var < 70)
    {
        RDA5888_FieldWriteReg(0x19b, 0x0020, 0x1);
    }
}
#endif


static void soft_blending(void)
{
    uint16 var,midvar,AveVar;
    uint8 index;

#if ((RDA5888_CHIP_VER == 5) || (RDA5888_CHIP_VER == 6) || (RDA5888_CHIP_VER == 7))
    RDA5888_WriteReg(0x12f, 0x0076);  //0x0075
#else
    RDA5888_WriteReg(0x12f, 0x0075);
#endif

    var = RDA5888_FieldReadReg(0x117, 0xfff8); // get 117H_15~3bits

#ifdef RDA5888_SOFTAFC_ENABLE
    if (var < XTAL_117H_TH)
    {
        xtal_bank_process();
    }
#endif

#ifdef RDA5888_MULTIPATH_ENABLE
    multi_path_process(var);
#endif

#ifdef RDA5888_LPFSWITCH_ENABLE
    LPF_switch_process(var);
#endif

    if(!get_mid(var, &midvar))
    {
        return;
    }

    if(!average(midvar, &AveVar))
    {
        return;
    }

    index = search_blend_table(AveVar);
    if(index == 0xff)
        return;

#ifdef RDA5888_YCADJUST_ENABLE
    RDA5888_FieldWriteReg(0x10b, 0x07f0, BlendTable[g_nBlendMode][index].yc_adjust);
#endif

    RDA5888_FieldWriteReg(0x11f, 0x003f, BlendTable[g_nBlendMode][index].vt_fail_th);

#ifdef RDA5888_FRAMECOEF_ENABLE
    RDA5888_FieldWriteReg(0x1c3, 0x03ff, BlendTable[g_nBlendMode][index].frame_coef);
#endif

    //RDAEXT_DebugPrint(RDA5888_TRACE_MOD "[rdamtv]soft_blending, AveVar=%d, index = %d\n", AveVar, index);
}

#ifdef RDA5888_SECAM_SOFTMUTE_ENABLE
#define SECAM_SOFTMUTE_TBL_LENGTH    5
#define SECAM_SOFTMUTE_OFFSET       (0)
static BlendStruct secam_softmute_table[SECAM_SOFTMUTE_TBL_LENGTH] =
{
    //       min                        max          Y/C   vt_f  coef
    {0,                          30 + SECAM_SOFTMUTE_OFFSET, 0x22, 0x06, 0x0a3}, // index_0
    {32 + SECAM_SOFTMUTE_OFFSET, 35 + SECAM_SOFTMUTE_OFFSET, 0x2E, 0x25, 0x0c2}, // index_1
    {36 + SECAM_SOFTMUTE_OFFSET, 41 + SECAM_SOFTMUTE_OFFSET, 0x2C, 0x34, 0x0c2}, // index_2
    {42 + SECAM_SOFTMUTE_OFFSET, 45 + SECAM_SOFTMUTE_OFFSET, 0x4A, 0x3f, 0x0e1}, // index_3
    {46 + SECAM_SOFTMUTE_OFFSET, 255,                        0x78, 0x3f, 0x0e1}  // index_4
};

static uint8 search_softmute_table(uint16 var, BlendStruct *tbl)
{
    uint8 idx;
    static uint8 last_idx = 0;
    static uint8 cnt = 0;

    for (idx = 0; idx < SECAM_SOFTMUTE_TBL_LENGTH; idx++)
    {
        if ((var >= tbl[idx].Lim_Low) && (var < tbl[idx].Lim_UP))
            break;
    }

    RDAEXT_DebugPrint(RDA5888_TRACE_MOD "[rdamtv]softmute_secam_1, idx = %d, %d~%d\n", idx, tbl[idx].Lim_Low, tbl[idx].Lim_UP);

    if (idx == last_idx)
    {
        if ((cnt++) > 3)
            return idx;
    }
    else
    {
        cnt = 0;
        last_idx = idx;
    }

    return 0xff;
}

static void softmute_secam(void)
{
    uint16 reg;
    uint16 noise = 0;
    uint8 index = 0;

    // read noise
    noise = RDA5888_FieldReadReg(0x1ff, 0x007f);

    index = search_softmute_table(noise, secam_softmute_table);
    if(index == 0xff)
        return;

#ifdef RDA5888_YCADJUST_ENABLE
    RDA5888_FieldWriteReg(0x10b, 0x00f0, secam_softmute_table[index].yc_adjust); // yc:0x07f0, c: 0x00f0
#endif

    RDAEXT_DebugPrint(RDA5888_TRACE_MOD "[rdamtv]softmute_secam, noise = %d, index = %d\n", noise, index);
}
#endif // end of RDA5888_SECAM_SOFTMUTE_ENABLE

#endif // end of RDA5888_SOFTBLENDING_ENABLE

#ifdef RDA5888_INCHFILTER_ENABLE
#define CHANNEL_FILTER_SAMP_NUM     11
#define CHANNEL_FILTER_THRESHOLD   145

static int InvalidChannelFilter(void)
{
    int var[CHANNEL_FILTER_SAMP_NUM], i, j, temp, exchange;
    int rt = RDAMTV_RT_SCAN_DONE;

    RDAEXT_DelayMs(100);

    RDA5888_WriteReg(0x12f, 0x0076);
    for (i = 0; i < CHANNEL_FILTER_SAMP_NUM; i++)
    {
        RDAEXT_DelayMs(10);
        var[i] = RDA5888_FieldReadReg(0x117, 0x7FF8);
    }

    for (i = 0; i < CHANNEL_FILTER_SAMP_NUM; i++)
    {
        exchange = 0;
        for (j = CHANNEL_FILTER_SAMP_NUM-1; j > i; j--)
        {
            if (var[j] > var[j-1])
            {
                exchange = 1;
                temp = var[j];
                var[j] = var[j-1];
                var[j-1] = temp;
            }
        }
        if (!exchange)
            break;
    }

    temp = var[CHANNEL_FILTER_SAMP_NUM/2];

    RDAEXT_DebugPrint(RDA5888_TRACE_MOD "[rdamtv]InvalidChannel, temp = %d, var = %d, %d, %d, %d, %d, %d, %d\n",
                      temp, var[0],var[1],var[2],var[3],var[4],var[5],var[6]);

    if (temp > CHANNEL_FILTER_THRESHOLD)
    {
        rt = RDAMTV_RT_SCAN_FAIL;
        RDAEXT_DebugPrint(RDA5888_TRACE_MOD "[rdamtv]InvalidChannel done1!!\n");
    }
    else if (temp == 0)
    {
        RDA5888_WriteReg(0x12f, 0x0050);
        i = RDA5888_FieldReadReg(0x117, 0x0780);

        if ((i != 8)&&(i != 9)) // sync lock fail
        {
            rt = RDAMTV_RT_SCAN_FAIL;
            RDAEXT_DebugPrint(RDA5888_TRACE_MOD "[rdamtv]InvalidChannel done2!!!!\n");
        }
    }

    return rt;
}
#endif

static uint8 SetChannelAux(uint32 nChannel, uint32 nFlag)
{
    int i = 0;
    int rt = RDAMTV_RT_SCAN_FAIL;

    if ((nChannel == 0) || (p_rdamtv_cur_map == NULL))
    {
        RDA5888_ScanChannel(471250000, RDAMTV_VSTD_PAL_D, 0);
#if ((RDA5888_CHIP_VER == 4) ||(RDA5888_CHIP_VER == 5))
        RDA5888_WriteReg(0x130, 0x0803);
#endif
        return rt;
    }

    while (p_rdamtv_cur_map[i].chn != 0)
    {
        if (p_rdamtv_cur_map[i].chn == nChannel)
        {
            rt = RDA5888_ScanChannel(p_rdamtv_cur_map[i].freq, p_rdamtv_cur_map[i].chstd, nFlag);
            break;
        }
        i++;
    }

    if (p_rdamtv_cur_map[i].chn == 0)
    {
        RDAEXT_DebugPrint(RDA5888_TRACE_MOD "SetChannelAux, channel not exist");
        RDA5888_SetTestPattern();
    }

#ifdef RDA5888_INCHFILTER_ENABLE
    if ((rt == RDAMTV_RT_SCAN_DONE) && (g_nBlendMode != BLENDMODE_INVALID) && (1 == nFlag))
    {
        rt = InvalidChannelFilter();
    }
#endif

#ifdef RDA5888_VIDENHANCE_ENABLE
    g_nFirstBufferFlag = 5;
#endif

    return (rt);
}

static void CheckSignalLevel(void)
{
    short rssi = 0;
#ifdef RDA5888_VIDENHANCE_ENABLE
    //static uint8 weak_sig_cnt = 0;
#endif

    rssi = RDA5888_GetRSSI();

    if (rssi <= -85)
    {
        g_nSignalLevel = 0;
    }
    else if ((rssi >= -84)&&(rssi <= -78))
    {
        g_nSignalLevel = 1;
    }
    else if ((rssi >= -77)&&(rssi <= -73))
    {
        g_nSignalLevel = 2;
    }
    else if ((rssi >= -72)&&(rssi <= -63))
    {
        g_nSignalLevel = 3;
    }
    else if (rssi >= -62)
    {
        g_nSignalLevel = 4;
    }

#ifdef RDA5888_VIDENHANCE_ENABLE
    if (1 == g_nVideoEnhanceEnable)
    {
        // image sharp process.
        if (RDAAPP_Is625Lines())
        {
            m_nImageSharpEn = 1;
        }
        else
        {
            m_nImageSharpEn = 0;
        }

        if (g_nFrameBufSize < 102400)
        {
            g_nDiscardFrameNum = 2;
        }
        else
        {
            g_nDiscardFrameNum = 3;
        }
    }
    else
    {
        m_nImageSharpEn = 0;
        g_nDiscardFrameNum = 2;
    }
#endif

#ifdef RDA5888_VIDENHANCE_ENABLE
    RDAEXT_DebugPrint(RDA5888_TRACE_MOD "[rdamtv]rssi = %d, sig_level = %d, video_en = %d, dis_fra = %d, fra_size = %d, tp_mode = %d\n",
                      rssi, g_nSignalLevel, m_nImageSharpEn, g_nDiscardFrameNum, g_nFrameBufSize, m_nIsTPMode);
#else
    RDAEXT_DebugPrint(RDA5888_TRACE_MOD "[rdamtv]rssi = %d, sig_level = %d, tp_mode = %d\n", rssi, g_nSignalLevel, m_nIsTPMode);
#endif
}

//*****************************************************************************
//**************** START OF PUBLIC PROCEDURE IMPLEMENTATIONS ******************
//*****************************************************************************

extern void* memset(void *dst0,int c0,size_t length);

// ----------------------------------------------------------------------------
// This function sets up the internal state of this component.
//
// Params:
//  none.
//
// Return:
//  none.
// ----------------------------------------------------------------------------
void RDAAPP_Init(void)
{
    RDAEXT_DebugPrint(RDA5888_TRACE_MOD  "[rdamtv]CHIP_VER: %d, LIB_VER: %d.%d.%d", RDA5888_CHIP_VER,
                      RDA5888DRV_VER_MAJOR, RDA5888DRV_VER_MINOR, RDA5888DRV_VER_PATCH);

    memset(&g_stRdamtvCtx, 0x00, sizeof(rdamtv_ctx_t));

    g_stRdamtvCtx.work_mode = RDAMTV_MODE_ANALOG_TV;

    RDA5888_DCDCInit();
#if ((RDA5888_CHIP_VER == 6) || (RDA5888_CHIP_VER == 7))
    RDA5888_DSPInit();
#ifdef RDA5888_26MCRYSTAL_ENABLE
    RDA5888_26MCrystalInit();
#endif
#endif
    atvd_SetVolume(10);
}

// ----------------------------------------------------------------------------
// This function performs cyclic processing for signal check, soft blending,
// and other operations. the system want to call this function periodic.
//
// Params:
//  none.
//
// Return:
//  none.
// ----------------------------------------------------------------------------
void RDAAPP_TimerHandler(void)
{
#ifdef RDA5888_SYNC_CHECK_ENABLE
    uint16 var;
#endif
    static uint8 check_time = 0;
#ifdef RDA5888_SYNC_CHECK_ENABLE
    static uint8 sync_fail_time = 0;
#endif

    check_time++;
    if (check_time > 5)
    {
        check_time = 1;
        CheckSignalLevel();
    }

#ifdef RDA5888_SOFTBLENDING_ENABLE
    if (g_nBlendMode != BLENDMODE_INVALID)
    {
        soft_blending();
    }
#ifdef RDA5888_SECAM_SOFTMUTE_ENABLE
    else
    {
        softmute_secam();
    }
#endif
#endif

#ifdef RDA5888_SYNC_CHECK_ENABLE
#if ((RDA5888_CHIP_VER == 6) || (RDA5888_CHIP_VER == 7))
    var = RDA5888_FieldReadReg(0x1b1, 0x8000);

    if (var == 1) // sync locked
    {
        sync_fail_time = 0;
    }
    else if (sync_fail_time < 255)
    {
        sync_fail_time++;
    }

    if (sync_fail_time > CHECK_SYNC_FAIL_TIMES)
    {
        if (!m_nIsTPMode)
        {
            m_nIsTPMode = 1;
        }
    }
    else if (m_nIsTPMode)
    {
        m_nIsTPMode = 0;
    }
#else
    RDA5888_WriteReg(0x12f, 0x0050);
    var = RDA5888_FieldReadReg(0x117, 0x0780);

    if ((var == 8)||(var == 9)) // sync locked
    {
        sync_fail_time = 0;
    }
    else if (sync_fail_time < 255)
    {
        sync_fail_time++;
    }


    if (sync_fail_time > CHECK_SYNC_FAIL_TIMES)
    {
        if (!m_nIsTPMode)
        {
            RDA5888_ReadReg(0x130, &m_nTPRegBak);
            RDA5888_WriteReg(0x130, 0x803);
            RDA5888_ReadReg(0x116, &g_n116RegBak);
            RDA5888_WriteReg(0x116, 0x0271);
            m_nIsTPMode = 1;
        }
    }
    else if (m_nIsTPMode)
    {
        RDA5888_WriteReg(0x130, m_nTPRegBak);
        RDA5888_WriteReg(0x116, g_n116RegBak);
        m_nIsTPMode = 0;
    }
#endif
#endif

#if defined(__MTK_TARGET__)
    GPTI_StopItem(g_nRdamtvTimer);
    GPTI_StartItem(g_nRdamtvTimer, kal_milli_secs_to_ticks(RDAAPP_TIMER_CYCLE), RDAAPP_TimerHandler,0);
#elif defined(SPRD_CUSTOMER)
    if ((g_ptRdaAtvTimer != NULL) && (!SCI_IsTimerActive(g_ptRdaAtvTimer)))
    {
        SCI_ChangeTimer(g_ptRdaAtvTimer, RDAAPP_TimerHandler, RDAAPP_TIMER_CYCLE);
        SCI_ActiveTimer(g_ptRdaAtvTimer);
    }
#endif
}

// ----------------------------------------------------------------------------
// This function sets up channel map, works mode, and video standard.
//
// Params:
//  pstChMap:  channel map to set.
//  nWorkMode: current work mode.
//  nVidStd: current video standard.
//
// Return:
//  none.
// ----------------------------------------------------------------------------
void RDAAPP_SetChannelMap(const rdamtv_channel_t *pstChMap, uint32 nWorkMode, uint32 nVidStd)
{
    uint32 i;

    g_stRdamtvCtx.vidstd = nVidStd;
    p_rdamtv_cur_map = pstChMap;
    g_stRdamtvCtx.work_mode = nWorkMode;
    g_nCurChCnt = 0;

    for (i = 0; p_rdamtv_cur_map[i].chn != 0; i++)
    {
        g_nCurChCnt++;
    }
    RDAEXT_DebugPrint(RDA5888_TRACE_MOD "RDAAPP_SetChannelMap, channel counter = %d\n", g_nCurChCnt);
}

// ----------------------------------------------------------------------------
// This function sets current country or area.
//
// Params:
//  cTVArea: Area to set.
//
// Return:
//  none.
// ----------------------------------------------------------------------------
void RDAAPP_SetTVArea(uint32 cTVArea)
{
    m_nCurTvArea = cTVArea;

    RDAEXT_DebugPrint(RDA5888_TRACE_MOD "RDAAPP_SetTVArea, tv_area=%d", m_nCurTvArea);

    switch (cTVArea)
    {
        case RDAMTV_AREA_RUSSIA:
            RDAAPP_SetChannelMap(rdamtv_secam_d_k_chn_map, RDAMTV_MODE_ANALOG_TV, RDAMTV_VSTD_SECAM_D);
            break;

        case RDAMTV_AREA_CHINA:
            RDAAPP_SetChannelMap(rdamtv_pald_chn_map, RDAMTV_MODE_ANALOG_TV, RDAMTV_VSTD_PAL_D);
            break;

        case RDAMTV_AREA_CHINA_SHENZHEN:
            RDAAPP_SetChannelMap(rdamtv_pald_sz_map, RDAMTV_MODE_ANALOG_TV, RDAMTV_VSTD_PAL_I);
            break;

        case RDAMTV_AREA_CHINA_HONGKONG:
        case RDAMTV_AREA_UK:
        case RDAMTV_AREA_SOUTH_AFRICA:
        case RDAMTV_AREA_CHINA_MACAO:
            RDAAPP_SetChannelMap(rdamtv_a_hongkong_chn_map, RDAMTV_MODE_ANALOG_TV, RDAMTV_VSTD_PAL_I);
            break;

        case RDAMTV_AREA_CHINA_TAIWAN:
        case RDAMTV_AREA_AMERICA:
        case RDAMTV_AREA_MEXICO:
        case RDAMTV_AREA_PHILIPINES:
        case RDAMTV_AREA_KOREA:
        case RDAMTV_AREA_CHILE:
        case RDAMTV_AREA_VENEZUELA:
        case RDAMTV_AREA_CANADA:
            RDAAPP_SetChannelMap(rdamtv_ntsc_chn_map, RDAMTV_MODE_ANALOG_TV, RDAMTV_VSTD_NTSC_M);
            break;

        case RDAMTV_AREA_ARGENTINA:
            RDAAPP_SetChannelMap(rdamtv_argentina_chn_map, RDAMTV_MODE_ANALOG_TV, RDAMTV_VSTD_PAL_NC);
            break;

        case RDAMTV_AREA_BRAZIL:
            RDAAPP_SetChannelMap(rdamtv_brazil_chn_map, RDAMTV_MODE_ANALOG_TV, RDAMTV_VSTD_PAL_M);
            break;

        case RDAMTV_AREA_JAPAN:
            RDAAPP_SetChannelMap(rdamtv_ntsc_japan_chn_map, RDAMTV_MODE_ANALOG_TV, RDAMTV_VSTD_NTSC_M);
            break;

        case RDAMTV_AREA_WESTERNEUROP:
        case RDAMTV_AREA_TURKEY:
        case RDAMTV_AREA_AFGHANISTA:
        case RDAMTV_AREA_SINGAPORE:
        case RDAMTV_AREA_THAI:
        case RDAMTV_AREA_CAMBODIA:
        case RDAMTV_AREA_INDONESIA:
        case RDAMTV_AREA_MALAYSIA:
        case RDAMTV_AREA_LAOS:
        case RDAMTV_AREA_INDIA:
        case RDAMTV_AREA_PAKISTAN:
        case RDAMTV_AREA_SPAIN:
        case RDAMTV_AREA_PORTUGAL:
        case RDAMTV_AREA_YEMEN:
        case RDAMTV_AREA_BAHRAIN:
        case RDAMTV_AREA_ABU_DHABI:
        case RDAMTV_AREA_KUWAIT:
            RDAAPP_SetChannelMap(rdamtv_w_europe_chn_map, RDAMTV_MODE_ANALOG_TV, RDAMTV_VSTD_PAL_B);
            break;

        case RDAMTV_AREA_VIETNAM:
            RDAAPP_SetChannelMap(rdamtv_vietnam_chn_map, RDAMTV_MODE_ANALOG_TV, RDAMTV_VSTD_PAL_D);
            break;

        case RDAMTV_AREA_BURMA:
            RDAAPP_SetChannelMap(rdamtv_burma_chn_map, RDAMTV_MODE_ANALOG_TV, RDAMTV_VSTD_NTSC_M);
            break;

        //for factory test
        case RDAMTV_AREA_FACTORYMODE:
            RDAAPP_SetChannelMap(rdamtv_factory_chn_map, RDAMTV_MODE_ANALOG_TV, RDAMTV_VSTD_PAL_D);
            break;

        default:
            m_nCurTvArea = RDAMTV_AREA_CHINA;
            RDAAPP_SetChannelMap(rdamtv_pald_chn_map, RDAMTV_MODE_ANALOG_TV, RDAMTV_VSTD_PAL_D);
            break;
    }
}

// ----------------------------------------------------------------------------
// This function sets current country or area.
//
// Params:
//  none.
//
// Return:
//  current area.
// ----------------------------------------------------------------------------
uint32 RDAAPP_GetTVArea(void)
{
    return m_nCurTvArea;
}

// ----------------------------------------------------------------------------
// This function sets a channel or scan a channel.
//
// Params:
//  nChannel: channel index to set.
//  nFlag:    0: set channel, 1: scan channel.
//
// Return:
//  TRUE: channel search done, FALSE: channel search fail.
// ----------------------------------------------------------------------------
uint8 RDAAPP_SetChannel(uint32 nChannel, uint32 nFlag)
{
    uint8 rt = 0;

#if defined(__MTK_TARGET__)
    DISABLE_VIEW_FINDER_MODE;
    GPTI_StopItem(g_nRdamtvTimer);
#elif defined(SPRD_CUSTOMER)
    if ((g_ptRdaAtvTimer != NULL) && (SCI_IsTimerActive(g_ptRdaAtvTimer)))
    {
        SCI_TRACE_LOW("Timer deactive\n");
        SCI_DeactiveTimer(g_ptRdaAtvTimer);
    }
#endif

    rt = SetChannelAux(nChannel, nFlag);

    if (rt == RDAMTV_RT_SCAN_FAIL)
    {
        RDAEXT_DebugPrint(RDA5888_TRACE_MOD "[rdamtv]RDAAPP_SetChannel, fail! nChannel = %d\n", nChannel);
        m_nIsTPMode = 1;
#if ((RDA5888_CHIP_VER == 4) ||(RDA5888_CHIP_VER == 5))
        RDA5888_ReadReg(0x130, &m_nTPRegBak);
        RDA5888_WriteReg(0x130, 0x803);
        RDA5888_ReadReg(0x116, &g_n116RegBak);
        RDA5888_WriteReg(0x116, 0x0271);
        //RDA5888_FieldWriteReg(0x1b1, 0x0008, 1);
        //   atvd_SetVolume(0);

#endif
        rt = FALSE;
    }
    else
    {
        //   atvd_SetVolume(10);
        RDAEXT_DebugPrint(RDA5888_TRACE_MOD "[rdamtv]RDAAPP_SetChannel, done! nChannel = %d\n", nChannel);
        m_nIsTPMode = 0;
        rt = TRUE;
    }

#if defined(__MTK_TARGET__)
    GPTI_StopItem(g_nRdamtvTimer);
    GPTI_StartItem(g_nRdamtvTimer,kal_milli_secs_to_ticks(100),RDAAPP_TimerHandler,0);
    ENABLE_VIEW_FINDER_MODE;
#elif defined(SPRD_CUSTOMER)
    if ((g_ptRdaAtvTimer != NULL) && (!SCI_IsTimerActive(g_ptRdaAtvTimer)))
    {
        SCI_ChangeTimer(g_ptRdaAtvTimer, RDAAPP_TimerHandler, RDAAPP_TIMER_CYCLE);
        SCI_ActiveTimer(g_ptRdaAtvTimer);
    }
#endif

    return rt;
}

// ----------------------------------------------------------------------------
// This function sets audio mute enable.
//
// Params:
//  bMute: 0: unmute, 1: mute.
//
// Return:
//  none.
// ----------------------------------------------------------------------------
void RDAAPP_SetMute(bool bMute)
{
    if (bMute) // mute
    {
        RDA5888_WriteReg(0x10, 0xc500);  // close adac
    }
    else // unmute
    {
        RDA5888_WriteReg(0x10, 0x8500);  // open adac
    }
}

// ----------------------------------------------------------------------------
// This function sets the brightness for current channel being processed.
//
// Params:
//  nLevel: 0~15
//
// Return:
//  none.
// ----------------------------------------------------------------------------
void RDAAPP_SetBright(uint32 nLevel)
{
    if (nLevel > 15)
        nLevel = 15;

    RDA5888_SetLumaLevel(nLevel);
}

// ----------------------------------------------------------------------------
// This function sets volume value for audio output.
//
// Params:
//  nLevel: 0~40
//
// Return:
//  none.
// ----------------------------------------------------------------------------
void RDAAPP_SetVolume(uint16 nValue)
{
#if 1
    if (nValue > 40)
        nValue = 40;

    RDA5888_SetVolume(10);
#else
    uint16 arrVolMap[] = {0x0, 0x0003, 0x0008, 0x000c,  0x0012, 0x0015, 0x0018, 0x001b, 0x001e, 0x0025};

    if (nValue > 9)
        nValue = 9;

    RDA5888_SetVolume(arrVolMap[nValue]);
#endif
}

// ----------------------------------------------------------------------------
// This function gets current signal level.
//
// Params:
//  none.
//
// Return:
//  current signal level.
// ----------------------------------------------------------------------------
uint8 RDAAPP_GetSignalLevel(void)
{
    uint16 rssi = 0;

    rssi = RDA5888_GetRSSI();

    if (rssi <= -85)
    {
        g_nSignalLevel = 0;
    }
    else if ((rssi >= -84)&&(rssi <= -78))
    {
        g_nSignalLevel = 1;
    }
    else if ((rssi >= -77)&&(rssi <= -73))
    {
        g_nSignalLevel = 2;
    }
    else if ((rssi >= -72)&&(rssi <= -63))
    {
        g_nSignalLevel = 3;
    }
    else if (rssi >= -62)
    {
        g_nSignalLevel = 4;
    }

    return g_nSignalLevel;
}

// ----------------------------------------------------------------------------
// This function gets total number of channels for current frequencies table.
//
// Params:
//  none.
//
// Return:
//  total number of channels.
// ----------------------------------------------------------------------------
uint8 RDAAPP_GetChannelCount(void)
{
    return g_nCurChCnt;
}

// ----------------------------------------------------------------------------
// This function exit and clear internal state of this component.
//
// Params:
//  none.
//
// Return:
//  none.
// ----------------------------------------------------------------------------
void RDAAPP_Exit(void)
{
    RDAEXT_DebugPrint(RDA5888_TRACE_MOD  "RDAAPP_Exit");

    RDA5888_PowerDown();

    g_stRdamtvCtx.work_mode = RDAMTV_MODE_NONE;

#if defined(__MTK_TARGET__)
    GPTI_StopItem(g_nRdamtvTimer);
#elif defined(SPRD_CUSTOMER)
    if (g_ptRdaAtvTimer != NULL)
    {
        SCI_DeleteTimer(g_ptRdaAtvTimer);
        g_ptRdaAtvTimer = NULL;
    }
#endif
}

// --------------------------------------------------------------------------------
// wuhp@2009-12-19, add video enhance process for rda5888.
// --------------------------------------------------------------------------------
#ifdef RDA5888_VIDENHANCE_ENABLE
extern void ImageSharp_asm(unsigned short *pSrc, unsigned int Size);
extern ImageAvg_asm(unsigned short *pSrc1,unsigned short *pSrc2, unsigned int Size);
extern ImageSharpAvg_asm(unsigned short *pSrc, unsigned short *pSrc1,unsigned int Size);

#if 0
static void ImageSharpProcess(unsigned short *pSrc, unsigned int Size)
{
    unsigned short *pBuf1 = pSrc;
    unsigned int i;
    short r, g, b;

    // video sharpening process.
    //for (i = 0; i < height/2; i++)
    {
        //for (j = 0; j < (width-2); j++)
        for (i = 1; i < Size/2; i++)
        {
            r = (4*((pBuf1[i]&R_MASK)>>R_SHIFT) - ((pBuf1[i-1]&R_MASK)>>R_SHIFT) - ((pBuf1[i+1]&R_MASK)>>R_SHIFT))/2;
            g = (4*((pBuf1[i]&G_MASK)>>G_SHIFT) - ((pBuf1[i-1]&G_MASK)>>G_SHIFT) - ((pBuf1[i+1]&G_MASK)>>G_SHIFT))/2;
            b = (4*((pBuf1[i]&B_MASK)>>B_SHIFT) - ((pBuf1[i-1]&B_MASK)>>B_SHIFT) - ((pBuf1[i+1]&B_MASK)>>B_SHIFT))/2;

            r = ((r < 0) ? 0 : ((r > 31) ? 31 : r));
            g = ((g < 0) ? 0 : ((g > 63) ? 63 : g));
            b = ((b < 0) ? 0 : ((b > 31) ? 31 : b));

            pBuf1[i] = ((r<<R_SHIFT)&R_MASK) | ((g<<G_SHIFT)&G_MASK) | ((b<<B_SHIFT)&B_MASK);
        }
        //pBuf3 += width;
    }
}
#endif

#if (RDA5888_CHIP_VER == 4)
static void FrameAverageProcess(unsigned int buffer1, unsigned int buffer2, unsigned int size)
{
    unsigned int *pBuf1 = (unsigned int *)buffer1;
    unsigned int *pBuf2 = (unsigned int *)buffer2;
    unsigned int i;

    size = size >> 3;

    for (i = size; i != 0; i--)
    {
        *(pBuf1++) = ((*pBuf1 & RGB_MASK) >> 1) + ((*(pBuf2++) & RGB_MASK) >> 1);
        *(pBuf1++) = ((*pBuf1 & RGB_MASK) >> 1) + ((*(pBuf2++) & RGB_MASK) >> 1);
    }
}
#endif

void random_patten(unsigned int *buffer1, unsigned int Size)
{
    unsigned int i, temp, j;
    static unsigned int reseed = 0xffffffff;

    for (i = 0; i < Size/4; i+=2 )
    {
        temp=(reseed>>3)^reseed;
        reseed=(reseed<<16)|((temp>>13)&0xffff);
        temp=reseed&0xffff;
        for(j=0; j<2; j++)
        {
            buffer1[i+j] =  (((temp&0x0f)<<R_SHIFT))
                            |(((temp&0x0f)<<(G_SHIFT+1)))
                            | (((temp&0x0f)<<B_SHIFT))
                            | (temp&0xf0)<<(R_SHIFT+12)
                            |(((temp&0xf0)<<(G_SHIFT+1+12)))
                            |(((temp&0xf0)<<(B_SHIFT+12)));

            temp>>=8;
        }
    }
}

void RDAAPP_VideoEnhance(unsigned int buffer1, unsigned int buffer2, unsigned int size)
{
    unsigned int temp;

#if defined(RDA5888_SNOWPATTERN_ENABLE)
    if (m_nIsTPMode)
    {
        random_patten((unsigned int * )buffer1, size);
        return;
    }
#endif

    g_nFrameBufSize = size;

    if (g_nFirstBufferFlag)
    {
        g_nFirstBufferFlag--;
        return;
    }

    if (g_nVideoEnhanceEnable)
    {
#if defined(SPRD_CUSTOMER)
        temp = SCI_GetTickCount();
#elif defined(__MTK_TARGET__)
        //temp = drv_get_current_time();
#endif

#if defined(RDA5888_SHARPENING_ENABLE)
        if (m_nImageSharpEn)
        {
            //ImageSharpAvg_asm((unsigned short *)buffer1, (unsigned short *)buffer2, size);
            ImageSharp_asm((unsigned short *)buffer1, size);
        }
#endif

#if (RDA5888_CHIP_VER == 4)
        FrameAverageProcess(buffer1, buffer2, size);
#endif

#if defined(SPRD_CUSTOMER)
        temp = SCI_GetTickCount() - temp;
#elif defined(__MTK_TARGET__)
        //temp = drv_get_current_time() - temp;
#endif

        //RDAEXT_DebugPrint(RDA5888_TRACE_MOD  "[rdamtv]RDAAPP_VideoEnhance, size = %d, time = %d", size, temp);
    }
}

// function: set video enhance process
// parameters: t = 0: close video enhance function, t = 1: open video enhance function.
void RDAAPP_SetVideoEnhance(bool t)
{
    RDAEXT_DebugPrint(RDA5888_TRACE_MOD  "[rdamtv]RDAAPP_SetVideoEnhance, t = %d", t);

    g_nVideoEnhanceEnable = t;
}

uint8 RDAAPP_GetVideoEnhance(void)
{
    return g_nVideoEnhanceEnable;
}
#endif // end RDA5888_VIDENHANCE_ENABLE

// return 1: test pattern, 0: normal signal
uint16 RDAAPP_GetTPMode(void)
{
    return m_nIsTPMode;
}

// return 1: 625 lines, 0: 525 lines
#if 1
uint8 RDAAPP_Is625Lines(void)
{
    uint8 rt = 1;

    switch (p_rdamtv_cur_map[0].chstd)
    {
        case RDAMTV_VSTD_PAL_M:
        case RDAMTV_VSTD_NTSC_B_G:
        case RDAMTV_VSTD_NTSC_D_K:
        case RDAMTV_VSTD_NTSC_I:
        case RDAMTV_VSTD_NTSC_M:
            rt = 0;
            break;

        default:
            break;
    }

    return rt;
}
#else
uint8 RDAAPP_Is625Lines(void)
{
    uint8 rt = 1;

    switch (g_stRdamtvCtx.vidstd)
    {
        case RDAMTV_VSTD_PAL_M:
        case RDAMTV_VSTD_NTSC_B_G:
        case RDAMTV_VSTD_NTSC_D_K:
        case RDAMTV_VSTD_NTSC_I:
        case RDAMTV_VSTD_NTSC_M:
            rt = 0;
            break;

        default:
            break;
    }

    return rt;
}
#endif

