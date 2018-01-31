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
#include "hal_sys.h"
#include "snd_types.h"
#include "aud_m.h"

// This bcpu_main replaces the regular bcpu_main present in the platform library
PROTECTED VOID bcpu_main() {}
PUBLIC VOID patch_Open(VOID) {}
PUBLIC VOID pal_Open(VOID) {}
PUBLIC VOID calib_DaemonInit(VOID) {}
PUBLIC VOID rfd_RegisterYourself(VOID) {}
PUBLIC VOID tsd_Open_pmd() {}
PUBLIC VOID* calib_GetPointers(VOID) {
    return NULL;
}
void rfd_XcvSleep (BOOL toDeepPD) {}
void rfd_XcvWakeUp (BOOL fromDeepPD) {}
int calib_StubMain(int a, char*v[]) {
    return 0;
}
PUBLIC VOID calib_RegisterYourself(VOID) {}
PUBLIC VOID bcpu_RegisterYourself(VOID) {}
VOID pal_Sleep (VOID) {}

VOID rfd_XcvReadChipId(VOID) {}
PUBLIC UINT32 rfd_XcvGetChipId(VOID) {
    return 0;
}
PUBLIC VOID hal_RfspiInitQueue(VOID) {}
PUBLIC VOID hal_RfspiIrqHandler(VOID) {}

CONST UINT32 g_spiCfg = 0;
PUBLIC VOID hal_RfspiOpen(VOID) {}
PUBLIC VOID hal_RfspiNewSingleCmd(VOID) {}
PUBLIC VOID hal_RfspiSendCmd(VOID) {}
PUBLIC VOID xcvBuildSpiCmd(VOID) {}
PUBLIC VOID xcvReadReg(VOID) {}


#include "aud_m.h"
PUBLIC AUD_ERR_T aud_AnalogSetup(SND_ITF_T itf, CONST AUD_DEVICE_CFG_T* cfg) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_AnalogStreamStart(SND_ITF_T itf,
                                       CONST HAL_AIF_STREAM_T* stream,
                                       CONST AUD_DEVICE_CFG_T*        cfg) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_AnalogStreamStop(SND_ITF_T itf) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_AnalogStreamPause(SND_ITF_T itf, BOOL pause) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_AnalogStreamRecord(
    SND_ITF_T itf,
    CONST HAL_AIF_STREAM_T* stream,
    CONST AUD_DEVICE_CFG_T*        cfg) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_AnalogTone(
    SND_ITF_T itf,
    CONST SND_TONE_TYPE_T  tone,
    CONST AUD_DEVICE_CFG_T*       cfg,
    CONST BOOL             start) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_AnalogTonePause(SND_ITF_T itf, BOOL pause) {
    return AUD_ERR_NO;
}

PUBLIC AUD_ERR_T aud_AnalogCalibUpdateValues(SND_ITF_T itf) {
    return AUD_ERR_NO;
}


PUBLIC AUD_ERR_T aud_MaximSetup(SND_ITF_T itf, CONST AUD_DEVICE_CFG_T* cfg) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_MaximStreamStart(SND_ITF_T itf,
                                      CONST HAL_AIF_STREAM_T* stream,
                                      CONST AUD_DEVICE_CFG_T*        cfg) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_MaximStreamStop(SND_ITF_T itf) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_MaximStreamPause(SND_ITF_T itf, BOOL pause) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_MaximStreamRecord(
    SND_ITF_T itf,
    CONST HAL_AIF_STREAM_T* stream,
    CONST AUD_DEVICE_CFG_T*        cfg) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_MaximTone(
    SND_ITF_T itf,
    CONST SND_TONE_TYPE_T  tone,
    CONST AUD_DEVICE_CFG_T*       cfg,
    CONST BOOL             start) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_MaximTonePause(SND_ITF_T itf, BOOL pause) {
    return AUD_ERR_NO;
}

PUBLIC AUD_ERR_T aud_MaximCalibUpdateValues(SND_ITF_T itf) {
    return AUD_ERR_NO;
}

PUBLIC AUD_ERR_T aud_NullSetup(SND_ITF_T itf, CONST AUD_DEVICE_CFG_T* cfg) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_NullStreamStart(SND_ITF_T itf,
                                     CONST HAL_AIF_STREAM_T* stream,
                                     CONST AUD_DEVICE_CFG_T*        cfg) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_NullStreamStop(SND_ITF_T itf) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_NullStreamPause(SND_ITF_T itf, BOOL pause) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_NullStreamRecord(
    SND_ITF_T itf,
    CONST HAL_AIF_STREAM_T* stream,
    CONST AUD_DEVICE_CFG_T*        cfg) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_NullTone(
    SND_ITF_T itf,
    CONST SND_TONE_TYPE_T  tone,
    CONST AUD_DEVICE_CFG_T*       cfg,
    CONST BOOL             start) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_NullTonePause(SND_ITF_T itf, BOOL pause) {
    return AUD_ERR_NO;
}

