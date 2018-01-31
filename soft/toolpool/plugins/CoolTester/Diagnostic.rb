require "Board_Manual_HelloWorld"
require "CoolTester.rb"

include CoolTester

$TEST=[
#{"Call"=>{:start=>1} } 
        #"HelloWorld"=> {"timeout"=>5,"numberHelloWorld"=>5},
        #"Aud"=>{"spkLevel"=>25, "count"=>10},
        { "Proto_Auto_BerBer"=>{} },
		#{ "Ber"=>{:frames=>5000, :speech=>3, } },
		#{ "Ber"=>{:frames=>5000, :speech=>2, } } ,
      ]

class Diagnostic

    def start ( testsArray, reportName )
        
        File.open("#{reportName}"+".html", "wb") do |report| 
             
             mainReport=Markaby::Builder.new.capture do
                html
                    
                    head do
                        title " CoolTester Report "
                    end
                    
                    body do
                        self<< h1 { "CoolTester Report"}
                        
                        self<< CoolTester.init
                        self<< " The following tests will be processed automatically by CoolTester :"
                       
						ul do
                            testsArray.each do |testHash|
                                testName=testHash.keys[0]
                                testParams=testHash[testName]
                                # Puts an internal link to the specific test section.
                                # The tag reference is the concatenation of the test name, and its specifed parameters.
                                # Thus, assuming that two same tests with EXACTLY the sames parameters won't be asked,
                                # It can't be possible to generate the same link tag.
                                self<< a(:href=>"##{testHash}") {li {testName.to_s}}
                                self<< "Specified parameters :"
                                testParams.each_pair{ |key,value| self<<"#{key} is #{value}, "}
                                br
                            end
                        end
                        
                        ol do 
                            testsArray.each do |testHash|
                                testName=testHash.keys[0]
                                testParams=testHash[testName]
                                testObject= eval(testName.to_s() + ".new(" + "testParams" + ")" )
                                a(:name=>"#{testHash}") { li  { u {h2 testName.to_s} } }
                                testObject.process
                                self<< testObject.appendToReport
                                end
                        end    
                            
                    end
               end
               
               # Delete all "html>" tag used by QT to print html string.
               mainReport.gsub!(/html>/,"")
               report.write mainReport
               
               # Add an "html>" tag to print the report in CoolWatcher.
               puts "html>"+mainReport
            end        
    end

end
