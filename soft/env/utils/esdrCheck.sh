#!/bin/sh


#-e "[^@]return.*[^\(]"

EREG_ESDR_CHECK="-e '[\)=].*{' -e 'typedef.*{' -e '.{120,}'"

egrep -H -n $EREG_ESDR_CHECK `find . -name "*.[ch]"`
# egrep -H -n $EREG_ESDR_CHECK `find . -name "*.[ch]p"`