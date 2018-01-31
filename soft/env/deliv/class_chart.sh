#!/bin/bash
# must be same order as in rdoc.sh
files="changeloglib.rb deliv delivlib.rb svnlib.rb workspacelib.rb wsh"
i=0
d="class_chart_skel.dot"
echo "digraph ClassChart {" > $d
for f in $files; do
    n="doc/dot/f_$i.dot"
    echo "Adding: $n"
    l=${f/.rb/}
    sed -e "s/digraph TopLevel/subgraph cluster_$l/" -e "s/TopLevel/$f/" $n >> $d
    i=`expr $i + 1`
    if [[ $i == 5 ]]; then
        i=1
    fi
done

echo "}" >> $d