PUBLIC AUD_ERR_T aud_NullCalibUpdateValues(SND_ITF_T itf) {
    return AUD_ERR_NO;
}


PUBLIC AUD_ERR_T aud_FmSetup(SND_ITF_T itf, CONST AUD_DEVICE_CFG_T* cfg) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_FmStreamStart(SND_ITF_T itf,
                                   CONST HAL_AIF_STREAM_T* stream,
                                   CONST AUD_DEVICE_CFG_T*        cfg) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_FmStreamStop(SND_ITF_T itf) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_FmStreamPause(SND_ITF_T itf, BOOL pause) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_FmStreamRecord(
    SND_ITF_T itf,
    CONST HAL_AIF_STREAM_T* stream,
    CONST AUD_DEVICE_CFG_T*        cfg) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_FmTone(
    SND_ITF_T itf,
    CONST SND_TONE_TYPE_T  tone,
    CONST AUD_DEVICE_CFG_T*       cfg,
    CONST BOOL             start) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_FmTonePause(SND_ITF_T itf, BOOL pause) {
    return AUD_ERR_NO;
}

PUBLIC AUD_ERR_T aud_FmCalibUpdateValues(SND_ITF_T itf) {
    return AUD_ERR_NO;
}

PUBLIC AUD_ERR_T aud_BtStreamStart(
    SND_ITF_T itf,
    CONST HAL_AIF_STREAM_T* stream,
    CONST AUD_DEVICE_CFG_T* cfg) {
    return AUD_ERR_NO;
}

PUBLIC AUD_ERR_T aud_BtStreamRecord(
    SND_ITF_T itf,
    CONST HAL_AIF_STREAM_T* stream,
    CONST AUD_DEVICE_CFG_T* cfg) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_BtStreamStop(SND_ITF_T itf) {
    return AUD_ERR_NO;
}

PUBLIC AUD_ERR_T aud_BtStreamPause(SND_ITF_T itf, BOOL pause) {
    return AUD_ERR_NO;
}

PUBLIC AUD_ERR_T aud_BtSetup(SND_ITF_T itf, CONST AUD_DEVICE_CFG_T* cfg) {
    return AUD_ERR_NO;
}

PUBLIC AUD_ERR_T aud_BtTone(SND_ITF_T itf,
                            CONST SND_TONE_TYPE_T tone,
                            CONST AUD_DEVICE_CFG_T* cfg,
                            CONST BOOL start) {
    return AUD_ERR_NO;
}

PUBLIC AUD_ERR_T aud_BtTonePause(SND_ITF_T itf, BOOL pause) {
    return AUD_ERR_NO;
}

PUBLIC AUD_ERR_T aud_BtCalibUpdateValues(SND_ITF_T itf) {
    return AUD_ERR_NO;
}

PUBLIC AUD_ERR_T aud_TvStreamStart(
    SND_ITF_T itf,
    CONST HAL_AIF_STREAM_T* stream,
    CONST AUD_DEVICE_CFG_T* cfg) {
    return AUD_ERR_NO;
}

PUBLIC AUD_ERR_T aud_TvStreamRecord(
    SND_ITF_T itf,
    CONST HAL_AIF_STREAM_T* stream,
    CONST AUD_DEVICE_CFG_T* cfg) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_TvStreamStop(SND_ITF_T itf) {
    return AUD_ERR_NO;
}

PUBLIC AUD_ERR_T aud_TvStreamPause(SND_ITF_T itf, BOOL pause) {
    return AUD_ERR_NO;
}

PUBLIC AUD_ERR_T aud_TvSetup(SND_ITF_T itf, CONST AUD_DEVICE_CFG_T* cfg) {
    return AUD_ERR_NO;
}

PUBLIC AUD_ERR_T aud_TvTone(SND_ITF_T itf,
                            CONST SND_TONE_TYPE_T tone,
                            CONST AUD_DEVICE_CFG_T* cfg,
                            CONST BOOL start) {
    return AUD_ERR_NO;
}

PUBLIC AUD_ERR_T aud_TvTonePause(SND_ITF_T itf, BOOL pause) {
    return AUD_ERR_NO;
}

