#!/bin/bash
# Bash script to generate the header files from the XMD root.

VERBOSE=$1
# Build the flag list, by keeping only the flags containing "=".
for flag in ${2}
do
    if [ -n "`echo $flag | grep '.*=[0-9]\+'`" ]
	then
		XMD_FLAGS=`echo "$XMD_FLAGS -D $flag"`
    fi
done
XMD_CMD="coolxml -safeh "$XMD_FLAGS

XMD_FILE=`ls -1 ${SOFT_WORKDIR}/toolpool/map/${CT_ASIC_CAPD}/${CT_ASIC}_soft_root.xmd`

# Construct the proper CYGPATH building path function
# If we're on a unix machine we don't change the path
# If we are on a CYGWIN install we need to convert the 
# cygwin path into a windows path to use coolxml
if [[ $(uname | grep -c CYGWIN) > 0 ]]
then
    CYGPATH="cygpath -am"
else
    CYGPATH="echo"
fi

# The header files must be re-generated from the the XMD.
#echo "XMD2H             ${LOCAL_NAME}"

# Build the command.
XMD_INPUT="-si `${CYGPATH} ${XMD_FILE}`"
XMD_OUTFILE=`echo ${XMD_FILE} | sed "s/\.xmd/\.h/"`
XMD_OUTPUT_DIR=`dirname ${XMD_FILE}`
XMD_OUTPUT="-sh `${CYGPATH} ${XMD_OUTPUT_DIR}`"
XMD_CMD_LINE="${XMD_CMD} ${XMD_INPUT} ${XMD_OUTPUT}"

# Show the command.
if [[ $VERBOSE -eq 1 ]]
then
    echo ${XMD_CMD_LINE}
fi

# Execute the command.
if [[ $VERBOSE -eq 1 ]]
then
    ${XMD_CMD_LINE}
else
    ${XMD_CMD_LINE} 1>/dev/null 2>&1 | grep -v 'already declared' 1>&2
fi

# Get result of the XMD_CMD_LINE
xmdRes=${PIPESTATUS[0]}
    
# Check error status
if [[ $xmdRes -ne 0 ]];
then
    exit 2
else
    exit 0
fi



