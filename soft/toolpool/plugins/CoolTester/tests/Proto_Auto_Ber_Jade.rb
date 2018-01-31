# require "UnitTest"
require 'gpib.rb'
require 'cmu.rb'
require 'abfs.rb'

# require 'Proto_Auto_Call.rb'


class BERUserOptions
        attr_accessor :band, :level, :arfcn, :freq, :speech, :do_static, :do_tu50, :do_ra250, :do_ht100, :nb_frames, :amr_bitrate
end

class Proto_Auto_Ber_Jade
    
        #####################################
	#		DEFAULT PARAMETERS  #
	#####################################
    
    @@DEFAULT_BAND = 2
    @@DEFAULT_LEVEL = 104
    @@DEFAULT_ARFCN = 40
    @@DEFAULT_SPEECH = 1
    @@DEFAULT_STATIC = false
    @@DEFAULT_TU50 = false
    @@DEFAULT_RA250 = false
    @@DEFAULT_HT100 = false
    @@DEFAULT_FRAMES= 10000
    @@DEFAULT_AMRRATE= 0
    
    #####################################
	#		TEST DESCRIPTION			#
	#####################################
        
    @@TEST_NAME                 = "Proto Auto BER Test"
    @@VERSION                   = "1.0"
    @@ENVIRONEMENT              = "PROTO"
    @@TYPE                      = "AUTO"
    @@PREREQUISITE              = "Must be used with a .lod including GTES and with the Proto Auto Call test previously registerd. This test must be run on a PC connected to CMU and Fading simulator via GPIB. Obviously, the CMU and the Fading simulator must be on before running the test( Unless you run it only in static conditions). Furthermore, you be sure you use the specfic BER SIM CARD when running this test. This test is based on cmu.rb, gpib.rb and abfs.rb module."
    @@PARAMETERS=[
        ["band","#{@@DEFAULT_BAND}","Band mode : 1 s GSM850, 2 is GSM, 3 is DCS and 4 is PCS"],
        ["level","#{@@DEFAULT_LEVEL}","dB alleviation"],
        ["arfcn","#{@@DEFAULT_ARFCN}","Arfcn number"],
        ["speech","#{@@DEFAULT_SPEECH}","Speech mode : 0 is Full Speech Rate, 1 is Enhanced Full Speech Rate,2 is Half Rate Speech and >=3 is AMR."],
        ["bool_static","#{@@DEFAULT_STATIC}","Run the BER in static conditions"],
        ["bool_tu50","#{@@DEFAULT_TU50}","Run the BER in Typical Urban conditions at 50 kilometers"],
        ["bool_ra250","#{@@DEFAULT_RA250}","Run the BER in Rural Area conditions at 250 kilometers ( TGV conditions)"],
        ["bool_ht100","#{@@DEFAULT_HT100}","Run the BER in Hilly Terrain conditions at 100 kilometers"],  
        ["frames","#{@@DEFAULT_FRAMES}","Number of frames which will be processed"],
	    ["amr_bitrate","#{@@DEFAULT_AMRRATE}","The bit rate of the AMR speech codec"]
                ]   
    
    @@DESCRIPTION               = "Run a typical BER test with the specified options, in a fully automatic way. The CMU and the fading simulator will behave in 'remote control' during the test. The CMU will wait phone signalling. In the same time, the phone will be restarted automatically. Then CoolTester will be intialized. Once the CMU have got the signalling signal from MS, it waits for MS synchronization. Next, the MS intiates a call. If it sucessed, the required BER will be processed in order : static, tu50, ra250, ht100. "
       
    @@RESULTS                   =" Test Conditions : ... <br><ul class= 'parameter_ul'><li>Number of frames evaluated : ...</li><li>Alpha : ...[limit ...]</li><li>Frame error rate : ...% [limit ...%]</li><li>Class Ib residual bit error:  ...% [limit ...%]</li><li>Class II residual bit error: ...% [limit ...%]<br> : PASS or FAILED</li></ul>"
    
    @@EXAMPLE                   ="puts Proto_Auto_Ber.new( {:bool_static=>false,bool_tu50=>false} ).process <br / > We ask to do a BER on #{@@DEFAULT_BAND} band, with -#{@@DEFAULT_LEVEL}dB level, on arfcn #{@@DEFAULT_ARFCN}, in EFS, with #{@@DEFAULT_FRAMES} frames, strictly in RA250 and HT100 conditions."
    
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
    
    
    
      
    #####################################
    #           Private section         #
    #####################################
    
    private
        
        #####################################
        #           Constants section       #
        #####################################
    
    
    CMU_CODEC           = ["FRV1","FRV2","HRV1","AMRH", "AMRF"]

    AMR_CODEC           = ["C1220","C1020","C0795","C0740","C0670","C0590","C0515","C0475"]
    

    EXT_IN_LOSS         = "0.5"
    EXT_OUT_LOSS_STATIC = "0.5"
    EXT_OUT_LOSS_TU50   = "8.5"
    EXT_OUT_LOSS_RA250  = "10.4"
    EXT_OUT_LOSS_HT100  = "8.8"     
    
    BCCH_CHANNEL    = [0,162,32,600,600]
    BCCH_LEVEL      = 85
    BCCH_TN         = 3
    
    TCH_TN          = 3
    TCH_PCL         = 5 
    
    MODE_STATIC = 0
    MODE_TU50   = 1
    MODE_RA250  = 2
    MODE_HT100  = 3 
    
    POS_FER     = 0
    POS_RBER_Ib = 1
    POS_RBER_II = 2
    
    
    BERSpeechStr    = ["FS ","EFS","HS ","AMR_HR","AMR_FR"]

    BERAmrStr = ["12.2","10.2", "7.95","7.4","6.7","5.9","5.15","4.75"]

    BERBandStr      = ["???","GSM850","GSM900","DCS1800","PCS1900"]
    
    
    BERLimit        =  [
    [   #Full Rate Speech
        #FER  RBER Ib   RBER II
        [0.1,   0.4,    2], #STATIC
        [6,     0.4,    8], #TU 50 (no FH)
        [2,     0.2,    7], #RA 250 (no FH)
        [7,     0.5,    9], #HT 100 (no FH)
        [3,     0.3,    8], #PTU 50 (ideal FH)
        [2,     0.2,    7], #PRA 130 (no FH)
        [7,     0.5,    9]  #PHT 100 (no FH)
    ],
    [   #Enhanced Full Rate Speech
        # FER  RBER Ib  RBER II
        [0.1,   0.1,    2], #STATIC
        [8,     0.21,   7], #TU 50 (no FH)
        [3,     0.1,    7], #RA 250 (no FH)
        [7,     0.2,    9], #HT 100 (no FH)
        [4,     0.12,   8], #PTU 50 (no FH)
        [3,     0.1,    7], #PRA 130 (no FH)
        [7,     0.24,   9]  #PHT 100 (no FH)
    ],
    [   #Half Rate Speech
        # FER  RBER Ib  RBER II
        [0.025,  0.001,  0.72], #STATIC
        [  4.1,   0.36,   6.9], #TU 50 (no FH)
        [  4.1,   0.28,   6.8], #RA 250 (no FH)
        [  4.5,   0.56,   7.6], #HT 100 (no FH)
        [  4.2,   0.38,   6.9], #PTU 50 (no FH)
        [  4.1,   0.28,   6.8], #PRA 130 (no FH)
        [  5.0,   0.63,   7.8]  #PHT 100 (no FH)
    ],
    [   
	#AFS 12.2    
        # FER  RBER Ib  RBER II
        [ 50, 0.001, 50], #STATIC
        [4.9,   1.5, 50], #TU 50 (no FH)
        [1.4,   1.2, 50], #RA 250 (no FH)
        [4.5,   2.1, 50], #HT 100 (no FH)
        [  2,   1.4, 50], #PTU 50 (no FH)
        [1.3,   1.2, 50], #PRA 130 (no FH)
        [4.6,   2.1, 50]  #PHT 100 (no FH)
    ],
    [   #AFS 10.2 
        # FER  RBER Ib  RBER II
        [  50, 0.001, 50], #STATIC
        [ 2.1,  0.23, 50], #TU 50 (no FH)
        [0.45, 0.092, 50], #RA 250 (no FH)
        [ 1.6,  0.26, 50], #HT 100 (no FH)
        [0.65,  0.12, 50], #PTU 50 (no FH)
        [0.41, 0.084, 50], #PRA 130 (no FH)
        [1.6,   0.26, 50]  #PHT 100 (no FH)
    ],
    [   #AFS 7.95 
        # FER  RBER Ib  RBER II
        [   50,    50, 50], #STATIC
        [ 0.36,  0.11, 50], #TU 50 (no FH)
        [0.024,  0.02, 50], #RA 250 (no FH)
        [0.096,  0.06, 50], #HT 100 (no FH)
        [0.025, 0.023, 50], #PTU 50 (no FH)
        [0.018, 0.016, 50], #PRA 130 (no FH)
        [0.089, 0.061, 50]  #PHT 100 (no FH)
    ],
    [   #AFS 7.4
        # FER  RBER Ib  RBER II
        [   50,    50, 50], #STATIC
        [ 0.41, 0.054, 50], #TU 50 (no FH)
        [0.028, 0.009, 50], #RA 250 (no FH)
        [ 0.13, 0.033, 50], #HT 100 (no FH)
        [0.036, 0.013, 50], #PTU 50 (no FH)
        [0.023,  0.07, 50], #PRA 130 (no FH)
        [ 0.13, 0.031, 50]  #PHT 100 (no FH)
    ],
    [   #AFS 6.7
        # FER  RBER Ib  RBER II
        [   50,    50, 50], #STATIC
        [ 0.16, 0.082, 50], #TU 50 (no FH)
        [ 0.01, 0.013, 50], #RA 250 (no FH)
        [0.026, 0.044, 50], #HT 100 (no FH)
        [ 0.01, 0.017, 50], #PTU 50 (no FH)
        [ 0.01,  0.01, 50], #PRA 130 (no FH)
        [0.031, 0.041, 50]  #PHT 100 (no FH)
    ],
    [   #AFS 5.9
        # FER  RBER Ib  RBER II
        [   50,     50, 50], #STATIC
        [0.094,  0.014, 50], #TU 50 (no FH)
        [ 0.01,  0.001, 50], #RA 250 (no FH)
        [0.011,  0.003, 50], #HT 100 (no FH)
        [ 0.01,  0.001, 50], #PTU 50 (no FH)
        [ 0.01,  0.001, 50], #PRA 130 (no FH)
        [ 0.01,  0.002, 50]  #PHT 100 (no FH)
    ],
    [   #AFS 5.15
        # FER  RBER Ib  RBER II
        [  50,    50, 50], #STATIC
        [0.07, 0.014, 50], #TU 50 (no FH)
        [0.01, 0.001, 50], #RA 250 (no FH)
        [0.01, 0.002, 50], #HT 100 (no FH)
        [0.01, 0.001, 50], #PTU 50 (no FH)
        [  50, 0.001, 50], #PRA 130 (no FH)
        [0.01, 0.003, 50]  #PHT 100 (no FH)
    ],
    [   #AFS 4.75
        # FER  RBER Ib  RBER II
        [   50,    50, 50], #STATIC
        [0.029, 0.005, 50], #TU 50 (no FH)
        [   50, 0.001, 50], #RA 250 (no FH)
        [ 0.01, 0.001, 50], #HT 100 (no FH)
        [ 0.01, 0.001, 50], #PTU 50 (no FH)
        [   50, 0.001, 50], #PRA 130 (no FH)
        [ 0.01, 0.001, 50]  #PHT 100 (no FH)
    ],
     [  #AHS 7.95
        # FER  RBER Ib  RBER II
        [0.01, 0.04, 0.66], #STATIC
        [  20,  2.3,    5], #TU 50 (no FH)
        [  17,    2,  4.7], #RA 250 (no FH)
        [  28,  2.9,  5.7], #HT 100 (no FH)
        [  20,  2.3,    5], #PTU 50 (no FH)
        [  17,    2,  4.8], #PRA 130 (no FH)
        [  27,  2.9,  5.7]  #PHT 100 (no FH)
    ],
    [  #AHS 7.4
        # FER  RBER Ib  RBER II
        [0.01, 0.001, 0.66], #STATIC
        [  16,   1.4,  5.3], #TU 50 (no FH)
        [  14,   1.1,    5], #RA 250 (no FH)
        [  22,   1.8,    6], #HT 100 (no FH)
        [  16,   1.4,  5.3], #PTU 50 (no FH)
        [  13,   1.1,  5.1], #PRA 130 (no FH)
        [  22,   1.9,    6]  #PHT 100 (no FH)
    ],
    [   #AHS 6.7
        # FER  RBER Ib  RBER II
        [0.01, 0.01, 0.66], #STATIC
        [ 9.2,  1.1,  5.8], #TU 50 (no FH)
        [   8, 0.93,  5.5], #RA 250 (no FH)
        [  13,  1.5,  6.6], #HT 100 (no FH)
        [ 9.4,  1.1,  5.8], #PTU 50 (no FH)
        [ 7.5, 0.92,  5.5], #PRA 130 (no FH)
        [  13,  1.5,  6.6]  #PHT 100 (no FH)
    ],
    [  #AHS 5.9
        # FER  RBER Ib  RBER II
        [ 50,   50, 0.66], #STATIC
        [5.7, 0.51,    6], #TU 50 (no FH)
        [4.9, 0.42,  5.7], #RA 250 (no FH)
        [8.6, 0.73,  6.8], #HT 100 (no FH)
        [5.9, 0.52,  6.1], #PTU 50 (no FH)
        [4.6, 0.39,  5.8], #PRA 130 (no FH)
        [8.5, 0.72,  6.8]  #PHT 100 (no FH)
    ],
    [   #AHS 5.15
        # FER  RBER Ib  RBER II
        [ 50,   50, 0.66], #STATIC
        [2.5, 0.51,  6.3], #TU 50 (no FH)
        [2.2, 0.43,    6], #RA 250 (no FH)
        [  4, 0.78,  7.2], #HT 100 (no FH)
        [2.6, 0.53,  6.3], #PTU 50 (no FH)
        [  2,  0.4,  6.1], #PRA 130 (no FH)
        [3.7, 0.76,  7.2]  #PHT 100 (no FH)
    ],
    [   #AHS 4.75
        # FER  RBER Ib  RBER II
        [ 50,   50, 0.66], #STATIC
        [1.2, 0.17,  6.4], #TU 50 (no FH)
        [1.2, 0.14,  6.2], #RA 250 (no FH)
        [1.8, 0.26,  7.4], #HT 100 (no FH)
        [1.2, 0.18,  6.5], #PTU 50 (no FH)
        [1.1, 0.13,  6.2], #PRA 130 (no FH)
        [1.7, 0.25,  7.3]  #PHT 100 (no FH)
    ]]
       
    # Timeout options in seconds :
    TIMEOUT_SIGNALING_ON =  10
    TIMEOUT_MS_SYNC = 15
    TIMEOUT_CALL_ESTABLISHED = 15
 
        #####################################
        #           Variables section       #
        #####################################
    
    # Specific BER test instance/class variables added to the common Unit test instance/class variables.
    
    # GPIB handler.    
    @@BERHBASE= -1
    
    # Call Object. 
    @berCall
    
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
    
    # Store and convert user test options in the @userOptions BERUserOptions object.
    def BERMakeOptions(band, level, arfcn, speech, bool_static, bool_tu50, bool_ra250, bool_ht100, frames, amr_bitrate)       
             
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
        
        @userOptions.level = level
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
        
        case speech
            when 5
                @userOptions.speech = 4
            when 4
                @userOptions.speech = 3
            when 3
                @userOptions.speech = 2
            when 2
                @userOptions.speech = 1
            else
                @userOptions.speech = 0
        end
        
        case amr_bitrate
	        when 8
                @userOptions.amr_bitrate = 7
	        when 7
                @userOptions.amr_bitrate = 6
	        when 6
		        @userOptions.amr_bitrate = 5    
	        when 5
	            @userOptions.amr_bitrate = 4 	    
	        when 4 
	            @userOptions.amr_bitrate = 3	    
	        when 3
	            @userOptions.amr_bitrate = 2	    
	        when 2
	            @userOptions.amr_bitrate = 1
            else
                @userOptions.amr_bitrate = 0	    
	    	
	     end

         @userOptions.do_static  = bool_static
         @userOptions.do_tu50    = bool_tu50
         @userOptions.do_ra250   = bool_ra250
         @userOptions.do_ht100   = bool_ht100
         @userOptions.nb_frames  = frames
        
   
    end
    
    # Add str to @html_report.
    def berputs(str)
	    puts "html>"+str
        @logfile << str
    end
    
    # Add str to @results_report.
    def resultsputs(str)
        puts "html>"+str
        @logfile << str
    end
    
    # Config Fading Simulator with a specific propagation model ( Typical Urban, Rural Area, Hilly Terrain).
    def BERSetFadingMode(mode,options)
    
        case mode
            when MODE_STATIC
                # Set ABFS stat=OFF.
                ABFSW("FSIM:STAT OFF")
                
                # Set CMU IQIF to fading Path.
                CMUW("%d;CONF:IQIF:RXTX BYP" % options.band)
                
                # Set external attenuation
                CMUW("%d;SENS:CORR:LOSS:OUTP2 %s" % [options.band,EXT_OUT_LOSS_STATIC])
            
            # Typical Urban mode at 50 kilometers : Urban driving context.      
            when MODE_TU50
                # Set CMU external attenuation.
                CMUW("%d;SENS:CORR:LOSS:OUTP2 %s" % [options.band,EXT_OUT_LOSS_TU50])
                
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
            when MODE_RA250
                # Set external attenuation. 
                CMUW("%d;SENS:CORR:LOSS:OUTP2 %s" % [options.band,EXT_OUT_LOSS_RA250])
                
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
            when MODE_HT100
                # Set external attenuation.
                CMUW("%d;SENS:CORR:LOSS:OUTP2 %s" % [options.band, EXT_OUT_LOSS_HT100])
                
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
    # Wait for signalling ON, for MS synchronization, call established etc...
    # COULD BE INTEGRATED IN THE CMU MODULE.
    def WaitCMUSignal(signal,timeout,band)
            
        signal_received=false
        # Query the current signaling state
        CMUW("%d;SIGN:STAT?" % band)
        rdbuf = CMUR()
        
        begin
            Timeout::timeout(timeout) do
                while(rdbuf.slice(0..(signal.length-1)) != signal)
                    # Query the current signaling state.
                    CMUW("%d;SIGN:STAT?" % band)
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
    # Wait for Signalling ON until TIMEOUT_SIGNALING_ON.
    # If received, wait for MS synchronization until TIMEOUT_MS_SYNC.
    # If received, start a call.
    # If succeeded, wait for call establishement until TIMEOUT_CALL_ESTABLISHED.
    # Stores a log of all information in @html_report.
    # Return a boolean : true if the call has been established, false otherwise.
    def BERWaitForMsSyncAndMsConnect(band)
        
        call_established=false
        
        # Switch on BCCH channel.
        CMUW("%d;PROC:SIGN:ACT SON;*OPC?" % band)
        # Get the query information.
        CMUR()
        
        # Query the current signaling state
        CMUW("%d;SIGN:STAT?" % band)
        rdbuf = CMUR()
    
        # Restart the phone
        # restart(false, true)
        0xe0005c.w(0x100); 0xe00000.w(0xa50001); 0xE00004.w(0x80000000)
        
        
        berputs "<font color='#008800'>"+"Restarted the phone at #{Time.now}"+"</font>"+"<br>"
        
        # Wait for Signaling ON.
        if (WaitCMUSignal("SON",TIMEOUT_SIGNALING_ON,band)==true)
            berputs "<font color='#008800'>"+"Signaling ON at #{Time.now}"+"</font>"+"<br>"
            
            sleep(5)
            
            # Init CoolTester
            # CoolTester.init
            
            # Wait for Synchro.
            if(WaitCMUSignal("SYNC",TIMEOUT_MS_SYNC,band)==true)
                berputs "<font color='#008800'>"+"MS synchronized at #{Time.now}"+"</font>"+"<br>"
                
                sleep(5)
                
                # Initialize a MT call on the CMU
				CMUW("%d;PROC:SIGN:ACT MTC" % @userOptions.band)
                
                # Wait for Call Established.
                if(WaitCMUSignal("CEST",TIMEOUT_CALL_ESTABLISHED,band)==true)
                    berputs "<font color='#008800'>"+"Call established at #{Time.now}"+"</font>"+"<br>"
                    call_established=true
                else 
                    berputs "<font color='red'>"+"Call not established : BER test aborted "+"</font>"+"<br>"
                    berputs " Below, log of the call (last API mesages from the stack) :"+"<br>"
                end
            else 
                berputs "<font color='red'>"+"MS not synchronized: BER test aborted "+"</font>"+"<br>"
            end
        else 
            berputs "<font color='red'>"+"Signaling on not received : BER test aborted "+"</font>"+"<br>"
            puts "On NOT find"

        end 
        return call_established
    end
    
    # Start a BER test with a specific propagation model ( Typical Urban, Rural Area, Hilly Terrain).
    # Options are specified in a instance of the class BERUserOptions : "options".
    # Stores results in @results_report. 
    def DOFERRBERMeasurement(mode, options)
        
        # By default.
        alpha   = 1.0
        test_success=true
        
        case mode
            when MODE_STATIC
                temp_str = "STATIC"
            when MODE_TU50
                temp_str = "TU50"
            when MODE_RA250
            
                if(options.band<3) 
                    temp_str = "RA250"
                else
                    temp_str = "RA130"
                end
                
            when MODE_HT100
                temp_str = "HT100"
            else
                temp_str = "UNKNOWN"
        end

	case options.amr_bitrate
         when 0
		      blerLimit_index = 3     
	     when 1
		      blerLimit_index = 4    
	     when 2
              blerLimit_index = 5  		 
	     when 3 
		      blerLimit_index = 6	    
	     when 4
              blerLimit_index = 7 
	     when 5
              blerLimit_index = 8
	     when 6
              blerLimit_index = 9
	     when 7
	          blerLimit_index = 10 	     
	     else	     
	end

	case options.speech
	     when 0
		      blerLimit_index = 0
	     when 1 
	          blerLimit_index = 1
         when 2
              blerLimit_index = 2	
         when 3
              blerLimit_index += 6
         else		 
	end
          
        # Add 3 to mode to address the PCS/DCS ber values in the table.
        mode += 3 if(options.band > 2) 
       
        resultsputs "<br>"
        resultsputs (" Test Conditions : %s -%d dBm" % [temp_str, options.level]) + "<br>"
        
        # Set UNUSED = 20dB.
        CMUW("%d;PROC:BSS:TCH:LEV:UNT 20" % options.band)
    
        # Defines hold off times during which the mobile can adapt
        # Itself to the new RF level at the beginning of the RXQ
        # Mesurement and send back the bit stream received
        CMUW("%d;CONF:RXQ:CONT:HTIM 0.80,0.20" % options.band)
    
        # Defines the measured timeslot of the MS signal.
        CMUW("%d;CONF:MCON:MSL:MESL %d" % [options.band,TCH_TN])
    
        # Set stop condition and step mode: not abort although it's obvious the test will fail.
        CMUW("%d;CONF:RXQ:BER1:CONT:REP NONE,NONE" % options.band)
    
        # RFER test on ? frames.
        CMUW("%d;CONF:RXQ:BER1:CONT RFER,%d" % [options.band,options.nb_frames])
    
        # Set USED = -104dB in GSM or -102dBm in D(P)CS. 
        CMUW("%d;CONF:RXQ:BER1:CONT:LEV:UTIM -%d.0" % [options.band,options.level])
    
        # Apply the above parameters.
        CMUW("%d;INIT:RXQ:BER" % options.band)
    
        # Query BER result.
        CMUW("%d;FETC:RXQ:BER?" % options.band)
            
        # Change all ',' to ' ' in the buffer.
        res = CMUR().split(',')
        
        if ( res[2]=="NAN" || res[3]=="NAN" )
            # An error occured during the BER test process.
            # Results are not available.
            # Stop the results analysis here.
            resultsputs "<font color='red'> An error occured during test process. Results are not available !</font>"
            # TODO => PRECISE kind of error.
            #resultsputs "Check the SIM card if it's a BER test one etc..." 
        else        
            iires       = res[1].to_f
            ibres       = res[2].to_f
            ferres      = res[3].to_f
        
            # Derive alpha if needed ( Alpha term exists for FS only ).
            # In such a way, ferres= limit*alpha.
            if(ferres > BERLimit[blerLimit_index][mode][POS_FER] && options.speech == 0)
                alpha = ferres * 1.0 / (BERLimit[blerLimit_index][mode][POS_FER])
	    end
            
            # Modify limit value in function of alpha.
            # If alpha > 1.6, use 1.6 instead of alpha.
            if(alpha > 1.6)
                ibLimit     = BERLimit[blerLimit_index][mode][POS_RBER_Ib]   / 1.6
                ferLimit    = BERLimit[blerLimit_index][mode][POS_FER]       * 1.6
            else
                ibLimit     = BERLimit[blerLimit_index][mode][POS_RBER_Ib]   / alpha
                ferLimit    = BERLimit[blerLimit_index][mode][POS_FER]       * alpha
            end
            
            # Print measurement result.
            resultsputs "<ul>"
            resultsputs "<li>Number of frames evaluated : %d" % options.nb_frames
            
            # Alpha term exists for FS only.
            if options.speech == 0
                # 1<=alpha<=1.6.
                if alpha>1.6
                    test_success=false
                    resultsputs "<li>Alpha                      :<font color='red'> %.2f</font> [limit 1.6]" % alpha
                else
                    resultsputs "<li>Alpha                      : %.2f [limit 1.6]" % alpha
                end
            end
            
            # Check FER limit ( class Ia limit).
            if ferres > ferLimit
                test_success=false
                resultsputs "<li>Frame error rate           :<font color='red'> %6.3f%%</font> [limit %4.3f%%]" % [ferres, ferLimit]
            else
                resultsputs "<li>Frame error rate           : %6.3f%% [limit %4.3f%%]" % [ferres, ferLimit]
            end
            
            # Check class Ib limit.
            if ibres  > ibLimit
                test_success=false
                resultsputs "<li>Class Ib residual bit error:<font color='red'> %6.3f%%</font> [limit %4.3f%%]" % [ibres, ibLimit]
            else
                resultsputs "<li>Class Ib residual bit error: %6.3f%% [limit %4.3f%%]" % [ibres, ibLimit]
            end
            
            # Check class II limit.
            if (iires  > BERLimit[blerLimit_index][mode][POS_RBER_II])
                test_success=false
                resultsputs "<li>Class II residual bit error:<font color='red'> %6.3f%%</font> [limit %4.3f%%]" % [iires, BERLimit[blerLimit_index][mode][POS_RBER_II] ]
            else
                resultsputs "<li>Class II residual bit error: %6.3f%% [limit %4.3f%%]" % [iires, BERLimit[blerLimit_index][mode][POS_RBER_II] ]
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
        
        end    
                
    end
    
    # Close CMU and Fading simulator : It will disconnect the call.
    def BERGetOffline()
        CMUClose()
        ABFSClose()
    end
        
    
    #####################################
    #           Public section          #
    #####################################
    
    public
            
    def initialize( params )
        
        # CAREFUL !
        # Do not call the super method "initialize" since this test is not an embedded one !
        
        # Call to a UnitTest method :
        # Intialize html report to be compatible with CoolWatcher.
        # initializeReport
        @logfile=File.open("ber_test_jade.html","ab+")
        
        
        
        
        # Initialize user options.
        @userOptions = BERUserOptions.new
        BERMakeOptions( params[:band] || @@DEFAULT_BAND ,
                        params[:level] || @@DEFAULT_LEVEL,
                        params[:arfcn] || @@DEFAULT_ARFCN,
                        params[:speech]|| @@DEFAULT_SPEECH, 
                        params[:bool_static]|| @@DEFAULT_STATIC,
                        params[:bool_tu50] || @@DEFAULT_TU50,
                        params[:bool_ra250]|| @@DEFAULT_RA250,
                        params[:bool_ht100]|| @@DEFAULT_HT100,
                        params[:frames]|| @@DEFAULT_FRAMES,   
                        params[:amr_bitrate]|| @@DEFAULT_AMRRATE)   
    end
    
    # Change any paramaters present as a key in the hash #params.
    # If a not a key, use old param value.
    def changeParams( params )
         BERMakeOptions( params[:band] || @userOptions.band,
                        params[:level] || @userOptions.level,
                        params[:arfcn] || @userOptions.arfcn,
                        params[:speech]|| @userOptions.speech, 
                        params[:bool_static] || @userOptions.do_static,
                        params[:bool_tu50] || @userOptions.do_tu50,
                        params[:bool_ra250]|| @userOptions.do_ra250,
                        params[:bool_ht100]|| @userOptions.do_ht100,
                        params[:frames]|| @userOptions.nb_frames,
		        params[:amr_bitrate]|| @userOptions.amr_bitrate)      
    end
    
    def start
        
        # CAREFUL !
        # Do not call the super method "start" of the UnitTest class since this test is not an embedded one !
        # Indeed, there is no embedded test function to be called which will realize this test.
        
        # Boolean at true if the call has been established, false otherwise.
        call_established=false      
        
        # berputs  CoolTester.reportAddTitleParameters
        
        berputs "<ul>"
        berputs " <li> Level    : -%d dBm </li>"    % @userOptions.level
        berputs " <li> Channel  : %d %s </li>"  % [@userOptions.arfcn, BERBandStr[@userOptions.band] ]
        berputs " <li> Speech   : %s </li> "        % BERSpeechStr[@userOptions.speech]
        berputs " <li> Nb Frames: %d </li>"     % @userOptions.nb_frames
        
	    if(@userOptions.speech>2 )
	       berputs " <li> AMR bit rate: %s </li>"     % BERAmrStr[@userOptions.amr_bitrate] 
        end

        berputs "</ul>"
        
        # berputs  CoolTester.reportAddTitleInformations
        # berputs  CoolTester.reportAddStartTime
        
        
        # Fadding simulator initialization.
        ABFSBERInit()
        
        # CMU initialization.
        CMUBERInit( @userOptions.band,
                    CMU_CODEC[@userOptions.speech],
		            AMR_CODEC[@userOptions.amr_bitrate],
                    EXT_IN_LOSS,
                    BCCH_CHANNEL[@userOptions.band],
                    BCCH_LEVEL,
                    BCCH_TN,
                    @userOptions.arfcn,
                    TCH_TN,
                    TCH_PCL,@userOptions.level)
        
        BERSetFadingMode(MODE_STATIC,@userOptions)
        call_established=BERWaitForMsSyncAndMsConnect(@userOptions.band)
        
        if call_established==true
            # STATIC Test.
            if (@userOptions.do_static)
                DOFERRBERMeasurement(MODE_STATIC,@userOptions)
            end
                
             
            # TU50 Test.
            if(@userOptions.do_tu50==true)
                berputs "Do i get in?"
                BERSetFadingMode(MODE_TU50,@userOptions)
                DOFERRBERMeasurement(MODE_TU50,@userOptions)
            end
            
            # RA250 Test.
            if (@userOptions.do_ra250==true)
                BERSetFadingMode(MODE_RA250,@userOptions)
                DOFERRBERMeasurement(MODE_RA250,@userOptions)
            end
            
            # HT100 Test.
            if (@userOptions.do_ht100==true)
                BERSetFadingMode(MODE_HT100,@userOptions)
                DOFERRBERMeasurement(MODE_HT100,@userOptions)
            end
            
            # Terminate call & switch on BCCH channel.
            CMUW("%d;PROC:SIGN:ACT CREL;*OPC?" % @userOptions.band)
            
            # Back to Static.
            BERSetFadingMode(MODE_STATIC,@userOptions)
            
            @results_available=true 
              
        else
            @results_available=false     
        end
        
        # berputs CoolTester.reportAddDoneTime
                
        BERGetOffline()
        
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
