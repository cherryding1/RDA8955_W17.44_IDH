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


#ifdef  __RDA_CHIP_R10_5870E__
const uint16 rdabt_dccal_10_e[][2]=
{
    {0x3f,0x0000},
    {0x30,0x0169},
    {0x3C,0x0002},
    {0x30,0x016B},
    {0x30,0x012B}
};



const uint16 rdabt_rf_init_10_e[][2] =
{
    {0x3f,0x0000},//page 0
    {0x01,0x0FFF},//;Padrv_gain_tb_en
    {0x06,0x03FF},//;PSK
    {0x08,0x00FF},//;
    {0x09,0x046C},//;
    {0x0B,0x021F},//;
    {0x0C,0x85D8},//;
    {0x0F,0x1CC8},//;adc_refi_cal_reg<2:0> Set to 000,20100820.
    {0x12,0x0107},//;GFSK
    {0x1B,0xE224},//;
    {0x1C,0xF5F3},//;Xtal_capbank,20100820
    {0x1D,0x21BB},//;5870E EVM,20100827.
    {0x21,0x0000},//;Gain_psk_hi4_PSK_4>5>6>7
    {0x22,0x0000},//;Gain_psk_hi4_PSK_4>5>6>7
    {0x23,0x2458},//;Gain_psk_hi4_PSK_8>9>A>B
    {0x24,0x8ddd},//;Gain_psk_hi4_PSK_C>D>E>F
    {0x25,0x0000},//;Gain_fsk_hi4 GFSK 0>1>2>3
    {0x26,0x0000},//;Gain_fsk_hi4 GFSK 4>5>6>7
    {0x27,0x1235},//;Gain_fsk_hi4 GFSK 8>9>A>B
    {0x28,0x5888},//;Gain_fsk_hi4 GFSK C>D>E>F
    {0x3f,0x0001},//page 1
    {0x00,0x4005},//;;agc0
    {0x01,0x4025},//;;agc1
    {0x02,0x5025},//;;agc2
    {0x03,0x506D},//;;agc3
    {0x04,0x50bD},//;;agc4
    {0x05,0x713D},//;;agc5
    {0x06,0x7A3D},//;;agc6
    {0x07,0x7E3E},//;;agc7
    {0x0A,0x001F},//;
    {0x0D,0x0017},//;APC
    {0x11,0x0000},//;;padrv_gain_1
    {0x12,0x0000},//;;padrv_gain_2
    {0x13,0x0000},//;;padrv_gain_3
    {0x14,0x0000},//;;padrv_gain_4
    {0x15,0x0000},//;;padrv_gain_5
    {0x16,0x0000},//;;padrv_gain_6
    {0x17,0x0000},//;;padrv_gain_7
    {0x18,0x0000},//;;padrv_gain_8
    {0x19,0x0000},//;;padrv_gain_9
    {0x1A,0x1818},//;;padrv_gain_A
    {0x1B,0x1818},//;;padrv_gain_B
    {0x1C,0x6e6e},//;;padrv_gain_C
    {0x1D,0x6e6e},//;;padrv_gain_D
    {0x1E,0xa7a7},//;;padrv_gain_E
//{0x1F,0xb5b5},//;;padrv_gain_F
    {0x1F,0xd8d8},//;;padrv_gain_F gfsk 4dBm,psk 2.4dBm
//{0x1F,0xdddd},//;;padrv_gain_F gfsk 5dBm,psk 3dBm
//{0x1F,0xe4e4},//;;padrv_gain_F gfsk 6dBm,psk 4.1dBm
//{0x1F,0xecec},//;;padrv_gain_F gfsk 7dBm,psk 5.2dBm (max power)
    {0x23,0x4221},//;;use EXT32k
#ifdef __587x_USE_DCDC__
    {0x24,0x48d1},//dcdc_enable set to 1
    {0x26,0x45f5},//lower regulator voltage, current goes from DCDC
#else
    {0x24,0x0090},//dcdc_enable set to 1
    {0x26,0x4535},//;;1,reg_vbit_normal<2:0>Set to 010；2，reg_vbit_deepsleep<2:0> Set to 111。
#endif
    {0x2F,0x114E},//;;
    {0x3f,0x0000},//page 0
};

