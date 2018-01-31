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











#ifndef H263_GLOBAL_H
#define H263_GLOBAL_H


//#include <stdio.h>
#include "yuv420_bilinear_resample.h"
#include "h263_config.h"
#include "tmndec.h"

#ifndef GLOBAL
#define EXTERN extern
#else
#define EXTERN
#endif

/* Some macros */
#define mmax(a, b)        ((a) > (b) ? (a) : (b))
#define mmin(a, b)        ((a) < (b) ? (a) : (b))
#define mnint(a)        ((a) < 0 ? (int)(a - 0.5) : (int)(a + 0.5))
#define sign(a)         ((a) < 0 ? -1 : 1)

/* prototypes of global functions */

/* getbits.c */
void initbits _ANSI_ARGS_ ((unsigned char *pInbuf));//change shenh
void resetbits _ANSI_ARGS_((unsigned char *pInbuf, unsigned int datalen));//add shenh
void fillbfr _ANSI_ARGS_ ((void));
unsigned int showbits _ANSI_ARGS_ ((int n));
unsigned int getbits1 _ANSI_ARGS_ ((void));
void flushbits _ANSI_ARGS_ ((int n));
unsigned int getbits _ANSI_ARGS_ ((int n));

/* getblk.c */
void DctTabMov();//add shenh
void getblock _ANSI_ARGS_ ((int comp, int mode, int INTRA_AC_DC, int Mode));
//void get_sac_block _ANSI_ARGS_ ((int comp, int mode, int INTRA_AC_DC, int Mode));

/* gethdr.c */
int getheader _ANSI_ARGS_ ((void));
void startcode _ANSI_ARGS_ ((void));
void initoptions _ANSI_ARGS_ ((void));
void getgobheader _ANSI_ARGS_ ((void));
/* getpic.c */
int init_tab_map _ANSI_ARGS_ ((void));//add shenh
void getpicture _ANSI_ARGS_ ((unsigned char *outbuf, unsigned short LcdBufWidth, unsigned short StartX, unsigned short StartY, int gob, zoomPar *pZoom));
void putlast _ANSI_ARGS_ ((int framenum));
//void PictureDisplay _ANSI_ARGS_((int *framenum));//change shenh
//void PictureDisplay _ANSI_ARGS_((unsigned char * outbuf));
void initdecoder _ANSI_ARGS_ ((void));//move from tmndec.c. shenh
void cleandecoder (void);
void flushdisplaybuffer _ANSI_ARGS_ ((int framenum));//move from tmndec.c. shenh


/* scalability funcitons */
//void UpsampleReferenceLayerPicture _ANSI_ARGS_((void)); //shenh change
//void UpsampleComponent _ANSI_ARGS_((unsigned char *enhanced, unsigned char *base,//shenh change
//                        int horiz, int vert));
/* advanced intra coding functions */
void Intra_AC_DC_Decode _ANSI_ARGS_ ((short *store_qcoeff, int INTRA_AC_DC, int MBA, int xpos, int ypos, int comp, int newgob));
void fill_null _ANSI_ARGS_ ((int pred[]));
void fill_A _ANSI_ARGS_ ((int pred[], short *store_qcoeff, int xpos, int ypos, int block));
void fill_B _ANSI_ARGS_ ((int pred[], short *store_qcoeff, int xpos, int ypos, int block));
int oddifyclipDC _ANSI_ARGS_ ((int x));
int clipAC _ANSI_ARGS_ ((int x));
int clipDC _ANSI_ARGS_ ((int x));
//void interpolate_image _ANSI_ARGS_ ((unsigned char *in, unsigned char *out, int width, int height));//change shenh

/* getvlc.c */
int getTMNMV _ANSI_ARGS_ ((void));
int getRVLC  _ANSI_ARGS_ ((void));
int getMCBPC _ANSI_ARGS_ ((void));
int getMODB _ANSI_ARGS_ ((void));
int getMCBPCintra _ANSI_ARGS_ ((void));
int getCBPY _ANSI_ARGS_ ((void));
//int getMBTYPE _ANSI_ARGS_ ((int *true_B_cbp, int *true_B_quant));
int getscalabilityCBPC ();

/* idct.c */
void idct _ANSI_ARGS_ ((short *block));
int init_idct _ANSI_ARGS_ ((void));

