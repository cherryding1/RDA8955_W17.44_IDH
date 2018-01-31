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
#ifdef RDA5888_FM_ENABLE
#include "rda5888fm.h"
#if defined(__MTK_TARGET__)
#include "rda5888mtk.h"
#elif defined(SPRD_CUSTOMER)
#include "rda5888sprd.h"
#endif

// for rda5888d
#define FM_RSSI_OFFSET            57
#define FM_SEEK_TIMER             20
#define FM_SEEK_THRESHOLD          5

// for rda5888s
#define FM_SEEK_THRESHOLD2        67 // default: 67, range: 61~68.
#define FM_SEEK_CHECK_TIMES        2

#ifdef RDA5888_32KFMMODE_ENALBE
#define FM_READY_CHECK_TIMES      35
#else
#define FM_READY_CHECK_TIMES      10
#endif

void RDA_FMEnable(void)
{
#if (RDA5888_CHIP_VER == 7)
    RDA5888_FieldWriteReg(0x082, 0x0001, 0); //reg:0x1e0[1]->0
    RDAEXT_DelayMs(5);
    RDA5888_FieldWriteReg(0x082, 0x0001, 1); //reg:0x1e0[1]->1
#else
    RDA5888_FieldWriteReg(0x1e0, 0x0002, 0); //reg:0x1e0[1]->0
    RDA5888_FieldWriteReg(0x1e0, 0x0002, 1); //reg:0x1e0[1]->1
#endif
}

#ifdef RDA5888_32KFMMODE_ENALBE
static void RDA_32KFMModeInit()
{
    // item2345235:5888h_fm
    RDA5888_WriteReg(0x00a,0x34ff); // 9'h00a,16'h34ff
    RDA5888_WriteReg(0x00b,0x0820); // 9'h00b,16'h0820
    RDA5888_WriteReg(0x00c,0x0820); // 9'h00c,16'h0820
    RDA5888_WriteReg(0x01b,0x1ee7); // 9'h01b,16'h1ee7;by 110115 for ECO1
    RDA5888_WriteReg(0x01d,0x4f10); // 9'h01d,16'h4f10
    RDA5888_WriteReg(0x037,0xf270); // 9'h037,16'hf270; by 110115 ya'nan
    RDA5888_WriteReg(0x106,0x603c); // 9'h106,16'h603c
    RDA5888_WriteReg(0x19b,0x0fc9); // 9'h19b,16'h0fc9
    RDA5888_WriteReg(0x082,0x0041); // 9'h082,16'h0041
    RDA5888_WriteReg(0x02e,0x990d); // 9'h02e,16'h990d;  by 110115 for ECO1
    RDA5888_WriteReg(0x02e,0xf90d); // 9'h02e,16'hf90d;  by 110115 for ECO1
    RDA5888_WriteReg(0x030,0x0807); // 9'h030,16'h0807;
    // wait 5ms;
    RDAEXT_DelayMs((uint16)5);
    RDA5888_WriteReg(0x030,0x080f); // 9'h030,16'h080f
}

#include "hal_sys.h"
#ifndef TV_USE_32K_CLK_PIN
static HAL_SYS_CLOCK_OUT_ID_T g_RdaFMClockOutId = HAL_SYS_CLOCK_OUT_ID_QTY;
#endif
void RDA_Open32KOnChip(void)
{
#ifdef TV_USE_32K_CLK_PIN
    hal_Sys32kClkOut(TRUE);
#else
    if(g_RdaFMClockOutId == HAL_SYS_CLOCK_OUT_ID_QTY)
    {
        g_RdaFMClockOutId = hal_SysClkOutOpen(HAL_SYS_CLOCK_OUT_FREQ_32K);
    }
    RDAEXT_DebugPrint(0,"RDA_Open32KOnChip   g_RdaFMClockOutId:%d ",g_RdaFMClockOutId);
#endif
}

void RDA_Close32KOnClip(void)
{
#ifdef TV_USE_32K_CLK_PIN
    hal_Sys32kClkOut(FALSE);
#else
    RDAEXT_DebugPrint(0,"RDA_Close32KOnClip   g_RdaFMClockOutId:%d ",g_RdaFMClockOutId);
    hal_SysClkOutClose(g_RdaFMClockOutId);
    g_RdaFMClockOutId = HAL_SYS_CLOCK_OUT_ID_QTY;
#endif
}
#endif

int RDA_FMIsReady(void)
{
    uint32 i;
    uint16 seek_ready;

    for (i = 0; i < FM_READY_CHECK_TIMES; i++)
    {
#if (RDA5888_CHIP_VER == 7)
        seek_ready = RDA5888_FieldReadReg(0x08b, 0x0080);
#else
        seek_ready = RDA5888_FieldReadReg(0x1fc, 0x1000);
#endif
        if (seek_ready)
            break;
        RDAEXT_DelayMs(5);
    }

    RDAEXT_DebugPrint(0, "[rdamtv]RDA_FMIsReady, i = %d\n", i);

    return seek_ready;
}

