/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/



#ifndef SPPP_PRIVATE_H
#define SPPP_PRIVATE_H


//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "cs_types.h"
#include "spp_gsm.h"
#include "spal_xcor.h"
#include "spal_vitac.h"
#include "spal_cap.h"


//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------



// Burst definitions (tail bits disregarded)
#define NB_HPAYLD_SIZE 57
#define NB_TS_START (NB_HPAYLD_SIZE+1) // Steal bit
#define NB_TS_SIZE 26
#define SCH_HPAYLD_SIZE 39
#define SCH_TS_START (SCH_HPAYLD_SIZE)
#define SCH_TS_SIZE 64

// Number/size of Tsc Cross-correlation for time syncronization
#if (CHIP_VAMOSI_SUPPORTED)
#define SCH_TSC_IDX 32
#else
#define SCH_TSC_IDX 8
#endif

// defines for EGPRS
#define EGPRS_LSE_TAP_NB 8
#define EGPRS_LP_QP 10


// SPP MACROS
//-----------

/// Macros for cache/uncache address
//#define SP_CACHED(addr)     ((UINT32*)((UINT32)(addr)&0xdfffffff))
//#define SP_UNCACHED(addr)   ((UINT32*)((UINT32)(addr)|0x20000000))

// "Safe" shifts for variable shift amount
#define SPP_SAFE_LSHIFT(val,shift) ((shift==32)?0:(val << shift))
#define SPP_SAFE_RSHIFT(val,shift) ((shift==32)?0:(val >> shift))

#define SCH_FWEQU_START_ST 0xB
#define SCH_BWEQU_START_ST 0xD

// Debug
#define SPP_AMR_DEBUG(n) //CT_set_w0(n)

// Softbit to Hardbit
#define hard8(n)    (n & 0x80)

// Math
#define SPP_ABS(x)                              (((x)>0)?(x):-(x))
#define SPP_SAT_INT16(x)                        (((x) < MIN_INT16) ? MIN_INT16 : ((x) > MAX_INT16) ? MAX_INT16 : (INT16)(x))
#define SPP_MAC_64(x, y, hi, lo)                asm ("madd %2,%3"   : "+l" (lo), "+h" (hi) : "%r" (x), "r" (y))
#define SPP_MSU_64(x, y, hi, lo)                asm ("msub %2,%3"   : "+l" (lo), "+h" (hi) : "%r" (x), "r" (y))
#define SPP_INT64_TO_INT32(fractBits, hi, lo)  ((INT32)(((hi) << (32 - fractBits)) |  ((lo) >> fractBits)))

#ifndef CHIP_DIE_8955
//8955 definition moved to spp_gsm.h
typedef struct
{
    UINT32 *samples; // ptr to received symbols
    UINT32 *ref;     // ptr to reconstructed symbols
    UINT32 taps[SPAL_ISI]; // channel taps
    UINT8  *sv;   // pointer to equalized softvalues
    UINT32 power; // normalized power (/symbol)
    UINT32 noise; // normalized noise power (/symbol)
    SPP_COMPLEX_T dco; ///< DC Offset level
    INT16 fof;
    UINT8  tof;
    //UINT8  type;
    UINT8  tsc;
    UINT8  samples_offset;
} SPP_EQU_BURST_T;
#endif

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------
//-------------------------------------
// AFS Speech channel coding parameters
//-------------------------------------
// Input: 244, 204, 159, 148, 134, 118, 103, 95 bits
EXPORT PROTECTED CONST UINT8 g_sppInputLengthBitAfs[8];
// Reordering table
EXPORT PROTECTED CONST UINT8* CONST g_sppAfsReorderTables[8];


// AHS Speech channel coding parameters
//-------------------------------------
// Input: 159, 148, 134, 118, 103, 95 bits
EXPORT PROTECTED CONST UINT8 g_sppInputLengthBitAhs[6];
// Reordering table
EXPORT PROTECTED CONST UINT8* CONST g_sppAhsReorderTables[6];


