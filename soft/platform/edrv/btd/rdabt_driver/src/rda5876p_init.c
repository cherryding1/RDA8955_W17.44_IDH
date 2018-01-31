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


#ifdef __BT_RDABT__
#include "rdabt_drv.h"
#include "edrvp_debug.h"


PUBLIC BOOL g_FMOpen_Flag = FALSE;
PUBLIC BOOL g_BTOpen_Flag = FALSE;

#ifdef __RDA_CHIP_R16_5876P__

extern uint16 rdabt_chipid,rdabt_revid;

const uint16 rdabt_phone_init_16[][2] =
{

};

const uint16 rdabt_rf_init_16[][2] =
{
    {0x003f,0x0000},   //
    {0x0001,0x1FFF},   //;//Padrv_gain_tb_en=1
    {0x0006,0x161C},   //;//Padrv_op_gain=11;Padrv_ibit_psk<3:0>=000;Padrv_input_range_psk<1:0>=11
    {0x0007,0x040D},   //;
    {0x0008,0x8326},   //;//padrv_lo_tune_psk[5:0]=10000;Lna_notch_en=1
    {0x0009,0x04B5},   //;//rmx_imgrej=1
    {0x000B,0x238F},   //;//Filter_cal_mode =1
    {0x000C,0x85E8},   //;//filter_bpmode<2:0>=101
    {0x000E,0x0920},   //;//Tx_cal_polarity=1
    {0x000F,0x8DB3},   //;//adc_iq_swap=1
    {0x0010,0x1402},   //;//tx_sys_cal_sel<1:0>=10
    {0x0012,0x5604},   //;//Padrv_opa_bandwidth[1:0]=10;16'h1604;//;padrv_input_range<1:0>=00
    {0x0014,0x4ECC},   //;
    {0x0015,0x5124},   //;pll_gain_rx<2:0> 100
    {0x0018,0x0812},   //;//pll_refmulti2_en=1;
    {0x0019,0x10C8},   //;//pll_adcclk_en=1
    {0x001E,0x3024},   //;//Pll_lpf_gain_tx<1:0>=00;Pll_pfd_res_tx<5:0>=100100
    {0x0023,0x9991},   //;PSK
    {0x0024,0x2369},   //;
    {0x0027,0x8881},   //;GFSK
    {0x0028,0x2358},   //;
    {0x0032,0x0E00},   //;//tx_dsp_reset_delay=2
    {0x003f,0x0001},   //
    {0x000A,0x0018},   //;//thermo_pll_vcoibit_8<3:0>=1000
    {0x0000,0x020f},   //;//80H,16'hd19f;
    {0x0001,0xf9cf},   //;
    {0x0002,0xfc2f},   //;
    {0x0003,0xf92f},   //;
    {0x0004,0xfa2f},   //;
    {0x0005,0xfc2f},   //;
    {0x0006,0xfb3f},   //;
    {0x0007,0x7fff},   //;
    {0x0018,0xFFFF},   //;
    {0x0019,0xFFFF},   //;
    {0x001A,0xFFFF},   //;
    {0x001B,0xFFFF},   //;
    {0x001C,0xFFFF},   //;
    {0x001D,0xFFFF},   //;
    {0x001E,0xFFFF},   //;
    {0x001F,0xFFFF},   //;padrv_gain;9FH,16'hFFEC;padrv_gain20101103;improve ACPR;
    /*{0x0022,0x0E93},   //;//default
    {0x0025,0x03E1},   //;//default
    {0x0026,0x47A5},   //;//default;set voltage=1.2v
    {0x0027,0x0108},   //;
    {0x0028,0x6800},   //;//default
    {0x002D,0x006A},   //;//default OFF_FM;
    {0x002F,0x1100},   //;//default
    {0x0032,0x88F9},   //;//TM=011;DN=1111*/
    {0x003f,0x0000},   //
};

