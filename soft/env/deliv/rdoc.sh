#!/bin/bash
rm -rf doc
LANG=C rdoc -a --inline-source -d changeloglib.rb deliv delivlib.rb svnlib.rb workspacelib.rb wsh 

#rm -rf /n/DOC/Public/Modem2G/ENV/deliv
#cp -R doc /n/DOC/Public/Modem2G/ENV/deliv
