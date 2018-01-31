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



#ifndef __SPPP_EQUALIZER_PRIVATE_H__
#define __SPPP_EQUALIZER_PRIVATE_H__


//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "cs_types.h"
#include "spp_cfg.h"


extern volatile SPP_GLOBAL_PARAMS_T g_sppGlobalParams;


//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------
#define  DIG_AMP_TARGET_PWR_SAIC   (1<<(g_sppGlobalParams.DigitalAgcTarget))
#define  Y1_Start_Pos  (158)
#define  Lf         (4)
#define  Lb        (4)
#define  K1        (88)
#define  N1        (22)
#define  Pos0_saic (5)
#define  Lf_saic1   (Lf+1)
#define  Lb_saic1  (Lb-1)
#define  L_saic1   (((Lf + 1) << 2) + Lb - 1)
#define  NCE_SHIFT_DIV  (11)
#define  Conv_shift0       (8)
#define  Conv_shift1       (NCE_SHIFT_DIV - Conv_shift0)
#define  DIAG_FAC1        (g_sppGlobalParams.SaicDiagDelta1)
#define  DIAG_FAC2       (g_sppGlobalParams.SaicDiagDelta2)
#define  Lf_2x              (5)
#define  Lb_2x             (1)
#define  N2                 (142)
#define  Lf_saic2           (Lf_2x + 1)
#define  L_saic2           (((Lf_2x + 1) << 2) + Lb_2x - 1)
#define LPCH             (11)
#define H_TARGET_LEN (6)
#define PREFILTER_LEN (10)
#define DELTA_SHIFT (15)
#define R_TMP_RIGHT_SHIFT (11)
#define LPCH_GSM (11)
#define GAUSS_UPDATE_LENGTH (64)
#define GAUSS_LEFT_LENGTH (19)
#define GAUSS_RIGHT_LENGTH (23)
#define GAUSS_TOTAL_LENGTH (68)
#define SVAL_ONE_GPRS    (2047)
#define SVAL_NEGONE_GPRS (2048)

//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------

EXPORT VOID spal_InvalidateDcache(VOID);

//=============================================================================
// spp_Sqrt
//-----------------------------------------------------------------------------
/// Performs square root.
///
/// @param dataSource UINT32. Input value.
/// @return mid UINT16. Square root of the input value.
//=============================================================================
PUBLIC UINT16 spp_Sqrt(UINT32 dataSource);

//=============================================================================
// spal_InterfereDetect
//-----------------------------------------------------------------------------
// This function is used to detect interference
//
// @param samples      UINT32*. Pointer to the received sequence samples.
// @param ref           UINT32*. Pointer to the ideal sequence samples.
// @param power        UINT32.  Power of the sequence.
// @param noise         UINT32.  Noise of the sequence.
//
// @return               BOOL. Return the possibility of the existence of interference
//=============================================================================
PUBLIC BOOL spp_InterfereDetect(UINT32* samples, UINT32* ref, UINT32 power, UINT32 noise);

// ============================================================================
// SPP_UNCACHED_GAUSSS_T
// -----------------------------------------------------------------------------
/// buffer for gauss ce update
// =============================================================================
typedef struct
{
    UINT32                        hard_bit_back[2];
    UINT32                        hard_bit_forward[3];
    SPAL_COMPLEX_T                x_matrix[5];
    SPAL_COMPLEX_T                sym_value[GAUSS_TOTAL_LENGTH];
    SPAL_COMPLEX_T                sym_value_conj[GAUSS_TOTAL_LENGTH];
    SPAL_COMPLEX_T                q;
    SPAL_COMPLEX_T                temp[5];
    SPAL_COMPLEX_T                k_matrix[5][5];
} SPP_UNCACHED_GAUSS_GMSK_T;



#endif

