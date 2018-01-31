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
#include "tgt_mcd_cfg.h"
#include <hal_debug.h>
#include "hal_gpio.h"
#include "hal_i2c.h"
#include "hal_sys.h"
#include "sxr_mem.h"
#include "sxr_sbx.h"
#include "sxs_io.h"
#include "sxr_tls.h"
#include "sxr_tim.h"
#include <string.h>
#include <stdio.h>

//#include "wifi_init.h"
#include "wifi_version.h"

//#include "rda5991.h"
#include "wlan_sdio_patch_91.h"
#include "wlan_sdio_patch_91e.h"

/******************************************************************************
 *                文件内部使用常量定义                                        *
 ******************************************************************************/

//extern ATM_MODE_T WIFI_TRACE_FLAG;
/******************************************************************************
 *                文件内部使用宏                                               *
 ******************************************************************************/



/******************************************************************************
 *                文件内部使用的数据类型                                      *
 ******************************************************************************/


/******************************************************************************
 *                全局变量                                                    *
 ******************************************************************************/

//#define RDA5990_WIFI_USE_DCDC_MODE
#define WLAN_USE_DCDC

/******************************************************************************
 *                本地变量                                                    *
 ******************************************************************************/
#ifdef WLAN_USE_DCDC
static const u16 pmu_setting[][2] =
{
    {0x3F,  0x0001},
    {0x22,  0xA1F3},
    {0x24,  0xF908}, //{0x24,  0x8908}  sleep mode with analog power on
    {0x26,  0x8055},
    {0x33,  0x0510},
    {0x39,  0x120C},
    {0x23,  0xA200}, //{0x23,  0x8200}
    {0x25,  0xA247}, //{0x25,  0x8247} heavy_load_dig
    {0x37,  0x0B8A},
    {0x3F,  0x0000},
};
#else
static const u16 pmu_setting[][2] =
{
    {0x3F,  0x0001},
    {0x22,  0xA1F3},
    {0x24,  0x8908},
    {0x26,  0x8555},
    {0x33,  0x0510},
    {0x39,  0x120C},
    {0x23,  0x0200},
    {0x25,  0x027F},
    {0x3F,  0x0000},
};
#endif

/*add according to hongjun's new config*/
static const u16 soft_reset[][2] =
{
    {0x3F, 0x0000},
    //DELAY_MS(10)
    {0x30, 0x8000},
    // DELAY_MS(2000)
    {0x30, 0x0000},
    //DELAY_MS(2000)
};

static const u16 wf_en[][2] =
{
    {0x3F, 0x0001},
    //  {0x28, 0x85A1},//ADD FOR TEST ZHOULY
    {0x31, 0x8B40},  //;WIFI_en=1
    {0x3F, 0x0000},
};

static const u16 wifi_disable[][2] =
{
    {0x3F, 0x0001},
    {0x31, 0x0B40},  //;WIFI_en=0
    {0x3F, 0x0000},
};

#define COMBO_WITH_26MHZ