const uint16 rdabt_rf_init_16_c[][2] =
{
    {0x003f,0x0000},   //
    {0x0001,0x1FFF},   //;//Padrv_gain_tb_en=1
    {0x0006,0x161C},   //;//Padrv_op_gain=11;Padrv_ibit_psk<3:0>=000;Padrv_input_range_psk<1:0>=11
    {0x0007,0x040D},   //;
    {0x0008,0x8326},   //;//padrv_lo_tune_psk[5:0]=10000;Lna_notch_en=1
    {0x0009,0x04B5},   //;//rmx_imgrej=1
    {0x000B,0x238F},   //;//Filter_cal_mode =1
    {0x000C,0x85E8},   //;//filter_bpmode<2:0>=101
    {0x000E,0x0920},   //;//Tx_cal_polarity=1
    {0x000F,0x8DB3},   //;//adc_iq_swap=1
    {0x0010,0x1400},   //;//tx_sys_cal_sel<1:0>=10
    {0x0012,0x5604},   //;//Padrv_opa_bandwidth[1:0]=10;16'h1604;//;padrv_input_range<1:0>=00
    {0x0014,0x4ECC},   //;
    {0x0015,0x5124},   //;pll_gain_rx<2:0> 100
    {0x0018,0x0812},   //;//pll_refmulti2_en=1;
    {0x0019,0x10C8},   //;//pll_adcclk_en=1
    {0x001E,0x3024},   //;//Pll_lpf_gain_tx<1:0>=00;Pll_pfd_res_tx<5:0>=100100
    {0x0023,0x1111},   //;PSK
    {0x0024,0x2468},   //;
    {0x0027,0x1111},   //;GFSK
    {0x0028,0x2358},   //;
    {0x0032,0x0E00},   //;//tx_dsp_reset_delay=2
    {0x003f,0x0001},   //
    {0x000A,0x0018},   //;//thermo_pll_vcoibit_8<3:0>=1000
    {0x0000,0x020f},   //;//80H,16'hd19f;
    {0x0001,0xf9cf},   //;
    {0x0002,0xfc2f},   //;
    {0x0003,0xf92f},   //;
    {0x0004,0xfa2f},   //;
    {0x0005,0xfc2f},   //;
    {0x0006,0xfb3f},   //;
    {0x0007,0x7fff},   //;
    {0x0017,0xE7F4},   //;
    {0x0018,0xF8F8},   //;
    {0x0019,0xFDFD},   //;
    {0x001A,0xF2F3},   //;
    {0x001B,0xFEFF},   //;
    {0x001C,0xFEFF},   //;
    {0x001D,0xFFFF},   //;
    {0x001E,0xFFFF},   //;
    {0x001F,0xFFFF},   //;padrv_gain;9FH,16'hFFEC;padrv_gain20101103;improve ACPR;
    /*{0x0022,0x0E93},   //;//default
    {0x0025,0x03E1},   //;//default
    {0x0026,0x47A5},   //;//default;set voltage=1.2v
    {0x0027,0x0108},   //;
    {0x0028,0x6800},   //;//default
    {0x002D,0x006A},   //;//default OFF_FM;
    {0x002F,0x1100},   //;//default
    {0x0032,0x88F9},   //;//TM=011;DN=1111*/
    {0x003f,0x0000},   //
};

const uint32 rdabt_pskey_rf_16_c[][2] =
{
#ifdef __RDABT_ENABLE_SP__

    {0x40240000,0x2004f39c},   //; SPI2_CLK_EN PCLK_SPI2_EN
#else
    {0x40240000,0x0004f39c},   //; SPI2_CLK_EN PCLK_SPI2_EN

#endif
    {0x800000C0,0x0000000F},   //; CHIP_PS PSKEY: Total number -----------------
    {0x800000C4,0x003F0000},   //;         PSKEY: Page 0
    {0x800000C8,0x0041000B},   //;//TX_DC_OFFSET+/_
    {0x800000CC,0x004225BD},   //;//42H=32D0,1CH+9K,2CH -11K,afc_smtif
    {0x800000D0,0x0043BFC6},   //;//43H=BFC6
    {0x800000D4,0x0044040F},   //;//44H=0580;44H=040F;tx_gain_dr=0
    {0x800000D8,0x004908E4},   //;//49H=08E4;
    {0x800000DC,0x00694075},   //;//TX_DC_OFFSET+/_
    {0x800000E0,0x006B10C0},   //;
    {0x800000E4,0x003F0001},   //;         PSKEY: Page 1
    {0x800000E8,0x00402000},   //;
    {0x800000EC,0x00453000},   //;
    {0x800000F0,0x0047D132},   //;//20120512
    {0x800000F4,0x00490008},   //;
    {0x800000F8,0x003F0000},   //;         PSKEY: Page 0
    {0x80000040,0x10000000},   //;         PSKEY: Flage
//{0x40240000,0x0000f29c},   //; SPI2_CLK_EN PCLK_SPI2_EN
};

