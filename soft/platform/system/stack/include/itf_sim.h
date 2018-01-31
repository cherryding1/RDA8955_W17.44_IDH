//------------------------------------------------------------------------------
//  $Log: itf_sim.h,v $
//  Revision 1.6  2006/06/28 12:28:33  aba
//  Access to EF_IMG file
//
//  Revision 1.5  2006/06/27 10:22:59  jrh
//  copyright update
//
//  Revision 1.4  2006/06/23 14:59:43  aba
//  Access to the EF_IMG file
//
//  Revision 1.3  2006/06/09 15:31:58  aba
//  *** empty log message ***
//
//  Revision 1.2  2005/12/29 17:38:44  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or
  transmission of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (C) 2003-2006

================================================================================
*/

/*
================================================================================
  File       : itf_sim.h
--------------------------------------------------------------------------------

  Scope      : Interface of the sim software unit

  History    :
--------------------------------------------------------------------------------
  Jun 09 06  |  ABA   | Add EF_ICCID and EF_ELP
--------------------------------------------------------------------------------
  Nov 02 05  |  ABA   | Change Fetch length to 16 bits
--------------------------------------------------------------------------------
  Jun 06 05  |  ABA   | ISSUE 19: poll interval for simtoolkit
--------------------------------------------------------------------------------
  Sep 21 03  |  ABA   | Creation
================================================================================
*/
#ifndef __ITF_SIM_H__
#define __ITF_SIM_H__

#ifdef __SIM_C__
#define DefExtern
#else
#define DefExtern extern
#endif

#include "sxs_type.h"
#include "cmn_defs.h"
#include "itf_msgc.h"

#define SIM_RESET_REQ             			( HVY_SIM + 1  )
#define SIM_RUN_GSM_ALGO_REQ      		( HVY_SIM + 2  )
#define SIM_READ_BINARY_REQ       		( HVY_SIM + 3  )
#define SIM_UPDATE_BINARY_REQ     		( HVY_SIM + 4  )
#define SIM_READ_RECORD_REQ       		( HVY_SIM + 5  )
#define SIM_UPDATE_RECORD_REQ     		( HVY_SIM + 6  )
#define SIM_INVALIDATE_REQ        		( HVY_SIM + 7  )
#define SIM_REHABILITATE_REQ      		( HVY_SIM + 8  )
#define SIM_VERIFY_CHV_REQ        		( HVY_SIM + 9  )
#define SIM_CHANGE_CHV_REQ        		( HVY_SIM + 10 )
#define SIM_DISABLE_CHV_REQ       		( HVY_SIM + 11 )
#define SIM_ENABLE_CHV_REQ        		( HVY_SIM + 12 )
#define SIM_UNBLOCK_CHV_REQ       		( HVY_SIM + 13 )
#define SIM_SEEK_REQ              			( HVY_SIM + 14 )
#define SIM_INCREASE_REQ          			( HVY_SIM + 15 )
#define SIM_DEDIC_FILE_STATUS_REQ 	( HVY_SIM + 16 )
#define SIM_ELEM_FILE_STATUS_REQ  		( HVY_SIM + 17 )
#define SIM_TOOL_KIT_REQ          			( HVY_SIM + 18 )
#define SIM_TERMINAL_PROFILE_REQ  		( HVY_SIM + 19 )
#define SIM_ENVELOPE_REQ          			( HVY_SIM + 20 )
#define SIM_TERMINAL_RESPONSE_REQ 	( HVY_SIM + 21 )
#define SIM_STOP_REQ              			( HVY_SIM + 22 )
#define SIM_POLL_REQ              			( HVY_SIM + 23 )
#define SIM_POLL_OFF_REQ          			( HVY_SIM + 24 )

#define SIM_SELECT_APPLICATION_REQ      ( HVY_SIM + 25 )
#define SIM_RUN_UMTS_ALGO_REQ          	( HVY_SIM + 26 )
#define SIM_SET_USIM_PBK_REQ      		( HVY_SIM + 27 )

#define SIM_POWER_OFF_REQ      			( HVY_SIM + 60 )
#define SIM_RETRY_REQ		      			( HVY_SIM + 61 )
#define SIM_SEEK_ALL_REQ           			( HVY_SIM + 62 )

#define SIM_STATUS_ERROR_IND      		( HVY_SIM + 28 )
#define SIM_FETCH_IND             			( HVY_SIM + 29 )


#define SIM_RESET_CNF             			( HVY_SIM + 31 )                    
#define SIM_RUN_GSM_ALGO_CNF      		( HVY_SIM + 32 )                    
#define SIM_READ_BINARY_CNF       		( HVY_SIM + 33 )                    
#define SIM_UPDATE_BINARY_CNF     		( HVY_SIM + 34 )                    
#define SIM_READ_RECORD_CNF       		( HVY_SIM + 35 )                    
#define SIM_UPDATE_RECORD_CNF     		( HVY_SIM + 36 )                    
#define SIM_INVALIDATE_CNF        		( HVY_SIM + 37 )                    
#define SIM_REHABILITATE_CNF      		( HVY_SIM + 38 )                    
#define SIM_VERIFY_CHV_CNF        		( HVY_SIM + 39 )                    
#define SIM_CHANGE_CHV_CNF        		( HVY_SIM + 40 )                    
#define SIM_DISABLE_CHV_CNF       		( HVY_SIM + 41 )                    
#define SIM_ENABLE_CHV_CNF        		( HVY_SIM + 42 )                    
#define SIM_UNBLOCK_CHV_CNF       		( HVY_SIM + 43 )                  
#define SIM_SEEK_CNF              			( HVY_SIM + 44 )                  
#define SIM_INCREASE_CNF          			( HVY_SIM + 45 )                      
#define SIM_DEDIC_FILE_STATUS_CNF 	( HVY_SIM + 46 )                    
#define SIM_ELEM_FILE_STATUS_CNF  		( HVY_SIM + 47 )                    
#define SIM_TERMINAL_PROFILE_CNF  		( HVY_SIM + 48 )                    
#define SIM_ENVELOPE_CNF         			( HVY_SIM + 49 )                      
#define SIM_TERMINAL_RESPONSE_CNF 	( HVY_SIM + 50 )
#define SIM_STATUS_TIMER_EXP     		(( HVY_SIM + 51 )| SXS_TIMER_EVT_ID )                     
#define SIM_SLEEP_AFTER_EPPS       		( HVY_SIM + 52 )                      
#define SIM_SELECT_APPLICATION_CNF	( HVY_SIM + 53 )                        
#define SIM_RUN_UMTS_ALGO_CNF 		( HVY_SIM + 54 )            
#define SIM_SEEK_ALL_CNF           			( HVY_SIM + 55 )                  
#ifdef LTE_NBIOT_SUPPORT 

#define NBIOT_SIM_BASE                 (HVY_SIM + 0x70)

#define NBIOT_SIM_READ_BINARY_REQ      ( NBIOT_SIM_BASE + 1  )
#define NBIOT_SIM_UPDATE_BINARY_REQ    ( NBIOT_SIM_BASE + 2  )
#define NBIOT_SIM_READ_RECORD_REQ       ( NBIOT_SIM_BASE + 3  )
#define NBIOT_SIM_UPDATE_RECORD_REQ     ( NBIOT_SIM_BASE + 4  )

#define NBIOT_SIM_DEDIC_FILE_STATUS_REQ ( NBIOT_SIM_BASE + 5 )
#define NBIOT_SIM_ELEM_FILE_STATUS_REQ  ( NBIOT_SIM_BASE + 6 )
#define NBIOT_SIM_RUN_UMTS_ALGO_REQ     ( NBIOT_SIM_BASE + 7 )

#define NBIOT_SIM_READ_BINARY_CNF       ( NBIOT_SIM_BASE + 8 )                    
#define NBIOT_SIM_UPDATE_BINARY_CNF     ( NBIOT_SIM_BASE + 9 )                    
#define NBIOT_SIM_READ_RECORD_CNF       ( NBIOT_SIM_BASE + 10 )                    
#define NBIOT_SIM_UPDATE_RECORD_CNF     ( NBIOT_SIM_BASE + 11 )  


#define NBIOT_SIM_DEDIC_FILE_STATUS_CNF   ( NBIOT_SIM_BASE + 12 )                    
#define NBIOT_SIM_ELEM_FILE_STATUS_CNF    ( NBIOT_SIM_BASE + 13 )      
#define NBIOT_SIM_RUN_UMTS_ALGO_CNF       ( NBIOT_SIM_BASE + 14 )            


#endif


// DON'T CHANGE THE PARAMETER ORDER  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// The sim software process all request in the same way so the paramter
// must be in the same order for the confirm message

#define SW1_NO_SIM_ANSWER 0xFF
#define SW2_NO_SIM_ANSWER 0xFF

#define SIM_NOT_PRESENT  0
#define SIM_NOT_READY    1
#define SIM_READY        2

typedef struct
{
  u8 Status;
} sim_NotifInd_t;


typedef struct
{
  u8 DfAddress[2];  // DfAddress[0] = upper, DfAddress[1] = lower
  u8 EfAddress[2];
}EfAddress_t;

typedef struct
{
  u8 DfAddress[2];  // DfAddress[0] = upper, DfAddress[1] = lower
}DfAddress_t;

