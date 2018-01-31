#!/bin/bash

REVISION="$1"

if [[ $REVISION = "" ]]
then
  REVISION="HEAD"
fi

EXTERNAL_LINKS="application at env platform platform/base platform/chip \
    ${PLATFORM_SERVICE} platform/edrv ${PLATFORM_SYSTEM}/mdi ${PLATFORM_SYSTEM}/stack ${PLATFORM_SYSTEM}/svc \
    ${PLATFORM_SYSTEM}/vpp platform_test target toolpool"

for subdir in ${EXTERNAL_LINKS} . ; do
    if [ -d ${subdir} ]; then
        printf "\nsvn up --ignore-externals %-15s -r ${REVISION}\n" ${subdir}
        svn up --ignore-externals ${subdir} -r ${REVISION}
    fi
done