const uint32 rdabt_pskey_rf_16[][2] =
{
#ifdef __RDABT_ENABLE_SP__

    {0x40240000,0x2004f39c},   //; SPI2_CLK_EN PCLK_SPI2_EN
#else
    {0x40240000,0x0004f39c},   //; SPI2_CLK_EN PCLK_SPI2_EN

#endif
    {0x800000C0,0x0000000E},   //; CHIP_PS PSKEY: Total number -----------------
    {0x800000C4,0x003F0000},   //;         PSKEY: Page 0
    {0x800000C8,0x0041000B},   //;//TX_DC_OFFSET+/_
    {0x800000CC,0x004232D0},   //;//42H=32D0,1CH+9K,2CH -11K,afc_smtif
    {0x800000D0,0x0043BFC6},   //;//43H=BFC6
    {0x800000D4,0x0044040F},   //;//44H=0580;44H=040F;tx_gain_dr=0
    {0x800000D8,0x004908E4},   //;//49H=08E4;
    {0x800000DC,0x00694075},   //;//TX_DC_OFFSET+/_
    {0x800000E0,0x006B10C0},   //;
    {0x800000E4,0x003F0001},   //;         PSKEY: Page 1
    {0x800000E8,0x00453000},   //;
    {0x800000EC,0x0047D132},   //;//20120512
    {0x800000F0,0x00490008},   //;
    {0x800000F4,0x003F0000},   //;         PSKEY: Page 0
    {0x80000040,0x10000000},   //;         PSKEY: Flage
//{0x40240000,0x0000f29c},   //; SPI2_CLK_EN PCLK_SPI2_EN
};
uint32 rda_pskey_16[][2] =
{
    {0x800000a8,0x0Bbaba30},//min power level
#ifdef __BT_PCM_OVER_UART__
    {0x80000070,0x00002080},
#else
    {0x80000070,0x00002000},//zhou siyou esco->soc 0x00002000->0x00002008
#endif
    {0x80000074,0xa5025010},
    {0x80000078,0x0a004000},
    {0x8000007c,0xb530b530},
//{0x80000084,0x9098c007},
#if defined( _RDA_PATCH_UART3_) || defined(_RDA_PATCH_UART4_) || defined(_RDA_PATCH_UART5_) || defined(_RDA_PATCH_UART6_) || defined(_RDA_PATCH_UART7_) || defined(_RDA_PATCH_UART8_) || defined(_RDA_PATCH_UART9_)
    {0x80000064,0x0030d400},//new baudrate 3200000
#else
    {0x80000064,0x000e1000},//new baudrate 921600   0x001C2000
#endif
    {0x80000040,0x0400f200},//PSKEY: modify flag

#ifdef __RDABT_DISABLE_EDR__
    {0x800004f0,0xf88dffff}, ///disable edr
#ifdef __RDABT_ENABLE_SP__
    {0x800004f4,0x83793998}, ///disable 3m esco ev4 ev5
#else
    {0x800004f4,0x83713998}, ///disable 3m esco ev4 ev5
#endif

#else  //__RDABT_DISABLE_EDR__

#ifdef __RDABT_ENABLE_SP__
    {0x800004f4,0x83793b98}, ///disable 3m esco ev4 ev5
#else
    {0x800004f4,0x83713b98}, ///disable 3m esco ev4 ev5
#endif

#endif

#ifdef BT_UART_BREAK_INT_WAKEUP
    {0x800000a4,0x0f20280a}, //use uart tx to wake host
    {0x80000040,0x02000000},
    {0x40200010,0x00007fff}, // set hostwake  gpio to   input
#else
//host wakeup
    {0x40200010,0x00007f7f},
#endif

//CoolSand Only CTS-->GPIO2
    {0x40200044,0x0000003c}, //;
};



