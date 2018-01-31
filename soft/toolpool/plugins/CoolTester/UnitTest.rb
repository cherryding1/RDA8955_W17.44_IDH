require 'rubygems'
require 'markaby'
require 'CoolTester/CoolTester.rb'
require 'CoolTester/TestsHelp.rb'

class UnitTest
    include CoolTester
    @testName
    @params
    @results
    @timeout
    @results_available
    @html_report
    @results_report  
    @test_succeeded
    @init_succeeded=false
    
    def initialize(timeout,testName)
        @testName=testName
        @timeout=timeout
        @test_succeeded=false
        begin
            @params, @results = CoolTester.instanceParamsResults(@testName)
            @init_succeeded=true
        rescue CT_Err_InstanceFailed
            puts "html>"+"<font color='red'>"+"Initialization failed !"+"</font>"
            @init_succeeded=false 
        end
        
    end
    
    def initializeReport
        @html_report="html>"
        @results_report=""  
    end
    
    def changeParams
    end
    
    def start
    	initializeReport
    	if @init_succeeded==true
    	    # Let's start the test.
    	    @html_report,@results_available=CoolTester.startTest(@testName, @timeout, @params)
    	else
    	    @html_report << "<font color='red'>"+"Can not start the test : its initialization failed !"+"</font>"
    	    @results_available=false
    	end
    end
    
    def results
        if @init_succeeded==true
            @html_report<< CoolTester.reportAddTitleResults
		    @html_report<< "<font color='red'>"+"Tests results are not available <br>"+"</font>" if  @results_available==false      
	    else
	        # The initialization failed, the test has not been started.
	    end
    end
    
    def appendToReport
        @html_report
    end
    
   
end
