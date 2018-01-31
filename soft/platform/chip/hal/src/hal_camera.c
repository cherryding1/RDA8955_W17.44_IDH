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


#include "chip_id.h"

#include "cs_types.h"
#include "camera.h"
#include "hal_camera.h"
#include "hal_sys.h"
#include "halp_sys.h"
#include "global_macros.h"
#include "halp_camera.h"
#include "halp_config.h"
#include "hal_gpio.h"
#include "cfg_regs.h"

#include "halp_debug.h"
#include "halp_sys_ifc.h"

#include "hal_clk.h"
#include "hal_error.h"
#include "sys_ctrl.h"

#ifdef TGT_CAMERA_PDN_CHANGE_WITH_ATV
extern BOOL IsAtvPowerOn(void);
#endif

// ============================================================================
// MACROS
// ============================================================================
//
/// Flag to enable the Camera Module test mode. This is useful when the
/// Debug Bus needs to be used: the Camera Bus and the Debug Bus are
/// multiplexed in the chip IOMux, so they cannot be used both at same
/// time. The idea of the Camera test mode is to displays a dummy image
/// instead of getting its data from the sensor.
/// So, when CT_RELEASE=cool_profile, the test mode is enabled: the PXTS,
/// EXL or signal spy cannot be used along with the Camera. When the camera
/// needs to be used, use CT_RELEASE=debug, for instance.
#ifdef ENABLE_PXTS
#define CAM_TEST_MODE     CAMERA_TEST
#else
#define CAM_TEST_MODE     0 // CAMERA_TEST
#endif

#ifdef HAL_CAMERA_PRINTF
#define HAL_CAMERA_TRACE(a, ...)    HAL_TRACE(HAL_CAMERA_TRC, 0, a, ##__VA_ARGS__)
#else
#define HAL_CAMERA_TRACE(a, ...)
#endif
// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

PRIVATE UINT8  g_halCameraIfcChan  = HAL_UNKNOWN_CHANNEL;

PRIVATE HAL_CAMERA_CFG_T g_halCameraConfig =
{
    .rstActiveH = TRUE,
    .pdnActiveH = TRUE,
    .dropFrame  = FALSE,
    .camClkDiv  = 18,
    .endianess  = BYTE_SWAP,
    .camId      = 0,
    .colRatio   = COL_RATIO_1_1,
    .rowRatio   = ROW_RATIO_1_1,
};

static HAL_CLK_T *gCamClk = NULL;

// =============================================================================
// g_halCameraIrqHandler
// -----------------------------------------------------------------------------
/// Variable to store the user irq handler for the camera
/// interruption.
// =============================================================================
PRIVATE HAL_CAMERA_IRQ_HANDLER_T g_halCameraIrqHandler;

PUBLIC VOID hal_CameraSetVsyncInvert(BOOL polarity)
{
    if (polarity)
    {
        // VSYNC low effective
        hwp_camera->CTRL |= CAMERA_VSYNC_POL_INVERT;
        hwp_camera->CAM_SPI_REG_0 |= CAMERA_VSYNC_INV;
    }
    else
    {
        // VSYNC high effective
        hwp_camera->CTRL &= ~CAMERA_VSYNC_POL_INVERT;
        hwp_camera->CAM_SPI_REG_0 &= ~CAMERA_VSYNC_INV;
    }
}
PUBLIC UINT32 hal_CameraIrqstatus(void)
{
    return hwp_camera->STATUS;
}

// ============================================================================
// FUNCTIONS
// ============================================================================

// =============================================================================
// hal_CameraReset(BOOL InReset)
// -----------------------------------------------------------------------------
/// Puts the Camera sensor in Reset or out of Reset.
///
/// @param InReset if true, put the external camera sensor in reset
// =============================================================================
PUBLIC VOID hal_CameraReset(BOOL InReset)
{
    if (g_halCameraConfig.camId == 0)
    {
        // Use sensor 0
        if (g_halCfg->camCfg.camRstRemap.gpioId == HAL_GPIO_NONE)
        {
            // Configures the Reset bit to the camera
            if (InReset)
            {
                hwp_camera->CMD_SET = CAMERA_RESET;
            }
            else
            {
                hwp_camera->CMD_CLR = CAMERA_RESET;
            }
        }
        else
        {
            // Use GPIO
            if ((g_halCameraConfig.rstActiveH && InReset) || ((!g_halCameraConfig.rstActiveH) && (!InReset)))
            {
                hal_GpioSet(g_halCfg->camCfg.camRstRemap);
            }
            else
            {
                hal_GpioClr(g_halCfg->camCfg.camRstRemap);
            }
        }
    }
    else
    {
        // Use sensor 1
        // Use GPIO
        if (g_halCfg->camCfg.cam1RstRemap.gpioId == HAL_GPIO_NONE)
        {
            // Configures the Reset bit to the camera
            if (InReset)
            {
                hwp_camera->CMD_SET = CAMERA_RESET;
            }
            else
            {
                hwp_camera->CMD_CLR = CAMERA_RESET;
            }
        }
        else
        {
            // Use GPIO
            if ((g_halCameraConfig.rstActiveH && InReset) || ((!g_halCameraConfig.rstActiveH) && (!InReset)))
            {
                hal_GpioSet(g_halCfg->camCfg.cam1RstRemap);
            }
            else
            {
                hal_GpioClr(g_halCfg->camCfg.cam1RstRemap);
            }
        }
    }
}