/* idctref.c */
//void init_idctref _ANSI_ARGS_ ((void));//change shenh
//void idctref _ANSI_ARGS_ ((short *block));//change shenh

/* tmndec.c */
//void error _ANSI_ARGS_ ((char *text));//change shenh
//void printbits _ANSI_ARGS_ ((int code, int bits, int len));

/* recon.c */
void reconstruct _ANSI_ARGS_ ((int bx, int by, int P, int bdx, int bdy, int MODB, int newgob1));
//void reconstruct_true_B _ANSI_ARGS_ ((int bx, int by, int true_B_prediction_type));change shenh
//void reconstruct_ei_ep _ANSI_ARGS_ ((int bx, int by, int ei_ep_prediction_type));//change shenh

/* store.c */
//void storeframe _ANSI_ARGS_ ((unsigned char *src[], int frame));change shenh
void storeframe _ANSI_ARGS_ ((unsigned char *src[], unsigned char * outbuf, unsigned short LcdBufWidth, unsigned short StartX, unsigned short StartY,  zoomPar *pZoom));
//void store_enhancement_frame _ANSI_ARGS_ ((unsigned char *src[], int frame));change shenh
//void save_frame _ANSI_ARGS_ ((unsigned char *src[], int framenum, FILE * file));
//void store_one _ANSI_ARGS_ ((char *outname, unsigned char *src[], int offset, int incr,
//                             int height));

/* sac.c */
int decode_a_symbol _ANSI_ARGS_ ((/*int cumul_freq[]*/ short *cumul_freq));
void decoder_reset ();

/* ring_buf.c */
//int  get_reference_picture _ANSI_ARGS_ ((void)); //change shenh
//void store_picture _ANSI_ARGS_ ((int quality));//change shenh


/* global variables */

#if 0//change shenh
EXTERN char version[]
#ifdef GLOBAL
    = "tmndecode v3.0\n(C) 1997, University of British Columbia, Canada\n(C) 1995, 1996 Telenor R&D, Norway\n"
#endif
      ;
#endif

#if 0 //move to getblk.c
/* zig-zag scan */

EXTERN unsigned char zig_zag_scan[64]//use shenh. only for getblk.c
#ifdef GLOBAL
=
{
    0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5,
    12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63
}
#endif
;
#endif

//EXTERN int lines[]//change shenh. for gethdr.c getpic.c tmndec.c
EXTERN short lines[]
#ifdef GLOBAL
    = {-1,128,176,352,704,1408,-1}
#endif
      ;

//EXTERN int pels[7]//change shenh. for gethdr.c getpic.c tmndec.c
EXTERN short pels[7]
#ifdef GLOBAL
    = {-1,96,144,288,576,1152,-1}
#endif
      ;

/* other scan orders needed in advanced intra coding mode */

/*
EXTERN unsigned char alternate_horizontal_scan[64]//use shenh.only for getblk.c
#ifdef GLOBAL
=
{
  0, 1, 2, 3, 8, 9, 16, 17, 10, 11, 4, 5, 6, 7, 15, 14,
  13, 12, 19, 18, 24, 25, 32, 33, 26, 27, 20, 21, 22, 23, 28, 29,
  30, 31, 34, 35, 40, 41, 48, 49, 42, 43, 36, 37, 38, 39, 44, 45,
  46, 47, 50, 51, 56, 57, 58, 59, 52, 53, 54, 55, 60, 61, 62, 63
}
#endif
;
EXTERN unsigned char alternate_vertical_scan[64]//use shenh. only for getblk.c
#ifdef GLOBAL
=
{
  0, 8, 16, 24, 1, 9, 2, 10, 17, 25, 32, 40, 48, 56, 57, 49,
  41, 33, 26, 18, 3, 11, 4, 12, 19, 27, 34, 42, 50, 58, 35, 43,
  51, 59, 20, 28, 5, 13, 6, 14, 21, 29, 36, 44, 52, 60, 37, 45,
  53, 61, 22, 30, 7, 15, 23, 31, 38, 46, 54, 62, 39, 47, 55, 63
}
#endif
;
*/

/* color space conversion coefficients
 *
 * entries are {crv,cbu,cgu,cgv}
 *
 * crv=(255/224)*65536*(1-cr)/0.5 cbu=(255/224)*65536*(1-cb)/0.5
 * cgu=(255/224)*65536*(cb/cg)*(1-cb)/0.5
 * cgv=(255/224)*65536*(cr/cg)*(1-cr)/0.5
 *
 * where Y=cr*R+cg*G+cb*B (cr+cg+cb=1) */
