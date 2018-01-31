
cwRegisterPlugin(File.dirname(__FILE__)+"/calibplugin.dll")

include CalibPlugin
    
def calibPluginSetParameters(params)

    calibPluginSetSaveParameters(params["bool_dump_flash_parameters"], params["dump_filename"])
    calibPluginSetBurnFlash(params["bool_burn_flash"])
   
    if(params["bool_reset_param"])
        calibPluginSetInitialParameters(1, "")
    elsif(params["load_cfp"])
        calibPluginSetInitialParameters(2, "")    
    else
        #Keep current
        calibPluginSetInitialParameters(0, "")        
    end
   
    calibPluginSetGpadcCrystal(params["bool_gpadc_calib"],  params["bool_crystal_calib"])
    calibPluginSetAFCGain(params["bool_afc_gain0"], params["bool_afc_gain1"], params["bool_afc_gain2"], params["bool_afc_gain3"])
    calibPluginSetPAProfile(params["bool_pa_profile0"], params["bool_pa_profile1"], params["bool_pa_profile2"], params["bool_pa_profile3"])
    calibPluginSetPAOffset(params["bool_pa_offset0"], params["bool_pa_offset1"], params["bool_pa_offset2"], params["bool_pa_offset3"])
    calibPluginSetILoss(params["bool_iloss0"], params["bool_iloss1"], params["bool_iloss2"], params["bool_iloss3"])
    calibPluginSetIP2(params["bool_ip2_0"], params["bool_ip2_1"], params["bool_ip2_2"], params["bool_ip2_3"])
    calibPluginEquipmentSetInputLoss(params["input_loss"][0], params["input_loss"][1], params["input_loss"][2], params["input_loss"][3])
    calibPluginEquipmentSetOutputLoss(params["output_loss"][0], params["output_loss"][1], params["output_loss"][2], params["output_loss"][3])
    calibPluginEquipmentSetFofOffset(params["int_fof_offset"]) 
       
rescue Exception => e
    puts e.to_s
end

calibPluginSetParameters(calib_auto_get_default_config())