// External declaration of tables used by SPP
// (constant tables are in spp_Tables.c)
EXPORT PROTECTED CONST UINT8 g_sppMatrixHr[2][112];
EXPORT PROTECTED CONST UINT16 g_sppMatrixFrInv[];
EXPORT PROTECTED CONST UINT8  g_sppMatrixEfr1[];
EXPORT PROTECTED CONST UINT8  g_sppMatrixEfr1Inv[];
EXPORT PROTECTED CONST UINT16 g_sppMatrixEfr2Inv[];

EXPORT PROTECTED CONST UINT32 g_sppGsmTscAburst[2];
#ifdef CHIP_DIE_8955
EXPORT PROTECTED CONST UINT32 g_sppGsmTscAburst_1[2];
EXPORT PROTECTED CONST UINT32 g_sppGsmTscAburst_2[2];
#endif

EXPORT PROTECTED CONST UINT32 g_sppGsmTscNburst[];
EXPORT PROTECTED CONST UINT32 g_sppGsmTscSch[2];
#if (CHIP_VAMOSI_SUPPORTED)
EXPORT PROTECTED CONST UINT8 g_sppNbFwequStartSt[32];
EXPORT PROTECTED CONST UINT8 g_sppNbBwequStartSt[32];
#else
EXPORT PROTECTED CONST UINT8 g_sppNbFwequStartSt[8];
EXPORT PROTECTED CONST UINT8 g_sppNbBwequStartSt[8];
#endif

EXPORT PROTECTED CONST UINT8 g_sppUsfCs2Cs3[8];
EXPORT PROTECTED CONST UINT16 g_spp_UsfCs4[8];

// In-band data encoding
EXPORT PROTECTED CONST UINT16 g_sppIcSidRatscch[4];

EXPORT PROTECTED CONST UINT16 g_sppIcAfsSpeech[4];
EXPORT PROTECTED CONST UINT16 g_sppIcAhsSpeech[4];

// Identity marker
//----------------
// 24 times repetition of sequence "0 1111 0010"
EXPORT PROTECTED CONST UINT32 g_sppImAfsSid[7];
// 24 times repetition of sequence "1 0000 1101"
EXPORT PROTECTED CONST UINT32 g_sppImAhsSid[8];
// IM of RATSCCH (Note: the data have been shift by 16 bits)
// 20 times repetition of sequence "100 0110 1001"
EXPORT PROTECTED CONST UINT32 g_sppImRatscch[8];

// Silence Frame markers
//----------------------
EXPORT PROTECTED CONST UINT32 g_sppAfsSidMarker[7];
EXPORT PROTECTED CONST UINT32 g_sppAhsSidMarker[7];
EXPORT PROTECTED CONST UINT32 g_sppRatscchMarker[7];

// Convolutional encoder parameters
//---------------------------------
// Number of words of Convolutional code output (AHS)
EXPORT PROTECTED CONST UINT8 g_sppConvOutLengthBitAhs[];

// AHS BitError calculation parameters
EXPORT PROTECTED CONST UINT32 g_sppConvOutMaskAhs[6];
#ifdef CHIP_DIE_8955
EXPORT PROTECTED CONST INT16 g_spp_8TapChestTab[8][8][19];
EXPORT PROTECTED CONST INT8 g_sppSaicTable[8][9];
EXPORT PROTECTED CONST INT8 g_sppSaicPMatrixTable[8][3];
#endif

// Channel estimation matrix
//--------------------------
#if (CHIP_VAMOSI_SUPPORTED)
EXPORT PROTECTED CONST INT16 g_sppChestTab[33][CHIP_CHEST_LEN][27-CHIP_CHEST_LEN];
#else
EXPORT PROTECTED CONST INT16 g_sppChestTab[9][CHIP_CHEST_LEN][27-CHIP_CHEST_LEN];
#endif
// Puncturing pointers table
EXPORT PROTECTED CONST UINT32* CONST g_sppCsPuncturingTable[CS_END_NEED_SRC];

// Vitac configuration table
EXPORT PROTECTED CONST SPAL_VITAC_DEC_PARAM_T g_sppVitacDecParam[CS_PARAM_QTY];

// Convolutional polynomes table
EXPORT PROTECTED CONST UINT32 g_sppVitacPolyres[8];