#if 0 //change shenh
EXTERN int convmat[8][4]
#ifdef GLOBAL
=
{
    {117504, 138453, 13954, 34903}, /* no sequence_display_extension */
    {117504, 138453, 13954, 34903}, /* ITU-R Rec. 709 (1990) */
    {104597, 132201, 25675, 53279}, /* unspecified */
    {104597, 132201, 25675, 53279}, /* reserved */
    {104448, 132798, 24759, 53109}, /* FCC */
    {104597, 132201, 25675, 53279}, /* ITU-R Rec. 624-4 System B, G */
    {104597, 132201, 25675, 53279}, /* SMPTE 170M */
    {117579, 136230, 16907, 35559}/* SMPTE 240M (1987) */
}
#endif
;
#endif

EXTERN int quiet;
EXTERN int errorstate;//for exit(). add shenh
//EXTERN int trace; shenh
//EXTERN int save_frames; shenh
//EXTERN char errortext[256]; shenh
//EXTERN unsigned char *prev_frame[3], *current_frame[3], *bframe[3];//change shenh
EXTERN unsigned char *prev_frame[3], *current_frame[3];
//EXTERN unsigned char *tmp_f[3], *tmp_b[3];//change shenh
//EXTERN unsigned char *edgeframe[3], *edgeframeorig[3], *exnewframe[3];//change shenh
//EXTERN unsigned char *nextedgeframe[3], *nextedgeframeorig[3];//change shenh
//EXTERN unsigned char *enhancementedgeframe[3], *enhacnementedgeframeorig[3];//change shenh
//EXTERN unsigned char *next_I_P_frame[3], *prev_I_P_frame[3], *buffered_frame[3];//change shenh
EXTERN unsigned char *next_I_P_frame[3], *prev_I_P_frame[3] ;
//EXTERN int MV[2][6][MBR + 1][MBC + 2];//change shenh
//EXTERN short ****MV;
EXTERN short *MV;
//EXTERN int true_B_direct_mode_MV[2][5][MBR + 1][MBC + 2];//change shenh
//EXTERN short ****true_B_direct_mode_MV;
//EXTERN int modemap[MBR + 1][MBC + 2];//change shenh
EXTERN short *modemap;
//EXTERN int anchorframemodemap[MBR + 1][MBC + 2];//change shenh
//EXTERN short **anchorframemodemap;
//EXTERN int predictionmap[MBR + 1][MBC + 2];//change shenh
//EXTERN short **predictionmap;
EXTERN unsigned char *clp;
EXTERN int horizontal_size, vertical_size, mb_width, mb_height;
EXTERN int coded_picture_width, coded_picture_height;
EXTERN int ref_coded_picture_width, ref_coded_picture_height;
EXTERN int chrom_width, chrom_height, blk_cnt;
EXTERN int ref_chrom_width, ref_chrom_height;
EXTERN int pict_type, newgob, gfid, pgfid;
EXTERN int mv_outside_frame;
EXTERN int syntax_arith_coding;
EXTERN int adv_pred_mode;
EXTERN int overlapping_MC;
EXTERN int use_4mv;
EXTERN int pb_frame;
EXTERN int true_B_frame;
EXTERN int scalability_mode;
//EXTERN int buffered_framenum;//change shenh
EXTERN int long_vectors;
//EXTERN int fault, expand;// change shenh
EXTERN int fault;// change shenh
//EXTERN int verbose;// change shenh
//EXTERN int refidct; //change shenh
EXTERN int matrix_coefficients;
EXTERN int temp_ref, prev_non_disposable_temp_ref, next_non_disposable_temp_ref;
EXTERN int pic_quant, quant, source_format, base_source_format;
EXTERN int UFEP;
EXTERN int prev_mv_outside_frame, prev_sac, prev_adv_pred, prev_aic, prev_df;
EXTERN int prev_slice_struct, prev_rps, prev_isd, prev_aivlc, prev_mq;
EXTERN int prev_4mv, prev_long_vectors, prev_obmc;

/* Following variables are used in H.263+ bitstream decoding */
EXTERN int plus_type;           /* indicates if extended PTYPE exists or
                                 * not. */
