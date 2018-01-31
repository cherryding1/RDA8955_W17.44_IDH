TB_TOP=tb_$(CT_ASIC)_chip
GEN_PAR=


#XEMUL_TYPE=$(shell cat /n/projects/$(CT_ASIC)/axis_last_comps/$(CT_ASIC)_chip)
XEMUL_TYPE=$(shell cat /n/projects/Emerald/axis_last_comps/emerald_chip)

SIMU_BINARY_PATH:=${BINARY_PATH}
#MEMBRIDGE_ROM_DIR=$(ASIC_WORKDIR)/romimages/
MEMBRIDGE_ROM_DIR=/n/projects/Emerald/jba/Emerald/asic/romimages/

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
ifneq ($(VOC_ROM_DIR),)
	$(MAKE) -C $(VOC_ROM_DIR) voc_romz.dat
	cp $(VOC_ROM_DIR)/voc_romz.dat ${SIMU_BINARY_PATH}/voc_romz.dat
endif
ifneq (${FAKE_ROM}, 1)
	$(MAKE) -C $(MEMBRIDGE_ROM_DIR) mem_bridge_rom.dat
	cp -f $(MEMBRIDGE_ROM_DIR)/mem_bridge_rom.dat ${SIMU_BINARY_PATH}/mem_bridge_rom.dat
endif

${SIMU_BINARY_PATH}/testprnt.lst:
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
	@touch ${BINARY_PATH}/usbc_ram.dat
	@if [[ -n "$(INPUT_FILES)" ]]; then \
		cp -f $(INPUT_FILES) ${SIMU_BINARY_PATH}/. ;\
	fi



xsim: sim_touch

run_xemul: xsim
	@echo "---------------------------------------"
	@echo "using compiled design" $(XEMUL_TYPE)
	@echo "---------------------------------------"
	cd ${SIMU_BINARY_PATH}; rm -f vlg_hw AxisWork; \
	ln -s /vulcain/AXISWORK/Emerald/$(XEMUL_TYPE)/emerald_chip/AxisWork AxisWork; \
	ln -s /vulcain/AXISWORK/Emerald/$(XEMUL_TYPE)/emerald_chip/vlg_$(TB_TOP) vlg_hw; 
	@echo launching xtreme emulation 
	cd ${SIMU_BINARY_PATH}; ./vlg_hw -s -i cmd_hw.i +rccSpeed+23

