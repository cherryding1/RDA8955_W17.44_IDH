require "sxs"

include SXS

class ProfilerControl
    DEFAULT_BUFFER_SIZE = 1024*512

    PROFILE_IN_RAM      = :inRam
    PROFILE_ON_TRACE    = :onTrace

    # Properties of the allocated buffer
    @@bufferAddress     = 0
    @@bufferSize        = DEFAULT_BUFFER_SIZE


    # setter
    def self.bufferSize=(size)
        @@bufferSize = size
    end
    def self.bufferAddress=(size)
        @@bufferAddress = size
    end

    # getter
    def self.bufferSize()
        return @@bufferSize
    end
    def self.bufferAddress()
        return @@bufferAddress
    end

    

end

def profilerControlReset()
    ProfilerControl::bufferAddress  = 0
    ProfilerControl::bufferSize     = 0
end

# Return true if code is not crashed, in which case 
# SX remote feature (sxs_execute, sxs_RB, ...) can be used
def codeIsNotCrashed(myCon)
    if (($xcpu_error_info.cpu_sp_context.read(myCon) == 0)     &&
        (($XCPU.cp0_Cause.read(myCon) & 0x7FFFFFFF) == 0)      &&
        ($INT_REG_DBG_HOST.CTRL_SET.Force_BP_XCPU.read(myCon) == 0)     &&
        ($SYS_CTRL.XCpu_Dbg_BKP.Stalled.read(myCon) == 0)
       )
        return true
    else
        return false
    end
end
    

# Allocate the Remote Buffer use to profile in RAM.
def profilerMallocRamBuffer(size=ProfilerControl::DEFAULT_BUFFER_SIZE, connection=$CURRENTCONNECTION)

    CRVERBOSE("Malloc Profiling Ram Buffer", 2)

    myCon           = CHEmptyConnection.new()
    sxsConnection   = CHEmptyConnection.new()

    if (ProfilerControl::bufferAddress != 0)
        # Buffer already allocated.
        raise MultiProfilerExceptionBufferAlreadyAllocated, "Ram buffer already allocated at:%0x8" % [ProfilerControl::bufferAddress]
    end

    # Duplicate connection.
    myCon = connection.copyConnection()
    myCon.open(false)

  	sxsConnection = CHBPConnection.new(myCon, [SXS_RPC_RMC,  SXS_RPC_RSP_RMC])
  	sxsConnection.open(false)
    

    if (codeIsNotCrashed(myCon))
        # Get and check malloc function.
        mallocFunctionAddress = $map_table.>(myCon).hal_access.>(myCon).profileControl.mallocRamBuffer.read(myCon)
        if (mallocFunctionAddress == 0)
            raise MultiProfilerExceptionUnsupportedEmbeddedCode, "Embedded Code not compiled with the necessary flags. See http://atlas/twiki/bin/view/CoolWiki/MultiProfilerPlugin#Usage"
        end

        # Allocate buffer remotely.
        sxsExecute(sxsConnection,
    				mallocFunctionAddress,
    				SXS_RPC_RETURN_VALUE,
    				[SXS_RPC_VALUE_PARAM,SXS_RPC_NO_PARAM,SXS_RPC_NO_PARAM,SXS_RPC_NO_PARAM],
    				[size].from32to8bits)
    else
        raise MultiProfilerExceptionSystemCrashed, "Embedded System Crashed, cannot profile it."
    end
    
    # Hypothesis: Malloc succeeded
    ProfilerControl::bufferAddress = sxsWaitExecuteResult(sxsConnection, 8.0)
    ProfilerControl::bufferSize = size if (ProfilerControl::bufferAddress() != 0)


ensure
    sxsConnection.close()
    myCon.close()
end


