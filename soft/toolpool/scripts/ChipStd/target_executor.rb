#!/usr/bin/env ruby
include CoolHost

#require "error.rb"
#require "breakpoint.rb"
#require "cpu_debug.rb"

require "help.rb"
require "timeout.rb"

include Timeout

# Scripts to ease the access to the host executor  capability to 
# execute code on the target 
# through the exchange structures. Much improvment to be awaited
# with the internal register access !

module TargetExecutor

    # Defines the command to the target.
    BOOT_HST_MONITOR_START_CMD  = 0xfd
    BOOT_HST_MONITOR_END_CMD    = 0xfe
    HST_EXECUTOR_X_CMD          = 0xff
    HST_EXECUTOR_X_IRQ_CMD      = 0xef

    BOOT_HST_MONITOR_ENTER      = 0xee

    # Defines the status announced by the target
    BOOT_HST_STATUS_NONE                = 0x00
    BOOT_HST_STATUS_WAITING_FOR_COMMAND = 0xaa

    # HST_EXECUTOR_X_I_CMD    = 0x1ff # Used in the cmd_type field of the execution
                                    # context. For immediate execution

    # Defines the event received from the target.
    BOOT_HST_MONITOR_START_EVT       = 0xff000001
    BOOT_HST_MONITOR_END_EVT         = 0xff000003
    BOOT_HST_UNSUPPORTED_CMD_EVT     = 0xff000002
    BOOT_HST_START_CMD_TREATMENT_EVT = 0xff000004
    BOOT_HST_END_CMD_TREATMENT_EVT   = 0xff000005

    # Event signalling we entered the boot sector, whose code patches
    # the monitor.
    BOOT_HST_ENTER_BOOT_SECTOR       = 0x00000056

    class EnterHostError<Exception

    end
    
    class TargetExecuteException<Exception

    end
    
    # TODO Think all this exception stuff cause, well, that NEED
    # to be properly done. Like use the message capability 
    # to be meaning full http://www.rubycentral.com/book/ref_c_exception.html
    # Ben : Yeah, yeah, I've done a small pass on that stuff, it's a been a bit cleaned.
    # Better than nothing...

    

    class TargetXtor
        
        private
        
        @connection = nil
        
    	#####################################
        #	       CHST methods     	    #
    	#####################################
    	
    	public
 
        # Reopens the connection (might be useful one day?)
        def reopenConnection
            @connection=@connection.reopen()
        end
    
        # Enable host events received on @connection.
        def enableEvents
            @connection.enableEvents(true)
        end
        
        # Disable host events received on @connection.
        def disableEvents
    	    @connection.enableEvents(false)    
        end
    	
    	# Flush the @connection events in its FIFO.
    	def flushFifo
    	    @connection.flushEvents()	
    	end
	
    	# Check if an event is in the FIFO until timeout is reached. 
    	# If not, raise a NoEventError.
    	def popEvent(timeout)
    	    @connection.popEvent()
    	end
	
    	# Close the opened connection.
    	def closeConnection
          begin
            @connection.close
          rescue Exception
          end
    	end

        private

        addHelpEntry("targetxtor", "TargetXtor.targetExecuteGeneric",
                     "cmd, field_0, field_1, field_2, field_3,
                     timeOutStart(5), timeOutEnd(5), terminate(true), immediate(false)",
                     "0, (exectutionReturnedValue)",
                "[This function is Private]. \n
                Send the [cmd] command to the host monitor. If this command is 
                not supported by the standard handler, it will be passed to the
                extended command executor on the target, if one has previously
                been defined by TargetXtor.setExtdHandler, or ignored otherwise.\n
                The four fields [field_0] ... [field_3] usage depend
                on the extended monitor used, if an extended monitor is used. \n
                Otherwise, they are respectively interpreted by the standard 
                handler as PC, SP, The Parameter and pointer to the returned value. \n
                [timeOutStart] defines the time out before 
                which the target should inform that it has started the execution
                of the specified command. \n
                If the terminate parameter is true, we expected the executed function
                will end within a finished time, and [timeOutEnd] is the time after 
                the execution has started
                during which we wait for the \"end of execution\"
                acknowledgement from the target. \n
                If the terminate parameter is false, we don't wait for the end of
                execution event, as it will never come.
                [immediate] is used to select if the execution of the command
                should interrupt the current processing on the target [true]
                or wait for an oportunity to execute on idle or during a monitor
                [false, default value]. \n
                The status of the exection is returned. \n",
                false)
        def targetExecuteGeneric(cmd, field_0, field_1, field_2, field_3,
                            timeOutStart=5, timeOutEnd=5, terminate=true, immediate=false)
            # we want to see events
            enableEvents
    
            # Write the execution context in target internal memory
            targetExecConfig(cmd, field_0, field_1, field_2, field_3)
    
            # Unleash the execution
            if (immediate)
                # Write the Execution command in the H2P register. (On IRQ command)
                $INT_REG_DBG_HOST.H2P_STATUS.write(@connection,HST_EXECUTOR_X_IRQ_CMD)
                # Trigs the host interrupt.
                $INT_REG_DBG_HOST.CTRL_SET.IT_XCPU.set!(@connection)
            else
                # Write the Execution command in the H2P register
                $INT_REG_DBG_HOST.H2P_STATUS.write(@connection,HST_EXECUTOR_X_CMD)
            end
    
            # Wait for the ack event  
            if(@connection.waitEvent(BOOT_HST_START_CMD_TREATMENT_EVT,timeOutStart))
                puts "Execution started."
            else
                # Finished
                disableEvents
                # Proper event not received
                raise TargetExecuteException
            end
    
            # Wait for treatment done, if needed
            if (terminate)
                if(@connection.waitEvent(BOOT_HST_END_CMD_TREATMENT_EVT,timeOutEnd))
                    puts "Execution done on target!"
                else
                    # Finished
                    disableEvents
                    # Proper event not received
                    raise TargetExecuteException
                end
            end
            
			# Wait usb host reconnect
            if(@connection.implementsUsb)
				sleep 1
                @connection.waitCoolhostConnected()
				sleep 0.2
			end	
            # Return status and function resulta
            # TODO define a no exception exception
            return 0, $host_monitor_control.execution_context.returned_value.read(@connection)
        end

        public
        
        class ExitHostError<Exception
    
        end        

        addHelpEntry("targetxtor","TargetXtor.new","connectionToCopy=$CURRENTCONNECTION","","Creates a TargetXtor object that will perform its operations on a dedicated connection which is a copy of connectionToCopy (usually $CURRENTCONNECTION.")
        def initialize(connectionToCopy = $CURRENTCONNECTION)
            @connection = connectionToCopy.copyConnection()
            @connection.open(false)
        end        
    
        addHelpEntry("targetxtor", "TargetXtor.enterHostMonitor", "","bool","Put the target in the\
            host monitor mode. That implies a reset of the target, which is \
            put in a state waiting for host command. \n \
            The function returns TRUE if the monitor could be opened. \
            FALSE otherwise" )
        def enterHostMonitor()
            raise "The enterHostMonitor function needs to be overloaded and implemented for this chip!"
        end


        addHelpEntry("targetxtor", "TargetXtor.exitHostMonitor", "","","When the target is in the \
            host monitor mode, exit and carry on normal execution" )
        def exitHostMonitor()
            # we want to see events
            enableEvents
    
            # Write the end monitor command
            $INT_REG_DBG_HOST.H2P_STATUS.write(@connection,BOOT_HST_MONITOR_END_CMD)
    
            # Wait for the ack event
            if(@connection.waitEvent(BOOT_HST_MONITOR_END_EVT,1))
                puts "Target out of host monitor mode. Normal execution resumed"
                disableEvents
                flushFifo
                return true
            else
                # Finished
                disableEvents
                # Proper event not received
                raise(ExitHostError,"Exit Host Monitor failed!")
            end    
        end
    
    
        # Internal function to set the execution structure
        def targetExecConfig(cmdType, pc, sp, param, returnedValue)
            
                $host_monitor_control.execution_context.pc.write(@connection,pc)
                $host_monitor_control.execution_context.sp.write(@connection,sp)
                $host_monitor_control.execution_context.param.write(@connection,param)
                $host_monitor_control.execution_context.returned_value.write(@connection,returnedValue)
                $host_monitor_control.execution_context.cmd_type.write(@connection,cmdType)
    
                # Ensure that the write is effective
                # Ben : WTF ????? Not done? TODO ?
                # Read block?
        end


    # TODO add a function to set the extended callback

        addHelpEntry("targetxtor",
                    "TargetXtor.targetExecute",
                    "pc, sp, param, timeOutStart(5), timeOutEnd(5)",
                    "0, (exectutionReturnedValue)",
                    "Execute when possible on the target the code at [pc] with the stack pointer set at
                    [sp]. If [sp]=0, the current stack is used. One [parameter] is passed 
                    to the called function. \n
                    [timeOutStart] defines the time out before 
                    which the target should inform that it has started the execution
                    of the specified code. [timeOutEnd] is the time after that event
                    is received during we waits for the \"end of execution\"
                    acknowledgement from the target. \n
                    Those timeout parameters are optionals.
                    The status of the exection as well as the value returned
                    by the function call are returned. \n
                    This function is for
                    a code execution at the will of the target. That is immediately
                    in the host monitor or on idle time if the target is 
                    normally running.")
        def targetExecute(pc, sp, param, timeOutStart=5, timeOutEnd=5)
            targetExecuteGeneric(HST_EXECUTOR_X_CMD, pc, sp, param, 0,
                            timeOutStart, timeOutEnd)
        end


        addHelpEntry("targetxtor",
                    "TargetXtor.targetExecuteAndJump",
                    "pc, sp, param, timeOutStart(5)",
                    "0",
                    "Execute when possible on the target the code at [pc] with the stack pointer set at
                    [sp]. If [sp]=0, the current stack is used. One [parameter] is passed 
                    to the called function. This code is expected never to return. This is 
                    useful to start a 'Ramrun' or launching the calibration stub.\n
                    [timeOutStart] defines the time out before 
                    which the target should inform that it has started the execution
                    of the specified code.
                    This function is for a code execution at the will of the target.
                    That is immediately
                    in the host monitor or on idle time if the target is 
                    normally running.")
        def targetExecuteAndJump(pc, sp, param, timeOutStart=5)
            targetExecuteGeneric(HST_EXECUTOR_X_CMD, pc, sp, param, 0,
                            timeOutStart, 0, false)
        end

        addHelpEntry("targetxtor",
                "TargetXtor.targetExecuteImmediate",
                "pc, sp, param, timeOutStart(5), timeOutEnd(5)",
                "0, (exectutionReturnedValue)",
                "Execute immediatlely on the target the code at [pc] with the stack pointer set at
                [sp]. If [sp]=0, the current stack is used. One [parameter] is passed 
                to the called function. \n
                [timeOutStart] defines the time out before 
                which the target should inform that it has started the execution
                of the specified code. [timeOutEnd] is the time after that event
                is received during we waits for the \"end of execution\"
                acknowledgement from the target. \n
                The status of the exection as well as the value returned
                by the function call are returned. \n
                This function is for executing code immediately.")
    
        def targetExecuteImmediate(pc, sp, param, timeOutStart=5, timeOutEnd=5)
            targetExecuteGeneric(HST_EXECUTOR_X_CMD, pc, sp, param, 0,
                            timeOutStart, timeOutEnd, true, true)
        end
    
        addHelpEntry("targetxtor",
                    "TargetXtor.setExtdHandler",
                    "pc, (exectutionReturnedValue)",
                    "",
                    "Set the field in memory where the target 
                    expects to find the PC for the extended remote
                    execution handler.")
        def setExtdHandler(pc)
            $host_monitor_control.extended_handler.extended_command_handler.write(@connection,pc)
        end

        addHelpEntry("targetxtor",
                "TargetXtor.targetExecuteExtd",
                "cmd, field_0, field_1, field_2, field_3, timeOutStart(5),
                 timeOutEnd(5), terminate(true), immediate(false)",
                "0, (exectutionReturnedValue)",
                "Send the [cmd] command to the extended command executor on the 
                target. The four fields [field_0] ... [field_3] usage depend
                on the extended monitor used. \n
                [timeOutStart] defines the time ut before 
                which the target should inform that it has started the execution
                of the specified command. [timeOutEnd] is the time after that event
                is received during which we wait for the \"end of execution\"
                acknowledgement from the target. \n
                [immediate] is used to select if the execution of the command
                should interrupt the current processing on the target [true]
                or wait for an oportunity to execute on idle or during a monitor
                [false, default value]. \n
                The status of the exection is returned. \n")
    
        def targetExecuteExtd(cmd, field_0, field_1, field_2, field_3,
                                             timeOutStart=5, timeOutEnd=5, terminate=true, immediate=false)
            # mmm ...
            targetExecuteGeneric(cmd, field_0, field_1, field_2, field_3,
                            timeOutStart, timeOutEnd, terminate, immediate)
        end

        addHelpEntry("targetxtor",
                    "TargetXtor.session",
                    "connection=$CURRENTCONNECTION",
                    "",
                    "{|session| ... } Class method. Opens a session on the target executor, closes it at the end. |session| is the opened TargetXtor (yielded to perform things on the target executor in the block of code). Exemple: <i>TargetXtor.session{ |s| s.exitHostMonitor() }</i> will enter the host monitor, and immediately exit it. ")
        def TargetXtor.session(connection=$CURRENTCONNECTION)
            s = TargetXtor.new(connection)
            begin
                s.enterHostMonitor()
                yield(s)
            ensure
                s.closeConnection
            end
        end
    
    #end of class TargetXtor
    end

end


include TargetExecutor

