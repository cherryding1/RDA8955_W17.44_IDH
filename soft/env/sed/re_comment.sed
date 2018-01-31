# We don't officialy support /* */ comments
# INSIDE code ... like for (i=0; i<5 /* number fo fingers*/; i++)

# full lines of /*********/
{
/^[ \t]*\/\*\**\*\/[ \t]*/s@^[ \t]*\/\*\(.*\)\*\/@\/\/ \1@
}


# Case with /** xxx */ on the same line, DOXYGEN
{
/^[ \t]*\/\*\**\*\/[ \t]*/!s@^[ \t]*\/\*\*\(.*\)\*\/@\/\/\/ \1@
}

# Case with /* xxx */ on the same line
{
s@^[ \t]*\/\*\(.*\)\*\/[ \t]*@\/\/ \1@
}

# case with mutli-line DOXYGEN
# Between /** on one line and */ on another
/^[ \t]*\/\*\*/, /\*\/[ \t]*/{

# Replace /** by /// on the first line
/^[ \t]*\/\*\*/{
    s@\/\*\*\(.*\)@\/\/\/ \1@
    h
    d
}
# Append /// at the beginning of every line
# after /** but which is not the last line (no */)
/\*\//!{
    s@\(.*\)@\/\/\/ \1@
    H
    d
}
# Last line: begin with /// and remove the */
/\*\//{
    s@\(.*\)\*\/@\/\/\/ \1@
    H
    g

}
}

# case with mutli-line not Doxygen
/^[ \t]*\/\*/, /\*\/[ \t]*$/{

/^[ \t]*\/\*/{
    s@\/\*\(.*\)@\/\/ \1@
    h
    d
}
/\*\//!{
    s@\(.*\)@\/\/ \1@
    H
    d
}
/\*\//{
    s@\(.*\)\*\/@\/\/ \1@
    H
    g
}
}


# Last case we can deal with: a /* xxx */ comment at the 
# end of a line
s@\(.*\)\/\*\(.*\)\*\/\([[:space:]]*$\)@\1\/\/ \2\3@


# Get rid of line filled with /// ******
/^\/\/\/[[:space:]]\+\*\*\*.*$/d