# Free the Remote Buffer use to profile in RAM.
def profilerFreeRamBuffer(connection=$CURRENTCONNECTION)

    CRVERBOSE("Malloc Profiling Ram Buffer", 2)

    myCon           = CHEmptyConnection.new()
    sxsConnection   = CHEmptyConnection.new()

    if (ProfilerControl::bufferAddress == 0)
        # Buffer already freed.
        raise MultiProfilerExceptionBufferAlreadyFreed, "Ram buffer already freed"
    end

    # Duplicate connection.
    myCon = connection.copyConnection()
    myCon.open(false)

  	sxsConnection = CHBPConnection.new(myCon, [SXS_RPC_RMC,  SXS_RPC_RSP_RMC])
  	sxsConnection.open(false)
    
    if (codeIsNotCrashed(myCon))
        # Get and check free function.
        freeFunctionAddress = $map_table.>(myCon).hal_access.>(myCon).profileControl.freeRamBuffer.read(myCon)
        if (freeFunctionAddress == 0)
            raise MultiProfilerExceptionUnsupportedEmbeddedCode, "Embedded Code not compiled with the necessary flags. See http://atlas/twiki/bin/view/CoolWiki/MultiProfilerPlugin#Usage"
        end

      	sxsExecute(sxsConnection,
                   freeFunctionAddress,
                   SXS_RPC_RETURN_VOID,
                   [SXS_RPC_NO_PARAM,SXS_RPC_NO_PARAM,SXS_RPC_NO_PARAM,SXS_RPC_NO_PARAM],
                   [])
    else
        raise MultiProfilerExceptionSystemCrashed, "Embedded System Crashed, cannot free the buffer. (Not that it matters anymore ...)"
    end


ensure
    profilerControlReset()
    sxsConnection.close()
    myCon.close()
end

# Enable/Disable the Profiling on Ram
def profilerEnableProfiling(enable, mode=ProfilerControl::PROFILE_IN_RAM, levelmask = 0xFFFF, connection=$CURRENTCONNECTION)
    myCon           = CHEmptyConnection.new()

    # Duplicate connection.
    myCon = connection.copyConnection()
    myCon.open(false)

    # Build accessor (dereference in the wind to gain some time).
    pc = $map_table.>(myCon).hal_access.>(myCon).profileControl

    val = pc.config.read(myCon)
    val = pc.config.prepl(val)

    case mode
        when ProfilerControl::PROFILE_IN_RAM
            val = pc.config.Global_Enable_Ram.wl(val, (enable)?(1):(0))
    
        when ProfilerControl::PROFILE_ON_TRACE
            val = pc.config.Global_Enable_Trace.wl(val, (enable)?(1):(0))
    end
    
    val = pc.config.Enable_Pxts_Tag_Reserved_0.wl(val,      (levelmask>>0)&1 )
    val = pc.config.Enable_Pxts_Tag_Boot.wl(val,            (levelmask>>1)&1 )
    val = pc.config.Enable_Pxts_Tag_Hal.wl(val,             (levelmask>>2)&1 )
    val = pc.config.Enable_Pxts_Tag_Sx.wl(val,              (levelmask>>3)&1 )
    val = pc.config.Enable_Pxts_Tag_Pal.wl(val,             (levelmask>>4)&1 )
    val = pc.config.Enable_Pxts_Tag_Edrv.wl(val,            (levelmask>>5)&1 )
    val = pc.config.Enable_Pxts_Tag_Svc.wl(val,             (levelmask>>6)&1 )
    val = pc.config.Enable_Pxts_Tag_Stack.wl(val,           (levelmask>>7)&1 )
    val = pc.config.Enable_Pxts_Tag_Csw.wl(val,             (levelmask>>8)&1 )
    val = pc.config.Enable_Pxts_Tag_Reserved_9.wl(val,      (levelmask>>9)&1 )
    val = pc.config.Enable_Pxts_Tag_Reserved_10.wl(val,     (levelmask>>10)&1 )
    val = pc.config.Enable_Pxts_Tag_Reserved_11.wl(val,     (levelmask>>11)&1 )
    val = pc.config.Enable_Pxts_Tag_Bb_Irq.wl(val,          (levelmask>>12)&1 )
    val = pc.config.Enable_Pxts_Tag_Spal.wl(val,            (levelmask>>13)&1 )
    val = pc.config.Enable_Pxts_Tag_Spp.wl(val,             (levelmask>>14)&1 )
    val = pc.config.Enable_Pxts_Tag_Spc.wl(val,             (levelmask>>15)&1 )

    # Clear status if start
    if (enable)
        pc.status.write(myCon, 0)
    end

    # Write register
    pc.config.write(myCon, val)

