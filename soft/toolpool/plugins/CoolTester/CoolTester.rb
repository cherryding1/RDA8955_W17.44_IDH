require "timeout"
require "rubygems"
require "markaby" 
require "CoolTester/dump_in_html"
require "help.rb"

begin
    include CoolHost
rescue
end

module CoolTester
		
	#####################################
	#			Module constants	    #
	#####################################
	
	# CoolTester Version.
	SCRIPT_VERSION = "CoolTester version : 0.1\n"
    
    # Timeout ( in second ) after what we consider GTES has not processed the CoolTester start test command.
    TIMEOUT_CMD = 4
	
	# GTES globals from gtes.xmd .
	@@GTES_globals = CH__gtes_globals
	
	# GTES globals from gtes_config_tests.xmd .
	@@GTES_tests_globals = CH__gtes_tests_globals
	
	# GTES stack globals msg from gtes_stack_msg.xmd .
	@@GTES_stack_globals = CH__gtes_stack_globals
	
	# Hash from gtes_config_tests.xmd with the paire {key,value}={testName,testValue}.
	@@GTES_XML_tests_id_hash={}
	
	# Hash from gtes_config_tests.xmd with the paire {key,value}={ErrorName,ErrorValue}.
	@@GTES_XML_tests_err_hash={}
	
	# Hash from a specific .lod with the paire {key,value}={testName,index g_gtesTestList}.
	@@GTES_available_tests_hash={}
	
	# Hash from gtes_stack_msg.xmd with the paire {key,value}={MsgName,MsgValue}.
	@@GTES_XML_stack_msg_hash={}
		
	# Array listing CC errors. The array index is the corresponding error value.
	@@GTES_XML_cc_error_array=[
                                "NONE",
                                "NORMAL_RELEASE",
	                            "LOWER_LAYER_FAILURE",
	                            "REQUEST_IMPOSSIBLE",
	                            "INCOMING_CALL_BARRED",
	                            "OUTGOING_CALL_BARRED",
	                            "CALL_HOLD_REJECTED",
	                            "CALL_RETRIEVE_REJECTED",
	                            "CC_TIMER_EXPIRY",
	                            "CC_ERROR_PROTOCOL_ERROR"
                            ]
			
	# Global variable : CoolTester specific comH.
	$coolTesterCOMH
	
	# Global variable : Specifies if CoolTester has been initialized or not.
	$coolTesterInit=false
	
	#####################################
	#			CT Error Classes        #
	#####################################
	
	# Generic CoolTester class error.
	class CT_Err < StandardError
	    #include CoolTester
	end
	
	class CT_Err_Unknown_Test < CT_Err
        attr:details
        
        def initialize
            if $coolTesterInit==false
                @details = "<font color='red'>Error : CoolTester has not been initialized !</font> <br />"
                @details += "<font color='red'>Please do it executing CoolTester.init method. </font> <br />"   
            else
                @details = "<font color='red'>This test does not exist in gtes_config.xmd. !</font> <br />"
            end
        end
                       
	end
	
	class CT_Err_Unavailable_Test < CT_Err
        attr:details
        
        def initialize
            @details = "<font color='red'>Error : This test is not available in the current .lod !</font> <br />"
            @details +="You may have forgotten to register your test. <br />"
            @details +="Perhaps you made a mistake registering it... <br />"    
        end       
                
	end
	
	class CT_Err_Unknown_Parameters_Structure < CT_Err
        attr:details
        
        def initialize(testName)             
            @details = "<font color='#FF6600'>Warning : GTES_TEST_PARAMS_#{testName} has not been defined in your test .xmd ! </font> <br />"
            @details +="<font color='#FF6600'> Are you sure your test does not need a parameter structure ?"    
        end       
                
	end

    class CT_Err_Unknown_Results_Structure < CT_Err
        attr:details
        
        def initialize(testName)             
            @details = "<font color='#FF6600'>Warning : GTES_TEST_RESULTS_#{testName} has not been defined in your test .xmd ! </font> <br />"
            @details +="<font color='#FF6600'> Are you sure your test does not need a results structure ?"    
        end       
                
	end
		
	# Exception raised when CoolTester is expecting an event from GTES, and received a bad one.
	class CT_Err_GTES_events < CT_Err
		attr:receivedEvent,:expectedEvent
		
		def initialize(receivedEvent, expectedEvent)
			@@receivedEvent=receivedEvent
			@@expectedEvent=expectedEvent
		end
		
		def detail
		    html=""
		    html+=Markaby::Builder.new.capture do
    		    p(:style=>"color:red") do 
    		        self<< "CT_Err_GTES_events exception."
    		        br
    		        # An GTES event is described by its 16 MSB : They are equal to GTES_MAGIC_NUMBER.
    		        if ( ( @@receivedEvent >> 16 ) == @@GTES_globals::GTES_MAGIC_NUMBER )
    		            self<< "Received event is a GTES one but not the expected one."
    		            br
    		        else
    		            self<< "Received event is NOT a GTES one !"
    		            br
    		        end
    		        self<<"Received event is 0x%x whereas expected one was 0x%x.\n" % [@@receivedEvent,@@expectedEvent]
    		    end
    		end
		    return html
		end
	end
	
	# Exception raised when CoolTester has expected an event for more time than a specified timeout. 
	class CT_Err_Timeout < CT_Err
	    attr:testBegun,:testDone
	    
	    def initialize(testBegun,testDone )
			@@testBegun=testBegun
			@@testDone=testDone
		end
	    
	    def detail
	        html="" 
		    html+=Markaby::Builder.new.capture do
	            p(:style=>"color:red") do 
	                self<<"CT_Err_Timeout exception."
	                br
	                if (@@testBegun ==false )
		                self<<"The test has not begun !"
		            else
		                if ( @@testDone==false )
		                    self<<"The test has not finished with its specified timeout execution time !"
		                else
		                    self<<"This exeception should have not been raising...it's a bug"
		                end
		            end
		        end
		    end
		    return html
	    end
	end
	
	# Exception not used for the moment...
	class CT_Err_Start_Test < CT_Err
	    attr:err_explanation
	    
	    def initialize ( detail )
	        @err_explanation=detail
	    end
	
	end
	
	class CT_Err_InstanceFailed < CT_Err
	
	end
	
	
	
	#####################################
    #	    CT Error Classes End        #
	#####################################
	
        	#####################################
        	#			Module Methods   		#
        	#####################################
		
	#####################################
	#			Private section			#
	#####################################
	
	private
	
	# This method fills the two following hashes from gtes_config_tests.xml:
	# -> @@GTES_XML_tests_id_hash
	# -> @@GTES_XML_tests_err_hash
    def CoolTester.fillHashesFromXML
        
        @@GTES_XML_tests_id_hash.clear
        @@GTES_XML_tests_err_hash.clear
        
        @@GTES_tests_globals.constants.each do |constant|
            case constant
                when /GTES_TEST_ID*/
                    testName=constant.gsub(/GTES_TEST_ID_/,"")
                    # We discard GTES_TEST_ID_NONE and GTES_TEST_ID_QTY
                    if testName!="NONE" && testName!="QTY"
                        @@GTES_XML_tests_id_hash[testName]= eval("@@GTES_tests_globals::" + constant)
                    end 
                when /GTES_TEST_ERR*/
                    @@GTES_XML_tests_err_hash[constant.gsub(/GTES_TEST_ERR_/,"")]=eval("@@GTES_tests_globals::" + constant)
                
                else
                    #TO DO : RAISE AN EXECPTION de type PARSING ERROR
                    #puts "Unexpected kind of constant in the XML"
            end
         end         
    end
    
    def CoolTester.fillStackHashFromXML
        
        @@GTES_XML_stack_msg_hash.clear
       
        
        @@GTES_stack_globals.constants.each do |constant|
            case constant
                when /GTES_API*/
                   @@GTES_XML_stack_msg_hash[constant.gsub(/GTES_/,"")]= eval("@@GTES_stack_globals::" + constant)          
                else
                    # do nothing.
                   
            end
         end         
    end
    
        
    # This method fills, inspecting the .lod embedded in the chip the @@GTES_available_tests_hash hash.
    def CoolTester.fillAvailableTestsHashFromChip
        index=0
		
		# We remove all previous pair (key,value) from the available test hash.
		# It's typically necessary when loading a new .lod in the chip with new tests.
		@@GTES_available_tests_hash.clear
		
		# Look Through the embedded test table while it find an Id different from the initialization value.
		# If we find one, it means the ID's test is available.
		while( (currentIdValue=rTestId(index) ) != @@GTES_tests_globals::GTES_TEST_ID_NONE )
                 
		    # We store the testName in currentTest, found thanks to its id value.
		    currentTest= @@GTES_XML_tests_id_hash.index(currentIdValue)
		    
		    # Store a new pair {testName,EmbeddedIndex}.
		    @@GTES_available_tests_hash[currentTest]=index
		    
		    index+=1
		end 		
    end
	
	# This method converts an specified hash into an html table. 
	def CoolTester.printTable(which)
	    case which
	        when "XML tests id"
	            hash=@@GTES_XML_tests_id_hash
	            header1="Test name"
	            header2="Test value"
	            title="<b>"+ "All known tests from gtes_config.xmd :" + "</b>"
	        when "XML tests err"
	            hash=@@GTES_XML_tests_err_hash
	            header1="Error name"
	            header2="Error value"
	            title="<b>"+ "All known tests errors from gtes_config.xmd :" + "</b>"
	        when "available tests"
	            hash= @@GTES_available_tests_hash
	            header1="Test available"
	            header2="Index position in g_gtesTestsList"
	            title="<b>"+ "Available tests in current .lod :" + "</b>"
	        else
	            return ""
	    end
	    ret_html=Markaby::Builder.new.capture do
	        #center do
    	        self<< title
    	        table(:border=>"1",:frame=>"all") do
        	        # WARNING : Caption are not interpreted by Qt...=> self<< title is used instead.
        	        #caption title
        	        tr do
            	            th(:align=>"center") {" #{header1}"}
            	            th(:align=>"center") {" #{header2}"}
            	    end
        	        hash.each do |key,value|
    	                tr do
        	                td(:align=>"center") {key}
        	                td(:align=>"center") {"#{value}"}
        	            end
    	            end  
    	        end 
    	    #end
	    end
	    
	    return ret_html
	end
	
	#####################################
	#			Read Methods   		    #
	#####################################
		
	# Return embedded major GTES version.
	def CoolTester.getMajorVersion
		($g_gtesRemoteStructure.gtesVersion.R & 0x0000FF00) >> 8
	end

	# Return embedded minor GTES version.
	def CoolTester.getMinorVersion
		($g_gtesRemoteStructure.gtesVersion.R & 0x000000FF) 
	end

	# Return value stored at the GTES magic number address.
	def CoolTester.getMagicNumber
		$g_gtesRemoteStructure.gtesVersion.R >> 16
	end
	
	def CoolTester.rMainStatus
		$g_gtesRemoteStructure.gtesMainStatus.R 
	end

	def CoolTester.rTestStatus
		$g_gtesRemoteStructure.gtesTestStatus.R 
	end

	def CoolTester.rTestErr
		$g_gtesRemoteStructure.gtesTestErr.R 
	end
	
	def CoolTester.rApiToMmiMsg
		$g_gtesRemoteStructure.gtesStackStatus.>.lastApiToMmiMsg.R
	end
	
	def CoolTester.rCcError
		$g_gtesRemoteStructure.gtesStackStatus.>.lastCcError.R
	end
	
	def CoolTester.rCallStatus
		$g_gtesRemoteStructure.gtesStackStatus.>.callStatus.R
	end
	
	def CoolTester.rSmsStatus
		$g_gtesRemoteStructure.gtesStackStatus.>.smsStatus.R
	end
	
	def CoolTester.rTestId(index)
	    $g_gtesRemoteStructure.gtesTestsList.>[index].testId.R
	end

	def CoolTester.rTestParamsPtr(index)
	    $g_gtesRemoteStructure.gtesTestsList.>[index].testParams.R
	end
	
	def CoolTester.rTestResultsPtr(index)
	    $g_gtesRemoteStructure.gtesTestsList.>[index].testResults.R
	end
	
	#####################################
	#			Read Methods end  		#
	#####################################
	
	
	#####################################
	#			Write Methods   		#
	#####################################
	
	def CoolTester.wCmd(cmd)
		$g_gtesRemoteStructure.coolTesterCmd.w cmd
	end

	def CoolTester.wParam(param)
		$g_gtesRemoteStructure.coolTesterParam.w param
	end
	
	#####################################
	#			Write Methods end 		#
	#####################################
	
		
	#####################################
    #	       CHST methods     	    #
	#####################################
	
    # Open a specific connection on CoolHost to get GTES events.
    # Thus, events won't be freed by EventSniffer or others...
    # The connection is stored in $coolTesterCOMH.
    def CoolTester.openConnection
        $coolTesterCOMH=CHHostConnection.new($CURRENTCONNECTION.comport)
        $coolTesterCOMH.open(false)
    end
    
    # Enable host events received on $coolTesterCOMH.
    def CoolTester.enableEvents
        $coolTesterCOMH.enableEvents(true)
    end
    
    # Disable host events received on $coolTesterCOMH.
    def CoolTester.disableEvents
	    $coolTesterCOMH.enableEvents(false)    
    end
	
	# Flush the $coolTesterCOMH events in its FIFO.
	def CoolTester.flushFifo
	    $coolTesterCOMH.flushEvents()
	end
	
	# Check if an event is in the FIFO until timeout is reached. 
	# If not, raise a NoEventError.
	def CoolTester.popEvent(timeout)
	    $coolTesterCOMH.popEvent(timeout)
	end
	
	# Close the opened connection.
	def CoolTester.closeConnection
	    $coolTesterCOMH.close
	end
	
	# Wait expected_event on comH until timeout is reached. 
	# Return true if the expected event has been poped. Otherwise raise an exception.
	# Could propagate two kind of exceptions :
	# -> CT_Err_GTES_events, if we poped an event but not the expected one.
	# -> Timeout::Error, if none event has been poped during timeout.
	def CoolTester.waitEvent(expected_event,timeout)
	        
	        Timeout::timeout(timeout) do
			    while(true)
			        begin
    			        # Check if an event is in the FIFO. If not raise a NoEventError imediately since timeout=0.
    			        poped_event = CoolTester.popEvent(0) 
    			        
    			        # This code is executed only if an event has been poped.
    			        # Check if it's the expected event.
    			        if ( poped_event == expected_event) 
    				       return true
    				    # Uncomment if you want to handle exactly one event.
    				    #else 
    			            # We poped an event but not the expected one ! Propagate an exception.
    			            #raise CT_Err_GTES_events.new(poped_event,expected_event),"unexpected GTES event"
    			        end
    			    # Rescue the NoEventError raised by CHS_PopEvent each time there is no event in the FIFO.
			        rescue NoEventError
			            # Reschedule.
			            sleep(0.01) 
			        end
			    end
			end      
	end
	
	#####################################
    #	       CHST methods end    	    #
	#####################################
	
	# Compare GTES magic number with the value stored at the GTES magic number address.
	# Returned true if an GTES is embedded in the .lod. Otherwise, returns false.
	def CoolTester.checkGtesPresence
		gtesPresent=false		
		begin
    		# Wait 5 s if the chip has just been restarted.
    		Timeout::timeout(5) do
        		while(gtesPresent==false)   
    	            gtesPresent=true if getMagicNumber == @@GTES_globals::GTES_MAGIC_NUMBER
    	        end
	        end
       rescue Timeout::Error
       end  
       gtesPresent               		
	end
	
	# Get informations about the embedded .lod.
	# Analyze if the .lod holds a GTES. In this case, check its version 
	# and the available tests ( fill the available tests hash).
	# Return all this informations under html format.
	def CoolTester.checkEmbeddedLodFile
	
	     html=Markaby::Builder.new.capture do
	        if CoolTester.checkGtesPresence
			    self<< "GTES is embedded in this .lod ."
			    br
			    self<<("Embedded version : %01d.%01d.\n" % [CoolTester.getMajorVersion,CoolTester.getMinorVersion])
		        br
		        br
		        CoolTester.fillAvailableTestsHashFromChip
		        self<< (CoolTester.printTable "available tests")
		        
		        begin
        		    # Wait 5 s if the chip has just been restarted.
        		    # Avoid getting wrong informations !
            		Timeout::timeout(5) do
                		while(CoolTester.rMainStatus!=@@GTES_globals::GTES_MAIN_STATUS_READY)            	            
            	        end
        	       end
                rescue Timeout::Error
                end                 		
		        
                case CoolTester.rMainStatus
		            when @@GTES_globals::GTES_MAIN_STATUS_READY
		                if CoolTester.gtesIsAlive?
		                    self<<  "<font color='darkgreen'>GTES is ready to process your test</font>"
		                    br
	                    end
		            when @@GTES_globals::GTES_MAIN_STATUS_BUSY
	                    self<<  "<font color='red'>GTES is busy...strange, report this problem</font>"  
		            when @@GTES_globals::GTES_MAIN_STATUS_NOT_INIT
		                self<<  "<font color='red'>GTES has not been initialized ! Please call gtes_Init() in your main</font>"             
		            when @@GTES_globals::GTES_MAIN_STATUS_REGISTER_ERR
		                self<<  "<font color='red'>A problem occured when registering a test in GTES</font>"    
		            else
		                self<<  "<font color='red'>Oups, unknown gtes main status...report this problem !</font>"     
		        end
		    else 
		    self<< "<font color='red'>Warning : GTES is NOT embedded in this .lod .</font>"
		    br
	        end
	     end
		return html
	end 
	
	# This methods returns true if testId is known in gtes_config.xmd Returns false otherwise.
	def CoolTester.testKnownInXMD? ( testId )
	    return available=@@GTES_XML_tests_id_hash.has_key?(testId)
	end
	
	# This methods returns true if testId is available in the embedded .lod. Returns false otherwise.
	def CoolTester.testAvailable? ( testId )
	    return available=@@GTES_available_tests_hash.has_key?(testId)
	end
	
	# This methods analyse the g_gtesRemoteStructure.testStatus field.
	# Return an array [html, resultsAvailable ] with html-> analysis and resultsAvaible-> bool true if results are availble,
	# false otherwise.
	def CoolTester.testStatus
		html=""
		resultsAvailable=false
		case CoolTester.rTestStatus
    		when @@GTES_globals::GTES_TEST_STATUS_PROCESSING
    		    html+="<font color='red'>"+ "The test is still processing..."+"</font>"+"<br>"
    		when @@GTES_globals::GTES_TEST_STATUS_ID_NOT_PRECISED 
    			html+= "<font color='red'>"+"Test launching failed because the test ID was not precised."+"</font>"+ "<br>"
    		when @@GTES_globals::GTES_TEST_STATUS_UNKNOWN_ID
    		    html+= "<font color='red'>"+"Test launching failed because the test ID was not available in the chip." +"<font/>"+ "<br>"
    		when @@GTES_globals::GTES_TEST_STATUS_RESULTS_AVAILABLE
    			resultsAvailable=true
    			html+= "<font color='#008800'>"+"Tests results are available." +"</font>"+ "<br>"
    		when @@GTES_globals::GTES_TEST_STATUS_ERR
    		    err = @@GTES_XML_tests_err_hash.index(CoolTester.rTestErr)
    		    err = "Unknown error : #{CoolTester.rTestErr}" if(!err)
    			html+= "<font color='red'>"+"An error occured during test processing :"+"</font>"+ "<br>"
    			# We precise the error, reading the gtesTestErr field.
    			html+= "<font color='red'>"+err+"</font>"
    		else 
    			html+= "<font color='red'>"+"ALERT : The test status hold a unknown value !" +"</font>"+ "<br>"
		end
		return html,resultsAvailable
	end
	
	def CoolTester.waitCallStatus(timeout,expected_status)
	
	    # Return values :
	    # Boolean indicating whether the expected status has been reached or not.
	    status_reached = false 
	    # Call log with all API->MMI msgs received. Could be use for debug purpose.
	    msg_Api_log=""
	    
	    current_Api_msg=""
	    last_Api_msg=""
	    
	    begin
	        Timeout::timeout(timeout) do
	            msgId = CoolTester.rApiToMmiMsg
    	        current_Api_msg=@@GTES_XML_stack_msg_hash.index(msgId)
    	        if(!current_Api_msg)
    	            current_Api_msg = "UNKNOWN_MESSAGE_%d" % msgId
    	        end
    	        
    	        current_status=CoolTester.rCallStatus
    	        while (current_status != expected_status)

    	            # If a new API msg has been received, we store it in the call log.
    	            if current_Api_msg!=last_Api_msg
    	                msg_Api_log+= current_Api_msg + " received"   
    	                # If its an CC error, we identify it :
        	            if current_Api_msg=="API_CC_ERROR_IND"
    	                    msg_Api_log+= " : " + "<font color='red'>" + @@GTES_XML_cc_error_array[CoolTester.rCcError] + "</font>" 
    	                end
    	            msg_Api_log+="<br />"
    	            end
    	                            
                    # It becomes an old msg since it has been handled.
                    last_Api_msg=current_Api_msg
                    
                    # We store the new msg.
                    msgId = CoolTester.rApiToMmiMsg
                    current_Api_msg=@@GTES_XML_stack_msg_hash.index(msgId)
                    if(!current_Api_msg)
                        current_Api_msg = "UNKNOWN_MESSAGE_%d" % msgId
                    end  
                    
                    #Temper the while
                    sleep 0.01
                    
                    # We have not yet reached the expected call status.
                    # Let's store the new one in current_status.
                    current_status=CoolTester.rCallStatus    
                      
    	        end
	            # We have reached the expected call status before the timeout.
	            status_reached = true
	            return status_reached,msg_Api_log
	        end    
	    
	    rescue Timeout::Error
	        return status_reached,msg_Api_log
	    end
	    	
	end
	
	
	#####################################
	#			Public section			#
	#####################################
	
	public
    
        #####################################
    	#	 Test Report API section		#
    	#####################################
    
    def CoolTester.reportAddTitleParameters
        return Markaby::Builder.new.capture{ u {h3 "Test parameters"} }  
    end	
    
    def CoolTester.reportAddTitleInformations
        return Markaby::Builder.new.capture{ u {h3 "Test informations"} }  
    end	
    
    def CoolTester.reportAddTitleResults
        return Markaby::Builder.new.capture{ u {h3 "Test results"} }  
    end	
    
    def CoolTester.reportAddStartTime
        @@start_time=Time.new
        return Markaby::Builder.new.capture{ font(:color=>"#008800"){"Test started at %s"% @@start_time}; br }
    end
    
    def CoolTester.reportAddDoneTime
        done_time=Time.new
        test_duration=done_time-@@start_time
        return Markaby::Builder.new.capture{font(:color=>"#008800"){"Test finished at %s and lasted %s"% [done_time,test_duration]}; br }
    end 
    
    # Returns a "test paramaters" section under html format.
	# Dump the tests paramters before lauching a test.
	# Thus, in the report, the user could check the paramaters of a given test.
	def CoolTester.parametersToHtml(params,call=false)
	    html=""
	    html+=CoolTester.reportAddTitleParameters
		html+=params.dump_in_html(call)
		return html
	end
    
        #####################################
    	#	 Test processing section		#
    	#####################################
	
	def CoolTester.callEstablished?(timeout)
	    CoolTester.waitCallStatus(timeout,@@GTES_globals::GTES_CALL_STATUS_CONNECTED)
	end
	
	def CoolTester.callReleased?(timeout)
	    CoolTester.waitCallStatus(timeout,@@GTES_globals::GTES_CALL_STATUS_IDLE)
	end
    
	def CoolTester.callStatusIdle?(timeout)
	    CoolTester.waitCallStatus(timeout,@@GTES_globals::GTES_CALL_STATUS_IDLE)
	end
    
    def CoolTester.gtesIsAlive?
        
        alive=false
        
        # Open specific connection.
        CoolTester.openConnection
            
		# Enable host events.
		CoolTester.enableEvents
		
		# Flush events FIFO associated with the handle specified.		
		CoolTester.flushFifo	
        
        # Write GTES_CMD_IS_ALIVE in coolTesterCmd field.		
		wCmd(@@GTES_globals::GTES_CMD_IS_ALIVE)
        
        begin
            # Wait an event from GTES meaning the test is finished with a specific testTimeout for each test.
            alive = waitEvent(  @@GTES_globals::GTES_EVENT_ALIVE,
    	                        ((@@GTES_globals::GTES_TASK_POLLING_PERIOD)/16384.0)*5
                                )
        
        
        return alive
        
        rescue Timeout::Error
            # alive will be false.
            return alive
        ensure
		    if alive==true
                puts "CoolTester is alive"
            else
                puts "CoolTester is dead"
            end
		    	    
		    # Flush events FIFO associated with the handle specified.		
			CoolTester.flushFifo		
			# Disable host events whatever happens.
			CoolTester.disableEvents
			CoolTester.closeConnection
			
		end   
           
        
    end    
	
	addHelpEntry(   "CoolTester",
                    "CoolTester.init",
                    "",
                    "html_welcome_text",
                    "This method will init CoolTester core. The following things will be done : Checking CoolTester version; Parsing gtes_config.xmd to get tests names/errors which have been designed for CoolTester; Checking if GTES is embedded in the current .lod; Listing all the avalaible tests in this .lod; Adding in the watch window the remote structure of communication between CoolTester and GTES")	
    def CoolTester.init
        
        begin
            # Try to access to GTES through the modules map.
            $g_gtesRemoteStructure=$map_table.>.gtes_access.>
        rescue NullPointer
            welcomeText = "$map_table.>.gtes_access not found on target. Check that your code is compiled with GTES."
		    return welcomeText
		end    
              
		Markaby::Builder.set(:indent,2)
		CoolTester.fillStackHashFromXML
		# Add Qt tag to print html.
		welcomeText="html>"
		welcomeText+=Markaby::Builder.new.capture do
		    h3 "CoolTester Initialization"
    		p do 
        		self<< SCRIPT_VERSION
        		self<< "<br /> <br />"
        		# Fill tests informations in hashes from gtes_config.xmd .
        		CoolTester.fillHashesFromXML
        		# Print these informations.
        		self<< (CoolTester.printTable "XML tests id" ) + "<br>"
        		self<< (CoolTester.printTable "XML tests err") + "<br>"
        		begin	
            		# Add automatically in the watch window the Remote Structure.
                    cwAddWatch($g_gtesRemoteStructure);
                    
                    # Check the .lod file in the chip.
        		    self<< (CoolTester.checkEmbeddedLodFile + "<br>")
		        rescue Exception => e
		        	raise e    
		        end
            end 
		end
			
		$coolTesterInit=true
		welcomeText
	end

	# This methode instantiates the ruby parameters/results structure of test which name is testName.
	# Returns references to those structures.
	# This method could raise a CT_Err_InstanceFailed exception :
	def CoolTester.instanceParamsResults(testName,debug=false)
        begin
            # We first check whether the test IDentifier has been set in gtes_config.xmd.
            if testKnownInXMD?(testName)
                # Then, we check whether the test is load in the current .lod.
                if testAvailable?(testName)
                    # Get the parameters/results strcuture addresses.
                    addressParams=rTestParamsPtr(@@GTES_available_tests_hash[testName])
                    addressResults=rTestResultsPtr(@@GTES_available_tests_hash[testName])
                    
                    # Instantiate both structures.   
                    begin
                        params=eval("CH__GTES_TEST_PARAMS_"+testName).new(addressParams)
                        # Manual debug mode : Watch the parameters structure in the "Watch Window"
                        cwAddWatch(params) if debug==true
                    rescue NameError
                        # WARNING : the parameters structure of this test does not exist ! 
                        # The exception is not raised since it's possible a test does need any parameter !
                        puts "html>" + CT_Err_Unknown_Parameters_Structure.new(testName).details
                    end
                    
                    begin
                        results=eval("CH__GTES_TEST_RESULTS_"+testName).new(addressResults)
                        # Manual debug mode : Watch the results structure in the "Watch Window"
                        cwAddWatch(results) if debug==true
                    rescue NameError
                       # WARNING : the results structure of this test does not exist ! 
                       # The exception is not raised since it's possible a test does need any result !
                       puts "html>" + CT_Err_Unknown_Results_Structure.new(testName).details
                    end
                                                  	   
                    return params, results
                    
        	    else 
        	        # This test is not available in the current .lod.
        	        raise CT_Err_Unavailable_Test.new
        	    end
    		
    		else
    		    # This test is not known in the .xmd.
    		    raise CT_Err_Unknown_Test.new
    		end
		
		rescue CT_Err_Unknown_Test => exception
		    
		    # This test is not known in the .xmd.
		    puts "html>" + exception.details
		    # Propagate a new exception to the upper caller, to indicate the method failed.
		    raise CT_Err_InstanceFailed.new
		    
		rescue CT_Err_Unavailable_Test => exception
		
		    # This test is not available in the current .lod.
		    puts "html>" + exception.details	
	    	# Propagate a new exception to the upper caller, to indicate the method failed.
		    raise CT_Err_InstanceFailed.new
		end
		
	end

        #####################################
    	#	 Debug embedded test section	#
    	#####################################
	
	addHelpEntry(   "CoolTester",
                    "CoolTester.debugEmbeddedTest",
                    "testName",
                    "paramsStructure,resultsStructure",
                    "This method will help you debug your test which name is testName. It will check whether your test ID has well been set in gtes_config.xmd, if your test has well been registered etc... A parameters structure and a results one will be created and added in the 'Watch window'.Thus, you can change easily your test parameters, and analyze your test results. Example : CoolTester.debugEmbeddedTest('UART')")
	def CoolTester.debugEmbeddedTest(testName)
	    # Init CoolTester if it has not been done yet. 
	    CoolTester.init if $coolTesterInit==false
	    CoolTester.instanceParamsResults(testName,true)     
    end
		
	addHelpEntry(   "CoolTester",
                    "CoolTester.debugStartTest",
                    "testName,testTimeout",
                    "",
                    "")
	def CoolTester.debugStartTest(testName,testTimeout)
	    
	    # Init CoolTester if it has not been done yet. 
	    CoolTester.init if $coolTesterInit==false
	    	    	    
	    CoolTester.openConnection
            
		# Enable host events.
		CoolTester.enableEvents
		
		# Flush events FIFO associated with the handle specified.		
		CoolTester.flushFifo	
		
		begin
    		# Write test ID in coolTesterParam field.		
    		wParam(@@GTES_XML_tests_id_hash[testName])
    		
    		# Write GTES_CMD_START_TEST in coolTesterCmd field.		
    		wCmd(@@GTES_globals::GTES_CMD_START_TEST)
    	    
    	    begin
    	        # Wait an event from GTES meaning the test begins with a timeout of TIMEOUT_CMD.		
    		    waitEvent(@@GTES_globals::GTES_EVENT_TEST_STARTED,TIMEOUT_CMD)
    		rescue Timeout::Error
    		    raise CT_Err_Timeout.new(false,false),"Timeout"
    		end
    		
    		puts "Your test begins"
    		
    		begin
        		# Wait an event from GTES meaning the test is finished with a specific testTimeout for each test.
        		testdone=waitEvent(@@GTES_globals::GTES_EVENT_TEST_FINISHED,testTimeout)
		    rescue Timeout::Error
    		    raise CT_Err_Timeout.new(true,false),"Timeout"
    		end
		    
		    puts "Your test is finished. Please check your results structure in the watch window"
		    
		rescue CT_Err_Timeout => timeout
		    puts "html>" + timeout.detail
		
		ensure
		    # Flush events FIFO associated with the handle specified.		
			CoolTester.flushFifo		
			
			# Disable host events whatever happens.
			CoolTester.disableEvents
			
			CoolTester.closeConnection
		end   
	end
	    
	    #####################################
    	# Debug embedded test section end	#
    	#####################################
		
	#addHelpEntry("Cooltester","startTest","testId,testTimeout","","Starts the test")
	def CoolTester.startTest ( testID, testTimeout, params)
		
		# Boolean which indicates whether the test has begun or not.
		testbegun=false
		# Boolean which indicates whether the test has finished or not.
		testdone=false
		# Return value [ html_report, resultsAvailable(true|false) ].
		resultsAvailable=[]
		# Chunk of report, which will be return.
		html="html>"
		
		# Let's check gtesMainStatus.
		# To do !
				
		call=true if testID=="PROTO_AUTO_CALL"
		html+=CoolTester.parametersToHtml(params,call)
	
		html+=CoolTester.reportAddTitleInformations
			
		begin
		    
            # Open a specific connection.
            CoolTester.openConnection
            
			# Enable host events.
			CoolTester.enableEvents
			
			# Flush events FIFO associated with the handle specified.		
			CoolTester.flushFifo	
			
			# Write test ID in coolTesterParam field.		
			wParam(@@GTES_XML_tests_id_hash[testID])
			
			# Write GTES_CMD_START_TEST in coolTesterCmd field.		
			wCmd(@@GTES_globals::GTES_CMD_START_TEST)
		
			# Wait an event from GTES meaning the test begins with a timeout of TIMEOUT_CMD.		
			testbegun=waitEvent(@@GTES_globals::GTES_EVENT_TEST_STARTED,TIMEOUT_CMD)
			
			# Get test start time informations.
			html+= CoolTester.reportAddStartTime
			
			# Wait an event from GTES meaning the test is finished with a specific testTimeout for each test.
			testdone=waitEvent(@@GTES_globals::GTES_EVENT_TEST_FINISHED,testTimeout)
			
			# Get test stop time informations.
			html+=CoolTester.reportAddDoneTime
			
			resultsAvailable=CoolTester.testStatus
			# Add to the report details about the testStatus.
			html+= resultsAvailable[0]
			
		    return html,resultsAvailable[1]
					
		rescue Timeout::Error
		    # The GTES start event OR the GTES stop event has not been received !
		    # Propagate a new exception which contains which event has not been received.
		    begin
		    raise CT_Err_Timeout.new(testbegun,testdone),"Timeout"
		    # Rescue it now.
		    rescue CT_Err_Timeout => exception
		       html+= exception.detail
		       # Results can't be available, return false.
		       return html,false
		    end
		ensure
			# Whatever happens do the following things :
			# Flush events FIFO associated with the handle specified.		
			CoolTester.flushFifo		
			# Disable host events whatever happens.
			CoolTester.disableEvents
		    # Close the connection.	
			CoolTester.closeConnection			
		end
	
	end
		      	
end
