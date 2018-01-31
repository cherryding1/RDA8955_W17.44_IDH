#!/bin/bash

echo ====== Delete source files not open to customer ======
echo

echo "------ Enter working dir: ${SOFT_WORKDIR} ------"
echo

cd ${SOFT_WORKDIR}
if [ "x$?" != "x0" ]; then
    echo
    echo "!!!!!! Failed to enter working dir !!!!!!"
    exit 1
fi

echo "------ Start to delete source files ------"
echo

rm -fr ./${PLATFORM_SYSTEM}/stack/*/src/
rm -fr ./${PLATFORM_SYSTEM}/stack/src/


rm -fr ./platform_test/

rm -fr ./toolpool/t_flash_programmer/SourceDepends/src
rm -fr ./toolpool/t_flash_programmer/SourceDepends/include
rm -fr ./toolpool/flash_programmer/SourceDepends/src
rm -fr ./toolpool/flash_programmer/SourceDepends/include

echo ======= Done =======