// Cap configuration tables
EXPORT PROTECTED CONST SPAL_CAP_CRC_PARAM_T  g_sppCsCrcParam[CS_PARAM_QTY];
EXPORT PROTECTED CONST SPAL_CAP_CONV_PARAM_T g_sppCsConvParam[CS_PARAM_QTY];
EXPORT PROTECTED CONST SPAL_CAP_CONV_PARAM_T g_sppCsSrcParam[CS_SRC_PARAM_QTY];

//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------


//=============================================================================
// spp_GetChannelType
//-----------------------------------------------------------------------------
/// TODO : temporary REALLY dirty Mode selection ...
///
/// @param ChMode UINT16. ?? See spp_rx.c.
/// @param AMRType UINT32. ?? See spp_rx.c.
/// @param AMRMode UINT32. ?? See spp_rx.c.
///
/// @return SPP_CODING_SCHEME_T. ?? See spp_fcchDetect.S
///
//=============================================================================
PROTECTED SPP_CODING_SCHEME_T spp_GetChannelType(UINT16 ChMode,UINT32 AMRType, UINT32 AMRMode);

//-----------------------
// Miscellanous Functions
//-----------------------

//=============================================================================
// spp_10log10
//-----------------------------------------------------------------------------
/// This function performs output = 10 x log10(input).
///
/// @param input UINT32. Linear power.
///
/// @return UINT8. 10Log10(input) => dB.
///
/// @addgroup misc_func
/// @author  jba
/// @date 17-06-2005
//=============================================================================
PROTECTED UINT8 spp_10log10(UINT32 in);

//=============================================================================
// spp_Log2
//-----------------------------------------------------------------------------
/// This function performs output=Log2(input).
///
/// @param input UINT32. Linear power.
///
/// @return UINT8. Log2(input).
///
/// @addgroup misc_func
/// @author  jba
/// @date 17-06-2005
//=============================================================================
PROTECTED UINT8 spp_Log2(UINT32 in);


//=============================================================================
// spp_DivL
//-----------------------------------------------------------------------------
/// This function performs the division of two signed integer
/// and generated a result in Q(fracBits) representation.
/// Example : with num = 1, denom = 4 and fracBits = 3, the returned value is 2.
///
/// @param num INT32. Numerator.
/// @param denom INT32 Denominator.
/// @param fracBits UINT8 Franctional bits in the result representation
///
/// @return INT16. The saturated LO of the 32-bit result of the division in Q(fracBits) representation.
///
/// @addgroup misc_func
/// @author  yordan
/// @date 14-04-2008
//=============================================================================
PROTECTED INT16 spp_DivL(INT32 num, INT32 denom, UINT8 fracBits);


//======================================================================
/// spp_TofSearch
//----------------------------------------------------------------------
/// This function is used in order to calculate from the 13 correlations
/// of the ouput of spal_XcorCorrTsc(), the maximum power profile.
/// The channel is consider to be 5 taps long with the main tap in the middle.
///
/// @param in          UINT32*  INPUT.  Pointer to the input matrix.
/// @param pWindow     UINT16*  INPUT.  Pointer to a table containing the taps of the channel window.
/// @param search_size UINT8    INPUT.  Size to search
/// @param BurstTOF    UINT8*   OUTPUT. Address containing the calculated TOf in Quarter bits.
/// @param SampleTOF   UINT8*   OUTPUT. Address containing the calculated TOf per sample.
///
/// @author Nadia Touliou based on the code on the spp_ChannelEstimation().
/// @date 30/11/07
//======================================================================
PROTECTED VOID spp_TofSearch(     UINT32* in,
                                  UINT16* pWindow,
                                  UINT8   search_size,
                                  UINT8*  BurstTOF,
                                  UINT8*  SampleTOF);

#ifdef CHIP_DIE_8955
//=============================================================================
// spp_Equalize
//-----------------------------------------------------------------------------
/// This function performs Viterbi equalization of a burst.
///
/// @param WinType UINT8. INPUT. Toggles channel estimation function to either
///                       normal burst or SCH.
/// @param offset UINT8. 0: original way. else: change for better gprs performance
/// @param burst SPP_EQU_BURST_T*. INPUT/OUTPUT Pointer to the burst structure.
/// @param UncachedBuffers SPP_UNCACHED_BUFFERS_T*. INPUT/OUTPUT. Pointer to
/// the work buffers designated by SPC layer.
//=============================================================================