PRIVATE VOID hal_camera_spi_reg_init()
{
    // clear all SPI camera related regs
    hwp_camera->CAM_SPI_REG_0 = 0;
    hwp_camera->CAM_SPI_REG_1 = 0;
    hwp_camera->CAM_SPI_REG_2 = 0;
    hwp_camera->CAM_SPI_REG_3 = 0;
    hwp_camera->CAM_SPI_REG_4 = 0;
    hwp_camera->CAM_SPI_REG_5 = 0;
    hwp_camera->CAM_SPI_REG_6 = 0;

    // set all SPI camera related regs to default
    hwp_camera->CAM_SPI_REG_5 = 0x00ffffff; // sync code 0xffffff by default
    hwp_camera->CAM_SPI_REG_6 = 0x01000240; // by default, frame start id 0x01
    // frame end id 0x00
    // line start id 0x02
    // packet id 0x40
}

// =============================================================================
// hal_CameraOpen
// -----------------------------------------------------------------------------
/// Enable the Camera Control module and set the default configuration
/// for the selected external camera module.  It is both held in reset
/// AND kept in power down.
///
/// In case of dual-sensor platform, the #camId field of the #camConfig
/// parameter is to be used to select which sensor is to be enabled.
///
/// @param camConfig Camera configuration. (cf #HAL_CAMERA_CFG_T).
// =============================================================================
PUBLIC VOID hal_CameraOpen(HAL_CAMERA_CFG_T* camConfig)
{
    UINT32 decimDiv = 0;

    HAL_ASSERT(g_halCameraIfcChan == HAL_UNKNOWN_CHANNEL,
               "hal_CameraOpen: Camera already open");

    g_halCameraConfig = *camConfig;

    HAL_ASSERT(((camConfig->spi_mode == SPI_MODE_MASTER2_1) ||((camConfig->spi_mode == SPI_MODE_MASTER1))
                || (camConfig->spi_mode == SPI_MODE_MASTER2_2)
                || (camConfig->spi_mode == SPI_MODE_MASTER2_4)
                || (camConfig->spi_mode == SPI_MODE_NO)),
               "hal_CameraOpen: unsupported spi mode");

    switch (camConfig->rowRatio)
    {
        case ROW_RATIO_1_1:
            decimDiv |= CAMERA_DECIMROW_ORIGINAL;
            break;
        case ROW_RATIO_1_2:
            decimDiv |= CAMERA_DECIMROW_DIV_2;
            break;
        case ROW_RATIO_1_3:
            decimDiv |= CAMERA_DECIMROW_DIV_3;
            break;
        case ROW_RATIO_1_4:
            decimDiv |= CAMERA_DECIMROW_DIV_4;
            break;
        default:
            HAL_ASSERT(FALSE, "Camera: Wrong row ratio: %d", camConfig->rowRatio);
            break;
    }
    switch (camConfig->colRatio)
    {
        case COL_RATIO_1_1:
            decimDiv |= CAMERA_DECIMCOL_ORIGINAL;
            break;
        case COL_RATIO_1_2:
            decimDiv |= CAMERA_DECIMCOL_DIV_2;
            break;
        case COL_RATIO_1_3:
            decimDiv |= CAMERA_DECIMCOL_DIV_3;
            break;
        case COL_RATIO_1_4:
            decimDiv |= CAMERA_DECIMCOL_DIV_4;
            break;
        default:
            HAL_ASSERT(FALSE, "Camera: Wrong column ratio: %d", camConfig->colRatio);
            break;
    }
    hwp_camera->CTRL  = CAM_TEST_MODE;
    hwp_camera->CTRL |= CAMERA_DATAFORMAT_YUV422;
    hwp_camera->CTRL |= decimDiv;

    if (camConfig->cropEnable)
    {
        hwp_camera->CTRL |= CAMERA_CROPEN_ENABLE;
        hwp_camera->DSTWINCOL =
            CAMERA_DSTWINCOLSTART(camConfig->dstWinColStart) |CAMERA_DSTWINCOLEND(camConfig->dstWinColEnd);
        hwp_camera->DSTWINROW =
            CAMERA_DSTWINROWSTART(camConfig->dstWinRowStart) |CAMERA_DSTWINROWEND(camConfig->dstWinRowEnd);
    }

    if(!g_halCameraConfig.rstActiveH)
    {
        hwp_camera->CTRL |= CAMERA_RESET_POL;
    }
    if(!g_halCameraConfig.pdnActiveH)
    {
        hwp_camera->CTRL |= CAMERA_PWDN_POL;
    }
    if (g_halCameraConfig.dropFrame)
    {
        hwp_camera->CTRL |= CAMERA_DECIMFRM_DIV_2;
    }

    hwp_camera->CTRL |= CAMERA_REORDER(camConfig->reOrder);

    hal_camera_spi_reg_init();
    if (camConfig->spi_mode != SPI_MODE_NO)
    {
        // CAM_SPI_REG_0
        hwp_camera->CAM_SPI_REG_0
        |= CAMERA_LINE_NUM_PER_FRAME(camConfig->spi_pixels_per_column);
        hwp_camera->CAM_SPI_REG_0
        |= CAMERA_BLOCK_NUM_PER_LINE((camConfig->spi_pixels_per_line) >> 1);
        hwp_camera->CAM_SPI_REG_0
        |= CAMERA_YUV_OUT_FORMAT(camConfig->spi_yuv_out);
        hwp_camera->CAM_SPI_REG_0
        |= (camConfig->spi_href_inv) ? CAMERA_HREF_INV : 0;
        hwp_camera->CAM_SPI_REG_0
        |= (camConfig->spi_little_endian_en) ? CAMERA_BIG_END_DIS : 0;
        // CAM_SPI_REG_1
        hwp_camera->CAM_SPI_REG_1 |= CAMERA_CLK_DIV_NUM(camConfig->spi_ctrl_clk_div);
        // Other REGs according to the spi mode
        switch (camConfig->spi_mode)
        {
            case SPI_MODE_MASTER2_1:
                hwp_camera->CAM_SPI_REG_4 |=
                    CAMERA_BLOCK_NUM_PER_PACKET((camConfig->spi_pixels_per_line) >> 1) |
                    CAMERA_SDO_LINE_CHOOSE_BIT(0) |
                    CAMERA_DATA_SIZE_CHOOSE_BIT |
                    CAMERA_IMAGE_WIDTH_CHOOSE_BIT |
                    CAMERA_IMAGE_HEIGHT_CHOOSE_BIT |
                    CAMERA_CAMERA_SPI_MASTER_EN_2;
                break;
            case SPI_MODE_MASTER2_2:
                hwp_camera->CAM_SPI_REG_4 |=
                    CAMERA_BLOCK_NUM_PER_PACKET((camConfig->spi_pixels_per_line) >> 1) |
                    CAMERA_SDO_LINE_CHOOSE_BIT(1) |
                    CAMERA_DATA_SIZE_CHOOSE_BIT |
                    CAMERA_IMAGE_WIDTH_CHOOSE_BIT |
                    CAMERA_IMAGE_HEIGHT_CHOOSE_BIT |
                    CAMERA_CAMERA_SPI_MASTER_EN_2;
                break;
            case SPI_MODE_MASTER2_4:
                hwp_camera->CAM_SPI_REG_4 |=
                    CAMERA_BLOCK_NUM_PER_PACKET((camConfig->spi_pixels_per_line) >> 1) |
                    CAMERA_SDO_LINE_CHOOSE_BIT(2) |
                    CAMERA_DATA_SIZE_CHOOSE_BIT |
                    CAMERA_IMAGE_WIDTH_CHOOSE_BIT |
                    CAMERA_IMAGE_HEIGHT_CHOOSE_BIT |
                    CAMERA_CAMERA_SPI_MASTER_EN_2;
                break;
            case SPI_MODE_MASTER1:
                hwp_camera->CAM_SPI_REG_0 |= CAMERA_CAMERA_SPI_MASTER_EN;
                if (camConfig->spi_ssn_high_en)
                    hwp_camera->CAM_SPI_REG_1 |=  CAMERA_SSN_CM_INV;
                break;
            default:
                break;
        }

        hwp_sysCtrl->Clk_Per_Mode |= SYS_CTRL_MODE_PER_CAMERA_MANUAL;

#ifndef CHIP_DIE_8955
        hwp_sysCtrl->Clk_Other_Enable |= SYS_CTRL_ENABLE_OC_PIX;
#endif

    }

    // Configure Camera Clock Divider
    hal_CameraSetupClockDivider(g_halCameraConfig.camClkDiv);
    // set vsync pole
    hal_CameraSetVsyncInvert(camConfig->vsync_inv);

    if (g_halCameraConfig.camId == 0)
    {
        // Use sensor 0

        // Check compatibility between sensor and HAL camera
        // module configuration.
        HAL_ASSERT(g_halCfg->camCfg.camPdnActiveH == g_halCameraConfig.pdnActiveH
                   && g_halCfg->camCfg.camRstActiveH == g_halCameraConfig.rstActiveH,
                   "Camera sensor and HAL camera module configurations incompatibles !");

        if (g_halCfg->camCfg.camRstRemap.gpioId != HAL_GPIO_NONE)
        {
            if(g_halCfg->camCfg.camRstRemap.type == HAL_GPIO_TYPE_IO) //GPIO
            {
                hal_GpioSetOut(g_halCfg->camCfg.camRstRemap.gpioId);
            }
            else if (g_halCfg->camCfg.camRstRemap.type != HAL_GPIO_TYPE_O)
            {
                HAL_ASSERT(FALSE, "Should Use GPIO/GPO as CAM_RST");
            }
        }

        if (g_halCfg->camCfg.camPdnRemap.gpioId != HAL_GPIO_NONE)
        {
            if (g_halCfg->camCfg.camPdnRemap.type == HAL_GPIO_TYPE_IO) //GPIO
            {
                hal_GpioSetOut(g_halCfg->camCfg.camPdnRemap.gpioId);
            }
            else if (g_halCfg->camCfg.camPdnRemap.type != HAL_GPIO_TYPE_O)
            {
                HAL_ASSERT(FALSE, "Should Use GPIO/GPO as CAM_PDN");
            }
        }
    }
    else
    {
        // Use sensor 1

        // Check compatibility between sensor and HAL camera
        // module configuration.
        HAL_ASSERT(g_halCfg->camCfg.cam1PdnActiveH == g_halCameraConfig.pdnActiveH
                   && g_halCfg->camCfg.cam1RstActiveH == g_halCameraConfig.rstActiveH,
                   "Camera sensor and HAL camera module configurations incompatibles !");

        if (g_halCfg->camCfg.cam1RstRemap.gpioId != HAL_GPIO_NONE)
        {
            if(g_halCfg->camCfg.cam1RstRemap.type == HAL_GPIO_TYPE_IO) //GPIO
            {
                hal_GpioSetOut(g_halCfg->camCfg.cam1RstRemap.gpioId);
            }
            else if (g_halCfg->camCfg.cam1RstRemap.type != HAL_GPIO_TYPE_O)
            {
                HAL_ASSERT(FALSE, "Should Use GPIO/GPO as CAM_RST");
            }
        }

        if (g_halCfg->camCfg.cam1PdnRemap.gpioId != HAL_GPIO_NONE)
        {
            if (g_halCfg->camCfg.cam1PdnRemap.type == HAL_GPIO_TYPE_IO) //GPIO
            {
                hal_GpioSetOut(g_halCfg->camCfg.cam1PdnRemap.gpioId);
            }
            else if (g_halCfg->camCfg.cam1PdnRemap.type != HAL_GPIO_TYPE_O)
            {
                HAL_ASSERT(FALSE, "Should Use GPIO/GPO as CAM_PDN");
            }
        }
    }
#ifdef TGT_CAMERA_PDN_CHANGE_WITH_ATV
    if(!IsAtvPowerOn())
#endif
    {
        hal_CameraReset(FALSE);
        hal_CameraPowerDown(FALSE);
    }
}