EXTERN int optional_custom_PCF;
EXTERN int advanced_intra_coding;
EXTERN int deblocking_filter_mode;
EXTERN int slice_structured_mode;
EXTERN int reference_picture_selection_mode;
EXTERN int independently_segmented_decoding_mode;
EXTERN int alternative_inter_VLC_mode;
EXTERN int modified_quantization_mode;
EXTERN int reduced_resolution_update_mode;
EXTERN int reference_picture_resampling_mode;
EXTERN int rtype;
//EXTERN int post_filter;//shenh change
EXTERN int unlimited_unrestricted_motion_vectors;
//EXTERN int concealment; //shenh change

/* Scalability globals */
#if 0//change shenh
EXTERN unsigned char *prev_enhancement_frame[MAX_LAYERS][3], *current_enhancement_frame[MAX_LAYERS][3];
EXTERN unsigned char *curr_reference_frame[3], *upsampled_reference_frame[3];
EXTERN unsigned char *enhance_edgeframeorig[MAX_LAYERS][3], *enhance_edgeframe[MAX_LAYERS][3];
EXTERN unsigned char *tmp_enhance_fwd[MAX_LAYERS][3], *tmp_enhance_up[MAX_LAYERS][3];
EXTERN int enhancement_layer_init[MAX_LAYERS];
EXTERN int enhance_pict;
#endif

/* custom format variables */
EXTERN int CP_PAR_code;
EXTERN int CP_picture_width_indication;
EXTERN int CP_picture_width;
EXTERN int CP_picture_height_indication;
EXTERN int CP_picture_height;
EXTERN int PAR_width;
EXTERN int PAR_height;

EXTERN int CP_clock_frequency;
EXTERN int SSS_rectangular_slices;
EXTERN int SSS_arbitary_slice_ordering;
EXTERN int enhancement_layer_num;
EXTERN int reference_layer_number;
EXTERN int MF_of_reference_picture_selection;
//EXTERN int TRI, TRPI;
EXTERN int temporal_reference_for_prediction;



EXTERN int trd, trb, bscan, bquant, true_b_trb;
/* change shenh
EXTERN int bscan_tab[]
#ifdef GLOBAL
= {2, 4, 6, 8}
#endif
;
*/
//EXTERN int bquant_tab[] //change shenh. for getblk.c getpic.c
/*
EXTERN char bquant_tab[]
#ifdef GLOBAL
= {5, 6, 7, 8}
#endif
;*/

//EXTERN int OM[5][8][8]//change shenh.only for recon.c
#if 0
EXTERN char OM[5][8][8]
#ifdef GLOBAL
=
{
    {
        {4, 5, 5, 5, 5, 5, 5, 4},
        {5, 5, 5, 5, 5, 5, 5, 5},
        {5, 5, 6, 6, 6, 6, 5, 5},
        {5, 5, 6, 6, 6, 6, 5, 5},
        {5, 5, 6, 6, 6, 6, 5, 5},
        {5, 5, 6, 6, 6, 6, 5, 5},
        {5, 5, 5, 5, 5, 5, 5, 5},
        {4, 5, 5, 5, 5, 5, 5, 4},
    }, {
        {2, 2, 2, 2, 2, 2, 2, 2},
        {1, 1, 2, 2, 2, 2, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
    }, {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 2, 2, 2, 2, 1, 1},
        {2, 2, 2, 2, 2, 2, 2, 2},
    }, {
        {0, 0, 0, 0, 1, 1, 1, 2},
        {0, 0, 0, 0, 1, 1, 2, 2},
        {0, 0, 0, 0, 1, 1, 2, 2},
        {0, 0, 0, 0, 1, 1, 2, 2},
        {0, 0, 0, 0, 1, 1, 2, 2},
        {0, 0, 0, 0, 1, 1, 2, 2},
        {0, 0, 0, 0, 1, 1, 2, 2},
        {0, 0, 0, 0, 1, 1, 1, 2},
    }, {
        {2, 1, 1, 1, 0, 0, 0, 0},
        {2, 2, 1, 1, 0, 0, 0, 0},
        {2, 2, 1, 1, 0, 0, 0, 0},
        {2, 2, 1, 1, 0, 0, 0, 0},
        {2, 2, 1, 1, 0, 0, 0, 0},
        {2, 2, 1, 1, 0, 0, 0, 0},
        {2, 2, 1, 1, 0, 0, 0, 0},
        {2, 1, 1, 1, 0, 0, 0, 0},
    }
}
#endif
;
#endif
/*
//EXTERN int h263_roundtab[16]//change shenh. for recon.c getpic.c
EXTERN char h263_roundtab[16]
#ifdef GLOBAL
= {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2}
#endif
;
*/
#if 0
//EXTERN int MQ_chroma_QP_table[32]//change shenh. for getblk.c getpic.c
EXTERN char MQ_chroma_QP_table[32]
#ifdef GLOBAL
    = {0, 1, 2, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 10, 11, 11, 12, 12, 12, 13, 13, 13,
       14, 14, 14, 14, 14, 15, 15, 15, 15, 15
      }
