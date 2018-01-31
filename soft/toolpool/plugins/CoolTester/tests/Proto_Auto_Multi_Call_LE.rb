require "UnitTest"
require "Proto_Auto_Call"

class McallLEUserOptions
        attr_accessor :nb_calls, :dial_number, :call_duration
end

class Proto_Auto_Multi_Call_LE < UnitTest
    
    #####################################
	#		DEFAULT PARAMETERS		    #
	#####################################
    
    @@DEFAULT_DIAL_NUMBER= "666"
    @@DEFAULT_NB_CALLS= 2
    @@DEFAULT_CALL_DURATION= 4
    
    #####################################
	#		TEST DESCRIPTION			#
	#####################################
    
    @@TEST_NAME                 = "Proto Auto Multi Call Light Edition Test"
    @@VERSION                   = "1.0"
    @@ENVIRONEMENT              = "PROTO"
    @@TYPE                      = "AUTO"
    @@PREREQUISITE              = "MS must be camped on Tester or Network"
    @@DESCRIPTION               = "Performs N :nb_calls calls with the given call number :number. For each call the test records messages from API task to MMI task and updates a state machine with connected / disconnected state. The test compares this state with the expected one and gives a result. If one call fails the test fails."
    
    @@PARAMETERS=[
        ["nb_calls","#{@@DEFAULT_NB_CALLS}","Number of calls to be performed."],
        ["dial_number","#{@@DEFAULT_DIAL_NUMBER}","Called phone number"],
        ["call_duration","#{@@DEFAULT_CALL_DURATION}","Duration of each call in seconds"]
                ]
    @@RESULTS                   ="Pass or Fail. If failed, you got a log of messages from API task to MMI task."
    @@EXAMPLE                   ="puts Proto_Auto_Multi_Call_LE.new( {:nb_calls=>12, number=>'112'} ).process <br /> Call emergency."
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
    TIMEOUT_SIGNALING_ON =  10
    TIMEOUT_MS_SYNC = 15
    TIMEOUT_CALL_ESTABLISHED = 15

    # Call Object. 
    @myCall

    # userOptions.
    @userOptions

    def BuildParams(nb_calls,dial_number,call_duration)
        @userOptions.nb_calls    = nb_calls
        @userOptions.dial_number = dial_number
        @userOptions.call_duration = call_duration
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
        @userOptions = McallLEUserOptions.new
        BuildParams( params[:nb_calls]|| @@DEFAULT_NB_CALLS,
                     params[:dial_number]|| @@DEFAULT_DIAL_NUMBER,
                     params[:call_duration]|| @@DEFAULT_CALL_DURATION)   

    end
      
    def changeParams( params )
        BuildParams( params[:nb_calls]|| @userOptions.nb_calls,
                     params[:dial_number]|| @userOptions.dial_number,
                     params[:call_duration]|| @@DEFAULT_CALL_DURATION)   
    end

    def start
    
        @test_succeeded = true
        for cur_call in 1..@userOptions.nb_calls
            # Initialize, start a Call and register the log informations in call_log.
            #puts "Initiate MO call #%d ..." % cur_call
            @html_report<< "Initiate MO call #%d ..." % cur_call + "<br /> "
            @myCall = Proto_Auto_Call.new( {:timeout=>10,
                                            :start=>1,
                                            :number=>@userOptions.dial_number} ) 
            @myCall.process
                

            if (@myCall.call_success == false)
                @test_succeeded = false
                #puts "MO call failed !"
                @html_report<< "MO call failed !" + "<br /> "
                @html_report<< @myCall.call_log
                break
            else
                # Wait for call_duration.
                sleep(@userOptions.call_duration)
                # Hang up
                #puts "...Hang up"
                @html_report<< "...Hang up" + "<br /> "
                @myCall = Proto_Auto_Call.new( {:timeout=>5, :start=>0} ) 
                @myCall.process
                sleep(2)
                if (@myCall.call_success == false)
                    @test_succeeded = false
                    #puts "Hanging up call failed !" + "<br /> "
                    @html_report<< "Hanging up call failed !" + "<br /> "
                    @html_report<< @myCall.call_log
                    break
                end
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
            @html_report<< "<font color='green'>"+"     PASSED !"+"</font>"
        else
            @html_report<< "<font color='red'>"+"     FAILED !"+"</font>"
        end
        return @html_report
    end
    
end