// =============================================================================
// hal_CameraClose
// -----------------------------------------------------------------------------
/// Power off the camera sensor by setting the PowerDown bit.
/// Resets the camera sensor by enabling the Camera Reset bit.
/// This function can only be called after the camera transfer has been stopped
/// by a call to #hal_CameraStopXfer().
// =============================================================================
PUBLIC VOID hal_CameraClose(VOID)
{
    HAL_ASSERT(g_halCameraIfcChan == HAL_UNKNOWN_CHANNEL,
               "HAL Camera: %s called while a transfer was in progress.",
               __func__);
#ifdef TGT_CAMERA_PDN_CHANGE_WITH_ATV
    if(!IsAtvPowerOn())
#endif
    {
        hal_CameraReset(TRUE);
    }
    hal_CameraControllerEnable(FALSE);

    hwp_camera->CAM_SPI_REG_0 = 0;
    hwp_camera->CAM_SPI_REG_1 = 0;
    hwp_camera->CAM_SPI_REG_4 = 0;

    hwp_sysCtrl->Clk_Per_Mode &= ~SYS_CTRL_MODE_PER_CAMERA_MANUAL;

#ifndef CHIP_DIE_8955
    hwp_sysCtrl->Clk_Other_Disable = SYS_CTRL_ENABLE_OC_PIX;
#endif

#ifdef TGT_CAMERA_PDN_CHANGE_WITH_ATV
    if(!IsAtvPowerOn())
#endif
    {
        hal_CameraPowerDown(TRUE);
    }

    g_halCameraIfcChan = HAL_UNKNOWN_CHANNEL;
}

