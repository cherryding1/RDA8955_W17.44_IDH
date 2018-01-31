require "base.rb"
require "sxs.rb"
require "lod.rb"
require "timeout"

module UartRamrun
    
    include SXS
    
    def sxsGetRPCAnswer(handle)
    	a = sxsGetNextPacket(SXS_RPC_RSP_RMC)
    	puts "[0x" + a.slice(4..8).reverse.map{ |x| "%02X" % x}.to_s + "]"
    end
    
    def sxsUartLd(sxsconnection,filename,bool_verify = false)
        
        packets = LOD_Read( filename )
        packets.each { |packet|
            i=0
			while(i<packet.data.size)
			
				data = packet.data.slice(i..(i+0x1FFF))
								
				puts "Writing block at : %08x, size : %08X" % [( (packet.address+i) | 0x80000000 ),data.size]
				
				sxsWrite(sxsconnection,( (packet.address+i) | 0x80000000 ),data)
				sxsGetDataRMCAnswer(sxsconnection,true)
				
				i += 0x2000
			end
			puts "===================================" 
        }
    	
    	return if(!bool_verify)
    		
    	packets.each { |packet|
			i=0
			while(i<packet.data.size)
			
				data = packet.data.slice(i..(i+0x1FFF))
				
				puts "Reading block at : 0x%08X, size : 0x%08X" % [( (packet.address+i) | 0x80000000 ), data.size]
				sxsReadRequest32(sxsconnection,( (packet.address+i) | 0x80000000 ),data.size)
				
			  # Wait for DATA packet					
				p = sxsGetDumpPacket(sxsconnection,:uartMode)[0]
				# Wait for "OK" packet
				sxsGetDataRMCAnswer(sxsconnection,true)
				
				if(p == data)
					puts "[VERIFY OK]"
				else
					raise "Verify FAILED"
				end
				i += 0x2000
			end
			puts "==================================="
    	}	
    	
    end
    
end

addHelpEntry("ramrun", "uartRamrun", "uartComPortNum,filename,verify=false", "", "Performs a ramrun of the file [filename] on the uart \
    device plugged on the COM port [uartComPortNum]. \
    If verify is true, the downloaded code integrity will be checked before being launched.")
def uartRamrun(uartComPortNum, fileToRun, verify = false)
    
    include UartRamrun
    include SXS

    br = 921600    
    
    uartConnection = CHUartConnection.new( uartComPortNum, br )
    uartConnection.open(true)
    uartSxsConnection = CHBPConnection.new( uartConnection , [SXS_WRITE_RMC, SXS_READ_RMC, SXS_DATA_RMC, SXS_DUMP_RMC, SXS_RPC_RMC, SXS_EXIT_RMC, SXS_RPC_RSP_RMC]) 
    uartSxsConnection.open(false)
    
    begin
        #puts "Restarting chip"
        #puts "### TODO : Remove the host dependancy (the restart command uses the current host connection)"
        #restart false,true,true
        #sleep(0.05)
        
        begin
            puts "Launching auto bauding"
            uartConnection.launchTargetAutoBauding()
            # Wait for "OK" packet
            sxsGetDataRMCAnswer(uartSxsConnection,true)
        rescue Exception => e
            puts "*** Make sure you did set your boot mode with KEYIN_1 (BOOT_MODE_FORCE_MONITOR=1), KEYIN_2 (BOOT_MODE_UART_MONITOR_ENABLE=1) and KEYIN_3 (BOOT_MODE_USB_MONITOR_DISABLE=1) pulled up, and RESTART MANUALLY before launching this function."
            puts "(On Greenstone_eng_ct1010, put the SW3 5,6 and 7 buttons to 'on' and RESTART MANUALLY)."
            raise e            
        end
        
        puts "Loading : " + fileToRun
        sxsUartLd(uartSxsConnection, fileToRun, verify)
        
        puts "Requesting start address..."
        sxsReadRequest32(uartSxsConnection,$host_monitor_control.execution_context.pc.address,4)
	
				# Wait for data.
				p = sxsGetDumpPacket(uartSxsConnection,:uartMode)[0]

				# Get "OK"
				sxsGetDataRMCAnswer(uartSxsConnection,true)
        
				start_addr = p[0] | (p[1]<<8) | (p[2]<<16) | (p[3]<<24) 
				
        puts "Starting XCPU_MAIN at 0x%08X" % start_addr        
        # Send Execute command
        sxsExecute(uartSxsConnection,start_addr,SXS_RPC_RETURN_VOID,[SXS_RPC_VALUE_PARAM,SXS_RPC_NO_PARAM,SXS_RPC_NO_PARAM,SXS_RPC_NO_PARAM],[0,0,0,0])
        # Wait for "OK" packet
        sxsGetDataRMCAnswer(uartSxsConnection,true) 
        
        puts "Ramrun Done."   

    ensure     
        uartConnection.close
        uartSxsConnection.close
    end

end
