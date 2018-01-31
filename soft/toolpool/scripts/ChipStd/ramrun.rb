
require "base.rb"
require "help.rb"
require "target_executor.rb"

module Ramrun
   
public   
    #Low level function, but highly configurable
    def doramrunXfer(connection,lodPackets,verify=false)
        # Open a target executor session
        TargetXtor.session(connection) {         
            #puts "Downloading code..."
                   
            #Writes the Ramrun Lodfile in the phone's RAM memory, this is: internal SRAM, internal BB SRAM
            #and external SRAM.
            begin
                ldPackets(connection,lodPackets,verify)
            rescue LdVerifyFailed
                puts "Verify failed during ramrun! Continuing at YOUR own risk!"
            end

            #Set execution command
            $host_monitor_control.execution_context.cmd_type.write(connection,TargetExecutor::HST_EXECUTOR_X_CMD)
 
            #puts "Starting code..."
           
            #Launch execution command
            $INT_REG_DBG_HOST.H2P_STATUS.write(connection,TargetExecutor::HST_EXECUTOR_X_CMD)
            
            sleep(0.1)
            
            #puts "Ramrun done."     
        }
    end

    #High level function
    def doramrun(str_filename, verify=false)
        packets = LOD_Read(str_filename)
        begin
            connection = $CURRENTCONNECTION.copyConnection()
            connection.open(false)
            doramrunXfer(connection,packets,verify)
        ensure
            connection.close()
        end
    end
    

end

include Ramrun

addHelpEntry("ramrun", "ramrun", "filename,verify=false", "", "Performs a ramrun of the file [filename] on the current opened device. \
    If verify is true, the downloaded code integrity will be checked before being launched.")
def ramrun(str_filename, verify=false)
    Ramrun.doramrun(str_filename, verify)
end


addHelpEntry("ramrun", "ramrunXfer", "connection,lodPackets,verify=false", "", "Performs a ramrun of the 'lodPackets' on the device 'connection'. The 'lodPackets' are likely to have been loaded with the LOD_Load(...) function provided with the CoolHost module. \
    If verify is true, the downloaded code integrity will be checked before being launched.")
def ramrunXfer(connection,lodPackets,verify=false)
    Ramrun.doramrunXfer(connection,lodPackets, verify)
end
