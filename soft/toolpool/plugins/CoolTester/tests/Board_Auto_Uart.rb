require "UnitTest"

class Board_Auto_Uart < UnitTest
  private
    
    #####################################
	#		DEFAULT PARAMETERS		    #
	#####################################
    
    @@DEFAULT_EXECUTION_TIMEOUT = 10
    @@DEFAULT_COM_PORT          = 1
    
    #####################################
	#		TEST DESCRIPTION			#
	#####################################
    @@TEST_NAME                 = "Uart auto board test"
    @@VERSION                   = "1.0"
    @@ENVIRONEMENT              = "BOARD"
    @@TYPE                      = "AUTO"
    @@PREREQUISITE              = "Connect the serial cable between the PC and board"
    @@DESCRIPTION               = "The embedded software is a software loopback. Cooltester send a data, receive it and compare the data"
    @@PARAMETERS=[
                    ["timeout","#{@@DEFAULT_EXECUTION_TIMEOUT}s","Timeout for the test execution"],
                    ["port","#{@@DEFAULT_COM_PORT}","PC serial port"]                  
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
    
    @@TEST_ACTION_CODE          = CH__gtes_test_board_auto_uart_globals
    @@PACKET_SEND   = [0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0xFF]
    @@LOOPBACK_FLOW = 0xE0
    
    @@OK            = 0
    @@KO            = 1
                    
    @result         = @@KO
    @com_port
    
  public

    def initialize(params)
        super(params[:timeout] || @@DEFAULT_EXECUTION_TIMEOUT ,"BOARD_AUTO_UART")        
        @com_port = params[:port]
        @result   = @@KO
    end


    def process
        begin
            
            if(!@com_port)
                puts "No com port defined for UART! Please enter a number for this com port :"
                @com_port = gets.to_i
            end
            
            @params.start.w(1)
            start()
            connection = CHUartConnection.new(@com_port,115200)    
            connection.open(true)    
            uartbypass = CHBPConnection.new(connection, [@@LOOPBACK_FLOW])
            uartbypass.open(false)
            
            sleep 0.1
            puts "Sending #{@@PACKET_SEND.inspect}"
            uartbypass.sendPacket(@@LOOPBACK_FLOW, @@PACKET_SEND)
            
            begin
                packet = uartbypass.waitPacketForFlowId(@@LOOPBACK_FLOW,5.0)
                puts "Received #{packet[1].inspect}"
                if(@@PACKET_SEND == packet[1])
                  puts "html><font color=green>[Data is the same as sent.]</font>"
                  @result = @@OK
                else
                  puts "html><font color=red>[Data is NOT the same as sent.]</font>"
                  @result = @@KO
                end        
            rescue
                @result = @@KO          
            end
    
            @params.start.w(0)
            #start()
            results()
        ensure
            uartbypass.close() if(uartbypass)
            connection.close() if(connection)
        end
    end
    
    def results
        super
        @test_succeeded = 1
        if(@results_available)
            @html_report << "<UL>"
            @html_report << "<LI><TABLE BORDER=0 WIDTH=100%><TR><TD WIDTH=50%>Uart board test</TD><TD>"
            if(@result == @@OK)
                @html_report<< "<font color='black'><B>Test successed !</B></font>"
            else
                @html_report<< "<font color='red'><B>Test failed !</B></font>"
                @test_succeeded = 0
            end

            @html_report << "</TD></TR></TABLE></UL>"
        end
        return @html_report
    end

end
