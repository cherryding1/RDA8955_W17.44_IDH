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

#include "boot_usb_monitor.h"

#include "hal_sys.h"
#include "hal_usb.h"
#include "hal_mem_map.h"
#ifdef CHIP_HAS_AP
#include "hal_ap_comm.h"
#endif


// The purpose of the following is to avoid the inclusion of big symbols in
// the final flash programmer lodfile.



// This bcpu_main replaces the regular bcpu_main present in the platform library
PROTECTED VOID bcpu_main()
{
}


//================================================
// EDRV stuff
//================================================

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


//================================================
// Calib stuff
//================================================

int calib_StubMain(int a, char*v[]) {
    return 0;
}

PUBLIC VOID calib_DaemonInit(VOID) {}

PUBLIC VOID* calib_GetPointers(VOID)
{
    return NULL;
}


//================================================
// SX stuff
//================================================

VOID sxr_Tampax (UINT32 Period, UINT8 I2d) {}

VOID sxs_BackTrace (UINT32 Ra, UINT32 *Sp)
{}
VOID sxs_SaveRmtBuffer (VOID) {}

UINT32 sxs_Boot;

VOID sxs_Raise (UINT32 Id, CONST CHAR *Fmt, ...) {}

#include <stdarg.h>
VOID sxs_vprintf (UINT32 Id, CONST CHAR *Fmt, va_list argPtr) {}

PUBLIC VOID _sxr_StartFunctionTimer(VOID) {}
PUBLIC VOID _sxr_StopFunctionTimer(VOID) {}

VOID sxr_Sleep (UINT32 Period) {}


//================================================
// HAL stuff
//================================================

//INT32 g_halLpsCtx;



VOID HAL_FUNC_INTERNAL _sxr_Irq_handler(VOID)
{
}

VOID HAL_FUNC_INTERNAL hal_IrqDispatch(VOID)
{
}


/*
#include "hal_pwm.h"

PUBLIC VOID hal_PwlGlow(UINT8 levelMin, UINT8 levelMax,
                       UINT8 pulsePeriod, BOOL pulse) {}
PUBLIC VOID hal_PwlSelLevel(HAL_PWL_ID_T id, UINT8 level) {}
PROTECTED VOID hal_PwlSetGlowing(HAL_PWL_ID_T id) {}

#include "hal_ana_gpadc.h"

PUBLIC VOID hal_AnaGpadcOpen(HAL_ANA_GPADC_CHAN_T channel, HAL_ANA_GPADC_ATP_T atp)
{
}

PUBLIC VOID hal_AnaGpadcClose(HAL_ANA_GPADC_CHAN_T channel)
{
}

PUBLIC HAL_ANA_GPADC_MV_T hal_AnaGpadcGet(HAL_ANA_GPADC_CHAN_T channel)
{
    return 1500;
}

PUBLIC UINT32 * hal_AnaGpadcGetCalibHigh(VOID)
{
    return &g_halSysLpsLock;
}

PUBLIC UINT32 * hal_AnaGpadcGetCalibLow(VOID)
{
    return &g_halSysLpsLock;
}

PUBLIC UINT16 hal_AnaGpadcGetTscX(VOID)
{
    return 0;
}

PUBLIC UINT16 hal_AnaGpadcGetTscY(VOID)
{
    return 0;
}

PROTECTED VOID hal_GdbBreaks(UINT32 cpuid)
{
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
{
}

PROTECTED VOID hal_GdbIrqDebug(UINT32 cpuid, UINT32 cause)
{
}

PUBLIC VOID hal_GdbFlushCacheData(VOID)
{
}

PROTECTED BOOL hal_GdbIsInGdbLoop(VOID)
{
    return FALSE;
}
*/

#if (CHIP_HAS_USB == 1)
// USB need a lot of stuff, so stub what can be stubbed
PUBLIC HAL_USB_EP_DESCRIPTOR_T*
hal_UsbDescriptorNewCmdEpDescriptor(UINT8 ep, HAL_USB_CALLBACK_T callback) {
    return 0;
}

