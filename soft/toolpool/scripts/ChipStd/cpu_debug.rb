#!/usr/bin/env ruby
include CoolHost

require "chip_control.rb"


# TODO Do real diag functions usings xml defined regs
addHelpEntry("chip", "diag", "", "", "Returns the status of the system CPU.")
def diag()

    # Test if CPU running
    # if
    pc     = $XCPU.rf0_addr.R
    others = RB($XCPU.cp0_Cause.address, 3)
    puts "\n XCPU:"
    puts "PC     = 0x%08X" % pc 
    puts "EPC    = 0x%08X" % others[2]
    puts "Status = 0x%08X" % others[1] 
    puts "Cause  = 0x%08X" % others[0] 

    # if not
    # todo
end



addHelpEntry("chip", "diagb", "", "", "Returns the status of the BCPU.")
def diagb()

    # Test if CPU running
    # if
    pc     = $BCPU.rf0_addr.R
    others = RB($BCPU.cp0_Cause.address, 3)
    puts "\n BCPU:"
    puts "PC     = 0x%08X" % pc 
    puts "EPC    = 0x%08X" % others[2]
    puts "Status = 0x%08X" % others[1] 
    puts "Cause  = 0x%08X" % others[0] 

    # if not
    # todo
end



addHelpEntry("chip", "freq", "", "", "Returns the current system frequencies.")
def freq()
    puts "System CPU running at:"
    $SYS_CTRL.Cfg_Clk_Sys.Freq.r
    puts "EBC running at:"    
    $SYS_CTRL.Cfg_Clk_Mem_Bridge.Freq.r
    puts "VoC running at:"
    $SYS_CTRL.Cfg_Clk_Voc.Freq.r
end

addHelpEntry("chip", "inCache", "addr, cache", "inCacheAddress", "This function read the cpu tag \
            and check if the wanted address is in the cache,
            if so it return the address of the corresponding data in the cache.
            <br/>There is two 1 parameter shortcuts as folowing exemples shows:
            <br/> *  0x80001570.inCache($XCPU_IDATA).r : this checks in the XCPU Instruction cache is the address 0x80001570 is in cache, and if found, it reads the data from the cache.
            <br/> * $BCPU_DDATA.inCache(0x80c01afc).r : this checks in the BCPU Data cache is the address 0x80c01afc is in cache, and if found, it reads the data from the cache.")
            
class NotInCacheException < RuntimeError
  attr :tagVal
  def initialize(tagVal)
    @tagVal = tagVal
  end
end

def __inCache(addr, cache)
    case cache
    when $XCPU_IDATA
        #puts "XCPU Instrunction Cache"
        cpu=$XCPU
        tag=$XCPU_TAG
        tagbf=tag.Line[0].Tag
        cpu.Debug_Page_Address.w(0)
    when $XCPU_DDATA
        #puts "XCPU Data Cache"
        cpu=$XCPU
        tag=$XCPU_TAG
        tagbf=tag.Line[0].Tag
        cpu.Debug_Page_Address.w(0x8)
    when $BCPU_IDATA
        #puts "BCPU Instrunction Cache"
        cpu=$BCPU
        tag=$BCPU_TAG
        tagbf=tag.Line[0].Tag
        cpu.Debug_Page_Address.w(0)
    when $BCPU_DDATA
        #puts "BCPU Data Cache"
        cpu=$BCPU
        tag=$BCPU_TAG
        tagbf=tag.Line[0].Tag
        cpu.Debug_Page_Address.w(0x8)
    else
        raise ArgumentError, "not a cpu Cache"
    end
    # currently assuming cpu 4 word lines of cache
    line = (addr & (tagbf.wl(0,1)-1)) >> 4;
    tagval = tag.Line[line].R
    if tagval == tag.Line[line].Valid.wl((addr & tagbf.wl(0,0xffffffff)),1) then
        offset = (addr & (tagbf.wl(0,1)-1))
        # bits upper that 12 are in Page Address
        cpu.Debug_Page_Address.w(offset >> 12)
        return cache.address + (offset & ((1<<13)-1))
    else
        raise NotInCacheException.new(tagval), "data not in cache, (tag: 0x%08X)" % tagval
    end
end

class CHStruct
    def inCache(addr)
        __inCache(addr,self)
    end
end

class Integer
    def inCache(cache)
        __inCache(self,cache)
    end
    def cacheOk(cache)
        addr=self & 0xfffffff0
        return addr.RB(4) == __inCache(addr,cache).RB(4)
    end
end
class CHValuable
    def inCache(cache)
        __inCache(self,cache)
    end
end

def inCache(a,c)
    __inCache(a,c)
end



addHelpEntry("chip", "exlEnable", "", "", "This function enable \
            EXL logging with a useful default configuration, 
            that is record all jump and branch and IRQ on 
            the XCPU. To change this, use directly the appropriate
            registers")

def exlEnable()

    # EXL configuration
    $DEBUG_PORT.Debug_Port_Mode.w 1
    $DEBUG_PORT.Exl_Cfg.Record_Trigger_Type.w 0
    $DEBUG_PORT.Exl_Cfg.Int_En.w 1
    $DEBUG_PORT.Exl_Cfg.Any_Branch.w 1
    $DEBUG_PORT.Exl_Cfg.Cpu_Select.w 0

    # Choose proper IO mode
    $IOMUX.DBG_Mode.w 1
end


