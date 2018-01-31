#!/usr/bin/ruby

puts "Generating XMD packages..."

Dir.chdir(ENV["SOFT_WORKDIR"]+"/platform")

targets = ["greenstone_eng_ct1010", "granite_eng", "nephrite_fpga","gallite_fpga", "esperite_fpga"]

targets.each{ |t|
  `make xmdpack CT_TARGET=#{t}`   
}
puts "\n"
puts "Operations done for targets:"
puts "############################"

targets.each{ |t|
  puts t
}