PUBLIC AUD_ERR_T aud_TvCalibUpdateValues(SND_ITF_T itf) {
    return AUD_ERR_NO;
}

PUBLIC AUD_ERR_T aud_TiSetup(SND_ITF_T itf, CONST AUD_DEVICE_CFG_T* cfg) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_TiStreamStart(SND_ITF_T itf,
                                   CONST HAL_AIF_STREAM_T* stream,
                                   CONST AUD_DEVICE_CFG_T*        cfg) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_TiStreamStop(SND_ITF_T itf) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_TiStreamPause(SND_ITF_T itf, BOOL pause) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_TiStreamRecord(
    SND_ITF_T itf,
    CONST HAL_AIF_STREAM_T* stream,
    CONST AUD_DEVICE_CFG_T*        cfg) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_TiTone(
    SND_ITF_T itf,
    CONST SND_TONE_TYPE_T  tone,
    CONST AUD_DEVICE_CFG_T*       cfg,
    CONST BOOL             start) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_TiTonePause(SND_ITF_T itf, BOOL pause) {
    return AUD_ERR_NO;
}

PUBLIC AUD_ERR_T aud_TiCalibUpdateValues(SND_ITF_T itf) {
    return AUD_ERR_NO;
}

PUBLIC AUD_ERR_T aud_CodecGalliteSetup(SND_ITF_T itf, CONST AUD_DEVICE_CFG_T* cfg) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_CodecGalliteStreamStart(SND_ITF_T itf,
        CONST HAL_AIF_STREAM_T* stream,
        CONST AUD_DEVICE_CFG_T*        cfg) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_CodecGalliteStreamStop(SND_ITF_T itf) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_CodecGalliteStreamPause(SND_ITF_T itf, BOOL pause) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_CodecGalliteStreamRecord(
    SND_ITF_T itf,
    CONST HAL_AIF_STREAM_T* stream,
    CONST AUD_DEVICE_CFG_T*        cfg) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_CodecGalliteTone(
    SND_ITF_T itf,
    CONST SND_TONE_TYPE_T  tone,
    CONST AUD_DEVICE_CFG_T*       cfg,
    CONST BOOL             start) {
    return AUD_ERR_NO;
}
PUBLIC AUD_ERR_T aud_CodecGalliteTonePause(SND_ITF_T itf, BOOL pause) {
    return AUD_ERR_NO;
}

PUBLIC AUD_ERR_T aud_CodecGalliteCalibUpdateValues(SND_ITF_T itf) {
    return AUD_ERR_NO;
}

PUBLIC VOID aud_InitEarpieceSetting(VOID) {}

PUBLIC VOID aud_EnableEarpieceSetting(BOOL enable) {}

BOOL DM_Reset(VOID) {
    return FALSE;
}

#if 0
// The purpose of the following is to avoid the inclusion of big symbols in
// the final flash programmer lodfile.








void hal_LpsDeepSleep (BOOL die)
{
}

PROTECTED VOID __attribute__((section(".sramtext"))) hal_IrqDispatch(VOID)
{
}


PROTECTED INT32 g_lpsCtx;

/*
PROTECTED VOID hal_HstSleep(VOID)
{
}

PROTECTED VOID hal_HstWakeup(VOID)
{
}

PROTECTED VOID hal_HstRamClientIrq(VOID)
{
}
*/


PUBLIC VOID hal_LpsInvalidateCalib()
{
}

VOID hal_RfspiUpdateDivider(HAL_SYS_FREQ_T fastClock)
{
}

VOID hal_SpiUpdateDivider(HAL_SYS_FREQ_T fastClock)
{
}

VOID hal_SimUpdateDivider(HAL_SYS_FREQ_T fastClock)
{
}

VOID hal_PwmUpdateDivider(HAL_SYS_FREQ_T fastClock)
{
}

#include "hal_pwm.h"

PUBLIC VOID hal_PwlGlow(UINT8 levelMin, UINT8 levelMax,
                        UINT8 pulsePeriod, BOOL pulse)
{
}

PUBLIC VOID hal_PwlSelLevel(HAL_PWL_ID_T id, UINT8 level)
{
}

PROTECTED VOID hal_PwlSetGlowing(HAL_PWL_ID_T id)
{
}

PROTECTED VOID hal_GdbBreaks(UINT32 cpuid)
{
    return;
}

PROTECTED VOID hal_GdbBreakPrint(UINT32 cpuid)
{
}

PROTECTED VOID hal_GdbBreakSoftBreakPoint(UINT32 cpuid)
{
}