#if defined(_RDA_PATCH_UART_)
uint8 rda_filter_16[] = {0x01,0x72,0xfc,0x05,0x01,0x01,0x00,0x00,0x00};
#elif defined(_RDA_PATCH_UART2_)
uint8 rda_filter_16[] = {0x01,0x72,0xfc,0x05,0x01,0x02,0x00,0x00,0x00};
#elif defined(_RDA_PATCH_UART3_)
uint8 rda_filter_16[] = {0x01,0x72,0xfc,0x05,0x01,0x03,0x00,0x00,0x00};
#elif defined(_RDA_PATCH_UART4_)
uint8 rda_filter_16[] = {0x01,0x72,0xfc,0x05,0x01,0x04,0x00,0x00,0x00};
#elif defined(_RDA_PATCH_UART5_)
uint8 rda_filter_16[] = {0x01,0x72,0xfc,0x05,0x01,0x04,0x00,0x00,0x00};
#elif defined(_RDA_PATCH_UART6_)
uint8 rda_filter_16[] = {0x01,0x72,0xfc,0x05,0x01,0x05,0x00,0x00,0x00};
#elif defined(_RDA_PATCH_UART8_)
uint8 rda_filter_16[] = {0x01,0x72,0xfc,0x05,0x01,0x05,0x00,0x00,0x00};
#elif defined(_RDA_PATCH_UART7_)
uint8 rda_filter_16[] = {0x01,0x72,0xfc,0x05,0x01,0x06,0x00,0x00,0x00};
#elif defined(_RDA_PATCH_UART9_)
uint8 rda_filter_16[] = {0x01,0x72,0xfc,0x05,0x01,0x06,0x00,0x00,0x00};
#else
uint8 rda_filter_16[] = {0x01,0x72,0xfc,0x05,0x01,0x00,0x00,0x00,0x00};
#endif
#if 0
const uint32 rda_trap_16[][2] =
{
    {0x40180004,0x0001b578},
    {0x40180024,0x0001dc68},
    {0x40180008,0x00003034},
    {0x40180028,0x00000014},
    {0x4018000c,0x0001514c},
    {0x4018002c,0x00000014},
#if defined(_RDA_PATCH_UART4_)  || defined(_RDA_PATCH_UART8_) || defined(_RDA_PATCH_UART9_)
    {0x40180100,0x0001f354},
    {0x40180120,0x00021e48},
    {0x40180000,0x00000107},
#else
    {0x40180000,0x00000007},
#endif
    {0x700000b0,0x0000000a}
};
#else
const uint32 rda_trap_16[][2] =
{
    {0x40180004,0x0001b578},
    {0x40180024,0x0001dc68},
    {0x40180008,0x00003034},
    {0x40180028,0x00000014},
    {0x4018000c,0x0001514c},
    {0x4018002c,0x00000014},


    {0x40180010,0x00016b38},// ESCO link connect by headset 20130131
    {0x40180030,0x00017dc4},

    {0x401800014,0x000193b8},
    {0x401800034,0x00023718},
    {0x401800018,0x000193cc },////seconed security zhou siyou 20121016
    {0x401800038,0x00000014},

    {0x80000000,0xea00003e},
    {0x80000100,0x05d41027},
    {0x80000104,0x00611181},
    {0x80000108,0x00811004},
    {0x8000010c,0x05d1102a},
    {0x80000110,0x01510000},
    {0x80000114,0xe3a00bb5},
    {0x80000118,0x1280ffc2},
    {0x8000011c,0x0280ffa2},
    {0x4018001c,0x0002d684},
    {0x4018003c,0x000312e0},


#if defined(_RDA_PATCH_UART4_)  || defined(_RDA_PATCH_UART8_) || defined(_RDA_PATCH_UART9_)
    {0x40180100,0x0001f354},
    {0x40180120,0x00021e48},
    {0x40180000,0x0000017f},
#else
    {0x40180000,0x0000007f},
#endif


    {0x700000b0,0x0000000a}
};
#endif
const uint16 rdabt_bton_fmoff_init_16_c[][2] =
{
    {0x003f,0x0001},//page 1
    {0x0025,0x03e1},//
    {0x0026,0x43a5},//set voltage=1.2v
    {0x0028,0x6800},//
    {0x002d,0x00aa},//FM_OFF
    {0x002f,0x1100},//
    {0x0032,0x88f9},//TM=001;DN=1111
    {0x0022,0x0e93},//
    {0x003f,0x0000} //page 0
};

