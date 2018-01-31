require "UnitTest"

class Board_Manual_HelloWorld < UnitTest
    
    #####################################
	#		DEFAULT PARAMETERS		    #
	#####################################
    
    @@DEFAULT_EXECUTION_TIMEOUT = 5
    @@DEFAULT_NUMBEROFPRINTEDHELLOWORLD = 1
    
    #####################################
	#		TEST DESCRIPTION			#
	#####################################
    
    @@TEST_NAME                 = "HelloWorld";
    @@VERSION                   = "1.0";
    @@ENVIRONEMENT              = "BOARD";
    @@TYPE                      = "MANUAL";
    @@PREREQUISITE              = "NONE";
    @@COVERAGE              = "Print numberOfprintedHelloWorld 'HelloWord' on LCD screen. Check board LCD connection and font manager driver"
    @@PARAMETERS=[
                    ["timeout","#{@@DEFAULT_EXECUTION_TIMEOUT}s","Timeout for the test execution"],                 
                    ["numberOfprintedHelloWorld","#{@@DEFAULT_NUMBEROFPRINTEDHELLOWORLD}","Number of 'Hello World' print on the LCD"]   
                ]
    @@RESULTS                   ="The user is asked to check whether the expected number of 'Hello World' has been printed"
    @@EXAMPLE                   ="puts Board_Manual_HelloWorld.new( {:numberOfprintedHelloWorld=>5} ).process . The test try to print five 'Hello World' string on the LCD" 
    @@AUTHOR                    ="Matthieu"
    
    TestsHelp.addTestHelpEntry(     @@TEST_NAME,
                                    @@VERSION,    
                                    @@ENVIRONEMENT,   
                                    @@TYPE,   
                                    @@PREREQUISITE,
                                    @@PARAMETERS,
                                    @@COVERAGE,
                                    @@RESULTS,
                                    @@EXAMPLE,  
                                    @@AUTHOR  
                            )
    
    #####################################
	#		TEST DESCRIPTION END		#
	#####################################
    
    def initialize(params)
        super(params[:timeout] || @@DEFAULT_EXECUTION_TIMEOUT ,"BOARD_MANUAL_HELLOWORLD")
        @params.numberOfprintedHelloWorld.w(params[:numberHelloWorld] || @@DEFAULT_NUMBEROFPRINTEDHELLOWORLD )
    end
      
    def changeParams( params )
        @params.numberOfprintedHelloWorld.w @params.numberOfprintedHelloWorld
    end
    
    def process
        start
        results
    end
    
    def results
        super
        if @results_available
            puts "Do you see %d times the string 'Hello World !' on the LCD ? (y/n)" % @params.numberOfprintedHelloWorld.R
            entry= gets
            if (entry.downcase=="y")
                @html_report << "Test Succeeded."
            else
                @html_report<< "Test Failed."
            end
        end
        return @html_report
    end

end
