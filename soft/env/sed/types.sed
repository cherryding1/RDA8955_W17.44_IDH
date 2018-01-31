{
# Use types 
s/u8/UINT8/g
s/u16/UINT16/g
s/u32/UINT32/g
s/s8/INT8/g
#damned mips16
/[mM]ips16/!s/s16/INT16/g
#damned mips32
/[mM]ips32/!s/s32/INT32/g
#
s/bool/BOOL/g
s/ascii/CHAR/g
s/void/VOID/g


# Structure and enum are now defined as types ...
s/enum hal_DebugBusMode/hal_DebugBusMode_t/g
s/enum hal_DebugBusIrqSel/hal_DebugBusIrqSel_t/g
s/t_hal_ErrorCtx/hal_ErrorCtx_t/g
s/enum hal_ErrorCode/hal_ErrorCode_t/g
s/t_hal_WatchdogCtx/hal_WatchdogCtx_t/g
s/enum hal_BkptType/hal_BkptType_t/g
s/enum hal_Pagenum/hal_Pagenum_t/g
s/t_hal_PageSpyMode/hal_PageSpyMode_t/g
s/enum hal_RecordType/hal_RecordType_t/g
s/enum hal_BranchType/hal_BranchType_t/g
s/enum hal_CpuSel/hal_CpuSel_t/g
s/enum hal_RomuUserId/hal_RomuUserId_t/g
s/t_hal_DmaError/hal_DmaError_t/g
s/t_hal_DmaStopTransfer/hal_DmaStopTransfer_t/g
s/enum hal_MemoryCS/hal_MemoryCS_t/g
s/struct hal_MemoryCfg/hal_MemoryCfg_t/g
s/struct hal_CSSettings/hal_CSSettings_t/g
s/enum hal_FlashErrors/hal_FlashErrors_t/g
s/t_hal_GpioAccess/hal_GpioAccess_t/g
s/t_hal_UsrVector/hal_UsrVector_t/g
s/struct hal_LpsStatus/hal_LpsStatus_t/g
s/t_hal_MMCCSD/hal_MmcCsd_t/g
s/enum hal_PaBand/hal_PaBand_t/g
s/struct hal_RfspiGain/hal_RfspiGain_t/g
s/enum hal_RfspiDelay/hal_RfspiDelay_t/g
s/struct hal_RfspiCfg/hal_RfspiCfg_t/g
s/struct hal_RfspiStatus/hal_RfspiStatus_t/g
s/enum hal_SimClockStopMode/hal_SimClockStopMode_t/g
s/enum hal_SimVoltage/hal_SimVoltage_t/g
s/enum hal_SimEtuDuration/hal_SimEtuDuration_t/g
s/e_hal_EventId/hal_EventId_t/g
s/enum hal_UartId/hal_UartId_t/g
s/enum hal_UartDataBits/hal_UartDataBits_t/g
s/enum hal_UartStopBitsQty/hal_UartStopBitsQty_t/g
s/enum hal_UartParityCfg/hal_UartParityCfg_t/g
s/enum hal_UartRxTriggerCfg/hal_UartRxTriggerCfg_t/g
s/enum hal_UartTxTriggerCfg/hal_UartTxTriggerCfg_t/g
s/enum hal_UartAfcMode/hal_UartAfcMode_t/g
s/enum hal_UartIrdaMode/hal_UartIrdaMode_t/g
s/enum hal_UartBaudRate/hal_UartBaudRate_t/g
s/enum hal_UartTransfertMode/hal_UartTransfertMode_t/g
s/struct hal_UartCfg/hal_UartCfg_t/g
s/t_hal_VifSelect/hal_VifSelect_t/g
s/t_hal_VifMicSel/hal_VifMicSel_t/g
s/t_hal_VifSpkSel/hal_VifSpkSel_t/g
s/t_hal_VifSpkPd/hal_VifSpkPd_t/g
s/t_hal_VifCfg/hal_VifCfg_t/g
#hal_audio
s/enum hal_CodecSel/hal_CodecSel_t/g
s/enum hal_AudioFrequency/hal_AudioFrequency_t/g
s/enum hal_CodecMode/hal_CodecMode_t/g
s/enum hal_CodecFrameSize/hal_CodecFrameSize_t/g
s/enum hal_SpeakerGain/hal_SpeakerGain_t/g
s/enum hal_SpeakerSel/hal_SpeakerSel_t/g
s/enum hal_MicGain/hal_MicGain_t/g
s/enum hal_MicSel/hal_MicSel_t/g
s/enum hal_SideToneGain/hal_SideToneGain_t/g
s/enum hal_ToneAttenuation/hal_ToneAttenuation_t/g
s/enum hal_ToneType/hal_ToneType_t/g
s/struct hal_AudioCfg/hal_AudioCfg_t/g
s/struct hal_AudioCalib/hal_AudioCalib_t/g

# Reorder t_hal_types as HAL_TYPES_T
# and capitalize in the process
s/t_hal_SysFreq/HAL_SYS_FREQ_T/g
s/t_hal_StopSimStatus/HAL_STOP_SIM_STATUS_T/g
s/t_hal_DriverId/HAL_DRIVER_ID_T/g
s/t_hal_IfcRequestId/HAL_IFC_REQUEST_ID_T/g
s/t_hal_SysFreq/HAL_SYS_FREQ_T/g
s/t_hal_CpuId/HAL_CPUA_ID_T/g
s/t_hal_BKPTType/HAL_BKPT_TYPE_T/g
s/t_hal_ClockDbgSel/HAL_CLOCK_DBG_SEL_T/g
s/t_hal_ClockId/HAL_CLOCK_ID_T/g
s/t_hal_ModuleIrqHandler/HAL_MODULE_IRQ_HANDLER_T/g
s/t_hal_InterruptId/HAL_INTERRUPT_ID_T/g
s/t_hal_IrqRegistry/HAL_IRQ_REGISTRY_T/g
s/t_hal_GdbData/HAL_GDB_DATA_T/g
s/t_hal_ResetId/HAL_RESET_ID_T/g
s/t_hal_SlowClock/HAL_SLOW_CLOK_T/g
s/t_hal_SysClock/HAL_SYS_CLOK_T/g
s/t_hal_BbresetId/HAL_BB_RESET_ID/g
s/t_hal_VifMicMute/HAL_VIF_MIC_MUTE_T/g
s/t_hal_PaProfile/HAL_PA_PROFILE_T/g
s/t_hal_FunctimerCallback/HAL_FUNCTIMER_CALLBACK_T/g
s/t_hal_IrqSetMask/HAL_IRQ_SET_MASK_T/g


# Capitalize types names (xxx_Xxx_t becomes XXX_XXX_T)
:carry_on
s/\(hal_[[:alnum:]_]*[a-zA-Z]\)\([A-Z][[:alnum:]_]*_t\)/\1_\2/g
t carry_on
}

{
:label
/hal_[[:alnum:]_]*_t/{
h
s/.*\(hal_[[:alnum:]_]*_t\).*/\1/
y/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/
G
s/\(.*\)\n\(.*\)hal_[[:alnum:]_]*_t\(.*\)/\2\1\3/
}
t label
}
