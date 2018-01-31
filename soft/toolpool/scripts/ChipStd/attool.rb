
def attool(com, br=115200)
    CHRawConnection.session(com, 115200) { |c|
            # Launch printing thread
            a = Thread.new(c) { |acon|
                Thread.current[:name] = 'Old AT Tool'
                while(true)
                    begin
                        puts acon.getData().pack("C*")
                    rescue CHRawConnectionGetDataTimeout
                    end
                    sleep(0.001)
                end
            }
        
        begin
            while(true)
                command = cwgets
                break if(command == "exit" || command == "quit")
                c.sendData((command+"\r").unpack('C*'))
            end
        
        ensure
            a.kill
        end
    }
end
