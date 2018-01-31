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



#include "cs_types.h"

#include "chip_id.h"

#include "global_macros.h"
#include "vitac.h"
// SPAL public headers
#include "spal_vitac.h"
#include "spal_profile_codes.h"
// SPAL private headers
#include "spalp_private.h"
#include "spalp_debug.h"


//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------

#if  (!CHIP_VITAC_8PSK_SUPPORTED)

// granite, greenstone, ...
#define VITAC_STATUS          (VITAC_EQU_PENDING|VITAC_DEC_PENDING|VITAC_TB_PENDING)
#define VITAC_START_GMSK_EQU   VITAC_START_EQU
#define VITAC_RESCALE          hwp_vitac->rescale = EQU_RESC_THRES

#else // (CHIP_VITAC_8PSK_SUPPORTED)

#define VITAC_STATUS          (VITAC_EQU_GMSK_PENDING|VITAC_EQU_8PSK_PENDING|VITAC_DEC_PENDING|VITAC_TB_PENDING)
#define VITAC_RESCALE

#endif // (!CHIP_VITAC_8PSK_SUPPORTED)

#define VITAC_DONE  0x717ACU


//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------


//=============================================================================
// spal_VitacBusy
//-----------------------------------------------------------------------------
// Checks whether the VITAC is busy ?
//
//=============================================================================
PUBLIC BOOL spal_VitacBusy()
{
    UINT32 busy;
    busy =  (hwp_vitac->status) & VITAC_STATUS;
    if (busy)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


//=============================================================================
// spal_VitacEqualize
//-----------------------------------------------------------------------------
// Run Equalization Viterbi Process on a half burst without traceback.
//
// @param trellis BOOL. INPUT. This parameters makes the Viterbi equalize the
// backward part of the treillis of the forward part of the treillis. Two defines
// have been specifically designed for this purpose:
// - BACKWARD_TRELLIS (defined as TRUE in spal_vitac.h) is used for backward operation,
// - FORWARD_TRELLIS (defined as FALSE in spal_vitac.h) is used for forward operation.
//
// @param param SPAL_VITAC_EQU_PARAM_T*. INPUT/OUTPUT. param sets the working buffer,
// the operation mode, provides the input as well as the output.
// Parameters:
// - param.H0. UINT32. INPUT. Is the value of the first complex channel tap where
// ((INT16*)param.H0)[0] is the real part (inphase) of the channel coefficient and
// ((INT16*)param.H0)[1] is the imaginary part (quadrature) of the channel coefficient.
// - param.HL. UINT32. INPUT. Is the value of the last (5th) complex channel tap where
// ((INT16*)param.HL)[0] is the real part (inphase) of the channel coefficient and
// ((INT16*)param.HL)[1] is the imaginary part (quadrature) of the channel coefficient.
// - param.bmSum. INT16*. INPUT. The complex branch metric for a specific state of the
// register are precalculated by the spal_XcorBmsum function and stored in a
// INT16 buffer. ((INT16*)param.bmSum)[2*i] is the real part (inphase) of the branch metric
// corresponding to the ith register state and ((INT16*)param.bmSum)[2*i+1] is the
// imaginary part (quadrature) of the branch metric corresponding to the ith register
// state.
// - param.length. UINT8. INPUT. Number of samples in half burst including tail samples.
// - param.shift_bm. UINT8. INPUT. Branch metric shift value. The path metric are
// accumulated by adding branch metric. Each computed branch metric is shifted by
// param.shift_bm before being added to the path metric. param.shift_bm is computed outside
// of the viterbi equalizer and depends on the SNR. Hardware limitations impose that
// param->shift_bm should belong to [4,13],
// - param.shift_sb. UINT8. INPUT. Soft bit shift value. The soft bits are computed as
// a difference between two path metrics. The soft bits are on 8 bits whereas the
// path metrics are on 16 bits, therefore a shift is sometimes required to determine
// the most appropriate 8 bits to output as a soft bit. param.shift_sb is computed outside
// of the viterbi equalizer and depends on the SNR. In practice param.shift_sb=0 until
// param.shift_bm is maximum (13). Hardware limitations impose that param->shift_sb
// should belong to [0,9],
// - first_state. UINT8. INPUT. The initial state of the register is determined by
// the first and last bits of the training sequence for respectively the backward
// and forward treillis equalization.
// .
// Working buffers:
// - param.pm UINT16*. INPUT/OUTPUT. Sets the buffer where path metrics (a.k.a. node
// metrics) are stored. When the VITAC finishes equalization, the buffer can
// be used to select the most likely end state when the tail bits do not close
// the treillis.
// .
// Input/Output buffers:
// - in. UINT32*. INPUT. Pointer to the input buffer. It contains the complex
// received samples where ((INT16*)in)[2*i] is the real part (inphase) of the
// ith complex received sample and ((INT16*)in)[2*i+1] is the imaginary part
// (quadrature) of the ith complex received sample.
// - out. UINT8*. OUTPUT. Pointer to the output buffer. It contains the 8 bits
// softbits for each state, for each sample (before trace back) coded as 1 sign
// bit and 7 amplitude bits and where path_diff[n+(m*VIT_NSTATES)] is the soft
// bit for the nth state for the mth sample.
// .
//
// @return SPAL_VITAC_ERROR_T. Indicates valid or invalid VITAC processing.
//=============================================================================
PUBLIC UINT8 spal_VitacEqualize(BOOL trellis, SPAL_VITAC_EQU_PARAM_T* param)
{
    UINT8 bm_shift_lev;
    UINT8 sv_shift_lev;
    UINT32 cmd;
    int i;

#define BM_SHIFT_MAX    17

    // TODO : no error return anymore, was useless and misleading
    // as errors were not handled in upper levels...

    // derive/saturate shift parameter for Branch metrics
    if (param->shift_bm < 4)
        bm_shift_lev = BM_SHIFT_MAX-4;
    else if (param->shift_bm > BM_SHIFT_MAX)
        bm_shift_lev = 0;
    else
        bm_shift_lev = BM_SHIFT_MAX - param->shift_bm;

    // derive/saturate shift parameter for Softvalues
    if  (param->shift_sb > BM_SHIFT_MAX-4)
        sv_shift_lev = 0;
    else
        sv_shift_lev = BM_SHIFT_MAX-4 - param->shift_sb;

    // Init Path Metrics
    for (i=0; i<(EQU_NB_STATES/2); i++)
    {
        *(UINT32*)(&(param->pm[2*i])) = EQU_PM_INIT;
    }
    // Put PM of starting state to min value
    param->pm[param->first_state] = 0;

    // Symbol buffer start address
    hwp_vitac->ch_symb_addr = (UINT32)param->in;

    // Path Metrics base address
    hwp_vitac->pm_base_addr = (UINT32)param->pm;

    // Output start address
    hwp_vitac->out_base_addr = (UINT32)param->out;

    if (trellis == BACKWARD_TRELLIS)
    {
        // First and last channel parameters
        hwp_vitac->H0_param = param->HL;
        hwp_vitac->HL_param = param->H0;

        // Precalculated sum terms base address
        hwp_vitac->exp_symb_addr = (UINT32)&(param->bmSum[EQU_NB_STATES]);

        cmd = VITAC_BKWD_TRELLIS;
    }
    else
    {
        hwp_vitac->H0_param = param->H0;
        hwp_vitac->HL_param = param->HL;

        // Precalculated sum terms base address
        hwp_vitac->exp_symb_addr = (UINT32)&(param->bmSum[0]);

        cmd = 0;
    }

    // Rescaling threshold
    VITAC_RESCALE;

    // Command
    cmd |=  VITAC_START_GMSK_EQU |
            //(VITAC_INT_ENABLE?VITAC_INT_MASK:0) | // TODO write the IT based version
            VITAC_NB_STATES(VITAC_16_STATES) |
            VITAC_BM_SHIFT_LEV(bm_shift_lev) |
            VITAC_SV_SHIFT_LEV(sv_shift_lev) |
            VITAC_NB_SYMBOLS(param->length);


    hwp_vitac->command = cmd;

    return vitac_err_none;
}


//=============================================================================
// spal_VitacFullDecoding
//-----------------------------------------------------------------------------
// This function performs the Viterbi decoding of a coded block. It receives
// the soft bits and coding scheme and outputs the decoded bits. The parameters
// corresponding to each coding scheme are stored inside SPAL internal
// variables.
//
// @param cs_type SPAL_CODING_SCHEME_T. INPUT. SPAL coding scheme definition.
// FACCH_H, FACCH_F, SACCH, SDCCH, BCCH and AGCH are not included as they are
// considered as CS_CS1
// @param punct_ptr UINT32*. Pointer to the puncturing table
// @param survivorMem_ptr UINT32*. Pointer to the survivor memory, allocated in an upper layer
// @param softBit_ptr UINT32*. INPUT. Pointer to the soft bit buffer. The soft
// bit are coded on 8 bits with 1 sign bit and 7 amplitude bits.
// @param output_ptr UINT32*. OUTPUT. Pointer to the decoded bits buffer.
//
// @return SPAL_VITAC_ERROR_T. Indicates valid or invalid VITAC processing.
//=============================================================================
PUBLIC UINT8 spal_VitacFullDecoding(SPAL_VITAC_DEC_CFG_T* pCfg, UINT32* output_ptr)
{
    UINT32 PathMetric_ptr[64];
    UINT16* PathMetric = (UINT16*)MEM_ACCESS_UNCACHED((UINT32)PathMetric_ptr);
    UINT32 localPuncTable[SPAL_PUNCTURE_TABLE_SIZE];
    UINT32 cmd = 0;

    UINT16 nbStates = 16;
    UINT8  tail = 4;
    UINT16 i;
    UINT16 nb_symbols = pCfg->vitacDecParam.nbSymbol;

    SPAL_PROFILE_FUNCTION_ENTER(spal_VitacFullDecoding);

    // TODO : for Emerald, tables does not need to be copied for IR
    if (pCfg->vitacDecParam.is_punct)
    {
        for (i=0; i < SPAL_PUNCTURE_TABLE_SIZE; i++)
            localPuncTable[i] = pCfg->punctTable[i];
        cmd = VITAC_PUNCTURED;
    }

    if (pCfg->vitacDecParam.states == VITAC_64_STATES)
    {
        nbStates = 64;
        tail = 6;
    }

    // Init Path Metrics
    for (i=0; i<(nbStates/2); i++)
    {
        PathMetric_ptr[i] = EQU_PM_INIT;
    }

    // Starting state
    PathMetric[0] = 0;

    // VITERBI TRELLIS
    // Symbol buffer start address
    hwp_vitac->ch_symb_addr = (UINT32)pCfg->inSoftBit;

    // Puncturing Matrix base address
    hwp_vitac->exp_symb_addr = (UINT32)localPuncTable;

    // Path Metrics base address
    hwp_vitac->pm_base_addr = (UINT32)PathMetric;

    // Output start address
    hwp_vitac->out_base_addr = (UINT32)pCfg->survivorMem;

    // Polynom results
    hwp_vitac->res_poly[0] = pCfg->convPoly0;
    hwp_vitac->res_poly[1] = pCfg->convPoly1;
    hwp_vitac->res_poly[2] = pCfg->convPoly2;

    // Rescaling threshold
    VITAC_RESCALE;

    // Command
    cmd |=  VITAC_START_DEC |
            //(VITAC_INT_ENABLE?VITAC_INT_MASK:0) | // TODO write the IT based version
            VITAC_NB_STATES(pCfg->vitacDecParam.states) |
            VITAC_CODE_RATE(pCfg->vitacDecParam.rate) |
            VITAC_NB_SYMBOLS(nb_symbols);


    hwp_vitac->command = cmd;

    // Poll
    while(spal_VitacBusy());

    // TRACEBACK
    // Survivor Memory end address
    hwp_vitac->exp_symb_addr = (UINT32)(&(((UINT16*)pCfg->survivorMem)[(nbStates/16)*(nb_symbols-1)]));

    // Output start address
    hwp_vitac->out_base_addr = (UINT32)(&output_ptr[(nb_symbols-tail-1)/32]);

    // Command
    // Don't use IT here as the processing time is really short
    hwp_vitac->command =
        VITAC_START_TB |
        VITAC_NB_STATES(pCfg->vitacDecParam.states) |
        VITAC_NB_SYMBOLS((nb_symbols-tail));

    // Poll
    while(spal_VitacBusy());

    SPAL_PROFILE_FUNCTION_EXIT(spal_VitacFullDecoding);
    return vitac_err_none;
}
