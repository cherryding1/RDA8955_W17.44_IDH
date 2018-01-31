/*************************************************************************/
/*                                                                       */
/*              Copyright (C) 2006, Coolsand Technologies, Inc.          */
/*                            All Rights Reserved                        */
/*                                                                       */
/*************************************************************************/
/*                                                                       */
/* FILE NAME                                                             */
/*      vppp_speech_asm.c                                                */
/*                                                                       */
/* DESCRIPTION                                                           */
/*     This contains the VoC assembly code for the Speech.               */
/*                                                                       */
/*************************************************************************/
///
///   @file vppp_speech.h
///   This contains the VoC assembly code for the Speech.
///

#include "cs_types.h"
#include "chip_id.h"

#ifdef CHIP_HAS_AP
#define MOVE_AEC_AND_NS_CODE_TO_RAM 0
#else // !CHIP_HAS_AP
// The following macro must be enabled to improve RF performance
// for CT1129 and CT8852D. Here it is enabled by default.
#define MOVE_AEC_AND_NS_CODE_TO_RAM 1
#endif // !CHIP_HAS_AP

//#ifndef MICRON_ALIGN
//#define MICRON_ALIGN(a)
// #endif



#if (MOVE_AEC_AND_NS_CODE_TO_RAM)
#define SPEECH_SRAMDATA_INTERNAL __attribute__((section (".sram_overlay")))
#else
#define SPEECH_SRAMDATA_INTERNAL
#endif

#ifdef VOCCODE_IN_PSRAM


#ifdef CHIP_HAS_AP
PROTECTED  INT32   g_vppSpeechMainCode[] = {
#include "vpp_speech_asm_main_modem.tab"
};
#else // !CHIP_HAS_AP

PROTECTED  INT32   g_vppSpeechMainCode[] = {
#include "vpp_speech_asm_main.tab"
};
#endif // !CHIP_HAS_AP




PROTECTED  INT32   g_vppHrCode[] = {
#include "vpp_speech_asm_hr.tab"
};

PROTECTED  INT32   g_vppEfrAmrCode[] = {
#include "vpp_speech_asm_amr_efr.tab"
};

PROTECTED  INT32   g_vppFrCode[] = {
#include "vpp_speech_asm_fr.tab"
};

PROTECTED  INT32   g_vppAmrCode[] = {
#include "vpp_speech_asm_amr.tab"
};

PROTECTED  INT32   g_vppEfrCode[] = {
#include "vpp_speech_asm_efr.tab"
};

PROTECTED  INT32   g_vppAmrEncCode475[] = {
#include "vpp_speech_asm_amr475_enc.tab"
};

PROTECTED  INT32   g_vppAmrEncCode515[] = {
#include "vpp_speech_asm_amr515_enc.tab"
};

PROTECTED  INT32   g_vppAmrEncCode59[] = {
#include "vpp_speech_asm_amr59_enc.tab"
};

PROTECTED  INT32   g_vppAmrEncCode67[] = {
#include "vpp_speech_asm_amr67_enc.tab"
};

PROTECTED  INT32   g_vppAmrEncCode74[] = {
#include "vpp_speech_asm_amr74_enc.tab"
};

PROTECTED  INT32   g_vppAmrEncCode795[] = {
#include "vpp_speech_asm_amr795_enc.tab"
};

PROTECTED  INT32   g_vppAmrEncCode102[] = {
#include "vpp_speech_asm_amr102_enc.tab"
};

PROTECTED  INT32   g_vppAmrEncCode122[] = {
#include "vpp_speech_asm_amr122_enc.tab"
};

PROTECTED  INT32   g_vppAmrDecCode475[] = {
#include "vpp_speech_asm_amr475_dec.tab"
};

PROTECTED  INT32   g_vppAmrDecCode515[] = {
#include "vpp_speech_asm_amr515_dec.tab"
};

PROTECTED  INT32   g_vppAmrDecCode59[] = {
#include "vpp_speech_asm_amr59_dec.tab"
};

