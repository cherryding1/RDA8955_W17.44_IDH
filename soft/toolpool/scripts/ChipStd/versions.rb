#!/usr/bin/env ruby
include CoolHost
require 'help.rb'

addHelpEntry("chip", "sver", "", "", "Prints the version information of the software modules.")
def sver()
    
    puts "Please wait, reading info..."
    
    dark = true
    color = (dark)?("#d0d0d0"):("#f0f0f0")
    str = ("html><table><tr bgcolor=%s>" % color) + 
            "<td><font color=red><b>Name</b></font></td>" +
            "<td><font color=red><b>Revision</b></font></td>" +
            "<td><font color=red><b>Number</b></font></td>" +
            "<td><font color=red><b>Date</b></font></td>" +
            "<td><font color=red><b>String</b></font></td>" +
            "</tr>" 

    $map_table.>.members.each { |e| 
         
        begin
            e.derefAt(4).revision
        rescue NoMethodError
            next
        end

        dark = !dark
        color = (dark)?("#d0d0d0"):("#f0f0f0")
        
        str +=  ("<tr bgcolor=%s><td><b>" % color) + e.to_s + "</b></td>"
        
        begin
            content = e.>
        rescue NullPointer
            str += "<td>--</td><td>--</td><td>--</td><td>--</td></tr>"
            next
        end
        
        str += "<td>%d</td>" % content.revision.R    
        str += "<td>0x%X</td>" % content.number.R  
        datestr = "%d" % content.date.R  
        str += "<td><font color=blue>%s/%s/%s</font></td>" % [datestr[0..3],datestr[4..5],datestr[6..7]] 
        str += "<td><font color=darkgreen>%s</font></td>" % content.string.R().RS(128)
        str += "</tr>"
    }
    
    puts str
    
end

addHelpEntry("chip", "flashid", "", "", "Attempts to read the Autoselect fields in the flash to identify the part number.")
def flashid
	xfbp;bfbp
	($CS0.address+0x0aaa).w16 0x00aa
	($CS0.address+0x0554).w16 0x0055
	($CS0.address+0x0aaa).w16 0x0090
	manufId = ($CS0.address+0x0000).R16
	id1 = ($CS0.address+0x0002).R16
	id2 = ($CS0.address+0x001c).R16
	id3 = ($CS0.address+0x001e).R16
	puts "Manuf ID: 0x%04x"%manufId
	puts "ID 1: 0x%04x"%id1
	puts "ID 2: 0x%04x"%id2
	puts "ID 3: 0x%04x"%id3
	$CS0.address.w16 0x00f0

	case manufId
	when 0x0001
		puts "Spansion"
		case id2
		when 0x2220
			puts "S71PL127J/N"
		when 0x2230
			puts "S71WS256N"
		when 0x2242
			puts "S71WS256P"
		when 0x223c
			puts "S71PL256N"
		when 0x3141
			puts "S71NS256P"
		else
			puts "Unknown model, please add"
		end
	when 0x0020
		puts "Numonyx/ST"
		case id1
		when 0x882e
			puts "M36L0R7050U"
		else
			puts "Unknown model, please add"
		end
	else
		puts "Unknown manufacturer, please add to table"
	end


end
