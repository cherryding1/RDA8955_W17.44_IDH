require "UnitTest"

class Proto_Auto_Attach < UnitTest
    
    attr :attach_log
	attr :attach_success
    
    #####################################
	#			Private section			#
	#####################################
    private
    
    #####################################
	#		DEFAULT PARAMETERS		    #
	#####################################
    
    @@DEFAULT_EXECUTION_TIMEOUT = 10
    @@DEFAULT_START = 0xF1F1F1F1 
    
    #####################################
	#		TEST DESCRIPTION			#
	#####################################
    
    @@TEST_NAME                 = "Proto Auto Attach Test"
    @@VERSION                   = "1.0"
    @@ENVIRONEMENT              = "PROTO"
    @@TYPE                      = "AUTO"
    @@PREREQUISITE              = "Plug an antenna or connect the RF to the CMU + a SIM card"
    @@DESCRIPTION               = "gprs attach or detach"
    
    @@PARAMETERS=[
                    ["timeout","#{@@DEFAULT_EXECUTION_TIMEOUT}s","Timeout for the test execution"],
                    ["start","#{@@DEFAULT_START}","1:Attach, 0:Detach. This parameter MUST be specified because the default value wont be interpreted by the embedded test."],
                ]
    @@RESULTS                   ="Failed or successed."
    @@EXAMPLE                   ="puts Proto_Auto_Attach.new( {:start=>1} ).process <br /> "
    @@AUTHOR                    ="jba"
      
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
    
   
    #####################################
	#			Public section			#
	#####################################
    public
      
    def initialize(params)
       
        super(( params[:timeout] || @@DEFAULT_EXECUTION_TIMEOUT ),"PROTO_AUTO_ATTACH")
        
        # If the key :start is defined :
        if params.has_key? :start 
            wStart params[:start] 
        else
            wStart @@DEFAULT_START
        end
                             
    end
      
    def changeParams( params )
        
        # If the key :start is defined, update this field.
        wStart params[:start]  if params.has_key? :start
          
    end
    
    def process
        start
        results
    end
    
    def results
        super
        # If no error during the call start/end process occured, let's check the stack answer.
        @attach_log=''
        @attach_success=true
		if @results_available
            case @start
                when 0
                when 1
                else
                    # This case shall not happen since @results_available will be false...
                    @attach_log << "<font color='red'> Test parameter :start must be 0 or 1 and not #{@start}</font>"    
            end
           
            
        else
            # Error on the @start parameters !
            # An GTES_TEST_ERR_BAD_PARAMETERS/GTES_TEST_ERR_PARAMATERS_MISSING happened.
            @attach_log << "<font color='red'> Test parameter :start must be 0 or 1 and not #{@start}</font>"    
        end
        
        
        return @html_report
    end
    
end