PUBLIC VOID hal_CameraPowerDownBoth(BOOL PowerDown)
{
#ifdef TGT_CAMERA_PDN_CHANGE_WITH_ATV
    if (g_halCfg->camCfg.camUsed)
    {
        // Power on sensor 0
        if (g_halCfg->camCfg.camPdnRemap.gpioId == HAL_GPIO_NONE)
        {
            if(PowerDown)
            {
                hwp_camera->CMD_SET = CAMERA_PWDN;
            }
            else
            {
                hwp_camera->CMD_CLR = CAMERA_PWDN;
            }
        }
        else
        {
            if(( PowerDown && g_halCameraConfig.pdnActiveH) || ( !PowerDown && !g_halCameraConfig.pdnActiveH))
            {
                hal_GpioSet(g_halCfg->camCfg.camPdnRemap);
            }
            else
            {
                hal_GpioClr(g_halCfg->camCfg.camPdnRemap);
            }
        }
    }
    if (g_halCfg->camCfg.cam1Used)
    {
        // Power on sensor 1
        if (g_halCfg->camCfg.cam1PdnRemap.gpioId == HAL_GPIO_NONE)
        {
            if(PowerDown)
            {
                hwp_camera->CMD_SET = CAMERA_PWDN;
            }
            else
            {
                hwp_camera->CMD_CLR = CAMERA_PWDN;
            }
        }
        else
        {
            if(( PowerDown && g_halCameraConfig.pdnActiveH) || ( !PowerDown && !g_halCameraConfig.pdnActiveH))
            {
                hal_GpioSet(g_halCfg->camCfg.cam1PdnRemap);
            }
            else
            {
                hal_GpioClr(g_halCfg->camCfg.cam1PdnRemap);
            }
        }
    }
#endif

}
// =============================================================================
// hal_CameraPowerDown(BOOL PowerDown)
// -----------------------------------------------------------------------------
/// Controls the Power Down line of the sensor, turns the clock on and off
/// The camera stays in reset.
/// @param PowerDown: if TRUE the sensor module is in Power Down and the clock is shut down
///                   if FALSE the sensor module is in Power Down and the clock is shut down
// =============================================================================
PUBLIC VOID hal_CameraPowerDown(BOOL PowerDown)
{
    bool ret;
    static BOOL gCamClkEnabled = FALSE;
    if (PowerDown)
    {
        hwp_sysCtrl->Cfg_Clk_Camera_Out = SYS_CTRL_CLK_CAMERA_OUT_EN_DISABLE;
        if (gCamClkEnabled) {
            hal_ClkDisable(gCamClk);
            ret = hal_SwReleaseClk(FOURCC_HAL_CAM);
            HAL_ASSERT((ret == true), "release clk sys err");
            gCamClkEnabled = FALSE;
        }
    }
    else
    {
        if (!gCamClkEnabled) {
            ret = hal_SwRequestClk(FOURCC_HAL_CAM, HAL_CLK_RATE_208M);
            HAL_ASSERT((ret == true), "set clk sys err");

            hal_ClkEnable(gCamClk);
            gCamClkEnabled = TRUE;
        }
        hwp_sysCtrl->Cfg_Clk_Camera_Out |= SYS_CTRL_CLK_CAMERA_OUT_EN_ENABLE;
    }


    hal_TimDelay(16);

    if (g_halCameraConfig.camId == 0)
    {
        // Use sensor 0
        if (!PowerDown && g_halCfg->camCfg.cam1Used)
        {
            // Power off the other sensor
            if (g_halCfg->camCfg.cam1PdnRemap.gpioId == HAL_GPIO_NONE)
            {
                hwp_camera->CMD_SET = CAMERA_PWDN;
            }
            else
            {
                if(g_halCameraConfig.pdnActiveH)
                {
                    hal_GpioSet(g_halCfg->camCfg.cam1PdnRemap);
                }
                else
                {
                    hal_GpioClr(g_halCfg->camCfg.cam1PdnRemap);
                }
            }
        }
        // Power on sensor 0
        if (g_halCfg->camCfg.camPdnRemap.gpioId == HAL_GPIO_NONE)
        {
            if(PowerDown)
            {
                hwp_camera->CMD_SET = CAMERA_PWDN;
            }
            else
            {
                hwp_camera->CMD_CLR = CAMERA_PWDN;
            }
        }
        else
        {
            if((PowerDown && g_halCameraConfig.pdnActiveH) ||
                    (!PowerDown && !g_halCameraConfig.pdnActiveH))
            {
                hal_GpioSet(g_halCfg->camCfg.camPdnRemap);
            }
            else
            {
                hal_GpioClr(g_halCfg->camCfg.camPdnRemap);
            }
        }
    }
    else
    {
        // Use sensor 1
        if (!PowerDown && g_halCfg->camCfg.camUsed)
        {
            // Power off the other sensor
            if (g_halCfg->camCfg.camPdnRemap.gpioId == HAL_GPIO_NONE)
            {
                hwp_camera->CMD_SET = CAMERA_PWDN;
            }
            else
            {
                if(g_halCameraConfig.pdnActiveH)
                {
                    hal_GpioSet(g_halCfg->camCfg.camPdnRemap);
                }
                else
                {
                    hal_GpioClr(g_halCfg->camCfg.camPdnRemap);
                }
            }
        }
        // Power on sensor 1
        if (g_halCfg->camCfg.cam1PdnRemap.gpioId == HAL_GPIO_NONE)
        {
            if(PowerDown)
            {
                hwp_camera->CMD_SET = CAMERA_PWDN;
            }
            else
            {
                hwp_camera->CMD_CLR = CAMERA_PWDN;
            }
        }
        else
        {
            if((PowerDown && g_halCameraConfig.pdnActiveH) ||
                    (!PowerDown && !g_halCameraConfig.pdnActiveH))
            {
                hal_GpioSet(g_halCfg->camCfg.cam1PdnRemap);
            }
            else
            {
                hal_GpioClr(g_halCfg->camCfg.cam1PdnRemap);
            }
        }
    }


    hwp_camera->IRQ_CLEAR = hwp_camera->IRQ_CAUSE;

}


