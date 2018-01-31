require "CoolTester/UnitTest"

class Proto_Auto_Call < UnitTest
    
    attr :call_log
	attr :call_success
    
    #####################################
	#			Private section			#
	#####################################
    private
    
    #####################################
	#		DEFAULT PARAMETERS		    #
	#####################################
    
    @@DEFAULT_EXECUTION_TIMEOUT = 10
    @@DEFAULT_START = 0xF1F1F1F1 
    # By default use CoolSand number.
    @@DEFAULT_CALL_NUMBER= "0155331616"
    
    #####################################
	#		TEST DESCRIPTION			#
	#####################################
    
    @@TEST_NAME                 = "Proto Auto Call Test"
    @@VERSION                   = "1.0"
    @@ENVIRONEMENT              = "PROTO"
    @@TYPE                      = "AUTO"
    @@PREREQUISITE              = "Plug an antenna or connect the RF to the CMU + a SIM card"
    @@DESCRIPTION               = "Start/End a call with the given call number :number. The test records messages from API task to MMI task and updates a state machine with connected / disconnected state. The test compares this state with the expected one and gives a result."
    
    @@PARAMETERS=[
                    ["timeout","#{@@DEFAULT_EXECUTION_TIMEOUT}s","Timeout for the test execution"],
                    ["start","#{@@DEFAULT_START}","1:Start, 0:Stop. This parameter MUST be specified because the default value wont be interpreted by the embedded test."],
                    ["number","#{@@DEFAULT_CALL_NUMBER}","Called phone number"]
                ]
    @@RESULTS                   ="Failed or successed. If failed, you got a log of messages from API task to MMI task."
    @@EXAMPLE                   ="puts Proto_Auto_Call.new( {:start=>1, number=>'112'} ).process <br /> Call emergency."
    @@AUTHOR                    ="Matthieu"
      
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
    
   # Store the kind of the current test : "Start call" one or "End call" one.
    @start= @@DEFAULT_START
    
    def wStart ( value )
         # If the value is a good one ( 0 or 1) :
        if value==1 || value ==0
            # Write it in the params structure.
            @params.start.w value
            # Store it in the object.
            @start=value
        else
            # Otherwise, write a default value : the test will return an error :
            # GTES_TEST_ERR_PARAMATERS_MISSING.
            puts "html> <font color='red'> WARNING : Test parameter :start must be 0 or 1 and not #{value} </font> "
            @params.start.w value
        end
    end
    
    def wNumber ( numberStr )
        # First, erase the previous one.
        index=0
        20.times do
            @params.callNumber[index].w 0
            index+=1
        end
        # Then, write the new one.
        index=0 
        numberStr.unpack('U*').each do |char|
            @params.callNumber[index].w char
            index+=1        
        end
    end
    
   
    #####################################
	#			Public section			#
	#####################################
    public
      
    def initialize(params)
       
        super(( params[:timeout] || @@DEFAULT_EXECUTION_TIMEOUT ),"PROTO_AUTO_CALL")
        
        # If the key :start is defined :
        if params.has_key? :start 
            wStart params[:start] 
        else
            wStart @@DEFAULT_START
        end
                             
        # If the key :number is defined we fix it, otherwise we use the default one.
        if params.has_key? :number
           wNumber params[:number] 
        else
           wNumber @@DEFAULT_CALL_NUMBER
        end 
                            
    end
      
    def changeParams( params )
        
        # If the key :start is defined, update this field.
        wStart params[:start]  if params.has_key? :start
          
        # If the key :number is defined we change the callNumber.
        wNumber params[:number] if params.has_key? :number
                         
    end
    
    def process
        start
        results
    end
    
    def results
        super
        # If no error during the call start/end process occured, let's check the stack answer.
        @call_log=''
        @call_success=false
		if @results_available
            case @start
                when 0
                    # If the call test is a "End call" one :
                    @test_succeeded,@call_log=CoolTester.callReleased?(10)
                when 1
                    # If the call test is a "Start call" one :
                    # We expect an API_CC_CONNECT_IND msg from the stack.
                    @test_succeeded,@call_log=CoolTester.callEstablished?(10)
                else
                    # This case shall not happen since @results_available will be false...
                    @call_log << "<font color='red'> Test parameter :start must be 0 or 1 and not #{@start}</font>"    
            end
           
            if @test_succeeded==false
                    
                    @html_report<< "<font color='red'> Test failed !</font>" + "<br />"
                    @html_report<< "Below, log of the call (API->MMI mesages)" + "<br /> "
                    @html_report<< @call_log
            else
                    @html_report<< "<font color='#008800'> Test suceeded !</font>" + "<br />" 
            end    
            
            @html_report<< @call_log if @test_succeeded==false
        
            
        else
            # Error on the @start parameters !
            # An GTES_TEST_ERR_BAD_PARAMETERS/GTES_TEST_ERR_PARAMATERS_MISSING happened.
            @call_log << "<font color='red'> Test parameter :start must be 0 or 1 and not #{@start}</font>"    
        end
        
        
        @call_success=@test_succeeded
        return @html_report,@call_success
    end
    
end