PROTECTED  INT32   g_vppAmrDecCode67[] = {
#include "vpp_speech_asm_amr67_dec.tab"
};

PROTECTED  INT32   g_vppAmrDecCode74[] = {
#include "vpp_speech_asm_amr74_dec.tab"
};

PROTECTED  INT32   g_vppAmrDecCode795[] = {
#include "vpp_speech_asm_amr795_dec.tab"
};

PROTECTED  INT32   g_vppAmrDecCode102[] = {
#include "vpp_speech_asm_amr102_dec.tab"
};

PROTECTED  INT32   g_vppAmrDecCode122[] = {
#include "vpp_speech_asm_amr122_dec.tab"
};

PROTECTED  INT32   g_vppEfrAmrConstX[] = {
#include "vpp_speech_const_amr_efr_x.tab"
};

PROTECTED INT32   g_vppEfrAmrConstY[] = {
#include "vpp_speech_const_amr_efr_y.tab"
};

PROTECTED  INT32   g_vppHrConstX[] = {
#include "vpp_speech_const_hr_x.tab"
};

PROTECTED  INT32   g_vppHrConstY[] = {
#include "vpp_speech_const_hr_y.tab"
};

#ifdef MAGIC_AE_SUPPORT
PROTECTED  INT32   g_vppMorphConstantX[] = {
#include "vpp_morph_const.tab"
};

PROTECTED  INT32   g_vppMorphCode[] = {
#include "vpp_speech_asm_morph.tab"
};
#endif


PROTECTED  INT32   g_vppAecConstantX[] = {
#include "vpp_aec_const.tab"
};

PROTECTED  INT32   g_vppAecCode[] = {
#include "vpp_speech_asm_aec.tab"
};


#if  !defined(SPEECH_NS_MIC_VOC_CLOSE)  ||  !defined(SPEECH_NS_RECEIVER_VOC_CLOSE)

PROTECTED  INT32   g_vppNoiseSuppressCode[] = 
{
#include "vpp_speech_asm_noisesuppress.tab"
};

PROTECTED  INT32   g_vppNoiseSuppressConstantX[]  = {
#include "vpp_noisesuppress_const_x.tab"
};
#endif


PROTECTED INT32 g_vppAgcCode[] =
{
#include "vpp_speech_asm_agc.tab"
};

PROTECTED INT32 g_vppAgcConstantX[] =
{
#include "vpp_agc_const_x.tab"
};


PROTECTED CONST INT32 g_vppSpeechMainCodeSize = sizeof(g_vppSpeechMainCode);


#else



#ifdef CHIP_HAS_AP
PROTECTED CONST INT32   g_vppSpeechMainCode[] MICRON_ALIGN(32)= {
#include "vpp_speech_asm_main_modem.tab"
};

#else // !CHIP_HAS_AP

PROTECTED CONST INT32   g_vppSpeechMainCode[] MICRON_ALIGN(32)= {
#include "vpp_speech_asm_main.tab"
};
#endif // !CHIP_HAS_AP


PROTECTED CONST INT32   g_vppHrCode[] MICRON_ALIGN(32)= {
#include "vpp_speech_asm_hr.tab"
};

PROTECTED CONST INT32   g_vppEfrAmrCode[] MICRON_ALIGN(32) = {
#include "vpp_speech_asm_amr_efr.tab"
};

PROTECTED CONST INT32   g_vppFrCode[] MICRON_ALIGN(32)= {
#include "vpp_speech_asm_fr.tab"
};

PROTECTED CONST INT32   g_vppAmrCode[] MICRON_ALIGN(32)= {
#include "vpp_speech_asm_amr.tab"
};

PROTECTED CONST INT32   g_vppEfrCode[] MICRON_ALIGN(32)= {
#include "vpp_speech_asm_efr.tab"
};

PROTECTED CONST INT32   g_vppAmrEncCode475[] MICRON_ALIGN(32)= {
#include "vpp_speech_asm_amr475_enc.tab"
};

