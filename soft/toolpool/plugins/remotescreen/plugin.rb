
module RemoteScreenPlugin

    @@remotescreenActivated = false



    def RemoteScreenPlugin.activateRemoteScreenGui

        if (@@remotescreenActivated == false)
            puts "Loading RemoteScreen GUI ..."

            # TODO: Add the GUI code here.

            @@remotescreenActivated = true
            puts "RemoteScreen GUI loaded"
        else
            puts "RemoteScreen GUI already loaded"
        end
    end



    # Read the text displayed on the embedded screen.
    # To do so, the disassembly plugin is used.
    def RemoteScreenPlugin.loadTextScreen

        # Read the text frame buffer in the remote memory.
        matrixW = "g_fmgScreenTextMatrixWidth".address.R16
        matrixH = "g_fmgScreenTextMatrixHeight".address.R16
        matrixAdd = "g_fmgScreenTextMatrix".address
        matrixData = matrixAdd.RB(matrixW * matrixH)

        # Reformat the text screen data read into a table of strings.
        text = []
        matrixH.times { |j|
            line = ""
            matrixW.times { |i|
                ch = matrixData[i + j * matrixW]
                # Replace the null and space characters by
                # non breakable spaces.
                if (ch != 0 && ch != ' ') then
                    line += "%c" % ch
                else
                    line += "&nbsp;"
                end
            }
            text << line;
        }
        
        return text
    end


    
    # Read the remote text screen and display it.
    def RemoteScreenPlugin.snapShot
        
        textscreen = loadTextScreen
        textscreen.each { |line|
            puts "html>" + line
        }
    end



    # Read the remote text screen and display it, in loop.
    def RemoteScreenPlugin.loopSnapShot
        
        9999.times {
            textscreen = loadTextScreen
            cwClear
            textscreen.each { |line|
                puts "html>" + line
            }
            sleep 1
        }
    end

end



# CoolWatcher specific GUI stuff.
begin
    include CoolWatcher

    def activateRemoteScreenGui
        RemoteScreenPlugin::activateRemoteScreenGui()
    end
    
    begin
        cwAddMenuCommand("Plugins", "Activate RemoteScreen",
        "activateRemoteScreenGui()", 0) if(!$enterprisever)
    rescue Exception
        puts "*** Could not load plugin. Investigate yourself. ***"
    end
    
rescue Exception
end


activateRemoteScreenGui()

addHelpEntry("remotescreen", "snapshot", "", "",
"Get the text written on the screen of the phone and display it. " +
"Only the text displayed using the FMG service is fetched.")
def snapshot
    RemoteScreenPlugin.snapShot
end