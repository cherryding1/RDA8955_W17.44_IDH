# This file is the regression script which is executed to make BER measurements for all different speech modes and channel types. 

require 'Proto_Auto_Ber.rb'
require 'Diagnostic.rb'


testList=[

	###############
	#     GSM     #
	###############
	
	#FR
	{ :Proto_Auto_Ber=>{:frames=>10000, :band=>1, :speech=>1, :arfcn=>192, :level=>104, :bool_static=>true, :bool_tu50=>true, :bool_ra250=>true,  :bool_ht100=>true } },
	#EFR
	{ :Proto_Auto_Ber=>{:frames=>10000, :band=>1, :speech=>2, :arfcn=>192, :level=>104, :bool_static=>true, :bool_tu50=>true, :bool_ra250=>true,  :bool_ht100=>true } },
	#HR
	{ :Proto_Auto_Ber=>{:frames=>10000, :band=>1, :speech=>3, :arfcn=>192, :level=>104, :bool_static=>true, :bool_tu50=>true, :bool_ra250=>true,  :bool_ht100=>true } },
	#AMR Half Rate 7.9           
	{ :Proto_Auto_Ber=>{:frames=>10000, :band=>1, :speech=>4, :arfcn=>192, :level=>104, :bool_static=>true, :bool_tu50=>true, :bool_ra250=>true,  :bool_ht100=>true, :amr_bitrate =>3 } },
	#AMR Half Rate 7.4           
	{ :Proto_Auto_Ber=>{:frames=>10000, :band=>1, :speech=>4, :arfcn=>192, :level=>104, :bool_static=>true, :bool_tu50=>true, :bool_ra250=>true,  :bool_ht100=>true, :amr_bitrate =>4 } },
	#AMR Half Rate 6.7           
	{ :Proto_Auto_Ber=>{:frames=>10000, :band=>1, :speech=>4, :arfcn=>192, :level=>104, :bool_static=>true, :bool_tu50=>true, :bool_ra250=>true,  :bool_ht100=>true, :amr_bitrate =>5 } },
	#AMR Half Rate 5.9           
	{ :Proto_Auto_Ber=>{:frames=>10000, :band=>1, :speech=>4, :arfcn=>192, :level=>104, :bool_static=>true, :bool_tu50=>true, :bool_ra250=>true,  :bool_ht100=>true, :amr_bitrate =>6 } },
	#AMR Half Rate 5.15           
	{ :Proto_Auto_Ber=>{:frames=>10000, :band=>1, :speech=>4, :arfcn=>192, :level=>104, :bool_static=>true, :bool_tu50=>true, :bool_ra250=>true,  :bool_ht100=>true, :amr_bitrate =>7 } },
	#AMR Half Rate 4.75           
	{ :Proto_Auto_Ber=>{:frames=>10000, :band=>1, :speech=>4, :arfcn=>192, :level=>104, :bool_static=>true, :bool_tu50=>true, :bool_ra250=>true,  :bool_ht100=>true, :amr_bitrate =>8 } },
	#AMR Full Rate 12.2           
	{ :Proto_Auto_Ber=>{:frames=>10000, :band=>1, :speech=>5, :arfcn=>192, :level=>104, :bool_static=>true, :bool_tu50=>true, :bool_ra250=>true,  :bool_ht100=>true, :amr_bitrate =>1} },
	#AMR Full Rate 10.2          
	{ :Proto_Auto_Ber=>{:frames=>10000, :band=>1, :speech=>5, :arfcn=>192, :level=>104, :bool_static=>true, :bool_tu50=>true, :bool_ra250=>true,  :bool_ht100=>true, :amr_bitrate =>2} },
	#AMR Full Rate 7.9 
	{ :Proto_Auto_Ber=>{:frames=>10000, :band=>1, :speech=>5, :arfcn=>192, :level=>104, :bool_static=>true, :bool_tu50=>true, :bool_ra250=>true,  :bool_ht100=>true, :amr_bitrate =>3 }},
	#AMR Full Rate 7.4 
	{ :Proto_Auto_Ber=>{:frames=>10000, :band=>1, :speech=>5, :arfcn=>192, :level=>104, :bool_static=>true, :bool_tu50=>true, :bool_ra250=>true,  :bool_ht100=>true, :amr_bitrate =>4 }},
	#AMR Full Rate 6.7 
	{ :Proto_Auto_Ber=>{:frames=>10000, :band=>1, :speech=>5, :arfcn=>192, :level=>104, :bool_static=>true, :bool_tu50=>true, :bool_ra250=>true,  :bool_ht100=>true, :amr_bitrate =>5 }},
	#AMR Full Rate 5.9 
	{ :Proto_Auto_Ber=>{:frames=>10000, :band=>1, :speech=>5, :arfcn=>192, :level=>104, :bool_static=>true, :bool_tu50=>true, :bool_ra250=>true,  :bool_ht100=>true, :amr_bitrate =>6 }},
	#AMR Full Rate 5.15 
	{ :Proto_Auto_Ber=>{:frames=>10000, :band=>1, :speech=>5, :arfcn=>192, :level=>104, :bool_static=>true, :bool_tu50=>true, :bool_ra250=>true,  :bool_ht100=>true, :amr_bitrate =>7 }},
	#AMR Full Rate 4.75 
	{ :Proto_Auto_Ber=>{:frames=>10000, :band=>1, :speech=>5, :arfcn=>192, :level=>104, :bool_static=>true, :bool_tu50=>true, :bool_ra250=>true,  :bool_ht100=>true, :amr_bitrate =>8 }}
         
	]


puts "Regression starts"

Regression = Diagnostic.new
Regression.start( testList, "ber_test_speech_gsm"+Time.new.strftime("_%Y_%b_%d"))
