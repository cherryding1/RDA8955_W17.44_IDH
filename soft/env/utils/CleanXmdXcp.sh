#!/bin/bash

find . -name '*.xmd' -exec bash -c "rm \`echo "{}"|sed s/xmd/h/g\`" \;
find . -name '*.xcp' -exec bash -c "rm \`echo "{}"|sed s/xcp/h/g\`" \;


