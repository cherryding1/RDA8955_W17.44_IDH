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








#ifndef TMNDEC_H
#define TMNDEC_H

/* default filename for enhancemennt layer reconstructed sequence */
#define DEF_ENHANCE_FILENAME_ROOT   "enhance.raw"

/* From sim.h */
#define PSC        1
#define PSC_LENGTH        17
#define SE_CODE                         31

#define MODE_INTER                      0
#define MODE_INTER_Q                    1
#define MODE_INTER4V                    2
#define MODE_INTRA                      3
#define MODE_INTRA_Q                    4
#define MODE_INTER4V_Q                  5

/* MODB options in normal PB frames mode */
#define PBMODE_NORMAL                   0
#define PBMODE_MVDB                     1
#define PBMODE_CBPB_MVDB                2

/* MODB options in improved PB frames mode */
#define PBMODE_BIDIR_PRED                   0
#define PBMODE_CBPB_BIDIR_PRED              2
#define PBMODE_FRW_PRED                     6
#define PBMODE_CBPB_FRW_PRED                14
#define PBMODE_BCKW_PRED                    30
#define PBMODE_CBPB_BCKW_PRED               31

#define B_DIRECT_PREDICTION               0
#define B_FORWARD_PREDICTION              1
#define B_BACKWARD_PREDICTION             2
#define B_BIDIRECTIONAL_PREDICTION        3
#define B_INTRA_PREDICTION                4

#define EP_FORWARD_PREDICTION             0
#define EI_EP_UPWARD_PREDICTION           1
#define EP_BIDIRECTIONAL_PREDICTION       2
#define EI_EP_INTRA_PREDICTION            3

#define B_EI_EP_STUFFING                  5
#define INVALID_MBTYPE                    255

#define FORWARD_PREDICTION                0
#define BIDIRECTIONAL_PREDICTION          1

#define ESCAPE                          7167
#define ESCAPE_INDEX                    102

/* picture types */
#define PCT_INTRA                       0
#define PCT_INTER                       1
#define PCT_IPB                         2
#define PCT_B                           3
#define PCT_EI                          4
#define PCT_EP                          5
#define PCT_PB                          6

/* scalability layers */
#define MAX_LAYERS                      2

/* pb frame type */
#define PB_FRAMES                       1
#define IM_PB_FRAMES                    2

#define ON                              1
#define OFF                             0

#define YES                             1
#define NO                              0

/* source formats */
#define SF_SQCIF                        1 /* 001 */
#define SF_QCIF                         2 /* 010 */
#define SF_CIF                          3 /* 011 */
#define SF_4CIF                         4 /* 100 */
#define SF_16CIF                        5 /* 101 */
#define SF_CUSTOM                       6 /* 110 */
#define EXTENDED_PTYPE                  7 /* 111 */

#define EXTENDED_PAR                    15  /* 1111 */

/* this is necessary for the max resolution 16CIF */
//#define MBC                             88 //max 16cif macro block column 88x16=352x4=1408 . change shenh
//#define MBR                             72//max 16cif macro block row 72x16=288x4=1152 .change shenh
#define MBC                             24 //max for cif 22x16=352 . change shenh
#define MBR                             20//max for cif 18x16=288.change shenh


#define NO_VEC                          999

/* Intra Mode for Advanced Intra Coding mode */
#define INTRA_MODE_DC                   0
#define INTRA_MODE_VERT_AC              2
#define INTRA_MODE_HORI_AC              3

/* Scalability modes */
#define SNR_SCALABILITY                     1
#define SPATIAL_SCALABILITY_H               3
#define SPATIAL_SCALABILITY_V           5
#define SPATIAL_SCALABILITY_HV          7


/* Pixel aspect ration for custom source format */
#define PAR_0                           0  /* 0000 */
#define PAR_SQUARE                      1  /* 0001 */
#define PAR_CIF                         2  /* 0010 */
#define PAR_525                         3  /* 0011 */
#define PAR_CIF_STRETCHED               4  /* 0100 */
#define PAR_525_STRETCHED               5  /* 0101 */
#define PAR_EXTENDED                    15 /* 1111 */

#endif