PROTECTED VOID hal_GdbBreakBreakPoint(UINT32 cpuid)
{
}

PROTECTED VOID hal_GdbBreakDivBy0(UINT32 cpuid)
{
}

PROTECTED VOID hal_GdbException(UINT32 cpuid, UINT32 cause)
{
}

PROTECTED VOID hal_GdbIrqPage(UINT32 cpuid)
{}
PROTECTED VOID hal_GdbIrqDebug(UINT32 cpuid, UINT32 cause)
{}

PUBLIC VOID hal_LpsCOCtrl(VOID)
{
}

// When you use usb, the LDO is already enabled for usb, be careful normally the
// pmd_Open will disable it.

#include "pmd_m.h"
PUBLIC VOID pmd_Open(CONST PMD_CONFIG_T* pmdConfig)
{
}

PUBLIC VOID pmd_SetPowerMode(PMD_POWERMODE_T powerMode)
{
}

PUBLIC UINT32 pmd_ConfigureRfLowPower(BOOL xcvUseVRF, BOOL xcvLowPowerKeepVRFOn)
{
    return 0;
}

PUBLIC VOID pmd_EnablePower(PMD_POWER_ID_T id, BOOL on)
{
}

PUBLIC VOID pmd_RegisterYourself(VOID)
{
}



#if (CHIP_HAS_USB == 1) && defined(FASTPF_USE_USB)
// USB need a lot of stuff, so stub what can be stubbed
PUBLIC HAL_USB_EP_DESCRIPTOR_T*
hal_UsbDescriptorNewCmdEpDescriptor(UINT8 ep, HAL_USB_CALLBACK_T callback) {
    return 0;
}

PUBLIC HAL_USB_EP_DESCRIPTOR_T*
hal_UsbDescriptorNewBulkEpDescriptor(UINT8 ep, HAL_USB_CALLBACK_T callback) {
    return 0;
}

PUBLIC HAL_USB_EP_DESCRIPTOR_T*
hal_UsbDescriptorNewInterruptEpDescriptor(UINT8 ep, UINT8 interval, HAL_USB_CALLBACK_T callback) {
    return 0;
}

PUBLIC HAL_USB_EP_DESCRIPTOR_T*
hal_UsbDescriptorNewIsoEpDescriptor(UINT8 ep, HAL_USB_CALLBACK_T callback) {
    return 0;
}

PUBLIC HAL_USB_INTERFACE_DESCRIPTOR_T*
hal_UsbDescriptorNewInterfaceDescriptor(
    UINT8 usbClass,
    UINT8 usbSubClass,
    UINT8 usbProto,
    UINT8 interfaceIdx,
    HAL_USB_EP_DESCRIPTOR_T** epList,
    UINT8* description,
    HAL_USB_CALLBACK_T callback) {
    return 0;
}

PUBLIC VOID hal_UsbDescriptorCleanInterfaceListDescriptor(HAL_USB_INTERFACE_DESCRIPTOR_T** interfaces) {}

PUBLIC VOID hal_UsbDescriptorCleanDeviceDescriptor(HAL_USB_DEVICE_DESCRIPTOR_T* dev) {}

PUBLIC HAL_USB_INTERFACE_DESCRIPTOR_T*
hal_UsbDescriptorCopyInterfaceDescriptor(CONST HAL_USB_INTERFACE_DESCRIPTOR_T* interface) {
    return 0;
}

#else // USE_HST
// no USB can stub out usb
// usb (used by hal_sys)
PUBLIC BOOL hal_HostUsbSend(VOID)
{
    return FALSE;
}

PUBLIC UINT8 hal_HostUsbRecv(VOID)
{
    return 0;
}

// pmd
#endif

VOID sxr_Tampax (UINT32 Period, UINT8 I2d) {}

VOID sxs_BackTrace (UINT32 Ra, UINT32 *Sp)
{}
VOID sxs_SaveRmtBuffer (VOID) {}

UINT32 sxs_Boot;

VOID sxs_Raise (UINT32 Id, CONST CHAR *Fmt, ...) {}

#include <stdarg.h>
VOID sxs_vprintf (UINT32 Id, CONST CHAR *Fmt, va_list argPtr) {}

#include "hal_debug.h"
PUBLIC VOID hal_DbgPortSetup(HAL_DBG_PORT_SEL_T mode)
{}
PUBLIC VOID hal_DbgPxtsSetup(UINT16 mask) {}