const uint16 rdabt_bton_fmoff_init_16[][2] =
{
    {0x003f,0x0001},//page 1
    {0x0025,0x03e1},//
    {0x0026,0x47a5},//set voltage=1.2v
    {0x0028,0x6800},//
    {0x002d,0x00aa},//FM_OFF
    {0x002f,0x1100},//
    {0x0032,0x88f9},//TM=001;DN=1111
    {0x0022,0x0e93},//
    {0x003f,0x0000} //page 0
};

const uint16 rdabt_bton_fmon_init_16[][2] =
{
    {0x003f,0x0001},//page 1
    {0x0025,0x07ff},//
    {0x0026,0x23b3},//set voltage=1.2v
    {0x0028,0x6800},//
    {0x002d,0x006a},//FM_OFF
    {0x002f,0x1100},//
    {0x0032,0x88f9},//TM=001;DN=1111
    {0x0022,0x0e93},//
    {0x003f,0x0000} //page 0
};

const uint16 rdabt_btoff_fmon_init_16[][2] =
{
    {0x003f,0x0001},//page 1
    {0x0025,0x67e5},//
    {0x0026,0x23a3},//set voltage=1.2v
    {0x0028,0x2800},//
    {0x002d,0x006a},//FM_OFF
    {0x002f,0x2100},//
    {0x0032,0x88f9},//TM=001;DN=1111
    {0x0022,0x0ea3},//
    {0x003f,0x0000} //page 0
};

const uint16 rdabt_btoff_fmoff_init_16[][2] =
{
//   {0x003f,0x0001},//page 1
//   {0x003f,0x0000} //page 0
};


uint16 rdabt_dccal_16[][2]=
{
    {0x0030,0x0129},
    {0x0030,0x012b}
};

void RDABT_5876p_bton_fmoff_patch(void)
{
    uint16 i=0;
    I2C_Open();
    if(rdabt_revid == 0x01) //5876p verB
    {
        for(i =0; i<sizeof(rdabt_bton_fmoff_init_16)/sizeof(rdabt_bton_fmoff_init_16[0]); i++)
        {
            rdabt_iic_rf_write_data(rdabt_bton_fmoff_init_16[i][0],&rdabt_bton_fmoff_init_16[i][1],1);
        }
    }
    else//5876p verC
    {
        for(i =0; i<sizeof(rdabt_bton_fmoff_init_16_c)/sizeof(rdabt_bton_fmoff_init_16_c[0]); i++)
        {
            rdabt_iic_rf_write_data(rdabt_bton_fmoff_init_16_c[i][0],&rdabt_bton_fmoff_init_16_c[i][1],1);
        }
    }
    RDABT_DELAY(10);
    I2C_Close();
}

void RDABT_5876p_bton_fmon_patch(void)
{
    uint16 i=0;
    I2C_Open();
    for(i =0; i<sizeof(rdabt_bton_fmon_init_16)/sizeof(rdabt_bton_fmon_init_16[0]); i++)
    {
        rdabt_iic_rf_write_data(rdabt_bton_fmon_init_16[i][0],&rdabt_bton_fmon_init_16[i][1],1);
    }
    RDABT_DELAY(10);
    I2C_Close();
}

void RDABT_5876p_btoff_fmon_patch(void)
{
    uint16 i=0;
    I2C_Open();
    for(i =0; i<sizeof(rdabt_btoff_fmon_init_16)/sizeof(rdabt_btoff_fmon_init_16[0]); i++)
    {
        rdabt_iic_rf_write_data(rdabt_btoff_fmon_init_16[i][0],&rdabt_btoff_fmon_init_16[i][1],1);
    }
    RDABT_DELAY(10);
    I2C_Close();
}

void RDABT_5876p_btoff_fmoff_patch(void)
{
    uint16 i=0;
    I2C_Open();
    for(i =0; i<sizeof(rdabt_btoff_fmoff_init_16)/sizeof(rdabt_btoff_fmoff_init_16[0]); i++)
    {
        rdabt_iic_rf_write_data(rdabt_btoff_fmoff_init_16[i][0],&rdabt_btoff_fmoff_init_16[i][1],1);
    }
    RDABT_DELAY(10);
    I2C_Close();
}


