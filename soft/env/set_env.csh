# must be sourced with tcsh

# the SOFT_WORKDIR_CLEAN_ENV variable is set to a script responsible to cleanup some stuff before setting a new env (mainly the path)
if ($?SOFT_WORKDIR_CLEAN_ENV) then
    if ( -f $SOFT_WORKDIR_CLEAN_ENV.csh) then
        source $SOFT_WORKDIR_CLEAN_ENV.csh
    endif
endif

## Set perl environment variables
setenv PERL5LIB ${SOFT_WORKDIR}/env/utils/lib
setenv PERL_RL "Perl"
setenv PERL_READLINE_NOWARN 1

if (`uname | grep -c CYGWIN` > 0) then  
    setenv PATH "${SOFT_WORKDIR}/env/win32:${PATH}"
endif
setenv PATH "${SOFT_WORKDIR}/env/utils:${PATH}"
setenv PATH "${SOFT_WORKDIR}/env/deliv:${PATH}"
setenv SOFT_WORKDIR_CLEAN_ENV "${SOFT_WORKDIR}/env/clean_env"

#set host = `hostname`
## use the OBJECT_DIR stuff on machines with big temp
## also create the directory if needed
#if ( ( $host == vulcain ) || ( $host == helios ) || ( $host == petrus ) || ( $host == jupiter ) || ( $host == pluton ) || ( $host == sancerre ) || ( $host == bergerac ) || ( $host == medoc ) || ( $host == cadillac ) ) then
#    setenv OBJECT_DIR /tmp/${USER}_obj/${PROJ}/`echo ${SOFT_WORKDIR} | sed "s;$PROJ_WORKDIR/;;" | tr "/" "_"`
#    if ( ! -e $OBJECT_DIR ) then
#        mkdir -p $OBJECT_DIR
#    endif
#endif

alias cds cd ${SOFT_WORKDIR}
alias cde cd ${SOFT_WORKDIR}/env
alias cdenv cd ${SOFT_WORKDIR}/env
alias cdpal cd ${SOFT_WORKDIR}/platform/chip/pal
alias cdp cd ${SOFT_WORKDIR}/platform/chip/pal
alias cdphy cd ${SOFT_WORKDIR}/platform/phy
alias cdhal cd ${SOFT_WORKDIR}/platform/chip/hal
alias cdboot cd ${SOFT_WORKDIR}/platform/chip/boot
alias cdrom cd ${SOFT_WORKDIR}/platform_test/hal/int_rom_programmer
alias cdh cd ${SOFT_WORKDIR}/platform/chip/hal
alias cdspp cd ${SOFT_WORKDIR}/platform/chip/bcpu/spp
alias cdspc cd ${SOFT_WORKDIR}/platform/chip/bcpu/spc
alias cdspal cd ${SOFT_WORKDIR}/platform/chip/bcpu/spal
alias cdespp cd ${SOFT_WORKDIR}/platform/chip/bcpu/spp/edge/src
alias cdespal cd ${SOFT_WORKDIR}/platform/chip/bcpu/spal/edge/src
alias cdgspp cd ${SOFT_WORKDIR}/platform/chip/bcpu/spp/gsm/src
alias cdgspal cd ${SOFT_WORKDIR}/platform/chip/bcpu/spal/gsm/src
alias cdrfd cd ${SOFT_WORKDIR}/platform/edrv/rfd
alias cdsx cd ${SOFT_WORKDIR}/platform/base/sx
alias cdstack cd ${SOFT_WORKDIR}/${PLATFORM_SYSTEM}/stack
alias cdtest cd ${SOFT_WORKDIR}/platform_test
alias cdtst cd ${SOFT_WORKDIR}/platform_test
alias cdt cd ${SOFT_WORKDIR}/platform_test
alias cdproto cd ${SOFT_WORKDIR}/platform_test/stack/proto
alias cdpro cd ${SOFT_WORKDIR}/platform_test/stack/proto
alias cdcal cd ${SOFT_WORKDIR}/platform_test/calib/calib_stub
alias cdhex cd ${SOFT_WORKDIR}/hex
alias cdimgs cd ${SOFT_WORKDIR}/${PLATFORM_SYSTEM}/svc/imgs
alias cdavps cd ${SOFT_WORKDIR}/${PLATFORM_SYSTEM}/svc/avps
alias cdavrs cd ${SOFT_WORKDIR}/${PLATFORM_SYSTEM}/svc/avrs
alias cdmps cd ${SOFT_WORKDIR}/${PLATFORM_SYSTEM}/svc/mps
alias cdmrs cd ${SOFT_WORKDIR}/${PLATFORM_SYSTEM}/svc/mrs
alias cdcams cd ${SOFT_WORKDIR}/${PLATFORM_SYSTEM}/svc/cams
alias cdcamd cd ${SOFT_WORKDIR}/platform/edrv/camd
alias cdtarget cd ${SOFT_WORKDIR}/target

alias cdmmi cd ${SOFT_WORKDIR}/application/coolmmi/main
alias cdm cd ${SOFT_WORKDIR}/application/coolmmi/main

echo "Soft base SOFT_WORKDIR=$SOFT_WORKDIR"