// Elementary file name. These names must be used in all sim request message.
// Warning, if this list change, update also itf_api.h
#define  EF_ADN     		0
#define  EF_FDN     		1
#define  EF_SMS     		2
#define  EF_CCP     		3
#define  EF_MSISDN  		4
#define  EF_SMSP    		5	 	// 74
#define  EF_SMSS    		6 		// 75
#define  EF_LND     		7
#define  EF_SMSR    		8
#define  EF_SDN     		9
#define  EF_EXT1    		10
#define  EF_EXT2    		11
#define  EF_EXT3    		12
#define  EF_BDN     		13
#define  EF_EXT4    		14
#define  EF_LP        		15
#define  EF_IMSI      		16
#define  EF_KC        		17
#define  EF_PLMNSEL   	18
#define  EF_HPLMN     		19
#define  EF_ACMMAX    	20
#define  EF_SST       		21
#define  EF_ACM       		22
#define  EF_GID1      		23
#define  EF_GID2      		24
#define  EF_PUCT      		25
#define  EF_CBMI      		26
#define  EF_SPN       		27
#define  EF_CBMID     		28
#define  EF_BCCH      		29
#define  EF_ACC       		30
#define  EF_FPLMN     		31
#define  EF_LOCI      		32
#define  EF_AD        		33
#define  EF_PHASE     		34
#define  EF_VGCS      		35
#define  EF_VGCSS     		36
#define  EF_VBS       		37
#define  EF_VBSS      		38
#define  EF_EMLPP     		39
#define  EF_AAEM      		40
#define  EF_ECC       		41
#define  EF_CBMIR     		42
#define  EF_NIA       		43
#define  EF_KCGPRS    	44
#define  EF_LOCIGPRS  	45
#define  EF_SUME      		46
#define  EF_PLMNWACT  	47
#define  EF_OPLMNWACT 	48
#define  EF_HPLMNACT  	49
#define  EF_CPBCCH    	50
#define  EF_INVSCAN   	51
#define  EF_RPLMNAC   	52
#define  EF_ICCID     		53
#define  EF_ELP       		54
#define  EF_IMG       		55
#define  EF_MBDN      		56

//USIM
#define    USIM_EF_LI				57
#define    USIM_EF_ARR         		58
#define    USIM_EF_IMSI        		59
#define    USIM_EF_KEYS        		60
#define    USIM_EF_KEYS_PS           61
#define    USIM_EF_DCK		       62   
#define    USIM_EF_HPLMN      		63
#define    USIM_EF_CNL		       64    
#define    USIM_EF_ACM_MAX    	65
#define    USIM_EF_UST        		66
#define    USIM_EF_ACM        		67
#define    USIM_EF_FDN        		68
#define    USIM_EF_SMS        		69
#define    USIM_EF_GID1       		70
#define    USIM_EF_GID2       		71
#define    USIM_EF_MSISDN     		72
#define    USIM_EF_PUCT       		73
#define    USIM_EF_SMSP       		74
#define    USIM_EF_SMSS       		75
#define    USIM_EF_CBMI       		76
#define    USIM_EF_SPN        		77
#define    USIM_EF_SMSR       		78
#define    USIM_EF_CBMID      		79
#define    USIM_EF_SDN        		80
#define    USIM_EF_EXT2       		81
#define    USIM_EF_EXT3       		82
#define    USIM_EF_BDN        		83
#define    USIM_EF_EXT5       		84
#define    USIM_EF_CBMIR      		85
#define    USIM_EF_EXT4       		86
#define    USIM_EF_EST        		87
#define    USIM_EF_ACL        		88
#define    USIM_EF_CMI        		89
#define    USIM_EF_START_HFN  	90
#define    USIM_EF_THRESHOLD  	91
#define    USIM_EF_PLMNWACT       92   
#define    USIM_EF_OPLMNWACT     93    
#define    USIM_EF_HPLMNWACT     94    
#define    USIM_EF_EHPLMN    	       95
#define    USIM_EF_PS_LOCI    		96
#define    USIM_EF_ACC        		97
#define    USIM_EF_FPLMN       		98
#define    USIM_EF_LOCI        		99
#define    USIM_EF_ICI         		100
#define    USIM_EF_OCI         		101
#define    USIM_EF_ICT         		102
#define    USIM_EF_OCT        		103
#define    USIM_EF_AD			104     	       
#define    USIM_EF_EMLPP	           	105
#define    USIM_EF_AAEM		       106    
#define    USIM_EF_ECC		       107   
#define    USIM_EF_HIDDENKEY   	108
#define    USIM_EF_NETPAR      	109
#define    USIM_EF_PNN         		110
#define    USIM_EF_OPL         		111
#define    USIM_EF_MBDN        		112
#define    USIM_EF_EXT6        		113
#define    USIM_EF_MBI         		114
#define    USIM_EF_MWIS       		115
#define    USIM_EF_CFIS       		116
#define    USIM_EF_EXT7       		117
#define    USIM_EF_SPDI		       118    	
#define    USIM_EF_MMSN		       119   
#define    USIM_EF_EXT8		       120    
#define    USIM_EF_MMSICP	       121    
#define    USIM_EF_MMSUP	       122    
#define    USIM_EF_MMSUCP	       123    
#define    USIM_EF_NIA		       124    
#define    USIM_EF_VGCS		       125     
#define    USIM_EF_VGCSS	           	126
#define    USIM_EF_VBS		       127   
#define    USIM_EF_VBSS		       128    
#define    USIM_EF_VGCSCA	       129    
#define    USIM_EF_VBSCA	           	130
#define    USIM_EF_GBAP		       131   
#define    USIM_EF_MSK		       132    
#define    USIM_EF_MUK		       133    
#define    USIM_EF_GBANL	           	134
#define    USIM_EF_EHPLMNPI        	135 
#define    USIM_EF_LRPLMNSI         136 
#define    USIM_EF_NAFKCA	       137    
                              
//files under DF MEXE         
#define    USIM_EF_MEXE_ST    	138
#define    USIM_EF_ARPK       		139
#define    USIM_EF_TPRK       		140
#define    USIM_EF_ORPK                141
                                       
//files under DF GSM ACCESS            
#define    USIM_EF_KC         		142
#define    USIM_EF_KC_GPRS    	143
#define    USIM_EF_CPBCCH     		144
#define    USIM_EF_INVSCAN    	145
                              
           
#define    USIM_EF_DIR      	       146                                   
#define    USIM_EF_ARR_MF		147	     

// phonebook under DF_TELECOM, USIM globle phonebook
#define    USIM_EF_GB_PBR		148   
#define    USIM_EF_GB_IAP		149   
#define    USIM_EF_GB_IAP1		150   
#define    USIM_EF_GB_ADN		151   
#define    USIM_EF_GB_ADN1		152   
#define    USIM_EF_GB_EXT1		153   
#define    USIM_EF_GB_PBC		154   
#define    USIM_EF_GB_PBC1		155   
#define    USIM_EF_GB_GRP		156   
#define    USIM_EF_GB_GRP1		157   
#define    USIM_EF_GB_AAS		158   
#define    USIM_EF_GB_GAS		159   
#define    USIM_EF_GB_ANRA		160   
#define    USIM_EF_GB_ANRB		161   
#define    USIM_EF_GB_ANRC		162   
#define    USIM_EF_GB_ANRA1		163 
#define    USIM_EF_GB_ANRB1		164 
#define    USIM_EF_GB_ANRC1		165 
#define    USIM_EF_GB_SNE		166   
#define    USIM_EF_GB_SNE1		167   
#define    USIM_EF_GB_CCP1		168   
#define    USIM_EF_GB_UID		169   
#define    USIM_EF_GB_UID1		170   
#define    USIM_EF_GB_PSC		171   
#define    USIM_EF_GB_CC			172 
#define    USIM_EF_GB_PUID		173 
#define    USIM_EF_GB_EMAIL		174 
#define    USIM_EF_GB_EMAIL1	175	

// phonebook under ADF USIM, USIM local phonebook
#define    USIM_EF_PBR			176        
#define    USIM_EF_IAP			177        
#define    USIM_EF_IAP1			178      
#define    USIM_EF_ADN			179        
#define    USIM_EF_ADN1			180      
#define    USIM_EF_EXT1			181      
#define    USIM_EF_PBC			182        
#define    USIM_EF_PBC1			183      
#define    USIM_EF_GRP			184        
#define    USIM_EF_GRP1			185      
#define    USIM_EF_AAS			186        
#define    USIM_EF_GAS			187        
#define    USIM_EF_ANRA			188      
#define    USIM_EF_ANRB			189      
#define    USIM_EF_ANRC			190      
#define    USIM_EF_ANRA1		191      
#define    USIM_EF_ANRB1			192      
#define    USIM_EF_ANRC1			193      
#define    USIM_EF_SNE			194        
#define    USIM_EF_SNE1			195      
#define    USIM_EF_CCP1			196      
#define    USIM_EF_UID			197        
#define    USIM_EF_UID1			198      
#define    USIM_EF_PSC			199        
#define    USIM_EF_CC			200          
#define    USIM_EF_PUID			201        
#define    USIM_EF_EMAIL   		202      
#define    USIM_EF_EMAIL1		203        

// esim file
#define    EF_EID					204

// LTE usim file
#define    USIM_EF_SPNI			205
#define    USIM_EF_PNNI			206
#define    USIM_EF_NCPIP			207
#define    USIM_EF_EPSLOCI		208
#define    USIM_EF_EPSNSC		209
#define    USIM_EF_UFC			210
#define    USIM_EF_UICCIARI		211
#define    USIM_EF_NASCONFIG	212

#define    NB_ELEM_FILE			213