#ifdef COMBO_WITH_26MHZ
static const u16 wf_rf_setting[][2] =
{
    {0x3F,  0x0000},
    {0x05,  0x0000},
    {0x06,  0x112C}, //wf_bbpll_cpaux_bit[2:0]=100
    {0x07,  0x0820},
    {0x10,  0x9ff7},
    {0x11,  0xFF8A},
    {0x13,  0x5054},
    {0x14,  0x988C},
    {0x15,  0x58eF},
    {0x16,  0x200B},
    {0x19,  0x9C01},
    {0x1C,  0x06E4},
    {0x1D,  0x3A8C},
    {0x22,  0xFF7B},
    {0x23,  0x283C},
    {0x24,  0xA0C4},
    {0x28,  0x4320},
    {0x2A,  0x1036},//{0x2A,  0x0077}
    {0x2B,  0x41BB},
    {0x2D,  0xFF03},
    {0x2F,  0x00DE},
    {0x34,  0x3000},
    {0x35,  0x8011},
    {0x39,  0x8C00},
    {0x40,  0xFFFF},//wf_tmx_gain,wf_pabias
    {0x41,  0xFFFF},
    {0x3F,  0x0001},
    {0x37,  0x0B8A},
    {0x3F,  0x0000},
    {0x30,  0x0100},
    {0x28,  0x4F20},
    DELAY_MS(1)
    {0x28,  0x4320},
    {0x30,  0x0149},
};
#else
static const u16 wf_rf_setting[][2] =
{
    {0x3F,  0x0000},
    {0x03,  0x16AA},
    {0x04,  0xAAAB},
    {0x05,  0x0000},
    {0x06,  0x112C}, //wf_bbpll_cpaux_bit[2:0]=100
    {0x07,  0x0820},
    {0x10,  0x9ff7},
    {0x11,  0xFF8A},
    {0x13,  0x5054},
    {0x14,  0x988C},
    {0x15,  0x58eF},
    {0x16,  0x200B},
    {0x19,  0x9C01},
    {0x1C,  0x06E4},
    {0x1D,  0x3A8C},
    {0x22,  0xFF7B},
    {0x23,  0x3D3C},
    {0x24,  0xA0C4},
    {0x27,  0x5318},
    {0x28,  0x3318},
    {0x2A,  0x0036},
    {0x2B,  0x41BB},
    {0x2D,  0xFF03},
    {0x2F,  0x00DE},
    {0x34,  0x3000},
    {0x35,  0x8011},
    {0x39,  0x8C00},
    {0x40,  0xFFFF},
    {0x41,  0xFFFF},
    {0x3F,  0x0001},
    {0x37,  0x0B8A},
    {0x3F,  0x0000},
    {0x28,  0x1F20},
    {0x28,  0x1320},
    {0x30,  0x0100},
    {0x28,  0x3F18},
    DELAY_MS(1)
    {0x28,  0x3318},
    {0x30,  0x0149},
};
#endif


static const u16 wf_agc_setting_for_dccal[][2] =
{

};


static const u16 wf_agc_setting[][2] =
{
    {0x3F,  0x0000},
    {0x0F,  0x01F7},
    {0x0E,  0x01F0},
    {0x0D,  0x00F0},
    {0x0C,  0x0070},
    {0x0B,  0x0030},
    {0x0A,  0x0010},
    {0x09,  0x3033},
    {0x08,  0x0830},
    {0x3F,  0x0001},
    {0x07,  0x7031},
    {0x06,  0x7011},
    {0x05,  0x7001},
    {0x04,  0x7831},
    {0x03,  0x7811},
    {0x02,  0x7801},
    {0x01,  0x7800},
    {0x00,  0x7800},
    {0x3F,  0x0000},
};

static const u16 wf_calibration[][2] =
{
    {0x3F,  0x0000},
    {0x30,  0x0148},
    {0x30,  0x0149},
    DELAY_MS(50)
};

static const u16 fix_agc_gain[][2] =
{
    {0x3F,  0x0000},
    {0x30,  0x0349},
};

static const u16 bt_rf_setting[][2] =
{
    {0x3F,  0x0000},
    {0x02,  0x0E00},
    {0x08,  0xEFFF},
    {0x0A,  0x09FF},
    {0x11,  0x00B5},
    {0x13,  0x07C0},
    {0x14,  0xFDC4},
    {0x18,  0x2010},
    {0x19,  0x7956},
    {0x1B,  0xDFE0},
    {0x26,  0x7800},
    {0x2B,  0x007F},
    {0x2C,  0x600F},
    {0x2D,  0x007F},
#ifdef COMBO_WITH_26MHZ
    {0x2E,  0xCAB3},
#else
    {0x2E,  0xCAA3},
#endif
    {0x2F,  0x1000},
    {0x30,  0x0141},
    {0x37,  0x4244}, //PSK
    {0x38,  0x4688}, //PSK
    {0x3B,  0x2122}, //GFSK
    {0x3C,  0x2355}, //GFSK
    {0x3F,  0x0001},
    {0x00,  0xFFFF},
    {0x01,  0xFFFF}, // bt power
    {0x02,  0xFFFF},
    {0x03,  0xFFF7},
    {0x04,  0xFFFF},
    {0x05,  0xFFF7},
    {0x06,  0xFFFF},
    {0x07,  0xFFF1},
    {0x08,  0xFFFF},
    {0x09,  0xFFE1},
    {0x0A,  0xFFFF},
    {0x0B,  0xFFE1},
    {0x39,  0x1208},
    {0x3F,  0x0000},
};

