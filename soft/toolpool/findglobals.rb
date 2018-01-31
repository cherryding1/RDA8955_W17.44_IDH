require 'find'
require 'rexml/document'

EXPGLOBAL = /^.*(\$[A-Za-z0-9_]+).*$/

def crawlForModules(element,archivename,modules)
    element.each_element{ |e|   
        if(e.name=="archive" || e.name=="bigarchive")
            crawlForModules(e,e.attributes["relative"],modules)
        elsif(e.name=="module" || e.name=="typedef"|| e.name=="alias")
            modules[e.attributes["name"]] = archivename 
        end
    }
end

def crawlForInstances(element,archivename,modules,instances)
    element.each_element{ |e| 
       
        if(e.name=="archive" || e.name=="bigarchive")
            crawlForInstances(e,e.attributes["relative"],modules,instances)
        elsif(e.name=="instance")
            instances[e.attributes["name"]] = [archivename,e.attributes["type"],modules[e.attributes["type"]]]
        end
    }
end

def findInstances(filename)
    instances = {}
    modules = {}
    File.open(filename) { |f|
        
        doc = REXML::Document.new(f.read)
        crawlForModules(doc,filename,modules)
        crawlForInstances(doc,filename,modules,instances)
        
        str = "Instances found in #{filename} : "
        
        puts "\n"
        puts str
        puts "*" * str.size
        instances.each{ |i,v|
            puts "#{i} => #{v[0]} <=> #{v[1]} => #{v[2]}"
        }
    }
    puts "\n\n"
    return instances
end

def findGlobalsInScriptsAndAddToHash(rootfolder,globals)

    rootfolder = File.expand_path(rootfolder)
    Find.find(rootfolder) do |path|
        if FileTest.directory?(path)
            if File.basename(path)[0] == ?.
                Find.prune       # Don't look any further into this directory.
            else
                next
            end
        else
            
            if(File.extname(path) == ".rb")
                File.open(path,"rb") { |f|
                  m = f.read.scan(EXPGLOBAL).flatten
                  globals[path] = m.sort.uniq if(m.size() > 0)
                }
            end
        end
    end
end

def convertGlobalHashByFileToGlobalHashByGlobalName(globalsbyfile)

    globals = {}
    puts "Found the following occurences :"
    puts "********************************"
    puts "\n"
    
    globalsbyfile.each { |filename,globs|
     
        puts filename
        puts "="*40
        globs.each{ |v|
            puts v
            if(!globals[v])
                globals[v] = [filename]
            else
                globals[v] << filename
            end
        }
        puts "\n"
    }
    return globals
end

def whatToKeep(globals,instances)
    tokeep = []
    
    puts "\n"
    puts "Summary :"
    puts "*********"
    
    globals.each{ |global,pathes|
        str = global + "\nUsed in:\n---------\n    [XMD DECLARATION NOT FOUND]\n\n"
        
        if(instances[global[1..-1]])
            i = instances[global[1..-1]]
            str = global+" => #{i[0]} <=> #{i[1]} => #{i[2]}\n"
            str += "Used in :\n"
            str += "---------\n"
            str += "    "+pathes.join("\n    ")
            str += "\n\n"
            tokeep << i[0]
            tokeep << i[2]
        end  
        puts str
    }
    
    tokeep.sort!.uniq!
    
    puts "You should keep :"
    puts "*****************"
    puts tokeep.join("\n")
end


instances = {} 
globalsbyfile = {}
globals = {}

if(!ARGV[0])
    puts "Usage : findglobals.rb pathtoxml [directorieswithscripts]"
    puts "    Crawls into the list of 'directorieswithscripts', read all the scripts, find the globals variables used in them and tries to see in which xml files they are originally defined and instanciated, based on the package 'pathtoxml'."
    puts "    Note : this may be bogguous with xml packages used for the soft, because global variables having pointer fields are not handled."
    
end

instances = findInstances(ARGV[0])

ARGV.shift()
ARGV.each{ |rubydir|
    findGlobalsInScriptsAndAddToHash(rubydir,globalsbyfile)
}

globals = convertGlobalHashByFileToGlobalHashByGlobalName(globalsbyfile)
whatToKeep(globals,instances)


