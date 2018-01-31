# cleanup script (tcsh version)
# remove all "${SOFT_WORKDIR}/env...:" from the path
set oldsoft_workdir=`echo ${SOFT_WORKDIR_CLEAN_ENV} | sed "s;/env/clean_env;;"`
echo Cleaning PATH from $oldsoft_workdir
if ( -d ${oldsoft_workdir}) then
    setenv PATH `echo ${PATH} | sed "s;${oldsoft_workdir}/env[^:]*:;;g"`
    unsetenv SOFT_WORKDIR_CLEAN_ENV
endif
unset oldsoft_workdir
