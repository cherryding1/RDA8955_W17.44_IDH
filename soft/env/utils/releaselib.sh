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
LIB_TEST=`echo $PROGRAM_NAME | grep releaselib_ | sed 's/^releaselib_\([^.]*\).*$/\1/' | sed 's%_%/%'`
if [ -n "$LIB_TEST" ]; then
	if [ -d "${SOFT_WORKDIR}/${LIB_TEST}" ]; then
		LIB_PATH="$LIB_TEST"
	elif [ -d "${SOFT_WORKDIR}/platform/${LIB_TEST}" ]; then
		LIB_PATH="platform/${LIB_TEST}"
	fi
fi

function help()
{
if [ "x$LIB_PATH" = "x" ]; then
	LIB_PATH_OPTION="
	-lib <LibPath>
		Set the relative path of the library to be built.
"
else
	LIB_PATH_OPTION=
fi

cat <<EOF
Usage: $PROGRAM_NAME [OPTIONS]... [TARGET]

OPTIONS: $LIB_PATH_OPTION
	-pack <Dir>
		Append <Dir> to the output directory name:
		\${SOFT_WORKDIR}/\${LIB_PATH}/packed
	-ncsim
		Do not care about SIM card number.
	-ncps
		Do not care about packet service type.
	-asic
		Take the chip ASIC type as a directory name, and append it to
		the output directory name, e.g.,
		\${SOFT_WORKDIR}/\${LIB_PATH}/packed/8809
		If there is a -pack option, the chip ASIC type will be appended
		after the directory name specified by -pack.
	-2
	-3
	-4
		The number of SIM cards. Default all on.
	-ng
		Non-gprs build. Default on.
	-g
		Gprs build. Default on.
	-e
		Egprs build.
	-d
		Debug version. Default on.
	-r
		Release version. Default on.
	-h
		Show this help.
TARGET:
	The target to be built.
EOF
}

while [ "x$1" != "x" ]; do
	case "$1" in
		-lib ) shift; LIB_PATH="$1" ;;
		-pack ) shift; PACKED_DIR="$1" ;;
		-ncsim ) NC_SIM=1 ;;
		-ncps ) NC_PS=1 ;;
		-asic ) WITH_ASIC=1 ;;
		-2 ) SIM_LIST="$SIM_LIST 2" ;;
		-3 ) SIM_LIST="$SIM_LIST 3" ;;
		-4 ) SIM_LIST="$SIM_LIST 4" ;;
		-ng ) PS_LIST="$PS_LIST non-gprs" ;;
		-g ) PS_LIST="$PS_LIST gprs" ;;
		-e ) PS_LIST="$PS_LIST egprs" ;;
		-d ) REL_LIST="$REL_LIST debug" ;;
		-r ) REL_LIST="$REL_LIST release" ;;
		-h ) help; exit 0 ;;
		-* ) echo "Unknown option: $1" >&2; exit 1 ;;
		* ) BUILD_TARGET="$1" ;;
	esac
	shift
done

if [ "x$LIB_PATH" = "x" ]; then
	echo "Error: No library path specified. (-h to show the usage)"
	exit 2
fi
LIB_NAME=`basename $LIB_PATH`

if [ "x$LIB_PATH" = "x${PLATFORM_SYSTEM}/stack" ] ||
   [ "x$LIB_PATH" = "xplatform/base" ]; then
	: # To avoid syntax error (no command after 'then')
	# Stack and base lib are common for all chip ASIC types
	# Useless as chip ASIC types are not applied by default?
	#WITH_ASIC=
fi

function remove_duplicate()
{
	if [ "x$*" = "x" ]; then
		return
	fi
	echo "$*" | tr '[:space:]' '\n' | sort -u | tr '\n' ' '
}

SIM_LIST=`remove_duplicate $SIM_LIST`
PS_LIST=`remove_duplicate $PS_LIST`
REL_LIST=`remove_duplicate $REL_LIST`

if [ "x$BUILD_TARGET" = "x" ]; then
	BUILD_TARGET=8851c_gh021
fi
if [ "x$SIM_LIST" = "x" ]; then
	SIM_LIST="2 3 4"
fi
if [ "x$PS_LIST" = "x" ]; then
	PS_LIST="non-gprs gprs"
fi
if [ "x$REL_LIST" = "x" ]; then
	REL_LIST="debug release"
fi

if [ "x$NC_SIM" != "x" ]; then
	# Get the SIM card number from target
	SIM_LIST="nc"
fi
if [ "x$NC_PS" != "x" ]; then
	# Get the packet service type from target
	PS_LIST="nc"
fi

echo
echo "LIB_PATH = $LIB_PATH"
echo "BUILD_TARGET = $BUILD_TARGET"
echo "SIM_LIST = $SIM_LIST"
echo "PS_LIST = $PS_LIST"
echo "REL_LIST = $REL_LIST"
echo

SIM_STEM_LIST=([2]=dualsim [3]=threesim [4]=foursim)

if [ ${BASH_VERSINFO[0]} -ge 4 ]; then
	# Associative array variables are introduced with Bash 4
	declare -A PS_VAR_LIST PS_PATH_STEM_LIST
	PS_VAR_LIST=(
		["non-gprs"]="GPRS_SUPPORT=n EGPRS_SUPPORT=n"
		    ["gprs"]="GPRS_SUPPORT=y EGPRS_SUPPORT=n"
		   ["egprs"]="GPRS_SUPPORT=y EGPRS_SUPPORT=y"
		    )
	PS_PATH_STEM_LIST=([non-gprs]= [gprs]=gprs [egprs]=egprs)

	function get_ps_index()
	{
		:
	}
