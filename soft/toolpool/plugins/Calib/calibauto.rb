require 'cmu'
require 'Calib/calib'

# As a calib init is done at the beginning of the calib_auto
# script, there is no need for any other function called
# in this file to call calib_init. Therefore, the implicit
# parameter initCalib of those function is always 
# explicited to false.

module CalibAuto
	
	private
	
	SCRIPT_VERSION = "Modem2G 2.0"
	
	ARFCN_MIN = 0
	ARFCN_MAX = 1

    NB_MEAS_ILOSS = 200
	
	#Get the calib constants
	@@C = CH__calib_globals
	
    # Default configuration for the Automatic Calibration
    @@CALIBAUTO_DEFAULT_CONFIG = {"bool_dump_flash_parameters" => false,
                    "dump_filename" => "Calibration Parameters.cfp",
                    "load_cfp" => false,
	                "cfp_to_load_filename" => "",
					"bool_reset_param" => true,
					"bool_gpadc_calib" => true,
					"bool_crystal_calib" => true,
                    "int_fof_offset" => 0,
					"bool_afc_gain0" => false, "bool_afc_gain1" => true, 
                    "bool_afc_gain2" => true, "bool_afc_gain3" => false,
					"bool_pa_profile0" => false, "bool_pa_profile1" => true,
                    "bool_pa_profile2" => true, "bool_pa_profile3" => false,
					"bool_pa_offset0" => false, "bool_pa_offset1" => true,
                    "bool_pa_offset2" => true, "bool_pa_offset3" => false,
					"bool_iloss0" => false, "bool_iloss1" => true,
					"bool_iloss2" => true, "bool_iloss3"=> false,
					"bool_ip2_0" => false, "bool_ip2_1" => false,
					"bool_ip2_2" => false, "bool_ip2_3" => false,
					"bool_burn_flash" => false,
					"input_loss" => [0.5,0.5,0.6,0.7], "output_loss" => [0.5,0.5,0.6,0.7]}


	def logText(text)
		puts text
	end
	
	def logTitle(title)
		puts " "
		puts "html><font color=darkgreen>"+title+"</font>"
		puts " "
	end

