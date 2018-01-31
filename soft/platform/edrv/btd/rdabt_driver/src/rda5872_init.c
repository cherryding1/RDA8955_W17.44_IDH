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


#if defined( __RDA_CHIP_R10_5872__) || defined(__RDA_CHIP_R10_5868E__)



const uint16 rdabt_rf_init_10[][2] =
{
    {0x3F,0x0001},// page1
    {0x22,0x4A92},//;;pmu reset,clk_source_mode=3'b100 when as 5868e
    {0x22,0x4A93},//;;pmu reset,clk_source_mode=3'b100 when as 5868e
    {0x3F,0x0000},// page0
    {0x30,0x0128},//;;soft_resetn
    {0x30,0x0129},//;;soft_resetn
    {0x3F,0x0000},// page0
//initial configuration
    {0x08,0x0093},// lna_i_bit
    {0x09,0x0435},// rmx_lo_reg
    {0x0A,0x1000},// rx_sys_cal_range
    {0x0c,0x85e8},//
    {0x0E,0x0910},// dc_cal_mode, rxfilter_op_cal_bit_dr=1, Tx_sys_cal_bit_dr=1
    {0x0F,0x1CB2},// adc setting
    {0x11,0x0781},// thermo_ptat_bit, thermo_cal_polarity
    {0x12,0x019F},// padrv_bias_reg, padrv_vtrl, optimize edr tx
    {0x13,0x0048},// agpio mode=output low
    {0x15,0x0524},// pll setting, pll_gain_rx, pll_reg_digi
    {0x16,0x4918},// pll setting, pll_fbc_sel_rx, pll_adcclk_fbc_sel
    {0x18,0x8802},// pll setting, pll_fref_mdll_en_rx=1
    {0x19,0x00C8},// pll setting, pll_adcclk_en=0, pll_cal_cnt_sel=001
    {0x1A,0x10C8},// mdll_adcclk_out_en=1
    {0x1B,0xE224},// mdll setting
    {0x1C,0xF6D3},// xtal_capbank
    {0x1D,0x219D},// optimize edr tx
    {0x1E,0x30C4},// pll_lpf_gain_rx
    {0x1F,0x0200},// Tx_sys_cal_bit_reg
    {0x2A,0x8100},// sdm_tx_clk_mode=000;
    {0x2B,0x00A2},// set sdm_int_dec_sel=01
    {0x3B,0x0001},// ldo_ivref_pd=0
    {0x3F,0x0001},// page1
//agc table
    {0x00,0x0027},// agc0
    {0x01,0x006F},// agc1
    {0x02,0x00AF},// agc2
    {0x03,0x012F},// agc3
    {0x04,0x062F},// agc4
    {0x05,0x362F},// agc5
    {0x06,0x562F},// agc6
    {0x07,0x7E2F},// agc7
//apc table
//{0x18,0x0000},// apc8
    {0x19,0x0000},// apc9
    {0x1A,0x0000},// apc10
    {0x1B,0x0000},// apc11
    {0x1C,0x0040},// apc12
    {0x1D,0x0080},// apc13
    {0x1E,0x00b8},// apc14
    {0x1F,0x00FF},// apc15,max power
    {0x0A,0x001A},// set pll_vco_bit
    {0x0D,0x0016},// set pll_presc

#if defined( __RDA_CHIP_R10_5872__)
    {0x22,0x0E93},// pmu reset
    {0x23,0x0291},// default
#elif defined(__RDA_CHIP_R10_5868E__)
    {0x22,0x4a93},// pmu reset,clk_source_mode=3'b100, when as 5868e
    {0x23,0x0292},// turn off 32K xtal, when as 5868e
#endif

#ifdef __587x_USE_DCDC__
    {0X24,0X4890},// dcdc_sync_clk_div, dcdc enable(enable=0x4890)
#else
    {0x24,0x0090},// dcdc_sync_clk_div, dcdc disable(enable=0x4891)
#endif
    {0x26,0x4525},// set reg_ibit_reg for dvdd=1.00v
    {0x25,0x43e1},//set ldo_vbit_nomal=111 for Temptest, 20100506
    {0x27,0x0108},
    {0x3F,0x0000}// page0
};


