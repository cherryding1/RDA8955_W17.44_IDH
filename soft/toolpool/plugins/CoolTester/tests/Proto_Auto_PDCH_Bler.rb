require "UnitTest"
require 'gpib.rb'
require 'cmu.rb'
require 'abfs.rb'

require 'Proto_Auto_Attach.rb'

class BLERUserOptions
        attr_accessor :band, :arfcn, :freq, :cs, :do_static, :do_tu50, :do_ra250, :do_ht100, :rlc_blocks, :nb_slots
end

class Proto_Auto_PDCH_Bler < UnitTest
    
    #####################################
	#		DEFAULT PARAMETERS          #
	#####################################
    
    @@DEFAULT_BAND = 2
    @@DEFAULT_ARFCN = 40
    @@DEFAULT_CS = 1
    @@DEFAULT_STATIC = false
    @@DEFAULT_TU50 = false
    @@DEFAULT_RA250 = false
    @@DEFAULT_HT100 = false
    @@DEFAULT_RLC_BLOCKS= 1000
    @@DEFAULT_NB_SLOTS= 1
    
    #####################################
	#		TEST DESCRIPTION			#
	#####################################
        
    @@TEST_NAME                 = "Proto Auto PDCH BLER Test"
    @@VERSION                   = "1.0"
    @@ENVIRONEMENT              = "PROTO"
    @@TYPE                      = "AUTO"
    @@PREREQUISITE              = "Must be used with a .lod including GTES and with the Proto Auto Attach test previously registerd. This test must be run on a PC connected to CMU and Fading simulator via GPIB. Obviously, the CMU and the Fading simulator must be on before running the test( Unless you run it only in static conditions). Furthermore, you be sure you use the specfic BER SIM CARD when running this test. This test is based on cmu.rb, gpib.rb and abfs.rb module."
    @@PARAMETERS=[
        ["band","#{@@DEFAULT_BAND}","Band mode : 1 is GSM850, 2 is GSM900, 2 is DCS and 3 is PCS"],
        ["arfcn","#{@@DEFAULT_ARFCN}","Arfcn number"],
        ["cs","#{@@DEFAULT_CS}","Coding Scheme : 1 is CS1, 2 is CS2, 3 is CS3 and 4 is CS4."],
        ["bool_static","#{@@DEFAULT_STATIC}","Run the BER in static conditions"],
        ["bool_tu50","#{@@DEFAULT_TU50}","Run the BER in Typical Urban conditions at 50 kilometers"],
        ["bool_ra250","#{@@DEFAULT_RA250}","Run the BER in Rural Area conditions at 250 kilometers ( TGV conditions)"],
        ["bool_ht100","#{@@DEFAULT_HT100}","Run the BER in Hilly Terrain conditions at 100 kilometers"],  
        ["rlc_blocks","#{@@DEFAULT_RLC_BLOCKS}","Number of RLC blocks that will be processed"],
	    ["nb_slots","#{@@DEFAULT_NB_SLOTS}","Number of used Rx time slots"]
                ]   
    
    @@DESCRIPTION               = "Run a typical BLER test with the specified options, in a fully automatic way. The CMU and the fading simulator will behave in 'remote control' during the test. The CMU will wait phone signalling. In the same time, the phone will be restarted automatically. Then CoolTester will be intialized. Once the CMU have got the signalling signal from MS, it waits for MS synchronization and attachement. Next, the CMU intiates a TBF dowlink. If it sucessed, the required BER will be processed in order : static, tu50, ra250, ht100. "
       
    @@RESULTS                   =" Test Conditions : ... <br><ul class= 'parameter_ul'><li>Number of RLC blokcs : ...</li><li>Block error rate : ...% [limit ...%]</li><li><br> : PASS or FAILED</li></ul>"
    
    @@EXAMPLE                   ="puts Proto_Auto_PDCH_Bler.new( {:bool_static=>false,bool_tu50=>false} ).process <br / > We ask to do a BLER on #{@@DEFAULT_BAND} band, on arfcn #{@@DEFAULT_ARFCN}, in CS1, with #{@@DEFAULT_RLC_BLOCKS} rlc blocks on one Rx slot, strictly in RA250 and HT100 conditions."
    
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
    
    
    
      
    #####################################
    #           Private section         #
    #####################################
    
    private
        
        #####################################
        #           Constants section       #
        #####################################
    
    
    CMU_CS           = ["CS1","CS2","CS3","CS4"]


    PDCH_EXT_IN_LOSS         = "0.5"
    PDCH_EXT_OUT_LOSS_STATIC = "0.5"
    PDCH_EXT_OUT_LOSS_TU50   = "8.5"
    PDCH_EXT_OUT_LOSS_RA250  = "10.4"
    PDCH_EXT_OUT_LOSS_HT100  = "8.8"     
    
    PDCH_BCCH_CHANNEL    = [0,162,32,600,600]
    PDCH_BCCH_LEVEL      = 90
    PDCH_BCCH_TN         = 3
    
    PDCH_TCH_TN          = 3
    PDCH_TCH_PCL         = 5 
    
    PDCH_MODE_STATIC = 0
    PDCH_MODE_TU50   = 1
    PDCH_MODE_RA250  = 2
    PDCH_MODE_HT100  = 3 
    
    BLERCSStr    = ["CS1","CS2","CS3","CS4"]

    BLERBandStr      = ["???","GSM850","GSM900","DCS","PCS"]
    
    
    LevelLimit        =  [
    [   #CS1
        #Level
        104, #STATIC
        104, #TU 50 (no FH)
        104, #RA 250 (no FH)
        103, #HT 100 (no FH)
        104, #STATIC
        104, #PTU 50 (ideal FH)
        104, #PRA 130 (no FH)
        103 #PHT 100 (no FH)
    ],
    [   #CS2
        #Level
        104, #STATIC
        100, #TU 50 (no FH)
        101, #RA 250 (no FH)
         99, #HT 100 (no FH)
        104, #STATIC
        100, #PTU 50 (ideal FH)
        101, #PRA 130 (no FH)
        99  #PHT 100 (no FH)
    ],
    [   #CS3
        #Level
        104, #STATIC
        98,  #TU 50 (no FH)
        98,  #RA 250 (no FH)
        96,  #HT 100 (no FH)
        104, #STATIC
        98,  #PTU 50 (ideal FH)
        98,  #PRA 130 (no FH)
        94  #PHT 100 (no FH)
    ],
    [   #CS4
        #Level
        104, #STATIC
        90,  #TU 50 (no FH)
        0,   #RA 250 (no FH)
        0,   #HT 100 (no FH)
        101, #STATIC
        88,  #PTU 50 (ideal FH)
        0,   #PRA 130 (no FH)
        0   #PHT 100 (no FH)
    ]
    
]
       
    # Timeout options in seconds :
    TIMEOUT_SIGN_ON =  10
    TIMEOUT_TBF_ESTABLISHED = 15
    TIMEOUT_MS_ATTACHED = 15
 
        #####################################
        #           Variables section       #
        #####################################
    
    # Specific BER test instance/class variables added to the common Unit test instance/class variables.
    
    # GPIB handler.    
    @@BERHBASE= -1
    
    # Attach Object. 
    @attach
    
    # userOptions.
    @userOptions   
    
    
        #####################################
        #           Methods section         #
        #####################################
    
    # Write to CMU. 
    def BERW(str)
        GPIBwrt(@@BERHBASE,str)
    end
    
    # Read from CMU.
    def BERR()
        return GPIBrd(@@BERHBASE)
    end
    
    # Store and convert user test options in the @userOptions BLERUserOptions object.
    def BLERMakeOptions(band, arfcn, cs, bool_static, bool_tu50, bool_ra250, bool_ht100, rlc_blocks, nb_slots)       
             
        case band
            when 4
                @userOptions.band = 4
            when 3
                @userOptions.band = 3
            when 2
                @userOptions.band = 2
            else
                @userOptions.band = 1
        end
        
        @userOptions.arfcn = arfcn
        
        case @userOptions.band
            when 4
                @userOptions.freq = 18502.0 + 2*(@userOptions.arfcn-512) + 800
            when 3
                @userOptions.freq = 17102.0 + 2*(@userOptions.arfcn-512) + 950
            when 2
                @userOptions.freq = 8900.0 + 2*@userOptions.arfcn + 450
            else
                @userOptions.freq = 8242.0 + 2*(@userOptions.arfcn-128) + 450
        end
        
        case cs
            when 4
                @userOptions.cs = 3
            when 3
                @userOptions.cs = 2
            when 2
                @userOptions.cs = 1
            else
                @userOptions.cs = 0
        end
        
         @userOptions.nb_slots   = nb_slots
         @userOptions.do_static  = bool_static
         @userOptions.do_tu50    = bool_tu50
         @userOptions.do_ra250   = bool_ra250
         @userOptions.do_ht100   = bool_ht100
         @userOptions.rlc_blocks = rlc_blocks
        
   
    end
    
    # Add str to @html_report.
    def berputs(str)
	    @html_report<< str
        
        @logfile << str

    end
    
    # Add str to @results_report.
    def resultsputs(str)
        @results_report<< str
        @logfile << str
    end
    
    # Config Fading Simulator with a specific propagation model ( Typical Urban, Rural Area, Hilly Terrain).
    def BERSetFadingMode(mode,options)
    
        case mode
            when PDCH_MODE_STATIC
                # Set ABFS stat=OFF.
                ABFSW("FSIM:STAT OFF")
                
                # Set CMU IQIF to fading Path.
                CMUW("%d;CONF:IQIF:RXTX BYP" % options.band)
                
                # Set external attenuation
                CMUW("%d;SENS:CORR:LOSS:OUTP2 %s" % [options.band,PDCH_EXT_OUT_LOSS_STATIC])
            
            # Typical Urban mode at 50 kilometers : Urban driving context.      
            when PDCH_MODE_TU50
                # Set CMU external attenuation.
                CMUW("%d;SENS:CORR:LOSS:OUTP2 %s" % [options.band,PDCH_EXT_OUT_LOSS_TU50])
                
                # Set ABFS stat=ON.
                ABFSW("FSIM:STAT ON")
                
                #set ABFS RF Frequ
                ABFSW("FSIM:CHANNEL:RF %dE5" % options.freq)
                
                # Set Fading mode.
                if(options.band < 3) 
                    ABFSW("FSIM:STANDARD G6TU50")
                else    
                    ABFSW("FSIM:STANDARD P6TU50")
                end
                
                # ABFS wait for synchronism.
                ABFSW("*WAI")
                
                # Set CMU IQIF to fading Path.
                CMUW("%d;CONF:IQIF:RXTX FPAT" % options.band)
            
            # Rural Area mode at 250 kilometers : TGV context.                  
            when PDCH_MODE_RA250
                # Set external attenuation. 
                CMUW("%d;SENS:CORR:LOSS:OUTP2 %s" % [options.band,PDCH_EXT_OUT_LOSS_RA250])
                
                # Set ABFS stat=ON.
                ABFSW("FSIM:STAT ON")
                
                # Set ABFS RF Frequ.
                ABFSW("FSIM:CHANNEL:RF %dE5" % options.freq)
                
                # Set Fading mode.
                if(options.band < 3)
                    ABFSW("FSIM:STANDARD GRA250")
                else
                    ABFSW("FSIM:STANDARD PRA130")
                end
                
                # ABFS wait for synchronism. 
                ABFSW("*WAI")
                
                # Set CMU IQIF to fading Path.
                CMUW("%d;CONF:IQIF:RXTX FPAT" % options.band)
            
            # Hilly terrain at 100 kilometers : Mountain driving context.                   
            when PDCH_MODE_HT100
                # Set external attenuation.
                CMUW("%d;SENS:CORR:LOSS:OUTP2 %s" % [options.band, PDCH_EXT_OUT_LOSS_HT100])
                
                # Set ABFS stat=ON.
                ABFSW("FSIM:STAT ON")
                
                # Set ABFS RF Frequ.
                ABFSW("FSIM:CHANNEL:RF %dE5" % options.freq)
                
                # Set Fading mode.
                if(options.band < 3) 
                    ABFSW("FSIM:STANDARD G6HT100")
                else
                    ABFSW("FSIM:STANDARD P6HT100")
                end
                
                # ABFS wait for synchronism. 
                ABFSW("*WAI")
                
                # Set CMU IQIF to fading Path.
                CMUW("%d;CONF:IQIF:RXTX FPAT" % options.band)       
            else
        end
    end
    
    # Wait a specific signal from CMU, on a specific band, in a polling way and with a specified timeout.
    # Return a boolean : true if the expected signal has been received, false otherwise.
    # Typical use are the followings :
    # Wait for signalling ON, for MS synchronization, tbf established etc...
    # COULD BE INTEGRATED IN THE CMU MODULE.
    def WaitCMUSignal(signal,timeout,band)
            
        signal_received=false
        # Query the current signaling state
        CMUW("%d;SIGN:PDAT:STAT?" % band)
        rdbuf = CMUR()
        
        begin
            Timeout::timeout(timeout) do
                while(rdbuf.slice(0..(signal.length-1)) != signal)
                    # Query the current signaling state.
                    CMUW("%d;SIGN:PDAT:STAT?" % band)
                    rdbuf = CMUR()
                    sleep(0.01)
                end 
                signal_received=true
                return signal_received
            end
        
        rescue Timeout::Error
           return signal_received
        end 
    end
    
    # Restart the MS.
    # Wait for Signalling ON until TIMEOUT_SIGN_ON.
    # If received, wait for MS attachement until TIMEOUT_MS_ATTACHED.
    # If received, start a tbf.
    # If succeeded, wait for tbf establishement until TIMEOUT_TBF_ESTABLISHED.
    # Stores a log of all information in @html_report.
    # Return a boolean : true if the tbf has been established, false otherwise.
    def BLERWaitForMsSyncAndMsAttach(band)
        
        tbf_established=false
        
        # Switch on BCCH channel.
        CMUW("%d;PROC:SIGN:PDAT:ACT SON;*OPC?" % band)
        # Get the query information.
        CMUR()
        
        # Query the current signaling state
        CMUW("%d;SIGN:PDAT:STAT?" % band)
        rdbuf = CMUR()
    
        # Restart the phone
        restart(false, true)
        berputs "<font color='#008800'>"+"Restarted the phone at #{Time.now}"+"</font>"+"<br>"
        
        # Wait for Signaling ON.
        if (WaitCMUSignal("IDLE",TIMEOUT_SIGN_ON,band)==true)
            berputs "<font color='#008800'>"+"Signaling ON at #{Time.now}"+"</font>"+"<br>"
            
            sleep(5)
            
            # Init CoolTester
            CoolTester.init
            
            @attach = Proto_Auto_Attach.new( {:timeout=>10, :start=>1} )
            @attach.process

            sleep(5)

            # Wait for Synchro.
            if(WaitCMUSignal("ATT",TIMEOUT_MS_ATTACHED,band)==true)
                berputs "<font color='#008800'>"+"MS synchronized at #{Time.now}"+"</font>"+"<br>"
                
                sleep(5)
                
                # Initialize, start a TBF and register the log informations in tbf_log.
                # Initialize a TBF on the CMU
				CMUW("%d;PROC:SIGN:PDAT:ACT CBL" % @userOptions.band)
               
                # Wait for TBF Established.
                if(WaitCMUSignal("TEST",TIMEOUT_TBF_ESTABLISHED,band)==true)
                    berputs "<font color='#008800'>"+"TBF established at #{Time.now}"+"</font>"+"<br>"
                    tbf_established=true
                else 
                    berputs "<font color='red'>"+"TBF not established : BLER test aborted "+"</font>"+"<br>"
                    berputs " Below, log of the call (last API mesages from the stack) :"+"<br>"
                end
            else 
                berputs "<font color='red'>"+"MS not attached: BLER test aborted "+"</font>"+"<br>"
            end
        else 
            berputs "<font color='red'>"+"Signaling on not received : BLER test aborted "+"</font>"+"<br>"
            puts "On NOT find"

        end 
        return tbf_established
    end
    
    # Start a BLER test with a specific propagation model ( Typical Urban, Rural Area, Hilly Terrain).
    # Options are specified in a instance of the class BLERUserOptions : "options".
    # Stores results in @results_report. 
    def DOBLERMeasurement(mode, options)
        
        # By default.
        test_success=true
        
        case mode
            when PDCH_MODE_STATIC
                temp_str = "STATIC"
            when PDCH_MODE_TU50
                temp_str = "TU50"
            when PDCH_MODE_RA250
            
                if(options.band<3) 
                    temp_str = "RA250"
                else
                    temp_str = "RA130"
                end
                
            when PDCH_MODE_HT100
                temp_str = "HT100"
            else
                temp_str = "UNKNOWN"
        end

      
        # Add 4 to mode to address the PCS/DCS ber values in the table.
        mode += 4 if(options.band > 2) 
       
        resultsputs "<br>"
        resultsputs (" Test Conditions : %s -%d dBm" % [temp_str, LevelLimit[options.cs][mode]]) + "<br>"
        
        CMUW("%d;CONF:RXQ:BLER:CONT:PDAT:MSL:RLEV -%d.0" % [options.band,LevelLimit[options.cs][mode]])
        CMUW("%d;CONF:RXQ:BLER:CONT:REP SING,NONE,NONE" % options.band)
        CMUW("%d;CONF:RXQ:BLER:CONT:RLBC %d" % [options.band,options.rlc_blocks])
    
        # Apply the above parameters.
        CMUW("%d;INIT:RXQ:BLER" % options.band)

        # Query the current signaling state
        CMUW("%d;FETC:RXQ:BLER:STAT?" % options.band)
        rdbuf = CMUR()
    
        # Query BLER result.
        CMUW("%d;READ:RXQ:BLER?" % options.band)
        res = CMUR().split(',')

        # Query the current signaling state
        CMUW("%d;FETC:RXQ:BLER:STAT?" % options.band)
        rdbuf = CMUR()

        # Apply the above parameters.
        CMUW("%d;ABORT:RXQ:BLER" % options.band)

        # Query the current signaling state
        CMUW("%d;FETC:RXQ:BLER:STAT?" % options.band)
        rdbuf = CMUR()

        bler=res[1].to_f
        if bler > 10
           	test_success=true
           	resultsputs "<li>Block  error rate           :<font color='red'> %6.3f%%</font> [limit 10%%]" % bler
        else
			if (res[1]=="NAN")
				test_success=false
				resultsputs "<li><font color='red'> TEST FAILED returned NAN</font>"
			else	
				test_success=true
           		resultsputs "<li>Blocl error rate           : %6.3f%% [limit 10%%]" % bler
        	end
		end
            
        resultsputs "<br>"
        
        # General result.
        if test_success==false
            resultsputs " : <font color='red'>"+"TEST FAIL"+"</font>"
        else
            resultsputs " : <font color='#008800'>"+"TEST PASS"+"</font>"
        end
        
        # End of the list.
        resultsputs "</ul>"

        return test_success

    end
    
    # Close CMU and Fading simulator : It will disconnect the call.
    def BLERGetOffline()
        CMUClose()
        ABFSClose()
    end
        
    
    #####################################
    #           Public section          #
    #####################################
    
    public
            
    def initialize( params )
        
        @logfile=File.open("ber_test_gprs.html","ab+")
        
        # CAREFUL !
        # Do not call the super method "initialize" since this test is not an embedded one !
        
        # Call to a UnitTest method :
        # Intialize html report to be compatible with CoolWatcher.
        initializeReport
        
        # Initialize user options.
        @userOptions = BLERUserOptions.new
        BLERMakeOptions( params[:band] || @@DEFAULT_BAND ,
                         params[:arfcn] || @@DEFAULT_ARFCN,
                         params[:cs]|| @@DEFAULT_CS, 
                         params[:bool_static]|| @@DEFAULT_STATIC,
                         params[:bool_tu50] || @@DEFAULT_TU50,
                         params[:bool_ra250]|| @@DEFAULT_RA250,
                         params[:bool_ht100]|| @@DEFAULT_HT100,
                         params[:rlc_blocks]|| @@DEFAULT_FRAMES,   
                         params[:nb_slots]|| @@DEFAULT_NB_SLOTS)   
    end
    
    # Change any paramaters present as a key in the hash #params.
    # If a not a key, use old param value.
    def changeParams( params )
        BLERMakeOptions( params[:band] || @userOptions.band,
                         params[:arfcn] || @userOptions.arfcn,
                         params[:cs]|| @userOptions.cs, 
                         params[:bool_static] || @userOptions.do_static,
                         params[:bool_tu50] || @userOptions.do_tu50,
                         params[:bool_ra250]|| @userOptions.do_ra250,
                         params[:bool_ht100]|| @userOptions.do_ht100,
                         params[:rlc_blocks]|| @userOptions.rlc_blocks)      
    end
    
    def start
        
        # CAREFUL !
        # Do not call the super method "start" of the UnitTest class since this test is not an embedded one !
        # Indeed, there is no embedded test function to be called which will realize this test.
        
        # Boolean at true if the TBF has been established, false otherwise.
        tbf_established=false      
        test_success=false
        test_counter=0
        
        berputs  CoolTester.reportAddTitleParameters
        
        berputs "<ul>"
        berputs " <li> Channel  : %d %s </li>"  % [@userOptions.arfcn, BLERBandStr[@userOptions.band] ]
        berputs " <li> CS   : %s </li> "        % BLERCSStr[@userOptions.cs]
        berputs " <li> RLC blocks nb: %d </li>"     % @userOptions.rlc_blocks
        
        berputs "</ul>"
        
        berputs  CoolTester.reportAddTitleInformations
        berputs  CoolTester.reportAddStartTime
        
        while(test_success==false)
        
        # Fadding simulator initialization.
        ABFSBERInit()
        
        # CMU initialization.
        CMUBLERInit( @userOptions.band,
					 CMU_CS[@userOptions.cs],
                     PDCH_EXT_IN_LOSS,
                     @userOptions.arfcn,
                     PDCH_BCCH_LEVEL,
                     PDCH_BCCH_TN,
                     @userOptions.arfcn
                     )
        
        BERSetFadingMode(PDCH_MODE_STATIC,@userOptions)
        tbf_established=BLERWaitForMsSyncAndMsAttach(@userOptions.band)
        
        if tbf_established==true
            # STATIC Test.
            if (@userOptions.do_static)
                test_success=DOBLERMeasurement(PDCH_MODE_STATIC,@userOptions)
                sleep(5)
            end
                
             
            # TU50 Test.
            if(@userOptions.do_tu50==true)
                BERSetFadingMode(PDCH_MODE_TU50,@userOptions)
                test_success=DOBLERMeasurement(PDCH_MODE_TU50,@userOptions)
                sleep(5)
            end
            
            # RA250 Test.
            if (@userOptions.do_ra250==true)
                BERSetFadingMode(PDCH_MODE_RA250,@userOptions)
                test_success=DOBLERMeasurement(PDCH_MODE_RA250,@userOptions)
                sleep(5)
            end
            
            # HT100 Test.
            if (@userOptions.do_ht100==true)
                BERSetFadingMode(PDCH_MODE_HT100,@userOptions)
                test_success=DOBLERMeasurement(PDCH_MODE_HT100,@userOptions)
                sleep(5)
            end
        
			# Query the current signaling state
        	CMUW("%d;SIGN:PDAT:STAT?" % @userOptions.band)
        	rdbuf = CMUR()
            
            # Terminate call & switch on BCCH channel.
            CMUW("%d;PROC:SIGN:PDAT:ACT DISC" % @userOptions.band)
        
			# Switch on BCCH channel.
        	CMUW("%d;PROC:SIGN:PDAT:ACT SOFF" % @userOptions.band)
            
            # Back to Static.
            BERSetFadingMode(PDCH_MODE_STATIC,@userOptions)
            
            @results_available=true 
              
        else
            @results_available=false     
        end
        
        berputs CoolTester.reportAddDoneTime
                
        BLERGetOffline()

        test_counter=test_counter+1
        end
        
        @logfile.close();

    end
           
    def process
        start
        results
    end
    
    def results
        super
        @html_report<<@results_report
        return @html_report
    end

end
