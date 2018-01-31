TB_TOP=tb_$(CT_ASIC)_chip
GEN_PAR=
XEMUL_TYPE=$(shell cat /n/projects/${PROJ}/last_comp)


#--------------------------------------------------------------------------
## board type switch. 
#--------------------------------------------------------------------------
# BOARD_NUMBER is also set up and passed to preprocessor, as a bitmask, 
# to set up board dependent code options
# We also define -D<BOARD> as legacy support, but it should be obsoleted soon
# BOARD_TYPE_FPGA 		1
# BOARD_TYPE_ENG		2
# BOARD_TYPE_DEV		4
# BOARD_TYPE_SIMU		8
# BOARD_TYPE_ANALOG		16

ifeq "${CT_TARGET}" "fpga"
BOARD_NUMBER := 1
else # !fpga
ifeq "${CT_TARGET}" "eng"
BOARD_NUMBER := 2 
else # !eng
ifeq "${CT_TARGET}" "simu"
BOARD_NUMBER := 8 
# For Simu the ram is preloaded
LDPPFLAGS += -DPRELOADED_RAM
else # !simu
ifeq "${CT_TARGET}" "ana"
BOARD_NUMBER := 16
else # !ana
# For all other ones
BOARD_NUMBER := 4 
endif # CT_TARGET "ana"
endif # CT_TARGET "simu"
endif # CT_TARGET "eng"
endif # CT_TARGET "fpga"

MYCPPFLAGS := ${MYCPPFLAGS} -DBOARD_NUMBER=${BOARD_NUMBER}

############################ Phy Delivery #######################################

# TODO: Update the delivery targets...

phy_clean_includes:
ifeq "WITH_SVN" "1"
	cd ${SOFT_WORKDIR}/phy;../env/utils/CleanTopInclude.pl
endif

phy_build_version:
	@${ECHO} "-----------------------------------------"    
	@${ECHO} "Building PHY version $(PHY_VERSION)"    
	@${ECHO} "-----------------------------------------"    

PHY_RELEASE_LIST = "release debug debug_romu cool_profile sa_profile calib"

ifdef PHY_VERSION
MYCPPFLAGS := ${MYCPPFLAGS} -DPHY_DATE_STRING=${BUILD_DATE} -DPHY_VERSION=$(PHY_VERSION)
phy_deliv:  clean phy_clean_includes phy_build_version 
	cat hal/include/hal.h | sed 's/ct_types/cswtype/g;s/u8/UINT8/g;s/u16/UINT16/g;s/u32/UINT32/g;s/s8/INT8/g;s/s16/INT16/g;s/s32/INT32/g;s/bool/BOOL/g' > include/hal_CT.h
	echo
	echo "Generating the Doxygen documentation for HAL..."
	rm -rf ../phy/deliv/HAL_User_Manual_v${PHY_VERSION}
	cd ../../docgen/; cat hal.doxyfile > hal_version.doxyfile; echo "PROJECT_NUMBER         = ${PHY_VERSION}" >> hal_version.doxyfile; doxygen hal_version.doxyfile >& /dev/null; mkdir DOC/HAL_User_Manual_v${PHY_VERSION} >& /dev/null; cat hal_no_version.html | sed "s/PHY_VERSION/${PHY_VERSION}/g" > DOC/HAL_User_Manual_v${PHY_VERSION}/hal.html; cp -rf DOC/html DOC/HAL_User_Manual_v${PHY_VERSION}/hal_v${PHY_VERSION}; mv DOC/HAL_User_Manual_v${PHY_VERSION} ../firmware/phy/deliv
	cd deliv/; zip -r HAL_User_Manual_v${PHY_VERSION}.zip HAL_User_Manual_v${PHY_VERSION} > /dev/null
    # Here, we use a fake release so that the first time the makefile
    # is evaluated, no flags are added to the variable LOCAL_EXPORT_FLAGS.
	$(MAKE) deliv CT_RELEASE=fake_for_deliv RELEASE_LIST=${PHY_RELEASE_LIST}
else
phy_deliv:force
	@${ECHO} "---------------------------------------------------"
	@${ECHO} " ERROR : PHY_VERSION MUST BE DEFINED"
	@${ECHO} "---------------------------------------------------"
endif



############################ Firmware Cfg Delivery ###################################

firmware_cfg_clean_includes:
ifeq "WITH_CVS" "1"
	cd ${SOFT_WORKDIR}/firmware_cfg;CleanTopInclude.pl
endif
	
