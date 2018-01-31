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
require "chip_control.rb"



addHelpEntry("CoolWatcher", "cwBuildModem2GGui", "","", "Setup the Modem2G 
        interface for CoolWatcher.")
    
def cwBuildModem2GGui
    # Setup some menus.
    cwAddMenuCommand("Chip", "Turn off the chip", "turnoff()", 0)
    cwAddMenuCommand("Chip", "Restart the chip", "restart(false, true)", 0)
    cwAddMenuCommand("Chip", "Restart and freeze the chip", "restart(true, true)", 0)
    cwAddMenuCommand("Chip", "Unfreeze the chip", "unfreeze()", 0)
    cwAddMenuCommand("Chip", "", "", 0)
    cwAddMenuCommand("Chip", "Get software module versions", "sver()", 0)

    # Setup some toolbar.
    cwAddScriptButton('cwtarget', "restart(false, true)", CWIconLib::RESTARTICON,"Restart chip")
    cwAddScriptButton('cwtarget', "turnoff()", CWIconLib::OFFICON,"Turn off chip")
    
end



cwBuildModem2GGui();


