#!/bin/bash

LOOKUP_DIR="platform/base/sx ${PLATFORM_SYSTEM}/stack"
TRACEDB_FILE=toolpool/map/ChipStd/traceDb.yaml
NO_DUP_FILE=${TRACEDB_FILE}_nodup

RET=0

cd $SOFT_WORKDIR
echo "---" > $TRACEDB_FILE
grep -R 'TSTR(.*,[ \t]*0[xX][0-9a-zA-Z]*)' $LOOKUP_DIR --exclude-dir=.svn --include="*.[ch]" --include="*.hp" | sed '
	s/.*TSTR(\(.*\),[ \t]*\(0[xX][0-9a-zA-Z]*\)).*/\2: \1/
	h
	s/^\([^:]\+\):.*$/:\1/
	y/XABCDEF/xabcdef/
	x
	G
	s/^[^:]\+:\(.*\)\n:\([^:]\+\)$/\2:\1/
' | sort -u >> $TRACEDB_FILE

sort -u -t ':' -k 1,1 $TRACEDB_FILE > $NO_DUP_FILE
if ! cmp -s $TRACEDB_FILE $NO_DUP_FILE; then
	RET=1
	echo
	echo "WARNING: There are duplicate trace IDs. Please compare files:"
	echo "$TRACEDB_FILE"
	echo "$NO_DUP_FILE"
	echo "-------------------------------------------------------------"
	diff $TRACEDB_FILE $NO_DUP_FILE
else
	rm -f $NO_DUP_FILE
fi

if grep -q -v '^0[xX][0-9a-fA-F]\{8\}:\|^---$' $TRACEDB_FILE; then
	RET=1
	echo
	echo "WARNING: There are trace IDs not in recommended format:"
	echo "-------------------------------------------------------------"
	grep -n -v '^0[xX][0-9a-fA-F]\{8\}:\|^---$' $TRACEDB_FILE
fi

exit $RET

