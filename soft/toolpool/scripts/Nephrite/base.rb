
module FlowMode
    FLOWMODE = CoolHost::FLOWMODE_XONXOFF
end

require "ChipStd/base.rb"


def enableFlowControlPins()
    val = $CONFIG_REGS.GPIO_Mode.R
    val = $CONFIG_REGS.GPIO_Mode.Mode_PIN_HST_CTS.wl(val,0)
    val = $CONFIG_REGS.GPIO_Mode.Mode_PIN_HST_RTS.wl(val,0)
    $CONFIG_REGS.GPIO_Mode.w val
    val = $DEBUG_UART.ctrl.R
    val = $DEBUG_UART.ctrl.SWRX_flow_ctrl.wl(val,0)
    val = $DEBUG_UART.ctrl.SWTX_flow_ctrl.wl(val,0)
    val = $DEBUG_UART.ctrl.BackSlash_En.wl(val,0)
    $DEBUG_UART.ctrl.w val
    puts "TODO : Disable XON/XOFF in CoolHost."
end

def disableFlowControlPins()
    val = $DEBUG_UART.ctrl.R
    val = $DEBUG_UART.ctrl.SWRX_flow_ctrl.wl(val,1)
    val = $DEBUG_UART.ctrl.SWTX_flow_ctrl.wl(val,1)
    val = $DEBUG_UART.ctrl.BackSlash_En.wl(val,1)
    $DEBUG_UART.ctrl.w val
    puts "TODO : ENABLE XON/XOFF in CoolHost."
end