PUBLIC VOID spp_Equalize(UINT8 WinType,
                         UINT8 offset,
                         SPP_EQU_BURST_T* burst,
                         SPP_UNCACHED_BUFFERS_T* UncachedBuffers);





//=============================================================================
// spp_Equalize_saic
//-----------------------------------------------------------------------------
/// This function performs Viterbi equalization of a burst.
///
/// @param WinType UINT8. INPUT. Toggles channel estimation function to either
///                       normal burst or SCH.
/// @param burst SPP_EQU_BURST_T*. INPUT/OUTPUT Pointer to the burst structure.
/// @param temsv UINT8. tempbuffer,is used to save the softvalues of whole burst.
/// @param UncachedBuffers SPP_UNCACHED_BUFFERS_T*. INPUT/OUTPUT. Pointer to
/// the work buffers designated by SPC layer.
//=============================================================================

PUBLIC VOID  spp_Equalize_saic(UINT8 WinType,
                               SPP_EQU_BURST_T* burst,
                               UINT8 temsv[],
                               SPP_UNCACHED_BUFFERS_T* UncachedBuffers);
#else
//=============================================================================
// spp_Equalize
//-----------------------------------------------------------------------------
/// This function performs Viterbi equalization of a burst.
///
/// @param WinType UINT8. INPUT. Toggles channel estimation function to either
///                       normal burst or SCH.
/// @param burst SPP_EQU_BURST_T*. INPUT/OUTPUT Pointer to the burst structure.
/// @param UncachedBuffers SPP_UNCACHED_BUFFERS_T*. INPUT/OUTPUT. Pointer to
/// the work buffers designated by SPC layer.
//=============================================================================

PUBLIC VOID spp_Equalize(UINT8 WinType,
                         SPP_EQU_BURST_T* burst,
                         SPP_UNCACHED_BUFFERS_T* UncachedBuffers);

#endif


//=============================================================================
// spp_FofEstimation
//-----------------------------------------------------------------------------
/// Reconstruct the reference burst (softvalues), part of it
/// (training sequence) has already been done during SNR estimation.
///
/// Once the reference burst has been rebuilt, determines the Fof by
/// comparing the first FOF_NB_SYMB symbols of reference burst and
/// received burst, as well as the last FOF_NB_SYMB symbols of reference
/// burst and received burst.
///
/// The Fof is returned in Hertz.
///
/// @param WinType UINT8. Toggles channel estimation function to either
///                       normal burst or SCH.
/// @param burst SPP_EQU_BURST_T*. Pointer to the burst structure.
///
//=============================================================================
PUBLIC VOID spp_FofEstimation(UINT8 WinType, SPP_EQU_BURST_T* burst);


//=============================================================================
// spp_Chest
//-----------------------------------------------------------------------------
/// This function performs part of channel estimation and tap filtering.
///
/// @param burst SPP_EQU_BURST_T*. Pointer to the burst structure.
/// @param taps INT16*. Output taps pointer
///
//=============================================================================
PROTECTED VOID spp_Chest(SPP_EQU_BURST_T* burst, INT16 *taps);

//=============================================================================
// spp_DigitalAgc
//-----------------------------------------------------------------------------
/// Performs digital automatic gain control.
/// This function try to adjust amplitude of samples closest to a given target value.
///
/// @param pwrValue UINT32. Power of samples before go through the function spp_DigitalAgc.
/// @param RxBuffer UINT32*. Address of the received samples.
/// @param pwrAfterDgc UINT32*. Power of samples after go through the function spp_DigitalAgc.
/// @return dgcShift INT8. Bit shift value, positive for right shift and negatvie for left shift.
//=============================================================================
PUBLIC INT8 spp_DigitalAgc(UINT32 pwrValue, UINT32* RxBuffer, UINT32* pwrAfterDgc);

//=============================================================================
// spp_EqSoftShift
//-----------------------------------------------------------------------------
/// Shift Equalizer's Output Based on SNR, if SNR<=0,clear softbits except sign bits
///
/// @param Snr INT8. input SNR
/// @param EqBufferOut UINT32*. equalizer's output ptr
///
//=============================================================================
PROTECTED VOID spp_EqSoftShift(INT8 Snr, UINT32* EqBufferOut);

#endif // SPPP_PRIVATE_H