// =============================================================================
// hal_CameraControllerEnable(BOOL enable)
// -----------------------------------------------------------------------------
// =============================================================================
PUBLIC VOID hal_CameraControllerEnable(BOOL enable)
{
    if(enable)
    {
        // Turn on controller and apbi
        hwp_camera->CMD_SET  =  CAMERA_FIFO_RESET;
        hwp_camera->CTRL    |=  CAMERA_DATAFORMAT_YUV422;
        hwp_camera->CTRL    |=  CAMERA_ENABLE;
    }
    else
    {
        // Turn off controller and apbi
        hwp_camera->CTRL    &= ~CAMERA_ENABLE;
    }
}


// ============================================================================
// hal_CameraIrqSetHandler
// ----------------------------------------------------------------------------
/// This function configures which user function will be called by the
/// camera driver when an interruption is generated.
///
/// @param handler Pointer to a user function called when an interruption
/// is generated by the camera driver.
// ============================================================================
PUBLIC VOID hal_CameraIrqSetHandler(HAL_CAMERA_IRQ_HANDLER_T handler)
{
    g_halCameraIrqHandler = handler;
}



// =============================================================================
// hal_CameraIrqConfig()
// -----------------------------------------------------------------------------
/// Configure the desired interrupts
///
/// @param mask Mask to enable specific interrupts.  Valid interrupts are
/// liste in HAL_CAMERA_IRQ_CAUSE_T.
// =============================================================================
PUBLIC VOID hal_CameraIrqSetMask(HAL_CAMERA_IRQ_CAUSE_T mask)
{
    UINT32 realMask = 0;

    if (mask.overflow)
        realMask |= CAMERA_OVFL;

    if (mask.fstart)
        realMask |= CAMERA_VSYNC_R;

    if (mask.fend)
        realMask |= CAMERA_VSYNC_F;

    if (mask.dma)
        realMask |= CAMERA_DMA_DONE;

    hwp_camera->IRQ_CLEAR = realMask;
    hwp_camera->IRQ_MASK  = realMask;
}

