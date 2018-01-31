#!/bin/bash

REVISION="$1"

if [[ $REVISION = "" ]]
then
  REVISION="HEAD"
fi

EXTERNAL_LINKS="at env platform platform/base platform/chip platform/csw \
    platform/edrv platform/mdi platform/stack platform/svc \
    platform/vpp platform_test target toolpool"

for subdir in ${EXTERNAL_LINKS} . ; do
    if [ -d ${subdir} ]; then
        printf "\nsvn up --ignore-externals %-15s -r ${REVISION}\n" ${subdir}
        svn up --ignore-externals ${subdir} -r ${REVISION}
    fi
done

