require 'rom.rb'
require 'romburn/romburnicons.rb'


module RomburnGui


begin
    cwAddScriptButton("Rom Burn", "romburnChooseLod()", RomburnIcons::ROMCHOOSE, "Choose LOD ROM file...")
    cwAddScriptButton("Rom Burn", "romburnGo()", RomburnIcons::ROMBURN, "Launch rom burning with current LOD ROM")
   
    cwAddMenuCommand("Rom Burn", "Choose LOD ROM file...", "romburnChooseLod()", 0)
    cwAddMenuCommand("Rom Burn", "", "", 0)
    cwAddMenuCommand("Rom Burn", "Launch rom burning with current LOD ROM file.", "romburnGo()", 0)
    
rescue #No CoolWatcher.
end


end
      
def romburnChooseLod()
    lastLodForRomburn  = cwGetProfileEntry("lastLodForRomburn", "")
    file = cwGetOpenFileName("Choose LOD ROM file for rom burning", lastLodForRomburn, "*.lod")
    if(file=="?")
        return
    end
    lastLodForRomburn = file
    cwSetProfileEntry("lastLodForRomburn", lastLodForRomburn)
    puts "html>Lod file for rom burning set to: <br><i>%s</i>" % lastLodForRomburn
end

def romburnGo()
    lastLodForRomburn = cwGetProfileEntry("lastLodForRomburn", "")
  
    if(lastLodForRomburn == "")
        puts "No file chosen for rom burning!"
        return
    end
    
    romBurn(lastLodForRomburn, false)
end