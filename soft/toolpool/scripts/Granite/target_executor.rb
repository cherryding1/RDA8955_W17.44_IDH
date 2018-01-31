#!/usr/bin/env ruby

require "ChipStd/target_executor.rb"

module TargetExecutor

public

    class TargetXtor
        
        #Overload the ChipStd enterHostMonitor
        def enterHostMonitor()
    
            # we want to see events
            enableEvents
            flushFifo
            
            #Reset the target and freeze
            crestart(@connection, true, true) 
            
            # write enter monitor command
            $INT_REG_DBG_HOST.H2P_STATUS.write(@connection, BOOT_HST_MONITOR_START_CMD)
    
            # let the target go
            cunfreeze(@connection)
    
            #Ack the event
            if(@connection.waitEvent(BOOT_HST_MONITOR_START_EVT,2))
                puts "Target in host monitor mode"
                disableEvents
                flushFifo            
                return true        
            else
                # Proper event not received
                disableEvents
                flushFifo
                closeConnection
                raise(EnterHostError,"Enter Host Monitor failed!")
                return false
            end
        end
        
    end


end