uint16 RDA5888FM_GetRSSI(void)
{
#if (RDA5888_CHIP_VER == 4)
    uint16 reg, i, data1, data2, data3, rssi[3];

    for (i = 0; i < 3; i++)
    {
        // 将寄存器12FH写成000ah，读取117H的14~8bit的值，记为D1，再将寄存器12FH写成0019h，
        // 读取117H的14~8bit的值，记为D2；
        RDA5888_WriteReg(0x12F,0x000a);
        data1 = RDA5888_FieldReadReg(0x117, 0x7f00);

        RDA5888_WriteReg(0x12F,0x0019);
        data2 = RDA5888_FieldReadReg(0x117, 0x7f00);

        //读取96H的2~0bit的值，此值即为档位值，根据此值获得档值对应的D3?
        data3 = RDA5888_FieldReadReg(0x96, 0x0007);

        switch (data3)
        {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
                data3 = 10;
                break;

            case 5:
                data3 = 28;
                break;

            case 6:
                data3 = 44;
                break;

            case 7:
                data3 = 55;
                break;

            default:
                break;
        }
        rssi[i]= data1 - data2 - data3 - FM_RSSI_OFFSET + 107;
    }

    return ((rssi[0]+rssi[1]+rssi[2])/3);
#elif ((RDA5888_CHIP_VER == 5) || (RDA5888_CHIP_VER == 6) || (RDA5888_CHIP_VER == 7))
    uint16 rssi;

    rssi = RDA5888_FieldReadReg(0x1ff, 0x3f80);

    return rssi;
#endif
}

void RDA5888FM_Init(void)
{
    RDAEXT_DebugPrint(0, "[rdamtv]RDAFM_Init");

    RDAEXT_DebugPrint(0, "[rdamtv]CHIP_VER: %d, LIB_VER: %d.%d.%d", RDA5888_CHIP_VER,
                      RDA5888DRV_VER_MAJOR, RDA5888DRV_VER_MINOR, RDA5888DRV_VER_PATCH);

    memset(&g_stRdamtvCtx, 0x00, sizeof(rdamtv_ctx_t));

    g_stRdamtvCtx.work_mode = RDAMTV_MODE_FM_RADIO;

    RDA5888_DCDCInit();

    //RDA5888FM_SetFreq(870);
    //RDAEXT_DelayMs((uint16)20);

    RDA5888_DSPInit();

#ifdef RDA5888_26MCRYSTAL_ENABLE
    RDA5888_26MCrystalInit();
#endif
#ifdef RDA5888_32KFMMODE_ENALBE
    RDA_32KFMModeInit();
#endif

#if (RDA5888_CHIP_VER == 4)
    //将寄存器183H的3bit置为1，寄存器105H的15bit置为1，向寄存器121H写入0020h；
    RDA5888_FieldWriteReg(0x183, 0x0008, 0x1); //reg:0x183[3]->1
    RDA5888_FieldWriteReg(0x105, 0x8000, 0x1); //reg:0x105[15]->1
    RDA5888_WriteReg(0x121,0x0020);
#elif (RDA5888_CHIP_VER == 5)
    // set fm seek threshold
    //RDA5888_WriteReg(0x1f4, 0x20ad); // 9'h1f4,16'h1d2d; bits <13:7>
    RDA5888_FieldWriteReg(0x1f4, 0x3f8, FM_SEEK_THRESHOLD2); // 9'h1f4,16'h1d2d; bits <13:7>
#elif (RDA5888_CHIP_VER == 6)
    //item4: FM mode
    RDA5888_WriteReg(0x103,0x020d); // 9'h103,16'h020d
    RDA5888_WriteReg(0x114,0xefe0); // 9'h114,16'hefe0;
    RDA5888_WriteReg(0x11e,0x61e2); // 9'h11e,16'h61e2

    // set fm seek threshold
    RDA5888_FieldWriteReg(0x1f4, 0x3f8, FM_SEEK_THRESHOLD2); // 9'h1f4,16'h1d2d; bits <13:7>
#elif (RDA5888_CHIP_VER == 7)
    // set fm seek threshold
    //RDA5888_FieldWriteReg(0x0a3, 0x3f8, FM_SEEK_THRESHOLD2); // 9'h1f4,16'h1d2d; bits <13:7>
    //lib3.09 add 2 regs
    RDA5888_WriteReg(0x103,0x020d); // 9'h103,16'h020d
    RDA5888_WriteReg(0x114,0xefe0); // 9'h114,16'hefe0;
#endif

#if ((RDA5888_CHIP_VER == 6) || (RDA5888_CHIP_VER == 7))
    atvd_SetVolume(30);
    //RDA5888_WriteReg(0x12f, 0x0076);  //0x0075
#else
    fmd_FieldWrite(0x125, 0xff80, 0x1b);
#endif

    //RDAEXT_DelayMs((uint16)200);
    //RDA5888FM_SetFreq(870);
}

