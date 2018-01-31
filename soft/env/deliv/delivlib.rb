#!/usr/bin/ruby
# Functions for building deliveries.
# See module DELIV for documentation.

## YAML language
require 'yaml'
## svn helpers 
require 'svnlib.rb'
## changelog
require 'changeloglib.rb'
## workspaces
require 'workspacelib.rb'
## pathname
require 'pathname'

#module structure inspired from /usr/lib/ruby/1.8/irb.rb


module DELIV
    # CT_RELEASES
    RELEASES=['release', 'debug', 'cool_profile']
    LAST_CHANGES_FILENAME='commits.txt'
    CHANGELOG_FILENAME=Changelog::DEFAULT_FILENAME

    MERGED_CHANGES_FILENAME='merged.txt'
    ELIGIBLE_CHANGES_FILENAME='eligible.txt'


    # configuration class for loading yaml configuration
    class Config
        attr_accessor :moduleName, :libDeliveryCopy, :compileOptions
        # create a configuration
        def initialize(moduleName,libDeliveryCopy=false,compileOptions=false)
            @moduleName      = moduleName
            @libDeliveryCopy = libDeliveryCopy
            @compileOptions   = compileOptions
        end
        # load from string
        def Config.load(input)
            YAML::load(input)
        end
        # load from file
        def Config.loadFile(filename)
            File.open(filename, 'r') { |input|
                Config.load(input)
            }
        end
    end

    # configuration
    @CONF = DELIV::Config.new(File.basename(Dir.pwd))
    @CONF_LOADED = false

    # configuration attribute reader
    def DELIV::conf
        @CONF
    end

    # configuration loaded (or generated)
    def DELIV::confLoaded?
        @CONF_LOADED
    end

    # ensure that configuration was loaded or raise an error
    def DELIV::confEnsureLoaded
        if !@CONF_LOADED then
            puts "Couldn't load config file."
            puts "a sample 'deliv.yaml' file is as follows:"
            puts DELIV::conf.to_yaml
            puts "\n\n"
            raise "no config."
        end
    end

    # load configuration from filename
    def DELIV::loadConfig(filename)
        @CONF = Config.loadFile(filename)
        @CONF_LOADED = true
    end

    # main
    def DELIV::main
        deliv = Deliv.new
        deliv.run
    end

    class Deliv
        def initialize
            begin
                DELIV::loadConfig('deliv.yaml')
            rescue
                puts "WARNING: couldn't load config file 'deliv.yaml'."
            end
        end

        private
        #= helper internal func

        #== compile helpers

        # launch 'make' for compilation, compilation log is stored to a file
        def compile (options,release)
            ops="CT_RELEASE=#{release} #{options}"
            cmd="make IGNORE_CUSTOM_VARS=1 CT_USER=ADMIN #{ops} binlib"
            logfile="make_binlib_#{ops.gsub(/[ ]+/,'_')}.log"
            puts cmd
            puts 'Log in: '+logfile
            ok=system(cmd+" > #{logfile} 2>&1")
            raise "Compilation Error" unless ok
        end

        # launch compilation for every release in DELIV::RELEASES
        def compileAllReleases (options)
            DELIV::RELEASES.each { |release|
                compile(options,release)
            }
        end

        #== deliv internal funcs

        # read informations from current directory and subversion
        def updateModuleInfo
            dir=File.basename(Dir.pwd)
            conf=DELIV::conf.moduleName
            st ="\n current directory:'#{dir}'"
            st+="\n in config file   :'#{conf}'"
            raise "inconsistant module name:#{st}" if dir!=conf
            url,root,rev = SVN::info('.')
            if url=~/#{root}\/([^\/]*)\/([^\/]*)\/(.*)/
                kind=$1
                svnmodule=$2
                branch=$3
                st+="\n on svn url       :'#{svnmodule}'"
                raise "inconsistant module name:#{st}" if svnmodule!=conf
                @url=url
                @moduleName=conf
                @branch=branch
                @sourceOrLib=kind
                @rootUrl=root
                @wdRev=rev
            else
                raise "Invalid url: #{url}"
            end
        end

        # url attribute reader that ensure the value is up to date
        def url
            updateModuleInfo unless @url
            @url
        end

        # moduleName attribute reader that ensure the value is up to date
        def moduleName
            updateModuleInfo unless @moduleName
            @moduleName
        end

        # branch attribute reader that ensure the value is up to date
        def branch
            updateModuleInfo unless @branch
            @branch
        end

        # sourceOrLib attribute reader that ensure the value is up to date
        def sourceOrLib
            updateModuleInfo unless @sourceOrLib
            @sourceOrLib
        end

        # rootUrl attribute reader that ensure the value is up to date
        def rootUrl
            updateModuleInfo unless @rootUrl
            @rootUrl
        end

        # check local changes and latest revision of current workdir.
        def upToDateCheck
            changes=SVN::uncommittedChanges()
            if !changes.empty? then
                puts "Your workdir include uncommited changes !:\n"+changes
                puts "Type [ctrl]+[C] (^C) to stop now!"
                gets
            end
            u,r,headRev,commitRev=SVN::info(url)
            if commitRev > @wdRev then
                puts "Your workdir is not on the latest revision\n WORKDIR: #{@wdRev}\n HEAD: #{headRev}\n LastCommit: #{commitRev}"
                puts "Type [ctrl]+[C] (^C) to stop now!"
                gets
            end
        end

        # compute delivery info
        def updateDelivInfo(kind)
            case kind
            when :source
                raise 'Source delivery only allowed from trunk' unless branch == 'trunk'
                @releasePath='/Sources/'+moduleName+'/releases/'
                @releaseUrl=rootUrl+@releasePath
                @releasePrefix='v'
                @releaseVersionStart=1
                @releaseKind=:source
            when :library
                raise 'Library delivery not allowed from trunk' if branch == 'trunk'
                base,prefix=branch.split('/')
                @releasePath="/Libraries/#{moduleName}/#{base}/"
                @releaseUrl=rootUrl+@releasePath
                @releasePrefix="#{prefix}."
                @releaseVersionStart=0
                @releaseKind=:library
            else
                raise "unknown delivery kind #{kind}"
            end
            @lastReleaseNum=SVN::listReleases(@releaseUrl, @releasePrefix).max
        end

        # compute the last release number or nil if no release
        def lastRelease
            raise 'updateDelivInfo() not called' unless @releaseKind
            n=@lastReleaseNum
            if n then
                return "#{@releasePrefix}#{n}"
            else
                return nil
            end
        end

        # compute the next release number
        def nextRelease
            raise 'updateDelivInfo() not called' unless @releaseKind
            n=@lastReleaseNum
            if n then
                n+=1
            else
                n=@releaseVersionStart
            end
            return "#{@releasePrefix}#{n}"
        end

        # compute the list of files that should be removed
        def libSwitchCleanFileList
            return [] unless DELIV::conf.libDeliveryCopy.is_a? Array
            cleanfiles=[]
            Dir['**/'].each{ |d|
                if !DELIV::conf.libDeliveryCopy.find { |p| d =~ /^#{p}/ } then
                    SVN::propget('svn:ignore',d).each_line do |l|
                        l.strip!
                        if not(l =~ /^[ \t]*$/) then
                            if FileTest.exist?(d+l)
                                cleanfiles.push(d+l)
                            end
                        end
                    end
                end
            }
            return cleanfiles
        end

        def saveLastChanges(kind)
            updateDelivInfo(kind)
            release=lastRelease
            system "rm -f #{LAST_CHANGES_FILENAME}"
            if release then
                begin
                    originUrl,originRev = SVN::getTagSource("#{@releaseUrl}#{release}")
                    if "#{rootUrl}#{originUrl}" == url then
                        SVN::logToFile(url, "-v -r#{originRev}:HEAD", LAST_CHANGES_FILENAME)
                        puts "Changes since last release (#{release}) are saved in '#{LAST_CHANGES_FILENAME}'."
                    end
                rescue SVN::SvnNotATag => e
                    puts "Not generating '#{LAST_CHANGES_FILENAME}' because detected last release (#{release}) does not seam to be a release (#{e.message})."
                end
            else
                SVN::logToFile(url, "-v --stop-on-copy", LAST_CHANGES_FILENAME)
                puts "Changes since creation are saved in '#{LAST_CHANGES_FILENAME}'."
            end
        end

        def prepareLog(kind)
            saveLastChanges(kind)
            release=nextRelease
            chlog=Changelog.new(CHANGELOG_FILENAME)
            chlog.moduleName=moduleName
            if chlog.kind != :unreleased then
                if chlog.branch == release then
                    puts "#{CHANGELOG_FILENAME} already prepared for #{release} updating..."
                else
                    puts "Adding header to #{CHANGELOG_FILENAME} for #{release}."
                    puts "Note: You can prepare your log with #{File.basename($0)} newlog)"
                    chlog.addHeaderUnreleased('tmp')
                end
            end
            ws=WORKSPACE::Workspace.new()
            ws.applySwitch()
            wdirpath=Pathname.new(ENV['SOFT_WORKDIR'])
            localpath=Pathname.pwd
            relpath=localpath.relative_path_from(wdirpath)
            ws.change(relpath.to_s,"^#{@releasePath}#{release}")

            chlog.replaceHeader(kind,release,"  Workspace:\n#{ws.to_s(false)}\n")
        end

        def checkLog(kind,release)
            chlog=Changelog.new(CHANGELOG_FILENAME)
            if chlog.kind != kind or chlog.moduleName != moduleName or chlog.branch != release then
                raise <<ASSERT_EOF
Invalid #{CHANGELOG_FILENAME} header:
expecting: #{Changelog::KINDS[kind]} #{moduleName} #{release}
but got:   #{Changelog::KINDS[chlog.kind]} #{chlog.moduleName} #{chlog.branch}
ASSERT_EOF
            end
        end

        #== action available to command line
        @@actions={}

        @@actions['help']="This help."
        # This help.
        def help
            puts "syntax: #{File.basename($0)} <action>"
            puts "actions:"
            @@actions.each_pair { |a,d|
                puts "    #{a.ljust(10)}: #{d}"
            }

        end

        @@actions['check']="Do some checks and display the configuration file."
        # do some checks and display the configuration file.
        def check
            DELIV::confEnsureLoaded
            updateModuleInfo
            puts DELIV::conf.to_yaml
        end

        @@actions['do']="Tells how to do a delivery."
        # Tells how to do a delivery.
        def do
            DELIV::confEnsureLoaded
            raise "cannot deliver from Libraries" unless sourceOrLib == 'Sources'
            case branch
            when 'trunk'
                puts <<TRUNK_EOF
The procedure is as follows:
    1. Prepare the delivery with:
        #{File.basename($0)} prepare
    Edit and update '#{CHANGELOG_FILENAME}' refer to '#{LAST_CHANGES_FILENAME}' that contains the lists of commit since last delivery.
    Commit your changes and update. (make sure you get only your changes)
    2. Deliver with the following command:
        #{File.basename($0)} cpsrc
    3. You can then go and test your delivery (or prepare libraries) with:
        #{File.basename($0)} switch
    4. Or go back to developing on the trunk
        #{File.basename($0)} newlog
    This will add a new UNRELEASED header to your '#{CHANGELOG_FILENAME}' where you can describe the changes while the code is evolving (to ease your next delivery process).
TRUNK_EOF
            when /^branches/
                raise "todo working branch library deliv"
            when /^releases/
                puts <<RELEASE_EOF
The procedure is as follows:
    1. Prepare the delivery with:
        #{File.basename($0)} prepare
    Edit and update '#{CHANGELOG_FILENAME}' refer to '#{LAST_CHANGES_FILENAME}' that contains the lists of commit since last delivery.
    2. Build all libraries
        #{File.basename($0)} build
    Commit your changes and update. (make sure you get only your changes)
    3. Deliver with the following command:
        #{File.basename($0)} cplib
    4. You can then go and test your delivery with:
        #{File.basename($0)} switch
    5. Or go back to developing on the trunk
        #{File.basename($0)} switch trunk
        #{File.basename($0)} newlog
RELEASE_EOF
            else
                raise "Unknown location #{url}"
            end
        end

        @@actions['prepare']="Prepare a delivery: build '#{LAST_CHANGES_FILENAME}' and prepare '#{CHANGELOG_FILENAME}'."
        # Prepare a delivery: build a file with commits since last delivery and update the changelog.
        def prepare
            DELIV::confEnsureLoaded
            raise "cannot deliver from Libraries" unless sourceOrLib == 'Sources'
            case branch
            when 'trunk'
                puts "Preparing source delivery"
                prepareLog(:source)
            when /^branches/
                raise "todo working branch library deliv"
            when /^releases/
                puts "Preparing library delivery"
                prepareLog(:library)
            else
                raise "Unknown location #{url}"
            end
        end

        @@actions['newlog']="Add a new UNRELEASED header to '#{CHANGELOG_FILENAME}' also build '#{LAST_CHANGES_FILENAME}'"
        # Add a new UNRELEASED header to the changelog and build a file with commits since last delivery.
        def newlog
            DELIV::confEnsureLoaded
            raise "You are not supposed to edit in Libraries" unless sourceOrLib == 'Sources'
            chlog=Changelog.new(CHANGELOG_FILENAME)
            chlog.moduleName=moduleName
            case branch
            when 'trunk'
                release='trunk'
                kind=:source
            when /^branches\/(.*)/
                release=$1
                kind=false
            when /^releases\/(.*)/
                release=$1
                kind=:library
            else
                raise "Unknown location #{url}"
            end
            if chlog.kind == :unreleased then
                puts "current changelog is already UNRELEASED, no header added!"
            else
                chlog.addHeaderUnreleased(release)
            end
            if kind then
                saveLastChanges(kind)
            end        
        end

        @@actions['cpsrc']="Copy the trunk to a new Source delivery"
        # Copy the trunk to a new Source delivery
        def cpsrc
            DELIV::confEnsureLoaded
            raise "cpsrc is valid only from trunk" unless branch == 'trunk' and sourceOrLib == 'Sources'
            updateDelivInfo(:source)
            puts "preparing to copy '/Sources/#{moduleName}/trunk/'"

            release=nextRelease
            puts "to '#{@releasePath}#{release}/'."

            upToDateCheck
            checkLog(:source,release)

            SVN::copy("#{url}@#{@wdRev}",@releaseUrl+release, "Delivery of #{moduleName} Sources into #{release}")
        end


        @@actions['switch']="Switch to a source or library delivery of current module."
        # Switch to a source or library delivery of current module.
        def switch
            if ARGV[0] then
                swto=ARGV.shift
            else # autodetect
                case branch
                when 'trunk'
                    swto='src'
                when /^branches/
                    raise "You are in a branch, don't know where to switch..."
                when /^releases/
                    swto='lib'
                else
                    raise "Unknown location #{url}"
                end
            end
            case swto
            when /^#{rootUrl}\/([^\/]*)\/([^\/]*)\/(.*)/
                swkind=$1
                swsvnmodule=$2
                swbranch=$3
                raise "Cannot switch to a different module #{swsvnmodule} != #{moduleName}" if swsvnmodule != moduleName
                swPath="/#{swkind}/#{moduleName}/#{swbranch}"
            when /^\^?\/?([^\/]*)\/([^\/]*)\/(.*)/
                swkind=$1
                swsvnmodule=$2
                swbranch=$3
                raise "Cannot switch to a different module #{swsvnmodule} != #{moduleName}" if swsvnmodule != moduleName
                swPath="/#{swkind}/#{moduleName}/#{swbranch}"
            when 'trunk'
                swkind='Sources'
                swPath="/Sources/#{moduleName}/trunk"
            when 'src', 'Sources'
                swkind='Sources'
                releasePath='/Sources/'+moduleName+'/releases/'
                prefix='v'
                n=SVN::listReleases(rootUrl+releasePath, prefix).max
                swPath="#{releasePath}#{prefix}#{n}"
            when 'lib', 'Libraries'
                swkind='Libraries'
                releasePath='/Sources/'+moduleName+'/releases/'
                prefix='v'
                n=SVN::listReleases(rootUrl+releasePath, prefix).max
                prefix="#{prefix}#{n}."
                releasePath="/Libraries/#{moduleName}/releases/"
                n=SVN::listReleases(rootUrl+releasePath, prefix).max
                swPath="#{releasePath}#{prefix}#{n}"
            end
            if swkind=='Libraries' then
                puts "Preparing to switch to a library delivery..."
                # up to date check ? To avoid destroying user files
                changes=SVN::uncommittedChanges()
                if !changes.empty? then
                    puts "Your workdir include uncommited changes !:\n"+changes
                    puts "Type [ctrl]+[C] (^C) to stop now!"
                    gets
                end
                # do some cleaning to avoid leaving files in sources.
                list=libSwitchCleanFileList
                if !list.empty? then
                    puts "The following local files will be removed from your working copy:"
                    puts libSwitchCleanFileList.join("\n")
                    puts "Type [ctrl]+[C] (^C) to stop now! or return to continue."
                    gets
                    list.each { |f|
                        File.delete(f)
                    }
                end

            end
            puts "switching current module to '#{swPath}'."
            SVN::switch(rootUrl+swPath)
        end

        @@actions['build']="Compile the libraries for each releases and each options"
        # Compile the libraries for each releases and each options
        def build
            DELIV::confEnsureLoaded
            DELIV::conf.compileOptions.each { |o|
                compileAllReleases(o)
            }
        end

        @@actions['cplib']="Copy the libraries and includes to Libraries delivery space"
        # Copy the libraries and includes to Libraries delivery space
        def cplib
            DELIV::confEnsureLoaded
            raise "cplib is not valid from here" unless branch != 'trunk' and sourceOrLib == 'Sources'
            raise "Accroding to the configuration file, this module cannot be delivered as library." unless DELIV::conf.libDeliveryCopy
            updateDelivInfo(:library)
            puts "preparing to copy '/Sources/#{moduleName}/#{branch}/...'"

            release=nextRelease
            puts "to '#{@releasePath}#{release}/'."

            upToDateCheck
            checkLog(:library,release)

            tempdir=ENV['TMPDIR']
            tempdir='/tmp' unless tempdir
            tempdir+="/deliv_lib_#{ENV['USER']}#{@releasePath.gsub(/\/(Libraries\/)?/,'_')}#{release}"
            puts "preparing delivery in #{tempdir}..."
            SVN::checkout(@releaseUrl,tempdir,"--depth empty")

            libReldir="#{tempdir}/#{release}"
            SVN::mkdir(libReldir)
            DELIV::conf.libDeliveryCopy.each { |e|
                puts "- #{e}:"
                dirs=e.split('/')
                mod=dirs.pop
                r=''
                while(d=dirs.shift) do
                    r+=d+'/'
                    puts "  svn mkdir [LIB]/#{r}"
                    SVN::mkdir("#{libReldir}/#{r}") unless FileTest.directory?("#{libReldir}/#{r}")
                end
                puts "  svn copy [SRC]/#{e} [LIB]/#{e}"
                SVN::copy(e,"#{libReldir}/#{e}")
            }
            if !FileTest.exist?("#{libReldir}/#{CHANGELOG_FILENAME}") then
                puts "  svn copy [SRC]/#{CHANGELOG_FILENAME} [LIB]/#{CHANGELOG_FILENAME}"
                SVN::copy(CHANGELOG_FILENAME,"#{libReldir}/#{CHANGELOG_FILENAME}")
            end
            SVN::commit(libReldir,"Delivery of #{moduleName} Libraries into #{release}")
            puts "Cleaning up #{tempdir}..."
            system "rm -rf #{tempdir}"
            if $? != 0 then
                puts "cound not remove temporary directory: '#{tempdir}'"
            end
        end

        @@actions['libcheck']="Do some checks on libDeliveryCopy item of configuration."
        # Do some checks on libDeliveryCopy item of configuration.
        def libcheck
            DELIV::confEnsureLoaded
            #todo: check if file exists
            updateModuleInfo
            puts "File to copy"
            DELIV::conf.libDeliveryCopy.each { |e|
                puts "- #{e}:"
                dirs=e.split('/')
                mod=dirs.pop
                r=''
                while(d=dirs.shift) do
                    r+=d+'/'
                    puts "  svn mkdir [LIB]/#{r}"
                end
                puts "  svn cp [SRC]/#{e} [LIB]/#{e}"
            }

            puts "File to clean when switching to lib:"
            puts libSwitchCleanFileList.join("\n")
        end

        @@actions['mergeinfo']="Retrieve mergeinfo from trunk and build '#{MERGED_CHANGES_FILENAME}' and '#{ELIGIBLE_CHANGES_FILENAME}'."
        # Retrieve mergeinfo from trunk
        def mergeinfo
            raise 'merge to library is not supported, you probably want to build a new release instead.' unless sourceOrLib == 'Sources'
            raise 'merge from trunk is not supported, use svn instead.' if branch == 'trunk'
            system "rm -f #{MERGED_CHANGES_FILENAME} #{ELIGIBLE_CHANGES_FILENAME}"
            trunkPath='/Sources/'+moduleName+'/trunk'
            trunkUrl=rootUrl+trunkPath
            # showRev can be either :merged or :eligible
            # SVN::mergeinfo(src,dst='',showRev=nil)
            puts "mergeinfo: eligible revisions from #{trunkUrl}"
            mergeinfo=SVN::mergeinfo(trunkUrl,dst='',:eligible)
            puts mergeinfo.join(", ")
            originPath,originRev = SVN::getTagSource(url)
            raise "Inconsistant path branch origin (#{originPath}) is not trunk !" unless originPath == trunkPath
            puts "processing svn log -r#{originRev}:HEAD #{originPath}"
            log=SVN::log(trunkUrl,"-v -r#{originRev}:HEAD")
            eligibleLog=[]
            mergedLog=[]
            log.each { |l|
                if mergeinfo.include?("r#{l.revision}") then
                    eligibleLog.push(l)
                else
                    mergedLog.push(l)
                end
            }
            File.open(MERGED_CHANGES_FILENAME,"w") { |f|
                f.write(mergedLog.join(SVN::LOG_DELIMITER))
            }
            File.open(ELIGIBLE_CHANGES_FILENAME,"w") { |f|
                f.write(eligibleLog.join(SVN::LOG_DELIMITER))
            }
            puts "Writing '#{MERGED_CHANGES_FILENAME}' and  '#{ELIGIBLE_CHANGES_FILENAME}'."
        end

        @@actions['merge']="Process svn merge from trunk into a release (this command basically add the trunk's URL to the svn command line"
        # Process svn merge from trunk into a release (this command basically add the trunk's URL to the svn command line
        def merge
            raise 'merge to library is not supported, you probably want to build a new release instead.' unless sourceOrLib == 'Sources'
            raise 'merge from trunk is not supported, use svn instead.' if branch == 'trunk'
            trunkPath='/Sources/'+moduleName+'/trunk'
            trunkUrl=rootUrl+trunkPath

            args=ARGV.join(' ')
            ARGV.clear

            puts "Executing: svn merge #{args} #{trunkUrl}"

            SVN::merge("#{args} #{trunkUrl}")

        end

        public
        # Command line parser
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