PROTECTED CONST INT32   g_vppAmrEncCode515[] MICRON_ALIGN(32)= {
#include "vpp_speech_asm_amr515_enc.tab"
};

PROTECTED CONST INT32   g_vppAmrEncCode59[] MICRON_ALIGN(32)= {
#include "vpp_speech_asm_amr59_enc.tab"
};

PROTECTED CONST INT32   g_vppAmrEncCode67[] MICRON_ALIGN(32)= {
#include "vpp_speech_asm_amr67_enc.tab"
};

PROTECTED CONST INT32   g_vppAmrEncCode74[] MICRON_ALIGN(32)= {
#include "vpp_speech_asm_amr74_enc.tab"
};

PROTECTED CONST INT32   g_vppAmrEncCode795[] MICRON_ALIGN(32)= {
#include "vpp_speech_asm_amr795_enc.tab"
};

PROTECTED CONST INT32   g_vppAmrEncCode102[] MICRON_ALIGN(32)= {
#include "vpp_speech_asm_amr102_enc.tab"
};

PROTECTED CONST INT32   g_vppAmrEncCode122[] MICRON_ALIGN(32)= {
#include "vpp_speech_asm_amr122_enc.tab"
};

PROTECTED CONST INT32   g_vppAmrDecCode475[] MICRON_ALIGN(32)= {
#include "vpp_speech_asm_amr475_dec.tab"
};

PROTECTED CONST INT32   g_vppAmrDecCode515[] MICRON_ALIGN(32)= {
#include "vpp_speech_asm_amr515_dec.tab"
};

PROTECTED CONST INT32   g_vppAmrDecCode59[] MICRON_ALIGN(32)= {
#include "vpp_speech_asm_amr59_dec.tab"
};

PROTECTED CONST INT32   g_vppAmrDecCode67[] MICRON_ALIGN(32)= {
#include "vpp_speech_asm_amr67_dec.tab"
};

PROTECTED CONST INT32   g_vppAmrDecCode74[] MICRON_ALIGN(32)= {
#include "vpp_speech_asm_amr74_dec.tab"
};

PROTECTED CONST INT32   g_vppAmrDecCode795[] MICRON_ALIGN(32)= {
#include "vpp_speech_asm_amr795_dec.tab"
};

PROTECTED CONST INT32   g_vppAmrDecCode102[] MICRON_ALIGN(32)= {
#include "vpp_speech_asm_amr102_dec.tab"
};

PROTECTED CONST INT32   g_vppAmrDecCode122[] MICRON_ALIGN(32)= {
#include "vpp_speech_asm_amr122_dec.tab"
};

PROTECTED CONST INT32   g_vppEfrAmrConstX[] MICRON_ALIGN(32)= {
#include "vpp_speech_const_amr_efr_x.tab"
};

PROTECTED CONST INT32   g_vppEfrAmrConstY[] MICRON_ALIGN(32)= {
#include "vpp_speech_const_amr_efr_y.tab"
};

PROTECTED CONST INT32   g_vppHrConstX[] MICRON_ALIGN(32)= {
#include "vpp_speech_const_hr_x.tab"
};

PROTECTED CONST INT32   g_vppHrConstY[] MICRON_ALIGN(32)= {
#include "vpp_speech_const_hr_y.tab"
};


#ifdef MAGIC_AE_SUPPORT
PROTECTED CONST INT32   g_vppMorphConstantX[] MICRON_ALIGN(32)= {
#include "vpp_morph_const.tab"
};
PROTECTED CONST INT32   g_vppMorphCode[] MICRON_ALIGN(32)= {
#include "vpp_speech_asm_morph.tab"
};
#endif


PROTECTED CONST INT32   g_vppAecConstantX[] MICRON_ALIGN(32)= {
#include "vpp_aec_const.tab"
};

#if (MOVE_AEC_AND_NS_CODE_TO_RAM)


PROTECTED INT32 SPEECH_SRAMDATA_INTERNAL g_vppAecCode[] = 
{
#include "vpp_speech_asm_aec.tab"
};

