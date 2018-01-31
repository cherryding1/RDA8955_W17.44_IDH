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


#if defined( __RDA_CHIP_R11_5875__)

const uint16 rdabt_phone_init_11[][2] =
{
    {0x3f,0x0001},//;page 1
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)
    {0x32,0x0059},//;TM mod 001
#else // 8808 or later
    {0x32,0x0009},// Init UART IO status
#endif
    {0x3f,0x0000},//;page 0
};

const uint16 rdabt_rf_init_11[][2] =
{
    {0x3f,0x0000},
    {0x01,0x1FFF},//;
    {0x06,0x07F4},//;padrv_set,increase the power.
    {0x08,0x01E7},//;
    {0x09,0x0520},//;
    {0x0B,0x030F},//;filter_cap_tuning<3:0>1101
    {0x0C,0x85E8},//;
    {0x0F,0x1DCC},//; adc_clk_sel=1 20101126
    {0x12,0x07F4},//;padrv_set,increase the power.
    {0x13,0x0327},//;agpio down pullen
    {0x14,0x0CFE},//;
    {0x15,0x0526},//;Pll_bypass_ontch:1,improve ACPR.
    {0x16,0x8918},//;add div24 20101126
    {0x18,0x8800},//;//pll_fref_mdll_en=0  20101231 by shuran
    {0x19,0x10C8},//;pll_adcclk_en=1 20101126
    {0x1A,0x9128},//;Mdll_adcclk_out_en=0
    {0x1B,0x80C2},//;//mdll_en_rx=0;mdll_div_rx=0001 20101231 by shuran
    {0x1C,0x3613},//;
    {0x1D,0x13E3},//;Pll_cp_bit_tx<3:0>1110;13D3
    {0x1E,0x300C},//;Pll_lpf_gain_tx<1:0> 00;304C
    {0x23,0x3333},//; //TX GAIN_GFSK
    {0x24,0x78AF},//; //TX GAIN_GFSK
    {0x27,0x1111},//; //TX GAIN_DPSK
    {0x28,0x234F},//; //TX GAIN_DPSK
    {0x3f,0x0001},
    {0x00,0x043F},//;agc
    {0x01,0x467F},//;agc
    {0x02,0x68FF},//;agc
    {0x03,0x67FF},//;agc
    {0x04,0x57FF},//;agc
    {0x05,0x7BFF},//;agc
    {0x06,0x3FFF},//;agc
    {0x07,0x7FFF},//;agc
    {0x18,0xB1B1},//;padrv_gain
    {0x19,0xB1B1},//;padrv_gain
    {0x1A,0xB1B1},//;padrv_gain
    {0x1B,0xB2B2},//;padrv_gain
    {0x1C,0xD7B0},//;padrv_gain
    {0x1D,0xE5E0},//;padrv_gain
    {0x1E,0xE7E9},//;padrv_gain
    {0x1F,0xE9E9},//;padrv_gain;9FH,16'hFFEC;padrv_gain20101103;improve ACPR;
    {0x23,0x4224},//;ext32k
#ifdef __5875_USE_DCDC__
    {0x24,0x9b10},
#else
    {0x24,0x0110},
#endif
    {0x25,0x43A0},//;ldo_vbit:110,2.04v
    {0x26,0x49B5},//;reg_ibit:100,reg_vbit:110,1.1v,reg_vbit_deepsleep:110,750mV
    {0x27,0x0106},  //enlarge OSC stable timer from 4ms to 6ms
    {0x3f,0x0000}
};