//This table indicates the path of each elementary file. Do not insert or delete
// an element of this table without updating the sim_EfName_t table
DefExtern EfAddress_t sim_ElementaryFileAddress[NB_ELEM_FILE]
#ifdef __SIM_C__
=
{
    {{0x7F,0x10},{0x6F,0x3A}},  // EF_ADN          
    {{0x7F,0x10},{0x6F,0x3B}},  // EF_FDN         
    {{0x7F,0x10},{0x6F,0x3C}},  // EF_SMS         
    {{0x7F,0x10},{0x6F,0x3D}},  // EF_CCP         
    {{0x7F,0x10},{0x6F,0x40}},  // EF_MSISDN      
    {{0x7F,0x10},{0x6F,0x42}},  // EF_SMSP        
    {{0x7F,0x10},{0x6F,0x43}},  // EF_SMSS        
    {{0x7F,0x10},{0x6F,0x44}},  // EF_LND         
    {{0x7F,0x10},{0x6F,0x47}},  // EF_SMSR        
    {{0x7F,0x10},{0x6F,0x49}},  // EF_SDN,        
    {{0x7F,0x10},{0x6F,0x4A}},  // EF_EXT1          		// 10
    {{0x7F,0x10},{0x6F,0x4B}},  // EF_EXT2        
    {{0x7F,0x10},{0x6F,0x4C}},  // EF_EXT3        
    {{0x7F,0x10},{0x6F,0x4D}},  // EF_BDN         
    {{0x7F,0x10},{0x6F,0x4E}},  // EF_EXT4        
    {{0x7F,0x20},{0x6F,0x05}},  // EF_LP          
    {{0x7F,0x20},{0x6F,0x07}},  // EF_IMSI        
    {{0x7F,0x20},{0x6f,0x20}},  // EF_KC          
    {{0x7F,0x20},{0x6F,0x30}},  // EF_PLMNSEL     
    {{0x7F,0x20},{0x6F,0x31}},  // EF_HPLMN       
    {{0x7F,0x20},{0x6F,0x37}},  // EF_ACMMAX      		// 20
    {{0x7F,0x20},{0x6F,0x38}},  // EF_SST         
    {{0x7F,0x20},{0x6F,0x39}},  // EF_ACM         
    {{0x7F,0x20},{0x6F,0x3E}},  // EF_GID1        
    {{0x7F,0x20},{0x6F,0x3F}},  // EF_GID2        
    {{0x7F,0x20},{0x6F,0x41}},  // EF_PUCT        
    {{0x7F,0x20},{0x6F,0x45}},  // EF_CBMI        
    {{0x7F,0x20},{0x6F,0x46}},  // EF_SPN         
    {{0x7F,0x20},{0x6F,0x48}},  // EF_CBMID       
    {{0x7F,0x20},{0x6F,0x74}},  // EF_BCCH        
    {{0x7F,0x20},{0x6F,0x78}},  // EF_ACC          		//30
    {{0x7F,0x20},{0x6F,0x7B}},  // EF_FPLMN       
    {{0x7F,0x20},{0x6F,0x7E}},  // EF_LOCI        
    {{0x7F,0x20},{0x6F,0xAD}},  // EF_AD          
    {{0x7F,0x20},{0x6F,0xAE}},  // EF_PHASE       
    {{0x7F,0x20},{0x6F,0xB1}},  // EF_VGCS        
    {{0x7F,0x20},{0x6F,0xB2}},  // EF_VGCSS       
    {{0x7F,0x20},{0x6F,0xB3}},  // EF_VBS         
    {{0x7F,0x20},{0x6F,0xB4}},  // EF_VBSS        
    {{0x7F,0x20},{0x6F,0xB5}},  // EF_EMLPP       
    {{0x7F,0x20},{0x6F,0xB6}},  // EF_AAEM         		// 40
    {{0x7F,0x20},{0x6F,0xB7}},  // EF_ECC         
    {{0x7F,0x20},{0x6F,0x50}},  // EF_CBMIR       
    {{0x7F,0x20},{0x6F,0x51}},  // EF_NIA         
    {{0x7F,0x20},{0x6F,0x52}},  // EF_KCGPRS      
    {{0x7F,0x20},{0x6F,0x53}},  // EF_LOCIGPRS    
    {{0x7F,0x20},{0x6F,0x54}},  // EF_SUME        
    {{0x7F,0x20},{0x6F,0x60}},  // EF_PLMNWACT    
    {{0x7F,0x20},{0x6F,0x61}},  // EF_OPLMNWACT   
    {{0x7F,0x20},{0x6F,0x62}},  // EF_HPLMNACT    
    {{0x7F,0x20},{0x6F,0x63}},  // EF_CPBCCH       		// 50
    {{0x7F,0x20},{0x6F,0x64}},  // EF_INVSCAN     
    {{0x7F,0x20},{0x6F,0x65}},  // EF_RPLMNACT    
    {{0x3F,0x00},{0x2F,0xE2}},  // EF_ICCID       
    {{0x3F,0x00},{0x2F,0x05}},  // EF_ELP         
    {{0x5F,0x50},{0x4F,0x20}},  // EF_IMG         
    {{0x7F,0x20},{0x6F,0xC7}},  // EF_MBDN        

//USIM
    {{0x7F,0xFF},{0x6F,0x05}},  // USIM_EF_LI						                                    
    {{0x7F,0xFF},{0x6F,0x06}},  // USIM_EF_ARR        	                                    
    {{0x7F,0xFF},{0x6F,0x07}},  // USIM_EF_IMSI                                             
    {{0x7F,0xFF},{0x6F,0x08}},  // USIM_EF_KEYS      	// 60                         
    {{0x7F,0xFF},{0x6F,0x09}},  // USIM_EF_KEYS_PS                                          
    {{0x7F,0xFF},{0x6F,0x2C}},  // USIM_EF_DCK		  		                                    
    {{0x7F,0xFF},{0x6F,0x31}},  // USIM_EF_HPLMN       	                                    
    {{0x7F,0xFF},{0x6F,0x32}},  // USIM_EF_CNL		  		                                    
    {{0x7F,0xFF},{0x6F,0x37}},  // USIM_EF_ACM_MAX  		                                    
    {{0x7F,0xFF},{0x6F,0x38}},  // USIM_EF_UST                                              
    {{0x7F,0xFF},{0x6F,0x39}},  // USIM_EF_ACM                                              
    {{0x7F,0xFF},{0x6F,0x3B}},  // USIM_EF_FDN                                              
    {{0x7F,0xFF},{0x6F,0x3C}},  // USIM_EF_SMS                                              
    {{0x7F,0xFF},{0x6F,0x3E}},  // USIM_EF_GID1       	// 70                       
    {{0x7F,0xFF},{0x6F,0x3F}},  // USIM_EF_GID2                                             
    {{0x7F,0xFF},{0x6F,0x40}},  // USIM_EF_MSISDN     	                                    
    {{0x7F,0xFF},{0x6F,0x41}},  // USIM_EF_PUCT                                             
    {{0x7F,0xFF},{0x6F,0x42}},  // USIM_EF_SMSP                                             
    {{0x7F,0xFF},{0x6F,0x43}},  // USIM_EF_SMSS                                             
    {{0x7F,0xFF},{0x6F,0x45}},  // USIM_EF_CBMI                                             
    {{0x7F,0xFF},{0x6F,0x46}},  // USIM_EF_SPN                                              
    {{0x7F,0xFF},{0x6F,0x47}},  // USIM_EF_SMSR                                             
    {{0x7F,0xFF},{0x6F,0x48}},  // USIM_EF_CBMID                                            
    {{0x7F,0xFF},{0x6F,0x49}},  // USIM_EF_SDN           	// 80                       
    {{0x7F,0xFF},{0x6F,0x4B}},  // USIM_EF_EXT2                                             
    {{0x7F,0xFF},{0x6F,0x4C}},  // USIM_EF_EXT3                                             
    {{0x7F,0xFF},{0x6F,0x4D}},  // USIM_EF_BDN                                              
    {{0x7F,0xFF},{0x6F,0x4E}},  // USIM_EF_EXT5                                             
    {{0x7F,0xFF},{0x6F,0x50}},  // USIM_EF_CBMIR                                            
    {{0x7F,0xFF},{0x6F,0x55}},  // USIM_EF_EXT4                                             
    {{0x7F,0xFF},{0x6F,0x56}},  // USIM_EF_EST                                              
    {{0x7F,0xFF},{0x6F,0x57}},  // USIM_EF_ACL                                              
    {{0x7F,0xFF},{0x6F,0x58}},  // USIM_EF_CMI                                              
    {{0x7F,0xFF},{0x6F,0x5B}},  // USIM_EF_START_HFN     // 90                          
    {{0x7F,0xFF},{0x6F,0x5C}},  // USIM_EF_THRESHOLD                                        
    {{0x7F,0xFF},{0x6F,0x60}},  // USIM_EF_PLMNWACT                                         
    {{0x7F,0xFF},{0x6F,0x61}},  // USIM_EF_OPLMNWACT                                        
    {{0x7F,0xFF},{0x6F,0x62}},  // USIM_EF_HPLMNWACT                                        
    {{0x7F,0xFF},{0x6F,0xD9}},  // USIM_EF_EHPLMN    	                                      
    {{0x7F,0xFF},{0x6F,0x73}},  // USIM_EF_PS_LOCI                                          
    {{0x7F,0xFF},{0x6F,0x78}},  // USIM_EF_ACC                                              
    {{0x7F,0xFF},{0x6F,0x7B}},  // USIM_EF_FPLMN                                            
    {{0x7F,0xFF},{0x6F,0x7E}},  // USIM_EF_LOCI                                             
    {{0x7F,0xFF},{0x6F,0x80}},  // USIM_EF_ICI           	// 100                      
    {{0x7F,0xFF},{0x6F,0x81}},  // USIM_EF_OCI                                              
    {{0x7F,0xFF},{0x6F,0x82}},  // USIM_EF_ICT                                              
    {{0x7F,0xFF},{0x6F,0x83}},  // USIM_EF_OCT                                              
    {{0x7F,0xFF},{0x6F,0xAD}},  // USIM_EF_AD			  	                                      
    {{0x7F,0xFF},{0x6F,0xB5}},  // USIM_EF_EMLPP	                                          
    {{0x7F,0xFF},{0x6F,0xB6}},  // USIM_EF_AAEM		                                          
    {{0x7F,0xFF},{0x6F,0xB7}},  // USIM_EF_ECC		                                          
    {{0x7F,0xFF},{0x6F,0xC3}},  // USIM_EF_HIDDENKEY                                        
    {{0x7F,0xFF},{0x6F,0xC4}},  // USIM_EF_NETPAR                                           
    {{0x7F,0xFF},{0x6F,0xC5}},  // USIM_EF_PNN         	// 110                      
    {{0x7F,0xFF},{0x6F,0xC6}},  // USIM_EF_OPL                                              
    {{0x7F,0xFF},{0x6F,0xC7}},  // USIM_EF_MBDN                                             
    {{0x7F,0xFF},{0x6F,0xC8}},  // USIM_EF_EXT6                                             
    {{0x7F,0xFF},{0x6F,0xC9}},  // USIM_EF_MBI                                              
    {{0x7F,0xFF},{0x6F,0xCA}},  // USIM_EF_MWIS                                             
    {{0x7F,0xFF},{0x6F,0xCB}},  // USIM_EF_CFIS                                             
    {{0x7F,0xFF},{0x6F,0xCC}},  // USIM_EF_EXT7                                             
    {{0x7F,0xFF},{0x6F,0xCD}},  // USIM_EF_SPDI		                                          
    {{0x7F,0xFF},{0x6F,0xCE}},  // USIM_EF_MMSN		  	                                      
    {{0x7F,0xFF},{0x6F,0xCF}},  // USIM_EF_EXT8		 // 120                                 
    {{0x7F,0xFF},{0x6F,0xD0}},  // USIM_EF_MMSICP	      	                                  
    {{0x7F,0xFF},{0x6F,0xD1}},  // USIM_EF_MMSUP	                                          
    {{0x7F,0xFF},{0x6F,0xD2}},  // USIM_EF_MMSUCP	                                          
    {{0x7F,0xFF},{0x6F,0xD3}},  // USIM_EF_NIA		                                          
    {{0x7F,0xFF},{0x6F,0xB1}},  // USIM_EF_VGCS		  	                                      
    {{0x7F,0xFF},{0x6F,0xB2}},  // USIM_EF_VGCSS	                                          
    {{0x7F,0xFF},{0x6F,0xB3}},  // USIM_EF_VBS		                                          
    {{0x7F,0xFF},{0x6F,0xB4}},  // USIM_EF_VBSS		                                          
    {{0x7F,0xFF},{0x6F,0xD4}},  // USIM_EF_VGCSCA	                                          
    {{0x7F,0xFF},{0x6F,0xD5}},  // USIM_EF_VBSCA	      	// 130                        
    {{0x7F,0xFF},{0x6F,0xD6}},  // USIM_EF_GBAP		                                          
    {{0x7F,0xFF},{0x6F,0xD7}},  // USIM_EF_MSK		                                          
    {{0x7F,0xFF},{0x6F,0xD8}},  // USIM_EF_MUK		                                          
    {{0x7F,0xFF},{0x6F,0xDA}},  // USIM_EF_GBANL	                                          
    {{0x7F,0xFF},{0x6F,0xDB}},  // USIM_EF_EHPLMNPI                                         
    {{0x7F,0xFF},{0x6F,0xDC}},  // USIM_EF_LRPLMNSI                                         
    {{0x7F,0xFF},{0x6F,0xDD}},  // USIM_EF_NAFKCA	                                          
                                                                                            
//files under DF MEXE                                                                       
    {{0x5F,0x3C},{0x4F,0x40}},  // USIM_EF_MEXE_ST                                          
    {{0x5F,0x3C},{0x4F,0x42}},  // USIM_EF_ARPK                                             
    {{0x5F,0x3C},{0x4F,0x43}},  // USIM_EF_TPRK       	// 140                        
    {{0x5F,0x3C},{0x4F,0x41}},  // USIM_EF_ORPK                                             
                                                                                            
//files under DF GSM ACCESS                                                                 
    {{0x5F,0x3B},{0x4F,0x20}},  // USIM_EF_KC                                               
    {{0x5F,0x3B},{0x4F,0x52}},  // USIM_EF_KC_GPRS                                          
    {{0x5F,0x3B},{0x4F,0x63}},  // USIM_EF_CPBCCH                                           
    {{0x5F,0x3B},{0x4F,0x64}},  // USIM_EF_INVSCAN                                          
                                                                                            
                                                                                            
    {{0x3F,0x00},{0x2F,0x00}}, 	// USIM_EF_DIR      		      		                          
    {{0x3F,0x00},{0x2F,0x06}},   //USIM_EF_ARR_MF		                                        

// phonebook under DF_TELECOM, USIM globle phonebook                     
    {{0x5F,0x3A},{0x4F,0x30}},  //    USIM_EF_GB_PBR		148              
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_GB_IAP		149              
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_GB_IAP1		150              
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_GB_ADN		151              
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_GB_ADN1		152              
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_GB_EXT1		153              
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_GB_PBC		154              
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_GB_PBC1		155              
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_GB_GRP		156              
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_GB_GRP1		157              
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_GB_AAS		158              
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_GB_GAS		159              
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_GB_ANRA		160              
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_GB_ANRB		161              
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_GB_ANRC		162              
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_GB_ANRA1		163            
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_GB_ANRB1		164            
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_GB_ANRC1		165            
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_GB_SNE		166              
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_GB_SNE1		167              
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_GB_CCP1		168              
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_GB_UID		169              
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_GB_UID1		170              
    {{0x5F,0x3A},{0x4F,0x22}},  //    USIM_EF_GB_PSC		171              
    {{0x5F,0x3A},{0x4F,0x23}},  //    USIM_EF_GB_CC		172              
    {{0x5F,0x3A},{0x4F,0x24}},  //    USIM_EF_GB_PUID		173              
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_GB_EMAIL		174            
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_GB_EMAIL1		175	             
                                                                         
// phonebook under ADF USIM, USIM local phonebook                        
    {{0x5F,0x3A},{0x4F,0x30}},  //    USIM_EF_PBR			176                
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_IAP			177                
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_IAP1			178              
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_ADN			179                
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_ADN1			180              
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_EXT1			181              
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_PBC			182                
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_PBC1			183              
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_GRP			184                
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_GRP1			185              
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_AAS			186                
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_GAS			187                
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_ANRA			188              
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_ANRB			189              
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_ANRC			190              
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_ANRA1			191                
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_ANRB1			192              
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_ANRC1			193              
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_SNE			194                
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_SNE1			195              
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_CCP1			196              
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_UID			197                
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_UID1			198              
    {{0x5F,0x3A},{0x4F,0x22}},  //    USIM_EF_PSC			199                
    {{0x5F,0x3A},{0x4F,0x23}},  //    USIM_EF_CC			200                
    {{0x5F,0x3A},{0x4F,0x24}},  //    USIM_EF_PUID			201              
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_EMAIL   		202            
    {{0x5F,0x3A},{0x4F,0x00}},  //    USIM_EF_EMAIL1		203              

    {{0x3F,0x00},{0x2F,0x02}},  //    EF_EID				204      
    {{0x7F,0xFF},{0x6F,0xDE}},  //    USIM_EF_SPNI			205
    {{0x7F,0xFF},{0x6F,0xDF}},  //    USIM_EF_PNNI			206
    {{0x7F,0xFF},{0x6F,0xE2}},  //    USIM_EF_NCPIP			207
    {{0x7F,0xFF},{0x6F,0xE3}},  //    USIM_EF_EPSLOCI		208
    {{0x7F,0xFF},{0x6F,0xE4}},  //    USIM_EF_EPSNSC		209
    {{0x7F,0xFF},{0x6F,0xE6}},  //    USIM_EF_UFC			210
    {{0x7F,0xFF},{0x6F,0xE7}},  //    USIM_EF_UICCIARI		211
    {{0x7F,0xFF},{0x6F,0xE8}}   //    USIM_EF_NASCONFIG		212
     
};
#else
;
#endif


