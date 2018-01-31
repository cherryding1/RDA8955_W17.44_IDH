require "UnitTest"

class Board_Manual_Rf < UnitTest
  private
    
    #####################################
	#		DEFAULT PARAMETERS		    #
	#####################################
        
    @@DEFAULT_EXECUTION_TIMEOUT = 10;
    
    #####################################
	#		TEST DESCRIPTION			#
	#####################################
    
    @@TEST_NAME                 = "Rf Test";
    @@VERSION                   = "1.0"
    @@ENVIRONEMENT              = "BOARD";
    @@TYPE                      = "MANUAL";
    @@PREREQUISITE              = "RF chip"
    @@DESCRIPTION               = "This test performs a loopback test on the RF I & Q analog interface.  The means and variances on the Rx ADC are calculated and reported to the tester.  This validates both the board connections as well as the Rx ADC in the chip."
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
  
  
  public

    def initialize(params)
        super(params[:timeout] || @@DEFAULT_EXECUTION_TIMEOUT ,"BOARD_MANUAL_RF")        
    end


    def process
        start();
        results();
    end

    def results
        super
        @test_succeeded = 1;
        if(@results_available)
            @html_report << "<P><b>Average</b></P>"
            @html_report << "<TABLE BORDER=0><TR><TD></TD><TD>I</TD><TD>Q</TD></TR>";
            @html_report << "<TR><TD>A</TD><TD>%i</TD><TD>%i</TD></TR>" % [@results.mean_IA.R, @results.mean_QA.R];
            @html_report << "<TR><TD>B</TD><TD>%i</TD><TD>%i</TD></TR>" % [@results.mean_IB.R, @results.mean_QB.R];
            @html_report << "<TR><TD>C</TD><TD>%i</TD><TD>%i</TD></TR>" % [@results.mean_IC.R, @results.mean_QC.R];
            @html_report << "<TR><TD>D</TD><TD>%i</TD><TD>%i</TD></TR>" % [@results.mean_ID.R, @results.mean_QD.R];
            @html_report << "</TABLE>";

            @html_report << "<P><b>Variance</b></P>"
            @html_report << "<TABLE BORDER=0><caption></caption><TR><TD></TD><TD>I</TD><TD>Q</TD></TR>";
            @html_report << "<TR><TD>A</TD><TD>%i</TD><TD>%i</TD></TR>" % [@results.var_IA.R, @results.var_QA.R];
            @html_report << "<TR><TD>B</TD><TD>%i</TD><TD>%i</TD></TR>" % [@results.var_IB.R, @results.var_QB.R];
            @html_report << "<TR><TD>C</TD><TD>%i</TD><TD>%i</TD></TR>" % [@results.var_IC.R, @results.var_QC.R];
            @html_report << "<TR><TD>D</TD><TD>%i</TD><TD>%i</TD></TR>" % [@results.var_ID.R, @results.var_QD.R];
            @html_report << "</TABLE>";

            puts @html_report

            puts("Value is correct ? (y/n)");
            entry = gets();
            @html_report << "<UL>"
            if (entry.downcase == "y")
                @html_report << "<LI><TABLE BORDER=0 WIDTH=100%><TR><TD WIDTH=50%>RF test</TD><TD>"
                @html_report << "<font color='black'><B>Test successed !</B></font></TD></TR></TABLE>"            
            else
                @html_report << "<LI><TABLE BORDER=0 WIDTH=100%><TR><TD WIDTH=50%>RF test</TD><TD>"
                @html_report << "<font color='red'><B>Test failed !</B></font></TD></TR></TABLE>"    
                @test_succeeded = 0;
            end
            @html_report << "</UL>"
        end
                
        return @html_report
    end

end
