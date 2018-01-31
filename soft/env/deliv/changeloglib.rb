#!/usr/bin/ruby
# Functions for managing coolsand's changelog.
# See class Changelog for documentation.

# Manage a changelog file.
# The first header is parsed to get relevant information about current module.
class Changelog
    DEFAULT_FILENAME='changelog.txt'
    KINDS={ :unreleased => 'UNRELEASED',
            :library    => 'Library delivery',
            :source     => 'Source delivery',
#            :branch     => 'Branch creation',
#            :branchlib  => 'Branch Library delivery',
            :invalid    => false,
            :nofile     => false}

    BLANK_CONTENT=<<END_OF_BLANK_CONTENT
  Compatibility, upgrade notes:
    *

  Changes:
    *

END_OF_BLANK_CONTENT

    attr_reader :kind, :moduleName, :branch

    # Build a Changelog object, try to open the file and parse the first line
    # initializing attributes :kind, :moduleName, :branch
    def initialize(filename=DEFAULT_FILENAME)
        @filename = filename
        begin
            File.open(@filename,'r') { |f|
                line=f.gets
                KINDS.each_pair { |k,v|
                    if (v) then
                        if line.gsub!(/^#{v} /,'') then
                            @kind=k
                            break
                        end
                    end
                }
                if @kind then
                    a=line.split(' ')
                    @moduleName=a.shift
                    @branch=a.shift
                else
                    @kind=:invalid
                end
            }
        rescue Errno::ENOENT
            @kind=:nofile
        end
    end

    private

    # Load the log text into object,
    # except recognized header if skipHeader is true
    def loadLog(skipHeader)
        if @kind==:nofile then
            if skipHeader then
                @content = BLANK_CONTENT
            else
                @content = ''
            end
        else
            File.open(@filename,'r') { |f|
                @content = ''
                if skipHeader then
                    if KINDS[@kind] then
                        # recognized header line, so skip it
                        line=f.gets
                        line=f.gets
                        if not (line =~ /^[=]*$/) then
                            @content << line
                        end
                    end
                end
                @content << f.read
            }
        end
    end

    public

    # Add an Unreleased header for branch.
    # This directly modify the file.
    def addHeaderUnreleased(branch='trunk')
        #read the log content with header
        loadLog(false)
        # rewrite a new log
        File.open(@filename,'w') { |f|
            head="#{KINDS[:unreleased]} #{@moduleName} #{branch}"
            f.puts(head)
            f.puts('='*head.length)
            f.write(BLANK_CONTENT)
            f.write(@content)
        }
        # update internal state
        @content=nil
        @branch=branch
        @kind=:unreleased
        
    end
        
    # Replace the current header with a new one
    # containing new kind and branch name.
    # Also optionally insert extraText after header
    # This directly modify the file.
    def replaceHeader(kind,branch='trunk',extraText=nil)
        raise 'invalid kind' if !KINDS[kind]
        #read the log content without header
        loadLog(true)
        # rewrite a new log
        File.open(@filename,'w') { |f|
            head="#{KINDS[kind]} #{@moduleName} #{branch}"
            f.puts(head)
            f.puts('='*head.length)
            if extraText then
                if @content.slice(0..extraText.length-1) != extraText then
                    f.write(extraText)
                end
            end
            f.write(@content)
        }
        # update internal state
        @content=nil
        @branch=branch
        @kind=kind
    end

    # Set the module name, only usable when it was not read from the file.
    def moduleName=(name)
        raise "cannot change name of module #{@moduleName}!" if KINDS[kind] && @moduleName!=name
        @moduleName=name
    end

end
