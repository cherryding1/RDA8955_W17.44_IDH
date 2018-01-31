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


#if defined( __RDA_CHIP_R10_5872H__) || defined(__RDA_CHIP_R10_5868H__)



const uint16 rdabt_dccal_10_al[][2]=
{
    {0x3f,0x0000},
    {0x30,0x0169},
    {0x3C,0x0002},
    {0x30,0x016B},
    {0x30,0x012B}
};



const uint16 rdabt_rf_init_10_al[][2] =
{
    {0x3f,0x0000},  //;page 0
    {0x01,0x1FFF},  //;Padrv_gain_tb_en
    {0x06,0x03FF},  //;PSK
    {0x08,0x00FF},  //;
    {0x09,0x046C},  //;
    {0x0C,0x85D8},  //;
    {0x0B,0x021F},  //;
    {0x0F,0x1CC8},  //;
    {0x12,0x03FF},  //;GFSK
    {0x1B,0xE224},  //;
    {0x1C,0xF5f3},  //;Xtal_capbank
    {0x1d,0x21b9},  //;improve 93ch evm
    {0x22,0x1111},  //;Gain_psk_hi4_PSK_4>5>6>7
    {0x23,0x1111},  //;Gain_psk_hi4_PSK_8>9>A>B
    {0x24,0x246b},  //;Gain_psk_hi4_PSK_C>D>E>F
    {0x25,0x0000},  //;Gain_psk_hi4_PSK_C>D>E>F
    {0x26,0x0000},  //;Gain_fsk_hi4 GFSK 4>5>6>7
    {0x27,0x0011},  //;Gain_fsk_hi4 GFSK 8>9>A>B
    {0x28,0x1248},  //;Gain_fsk_hi4 GFSK C>D>E>F
    {0x3f,0x0001},  //;page 1
    {0x00,0x4005},  //;;agc0
    {0x01,0x4025},  //;;agc1
    {0x02,0x5025},  //;;agc2
    {0x03,0x506d},  //;;agc3
    {0x04,0x50bd},  //;;agc4
    {0x05,0x713d},  //;;agc5
    {0x06,0x7a3d},  //;;agc6
    {0x07,0x7e3e},  //;;agc7
    {0x0A,0x001F},  //;
//{0x16,0xFFF4},  //;;padrv_gain_6
    {0x17,0xb575},  //;;padrv_gain_7
    {0x18,0xb575},  //;;padrv_gain_8;
    {0x19,0xf5b6},  //;;padrv_gain_9
    {0x1A,0xb4f3},  //;;padrv_gain_A
    {0x1B,0xeffd},  //;;padrv_gain_B
    {0x1C,0xf9f9},  //;;padrv_gain_C
    {0x1D,0xf7f4},  //;;padrv_gain_D
    {0x1E,0xf3fb},  //;;padrv_gain_E
    {0x1F,0xFFFf},  //;;padrv_gain_F
//#ifdef __RDA_CHIP_R10_5872H__
    {0x22,0x0E93},  //;clk_source_mode<2:0>set to 000 for sleep 32k
    {0x23,0x0224},  //;Lpo_or_xtal32k set to 0000 for sleep 32k
#ifdef __587x_USE_DCDC__
    {0x24,0x4891},  //;dcdc_enable set to 1
#endif
//#endif
    {0x26,0x44BD},  //;降低睡眠电压，减小睡眠电流
    {0x27,0x0106},  //enlarge OSC stable timer from 4ms to 6ms
    {0x2F,0x114E},  //;;;AGC:
    {0x3f,0x0000}
};


const uint32 rdabt_pskey_rf_10_al[][2] =
{
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
};





const uint32 rdabt_uart_dccal_10_al[][2]=
{
    {0x000002fc,0x00000000},
    {0x000002c0,0x00000169},
    {0x000002f0,0x00000002},
    {0x000002c0,0x0000016B},
    {0x000002c0,0x0000012B}
};