#	addHelpEntry("Calibration","calibCmuBand","band","","Function used by the 
#                    CMU module to recover the CMU secondary address used by 
#                    the [band] band")
    def calibCmuBand(band)
        return band+1
    end
	 
	def calib_auto_init()
		#Init the calibration pointers.
		logTitle "DISPLAY VERSIONS"
		not_mod, phy_ready = calib_init()
		
		#RF CHIP NAMES
		xcv_name_str, pa_name_str, sw_name_str = calib_get_rf_chip_names_str(false)
		logText "Rf Chips"
		logText xcv_name_str
		logText pa_name_str
		logText sw_name_str
		
		#Versions
		versiontext = calib_get_versions(not_mod,phy_ready, false)
		logText versiontext
	end
	
	def calib_auto_reset_parameters()
		#Init the calibration pointers.
		logTitle "RESET CALIBRATION PARAMETERS"
		logText "Reseting calibration parameters..."
		calib_reset_parameters()
		logText "Calibration parameters reseted to default!"
	end
    
    def calib_auto_load_parameters(filename)
        logTitle "Load parameters from the file \"" + File.basename(filename) + "\"..."
        calib_load_parameters(filename, false)
    end

	def calib_auto_gpadc_calibration()
		#TODO !
		logTitle "GPADC CALIBRATION"
		logText "Need to add the GPADC Calibration here!!!"
	end
	
	def calib_auto_ramrun()
		#TODO !
		logTitle "TODO RAMRUN"
	end
	
    # Measure the Target's Rx power until we get a stable value
    # Returns the measured power
    def bbMeasureRxPower(band, arfcn, expPow)
        measure = 0
		measureTable = []
		lastMeasure = 0
		lastLastMeasure = 0
		nbMeas = 0

		# Put Target in Rx Monit mode.
		calib_mode_rx_power(band, arfcn, -expPow, false)
        begin
			#Keep the 2 previously measure poweres, to compare
			lastLastMeasure = lastMeasure
			lastMeasure = measure
			
			begin
				measure = calib_get_rx_nb_power(false)
			end while(measure == 0)
			
			#Calculate the offset between the CMU power and measured power
			iloss = expPow + measure
			
			
			#Put the measured values in a table
			if( nbMeas < NB_MEAS_ILOSS)
				measureTable[nbMeas] = measure
				nbMeas += 1
			else
				#Print the values measured used for Debug.
				nbMeas.times { |i| 
					logText "Power measured [%2d] = %d " % [i, measureTable[i]]
				}
				
				raise "Error : iloss too high"
			end
		end while( iloss.abs() > 10 || measure != lastMeasure || measure != lastLastMeasure )

        #Print the values measured used for Debug.
        for i in (nbMeas - 3)..(nbMeas -1)
            logText "Power measured [%2d] = %d" % [i, measureTable[i]]
        end

        return -measure
    end
			
	def calib_auto_crystal_calibration(fofOffset)
		
		band = @@C::CALIB_STUB_BAND_GSM900
		arfcn = 30 
		pcl = 19 
		tsc= 0
		
		logTitle "CRYSTAL"
			
		#Adjust crystal
		calib_set_xtal_freq_offset(fofOffset, false) if(fofOffset!=0)
		
		logText "Set the initial frequency offset :\t%d" % fofOffset
		
		#Send TX bursts by the chip
		calib_mode_tx_pcl(band,arfcn,pcl,tsc, false)
		
		#CMU measurement
		measuredFof = 0
	
		#This loop runs as long as CES doesn't ask to stop.
		begin
			#Get the FOf from CMU.
			measuredFof = CMUMeasureFOf(band,arfcn)
	
			#Display some information
			logText "Current FOF:\t%f" % measuredFof
		
			stop = calib_set_xtal_freq_offset(measuredFof, false)
		end while(!stop)

        #Update the calibration values in Target.
		logText "Updating calibration..."
		calib_update()
      
				
	end
	
	def calib_auto_afc_gain(band)
	
		#Some band dependent parameters.
		case band
			when @@C::CALIB_STUB_BAND_PCS1900
				arfcn = 512
				pcl = 15
				logTitle("AFC GAIN PCS1900")
			when @@C::CALIB_STUB_BAND_DCS1800
				arfcn = 512
				pcl = 15
				logTitle("AFC GAIN DCS1800")
			when @@C::CALIB_STUB_BAND_GSM900
				arfcn = 30
				pcl = 19
				logTitle("AFC GAIN GSM900")
			else
				arfcn = 162
				pcl = 19
				band = 0
				logTitle("AFC GAIN GSM850")
		end
		
		logText "Measuring the AFC bound FOf..."
		
		#Put Target in Tx Send bursts mode.
		calib_mode_tx_pcl(band, arfcn, pcl, 0, false)

		#### Set AFC DAC Low value.
		#Set the AFC DAC Min value.
		calib_set_afc_bound(1, false)

		#Get the FOf.
		fofLow = CMUMeasureFOf(band, arfcn)
		logText "FOf Low: %d" % fofLow
		
		#Set the AFC DAC High value.
		calib_set_afc_bound(2, false)
		
		#Get the FOf.
		fofHigh = CMUMeasureFOf(band, arfcn)
		logText "FOf High: %d" % fofHigh
		
		#### AFC Gain Calculation.
		calib_set_afc_freq_offset( (fofHigh - fofLow).abs , false)
		
		# Set the AFC DAC to the default value.
		calib_set_afc_bound(0, false)
		
		#Target will use the calculated PA profile.
		calib_update()
	end
	
	def calib_auto_pa_profile(band)
		
		#### Some band dependent parameters.
		
		#No profile for PCS, use DCS one.
		band = @@C::CALIB_STUB_BAND_DCS1800 	if(@@C::CALIB_STUB_BAND_PCS1900 == band)
		
		if(band==@@C::CALIB_STUB_BAND_DCS1800) then
			arfcn = 512
			logTitle "PA PROFILE DCS/PCS"
		else
            # @@C::CALIB_STUB_BAND_GSM850
            # or @@C::CALIB_STUB_BAND_GSM900
			band = @@C::CALIB_STUB_BAND_GSM900 #0
			arfcn = 30
			logTitle "PA PROFILE GSM850/900"
		end
		
		powerMeasure = 0
		powerMeasureF = 0
		count=0
		
		#Run this until the PA driver asks to stop.
		begin
			nextDacVal = 0
			processStatus = 0
						
			#Send the measurement result and get the next DAC value
			nextDacVal, processStatus = calib_set_pa_profile_meas(band,powerMeasure, false)
			
			#Do a power measurement
			if(processStatus == @@C::CALIB_PROCESS_CONTINUE) then
				#Set Target in Tx mode
				calib_mode_tx_pa_val(band,arfcn,nextDacVal,0, false)	
				
				#Measure the power with the CMU.
				powerMeasureF = CMUMeasurePower(band, arfcn)
				logText "DAC Value[%d] = %d - Power = %6.2f dBm" % [count,nextDacVal,powerMeasureF]
			
				powerMeasure = (powerMeasureF * 100).to_i
				count += 1
			elsif(processStatus == @@C::CALIB_PROCESS_NEED_CALM) then
				calib_mode_stop(false)
				powerMeasure = 0
			end
		end while(processStatus == @@C::CALIB_PROCESS_CONTINUE || processStatus == @@C::CALIB_PROCESS_NEED_CALM)
	
		#Ask the Chip to update the calibration parameters
		logText " "
		logText "Updating the calibration..."
		calib_update()
	end
	
	def calib_auto_pa_offset(band)
		
		#Allocate array to store measure values
		measuredPow = []
	
		#Some band dependent parameters.
		case band
			when @@C::CALIB_STUB_BAND_PCS1900
				pclNum = 18
				pclFirstOffset = 0
				pclToSkip = 2
				arfcnMin = 512
				arfcnMax = 810
				logTitle "PA OFFSET PCS1900"
			when @@C::CALIB_STUB_BAND_DCS1800
				pclNum = 17
				pclFirstOffset = 0
				pclToSkip = 1
				arfcnMin = 512
				arfcnMax = 885
				logTitle "PA OFFSET DCS1800"
            when @@C::CALIB_STUB_BAND_GSM850
				pclNum = 15
				pclFirstOffset = 5
				pclToSkip = 0
				arfcnMin = 128
				arfcnMax = 251
				logTitle "PA OFFSET GSM850"
            else # @@C::CALIB_STUB_BAND_GSM900
				band = @@C::CALIB_STUB_BAND_GSM900
				pclNum = 15
				pclFirstOffset = 5
				pclToSkip = 0
				arfcnMin = 1
				arfcnMax = 124
				logTitle "PA OFFSET GSM900"
		end
		
		#Measure the power for some PCL
		for i in 0..((pclNum*2)-1)
			#Do not do the measure for the extra PCLs.
			next if( (i%pclNum) >= (pclNum - pclToSkip))
			
			logText " " if(i==pclNum)
			
			#Skipped because of interpolation.
			next if( (i >= pclNum + 2) && (i < pclNum*2 - 1 - pclToSkip) )
			
			if(i < pclNum)
				pcl = i + pclFirstOffset
				arfcn = arfcnMin
			else
				pcl = i + pclFirstOffset - pclNum
				arfcn = arfcnMax
			end
			
			#Send a command to the Chip
			calib_mode_tx_pcl(band,arfcn,pcl,0, false)

			#Do the measure only for ARFCN min and for 3 PCL for ARFCN Max
			if( (i < pclNum) || (i==pclNum) || (i==pclNum+1) || (i== (pclNum*2 - 1- pclToSkip) ) ) then
				#Measure power with the CMU
				measuredPow[i] = CMUMeasurePower(band,arfcn)
				
				#Display the measure
				logText "ARFCN=%d\tPCL=%d\t%6.2f dBm " %  [arfcn, pcl, measuredPow[i]]
			end
			
		end
			
		#Some display
		logText " "
		logText "Interpolated values : "
		
		#Interpolation for ARFCN Max
		curr_offset = measuredPow[pclNum+1] - measuredPow[1]
		delta_offset = ( (measuredPow[pclNum+1] - measuredPow[1]) -	(measuredPow[pclNum*2-1-pclToSkip] - measuredPow[pclNum-1-pclToSkip]) ) / (pclNum-3+1-pclToSkip)
		
		for i in (pclNum+2)..((pclNum*2 - 1 - pclToSkip)-1)
			curr_offset -= delta_offset
			measuredPow[i] = measuredPow[i-pclNum] + curr_offset
		end
		
		#Display and send to Target the interpolated values.

		for i in 0..((pclNum*2) - 1)
			#Do not display the result for the extra PCLs.
			next if(i%pclNum >= pclNum - pclToSkip)
			
			logText " " if(i==0 || i==pclNum)
			
			if(i<pclNum) then
				#Display the powers. 
				logText "Measured PCL Power Min ARFCN [%d] = %d" % [ i%pclNum+pclFirstOffset, (100*measuredPow[i]).to_i ]
				
				#Send the powers to Target
				calib_set_power_per_pcl_per_arfcn(band,i%pclNum,ARFCN_MIN,(100*measuredPow[i].to_i), false)
			else
				#Display the powers. 
				logText "Measured PCL Power Max ARFCN [%d] = %d" % [ i%pclNum+pclFirstOffset, (100*measuredPow[i]).to_i ]
				
				#Send the powers to Target
				calib_set_power_per_pcl_per_arfcn(band,i%pclNum,ARFCN_MAX,(100*measuredPow[i].to_i), false)
			end
		end
	
		#Update the calibration values in Target.
		logText " "
		logText "Updating calibration..."

		calib_update()
	end
	
	def calib_auto_iloss(band)
	
        iLoss = 0

        # TODO Why do we set nextArfcn here knowing there's the 
        # iLoss = 0 call before.
		
		case band
			#Some band dependent parameters.
			when @@C::CALIB_STUB_BAND_PCS1900	#PCS
				nextArfcn = 512
				expPow = -60
				logTitle "INSERTION LOSS PCS1900"
			when @@C::CALIB_STUB_BAND_DCS1800	#DCS
				nextArfcn = 512
				expPow = -60
				logTitle "INSERTION LOSS DCS1800"
            when @@C::CALIB_STUB_BAND_GSM850
                nextArfcn = 128
                expPow = -60
                logTitle "INSERTION LOSS GSM850"
			else
				band = @@C::CALIB_STUB_BAND_GSM900
				nextArfcn = 30
				expPow = -60
				logTitle "INSERTION LOSS GSM900"
		end
		
		#Put the Target in Rx Monit mode.
		calib_mode_rx_power(band, nextArfcn, -expPow, false)
		
        # Call with a null iLoss to get the ARFCN to measure from XCV driver.
        nextArfcn, stop = calib_set_iloss_offset(iLoss, false)

		logText "Measuring the insertion loss..."
		logText " "
		measuredPow = 0
		
		# Measure a new insertion loss as long as the transciever requests it.
		while (!stop) do
            logText " "

            # Set the CMU in generator mode
            CMUGeneratorMode(band, nextArfcn, expPow)
            
            # Measure Target's Rx power until we get a stable value.
            measPower = bbMeasureRxPower(band, nextArfcn, expPow)

            iLoss = expPow - measPower

            logText " "
            logText "Insertion loss for ARFCN %d: %d " % [nextArfcn, iLoss]

            # Set Target's iLoss to measured value and get next ARFCN to measure.
            nextArfcn, stop = calib_set_iloss_offset(iLoss, false)
		end 

		#Update the calibration values in the Target
		#logText " "
		#logText "Updating calibration..."
		
		#calib_update()
	end
	
	#Set the IP2 magnitude, only for Skyworks!
    #TODO That properly
	def calib_auto_setIP2IQMag(band,magI,magQ)
		#Change the IP2 magnitude in the calibration fields.
        # FIXME
		calib_set_xcv_param(band+7,magI, false)
		calib_set_xcv_param(band+10,magQ, false)
		calib_update()
		
		#Write the RF parameters from calibration fields to RF chips.
		calib_set_update_values()
	end
	
	def calib_auto_ip2(band)
		#Some band dependent parameters.
		case band
			when @@C::CALIB_STUB_BAND_PCS1900 #For PCS
				arfcnMin = 600
				arfcnMax = 630
				expPowMin = -99
				expPowMax = -32
				logTitle "SKYWORKS IP2 PCS1900"
			when @@C::CALIB_STUB_BAND_DCS1800 #For DCS
				arfcnMin = 600
				arfcnMax = 630
				expPowMin = -99
				expPowMax = -32
				logTitle "SKYWORKS IP2 DCS1800"
            when @@C::CALIB_STUB_BAND_GSM850
				arfcnMin = 128
				arfcnMax = 251
				expPowMin = -99
				expPowMax = -32
				logTitle "SKYWORKS IP2 GSM850"
            else
				band = @@C::CALIB_STUB_BAND_GSM900;
				arfcnMin = 65
				arfcnMax = 95
				expPowMin = -99
				expPowMax = -32
				logTitle "SKYWORKS IP2 GSM900"
		end

		iP2MagMin = 40
		iP2MagMax = 60
		
		#### Display the IP2 magnitude.
		#IP2 magnitude 40, blocker OFF.
		logText "Set IP2 magnitude to %d." % iP2MagMin
		
		#Set the CMU in generator mode, with null power.
		CMUGeneratorStop(band)

		#Set the IP2 I/Q magnitude manually.
		calib_auto_setIP2IQMag(band, iP2MagMin, iP2MagMin)

		#Set Target into power monitoring mode.
		calib_mode_rx_power(band, arfcnMin, -expPowMin, false)

		step = 0
		dcoI = []
		dcoQ = []

		#Get the DC offset from Target.
		dcoI[step], dcoQ[step] = calib_get_iq_dc_offsets(false)

		#### IP2 magnitude 40, blocker ON.
		#Set Target into stop mode.
		calib_mode_stop(false)	
		
		#Set the CMU in generator mode.
		CMUGeneratorMode(band, arfcnMax, expPowMax, false)

		#Set Target into power monitoring mode.
		calib_mode_rx_power(band, arfcnMin, -expPowMin, false)

		#Get the DC offset from Target.
		step += 1
		dcoI[step], dcoQ[step] = calib_get_iq_dc_offsets(false)

		#Display some information.
		logText "Step %d: DC offset I: %d - DC offset Q: %d" % [step, dcoI[step - 1], dcoQ[step - 1]]
		logText "Step %d: DC offset I: %d - DC offset Q: %d" % [step + 1, dcoI[step], dcoQ[step]]

		#### IP2 magnitude 60, blocker OFF.
		#Display the IP2 magnitude.
		logText "Set IP2 magnitude to %d." % iP2MagMax

		#Set the CMU in generator mode, with null power.
		CMUGeneratorStop(band)

		#Set the IP2 I/Q magnitude manually.
		calib_auto_setIP2IQMag(band, iP2MagMax, iP2MagMax)

		#Set Target into power monitoring mode.
		calib_mode_rx_power(band, arfcnMin, -expPowMin, false)

		#Get the DC offset from Target.
		step += 1
		dcoI[step], dcoQ[step] = calib_get_iq_dc_offsets(false)

		#### IP2 magnitude 60, blocker ON.
		#Set Target into stop mode. 
		calib_mode_stop()

		#Set the CMU in generator mode.
		CMUGeneratorMode(band, arfcnMax, expPowMax)

		#Set Target into power monitoring mode.
		calib_mode_rx_power(band, arfcnMin, -expPowMin, false)

		#Get the DC offset from Target.
		step +=1
		dcoI[step], dcoQ[step] = calib_get_iq_dc_offsets(false)

		#Display some information.
		logText "Step %d: DC offset I: %d - DC offset Q: %d" % [step, dcoI[step - 1], dcoQ[step - 1]]
		logText "Step %d: DC offset I: %d - DC offset Q: %d" % [step + 1, dcoI[step], dcoQ[step]]
		
		#### IP2 magnitude calculation.
		#Zero division check.

		if (0 == (dcoI[1] - dcoI[0]) - (dcoI[3] - dcoI[2]) || 0 == (dcoQ[1] - dcoQ[0]) - (dcoQ[3] - dcoQ[2]) ) then
			raise "Error ! Division by 0 in IP2 calculation ! "
		end
		
		#IP2 magnitude for I.
		iP2MagI = iP2MagMin - ((iP2MagMin - iP2MagMax) * (dcoI[1] - dcoI[0])) /	((dcoI[1] - dcoI[0]) - (dcoI[3] - dcoI[2]))

		#IP2 magnitude for Q.
		iP2MagQ = iP2MagMin - ((iP2MagMin - iP2MagMax) * (dcoQ[1] - dcoQ[0])) /	((dcoQ[1] - dcoQ[0]) - (dcoQ[3] - dcoQ[2]))

		logText " "
		logText "IP2 magnitude I: %d - IP2 magnitude Q: %d" % [iP2MagI, iP2MagQ]

		#Send the result to Target.
		calib_auto_setIP2IQMag(band, iP2MagI, iP2MagQ)
	end
		
	def calib_auto_burn_flash()
		logTitle("BURNING THE FLASH");
		
		#Display some information for the user.
		logText "Start burning the Flash..."

		#Burn Target's flash with calibrated values.
		calib_burn_flash(true, false)
		
		#Display some information for the user.
		logText "Flash burnt!"
    end
	
	def calib_auto_dump_flash_parameters(filename)
		logTitle "DUMP FLASH PARAMETERS"
		logText "Dumping parameters to the file \"" + File.basename(filename) + "\"..."

		calib_dump_parameters(filename,true, false)

		logText "Parameters dumped to file!"
	end
		
	
    
    
    public
    
    addHelpEntry("Calibration",
                 "calib_auto_get_default_config",
                 "",
                 "[CALIBAUTO_DEFAULT_CONFIG]",
                 "Return the hash with the default value for the
                 automatic calibration"
                )
    def calib_auto_get_default_config()
        return @@CALIBAUTO_DEFAULT_CONFIG
    end


	addHelpEntry("Calibration",
		     "calib_auto",
             "Hash whose keys are (default value) bool_dump_flash_parameters(false),
              \"dump_filename\"(\"Calibration Parameters.cfp\"),
			  \"bool_reset_param\"(false),
              \"load_cfp\"(false),
              \"cfp_to_load_filename\"(\"Cfp file to load\")
			  \"bool_gpadc_calib\"(true),
			  \"bool_crystal_calib\"(true),
              \"int_fof_offset\"(0),
			  \"bool_afc_gain0\"(false), 
			  \"bool_afc_gain1\"(true), 
              \"bool_afc_gain2\"(true),
              \"bool_afc_gain3\"(true),
			  \"bool_pa_profile0\"(false),
			  \"bool_pa_profile1\"(true),
              \"bool_pa_profile2\"(true),
              \"bool_pa_profile3\"(false),
			  \"bool_pa_offset0\"(false),
			  \"bool_pa_offset1\"(true),
              \"bool_pa_offset2\"(true),
              \"bool_pa_offset3\"(false),
			  \"bool_iloss0\"(false),
			  \"bool_iloss1\"(true),
			  \"bool_iloss2\"(true),
			  \"bool_iloss3\"(false),
			  \"bool_ip2_0\"(false),
			  \"bool_ip2_1\"(false),
			  \"bool_ip2_2\"(false),
			  \"bool_ip2_3\"(false),
			  \"bool_burn_flash\"(false),
              \"input_loss\"([0.5,0.5,0.6,0.7]),
              \"output_loss\"([0.5,0.5,0.6,0.7])
			  ",
              "",
              "Automatic calibration. All parameter are optional.
              Defaults values are in parenthesis. You can force 
              an automatic calibration to save the calibration 
              sector in a given filename by calling:
              calib_auto(\"bool_dump_flash_parameters\"  => true, \"dump_filename\" => \"c:/path/to/file.cfp\").
              This function requires the target to be running the 
              calib stub.
              ")
	def calib_auto(userConfig)
        # The default config is updated with the parameters
        # specified by the user.
        config = @@CALIBAUTO_DEFAULT_CONFIG
		
        config.merge!(userConfig)

		calib_plus_init_start = Time.now
		
		logText "Automatic Calibration"
	
		#Init the telnet connection and the device (CMU-200).
		CMUCalibInit(config["input_loss"], config["output_loss"])
		
		calib_start = Time.now
	
		# Load CES in Ram.
#		load_start = Time.now
#		calib_auto_ramrun() if(config["bool_do_ramrun"])
#		load_end  = Time.now;
		
		#Init the calibration pointers.
		calib_auto_init() 
	
        # Initial Parameters 
        # Check coherency
		if (config["bool_reset_param"] && config["load_cfp"])
            raise "mmm Loading from a file and resetting flash parameters ? There's something I don't really get there ..."
        end 

        #Reset the calibration parameters.
		calib_auto_reset_parameters() if(config["bool_reset_param"])

        # Or load from a file
        calib_auto_load_parameters(config["cfp_to_load_filename"]) if(config["load_cfp"])
        # Or keep current parameters
		
		#GP ADC calibration.
		gp_start = Time.now
		calib_auto_gpadc_calibration() if(config["bool_gpadc_calib"])
		gp_end = Time.now
		
		#Crystal calibration.
		cdac_start = Time.now
		calib_auto_crystal_calibration(config["int_fof_offset"]) if(config["bool_crystal_calib"])
		cdac_end = Time.now
		
		#AFC Gains calibration.
		afc_start = Time.now
		calib_auto_afc_gain(0) 	if(config["bool_afc_gain0"])
		calib_auto_afc_gain(1)	if(config["bool_afc_gain1"])
		calib_auto_afc_gain(2)	if(config["bool_afc_gain2"])
		calib_auto_afc_gain(3)	if(config["bool_afc_gain3"])
		afc_end = Time.now

		#PA Profile calibration.
		# Only one must be done, but once can choose on which band: GSM900
        # or DCS1800.
		pa_start = Time.now
		calib_auto_pa_profile(1)	if(config["bool_pa_profile1"])
		calib_auto_pa_profile(2)	if(config["bool_pa_profile2"])
		pa_end = Time.now
		
		#PA Offset calibration.
		offset_start = Time.now
		calib_auto_pa_offset(0)	if(config["bool_pa_offset0"])
		calib_auto_pa_offset(1)	if(config["bool_pa_offset1"])
		calib_auto_pa_offset(2)	if(config["bool_pa_offset2"])
		calib_auto_pa_offset(3)	if(config["bool_pa_offset3"])
		offset_end = Time.now
		
		#Insertion Loss calibration.
		il_start = Time.now
		calib_auto_iloss(0)	if(config["bool_iloss0"])
		calib_auto_iloss(1)	if(config["bool_iloss1"])
		calib_auto_iloss(2)	if(config["bool_iloss2"])
		calib_auto_iloss(3)	if(config["bool_iloss3"])
		il_end = Time.now
	
		#Skyworks' IP2 calibration.
		ip2_start = Time.now
		calib_auto_ip2(0)	if(config["bool_ip2_0"])
		calib_auto_ip2(1)	if(config["bool_ip2_1"])
		calib_auto_ip2(2)	if(config["bool_ip2_2"])	
		calib_auto_ip2(3)	if(config["bool_ip2_3"])	
		ip2_end = Time.now
		
		#Flash burn.
		burn_start = Time.now
		calib_auto_burn_flash() if(config["bool_burn_flash"])
		burn_end = Time.now
		
		#Profiling: end of calibration.	
		calib_end = Time.now
		calib_plus_init_end = Time.now
		
		#Dump the calib flash parameters to a file.
		calib_auto_dump_flash_parameters(config["dump_filename"]) if(config["bool_dump_flash_parameters"])
		
		#Calculate the time spent including init and close.
		logTitle("TIMING REPORT");
		str = "html><table>"
		str+= "<tr><td>Init / close time:</td><td>%5d sec</td></tr>" % ( (calib_plus_init_end - calib_plus_init_start) - (calib_end - calib_start) )
#		str+= "<tr><td>Load Ram time:</td><td>%5d sec</td></tr>" % (load_end - load_start)
		str+= "<tr><td>GP ADC time:</td><td>%5d sec</td></tr>" % (gp_end - gp_start)
		str+= "<tr><td>PA Profile time:</td><td>%5d sec</td></tr>" % (pa_end - pa_start)
		str+= "<tr><td>PA Offset time:</td><td>%5d sec</td></tr>" % (offset_end - offset_start)
		str+= "<tr><td>Insertion loss time:</td><td>%5d sec</td></tr>" % (il_end - il_start)
		str+= "<tr><td>Crystal time:</td><td>%5d sec</td></tr>" % (cdac_end - cdac_start)
		str+= "<tr><td>Skyworks' IP2 time:</td><td>%5d sec</td></tr>" % (ip2_end - ip2_start)
		str+= "<tr><td>AFC time:</td><td>%5d sec</td></tr>" % (afc_end - afc_start)
		str+= "<tr><td>Burn Flash time:</td><td>%5d sec</td></tr>"% (burn_end - burn_start)
		str+= " "
		str+= "<tr><td>Total calibration time:</td><td>%5d sec</td></tr>" % (calib_end - calib_start)
		str+= "<tr><td>Total global time (including init):</td><td>%5d sec</td></tr>" % (calib_plus_init_end - calib_plus_init_start)
		str+= "</table>"
		puts str	
	ensure
		CMUClose()
	end


end

include CalibAuto

