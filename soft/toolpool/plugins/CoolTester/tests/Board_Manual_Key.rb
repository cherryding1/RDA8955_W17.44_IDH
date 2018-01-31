require "UnitTest"

class Board_Manual_Key < UnitTest
  private
    
    #####################################
	#		DEFAULT PARAMETERS		    #
	#####################################
        
    @@DEFAULT_EXECUTION_TIMEOUT = 30;
    
    #####################################
	#		TEST DESCRIPTION			#
	#####################################
    
    
    @@TEST_NAME                 = "Key Test";
    @@VERSION                   = "1.0"
    @@ENVIRONEMENT              = "BOARD";
    @@TYPE                      = "MANUAL";
    @@PREREQUISITE              = "NONE"
    @@DESCRIPTION               = "The user is invited to press in order the key 1,2,3,4,5,6,7,8,9,*,0,#. Timeout after 20s"
    @@PARAMETERS=[
                    ["timeout","#{@@DEFAULT_EXECUTION_TIMEOUT}s","Timeout for the test execution"]                  
                ]
    @@RESULTS                   =""
    @@EXAMPLE                   =""
    @@AUTHOR                    ="Lilian"
    
   TestsHelp.addTestHelpEntry(      @@TEST_NAME,
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
    
     @@TEST_RETURN_CODE          = CH__gtes_test_board_manual_key_globals;
    
  public

    def initialize(params)
        super(params[:timeout] || @@DEFAULT_EXECUTION_TIMEOUT ,"BOARD_MANUAL_KEY")        
    end


    def process
        puts("Press key in order 1, 2, 3, 4, 5, 6, 7, 8, 9, *, 0, #");
        start();
        results();
    end

    def results
        super
        @test_succeeded = 1;
        if(@results_available)
            @html_report << "<UL>"
            if(@results.result.R == @@TEST_RETURN_CODE::GTES_TEST_BOARD_MANUAL_KEY_OK)
                @html_report << "<LI><TABLE BORDER=0 WIDTH=100%><TR><TD WIDTH=50%>Key test</TD><TD>"
                @html_report << "<font color='black'><B>Test successed !</B></font></TD></TR></TABLE>"            
            else
                @html_report << "<LI><TABLE BORDER=0 WIDTH=100%><TR><TD WIDTH=50%>Key test</TD><TD>"
                @html_report << "<font color='red'><B>Test failed !</B></font></TD></TR></TABLE>"    
                @test_succeeded = 0;
            end
            @html_report << "</UL>"
        end
                
        return @html_report
    end

end
