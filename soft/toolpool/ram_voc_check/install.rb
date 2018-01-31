#!/usr/bin/ruby

ff_flash =[
	"FLSH_MODEL=flsh_spi8m","FLSH_MODEL=flsh_spi16m","FLSH_MODEL=flsh_spi32m","FLSH_MODEL=flsh_spi32x2m","FLSH_MODEL=flsh_spi64m","FLSH_MODEL=flsh_spi128m","FLSH_MODEL=flsh_spi64_32m","FLSH_MODEL=flsh_spi64x2m","FLSH_MODEL=flsh_spi128_32m","FLSH_MODEL=flsh_spi128_64m"
]

Dir.chdir(ENV["SOFT_WORKDIR"]+"/toolpool/flash_programmer")

puts "Generating all the flash programmer files..."

# Flash programmers for Form Factor boards.
ff_flash.each{ |flsh|
    # Flash programmer for Host.
    puts "FP over Host for CT_TARGET=8853_gh021 CT_ASIC=8808  #{flsh}..."
    `make FASTPF_USE_USB=0 CT_TARGET=8853_gh021 CT_ASIC=8808  #{flsh} CT_USER=ADMIN CT_RELEASE=release clean lod flshprog_install_ff flshprog_clean`

    # Flash programmer for USB.
    puts "FP over USB  for CT_TARGET=8853_gh021 CT_ASIC=8808  #{flsh}..."
    `make FASTPF_USE_USB=1 CT_TARGET=8853_gh021 CT_ASIC=8808  #{flsh} CT_USER=ADMIN CT_RELEASE=release clean lod flshprog_install_ff flshprog_clean`

    # Flash programmer for Host.
    puts "FP over Host for CT_TARGET=8851c_gh021 CT_ASIC=8809   #{flsh}..."
    `make FASTPF_USE_USB=0 CT_TARGET=8851c_gh021 CT_ASIC=8809  #{flsh} CT_USER=ADMIN CT_RELEASE=release clean lod flshprog_install_ff flshprog_clean`

    # Flash programmer for USB.
    puts "FP over USB  for CT_TARGET=8851c_gh021 CT_ASIC=8809  #{flsh}..."
    `make FASTPF_USE_USB=1 CT_TARGET=8851c_gh021 CT_ASIC=8809  #{flsh} CT_USER=ADMIN CT_RELEASE=release clean lod flshprog_install_ff flshprog_clean`
}


puts "Done."
