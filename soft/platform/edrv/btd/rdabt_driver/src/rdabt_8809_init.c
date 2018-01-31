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


//sync btdV3.45
#ifdef __RDA_CHIP_R17_8809__

const uint16 rdabt_phone_init_17[][2] =
{
#if 0
    {0x3F, 0x0001}, // page 1
    {0x32, 0x0009}, // Init UART IO status
#endif
    {0x3F, 0x0000}, // page 0
};

uint16 rdabt_rf_init_17[][2] =
{
    {0x3F, 0x0000}, // page 0
    {0x01, 0x1fff}, // Padrv_gain_tb_en=1
    {0x06, 0x161c}, // Padrv_op_gain=11;Padrv_ibit_psk<3:0>=000;Padrv_input_range_psk<1:0>=11
    {0x07, 0x040d}, //
    {0x08, 0x8326}, // padrv_lo_tune_psk[5:0]=10000;Lna_notch_en=1
    {0x09, 0x04b5}, // rmx_imgrej=1
    {0x0B, 0x238f}, // Filter_cal_mode =1
    {0x0C, 0x85e8}, // filter_bpmode<2:0>=101
    {0x0E, 0x0920}, // Tx_cal_polarity=1
    {0x0F, 0x8db3}, // adc_iq_swap=1
    {0x10, 0x1400}, // tx_sys_cal_sel<1:0>=10
    {0x12, 0x560c}, // 12H,16'h1604;//Filter_cal_mode=0;padrv_ibit<3:0>=000;padrv_input_range<1:0>=00
//{0x12, 0x1604}, // Lower the BT Tx power
    {0x14, 0x4ecc}, //
    {0x18, 0x0812}, // pll_refmulti2_en=1;
    {0x19, 0x10c8}, // pll_adcclk_en=1
    {0x1E, 0x3024}, // Pll_lpf_gain_tx<1:0>=00;Pll_pfd_res_tx<5:0>=100100
// TX GAIN
    {0x23, 0x7777}, // PSK
    {0x24, 0x2368}, //
    {0x27, 0x5555}, // GFSK
    {0x28, 0x1247}, //
    {0x32, 0x0200}, // tx_dsp_reset_delay=2
// AGC_by_xudonglin
    {0x3F, 0x0001}, // page 1
    {0x00, 0x020f}, // 80H,16'hd19f;
    {0x01, 0xf9cf}, //
    {0x02, 0xfc2f}, //
    {0x03, 0xf92f}, //
    {0x04, 0xfa2f}, //
    {0x05, 0xfc2f}, //
    {0x06, 0xfb3f}, //
    {0x07, 0x7fff}, //
    {0x0A, 0x0018}, // thermo_pll_vcoibit_8<3:0>=1000
// apc;padrv_gain
    {0x18, 0xffff}, //
    {0x19, 0xffff}, //
    {0x1A, 0xffff}, //
    {0x1B, 0xffff}, //
    {0x1C, 0xffff}, //
    {0x1D, 0xffff}, //
    {0x1E, 0xffff}, //
    {0x1F, 0xffff}, // padrv_gain;9FH,16'hFFEC;padrv_gain20101103;improve ACPR;
    {0x22, 0x0e93}, // default
    {0x25, 0x03e1}, // default
    {0x26, 0x47a5}, // default;set voltage=1.2v
    {0x27,0x0108},  //osc_stable_timer
    {0x28, 0x6800}, // default
    {0x2D, 0x006a}, // default OFF_FM;
    {0x2F, 0x1100}, // default
    {0x32, 0x88f9}, // TM=001;DN=1111
    {0x3F, 0x0000}, // page 0
};


