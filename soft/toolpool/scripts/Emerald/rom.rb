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
#  Functions to burn the ROM of Granite on the FPGA platform. (If you don't
#  understand this, don't use this script.)
#
###############################################################################



addHelpEntry("rom", "romBurn","file,verify=false","", "Load the internal ROM \
        whose lod file is [file] if specified or the latest from the latest \
        FPGA release directory into the FPGA. The [verify] parameter is \
        optional. Set it to true to force the ROM checking (longer).")
        
def romBurn(file="//atlas/n/projects/Granite/laurent/Granite/soft/hex/int_rom_programmer_emerald_fpga_cool_profile/int_rom_programmer_emerald_fpga_cool_profile_introm.lod",check=false)
    #puts "Disabling XON/XOFF of Host"
    #puts "Burning the FPGA ROM..."
    puts "html><i>(Using the file \"" + file + "\")</i>"
    restart(true, true)
    #$DEBUG_UART.ctrl.SWRX_flow_ctrl.w 0
    #$DEBUG_UART.ctrl.SWTX_flow_ctrl.w 0
    #$DEBUG_UART.ctrl.BackSlash_En.w 0
    $MEM_BRIDGE.Rom_Patch[0].Patch.w 1
    print(ld(file, check))
    $MEM_BRIDGE.Rom_Patch[0].Patch.w 0
    unfreeze
end



addHelpEntry("rom", "romAddCwButton", "","", "Add a button to the CoolWatcher \
        toolbar (romToolbar) to call the romBurn function.")

def romAddCwButton
  cwAddScriptButton('romToolbar', "romBurn", CWIconLib::ROMBURNICON,"Burn ROM")
end