ensure
    myCon.close()
end

# Dumper function.
def profilerDumpRamBuffer(forceHwRead=false, connection=$CURRENTCONNECTION)

    CRVERBOSE("Dump Profiling Ram Buffer", 2)

    myCon           = CHEmptyConnection.new()
    sxsConnection   = CHEmptyConnection.new()

    # Duplicate connection.
    myCon = connection.copyConnection()
    myCon.open(false)

  	sxsConnection = CHBPConnection.new(connection, [SXS_READ_RMC, SXS_DUMP_RMC])
  	sxsConnection.open(false)

    # Build accessor (dereference in the wind to gain some time).
    pc = $map_table.>(myCon).hal_access.>(myCon).profileControl
 
    # Get size and address of the buffer (It can be set by embedded code without
    # knowledge of remote script)
    ProfilerControl::bufferAddress = pc.startAddr.read(myCon)
    ProfilerControl::bufferSize  = pc.size.read(myCon)


    # If we had wrapped, we need the whole file.
    if (pc.status.wrapped.read(myCon) == 1)
        size = ProfilerControl::bufferSize
    else
        writePointer = pc.writePointer.read(myCon)
        size = writePointer - ProfilerControl::bufferAddress
    end

    puts "html> Start dumping data ... (May take a while)"

    puts "Size #{size}, addr:%08x" % [ProfilerControl::bufferAddress]
    
    # Check is code is not crashed, and the CPU is not stalled.
    if (codeIsNotCrashed(myCon) && !forceHwRead)
        capturedData = (sxsRB(sxsConnection, ProfilerControl::bufferAddress, size)).from8to32bits
        puts "html> Dump done."
        return capturedData
    else
        puts "html> SX Dump failed. Fallback on hw."
        capturedData = (myCon.readBlock(ProfilerControl::bufferAddress, size){|p| mpSetProgress(p*100)} ).from8to32bits
        puts "html> Dump done."
        return capturedData
    end

ensure
    sxsConnection.close()
    myCon.close()
end


# Display the control structure in the register watcher
def profilerDisplayControl()
    controlPanel = "$map_table.>.hal_access.>.profileControl"

    # Check if already displayed
    inwin=cwGetRootWatches
    if (!(inwin.include?(controlPanel)))
        # Add to watch
        cwAddWatch(eval(controlPanel.gsub(/\.>/,".derefAt(4)")))
    end

    # Refresh
    cwRefreshWatches
end


# Gets the Profiler Control Status
def profilerGetStatus(connection=$CURRENTCONNECTION)
    hash = {}
    myCon           = CHEmptyConnection.new()

    # Duplicate connection.
    myCon = connection.copyConnection()
    myCon.open(false)

    # Build status.
    if ($map_table.>(myCon).hal_access.>(myCon).profileControl.status.wrapped.read(myCon) == 1)
        hash[:wrapped] = true;
        writePointer = $map_table.>(myCon).hal_access.>(myCon).profileControl.writePointer.read(myCon)
        # Index refers to a 32 bits array
        hash[:wrapIndex] = (writePointer - ProfilerControl::bufferAddress)/4
    else
        # No wrap
        hash[:wrapIndex] = 0
    end
    
    if ($map_table.>(myCon).hal_access.>(myCon).profileControl.status.overflowed.read(myCon) == 1)
        hash[:overflowed] = true
    end
    

    return hash
ensure
    myCon.close()
end