const uint32 rdabt_pskey_rf_17[][2] =
{
// item:RDA8809BT_B_digi_20120512
#ifdef __RDABT_ENABLE_SP__

    {0x40240000,0x2004f39c},   //; SPI2_CLK_EN PCLK_SPI2_EN
#else
    {0x40240000,0x0004f39c},   //; SPI2_CLK_EN PCLK_SPI2_EN

#endif
    {0x800000C0, 0x0000000b}, // CHIP_PS PSKEY: Total number -----------------
    {0x800000C4, 0x003f0000}, // PSKEY: Page 0
    {0x800000C8, 0x0041000b}, // TX_DC_OFFSET+/_
    {0x800000CC, 0x00430fc6}, // 43H=BFC6
    {0x800000D0, 0x0044048f}, // 44H=0580;
    {0x800000D4, 0x00694075}, // TX_DC_OFFSET+/_
    {0x800000D8, 0x006b10c0}, //
    {0x800000DC, 0x003f0001}, // PSKEY: Page 1
    {0x800000E0, 0x00453000}, //
    {0x800000E4, 0x0047f13b}, // 20120512
    {0x800000E8, 0x00490008}, //
    {0x800000EC, 0x003f0000}, // PSKEY: Page 0
    {0x80000040, 0x10000000}, // PSKEY: Flage
    {0x40240000, 0x0000f29c}, // Disable SPI2_CLK_EN PCLK_SPI2_EN
};


const uint16 rdabt_dccal_17[][2]=
{
// item:rda8809bt_verB_DC_CAL
    {0x30, 0x0129}, //
    {0x30, 0x012b}, //
};


const uint32 rda_pskey_17[][2] =
{
    {0x800000a8,0x0cbaba30},//min power level 0x0cbfbf30->0x0cbaba30
#ifdef __BT_PCM_OVER_UART__
    {0x80000070,0x00002080},
#else
    {0x80000070,0x00002000},
#endif
    {0x80000074,0xa5025010},
    {0x80000078,0x0f054000},
    {0x8000007c,0xb530b530},
//{0x80000084,0x9098c007},
#ifdef USE_VOC_CVSD
    {0x80000084,0x9098c008},//csvd
#endif
//{0x80000064,0x001C2000},//new baudrate 3200000
    {0x80000064,0x000e1000},//new baudrate 3200000
#ifdef USE_VOC_CVSD
    {0x80000040,0x0402f200},//PSKEY: CVSD
#else
    {0x80000040,0x0400f200},//PSKEY: modify flag
#endif
#ifdef __RDABT_DISABLE_EDR__

    {0x80000470,0xf88dffff}, ///disable edr
#ifdef __RDABT_ENABLE_SP__
    {0x80000474,0x83793818}, ///disable 3m esco ev4 ev5
#else
    {0x80000474,0x83713818}, ///disable 3m esco ev4 ev5
#endif

#else // !__RDABT_DISABLE_EDR__


#ifdef __RDABT_ENABLE_SP__
    {0x80000474,0x83793998}, ///disable 3m esco ev4 ev5
#else
    {0x80000474,0x83713998}, ///disable 3m esco ev4 ev5
#endif

#endif // !__RDABT_DISABLE_EDR__

#ifdef BT_UART_BREAK_INT_WAKEUP
    {0x800000a4,0x0f20280a}, // use uart tx to wake host
    {0x80000040,0x02000000},
    {0x40200010,0x00007fff}, // set hostwake gpio to input
#else
//host wakeup
    {0x40200010,0x00007f7f},
#endif
#ifdef USE_VOC_CVSD
    {0x80000550,0x003c03fd}, // set buffer size
    {0x80000554,0x000f0006},
    {0x80000558,0x003c03fd},
    {0x8000055c,0x000f0004},
    {0x80000560,0x0001003c},
#else
    {0x80000550,0x007803fd}, // set buffer size
    {0x80000554,0x00080006},
    {0x80000558,0x007803fd},
    {0x8000055c,0x00080004},
    {0x80000560,0x00010078},
#endif
//CoolSand Only CTS-->GPIO2
//{0x40200044,0x0000003c}, //;
};

