#!/usr/bin/env ruby
include CoolHost
require 'help.rb'

addHelpEntry("chip", "hver", "", "", "Returns the version of the hardware binary of the FPGA. It also says if the chip is a FPGA or a FPGA_USB.")
def hver()
    
    chipStyle = [ "FPGA", "CHIP", "FPGA_USB", "FPGA_GSM" ]
    greentoneCrcAddressRom1v1 = 0x80e13ffc;
    greentoneCrcRom1v1 = 0xf52bc001;
    
	puts "Chip version: %dv%d." %[$EXT_APB.Build_Version.Major.R, $EXT_APB.Build_Version.Build_Revision.R]
    puts "Chip synthesis date: 20%02x-%02x-%02x." %[$EXT_APB.Build_Version.Year.R, $EXT_APB.Build_Version.Month.R, $EXT_APB.Build_Version.Day.R]
	puts "Chip style: %s." %chipStyle[$EXT_APB.Build_Version.Build_Style.R]
	puts "Chip fab infos: metal ID %d, bond ID %d, production ID %d." %[$EXT_APB.CHIP_ID.METAL_ID.R, $EXT_APB.CHIP_ID.BOND_ID.R, $EXT_APB.CHIP_ID.PROD_ID.R]
	
	# Get the CRC to determine the ROM version. This is a hack used only
	# for the first Greenstone ROM (no real version in this ROM).
    if (greentoneCrcAddressRom1v1.R == greentoneCrcRom1v1) then
        puts "Chip ROM version: 1v1 (Greenstone)."
    else
        # For the other chips, we just read the version at the right address.
        romVersion = $rom_version.R
        puts "Chip ROM version: %xv%x (20%02x-%02x-%02x)." % [
            (romVersion >> 28) & 0xF,
            (romVersion >>  0) & 0xF,
            (romVersion >> 20) & 0xFF,
            (romVersion >> 12) & 0xFF,
            (romVersion >>  4) & 0xFF]
    end
end
