require "UnitTest"

class Board_Manual_Gpo_Clk < UnitTest
  private
    #####################################
	#		DEFAULT PARAMETERS		    #
	#####################################
    
    @@DEFAULT_EXECUTION_TIMEOUT = 360;
    
    
    #####################################
	#		TEST DESCRIPTION			#
	#####################################
    
    
    @@TEST_NAME                 = "Gpo_Clk Test";
    @@VERSION                   = "1.0";
    @@ENVIRONEMENT              = "BOARD";
    @@TYPE                      = "MANUAL";
    @@PREREQUISITE              = "Connecting scope on the GPO1, GPO2 and DBGCLK"
    @@DESCRIPTION               = "This test generate a square signal on GPO1 and GPO2 with a frequence of 50Hz and 25Hz. On DBGCLK, you must see the 32kHz clock on scope"
    @@PARAMETERS=[
                    ["timeout","#{@@DEFAULT_EXECUTION_TIMEOUT}s","Timeout for the test execution"]                  
                ]
    @@RESULTS                   =""
    @@EXAMPLE                   =""
    @@AUTHOR                    ="Lilian"
    
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
    @@TEST_RETURN_CODE          = CH__gtes_test_board_manual_gpo_clk_globals;
    
    OK   = 0;
    KO   = 1;
    
    
  public

    def initialize(params)
        super(params[:timeout] || @@DEFAULT_EXECUTION_TIMEOUT ,"BOARD_MANUAL_GPO_CLK")
    end


    def process
        start
        results
    end

    def results
        super
        @test_succeeded = 1;
        if(@results_available)
            puts("Do you watch on GPO1 square signal freq 50Hz ? (y/n)");
            entry = gets();
            if (entry.downcase == "y")
                @gpo1_result    = OK;
            else
                @gpo1_result    = KO;
                @test_succeeded = 0;
            end
            puts("Do you watch on GPO2 square signal freq 25Hz ? (y/n)");
            entry = gets();
            if (entry.downcase == "y")
                @gpo2_result    = OK;
            else
                @gpo2_result    = KO;
                @test_succeeded = 0;
            end
            puts("Do you watch on CLK_DBG clock signal freq 32kHz ? (y/n)");
            entry = gets();
            if (entry.downcase == "y")
                @clkdbg_result    = OK;
            else
                @clkdbg_result    = KO;
                @test_succeeded = 0;
            end

            @html_report << "<UL>";
            @html_report<< "<LI><TABLE BORDER=0 WIDTH=100%><TR><TD WIDTH=50%>GPO1</TD>";
            if(@gpo2_result == KO)     
                @html_report<< "<TD ALIGN=\"left\"><font color='red'><B>Test failed!</B></font></TD></TR></TABLE>";
            else
                @html_report<< "<TD ALIGN=\"left\"><font color='black'><B>Test OK!</B></font></TD></TR></TABLE>";
            end       
            @html_report<< "<LI><TABLE BORDER=0 WIDTH=100%><TR><TD WIDTH=50%>GPO2</TD>";
            if(@gpo2_result == KO)     
                @html_report<< "<TD ALIGN=\"left\"><font color='red'><B>Test failed!</B></font></TD></TR></TABLE>";
            else
                @html_report<< "<TD ALIGN=\"left\"><font color='black'><B>Test OK!</B></font></TD></TR></TABLE>";
            end       
            @html_report<< "<LI><TABLE BORDER=0 WIDTH=100%><TR><TD WIDTH=50%>CLK_DBG</TD>";
            if(@clkdbg_result == KO)     
                @html_report<< "<TD ALIGN=\"left\"><font color='red'><B>Test failed!</B></font></TD></TR></TABLE>";
            else
                @html_report<< "<TD ALIGN=\"left\"><font color='black'><B>Test OK!</B></font></TD></TR></TABLE>";
            end       
            @html_report << "</UL>";
        else
            # Error on the @start parameters !
            # An GTES_TEST_ERR_BAD_PARAMETERS/GTES_TEST_ERR_PARAMATERS_MISSING happened.
        end
        
        
        return @html_report
    end

end
