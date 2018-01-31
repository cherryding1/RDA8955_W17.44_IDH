
addHelpEntry("disassembly", "where", "scope=8", "", "See 'dis' function.")
def where(scope=8)
    dis(scope)
end

addHelpEntry("disassembly", "dis", "scope=8", "", "Prints disassembly of the code around the XCPU execution pointer.")
def dis(scope=8)
    $XCPU.rf0_addr.R.dis(scope)
end

addHelpEntry("disassembly", "bwhere", "scope=8", "", "BCPU version of 'where'.")
def bwhere(scope=8)
    bdis(scope)
end

addHelpEntry("disassembly", "bdis", "scope=8", "", "BCPU version of 'dis'.")
def bdis(scope=8)
    $BCPU.rf0_addr.R.dis(scope)
end
