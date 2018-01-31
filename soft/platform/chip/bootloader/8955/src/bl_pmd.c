/* Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
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

#include "bl_pmd.h"
#include "bl_platform.h"
#include "global_macros.h"
#include "opal.h"

#define ISPI_CS_PMU 0
#define PMD_LDO_VCORE_VALUE 6
#define PMD_VMEM_LEVEL 7

#define SET_CLEAR(set, val, mask) ((set) ? ((val) | (mask)) : ((val) & ~(mask)))

void bl_pmu_write(uint16_t address, uint16_t data)
{
    bl_ispi_write(ISPI_CS_PMU, address, data);
}

uint16_t bl_pmu_read(uint16_t address)
{
    return bl_ispi_read(ISPI_CS_PMU, address);
}

void bl_pmd_enable_ldo_blled(bool enabled)
{
    uint16_t ldosetting1 = bl_pmu_read(RDA_ADDR_LDO_ACTIVE_SETTING1);
    uint16_t ldosetting2 = bl_pmu_read(RDA_ADDR_LDO_ACTIVE_SETTING2);
    ldosetting1 = SET_CLEAR(!enabled, ldosetting1, 1 << 2); // Pd_bl_led_act
    ldosetting2 = SET_CLEAR(!enabled, ldosetting2, 1 << 1); // pd_rgb_led_act

    bl_pmu_write(RDA_ADDR_LDO_ACTIVE_SETTING1, ldosetting1);
    bl_pmu_write(RDA_ADDR_LDO_ACTIVE_SETTING2, ldosetting2);
}

void bl_pmd_set_lcd_level(uint8_t level)
{
    static const uint8_t lightLevelToBacklight[] = {0, 16, 48, 64, 64, 64, 64, 64};

    if (level > 7)
        level = 7;

    uint16_t ledsetting2 = bl_pmu_read(RDA_ADDR_LED_SETTING2);
    if (level == 0)
    {
        ledsetting2 |= RDA_PMU_BL_OFF_ACT;
    }
    else
    {
        uint8_t value = (lightLevelToBacklight[level] & 0xff) >> 4;
        ledsetting2 &= ~RDA_PMU_BL_OFF_ACT;
        ledsetting2 = PMU_SET_BITFIELD(ledsetting2, RDA_PMU_BL_IBIT_ACT, value);
    }

    uint16_t misc48h = bl_pmu_read(RDA_ADDR_LDO_BUCK_48H);
    if (level <= 3)
        misc48h |= PMU_IDIV10_LED;
    else
        misc48h &= ~PMU_IDIV10_LED;

    bl_pmu_write(RDA_ADDR_LED_SETTING2, ledsetting2);
    bl_pmu_write(RDA_ADDR_LDO_BUCK_48H, misc48h);
    bl_pmd_enable_ldo_blled(level == 0 ? false : true);
}

void bl_pmd_set_sublcd_level(uint8_t level)
{
    ; // not exist
}

void bl_pmd_set_led0_level(uint8_t level)
{
    uint16_t ledsetting5 = bl_pmu_read(RDA_ADDR_LED_SETTING5);
    ledsetting5 = SET_CLEAR(level == 0, ledsetting5, RDA_PMU_LED_R_OFF_ACT);
    bl_pmu_write(RDA_ADDR_LED_SETTING5, ledsetting5);
    bl_pmd_enable_ldo_blled(level == 0 ? false : true);
}

void bl_pmd_set_led1_level(uint8_t level)
{
    uint16_t ledsetting5 = bl_pmu_read(RDA_ADDR_LED_SETTING5);
    ledsetting5 = SET_CLEAR(level == 0, ledsetting5, RDA_PMU_LED_G_OFF_ACT);
    bl_pmu_write(RDA_ADDR_LED_SETTING5, ledsetting5);
    bl_pmd_enable_ldo_blled(level == 0 ? false : true);
}

void bl_pmd_set_led2_level(uint8_t level)
{
    uint16_t ledsetting5 = bl_pmu_read(RDA_ADDR_LED_SETTING5);
    ledsetting5 = SET_CLEAR(level == 0, ledsetting5, RDA_PMU_LED_B_OFF_ACT);
    bl_pmu_write(RDA_ADDR_LED_SETTING5, ledsetting5);
    bl_pmd_enable_ldo_blled(level == 0 ? false : true);
}

void bl_pmd_set_led3_level(uint8_t level)
{
    bl_pmd_set_led0_level(level);
}

void bl_pmd_set_keypad_level(uint8_t level)
{
    bl_pmd_set_led1_level(level);
}

void bl_pmd_enable_camera_flash(bool enabled)
{
    bl_pmd_set_led0_level(enabled ? 1 : 0);
}

void bl_pmd_set_charge_current(uint32_t current)
{
    uint32_t value;
    if (current <= 100)
        value = RDA_PMU_CHR_CC_IBIT_100MA;
    else if (current <= 200)
        value = RDA_PMU_CHR_CC_IBIT_200MA;
    else if (current <= 300)
        value = RDA_PMU_CHR_CC_IBIT_300MA;
    else if (current <= 400)
        value = RDA_PMU_CHR_CC_IBIT_400MA;
    else if (current <= 500)
        value = RDA_PMU_CHR_CC_IBIT_500MA;
    else if (current <= 600)
        value = RDA_PMU_CHR_CC_IBIT_600MA;
    else
        value = RDA_PMU_CHR_CC_IBIT_700MA;

    uint16_t chargersetting2 = bl_pmu_read(RDA_ADDR_CHARGER_SETTING2);
    uint16_t chargercontrol = bl_pmu_read(RDA_ADDR_CHARGER_CONTROL);
    chargersetting2 &= ~RDA_PMU_CHR_CC_IBIT_REG_MASK;
    chargersetting2 |= (value | RDA_PMU_CHR_CC_IBIT_DR);
    chargercontrol |= (RDA_PMU_CHR_CC_MODE_DR | RDA_PMU_CHR_CC_MODE_REG);
    bl_pmu_write(RDA_ADDR_CHARGER_CONTROL, chargercontrol);
    bl_pmu_write(RDA_ADDR_CHARGER_SETTING2, chargersetting2);
}

void bl_pmd_set_charge_max_vfb(void)
{
    uint16_t chargersetting2 = bl_pmu_read(RDA_ADDR_CHARGER_SETTING2);
    chargersetting2 = PMU_SET_BITFIELD(chargersetting2, RDA_PMU_CHR_VFB_SEL_REG, 0xe);
    chargersetting2 |= RDA_PMU_CHR_VFB_SEL_DR;
    bl_pmu_write(RDA_ADDR_CHARGER_SETTING2, chargersetting2);
}

void bl_pmd_set_charge_nodr(void)
{
    uint16_t chargercontrol = bl_pmu_read(RDA_ADDR_CHARGER_CONTROL);
    chargercontrol &= ~(RDA_PMU_CHR_CC_MODE_DR | RDA_PMU_CHR_CC_MODE_REG);
    bl_pmu_write(RDA_ADDR_CHARGER_CONTROL, chargercontrol);
}

void bl_pmd_set_low_vbat_shutdown(bool enabled)
{
    if (enabled)
    {
        uint16_t chargersetting1 = bl_pmu_read(RDA_ADDR_CHARGER_SETTING1);
        uint16_t calibsetting1 = bl_pmu_read(RDA_ADDR_CALIBRATION_SETTING1);
        chargersetting1 = PMU_SET_BITFIELD(chargersetting1, RDA_PMU_UV_SEL, 3); // 3.0v
        calibsetting1 &= ~RDA_PMU_VBAT_OVER_3P2_BYPASS;
        bl_pmu_write(RDA_ADDR_CHARGER_SETTING1, chargersetting1);
        bl_pmu_write(RDA_ADDR_CALIBRATION_SETTING1, calibsetting1);
    }
    else
    {
        uint16_t calibsetting1 = bl_pmu_read(RDA_ADDR_CALIBRATION_SETTING1);
        calibsetting1 |= RDA_PMU_VBAT_OVER_3P2_BYPASS;
        bl_pmu_write(RDA_ADDR_CALIBRATION_SETTING1, calibsetting1);
    }
}

void bl_pmd_close(void)
{
    bl_pmd_set_lcd_level(0);
    bl_pmd_set_sublcd_level(0);
    bl_pmd_set_led0_level(0);
    bl_pmd_set_led1_level(0);
    bl_pmd_set_led2_level(0);
    bl_pmu_write(RDA_ADDR_LDO_SETTINGS, 0);
    bl_pmu_write(RDA_ADDR_LDO_POWER_OFF_SETTING2, 0x1fc7);

    bl_pmd_set_charge_current(50); // possible minimum
    bl_pmd_set_charge_max_vfb();
    bl_pmd_set_charge_nodr();
    bl_pmd_set_low_vbat_shutdown(true);

    bl_pmu_write(RDA_ADDR_IRQ_SETTINGS, 0);
}

void bl_pmu_init(void)
{
    extern void bl_xcv_configure(void);
    bl_xcv_configure();

    // Reset the registers except some special regs
    bl_pmu_write(RDA_ADDR_PMU_RESET, RDA_PMU_SOFT_RESETN);
    bl_delay_us(1000); // 1ms
    bl_pmu_write(RDA_ADDR_PMU_RESET, RDA_PMU_REGISTER_RESETN | RDA_PMU_SOFT_RESETN);
    bl_delay_us(1000); // 1ms

    // Disable SDMMC LDO
    uint16_t ldosettings = bl_pmu_read(RDA_ADDR_LDO_SETTINGS);
    ldosettings &= ~RDA_PMU_VMMC_ENABLE;
    bl_pmu_write(RDA_ADDR_LDO_SETTINGS, ldosettings);

    // Set LDO vcore level
    uint16_t ldobuck = bl_pmu_read(RDA_ADDR_LDO_BUCK_4BH);
    ldobuck = PMU_SET_BITFIELD(ldobuck, RDA_PMU_VBUCK_VCORE_LDO_BIT, PMD_LDO_VCORE_VALUE);
    bl_pmu_write(RDA_ADDR_LDO_BUCK_4BH, ldobuck);

    // Set vcore LDO mode
    uint16_t ldosetting1 = bl_pmu_read(RDA_ADDR_LDO_ACTIVE_SETTING1);
    ldosetting1 &= 0x1fff; // [15:13] 000 -> LDO
    bl_pmu_write(RDA_ADDR_LDO_ACTIVE_SETTING1, ldosetting1);

    // Set VMEM voltage
    uint16_t ldosetting3 = bl_pmu_read(RDA_ADDR_LDO_ACTIVE_SETTING3);
    ldosetting3 = PMU_SET_BITFIELD(ldosetting3, RDA_PMU_VMEM_IBIT, PMD_VMEM_LEVEL);
    bl_pmu_write(RDA_ADDR_LDO_ACTIVE_SETTING3, ldosetting3);

    // Power on charger LDO (really needed??)
    uint16_t calibsetting2 = bl_pmu_read(RDA_ADDR_CALIBRATION_SETTING2);
    calibsetting2 &= ~RDA_PMU_PD_CHARGE_LDO;
    bl_pmu_write(RDA_ADDR_CALIBRATION_SETTING2, calibsetting2);

    // Set PMU to active stat
    uint16_t irqsetting = bl_pmu_read(RDA_ADDR_IRQ_SETTINGS);
    irqsetting |= RDA_PMU_PD_MODE_SEL;
    bl_pmu_write(RDA_ADDR_IRQ_SETTINGS, irqsetting);

    // Maybe wait a while (2ms) is helpfull
    bl_delay_us(2000); // 2ms

    // Set charge current. It should be enough for no-battary download
    bl_pmd_set_charge_current(200);

    // Set VFB to max to avoid over charge
    bl_pmd_set_charge_max_vfb();
}

// ============================================================================
// bl_pmd_force_set_core_voltage
// ----------------------------------------------------------------------------
/// Set vcore mode and value without update DDR timing. It is for debugging or
/// ramrun only.
// ============================================================================
void bl_pmd_force_set_core_voltage(bool ldo, uint8_t vcore)
{
    if (ldo)
    {
        uint16_t ldobuck = bl_pmu_read(RDA_ADDR_LDO_BUCK_4BH);
        ldobuck = PMU_SET_BITFIELD(ldobuck, RDA_PMU_VBUCK_VCORE_LDO_BIT, vcore);
        bl_pmu_write(RDA_ADDR_LDO_BUCK_4BH, ldobuck);

        uint16_t ldosetting1 = bl_pmu_read(RDA_ADDR_LDO_ACTIVE_SETTING1);
        ldosetting1 = (ldosetting1 & 0x1fff); // [15:13] 000 -> LDO
        bl_pmu_write(RDA_ADDR_LDO_ACTIVE_SETTING1, ldosetting1);
    }
    else
    {
        uint16_t dcdcbuck = bl_pmu_read(RDA_ADDR_DCDC_BUCK);
        dcdcbuck = PMU_SET_BITFIELD(dcdcbuck, RDA_PMU_VBUCK1_BIT_NLP, vcore);
        bl_pmu_write(RDA_ADDR_DCDC_BUCK, dcdcbuck);

        uint16_t ldosetting1 = bl_pmu_read(RDA_ADDR_LDO_ACTIVE_SETTING1);
        ldosetting1 = (ldosetting1 & 0x1fff) | 0x8000; // [15:13] 100 -> DCDC/PWM
        bl_pmu_write(RDA_ADDR_LDO_ACTIVE_SETTING1, ldosetting1);
    }
}

bool bl_pmd_ac_on(void)
{
    uint16_t chargerstatus = bl_pmu_read(RDA_ADDR_CHARGER_STATUS);
    return (chargerstatus & RDA_PMU_CHR_AC_ON) != 0;
}
