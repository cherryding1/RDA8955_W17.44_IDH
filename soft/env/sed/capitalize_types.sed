:label
/hal_[[:alnum:]_]*_t/{
h
s/.*\(hal_[[:alnum:]_]*_t\).*/\1/
y/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/
G
s/\(.*\)\n\(.*\)hal_[[:alnum:]_]*_t\(.*\)/\2\1\3/
}
t label