// 99MHZ, 0x0a = 0xc537, 0x0b = 0x0168
void RDA5888FM_SetFreq(int16 curf )
{
    uint16 rega, regb;
    uint32 c_freq = curf;

    c_freq *= 100; // convert to khz.

    RDAEXT_DelayMs((uint16)50);
#ifndef RDA5888_32KFMMODE_ENALBE
    c_freq -= 3875; // couvert to center frequency.
#endif

    //RDAEXT_DebugPrint(0, "[rdamtv]RDAFM_SetFreq, rega = %x, regb = %x", rega, regb);

#if ((RDA5888_CHIP_VER == 6) || (RDA5888_CHIP_VER == 7))
    //set center freq
    c_freq = (c_freq << 10);
    regb = (uint16)((c_freq&0xffff0000)>>16);
    rega = (uint16)((c_freq&0x0000ffff));

    RDA5888_WriteReg(0x033, rega);
    RDA5888_WriteReg(0x032, regb);
#else
#ifdef RDA5888_26MCRYSTAL_ENABLE
    //set center freq
    regb = (uint16)(((unsigned long)c_freq<<8)/65000);
    rega = (uint16)((((unsigned long)((c_freq<<8) - (regb * 65000))) << 16) /65000);
#else
    //set center freq
    regb = (uint16)(((unsigned long)c_freq<<8)/67500);
    rega = (uint16)((((unsigned long)((c_freq<<8) - (regb * 67500))) << 16) /67500);
#endif

    RDA5888_WriteReg(0x0a, rega);
    RDA5888_WriteReg(0x0b, regb);
#endif


    // set rx on
#if ((RDA5888_CHIP_VER == 6) || (RDA5888_CHIP_VER == 7))
    RDA5888_FieldWriteReg(0x030, 0x0008, 0x0);
    RDAEXT_DelayMs(5);
    RDA5888_FieldWriteReg(0x030, 0x0008, 0x1);
#else
    RDA5888_FieldWriteReg(0x09e, 0x0001, 0x0);
    RDAEXT_DelayMs(5);
    RDA5888_FieldWriteReg(0x09e, 0x0001, 0x1);
#endif
#if (RDA5888_CHIP_VER == 6)
//caihanling
    RDA5888_WriteReg(0x1b1,0x0700); // 9'h11e,16'h61e2
    RDA5888_WriteReg(0x1b2,0x0000); // 9'h11e,16'h61e2
    //    114H＝ 0xEFE0
    //      11eH= 0x61E2
    RDA5888_WriteReg(0x114,0xefe0); // 9'h11e,16'h61e2
    RDA5888_WriteReg(0x11e,0x61e2); // 9'h11e,16'h61e2

    RDAEXT_DelayMs(5);
#endif
}

void RDA5888FM_SetMute(uint8 cMute)
{
    RDAEXT_DebugPrint(0, "[rdamtv]RDAFM_SetMute");

#if (RDA5888_CHIP_VER == 4)
    if (cMute)
    {
        RDA5888_WriteReg(0x10,0x0000);  // close adac  0xc500
    }
    else
    {
        RDA5888_WriteReg(0x10,0x8500);  // open adac
    }
#elif ((RDA5888_CHIP_VER == 5) || (RDA5888_CHIP_VER == 6))
    if (cMute)
    {
        RDA5888_FieldWriteReg(0x1e0, 0x0200, 1);
    }
    else
    {
        RDA5888_FieldWriteReg(0x1e0, 0x0200, 0);
    }
#elif (RDA5888_CHIP_VER == 7)
    if (cMute)
    {
        RDA5888_FieldWriteReg(0x082, 0x4000, 1);
    }
    else
    {
        RDA5888_FieldWriteReg(0x082, 0x4000, 0);
    }
#endif
}

uint8 RDA5888FM_GetSigLevel(int16 curf)
{
    uint8 rssi = 0;

    if (875 == curf)
    {
        rssi = 1;
    }
    else
    {
        rssi = RDA5888FM_GetRSSI();
    }

    RDAEXT_DebugPrint(0, "[rdamtv]RDAFM_GetSigLevel, rssi = %d", rssi);

    return rssi;
}