// The HAL lib compiled for a chip that defines HAL_USB_DRIVER_DONT_USE_ROMED_CODE
// call         hal_HostUsbRecv(); and         hal_HostUsbSend();
PUBLIC BOOL hal_HostUsbSend(VOID)
{
    return boot_HostUsbRecv();
}

PUBLIC UINT8 hal_HostUsbRecv(VOID)
{
    return boot_HostUsbSend();
}

PUBLIC HAL_USB_EP_DESCRIPTOR_T*
hal_UsbDescriptorNewBulkEpDescriptor(UINT8 ep, HAL_USB_CALLBACK_T callback)
{
    return NULL;
}

PUBLIC HAL_USB_EP_DESCRIPTOR_T*
hal_UsbDescriptorNewInterruptEpDescriptor(UINT8 ep, UINT8 interval, HAL_USB_CALLBACK_T callback)
{
    return NULL;
}

PUBLIC HAL_USB_EP_DESCRIPTOR_T*
hal_UsbDescriptorNewIsoEpDescriptor(UINT8 ep, HAL_USB_CALLBACK_T callback)
{
    return NULL;
}

PUBLIC HAL_USB_INTERFACE_DESCRIPTOR_T*
hal_UsbDescriptorNewInterfaceDescriptor(
    UINT8 usbClass,
    UINT8 usbSubClass,
    UINT8 usbProto,
    UINT8 interfaceIdx,
    HAL_USB_EP_DESCRIPTOR_T** epList,
    UINT8* description,
    HAL_USB_CALLBACK_T callback)
{
    return NULL;
}

PUBLIC VOID hal_UsbDescriptorCleanInterfaceListDescriptor(HAL_USB_INTERFACE_DESCRIPTOR_T** interfaces)
{
}

PUBLIC VOID hal_UsbDescriptorCleanDeviceDescriptor(HAL_USB_DEVICE_DESCRIPTOR_T* dev)
{
}

PUBLIC HAL_USB_INTERFACE_DESCRIPTOR_T*
hal_UsbDescriptorCopyInterfaceDescriptor(CONST HAL_USB_INTERFACE_DESCRIPTOR_T* interface)
{
    return NULL;
}

PUBLIC VOID hal_UsbReset(BOOL reset)
{
}

PUBLIC VOID hal_UsbOpen(HAL_USB_GETDESCRIPTOR_CALLBACK_T callback)
{
}

PUBLIC HAL_USB_DEVICE_DESCRIPTOR_T* hal_HostUsbDeviceCallback(BOOL speedEnum)
{
    return NULL;
}

PROTECTED VOID hal_UsbHandleIrqInGdb(VOID)
{
}

PUBLIC HAL_USB_INTERFACE_DESCRIPTOR_T* hal_HostUsbOpen(VOID)
{
    return NULL;
}

#endif // (CHIP_HAS_USB == 1)


#include "hal_debug.h"
//PUBLIC VOID hal_DbgPortSetup(HAL_DBG_PORT_SEL_T mode){}
//PUBLIC VOID hal_DbgPxtsSetup(UINT16 mask){}

