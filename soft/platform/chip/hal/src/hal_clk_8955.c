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

#include "hal_clk.h"
#include "hal_mem_map.h"
#include "clock_8955.h"
#include "global_macros.h"
#include "sys_ctrl.h"

#include "halp_debug.h"

extern void hal_CameraSetupClockDivider(unsigned char divider);

extern unsigned long hal_I2cUpdateDivider(unsigned long);
extern unsigned long hal_IspiUpdateDivider(unsigned long);
extern unsigned long hal_RfspiUpdateDivider(unsigned long);
extern unsigned long hal_SimUpdateDivider(unsigned long);
extern unsigned long hal_SdioUpdateDivider(unsigned long);
extern unsigned long hal_SdmmcUpdateDivider(unsigned long);
extern unsigned long hal_SdmmcUpdateDivider_2(unsigned long);
extern unsigned long hal_GoudaUpdateTimings(unsigned long);
extern unsigned long hal_GoudaUpdateSerialTiming(unsigned long);
extern unsigned long hal_SpiUpdateAllDividers(unsigned long);

#define PWM_RATE (13000000 / 10)
static unsigned long pwm_set_divider(unsigned long rate)
{
    unsigned long divider = rate / PWM_RATE;

    if (divider > 0xFF)
        divider = 0xFF;
    else
        divider -= 1;

    hwp_sysCtrl->Cfg_Clk_PWM = divider;
    return PWM_RATE;
}

static int cam_set_rate(struct clk *clk, unsigned long rate)
{
    const unsigned long source_rate = HAL_CLK_RATE_156M;
    hal_CameraSetupClockDivider(source_rate / rate);
    return 0;
}
/* pll data */
static struct pll_data abb_pll_data = {
    .input_rate = 624000000,
};

/* gate data */
static struct gate_data pwm_gate = {
    .reg_en = KSEG1(REG_SYS_CTRL_BASE) | 0x18, // Clk_Sys_Enable;
    .val_en = SYS_CTRL_ENABLE_SYSD_PWM,
    .reg_dis = KSEG1(REG_SYS_CTRL_BASE) | 0x1c, // Clk_Sys_Disable;
    .val_dis = SYS_CTRL_DISABLE_SYSD_PWM,
};

static struct gate_data usb_gate = {
    .reg_en = KSEG1(REG_SYS_CTRL_BASE) | 0x3c, // Clk_Other_Enable;
    .val_en = SYS_CTRL_ENABLE_OC_USBPHY,
    .reg_dis = KSEG1(REG_SYS_CTRL_BASE) | 0x40, // Clk_Other_Disable;
    .val_dis = SYS_CTRL_DISABLE_OC_USBPHY,
};

/* divider data */
static struct div_data clk_bb_div = {
    .reg = KSEG1(REG_SYS_CTRL_BASE) | 0x5c,
    .mask = SYS_CTRL_BB_FREQ_MASK,
    .flags = DIV_LOOKUP_TABLE,
};

static struct div_data clk_sys_div = {
    .reg = KSEG1(REG_SYS_CTRL_BASE) | 0x4c,
    .mask = SYS_CTRL_SYS_FREQ_MASK,
    .flags = DIV_LOOKUP_TABLE,
};

static struct div_data clk_voc_div = {
    .reg = KSEG1(REG_SYS_CTRL_BASE) | 0x54,
    .mask = SYS_CTRL_VOC_FREQ_MASK,
    .flags = DIV_LOOKUP_TABLE,
};

static struct div_data xtal_div = {
    .reg = KSEG1(REG_SYS_CTRL_BASE) | 0xD0,
#ifdef CHIP_DIE_8955
    .mask = SYS_CTRL_CFG_XTAL_DIV_MASK,
#endif
    .trig_bit = 5,
    .flags = DIV_TRIGGER,
};

/* clk data */
static struct clk abb_pll = {
    .four_cc = FOURCC_ABBPLL,
    .rate = 624000000,
    .flags = CLK_FIXED,
    .pll_data = &abb_pll_data,
};

static struct clk clk_xtal = {
    .four_cc = FOURCC_XTAL,
    .rate = HAL_CLK_RATE_52M,
    .flags = CLK_FIXED,
    .div_data = &xtal_div,
};