const uint32 rdabt_pskey_rf_10_e[][2] =
{
#if 1
//rf para setting
    {0x40240000,0x0004F39c},//enable spi2
    {0x800000C0,0x0000000f},//PSKEY: Total number
    {0x800000C4,0x003F0000},//PSKEY: page0
    {0x800000C8,0x00410003},//PSKEY: Swch_clk_adc
    {0x800000CC,0x004224EC},//PSKEY: 625k if
    {0x800000D0,0x0047C939},//PSKEY: mod_adc_clk
    {0x800000D4,0x00431a74},//PSKEY: AM dac gain
    {0x800000D8,0x0044d01A},//PSKEY: gfsk dac gain
    {0x800000DC,0x004a0800},//PSKEY: 4A=0800
    {0x800000E0,0x0054a020},//PSKEY: 54=A020 AGCMAX=A0 AGCMIN=20
    {0x800000E4,0x0055a020},//PSKEY: 55=A020 AGC_TH_max_lg=a0 agc_th_min_lg =0x20
    {0x800000E8,0x0056a542},//PSKEY: 56=a542
    {0x800000EC,0x00574c18},//PSKEY: 57=4c18
    {0x800000F0,0x003f0001},//PSKEY: page=1
    {0x800000F4,0x00410c80},//PSKEY: phase delay
    {0x800000F8,0x003f0000},//PSKEY: page =0
    {0x800000Fc,0x00000000},//PSKEY: page =0
    {0x80000040,0x10000000},//PSKEY: flag
    {0x40240000,0x0000F29c},//enable spi2

#else
//rf para setting
    {0x40240000,0x0004F39c},//enable spi2
    {0x800000C0,0x00000014},//PSKEY: Total number
    {0x800000C4,0x003F0000},//PSKEY: page0
    {0x800000C8,0x00410003},//PSKEY: Swch_clk_adc
    {0x800000CC,0x004224EC},//PSKEY: 625k if
    {0x800000D0,0x0047C939},//PSKEY: mod_adc_clk
    {0x800000D4,0x00431a74},//PSKEY: AM dac gain
    {0x800000D8,0x0044d01A},//PSKEY: gfsk dac gain
    {0x800000DC,0x004a0800},//PSKEY: 4A=0800
    {0x800000E0,0x004d008a},
    {0x800000E4,0x004e1f1f},
    {0x800000E8,0x00694094},
    {0x800000EC,0x006a1b1b},
    {0x800000F0,0x0054a020},//PSKEY: 54=A020 AGCMAX=A0 AGCMIN=20
    {0x800000F4,0x0055a020},//PSKEY: 55=A020 AGC_TH_max_lg=a0 agc_th_min_lg =0x20
    {0x800000F8,0x0056a542},//PSKEY: 56=a542
    {0x800000Fc,0x00574c18},//PSKEY: 57=4c18
    {0x80000100,0x003f0001},//PSKEY: page=1
    {0x80000104,0x00410c80},//PSKEY: phase delay
    {0x80000108,0x004603f1},
    {0x8000010c,0x003f0000},//PSKEY: page =0
    {0x80000110,0x00000000},//PSKEY: page =0
    {0x80000040,0x10000000},//PSKEY: flag
    {0x40240000,0x0000F29c},//enable spi2

#endif
};



void rdabt_DC_write_r10_e(void)
{

    uint16 i=0;
    for(i =0; i<(sizeof(rdabt_dccal_10_e)/sizeof(rdabt_dccal_10_e[0])-1); i++)
    {
        rdabt_iic_rf_write_data(rdabt_dccal_10_e[i][0],&rdabt_dccal_10_e[i][1],1);
    }
    RDABT_DELAY(40);
    rdabt_iic_rf_write_data(rdabt_dccal_10_e[i][0],&rdabt_dccal_10_e[i][1],1);
    RDABT_DELAY(10);


}


void RDABT_rf_Intialization_r10_e(void)
{
    uint16 i=0;
    for(i =0; i<sizeof(rdabt_rf_init_10_e)/sizeof(rdabt_rf_init_10_e[0]); i++)
    {
        rdabt_iic_rf_write_data(rdabt_rf_init_10_e[i][0],&rdabt_rf_init_10_e[i][1],1);
    }
    RDABT_DELAY(10);
}


void Rdabt_Pskey_Write_rf_r10_e(void)
{
    uint16 i;
    for(i=0; i<sizeof(rdabt_pskey_rf_10_e)/sizeof(rdabt_pskey_rf_10_e[0]); i++)
    {
        rdabt_wirte_memory(rdabt_pskey_rf_10_e[i][0],&rdabt_pskey_rf_10_e[i][1],1,0);
        RDABT_DELAY(2);
    }
}



void RDABT_core_Intialization_r10_e(void)
{
    EDRV_TRACE(EDRV_BTD_TRC, 0,"RDABT_core_Intialization_r10_al");


    RDA_bt_Power_On_Reset();
    RDABT_DELAY(10); //wait for digi running
    RDABT_rf_Intialization_r10_e();
    Rdabt_Pskey_Write_rf_r10_e();
    RDABT_DELAY(5); //wait for digi running
    rdabt_DC_write_r10_e();

    Rdabt_Pskey_Write_r10();
    Rdabt_unsniff_prerxon_write_r10();
    Rdabt_patch_write_r10();
    Rdabt_setfilter_r10();
    Rdabt_acl_patch_write_r10();

    Rdabt_trap_write_r10();

}


#endif


#endif
