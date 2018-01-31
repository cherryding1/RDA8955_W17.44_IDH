#!/usr/bin/env ruby
include CoolHost

addHelpEntry("breakpoint", "xsbp", "mode, address", "", "Set a \
    breakpoint of type 'mode' at 'address' for the XCPU")
def xsbp(mode,address)

	case mode

	when /i|I/
		$SYS_CTRL.XCpu_Dbg_BKP.BKPT_Mode.w(0)
	when /rw|RW/
		$SYS_CTRL.XCpu_Dbg_BKP.BKPT_Mode.w(3)
	when /r|R/
		$SYS_CTRL.XCpu_Dbg_BKP.BKPT_Mode.w(1)
	when /w|W/
		$SYS_CTRL.XCpu_Dbg_BKP.BKPT_Mode.w(2)
	else
		puts "Use mode \"i\",\"r\",\"w\", or \"rw\""
		return
	end

	$SYS_CTRL.XCpu_Dbg_Addr.Breakpoint_address.w(address)
	$SYS_CTRL.XCpu_Dbg_BKP.BKPT_En.w(1)
end

addHelpEntry("breakpoint", "bsbp", "mode, address", "", "Set a \
    breakpoint of type 'mode' at 'address' for the BCPU")
def bsbp(mode,address)

	case mode

	when /i|I/
		$SYS_CTRL.BCpu_Dbg_BKP.BKPT_Mode.w(0)
	when /rw|RW/
		$SYS_CTRL.BCpu_Dbg_BKP.BKPT_Mode.w(3)
	when /r|R/
		$SYS_CTRL.BCpu_Dbg_BKP.BKPT_Mode.w(1)
	when /w|W/
		$SYS_CTRL.BCpu_Dbg_BKP.BKPT_Mode.w(2)
	else
		puts "Use mode \"i\",\"r\",\"w\", or \"rw\""
		return
	end

	$SYS_CTRL.BCpu_Dbg_Addr.Breakpoint_address.w(address)
	$SYS_CTRL.BCpu_Dbg_BKP.BKPT_En.w(1)
end
