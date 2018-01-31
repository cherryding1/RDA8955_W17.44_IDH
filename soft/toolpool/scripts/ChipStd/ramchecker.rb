require "help"
require "time"
require "target_executor"

include TargetExecutor

begin
    include CoolWatcher
    require "coolwatcher"
rescue Exception
end

module Ramchecker
	
	def getRamAddress
		return 0x80000000
	end

	def getRamSize
		return 1024*1024*2
	end

	def getBlockSize
		return 8192
	end
	
	def ramcheckerprogress(prog)
	    begin
	        cwSetProgress(prog*100,100)
	    rescue Exception
	        puts "Percentage %d" % (prog*100)
	    end
    end	

public

	def docheckblock(address,blocksize_in_bytes,random=true,writeblocks=true)

		#puts "Generating block patterns..."

		srand( Time::now.usec )
		a = Array.new(blocksize_in_bytes)

		n = (random)?(rand(0xFFFFFFFF)):0xA5A5A5A5
		
		(blocksize_in_bytes/4).times { |i|
			
			a[4*i] = (n & 0xFF)
			a[4*i+1] = ((n>>8) & 0xFF)
			a[4*i+2] = ((n>>16) & 0xFF)
			a[4*i+3] = ((n>>24) & 0xFF)

			n = rand(0xFFFFFFFF) 	if(random)

			(address+i*4).w n 		if(!writeblocks)			
		}
		
		address.wb a 				if(writeblocks)
		
		noerr = true

		b = address.RB blocksize_in_bytes	
	
	    errcount = 0
		a.each_index { |i|
              
			if(a[i]!=b[i])
				puts "Error at 0x%08X, wrote 0x%02X, read 0x%02X" % [address+i,a[i],b[i]]
				errcount += 1
				noerr = false
				
				if(errcount>10)
				    puts "Too many errors... aborting test."
				    return noerr
			    end
			end
		
		}
		
		return noerr 
	end
	
	def docheckram(random=true,writeblocks=true)
	    begin
	        puts "Entering host monitor..."
            tgxtor = TargetXtor.new()
            tgxtor.enterHostMonitor()
        rescue EnterHostError
            puts "Could not enter the host monitor!"
            tgxtor.closeConnection()
            return
		ensure
		    tgxtor.closeConnection()
		end		
		
		puts "Forcing breakpoints"

		xfbp
		bfbp
	
		puts "Checking RAM..."
		
		noerr = true
		
		#Check blocks of memory
		blockcount = getRamSize()/getBlockSize()
		blockcount.times { |i|
			ramcheckerprogress(i*1.0/blockcount)

			address = getRamAddress()+i*getBlockSize()
		
			res = docheckblock(address,getBlockSize(),random,writeblocks)	
			
			noerr = noerr && res 
		}
		
		if(!noerr)
		    puts "There were errors in this memory."
		else
		    puts "Test passed successfully!"
		end
        
        puts "The CPUs are frozen and breakpoints are forced, so don't forget to reset the target..."	
		return noerr
	end

end


addHelpEntry("memory", "checkMemorySegment","address,sizeInBytes","boolSuccess", "This function will perform \
    a random write block at [address] with a size of [sizeInBytes] bytes, and then check the content \
    of the memory. Errors will be displayed if there are some, and the function will return true if everything is ok \
    or false in case of errors.")
def checkMemorySegment(address,sizeInBytes)
    include Ramchecker
    return docheckblock(address,sizeInBytes)
end


addHelpEntry("memory", "checkRam","random=true,writeblocks=true","", "This function will test the RAM of the chip, by doing a \
    serie of writes and then reads (to verify the content of the memory). If [random] is false, the memory will be filled by 0xA5A5A5A5 patterns \
    instead of random patterns. If [writeblocks] is false, the memory will be written word by word. If you're using this option, we strongly advise \
    you to make yourself another coffee. Errors will be displayed if there are some, and the function will return true if everything is ok \
    or false in case of errors.")
        
def checkRam(random=true,writeblocks=true)
	
	include Ramchecker
	docheckram(random,writeblocks)
	
end

