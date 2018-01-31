#!/usr/bin/ruby

ff_flash =["FLSH_MODEL=modem_ram"]

Dir.chdir(ENV["SOFT_WORKDIR"]+"/toolpool/intsram_check")

puts "Generating the 8810 internal sram test file..."

# Flash programmers for Form Factor boards.
ff_flash.each{ |flsh|
    # Flash programmer for Host.
    puts "FP over Host for CT_TARGET=8810_r8810 CT_ASIC=8810 CT_CHIP_PKG=RDA8810 #{flsh}..."
    `make FASTPF_USE_USB=0 CT_TARGET=8810_r8810 CT_ASIC=8810 CT_CHIP_PKG=RDA8810 #{flsh} CT_USER=ADMIN CT_RELEASE=release clean lod flshprog_install_ff flshprog_clean`

}


puts "Done."
