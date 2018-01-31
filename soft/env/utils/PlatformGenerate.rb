#!/usr/bin/ruby

# This script will generate the Platform libraries for a list
# of targets. It will then copy a list if include files to the
# public include directory.

targets = [
    "greenstone_eng_ct1010",
    "greenstone_cs2324_light",
]

includes = [
    "platform",
    "platform/edrv/lcdd",
    "platform/edrv/tsd",
    "platform/csw",
    "platform/chip/defs",
    "platform/chip/hal",
    "platform/base/sx",
]

source = ENV["SOFT_WORKDIR"] + "/platform"
Dir.chdir(source)

destination = ENV["SOFT_WORKDIR"] + "/../wsdaw_pub/platform"
#destination = ENV["SOFT_WORKDIR"] + "/platform_as_lib"
destination_inc = destination + "/include"
`mkdir -p #{destination_inc}`

puts "Generating all the Platform libraries..."

targets.each{ |tgt|
#    puts "Library for #{tgt} as release..."
#    `make CT_TARGET=#{tgt} CT_RELEASE=release clean binlib`
    puts "Library for #{tgt} as debug..."
#    `make CT_TARGET=#{tgt} CT_RELEASE=debug clean binlib`
    puts "Library for #{tgt} as cool_profile..."
#    `make CT_TARGET=#{tgt} CT_RELEASE=cool_profile clean binlib`

    lib_tgt = "/packed/platform_#{tgt}/lib"
    destination_lib_tgt = destination + lib_tgt
    source_lib_tgt = source + lib_tgt
    `mkdir -p #{destination_lib_tgt}`
    `cp #{source_lib_tgt}/*.a #{destination_lib_tgt}`
    
    # Copying the useful Def files.
    `make copydefs PLATFORM_LIB_DEST=#{destination}`
}

puts "Copying all the includes in Platform include..."

includes.each{ |incdir|
    incdir_full = ENV["SOFT_WORKDIR"] + "/" + incdir
    `cp #{incdir_full}/include/*.h #{destination_inc}`
}

# Copying Makefile.
`cp #{source}/Makefile #{destination}`

puts "Platform as library generated in: #{destination}."
puts "Done."
