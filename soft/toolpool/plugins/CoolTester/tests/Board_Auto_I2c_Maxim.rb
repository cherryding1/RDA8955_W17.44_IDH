require "UnitTest"

class Board_Auto_I2c_Maxim < UnitTest
  private
    
    #####################################
	#		DEFAULT PARAMETERS		    #
	#####################################
    
    @@DEFAULT_EXECUTION_TIMEOUT = 10;
    
    #####################################
	#		TEST DESCRIPTION			#
	#####################################
	
    @@TEST_NAME                 = "Auto I2c Maxim Test";
    @@VERSION                   = "1.0";
    @@ENVIRONEMENT              = "BOARD";
    @@TYPE                      = "AUTO";
    @@PREREQUISITE              = "I2C Maxim chip on the board";
    @@DESCRIPTION               = "Write a value on right speak volume register, read this register and compare the value.";
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
    
    @@TEST_RETURN_CODE          = CH__gtes_test_board_auto_i2c_maxim_globals;
    
  public

    def initialize(params)
        super(params[:timeout] || @@DEFAULT_EXECUTION_TIMEOUT ,"BOARD_AUTO_I2C_MAXIM")
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
            @html_report<< "<LI><TABLE BORDER=0 WIDTH=100%><TR><TD WIDTH=50%>I2C on maxim</TD>";
            if(@results.result.R == @@TEST_RETURN_CODE::GTES_TEST_BOARD_AUTO_I2C_MAXIM_KO)     
                @html_report<< "<TD ALIGN=\"left\"><font color='red'><B>Test failed!</B></font></TD></TR></TABLE>";
                @test_succeeded = 0;
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