static struct clk clk_usb = {
    .four_cc = FOURCC_USB,
    .rate = HAL_CLK_RATE_48M,
    .parent = &abb_pll,
    .gate_data = &usb_gate,
    .flags = CLK_SYS | CLK_FIXED | GATE_MANUAL,
};

static struct clk clk_voc = {
    .four_cc = FOURCC_VOC,
    .minrate = HAL_CLK_RATE_26M,
    .maxrate = HAL_CLK_RATE_312M,
    .parent = &abb_pll,
    .flags = CLK_PLL,
    .set_rate = rda_set_pllclk_rate,
};

static struct clk clk_bb = {
    .four_cc = FOURCC_BCPU,
    .minrate = HAL_CLK_RATE_26M,
    .maxrate = HAL_CLK_RATE_312M,
    .parent = &abb_pll,
    .flags = CLK_PLL,
    .div_data = &clk_bb_div,
    .set_rate = rda_set_pllclk_rate,
};

/*
static struct clk clk_spiflash;
static struct clk clk_psram;
*/

/* camera clk is derived from 1/4 pll, i.e. 156M,
 * and then pass through a divider */
static struct clk clk_cam = {
    .four_cc = FOURCC_CAM,
    .parent = &abb_pll,
    .flags = CLK_PLL,
    .set_rate = cam_set_rate,
};

static struct clk clk_sys = {
    .four_cc = FOURCC_SYSTEM,
    .rate = HAL_CLK_RATE_52M,
    .minrate = HAL_CLK_RATE_52M,
    .maxrate = HAL_CLK_RATE_312M,
    .parent = &clk_xtal,
    .fast_parent = &abb_pll,
    .slow_parent = &clk_xtal,
    .flags = CLK_PLL,
    .div_data = &clk_sys_div,
    .set_rate = rda_set_pllclk_rate,
    .check_parent = rda_clk_check_parent,
};

static struct clk clk_pwm = {
    .four_cc = FOURCC_PWM,
    .parent = &clk_sys,
    .rate = PWM_RATE,
    .flags = CLK_SYS | CLK_FIXED | GATE_MANUAL,
    .gate_data = &pwm_gate,
    .sysclk_cb = pwm_set_divider,
};

static struct clk clk_i2c = {
    .four_cc = FOURCC_I2C,
    .parent = &clk_sys,
    .flags = CLK_SYS,
    .sysclk_cb = hal_I2cUpdateDivider,
};

static struct clk clk_sdio = {
    .four_cc = FOURCC_SDIO,
    .parent = &clk_sys,
    .flags = CLK_SYS | RATE_REQUEST,
    .sysclk_cb = hal_SdioUpdateDivider,
    .set_rate = rda_set_sysclk_rate,
};

static struct clk clk_sdmmc = {
    .four_cc = FOURCC_SDMMC,
    .parent = &clk_sys,
    .flags = CLK_SYS | RATE_REQUEST,
    .sysclk_cb = hal_SdmmcUpdateDivider,
    .set_rate = rda_set_sysclk_rate,
};

static struct clk clk_sdmmc2 = {
    .four_cc = FOURCC_SDMMC2,
    .parent = &clk_sys,
    .flags = CLK_SYS | RATE_REQUEST,
    .sysclk_cb = hal_SdmmcUpdateDivider_2,
    .set_rate = rda_set_sysclk_rate,
};

static struct clk clk_spi1 = {
    .four_cc = FOURCC_SPI1,
    .parent = &clk_sys,
    .flags = CLK_SYS | RATE_REQUEST,
    .sysclk_cb = hal_SpiUpdateAllDividers,
};

static struct clk clk_spi2 = {
    .four_cc = FOURCC_SPI2,
    .parent = &clk_sys,
    .flags = CLK_SYS | RATE_REQUEST,
    .sysclk_cb = hal_SpiUpdateAllDividers,
};

static struct clk clk_ispi = {
    .four_cc = FOURCC_ISPI,
    .parent = &clk_sys,
    .flags = CLK_SYS | IMPLICIT_ENABLED,
    .sysclk_cb = hal_IspiUpdateDivider,
};

