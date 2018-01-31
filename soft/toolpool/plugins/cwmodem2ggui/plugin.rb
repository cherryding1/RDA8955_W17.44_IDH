
filename = File.dirname(__FILE__)+"/cwmodem2gguixl.rb"
#puts filename

begin
    if(File.exists?(filename))
        require "cwmodem2ggui/cwmodem2gguixl.rb"
    else
        require "cwmodem2ggui/cwmodem2ggui.rb"
    end
    
    require "cwmodem2ggui/m2gevents.rb"
    
rescue Exception => e 
    puts "Failed to load plugin!"
    #puts e.inspect
    #puts e.backtrace
end