/*
PUBLIC VOID hal_DbgPageProtectSetup(
        HAL_DBG_PAGE_NUM_T pageNum, HAL_DBG_PAGE_SPY_MODE_T mode,
        UINT32 startAddr, UINT32 endAddr){}

PUBLIC VOID hal_DbgPageProtectEnable(HAL_DBG_PAGE_NUM_T pageNum){}


//PUBLIC VOID hal_DbgFatalTrigger(UINT8 trigId){}
PUBLIC VOID hal_DbgPxtsSendTrigger(HAL_DBG_PXTS_LEVEL_T level) {}

PUBLIC VOID hal_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_LEVEL_T level,
        UINT16 functionId) {}
PUBLIC VOID hal_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_LEVEL_T level,
        UINT16 functionId) {}
PUBLIC VOID hal_DbgPxtsProfileWindowEnter(HAL_DBG_PXTS_LEVEL_T level,
        UINT16 windowId) {}
PUBLIC VOID hal_DbgPxtsProfileWindowExit(HAL_DBG_PXTS_LEVEL_T level,
        UINT16 windowId) {}
PUBLIC VOID hal_DbgPxtsProfileTask(HAL_DBG_PXTS_LEVEL_T level,
                                   UINT16 taskId) {}
PUBLIC VOID hal_DbgPxtsProfilePulse(HAL_DBG_PXTS_LEVEL_T level,
                                    UINT16 code) {}
PUBLIC VOID hal_DbgPxtsProfileJobEnter(HAL_DBG_PXTS_LEVEL_T level,
                                       UINT16 jobId) {}
PUBLIC VOID hal_DbgPxtsProfileJobExit(HAL_DBG_PXTS_LEVEL_T level,
                                      UINT16 jobId) {}

PUBLIC VOID hal_DbgAssert(CONST CHAR* format, ...) {}
//PUBLIC VOID hal_DbgTrace(UINT32 level, UINT32 tsmap, CONST CHAR *fmt, ...){}


PUBLIC VOID hal_DbgPxtsSendCode(HAL_DBG_PXTS_LEVEL_T level, UINT16 code) {}
PROTECTED VOID hal_DbgPxtsOpen(VOID) {}
*/

/*
PUBLIC VOID hal_AudOpen(VOID) {}
PUBLIC VOID hal_AudClose(VOID) {}
PUBLIC VOID hal_AudForcePowerDown(VOID) {}
#if (CHIP_ASIC_ID != CHIP_ASIC_ID_GALLITE && \
     CHIP_ASIC_ID != CHIP_ASIC_ID_8808) // 8809 or later
PUBLIC VOID hal_AudForceDcdc4ClockOn(BOOL on) {}
#endif
*/

#if (CHIP_ASIC_ID != CHIP_ASIC_ID_8808 || !defined(FASTPF_USE_USB))
// XCV chip ID is needed for 8808 USB control.
VOID rfd_XcvReadChipId(VOID) {}
UINT32 rfd_XcvGetChipId(VOID) {
    return 0;
}
#endif


//PUBLIC VOID hal_GoudaInitResetPin(VOID) {}
//PUBLIC UINT32 hal_GoudaGetTiming(VOID) { return 0; }
//PUBLIC VOID hal_GoudaSetTiming(UINT32 gouda_timing) {}





//PUBLIC VOID hal_CameraOpen(VOID * camConfig) {}
//PUBLIC VOID hal_CameraClose(VOID) {}


// Watched through hal_access in the map engine,
// avoid linking with hal_speech.o
PROTECTED UINT32 g_halSpeechFifo = 0;
VOID hal_SpeechFifoReset(VOID) {}




// GPIO
// TODO:
// Is it safe to bypass all the GPIO operations in flash programmer (especially
// in PMD) ?
PUBLIC VOID hal_GpioSetOut(VOID) {}
PUBLIC VOID hal_GpioClr(VOID) {};
PUBLIC VOID hal_GpioSet(VOID) {};

PUBLIC VOID hal_GpioOpen(VOID) {}
PUBLIC VOID hal_GpioGet(VOID) {}
PUBLIC VOID hal_GpioResetIrq(VOID) {}
PUBLIC VOID hal_GpioIrqSetHandler(VOID) {}
PUBLIC VOID hal_GpioIrqSetMask(VOID) {}


//================================================
// MISC stuff
//================================================

PUBLIC VOID patch_Open(VOID) {}
PUBLIC VOID pal_Open(VOID) {}


PUBLIC VOID hal_RegisterYourself(VOID) {}
PUBLIC VOID calib_RegisterYourself(VOID) {}
PUBLIC VOID sx_RegisterYourself(VOID) {}
PUBLIC VOID std_RegisterYourself(VOID) {}
PUBLIC VOID rfd_RegisterYourself(VOID) {}
PUBLIC VOID memd_RegisterYourself(VOID) {}
//PUBLIC VOID pmd_RegisterYourself(VOID) {}

PUBLIC VOID bcpu_RegisterYourself(VOID) {}

//20130926 Added By Yangbiao
//PUBLIC VOID hal_AudForceDcdc4ClockOn(BOOL on){}
