

def handleAssert()
	begin
        puts "html><font color=red>Assert received!! " + 
        "Reading detail, please wait a few seconds...</font>"
        #First event is pointing to the assert string, second one to the parameter of the string
        event1 = $EVENTSNIFFERCONNECTION.connection.popEvent(1.0) #no timeout
        # Here we need to wait for the traces to be flushed by GDB before trying to
        # read in the target (very likely because of a hardware bug in the host
        # which causes traces and host commands to be mixed).
        sleep 4
        str1 = event1.RS(128)
        puts "ASSERT DETAIL : \n%s" % str1
        
	rescue Exception => e
	    puts "Failed to read assert detail."
        CRExceptionPrint e
	end
end

def handleGdbEvent(chipStr)
    printres = true
    if(defined?(ElfMasterModule))
        if(ElfMasterModule.emGdbConnected?())
            printres = false
        end
    end
    
    if(printres)
        puts "html><font color=darkgreen>Detected %s in the GDB loop (0x9db00000), connect GDB debugger for more info.</font>" % chipStr
    else
        # puts "Zapping GDB event print..."
    end
end


addHelpEntry("chip", "chipLastResetTime", "", "", "Returns the last time at which the reset event has been detected.")

$lastResetTime = nil

def chipLastResetTime
    $lastResetTime
end



def chipResetEventCallBack
    puts "html><font color=darkgreen>Detected system reset (0x57).</font>"
    $lastResetTime = Time.now
end

