require "UnitTest"

class Hal_Auto_Uart < UnitTest
  private
    
    #####################################
	#		DEFAULT PARAMETERS		    #
	#####################################
    
    @@DEFAULT_EXECUTION_TIMEOUT = 10
    @@DEFAULT_BITRATE1                                          = 115200
    @@DEFAULT_BITRATE2                                          = 57600
    @@DEFAULT_ENABLE_BIG_PACKET_TEST                            = 1
    @@DEFAULT_ENABLE_CTLFLOWS_TEST                              = 1
    @@DEFAULT_ENABLE_DATA_TRANSMIT_PARAMETER_TEST               = 1
       
    #####################################
	#		TEST DESCRIPTION			#
	#####################################
        
    @@TEST_NAME                 = "Uart Auto Test"
    @@VERSION                   = "1.0"
    @@ENVIRONEMENT              = "HAL"
    @@TYPE                      = "AUTO"
    @@PREREQUISITE              = "NONE"
    @@DESCRIPTION               = "HAL test on loopback mode. UART mode are tested (DMA, IRQ, Pool), 8-7bits mode, baudrate change, Control flow, FIFO overflow, Big packet for DMA mode"
    
    @@PARAMETERS=[
                    ["timeout","#{@@DEFAULT_EXECUTION_TIMEOUT}s","Timeout for the test execution"],
                    ["bitRate1","#{@@DEFAULT_BITRATE1}","Bit rate use for all test"],
                    ["bitRate2","#{@@DEFAULT_BITRATE2}","Bit rate use for the change bitrate test"],
                    ["enable_BigPacket_Test","#{@@DEFAULT_ENABLE_BIG_PACKET_TEST}","Enable the big packet test and Fifo overflow"],
                    ["enable_CtlFlows_Test","#{@@DEFAULT_ENABLE_CTLFLOWS_TEST}","Enable auto control flow test"],
                    ["enable_Data_Transmit_Parameter_Test","#{@@DEFAULT_ENABLE_DATA_TRANSMIT_PARAMETER_TEST}","Enable the 7 bits test and BitRate change"]
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
    
    @@TEST_RETURN_CODE        = CH__gtes_test_hal_auto_uart_globals
      
    def print_result(res) 
        case(res)       
            when @@TEST_RETURN_CODE::GTES_TEST_HAL_AUTO_UART_KO
                @html_report<< "<font color='red'><B>Test failed !</B></font>"
                @test_succeeded = 0;
            when @@TEST_RETURN_CODE::GTES_TEST_HAL_AUTO_UART_SHIP
                @html_report<< "<font color='orange'><B>Test shiped !</B></font>"
            when @@TEST_RETURN_CODE::GTES_TEST_HAL_AUTO_UART_OK
                @html_report<< "<font color='black'><B>Test successed !</B></font>"
        end      
    end

  public

    def initialize(params)
        super(params[:timeout] || @@DEFAULT_EXECUTION_TIMEOUT ,"HAL_AUTO_UART")        
        @params.BitRate1.w(params[:bitRate1]                           || @@DEFAULT_BITRATE1);
        @params.BitRate2.w(params[:bitRate2]                           || @@DEFAULT_BITRATE2);
        @params.Enable_BigPacket_Test.w(params[:enable_BigPacket_Test] || @@DEFAULT_ENABLE_BIG_PACKET_TEST);
        @params.Enable_CtlFlows_Test.w(params[:enable_CtlFlows_Test]   ||  @@DEFAULT_ENABLE_CTLFLOWS_TEST);
        @params.Enable_Data_Transmit_Parameter_Test.w(params[:enable_Data_Transmit_Parameter_Test] || @@DEFAULT_ENABLE_DATA_TRANSMIT_PARAMETER_TEST);
    end


    def process
        start
        results
    end

    def results
        super
        @test_succeeded = 1;
        if @results_available
            @html_report << "<UL>"
            @html_report<< "<LI>Mode Test"
                @html_report<< "<TABLE BORDER=0 WIDTH=100%>"
                @html_report<< "<TR><TD WIDTH=50%>Pool</TD><TD ALIGN=\"left\">"
                print_result(@results.Mode_Poll    .R);
                @html_report<< "</TD></TR><TR><TD>Pool Irq</TD><TD ALIGN=\"left\">"
                print_result(@results.Mode_Poll_Irq.R);
                @html_report<< "</TD></TR><TR><TD>Dma</TD><TD ALIGN=\"left\">"
                print_result(@results.Mode_Dma     .R);
                @html_report<< "</TD></TR><TR><TD>Dma Irq</TD><TD ALIGN=\"left\">"
                print_result(@results.Mode_Dma_Irq .R);
                @html_report<< "</TD></TR></TABLE>"
            @html_report<< "<LI>Big Packet"
                @html_report<< "<TABLE BORDER=0 WIDTH=100%>"
                @html_report<< "<TR><TD WIDTH=50%>FIFO OverFlow</TD><TD ALIGN=\"left\">"
                print_result(@results.Fifo_OVR     .R);
                @html_report<< "</TD></TR><TR><TD>Big Packet</TD><TD ALIGN=\"left\">"
                print_result(@results.BigPacket    .R);
                @html_report<< "</TD></TR></TABLE>"
            @html_report<< "<LI>Flows control"
                @html_report<< "<TABLE BORDER=0 WIDTH=100%>"
                @html_report<< "<TR><TD WIDTH=50%>Automatic Flux Control</TD><TD ALIGN=\"left\">"
                print_result(@results.CtlFlows     .R);
                @html_report<< "</TD></TR></TABLE>"
            @html_report<< "<LI>Data Transmit Parameter"
                @html_report<< "<TABLE BORDER=0 WIDTH=100%>"
                @html_report<< "<TR><TD WIDTH=50%>Data in 7 bits mode</TD><TD ALIGN=\"left\">"
                print_result(@results.Mode7bits    .R);
                @html_report<< "</TD></TR><TR><TD>BitRate change</TD><TD ALIGN=\"left\">"
                print_result(@results.Bitrate      .R);            
                @html_report<< "</TD></TR></TABLE>"
            @html_report << "</UL>"

        else
            # Error on the @start parameters !
            # An GTES_TEST_ERR_BAD_PARAMETERS/GTES_TEST_ERR_PARAMATERS_MISSING happened.
            @html_report << "<font color='red'> Test parameter : Same BitRate1 and BitRate2</font>"    
        end
        
        
        return @html_report
    end

end
