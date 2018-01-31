#!/bin/bash

if [ $# -eq 0 ]; then
	echo "Usage:"
	echo "	`basename $0` [ --c | --xmd | --all ] [ <dir1> ... ]"
	exit 0
fi

if [ "x$1" == "x--c" ]; then
	FORMAT_C=1
	shift
elif [ "x$1" == "x--xmd" ]; then
	FORMAT_XMD=1
	shift
elif [ "x$1" == "x--all" ]; then
	FORMAT_C=1
	FORMAT_XMD=1
	shift
else
	FORMAT_C=1
	FORMAT_XMD=1
fi

if [ "x$*" = "x" ]; then
	DIR_LIST=(.)
else
	DIR_LIST=("$@")
fi

function format_c()
{
	find "$1" \( -type d -name .svn -prune \) -o \
		\( -type d -name .git -prune \) -o \
		\( -name '*.c' -o -name '*.h' \) -print0 | \
		xargs -0r /bin/sed -i '
		# DOS line-ending to linux line-ending
		s/\r$//
		# Tab to 4 spaces
		s/	/    /g
		# Removing trailing spaces except for comment lines
		/^ *\/\//! s/ \+$//
		'

		# -- Obsolete --
		# Only 1 space is allowed after , or = except for comment lines
		#/^ *\/\//! s/\([,=] \) \+$/\1/
		# Removing trailing spaces except for comment lines and lines ending
		# with , or =
		#/\(^ *\/\/\)\|\([,=] $\)/! s/ \+$//
}


function format_xmd_cjoker()
{
	# Only format the contents of cjoker elements
	find "$1" \( -type d -name .svn -prune \) -o \
		\( -type d -name .git -prune \) -o \
		-name '*.xmd' -exec /bin/sed -i '
		# DOS line-ending to linux line-ending
		s/\r$//
		/<cjoker>/I b f
		b
		:f
		# Tab to 4 spaces
		s/	/    /g
		# Remove spaces around cjoker tag
		s/ *\(<cjoker>\) */\1/I
		# Remove spaces around cjoker tag
		/<\/cjoker>/I {s/ *\(<\/cjoker>\) */\1/I; b}
		# Removing trailing spaces except for comment lines
		/^ *\/\//! s/ \+$//
		n
		b f
		' "{}" \;
}

function format_xmd()
{
	# Format the whole xmd file
	find "$1" -name '*.xmd' -print0 | xargs -0r /bin/sed -i '
		# DOS line-ending to linux line-ending
		s/\r$//
		# Tab to 4 spaces
		s/	/    /g
		# Remove spaces around cjoker tag
		s/ *\(<cjoker>\) */\1/I
		# Remove spaces around cjoker tag
		/<\/cjoker>/I {s/ *\(<\/cjoker>\) */\1/I; b}
		# Removing trailing spaces except for comment lines
		/^ *\/\//! s/ \+$//
		'
}


for dir in "${DIR_LIST[@]}"; do
	if [ "x$FORMAT_XMD" = "x1" ]; then
		#format_xmd_cjoker "$dir"
		format_xmd "$dir"
	fi
	if [ "x$FORMAT_C" = "x1" ]; then
		format_c "$dir"
	fi
done

