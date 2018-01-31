#!/usr/bin/env bash
#===============================================================================

# Exit immediately if command exits with a non-zero status.
set -e

if [ "x${SOFT_WORKDIR}" = "x" ] || [ ! -d "${SOFT_WORKDIR}" ]; then
	echo "Error: SOFT_WORKDIR not valid"
	exit 1
fi

PROGRAM_NAME=`basename $0`

LIB_PATH=
LIB_TEST=`echo $PROGRAM_NAME | grep mergelib | sed 's/^mergelib_\([^.]*\).*$/\1/' | sed 's%_%/%'`
if [ -z "$LIB_TEST" ]; then
	echo "Error: Cannot extract library name: $PROGRAM_NAME"
	exit 2
else
	if [ -d "${SOFT_WORKDIR}/${LIB_TEST}" ]; then
		LIB_PATH="$LIB_TEST"
	elif [ -d "${SOFT_WORKDIR}/platform/${LIB_TEST}" ]; then
		LIB_PATH="platform/${LIB_TEST}"
	fi
fi

if [ "x${LIB_TEST}" = "xat" ]; then
	LIB_URL="http://svn.rdamicro.com/svn/developing1/modem2G/AT/source_code/bv5/soft_cmux/soft/at"
	MOD_LOWCASE="at"
	REV_FILE=${SOFT_WORKDIR}/$MOD_LOWCASE/src/${MOD_LOWCASE}p_version.h
elif [ "x${LIB_TEST}" = "xcsw" ]; then
	LIB_URL="http://svn.rdamicro.com/svn/developing1/Sources/csw/branches/4C4D"
	MOD_LOWCASE="csw"
	REV_FILE=${SOFT_WORKDIR}/platform/$MOD_LOWCASE/src/${MOD_LOWCASE}p_version.h
elif [ "x${LIB_TEST}" = "xstack" ]; then
	LIB_URL="http://vdi-svn.rdamicro.com/svn/developing1/Sources/stack/branches/4C4D"
	MOD_LOWCASE="stack"
	REV_FILE=${SOFT_WORKDIR}/platform/$MOD_LOWCASE/src/${MOD_LOWCASE}p_version.h
fi

#updateversion

REV_SKEL_FILE=${SOFT_WORKDIR}/env/compilation/mod_version_skel.h
LOCAL_REVISION_CMD=$( svn info ${LIB_URL} | grep "Last Changed Rev" | sed 's/.*: //' )
LOCAL_BRANCH_CMD=$( svn info ${LIB_URL} | grep "^URL" | cut -d '/' -f 6- )

MOD=$(echo $MOD_LOWCASE | tr '[a-z]' '[A-Z]')
LOCAL_BRANCH=` echo ${LOCAL_BRANCH_CMD} | sed 's/\\//\\\\\\//g'`;

sed -e "s/@{MOD}/$MOD/g" \
    -e "s/@{LOCAL_REVISION}/$LOCAL_REVISION_CMD/g"  \
    -e "s/@{LOCAL_BRANCH}/\"$LOCAL_BRANCH\"/g"  $REV_SKEL_FILE > $REV_FILE;
cd ${SOFT_WORKDIR} && svn merge "$@" "${LIB_URL}" "${LIB_PATH}"
