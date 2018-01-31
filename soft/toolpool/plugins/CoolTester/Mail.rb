require 'action_mailer'

ActionMailer::Base.smtp_settings = {
:address => "mailhost.coolsand-tech.fr",
:port => 25,
:domain =>"coolsand-tech.fr"
}

class CoolTesterMailer < ActionMailer::Base

  def report( subject, report, to, type="text/rfc822")
    @subject      = subject
    @body 		  = report
    @recipients   = to 
    @from         = 'cooltester@coolsand-tech.fr'
    @sent_on      = Time.now
    @headers      = {}
 	content_type type
 	
	#attachment  :body         => File.read(file_name),
    #            :filename     => file_name,
    #				:content_type => "application/x-msexcel"
  
  end 
  
end