PROTECTED VOID hal_CameraIrqHandler(UINT8 interruptId)
{
    HAL_CAMERA_IRQ_CAUSE_T cause = {0, };
    UINT32                 realCause;

    realCause = hwp_camera->IRQ_CAUSE;

    // Clear IRQ
    hwp_camera->IRQ_CLEAR = realCause;

    if(realCause & CAMERA_OVFL)
        cause.overflow = 1;

    if(realCause & CAMERA_VSYNC_R)
        cause.fstart = 1;

    if(realCause & CAMERA_VSYNC_F)
        cause.fend= 1;

    if(realCause & CAMERA_DMA_DONE)
        cause.dma = 1;

    // Call User handler
    if (g_halCameraIrqHandler)
    {
        g_halCameraIrqHandler(cause);
    }
}


// =============================================================================
// hal_CameraStopXfer
// -----------------------------------------------------------------------------
/// Must be called at the end of the Frame Capture
/// If an underflow occured and the IFC tranfer is not complete,
/// this function will handle the channel release
///
/// @param stop If \c TRUE, stops the camera controller.
/// @return 0 when the IC transfer was complete
///         1 when the IFC transfer was not complete and the channel had to be released
// =============================================================================
PUBLIC HAL_CAMERA_XFER_STATUS_T hal_CameraStopXfer(BOOL stop)
{
    UINT32               status;

    // Disable the Camera controller in any case to avoid toggling
    if(stop)
    {
        hal_CameraControllerEnable(FALSE);
    }

    if(g_halCameraIfcChan != HAL_UNKNOWN_CHANNEL)
    {
        /// @todo This test should not be necessary with the new IFC driver behaviour.
        status = hal_SysEnterCriticalSection();
        if(hal_IfcGetOwner(g_halCameraIfcChan) == HAL_IFC_CAMERA_RX)
        {
            UINT32 tc = hal_IfcGetTc(HAL_IFC_CAMERA_RX, g_halCameraIfcChan);
            if(tc != 0)
            {
                //hal_HstSendEvent(tc);
                if(!stop)
                {
                    hal_CameraControllerEnable(FALSE);
                }

                // Flush the remaining data in the IFC Fifo
                hal_IfcChannelFlush(HAL_IFC_CAMERA_RX, g_halCameraIfcChan);
                while(!hal_IfcChannelIsFifoEmpty(HAL_IFC_CAMERA_RX, g_halCameraIfcChan));
                // Release the channel
                hal_IfcChannelRelease(HAL_IFC_CAMERA_RX, g_halCameraIfcChan);

                // Channel is released
                g_halCameraIfcChan = HAL_UNKNOWN_CHANNEL;
                hal_SysExitCriticalSection(status);
                if(!stop)
                {
                    hal_CameraControllerEnable(TRUE);
                }
                // Try to determine why we missed data
                if(hwp_camera->IRQ_CAUSE & CAMERA_OVFL)
                {
                    HAL_CAMERA_TRACE("Overflow during transfer between camera module and IFC");
                }
                else
                {
                    HAL_CAMERA_TRACE("Missing data between external camera and camera module");
                }

                return(XFER_NOT_FINISHED);
            }
            else
            {
                hal_IfcChannelRelease(HAL_IFC_CAMERA_RX, g_halCameraIfcChan);
            }
        }

        // Channel has been released automatically
        g_halCameraIfcChan = HAL_UNKNOWN_CHANNEL;
        hal_SysExitCriticalSection(status);
        if(!(hwp_camera->IRQ_CAUSE & CAMERA_OVFL))
        {
            return(XFER_SUCCESS);
        }

        HAL_CAMERA_TRACE("IFC Transfer finished but Overflow bit is set - Transfer size not matching resolution");
        return(XFER_FINISHED_WITH_OVERFLOW);
    }

    HAL_ASSERT(stop == TRUE, "hal_CameraStopXfer called with no if channel reserved for camera !");
    return(XFER_ERR_WRONG_PARAM);
}


