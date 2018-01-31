require "base.rb"
require "sxs.rb"
require "lod.rb"
require "timeout"

def uartDump(comport,address,size)
    
    include SXS

    ret = [] 
    br = 921600    
    
    uartConnection = CHUartConnection.new( uartComPortNum, br )
    uartConnection.open(true)
    uartSxsConnection = CHBPConnection.new( uartConnection , [SXS_WRITE_RMC, SXS_READ_RMC, SXS_DATA_RMC, SXS_DUMP_RMC, SXS_RPC_RMC, SXS_EXIT_RMC, SXS_RPC_RSP_RMC]) 
    uartSxsConnection.open(false)
    
    begin
        puts "Restarting chip"
        puts "### TODO : Remove the host dependancy (the restart command uses the current host connection)"
        restart false,true,true
        sleep(0.05)
        
        puts "Launching auto bauding."
        uartConnection.launchTargetAutoBauding()
        sxsGetDataRMCAnswer(uartSxsConnection)
        
        puts "Reading data..."
        ret = sxsReadblock(uartSxsConnection,address,size)

        puts "Done."
        
    ensure     
        uartConnection.close
        uartSxsConnection.close
    end
    
    return ret

end