const uint16 rdabt_sleep_patch_on_10[][2] =
{
    {0x3f,0x0001},
    {0x2d,0x00ea},
    {0x3f,0x0000}
};


const uint16 rdabt_sleep_patch_off_10[][2] =
{
    {0x3f,0x0001},
    {0x2d,0x00aa},
    {0x3f,0x0000}
};


/*5872 register config, must use IIC*/
const uint32 rdabt_uart_init_10[][2] =
{
    {0x50000010,0x00000130},//
    {0x50000000,0x00000030},//
    {0x50000008,0x00000005},//
    {0x50000010,0x00000102},//
    {0x4024000C,0x0A280408},// pd_osc on timer, 60us->120us
    {0x40240030,0x2f010800} // pd_osc off timer, 4us->2us
};


const uint32 rdabt_pskey_rf_10[][2] =
{
//rf para setting
    {0x40240000,0x0004F39c},//enable spi2
    {0x800000C0,0x0000000c},//PSKEY: Total number
    {0x800000C4,0x003F0000},//PSKEY: page0
    {0x800000C8,0x00410003},//PSKEY: Swch_clk_adc
    {0x800000CC,0x004224EC},//PSKEY: 625k if
    {0x800000D0,0x0047C939},//PSKEY: mod_adc_clk
    {0x800000D4,0x0043B074},//PSKEY: AM dac gain
    {0x800000D8,0x0044d01A},//PSKEY: gfsk dac gain
    {0x800000DC,0x003F0001},//PSKEY: page1
    {0x800000E0,0x00410c80},//PSKEY: phase delay
    {0x800000E4,0x003F0000},//PSKEY: page0
    {0x800000E8,0x00300129},//PSKEY: selcal
    {0x800000EC,0x0030012B},//PSKEY: selcal
    {0x800000f0,0x00000000},//PSKEY: selcal
    {0x80000040,0x10000000},//PSKEY: flag
    {0x40240000,0x0000F29c},//enable spi2

};


void  RDABT_rf_Intialization_r10(void)
{
    uint16 i=0;
    for(i =0; i<sizeof(rdabt_rf_init_10)/sizeof(rdabt_rf_init_10[0]); i++)
    {
        rdabt_iic_rf_write_data(rdabt_rf_init_10[i][0],&rdabt_rf_init_10[i][1],1);
    }
    RDABT_DELAY(10);
}


void  Rdabt_core_uart_Intialization_r10(void)
{
    uint16 i=0;
    for(i =0; i<sizeof(rdabt_uart_init_10)/sizeof(rdabt_uart_init_10[0]); i++)
    {
        rdabt_iic_core_write_data(rdabt_uart_init_10[i][0],&rdabt_uart_init_10[i][1],1);
    }
    RDABT_DELAY(10);
}

void Rdabt_Pskey_Write_rf_r10(void)
{
    uint16 i;
    for(i=0; i<sizeof(rdabt_pskey_rf_10)/sizeof(rdabt_pskey_rf_10[0]); i++)
    {

        rdabt_wirte_memory(rdabt_pskey_rf_10[i][0],&rdabt_pskey_rf_10[i][1],1,0);
        RDABT_DELAY(2);
    }
}

void RDABT_core_Intialization_r10(void)
{
    EDRV_TRACE(EDRV_BTD_TRC, 0,"RDABT_core_Intialization_r10");

    RDA_bt_Power_On_Reset();
    RDABT_DELAY(10); //wait for digi running

    Rdabt_core_uart_Intialization_r10();
    RDABT_DELAY(10); //wait for first uart data
    Rdabt_Pskey_Write_rf_r10();
    Rdabt_Pskey_Write_r10();
    Rdabt_unsniff_prerxon_write_r10();
    Rdabt_patch_write_r10();
    Rdabt_setfilter_r10();
    Rdabt_acl_patch_write_r10();
    Rdabt_trap_write_r10();
}

#endif

#endif
