# This file is the regression script which is executed to make BLER measurements for all different cs modes. 

require 'Proto_Auto_PDCH_Bler.rb'
require 'Diagnostic.rb'


testList=[

	###############
	#     GSM     #
	###############
	
	#CS1
	{ :Proto_Auto_PDCH_Bler=>{:rlc_blocks=>2000, :band=>2, :cs=>1, :arfcn=>40, :bool_static=>true} },
	{ :Proto_Auto_PDCH_Bler=>{:rlc_blocks=>2000, :band=>2, :cs=>1, :arfcn=>40, :bool_tu50=>true} },
	{ :Proto_Auto_PDCH_Bler=>{:rlc_blocks=>2000, :band=>2, :cs=>1, :arfcn=>40, :bool_ra250=>true} },
	{ :Proto_Auto_PDCH_Bler=>{:rlc_blocks=>2000, :band=>2, :cs=>1, :arfcn=>40, :bool_ht100=>true} },
	#CS2
	{ :Proto_Auto_PDCH_Bler=>{:rlc_blocks=>2000, :band=>2, :cs=>2, :arfcn=>40, :bool_static=>true} },
	{ :Proto_Auto_PDCH_Bler=>{:rlc_blocks=>2000, :band=>2, :cs=>2, :arfcn=>40, :bool_tu50=>true} },
	{ :Proto_Auto_PDCH_Bler=>{:rlc_blocks=>2000, :band=>2, :cs=>2, :arfcn=>40, :bool_ra250=>true} },
	{ :Proto_Auto_PDCH_Bler=>{:rlc_blocks=>2000, :band=>2, :cs=>2, :arfcn=>40, :bool_ht100=>true} },
    #CS3
	{ :Proto_Auto_PDCH_Bler=>{:rlc_blocks=>2000, :band=>2, :cs=>3, :arfcn=>40, :bool_static=>true} },
	{ :Proto_Auto_PDCH_Bler=>{:rlc_blocks=>2000, :band=>2, :cs=>3, :arfcn=>40, :bool_tu50=>true} },
	{ :Proto_Auto_PDCH_Bler=>{:rlc_blocks=>2000, :band=>2, :cs=>3, :arfcn=>40, :bool_ra250=>true} },
	{ :Proto_Auto_PDCH_Bler=>{:rlc_blocks=>2000, :band=>2, :cs=>3, :arfcn=>40, :bool_ht100=>true} },
	#CS4
	{ :Proto_Auto_PDCH_Bler=>{:rlc_blocks=>2000, :band=>2, :cs=>4, :arfcn=>40, :bool_static=>true} },
	{ :Proto_Auto_PDCH_Bler=>{:rlc_blocks=>2000, :band=>2, :cs=>4, :arfcn=>40, :bool_tu50=>true} },

	###############
	#     DCS     #
	###############

	#CS1
	{ :Proto_Auto_PDCH_Bler=>{:rlc_blocks=>2000, :band=>3, :cs=>1, :arfcn=>40, :bool_static=>true} },
	{ :Proto_Auto_PDCH_Bler=>{:rlc_blocks=>2000, :band=>3, :cs=>1, :arfcn=>40, :bool_tu50=>true} },
	{ :Proto_Auto_PDCH_Bler=>{:rlc_blocks=>2000, :band=>3, :cs=>1, :arfcn=>40, :bool_ra250=>true} },
	{ :Proto_Auto_PDCH_Bler=>{:rlc_blocks=>2000, :band=>3, :cs=>1, :arfcn=>40, :bool_ht100=>true} },
	#CS2
	{ :Proto_Auto_PDCH_Bler=>{:rlc_blocks=>2000, :band=>3, :cs=>2, :arfcn=>40, :bool_static=>true} },
	{ :Proto_Auto_PDCH_Bler=>{:rlc_blocks=>2000, :band=>3, :cs=>2, :arfcn=>40, :bool_tu50=>true} },
	{ :Proto_Auto_PDCH_Bler=>{:rlc_blocks=>2000, :band=>3, :cs=>2, :arfcn=>40, :bool_ra250=>true} },
	{ :Proto_Auto_PDCH_Bler=>{:rlc_blocks=>2000, :band=>3, :cs=>2, :arfcn=>40, :bool_ht100=>true} },
    #CS3
	{ :Proto_Auto_PDCH_Bler=>{:rlc_blocks=>2000, :band=>3, :cs=>3, :arfcn=>40, :bool_static=>true} },
	{ :Proto_Auto_PDCH_Bler=>{:rlc_blocks=>2000, :band=>3, :cs=>3, :arfcn=>40, :bool_tu50=>true} },
	{ :Proto_Auto_PDCH_Bler=>{:rlc_blocks=>2000, :band=>3, :cs=>3, :arfcn=>40, :bool_ra250=>true} },
	{ :Proto_Auto_PDCH_Bler=>{:rlc_blocks=>2000, :band=>3, :cs=>3, :arfcn=>40, :bool_ht100=>true} },
	#CS4
	{ :Proto_Auto_PDCH_Bler=>{:rlc_blocks=>2000, :band=>3, :cs=>4, :arfcn=>40, :bool_static=>true} },
	{ :Proto_Auto_PDCH_Bler=>{:rlc_blocks=>2000, :band=>3, :cs=>4, :arfcn=>40, :bool_tu50=>true} }

	]


puts "Regression starts"

Regression = Diagnostic.new
Regression.start( testList, "ber_test_gprs"+Time.new.strftime("_%Y_%b_%d"))