else
	# No associative array variables supported
	declare -a PS_VAR_LIST PS_PATH_STEM_LIST PS_IDX_NAME
	PS_VAR_LIST=(
		[0]="GPRS_SUPPORT=n EGPRS_SUPPORT=n"
		[1]="GPRS_SUPPORT=y EGPRS_SUPPORT=y"
		[2]="GPRS_SUPPORT=y EGPRS_SUPPORT=y"
		    )
	PS_PATH_STEM_LIST=([0]= [1]=gprs [2]=egprs)
	PS_IDX_NAME=([0]=non-gprs [1]=gprs [2]=egprs)

	function get_ps_index()
	{
		local n
		for (( n = 0; n < ${#PS_IDX_NAME[@]}; n++ )); do
			if [ "x${PS_IDX_NAME[$n]}" = "x${!1}" ]; then
				eval "$1=$n"
				return
			fi
		done
	}
fi

OUTPUT_DIR=${SOFT_WORKDIR}/${LIB_PATH}/packed
if [ "x$PACKED_DIR" != "x" ]; then
	OUTPUT_DIR="${OUTPUT_DIR}/${PACKED_DIR}"
fi
if [ "x$WITH_ASIC" != "x" ]; then
	CHIP_ASIC=`cd "${SOFT_WORKDIR}" && make CT_TARGET=${BUILD_TARGET} CT_RELEASE=debug get_var_CT_ASIC`
	if [ "x$CHIP_ASIC" = "x" ]; then
		echo "Error: Failed to get chip ASIC type (CT_TARGET=${BUILD_TARGET})"
		exit 3
	fi
	OUTPUT_DIR="${OUTPUT_DIR}/${CHIP_ASIC}"
fi
BUILD_LIB_PATH="${SOFT_WORKDIR}/build/${BUILD_TARGET}/_default_/${LIB_PATH}/lib"

# Parameters:
# $1 - sim number
# $2 - packet service type
# $3 - release type
function build_lib()
{
	local SIM_STEM=
	local PS_STEM=
	local REL_STEM=
	local BUILD_OPTIONS=
	local INFO_STR=
	if [ "x$NC_SIM" = "x" ]; then
		SIM_STEM="_${SIM_STEM_LIST[$1]}"
		BUILD_OPTIONS="$BUILD_OPTIONS NUMBER_OF_SIM=$1"
		INFO_STR="$INFO_STR ${SIM_STEM_LIST[$1]}"
	fi
	if [ "x$NC_PS" = "x" ]; then
		if [ "x${PS_PATH_STEM_LIST[$2]}" != "x" ]; then
			PS_STEM="_${PS_PATH_STEM_LIST[$2]}"
		fi
		BUILD_OPTIONS="$BUILD_OPTIONS ${PS_VAR_LIST[$2]}"
		INFO_STR="$INFO_STR $2"
	fi
	# Library names should always contain a valid release type
	REL_STEM="_$3"
	INFO_STR="$INFO_STR $3"
	echo
	echo "-------------------------------------------------------------"
	echo -n "Building ${BUILD_TARGET} "
	if [ "x$INFO_STR" = "x" ]; then
		echo
	else
		echo for ${INFO_STR} ...
	fi
	echo "-------------------------------------------------------------"
	echo 
	mkdir -p "${OUTPUT_DIR}/${LIB_NAME}${SIM_STEM}${PS_STEM}/lib"
	ctmake CT_USER=ADMIN WITH_FLASH=1 AUTO_XMD2H=no AUTO_XCP2H=no WITH_SVN=1 CT_RELEASE=$3 CT_TARGET=${BUILD_TARGET} ${BUILD_OPTIONS} allclean
	ctmake CT_USER=ADMIN WITH_FLASH=1 AUTO_XMD2H=no AUTO_XCP2H=no WITH_SVN=1 CT_RELEASE=$3 CT_TARGET=${BUILD_TARGET} ${BUILD_OPTIONS}
	mv -f "${BUILD_LIB_PATH}/lib${LIB_NAME}${REL_STEM}.a" "${OUTPUT_DIR}/${LIB_NAME}${SIM_STEM}${PS_STEM}/lib/lib${LIB_NAME}${SIM_STEM}${PS_STEM}${REL_STEM}.a"
}

echo
echo "==========================================="
echo "Building $LIB_PATH libraries"
echo "==========================================="

cd "${SOFT_WORKDIR}/${LIB_PATH}"
for sim in ${SIM_LIST}; do
	for ps in ${PS_LIST}; do
		get_ps_index ps
		for rel in ${REL_LIST}; do
			build_lib "${sim}" "${ps}" "${rel}"
		done
	done
done

echo
echo "==========================================="
echo "All $LIB_PATH libraries have been built"
echo "==========================================="

echo
echo "LIB_PATH = $LIB_PATH"
echo "BUILD_TARGET = $BUILD_TARGET"
echo "SIM_LIST = $SIM_LIST"
echo "PS_LIST = $PS_LIST"
echo "REL_LIST = $REL_LIST"
echo
