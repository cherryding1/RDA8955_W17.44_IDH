# cleanup script (bash version)
# remove all "${SOFT_WORKDIR}/env...:" from the path
oldsoft_workdir=${SOFT_WORKDIR_CLEAN_ENV/\/env\/clean_env/}
echo Cleaning PATH from $oldsoft_workdir
if [ -d ${oldsoft_workdir} ];then
    export PATH=`echo ${PATH} | sed "s;${oldsoft_workdir}/env[^:]*:;;g"`
    export -n SOFT_WORKDIR_CLEAN_ENV=
fi
oldsoft_workdir=