//sync btdV3.45
const uint32 rdabt_pskey_rf_11[][2] =
{
    {0x40240000,0x0004f39c}, //; SPI2_CLK_EN PCLK_SPI2_EN
    {0x800000C0,0x00000021}, //; CHIP_PS PSKEY: Total number -----------------
    {0x800000C4,0x003F0000}, //;         PSKEY: Page 0
    {0x800000C8,0x00410003}, //;         PSKEY: Swch_clk_ADC
    {0x800000CC,0x004225BD}, //;         PSKEY: dig IF 625K IF  by lihua20101231
    {0x800000D0,0x004908E4}, //;         PSKEY: freq_offset_for rateconv  by lihua20101231(replace 47H)
    {0x800000D4,0x0043B074}, //;         PSKEY: AM dac gain, 20100605
    {0x800000D8,0x0044D01A}, //;         PSKEY: gfsk dac gain, 20100605//22
    {0x800000DC,0x004A0800}, //;         PSKEY: 4AH=0800
    {0x800000E0,0x0054A020}, //;         PSKEY: 54H=A020;agc_th_max=A0;agc_th_min=20
    {0x800000E4,0x0055A020}, //;         PSKEY: 55H=A020;agc_th_max_lg=A0;agc_th_min_lg=20
    {0x800000E8,0x0056A542}, //;         PSKEY: 56H=A542
    {0x800000EC,0x00574C18}, //;         PSKEY: 57H=4C18
    {0x800000F0,0x003F0001}, //;         PSKEY: Page 1
    {0x800000F4,0x00410900}, //;         PSKEY: C1=0900;Phase Delay, 20101029
    {0x800000F8,0x0046033F}, //;         PSKEY: C6=033F;modulation Index;delta f2=160KHZ,delta f1=164khz
    {0x800000FC,0x004C0000}, //;         PSKEY: CC=0000;20101108
    {0x80000100,0x004D0015}, //;         PSKEY: CD=0015;
    {0x80000104,0x004E002B}, //;         PSKEY: CE=002B;
    {0x80000108,0x004F0042}, //;         PSKEY: CF=0042
    {0x8000010C,0x0050005A}, //;         PSKEY: D0=005A
    {0x80000110,0x00510073}, //;         PSKEY: D1=0073
    {0x80000114,0x0052008D}, //;         PSKEY: D2=008D
    {0x80000118,0x005300A7}, //;         PSKEY: D3=00A7
    {0x8000011C,0x005400C4}, //;         PSKEY: D4=00C4
    {0x80000120,0x005500E3}, //;         PSKEY: D5=00E3
    {0x80000124,0x00560103}, //;         PSKEY: D6=0103
    {0x80000128,0x00570127}, //;         PSKEY: D7=0127
    {0x8000012C,0x0058014E}, //;         PSKEY: D8=014E
    {0x80000130,0x00590178}, //;         PSKEY: D9=0178
    {0x80000134,0x005A01A1}, //;         PSKEY: DA=01A1
    {0x80000138,0x005B01CE}, //;         PSKEY: DB=01CE
    {0x8000013C,0x005C01FF}, //;         PSKEY: DC=01FF
    {0x80000140,0x003F0000}, //;         PSKEY: Page 0
    {0x80000144,0x00000000}, //;         PSKEY: Page 0
    {0x80000040,0x10000000}, //;         PSKEY: Flage
    {0x40240000,0x0000f29c}, //; SPI2_CLK_EN PCLK_SPI2_EN
};


const uint16 rdabt_dccal_11[][2]=
{
    {0x0030,0x0129},
    {0x0030,0x012b}
};

void RDABT_phone_Intialization_r11(void)
{
    uint16 i=0;
    for(i =0; i<sizeof(rdabt_phone_init_11)/sizeof(rdabt_phone_init_11[0]); i++)
    {
        rdabt_iic_rf_write_data(rdabt_phone_init_11[i][0],&rdabt_phone_init_11[i][1],1);
    }
}

void RDABT_rf_Intialization_r11(void)
{
    uint16 i=0;
    for(i =0; i<sizeof(rdabt_rf_init_11)/sizeof(rdabt_rf_init_11[0]); i++)
    {
        rdabt_iic_rf_write_data(rdabt_rf_init_11[i][0],&rdabt_rf_init_11[i][1],1);
    }
    RDABT_DELAY(10);
}

void Rdabt_Pskey_Write_rf_r11(void)
{
    uint16 i;
    for(i=0; i<sizeof(rdabt_pskey_rf_11)/sizeof(rdabt_pskey_rf_11[0]); i++)
    {

        rdabt_wirte_memory(rdabt_pskey_rf_11[i][0],&rdabt_pskey_rf_11[i][1],1,0);
        RDABT_DELAY(2);
    }
}

void rdabt_DC_write_r11(void)
{
    uint16 i;
    for(i=0; i<sizeof(rdabt_dccal_11)/sizeof(rdabt_dccal_11[0]); i++)
    {

        rdabt_iic_rf_write_data(rdabt_dccal_11[i][0],&rdabt_dccal_11[i][1],1);
        RDABT_DELAY(2);
    }
}


void RDABT_core_Intialization_r11(void)
{
#ifdef RDA_BT_debug
    EDRV_TRACE(EDRV_BTD_TRC, 0,"RDABT_core_Intialization_r11");
#endif
    rda_bt_pin_to_low();
    rda_bt_ldoon_toggle_high();
    rda_bt_reset_to_high();
    RDABT_DELAY(10); //wait for digi running
    RDABT_rf_Intialization_r11();
    Rdabt_Pskey_Write_rf_r11();
    rdabt_DC_write_r11();
    Rdabt_Pskey_Write_r11();
    Rdabt_patch_write_r11();
    Rdabt_unsniff_prerxon_write_r11();
    Rdabt_setfilter_r11();
    Rdabt_trap_write_r11();
}

#endif

#endif