static const u16 control_mode_disable[][2] =
{
    {0x3F,  0x0000},
    {0x30,  0x0141},
};

static const u16 bt_en[][2] =
{
    {0x3F, 0x0001},
    {0x28, 0x85A1},  //;bt_en=1
    {0x3F, 0x0000},
};

static const u16 bt_disable[][2] =
{
    {0x3F, 0x0001},
    {0x28, 0x05A1},  //;bt_en=1
    {0x3F, 0x0000},
};


static const u16 rda_5991e_bt_dc_ca_fix_gain[][2] =
{
    {0x3f, 0x0000 },
    {0x30, 0x0141 },
    {0x30, 0x0140 },
    {0x30, 0x0141 },
//  {0x30, 0x0341 },  //force gain level to 7 before lna issue fixed
#ifdef COMBO_WITH_26MHZ
    {0x2e, 0x8ab3 },  //force gain level to 7 before lna issue fixed
#else
    {0x2e, 0x8aa3 },  //force gain level to 7 before lna issue fixed
#endif
    {0x3f, 0x0000 },
};
// add for pta

// add for pta
static const u16 rda_5991e_bt_dc_ca_fix_gain_no_wf[][2] =
{
    {0x3f, 0x0000 },
    {0x30, 0x0140 },
    {0x30, 0x0141 },
    {0x3f, 0x0000 },
};
// add for pta

// add for pta
static const u16 rda_5991e_bt_force_swtrx[][2] =
{
    {0x3f, 0x0000 },
#ifdef COMBO_WITH_26MHZ
    {0x2e, 0xcab3 },  //force gain level to 7 before lna issue fixed
#else
    {0x2e, 0xcaa3 },  //force gain level to 7 before lna issue fixed
#endif
    {0x3f, 0x0000 },
};
// add for pta

// add for pta
static const u16 rda_5991e_bt_no_force_swtrx[][2] =
{
    {0x3f, 0x0000 },
#ifdef COMBO_WITH_26MHZ
    {0x2e, 0x8ab3 },  //force gain level to 7 before lna issue fixed
#else
    {0x2e, 0x8aa3 },  //force gain level to 7 before lna issue fixed
#endif
    {0x3f, 0x0000 },
};
// add for pta

static const u16  bt_dc_cal[][2] =
{
    {0x3F,  0x0000},
    {0x30,  0x0140},
    {0x30,  0x0141},
    DELAY_MS(50)
};




/******************************************************************************
 *                外部函数原型                                                *
 ******************************************************************************/


/******************************************************************************
 *                局部函数原型                                                *
 ******************************************************************************/

static int rda5991_check_wifi_power_on(void)
{
    int ret = 0;
    u16 temp_data;

    ret = wifi_i2c_rf_reg_wr(0x3f, 0x0001);    //wifi_i2c_rf_write_data
    if (ret)
        goto err;

    ret = wifi_i2c_rf_reg_rd(0x31, &temp_data);
    if (ret)
        goto err;

    ret = wifi_i2c_rf_reg_wr(0x3f, 0x0000);

    if (temp_data & 0x8000)
        return 1;
err:
    return 0;
}

static int rda5991_check_bt_power_on(void)
{
    int ret = 0;
    u16 temp_data;

    ret = bt_i2c_rf_reg_wr(0x3f, 0x0001);
    if (ret)
        goto err;

    ret = bt_i2c_rf_reg_rd(0x28, &temp_data);
    if (ret)
        goto err;

    ret = bt_i2c_rf_reg_wr(0x3f, 0x0000);
    if (temp_data & 0x8000)
        return 1;
err:
    return 0;
}

