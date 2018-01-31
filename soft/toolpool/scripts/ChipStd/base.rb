require "rbbase/common/base.rb"

def chipRestart(connection, bool_xcpufreeze, bool_powerStayOn, bool_reEnableUart)
    
    evconnection = connection.copyConnection()
    evconnection.open(false)
    evconnection.enableEvents(true)
    
    # Redefining chipRestart
    val = $INT_REG_DBG_HOST.CTRL_SET.prepl(0)
    val = $INT_REG_DBG_HOST.CTRL_SET.Debug_Reset.wl(val,1)
    val = $INT_REG_DBG_HOST.CTRL_SET.XCPU_Force_Reset.wl(val,(bool_xcpufreeze)?1:0) 
    val = $INT_REG_DBG_HOST.CTRL_SET.Force_Wakeup.wl(val,(bool_powerStayOn)?1:0)
    
    $INT_REG_DBG_HOST.CTRL_SET.write(connection,val)
    
    if(!connection.implementsUsb())
        evconnection.waitEvent(0xFFFFFFFF,3)

        # Do a read modify write
        # (In case of com break, this get back the IFC access
        # and the trace.)
        val = $INT_REG_DBG_HOST.CFG.read(connection)
        val = $INT_REG_DBG_HOST.CFG.Disable_IFC_H.wl(val,0)
        val = $INT_REG_DBG_HOST.CFG.Disable_Uart_H.wl(val,(bool_reEnableUart)?0:1)
        val = $INT_REG_DBG_HOST.CFG.Force_Prio_H.wl(val,1)
        $INT_REG_DBG_HOST.CFG.write(connection,val)
    else
        # Wait the USB to be disconnected.
        sleep(0.5)

        # If the reset make us lose the connection to the device (just like for the USB).
        # Wait for Coolhost to be reconnected to device.
        connection.waitCoolhostConnected()
        # It seems that operations cannot be done on the USB right after the reset so sleep just a bit.
        sleep(0.1)
    end  
ensure    
    evconnection.close()
end

def chipUnfreeze(connection)
    $INT_REG_DBG_HOST.CTRL_CLR.XCPU_Force_Reset.clear!(connection)
end

def chipXfbp(connection)
    $INT_REG_DBG_HOST.CTRL_SET.Force_BP_XCPU.set!(connection)
end

def chipXrbp(connection)
    $INT_REG_DBG_HOST.CTRL_CLR.Force_BP_XCPU.clear!(connection)
    $SYS_CTRL.XCpu_Dbg_BKP.write(connection,0)
end

def chipXcbp(connection)
    val = $INT_REG_DBG_HOST.CTRL_SET.Force_BP_XCPU.read(connection)
    return (val==1)?true:false
end

def chipBfbp(connection)
    $INT_REG_DBG_HOST.CTRL_SET.Force_BP_BCPU.set!(connection)
end

def chipBrbp(connection)
    $INT_REG_DBG_HOST.CTRL_CLR.Force_BP_BCPU.clear!(connection)
    $SYS_CTRL.BCpu_Dbg_BKP.write(connection,0)
end

def chipBcbp(connection)
   val = $INT_REG_DBG_HOST.CTRL_SET.Force_BP_BCPU.read(connection)
   return (val==1)?true:false
end
