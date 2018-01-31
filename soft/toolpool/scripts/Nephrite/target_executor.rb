#!/usr/bin/env ruby

require "ChipStd/target_executor.rb"


module TargetExecutor

    begin
        @@UsbGlobals = CH__boot_usb_monitor_globals
    rescue
        puts "*** WARNING!! The XMD USB Global vars of the chip are not loaded and the target executor cannot work without them. Please reload the environment with the loading of the USB XMDs. ***"
    end

public
    class TargetXtor

        def enterHostMonitor()
            # we want to see events
            enableEvents
            flushFifo
    
            if(@connection.implementsUsb)
                monstate = $boot_usb_monitor.>(@connection).Context.>(@connection).MonitorMode.read(@connection)
                if(monstate == @@UsbGlobals::BOOT_HOST_USB_MODE_BOOT)
                    #Already in the monitor!
                    return true
                end 
            end
            
            #Write the Boot_Mode in Sys_CTRL Registers
            #(Read Modify Write in the Reset_Cause register)
            #Get the value to write in the register without tainting the reset cause
            #All the bitfields will be set to 0 except the Boot_Mode
            ############## puts "#TODO : GET THE BOOT MODE CONSTANT FROM THE XMD"
            previousBootMode = $SYS_CTRL.Reset_Cause.Boot_Mode.read(@connection)
            usbenable = (@connection.implementsUsb)?0:1
         
            if(@connection.implementsUsb)
              # Enter in boot sector
              $INT_REG_DBG_HOST.H2P_STATUS.write(@connection, BOOT_HST_MONITOR_ENTER);
              @connection.waitCoolhostConnected()
              sleep(0.2)
            else
              val = $SYS_CTRL.Reset_Cause.Boot_Mode.wl(0, previousBootMode | (1<<1) | (usbenable<<3))
              #puts "Writing 0x%08X in $SYS_CTRL.Reset_Cause" % val
              $SYS_CTRL.Reset_Cause.write(@connection, val,true) #write forced

              #Reset the target
              crestart(@connection)
            
              # Sometimes we need to unfreeze the phone's CPU (for example, when the code
              # loaded into a phone crashes early after the boot).
              cunfreeze(@connection)
            end
                
            # write enter monitor command
            $INT_REG_DBG_HOST.H2P_STATUS.write(@connection, BOOT_HST_MONITOR_START_CMD)
    
            #Ack the event, with a timeout
            if (@connection.waitEvent(BOOT_HST_MONITOR_START_EVT, 10))
                # Put back the previous boot mode, without that, the chip would
                # always enter in the boot monitor after a restart.
                $SYS_CTRL.Reset_Cause.Boot_Mode.write(@connection, previousBootMode)
                disableEvents
                flushFifo                

                puts "Entered Host Monitor mode."
                return true
            else
                # Put back the previous boot mode, without that, the chip would
                # always enter in the boot monitor after a restart.
                $SYS_CTRL.Reset_Cause.Boot_Mode.write(@connection, previousBootMode)
                disableEvents
                flushFifo                

                # Proper event not received
                closeConnection
                raise(EnterHostError,"Enter Host Monitor failed!")   
                return false
            end            
        end
    end
end


include TargetExecutor