MODULE_COPY := ${foreach module, ${LOCAL_MODULE_DEPENDS}, \
		${ECHO} "CP		  ${module}"; \
		mkdir -p ${DELIV_PATH}/${module}/lib/; \
		cp -f 	${INT_OBJECT_DIR}/${module}/lib/lib${subst /,_,${module}}_${CT_RELEASE}.a \
				${DELIV_PATH}/${module}/lib/.; \
		cp -f 	${INT_OBJECT_DIR}/${module}/lib/lib${subst /,_,${module}}_${CT_RELEASE}.a \
				${DELIV_PATH}/firmware_cfg/cust/.;} \

firmware_cfg_build_version:
	@${ECHO} "-----------------------------------------"    
	@${ECHO} "Building PHY Cfg version $(FIRMWARE_CFG_VERSION)"    
	@${ECHO} "-----------------------------------------"    

firmware_cfg_module_copy:
	mkdir -p ${DELIV_PATH}/firmware_cfg/cust/
	${MODULE_COPY}

firmware_cfg_deliv_release:
	$(MAKE) clean RELEASE_MODULE=1
	$(MAKE) binlib RELEASE_MODULE=1 CT_RELEASE=release
	$(MAKE) binlib RELEASE_MODULE=1 CT_RELEASE=calib
	$(MAKE) firmware_cfg_module_copy RELEASE_MODULE=1 CT_RELEASE=release
	$(MAKE) firmware_cfg_module_copy RELEASE_MODULE=1 CT_RELEASE=calib
ifndef CN
	cp ./Makefile ${DELIV_PATH}/firmware_cfg/.
	mkdir -p ${DELIV_PATH}/firmware_cfg/rfd/include/
	cp ./rfd/include/rf_names.h ${DELIV_PATH}/firmware_cfg/rfd/include/.
endif

ifdef FIRMWARE_CFG_VERSION
ifeq '${PARAMETERS_CHECK}' ''
MYCPPFLAGS := ${MYCPPFLAGS} -DFIRMWARE_CFG_DATE_STRING=${BUILD_DATE} -DFIRMWARE_CFG_VERSION=$(FIRMWARE_CFG_VERSION)
firmware_cfg_deliv: firmware_cfg_clean_includes
	rm -rf ${DELIV_PATH}/firmware_cfg
	$(MAKE) firmware_cfg_deliv_release 
	rm -rf ${DELIV_PATH}/lib
ifdef TOPLEVEL_DELIVER_SRCMOD
	-${FULL_DELIVER_SRCMOD_CLEAN}
	-${FULL_DELIVER_SRCMOD_CPY}
	${foreach src_mod, ${TOPLEVEL_DELIVER_SRCMOD},	\
		cd ${DELIV_PATH}/${LOCAL_NAME}/${src_mod}; CleanSrcDeliv.pl;} 
endif # TOPLEVEL_DELIVER_SRCMOD
# remove CLEANED files
	-${FIND} ${DELIV_PATH} -type f -name "*_CLEANED" -exec rm -f \{\} \; ${STDERR_NULL}
# remove trailing CVS directories
	-${FIND} ${DELIV_PATH} -type d -name CVS -exec rm -fr \{\} \; ${STDERR_NULL}
# remove .* files
	-${FIND} ${DELIV_PATH} -type f -name ".*" -exec rm -f \{\} \; ${STDERR_NULL}
	@${ECHO} "ZIP               libs"
	rm -f ${DELIV_PATH}/${DELIV_FILE}
	cd ${DELIV_PATH} && zip -9r ${DELIV_PATH}/${DELIV_FILE} ${LOCAL_NAME} ${STDOUT_NULL}
else
firmware_cfg_deliv:force
	@${ECHO} "---------------------------------------------------"
	@${ECHO} " ERROR : ${PARAMETERS_CHECK}"
	@${ECHO} "---------------------------------------------------"
endif
else
firmware_cfg_deliv:force
	@${ECHO} "---------------------------------------------------"
	@${ECHO} " ERROR : FIRMWARE_CFG_VERSION MUST BE DEFINED"
	@${ECHO} "---------------------------------------------------"
endif

firmware_cfg_src_deliv:
	$(MAKE) src_deliv RELEASE_MODULE=1


########################### Delivery Testsuite ##################################
SANDBOX_DIR := sandbox
SANDBOX_PATH := ${SOFT_WORKDIR}/${SANDBOX_DIR}
TESTS_MK := ${SANDBOX_PATH}/${LOCAL_NAME}/tests/make.tests	