static int rda5991_power_on(int isWifi)
{
    int ret = 0;

    //ret = wifi_rf_reg_arry_wr(soft_reset, ARRAY_SZ(soft_reset));
    // if (ret)
    //     goto power_off;
    //  SXS_TRACE(TSTDOUT, "rda5991_power_on write soft_reset succeed!! \n");

    ret = wifi_rf_reg_arry_wr(pmu_setting, ARRAY_SZ(pmu_setting));

    hal_HstSendEvent(0x77770005); hal_HstSendEvent(ret);
    if (ret)
    {
        SXS_TRACE(TSTDOUT,"rda_5991_wifi_power_on pmu_setting failed!!\r\n",sizeof("rda_5991_wifi_power_on pmu_setting failed!!\r\n"));
        goto err;
    }
    SXS_TRACE(TSTDOUT, "rda5991_power_on write pmu_setting succeed!! \n");

    if (isWifi)         // for wifi
    {
        ret = wifi_rf_reg_arry_wr(wf_en, ARRAY_SZ(wf_en));
        if (ret)
        {
            SXS_TRACE(TSTDOUT,"rda_5991_wifi_power_on write wf_en failed!!\r\n",sizeof("rda_5991_wifi_power_on write wf_en failed!!\r\n"));
            goto err;
        }
        SXS_TRACE(TSTDOUT, "rda5991_power_on write wf_en succeed!! \n");
    }

    /* rda_combo_i2c_unlock(); */
    WIFI_DELAY(5);
    /* rda_combo_i2c_lock(); */

    /* rda_combo_i2c_unlock(); */
    //WIFI_DELAY(100);
    /* rda_combo_i2c_lock(); */
    ret = wifi_rf_reg_arry_wr(soft_reset, ARRAY_SZ(soft_reset));
    if (ret)
    {
        SXS_TRACE(TSTDOUT,"rda_5991_wifi_power_on write soft_reset failed!!\r\n",sizeof("rda_5991_wifi_power_on write soft_reset failed!!\r\n"));
        goto power_off;
    }
    SXS_TRACE(TSTDOUT, "rda5991_power_on write soft_reset succeed!! \n");
    WIFI_DELAY(10);
    ret = wifi_rf_reg_arry_wr(wf_rf_setting,ARRAY_SZ(wf_rf_setting));
    if (ret)
    {
        SXS_TRACE(TSTDOUT,"rda_5991_wifi_power_on write wf_rf_setting failed!!\r\n",sizeof("rda_5991_wifi_power_on write wf_rf_setting failed!!\r\n"));
        goto power_off;
    }
    SXS_TRACE(TSTDOUT, "rda5991_power_on write wf_rf_setting succeed!! \n");

    ret = rdabt_rf_reg_arry_wr(bt_rf_setting, ARRAY_SZ(bt_rf_setting));
    if (ret)
    {
        SXS_TRACE(TSTDOUT,"rda_5991_wifi_power_on write bt_rf_setting failed!!\r\n",sizeof("rda_5991_wifi_power_on write bt_rf_setting failed!!\r\n"));
        goto power_off;
    }
    SXS_TRACE(TSTDOUT, "rda5991_power_on write bt_rf_setting succeed!! \n");
#if 0
    ret = wifi_rf_reg_arry_wr(wf_agc_setting_for_dccal,ARRAY_SZ(wf_agc_setting_for_dccal));    //---------------------
    if (ret)
    {
        SXS_TRACE(TSTDOUT,"rda_5991_wifi_power_on write wf_agc_setting_for_dccal failed!!\r\n",sizeof("rda_5991_wifi_power_on write wf_agc_setting_for_dccal failed!!\r\n"));
        goto power_off;
    }
    SXS_TRACE(TSTDOUT, "rda5991_power_on write wf_agc_setting_for_dccal succeed!! \n",  __func__);
#else
    ret = wifi_rf_reg_arry_wr(wf_agc_setting,ARRAY_SZ(wf_agc_setting));    //---------------------
    if (ret)
    {
        SXS_TRACE(TSTDOUT,"rda_5991_wifi_power_on write wf_agc_setting_for_dccal failed!!\r\n",sizeof("rda_5991_wifi_power_on write wf_agc_setting_for_dccal failed!!\r\n"));
        goto power_off;
    }
    SXS_TRACE(TSTDOUT, "rda5991_power_on write wf_agc_setting succeed!! \n");

#endif

    ret = wifi_rf_reg_arry_wr(wf_calibration,ARRAY_SZ(wf_calibration));   WIFI_DELAY(200);
    if (ret)
    {
        SXS_TRACE(TSTDOUT,"rda_5991_wifi_power_on write wf_calibration failed!!\r\n",sizeof("rda_5991_wifi_power_on write wf_calibration failed!!\r\n"));
        goto power_off;
    }
    SXS_TRACE(TSTDOUT, "rda5991_power_on write wf_calibration succeed!! \n");
#if 0
    ret = wifi_rf_reg_arry_wr(wf_agc_setting,ARRAY_SZ(wf_agc_setting));
    if (ret)
    {
        SXS_TRACE(TSTDOUT,"rda_5991_wifi_power_on write wf_agc_setting failed!!\r\n",sizeof("rda_5991_wifi_power_on write wf_agc_setting failed!!\r\n"));
        goto power_off;
    }
    SXS_TRACE(TSTDOUT, "rda5991_power_on write wf_agc_setting succeed!! \n");
#else

    ret = wifi_rf_reg_arry_wr(fix_agc_gain,ARRAY_SZ(fix_agc_gain));
    if (ret)
    {
        SXS_TRACE(TSTDOUT,"rda_5991_wifi_power_on write wf_agc_setting failed!!\r\n",sizeof("rda_5991_wifi_power_on write wf_agc_setting failed!!\r\n"));
        goto power_off;
    }
    SXS_TRACE(TSTDOUT, "rda5991_power_on write fix_agc_gain succeed!! \n");

#endif
    SXS_TRACE(TSTDOUT,"rda_5991_wifi_power_on write I2C over\r\n",sizeof("rda_5991_wifi_power_on write I2C over\r\n"));
    return 0;
power_off:
    if (isWifi)         // for wifi
    {
        wifi_rf_reg_arry_wr(wifi_disable,ARRAY_SZ(wifi_disable));
    }
    else
    {
        rdabt_rf_reg_arry_wr(bt_disable, ARRAY_SZ(bt_disable));
    }
err:
    return -1;
}