static struct clk clk_rfspi = {
    .four_cc = FOURCC_RFSPI,
    .parent = &clk_sys,
    .flags = CLK_SYS | IMPLICIT_ENABLED,
    .sysclk_cb = hal_RfspiUpdateDivider,
};

static struct clk clk_sim = {
    .four_cc = FOURCC_SIM,
    .parent = &clk_sys,
    .rate = 3250000,
    .flags = CLK_SYS | IMPLICIT_ENABLED,
    .sysclk_cb = hal_SimUpdateDivider,
};

static struct clk clk_lcd = {
    .four_cc = FOURCC_GOUDA,
    .parent = &clk_sys,
    .flags = CLK_SYS | RATE_REQUEST,
    .sysclk_cb = hal_GoudaUpdateSerialTiming,
    .set_rate = rda_set_sysclk_rate,
};

static struct clk clk_dma = {
    .four_cc = FOURCC_DMA,
    .parent = &clk_sys,
    .flags = CLK_SYS | RATE_REQUEST,
};

static struct clk clk_xcv = {
    .four_cc = FOURCC_XCV,
    .parent = &clk_sys,
    .flags = CLK_SYS,
};

/* not sure */
static struct clk clk_out = {
    .four_cc = FOURCC_CLKOUT,
    .parent = &clk_sys,
    .flags = CLK_SYS,
};

/* not sure */
static struct clk clk_audio = {
    .four_cc = FOURCC_AUDIO,
    .parent = &clk_sys,
    .flags = CLK_SYS,
};

static struct clk clk_soft = {
    .four_cc = FOURCC_SOFT,
    .parent = &clk_sys,
    .flags = CLK_SYS | RATE_REQUEST,
    .set_rate = rda_set_sysclk_rate,
};

static struct clk clk_aif = {
    .four_cc = FOURCC_AIF,
    .parent = &clk_xtal,
};

static struct clk clk_uart1 = {
    .four_cc = FOURCC_UART1,
    .parent = &clk_xtal,
};

static struct clk clk_uart2 = {
    .four_cc = FOURCC_UART2,
    .parent = &clk_xtal,
};

static struct clk clk_host_uart = {
    .four_cc = FOURCC_DBG_HOST,
    .parent = &clk_xtal,
};

/* NOTE order: parents must be initialized before children */
static struct clk_lookup rda8955_clks[] = {
    CLK(FOURCC_ABBPLL, &abb_pll),
    CLK(FOURCC_XTAL, &clk_xtal),

    CLK(FOURCC_CAM, &clk_cam),
    CLK(FOURCC_USB, &clk_usb),
    CLK(FOURCC_VOC, &clk_voc),
    CLK(FOURCC_BCPU, &clk_bb),
    CLK(FOURCC_SYSTEM, &clk_sys),

    CLK(FOURCC_PWM, &clk_pwm),
    CLK(FOURCC_I2C, &clk_i2c),
    CLK(FOURCC_SPI1, &clk_spi1),
    CLK(FOURCC_SPI2, &clk_spi2),
    CLK(FOURCC_ISPI, &clk_ispi),
    CLK(FOURCC_RFSPI, &clk_rfspi),
    CLK(FOURCC_SIM, &clk_sim),
    CLK(FOURCC_SDIO, &clk_sdio),
    CLK(FOURCC_SDMMC, &clk_sdmmc),
    CLK(FOURCC_SDMMC2, &clk_sdmmc2),
    CLK(FOURCC_GOUDA, &clk_lcd),
    CLK(FOURCC_DMA, &clk_dma),
    CLK(FOURCC_XCV, &clk_xcv),
    CLK(FOURCC_CLKOUT, &clk_out),
    CLK(FOURCC_SOFT, &clk_soft),
    CLK(FOURCC_AUDIO, &clk_audio),

    CLK(FOURCC_AIF, &clk_aif),
    CLK(FOURCC_UART1, &clk_uart1),
    CLK(FOURCC_UART2, &clk_uart2),
    CLK(FOURCC_DBG_HOST, &clk_host_uart),

    /* ending with a empty element */
    CLK(0, NULL),
};