// =============================================================================
// hal_CameraStartXfer
// -----------------------------------------------------------------------------
/// This function begins the IFC transfer of the camera data.  The camera
/// itself is reset and the camera module internal fifo is cleared.  The IFC
/// transfer is then started.
/// @param BufSize This is the size of the buffer in _bytes_
/// @param Buffer Pointer to the video buffer where the IFC will store the data.
/// @return IFC channel number.
// =============================================================================
PUBLIC UINT8 hal_CameraStartXfer(UINT32 bufSize, UINT8* buffer)
{
    hwp_camera->CMD_SET   =  CAMERA_FIFO_RESET;
    hwp_camera->IRQ_CLEAR = CAMERA_OVFL;       // Clear the Overflow bit for checking later
    g_halCameraIfcChan    = hal_IfcTransferStart(HAL_IFC_CAMERA_RX, buffer, bufSize, HAL_IFC_SIZE_32_MODE_AUTO);
    HAL_CAMERA_TRACE("hal_CameraStartXfer: channel %i", g_halCameraIfcChan);
    return(g_halCameraIfcChan);
}

// =============================================================================
// hal_CameraWaitXferFinish
// -----------------------------------------------------------------------------
///if IFC transfer not finish, do nothg.
/// @return 0 when the IC transfer was complete
///         1 when the IFC transfer was not complete
// =============================================================================