static int rda_5991_wifi_debug_en(void)
{
    u16 temp_data = 0;
    int ret = 0;
    ret = wifi_i2c_rf_reg_wr(0x3f, 0x0001);
    if (ret < 0)
        return -1;

    ret = wifi_i2c_rf_reg_wr(0x28, 0x80a1);
    if (ret < 0)
        return -1;
    ret = wifi_i2c_rf_reg_rd(0x39, &temp_data);
    if (ret < 0)
        return -1;

    ret =
        wifi_i2c_rf_reg_wr(0x39, temp_data | 0x04);
    if (ret < 0)
        return -1;

    ret = wifi_i2c_rf_reg_wr(0x3f, 0x0000);
    if (ret < 0)
        return -1;
    return ret;
}



int rda_5991e_wifi_power_on(void)
{
    int ret = 0, bt_power_on = 0;
    int ret1=0;
    uint8 temp1=0;

    //if bt is power on wait until it's complete
    /* wait_for_completion(&rda_wifi_bt_comp); */

    /* rda_combo_i2c_lock(); */
    SXS_TRACE(TSTDOUT,"5991 Power On\r\n",0);

    /* enable_32k_rtc(CLOCK_WLAN); */
    /* enable_26m_rtc(CLOCK_WLAN); */
    hal_HstSendEvent(0x77770002);
    bt_power_on = rda5991_check_bt_power_on();
    SXS_TRACE(TSTDOUT, "rda_5991_wifi_power_on bt_power_on=%d\n",bt_power_on);
    hal_HstSendEvent(0x77770003); hal_HstSendEvent(bt_power_on);

    if (bt_power_on)
    {
        ret = wifi_rf_reg_arry_wr(wf_en,ARRAY_SZ(wf_en));
        if (ret)
        {
            SXS_TRACE(TSTDOUT,"rda_5991_wifi_power_on bt_power_on\r\n",sizeof("rda_5991_wifi_power_on bt_power_on\r\n"));
            /* rda_combo_i2c_unlock(); */
            goto err;
        }


        SXS_TRACE(TSTDOUT, "rda_5991_wifi_power_on write wf_en succeed!! \n");
        ret = rdabt_rf_reg_arry_wr( rda_5991e_bt_no_force_swtrx,ARRAY_SZ(rda_5991e_bt_no_force_swtrx));

        if (ret)
        {
            SXS_TRACE(TSTDOUT,"rda_5991_wifi_power_on bt_power_on\r\n",sizeof("rda_5991_wifi_power_on bt_power_on\r\n"));
            /* rda_combo_i2c_unlock(); */
            goto err;
        }

        SXS_TRACE(TSTDOUT, "rda_5991_wifi_power_on write rda_5991e_bt_no_force_swtrx succeed!! \n");
        /* rda_combo_i2c_unlock(); */
        WIFI_DELAY(5);
        /* rda_combo_i2c_lock(); */
    }
    else
    {
        ret = rda5991_power_on(1);
        if (ret)
        {
            SXS_TRACE(TSTDOUT,"rda_5991_wifi_power_on failed!!\r\n",sizeof("rda_5991_wifi_power_on failed!!\r\n"));
            goto err;
        }
    }

    ret = wifi_rf_reg_arry_wr(control_mode_disable,ARRAY_SZ(control_mode_disable));
    if (ret)
    {
        SXS_TRACE(TSTDOUT,"rda_5991_wifi_power_on control_mode_disable failed!!\r\n",sizeof("rda_5991_wifi_power_on control_mode_disable failed!!\r\n"));
        goto power_off;

    }
    SXS_TRACE(TSTDOUT, "rda_5991_wifi_power_on write control_mode_disable succeed!! \n");
#if 0
    if (rda_combo_wifi_in_test_mode())
    {
        rda_5991_wifi_debug_en();
        SXS_TRACE(TSTDOUT,
                  "%s: IN test mode, switch uart to WIFI succeed!! \n",
                  __func__);
    }
#endif
    /*  rda_combo_i2c_unlock(); */
    WIFI_DELAY(100);
    /* complete(&rda_wifi_bt_comp); */
    return ret;

power_off:
    wifi_rf_reg_arry_wr(wifi_disable,ARRAY_SZ(wifi_disable));
err:
    /* disable_26m_rtc(CLOCK_WLAN); */
    /* disable_32k_rtc(CLOCK_WLAN); */
    /* rda_combo_i2c_unlock(); */
    return -1;
}