PUBLIC VOID hal_DbgAssert(CONST CHAR* format, ...)
{}
PUBLIC VOID hal_DbgPxtsSendCode(HAL_DBG_PXTS_LEVEL_T level, UINT16 code)
{}


PUBLIC VOID hal_DbgPageProtectSetup(
    HAL_DBG_PAGE_NUM_T pageNum, HAL_DBG_PAGE_SPY_MODE_T mode,
    UINT32 startAddr, UINT32 endAddr) {}

PUBLIC VOID hal_DbgPageProtectEnable(HAL_DBG_PAGE_NUM_T pageNum)
{}


PUBLIC VOID hal_DbgFatalTrigger(UINT8 trigId)
{}
PUBLIC VOID hal_DbgTrace(UINT32 level, UINT32 tsmap, CONST CHAR *fmt, ...)
{}


PUBLIC VOID sx_RegisterYourself(VOID) {}

PUBLIC VOID _sxr_StartFunctionTimer(VOID) {}

#if (CHIP_HAS_STEREO_DAC == 1)
VOID hal_AnaAudioStereoDacPreOpen(VOID) {}
#endif

// Watched through hal_access in the map engine,
// avoid linking with hal_speech.o
PROTECTED UINT32 g_halSpeechFifo = 0;
VOID hal_SpeechFifoReset(VOID) {}

PUBLIC VOID hal_GdbFlushCacheData(VOID) {}


VOID sxr_Sleep (UINT32 Period) {}


// To avoid including the LPS driver,
// which includes itself a lot of other objects.
PUBLIC VOID hal_LpsSetControlPin(VOID)
{
}
#endif //0
#if defined(RECV_USE_SPK_LINE) || defined(AUD_SPK_ON_WITH_LCD) || defined(AUD_EXT_PA_ON_WITH_LCD)
PUBLIC VOID aud_LcdPowerOnCallback(VOID)
{
}
#endif

#if  0//(CHIP_HAS_GOUDA == 1)
typedef enum
{
    LCDD_COLOR_FORMAT_RGB_INVALID,

    LCDD_COLOR_FORMAT_RGB_565,

    LCDD_COLOR_FORMAT_QTY
} LCDD_COLOR_FORMAT_T;

typedef struct
{
    UINT16*             buffer;
    UINT16              width;
    UINT16              height;
    LCDD_COLOR_FORMAT_T colorFormat;
} LCDD_FB_T;

typedef struct
{
    UINT16 x;
    UINT16 y;
    UINT16 width;
    UINT16 height;
} LCDD_ROI_T;

typedef struct
{
    LCDD_FB_T   fb;
    LCDD_ROI_T  roi;
} LCDD_FBW_T;
typedef enum
{
    LCDD_ERR_NO=0,
    LCDD_ERR_DEVICE_NOT_FOUND,
    LCDD_ERR_NOT_OPENED,
    LCDD_ERR_INVALID_PARAMETER,
    LCDD_ERR_RESOURCE_BUSY
} LCDD_ERR_T;



PUBLIC LCDD_ERR_T  lcdd_LayerMerge(CONST LCDD_FBW_T* frameBufferWin, UINT16 startX, UINT16 startY,BOOL merge_state)
{
    frameBufferWin = frameBufferWin;
    startX = startX;
    startY = startY;
    merge_state = merge_state;
    return LCDD_ERR_NO;
}

#endif // (CHIP_HAS_GOUDA == 1)
typedef VOID (*MCI_LCD_BPFUN_T)(UINT16* buffer, UINT16 startx,UINT16 starty,UINT16 endx,UINT16 endy);
VOID camerap_PAS_Enter_LowPower(BOOL tri)
{
    tri = tri;
}
#ifndef simulator_Merge_buffer
U16*	simulator_Merge_buffer;
#endif
#ifdef __PRJ_WITH_SPICAM__
VOID check_sensorID()
{
}
#endif

#ifndef _FILETIME
typedef struct _FILETIME {
    UINT32 DateTime;
} TM_FILETIME, *TM_PFILETIME;
#endif

UINT32 CFW_CfgSetTimeParam(TM_FILETIME*  pTM)
{
    pTM = pTM;
    return 0;
}
PUBLIC VOID aud_LcdPowerOffCallback(void) {
    return 0;
}

INT32 FACTORY_ReadField(UINT32 offset,UINT32 size,UINT8* buff)
{
    offset = offset;
    size = size;
    buff = buff;
    return 0;
}

INT32 FACTORY_WriteField(UINT32 offset,UINT32 size,UINT8* buff)
{
    offset = offset;
    size = size;
    buff = buff;
    return 0;
}


