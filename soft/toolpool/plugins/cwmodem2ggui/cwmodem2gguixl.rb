###############################################################################
#                                                                            ##
#            Copyright (C) 2003-2007, Coolsand Technologies, Inc.            ##
#                            All Rights Reserved                             ##
#                                                                            ##
#      This source code is the property of Coolsand Technologies and is      ##
#      confidential.  Any  modification, distribution,  reproduction or      ##
#      exploitation  of  any content of this file is totally forbidden,      ##
#      except  with the  written permission  of  Coolsand Technologies.      ##
#                                                                            ##
###############################################################################
#
#  $HeadURL$
#  $Author$
#  $Date$
#  $Revision$
#
###############################################################################
#
#  Functions to create a GUI for CoolWatcher adapted for Modem2G.
#  This GUI must be loaded after the CW default GUI.
#
###############################################################################

require "CoolWatcher/coolwatcher"
require "CoolWatcher/cwiconlib.rb"
require "help"
require "versions"
require "debug"
require "uart_ramrun"
require "cpu_debug"
require "hardware_versions"
require "disassembly"


def addXCpuWatches()
    cwInsertWatchesFromStringArray([ \
        \
        ":RubyLabel:XCPU Program Counter" + \
        ":RubyExpression:" + \
        "regval = $XCPU.rf0_addr.read($TOOLCONNECTIONS[$CWCREGWATCH].connection);" + \
        "funcname = 'NoDisassemblyLoaded' ; enforce { funcname = regval.functionAt };" + \
        "'%s (0x%x)' %[funcname, regval];", \
        \
        ":RubyLabel:XCPU Return Address" + \
        ":RubyExpression:" + \
        "regval = $XCPU.Regfile_RA.read($TOOLCONNECTIONS[$CWCREGWATCH].connection);" + \
        "funcname = 'NoDisassemblyLoaded' ; enforce { funcname = regval.functionAt };" + \
        "'%s (0x%x)' %[funcname, regval];", \
        \
        ":RubyLabel:XCPU Exception Pointer" + \
        ":RubyExpression:" + \
        "regval = $XCPU.cp0_EPC.read($TOOLCONNECTIONS[$CWCREGWATCH].connection);" + \
        "funcname = 'NoDisassemblyLoaded' ; enforce { funcname = regval.functionAt };" + \
        "'%s (0x%x)' %[funcname, regval];", \
        \
        "$XCPU.cp0_Cause", \
        \
        ":RubyLabel:BCPU Program Counter" + \
        ":RubyExpression:" + \
        "regval = $BCPU.rf0_addr.read($TOOLCONNECTIONS[$CWCREGWATCH].connection);" + \
        "funcname = 'NoDisassemblyLoaded' ; enforce { funcname = regval.functionAt };" + \
        "'%s (0x%x)' %[funcname, regval];", \
        ]);
end

def addBCpuWatches()
    cwInsertWatchesFromStringArray(["$BCPU.rf0_addr", "$BCPU.cp0_Cause", "$BCPU.cp0_EPC"]);
end

addHelpEntry("CoolWatcher", "cwBuildModem2GGui", "","", "Setup the Modem2G 
        interface for CoolWatcher.")
    
def cwBuildModem2GGui
    # Setup some menus.
    cwAddMenuCommand("Chip", "Turn off the chip", "turnoff()", 0)
    cwAddMenuCommand("Chip", "Restart the chip", "restart(false, true)", 0)
    cwAddMenuCommand("Chip", "Restart and freeze the chip", "restart(true, true)", 0)
    cwAddMenuCommand("Chip", "Unfreeze the chip", "unfreeze()", 0)
    cwAddMenuCommand("Chip", "", "", 0)
    cwAddMenuCommand("Chip", "Turn on the lights", "$PWM.PWL0_Config.PWL0_En_H.set ; $PWM.PWL0_Config.PWL0_Force_H.set ; $PWM.PWL1_Config.PWL1_En_H.set ; $PWM.PWL1_Config.PWL1_Force_H.set", 0)
    cwAddMenuCommand("Chip", "Turn off the lights", "$PWM.PWL0_Config.PWL0_En_H.set ; $PWM.PWL0_Config.PWL0_Force_L.clear ; $PWM.PWL1_Config.PWL1_En_H.set ; $PWM.PWL1_Config.PWL1_Force_L.clear", 0)
    cwAddMenuCommand("Chip", "Show HW timer", "cwAddWatch($TIMER.HWTimer_CurVal) ; cwRefreshWatches", 0)
    cwAddMenuCommand("Chip", "", "", 0)
    cwAddMenuCommand("Chip", "Get hardware versions", "hver()", 0)
    cwAddMenuCommand("Chip", "Get software module versions", "sver()", 0)
    cwAddMenuCommand("Chip", "", "", 0)
    cwAddMenuCommand("Chip", "Get date and time", "date()", 0)
    cwAddMenuCommand("Chip", "Get system frequencies", "freq()", 0)
    cwAddMenuCommand("Chip", "Get low power mode status", "lowpower()", 0)
    cwAddMenuCommand("Chip", "Get calibration status", "calib()", 0)
    cwAddMenuCommand("Chip", "Get PXTS/EXL configuration", "getDbgPortConfig()", 0)
    cwAddMenuCommand("Chip", "", "", 0)
    cwAddMenuCommand("Chip", "Check For Greenstone AHB Master Bug", "checkForGreenstoneAhbMasterBug()", 0)

    # Setup some toolbar.
    cwAddScriptButton('cwtarget', "restart(false, true)", CWIconLib::RESTARTICON,"Restart chip")
    cwAddScriptButton('cwtarget', "turnoff()", CWIconLib::OFFICON,"Turn off chip")
    
    cwAddRegWatchButton("addXCpuWatches &",CWIconLib::REGWATCH,"XCpu");
#    cwAddRegWatchButton("addBCpuWatches &",CWIconLib::REGWATCH,"BCpu");
    
end



cwBuildModem2GGui();


