
require "base.rb"
require "help.rb"
require "target_executor.rb"


module Opal
   
public   

    addHelpEntry("Opal","opalDebug","write_flag,reg,value","readvalue","Call the debug function of PMD to access opal registers")

def opalDebug(write_flag,reg,value)
    funcAddr=$map_table.>.pmd_access.>.accessFunction.R
    if 0 == funcAddr
        raise "opalDebug no function pointer"
    end
    # build parameter
    paramStruct=CH__PMD_OPAL_DEBUG_FUNC_PARAM_T.new(0)
    param=0;
    param=paramStruct.Param.Write_Flag.wl(param, write_flag);
    param=paramStruct.Param.Reg.wl(param, reg);
    param=paramStruct.Param.Value.wl(param, value);
    paramStruct=nil
    #xp param

    tgxtor = TargetXtor.new
    begin
        ret=tgxtor.targetExecute(funcAddr,0,param);
    ensure
        tgxtor.closeConnection
    end
    tgxtor=nil
    ret[1]
end

    addHelpEntry("Opal","opalRead","reg","readvalue","Call the debug function of PMD to read opal registers")

def opalRead(reg)
    opalDebug(0,reg,0)
end

   addHelpEntry("Opal","opalWrite","reg,value","","Call the debug function of PMD to write opal registers")
def opalWrite(reg,value)
    opalDebug(1,reg,value)
end

addHelpEntry("Opal","opalHWRead","addr,connection=$CURRENTCONNECTION","readvalue","Does a hardware read on opal register at address 'addr'.")
def opalHWRead(addr,connection=$CURRENTCONNECTION)
	$SPI.rxtx_buffer.write(connection,addr*2,   true)
	$SPI.rxtx_buffer.write(connection,0,        true)
	$SPI.rxtx_buffer.write(connection,0,        true)
	rb1 = $SPI.rxtx_buffer.read(connection,true)
	rb2 = $SPI.rxtx_buffer.read(connection,true)
	rb3 = $SPI.rxtx_buffer.read(connection,true)
	return (rb1<<16) | (rb2<<8) | (rb3)
end

addHelpEntry("Opal","opalHWWrite","addr, val, connection=$CURRENTCONNECTION","","Does a hardware wrute on opal register at address 'addr'.")
def opalHWWrite(addr, val, connection=$CURRENTCONNECTION)
	$SPI.rxtx_buffer.write(connection, (addr*2)+1,  true)
	$SPI.rxtx_buffer.write(connection, val>>8,      true)
	$SPI.rxtx_buffer.write(connection, val&0xff,    true)
#	rb1 = $SPI.rxtx_buffer.read(connection,true)
#	rb2 = $SPI.rxtx_buffer.read(connection,true)
#	rb3 = $SPI.rxtx_buffer.read(connection,true)
end

def opalHWInit(connection=$CURRENTCONNECTION)
	$SPI.cfg.write(connection,0x00190006)
	$SPI.ctrl.write(connection,0x0017A979)
	opalHWWrite(0x24,0,connection)
end


end

include Opal