void RDABT_phone_Intialization_r16(void)
{
    uint16 i=0;
    for(i =0; i<sizeof(rdabt_phone_init_16)/sizeof(rdabt_phone_init_16[0]); i++)
    {
        rdabt_iic_rf_write_data(rdabt_phone_init_16[i][0],&rdabt_phone_init_16[i][1],1);
    }
}

void Rdabt_Pskey_Write_rf_r16(void)
{
    uint16 i;
    if(rdabt_revid == 0x01) //5876p verB
    {
        for(i=0; i<sizeof(rdabt_pskey_rf_16)/sizeof(rdabt_pskey_rf_16[0]); i++)
        {
            rdabt_wirte_memory(rdabt_pskey_rf_16[i][0],&rdabt_pskey_rf_16[i][1],1,0);
            RDABT_DELAY(1);
        }
    }
    else //5876p verC
    {
        for(i=0; i<sizeof(rdabt_pskey_rf_16_c)/sizeof(rdabt_pskey_rf_16_c[0]); i++)
        {
            rdabt_wirte_memory(rdabt_pskey_rf_16_c[i][0],&rdabt_pskey_rf_16_c[i][1],1,0);
            RDABT_DELAY(1);
        }
    }
    RDABT_DELAY(10);
}


void Rdabt_Pskey_Write_r16(void)
{
    uint16 i;
    for(i=0; i<sizeof(rda_pskey_16)/sizeof(rda_pskey_16[0]); i++)
    {
        rdabt_wirte_memory(rda_pskey_16[i][0],&rda_pskey_16[i][1],1,0);
        RDABT_DELAY(1);
    }

}


void rdabt_set_filter_r16(void)
{
    uint16 num_send;
    rdabt_uart_tx(rda_filter_16,sizeof(rda_filter_16),&num_send);
    RDABT_DELAY(1);
}


void Rdabt_trap_write_r16(void)
{
    uint16 i;
    for(i=0; i<sizeof(rda_trap_16)/sizeof(rda_trap_16[0]); i++)
    {
        rdabt_wirte_memory(rda_trap_16[i][0],&rda_trap_16[i][1],1,0);
        RDABT_DELAY(1);
    }
}




void  RDABT_rf_Intialization_r16(void)
{
    uint16 i=0;
    if(rdabt_revid == 0x01) //5876p verB
    {
        for(i =0; i<sizeof(rdabt_rf_init_16)/sizeof(rdabt_rf_init_16[0]); i++)
        {
            rdabt_iic_rf_write_data(rdabt_rf_init_16[i][0],&rdabt_rf_init_16[i][1],1);
        }
    }
    else //5876p verC
    {
        for(i =0; i<sizeof(rdabt_rf_init_16_c)/sizeof(rdabt_rf_init_16_c[0]); i++)
        {
            rdabt_iic_rf_write_data(rdabt_rf_init_16_c[i][0],&rdabt_rf_init_16_c[i][1],1);
        }
    }
    RDABT_DELAY(10);
}


void rdabt_DC_write_r16(void)
{
    uint16 i;
    for(i=0; i<sizeof(rdabt_dccal_16)/sizeof(rdabt_dccal_16[0]); i++)
    {
        rdabt_iic_rf_write_data(rdabt_dccal_16[i][0],&rdabt_dccal_16[i][1],1);
        RDABT_DELAY(2);
    }
}


void RDABT_core_Intialization_r16(void)
{
#ifdef RDA_BT_debug
    EDRV_TRACE(EDRV_BTD_TRC, 0,"RDABT_core_Intialization_r16");
#endif
    rda_bt_pin_to_low();
    rda_bt_ldoon_toggle_high();
    rda_bt_reset_to_high();
    RDABT_DELAY(10); //wait for digi running
    RDABT_rf_Intialization_r16();
    Rdabt_Pskey_Write_rf_r16();
    rdabt_DC_write_r16();
    Rdabt_Pskey_Write_r16();
    rdabt_set_filter_r16();
    Rdabt_trap_write_r16();
}

#endif// __RDA_CHIP_R16_5876p__

#endif