deliv_test: 
	mkdir -p ${SANDBOX_PATH}
	mkdir -p ${SANDBOX_PATH}/hex
	rm -fr ${SANDBOX_PATH}/utils ${SANDBOX_PATH}/${LOCAL_NAME}
	cp -f ${SOFT_WORKDIR}/make.toplevel.test ${SANDBOX_PATH}/make.toplevel
	cp -f ${SOFT_WORKDIR}/custom.rules ${SANDBOX_PATH}/custom.rules
	@echo UNZIP\ \ \ \ \ \ \ \ \ \ \ \ \ Delivery File
	@echo
ifneq "${VERBOSE}" "1"
	cd ${SANDBOX_PATH} &&  unzip -o ${DELIV_PATH}/${DELIV_FILE} 1>/dev/null
else
	cd ${SANDBOX_PATH} &&  unzip -o ${DELIV_PATH}/${DELIV_FILE}
endif # VERBOSE
	cp -f ${SOFT_WORKDIR}/${LOCAL_NAME}/tests/make.tests ${TESTS_MK}
	SOFT_WORKDIR=${SANDBOX_PATH} make VERBOSE=${VERBOSE} CT_BOARD=${CT_BOARD} -C ${SANDBOX_PATH}/${LOCAL_NAME}/tests -f ${TESTS_MK} tests

########################### Reference tests delivery ##################################
# This is the list of the tests that will be delivered.
REFTEST_LIST := reftest_skel reftest_batmon reftest_uart reftest_dictaphone reftest_flash reftest_debug reftest_PowerManagment
REFTESTS := ${foreach reftest, ${REFTEST_LIST}, ${SOFT_WORKDIR}/${LOCAL_NAME}/tests/${reftest}}
REFTESTS_DELIV_FILE = ReftestsDelivery_${DELIV_CUR_DATE}.zip

ifneq "${DELIV_NO_CVS_CHECK}" "1"
REFTESTS_CHECK := ${foreach testdir, ${REFTESTS}, cd ${testdir} ; cvs -q diff | grep "[><]" && echo "CVS not up to date, aborting" && exit 1 || echo "      (${notdir ${testdir}} OK)"; }
SRCDIRS_CHECK := ${foreach modname, ${TOPLEVEL_DELIVER_SRCMOD}, cd ${modname} ; cvs -q diff | grep "[><]" && echo "CVS not up to date, aborting" && exit 1 || echo "      (${notdir ${modname}} OK)"; }
else
REFTESTS_CHECK := @echo "      (CVS check not activated, skipped it)"
endif # DELIV_NO_CVS_CHECK

REFTESTS_COPY := ${foreach testdir, ${REFTESTS}, cp -r ${testdir} ${DELIVTMP_ROOT_DIR}/tests;}

