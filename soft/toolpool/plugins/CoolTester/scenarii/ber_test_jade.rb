# This file is the regression script which is executed to make BER measurements for all different speech modes and channel types. 

require 'Proto_Auto_Ber_Jade.rb'
require 'Diagnostic.rb'

@logfile=File.open("ber_test_jade.html","ab+")
        
@logfile << "<h2> Reference sensibility level test </h1>" + "<br>" 
@logfile << "<b>The tests are passed on CMU200 with ABFS </b>" + "<br>" 
@logfile << "<b>the board used is a dev5 jade H with a rf3_4 radio </b>" + "<br>"
@logfile << "<b>the stack version is V1.00.51 and PHY version is Coolsand beta 20080402</b>" + "<br>"
@logfile << "<b>the Jade calibration tool version is V1.07.07 </b>" + "<br>" 
@logfile << "<br>" 
              
@logfile.close();


testList=[
          { :Proto_Auto_Ber_Jade=>{:frames=>100, :band=>2, :speech=>1, :arfcn=>40, :level=>104, :bool_static=>true, :bool_tu50=>true, :bool_ra250=>true,  :bool_ht100=>true } },
		  { :Proto_Auto_Ber_Jade=>{:frames=>100, :band=>2, :speech=>2, :arfcn=>40, :level=>104, :bool_static=>true, :bool_tu50=>true, :bool_ra250=>true,  :bool_ht100=>true } }
         ]


puts "BER test starts"

testList.each do |testHash|
	 @logfile=File.open("ber_test_jade.html","ab+")
	 @logfile << "<b>********************** Start a new sensibility level test **********************</b>" + "<br>" 
	 @logfile.close();
 	 testParams=testHash[testHash.keys[0]]
     regression = Proto_Auto_Ber_Jade.new testParams
     regression.start
 end


# Reset - Keep CPU in reset
0xe0005c.w(0x100); 0xe00000.w(0xa50001); 0xE00004.w(0x80000000); sleep(1); 0xe0005c.w(0x100);