DefExtern u8 sim_FilePath[NB_ELEM_FILE][6]
#ifdef __SIM_C__
= 
{
   {0x7F,0x10,0x6F,0x3A},    // EF_ADN                  
   {0x7F,0x10,0x6F,0x3B},    // EF_FDN                  
   {0x7F,0x10,0x6F,0x3C},    // EF_SMS                  
   {0x7F,0x10,0x6F,0x3D},    // EF_CCP                  
   {0x7F,0x10,0x6F,0x40},    // EF_MSISDN               
   {0x7F,0x10,0x6F,0x42},    // EF_SMSP                 
   {0x7F,0x10,0x6F,0x43},    // EF_SMSS                 
   {0x7F,0x10,0x6F,0x44},    // EF_LND                  
   {0x7F,0x10,0x6F,0x47},    // EF_SMSR                 
   {0x7F,0x10,0x6F,0x49},    // EF_SDN,                 
   {0x7F,0x10,0x6F,0x4A},    // EF_EXT1       		// 10     
   {0x7F,0x10,0x6F,0x4B},    // EF_EXT2                 
   {0x7F,0x10,0x6F,0x4C},    // EF_EXT3                 
   {0x7F,0x10,0x6F,0x4D},    // EF_BDN                  
   {0x7F,0x10,0x6F,0x4E},    // EF_EXT4                 
   {0x7F,0x20,0x6F,0x05},    // EF_LP                   
   {0x7F,0x20,0x6F,0x07},    // EF_IMSI                 
   {0x7F,0x20,0x6f,0x20},    // EF_KC                   
   {0x7F,0x20,0x6F,0x30},    // EF_PLMNSEL              
   {0x7F,0x20,0x6F,0x31},    // EF_HPLMN                
   {0x7F,0x20,0x6F,0x37},    // EF_ACMMAX     		// 20     
   {0x7F,0x20,0x6F,0x38},    // EF_SST                  
   {0x7F,0x20,0x6F,0x39},    // EF_ACM                  
   {0x7F,0x20,0x6F,0x3E},    // EF_GID1                 
   {0x7F,0x20,0x6F,0x3F},    // EF_GID2                 
   {0x7F,0x20,0x6F,0x41},    // EF_PUCT                 
   {0x7F,0x20,0x6F,0x45},    // EF_CBMI                 
   {0x7F,0x20,0x6F,0x46},    // EF_SPN                  
   {0x7F,0x20,0x6F,0x48},    // EF_CBMID                
   {0x7F,0x20,0x6F,0x74},    // EF_BCCH                 
   {0x7F,0x20,0x6F,0x78},    // EF_ACC         		//30     
   {0x7F,0x20,0x6F,0x7B},    // EF_FPLMN                
   {0x7F,0x20,0x6F,0x7E},    // EF_LOCI                 
   {0x7F,0x20,0x6F,0xAD},    // EF_AD                   
   {0x7F,0x20,0x6F,0xAE},    // EF_PHASE                
   {0x7F,0x20,0x6F,0xB1},    // EF_VGCS                 
   {0x7F,0x20,0x6F,0xB2},    // EF_VGCSS                
   {0x7F,0x20,0x6F,0xB3},    // EF_VBS                  
   {0x7F,0x20,0x6F,0xB4},    // EF_VBSS                 
   {0x7F,0x20,0x6F,0xB5},    // EF_EMLPP                
   {0x7F,0x20,0x6F,0xB6},    // EF_AAEM        		// 40    
   {0x7F,0x20,0x6F,0xB7},    // EF_ECC                  
   {0x7F,0x20,0x6F,0x50},    // EF_CBMIR                
   {0x7F,0x20,0x6F,0x51},    // EF_NIA                  
   {0x7F,0x20,0x6F,0x52},    // EF_KCGPRS               
   {0x7F,0x20,0x6F,0x53},    // EF_LOCIGPRS             
   {0x7F,0x20,0x6F,0x54},    // EF_SUME                 
   {0x7F,0x20,0x6F,0x60},    // EF_PLMNWACT             
   {0x7F,0x20,0x6F,0x61},    // EF_OPLMNWACT            
   {0x7F,0x20,0x6F,0x62},    // EF_HPLMNACT             
   {0x7F,0x20,0x6F,0x63},    // EF_CPBCCH      		// 50    
   {0x7F,0x20,0x6F,0x64},    // EF_INVSCAN              
   {0x7F,0x20,0x6F,0x65},    // EF_RPLMNACT             
   {0x2F,0xE2},    			// EF_ICCID                
   {0x2F,0x05},    			// EF_ELP                  
   {0x7F,0x10,0x5F,0x50,0x4F,0x20},   // EF_IMG                
   {0x7F,0x20,0x6F,0xC7},    // EF_MBDN                 
                                                        
   {0x7F,0xFF,0x6F,0x05},    //    USIM_EF_LI	                             
   {0x7F,0xFF,0x6F,0x06},    //    USIM_EF_ARR                             
   {0x7F,0xFF,0x6F,0x07},    //    USIM_EF_IMSI                            
   {0x7F,0xFF,0x6F,0x08},    //    USIM_EF_KEYS    			// 60                
   {0x7F,0xFF,0x6F,0x09},    //    USIM_EF_KEYS_PS                         
   {0x7F,0xFF,0x6F,0x2C},    //    USIM_EF_DCK			                       
   {0x7F,0xFF,0x6F,0x31},    //    USIM_EF_HPLMN                           
   {0x7F,0xFF,0x6F,0x32},    //    USIM_EF_CNL			                       
   {0x7F,0xFF,0x6F,0x37},    //    USIM_EF_ACM_MAX                         
   {0x7F,0xFF,0x6F,0x38},    //    USIM_EF_UST                             
   {0x7F,0xFF,0x6F,0x39},    //    USIM_EF_ACM                             
   {0x7F,0xFF,0x6F,0x3B},    //    USIM_EF_FDN                             
   {0x7F,0xFF,0x6F,0x3C},    //    USIM_EF_SMS                             
   {0x7F,0xFF,0x6F,0x3E},    //    USIM_EF_GID1    			// 70                
   {0x7F,0xFF,0x6F,0x3F},    //    USIM_EF_GID2                            
   {0x7F,0xFF,0x6F,0x40},    //    USIM_EF_MSISDN	                         
   {0x7F,0xFF,0x6F,0x41},    //    USIM_EF_PUCT                            
   {0x7F,0xFF,0x6F,0x42},    //    USIM_EF_SMSP                            
   {0x7F,0xFF,0x6F,0x43},    //    USIM_EF_SMSS                            
   {0x7F,0xFF,0x6F,0x45},    //    USIM_EF_CBMI                            
   {0x7F,0xFF,0x6F,0x46},    //    USIM_EF_SPN                             
   {0x7F,0xFF,0x6F,0x47},    //    USIM_EF_SMSR                            
   {0x7F,0xFF,0x6F,0x48},    //    USIM_EF_CBMID                           
   {0x7F,0xFF,0x6F,0x49},    //    USIM_EF_SDN     			// 80                
   {0x7F,0xFF,0x6F,0x4B},    //    USIM_EF_EXT2                            
   {0x7F,0xFF,0x6F,0x4C},    //    USIM_EF_EXT3                            
   {0x7F,0xFF,0x6F,0x4D},    //    USIM_EF_BDN                             
   {0x7F,0xFF,0x6F,0x4E},    //    USIM_EF_EXT5                            
   {0x7F,0xFF,0x6F,0x50},    //    USIM_EF_CBMIR                           
   {0x7F,0xFF,0x6F,0x55},    //    USIM_EF_EXT4                            
   {0x7F,0xFF,0x6F,0x56},    //    USIM_EF_EST                             
   {0x7F,0xFF,0x6F,0x57},    //    USIM_EF_ACL                             
   {0x7F,0xFF,0x6F,0x58},    //    USIM_EF_CMI                             
   {0x7F,0xFF,0x6F,0x5B},    //    USIM_EF_START_HFN  	 	// 90             
   {0x7F,0xFF,0x6F,0x5C},    //    USIM_EF_THRESHOLD	                     
   {0x7F,0xFF,0x6F,0x60},    //    USIM_EF_PLMNWACT                        
   {0x7F,0xFF,0x6F,0x61},    //    USIM_EF_OPLMNWACT  		                 
   {0x7F,0xFF,0x6F,0x62},    //    USIM_EF_HPLMNWACT  		                 
   {0x7F,0xFF,0x6F,0xD9},    //    USIM_EF_EHPLMN                          
   {0x7F,0xFF,0x6F,0x73},    //    USIM_EF_PS_LOCI                         
   {0x7F,0xFF,0x6F,0x78},    //    USIM_EF_ACC                             
   {0x7F,0xFF,0x6F,0x7B},    //    USIM_EF_FPLMN                           
   {0x7F,0xFF,0x6F,0x7E},    //    USIM_EF_LOCI                            
   {0x7F,0xFF,0x6F,0x80},    //    USIM_EF_ICI     			// 100               
   {0x7F,0xFF,0x6F,0x81},    //    USIM_EF_OCI                             
   {0x7F,0xFF,0x6F,0x82},    //    USIM_EF_ICT                             
   {0x7F,0xFF,0x6F,0x83},    //    USIM_EF_OCT                             
   {0x7F,0xFF,0x6F,0xAD},    //    USIM_EF_AD			                         
   {0x7F,0xFF,0x6F,0xB5},    //    USIM_EF_EMLPP	                         
   {0x7F,0xFF,0x6F,0xB6},    //    USIM_EF_AAEM		                         
   {0x7F,0xFF,0x6F,0xB7},    //    USIM_EF_ECC			                       
   {0x7F,0xFF,0x6F,0xC3},    //    USIM_EF_HIDDENKEY	  	                 
   {0x7F,0xFF,0x6F,0xC4},    //    USIM_EF_NETPAR                          
   {0x7F,0xFF,0x6F,0xC5},    //    USIM_EF_PNN    			// 110               
   {0x7F,0xFF,0x6F,0xC6},    //    USIM_EF_OPL                             
   {0x7F,0xFF,0x6F,0xC7},    //    USIM_EF_MBDN                            
   {0x7F,0xFF,0x6F,0xC8},    //    USIM_EF_EXT6                            
   {0x7F,0xFF,0x6F,0xC9},    //    USIM_EF_MBI                             
   {0x7F,0xFF,0x6F,0xCA},    //    USIM_EF_MWIS                            
   {0x7F,0xFF,0x6F,0xCB},    //    USIM_EF_CFIS                            
   {0x7F,0xFF,0x6F,0xCC},    //    USIM_EF_EXT7                            
   {0x7F,0xFF,0x6F,0xCD},    //    USIM_EF_SPDI		                         
   {0x7F,0xFF,0x6F,0xCE},    //    USIM_EF_MMSN		                         
   {0x7F,0xFF,0x6F,0xCF},    //    USIM_EF_EXT8				// 120                 
   {0x7F,0xFF,0x6F,0xD0},    //    USIM_EF_MMSICP	                         
   {0x7F,0xFF,0x6F,0xD1},    //    USIM_EF_MMSUP		                       
   {0x7F,0xFF,0x6F,0xD2},    //    USIM_EF_MMSUCP	                         
   {0x7F,0xFF,0x6F,0xD3},    //    USIM_EF_NIA		                         
   {0x7F,0xFF,0x6F,0xB1},    //    USIM_EF_VGCS		                         
   {0x7F,0xFF,0x6F,0xB2},    //    USIM_EF_VGCSS	 	                       
   {0x7F,0xFF,0x6F,0xB3},    //    USIM_EF_VBS		                         
   {0x7F,0xFF,0x6F,0xB4},    //    USIM_EF_VBSS		                         
   {0x7F,0xFF,0x6F,0xD4},    //    USIM_EF_VGCSCA	                         
   {0x7F,0xFF,0x6F,0xD5},    //    USIM_EF_VBSCA	  		// 130             
   {0x7F,0xFF,0x6F,0xD6},    //    USIM_EF_GBAP		                         
   {0x7F,0xFF,0x6F,0xD7},    //    USIM_EF_MSK			                       
   {0x7F,0xFF,0x6F,0xD8},    //    USIM_EF_MUK		                         
   {0x7F,0xFF,0x6F,0xDA},    //    USIM_EF_GBANL	                         
   {0x7F,0xFF,0x6F,0xDB},    //    USIM_EF_EHPLMNPI                        
   {0x7F,0xFF,0x6F,0xDC},    //    USIM_EF_LRPLMNSI                        
   {0x7F,0xFF,0x6F,0xDD},    //    USIM_EF_NAFKCA	                         
                                                                           
//files under DF MEXE                                                      
   {0x7F,0xFF,0x5F,0x3C,0x4F,0x40},   //    USIM_EF_MEXE_ST                
   {0x7F,0xFF,0x5F,0x3C,0x4F,0x42},   //    USIM_EF_ARPK                   
   {0x7F,0xFF,0x5F,0x3C,0x4F,0x43},   //    USIM_EF_TPRK   	// 140     
   {0x7F,0xFF,0x5F,0x3C,0x4F,0x41},   //    USIM_EF_ORPK                   
                                                                           
//files under DF GSM ACCESS                                                
   {0x7F,0xFF,0x5F,0x3B,0x4F,0x20},   //    USIM_EF_KC                     
   {0x7F,0xFF,0x5F,0x3B,0x4F,0x52},   //    USIM_EF_KC_GPRS 	             
   {0x7F,0xFF,0x5F,0x3B,0x4F,0x63},   //    USIM_EF_CPBCCH                 
   {0x7F,0xFF,0x5F,0x3B,0x4F,0x64},   //    USIM_EF_INVSCAN                
                                                                           
// file undre MF
   {0x2F,0x00},   //    USIM_EF_DIR                                        
   {0x2F,0x06},    //   USIM_EF_ARR_MF		 
   
// phonebook under DF_TELECOM, USIM globle phonebook                           
    {0x7F,0x10,0x5F,0x3A,0x4F,0x30},  //    USIM_EF_GB_PBR		148              
    {0x7F,0x10,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_GB_IAP		149              
    {0x7F,0x10,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_GB_IAP1		150              
    {0x7F,0x10,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_GB_ADN		151              
    {0x7F,0x10,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_GB_ADN1		152              
    {0x7F,0x10,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_GB_EXT1		153              
    {0x7F,0x10,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_GB_PBC		154              
    {0x7F,0x10,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_GB_PBC1		155              
    {0x7F,0x10,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_GB_GRP		156              
    {0x7F,0x10,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_GB_GRP1		157              
    {0x7F,0x10,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_GB_AAS		158              
    {0x7F,0x10,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_GB_GAS		159              
    {0x7F,0x10,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_GB_ANRA		160              
    {0x7F,0x10,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_GB_ANRB		161              
    {0x7F,0x10,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_GB_ANRC		162              
    {0x7F,0x10,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_GB_ANRA1	163            
    {0x7F,0x10,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_GB_ANRB1	164            
    {0x7F,0x10,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_GB_ANRC1	165            
    {0x7F,0x10,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_GB_SNE		166              
    {0x7F,0x10,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_GB_SNE1		167              
    {0x7F,0x10,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_GB_CCP1		168              
    {0x7F,0x10,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_GB_UID		169              
    {0x7F,0x10,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_GB_UID1		170              
    {0x7F,0x10,0x5F,0x3A,0x4F,0x22},  //    USIM_EF_GB_PSC		171              
    {0x7F,0x10,0x5F,0x3A,0x4F,0x23},  //    USIM_EF_GB_CC		172                
    {0x7F,0x10,0x5F,0x3A,0x4F,0x24},  //    USIM_EF_GB_PUID		173              
    {0x7F,0x10,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_GB_EMAIL		174            
    {0x7F,0x10,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_GB_EMAIL1	175	           
	   
// phonebook under ADF USIM, USIM local phonebook                       
    {0x7F,0xFF,0x5F,0x3A,0x4F,0x30},  //    USIM_EF_PBR			176         
    {0x7F,0xFF,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_IAP			177         
    {0x7F,0xFF,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_IAP1			178       
    {0x7F,0xFF,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_ADN			179         
    {0x7F,0xFF,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_ADN1		180       
    {0x7F,0xFF,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_EXT1			181       
    {0x7F,0xFF,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_PBC			182         
    {0x7F,0xFF,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_PBC1			183       
    {0x7F,0xFF,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_GRP			184         
    {0x7F,0xFF,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_GRP1		185       
    {0x7F,0xFF,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_AAS			186         
    {0x7F,0xFF,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_GAS			187         
    {0x7F,0xFF,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_ANRA		188       
    {0x7F,0xFF,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_ANRB		189       
    {0x7F,0xFF,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_ANRC		190       
    {0x7F,0xFF,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_ANRA1		191       
    {0x7F,0xFF,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_ANRB1		192       
    {0x7F,0xFF,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_ANRC1		193       
    {0x7F,0xFF,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_SNE			194         
    {0x7F,0xFF,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_SNE1			195       
    {0x7F,0xFF,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_CCP1			196       
    {0x7F,0xFF,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_UID			197         
    {0x7F,0xFF,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_UID1			198       
    {0x7F,0xFF,0x5F,0x3A,0x4F,0x22},  //    USIM_EF_PSC			199         
    {0x7F,0xFF,0x5F,0x3A,0x4F,0x23},  //    USIM_EF_CC			200         
    {0x7F,0xFF,0x5F,0x3A,0x4F,0x24},  //    USIM_EF_PUID			201       
    {0x7F,0xFF,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_EMAIL   		202     
    {0x7F,0xFF,0x5F,0x3A,0x4F,0x00},  //    USIM_EF_EMAIL1		203       

// file undre MF for esim
    {0x2F,0x02},                   		//    EF_EID				204                
    
    {0x7F,0xFF,0x6F,0xDE},    		//   USIM_EF_SPNI			205
    {0x7F,0xFF,0x6F,0xDF},    		//   USIM_EF_PNNI			206
    {0x7F,0xFF,0x6F,0xE2},    		//   USIM_EF_NCPIP			207
    {0x7F,0xFF,0x6F,0xE3},    		//   USIM_EF_EPSLOCI		208
    {0x7F,0xFF,0x6F,0xE4},    		//   USIM_EF_EPSNSC		209
    {0x7F,0xFF,0x6F,0xE6},    		//   USIM_EF_UFC			210
    {0x7F,0xFF,0x6F,0xE7},    		//   USIM_EF_UICCIARI		211
    {0x7F,0xFF,0x6F,0xE8}    		//   USIM_EF_NASCONFIG		212
}
;
#else
;
#endif


DefExtern u8 sim_FilePathLength[NB_ELEM_FILE]
#ifdef __SIM_C__
= 
{
   4,    // EF_ADN                  
   4,    // EF_FDN                  
   4,    // EF_SMS                  
   4,    // EF_CCP                  
   4,    // EF_MSISDN               
   4,    // EF_SMSP                 
   4,    // EF_SMSS                 
   4,    // EF_LND                  
   4,    // EF_SMSR                 
   4,    // EF_SDN,                 
   4,    // EF_EXT1       		// 10     
   4,    // EF_EXT2                 
   4,    // EF_EXT3                 
   4,    // EF_BDN                  
   4,    // EF_EXT4                 
   4,    // EF_LP                   
   4,    // EF_IMSI                 
   4,    // EF_KC                   
   4,    // EF_PLMNSEL              
   4,    // EF_HPLMN                
   4,    // EF_ACMMAX     		// 20     
   4,    // EF_SST                  
   4,    // EF_ACM                  
   4,    // EF_GID1                 
   4,    // EF_GID2                 
   4,    // EF_PUCT                 
   4,    // EF_CBMI                 
   4,    // EF_SPN                  
   4,    // EF_CBMID                
   4,    // EF_BCCH                 
   4,    // EF_ACC         		//30     
   4,    // EF_FPLMN                
   4,    // EF_LOCI                 
   4,    // EF_AD                   
   4,    // EF_PHASE                
   4,    // EF_VGCS                 
   4,    // EF_VGCSS                
   4,    // EF_VBS                  
   4,    // EF_VBSS                 
   4,    // EF_EMLPP                
   4,    // EF_AAEM        		// 40    
   4,    // EF_ECC                  
   4,    // EF_CBMIR                
   4,    // EF_NIA                  
   4,    // EF_KCGPRS               
   4,    // EF_LOCIGPRS             
   4,    // EF_SUME                 
   4,    // EF_PLMNWACT             
   4,    // EF_OPLMNWACT            
   4,    // EF_HPLMNACT             
   4,    // EF_CPBCCH      		// 50    
   4,    // EF_INVSCAN              
   4,    // EF_RPLMNACT             
   2,    // EF_ICCID                
   2,    // EF_ELP                  
   6,    // EF_IMG                  
   4,    // EF_MBDN                 
                                                        
   4,    //    USIM_EF_LI	                 
   4,    //    USIM_EF_ARR                 
   4,    //    USIM_EF_IMSI                
   4,    //    USIM_EF_KEYS    		// 60    
   4,    //    USIM_EF_KEYS_PS             
   4,    //    USIM_EF_DCK			           
   4,    //    USIM_EF_HPLMN               
   4,    //    USIM_EF_CNL			           
   4,    //    USIM_EF_ACM_MAX             
   4,    //    USIM_EF_UST                 
   4,    //    USIM_EF_ACM                 
   4,    //    USIM_EF_FDN                 
   4,    //    USIM_EF_SMS                 
   4,    //    USIM_EF_GID1    		// 70    
   4,    //    USIM_EF_GID2                
   4,    //    USIM_EF_MSISDN	             
   4,    //    USIM_EF_PUCT                
   4,    //    USIM_EF_SMSP                
   4,    //    USIM_EF_SMSS                
   4,    //    USIM_EF_CBMI                
   4,    //    USIM_EF_SPN                 
   4,    //    USIM_EF_SMSR                
   4,    //    USIM_EF_CBMID               
   4,    //    USIM_EF_SDN     		// 80    
   4,    //    USIM_EF_EXT2                
   4,    //    USIM_EF_EXT3                
   4,    //    USIM_EF_BDN                 
   4,    //    USIM_EF_EXT5                
   4,    //    USIM_EF_CBMIR               
   4,    //    USIM_EF_EXT4                
   4,    //    USIM_EF_EST                 
   4,    //    USIM_EF_ACL                 
   4,    //    USIM_EF_CMI                 
   4,    //    USIM_EF_START_HFN  	 // 90 
   4,    //    USIM_EF_THRESHOLD	         
   4,    //    USIM_EF_PLMNWACT            
   4,    //    USIM_EF_OPLMNWACT  		     
   4,    //    USIM_EF_HPLMNWACT  		     
   4,    //    USIM_EF_EHPLMN              
   4,    //    USIM_EF_PS_LOCI             
   4,    //    USIM_EF_ACC                 
   4,    //    USIM_EF_FPLMN               
   4,    //    USIM_EF_LOCI                
   4,    //    USIM_EF_ICI     		// 100   
   4,    //    USIM_EF_OCI                 
   4,    //    USIM_EF_ICT                 
   4,    //    USIM_EF_OCT                 
   4,    //    USIM_EF_AD			             
   4,    //    USIM_EF_EMLPP	             
   4,    //    USIM_EF_AAEM		             
   4,    //    USIM_EF_ECC			           
   4,    //    USIM_EF_HIDDENKEY	  	     
   4,    //    USIM_EF_NETPAR              
   4,    //    USIM_EF_PNN    		// 110   
   4,    //    USIM_EF_OPL                 
   4,    //    USIM_EF_MBDN                
   4,    //    USIM_EF_EXT6                
   4,    //    USIM_EF_MBI                 
   4,    //    USIM_EF_MWIS                
   4,    //    USIM_EF_CFIS                
   4,    //    USIM_EF_EXT7                
   4,    //    USIM_EF_SPDI		             
   4,    //    USIM_EF_MMSN		             
   4,    //    USIM_EF_EXT8		// 120     
   4,    //    USIM_EF_MMSICP	             
   4,    //    USIM_EF_MMSUP		           
   4,    //    USIM_EF_MMSUCP	             
   4,    //    USIM_EF_NIA		             
   4,    //    USIM_EF_VGCS		             
   4,    //    USIM_EF_VGCSS	 	           
   4,    //    USIM_EF_VBS		             
   4,    //    USIM_EF_VBSS		             
   4,    //    USIM_EF_VGCSCA	             
   4,    //    USIM_EF_VBSCA		// 130     
   4,    //    USIM_EF_GBAP		             
   4,    //    USIM_EF_MSK			           
   4,    //    USIM_EF_MUK		             
   4,    //    USIM_EF_GBANL	             
   4,    //    USIM_EF_EHPLMNPI            
   4,    //    USIM_EF_LRPLMNSI            
   4,    //    USIM_EF_NAFKCA	             
                                    
//files under D_F MEXE                     
   6,   //    USIM_EF_MEXE_ST              
   6,   //    USIM_EF_ARPK                 
   6,   //    USIM_EF_TPRK     		// 140   
   6,   //    USIM_EF_ORPK                 
                                           
//files under USIM_DF GSM ACCESS           
   6,   //    USIM_EF_KC                   
   6,   //    USIM_EF_KC_GPRS 	           
   6,   //    USIM_EF_CPBCCH               
   6,   //    USIM_EF_INVSCAN              
                                           
// file under MF                                           
   2,   //    USIM_EF_DIR                  
   2,    //   USIM_EF_ARR_MF		           
                                           
// phonebook under DF_TELECOM, USIM globle phonebook            
   6,  //    USIM_EF_GB_PBR		148                               
   6,  //    USIM_EF_GB_IAP		149                               
   6,  //    USIM_EF_GB_IAP1		150                             
   6,  //    USIM_EF_GB_ADN		151                               
   6,  //    USIM_EF_GB_ADN1		152                             
   6,  //    USIM_EF_GB_EXT1		153                             
   6,  //    USIM_EF_GB_PBC		154                               
   6,  //    USIM_EF_GB_PBC1		155                             
   6,  //    USIM_EF_GB_GRP		156                               
   6,  //    USIM_EF_GB_GRP1		157                             
   6,  //    USIM_EF_GB_AAS		158                               
   6,  //    USIM_EF_GB_GAS		159                               
   6,  //    USIM_EF_GB_ANRA		160                             
   6,  //    USIM_EF_GB_ANRB		161                             
   6,  //    USIM_EF_GB_ANRC		162                             
   6,  //    USIM_EF_GB_ANRA1		163                             
   6,  //    USIM_EF_GB_ANRB1		164                             
   6,  //    USIM_EF_GB_ANRC1		165                             
   6,  //    USIM_EF_GB_SNE		166                               
   6,  //    USIM_EF_GB_SNE1		167                             
   6,  //    USIM_EF_GB_CCP1		168                             
   6,  //    USIM_EF_GB_UID		169                               
   6,  //    USIM_EF_GB_UID1		170                             
   6,  //    USIM_EF_GB_PSC		171                               
   6,  //    USIM_EF_GB_CC		172                               
   6,  //    USIM_EF_GB_PUID		173                             
   6,  //    USIM_EF_GB_EMAIL		174                             
   6,  //    USIM_EF_GB_EMAIL1	175                           
                                                                
// phonebook under ADF USIM, USIM local phonebook               
   6,  //    USIM_EF_PBR			176                               
   6,  //    USIM_EF_IAP			177                               
   6,  //    USIM_EF_IAP1			178                               
   6,  //    USIM_EF_ADN			179                               
   6,  //    USIM_EF_ADN1			180                               
   6,  //    USIM_EF_EXT1			181                               
   6,  //    USIM_EF_PBC			182                               
   6,  //    USIM_EF_PBC1			183                               
   6,  //    USIM_EF_GRP			184                               
   6,  //    USIM_EF_GRP1			185                               
   6,  //    USIM_EF_AAS			186                               
   6,  //    USIM_EF_GAS			187                               
   6,  //    USIM_EF_ANRA			188                               
   6,  //    USIM_EF_ANRB			189                               
   6,  //    USIM_EF_ANRC			190                               
   6,  //    USIM_EF_ANRA1		191                             
   6,  //    USIM_EF_ANRB1		192                             
   6,  //    USIM_EF_ANRC1		193                             
   6,  //    USIM_EF_SNE			194                               
   6,  //    USIM_EF_SNE1			195                               
   6,  //    USIM_EF_CCP1			196                               
   6,  //    USIM_EF_UID			197                               
   6,  //    USIM_EF_UID1			198                               
   6,  //    USIM_EF_PSC			199                               
   6,  //    USIM_EF_CC			200                                 
   6,  //    USIM_EF_PUID			201                               
   6,  //    USIM_EF_EMAIL   		202                             
   6,  //    USIM_EF_EMAIL1		203                               

   2,  //    EF_EID				204
   4,  //    USIM_EF_SPNI			205                         
   4,  //    USIM_EF_PNNI			206                         
   4,  //    USIM_EF_NCPIP		207                         
   4,  //    USIM_EF_EPSLOCI		208                         
   4,  //    USIM_EF_EPSNSC		209                         
   4,  //    USIM_EF_UFC			210                         
   4,  //    USIM_EF_UICCIARI		211                         
   4   //    USIM_EF_NASCONFIG	212                         
}
;
#else
;
#endif


#define  DF_GSM                   0
#define  DF_TELECOM           1
#define  DF_APPLICATION    2


// ************************* SIM_RESET
#define NO_RESET_RESPONSE             0
#define WRONG_ATR                     1
#define RESET_SUCCESS_FIRST_ATTEMPT   2
#define RESET_SUCCESS_SEVERAL_ATTEMPT 3
#define RESET_SUCCESS_ACCESS_FILE_FAIL  4

typedef struct
{
  u8 SrcMbx;             // Used to send the response
  bool UsimFlag;
}sim_ResetReq_t;

typedef struct
{
  u8 ResetResult;
  u8 Sw1;
  u8 Sw2;
  u8 Data[256]; // Status of the DF_TELECOM
  u8 UsimFlag; // 0: sim; 1: Usim
}sim_ResetCnf_t;

// ************************* SIM_RUN_ALGO
typedef struct
{
  u8 SrcMbx;             // Used to send the response
  u8 CKSN;
  u8 Rand[17];
}sim_RunGsmAlgoReq_t;

typedef struct
{
  u8 Sw1;
  u8 Sw2;
  u8 Data[14]; // case1: SRES[4] + Kc[8]; case2(USIM run in Gsm): SRES_LENGTH+ SRES[4] + Kc_LENGTH + Kc[8]
// u8 SRES[4];
//  u8 Kc[8];
}sim_RunGsmAlgoCnf_t;

// ************************* UMTS SIM_RUN_ALGO
//#ifdef __MULTI_RAT__
typedef struct {
u8 SrcMbx;  // Used to send the response
u8 CKSN;
u8 Data[34];  // RAND_LENGTH + RAND + AUTN_LENGTH + AUTN
} usim_RunUmtsAlgoReq_t; 
 
typedef struct
{
     u8 Sw1;
     u8 Sw2;
     u8 Data[61];   //  1+ (1+16) +(1+16) + (1+16) + (1+8)
                     	// CASE1: STATUS + RES_LENGTH +  RES + CK_LENGTH + CK + IK_LENGTH + IK + KC_LENGTH + KC
                     	// CASE2: STATUS + RES_LENGTH +  RES + CK_LENGTH + CK + IK_LENGTH + IK
                     	// CASE3: STATUS + AUTS_LENGTH + AUTS
                     	// CASE4: SRES + Kc 
                     	// CASE5: SRES_LENGTH + SRES + Kc_LENGTH + Kc
/*	 	
     u8 Status;   // 0xdb: success,and IK,CK,AUTH are avaiable; 0xdc:synchronisation failure, and AUTH is avaiable
//     bool gsmAccess; //FALSE: no access, and Kc is unavailable ; TRUE: gsm access, and Kc is available 
     u8 SRES[17];  // first byte is length
     u8 Kc[8];
     u8 IK[16];
     u8 CK[16];
     u8 AUTH[17];  // first byte is length
*/     
}usim_RunUmtsAlgoCnf_t;
//#endif

// ************************* SIM_READ_BINARY
typedef struct
{
  u8  SrcMbx;             // Used to send the response
  u8  File;
  u16 Offset;
  u8  NbByte;
}sim_ReadBinaryReq_t;

typedef struct
{
  u8 Sw1;
  u8 Sw2;
  u8 File;
  u8 Data[256];
}sim_ReadBinaryCnf_t;

// ************************* SIM_UPDATE_BINARY
typedef struct
{
  u8  SrcMbx;             // Used to send the response
  u8  File;
  u16 Offset;
  u8  NbByte;
  u8  Data[256];
}sim_UpdateBinaryReq_t;

typedef struct
{
  u8 Sw1;
  u8 Sw2;
  u8 File;
}sim_UpdateBinaryCnf_t;

// ************************* SIM_READ_RECORD

#define SIM_NEXT_RECORD                   2
#define SIM_PREVIOUS_RECORD               3
#define SIM_ABSOLUTE_MODE_OR_CURRENT_MODE 4

typedef struct
{
  u8  SrcMbx;             // Used to send the response
  u8  File;
  u8  RecordNb;
  u8  Mode;
  u8  NbByte;
}sim_ReadRecordReq_t;

typedef struct
{
  u8  Sw1;
  u8  Sw2;
  u8  File;
  u8  Data[256];
}sim_ReadRecordCnf_t;

// ************************* SIM_UPDATE_RECORD
typedef struct
{
  u8  SrcMbx;             // Used to send the response
  u8  File;
  u8  RecordNb;
  u8  Mode;
  u8  NbByte;
  u8  Data[256];
}sim_UpdateRecordReq_t;

typedef struct
{
  u8 Sw1;
  u8 Sw2;
  u8 File;
}sim_UpdateRecordCnf_t;

// ************************* SIM_INVALIDATE
typedef struct
{
  u8  SrcMbx;             // Used to send the response
  u8  File;
}sim_InvalidateReq_t;

typedef struct
{
  u8 Sw1;
  u8 Sw2;
  u8 File;
}sim_InvalidateCnf_t;

// ************************* SIM_REHABILITATE
typedef struct
{
  u8  SrcMbx;             // Used to send the response
  u8  File;
}sim_RehabilitateReq_t;

typedef struct
{
  u8 Sw1;
  u8 Sw2;
  u8 File;
}sim_RehabilitateCnf_t;

// ************************* SIM_SEEK
typedef struct
{
  u8  SrcMbx;             // Used to send the response
  u8  TypeMode;
  u8  File;
  u8  PatternNbByte;
  u8  Pattern[256];
}sim_SeekReq_t;

typedef struct
{
  u8  SrcMbx;             // Used to send the response
  u8  TypeMode;
  u8  File;
  u8  PatternNbByte;
  u8  Pattern[256];
}sim_SeekAllReq_t;

typedef struct
{
  u8 Sw1;
  u8 Sw2;
  u8 File;
  u8 RecordNb;  // only for type 2 seek (meaningless for type 1 seek)
}sim_SeekCnf_t;

typedef struct
{
  u8 Sw1;
  u8 Sw2;
  u8 File;
  u8 Record[255];  
  u8 length;
}sim_SeekAllCnf_t;

// ************************* SIM_INCREASE
typedef struct
{
  u8  SrcMbx;             // Used to send the response
  u8  ValueToBeAdded[3];
  u8  File;
}sim_IncreaseReq_t;

typedef struct
{
  u8 Sw1;
  u8 Sw2;
  u8 File;
  u8 RecordDataAndAddedValue[256+3];
}sim_IncreaseCnf_t;

// ************************* SIM_VERIFY_CHV
typedef struct
{
  u8 SrcMbx;             // Used to send the response
  u8 CHVNumber;
  u8 CHVValue[8];
}sim_VerifyCHVReq_t;

typedef struct
{
  u8 Sw1;
  u8 Sw2;
}sim_VerifyCHVCnf_t;

// ************************* SIM_CHANGE_CHV
typedef struct
{
  u8 SrcMbx;             // Used to send the response
  u8 CHVNumber;
  u8 OldCHVValue[8];
  u8 NewCHVValue[8];
}sim_ChangeCHVReq_t;

typedef struct
{
  u8 Sw1;
  u8 Sw2;
}sim_ChangeCHVCnf_t;

// ************************* SIM_DISABLE_CHV
typedef struct
{
  u8 SrcMbx;             // Used to send the response
  u8 CHV1Value[8];
}sim_DisableCHVReq_t;

typedef struct
{
  u8 Sw1;
  u8 Sw2;
}sim_DisableCHVCnf_t;

// ************************* SIM_ENABLE_CHV
typedef struct
{
  u8 SrcMbx;             // Used to send the response
  u8 CHV1Value[8];
}sim_EnableCHVReq_t;

typedef struct
{
  u8 Sw1;
  u8 Sw2;
}sim_EnableCHVCnf_t;

// ************************* SIM_UNBLOCK_CHV
typedef struct
{
  u8 SrcMbx;             // Used to send the response
  u8 CHVNumber;
  u8 UnblockCHVValue[8];
  u8 NewCHVValue[8];
}sim_UnblockCHVReq_t;

typedef struct
{
  u8 Sw1;
  u8 Sw2;
}sim_UnblockCHVCnf_t;

// ************************* SIM_ELEM_STATUS
typedef struct
{
  u8  SrcMbx;             // Used to send the response
  u8  File;
}sim_ElemFileStatusReq_t;

typedef struct
{
  u8 Sw1;
  u8 Sw2;
  u8 File;
  u8 Status[256];
}sim_ElemFileStatusCnf_t;

// ************************* SIM_DEDIC_STATUS
typedef struct
{
  u8  SrcMbx;             // Used to send the response
  u8  File;
}sim_DedicFileStatusReq_t;

typedef struct
{
  u8 Sw1;
  u8 Sw2;
  u8 File;
  u8 Status[256];
}sim_DedicFileStatusCnf_t;

// ************************* SIM_TOOLKIT
typedef struct
{
  u8   FetchMbx;             // Used to send fetch command
  bool Activate;             // TRUE => Activate    FALSE => Desactivate
}sim_ToolKitReq_t;

// ************************* SIM_TERMINAL_PROFILE_REQ
typedef struct
{
  u8 SrcMbx;             // Used to send the response
  u8 Length;
  u8 ProfileData[256];
}sim_TerminalProfileReq_t;



// ************************* SIM_SELECT_APPLICATION_REQ
typedef struct
{
  u8 SrcMbx;         // Used to send the response
  u8 AID[16]; 		// application ID
  u8 AIDLength; 	// application ID length
}sim_SelectApplicationReq_t;

// ************************* SIM_SELECT_APPLICATION_CNF
typedef struct
{
 u8 Sw1       ;
 u8 Sw2       ;
 u8 File      ; // See the defines above. File which has been read
 u8 Data[256] ;
}sim_SelectApplicationCnf_t;


// ************************* SIM_SELECT_APPLICATION_REQ
typedef struct
{
 u8    SrcMbx;         // Used to send the response
 u8    DataLength; 	// USIM Phonebook data length
 u8    RecordNum;	// USIM Phonebook record number;
 u8    flag;			// USIM Phonebook type; global phk: 0; application phb: 1;
 u8    Data[256]; 		// USIM Phonebook data
}sim_SetUsimPbkReq_t;


// ************************* SIM_TERMINAL_PROFILE_CNF
typedef struct
{
   u8 Sw1;
   u8 Sw2;
}sim_TerminalProfileCnf_t;

// ************************* SIM_ENVELOPE_REQ
typedef struct
{
  u8 SrcMbx;             // Used to send the response
  u8 Length;
  u8 EnvelopeData[256];
}sim_EnvelopeReq_t;

// ************************* SIM_ENVELOPE_CNF
typedef struct
{
  u8 Sw1;
  u8 Sw2;   
  u8 NbByte;
  u8 EnvelopeData[256];
}sim_EnvelopeCnf_t;

// ************************* SIM_TERMINAL_RESPONSE_REQ/
typedef struct
{
  u8 SrcMbx;             // Used to send the response
  u8 Length;
  u8 Data[256];
}sim_TerminalResponseReq_t;

// ************************* SIM_TERMINAL_RESPONSE_CNF
typedef struct
{
   u8 Sw1;
   u8 Sw2;
}sim_TerminalResponseCnf_t;

// ************************* SIM_FETCH
typedef struct
{
  u16 Len            ;
  u8  FetchData[260] ;
}sim_FetchInd_t;

// ************************* SIM_POLL_REQ
typedef struct
{
  u8 SrcMbx;             // Used to send the response
  u32 Duration;
}sim_PollReq_t;

// ************************* SIM_STATUS_ERROR
#define NO_SIM_ANSWER    0
#define BAD_SIM_RESPONSE 1
typedef struct
{
  u8 StatusError;
}sim_StatusErrorInd_t;

typedef struct
{
  u8 Sw1;
  u8 Sw2;
  u8 Data[260];
}sim_Cnf_t;

typedef struct
{
  u8 Sw1;
  u8 Sw2;
  u8 File;
  u8 Data[260];
}sim_CnfWithFile_t;


#ifdef _MSC_VER //for test tool to generate xml parsing code
//NBIOT_SIM_ELEM_FILE_STATUS_REQ
typedef struct sim_ElemFileStatusReq_tag
{
    MsgHead_t Header;
    sim_ElemFileStatusReq_t body;
}h_sim_ElemFileStatusReq_t;

//NBIOT_SIM_READ_BINARY_REQ
typedef struct sim_ReadBinaryReq_tag
{
    MsgHead_t Header;
    sim_ReadBinaryReq_t body;
}h_sim_ReadBinaryReq_t;

//NBIOT_SIM_UPDATE_BINARY_REQ
typedef struct sim_UpdateBinaryReq_tag
{
    MsgHead_t Header;
    sim_UpdateBinaryReq_t body;
}h_sim_UpdateBinaryReq_t;

//NBIOT_SIM_READ_RECORD_REQ
typedef struct sim_ReadRecordReq_t_tag
{
    MsgHead_t Header;
    sim_ReadRecordReq_t body;
}h_sim_ReadRecordReq_t;

//NBIOT_SIM_UPDATE_RECORD_REQ
typedef struct sim_UpdateRecordReq_tag
{
    MsgHead_t Header;
    sim_UpdateRecordReq_t body;
}h_sim_UpdateRecordReq_t;

//NBIOT_SIM_RUN_UMTS_ALGO_REQ
typedef struct usim_RunUmtsAlgoReq_tag
{
    MsgHead_t Header;
    usim_RunUmtsAlgoReq_t body;
}h_usim_RunUmtsAlgoReq_t;


#endif

#undef DefExtern

#endif  // __ITF_SIM_H__
