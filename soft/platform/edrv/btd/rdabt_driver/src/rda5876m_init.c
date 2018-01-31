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

#if defined( __RDA_CHIP_R12_5876M__)

#ifdef __RDA_5876_SMALL_PLL__
const uint32 rdabt_rf_init_12_m[][2] =
{
    {0x3f,0x00000000}, //
    {0x01,0x00001FFF}, //;
    {0x06,0x000007F7}, //;padrv_set,increase the power.
    {0x08,0x000001E7}, //;
    {0x09,0x00000520}, //;
    {0x0B,0x000003DF}, //;filter_cap_tuning<3:0>1101
    {0x0C,0x000085E8}, //;
    {0x0F,0x00000DBC}, //; 0FH,16'h1D8C; 0FH,16'h1DBC;adc_clk_sel=1 20110314 ;adc_digi_pwr_reg<2:0>=011;
    {0x12,0x000007F7}, //;padrv_set,increase the power.
    {0x13,0x00000327}, //;agpio down pullen .
    {0x14,0x00000CCC}, //;h0CFE; bbdac_cm 00=vdd/2.
    {0x15,0x00000526}, //;Pll_bypass_ontch:1,improve ACPR.
    {0x16,0x00008918}, //;add div24 20101126
    {0x18,0x00008800}, //;add div24 20101231
    {0x19,0x000010C8}, //;pll_adcclk_en=1 20101126
    {0x1A,0x00009128}, //;Mdll_adcclk_out_en=0
    {0x1B,0x000080C0}, //;1BH,16'h80C2
    {0x1C,0x0000361F}, //;Xtal_amp=111,by xudonglin20110629
    {0x1D,0x000043E5}, //;improved EVM by xudonglin20110629
    {0x1E,0x0000303F}, //;improved EVM by xudonglin20110629
    {0x23,0x00002222}, //;    ;;TX GAIN
    {0x24,0x0000359D}, //; improved ACP by xudonglin20110629
    {0x27,0x00000011}, //;
    {0x28,0x0000124F}, //;
    {0x2F,0x00003F48}, //;//20110519set smallpll
    {0x36,0x00003400}, //;// set smallpll
    {0x37,0x00000000}, //;// set smallpll
    {0x38,0x00004811}, //;// set smallpll
//{0x39,0x00000CE4}, //;// set smallpll
    {0x3A,0x00001000}, //;// set smallpll
    {0x3f,0x00000001}, //
    {0x00,0x0000043F}, //;agc
    {0x01,0x0000467F}, //;agc
    {0x02,0x000028FF}, //;agc//2011032382H,16'h68FF;agc
    {0x03,0x000067FF}, //;agc
    {0x04,0x000057FF}, //;agc
    {0x05,0x00007BFF}, //;agc
    {0x06,0x00003FFF}, //;agc
    {0x07,0x00007FFF}, //;agc
    {0x18,0x0000F3F5}, //; ;;padrv_gain
    {0x19,0x0000F3F5}, //;
    {0x1A,0x0000E7F3}, //;
    {0x1B,0x0000F1FF}, //;
    {0x1C,0x0000FFFF}, //;
    {0x1D,0x0000FFFF}, //;
    {0x1E,0x0000FFFF}, //;
    {0x1F,0x0000FFFF}, //;padrv_gain;9FH,16'hFFEC;padrv_gain20101103;improve ACPR;
    {0x23,0x00004224}, //;ext32k
#ifdef __5875_USE_DCDC__
    {0x24,0x00009a53},
    {0x25,0x00004322}, //;ldo_vbit:110,2.04v
#else
    {0x24,0x00000110},
    {0x25,0x000043E1}, //;ldo_vbit:110,2.04v
#endif
//{0x25,0x000043E1}, //;ldo_vbit:110,1.96v
    {0x26,0x00004BB5}, //;reg_ibit:101,reg_vbit:110,1.12v,reg_vbit_deepsleep:110,750mV
    {0x32,0x00000079}, //;TM mod
    {0x3f,0x00000000}, //
    {0x39,0x00000CE4}, //;// set smallpll
};
#else
const uint32 rdabt_rf_init_12_m[][2] =
{
    {0x3f,0x00000000},  //
    {0x01,0x00001FFF},  //;
    {0x06,0x000007F7},  //;padrv_set,increase the power.
    {0x08,0x000001E7},  //;
    {0x09,0x00000520},  //;
    {0x0B,0x000003DF},  //;filter_cap_tuning<3:0>1101
    {0x0C,0x000085E8},  //;
    {0x0F,0x00000DBC},  //; 0FH,16'h1D8C; 0FH,16'h1DBC;adc_clk_sel=1 20110314 ;adc_digi_pwr_reg<2:0>=011;
    {0x12,0x000007F7},  //;padrv_set,increase the power.
    {0x13,0x00000327},  //;agpio down pullen .
    {0x14,0x00000CCC},  //;h0CFE; bbdac_cm 00=vdd/2.
    {0x15,0x00000526},  //;Pll_bypass_ontch:1,improve ACPR.
    {0x16,0x00008918},  //;add div24 20101126
    {0x18,0x00008800},  //;add div24 20101231
    {0x19,0x000010C8},  //;pll_adcclk_en=1 20101126
    {0x1A,0x00009128},  //;Mdll_adcclk_out_en=0
    {0x1B,0x000080C0},  //;1BH,16'h80C2
    {0x1C,0x0000361f},  //;
    {0x1D,0x000043e5},  //;Pll_cp_bit_tx<3:0>1110;13D3
    {0x1E,0x0000303f},  //;Pll_lpf_gain_tx<1:0> 00;304C
    {0x23,0x00002222},  //;
    {0x24,0x0000359d},  //;
    {0x27,0x00000011},  //;
    {0x28,0x0000124F},  //;
    {0x39,0x0000A5FC},  //;
    {0x3f,0x00000001},  //
    {0x00,0x0000043F},  //;agc
    {0x01,0x0000467F},  //;agc
    {0x02,0x000028FF},  //;agc//2011032382H,16'h68FF;agc
    {0x03,0x000067FF},  //;agc
    {0x04,0x000057FF},  //;agc
    {0x05,0x00007BFF},  //;agc
    {0x06,0x00003FFF},  //;agc
    {0x07,0x00007FFF},  //;agc
    {0x18,0x0000F3F5},  //;
    {0x19,0x0000F3F5},  //;
    {0x1A,0x0000E7F3},  //;
    {0x1B,0x0000F1FF},  //;
    {0x1C,0x0000FFFF},  //;
    {0x1D,0x0000FFFF},  //;
    {0x1E,0x0000FFFF},  //;
    {0x1F,0x0000FFFF},  //;padrv_gain;9FH,16'hFFEC;padrv_gain20101103;improve ACPR;
    {0x23,0x00004224},  //;ext32k
#ifdef __5875_USE_DCDC__
    {0x24,0x00009a53},
    {0x25,0x00004322}, //;ldo_vbit:110,2.04v
#else
    {0x24,0x00000110},
    {0x25,0x000043E1}, //;ldo_vbit:110,2.04v
#endif
//{0x25,0x000043E1},  //;ldo_vbit:110,1.96v
    {0x26,0x00004BB5},  //;reg_ibit:101,reg_vbit:110,1.12v,reg_vbit_deepsleep:110,750mV
    {0x32,0x00000079},  //;TM mod
    {0x3f,0x00000000},  //
};
#endif


