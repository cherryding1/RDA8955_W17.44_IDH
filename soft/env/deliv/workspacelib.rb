#!/usr/bin/ruby
# Functions for managing Workspaces.
# See module WORKSPACE for documentation.

## svn helpers 
require 'svnlib.rb'
## pathname
require 'pathname'

module WORKSPACE


    class Workspace
        attr_reader :externalProperty, :pegRevision, :switched, :path, :rootUrl
        # create a new workspace object
        def initialize(path=ENV['SOFT_WORKDIR'],checkSwitched=true)
            @path=Pathname.new(path).realpath.to_s
            url,@rootUrl,rev,commit=SVN::info(@path)
            if url.gsub!(@rootUrl+'/Workspaces/','') then
                @name=url
                @externalProperty,@pegRevision=fromExternals(@path)
                @switched=getSwitched() if checkSwitched
            else
                raise "invalid workspace location"
            end
        end

        private

        # read 'svn:externals' property and parse it
        def fromExternals(path)
            prop=SVN::propget('svn:externals',path)
            e={}
            p={}
            prop.each_line do |l|
                l.strip!
                case l
                #matching relative external only
                when /^(\^\/[^ @]*)@(\d+)[ \t]*([^ ]*)$/
                    #puts "found external #{$1} @ #{$2}"
                    e[$3]=$1
                    p[$3]=$2.to_i
                #matching relative external
                when /^(\^\/[^ ]*)[ \t]*([^ ]*)$/
                    #puts "found external #{$1}"
                    e[$2]=$1
                #matching http external
                when /^(http[s]?:\/\/[^ ]*)[ \t]*([^ ]*)$/
                    #puts "found external #{$1}"
                    e[$2]=$1
                when /^[ \t]*$/
                    # skip empty lines
                else
                    #puts "svn:externals line '#{l}'"
                    raise "unexpected format"
                end
            end
            return e,p
        end

        # retrieve externals directory that have been switched
        def getSwitched()
            s={}
            @externalProperty.each_key { |k|
                url,root,rev,commit=SVN::info(@path+'/'+k)
                url.gsub!(@rootUrl,'^')
                if @externalProperty[k] != url or (@pegRevision[k] and pegRevision[k] != rev)
                    s[k]=url
                end
            }
            return s
        end

        def fullProperty
            f={}
            @externalProperty.keys.each { |k|
                if @pegRevision[k] then
                    f[k]= "#{@externalProperty[k]}@#{@pegRevision[k]}"
                else
                    f[k]= @externalProperty[k]
                end
            }
            return f
        end

        public

        # whether or not the workspace include switched directory
        def switched?
            !@switched.empty?
        end

        # modify the workspace to reflect the switched directories
        def applySwitch
            @switched.each_pair { |k,v|
                @externalProperty[k] = v
                @pegRevision.delete(k)
            }
            @switched.clear
        end

        # modify workspace entry for given path
        def change(path, entry, pegRev=nil)
            raise "Cannot change inexisting path" unless @externalProperty[path]
            @externalProperty[path]=entry
            if pegRev then
                @pegRevision[path]=pegRev
            end
        end

        # output the workspace as a string
        def to_s(withTitle=true)
            s=''
            s<<"Workspace '#{@name}' (in #{@path}):\n" if withTitle
            
            #get max length of string for alignment
            len=fullProperty.values.collect {|v| v.length}.max
            if switched? then
                len = [len,@switched.values.collect {|v| v.length}.max].max
            end
            #roundup to next 4 with at least 1 space
            len=(len+4)/4*4
            #build the string sorting by keys to be consistent
            @externalProperty.keys.sort.each { |k|
                s<<("    "+fullProperty[k].ljust(len)+k+"\n")
            }
            if switched? then
                s<<"WARNING: your workdir include switched module#{(@switched.length>1)?'s':''}:\n"
                @switched.keys.sort.each { |k|
                    s<<("    "+@switched[k].ljust(len)+k+"\n")
                }
            end
            return s
        end
    end

    # main
    def WORKSPACE::main
        wsh = WorkspaceHelper.new
        wsh.run
    end

    class WorkspaceHelper
        def initialize
            @ws=Workspace.new()
        end
        private

        ## action available to command line
        @@actions={}

        @@actions['help']="This help."
        # this help.
        def help
            puts "syntax: #{File.basename($0)} <action>"
            puts "actions:"
            @@actions.each_pair { |a,d|
                puts "    #{a.ljust(8)}: #{d}"
            }

        end

        @@actions['view']="View current worspace configuration (and switched state)."
        # View current worspace configuration (and switched status).
        def view
            puts
            puts @ws
        end

        @@actions['status']="Process svn status looking into workspace subdir"
        # Process svn status looking into workspace subdir
        def status
            path=Dir.pwd
            path.sub!(@ws.path+'/','')

            dirs=@ws.externalProperty.keys
            dirs.delete_if { |d| !d.include?('/') }

            list=[]
            dirs.each { |d|
                if /^#{path}\/(.*)/ =~ d then
                    list.push($1)
                end
            }
            args= ". #{list.join(' ')} #{ARGV.join(' ')}"
            ARGV.clear
            puts "Executing: svn status #{args}"
            SVN::status(args)

        end

        @@actions['st']="alias for status"
        # alias for status
        def st
            status
        end

        @@actions['diff']="Process svn diff into workspace subdir"
        def diff
            path=Dir.pwd
            path.sub!(@ws.path+'/','')

            dirs=@ws.externalProperty.keys

            if path == @ws.path
                # at root, should diff in all subdirs
                list=dirs
            else
                dirs.delete_if { |d| !d.include?('/') }

                list=[]
                dirs.each { |d|
                    if /^#{path}\/(.*)/ =~ d then
                        list.push($1)
                    end
                }
            end
            args= ". #{list.join(' ')} #{ARGV.join(' ')}"
            ARGV.clear
            puts "Executing: svn diff #{args}"
            SVN::diff(args)
        end

        @@actions['update']="Process svn update into workspace subdir"
        # Process svn update into workspace subdir
        def update
            args=ARGV.join(' ')
            ARGV.clear
            path=Dir.pwd
            dirs=@ws.externalProperty.keys.sort
            if path == @ws.path
                # at root, should ignore external for updating .
                svnUpArgs= "#{dirs.join(' ')} #{args}"
                puts "Executing: svn update #{svnUpArgs}"
                SVN::update(svnUpArgs)
                
                svnUpArgs= ". --ignore-externals #{args}"
                puts "Executing: svn update #{svnUpArgs}"
                SVN::update(svnUpArgs)

                upws=Workspace.new()
                if upws.externalProperty != @ws.externalProperty
                    puts "WARNING: The workspace property has changed but wsh did not reflect the changes to your workdir, use svn instead if you want the externals to be properly switched."
                end
                
            else
                # in subdir, find external relative directory
                path.sub!(@ws.path+'/','')

                dirs.delete_if { |d| !d.include?('/') }

                list=[]
                dirs.each { |d|
                    if /^#{path}\/(.*)/ =~ d then
                        list.push($1)
                    end
                }
                svnUpArgs= ". #{list.join(' ')} #{args}"
                puts "Executing: svn update #{svnUpArgs}"
                SVN::update(svnUpArgs)
            end
            
        end

        @@actions['up']="alias for update"
        # alias for update
        def up
            update
        end

        @@actions['revert']="Process svn revert into workspace subdir"
        # Process svn revert into workspace subdir
        def revert
            recursive=ARGV.include?('-R') or ARGV.include?('--recursive');
            args=ARGV.join(' ')
            ARGV.clear
            if recursive then
                path=Dir.pwd
                dirs=@ws.externalProperty.keys.sort
                if path == @ws.path
                    # at root, revert 
                    svnRevertArgs= "#{dirs.join(' ')} #{args}"
                else
                    # in subdir, find external relative directory
                    path.sub!(@ws.path+'/','')

                    dirs.delete_if { |d| !d.include?('/') }

                    list=[]
                    dirs.each { |d|
                        if /^#{path}\/(.*)/ =~ d then
                            list.push($1)
                        end
                    }
                    svnRevertArgs= ". #{list.join(' ')} #{args}"
                end
                puts "Executing: svn revert #{svnRevertArgs}"
                SVN::revert(svnRevertArgs)
            else
                puts "Executing: svn revert #{args}"
                begin
                    SVN::revert(args)
                rescue SVN::SvnError
                    $stderr.puts "wsh: you probably miss the -R or --recursive option!"
                    raise SVN::SvnError
                end
            end
            
        end

        @@actions['available']="List available releases"
        # List available releases
        def available
            path=Dir.pwd
            path.sub!(@ws.path+'/','')

            if @ws.externalProperty[path]=~/^\^\/([^\/]*)\/([^\/]*)\/(.*)/ then
                sourceOrLib=$1
                moduleName=$2
                branch=$3
                if moduleName == 'modem2G' then
                    raise "Current directory is not a releaseable module root"
                end
                url,rootUrl,rev,commit=SVN::info()
                releasedir='/Sources/'+moduleName+'/releases/'
                begin
                    list=SVN::listReleases(rootUrl+releasedir,'v',true)
                rescue SVN::SvnError
                    list=false
                end
                libdir="/Libraries/#{moduleName}/releases/"
                begin
                    libsvnlist=SVN::list(rootUrl+libdir,true)
                rescue SVN::SvnError
                    libdir=false
                end
                if list then
                    list.each { |rel|
                        prefix="v#{rel}"
                        puts "^#{releasedir}#{prefix}"
                        if libsvnlist then
                            liblist=SVN::extractReleases(libsvnlist,"#{prefix}.")
                            liblist.each { |lib|
                                libprefix="#{prefix}.#{lib}"
                                puts "^#{libdir}#{libprefix}"
                            }
                        end
                    }
                elsif libsvnlist then
                    libsvnlist.each { |l|
                        if l=~/^(v\d.\d)\/$/ then
                            puts "^#{libdir}#{$1}"
                        end
                    }
                else
                    puts "No delivery available for module #{moduleName}."
                end
                
            else
                raise "Current directory is not a module root"
            end
        end

        @@actions['av']="alias for available"
        # alias for available
        def av
            available
        end

        public
        # command line parser
        def run
            if ARGV.empty? then
                help
            end
            begin
                while action=ARGV.shift
                    if @@actions[action] then
                        p=method(action)
                        p.call
                    elsif action == '--bt' then
                        @bt=true
                    else
                        puts "Unknown command : #{action} !"
                        help
                    end
                end
            rescue => e
                if @bt then
                    raise e
                else
                    puts "ERROR: #{e}"
                end
            end
        end
    end

end
