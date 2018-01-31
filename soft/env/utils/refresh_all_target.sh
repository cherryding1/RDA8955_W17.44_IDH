#!/bin/bash

set -e

if [ -z "$SOFT_WORKDIR" ]; then
    echo "SOFT_WORKDIR is unset"
    exit 1
fi

cd $SOFT_WORKDIR
echo "SOFT_WORKDIR=$SOFT_WORKDIR"

for target in $(ls $SOFT_WORKDIR/target); do
    if test -f $SOFT_WORKDIR/target/$target/target.config; then
        echo "refresh $target ......"
        $SOFT_WORKDIR/env/mkconfig.sh $target || true
    fi
done
