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











#define GSM_TABLE_C
#include "vpp_fr_private.h"
#include        "vpp_fr_gsm.h"

/*  Table 4.1  Quantization of the Log.-Area Ratios
 */
/* i                 1      2      3        4      5      6        7       8 */
INT16 gsm_A[8]  = {20480, 20480, 20480,  20480,  13964,  15360,   8534,  9036};
INT16 gsm_B[8]  = {    0,     0,  2048,  -2560,     94,  -1792,   -341, -1144};
INT16 gsm_MIC[8] = { -32,   -32,   -16,   -16,     -8,     -8,     -4,    -4 };
INT16 gsm_MAC[8] = {  31,   31,    15,     15,      7,      7,      3,     3 };


/*  Table 4.2  Tabulation  of 1/A[1..8]
 */
INT16 gsm_INVA[8]= { 13107, 13107,  13107, 13107,  19223, 17476, 31454, 29708 };


/*   Table 4.3a  Decision level of the LTP gain quantizer
 */
/*  bc                0         1         2          3                  */
INT16 gsm_DLB[4] = {  6554,   16384,    26214,     32767        };


/*   Table 4.3b   Quantization levels of the LTP gain quantizer
 */
/* bc                 0          1        2          3                  */
INT16 gsm_QLB[4] = {  3277,   11469,    21299,     32767        };


/*   Table 4.4   Coefficients of the weighting filter
 */
/* i                0      1   2    3   4      5      6     7   8   9    10  */
INT16 gsm_H[11] = {-134, -374, 0, 2054, 5741, 8192, 5741, 2054, 0, -374, -134 };


/*   Table 4.5   Normalized inverse mantissa used to compute xM/xmax
 */
/* i                    0        1    2      3      4      5     6      7   */
INT16 gsm_NRFAC[8] = { 29128, 26215, 23832, 21846, 20165, 18725, 17476, 16384 };


/*   Table 4.6   Normalized direct mantissa used to compute xM/xmax
 */
/* i                  0      1       2      3      4      5      6      7   */
INT16 gsm_FAC[8] = { 18431, 20479, 22527, 24575, 26623, 28671, 30719, 32767 };