PUBLIC HAL_CAMERA_XFER_STATUS_T hal_CameraWaitXferFinish(void)
{
    UINT32                 status;

    if(g_halCameraIfcChan != HAL_UNKNOWN_CHANNEL)
    {
        /// @todo This test should not be necessary with the new IFC driver behaviour.
        status = hal_SysEnterCriticalSection();
        if(hal_IfcGetOwner(g_halCameraIfcChan) == HAL_IFC_CAMERA_RX)
        {
            if(hal_IfcGetTc(HAL_IFC_CAMERA_RX, g_halCameraIfcChan) != 0)
            {
                hal_SysExitCriticalSection(status);

                // Try to determine why we missed data
                if(hwp_camera->IRQ_CAUSE & CAMERA_OVFL)
                {
                    HAL_CAMERA_TRACE("Overflow during transfer between camera module and IFC");
                }
                else
                {
                    HAL_CAMERA_TRACE("Missing data between external camera and camera module");
                }
                return(XFER_NOT_FINISHED);
            }
        }

        hal_SysExitCriticalSection(status);
        if(!(hwp_camera->IRQ_CAUSE & CAMERA_OVFL))
        {
            return(XFER_SUCCESS);
        }

        HAL_CAMERA_TRACE("IFC Transfer finished but Overflow bit is set - Transfer size not matching resolution");
        return(XFER_FINISHED_WITH_OVERFLOW);
    }

    HAL_ASSERT(FALSE, "hal_CameraWaitXferFinish called with no if channel reserved for camera !");
    return(XFER_ERR_WRONG_PARAM);
}

// =============================================================================
// hal_CameraSetupClockDivider
// -----------------------------------------------------------------------------
// =============================================================================
PUBLIC VOID hal_CameraSetupClockDivider(UINT8 divider)
{
    UINT32 newClkScale;
    UINT32 ctrlReg;
    // Save enable bit
    ctrlReg = hwp_sysCtrl->Cfg_Clk_Camera_Out & SYS_CTRL_CLK_CAMERA_OUT_EN_ENABLE;

    // Saturate the divider to the maximum value supported
    // by the hardware.
    if(divider-2 > (SYS_CTRL_CLK_CAMERA_OUT_DIV_MASK>>SYS_CTRL_CLK_CAMERA_OUT_DIV_SHIFT))
    {
        newClkScale = (SYS_CTRL_CLK_CAMERA_OUT_DIV_MASK>>SYS_CTRL_CLK_CAMERA_OUT_DIV_SHIFT) + 2;
    }
    else if(divider < 2)
    {
        newClkScale = 2;
    }
    else
    {
        newClkScale = divider;
    }
#ifdef CHIP_DIE_8955
    // Divider is register value+2, so we take off 2 here.
    ctrlReg |=  SYS_CTRL_CLK_CAM_OUT_SEL_156MHZ      |
                SYS_CTRL_CLK_CAMERA_OUT_DIV(newClkScale)
                | SYS_CTRL_CLK_SPI_CAMERA_DIV(0)
                ;
//hal_HstSendEvent(0x33331210);
//hal_HstSendEvent(newClkScale);
hwp_sysCtrl->Cfg_Clk_Camera_Out = ctrlReg;

#else
    // Divider is register value+2, so we take off 2 here.
    ctrlReg |=  SYS_CTRL_CLK_CAM_OUT_SEL_156MHZ      |
                SYS_CTRL_CLK_CAMERA_OUT_SRC_SEL_FROM_DIVIDER |
                SYS_CTRL_CLK_CAMERA_OUT_DIV(newClkScale-2)
                | SYS_CTRL_CLK_SPI_CAMERA_DIV(0)
                ;

    // Restore initial config with new clock scal.
    hwp_sysCtrl->Cfg_Clk_Camera_Out = ctrlReg;
#endif
}

HAL_MODULE_CLK_INIT(CAM)
{
    gCamClk = hal_ClkGet(FOURCC_CAM);
    HAL_ASSERT((gCamClk != NULL), "Clk CAM not found!");
}

