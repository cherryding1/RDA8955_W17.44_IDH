
module CalibPlugin

    @@calibActivated = false

    def CalibPlugin.activateCalibGui

        if (@@calibActivated == false)
            puts "Loading Calibration GUI ..."

            #We need the path to be set before we require anything
            load "Calib/calib_gui.rb"
            begin

            rescue Exception => e
                puts e.to_s
            end

            @@calibActivated = true
            puts "Calibration GUI loaded"
        else
            puts "Calibration GUI already loaded"
        end
        
    end

end

#CoolWatcher specific
begin
    include CoolWatcher

    def activateCalibGui
        CalibPlugin::activateCalibGui()
    end
    
    begin
        cwAddMenuCommand("Plugins","Activate CalibrationTool","activateCalibGui()",0) if(!$enterprisever)
    rescue Exception
        puts "*** Calib XMD not present. Calib unavailable ***"
    end
    
rescue Exception
end