const uint32 rda_trap_17[][2] =
{
    {0x40180004,0x0001b0cc},
    {0x40180024,0x0001d7bc},
    {0x40180008,0x00002e2c},
    {0x40180028,0x00000014},
    {0x4018000c,0x00014cb0},
    {0x4018002c,0x00000014},
    {0x40180010,0x00018f1c},
    {0x40180030,0x0001a1ac},
    {0x40180014,0x0001669c}, // ESCO link connect by headset 20130131
    {0x40180034,0x00017928},
//  {0x40180014,0x00018f30 },//seconed security zhou siyou 20121016
//  {0x40180024,0x00000014},
    {0x40180018,0x00013e40}, // for a2dp audio, fix 3M packet
    {0x40180038,0x0000a2f0},
    {0x4018001c,0x00012bb4}, //edr
    {0x4018003c,0x00000014},
    {0x40180020,0x00014fc4},//w2
    {0x40180040,0x0002c328},

    {0x80000010,0xea000062},//ltaddr
    {0x800001a0,0x05d41027},
    {0x800001a4,0x00611181},
    {0x800001a8,0x00811004},
    {0x800001ac,0x05d1102a},
    {0x800001b0,0x01510000},
    {0x800001b4,0xe3a00bae},
    {0x800001b8,0x1280fff6},
    {0x800001bc,0x0280ffd6},
    {0x40180100,0x0002bb54},
    {0x40180120,0x0002ee20},

    {0x80000350,0xe51ff004}, //pause enc
    {0x80000354,0x0000270c},
    {0x80000358,0xe92d4010},
    {0x8000035c,0xebfffffb},
    {0x80000360,0xe3a01000},
    {0x80000364,0xe5c01054},
    {0x80000368,0xe8bd8010},
    {0x80000014,0xea0000d4},
    {0x8000036c,0x03a00c74},
    {0x80000370,0x0280f074},
    {0x80000374,0xe3550012},
    {0x80000378,0x059f000c},
    {0x8000037c,0x05860004},
    {0x80000380,0xe3a00c74},
    {0x80000384,0x0280f0e0},
    {0x80000388,0xe280f094},
    {0x8000038c,0x80000358},
    {0x40180104,0x00007470},
    {0x40180124,0x0002ee24},

    {0x40180108,0x00014bf4},///2ev3-ev3
    {0x40180128,0x00018c7c},


    {0x8000001c,0xea0000ed},////release sco
    {0x800003c0,0xe51ff004},
    {0x800003c4,0x0001fa54},
    {0x800003c8,0xe51ff004},
    {0x800003cc,0x0001fb80},
    {0x800003d0,0xe51ff004},
    {0x800003d4,0x00001590},
    {0x800003d8,0xe59f0024},
    {0x800003dc,0xebfffff7},
    {0x800003e0,0xe3a00024},
    {0x800003e4,0xe1a01004},
    {0x800003e8,0xebfffff8},
    {0x800003ec,0xe1a01004},
    {0x800003f0,0xe3a00027},
    {0x800003f4,0xebfffff5},
    {0x800003f8,0xe59f0008},
    {0x800003fc,0xebfffff1},
    {0x80000400,0xe8bd8010},
    {0x80000404,0x80000408},
    {0x80000408,0x00000000} ,
    {0x4018010c,0x00004578},
    {0x4018012c,0x0002ee2c},


    {0x80000020,0xea00006c},    ////packet length to real length in queues
    {0x800001d8,0xe3530024},    //cmp r3 0x24
    {0x800001dc,0x13a03c17},    //movne r3,0x1700
    {0x800001e0,0x11dc11ba},    //LDRHne     r1,[r12,#0x1a]
    {0x800001e4,0x1283f0e8},    //addne pc,r3,0xe8
    {0x800001e8,0xe3a01c18},    //mov r1,0x1800
    {0x800001ec,0xe281f044},    //add pc,r1,0x44
    {0x40180118,0x000017e4},
    {0x40180138,0x0002ee30},

    {0x40180000,0x0000ffff},
};

void RDABT_phone_Intialization_r17(void)
{
    uint16 i=0;
    for(i =0; i<sizeof(rdabt_phone_init_17)/sizeof(rdabt_phone_init_17[0]); i++)
    {
        rdabt_iic_rf_write_data(rdabt_phone_init_17[i][0],&rdabt_phone_init_17[i][1],1);
    }
}

