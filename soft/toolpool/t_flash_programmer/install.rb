#!/usr/bin/ruby

# This script will generate the t-flash programmer ramrun files for a list
# of FF targets.

ff_target = [
    "5827_evb_u02",
]

dirname = ENV["SOFT_WORKDIR"]+"/toolpool/t_flash_programmer" 

puts "Generating all the t-flash programmer files..."

ff_target.each{ |target|
    # T-flash programmer.
    puts "filter out for #{target}..."
    filterout = dirname + "/filterout.sh " + target
    system filterout

    puts "TFP for #{target}..."
    `make CT_TARGET=#{target} CT_USER=ADMIN CT_RELEASE=release clean lod flshprog_install`

    puts "revert for #{target}..."
    revert = dirname + "/revert.sh " + target
    system revert
}

puts "Done."