const uint32 rdabt_enable_spi2_r12_m[][2] =
{
    {0x40240000,0x0004f39c}, //        SPI2_CLK_EN PCLK_SPI2_EN
};


const uint32 rdabt_disable_spi2_r12_m[][2] =
{
    {0x40240000,0x0000f29c},//; SPI2_CLK_EN PCLK_SPI2_EN
};

const uint32 rdabt_pskey_rf_12_m[][2] =
{
    {0x40240000,0x0004f39c}, //; SPI2_CLK_EN PCLK_SPI2_EN
    {0x800000C0,0x00000021}, //; CHIP_PS PSKEY: Total number -----------------
    {0x800000C4,0x003F0000}, //;         PSKEY: Page 0
    {0x800000C8,0x00414003}, //;         PSKEY: Swch_clk_ADC
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
//{0x40240000,0x0000f29c}, //; SPI2_CLK_EN PCLK_SPI2_EN
};


const uint32 rdabt_dccal_12_m[][2]=
{
    {0x00000030,0x00000129},
    {0x00000030,0x0000012b}
};

void  RDABT_rf_Intialization_r12_m(void)
{
    uint16 i=0;
    rdabt_wirte_memory(rdabt_enable_spi2_r12_m[0][0],&rdabt_enable_spi2_r12_m[0][1],1,0x10);
    RDABT_DELAY(1);
    for(i =0; i<sizeof(rdabt_rf_init_12_m)/sizeof(rdabt_rf_init_12_m[0]); i++)
    {
        rdabt_wirte_memory(rdabt_rf_init_12_m[i][0],&rdabt_rf_init_12_m[i][1],1,1);
        RDABT_DELAY(1);

    }
    RDABT_DELAY(1);
}



void Rdabt_Pskey_Write_rf_r12_m(void)
{
    uint16 i;
    for(i=0; i<sizeof(rdabt_pskey_rf_12_m)/sizeof(rdabt_pskey_rf_12_m[0]); i++)
    {

        rdabt_wirte_memory(rdabt_pskey_rf_12_m[i][0],&rdabt_pskey_rf_12_m[i][1],1,0x10);
        RDABT_DELAY(1);
    }
}

void rdabt_DC_write_r12_m(void)
{
    uint16 i;
    for(i=0; i<sizeof(rdabt_dccal_12_m)/sizeof(rdabt_dccal_12_m[0]); i++)
    {

        rdabt_wirte_memory(rdabt_dccal_12_m[i][0],&rdabt_dccal_12_m[i][1],1,1);
        RDABT_DELAY(1);
    }
    rdabt_wirte_memory(rdabt_disable_spi2_r12_m[0][0],&rdabt_disable_spi2_r12_m[0][1],1,0x10);
    RDABT_DELAY(1);

}


void RDABT_core_Intialization_r12_m(void)
{
#ifdef RDA_BT_debug
    EDRV_TRACE(EDRV_BTD_TRC, 0,"RDABT_core_Intialization_r12_m");
#endif
    RDABT_rf_Intialization_r12_m();
    rda_bt_pin_to_low();
    rda_bt_ldoon_toggle_high();
    rda_bt_reset_to_high();
    RDABT_DELAY(10); //wait for digi running
    Rdabt_Pskey_Write_rf_r12_m();
    RDABT_rf_Intialization_r12_m();
    rdabt_DC_write_r12_m();
    //core init
    Rdabt_Pskey_Write_r12();
    Rdabt_patch_write_r12();
    Rdabt_unsniff_prerxon_write_r12();
    Rdabt_setfilter_r12();
    Rdabt_trap_write_r12();
}

#endif

#endif





