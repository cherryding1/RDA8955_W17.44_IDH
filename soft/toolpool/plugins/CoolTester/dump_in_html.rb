class CHValuable
	    def dump_in_html(call)
	        html= "<li>" + self.to_s + " = " "#{self.R}" + "</li>"
	        return html
	    end
	end

class CHPointer
        def dump_in_html(call)
	        html= "<li>" + self.to_s + " = " + ("0x%08x"% self.R )+ "</li>" 
	        html+=self.>.dump_in_html
	        return html
	    end
end

class CHArray
    def dump_in_html(call)
	     
	     if call==true
	        html= "<li>" + self.to_s + " = "
	        self.members.each do |m|
    	        html+="#{m.R}".to_i.chr if "#{m.R}"!="0"
    	    end 
    	    html+= "</li>"
	     else
    	     html= "<ul>"+self.to_s  
    	     self.members.each do |m|
    	        html+= "<li>" + m.to_s + " = " "#{m.R}" + "</li>" 
    	     end 
    	     html+= "</ul>"	   
    	 end  
    	 return html
	end
end

class CHStruct
    def dump_in_html(call=false)
        html= "<ul>"+self.to_s  
        self.members.each do |object|
            html+=object.dump_in_html(call)
        end
        html+= "</ul>"
        return html
    end
end
