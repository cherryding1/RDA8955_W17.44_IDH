require "UnitTest"

class Board_Auto_Pll < UnitTest
  private
    #####################################
	#		DEFAULT PARAMETERS		    #
	#####################################
    
    @@DEFAULT_EXECUTION_TIMEOUT = 10;
    
    #####################################
	#		TEST DESCRIPTION			#
	#####################################
    
    @@TEST_NAME                 = "Pll Auto Test";
    @@VERSION                   = "1.0";
    @@ENVIRONEMENT              = "BOARD";
    @@TYPE                      = "AUTO";
    @@PREREQUISITE              = "NONE";
    @@DESCRIPTION               = "Disable the PLL, reenable the PLL and measure the lock time of the PLL";
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
    
  
  public

    def initialize(params)
        super(params[:timeout] || @@DEFAULT_EXECUTION_TIMEOUT ,"BOARD_AUTO_PLL")
    end


    def process
        start
        results
    end

    def results
        super
        @test_succeeded = 1;
        if(@results_available)
            @html_report << "<UL>";
            @html_report<< "<LI><TABLE BORDER=0 WIDTH=100%><TR><TD WIDTH=50%>PLL lock</TD>";
            if(@results.time.R == 0xFFFFFFFF)     
                @html_report<< "<TD ALIGN=\"left\"><font color='red'><B>Test failed! PLL not lock</B></font></TD></TR></TABLE>";
            else
                @html_report<< "<TD ALIGN=\"left\"><font color='black'><B>Pll lock after %d.%02ds</B></font></TD></TR></TABLE>" % [ @results.time.R/100, @results.time.R%100];
            end      
            @html_report << "</UL>";
        else
            # Error on the @start parameters !
            # An GTES_TEST_ERR_BAD_PARAMETERS/GTES_TEST_ERR_PARAMATERS_MISSING happened.
        end
        
        
        return @html_report
    end

end