const uint32 rdabt_uart_rf_init_10_al[][2] =
{
    {0x0000003f,0x00000000},  //;page 0
    {0x00000001,0x00001FFF},  //;Padrv_gain_tb_en
    {0x00000006,0x000003FF},  //;PSK
    {0x00000008,0x000000FF},  //;
    {0x00000009,0x0000046C},  //;
    {0x0000000c,0x000085D8},  //;
    {0x0000000b,0x0000021F},  //;
    {0x0000000f,0x00001CC8},  //;
    {0x00000012,0x000003FF},  //;GFSK
    {0x0000001b,0x0000E224},  //;
    {0x0000001c,0x0000F5f3},  //;Xtal_capbank
    {0x0000001d,0x000021b9},  //;improve 93ch evm
    {0x00000022,0x00001111},  //;Gain_psk_hi4_PSK_4>5>6>7
    {0x00000023,0x00001111},  //;Gain_psk_hi4_PSK_8>9>A>B
    {0x00000024,0x0000246b},  //;Gain_psk_hi4_PSK_C>D>E>F
    {0x00000025,0x00000000},  //;Gain_psk_hi4_PSK_C>D>E>F
    {0x00000026,0x00000000},  //;Gain_fsk_hi4 GFSK 4>5>6>7
    {0x00000027,0x00000011},  //;Gain_fsk_hi4 GFSK 8>9>A>B
    {0x00000028,0x00001248},  //;Gain_fsk_hi4 GFSK C>D>E>F
    {0x0000003f,0x00000001},  //;page 1
    {0x00000000,0x00004005},  //;;agc0
    {0x00000001,0x00004025},  //;;agc1
    {0x00000002,0x00005025},  //;;agc2
    {0x00000003,0x0000506d},  //;;agc3
    {0x00000004,0x000050bd},  //;;agc4
    {0x00000005,0x0000713d},  //;;agc5
    {0x00000006,0x00007a3d},  //;;agc6
    {0x00000007,0x00007e3e},  //;;agc7
    {0x0000000a,0x0000001F},  //;
//{0x00000258,0x0000FFF4},  //;;padrv_gain_6
    {0x00000017,0x0000b575},  //;;padrv_gain_7
    {0x00000018,0x0000b575},  //;;padrv_gain_8;
    {0x00000019,0x0000f5b6},  //;;padrv_gain_9
    {0x0000001a,0x0000b4f3},  //;;padrv_gain_A
    {0x0000001b,0x0000effd},  //;;padrv_gain_B
    {0x0000001c,0x0000f9f9},  //;;padrv_gain_C
    {0x0000001d,0x0000f7f4},  //;;padrv_gain_D
    {0x0000001e,0x0000f3fb},  //;;padrv_gain_E
    {0x0000001f,0x0000FFFf},  //;;padrv_gain_F

#ifdef __RDA_CHIP_R10_5872H__
    {0x00000022,0x00000E93},  //;clk_source_mode<2:0>set to 000 for sleep 32k
#ifdef __587x_USE_DCDC__
    {0x00000024,0x00004891},  //;dcdc_enable set to 1
#endif
#endif
    {0x00000026,0x000044BD},  //;降低睡眠电压，减小睡眠电流
    {0x0000002f,0x0000114E},  //;;;AGC:
    {0x0000003f,0x00000000},
    {0x00000000,0x00000000},
};


void rdabt_uart_DC_write_r10_al(void)
{
    uint16 i=0;
    for(i =0; i<(sizeof(rdabt_uart_dccal_10_al)/sizeof(rdabt_uart_dccal_10_al[0])-1); i++)
    {
        rdabt_wirte_memory(rdabt_uart_dccal_10_al[i][0],&rdabt_uart_dccal_10_al[i][1],1,1);
    }
    RDABT_DELAY(40);
    rdabt_wirte_memory(rdabt_uart_dccal_10_al[i][0],&rdabt_uart_dccal_10_al[i][1],1,1);
    RDABT_DELAY(10);


}


void RDABT_uart_rf_Intialization_r10_al(void)
{
    uint16 i=0;
    for(i =0; i<sizeof(rdabt_uart_rf_init_10_al)/sizeof(rdabt_uart_rf_init_10_al[0]); i++)
    {
        rdabt_wirte_memory(rdabt_uart_rf_init_10_al[i][0],&rdabt_uart_rf_init_10_al[i][1],1,1);
        RDABT_DELAY(1);
    }
    RDABT_DELAY(10);
}



void rdabt_DC_write_r10_al(void)
{
    uint16 i=0;
    for(i =0; i<(sizeof(rdabt_dccal_10_al)/sizeof(rdabt_dccal_10_al[0])-1); i++)
    {
        rdabt_iic_rf_write_data(rdabt_dccal_10_al[i][0],&rdabt_dccal_10_al[i][1],1);
    }
    RDABT_DELAY(40);
    rdabt_iic_rf_write_data(rdabt_dccal_10_al[i][0],&rdabt_dccal_10_al[i][1],1);
    RDABT_DELAY(10);


}


void RDABT_rf_Intialization_r10_al(void)
{
    uint16 i=0;
    for(i =0; i<sizeof(rdabt_rf_init_10_al)/sizeof(rdabt_rf_init_10_al[0]); i++)
    {
        rdabt_iic_rf_write_data(rdabt_rf_init_10_al[i][0],&rdabt_rf_init_10_al[i][1],1);
    }
    RDABT_DELAY(10);
}


void Rdabt_Pskey_Write_rf_r10_al(void)
{
    uint16 i;
    for(i=0; i<sizeof(rdabt_pskey_rf_10_al)/sizeof(rdabt_pskey_rf_10_al[0]); i++)
    {
        rdabt_wirte_memory(rdabt_pskey_rf_10_al[i][0],&rdabt_pskey_rf_10_al[i][1],1,0);
        RDABT_DELAY(2);
    }
}



void RDABT_core_Intialization_r10_al(void)
{
    EDRV_TRACE(EDRV_BTD_TRC, 0,"RDABT_core_Intialization_r10_al");


    RDA_bt_Power_On_Reset();
    RDABT_DELAY(10); //wait for digi running
#ifdef __RDABT_USE_IIC__
    RDABT_rf_Intialization_r10_al();
#else
    RDABT_uart_rf_Intialization_r10_al();
#endif
    Rdabt_Pskey_Write_rf_r10_al();

    RDABT_DELAY(5); //wait for digi running
#ifdef __RDABT_USE_IIC__
    rdabt_DC_write_r10_al();
#else
    rdabt_uart_DC_write_r10_al();
#endif
    Rdabt_Pskey_Write_r10();
    Rdabt_unsniff_prerxon_write_r10();
    Rdabt_patch_write_r10();
    Rdabt_setfilter_r10();
    Rdabt_acl_patch_write_r10();
    Rdabt_trap_write_r10();

}


#endif

#endif
