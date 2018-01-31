require "sxs"

include SXS

class Keypad
    @@keypadHandlerMutex = Mutex.new

    def self.keypadHandlerMutex
        @@keypadHandlerMutex    
    end
end

def keypadSendKeyToRemote(val,state,connection=$CURRENTCONNECTION)

    #For debugging purpose
    case state
        when Keypad::KPDOWN
            CRVERBOSE("+ %c " % val,2)
        when Keypad::KPHOLD
            CRVERBOSE("= %c " % val,2)
        when Keypad::KPUP
            CRVERBOSE("- %c " % val,2)
    end

  	sxsConnection = CHBPConnection.new(connection, [SXS_RPC_RMC,  SXS_RPC_RSP_RMC])
  	sxsConnection.open(false)
     	
    Keypad::keypadHandlerMutex().synchronize{    
     	begin
     		sxsExecute(sxsConnection,
     								$map_table.>(connection).hal_access.>(connection).keypadPulseHandler.read(connection),
     								SXS_RPC_RETURN_VOID,
     								[SXS_RPC_VALUE_PARAM,SXS_RPC_VALUE_PARAM,SXS_RPC_NO_PARAM,SXS_RPC_NO_PARAM],
     								[val,state].from32to8bits)
     	ensure
     		sxsConnection.close()
     	end
    }
end
