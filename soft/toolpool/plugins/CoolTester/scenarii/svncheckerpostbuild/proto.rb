# Available fields in the options are :
# mkopts     : make options (hash table)
# testname   : test name
# lodname    : lod file name (without dir)
# loddir     : lod directory (without lod filename)
# logprint   : proc passed by SVNChecker for printing stuff.
# postbuild  : proc to be called when postbuilding (this is actually what is called when arriving here).
# flashprogrammer : flash programmer file, passed as an argument

require 'CoolTester/tests/Proto_Auto_Call.rb'

include FastPf

module SVNCheckerPostBuild
    
    class SvnCheckerCallError < StandardError
    end
    
    class SvnCheckerFastpfError < StandardError
    end
    
    class SvnCheckerSynchroError < StandardError
    end

    def SVNCheckerPostBuild.protoCheck(options)
        # Ensure that we will have a functional coolhost  
        reop()
        
        # Copy the $CURRENTCONNECTION to be able to handle events in a clean way
        h = $CURRENTCONNECTION.copyConnection()
        h.open(false)
        
        logPrintProc    = options["logprint"]
        reportPrintProc = options["reportprint"]
        fp              = options["flashprogrammer"].to_s
        file            = options["loddir"].to_s + "/" + options["lodname"].to_s
        num             = options["num"]
        flamulate       = (options["flamulate"] == true )
        cfpfile         = options["cfpfile"]
        
        # Enable events
        h.enableEvents(true)
        sleep 1
        
        
        if(flamulate)
            logPrintProc.call "Flamulating file: %s" % file
            reportPrintProc.call "Flamulating file: %s" % file
            flamfiles = []
            flamfiles << file
            flamfiles << cfpfile if(cfpfile)
            flamulate(flamfiles, true, false)
        else
            logPrintProc.call "Fastpfing file: %s" % file
            reportPrintProc.call "Fastpfing file: %s" % file
            logPrintProc.call "With flash programmer: %s " % fp
            reportPrintProc.call "With flash programmer: %s " % fp
            
            # Do the fastpf
            fastpf(fp, file, FastPf::FULLFASTPF)
        end
            
        # Wait for the RESET event after fastpf
        if(!h.waitEvent(0x57, 15))
            reportPrintProc.call "Reset not received after fasptf!"
            raise SvnCheckerFastpfError, "Reset not received after fasptf!"
        else
            logPrintProc.call "The download of the proto has been achieved."
            reportPrintProc.call "The download of the proto has been achieved."
        end
       
        # Ok we've done fastpf, close the handle.
        h.close()
        
        # Waiting for GTES to be ready.
        sleep 3
        
        # Let's do some cooltesting. Reinit cooltester.
        CoolTester.init()
        
        # Waiting for the phone to be synchronized on the network.
        logPrintProc.call "Waiting for the phone to be synchronized on the network..."
        # TODO Use a class variable set by something as the timeout.
        success, apiMsgLog = CoolTester.callStatusIdle?(45)
        if (!success)
            reportPrintProc.call "Failed to synchronize with the network."
            raise SvnCheckerSynchroError, "Failed to synchronize with the network."
        end

        logPrintProc.call "Synchronized with the network."
        reportPrintProc.call "Synchronized with the network."

        # Do the call
        logPrintProc.call "Calling #{num.to_s}..."
        reportPrintProc.call "Calling #{num.to_s}..."
        myCall = Proto_Auto_Call.new({:timeout=>10, :start=>1, :number=>num })
        log, success = myCall.process
        
        # Log the call
        logPrintProc.call log

        # Raise exception if the call failed.
        if (!success)
            reportPrintProc.call "An error append during the call establishment."
            raise SvnCheckerCallError, "An error append during the call establishment."
        end
        
        # Wait a bit
        # TODO Get that time from somewhere
        reportPrintProc.call "Call in progress."
        logPrintProc.call "Call in progress."
        sleep(15)

        # Shut down the call
        logPrintProc.call "Releasing call..."
        myCall = Proto_Auto_Call.new({:timeout=>10, :start=>0, :number=>num })
        log,success = myCall.process
        
        # Log the call
        logPrintProc.call log

        # Raise exception if the call failed.
        if (!success)
            reportPrintProc.call "An error append during call release."
            raise SvnCheckerCallError, "An error append during call release."
        end

        logPrintProc.call "Call released successfully."
        reportPrintProc.call "Call released successfully."
        
        return true

    ensure
        # Kill coolhost. This will allow it to be relaunched for another test.
        # Since ce tool will run a long time without being humanly controlled,
        # Let's do some cleaning often.
        h.close()
		# $CURRENTCONNECTION.killCoolhost($CURRENTCONNECTION)
        cwClearWatches
    end
    
end

