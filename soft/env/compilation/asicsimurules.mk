TB_TOP=tb_$(CT_ASIC)_chip
# to use the full rom boot sequence (default when using regular code)
GEN_PAR?="-GBOOT_MODE=0 "

# for functionnal test
#GEN_PAR:="-GFUNCT_TEST=1 "


TOP_MODULE?=chip
TB_DIR:=$(TOP_MODULE)/tb

MKTEMP=mktemp


XEMUL_TYPE=$(shell cat /n/projects/$(CT_ASIC_CAPD)/axis_last_comps/$(CT_ASIC)_chip)

SIMU_BINARY_PATH:=${BINARY_PATH}/simu
#MEMBRIDGE_ROM_DIR=$(ASIC_WORKDIR)/romimages/
MEMBRIDGE_ROM_DIR=/g/projects/$(CT_ASIC_CAPD)/$(USER)/$(CT_ASIC_CAPD)/asic/romimages/

${SIMU_BINARY_PATH}:
	mkdir -p ${SIMU_BINARY_PATH}


simu: lod ${SIMU_BINARY_PATH}
	cp -f ${LODBASE}ram0.dat ${SIMU_BINARY_PATH}/ram0.dat	
	cp -f ${LODBASE}flash0.dat ${SIMU_BINARY_PATH}/flash0.dat	
	test -f ${LODBASE}ram1.dat && \
		cp -f ${LODBASE}ram1.dat ${SIMU_BINARY_PATH}/ram1.dat || \
		touch ${SIMU_BINARY_PATH}/ram1.dat
	test -f ${LODBASE}flash1.dat && \
		cp -f ${LODBASE}flash1.dat ${SIMU_BINARY_PATH}/flash1.dat || \
		touch ${SIMU_BINARY_PATH}/flash1.dat
ifeq (${FAKE_ROM}, 1)
	rom_crc ${LODBASE}mem_bridge_rom.dat ${SIMU_BINARY_PATH}/mem_bridge_rom.dat -c 0xc001 -s 20480 ${STDOUT_NULL}
endif

copy_roms_files: simu
ifneq (${FAKE_ROM}, 1)
	$(MAKE) -C $(MEMBRIDGE_ROM_DIR) mem_bridge_rom.dat
	cp -f $(MEMBRIDGE_ROM_DIR)/mem_bridge_rom.dat ${SIMU_BINARY_PATH}/mem_bridge_rom.dat
endif

${SIMU_BINARY_PATH}/testprnt.lst: ${SIMU_BINARY_PATH}
	@echo -ne "xcpu\nbcpu\n" >${SIMU_BINARY_PATH}/testprnt.lst
	for i in `seq 0 3` ; do echo state$$i; done >>${SIMU_BINARY_PATH}/testprnt.lst
	for i in `seq 0 63` ; do echo test$$i; done >>${SIMU_BINARY_PATH}/testprnt.lst

sim_touch: ${SIMU_BINARY_PATH}/testprnt.lst copy_roms_files
	@touch ${SIMU_BINARY_PATH}/flash0.dat
	@touch ${SIMU_BINARY_PATH}/flash1.dat
	@touch ${SIMU_BINARY_PATH}/ram0.dat
	@touch ${SIMU_BINARY_PATH}/ram1.dat
	@touch ${SIMU_BINARY_PATH}/pcm_in.pcm
	@touch ${SIMU_BINARY_PATH}/au_in.pcm
	@touch ${SIMU_BINARY_PATH}/rf_if_in.dat
	@touch ${SIMU_BINARY_PATH}/gpadc_data.dat
	@touch ${SIMU_BINARY_PATH}/gpadc_simdata.dat
	@touch ${SIMU_BINARY_PATH}/input.dat
	@touch ${SIMU_BINARY_PATH}/mem_bridge_rom.dat
	@touch ${SIMU_BINARY_PATH}/voc_ramx.dat
	@touch ${SIMU_BINARY_PATH}/voc_ramy.dat
	@touch ${SIMU_BINARY_PATH}/voc_romz.dat
	@touch ${SIMU_BINARY_PATH}/voc_code.dat
	@touch ${SIMU_BINARY_PATH}/voc_ramx_expected_lo.dat
	@touch ${SIMU_BINARY_PATH}/voc_ramx_expected_hi.dat
	@touch ${SIMU_BINARY_PATH}/voc_ramy_expected_lo.dat
	@touch ${SIMU_BINARY_PATH}/voc_ramy_expected_hi.dat
	@touch ${SIMU_BINARY_PATH}/rf_if_acco_inI.dat
	@touch ${SIMU_BINARY_PATH}/rf_if_acco_inQ.dat
	@touch ${SIMU_BINARY_PATH}/picture_yuv.dat
	@touch ${SIMU_BINARY_PATH}/usbc_ram.dat
	@if [[ -n "$(INPUT_FILES)" ]]; then \
		cp -f $(INPUT_FILES) ${SIMU_BINARY_PATH}/. ;\
	fi