void  RDABT_rf_Intialization_r17(void)
{
    uint16 i=0;
    for(i=0; i<sizeof(rdabt_rf_init_17)/sizeof(rdabt_rf_init_17[0]); i++)
    {
        rdabt_iic_rf_write_data(rdabt_rf_init_17[i][0],&rdabt_rf_init_17[i][1],1);
    }
    RDABT_DELAY(10);
#if 0
    UINT16 data;
    for(i=0; i<sizeof(rdabt_rf_init_17)/sizeof(rdabt_rf_init_17[0]); i++)
    {
        if (rdabt_rf_init_17[i][0] == 0x3f)
        {
            rdabt_iic_rf_write_data(rdabt_rf_init_17[i][0],&rdabt_rf_init_17[i][1],1);
        }
        rdabt_iic_rf_read_data(rdabt_rf_init_17[i][0],&data,1);
        sxs_fprintf((15<<5)|(1<<21), "RFInit 0x%02x = 0x%04x", rdabt_rf_init_17[i][0], data);
    }
#endif
}

void Rdabt_Pskey_Write_rf_r17(void)
{
    uint16 i;
    for(i=0; i<sizeof(rdabt_pskey_rf_17)/sizeof(rdabt_pskey_rf_17[0]); i++)
    {

        rdabt_wirte_memory(rdabt_pskey_rf_17[i][0],&rdabt_pskey_rf_17[i][1],1,0);
        RDABT_DELAY(2);
    }
#if 0
    uint32 data;
    for(i=0; i<sizeof(rdabt_pskey_rf_17)/sizeof(rdabt_pskey_rf_17[0]); i++)
    {

        rdabt_iic_core_read_data(rdabt_pskey_rf_17[i][0],&data,1);
        sxs_fprintf((15<<5)|(1<<21), "PSKey 0x%08x = 0x%08x", rdabt_pskey_rf_17[i][0], data);
    }
#endif
}

void rdabt_DC_write_r17(void)
{
    uint16 i;
    for(i=0; i<sizeof(rdabt_dccal_17)/sizeof(rdabt_dccal_17[0]); i++)
    {

        rdabt_iic_rf_write_data(rdabt_dccal_17[i][0],&rdabt_dccal_17[i][1],1);
        RDABT_DELAY(2);
    }
    RDABT_DELAY(10);
#if 0
    UINT16 data;
    for(i=0; i<sizeof(rdabt_dccal_17)/sizeof(rdabt_dccal_17[0]); i++)
    {

        rdabt_iic_rf_read_data(rdabt_dccal_17[i][0],&data,1);
        sxs_fprintf((15<<5)|(1<<21), "DC 0x%02x = 0x%04x", rdabt_dccal_17[i][0], data);
    }
#endif
}

void Rdabt_Pskey_Write_r17(void)
{
    uint16 i;
    for(i=0; i<sizeof(rda_pskey_17)/sizeof(rda_pskey_17[0]); i++)
    {
        rdabt_wirte_memory(rda_pskey_17[i][0],&rda_pskey_17[i][1],1,0);
        RDABT_DELAY(1);
    }

}

void Rdabt_trap_write_r17(void)
{
    uint16 i;
    for(i=0; i<sizeof(rda_trap_17)/sizeof(rda_trap_17[0]); i++)
    {
        rdabt_wirte_memory(rda_trap_17[i][0],&rda_trap_17[i][1],1,0);
        RDABT_DELAY(1);
    }
}

void RDABT_core_Intialization_r17(void)
{
#ifdef RDA_BT_debug
    EDRV_TRACE(EDRV_BTD_TRC, 0,"RDABT_core_Intialization_r17");
#endif

    RDABT_rf_Intialization_r17();
    Rdabt_Pskey_Write_rf_r17();
    rdabt_DC_write_r17();
    Rdabt_Pskey_Write_r17();

    Rdabt_trap_write_r17();
}

#endif // __RDA_CHIP_R17_8809__

#endif // __BT_RDABT__


