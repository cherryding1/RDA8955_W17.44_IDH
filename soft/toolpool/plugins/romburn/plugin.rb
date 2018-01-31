
module RomburnPlugin

    @@romburnActivated = false

    def RomburnPlugin.activateRomburnGui

        if (@@romburnActivated == false)
            puts "Loading RomBurn GUI ..."

            #We need the path to be set before we require anything
            load "romburn/romburn_gui.rb"
            begin

            rescue Exception => e
                puts e.to_s
            end

            @@romburnActivated = true
            puts "RomBurn GUI loaded"
        else
            puts "RomBurn GUI already loaded"
        end
        
    end

end

#CoolWatcher specific
begin
    include CoolWatcher

    def activateRomburnGui
        RomburnPlugin::activateRomburnGui()
    end
    
    begin
        cwAddMenuCommand("Plugins","Activate RomBurn","activateRomburnGui()",0) if(!$enterprisever)
    rescue Exception
        puts "*** Could not load plugin. Investigate yourself. ***"
    end
    
rescue Exception
end

