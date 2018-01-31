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
        #TODO Use an 'ensure' to do disableEvents,flushFifo
        def enterHostMonitor()
            # we want to see events
            enableEvents
            flushFifo
    
            # TODO This test is deprecated by the next one. Remove.
            if(@connection.implementsUsb)
                monstate = $boot_usb_monitor.>(@connection).Context.>(@connection).MonitorMode.read(@connection)
                if(monstate == @@UsbGlobals::BOOT_HOST_USB_MODE_BOOT)
                    #Already in the monitor!
                    disableEvents
                    flushFifo                
                    return true
                end 
            end
            
            # Check we're already in the monitor.
            if (BOOT_HST_STATUS_WAITING_FOR_COMMAND == $DEBUG_HOST.p2h_status.p2h_status.read(@connection))
                #Already in the monitor!
                disableEvents
                flushFifo                
                return true
            end

            #Write the Boot_Mode in Sys_CTRL Registers
            #(Read Modify Write in the Reset_Cause register)
            #Get the value to write in the register without tainting the reset cause
            #All the bitfields will be set to 0 except the Boot_Mode
            ############## puts "#TODO : GET THE BOOT MODE CONSTANT FROM THE XMD"
            previousBootMode = $SYS_CTRL.Reset_Cause.Boot_Mode.read(@connection)
         
            if(@connection.implementsUsb)
                # Enter in boot sector
                $INT_REG_DBG_HOST.H2P_STATUS.write(@connection, BOOT_HST_MONITOR_ENTER)
                # Flush CoolHost Fifos, as broken traces may have been in the process of being
                # received when we reset.
                @connection.flushCoolhostFifos()
                sleep 1
				@connection.waitCoolhostConnected()
				sleep 0.2
            else
                #Check if we have the Host Uart Clock
                if($DEBUG_HOST.mode.Clk_Host_On.read(@connection)==1)
                    # Force monitor, disabling the USB.
                    usbdisable = (1<<3)
                    val = $SYS_CTRL.Reset_Cause.Boot_Mode.wl(0, previousBootMode | (1<<1) | usbdisable)
                    #puts "Writing 0x%08X in $SYS_CTRL.Reset_Cause" % val
                    $SYS_CTRL.Reset_Cause.write(@connection, val, true) # Write forced
                
                    #Reset the target
                    crestart(@connection)
            
                    # Sometimes we need to unfreeze the phone's CPU (for example, when the code
                    # loaded into a phone crashes early after the boot).
                    cunfreeze(@connection)
                else
                    # Disable the USB, but don't force monitor to first go through the 
                    # boot sector and enable the clock from inside ?
                    # No need since the clock is on (We used the boot sector)
                    usbdisable = 0 #(1<<3)
                    val = $SYS_CTRL.Reset_Cause.Boot_Mode.wl(0, previousBootMode | usbdisable)
                    #puts "Writing 0x%08X in $SYS_CTRL.Reset_Cause" % val
                    $SYS_CTRL.Reset_Cause.write(@connection, val, true) # Write forced
                
                    # Go through the boot sector
                    $INT_REG_DBG_HOST.H2P_STATUS.write(@connection, BOOT_HST_MONITOR_ENTER)
                    # Need to go to idle task to get the 'enter monitor through boot sector'
                    # command, then about half a second (0.2) to get the clock on, rounding up.
                    
                    # Ack the enter the boot sector.
                    if (@connection.waitEvent(BOOT_HST_ENTER_BOOT_SECTOR, 10))
                        puts "Entered boot sector."
                    else
                        # Put back the previous boot mode, without that, the chip would
                        # always enter in the boot monitor after a restart.
                        $SYS_CTRL.Reset_Cause.Boot_Mode.write(@connection, previousBootMode)
                        disableEvents
                        flushFifo

                        #TODO Think of forcing the execution of the boot sector by setting
                        #all needed variable and reseting (crestart(@connection), cunfreeze(@connection))

                        # Proper event not received
                        closeConnection
                        raise(EnterHostError,"Enter Host Monitor Boot Sector failed!")
                        return false
                    end 

                end
                # Flush CoolHost Fifos, as broken traces may have been in the process of being
                # received when we reset.
                @connection.flushCoolhostFifos()
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