int rda_5991e_wifi_power_off(void)
{
    int ret = 0;
    /* rda_combo_i2c_lock(); */
    ret = wifi_rf_reg_arry_wr(wifi_disable,ARRAY_SZ(wifi_disable));
    if (ret)
    {
        SXS_TRACE(TSTDOUT, "rda_5991_wifi_power_off failed!! \n");
    }
    else
    {
        SXS_TRACE(TSTDOUT, "rda_5991_wifi_power_off succeed!! \n");
    }
    /* disable_26m_rtc(CLOCK_WLAN); */
    /* disable_32k_rtc(CLOCK_WLAN); */
    /* rda_combo_i2c_unlock(); */
    return ret;
}

int rda_5991e_bt_power_on(void)
{
#if 1
    int ret = 0, wifi_power_on = 0;

    //if wifi is power on wait until it's complete
    /* wait_for_completion(&rda_wifi_bt_comp); */

    /* rda_combo_i2c_lock(); */
    /* enable_26m_rtc(CLOCK_BT); */
    /* enable_32k_rtc(CLOCK_BT); */

    wifi_power_on = rda5991_check_wifi_power_on();
    SXS_TRACE(TSTDOUT, "rda_5991_bt_power_on wifi_power_on=%d \n",wifi_power_on);

    if (wifi_power_on)
    {
        ret = rdabt_rf_reg_arry_wr(bt_en, ARRAY_SZ(bt_en));
        if (ret)
            goto err;
        SXS_TRACE(TSTDOUT, "rda_5991_bt_power_on write bt_en succeed!! \n");
        /* rda_combo_i2c_unlock(); */
        WIFI_DELAY(5);
        /* rda_combo_i2c_lock(); */
    }
    else
    {
        ret = rda5991_power_on(0);
        if (ret)
            goto err;
    }

    SXS_TRACE(TSTDOUT, "rda_5991_bt_power_on succeed!! \n");

    /* rda_combo_i2c_unlock(); */
    WIFI_DELAY(10);
    /* complete(&rda_wifi_bt_comp); */
    return ret;

err:
    /* disable_26m_rtc(CLOCK_BT); */
    /* disable_32k_rtc(CLOCK_BT); */
    /* rda_combo_i2c_unlock(); */
#endif
    return -1;
}

