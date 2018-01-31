
def __GetLatency(mon)
    t=mon.Latency.getl;
    if mon.Latency.Access_Cnt.rl(t) == 0 then
        return 'none'
    elsif mon.Latency.Access_Cnt.rl(t) == 255 then
        return 'OV %.3f'%(1.0 * mon.Latency.Latency.rl(t) / mon.Access_Count.getl)
    else
        return '%.3f'%(1.0 * mon.Latency.Latency.rl(t) / mon.Latency.Access_Cnt.rl(t))
    end
end

class CHStruct
    def GetLatency()
        __GetLatency(self)
    end
end

def __GetAccUse(mon)
    t=mon.Access_Count.getl
    if t == 0 then
        return 'none'
    else
        return "%.3f"%(1.0*mon.Use.getl / t)
    end
end

class CHStruct
    def GetAccUse()
        __GetAccUse(self)
    end
end

def __GetUse(mon)
    t=mon.Control.getl
    case mon.Control.Time_Window.rl(t)
    when 0
        d=1024
    when 1
        d=32*1024
    when 2
        d=1024*1024
    when 3
        d=16*1024*1024
    when 4
        d=5000*$ahb_mon_qbit #1 frame is 5000 qbit
    when 5
        d=26*5000*$ahb_mon_qbit #1 frame is 5000 qbit
    when 6
        d=52*5000*$ahb_mon_qbit #1 frame is 5000 qbit
    when 7
        d=104*5000*$ahb_mon_qbit #1 frame is 5000 qbit
    end
    return "%5.2f\%"%(100.0*mon.Use.getl / d)
end


class CHStruct
    def GetUse()
        __GetUse(self)
    end
end


def setQbit
    case $SYS_CTRL.Cfg_Clk_Sys.Freq.r
    when 1: #26M
    $ahb_mon_qbit=24
    when 2: #39M
    $ahb_mon_qbit=36
    when 3: #52M
    $ahb_mon_qbit=48
    when 4: #78M
    $ahb_mon_qbit=72
    end
end

setQbit

cwAddWatch $BB_AHB_MONITOR
cwAddRubyWatch "$BB_AHB_MONITOR.GetLatency;","BB_AHB_MONITOR Latency"
cwAddRubyWatch "$BB_AHB_MONITOR.GetAccUse;","BB_AHB_MONITOR Acc Use"
cwAddRubyWatch "$BB_AHB_MONITOR.GetUse;","BB_AHB_MONITOR Use"

cwAddWatch $SYS_AHB_MONITOR
cwAddRubyWatch "$SYS_AHB_MONITOR.GetLatency;","SYS_AHB_MONITOR Latency"
cwAddRubyWatch "$SYS_AHB_MONITOR.GetAccUse;","SYS_AHB_MONITOR Acc Use"
cwAddRubyWatch "$SYS_AHB_MONITOR.GetUse;","SYS_AHB_MONITOR Use"

