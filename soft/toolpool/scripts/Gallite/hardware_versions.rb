#!/usr/bin/env ruby
include CoolHost
require 'help.rb'

addHelpEntry("chip", "hver", "", "", "Returns the version of the hardware binary of the FPGA. It also says if the chip is a FPGA or a FPGA_USB.")
def hver()
    
    chipStyle = [ "FPGA", "CHIP", "FPGA_USB", "FPGA_GSM" ]
    
	puts "Chip version: %dv%d." %[$CONFIG_REGS.Build_Version.Major.R, $CONFIG_REGS.Build_Version.Build_Revision.R]
    puts "Chip synthesis date: 20%02x-%02x-%02x." %[$CONFIG_REGS.Build_Version.Year.R, $CONFIG_REGS.Build_Version.Month.R, $CONFIG_REGS.Build_Version.Day.R]
	puts "Chip style: %s." %chipStyle[$CONFIG_REGS.Build_Version.Build_Style.R]
	puts "Chip fab infos: metal ID %d, bond ID %d, production ID %d." %[$CONFIG_REGS.CHIP_ID.METAL_ID.R, $CONFIG_REGS.CHIP_ID.BOND_ID.R, $CONFIG_REGS.CHIP_ID.PROD_ID.R]
	
    # We just read the version at the right address.
    romVersion = $rom_version.R
    puts "Chip ROM version: %xv%x (20%02x-%02x-%02x)." % [
        (romVersion >> 28) & 0xF,
        (romVersion >>  0) & 0xF,
        (romVersion >> 20) & 0xFF,
        (romVersion >> 12) & 0xFF,
        (romVersion >>  4) & 0xFF]

end
