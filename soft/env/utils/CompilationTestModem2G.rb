#!/usr/bin/ruby
#
# This is the ruby version of the checker script
#
#
require 'optparse'



class CompilationTester
    attr_reader :softWorkdir
    attr_reader :objectDir

    attr_reader :echoColor
    attr_reader :warnLimit
    attr_reader :testHash
    attr_reader :warnListLimit

    def initialize
        @echoColor = true
        @warnLimit = 10
        @softWorkdir=ENV['SOFT_WORKDIR']
        # prepare checker object directory
        if (ENV['PROJ_WORKDIR']) then
            workdir_name=softWorkdir.sub(ENV['PROJ_WORKDIR'],'').gsub('/','_')
        else
            workdir_name=softWorkdir.gsub('/','_')
        end
        
        @objectDir = "/tmp/CompilationTestModem2G_#{ENV['USER']}_#{workdir_name}_obj"
        @testHash = nil
        @defaultTestHash = nil

        @warnListLimit = 0
        @warnList = []

    end

    def defaultTestHash
        if !@defaultTestHash then
    
            load "#{softWorkdir}/toolpool/plugins/CoolTester/scenarii/svnChecker_testlist_modem2g.rb"
            bigHash = getTestHashModem2G('')
            
            #Get back to the old format
            smallHash = Hash.new()
            bigHash.each { |k,v| smallHash[k] = bigHash[k].collect(){ |h| h["mkopts"]} }
            
            @defaultTestHash = smallHash
        end
        @defaultTestHash
    end

    def displayTestList(testHash)
        testHash.each_pair do |test_path, test_options|
            test_name=test_path.split("/").last
            puts
            puts test_name.to_s + ' :'
            test_options.each_index do |i|
                puts ' '*10+i.to_s.rjust(5)+' : ' + test_options[i].to_s
            end
        end
    end

    def parseParam(args)
        # parse options parameters
        opts = OptionParser.new do |opts|

            objectDir_set=false
            last_key = nil

            opts.banner = "Usage: CompilationTestModem2G.rb [options]"


            opts.separator ""
            opts.separator "Directory options:"

            # Working directory
            opts.on("-d", "--soft-workdir DIR",
                    "Define compilation working directory to DIR", 'Defaults to "'+softWorkdir+'"') do |dir|
                @softWorkdir = dir
                if !objectDir_set then
                    # prepare checker object directory
                    workdir_name=softWorkdir.sub(ENV['PROJ_WORKDIR'],'').gsub('/','_')
                    @objectDir = "/tmp/CompilationTestModem2G_#{ENV['USER']}_#{workdir_name}_obj"
                end

            end

            # Object Directory
            opts.on("-o", "--object-dir DIR",
                    "Define compilation object directory to DIR", 'Defaults to "'+objectDir+'"') do |dir|
                @objectDir = dir
                objectDir_set = true
            end

            opts.separator ""
            opts.separator "Test Selection options: (must come after --soft-workdir)"

            # Test Selection
            opts.on("-t", "--test TESTNAME",
                    "Select test TESTNAME from the testlist") do |test_name|
                if !testHash then
                    @testHash = {}
                end
                th=defaultTestHash
                th.each_pair do |test_path, test_options|
                    if test_name == test_path.split("/").last then
                        last_key = test_path
                        @testHash[test_path] = test_options
                    end
                end
            end

            opts.on("-i", "--option-index 0,1,2", Array,
                    "Select make options list by index",
                    "must apear after a --test TESTNAME option") do |oi|
                if !last_key then
                    $stderr.puts 'warning ignored option --option-index '+oi.join(',')+' : no --test TESTNAME option'
                else
                    test_options = @testHash[last_key]
                    new_options = []
                    ei = []
                    oi.each do |i|
                        option = test_options[i.to_i]
                        if option then
                            new_options.push(option)
                        else
                            ei.push(i)
                        end
                    end
                    if new_options.length != 0 then
                        @testHash[last_key] = new_options
                    else
                        $stderr.puts 'warning ignored option --option-index '+oi.join(',')+' : invalid index'
                    end
                    if ei.length != 0 then
                        $stderr.puts 'warning ignored option --option-index '+ei.join(',')+' : invalid index'
                    end

                end

            end


            opts.separator ""
            opts.separator "Display options:"


            # Cast 'warn-limit' argument to a Float.
            opts.on("-w", "--[no-]warn-limit N", Integer, "Set the warning limit to N (default 10)", "or disable the limit with --no-warn-limit") do |n|
                @warnLimit = n
            end

            # Boolean switch.
            opts.on("-c", "--[no-]color", "Enable/[Disable] color output") do |c|
                @echoColor = c
            end

            # Build and Display warning list
            opts.on("--[no-]warn-summary", "Enable the warning summary") do |n|
                if n then
                    @warnList = []
                else
                    @warnList = nil
                end
            end
			# Display limit
            opts.on("--[no-]warn-summary-limit N", "Enable the warning summary limit to N (default 0)", "or disable the limit with --no-warn-summary-limit") do |n|
                @warnListLimit = n
            end
                
            opts.separator ""
            opts.separator "General options:"

            # No argument, shows at tail.  This will print an options summary.
            # Try it and see!
            opts.on_tail("-h", "--help", "Show this message") do
                puts opts
                exit
            end

            # Another typical switch to print the version.
            opts.on_tail('-l',"--list", "Show default test list") do
                displayTestList(defaultTestHash)
                exit
            end

            # Another typical switch to print the version.
            opts.on_tail("--version", "Show version") do
                puts "version 0.1"
                exit
            end

        end

        opts.parse!(args)
    end

    def init
        # prepare checker object directory

        ENV['OBJECT_DIR'] = objectDir
        ENV['HEX_PATH'] = "#{objectDir}/hex"

        system 'mkdir -p $OBJECT_DIR'
        system 'rm -Rf $OBJECT_DIR/*'

    end

    def setColor(colorName)
        if echoColor then
            colorStr = case colorName
                when /^[ ]*reset[ ]*$/i
                    "\033[0m"
                when /^[ ]*none[ ]*$/i
                    "\033[0m"
                when /^[ ]*white[ ]*on[ ]*black[ ]$/i
                    "\033[0;37;40m"

                when /^[ ]*red[ ]*$/i
                    "\033[1;31m"
                when /^[ ]*green[ ]*$/i
                    "\033[1;32m"
                when /^[ ]*yellow[ ]*$/i
                    "\033[1;33m"
                when /^[ ]*blue[ ]*$/i
                    "\033[1;34m"
                when /^[ ]*magenta[ ]*$/i
                    "\033[1;35m"
                when /^[ ]*cyan[ ]*$/i
                    "\033[1;36m"
                when /^[ ]*yellow[ ]*on[ ]*red[ ]*$/i
                    "\033[1;41;33m"
                else
                    colorName
            end
            print colorStr
        end
    end

    def analyseLog(logStr)
        warnCount=0;
        errCount=0;
        logStr.split("\n").each do |line|
            case line
            when /[^:]*: error/
                errCount+=1
                setColor('red')
                puts line
            when /No such file or directory/
                setColor('magenta')
                puts line
            when /application\/coolmmi/
                # we ignore warnings in application/coolmmi
            when /[^:]*: warning/
                warnCount+=1
                if warnLimit then
                    if warnCount == warnLimit + 1 then
                        setColor('cyan')
                        puts "More than #{warnLimit} Warnings, read the log !"
                    elsif warnCount <= warnLimit then
                        setColor('yellow')
                        puts line
                        STDOUT.flush
                    end
                else
                    setColor('yellow')
                    puts line
                    STDOUT.flush
                end
                if @warnList then
                if !@warnList.include?(line) then
                    @warnList.push(line)
                    end
                end
            else
                # nothing here
            end
        end
        setColor('none')        
        [errCount, warnCount]
    end

    def compileTarget(path, options)
        log="#{objectDir}/log_#{path.gsub('/','_')}_#{options.gsub(/[ ]+/,'_')}.log"
        puts log
        puts "make -C #{softWorkdir}/#{path} clean lod #{options}"
        STDOUT.flush
        # make ?
        makeLog = `LANG=C make WITH_FULL_PATH=1 -C #{softWorkdir}/#{path} clean lod #{options} 2>&1`
        File.open(log, "w") { |f|
            f.puts "make -C #{softWorkdir}/#{path} clean lod #{options}"
            f.puts makeLog
        }

        # return status ?
        status=$?
        # analyse
        errCount, warnCount = analyseLog(makeLog)
        if warnCount != 0 then
            setColor('yellow')
            puts "   Counted #{warnCount} Warnings"
        end
        if errCount != 0 then
            setColor('red')
            puts "   Counted #{errCount} Errors"
        end

        if status.success? then
            setColor('Green')
            puts "Compilation OK"
        else
            setColor('Red')
            puts "Compilation KO"
        end
        STDOUT.flush
        setColor('none')
        puts "\n"
        [status.success?, errCount, warnCount]
    end



    def test(testHash)

        # display the config hach
        # puts testHash.inspect.gsub('],',"],\n").gsub('=>',"=>\n   ").gsub(', ',",\n    ")
        errCountTotal = 0
        warnCountTotal = 0
        makeErrorCount = 0

        testHash.each_pair do |test_path, test_options|
               
            test_name=test_path.split("/").last
            puts
            puts "-" * 51
            puts "Checking the compilation of: #{test_name}"                 
            puts "-" * 51
            puts
            STDOUT.flush
            
            # try cd test_path
            # then
            test_options.each do |option|
                makeSuccess, errCount, warnCount = compileTarget(test_path, option)
                errCountTotal += errCount
                warnCountTotal += warnCount
                if !makeSuccess then
                    makeErrorCount += 1
                end

            end
        end

        if @warnList then
            setColor('none')
            log="#{objectDir}/log_warning_summary.log"
            puts "Warning summary file: #{log}"
            File.open(log, "w") { |f|
                f.puts @warnList.join("\n")
            }
            if @warnList.length != 0 then
                setColor('yellow')
                if !warnListLimit then
                    puts @warnList.join("\n")
                elsif warnListLimit != 0 then
                    puts @warnList[0,warnListLimit-1].join("\n")
                end
                puts "   Counted #{@warnList.length} Total uniq Warnings"
            end
        else
            if warnCountTotal != 0 then
                setColor('yellow')
                puts "   Counted #{warnCountTotal} Total Warnings"
            end
        end

        if makeErrorCount == 0 then
            setColor('Green')
            puts "-" * 51
            puts "Everything is compiling. You can do your commit." 
            puts "-" * 51
        else
            setColor('YellowOnRed')
            puts "-" * 51
            puts "ERROR please check your source and log file"
            puts "-" * 51
        end
        setColor('none')

    end
        
    def test_all
    
        test(testHash)
    end

    def run
        parseParam(ARGV)
        if !testHash then
            @testHash = defaultTestHash
        end
        init
        test_all

    end

end


a=CompilationTester.new
a.run