#if  !defined(SPEECH_NS_MIC_VOC_CLOSE)  ||  !defined(SPEECH_NS_RECEIVER_VOC_CLOSE)


PROTECTED  INT32  SPEECH_SRAMDATA_INTERNAL g_vppNoiseSuppressCode[] = 
{
#include "vpp_speech_asm_noisesuppress.tab"
};

PROTECTED  INT32 SPEECH_SRAMDATA_INTERNAL  g_vppNoiseSuppressConstantX[]  = {
#include "vpp_noisesuppress_const_x.tab"
};
#endif

PROTECTED INT32 g_vppAgcCode[] MICRON_ALIGN(32)=
{
#include "vpp_speech_asm_agc.tab"
};

PROTECTED INT32 g_vppAgcConstantX[] MICRON_ALIGN(32)=
{
#include "vpp_agc_const_x.tab"
};


#else

PROTECTED CONST INT32   g_vppAecCode[] MICRON_ALIGN(32)= 
{
#include "vpp_speech_asm_aec.tab"
};
  

#if  !defined(SPEECH_NS_MIC_VOC_CLOSE)  ||  !defined(SPEECH_NS_RECEIVER_VOC_CLOSE)

PROTECTED CONST INT32   g_vppNoiseSuppressCode[] MICRON_ALIGN(32)= 
{
#include "vpp_speech_asm_noisesuppress.tab"
};

PROTECTED CONST INT32   g_vppNoiseSuppressConstantX[] MICRON_ALIGN(32) = {
#include "vpp_noisesuppress_const_x.tab"
};
#endif

PROTECTED CONST INT32 g_vppAgcCode[] MICRON_ALIGN(32) =
{
#include "vpp_speech_asm_agc.tab"
};

PROTECTED CONST INT32 g_vppAgcConstantX[] MICRON_ALIGN(32) =
{
#include "vpp_agc_const_x.tab"
};

#endif



PROTECTED CONST INT32 g_vppSpeechMainCodeSize MICRON_ALIGN(32)= sizeof(g_vppSpeechMainCode);

#endif

#ifndef SMALL_BSS_RAM_SIZE


    #if  !defined(SPEECH_NS_MIC_VOC_CLOSE)
        PROTECTED  INT32   g_vppMicNoiseSuppressRAMX[2000] = 
        {
        	0
        };
    #endif


    #if  !defined(SPEECH_NS_RECEIVER_VOC_CLOSE)

        PROTECTED  INT32   g_vppReceiverNoiseSuppressRAMX[2000] = 
        {
        	0
        };
    #endif
    
    #ifdef MAGIC_AE_SUPPORT
        PROTECTED  INT32   g_vppMorhpRAMX[600] = 
        {
        	0
        };
    #endif
    
    PROTECTED  INT32    g_vppAecRAMX[1740] = 
    {
    	0
    };

	PROTECTED INT32 g_vppMicAgcRAMX[250] =
	{
		0
	};

	PROTECTED INT32 g_vppReceiverAgcRAMX[250] =
	{
		0
	};

	/* This buffer is used to backup aec and ns code, so this array length
	 * should be larger than GLOBAL_AEC_CODE_BACKUP_SIZE and
	 * GLOBAL_NOISESUPPRESS_CODE_BACKUP_SIZE
	 */
    PROTECTED   INT32   g_vppSpeechCode[2400] = 
    {
    	0
    };

	/* This buffer is used to backup aec and ns RAMX, so this array length
	 * should be larger than GLOBAL_AEC_RAMX_BACKUP_SIZE and
	 * GLOBAL_NOISESUPPRESS_TOTAL_RAMX_BACKUP_SIZE (which is
	 * GLOBAL_NOISESUPPRESS_RAMX_BACKUP_SIZE + SECTION_NOISESUPPRESS_CONSTX_SIZE)
	 */	
    PROTECTED   INT32   g_vppSpeechRAMX[3000] = 
    {
    	0
    };


#endif