int rda_5991e_bt_power_off(void)
{
    int ret = 0;
    /* rda_combo_i2c_lock(); */
    ret = rdabt_rf_reg_arry_wr(bt_disable, ARRAY_SZ(bt_disable));
    if (ret)
    {
        SXS_TRACE(TSTDOUT, "rda_5991_bt_power_off failed!! \n");
    }
    else
    {
        SXS_TRACE(TSTDOUT, "rda_5991_bt_power_off succeed!! \n");
    }
    /* disable_26m_rtc(CLOCK_BT); */
    /* disable_32k_rtc(CLOCK_BT); */
    /* rda_combo_i2c_unlock(); */
    return ret;
}

extern  u8 rda_wlan_version(void);
int rda_5991e_fm_power_on(void)
{
    int ret = 0;
    u16 temp = 0;

    /*
        if (!rda_wifi_rf_client) {
            SXS_TRACE(TSTDOUT,
                   "rda_wifi_rf_client is NULL, rda_fm_power_on failed!\n");
            return -1;
        }
    */

    /* enable_32k_rtc(CLOCK_FM); */
    WIFI_DELAY(8);
    /* rda_combo_i2c_lock(); */

    ret = wifi_i2c_rf_reg_wr(0x3f, 0x0001); // page down
    if (ret < 0)
    {
        SXS_TRACE(TSTDOUT,
                  "%s() write address(0x%02x) with value(0x%04x) failed! \n",
                  __func__, 0x3f, 0x0001);
        goto err;
    }

    if (WIFI_VERSION_5991 == rda_wlan_version())
    {

        ret = wifi_i2c_rf_reg_rd(0x27, &temp);  //read 0xA7
        if (ret < 0)
        {
            SXS_TRACE(TSTDOUT,
                      "%s() read from address(0x%02x) failed! \n",
                      __func__, 0xA7);
            goto err;
        }
        temp = temp | 0x1;  //set bit[0]
        ret = wifi_i2c_rf_reg_wr(0x27, temp);   //write back
        if (ret < 0)
        {
            SXS_TRACE(TSTDOUT,
                      "%s() write address(0x%02x) with value(0x%04x) failed! \n",
                      __func__, 0xA7, temp);
            goto err;
        }

        ret = wifi_i2c_rf_reg_rd(0x39, &temp);  //read 0xB9
        if (ret < 0)
        {
            SXS_TRACE(TSTDOUT,
                      "%s() read from address(0x%02x) failed! \n",
                      __func__, 0xB9);
            goto err;
        }
        temp = temp | (0x1 << 15);  //set bit[15]
        ret = wifi_i2c_rf_reg_wr(0x39, temp);   //write back
        if (ret < 0)
        {
            SXS_TRACE(TSTDOUT,
                      "%s() write address(0x%02x) with value(0x%04x) failed! \n",
                      __func__, 0xB9, temp);
            goto err;
        }
    }

    ret = wifi_i2c_rf_reg_wr(0x3f, 0x0000); // page up
    if (ret < 0)
    {
        SXS_TRACE(TSTDOUT,
                  "%s() write address(0x%02x) with value(0x%04x) failed! \n",
                  __func__, 0x3f, 0x0001);
        goto err;
    }

    /* rda_combo_i2c_unlock(); */
    return 0;

err:
    /* rda_combo_i2c_unlock(); */
    /* disable_32k_rtc(CLOCK_FM); */
    SXS_TRACE(TSTDOUT, "***rda_fm_power_on failed! \n");
    return -1;
}