uint8 RDA5888FM_IsValidFreq(int16 freq)
{
#if (RDA5888_CHIP_VER == 4)
    static uint16 rssi_1, rssi_2, rssi_3;
    static int16 freq_back = 0;

    //RDAEXT_DebugPrint(0, "[rdamtv]RDAFM_IsValidFreq");

    if (freq_back+1 == freq) // seek next freq.
    {
        rssi_1 = rssi_2;
        rssi_2 = rssi_3;
        // get rssi_3
        RDA5888FM_SetFreq(freq+1);
        RDAEXT_DelayMs(FM_SEEK_TIMER);
        rssi_3 = RDA5888FM_GetRSSI();

        RDA5888FM_SetFreq(freq); // write back.
    }
    else if (freq_back-1 == freq) // seek prev freq.
    {
        rssi_3 = rssi_2;
        rssi_2 = rssi_1;
        // get rssi_1
        RDA5888FM_SetFreq(freq-1);
        RDAEXT_DelayMs(FM_SEEK_TIMER);
        rssi_1 = RDA5888FM_GetRSSI();

        RDA5888FM_SetFreq(freq); // write back.
    }
    else
    {
        // get rssi_1
        RDA5888FM_SetFreq(freq-1);
        RDAEXT_DelayMs(FM_SEEK_TIMER);
        rssi_1 = RDA5888FM_GetRSSI();

        // get rssi_3
        RDA5888FM_SetFreq(freq+1);
        RDAEXT_DelayMs(FM_SEEK_TIMER);
        rssi_3 = RDA5888FM_GetRSSI();

        // get rssi_2
        RDA5888FM_SetFreq(freq);
        RDAEXT_DelayMs(FM_SEEK_TIMER);
        rssi_2 = RDA5888FM_GetRSSI();
    }

    freq_back = freq;

    //RDAEXT_DebugPrint(0, "[rdamtv]RDAFM_IsValidFreq, rssi: %d, %d, %d", rssi_1, rssi_2, rssi_3);

    // remove 96.0MHZ channel
    if (freq-1 == 960)
    {
        rssi_1 = 0;
    }
    else if (freq == 960)
    {
        rssi_2 = 0;
    }
    else if (freq+1 == 960)
    {
        rssi_3 = 0;
    }

    if (((rssi_2 - rssi_1) >= FM_SEEK_THRESHOLD)
            &&((rssi_2 - rssi_3) >= FM_SEEK_THRESHOLD))
    {
        RDAEXT_DebugPrint(0, "[rdamtv]RDAFM_IsValidFreq, freq = %d, rssi = %d!!!", freq, rssi_2);
        return 1;
    }
    else
    {
        return 0;
    }

#elif ((RDA5888_CHIP_VER == 5) || (RDA5888_CHIP_VER == 6) || (RDA5888_CHIP_VER == 7))
    uint16 seek_done, cnt;

#if (RDA5888_CHIP_VER == 7)
    RDA5888_WriteReg(0x037,0xf230);
#endif

    RDA5888FM_SetFreq(freq);

    // check seek_done
    cnt = 0;
    do
    {
#if (RDA5888_CHIP_VER != 7)
        RDA_FMEnable();
#endif
        if(!RDA_FMIsReady())
            return 0; // wait seek ready timeout.

        RDAEXT_DelayMs(5);
#if (RDA5888_CHIP_VER == 7)
        seek_done = RDA5888_FieldReadReg(0x08b, 0x0100);
#else
        seek_done = RDA5888_FieldReadReg(0x1fc, 0x0800);
#endif
        if ((freq == 945) || (freq == 960) || (freq == 1080))
        {
            //seek_done &= RDA5888_FieldReadReg(0x1fc, 0x0004);
            seek_done = 0;
            break;
        }
        else
        {
            if (seek_done)
            {
                break; // done!!!
            }
        }
    }
    while(++cnt < FM_SEEK_CHECK_TIMES);

#if (RDA5888_CHIP_VER == 7)
    RDA5888_WriteReg(0x037,0xf270);
#endif

    RDAEXT_DebugPrint(RDA5888_TRACE_MOD "FMDrv_ValidStop, freq = %d, seek_done = %d, reg = %x\n", freq, seek_done);

    return seek_done;
#endif
}

bool RDA5888FM_IsValidChip(void)
{
    uint16 reg;

    RDAEXT_DebugPrint(0, "[rdamtv]RDAFM_IsValidChip");

    RDA5888_ReadReg(0x0,&reg);

    if( (0x31E2 == reg) || (0x5888 == reg))
    {
        return TRUE;
    }
    else
    {
        RDAEXT_DebugPrint(0, "[rdamtv]RDAFM_IsValidChip, check fail!");
        return FALSE;
    }
}

void RDA5888FM_Exit(void)
{
    RDAEXT_DebugPrint(0,  "RDA5888FM_Exit");

    RDA5888_PowerDown();
}

#endif
