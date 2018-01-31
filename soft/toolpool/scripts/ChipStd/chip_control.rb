


addHelpEntry("chip", "turnoff", "", "",
    "Turn off the phone (set the Wakeup line to low).")

def turnoff()
    # Disable auto-polling when using coolwatcher.
    # It seems that reading phone registers
    # when it is switched off is a bad idea...
    begin 
        cwDisableAutoPoll()
    rescue
    end
    $INT_REG_DBG_HOST.CTRL_SET.Force_Wakeup.set
    $SYS_CTRL.REG_DBG.w 0xa50001
    $SYS_CTRL.WakeUp.force_Wakeup.w 0
    $INT_REG_DBG_HOST.CTRL_CLR.Force_Wakeup.clear
end