struct clk *g_clk_sw = NULL;

bool hal_ClkInit()
{
    int ret = rda_clk_init(rda8955_clks);

    HAL_ASSERT((ret == 0), "8955 clk init error!");

    MODULE_CLK_INIT(AIF);
    MODULE_CLK_INIT(AUDIO);
#ifdef HAL_HAS_CAMERA
    MODULE_CLK_INIT(CAM);
#endif
    MODULE_CLK_INIT(DMA);
    MODULE_CLK_INIT(HOST_UART);
    MODULE_CLK_INIT(I2C);
#ifdef HAL_HAS_LCD
    MODULE_CLK_INIT(LCD);
#endif
    MODULE_CLK_INIT(PWM);
    MODULE_CLK_INIT(SIM);
    MODULE_CLK_INIT(SDIO);
    MODULE_CLK_INIT(SDMMC);
    MODULE_CLK_INIT(SDMMC2);
    MODULE_CLK_INIT(SPI1);
    MODULE_CLK_INIT(SPI2);
    MODULE_CLK_INIT(UART1);
    MODULE_CLK_INIT(UART2);
    //MODULE_CLK_INIT(USB);
    MODULE_CLK_INIT(VOC);

    g_clk_sw = clk_get(FOURCC_SOFT);
    HAL_ASSERT((g_clk_sw != NULL), "soft clk error!");
    return ret == 0;
}

const unsigned long gClkRateList[] = {
    HAL_CLK_RATE_26M, HAL_CLK_RATE_39M, HAL_CLK_RATE_52M,
    HAL_CLK_RATE_78M, HAL_CLK_RATE_89M, HAL_CLK_RATE_104M,
    HAL_CLK_RATE_113M, HAL_CLK_RATE_125M, HAL_CLK_RATE_139M,
    HAL_CLK_RATE_156M, HAL_CLK_RATE_178M, HAL_CLK_RATE_208M,
    HAL_CLK_RATE_250M, HAL_CLK_RATE_312M,
};

static unsigned long getRoundRate(unsigned long rate)
{
    int num = sizeof(gClkRateList)/sizeof(gClkRateList[0]);
    if (rate >= gClkRateList[num - 1])
        return gClkRateList[num - 1];

    if (rate <= gClkRateList[0])
        return gClkRateList[0];

    int i = 0, j = num -1;
    while (i < j)
    {
        int mid = (i + j) / 2;
        if (gClkRateList[mid] == rate)
            return rate;

        if (gClkRateList[mid] < rate)
            i = mid + 1;
        else
            j = mid - 1;
    }

    if (rate > gClkRateList[i])
        i++;
    return gClkRateList[i];
}

HAL_CLK_T *hal_ClkGet(const uint32_t fourCC) { return clk_get(fourCC); }

void hal_ClkPut(HAL_CLK_T *clk) { return clk_put(clk); }

bool hal_ClkEnable(HAL_CLK_T *clk) { return 0 == clk_enable(clk); }

void hal_ClkDisable(HAL_CLK_T *clk) { return clk_disable(clk); }

bool hal_ClkIsEnabled(HAL_CLK_T *clk) { return clk_get_usecount(clk) != 0; }

bool hal_ClkSetRate(HAL_CLK_T *clk, uint32_t rate) { return 0 == clk_set_rate(clk, rate); }

uint32_t hal_ClkGetRate(HAL_CLK_T *clk) { return clk_get_rate(clk); }

bool hal_SwRequestClk(const uint32_t fourCC, uint32_t rate)
{
    unsigned long roundRate = getRoundRate(rate);
    return 0 == clk_request(fourCC, roundRate);
}

bool hal_SwReleaseClk(const uint32_t fourCC) { return 0 == clk_release(fourCC); }

bool hal_SwUserIsActive(const uint32_t fourCC) { return clk_is_active(fourCC); }

uint32_t hal_SwGetClkRate(const uint32_t fourCC) { return sw_clk_get_rate(fourCC); }

void hal_clk_dump(bool verbose) HAL_DBG_FUNC;
void hal_clk_dump(bool verbose) { rda_ck_show(verbose); }
