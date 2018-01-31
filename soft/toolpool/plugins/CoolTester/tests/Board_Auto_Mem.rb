
require "UnitTest"

class Board_Auto_Mem < UnitTest
  private
    #####################################
	#		DEFAULT PARAMETERS		    #
	#####################################
    
    @@DEFAULT_EXECUTION_TIMEOUT = 10;
    @@DEFAULT_ADDR_BEGIN    =   0
    @@DEFAULT_ADDR_END    =   0
    
    #####################################
	#		TEST DESCRIPTION			#
	#####################################
	
    @@TEST_NAME                 = "Auto Memory Test";
    @@VERSION                   = "1.0";
    @@ENVIRONEMENT              = "BOARD";
    @@TYPE                      = "AUTO";
    @@PREREQUISITE              = "";
    @@DESCRIPTION               = "Check an area memory. Write a patern and read it.";
    
    @@PARAMETERS=[
                    ["timeout","#{@@DEFAULT_EXECUTION_TIMEOUT}s","Timeout for the test execution"],
                    ["addr_begin","#{@@DEFAULT_ADDR_BEGIN}","Memory area check. Default value is a dynamic buffer allocated"] ,
                    ["addr_end","#{ @@DEFAULT_ADDR_END}","Memory area check. Default value is a dynamic buffer allocated"]               
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
    
    @@TEST_RETURN_CODE          = CH__gtes_test_board_auto_mem_globals;
    
    def print_result(res) 
        case(res)       
            when @@TEST_RETURN_CODE::GTES_TEST_BOARD_AUTO_MEM_KO
                @html_report<< "<font color='red'><B>Test failed !</B></font>"
                @test_succeeded = 0;
            when @@TEST_RETURN_CODE::GTES_TEST_BOARD_AUTO_MEM_OK
                @html_report<< "<font color='black'><B>Test successed !</B></font>"
        end      
    end

  public

    def initialize(params)
        super(params[:timeout] || @@DEFAULT_EXECUTION_TIMEOUT ,"BOARD_AUTO_MEM")
        @params.addr_begin.w(params[:addr_begin] ||  @@DEFAULT_ADDR_BEGIN);
        @params.addr_end  .w(params[:addr_end  ] ||  @@DEFAULT_ADDR_END  );
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
            @html_report<< "<LI><TABLE BORDER=0 WIDTH=100%><TR><TD WIDTH=50%>MemCheck</TD>";
            if(@results.mem_result.R == @@TEST_RETURN_CODE::GTES_TEST_BOARD_AUTO_MEM_OK)     
                @html_report<< "<TD ALIGN=\"left\"><font color='black'><B>Test successed !</B></font></TD></TR></TABLE>";
            else
                @html_report<< "<TD ALIGN=\"left\"><font color='red'><B>Test failed ! Addr 0x%08x</B></font></TD></TR></TABLE>" % @results.addr.R;
            end       
            @html_report << "</UL>";
        else
            # Error on the @start parameters !
            # An GTES_TEST_ERR_BAD_PARAMETERS/GTES_TEST_ERR_PARAMATERS_MISSING happened.
        end
        
        
        return @html_report
    end

end