#endif
      ;
#endif

/* output */
//EXTERN char *outputname; shenh change
//EXTERN char trace_file_name[100];// shenh
//EXTERN FILE *trace_file;// shenh
//EXTERN char recon_file_name[100];// shenh
//EXTERN FILE *recon_file_ptr;// shenh
//EXTERN char enhance_recon_file_name[MAX_LAYERS][100];// shenh
//EXTERN FILE *enhance_recon_file_ptr[MAX_LAYERS];// shenh

//EXTERN int outtype; change shenh
#define T_YUV      0
#define T_SIF      1
#define T_TGA      2
#define T_PPM      3
#define T_X11      4
#define T_YUV_CONC 5
#define T_WIN      6
/*
EXTERN struct ld
{
  // bit input
  //int infile; //change shenh
  unsigned int datalen;//add shenh
  //unsigned char rdbfr[2051];//change shenh
  unsigned char *rdbfr;
  unsigned char *rdptr;
  unsigned char inbfr[16];
  int incnt;
  int bitcnt;
  // block data
  //short block[12][64];change shenh
  short *block[12];
} base, *ld;
*/

typedef struct
{
    /* bit input */
    //int infile; //change shenh
    unsigned int datalen;//add shenh
    //unsigned char rdbfr[2051];//change shenh
    unsigned char *rdbfr;
    unsigned char *rdptr;
#ifdef USE_INTERNAL_SRAM
    unsigned char *inbfr;
#else
    unsigned char inbfr[16];
#endif
    int incnt;
    int bitcnt;
    /* block data */
    //short block[12][64];change shenh
#ifdef USE_INTERNAL_SRAM
    short *block;
#else
    //short *block[12];
    short *block;
#endif
    short *block_temp;//shenh for 1 block
} bitdata;
EXTERN bitdata base;
EXTERN bitdata *ld;

/* For reference picture selection Mode  (Annex N) */
#if 0//change shenh
#define RING_SIZE 8

EXTERN int   ring_pointer;               /* pointer - to last picture */
EXTERN void *ring_lum[RING_SIZE];    /* luminance */
EXTERN void *ring_Cr[RING_SIZE];     /* chominance */
EXTERN void *ring_Cb[RING_SIZE];
EXTERN int   ring_temporal_reference[RING_SIZE];     /* TR values for the pictures */
EXTERN int   ring_quality[RING_SIZE];     /* picture quality */
EXTERN int   stop_decoder;           /* flag if we completely out of sync */
#endif

/* Back Channel Message Structure */
/* used with Annex N with Videomux channel */
#if 0//change shenh
EXTERN struct bcm
{
    int present;           /* set to 1 if bcm is present */
    int BT;                /* Back-channel message type (2 bits) */
    int URF;               /* Unreliable flag (1 bit) */
    int TR;                /* Temporal reference (10 bits) */
    int ELNUMI;            /* Enhancement layer number indication (1 bit) */
    int ELNUM;             /* Enhancement layer number (4 bits) */
    int BCPM;              /* Indicates that BSBI is present (1 bit) */
    int BSPI;              /* Back-channel sub bitstream indicator (2 bits) */
    int BEPB1;             /* Back-channel emulation prevention bit 1 (1 bit set to 1 */
    int GN_MBA;            /* GOB number / Macroblock address (variable length) */
    int BEPB2;             /* Back-channel emulation prevention bit 2 (1 bit set to 1 */
    int RTR;               /* Requested temporal reference (10 bits) */
    int BSTUF;             /* stuffing (variable length) */
} BCM;
#endif

#endif