REFTESTS_CLEANUP := ${foreach testdir, ${REFTESTS}, rm -rf ${DELIVTMP_ROOT_DIR}/tests/${notdir ${testdir}}/obj; rm -rf ${DELIVTMP_ROOT_DIR}/tests/${notdir ${testdir}}/deps ; rm -rf ${DELIVTMP_ROOT_DIR}/tests/${notdir ${testdir}}/CVS ; rm -f ${DELIVTMP_ROOT_DIR}/tests/${notdir ${testdir}}/\.\#*; }

REFTESTS_CLEAN := ${foreach testdir, ${REFTESTS}, make -C ${testdir} locallclean hexclean ;} 

reftest_deliv:
	@echo CHECK\ \ \ \ \ \ \ \ \ \ \ \ \ Reference Tests
	${REFTESTS_CHECK}
	@echo CHECK\ \ \ \ \ \ \ \ \ \ \ \ \ Utils
	${SRCDIRS_CHECK}
	@echo
	@echo CLEAN\ \ \ \ \ \ \ \ \ \ \ \ \ Reference Tests
	${REFTESTS_CLEAN}
	@echo CLEAN\ \ \ \ \ \ \ \ \ \ \ \ \ Utils
	@echo
	-${FULL_DELIVER_SRCMOD_CLEAN}
	rm -f ${DELIV_PATH}/${REFTESTS_DELIV_FILE}
	rm -fr ${DELIVTMP_ROOT_DIR}
	mkdir -p ${DELIVTMP_ROOT_DIR}/tests
	@echo COPY\ \ \ \ \ \ \ \ \ \ \ \ \ \ Reference Tests
	${REFTESTS_COPY}
	${REFTESTS_CLEANUP}
	@echo COPY\ \ \ \ \ \ \ \ \ \ \ \ \ \ Utils
	-${FULL_DELIVER_SRCMOD_CPY}
	-find ${DELIV_PATH} -type d -name CVS -exec rm -fr \{\} \; 2>/dev/null
	@echo
	@echo ZIP\ \ \ \ \ \ \ \ \ \ \ \ \ \ \ Reference Tests \& Utils
	cd ${DELIV_PATH} && zip -9r ${DELIV_PATH}/${REFTESTS_DELIV_FILE} ./${LOCAL_NAME} 1>/dev/null
	@echo \ \ \ \ ------------------------------------------------
	@echo \ \ \ \ \ \ ${REFTESTS_DELIV_FILE} generated
	@echo \ \ \ \ ------------------------------------------------

####### Testsuite for reftest delivery #########

reftest_deliv_test: 
	test -f ${DELIV_PATH}/${DELIV_FILE} && test -f ${DELIV_PATH}/${REFTESTS_DELIV_FILE} || echo "*** Delivery files not found ***"
	mkdir -p ${SANDBOX_PATH}
	mkdir -p ${SANDBOX_PATH}/hex
	rm -fr ${SANDBOX_PATH}/utils ${SANDBOX_PATH}/${LOCAL_NAME}
	cp -f ${SOFT_WORKDIR}/make.toplevel.test ${SANDBOX_PATH}/make.toplevel
	cp -f ${SOFT_WORKDIR}/custom.rules ${SANDBOX_PATH}/custom.rules
	@echo UNZIP\ \ \ \ \ \ \ \ \ \ \ \ \ Delivery File
	@echo
ifneq "${VERBOSE}" "1"
	cd ${SANDBOX_PATH} &&  unzip -o ${DELIV_PATH}/${DELIV_FILE} 1>/dev/null
else
	cd ${SANDBOX_PATH} &&  unzip -o ${DELIV_PATH}/${DELIV_FILE}
endif # VERBOSE
	@echo UNZIP\ \ \ \ \ \ \ \ \ \ \ \ \ Reference Tests \& Utils
	@echo
ifneq "${VERBOSE}" "1"
	cd ${SANDBOX_PATH} &&  unzip -o ${DELIV_PATH}/${REFTESTS_DELIV_FILE} 1>/dev/null
else
	cd ${SANDBOX_PATH} &&  unzip -o ${DELIV_PATH}/${REFTESTS_DELIV_FILE}
endif # VERBOSE
	cp -f ${SOFT_WORKDIR}/${LOCAL_NAME}/tests/make.tests ${TESTS_MK}
	SOFT_WORKDIR=${SANDBOX_PATH} make VERBOSE=${VERBOSE} CT_BOARD=${CT_BOARD} -C ${SANDBOX_PATH}/${LOCAL_NAME}/tests -f ${TESTS_MK} tests
	@echo
	@echo "*****************************"
	@echo "*   Testsuite completed OK  *"
	@echo "*     Have a nice day :)    *"
	@echo "*****************************"



########################### Simu targets ########################################
# There is some  more work for simu, because the simulators use fixed input names
# Thus we simply copy the lod files
ifeq "${CT_TARGET}" "simu"
simu: lod
	cp -f ${LODBASE}ram0.dat ${BINARY_PATH}/ram0.dat	
	cp -f ${LODBASE}flash0.dat ${BINARY_PATH}/flash0.dat	
	test -f ${LODBASE}ram1.dat && \
		cp -f ${LODBASE}ram1.dat ${BINARY_PATH}/ram1.dat || \
		touch ${BINARY_PATH}/ram1.dat
	test -f ${LODBASE}flash1.dat && \
		cp -f ${LODBASE}flash1.dat ${BINARY_PATH}/flash1.dat || \
		touch ${BINARY_PATH}/flash1.dat
else
simu:
	@echo "*********************************"
	@echo "* CT_TARGET not set to simu 	   *"
	@echo "* use: make CT_TARGET=simu simu *"
	@echo "*********************************"
endif


sim_touch: simu
	@touch ${BINARY_PATH}/flash0.dat
	@touch ${BINARY_PATH}/flash1.dat
	@touch ${BINARY_PATH}/ram0.dat
	@touch ${BINARY_PATH}/ram1.dat
	@touch ${BINARY_PATH}/pcm_in.pcm
	@touch ${BINARY_PATH}/au_in.pcm
	@touch ${BINARY_PATH}/rf_if_in.dat
	@touch ${BINARY_PATH}/gpadc_data.dat
	@touch ${BINARY_PATH}/input.dat
	@touch ${BINARY_PATH}/voc_ramx.dat
	@touch ${BINARY_PATH}/voc_ramy.dat
	@touch ${BINARY_PATH}/voc_romz.dat
	@touch ${BINARY_PATH}/voc_code.dat
	@touch ${BINARY_PATH}/rf_if_acco_inI.dat
	@touch ${BINARY_PATH}/rf_if_acco_inQ.dat
	@if [[ -n "$(INPUT_FILES)" ]]; then \
		cp -f $(INPUT_FILES) ${BINARY_PATH}/. ;\
	fi

xsim: sim_touch

msim: sim_touch

run_msim: msim
	$(MAKE) -C ${ASIC_WORKDIR}/chip/tb/ $(TB_TOP).mlog
	@echo modelsim launching top simulation
	cd ${BINARY_PATH}; vsim -lib $(RTL_WORK) -wav /tmp/$(USER)_$(PROJ).topsimwav \
		-t ps -do "do $(TB_TOP).do" +notimingchecks $(TB_TOP) $(GEN_PAR)
	rm /tmp/$(USER)_$(PROJ).topsimwav

run_msimb: msim
	$(MAKE) -C ${ASIC_WORKDIR}/chip/tb $(TB_TOP).mlog
	@echo modelsim launching batch top simulation
	cd ${BINARY_PATH}; vsim -c -lib $(RTL_WORK) -t ps -do "run -all; quit -f" +notimingchecks $(TB_TOP)

run_msimg_max: msim
	# Compile the test bench for gate sim.
	vlog -work gate /n/projects/Jade/$(USER)/Jade/rtl/top/acco_analog.v
	$(MAKE) -C ${PROJ_DIR}/rtl/top $(TB_TOP).mlog
	vcom -work $(RTL_WORK) /n/projects/Jade/$(USER)/Jade/rtl/top/tb_jade_chip_gate_cfg.vhd
	# Launch the gate sim with timing check.
	@echo modelsim launching top simulation gate level
	cd ${BINARY_PATH}; vsim -lib $(RTL_WORK) -wav /tmp/$(USER)_$(PROJ).topsimwav \
		-t ps -do "do notifier_reg.do" -sdfmax /cmp_jade_chip=/n/projects/Jade/syn/INOUT/jade_msim.sdf -L TSMC +nowarnTSCALE +nowarnTFMPC +maxdelays tb_jade_chip_gate_cfg
	rm /tmp/$(USER)_$(PROJ).topsimwav

run_msimbg_max: msim
	# Compile the test bench for gate sim.
	vlog -work gate /n/projects/Jade/$(USER)/Jade/rtl/top/acco_analog.v
	$(MAKE) -C ${PROJ_DIR}/rtl/top $(TB_TOP).mlog
	vcom -work $(RTL_WORK) /n/projects/Jade/$(USER)/Jade/rtl/top/tb_jade_chip_gate_cfg.vhd
	# Launch the gate sim with timing check.
	@echo modelsim launching top simulation gate level
	cd ${BINARY_PATH}; vsim -lib $(RTL_WORK) -wav /tmp/$(USER)_$(PROJ).topsimwav \
		-c -t ps -do "do notifier_reg.do ; run -all ; quit -f" -sdfmax /cmp_jade_chip=/n/projects/Jade/syn/INOUT/jade_msim.sdf -L TSMC +nowarnTSCALE +nowarnTFMPC +maxdelays tb_jade_chip_gate_cfg
	rm /tmp/$(USER)_$(PROJ).topsimwav

run_msimg_min: msim
	# Compile the test bench for gate sim.
	vlog -work gate /n/projects/Jade/$(USER)/Jade/rtl/top/acco_analog.v
	$(MAKE) -C ${PROJ_DIR}/rtl/top $(TB_TOP).mlog
	vcom -work $(RTL_WORK) /n/projects/Jade/$(USER)/Jade/rtl/top/tb_jade_chip_gate_cfg.vhd
	# Launch the gate sim with timing check.
	@echo modelsim launching top simulation gate level
	cd ${BINARY_PATH}; vsim -lib $(RTL_WORK) -wav /tmp/$(USER)_$(PROJ).topsimwav \
		-t ps -do "do notifier_reg.do" -sdfmin /cmp_jade_chip=/n/projects/Jade/syn/INOUT/jade_msim.sdf -L TSMC +nowarnTSCALE +nowarnTFMPC +mindelays tb_jade_chip_gate_cfg
	rm /tmp/$(USER)_$(PROJ).topsimwav

run_msimbg_min: msim
	# Compile the test bench for gate sim.
	vlog -work gate /n/projects/Jade/$(USER)/Jade/rtl/top/acco_analog.v
	$(MAKE) -C ${PROJ_DIR}/rtl/top $(TB_TOP).mlog
	vcom -work $(RTL_WORK) /n/projects/Jade/$(USER)/Jade/rtl/top/tb_jade_chip_gate_cfg.vhd
	# Launch the gate sim with timing check.
	@echo modelsim launching top simulation gate level
	cd ${BINARY_PATH}; vsim -lib $(RTL_WORK) -wav /tmp/$(USER)_$(PROJ).topsimwav \
		-c -t ps -do "do notifier_reg.do ; run -all ; quit -f" -sdfmin /cmp_jade_chip=/n/projects/Jade/syn/INOUT/jade_msim.sdf -L TSMC +nowarnTSCALE +nowarnTFMPC +mindelays tb_jade_chip_gate_cfg
	rm /tmp/$(USER)_$(PROJ).topsimwav

run_xsim: xsim
	$(MAKE) -C ${ASIC_WORKDIR}/chip/tb $(TB_TOP).xelab
	cd ${BINARY_PATH}; rm -f vlg_sw AxisWork; \
	ln -s ${XSIM_WORK}/AxisWork AxisWork; \
	ln -s ${XSIM_WORK}/vlg_$(TB_TOP) vlg_sw; 
	@echo launching xsim simulation
	cd ${BINARY_PATH}; vlg_sw -s -i cmd_sw.i

run_xemul: xsim
	@echo "---------------------------------------"
	@echo "using compiled design" $(XEMUL_TYPE)
	@echo "---------------------------------------"
	cd ${BINARY_PATH}; rm -f vlg_hw AxisWork; \
	ln -s /vulcain/AXISWORK/${PROJ}/$(XEMUL_TYPE)/debussy_work.lib++ debussy_work.lib++; \
	ln -s /vulcain/AXISWORK/${PROJ}/$(XEMUL_TYPE)/AxisWork AxisWork; \
	ln -s /vulcain/AXISWORK/${PROJ}/$(XEMUL_TYPE)/vlg_$(TB_TOP) vlg_hw; 
	@echo launching xtreme emulation 
	cd ${BINARY_PATH}; vlg_hw -s -i cmd_hw.i

run_xplore:
	@echo "Launching xplore debugging environment..."
	cd ${BINARY_PATH}; xplore -lib $(RTL_WORK) -top $(TB_TOP) & vlg_hw -s -i cmd_replay.i +rccplay+test_vcd -l replay.log +vhseverity+error

# Prototype of a gate level run.
run_xemulg: xsim
	@echo "---------------------------------------"
	@echo "using compiled design axis_gate"
	@echo "   (Gate level)"
	@echo "---------------------------------------"
	cd ${BINARY_PATH}; rm -f vlg_hw AxisWork; \
	ln -s /vulcain/AXISWORK/$(PROJ)/axis_gate_in/AxisWork AxisWork; \
	ln -s /vulcain/AXISWORK/$(PROJ)/axis_gate_in/vlg_tb_jade_chip_gate vlg_hw; 
	@echo launching xtreme emulation gate
	cd ${BINARY_PATH}; vlg_hw -s -i cmd_hwg.i
	
run_xemuld: msim #########xsim
	@echo "---------------------------------------"
	@echo "using compiled design axis_diff"
	@echo "   (Gate level against RTL level)"
	@echo "---------------------------------------"
#	cd ${BINARY_PATH}; rm -f vlg_hw AxisWork; \
#	ln -s /vulcain/AXISWORK/$(PROJ)/axis_gate/AxisWork AxisWork; \
#	ln -s /vulcain/AXISWORK/$(PROJ)/axis_gate/vlg_tb_jade_chip_gate vlg_hw; 
#	@echo launching xtreme emulation gate
#	cd ${BINARY_PATH}; vlg_hw -s -i cmd_hwg.i
	cd ${BINARY_PATH}; vsim -lib $(RTL_WORK) -wav /tmp/$(USER)_$(PROJ).topsimwav -t ps -do "do tb_jade_chip_diff.do" +notimingchecks tb_jade_chip_diff