int rda_5991e_fm_power_off(void)
{
    int ret = 0;
    u16 temp = 0;

    /*
        if (!rda_wifi_rf_client) {
            SXS_TRACE(TSTDOUT,
                   "rda_wifi_rf_client is NULL, rda_fm_power_off failed!\n");
            return -1;
        }

        rda_combo_i2c_lock();
    */

    ret = wifi_i2c_rf_reg_wr(0x3f, 0x0001); // page down
    if (ret < 0)
    {
        SXS_TRACE(TSTDOUT,
                  "%s() write address(0x%02x) with value(0x%04x) failed! \n",
                  __func__, 0x3f, 0x0001);
        goto err;
    }

    if (WIFI_VERSION_5991 == rda_wlan_version())
    {
        ret = wifi_i2c_rf_reg_rd(0x27, &temp);  //read 0xA7
        if (ret < 0)
        {
            SXS_TRACE(TSTDOUT,
                      "%s() read from address(0x%02x) failed! \n",
                      __func__, 0xA7);
            goto err;
        }
        temp = temp & ~(0x1 << 15); //clear bit[0]
        ret = wifi_i2c_rf_reg_wr(0x27, temp);   //write back
        if (ret < 0)
        {
            SXS_TRACE(TSTDOUT,
                      "%s() write address(0x%02x) with value(0x%04x) failed! \n",
                      __func__, 0xA7, temp);
            goto err;
        }
    }

    ret = wifi_i2c_rf_reg_wr(0x3f, 0x0000); // page up
    if (ret < 0)
    {
        SXS_TRACE(TSTDOUT,
                  "%s() write address(0x%02x) with value(0x%04x) failed! \n",
                  __func__, 0x3f, 0x0001);
        goto err;
    }

    /* rda_combo_i2c_unlock(); */
    /* disable_32k_rtc(CLOCK_FM); */
    return 0;
err:
    /* rda_combo_i2c_unlock(); */
    SXS_TRACE(TSTDOUT, "***rda_fm_power_off failed! \n");
    return -1;
}

int rda5991e_sdio_patch(void)
{
//while(1)
    {
        wifi_sdio_patch_core_32(wifi_core_patch_data_32_91e, ARRAY_SIZE(wifi_core_patch_data_32_91e));//
    }
    WIFI_DELAY(10);
    wifi_sdio_patch_core_32(wifi_clock_switch_91e, ARRAY_SIZE(wifi_clock_switch_91e));//
    WIFI_DELAY(10);
    wifi_sdio_patch_core_32(wifi_core_init_data_32_91e,ARRAY_SZ(wifi_core_init_data_32_91e));//
    WIFI_DELAY(10);

    wifi_sdio_patch_core_32(wifi_core_AM_PM_data_32_91e,ARRAY_SZ(wifi_core_AM_PM_data_32_91e));//

    WIFI_DELAY(10);
    /* sdio 8 write */
    wifi_sdio_byte_write(wifi_core_patch_data_91e_8, ARRAY_SZ(wifi_core_patch_data_91e_8));//
    WIFI_DELAY(10);
    SXS_TRACE(TSTDOUT,"rda_5991e_wifi_power_on patch over\r\n",sizeof("rda_5991_wifi_power_on patch over\r\n"));
    return 0;
}


