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

#ifndef _CSW_CSP_H_
#define _CSW_CSP_H_

#include "hal_sys.h"

// Calibration related stuff

#define CT_calibration_t                CALIB_CALIBRATION_T
#define CT_calib_pcl2dbm_arfcn_g_t      CALIB_PCL2DBM_ARFCN_G_T
#define CT_calib_rf_chip_name_t         CALIB_RF_CHIP_NAME_T
#define CT_calib_padac_profile_interp_t CALIB_PADAC_PROFILE_INTERP_T
#define CT_calib_padac_profile_t        CALIB_PADAC_PROFILE_T
#define CT_calib_padac_profile_extrem_t CALIB_PADAC_PROFILE_EXTREM_T
#define CT_calib_padac_ramp_t           CALIB_PADAC_RAMP_T
#define CT_calib_padac_ramp_swap_t      CALIB_PADAC_RAMP_SWAP_T
#define CT_calib_padac_low_volt_t       CALIB_PADAC_LOW_VOLT_T
#define CT_calib_padac_low_dac_t        CALIB_PADAC_LOW_DAC_T
#define CT_calib_pa_t                   CALIB_PA_T

// Hazardous ...
// On the flight renaming of calib structures fields
#define code_version                    codeVersion
#define low_dac_limit                   lowDacLimit
#define low_volt_limit                  lowVoltLimit
#define pcl2dbm_arfcn_d                 pcl2dbmArfcnD
#define pcl2dbm_arfcn_g                 pcl2dbmArfcnG
#define pcl2dbm_arfcn_p                 pcl2dbmArfcnP
#define profile_dBm_max_dp              profileDbmMaxDp
#define profile_dBm_max_g               profileDbmMaxG
#define profile_dBm_min_dp              profileDbmMinDp
#define profile_dBm_min_g               profileDbmMinG
#define profile_interp_dp               profileInterpDp
#define profile_interp_g                profileInterpG
#define ramp_high                       rampHigh
#define ramp_low                        rampLow
#define ramp_swap_dp                    rampSwapDp
#define ramp_swap_g                     rampSwapG


#define CT_CALIB_XCV_MASK               CALIB_XCV_MASK
#define CT_CALIB_PA_MASK                CALIB_PA_MASK
#define CT_CALIB_SW_MASK                CALIB_SW_MASK


// PWM related
#define hal_pwl1_Threshold(a)       hal_PwlSelLevel(HAL_PWL_1, a)
#define hal_pwl0_Threshold(a)       hal_PwlGlow(a, 0, 0, FALSE)
#define hal_pwt_Start               hal_BuzzStart
#define hal_pwt_Stop                hal_BuzzStop

// Key
// FIXME Arbitrary *5
#define hal_key_Open(a, b, c)       hal_KeyOpen(a*b*15, c/3)
#define hal_key_IrqSetHandler       hal_KeyIrqSetHandler
#define hal_KeyMask_t               HAL_KEY_IRQ_MASK_T
#define hal_KeyState_t              HAL_KEY_STATE_T
#define hal_key_IrqSetMask          hal_KeyIrqSetMask
#define hal_key_Close               hal_KeyClose



#define hal_RestartLowPowerClockCalibration hal_LpsInvalidateCalib
#define hal_SoftReset                       hal_SysRestart
#define hal_ShutDown                        hal_SysShutdown
#define hal_ResetOut                        hal_SysResetOut
#define hal_UpdateXCpuCache                 hal_SysInvalidateCache
#define hal_EnterCriticalSection            hal_SysEnterCriticalSection
#define hal_ExitCriticalSection             hal_SysExitCriticalSection


