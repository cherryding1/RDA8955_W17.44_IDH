require "UnitTest"
require 'gpib.rb'
require 'cmu.rb'
require "Proto_Auto_Call"

class McallUserOptions
        attr_accessor :band, :level, :arfcn, :speech_mode, :nb_calls, :dial_number, :call_duration, :call_mode
end

class Proto_Auto_Multi_Call < UnitTest
    
    #####################################
	#		DEFAULT PARAMETERS		    #
	#####################################
    
    @@DEFAULT_DIAL_NUMBER= "666"
    @@DEFAULT_BAND = 1
    @@DEFAULT_LEVEL = 85
    @@DEFAULT_ARFCN = 32
    @@DEFAULT_SPEECH_MODE = 1
    @@DEFAULT_NB_CALLS= 10
    @@DEFAULT_CALL_DURATION= 10
    @@DEFAULT_CALL_MODE= 0
    
    #####################################
	#		TEST DESCRIPTION			#
	#####################################
    
    @@TEST_NAME                 = "Proto Auto Multi Call Test"
    @@VERSION                   = "1.0"
    @@ENVIRONEMENT              = "PROTO"
    @@TYPE                      = "AUTO"
    @@PREREQUISITE              = "Connect the CMU200 + a SIM card"
    @@DESCRIPTION               = "Performs N :nb_calls calls with the given call number :number. For each call the test records messages from API task to MMI task and updates a state machine with connected / disconnected state. The test compares this state with the expected one and gives a result. If one call fails the test fails."
    
    @@PARAMETERS=[
        ["band","#{@@DEFAULT_BAND}","Band mode : 1 is GSM, 2 is DCS and 3 is PCS"],
        ["level","#{@@DEFAULT_LEVEL}","dB alleviation"],
        ["arfcn","#{@@DEFAULT_ARFCN}","Arfcn number"],
        ["speech_mode","#{@@DEFAULT_SPEECH_MODE}","Speech mode : 0 is Full Speech Rate, 1 is Enhanced Full Speech Rate and 2 is Half Rate Speech."],
        ["nb_calls","#{@@DEFAULT_NB_CALLS}","Number of calls to be performed."],
        ["dial_number","#{@@DEFAULT_DIAL_NUMBER}","Called phone number"],
        ["call_duration","#{@@DEFAULT_CALL_DURATION}","Duration of each call in seconds"],
        ["call_mode","#{@@DEFAULT_CALL_MODE}","Type of call : 0 is MO call, 1 is MT call, 2 alternates between MO and MT"]
                ]
    @@RESULTS                   ="Pass or Fail. If failed, you got a log of messages from API task to MMI task."
    @@EXAMPLE                   ="puts Proto_Auto_Multi_Call.new( {:nb_calls=>12, number=>'112'} ).process <br /> Call emergency."
    @@AUTHOR                    ="Laurent"
      
    TestsHelp.addTestHelpEntry(     @@TEST_NAME,
                                    @@VERSION,    
                                    @@ENVIRONEMENT,   
                                    @@TYPE,   
                                    @@PREREQUISITE,
                                    @@PARAMETERS,
                                    @@DESCRIPTION,
                                    @@RESULTS,
                                    @@EXAMPLE,  
                                    @@AUTHOR  
                            )
    
    #####################################
	#		TEST DESCRIPTION END		#
	#####################################  
    
    #####################################
    #           Private section         #
    #####################################
    
    private
    #Constants
    CMU_CODEC       = ["FRV1","FRV2","HRV1"]
    EXT_IN_LOSS     = "0.5"
    BCCH_CHANNEL    = [0,32,600,600]
    BCCH_LEVEL      = 85
    BCCH_TN         = 3
    TCH_TN          = 3
    TCH_PCL         = 15 
    # Timeout options in seconds :
    TIMEOUT_SIGNALING_ON =  10
    TIMEOUT_MS_SYNC = 15
    TIMEOUT_CALL_ESTABLISHED = 15

    # Call Object. 
    @myCall

    # userOptions.
    @userOptions

    def BuildParams(band,level,arfcn,speech_mode,nb_calls,dial_number,call_duration,call_mode)
        case band
            when 3
                @userOptions.band = 3
            when 2
                @userOptions.band = 2
            else
                @userOptions.band = 1
        end
        @userOptions.level = level
        @userOptions.arfcn = arfcn
        case speech_mode
            when 3
                @userOptions.speech_mode = 2
            when 2
                @userOptions.speech_mode = 1
            else
                @userOptions.speech_mode = 0
        end
        @userOptions.nb_calls= nb_calls
        @userOptions.dial_number= dial_number
        @userOptions.call_duration = call_duration
		@userOptions.call_mode = call_mode
    end

    # COULD BE INTEGRATED IN THE CMU MODULE.
    def WaitCMUSignal(signal,timeout,band)
            
        signal_received=false
        # Query the current signaling state
        CMUW("%d;SIGN:STAT?" % band)
        rdbuf = CMUR()
        
        begin
            Timeout::timeout(timeout) do
                while(rdbuf.slice(0..(signal.length-1)) != signal)
                    # Query the current signaling state.
                    CMUW("%d;SIGN:STAT?" % band)
                    rdbuf = CMUR()
                    sleep(0.01)
                end 
                signal_received=true
                return signal_received
            end
        
        rescue Timeout::Error
           return signal_received
        end 
    end

	def MakeMTCall()
	# initiate an MT call
	#puts "MT call"
		@html_report<< "MT call..."
		CMUW("%d;PROC:SIGN:ACT MTC" % @userOptions.band)
		# in GTES mode the MMI should auto-respond to incoming calls
		if(WaitCMUSignal("CEST",TIMEOUT_CALL_ESTABLISHED,@userOptions.band)==false)
			#puts "failed"
			@test_succeeded = false
			redPrint("Failed to establish MO call !")
			return false
		else
			#puts "distant call established" + "<br /> "
			@html_report<< "Distant Call established..."
			#puts "wait in call.."
			sleep(@userOptions.call_duration)
			# Hang up
			@html_report<< "...Hang up..."
			#puts "Hang up.."
			CMUW("%d;PROC:SIGN:ACT CREL" % @userOptions.band)
			if (WaitCMUSignal("SYNC",TIMEOUT_CALL_ESTABLISHED,@userOptions.band)==false)
				@test_succeeded = false
				#puts "failed !"
				redPrint("<br />" + "Distant Hanging up failed !")
				return false
			end
			#puts "Done"
			@html_report<< "OK" + "<br />"
		end
		return true
	end

	def MakeMOCall()
		# Initialize, start a Call and register the log informations in call_log.
		@myCall = Proto_Auto_Call.new( {:timeout=>6, :start=>1,
										:number=>@userOptions.dial_number} ) 
		@html_report<< "MO call..."
		@myCall.process
			
		# Wait for Call Established.
		if(@myCall.call_success == false)
			#puts "failed"
			@test_succeeded = false
			redPrint("Failed to establish MO call !")
			@html_report<< @myCall.call_log
			return false
		else 
			#puts "call established"
			@html_report<< "Call established..."
			#puts "wait in call.."
			sleep(@userOptions.call_duration)
			# Hang up
			@html_report<< "Hang up..."
			@myCall = Proto_Auto_Call.new( {:timeout=>6, :start=>0} ) 
			#puts "Hanging up.."
			@myCall.process
			if (@myCall.call_success == false)
				@test_succeeded = false
				#puts "failed !"
				redPrint("<br />" + "Hanging up call failed !")
				@html_report<< @myCall.call_log
				return false
			end
			#puts "done"
			@html_report<< "OK" + "<br /> "
		end
		return true
	end

	def redPrint (str)
		@html_report<< "<font color='red'>" + str + "</font>" + "<br />"
	end
    #####################################
	#			Public section			#
	#####################################
    public
      
    def initialize(params)
       
        # CAREFUL !
        # Do not call the super method "initialize" since this test is not an embedded one !
        
        # Call to a UnitTest method :
        # Intialize html report to be compatible with CoolWatcher.
        initializeReport
        
        # Initialize user options.
        @userOptions = McallUserOptions.new
        BuildParams( params[:band] || @@DEFAULT_BAND ,
                     params[:level] || @@DEFAULT_LEVEL,
                     params[:arfcn] || @@DEFAULT_ARFCN,
                     params[:speech_mode]|| @@DEFAULT_SPEECH_MODE, 
                     params[:nb_calls]|| @@DEFAULT_NB_CALLS,
                     params[:dial_number]|| @@DEFAULT_DIAL_NUMBER,
                     params[:call_duration]|| @@DEFAULT_CALL_DURATION,
					 params[:call_mode]|| @@DEFAULT_CALL_MODE)   
                            
    end
      
    def changeParams( params )
        BuildParams( params[:band] ||  @userOptions.band,
                     params[:level] || @userOptions.level,
                     params[:arfcn] || @userOptions.arfcn,
                     params[:speech_mode]|| @userOptions.speech_mode, 
                     params[:nb_calls]|| @userOptions.nb_calls,
                     params[:dial_number]|| @userOptions.dial_number,
                     params[:call_duration]|| @@DEFAULT_CALL_DURATION,
					 params[:call_mode]|| @@DEFAULT_CALL_MODE)   
    end

    def start
        @test_succeeded = true
        # CMU initialization.
        CMUBERInit( @userOptions.band,CMU_CODEC[@userOptions.speech_mode],
                    EXT_IN_LOSS,
                    BCCH_CHANNEL[@userOptions.band],
                    BCCH_LEVEL,
                    BCCH_TN,
                    @userOptions.arfcn,
                    TCH_TN,
                    TCH_PCL,@userOptions.level)
                # Switch on BCCH channel.
        CMUW("%d;PROC:SIGN:ACT SON;*OPC?" % @userOptions.band)
        # Get the query information.
        CMUR()
        
        # Query the current signaling state
        CMUW("%d;SIGN:STAT?" % @userOptions.band)
        rdbuf = CMUR()
    
        # Restart the phone
        restart(false, true)
		#puts "Started the phone at #{Time.now}"
        
		puts "Starting test at #{Time.now}"
        # Wait for Signaling ON.
        if (WaitCMUSignal("SON",TIMEOUT_SIGNALING_ON,@userOptions.band)==true)
			@html_report<< "Signaling ON" + "<br /> "
            sleep(5)
            # Init CoolTester
            CoolTester.init
            # Wait for Synchro.
            if(WaitCMUSignal("SYNC",TIMEOUT_MS_SYNC,@userOptions.band)==true)
                @html_report<< "MS synchronized" + "<br /> "
            else
                redPrint("MS Synchronization Timeout !")
                raise "MS Synchronization Timeout !"
            end
        else
            redPrint("CMU Signaling Mode Timeout !")
            raise"CMU Signaling Mode Timeout !"
        end

        for cur_call in 1..@userOptions.nb_calls
            sleep(4)
			@html_report<< "Performing call #%d" % cur_call + "<br /> "
			if ( (@userOptions.call_mode==1) or
				 ((@userOptions.call_mode==2) and ((cur_call & 1) == 1)) )
				break if (!MakeMTCall())
			else
				break if (!MakeMOCall())
			end
        end

    end

    def process
        start
        results
    end
    
    def results
        super
        @html_report<< CoolTester.reportAddTitleResults
        if @test_succeeded == true
            @html_report<< "<font color='green'>"+"      PASSED !"+"</font>"
        else
            @html_report<< "<font color='red'>"+"      FAILED !"+"</font>"
        end
		CMUClose()
        return @html_report
    end
    
end
