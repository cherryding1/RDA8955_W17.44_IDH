require 'Calib/calibauto.rb'
require 'Calib/calib_gui_icons'
require 'Calib/calibplugin.rb'

# The main calibration gui mechanism is held in Coolwatcher
# as modalism is not stable.
# Qt windows through the Qtruby gems are only to be used to 
# set parameters. They cannot call functions outside their
# range.

module CalibGui

    # Build Interface, setting icons in the desi(ah ah ah )ired order
    # Configure automatic calibration
    #cwAddScriptButton("CalibGuiToolbar", "calibGuiSHAuto()", CalibGuiIconLib::CALIB_AUTO_CONFIG)
 
    # Start automatic calibration
    cwAddScriptButton("CalibGuiToolbar", "calibGuiRunCalibAuto()", CalibGuiIconLib::CALIB_AUTO_START,"Run automatic calibration")

    # Open CFP
    cwAddScriptButton("CalibGuiToolbar", "calibGuiOpenCalibration()", CalibGuiIconLib::CALIB_OPEN_CFP,"Open calibration")

    # Save CFP
    cwAddScriptButton("CalibGuiToolbar", "calibGuiSaveCalibration()", CalibGuiIconLib::CALIB_SAVE_TO_FILE,"Save calibration")

    # Restore Flash Parameters
    cwAddScriptButton("CalibGuiToolbar", "calib_reset_parameters()", CalibGuiIconLib::CALIB_RESTORE_FROM_FLASH,"Reset calibration parameters")

    # Update parameters
    cwAddScriptButton("CalibGuiToolbar", "calib_update()", CalibGuiIconLib::CALIB_UPDATE,"Calibration update")

    # Display Version Numbers
    # (calib_init parameter is set to true, first two parameters are ignored)
    cwAddScriptButton("CalibGuiToolbar", "puts calib_get_versions(true, false, true)", CalibGuiIconLib::CALIB_VERSIONS,"Get calibration versions")

    # Display Help
    cwAddScriptButton("CalibGuiToolbar", "help \"Calibration\"", CalibGuiIconLib::CALIB_HELP,"Print calibration help")

    # Creates a menu for the calibration.
    cwAddMenuCommand("Automatic Calib", "Launch Automatic Calibration", "calibGuiRunCalibAuto()", 0)
    cwAddMenuCommand("Automatic Calib", "", "", 0)
    cwAddMenuCommand("Automatic Calib", "Choose CFP File...", "calibGuiOpenCalibration()", 0)
    cwAddMenuCommand("Automatic Calib", "Save Calib Param from RAM to CFP File", "calibGuiSaveCalibration()", 0)
    cwAddMenuCommand("Automatic Calib", "", "", 0)
    cwAddMenuCommand("Automatic Calib", "Burn Calib Param from RAM to Flash", "calib_burn_flash()", 0)
    cwAddMenuCommand("Automatic Calib", "Erase Calib Param from Flash", "calib_erase_flash()", 0)
    cwAddMenuCommand("Automatic Calib", "", "", 0)
    cwAddMenuCommand("Automatic Calib", "Choose CFP File to burn to Flash...", "calibGuiChoseCFPToBurnToFlash", 0)
    cwAddMenuCommand("Automatic Calib", "Burn CFP File to Flash", "calibGuiBurnCfp()", 0)
    # Burn a CFP
    cwAddScriptButton("CalibBurnToolbar", "calibGuiChoseCFPToBurnToFlash()", CalibGuiIconLib::CALIB_CHOSE_CFP,"Chose calibration parameters to burn")
    cwAddScriptButton("CalibBurnToolbar", "calibGuiBurnCfp()", CalibGuiIconLib::CALIB_BURN_CFP,"Burn calibration parameters")

    cwAddMenuCommand("Manual Calib", "Enter Calibration Stub", "calib_start_stub", 0)
    cwAddMenuCommand("Manual Calib", "", "", 0)
    cwAddMenuCommand("Manual Calib", "Stop All Modes", "calib_mode_stop", 0)
    cwAddMenuCommand("Manual Calib", "", "", 0)
    cwAddMenuCommand("Manual Calib", "Start Mode Rx Power Monitoring", "calib_mode_rx_power(1, 32, 85)", 0)
    cwAddMenuCommand("Manual Calib", "Start Mode Rx Freq Monitoring", "calib_mode_rx_freq(1, 32, 85)", 0)
    cwAddMenuCommand("Manual Calib", "Start Mode Tx Send Burst Value", "calib_mode_tx_pa_val(1, 32, 600, 0)", 0)
    cwAddMenuCommand("Manual Calib", "Start Mode Tx Send Burst PCL", "calib_mode_tx_pcl(1, 32, 19, 0)", 0)
    cwAddMenuCommand("Manual Calib", "Start Mode Audio Out", "calib_mode_audio_out(0, 0, 7, 7, 170, 128, true)", 0)
    cwAddMenuCommand("Manual Calib", "", "", 0)
    cwAddMenuCommand("Manual Calib", "Get Rx Monitoring Power", "puts calib_get_rx_mon_power", 0)
    cwAddMenuCommand("Manual Calib", "Get Rx NBurst Power", "puts calib_get_rx_nb_power", 0)
    cwAddMenuCommand("Manual Calib", "Get Rx Freq Offset", "puts calib_get_rx_fof", 0)
    cwAddMenuCommand("Manual Calib", "", "", 0)
    cwAddMenuCommand("Manual Calib", "Reset Calib Param from Code's Default", "calib_reset_parameters()", 0)
    cwAddMenuCommand("Manual Calib", "Update Calib Param (update from HST RAM to RAM)", "calib_update()", 0)
    cwAddMenuCommand("Manual Calib", "", "", 0)
    cwAddMenuCommand("Manual Calib", "Get Calibration Versions", "puts calib_get_versions(true, false, true)", 0)
    cwAddMenuCommand("Manual Calib", "Get RF Chip Names", "calib_get_rf_chip_names.each {|i| xp i}", 0)
    cwAddMenuCommand("Manual Calib", "Calibration Help", "help \"Calibration\"", 0)


    def calibGuiSHAuto()
    
    end


    # Automatic Calibration Run
    def calibGuiRunCalibAuto()
        # enter the stub and run
        params = calibPluginGetConfigFromGui()
        calib_start_stub()
        calib_auto(params)
    end
   
    # Calibration parameters save to file
    def calibGuiSaveCalibration()
        file = cwGetSaveFileName("Chose file to save","","Calibration Flash Parameters(*.cfp)")
        if(file=="?") #User canceled
            return
        end
        calib_dump_parameters(file,false)
    end


    # Open a CFP file and apply the corresponding parameters
    def calibGuiOpenCalibration()
        file = cwGetOpenFileName("Chose file to open","","Calibration Flash Parameters(*.cfp)")
        if(file=="?") #User canceled
            return
        end
        calib_load_parameters(file,false)
    end
    
    def calibGuiChoseCFPToBurnToFlash()
        lastCfpForCalibFlashBurn  = cwGetProfileEntry("lastCfpForCalibFlashBurn", "")
        file = cwGetOpenFileName("Choose CFP file for Flash burning", lastCfpForCalibFlashBurn, "*.cfp")
        if(file=="?")
            return
        end
        lastCfpForCalibFlashBurn = file
        cwSetProfileEntry("lastCfpForCalibFlashBurn", lastCfpForCalibFlashBurn)
        puts "html>CFP file for Flash burning set to: <br><i>%s</i>" % lastCfpForCalibFlashBurn
    end
    
    # Burn a CFP file
    def calibGuiBurnCfp()
        lastCfpForCalibFlashBurn = cwGetProfileEntry("lastCfpForCalibFlashBurn", "")
        #Use fastpf flash programmer instead of asking one
        lastFlashProgForFastpf = cwGetProfileEntry("lastFlashProgForFastpf", "")
        if(lastCfpForCalibFlashBurn == "")
            puts "No file chosen for fastpfization!"
            return
        end
        if(lastFlashProgForFastpf == "")
            puts "No flash programmer chosen for fastpfization!"
            return
        end
        fastpf(lastFlashProgForFastpf,lastCfpForCalibFlashBurn,FastPf::NOBOOTSECTORFASTPF)
    end


end

include CalibGui

