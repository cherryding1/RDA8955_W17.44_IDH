require "UnitTest"

class Board_Auto_Sim < UnitTest
  private
    #####################################
	#		DEFAULT PARAMETERS		    #
	#####################################
    
    @@DEFAULT_EXECUTION_TIMEOUT  = 10;
    
    #####################################
	#		TEST DESCRIPTION			#
	#####################################
	
    @@TEST_NAME                  = "Sim Auto Test";
    @@VERSION                    = "1.0";
    @@ENVIRONEMENT               = "BOARD";
    @@TYPE                       = "AUTO";
    @@PREREQUISITE               = "Sim card support EPPS and Select command";
    @@DESCRIPTION                = "Reset the sim card (ATR) with different voltage (1.8V 3.0V 5.0V), send a EPPS command, send a Select command and check the reponse";
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
    
    @@TEST_RETURN_CODE           = CH__gtes_test_board_auto_sim_globals;
    
    def print_result(res) 
        case(res)       
            when @@TEST_RETURN_CODE::GTES_TEST_BOARD_AUTO_SIM_KO
                @html_report<< "<font color='red'><B>Test failed !</B></font>"
                @test_succeeded = 0;
            when @@TEST_RETURN_CODE::GTES_TEST_BOARD_AUTO_SIM_SHIP
                @html_report<< "<font color='orange'><B>Test shiped !</B></font>"
            when @@TEST_RETURN_CODE::GTES_TEST_BOARD_AUTO_SIM_OK
                @html_report<< "<font color='black'><B>Test successed !</B></font>"
        end      
    end

  public

    def initialize(params)
        super(params[:timeout] || @@DEFAULT_EXECUTION_TIMEOUT ,"BOARD_AUTO_SIM")        
    end

    def process
        start
        results
    end

    def results
        super
        @test_succeeded = 1;
        if(@results_available)
            @html_report << "<UL>"
            @html_report<< "<LI><TABLE BORDER=0 WIDTH=100%><TR><TD WIDTH=50%>ATR</TD><TD ALIGN=\"left\">"
            if(@results.ATR_Sim_Class_A.R == @@TEST_RETURN_CODE::GTES_TEST_BOARD_AUTO_SIM_KO)
                @html_report<< "<font color='red'><B>Failed</B></font> Check sim card present"
                @html_report<< "</TD></TR></TABLE>";
                @test_succeeded = 0;
            else
                if(@results.ATR_Sim_Class_A.R == @@TEST_RETURN_CODE::GTES_TEST_BOARD_AUTO_SIM_OK)
                    @html_report<< "<font color='black'><B>5V SIM</B></font>"                        
                end
                if(@results.ATR_Sim_Class_B.R == @@TEST_RETURN_CODE::GTES_TEST_BOARD_AUTO_SIM_OK)
                    @html_report<< "<font color='black'><B>3V SIM</B></font>"                        
                end
                if(@results.ATR_Sim_Class_C.R == @@TEST_RETURN_CODE::GTES_TEST_BOARD_AUTO_SIM_OK)
                    @html_report<< "<font color='black'><B>1.8V SIM</B></font>"                        
                end
                @html_report<< "</TD></TR></TABLE>";
                @html_report<< "<LI><TABLE BORDER=0 WIDTH=100%><TR><TD WIDTH=50%>EPPS</TD><TD ALIGN=\"left\">"
                print_result(@results.EPPS.R);
                @html_report<< "</TD></TR></TABLE>";
                @html_report<< "<LI><TABLE BORDER=0 WIDTH=100%><TR><TD WIDTH=50%>Select</TD><TD ALIGN=\"left\">"
                print_result(@results.Select.R);
                @html_report<< "</TD></TR></TABLE>";
            end
            @html_report << "</UL>"
        end
        
        
        return @html_report
    end

end