// UART
// Irq needs some code change, the rest can be handled with that
#define hal_uart_ClearErrorStatus           hal_UartClearErrorStatus
#define hal_uart_Open                       hal_UartOpen
#define hal_uart_PurgeFifo                  hal_UartFifoFlush
#define hal_uart_SendData                   hal_UartSendData
#define hal_uart_TxFinished                 hal_UartTxFinished
#define hal_uart_TxDmaDone                  hal_UartTxDmaDone
#define hal_uart_GetData                    hal_UartGetData
#define hal_uart_RxDmaDone                  hal_UartRxDmaDone
#define hal_uart_Close                      hal_UartClose
#define hal_uart_RxFifoLevel                hal_UartRxFifoLevel
//#define HAL_UART_IRQ_STATUS_TXMODEMSTATUS
#define HAL_UART_MASK_RX                    HAL_UART_IRQ_STATUS_RXDATAAVAILABLE
#define HAL_UART_MASK_TX                    HAL_UART_IRQ_STATUS_TXDATANEEDED
//#define HAL_UART_IRQ_STATUS_RXTIMEOUT
//#define HAL_UART_IRQ_STATUS_RXLINEERR
#define HAL_UART_MASK_DMA_TX                HAL_UART_IRQ_STATUS_TXDMADONE
#define HAL_UART_MASK_DMA_RX                HAL_UART_IRQ_STATUS_RXDMADONE
//#define HAL_UART_IRQ_STATUS_RXDMATIMEOUT
#define HAL_UART_AFC_MODE_RX_HALF           HAL_UART_AFC_MODE_RX_TRIG_8

// SPI
// Let it be almost Quick & Dirty
#define hal_spi_Close(cs)                   hal_SpiClose(HAL_SPI, cs)
#define hal_spi_ActivateCs(cs)              hal_SpiActivateCs(HAL_SPI, cs)
#define hal_spi_DeActivateCs(cs)            hal_SpiDeActivateCs(HAL_SPI, cs)
#define hal_spi_SendData(cs, data, len)     hal_SpiSendData(HAL_SPI, cs, data, len)
#define hal_spi_GetData(cs, outBuf, len)    hal_SpiGetData(HAL_SPI, cs, outBuf, len)
#define hal_spi_TxFinished(cs)              hal_SpiTxFinished(HAL_SPI, cs)

// Watchdog
#define hal_timWatchDog_Start               hal_TimWatchDogOpen
#define hal_timWatchDog_KeepAlive           hal_TimWatchDogKeepAlive
#define hal_timWatchDog_Disable             hal_TimWatchDogClose



// MEMD
#define hal_bank_layout_t                   MEMD_BANK_LAYOUT_T
#define bank_start_addr                     bankStartAddr
#define sector_layout_table                 sectorLayoutTable
#define hal_flash_Erase                     memd_FlashErase
#define hal_flash_Read                      memd_FlashRead
#define hal_flash_Write                     memd_FlashWrite
#define hal_flash_Lock                      memd_FlashLock
#define hal_flash_Unlock                    memd_FlashUnlock
#define hal_flash_GetGlobalAddress          memd_FlashGetGlobalAddress

// AUDIO
#define  HAL_AUDIO_DTMF_0         AUD_DTMF_0
#define  HAL_AUDIO_DTMF_1         AUD_DTMF_1
#define  HAL_AUDIO_DTMF_2         AUD_DTMF_2
#define  HAL_AUDIO_DTMF_3         AUD_DTMF_3
#define  HAL_AUDIO_DTMF_4         AUD_DTMF_4
#define  HAL_AUDIO_DTMF_5         AUD_DTMF_5
#define  HAL_AUDIO_DTMF_6         AUD_DTMF_6
#define  HAL_AUDIO_DTMF_7         AUD_DTMF_7
#define  HAL_AUDIO_DTMF_8         AUD_DTMF_8
#define  HAL_AUDIO_DTMF_9         AUD_DTMF_9
#define  HAL_AUDIO_DTMF_A         AUD_DTMF_A
#define  HAL_AUDIO_DTMF_B         AUD_DTMF_B
#define  HAL_AUDIO_DTMF_C         AUD_DTMF_C
#define  HAL_AUDIO_DTMF_D         AUD_DTMF_D
#define  HAL_AUDIO_DTMF_S         AUD_DTMF_S
#define  HAL_AUDIO_DTMF_P         AUD_DTMF_P
#define  HAL_AUDIO_COMFORT_425    AUD_COMFORT_425
#define  HAL_AUDIO_COMFORT_950    AUD_COMFORT_950
#define  HAL_AUDIO_COMFORT_1400   AUD_COMFORT_1400
#define  HAL_AUDIO_COMFORT_1800   AUD_COMFORT_1800
#define  HAL_AUDIO_SILENCE        AUD_NO_TONE

#define  HAL_DICTAPHONE_MP3_RING  APS_PLAY_MODE_AMR_RING

#endif // _CSW_CSP_H_

