#!/bin/bash

cd $SOFT_WORKDIR/platform/stack/api/src/
perl buildTrcDB.pl
cd $SOFT_WORKDIR/platform/stack/cb/src/
perl buildTrcDB.pl
cd $SOFT_WORKDIR/platform/stack/cc/src/
perl buildTrcDB.pl
cd $SOFT_WORKDIR/platform/stack/fsm/src/
perl buildTrcDB.pl
cd $SOFT_WORKDIR/platform/stack/interrat/src/
perl buildTrcDB.pl
cd $SOFT_WORKDIR/platform/stack/l1/src/
perl buildTrcDB.pl
cd $SOFT_WORKDIR/platform/stack/lap/src/
perl buildTrcDB.pl
cd $SOFT_WORKDIR/platform/stack/llc/src/
perl buildTrcDB.pl
cd $SOFT_WORKDIR/platform/stack/mm/src/
perl buildTrcDB.pl
cd $SOFT_WORKDIR/platform/stack/mssc/src/
perl buildTrcDB.pl
cd $SOFT_WORKDIR/platform/stack/rco/src/
perl buildTrcDB.pl
cd $SOFT_WORKDIR/platform/stack/rld/src/
perl buildTrcDB.pl
cd $SOFT_WORKDIR/platform/stack/rlp/src/
perl buildTrcDB.pl
cd $SOFT_WORKDIR/platform/stack/rlu/src/
perl buildTrcDB.pl
cd $SOFT_WORKDIR/platform/stack/rr/src/
perl buildTrcDB.pl
cd $SOFT_WORKDIR/platform/stack/sim/src/
perl buildTrcDB.pl
cd $SOFT_WORKDIR/platform/stack/sms/src/
perl buildTrcDB.pl
cd $SOFT_WORKDIR/platform/stack/snd/src/
perl buildTrcDB.pl
cd $SOFT_WORKDIR/platform/stack/ss/src/
perl buildTrcDB.pl

cd $SOFT_WORKDIR/platform/3gfd-20140211/as/cl2/
perl buildTrcDB.pl
cd $SOFT_WORKDIR/platform/3gfd-20140211/as/interrat/
perl buildTrcDB.pl
cd $SOFT_WORKDIR/platform/3gfd-20140211/as/ubmc/
perl buildTrcDB.pl
cd $SOFT_WORKDIR/platform/3gfd-20140211/as/ul2/
perl buildTrcDB.pl
cd $SOFT_WORKDIR/platform/3gfd-20140211/as/umac/
perl buildTrcDB.pl
cd $SOFT_WORKDIR/platform/3gfd-20140211/as/updcp/
perl buildTrcDB.pl
cd $SOFT_WORKDIR/platform/3gfd-20140211/as/uphy/
perl buildTrcDB.pl
cd $SOFT_WORKDIR/platform/3gfd-20140211/as/urabm/
perl buildTrcDB.pl
cd $SOFT_WORKDIR/platform/3gfd-20140211/as/urabv/
perl buildTrcDB.pl
cd $SOFT_WORKDIR/platform/3gfd-20140211/as/urlc/
perl buildTrcDB.pl
cd $SOFT_WORKDIR/platform/3gfd-20140211/as/urrc/
perl buildTrcDB.pl
cd $SOFT_WORKDIR/platform/3gfd-20140211/service/pbm/
perl buildTrcDB.pl



echo " "
echo "TraceDB Genenating ..."

cd $SOFT_WORKDIR
echo "--- " > toolpool/map/ChipStd/traceDb.yaml
grep -R 'TSTR(.*,[ \t]*0x[0-9a-zA-Z]*)' platform/base/sx/ platform/stack/ platform/3gfd-20140211/as platform/3gfd-20140211/service/pbm --exclude-dir=.svn --include="*.[ch]" | sed 's/.*TSTR(\(.*\),[ \t]*\(0x[0-9a-zA-Z]*\)).*/\2: \1/' |sort -n | uniq  >> toolpool/map/ChipStd/traceDb.yaml

echo " "
echo " "
echo "TraceDB Genenated"