xsim: sim_touch

ncsim: sim_touch

msim: sim_touch

##############################################################################
## MODELSIM
##############################################################################
run_msim: msim
	$(MAKE) -C ${ASIC_WORKDIR}/$(TB_DIR) $(TB_TOP).mlog
	@echo modelsim launching top simulation
	TMPWAVFILE=`${MKTEMP} /tmp/$(USER)_$(PROJ).${NBR}topsimwav.XXXXXX` && cd ${SIMU_BINARY_PATH}; vsim -lib $(RTL_WORK) -wav $$TMPWAVFILE \
		-t ps -do "do $(TB_TOP).do" +notimingchecks $(TB_TOP) $(GEN_PAR) && rm $$TMPWAVFILE

run_msimb: msim
	$(MAKE) -C ${ASIC_WORKDIR}/$(TB_DIR) $(TB_TOP).mlog
	@echo modelsim launching batch top simulation
	cd ${SIMU_BINARY_PATH}; vsim -c -lib $(RTL_WORK) -t ps -do "run -all; quit -f" +notimingchecks $(TB_TOP) $(GEN_PAR)


##############################################################################
## NCSIM
##############################################################################
run_ncsim: ncsim
	$(MAKE) -C ${ASIC_WORKDIR}/$(TB_DIR) $(TB_TOP).nclog
	@echo ncsim launching top simulation
	cd ${SIMU_BINARY_PATH}; \
	ncelab -WORK $(RTL_WORK) -NOTIMINGCHECKS -ACCESS RWC -TIMESCALE 1ns/1ps $(NC_GEN_PAR) $(TB_TOP);\
	ncsim -GUI -INPUT "@database  $(USER)_$(PROJ) -into /tmp/$(USER)_ncsim -default -shm;probe : -all -shm" $(TB_TOP)

run_ncsimb: ncsim
	$(MAKE) -C ${ASIC_WORKDIR}/$(TB_DIR) $(TB_TOP).nclog
	@echo ncsim launching top simulation
	cd ${SIMU_BINARY_PATH}; \
	ncelab -WORK $(RTL_WORK) -NOTIMINGCHECKS -TIMESCALE 1ns/1ps $(NC_GEN_PAR) $(TB_TOP);\
	ncsim $(TB_TOP) -REDMEM

##############################################################################
## Axis
##############################################################################

run_xemul: xsim
	@echo "---------------------------------------"
	@echo "using compiled design" $(XEMUL_TYPE)
	@echo "---------------------------------------"
	cd ${SIMU_BINARY_PATH}; rm -f vlg_hw AxisWork; \
	ln -s /vulcain/AXISWORK/$(CT_ASIC_CAPD)/$(XEMUL_TYPE)/$(CT_ASIC)_chip/AxisWork AxisWork; \
	ln -s /vulcain/AXISWORK/$(CT_ASIC_CAPD)/$(XEMUL_TYPE)/$(CT_ASIC)_chip/vlg_$(TB_TOP) vlg_hw; 
	@echo launching xtreme emulation 
	cd ${SIMU_BINARY_PATH}; ./vlg_hw -s -i cmd_hw.i +rccSpeed+23

