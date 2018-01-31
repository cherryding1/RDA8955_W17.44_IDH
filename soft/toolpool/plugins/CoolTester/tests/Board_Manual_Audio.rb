require "UnitTest"

class Board_Manual_Audio < UnitTest
  private
    
    #####################################
	#		DEFAULT PARAMETERS		    #
	#####################################
    
     @@DEFAULT_EXECUTION_TIMEOUT = 360;
    
    #####################################
	#		TEST DESCRIPTION			#
	#####################################
       
    @@TEST_NAME                 = "Audio Test";
    @@ENVIRONEMENT              = "BOARD";
    @@TYPE                      = "MANUAL";
    @@VERSION                   = "1.0";
    @@PREREQUISITE              = "Connecting the headphone and microphone to the board";
    @@DESCRIPTION               = "The first test playing a sample. The second test playing a tone. And the last test to make a loopback."
    @@PARAMETERS=[
                    ["timeout","#{@@DEFAULT_EXECUTION_TIMEOUT}s","Timeout for the test execution"]                  
                ]
    @@RESULTS                   =""
    @@EXAMPLE                   =""   
    @@AUTHOR                    ="Lilian"
    
    TestsHelp.addTestHelpEntry(    @@TEST_NAME,
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
    
    @@TEST_ACTION_CODE          = CH__gtes_test_board_manual_audio_globals;
    
    OK   = 0;
    KO   = 1;
    SHIP = 2;
    
    def print_result(res) 
        case(res)       
            when KO
                @html_report<< "<font color='red'><B>Test failed !</B></font>"
                @test_succeeded = 0;
            when SHIP
                @html_report<< "<font color='orange'><B>Test shiped !</B></font>"
            when OK
                @html_report<< "<font color='black'><B>Test successed !</B></font>"
        end      
    end

  public

    def initialize(params)
        super(params[:timeout] || @@DEFAULT_EXECUTION_TIMEOUT ,"BOARD_MANUAL_AUDIO")        
        @play_result              = SHIP;
        @tone_result              = SHIP;
        @loop_result              = SHIP; 
    end


    def process
        @params.action.w(@@TEST_ACTION_CODE::GTES_TEST_BOARD_MANUAL_AUDIO_ACTION_PLAY);
        start();
        puts("Do you hear the sample ? (y/n)");
        entry = gets();
        if (entry.downcase == "y")
            @play_result = OK;
        else
            @play_result = KO;
        end

        @params.action.w(@@TEST_ACTION_CODE::GTES_TEST_BOARD_MANUAL_AUDIO_ACTION_TONE);
        start();
        puts("Do you hear tone ? (y/n)");
        entry = gets();
        if (entry.downcase == "y")
            @tone_result = OK;
        else
            @tone_result = KO;
        end

        if(@play_result != KO) 
            @params.action.w(@@TEST_ACTION_CODE::GTES_TEST_BOARD_MANUAL_AUDIO_ACTION_LOOPBACK);
            start();
            puts("Do you hear your voice ? (y/n)");
            entry = gets();
            if (entry.downcase == "y")
                @loop_result = OK;
            else
                @loop_result = KO;
            end
        end

        @params.action.w(@@TEST_ACTION_CODE::GTES_TEST_BOARD_MANUAL_AUDIO_ACTION_STOP);
        start();
        results();
    end

    def results
        super
        @test_succeeded = 1;
        if(@results_available)
            @html_report << "<UL>"
            @html_report << "<LI><TABLE BORDER=0 WIDTH=100%><TR><TD WIDTH=50%>Play sample</TD><TD>"
            print_result(@play_result);
            @html_report << "</TD></TR></TABLE>"
            @html_report << "<LI><TABLE BORDER=0 WIDTH=100%><TR><TD WIDTH=50%>Play tone</TD><TD>"
            print_result(@tone_result);
            @html_report << "</TD></TR></TABLE>"
            @html_report << "<LI><TABLE BORDER=0 WIDTH=100%><TR><TD WIDTH=50%>LoopBack</TD><TD>"
            print_result(@loop_result);
            @html_report << "</TD></TR></TABLE>"            
            @html_report << "</UL>"
        end
                
        return @html_report
    end

end
