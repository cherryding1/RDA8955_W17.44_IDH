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












#include "h263_config.h"
#include "tmndec.h"
#define GLOBAL
#include "h263_global.h"
#include "sxr_ops.h"

#include "indices.h"
#include "sactbls.h"
#include "malloc.h"

#if 0//change shenh
static int coded_map[MBR + 1][MBC + 1];// change shenh
static int quant_map[MBR + 1][MBC + 1];
static int STRENGTH[] = {1, 1, 2, 2, 3, 3, 4, 4, 4, 5, 5, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 10, 11, 11, 11, 12, 12, 12};
static int STRENGTH1[] = {1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4};
static int STRENGTH2[] = {1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3};
#endif
//static int **coded_map;//[MBR + 1][MBC + 1];
//static int **quant_map;//[MBR + 1][MBC + 1];
#if 0//change shenh
static char STRENGTH[] = {1, 1, 2, 2, 3, 3, 4, 4, 4, 5, 5, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 10, 11, 11, 11, 12, 12, 12};
static char STRENGTH1[] = {1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4};
static char STRENGTH2[] = {1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3};
#endif

/* private prototypes */
static void get_I_P_MBs _ANSI_ARGS_ ((int framenum, int gob));
//static void get_B_MBs _ANSI_ARGS_ ((int framenum));//change shenh
//static void get_EI_EP_MBs _ANSI_ARGS_ ((int framenum));//change shenh
static void clearblock _ANSI_ARGS_ ((int comp));
static int motion_decode _ANSI_ARGS_ ((int vec, int pmv));
static int find_pmv _ANSI_ARGS_ ((int x, int y, int block, int comp));
//static void addblock _ANSI_ARGS_ ((int comp, int bx, int by, int addflag));
//static void reconblock_b _ANSI_ARGS_ ((int comp, int bx, int by, int mode, int bdx, int bdy));//change shenh
//static void find_bidir_limits _ANSI_ARGS_ ((int vec, int *start, int *stop, int nhv));//change shenh
//static void find_bidir_chroma_limits _ANSI_ARGS_ ((int vec, int *start, int *stop));//change shenh
//static void make_edge_image _ANSI_ARGS_ ((unsigned char *src, unsigned char *dst, int width, int height, int edge));change shenh
//static void init_enhancement_layer _ANSI_ARGS_((int layer));//shenh change
#if 0//change shenh
void edge_filter _ANSI_ARGS_ ((unsigned char *lum, unsigned char *Cb, unsigned char *Cr, int width, int height));
void horiz_edge_filter _ANSI_ARGS_ ((unsigned char *rec, int width, int height, int chr));
void vert_edge_filter _ANSI_ARGS_ ((unsigned char *rec, int width, int height, int chr));
void vert_post_filter (unsigned char *rec, int width, int height, int chr);
void horiz_post_filter (unsigned char *rec, int width, int height, int chr);
void PostFilter (unsigned char *lum, unsigned char *Cb, unsigned char *Cr, int width, int height);
#endif

//for h263_myMalloc
int init_tab_map ()//add shenh
{
    //int i,j,k;
    //int res;

    DctTabMov();
    //init_vlcTab();

    //static short coded_map[MBR + 1][MBC + 1];
    /*
      if(sizeof(int)*(MBR + 1)+sizeof(int)*(MBR + 1)*(MBC + 1)>sxr_Free()-32)
      {
      return -1;
      }
      coded_map=h263_myMalloc(sizeof(int)*(MBR + 1)+sizeof(int)*(MBR + 1)*(MBC + 1));
      if(!coded_map )
      return -1;
      memset(coded_map, 0, sizeof(int)*(MBR + 1)+sizeof(int)*(MBR + 1)*(MBC + 1));

      for(i=0;i<MBR + 1;i++)
      {
      coded_map[i]=(int*)(coded_map+(MBR + 1))+i*(MBC + 1);
      }
    */

    //static short quant_map[MBR + 1][MBC + 1];
    /*
      if(sizeof(int)*(MBR + 1)+sizeof(int)*(MBR + 1)*(MBC + 1)>sxr_Free()-32)
      {
      return -1;
      }
      quant_map=h263_myMalloc(sizeof(int)*(MBR + 1)+sizeof(int)*(MBR + 1)*(MBC + 1));
      if( !quant_map)
      return -1;
      memset(quant_map, 0, sizeof(int)*(MBR + 1)+sizeof(int)*(MBR + 1)*(MBC + 1));

      for(i=0;i<MBR + 1;i++)
      {
      quant_map[i]=(int*)(quant_map+(MBR + 1))+i*(MBC + 1);
      }
    */

    //short MV[2][6][MBR + 1][MBC + 2];
#if 0
    MV=h263_myMalloc(sizeof(int)*(2+2*6+2*6*(MBR + 1))+sizeof(short)*2*6*(MBR + 1)*(MBC + 2));
    if(!MV )
        return -1;
    memset(MV, 0, (sizeof(int)*(2+2*6+2*6*(MBR + 1))+sizeof(short)*2*6*(MBR + 1)*(MBC + 2)));

    for(i=0; i<2; i++)
    {
        MV[i]=MV+2+i*6;
        for(j=0; j<6; j++)
        {
            MV[i][j]=MV+2+2*6+i*6*(MBR + 1)+j*(MBR + 1);

            for(k=0; k<MBR + 1; k++)
            {
                MV[i][j][k]=(short*)(MV+2+2*6+2*6*(MBR + 1))+i*6*(MBR + 1)*(MBC + 2)+j*(MBR + 1)*(MBC + 2)+k*(MBC + 2);
            }
        }
    }
#else
    MV=(short*)h263_myMalloc(sizeof(short)*2*6*(MBR + 1)*(MBC + 2));
    if(!MV )
        return -1;
    memset(MV, 0, (sizeof(short)*2*6*(MBR + 1)*(MBC + 2)));
#endif

    //short true_B_direct_mode_MV[2][5][MBR + 1][MBC + 2];
    /*
      if((sizeof(int)*(2+2*5+2*5*(MBR + 1))+sizeof(short)*2*5*(MBR + 1)*(MBC + 2))>sxr_Free()-32)
      {
      return -1;
      }
      true_B_direct_mode_MV=h263_myMalloc(sizeof(int)*(2+2*5+2*5*(MBR + 1))+sizeof(short)*2*5*(MBR + 1)*(MBC + 2));
      if(!true_B_direct_mode_MV )
      return -1;
      memset(true_B_direct_mode_MV, 0, (sizeof(int)*(2+2*5+2*5*(MBR + 1))+sizeof(short)*2*5*(MBR + 1)*(MBC + 2)));

      for(i=0;i<2;i++)
      {
      true_B_direct_mode_MV[i]=true_B_direct_mode_MV+2+i*5;
      for(j=0;j<5;j++)
      {
      true_B_direct_mode_MV[i][j]=true_B_direct_mode_MV+2+2*5+i*5*(MBR + 1)+j*(MBR + 1);

      for(k=0;k<MBR + 1;k++)
      {
      true_B_direct_mode_MV[i][j][k]=(short*)(true_B_direct_mode_MV+2+2*5+2*5*(MBR + 1))+i*5*(MBR + 1)*(MBC + 2)+j*(MBR + 1)*(MBC + 2)+k*(MBC + 2);
      }
      }
      }
    */

    //short modemap[MBR + 1][MBC + 2];
#if 0
    modemap=h263_myMalloc(sizeof(int)*(MBR + 1)+sizeof(short)*(MBR + 1)*(MBC + 2));
    if(!modemap )
        return -1;
    memset(modemap, 0, sizeof(int)*(MBR + 1)+sizeof(short)*(MBR + 1)*(MBC + 2));

    for(i=0; i<MBR + 1; i++)
    {
        modemap[i]=(short*)(modemap+(MBR + 1))+i*(MBC + 2);
    }
#else
    modemap=(short*)h263_myMalloc(sizeof(short)*(MBR + 1)*(MBC + 2));
    if(!modemap )
        return -1;
    memset(modemap, 0, sizeof(short)*(MBR + 1)*(MBC + 2));
#endif

    //short anchorframemodemap[MBR + 1][MBC + 2];
    /*
      if((sizeof(int)*(MBR + 1)+sizeof(short)*(MBR + 1)*(MBC + 2))>sxr_Free()-32)
      {
      return -1;
      }
      anchorframemodemap=h263_myMalloc(sizeof(int)*(MBR + 1)+sizeof(short)*(MBR + 1)*(MBC + 2));
      if(!anchorframemodemap )
      return -1;
      memset(anchorframemodemap, 0, sizeof(int)*(MBR + 1)+sizeof(short)*(MBR + 1)*(MBC + 2));

      for(i=0;i<MBR + 1;i++)
      {
      anchorframemodemap[i]=(short*)(anchorframemodemap+(MBR + 1))+i*(MBC + 2);
      }
    */

    //short predictionmap[MBR + 1][MBC + 2];
    /*
      if((sizeof(int)*(MBR + 1)+sizeof(short)*(MBR + 1)*(MBC + 2))>sxr_Free()-32)
      {
      return -1;
      }
      predictionmap=h263_myMalloc(sizeof(int)*(MBR + 1)+sizeof(short)*(MBR + 1)*(MBC + 2));
      if(!predictionmap )
      return -1;
      memset(predictionmap, 0, sizeof(int)*(MBR + 1)+sizeof(short)*(MBR + 1)*(MBC + 2));

      for(i=0;i<MBR + 1;i++)
      {
      predictionmap[i]=(short*)(predictionmap+(MBR + 1))+i*(MBC + 2);
      }
    */


    return 0;
}

/* reference picture selection */
//int  get_reference_picture(void); //change shenh
//void store_picture(int);//change shenh

/* error concealment */
void conceal_missing_gobs(int start_mb_row_missing, int number_of_mb_rows_missing);
/**********************************************************************
 *
 *      Name:         getpicture
 *      Description:  decode and display one picture
 *      Input:        frame number
 *      Returns:
 *      Side effects:
 *
 *  Date: 971102  Author: mikeg@ee.ubc.ca
 *
 *
 ***********************************************************************/
//void getpicture (int *framenum, int gob)//change shenh
void getpicture (unsigned char *outbuf, unsigned short LcdBufWidth, unsigned short StartX, unsigned short StartY, int gob, zoomPar *pZoom)
{
    unsigned char *tmp;
    int            i;//,  quality=0;
    //int store_pb;
    // static int absolute_temp_ref = 0;
    //static int prev_temp_ref = 0;
    errorstate=0;

    switch (pict_type)
    {
        case PCT_INTRA:
        case PCT_INTER:
            // case PCT_PB:
            //  case PCT_IPB:
            //enhance_pict = NO; change shenh

            if (0 == UFEP /*&& plus_type*/)
            {
                mv_outside_frame = prev_mv_outside_frame;

                deblocking_filter_mode = prev_df;

                adv_pred_mode = prev_adv_pred;

                overlapping_MC = prev_obmc;
                use_4mv = prev_4mv;
                long_vectors = prev_long_vectors;

                syntax_arith_coding = prev_sac;

                advanced_intra_coding = prev_aic;
                if (advanced_intra_coding)//add shenh
                {
//                if (!quiet)
//                    diag_printf ("error: advanced_intra_coding is not supported for h263_myMalloc. shenh\n");
                    //exit (-1);
                    errorstate=-1;
                    return;
                }


                slice_structured_mode = prev_slice_struct;

                reference_picture_selection_mode = prev_rps;

                independently_segmented_decoding_mode = prev_isd;

                alternative_inter_VLC_mode = prev_aivlc;

                modified_quantization_mode = prev_mq;
            }

            for (i = 0; i < 3; i++)//switch  next_I_P_frame/ current_frame address for output yuv.shenh
            {
                tmp = prev_frame[i];
                prev_I_P_frame[i] = prev_frame[i] = next_I_P_frame[i];
                next_I_P_frame[i] = current_frame[i] = tmp;
            }
            /* if the picture header is missing, assume temp_ref increments */
            /* not done yet*/

            /* Annex N: get the correct reference picture for prediction */
#if 0
            if (reference_picture_selection_mode)
            {
                // if (-1 == (quality = get_reference_picture())) //change shenh
                //break;
                if (reference_picture_selection_mode)//add shenh
                {
//                if (!quiet)
//                    diag_printf ("error: ANNEX N Reference Picture Selection mode is not supported for h263_myMalloc\n");
                    //exit (-1);
                    errorstate=-1;
                    return;
                }

                if (quality > 256)
                {
                    //fprintf(stderr,"completely out of sync -- waiting for I-frame\n");
//                diag_printf("completely out of sync -- waiting for I-frame\n");
                    // stop_decoder = 1; //change shenh
                    break;
                }
            }
#endif

            //if ((mv_outside_frame) && (*framenum > 0))//change shenh
            if (mv_outside_frame)
            {
                /* change shenh
                   make_edge_image (prev_I_P_frame[0], edgeframe[0], coded_picture_width,
                   coded_picture_height, 32);
                   make_edge_image (prev_I_P_frame[1], edgeframe[1], chrom_width, chrom_height, 16);
                   make_edge_image (prev_I_P_frame[2], edgeframe[2], chrom_width, chrom_height, 16);
                */
                errorstate=-1;
                return;
            }

            if (pb_frame) //add shenh
            {
                errorstate=-1;
                return;
            }

            if (enhancement_layer_num > 1)//add shenh
            {
                errorstate=-1;
                return;
            }

            //get_I_P_MBs (*framenum, gob);//change shenh
#ifdef  PROFILE_DEBUG
            hal_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_HAL, 0x29b0);
#endif

            get_I_P_MBs (0, gob);
#ifdef  PROFILE_DEBUG
            hal_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_HAL, 0x29b0);
#endif
            if(errorstate)return;

            //store_pb = pb_frame;
            pb_frame = 0;

            //if (deblocking_filter_mode)
            //      {
            /* change shenh
               edge_filter (current_frame[0], current_frame[1], current_frame[2],
               coded_picture_width, coded_picture_height);
               if( errorstate) return;
            */
            //      }

            //pb_frame = store_pb;

            //if (pb_frame)
            //{
            /* change shenh
               if (deblocking_filter_mode)
               edge_filter(bframe[0],bframe[1],bframe[2],
               coded_picture_width,coded_picture_height);
            */
            //      errorstate=-1;
            //      return;

            //    }

            //PictureDisplay(framenum);  //change shenh
            //PictureDisplay(outbuf);
#ifdef  PROFILE_DEBUG
            hal_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_HAL, 0x29b1);
#endif

            storeframe (current_frame, outbuf, LcdBufWidth, StartX, StartY, pZoom);
#ifdef  PROFILE_DEBUG
            hal_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_HAL, 0x29b1);
#endif
            if( errorstate) return;
            //if (reference_picture_selection_mode) //change shenh
            // store_picture((pict_type == PCT_INTRA) ? 0 : quality);
            /*
              if (reference_picture_selection_mode)//add shenh
              {
              if (!quiet)
              diag_printf ("error: ANNEX N Reference Picture Selection mode is not supported for h263_myMalloc\n");
              //exit (-1);
              errorstate=-1;
              return;
              }
            */

            if (1 == UFEP)
            {
                prev_mv_outside_frame = mv_outside_frame;
                prev_sac = syntax_arith_coding;
                prev_adv_pred = adv_pred_mode;
                prev_aic = advanced_intra_coding;
                prev_df = deblocking_filter_mode;
                prev_slice_struct = slice_structured_mode;
                prev_rps = reference_picture_selection_mode;
                prev_isd = independently_segmented_decoding_mode;
                prev_aivlc = alternative_inter_VLC_mode;
                prev_mq = modified_quantization_mode;
                prev_long_vectors = long_vectors;
                prev_obmc = overlapping_MC;
                prev_4mv = use_4mv;
            }

            break;

        case PCT_B:

#if 0//change shenh. not support for h263_myMalloc
            if (enhancement_layer_num > 1)
                enhance_pict = YES;
            else
                enhance_pict = NO;

            for (i = 0; i < 3; i++)
            {
                current_frame[i] = bframe[i];
            }

            /* Forward prediction */
            make_edge_image (prev_I_P_frame[0], edgeframe[0], coded_picture_width,
                             coded_picture_height, 32);
            make_edge_image (prev_I_P_frame[1], edgeframe[1], chrom_width, chrom_height, 16);
            make_edge_image (prev_I_P_frame[2], edgeframe[2], chrom_width, chrom_height, 16);

            /* Backward prediction */
            make_edge_image (next_I_P_frame[0], nextedgeframe[0], coded_picture_width,
                             coded_picture_height, 32);
            make_edge_image (next_I_P_frame[1], nextedgeframe[1], chrom_width, chrom_height, 16);
            make_edge_image (next_I_P_frame[2], nextedgeframe[2], chrom_width, chrom_height, 16);

            get_B_MBs (*framenum);
            if(errorstate)return;

            PictureDisplay(framenum);
#endif

            //add shenh
//        if (!quiet)
//            diag_printf ("error: PCT_B is not supported !\n");
            errorstate=-1;
            return;

            break;

        case PCT_EI:
        case PCT_EP:
#if 0 //unsupport now. shenh change
            enhance_pict = YES;

            if (!enhancement_layer_init[enhancement_layer_num-2])
            {
                init_enhancement_layer(enhancement_layer_num-2);
                enhancement_layer_init[enhancement_layer_num-2] = ON;
            }

            for (i = 0; i < 3; i++)
            {
                tmp = prev_enhancement_frame[enhancement_layer_num-2][i];
                prev_enhancement_frame[enhancement_layer_num-2][i] =
                    current_enhancement_frame[enhancement_layer_num-2][i];
                current_enhancement_frame[enhancement_layer_num-2][i] = tmp;
            }

            if (scalability_mode >= 3)
            {
                UpsampleReferenceLayerPicture();
                curr_reference_frame[0] = upsampled_reference_frame[0];
                curr_reference_frame[1] = upsampled_reference_frame[1];
                curr_reference_frame[2] = upsampled_reference_frame[2];
            }
            else
            {
                curr_reference_frame[0] = current_frame[0];
                curr_reference_frame[1] = current_frame[1];
                curr_reference_frame[2] = current_frame[2];
            }

            make_edge_image (prev_enhancement_frame[enhancement_layer_num-2][0],
                             enhance_edgeframe[enhancement_layer_num-2][0],
                             coded_picture_width, coded_picture_height, 32);
            make_edge_image (prev_enhancement_frame[enhancement_layer_num-2][1],
                             enhance_edgeframe[enhancement_layer_num-2][1],
                             chrom_width, chrom_height, 16);
            make_edge_image (prev_enhancement_frame[enhancement_layer_num-2][2],
                             enhance_edgeframe[enhancement_layer_num-2][2],
                             chrom_width, chrom_height, 16);

            get_EI_EP_MBs (*framenum);

            if (deblocking_filter_mode)
                edge_filter (current_enhancement_frame[enhancement_layer_num-2][0],
                             current_enhancement_frame[enhancement_layer_num-2][1],
                             current_enhancement_frame[enhancement_layer_num-2][2],
                             coded_picture_width, coded_picture_height);
            /* shenh change
               store_one (enhance_recon_file_name[enhancement_layer_num-2],
               current_enhancement_frame[enhancement_layer_num-2],
               0, coded_picture_width, vertical_size);
            */
            storeframe ( current_enhancement_frame[enhancement_layer_num-2], *framenum);
#endif

            //add shenh
//        if (!quiet)
//            diag_printf ("error: PCT_EI PCT_EP is not supported !\n");
            errorstate=-1;
            return;

            break;

        default:

            break;
    }
}

/* decode all macroblocks of the current picture */
#if 0//change shenh
static int change_of_quant_tab_10[32] = {0, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3};
static int change_of_quant_tab_11[32] = {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 2, 1, -5};
#endif
static char change_of_quant_tab_10[32] = {0, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3};
static char change_of_quant_tab_11[32] = {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 2, 1, -5};

//#include "hal_debug.h"//profile
extern void simple_idct_put(uint8 *dest, int line_size, int16 *block);
extern void simple_idct_add(uint8 *dest, int line_size, int16 *block);

static void get_I_P_MBs (int framenum, int gob)
{
    int comp;
    int MBA, MBAmax;
    int bx, by;

    int COD = 0, MCBPC, CBPY, CBP = 0, CBPB = 0, MODB = 0, Mode = 0, DQUANT;
    //int COD_index, CBPY_index, MODB_index, DQUANT_index, MCBPC_index;
    //int INTRADC_index, YCBPB_index, UVCBPB_index, mvdbx_index, mvdby_index;
    int mvx = 0, mvy = 0, /*mvy_index, mvx_index,*/ pmv0, pmv1, xpos, ypos, i, k;
    int mvdbx = 0, mvdby = 0, pmvdbx, pmvdby, /*YCBPB, UVCBPB,*/ gobheader_read;
    int startmv, stopmv, offset, bsize, last_done = 0, pCBP = 0, pCBPB = 0, pCOD = 0, pMODB = 0;
    int DQ_tab[4] = {-1, -2, 1, 2};
    short *bp;
    //int long_vectors_bak;
    int tmp = 0;
    int tmp2,tmp3,tmp4;
    int pnewgob = 0;
    /*
      ModeBit. shenh add
      bit0=1= MODE_INTER                      0
      bit1=1= MODE_INTER_Q                    1
      bit2=1= MODE_INTER4V                    2
      bit3=1= MODE_INTRA                      3
      bit4=1= MODE_INTRA_Q                    4
      bit5=1= MODE_INTER4V_Q                  5
      switch(Mode)
      {
      case MODE_INTER:
      ModeBit=0x01;
      break;
      case MODE_INTER_Q:
      ModeBit=0x02;
      break;
      case MODE_INTER4V:
      ModeBit=0x04;
      break;
      case MODE_INTRA:
      ModeBit=0x08;
      break;
      case MODE_INTRA_Q:
      ModeBit=0x10;
      break;
      case MODE_INTER4V_Q:
      ModeBit=0x20;
      break;
      default:
      ModeBit=0;
      break;
      }
    */
    unsigned char ModeBit=0;

    /* Error concealment variables */
    //int pypos = 0;
    //int start_mb_row_missing, number_of_mb_rows_missing;


    /* variables used in advanced intra coding mode */
    // int INTRA_AC_DC = 0;//, INTRA_AC_DC_index;
    //short *store_qcoeff;
    //int quality=0;
    //int decode_last_mb = 0;
    // int dont_reconstruct_next_mb;

    errorstate=0;

    /* number of macroblocks per picture */
    MBAmax = mb_width * mb_height;
    gob -= 1; /* 1 was added in getheader() */

    MBA = 0;                      /* macroblock address */
    newgob = 0;
    xpos = ypos = 0;

    //hal_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_HAL, 0x29b0);// for profile


    /* mark MV's above the picture */
    tmp=(MBR+1)*(MBC+2);
    for (i = 1; i < mb_width + 1; i++)
    {
        for (k = 0; k < 5; k++)
        {
            //MV[0][k][0][i] = NO_VEC;//vector 无矢量
            tmp2=k*tmp+i;
            *(MV+tmp2) = NO_VEC;
            //MV[1][k][0][i] = NO_VEC;
            tmp2+=6*tmp;
            *(MV+tmp2) = NO_VEC;
        }
        //modemap[0][i] = MODE_INTRA;
        *(modemap+i) = MODE_INTRA;
    }
    /* zero MV's on the sides of the picture */
    tmp4=(mb_width + 1);
    for (i = 0; i < mb_height + 1; i++)
    {
        for (k = 0; k < 5; k++)
        {
            //MV[0][k][i][0] = 0;
            tmp2=k*tmp+i*(MBC+2);
            *(MV+tmp2) =0;
            //MV[1][k][i][0] = 0;
            tmp3=tmp2+6*tmp;
            *(MV+tmp3) =0;
            //MV[0][k][i][mb_width + 1] = 0;
            tmp2+=tmp4;
            *(MV+tmp2) =0;
            //MV[1][k][i][mb_width + 1] = 0;
            tmp3+=tmp4;
            *(MV+tmp3) =0;
        }
        //modemap[i][0] = MODE_INTRA;
        tmp2=(MBC+2);
        *(modemap+i*tmp2) = MODE_INTRA;
        //modemap[i][mb_width + 1] = MODE_INTRA;
        *(modemap+i*tmp2+tmp4) = MODE_INTRA;
    }
    /* initialize the qcoeff used in advanced intra coding */
    /*
      if (advanced_intra_coding)
      {
      // store the qcoeff for the frame
      if ((store_qcoeff = (short *) calloc (64 * MBAmax * blk_cnt, sizeof (short))) == 0)
      {
      fprintf (stderr, "getMB(): Couldn't allocate store_qcoeff.\n");
      exit (-1);
      }
      //fprintf (stderr, "advanced_intra_coding is not support for calloc.\n");
      diag_printf ("advanced_intra_coding is not support for calloc.\n");
      //exit (-1);
      errorstate=-1;
      return;
      }
    */
    fault = 0;
    gobheader_read = 0;
    /* if a gob with GN != 0 was present as the first gob
     * of a picture, decode the rest of the gob header */
    if (gob) goto getgobhrd;

    for (;;)
    {

        if (MBA >= MBAmax)
        {
            /* all macroblocks decoded */
            //memcpy(anchorframemodemap,modemap,(sizeof(int)*(MBR+1)*(MBC+2)) );
            return;
        }


resync:
        /* This version of the decoder does not resync on every possible
         * error, and it does not do all possible error checks. It is not
         * difficult to make it much more error robust, but I do not think it
         * is necessary to include this in the freely available version. */

        if (fault)
        {
            //diag_printf ("Warning: A Fault Condition Has Occurred - Resyncing \n");
            startcode ();             /* sync on new startcode */
            fault = 0;
        }

        if (!(showbits (22) >> 6))
        {
            /* startcode */
            startcode ();

            /* in case of byte aligned start code, ie. PSTUF, GSTUF or ESTUF is
             * used */
            tmp=showbits (22);
            if (tmp == (32 | SE_CODE) || (tmp == PSC << 5))
            {
                /* end of sequence */
                /*
                  if (!(syntax_arith_coding && MBA < MBAmax))
                  {
                  return;
                  }
                */
                return;
            }
#if 0
            else if ((tmp == PSC << 5))
            {
                /* new picture */
                /* conceal the last mb row */

                /*  shenh change
                    if (concealment && MBA < MBAmax)
                    {
                    // store the missing GOB number for error concealment
                    number_of_mb_rows_missing = (MBAmax - MBA)  / mb_width ;
                    start_mb_row_missing = MBA / mb_width;
                    decode_last_mb = 1;
                    pypos = ypos;
                    }
                    else if (!(syntax_arith_coding && MBA < MBAmax))
                    {
                    return;
                    }*/
                /*
                  if (!(syntax_arith_coding && MBA < MBAmax))
                  {
                  return;
                  }*/
                return;

            }
#endif
            else
            {
                // if (!(syntax_arith_coding && MBA % mb_width))
                //  {
                /*
                  if (syntax_arith_coding)
                  {
                  // SAC hack to finish GOBs which  end with MBs coded with no * bits.
                  gob = (showbits (22) & 31);
                  if (gob * mb_width != MBA)
                  goto finish_gob;
                  }
                */
                gob = getheader () - 1;//getheader()函数内部对得到的5bit gob加了1，此gob数即为图像块组序号
                if(errorstate)return;

                if (gob > mb_height)
                {
//                    if (!quiet)
//                        diag_printf ("GN out of range\n");
                    return;
                }
                /* Get the remaining of the GOB header:
                &                   this should be the complete gob header to facilitate the integration
                   of the slice structure and the syntax of Annex N and the BCM.
                   This will be changed when Slices are implemented */
                /* if the first sync of a picture in not GN 0, start here */
getgobhrd:

                getgobheader();
                if( errorstate) return;

                /* Get the reference picture for prediction corresponding to TRP
                   in the GOB header.  Only the reference GOB is needed, but it is
                   simpler to get the whole reference picture.
                   This should be changed to save time */

#if 0
                if (reference_picture_selection_mode)
                {
                    // if (-1 == (quality = get_reference_picture())) //change shenh
                    //   break;
                    if (reference_picture_selection_mode)//add shenh
                    {
//                        if (!quiet)
//                            diag_printf ("error: ANNEX N Reference Picture Selection mode is not supported for h263_myMalloc\n");
                        //exit (-1);
                        errorstate=-1;
                        return;
                    }

                    if (quality > 256)
                    {
                        //fprintf(stderr,"completely out of sync -- waiting for I-frame\n");
//                        diag_printf("completely out of sync -- waiting for I-frame\n");
                        // stop_decoder = 1;//change shenh
                        break;
                    }
                    //if ((mv_outside_frame) && (framenum > 0))//change shenh
                    if (mv_outside_frame)
                    {
                        /*
                          make_edge_image (prev_I_P_frame[0], edgeframe[0], coded_picture_width,
                          coded_picture_height, 32);
                          make_edge_image (prev_I_P_frame[1], edgeframe[1], chrom_width, chrom_height, 16);
                          make_edge_image (prev_I_P_frame[2], edgeframe[2], chrom_width, chrom_height, 16);
                        */
                        errorstate=-1;
                        return;
                    }
                }
#endif

                /* if some data is loss due to (channel) errors,
                 * decode the last (delayed) macroblock */
#if 0 //shenh change
                if (concealment && (gob * mb_width != MBA))
                {

                    /* if we are missing the last gob of the previous frame and
                     * the first gob of the current frame, framenum should be
                     * incremented */
                    /* store the missing GOB number for error concealment */
                    if (gob - MBA / mb_width < 0)
                    {
                        ++framenum;
                        number_of_mb_rows_missing = (MBAmax - MBA) / mb_width ;
                    }
                    else
                        number_of_mb_rows_missing = gob - MBA / mb_width ;
                    start_mb_row_missing = MBA / mb_width;
                    decode_last_mb = 1;
                    pypos = ypos;
                }
#endif

                xpos = 0;
                ypos = gob;
                MBA = ypos * mb_width;

                newgob = 1;
                gobheader_read = 1;

                //if (syntax_arith_coding)
                //  decoder_reset ();   /* init. arithmetic decoder buffer after
                //                      * gob */
                // }

            }
        }

        /*
          if (decode_last_mb)
          {
          if (!start_mb_row_missing)
          {
          // the first gob is missing, we don't need to decode the last MB
          goto conceal_gob;
          }
          else
          {
          xpos = 0;
          ypos = ++pypos;
          COD = 1;
          goto reconstruct_mb;
          }
          }
        */

        //dont_reconstruct_next_mb = 0;

        //finish_gob:

        /* SAC specific label */
        if (gobheader_read)
        {
            gobheader_read = 0;
        }
        else
        {
            xpos = MBA % mb_width;
            ypos = MBA / mb_width;
            if (xpos == 0 && ypos > 0)
                newgob = 0;
        }

read_cod: //读宏块编码指示位

        /*
          if (syntax_arith_coding)
          {
          if (pict_type == PCT_INTER || pict_type == PCT_IPB)
          {
          COD_index = decode_a_symbol (cumf_COD);
          COD = codtab[COD_index];
          }
          else
          {
          COD = 0;                // COD not used in I-pictures, set to zero
          coded_map[ypos + 1][xpos + 1] = 1;
          }
          }
          else
          {
        */
        if (PCT_INTER == pict_type /*|| PCT_IPB == pict_type*/)
        {
            COD = showbits (1);
        }
        else//帧内默认已编码COD=0
        {

            COD = 0;                /* Intra picture -> not skipped */
            //coded_map[ypos + 1][xpos + 1] = 1;//标记此位置宏块有编码
        }
        //   }

        if (!COD)//宏块有编码
        {
            /* COD == 0 --> not skipped */
            /*
                if (syntax_arith_coding)
                {
                if (pict_type == PCT_INTER || pict_type == PCT_IPB)
                {
                if (plus_type)
                {
                MCBPC_index = decode_a_symbol (cumf_MCBPC_4MVQ);
                MCBPC = mcbpctab_4mvq[MCBPC_index];
                }
                else
                {
                MCBPC_index = decode_a_symbol (cumf_MCBPC_no4MVQ);
                MCBPC = mcbpctab[MCBPC_index];
                }
                }
                else
                {
                MCBPC_index = decode_a_symbol (cumf_MCBPC_intra);
                MCBPC = mcbpc_intratab[MCBPC_index];
                }
                }
                else
                {
            */
#if 0//shenh
            if (PCT_INTER == pict_type /*|| PCT_IPB == pict_type*/)
            {
                /* flush COD bit */
                flushbits (1);
            }
#endif

            //MCBPC 宏块类型和色度编码块样式
            if (PCT_INTRA == pict_type)
            {
                MCBPC = getMCBPCintra ();
            }
            else
            {
                /* flush COD bit */
                flushbits (1);

                MCBPC = getMCBPC ();
            }
            // }

            if (fault)
                goto resync;

            if (MCBPC == 255)
            {
                /* stuffing - read next COD without advancing MB count. */
                goto read_cod;
            }
            else
            {
                /* normal MB data */
                Mode = MCBPC & 7;
                switch(Mode)
                {
                    case MODE_INTER:
                        ModeBit=0x01;
                        break;
                    case MODE_INTER_Q:
                        ModeBit=0x02;
                        break;
                    case MODE_INTER4V:
                        ModeBit=0x04;
                        break;
                    case MODE_INTRA:
                        ModeBit=0x08;
                        break;
                    case MODE_INTRA_Q:
                        ModeBit=0x10;
                        break;
                    case MODE_INTER4V_Q:
                        ModeBit=0x20;
                        break;
                    default:
                        ModeBit=0;
                        break;
                }
#if 0
                if (advanced_intra_coding && (Mode == MODE_INTRA || Mode == MODE_INTRA_Q))
                {
                    /* get INTRA_AC_DC mode for annex I */
                    /*
                      if (syntax_arith_coding)
                      {
                      INTRA_AC_DC_index = decode_a_symbol (cumf_INTRA_AC_DC);
                      INTRA_AC_DC = intra_ac_dctab[INTRA_AC_DC_index];
                      }
                      else
                      {
                    */
                    /* using VLC */
                    if (!showbits (1))
                        INTRA_AC_DC = getbits (1);
                    else
                        INTRA_AC_DC = getbits (2);
                    //}
                }
#endif

                /* MODB and CBPB */
                // if (pb_frame)
                //  {
                /*// change shenh
                  CBPB = 0;
                  if (syntax_arith_coding)
                  {
                  if (pb_frame== PB_FRAMES)
                  {
                  MODB_index = decode_a_symbol (cumf_MODB_G);
                  MODB = modb_tab_G[MODB_index];
                  }
                  else
                  {
                  MODB_index = decode_a_symbol (cumf_MODB_M);
                  MODB = modb_tab_M[MODB_index];
                  }
                  }
                  else
                  MODB = getMODB ();

                  if ( (MODB == PBMODE_CBPB_MVDB && pb_frame == PB_FRAMES) ||
                  (pb_frame == IM_PB_FRAMES &&
                  (MODB == PBMODE_CBPB_FRW_PRED ||
                  MODB == PBMODE_CBPB_BIDIR_PRED || MODB == PBMODE_CBPB_BCKW_PRED) ) )
                  {
                  if (syntax_arith_coding)
                  {
                  for (i = 0; i < 4; i++)
                  {
                  YCBPB_index = decode_a_symbol (cumf_YCBPB);
                  YCBPB = ycbpb_tab[YCBPB_index];
                  CBPB |= (YCBPB << (6 - 1 - i));
                  }
                  for (i = 4; i < 6; i++)
                  {
                  UVCBPB_index = decode_a_symbol (cumf_UVCBPB);
                  UVCBPB = uvcbpb_tab[UVCBPB_index];
                  CBPB |= (UVCBPB << (6 - 1 - i));
                  }
                  }
                  else
                  CBPB = getbits (6);

                  }
                */
                ///        errorstate=-1;
                //       return;

                //      }

                /*
                  if (syntax_arith_coding)
                  {
                  if ((Mode == MODE_INTRA || Mode == MODE_INTRA_Q))
                  {
                  // Intra
                  CBPY_index = decode_a_symbol (cumf_CBPY_intra);
                  CBPY = cbpy_intratab[CBPY_index];
                  }
                  else
                  {
                  CBPY_index = decode_a_symbol (cumf_CBPY);
                  CBPY = cbpytab[CBPY_index];
                  }

                  }
                  else
                  {
                */
                CBPY = getCBPY ();//亮度编码块样式
                //  }
            }

            /* Decode Mode and CBP */
            //if ((Mode == MODE_INTRA || Mode == MODE_INTRA_Q))
            if ((ModeBit&0x18)!=0)
            {
                /* Intra */
                //coded_map[ypos + 1][xpos + 1] = 1;

                // if (!syntax_arith_coding)
                CBPY = CBPY ^ 15;   /* needed in huffman coding only */

                CBP = (CBPY << 2) | (MCBPC >> 4);
            }
            else
            {
                if (/*!syntax_arith_coding &&*/ alternative_inter_VLC_mode && ((MCBPC >> 4) == 3))
                    CBPY = CBPY ^ 15;     /* Annex S.3 change */

                CBP = (CBPY << 2) | (MCBPC >> 4);

                //if ((Mode == MODE_INTER4V || Mode == MODE_INTER4V_Q) /*&& !use_4mv*/)
                if ((ModeBit&0x24)!=0)
                {
                    fault = 1;
                    // if (!quiet)
                    // {
                    /* Could set fault-flag and resync */
                    //   diag_printf ("8x8 vectors not allowed in normal prediction mode\n");
                    // }
                }

            }

#if 0 //shenh.move up
            CBP = (CBPY << 2) | (MCBPC >> 4);

            if ((Mode == MODE_INTER4V || Mode == MODE_INTER4V_Q) /*&& !use_4mv*/)
            {
                fault = 1;
                // if (!quiet)
                // {
                /* Could set fault-flag and resync */
                //   diag_printf ("8x8 vectors not allowed in normal prediction mode\n");
                // }
            }
#endif

            //if (Mode == MODE_INTER_Q || Mode == MODE_INTRA_Q || Mode == MODE_INTER4V_Q)
            if ((ModeBit&0x32)!=0)
            {
                /* Read DQUANT if necessary */
                /*
                  if (syntax_arith_coding)
                  {
                  DQUANT_index = decode_a_symbol (cumf_DQUANT);
                  DQUANT = dquanttab[DQUANT_index] - 2;
                  quant += DQUANT;
                  }
                  else
                  {
                */
                if (!modified_quantization_mode)
                {
                    DQUANT = getbits (2);//量化器信息
                    quant += DQ_tab[DQUANT];
                }
                else
                {
                    tmp = getbits (1);
                    if (tmp)
                    {
                        /* only one more additional bit was sent */
                        tmp = getbits (1);
                        if (tmp)
                        {
                            /* second bit of quant is 1 */
                            DQUANT = change_of_quant_tab_11[quant];
                        }
                        else
                        {
                            /* second bit of quant is 0 */
                            DQUANT = change_of_quant_tab_10[quant];
                        }
                        quant += DQUANT;
                    }
                    else
                    {
                        /* five additional bits were sent as
                         * DQUANT */
                        DQUANT = getbits (5);
                        quant = DQUANT;
                    }
                }
                //    }

                if (quant > 31 || quant < 1)
                {
                    //if (!quiet)
                    //  diag_printf ("Quantizer out of range: clipping\n");
                    quant = mmax (1, mmin (31, quant));
                    /* could set fault-flag and resync here */
                    fault = 1;
                }
            }

            /* motion vectors */
            //if (Mode == MODE_INTER || Mode == MODE_INTER_Q ||
            //  Mode == MODE_INTER4V || Mode == MODE_INTER4V_Q /*|| pb_frame*/)
            if ((ModeBit&0x27)!=0)
            {
                //if (Mode == MODE_INTER4V || Mode == MODE_INTER4V_Q)
                if ((ModeBit&0x24)!=0)
                {
                    startmv = 1;
                    stopmv = 4;
                }
                else
                {
                    startmv = 0;
                    stopmv = 0;
                }

                for (k = startmv; k <= stopmv; k++)
                {
                    /*
                      if (syntax_arith_coding)
                      {
                      mvx_index = decode_a_symbol (cumf_MVD);
                      mvx = mvdtab[mvx_index];
                      mvy_index = decode_a_symbol (cumf_MVD);
                      mvy = mvdtab[mvy_index];
                      }
                      else
                      {
                    */
                    if (!plus_type || ! long_vectors)
                    {
                        mvx = getTMNMV ();//横向运动矢量差值数据
                        mvy = getTMNMV ();

                        pmv0 = find_pmv (xpos, ypos, k, 0);//找到预测mv
                        if(errorstate)return;
                        pmv1 = find_pmv (xpos, ypos, k, 1);
                        if(errorstate)return;

                        mvx = motion_decode (mvx, pmv0);//由预测mv和mv差值获得真正mv
                        mvy = motion_decode (mvy, pmv1);
                    }
                    else
                    {
                        mvx = getRVLC ();
                        mvy = getRVLC ();

                        /* flush start code emulation bit */
                        if (mvx == 1 && mvy == 1)
                            flushbits(1);

                        pmv0 = find_pmv (xpos, ypos, k, 0);//找到预测mv
                        if(errorstate)return;
                        pmv1 = find_pmv (xpos, ypos, k, 1);
                        if(errorstate)return;

                        mvx += pmv0;
                        mvy += pmv1;
                    }
                    //  }

#if 0// shenh, move up
                    pmv0 = find_pmv (xpos, ypos, k, 0);//找到预测mv
                    if(errorstate)return;
                    pmv1 = find_pmv (xpos, ypos, k, 1);
                    if(errorstate)return;


                    if (plus_type && long_vectors /*&& !syntax_arith_coding*/)
                    {
                        mvx += pmv0;
                        mvy += pmv1;
                    }
                    else
                    {
                        mvx = motion_decode (mvx, pmv0);//由预测mv和mv差值获得真正mv
                        mvy = motion_decode (mvy, pmv1);
                    }
#endif

                    /* store coded or not-coded */
                    //coded_map[ypos + 1][xpos + 1] = 1;//此位置宏块有编码标志

                    /* Check mv's to prevent seg.faults when error rate is high */
                    // if (!mv_outside_frame)
                    // {
                    bsize = k ? 8 : 16;
                    offset = k ? (((k - 1) & 1) << 3) : 0;
                    /* checking only integer component */
                    tmp=(xpos << 4) + (mvx>>1) + offset;
                    if (tmp< 0 ||tmp > (mb_width << 4) - bsize)
                    {
                        // if (!quiet)
                        //  diag_printf ("mvx out of range: searching for sync\n");
                        fault = 1;
                    }

                    offset = k ? (((k - 1) & 2) << 2) : 0;
                    tmp=(ypos << 4) + (mvy>>1) + offset;
                    if (tmp< 0 ||tmp > (mb_height << 4) - bsize)
                    {
                        //  if (!quiet)
                        //    diag_printf ("mvy out of range: searching for sync\n");
                        fault = 1;
                    }
                    //  }

                    /*true_B_direct_mode_MV[0][k][ypos + 1][xpos + 1] =*/ // MV[0][k][ypos + 1][xpos + 1] = mvx;//x方向
                    tmp=(k*(MBR+1)+(ypos + 1))*(MBC+2)+(xpos + 1);
                    *(MV+tmp) =mvx;
                    /*true_B_direct_mode_MV[1][k][ypos + 1][xpos + 1] =*/  //MV[1][k][ypos + 1][xpos + 1] = mvy;//y方向
                    tmp+=6*(MBR+1)*(MBC+2);
                    *(MV+tmp) =mvy;
                }

                /* PB frame delta vectors */
                //        if (pb_frame)
                //        {
#if 0//change shenh
                if (((MODB == PBMODE_MVDB || MODB == PBMODE_CBPB_MVDB) && pb_frame == PB_FRAMES) ||
                        (pb_frame == IM_PB_FRAMES && (MODB == PBMODE_CBPB_FRW_PRED || MODB == PBMODE_FRW_PRED)))
                {
                    if (syntax_arith_coding)
                    {
                        mvdbx_index = decode_a_symbol (cumf_MVD);
                        mvdbx = mvdtab[mvdbx_index];
                        mvdby_index = decode_a_symbol (cumf_MVD);
                        mvdby = mvdtab[mvdby_index];
                    }
                    else
                    {
                        if (plus_type && long_vectors)
                        {
                            mvdbx = getRVLC ();
                            mvdby = getRVLC ();

                            /* flush start code emulation bit */
                            if (mvdbx == 1 && mvdby == 1)
                                flushbits(1);
                        }
                        else
                        {
                            mvdbx = getTMNMV ();
                            mvdby = getTMNMV ();
                        }
                    }

                    long_vectors_bak = long_vectors;

                    /* turn off long vectors when decoding forward motion vector
                     * of im.pb frames */
                    pmv0 = 0;
                    pmv1 = 0;
                    if (pb_frame == IM_PB_FRAMES &&
                            (MODB == PBMODE_CBPB_FRW_PRED || MODB == PBMODE_FRW_PRED))
                    {
                        long_vectors = 0;
                        if (MBA % mb_width && (pMODB == PBMODE_CBPB_FRW_PRED ||
                                               pMODB == PBMODE_FRW_PRED))
                        {
                            /* MBA%mb_width : if not new gob */
                            pmv0 = pmvdbx;
                            pmv1 = pmvdby;
                        }
                    }
                    if (plus_type && long_vectors && !syntax_arith_coding)
                    {
                        mvdbx += pmv0;
                        mvdby += pmv1;
                    }
                    else
                    {
                        mvdbx = motion_decode (mvdbx, pmv0);
                        mvdby = motion_decode (mvdby, pmv1);
                    }

                    long_vectors = long_vectors_bak;

                    /* This will not work if the PB deltas are so large they
                     * require the second colums of the motion vector VLC table to
                     * be used.  To fix this it is necessary to calculate the MV
                     * predictor for the PB delta: TRB*MV/TRD here, and use this
                     * as the second parameter to motion_decode(). The B vector
                     * itself will then be returned from motion_decode(). This
                     * will have to be changed to the PB delta again, since it is
                     * the PB delta which is used later */
                }
                else
                {
                    mvdbx = 0;
                    mvdby = 0;
                }
#endif
                //   errorstate=-1;
                //        return;
                //        }

            }
            /* Intra. */
#if 0
            else
            {
                /* Set MVs to 0 for potential future use in true B direct mode
                 * prediction. */
                /*
                  if (PCT_INTER == pict_type)
                  {
                  for (k=0; k<5; k++)
                  {
                  true_B_direct_mode_MV[0][k][ypos + 1][xpos + 1] = 0;
                  true_B_direct_mode_MV[1][k][ypos + 1][xpos + 1] = 0;
                  }
                  }*/

            }
#endif

            if (fault)
                goto resync;
        }
        else
        {
            /* COD == 1 --> skipped MB */
            if (MBA >= MBAmax)
            {
                /* all macroblocks decoded */
                //memcpy(anchorframemodemap,modemap,(sizeof(int)*(MBR+1)*(MBC+2)) );
                return;
            }
            //if (!syntax_arith_coding)
            if (PCT_INTER == pict_type /*|| PCT_IPB == pict_type*/)
                flushbits (1);

            Mode = MODE_INTER;
            ModeBit=0x01;

            /* Reset CBP */
            CBP = CBPB = 0;
            //coded_map[ypos + 1][xpos + 1] = 0;//此位置宏块未编码标志

            /* reset motion vectors */
            //MV[0][0][ypos + 1][xpos + 1] = 0;//x方向
            tmp=(ypos + 1)*(MBC+2)+(xpos + 1);
            *(MV+tmp) =0;
            //MV[1][0][ypos + 1][xpos + 1] = 0;//y方向
            tmp+=6*(MBR+1)*(MBC+2);
            *(MV+tmp) =0;

            /*
              if (PCT_INTER == pict_type)
              {
              for (k=0; k<5; k++)
              {
              true_B_direct_mode_MV[0][k][ypos + 1][xpos + 1] = 0;
              true_B_direct_mode_MV[1][k][ypos + 1][xpos + 1] = 0;
              }
              }
            */
            mvdbx = 0;
            mvdby = 0;
        }

        /* Store mode and prediction type */
        //modemap[ypos + 1][xpos + 1] = Mode;
        *(modemap+(ypos + 1)*(MBC+2)+(xpos + 1)) = Mode;

        /* store defaults for advanced intra coding mode */
        /*
          if (advanced_intra_coding)
          {
          for (i = 0; i < blk_cnt; i++)
          store_qcoeff[MBA * blk_cnt * 64 + i * 64] = 1024;
          }
        */

        //if (Mode == MODE_INTRA || Mode == MODE_INTRA_Q)
        if ((ModeBit&0x18)!=0)
        {
            //if (!pb_frame)
            //{
            //MV[0][0][ypos + 1][xpos + 1] = MV[1][0][ypos + 1][xpos + 1] = 0;
            tmp=(ypos + 1)*(MBC+2)+(xpos + 1);
            tmp2=tmp+6*(MBR+1)*(MBC+2);
            *(MV+tmp)= *(MV+tmp2) =0;
            //}
        }

reconstruct_mb:

        /* pixel coordinates of top left corner of current macroblock */
        /* one delayed because of OBMC */
        if (xpos > 0)
        {
            bx = (xpos - 1)<<4;
            by = ypos<<4;
        }
        else
        {
            bx = coded_picture_width - 16;
            by = (ypos - 1)<<4;
        }

        if (MBA > 0 /*&& !dont_reconstruct_next_mb*/)
        {
            //Mode = modemap[by / 16 + 1][bx / 16 + 1];
            Mode = *(modemap+((by>>4) + 1)*(MBC+2)+((bx>>4) + 1));
            switch(Mode)
            {
                case MODE_INTER:
                    ModeBit=0x01;
                    break;
                case MODE_INTER_Q:
                    ModeBit=0x02;
                    break;
                case MODE_INTER4V:
                    ModeBit=0x04;
                    break;
                case MODE_INTRA:
                    ModeBit=0x08;
                    break;
                case MODE_INTRA_Q:
                    ModeBit=0x10;
                    break;
                case MODE_INTER4V_Q:
                    ModeBit=0x20;
                    break;
                default:
                    ModeBit=0;
                    break;
            }

            /* forward motion compensation for B-frame */
            // if (pb_frame)
            //    {
#if 0//change shenh
            if (pCOD)
            {
                /* if the macroblock is not coded then it is bidir predicted */
                pMODB = PBMODE_BIDIR_PRED;
                reconstruct (bx, by, 0, pmvdbx, pmvdby, PBMODE_BIDIR_PRED, pnewgob);
            }
            else
            {
                if (!(pb_frame == IM_PB_FRAMES && (pMODB == PBMODE_CBPB_BCKW_PRED || pMODB == PBMODE_BCKW_PRED)))
                    reconstruct (bx, by, 0, pmvdbx, pmvdby, pMODB, pnewgob);
            }
            if(errorstate)return;
#endif
            // errorstate=-1;
            //     return;

            //   }

            /* motion compensation for P-frame 运动补偿*/
            //if (Mode == MODE_INTER || Mode == MODE_INTER_Q ||
            //  Mode == MODE_INTER4V || Mode == MODE_INTER4V_Q)
            if ((ModeBit&0x27)!=0)
            {
#ifdef  PROFILE_DEBUG
                hal_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_HAL, 0x29b5);
#endif

                reconstruct (bx, by, 1, 0, 0, pMODB, pnewgob);//按照运动矢量位置补偿出预测的像素
#ifdef  PROFILE_DEBUG
                hal_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_HAL, 0x29b5);
#endif
                if(errorstate)return;
            }
#if 0
            /* copy or add block data into P-picture */
            for (comp = 0; comp < blk_cnt; comp++)
            {
                /* inverse DCT */
                //if (Mode == MODE_INTRA || Mode == MODE_INTRA_Q)
                if ((ModeBit&0x18)!=0)
                {
                    //  if (refidct) //change shenh
                    //     idctref (ld->block[comp]);
                    //  else
                    hal_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_HAL, 0x29b2);
#ifdef USE_INTERNAL_SRAM
                    idct (ld->block+(comp<<6));
#else
                    idct (ld->block[comp]);
#endif
                    hal_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_HAL, 0x29b2);

                    hal_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_HAL, 0x29b3);
                    addblock (comp, bx, by, 0);//H.263 帧内未使用空间预测，idct变换后直接就为图像数据而非残差。
                    hal_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_HAL, 0x29b3);

                    if(errorstate)return;
                }
                else if ((pCBP & (1 << (blk_cnt - 1 - comp))))
                {
                    /* No need to to do this for blocks with no coeffs */
                    // if (refidct) //change shenh
                    //  idctref (ld->block[comp]);
                    // else
                    hal_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_HAL, 0x29b2);
#ifdef USE_INTERNAL_SRAM
                    idct (ld->block+(comp<<6));
#else
                    idct (ld->block[comp]);//计算出差值数据
#endif
                    hal_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_HAL, 0x29b2);

                    hal_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_HAL, 0x29b3);
                    addblock (comp, bx, by, 1);//差值数据加到运动补偿像素上
                    hal_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_HAL, 0x29b3);
                    if(errorstate)return;
                }
            }
#endif

            if ((ModeBit&0x18)!=0)
            {
                /* copy or add block data into P-picture */
#if 0
                for (comp = 0; comp < blk_cnt; comp++)
                {
                    /* inverse DCT */
                    //  if (refidct) //change shenh
                    //     idctref (ld->block[comp]);
                    //  else
#ifdef  PROFILE_DEBUG
                    hal_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_HAL, 0x29b2);
#endif
#ifdef USE_INTERNAL_SRAM
                    idct (ld->block+(comp<<6));
#else
                    idct (ld->block[comp]);
#endif
#ifdef  PROFILE_DEBUG
                    hal_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_HAL, 0x29b2);


                    hal_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_HAL, 0x29b3);
#endif
                    addblock (comp, bx, by, 0);//H.263 帧内未使用空间预测，idct变换后直接就为图像数据而非残差。
#ifdef  PROFILE_DEBUG
                    hal_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_HAL, 0x29b3);
#endif

                    if(errorstate)return;
                }
#endif
                //mips asm idct
                uint8* dest;
                int32 pos;

                dest=current_frame[0] + coded_picture_width * by + bx;
                simple_idct_put(dest, coded_picture_width, ld->block);
                simple_idct_put(dest+8, coded_picture_width, ld->block+64);
                simple_idct_put(dest+(coded_picture_width<<3), coded_picture_width, ld->block+128);
                simple_idct_put(dest+(coded_picture_width<<3)+8, coded_picture_width, ld->block+192);
                pos= chrom_width * (by>>1) + (bx>>1);
                simple_idct_put(current_frame[1]+pos, chrom_width, ld->block+256);
                simple_idct_put(current_frame[2]+pos, chrom_width, ld->block+320);

            }
            else
            {
                /* copy or add block data into P-picture */
#if 0
                tmp=blk_cnt - 1 ;
                for (comp = 0; comp < blk_cnt; comp++)
                {
                    /* inverse DCT */
                    if ((pCBP & (1 << (tmp - comp))))
                    {
                        /* No need to to do this for blocks with no coeffs */
                        // if (refidct) //change shenh
                        //  idctref (ld->block[comp]);
                        // else
#ifdef  PROFILE_DEBUG
                        hal_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_HAL, 0x29b2);
#endif
#ifdef USE_INTERNAL_SRAM
                        idct (ld->block+(comp<<6));
#else
                        idct (ld->block[comp]);//计算出差值数据
#endif
#ifdef  PROFILE_DEBUG
                        hal_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_HAL, 0x29b2);

                        hal_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_HAL, 0x29b3);
#endif
                        addblock (comp, bx, by, 1);//差值数据加到运动补偿像素上
#ifdef  PROFILE_DEBUG
                        hal_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_HAL, 0x29b3);
#endif

                        //simple_idct_add(uint8_t *dest, int line_size, DCTELEM *block)

                        if(errorstate)return;
                    }
                }
#endif
                //mips asm idct
                uint8* dest;
                int32 pos;
                dest=current_frame[0] + coded_picture_width * by + bx;
                if (pCBP & 0x20)
                    simple_idct_add(dest, coded_picture_width, ld->block);
                if (pCBP & 0x10)
                    simple_idct_add(dest+8, coded_picture_width, ld->block+64);
                if (pCBP & 0x08)
                    simple_idct_add(dest+(coded_picture_width<<3), coded_picture_width, ld->block+128);
                if (pCBP & 0x04)
                    simple_idct_add(dest+(coded_picture_width<<3)+8, coded_picture_width, ld->block+192);
                pos= chrom_width * (by>>1) + (bx>>1);
                if (pCBP & 0x02)
                    simple_idct_add(current_frame[1]+pos, chrom_width, ld->block+256);
                if (pCBP & 0x01)
                    simple_idct_add(current_frame[2]+pos, chrom_width, ld->block+320);
            }

            //     if (pb_frame)
            //      {
#if 0//change shenh
            if (pMODB == PBMODE_CBPB_BCKW_PRED || pMODB == PBMODE_BCKW_PRED)
            {
                reconstruct (bx, by, 0, 0, 0, pMODB, pnewgob);
                if(errorstate)return;
            }
            /* add block data into B-picture */
            for (comp = 6; comp < blk_cnt + 6; comp++)
            {
                if (!pCOD || adv_pred_mode)
                {
                    if (pb_frame == IM_PB_FRAMES)
                    {
                        if (pMODB == PBMODE_CBPB_BIDIR_PRED || pMODB == PBMODE_BIDIR_PRED || pCOD)
                        {
                            reconblock_b (comp - 6, bx, by, Mode, 0, 0);
                        }
                    }
                    else
                    {
                        reconblock_b (comp - 6, bx, by, Mode, pmvdbx, pmvdby);
                    }
                }
                if ((pCBPB & (1 << (blk_cnt - 1 - comp % 6))))
                {
                    //  if (refidct) //change shenh
                    //    idctref (ld->block[comp]);
                    //   else
                    idct (ld->block[comp]);
                    addblock (comp, bx, by, 1);
                }
            }
#endif
            //   errorstate=-1;
            //     return;

            //   }

        }

        /* end if (MBA > 0) */
        if (!COD)
        {
            // Mode = modemap[ypos + 1][xpos + 1];
            Mode = *(modemap+(ypos + 1)*(MBC+2)+(xpos + 1));
            switch(Mode)
            {
                case MODE_INTER:
                    ModeBit=0x01;
                    break;
                case MODE_INTER_Q:
                    ModeBit=0x02;
                    break;
                case MODE_INTER4V:
                    ModeBit=0x04;
                    break;
                case MODE_INTRA:
                    ModeBit=0x08;
                    break;
                case MODE_INTRA_Q:
                    ModeBit=0x10;
                    break;
                case MODE_INTER4V_Q:
                    ModeBit=0x20;
                    break;
                default:
                    ModeBit=0;
                    break;
            }
#if 0
            /* decode blocks */
            for (comp = 0; comp < blk_cnt; comp++)
            {
                clearblock (comp);//清空ld->block中idct数据

                //if ((Mode == MODE_INTRA || Mode == MODE_INTRA_Q) /*&& !(advanced_intra_coding)*/)
                if ((ModeBit&0x18)!=0)
                {
                    /* Intra (except in advanced intra coding mode) */

#ifdef USE_INTERNAL_SRAM
                    bp = ld->block+(comp<<6);
#else
                    bp = ld->block[comp];
#endif
                    /*
                      if (syntax_arith_coding)
                      {
                      INTRADC_index = decode_a_symbol (cumf_INTRADC);
                      bp[0] = intradctab[INTRADC_index];
                      }
                      else
                      {
                    */
                    bp[0] = getbits (8);//帧内直流系数，帧间没有此项。
                    //}

                    // if (bp[0] == 128)
                    //  if (!quiet)
                    //    fprintf (stderr, "Illegal DC-coeff: 1000000\n");
                    if (bp[0] == 255)   /* Spec. in H.26P, not in TMN4 */
                        bp[0] = 128;
                    bp[0] <<= 3;         /* Iquant 重建直流系数*/
                    if ((CBP & (1 << (blk_cnt - 1 - comp))))
                    {
                        // if (!syntax_arith_coding)
                        hal_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_HAL, 0x29b4);

                        getblock (comp, 0, 0, Mode);//vlc huffman解码、反量化，得到一块的dct系数

                        hal_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_HAL, 0x29b4);
                        // else
                        //   get_sac_block (comp, 0, 0, Mode);

                        if(errorstate)return;

                    }
                }
                else
                {
                    /* Inter (or Intra in advanced intra coding mode) */
                    if ((CBP & (1 << (blk_cnt - 1 - comp))))
                    {
                        // if (!syntax_arith_coding)
                        hal_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_HAL, 0x29b4);

                        getblock (comp, 1, 0/*INTRA_AC_DC*/, Mode);

                        hal_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_HAL, 0x29b4);
                        // else
                        //    get_sac_block (comp, 1, INTRA_AC_DC, Mode);

                        if(errorstate)return;
                    }
                }

                /*
                  if ((advanced_intra_coding) && (Mode == MODE_INTRA || Mode == MODE_INTRA_Q))
                  {
                  Intra_AC_DC_Decode (store_qcoeff, INTRA_AC_DC, MBA, xpos, ypos, comp, newgob);
                  if(errorstate)return;
                  memcpy ((void *) (store_qcoeff + MBA * blk_cnt * 64 + comp * 64),
                  (void *) ld->block[comp], sizeof (short) * 64);
                  }
                */

                if (fault)
                    goto resync;
            }
#endif

            if ((ModeBit&0x18)!=0)
            {
                /* decode blocks */
                tmp=blk_cnt - 1;
                for (comp = 0; comp < blk_cnt; comp++)
                {
                    clearblock (comp);//清空ld->block中idct数据

                    /* Intra (except in advanced intra coding mode) */

#ifdef USE_INTERNAL_SRAM
                    bp = ld->block+(comp<<6);
#else
                    //bp = ld->block[comp];
                    bp = ld->block+(comp<<6);
#endif
                    /*
                      if (syntax_arith_coding)
                      {
                      INTRADC_index = decode_a_symbol (cumf_INTRADC);
                      bp[0] = intradctab[INTRADC_index];
                      }
                      else
                      {
                    */
                    bp[0] = getbits (8);//帧内直流系数，帧间没有此项。
                    //}

                    // if (bp[0] == 128)
                    //  if (!quiet)
                    //    fprintf (stderr, "Illegal DC-coeff: 1000000\n");
                    if (bp[0] == 255)   /* Spec. in H.26P, not in TMN4 */
                        bp[0] = 128;
                    bp[0] <<= 3;         /* Iquant 重建直流系数*/
                    if ((CBP & (1 << (tmp - comp))))
                    {
                        // if (!syntax_arith_coding)
#ifdef  PROFILE_DEBUG
                        hal_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_HAL, 0x29b4);
#endif

                        getblock (comp, 0, 0, Mode);//vlc huffman解码、反量化，得到一块的dct系数
#ifdef  PROFILE_DEBUG
                        hal_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_HAL, 0x29b4);
#endif
                        // else
                        //   get_sac_block (comp, 0, 0, Mode);

                        if(errorstate)return;

                    }

                    /*
                      if ((advanced_intra_coding) && (Mode == MODE_INTRA || Mode == MODE_INTRA_Q))
                      {
                      Intra_AC_DC_Decode (store_qcoeff, INTRA_AC_DC, MBA, xpos, ypos, comp, newgob);
                      if(errorstate)return;
                      memcpy ((void *) (store_qcoeff + MBA * blk_cnt * 64 + comp * 64),
                      (void *) ld->block[comp], sizeof (short) * 64);
                      }
                    */

                    if (fault)
                        goto resync;
                }
            }
            else
            {
                /* decode blocks */
                tmp=blk_cnt - 1;
                for (comp = 0; comp < blk_cnt; comp++)
                {
                    clearblock (comp);//清空ld->block中idct数据

                    /* Inter (or Intra in advanced intra coding mode) */
                    if ((CBP & (1 << (tmp - comp))))
                    {
                        // if (!syntax_arith_coding)
#ifdef  PROFILE_DEBUG
                        hal_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_HAL, 0x29b4);
#endif

                        getblock (comp, 1, 0/*INTRA_AC_DC*/, Mode);
#ifdef  PROFILE_DEBUG
                        hal_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_HAL, 0x29b4);
#endif
                        // else
                        //    get_sac_block (comp, 1, INTRA_AC_DC, Mode);

                        if(errorstate)return;
                    }

                    /*
                      if ((advanced_intra_coding) && (Mode == MODE_INTRA || Mode == MODE_INTRA_Q))
                      {
                      Intra_AC_DC_Decode (store_qcoeff, INTRA_AC_DC, MBA, xpos, ypos, comp, newgob);
                      if(errorstate)return;
                      memcpy ((void *) (store_qcoeff + MBA * blk_cnt * 64 + comp * 64),
                      (void *) ld->block[comp], sizeof (short) * 64);
                      }
                    */

                    if (fault)
                        goto resync;
                }
            }

            /* Decode B blocks */
            //   if (pb_frame)
            //  {
#if 0//change shenh
            for (comp = 6; comp < blk_cnt + 6; comp++)
            {
                clearblock (comp);
                if ((CBPB & (1 << (blk_cnt - 1 - comp % 6))))
                {
                    if (!syntax_arith_coding)
                        getblock (comp, 1, 0, MODE_INTER);
                    else
                        get_sac_block (comp, 1, 0, MODE_INTER);

                    if(errorstate)return;
                }
                if (fault)
                    goto resync;
            }
#endif
            // errorstate=-1;
            //   return;

            // }

        }

        //conceal_gob:

        /* decode the last MB if data is missing */
        /*
          if (decode_last_mb)
          {
          conceal_missing_gobs(start_mb_row_missing, number_of_mb_rows_missing);
          if(errorstate)return;
          // all macroblocks in the picture are done, return
          //if the first gob in the next frame is also missing,
          // we will also lose the secon gob of that next frame.
          // This can be dealt with, but we will live with that for now.
          if ( (number_of_mb_rows_missing + start_mb_row_missing) * mb_width >= MBAmax) return;
          ypos = gob;
          decode_last_mb = 0;
          dont_reconstruct_next_mb = 1;
          goto finish_gob;
          }
          else
          {
        */
        /* advance to next macroblock */
        MBA++;
        pCBP = CBP;
        pCBPB = CBPB;
        pCOD = COD;
        pMODB = MODB;
        //quant_map[ypos + 1][xpos + 1] = quant;

        pmvdbx = mvdbx;
        pmvdby = mvdby;
        //fflush (stdout); //change shenh
        pnewgob = newgob;

        if (MBA >= MBAmax && !last_done)
        {
            COD = 1;
            xpos = 0;
            ypos++;
            last_done = 1;
            goto reconstruct_mb;
        }
        // }

    }

    //hal_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_HAL, 0x29b0);// for profile

}

/**********************************************************************
 *
 *  Name:             get_B_MBs
 *  Description:  decode MBs for one B picture
 *  Input:        frame number
 *  Returns:
 *  Side effects:
 *
 *  Date: 971102  Author: mikeg@ee.ubc.ca
 *
 *
 ***********************************************************************/
#if 0//change shenh
static void get_B_MBs (int framenum)
{
    int comp;
    int MBA, MBAmax;
    int bx, by;
    int COD = 0, CBP = 0, Mode = 0, DQUANT;
    int xpos, ypos, gob, i;
    int mvfx = 0, mvfy = 0, mvbx = 0, mvby = 0, pmvf0, pmvf1, pmvb0, pmvb1;
    int gfid, gobheader_read;
    int last_done = 0, pCBP = 0, pCOD = 0;
    int DQ_tab[4] = {-1, -2, 1, 2};
    short *bp;
    int true_B_cbp = 0, true_B_quant = 0, true_B_prediction_type;
    int CBPC = 0, CBPY = 0, tmp = 0;

    /* variables used in advanced intra coding mode */
    int INTRA_AC_DC = 0;
    short *store_qcoeff;

    errorstate=0;

    /* number of macroblocks per picture */
    MBAmax = mb_width * mb_height;

    MBA = 0;                      /* macroblock address */
    newgob = 0;

    /* mark MV's above the picture */
    for (i = 1; i < mb_width + 1; i++)
    {
        MV[0][0][0][i] = NO_VEC;
        MV[1][0][0][i] = NO_VEC;
        MV[0][5][0][i] = NO_VEC;
        MV[1][5][0][i] = NO_VEC;
        modemap[0][i] = MODE_INTRA;
        predictionmap[0][i] = B_INTRA_PREDICTION;
    }
    /* zero MV's on the sides of the picture */
    for (i = 0; i < mb_height + 1; i++)
    {
        MV[0][0][i][0] = 0;
        MV[1][0][i][0] = 0;
        MV[0][0][i][mb_width + 1] = 0;
        MV[1][0][i][mb_width + 1] = 0;

        MV[0][5][i][0] = 0;
        MV[1][5][i][0] = 0;
        MV[0][5][i][mb_width + 1] = 0;
        MV[1][5][i][mb_width + 1] = 0;

        modemap[i][0] = MODE_INTRA;
        modemap[i][mb_width + 1] = MODE_INTRA;
        predictionmap[i][0] = B_INTRA_PREDICTION;
        predictionmap[i][mb_width + 1] = B_INTRA_PREDICTION;
    }
    /* initialize the qcoeff used in advanced intra coding */
    if (advanced_intra_coding)
    {
        /* store the qcoeff for the frame */
        /* change shenh
           if ((store_qcoeff = (short *) calloc (64 * MBAmax * blk_cnt, sizeof (short))) == 0)
           {
           fprintf (stderr, "getMB(): Couldn't allocate store_qcoeff.\n");
           exit (-1);
           }*/
        //fprintf (stderr, "advanced_intra_coding is not support for calloc.\n");
//        diag_printf ( "advanced_intra_coding is not support for calloc.\n");
        //exit (-1);
        errorstate=-1;
        return;
    }
    fault = 0;
    gobheader_read = 0;

    for (;;)
    {


resync:
        /* This version of the decoder does not resync on every possible
         * error, and it does not do all possible error checks. It is not
         * difficult to make it much more error robust, but I do not think it
         * is necessary to include this in the freely available version. */

        if (fault)
        {
//            diag_printf ("Warning: A Fault Condition Has Occurred - Resyncing \n");
            startcode ();             /* sync on new startcode */
            fault = 0;
        }
        if (!(showbits (22) >> 6))
        {
            /* startcode */
            startcode ();

            /* in case of byte aligned start code, ie. PSTUF, GSTUF or ESTUF is
             * used */
            if (showbits (22) == (32 | SE_CODE))
            {
                /* end of sequence */
                if (!(MBA < MBAmax))
                {
                    return;
                }
            }
            else if ((showbits (22) == PSC << 5))
            {
                /* new picture */
                if (!(MBA < MBAmax))
                {
                    return;
                }
            }
            else
            {
                if (!(MBA % mb_width))
                {
                    gob = getheader () - 1;
                    if(errorstate)return;
                    if (gob > mb_height)
                    {
//                        if (!quiet)
//                            diag_printf ("GN out of range\n");
                        return;
                    }
                    /* GFID is not allowed to change unless PTYPE in picture header
                     * changes */
                    gfid = getbits (2);
                    /* NB: in error-prone environments the decoder can use this
                     * value to determine whether a picture header where the PTYPE
                     * has changed, has been lost */

                    quant = getbits (5);
                    xpos = 0;
                    ypos = gob;
                    MBA = ypos * mb_width;

                    newgob = 1;
                    gobheader_read = 1;
                }
            }
        }

        /* SAC specific label */
        if (!gobheader_read)
        {
            xpos = MBA % mb_width;
            ypos = MBA / mb_width;
            if (xpos == 0 && ypos > 0)
                newgob = 0;
        }
        else
            gobheader_read = 0;

        if (MBA >= MBAmax)
        {
            /* all macroblocks decoded */
            return;
        }
read_cod:

        COD = showbits (1);

        if (!COD)
        {
            /* COD == 0 --> not skipped */
            coded_map[ypos + 1][xpos + 1] = 1;

            /* flush COD bit */
            flushbits (1);

            true_B_prediction_type = getMBTYPE (&true_B_cbp, &true_B_quant);

            if (fault)
                goto resync;

            if (B_EI_EP_STUFFING == true_B_prediction_type)
            {
                /* stuffing - read next COD without advancing MB count. */
                goto read_cod;
            }

            if (B_INTRA_PREDICTION != true_B_prediction_type)
            {
                if (1 == true_B_quant)
                {
                    Mode = MODE_INTER_Q;
                }
                else
                {
                    Mode = MODE_INTER;
                }
            }
            else
            {
                if (1 == true_B_quant)
                {
                    Mode = MODE_INTRA_Q;
                }
                else
                {
                    Mode = MODE_INTRA;
                }
            }

            if (advanced_intra_coding && (B_INTRA_PREDICTION == true_B_prediction_type))
            {
                /* get INTRA_AC_DC mode for annex I */
                if (!showbits (1))
                    INTRA_AC_DC = getbits (1);
                else
                    INTRA_AC_DC = getbits (2);

            }
            if (1 == true_B_cbp)
            {
                CBPC = getscalabilityCBPC ();
                CBPY = getCBPY ();

                /* Decode Mode and CBP */
                if (Mode == MODE_INTRA || Mode == MODE_INTRA_Q)
                {
                    /* Intra */

                    /* needed in huffman coding only */
                    CBPY = CBPY ^ 15;
                }
                /* Annex S.3 change */
                else if (alternative_inter_VLC_mode && (CBPC == 3) )
                    CBPY = CBPY ^ 15;

                CBP = (CBPY << 2) | CBPC;
            }
            else
            {
                CBP = 0;
            }

            if (fault)
                goto resync;

            if (Mode == MODE_INTER_Q || Mode == MODE_INTRA_Q )
            {
                /* Read DQUANT if necessary */
                if (true_B_frame && true_B_quant)
                {
                    if (!modified_quantization_mode)
                    {
                        DQUANT = getbits (2);
                        quant += DQ_tab[DQUANT];
                    }
                    else
                    {
                        tmp = getbits (1);
                        if (tmp)
                        {
                            /* only one more additional bit was sent */
                            tmp = getbits (1);
                            if (tmp)
                            {
                                /* second bit of quant is 1 */
                                DQUANT = change_of_quant_tab_11[quant];
                            }
                            else
                            {
                                /* second bit of quant is 0 */
                                DQUANT = change_of_quant_tab_10[quant];
                            }
                            quant += DQUANT;
                        }
                        else
                        {
                            /* five additional bits were sent as
                             * DQUANT */
                            DQUANT = getbits (5);
                            quant = DQUANT;
                        }
                    }
                }
                if (quant > 31 || quant < 1)
                {
                    //if (!quiet)
                    // diag_printf ("Quantizer out of range: clipping\n");
                    quant = mmax (1, mmin (31, quant));
                    /* could set fault-flag and resync here */
                }
            }

            /* motion vectors */
            if (Mode == MODE_INTER || Mode == MODE_INTER_Q )
            {
                switch (true_B_prediction_type)
                {
                    case B_FORWARD_PREDICTION:

                        if (plus_type && long_vectors)
                        {
                            mvfx = getRVLC ();
                            mvfy = getRVLC ();

                            /* flush start code emulation bit */
                            if (mvfx == 1 && mvfy == 1)
                                flushbits(1);
                        }
                        else
                        {
                            mvfx = getTMNMV ();
                            mvfy = getTMNMV ();
                        }

                        pmvf0 = find_pmv (xpos, ypos, 0, 0);
                        if(errorstate)return;
                        pmvf1 = find_pmv (xpos, ypos, 0, 1);
                        if(errorstate)return;

                        if (plus_type && long_vectors)
                        {
                            mvfx += pmvf0;
                            mvfy += pmvf1;
                        }
                        else
                        {
                            mvfx = motion_decode (mvfx, pmvf0);
                            mvfy = motion_decode (mvfy, pmvf1);
                        }


                        MV[0][0][ypos + 1][xpos + 1] = mvfx;
                        MV[1][0][ypos + 1][xpos + 1] = mvfy;

                        MV[0][5][ypos + 1][xpos + 1] = 0;
                        MV[1][5][ypos + 1][xpos + 1] = 0;

                        break;

                    case B_BACKWARD_PREDICTION:

                        if (plus_type && long_vectors)
                        {
                            mvbx = getRVLC ();
                            mvby = getRVLC ();

                            /* flush start code emulation bit */
                            if (mvbx == 1 && mvby == 1)
                                flushbits(1);
                        }
                        else
                        {
                            mvbx = getTMNMV ();
                            mvby = getTMNMV ();
                        }

                        pmvb0 = find_pmv (xpos, ypos, 5, 0);
                        if(errorstate)return;
                        pmvb1 = find_pmv (xpos, ypos, 5, 1);
                        if(errorstate)return;

                        if (plus_type && long_vectors)
                        {
                            mvbx += pmvb0;
                            mvby += pmvb1;
                        }
                        else
                        {
                            mvbx = motion_decode (mvbx, pmvb0);
                            mvby = motion_decode (mvby, pmvb1);
                        }


                        MV[0][5][ypos + 1][xpos + 1] = mvbx;
                        MV[1][5][ypos + 1][xpos + 1] = mvby;

                        MV[0][0][ypos + 1][xpos + 1] = 0;
                        MV[1][0][ypos + 1][xpos + 1] = 0;

                        break;

                    case B_BIDIRECTIONAL_PREDICTION:

                        if (plus_type && long_vectors)
                        {
                            mvfx = getRVLC ();
                            mvfy = getRVLC ();

                            /* flush start code emulation bit */
                            if (mvfx == 1 && mvfy == 1)
                                flushbits(1);
                        }
                        else
                        {
                            mvfx = getTMNMV ();
                            mvfy = getTMNMV ();
                        }

                        pmvf0 = find_pmv (xpos, ypos, 0, 0);
                        if(errorstate)return;
                        pmvf1 = find_pmv (xpos, ypos, 0, 1);
                        if(errorstate)return;

                        if (plus_type && long_vectors)
                        {
                            mvfx += pmvf0;
                            mvfy += pmvf1;
                        }
                        else
                        {
                            mvfx = motion_decode (mvfx, pmvf0);
                            mvfy = motion_decode (mvfy, pmvf1);
                        }


                        if (plus_type && long_vectors)
                        {
                            mvbx = getRVLC ();
                            mvby = getRVLC ();

                            /* flush start code emulation bit */
                            if (mvbx == 1 && mvby == 1)
                                flushbits(1);
                        }
                        else
                        {
                            mvbx = getTMNMV ();
                            mvby = getTMNMV ();
                        }

                        pmvb0 = find_pmv (xpos, ypos, 5, 0);
                        if(errorstate)return;
                        pmvb1 = find_pmv (xpos, ypos, 5, 1);
                        if(errorstate)return;

                        if (plus_type && long_vectors)
                        {
                            mvbx += pmvb0;
                            mvby += pmvb1;
                        }
                        else
                        {
                            mvbx = motion_decode (mvbx, pmvb0);
                            mvby = motion_decode (mvby, pmvb1);
                        }


                        MV[0][0][ypos + 1][xpos + 1] = mvfx;
                        MV[1][0][ypos + 1][xpos + 1] = mvfy;

                        MV[0][5][ypos + 1][xpos + 1] = mvbx;
                        MV[1][5][ypos + 1][xpos + 1] = mvby;

                        break;

                    default:

                        /* No MV data for other modes. */
                        MV[0][0][ypos + 1][xpos + 1] = 0;
                        MV[1][0][ypos + 1][xpos + 1] = 0;

                        MV[0][5][ypos + 1][xpos + 1] = 0;
                        MV[1][5][ypos + 1][xpos + 1] = 0;

                        break;
                }
            }

            if (fault)
                goto resync;
        }
        else
        {
            /* COD == 1 --> skipped MB */
            if (MBA >= MBAmax)
            {
                /* all macroblocks decoded */
                return;
            }
            flushbits (1);

            Mode = MODE_INTER;

            /* Reset CBP */
            CBP = 0;

            coded_map[ypos + 1][xpos + 1] = 0;

            true_B_prediction_type = B_DIRECT_PREDICTION;

            /* reset motion vectors */
            MV[0][0][ypos + 1][xpos + 1] = 0;
            MV[1][0][ypos + 1][xpos + 1] = 0;
            MV[0][5][ypos + 1][xpos + 1] = 0;
            MV[1][5][ypos + 1][xpos + 1] = 0;
        }

        /* Store mode and prediction type */
        modemap[ypos + 1][xpos + 1] = Mode;
        predictionmap[ypos + 1][xpos + 1] = true_B_prediction_type;

        /* store defaults for advanced intra coding mode */
        if (advanced_intra_coding)
        {
            for (i = 0; i < blk_cnt; i++)
                store_qcoeff[MBA * blk_cnt * 64 + i * 64] = 1024;
        }
        if (Mode == MODE_INTRA || Mode == MODE_INTRA_Q)
        {
            MV[0][0][ypos + 1][xpos + 1] = MV[1][0][ypos + 1][xpos + 1] = 0;
            MV[0][5][ypos + 1][xpos + 1] = MV[1][5][ypos + 1][xpos + 1] = 0;
        }

reconstruct_mb:

        /* pixel coordinates of top left corner of current macroblock */
        /* one delayed because of OBMC */
        if (xpos > 0)
        {
            bx = 16 * (xpos - 1);
            by = 16 * ypos;
        }
        else
        {
            bx = coded_picture_width - 16;
            by = 16 * (ypos - 1);
        }

        if (MBA > 0)
        {
            Mode = modemap[by / 16 + 1][bx / 16 + 1];
            true_B_prediction_type = predictionmap[by / 16 + 1][bx / 16 + 1];

            /* motion compensation for true B frame. */
            if (Mode == MODE_INTER || Mode == MODE_INTER_Q)
                reconstruct_true_B (bx, by, true_B_prediction_type);

            if(errorstate)return;

            /* copy or add block data into true B picture */
            for (comp = 0; comp < blk_cnt; comp++)
            {
                /* inverse DCT */
                if (Mode == MODE_INTRA || Mode == MODE_INTRA_Q)
                {
                    //  if (refidct) //change shenh
                    //    idctref (ld->block[comp]);
                    //   else
                    idct (ld->block[comp]);
                    addblock (comp, bx, by, 0);
                }
                else if ((pCBP & (1 << (blk_cnt - 1 - comp))))
                {
                    /* No need to to do this for blocks with no coeffs */
                    // if (refidct)
                    //  idctref (ld->block[comp]);
                    //  else
                    idct (ld->block[comp]);
                    addblock (comp, bx, by, 1);
                }
            }
        }
        /* end if (MBA > 0) */
        if (!COD)
        {
            Mode = modemap[ypos + 1][xpos + 1];
            true_B_prediction_type = predictionmap[ypos + 1][xpos + 1];

            for (comp = 0; comp < blk_cnt; comp++)
            {
                clearblock (comp);
                if ((Mode == MODE_INTRA || Mode == MODE_INTRA_Q) && !(advanced_intra_coding))
                {
                    /* Intra (except in advanced intra coding mode) */
                    bp = ld->block[comp];
                    bp[0] = getbits (8);
                    // if (bp[0] == 128)
                    //   if (!quiet)
                    //    fprintf (stderr, "Illegal DC-coeff: 1000000\n");
                    if (bp[0] == 255)   /* Spec. in H.26P, not in TMN4 */
                        bp[0] = 128;
                    bp[0] *= 8;         /* Iquant */

                    if ((CBP & (1 << (blk_cnt - 1 - comp))))
                    {
                        getblock (comp, 0, 0, Mode);
                        if(errorstate)return;
                    }
                }
                else
                {
                    /* Inter (or Intra in advanced intra coding mode) */
                    if ((CBP & (1 << (blk_cnt - 1 - comp))))
                    {
                        getblock (comp, 1, INTRA_AC_DC, Mode);
                        if(errorstate)return;
                    }
                }
                if ((advanced_intra_coding) && (Mode == MODE_INTRA || Mode == MODE_INTRA_Q))
                {
                    Intra_AC_DC_Decode (store_qcoeff, INTRA_AC_DC, MBA, xpos, ypos, comp, newgob);
                    if(errorstate)return;
                    memcpy ((void *) (store_qcoeff + MBA * blk_cnt * 64 + comp * 64),
                            (void *) ld->block[comp], sizeof (short) * 64);

                }
                if (fault)
                    goto resync;
            }
        }

        /* advance to next macroblock */
        MBA++;
        pCBP = CBP;
        pCOD = COD;
        quant_map[ypos + 1][xpos + 1] = quant;

        //fflush (stdout);//change shenh

        if (MBA >= MBAmax && !last_done)
        {
            COD = 1;
            xpos = 0;
            ypos++;
            last_done = 1;
            goto reconstruct_mb;
        }
    }
}
#endif

/**********************************************************************
 *
 *  Name:             get_EI_EP_MBs
 *  Description:  decode MBs for one EI or EP picture
 *  Input:        frame number
 *  Returns:
 *  Side effects:
 *
 *  Date: 971102  Author: mikeg@ee.ubc.ca
 *
 ***********************************************************************/
#if 0//shenh change
static void get_EI_EP_MBs (int framenum)
{
    int comp;
    int MBA, MBAmax;
    int bx, by;
    int COD = 0, CBP = 0, Mode = 0, DQUANT;
    int xpos, ypos, gob, i;
    int mvfx = 0, mvfy = 0, pmvf0, pmvf1;
    int gfid, gobheader_read;
    int last_done = 0, pCBP = 0, pCOD = 0;
    int DQ_tab[4] = {-1, -2, 1, 2};
    short *bp;
    int ei_ep_cbp = 0, ei_ep_quant = 0, ei_ep_prediction_type;
    int CBPC = 0, CBPY = 0, tmp = 0;

    /* variables used in advanced intra coding mode */
    int INTRA_AC_DC = 0;
    short *store_qcoeff;

    errorstate=0;

    MBAmax = mb_width * mb_height;

    MBA = 0;                      /* macroblock address */
    newgob = 0;

    /* mark MV's above the picture */
    for (i = 1; i < mb_width + 1; i++)
    {
        MV[0][0][0][i] = NO_VEC;
        MV[1][0][0][i] = NO_VEC;
        modemap[0][i] = MODE_INTRA;
        predictionmap[0][i] = EI_EP_INTRA_PREDICTION;
    }
    /* zero MV's on the sides of the picture */
    for (i = 0; i < mb_height + 1; i++)
    {
        MV[0][0][i][0] = 0;
        MV[1][0][i][0] = 0;
        MV[0][0][i][mb_width + 1] = 0;
        MV[1][0][i][mb_width + 1] = 0;
        modemap[i][0] = MODE_INTRA;
        modemap[i][mb_width + 1] = MODE_INTRA;
        predictionmap[i][0] = EI_EP_INTRA_PREDICTION;
        predictionmap[i][mb_width + 1] = EI_EP_INTRA_PREDICTION;
    }
    /* initialize the qcoeff used in advanced intra coding */
    if (advanced_intra_coding)
    {
        /* store the qcoeff for the frame */
        /* change shenh
           if ((store_qcoeff = (short *) calloc (64 * MBAmax * blk_cnt, sizeof (short))) == 0)
           {
           fprintf (stderr, "getMB(): Couldn't allocate store_qcoeff.\n");
           exit (-1);
           }*/
        //fprintf (stderr, "advanced_intra_coding is not support for calloc.\n");
//        diag_printf ( "advanced_intra_coding is not support for calloc.\n");
        //exit (-1);
        errorstate=-1;
        return;
    }
    fault = 0;
    gobheader_read = 0;

    for (;;)
    {


resync:
        /* This version of the decoder does not resync on every possible
         * error, and it does not do all possible error checks. It is not
         * difficult to make it much more error robust, but I do not think it
         * is necessary to include this in the freely available version. */

        if (fault)
        {
//            diag_printf ("Warning: A Fault Condition Has Occurred - Resyncing \n");
            startcode ();             /* sync on new startcode */
            fault = 0;
        }
        if (!(showbits (22) >> 6))
        {
            /* startcode */
            startcode ();

            /* in case of byte aligned start code, ie. PSTUF, GSTUF or ESTUF is
             * used */
            if (showbits (22) == (32 | SE_CODE))
            {
                /* end of sequence */
                if (!(MBA < MBAmax))
                {
                    return;
                }
            }
            else if ((showbits (22) == PSC << 5))
            {
                /* new picture */
                if (!(MBA < MBAmax))
                {
                    return;
                }
            }
            else
            {
                if (!(MBA % mb_width))
                {
                    gob = getheader () - 1;
                    if(errorstate)return;
                    if (gob > mb_height)
                    {
//                        if (!quiet)
//                            diag_printf ("GN out of range\n");
                        return;
                    }
                    /* GFID is not allowed to change unless PTYPE in picture header
                     * changes */
                    gfid = getbits (2);
                    /* NB: in error-prone environments the decoder can use this
                     * value to determine whether a picture header where the PTYPE
                     * has changed, has been lost */

                    quant = getbits (5);
                    xpos = 0;
                    ypos = gob;
                    MBA = ypos * mb_width;

                    newgob = 1;
                    gobheader_read = 1;
                }
            }
        }

        /* SAC specific label */
        if (!gobheader_read)
        {
            xpos = MBA % mb_width;
            ypos = MBA / mb_width;
            if (xpos == 0 && ypos > 0)
                newgob = 0;
        }
        else
            gobheader_read = 0;

        if (MBA >= MBAmax)
        {
            /* all macroblocks decoded */
            return;
        }
read_cod:

        COD = showbits (1);

        if (!COD)
        {
            /* COD == 0 --> not skipped */
            coded_map[ypos + 1][xpos + 1] = 1;

            /* flush COD bit */
            flushbits (1);

            ei_ep_prediction_type = getMBTYPE (&ei_ep_cbp, &ei_ep_quant);

            if (fault)
                goto resync;

            if (B_EI_EP_STUFFING == ei_ep_prediction_type)
            {
                /* stuffing - read next COD without advancing MB count. */
                goto read_cod;
            }

            if (EI_EP_INTRA_PREDICTION != ei_ep_prediction_type)
            {
                if (1 == ei_ep_quant)
                {
                    Mode = MODE_INTER_Q;
                }
                else
                {
                    Mode = MODE_INTER;
                }
            }
            else
            {
                if (1 == ei_ep_quant)
                {
                    Mode = MODE_INTRA_Q;
                }
                else
                {
                    Mode = MODE_INTRA;
                }
            }

            if (advanced_intra_coding && (EI_EP_INTRA_PREDICTION == ei_ep_prediction_type))
            {
                /* get INTRA_AC_DC mode for annex I */
                if (!showbits (1))
                    INTRA_AC_DC = getbits (1);
                else
                    INTRA_AC_DC = getbits (2);

            }
            if (1 == ei_ep_cbp || PCT_EI == pict_type )
            {
                if (PCT_EP == pict_type)
                    CBPC = getscalabilityCBPC ();
                else
                    CBPC = ei_ep_cbp;

                CBPY = getCBPY ();

                /* Decode Mode and CBP */
                if ( (MODE_INTRA == Mode || MODE_INTRA_Q == Mode) ||
                        (EI_EP_UPWARD_PREDICTION == ei_ep_prediction_type) ||
                        (PCT_EP == pict_type &&
                         EP_BIDIRECTIONAL_PREDICTION == ei_ep_prediction_type) )
                {
                    /* needed in huffman coding only */
                    CBPY = CBPY ^ 15;
                }
                /* Annex S.3 change */
                else if (alternative_inter_VLC_mode && (CBPC == 3) )
                    CBPY = CBPY ^ 15;

                CBP = (CBPY << 2) | CBPC;
            }
            else
            {
                CBP = 0;
            }

            if (fault)
                goto resync;

            if (Mode == MODE_INTER_Q || Mode == MODE_INTRA_Q )
            {
                /* Read DQUANT if necessary */
                if (ei_ep_quant)
                {
                    if (!modified_quantization_mode)
                    {
                        DQUANT = getbits (2);
                        quant += DQ_tab[DQUANT];
                    }
                    else
                    {
                        tmp = getbits (1);
                        if (tmp)
                        {
                            /* only one more additional bit was sent */
                            tmp = getbits (1);
                            if (tmp)
                            {
                                /* second bit of quant is 1 */
                                DQUANT = change_of_quant_tab_11[quant];
                            }
                            else
                            {
                                /* second bit of quant is 0 */
                                DQUANT = change_of_quant_tab_10[quant];
                            }
                            quant += DQUANT;
                        }
                        else
                        {
                            /* five additional bits were sent as
                             * DQUANT */
                            DQUANT = getbits (5);
                            quant = DQUANT;
                        }
                    }
                }
                if (quant > 31 || quant < 1)
                {
                    //  if (!quiet)
                    //    diag_printf ("Quantizer out of range: clipping\n");
                    quant = mmax (1, mmin (31, quant));
                    /* could set fault-flag and resync here */
                }
            }

            /* motion vectors */
            if (Mode == MODE_INTER || Mode == MODE_INTER_Q )
            {
                switch (ei_ep_prediction_type)
                {
                    case EP_FORWARD_PREDICTION:

                        if (plus_type && long_vectors)
                        {
                            mvfx = getRVLC ();
                            mvfy = getRVLC ();

                            /* flush start code emulation bit */
                            if (mvfx == 1 && mvfy == 1)
                                flushbits(1);
                        }
                        else
                        {
                            mvfx = getTMNMV ();
                            mvfy = getTMNMV ();
                        }

                        pmvf0 = find_pmv (xpos, ypos, 0, 0);
                        if(errorstate)return;
                        pmvf1 = find_pmv (xpos, ypos, 0, 1);
                        if(errorstate)return;

                        if (plus_type && long_vectors)
                        {
                            mvfx += pmvf0;
                            mvfy += pmvf1;
                        }
                        else
                        {
                            mvfx = motion_decode (mvfx, pmvf0);
                            mvfy = motion_decode (mvfy, pmvf1);
                        }


                        MV[0][0][ypos + 1][xpos + 1] = mvfx;
                        MV[1][0][ypos + 1][xpos + 1] = mvfy;

                        break;

                    case EP_BIDIRECTIONAL_PREDICTION:

                        /* No MVs for Bi-Dir (no texture) MBTYPE */
                        if ( (0 == ei_ep_cbp) && (0 == ei_ep_quant) )
                        {
                            MV[0][0][ypos + 1][xpos + 1] = 0;
                            MV[1][0][ypos + 1][xpos + 1] = 0;
                            break;
                        }

                        if (plus_type && long_vectors)
                        {
                            mvfx = getRVLC ();
                            mvfy = getRVLC ();

                            /* flush start code emulation bit */
                            if (mvfx == 1 && mvfy == 1)
                                flushbits(1);
                        }
                        else
                        {
                            mvfx = getTMNMV ();
                            mvfy = getTMNMV ();
                        }

                        pmvf0 = find_pmv (xpos, ypos, 0, 0);
                        if(errorstate)return;
                        pmvf1 = find_pmv (xpos, ypos, 0, 1);
                        if(errorstate)return;

                        if (plus_type && long_vectors)
                        {
                            mvfx += pmvf0;
                            mvfy += pmvf1;
                        }
                        else
                        {
                            mvfx = motion_decode (mvfx, pmvf0);
                            mvfy = motion_decode (mvfy, pmvf1);
                        }


                        MV[0][0][ypos + 1][xpos + 1] = mvfx;
                        MV[1][0][ypos + 1][xpos + 1] = mvfy;

                        break;

                    case EI_EP_UPWARD_PREDICTION:
                    default:

                        /* No MV data for other modes. */
                        MV[0][0][ypos + 1][xpos + 1] = 0;
                        MV[1][0][ypos + 1][xpos + 1] = 0;

                        break;
                }
            }

            if (fault)
                goto resync;
        }
        else
        {
            /* COD == 1 --> skipped MB */
            if (MBA >= MBAmax)
            {
                /* all macroblocks decoded */
                return;
            }
            flushbits (1);

            Mode = MODE_INTER;

            /* Reset CBP */
            CBP = 0;

            coded_map[ypos + 1][xpos + 1] = 0;

            if (PCT_EI == pict_type)
            {
                ei_ep_prediction_type = EI_EP_UPWARD_PREDICTION;
            }
            else
            {
                ei_ep_prediction_type = EP_FORWARD_PREDICTION;
            }

            /* reset motion vectors */
            MV[0][0][ypos + 1][xpos + 1] = 0;
            MV[1][0][ypos + 1][xpos + 1] = 0;
        }

        /* Store mode and prediction type */
        modemap[ypos + 1][xpos + 1] = Mode;
        predictionmap[ypos + 1][xpos + 1] = ei_ep_prediction_type;

        /* store defaults for advanced intra coding mode */
        if (advanced_intra_coding)
        {
            for (i = 0; i < blk_cnt; i++)
                store_qcoeff[MBA * blk_cnt * 64 + i * 64] = 1024;
        }
        if (Mode == MODE_INTRA || Mode == MODE_INTRA_Q)
        {
            MV[0][0][ypos + 1][xpos + 1] = MV[1][0][ypos + 1][xpos + 1] = 0;
        }

reconstruct_mb:

        /* pixel coordinates of top left corner of current macroblock */
        /* one delayed because of OBMC */
        if (xpos > 0)
        {
            bx = 16 * (xpos - 1);
            by = 16 * ypos;
        }
        else
        {
            bx = coded_picture_width - 16;
            by = 16 * (ypos - 1);
        }

        if (MBA > 0)
        {
            Mode = modemap[by / 16 + 1][bx / 16 + 1];
            ei_ep_prediction_type = predictionmap[by / 16 + 1][bx / 16 + 1];

            /* motion compensation for true B frame. */
            if (Mode == MODE_INTER || Mode == MODE_INTER_Q)
                reconstruct_ei_ep (bx, by, ei_ep_prediction_type);
            if(errorstate)return;


            /* copy or add block data into true B picture */
            for (comp = 0; comp < blk_cnt; comp++)
            {
                /* inverse DCT */
                if (Mode == MODE_INTRA || Mode == MODE_INTRA_Q)
                {
                    // if (refidct) //change shenh
                    //    idctref (ld->block[comp]);
                    //  else
                    idct (ld->block[comp]);
                    addblock (comp, bx, by, 0);
                }
                else if ((pCBP & (1 << (blk_cnt - 1 - comp))))
                {
                    /* No need to to do this for blocks with no coeffs */
                    //  if (refidct) //change shenh
                    //    idctref (ld->block[comp]);
                    //  else
                    idct (ld->block[comp]);
                    addblock (comp, bx, by, 1);
                }
            }
        }
        /* end if (MBA > 0) */
        if (!COD)
        {
            Mode = modemap[ypos + 1][xpos + 1];
            ei_ep_prediction_type = predictionmap[ypos + 1][xpos + 1];

            for (comp = 0; comp < blk_cnt; comp++)
            {
                clearblock (comp);
                if ((Mode == MODE_INTRA || Mode == MODE_INTRA_Q) && !(advanced_intra_coding))
                {
                    /* Intra (except in advanced intra coding mode) */
                    bp = ld->block[comp];
                    bp[0] = getbits (8);
                    // if (bp[0] == 128)
                    //    if (!quiet)
                    //      fprintf (stderr, "Illegal DC-coeff: 1000000\n");
                    if (bp[0] == 255)   /* Spec. in H.26P, not in TMN4 */
                        bp[0] = 128;
                    bp[0] *= 8;         /* Iquant */

                    if ((CBP & (1 << (blk_cnt - 1 - comp))))
                    {
                        getblock (comp, 0, 0, Mode);
                        if(errorstate)return;
                    }
                }
                else
                {
                    /* Inter (or Intra in advanced intra coding mode) */
                    if ((CBP & (1 << (blk_cnt - 1 - comp))))
                    {
                        getblock (comp, 1, INTRA_AC_DC, Mode);
                        if(errorstate)return;
                    }
                }
                if ((advanced_intra_coding) && (Mode == MODE_INTRA || Mode == MODE_INTRA_Q))
                {
                    Intra_AC_DC_Decode (store_qcoeff, INTRA_AC_DC, MBA, xpos, ypos, comp, newgob);
                    if(errorstate)return;
                    memcpy ((void *) (store_qcoeff + MBA * blk_cnt * 64 + comp * 64),
                            (void *) ld->block[comp], sizeof (short) * 64);

                }
                if (fault)
                    goto resync;
            }
        }

        /* advance to next macroblock */
        MBA++;
        pCBP = CBP;
        pCOD = COD;
        quant_map[ypos + 1][xpos + 1] = quant;

        //fflush (stdout);//change shenh

        if (MBA >= MBAmax && !last_done)
        {
            COD = 1;
            xpos = 0;
            ypos++;
            last_done = 1;
            goto reconstruct_mb;
        }
    }
}
#endif
/* set block to zero */

static void clearblock (int comp)
{
    // int *bp;
    char *bp;
    // int i;
#ifdef USE_INTERNAL_SRAM
    bp = (char *) (ld->block+(comp<<6));
#else
    //bp = (char *) ld->block[comp];
    bp = (char *) (ld->block+(comp<<6));
#endif

    /*
      for (i = 0; i < 8; i++)
      {
      bp[0] = bp[1] = bp[2] = bp[3] = 0;
      bp += 4;
      }*///shenh

    memset(bp, 0, 128);

}


/* move/add 8x8-Block from block[comp] to refframe or bframe */
#if 0
static void addblock (int comp, int bx, int by, int addflag)
{
    int cc, i, iincr;//, P = 1;
    unsigned char *rfp;
    short *bp;
    //short x0,x1,x2,x3,x4,x5,x6,x7;
    //unsigned char *curr[3];shenh

    /*//change shenh
      if (enhancement_layer_num > 1)
      {
      curr[0] = current_enhancement_frame[enhancement_layer_num-2][0];
      curr[1] = current_enhancement_frame[enhancement_layer_num-2][1];
      curr[2] = current_enhancement_frame[enhancement_layer_num-2][2];
      errorstate=-1;
      return;
      }
      else
      {
      curr[0] = current_frame[0];
      curr[1] = current_frame[1];
      curr[2] = current_frame[2];
      }
    */
#ifdef USE_INTERNAL_SRAM
    bp = ld->block+comp*64;
#else
    bp = ld->block[comp];
#endif


#if 0 //shenh
    if (comp >= 6)
    {
        /* This is a component for B-frame forward prediction */
        /* change shenh
           P = 0;
           addflag = 1;
           comp -= 6;
        */
        errorstate=-1;
        return;
    }
#endif
    cc = (comp < 4) ? 0 : (comp & 1) + 1; /* color component index */

    if (cc == 0)
    {
        /* luminance */

        /* frame DCT coding */
        /*shenh
          if (P)
          rfp = curr[0]
          + coded_picture_width * (by + ((comp & 2) << 2)) + bx + ((comp & 1) << 3);
          else
          {
          //change shenh
          //  rfp = bframe[0]
          //    + coded_picture_width * (by + ((comp & 2) << 2)) + bx + ((comp & 1) << 3);
          errorstate=-1;
          return;
          }
        */
        //rfp = curr[0] + coded_picture_width * (by + ((comp & 2) << 2)) + bx + ((comp & 1) << 3);

        rfp = current_frame[0] + coded_picture_width * (by + ((comp & 2) << 2)) + bx + ((comp & 1) << 3);
        iincr = coded_picture_width;
    }
    else
    {
        /* chrominance */

        /* scale coordinates */
        bx >>= 1;
        by >>= 1;
        /* frame DCT coding */
        /*shenh
          if (P)
          rfp = curr[cc] + chrom_width * by + bx;
          else
          {
          // rfp = bframe[cc] + chrom_width * by + bx;//change shenh
          errorstate=-1;
          return;
          }
        */
        //rfp = curr[cc] + chrom_width * by + bx;
        rfp = current_frame[cc] + chrom_width * by + bx;

        iincr = chrom_width;
    }


    if (addflag)
    {
        for (i = 0; i < 8; i++)
        {
            rfp[0] = clp[bp[0] + rfp[0]];
            rfp[1] = clp[bp[1] + rfp[1]];
            rfp[2] = clp[bp[2] + rfp[2]];
            rfp[3] = clp[bp[3] + rfp[3]];
            rfp[4] = clp[bp[4] + rfp[4]];
            rfp[5] = clp[bp[5] + rfp[5]];
            rfp[6] = clp[bp[6] + rfp[6]];
            rfp[7] = clp[bp[7] + rfp[7]];
            bp += 8;
            rfp += iincr;
        }
    }
    else
    {
        for (i = 0; i < 8; i++)
        {

            rfp[0] = clp[bp[0]];
            rfp[1] = clp[bp[1]];
            rfp[2] = clp[bp[2]];
            rfp[3] = clp[bp[3]];
            rfp[4] = clp[bp[4]];
            rfp[5] = clp[bp[5]];
            rfp[6] = clp[bp[6]];
            rfp[7] = clp[bp[7]];

            /*
              x0=bp[0];
              x1=bp[1];
              x2=bp[2];
              x3=bp[3];
              x4=bp[4];
              x5=bp[5];
              x6=bp[6];
              x7=bp[7];

              rfp[0] = clp[x0];
              rfp[1] = clp[x1];
              rfp[2] = clp[x2];
              rfp[3] = clp[x3];
              rfp[4] = clp[x4];
              rfp[5] = clp[x5];
              rfp[6] = clp[x6];
              rfp[7] = clp[x7];
            */
            bp += 8;
            rfp += iincr;
        }
    }

    /* diag_printf("%s", (cc ? "Chrominance\n" : "Luminance\n"));  for (i=0; i<8;
     * ++i) { diag_printf("%d %d %d %d %d %d %d %d\n", rfp[0], rfp[1], rfp[2],
     * rfp[3],  rfp[4], rfp[5], rfp[6], rfp[7]); rfp+= iincr; }
     * diag_printf("\n");  */
}
#endif

/* bidirectionally reconstruct 8x8-Block from block[comp] to bframe */
#if 0//change shenh
static void reconblock_b (int comp, int bx, int by, int mode, int bdx, int bdy)
{
    int cc, i, j, k, ii;
    unsigned char *bfr, *ffr;
    int BMVx, BMVy;
    int xa, xb, ya, yb, x, y, xvec, yvec, mvx, mvy;
    int xint, xhalf, yint, yhalf, pel;

    x = bx / 16 + 1;
    y = by / 16 + 1;

    if (mode == MODE_INTER4V || mode == MODE_INTER4V_Q)
    {
        if (comp < 4)
        {
            /* luma */
            mvx = MV[0][comp + 1][y][x];
            mvy = MV[1][comp + 1][y][x];
            BMVx = (bdx == 0 ? (trb - trd) * mvx / trd : trb * mvx / trd + bdx - mvx);
            BMVy = (bdy == 0 ? (trb - trd) * mvy / trd : trb * mvy / trd + bdy - mvy);
        }
        else
        {
            /* chroma */
            xvec = yvec = 0;
            for (k = 1; k <= 4; k++)
            {
                mvx = MV[0][k][y][x];
                mvy = MV[1][k][y][x];
                xvec += (bdx == 0 ? (trb - trd) * mvx / trd : trb * mvx / trd + bdx - mvx);
                yvec += (bdy == 0 ? (trb - trd) * mvy / trd : trb * mvy / trd + bdy - mvy);
            }

            /* chroma rounding (table 16/H.263) */
            BMVx = sign (xvec) * (h263_roundtab[ABS (xvec) % 16] + (ABS (xvec) / 16) * 2);
            BMVy = sign (yvec) * (h263_roundtab[ABS (yvec) % 16] + (ABS (yvec) / 16) * 2);
        }
    }
    else
    {
        if (comp < 4)
        {
            /* luma */
            mvx = MV[0][0][y][x];
            mvy = MV[1][0][y][x];
            BMVx = (bdx == 0 ? (trb - trd) * mvx / trd : trb * mvx / trd + bdx - mvx);
            BMVy = (bdy == 0 ? (trb - trd) * mvy / trd : trb * mvy / trd + bdy - mvy);
        }
        else
        {
            /* chroma */
            mvx = MV[0][0][y][x];
            mvy = MV[1][0][y][x];
            xvec = (bdx == 0 ? (trb - trd) * mvx / trd : trb * mvx / trd + bdx - mvx);
            yvec = (bdy == 0 ? (trb - trd) * mvy / trd : trb * mvy / trd + bdy - mvy);
            xvec *= 4;
            yvec *= 4;

            /* chroma rounding (table 16/H.263) */
            BMVx = sign (xvec) * (h263_roundtab[ABS (xvec) % 16] + (ABS (xvec) / 16) * 2);
            BMVy = sign (yvec) * (h263_roundtab[ABS (yvec) % 16] + (ABS (yvec) / 16) * 2);
        }
    }

    cc = (comp < 4) ? 0 : (comp & 1) + 1; /* color component index */

    if (cc == 0)
    {
        /* luminance */
        find_bidir_limits (BMVx, &xa, &xb, comp & 1);
        find_bidir_limits (BMVy, &ya, &yb, (comp & 2) >> 1);
        bfr = bframe[0] +
              coded_picture_width * (by + ((comp & 2) << 2)) + bx + ((comp & 1) << 3);
        ffr = current_frame[0] +
              coded_picture_width * (by + ((comp & 2) << 2)) + bx + ((comp & 1) << 3);
        ii = coded_picture_width;
    }
    else
    {
        /* chrominance */
        /* scale coordinates and vectors */
        bx >>= 1;
        by >>= 1;

        find_bidir_chroma_limits (BMVx, &xa, &xb);
        find_bidir_chroma_limits (BMVy, &ya, &yb);

        bfr = bframe[cc] + chrom_width * (by + ((comp & 2) << 2)) + bx + (comp & 8);
        ffr = current_frame[cc] + chrom_width * (by + ((comp & 2) << 2)) + bx + (comp & 8);
        ii = chrom_width;
    }

    xint = BMVx >> 1;
    xhalf = BMVx - 2 * xint;
    yint = BMVy >> 1;
    yhalf = BMVy - 2 * yint;

    ffr += xint + (yint + ya) * ii;
    bfr += ya * ii;

    if (!xhalf && !yhalf)
    {
        for (j = ya; j < yb; j++)
        {
            for (i = xa; i < xb; i++)
            {
                pel = ffr[i];
                bfr[i] = ((unsigned int) (pel + bfr[i])) >> 1;
            }
            bfr += ii;
            ffr += ii;
        }
    }
    else if (xhalf && !yhalf)
    {
        for (j = ya; j < yb; j++)
        {
            for (i = xa; i < xb; i++)
            {
                pel = ((unsigned int) (ffr[i] + ffr[i + 1] + 1)) >> 1;
                bfr[i] = ((unsigned int) (pel + bfr[i])) >> 1;
            }
            bfr += ii;
            ffr += ii;
        }
    }
    else if (!xhalf && yhalf)
    {
        for (j = ya; j < yb; j++)
        {
            for (i = xa; i < xb; i++)
            {
                pel = ((unsigned int) (ffr[i] + ffr[ii + i] + 1)) >> 1;
                bfr[i] = ((unsigned int) (pel + bfr[i])) >> 1;
            }
            bfr += ii;
            ffr += ii;
        }
    }
    else
    {
        /* if (xhalf && yhalf) */
        for (j = ya; j < yb; j++)
        {
            for (i = xa; i < xb; i++)
            {
                pel = ((unsigned int) (ffr[i] + ffr[i + 1] + ffr[ii + i] + ffr[ii + i + 1] + 2)) >> 2;
                bfr[i] = ((unsigned int) (pel + bfr[i])) >> 1;
            }
            bfr += ii;
            ffr += ii;
        }
    }
    return;
}
#endif

int motion_decode (int vec, int pmv)
{
    if (vec > 31)
        vec -= 64;
    vec += pmv;
    if (!long_vectors)
    {
        if (vec > 31)
            vec -= 64;
        if (vec < -32)
            vec += 64;
    }
    else
    {
        if (pmv < -31 && vec < -63)
            vec += 64;
        if (pmv > 32 && vec > 63)
            vec -= 64;
    }
    return vec;
}


int find_pmv (int x, int y, int block, int comp)
{
    int p1, p2, p3;
    int xin1, xin2, xin3;
    int yin1, yin2, yin3;
    int vec1, vec2, vec3;
    int l8, o8, or8;
    short mode;//shenh

    errorstate=0;

    x++;
    y++;

    //l8 = (modemap[y][x - 1] == MODE_INTER4V ? 1 : 0);
    p1=y*(MBC+2)+x;
    mode=*(modemap+p1-1);
    l8 = (mode == MODE_INTER4V ? 1 : 0);
    //  l8 = (modemap[y][x - 1] == MODE_INTER4V_Q ? 1 : l8);
    l8 = (mode == MODE_INTER4V_Q ? 1 : l8);

    //o8 = (modemap[y - 1][x] == MODE_INTER4V ? 1 : 0);
    p1=p1-(MBC+2);
    mode=*(modemap+p1);
    o8 = (mode == MODE_INTER4V ? 1 : 0);
    //o8 = (modemap[y - 1][x] == MODE_INTER4V_Q ? 1 : o8);
    o8 = (mode == MODE_INTER4V_Q ? 1 : o8);

    //or8 = (modemap[y - 1][x + 1] == MODE_INTER4V ? 1 : 0);
    mode=*(modemap+p1+1);
    or8 = (mode == MODE_INTER4V ? 1 : 0);
    //or8 = (modemap[y - 1][x + 1] == MODE_INTER4V_Q ? 1 : or8);
    or8 = (mode == MODE_INTER4V_Q ? 1 : or8);


    switch (block)
    {
        case 0:
            vec1 = (l8 ? 2 : 0);
            yin1 = y;
            xin1 = x - 1;
            vec2 = (o8 ? 3 : 0);
            yin2 = y - 1;
            xin2 = x;
            vec3 = (or8 ? 3 : 0);
            yin3 = y - 1;
            xin3 = x + 1;
            break;
        case 1:
            vec1 = (l8 ? 2 : 0);
            yin1 = y;
            xin1 = x - 1;
            vec2 = (o8 ? 3 : 0);
            yin2 = y - 1;
            xin2 = x;
            vec3 = (or8 ? 3 : 0);
            yin3 = y - 1;
            xin3 = x + 1;
            break;
        case 2:
            vec1 = 1;
            yin1 = y;
            xin1 = x;
            vec2 = (o8 ? 4 : 0);
            yin2 = y - 1;
            xin2 = x;
            vec3 = (or8 ? 3 : 0);
            yin3 = y - 1;
            xin3 = x + 1;
            break;
        case 3:
            vec1 = (l8 ? 4 : 0);
            yin1 = y;
            xin1 = x - 1;
            vec2 = 1;
            yin2 = y;
            xin2 = x;
            vec3 = 2;
            yin3 = y;
            xin3 = x;
            break;
        case 4:
            vec1 = 3;
            yin1 = y;
            xin1 = x;
            vec2 = 1;
            yin2 = y;
            xin2 = x;
            vec3 = 2;
            yin3 = y;
            xin3 = x;
            break;
        case 5:
            vec1 = 5;
            yin1 = y;
            xin1 = x - 1;
            vec2 = 5;
            yin2 = y - 1;
            xin2 = x;
            vec3 = 5;
            yin3 = y - 1;
            xin3 = x + 1;
            break;
        default:
            //fprintf (stderr, "Illegal block number in find_pmv (getpic.c)\n");
//        diag_printf ( "Illegal block number in find_pmv (getpic.c)\n");
            //exit (-1);
            errorstate=-1;
            return -1;
            break;
    }

    //p1 = MV[comp][vec1][yin1][xin1];
    l8=comp*6;
    o8=(MBR+1);
    or8=(MBC+2);
    p1=*(MV+((l8+vec1)*o8+yin1)*or8+xin1);
    //p2 = MV[comp][vec2][yin2][xin2];
    p2=*(MV+((l8+vec2)*o8+yin2)*or8+xin2);
    //p3 = MV[comp][vec3][yin3][xin3];
    p3=*(MV+((l8+vec3)*o8+yin3)*or8+xin3);

    if (newgob && (block == 0 || block == 1 || block == 2))
        p2 = NO_VEC;

    if (p2 == NO_VEC)
    {
        p2 = p3 = p1;
    }
    return p1 + p2 + p3 - mmax (p1, mmax (p2, p3)) - mmin (p1, mmin (p2, p3));
}


#if 0//change shenh
void find_bidir_limits (int vec, int *start, int *stop, int nhv)
{
    /* limits taken from C loop in section G5 in H.263 */
    *start = mmax (0, (-vec + 1) / 2 - nhv * 8);
    *stop = mmin (7, 15 - (vec + 1) / 2 - nhv * 8);

    (*stop)++;                    /* I use < and not <= in the loop */
}

void find_bidir_chroma_limits (int vec, int *start, int *stop)
{

    /* limits taken from C loop in section G5 in H.263 */
    *start = mmax (0, (-vec + 1) / 2);
    *stop = mmin (7, 7 - (vec + 1) / 2);

    (*stop)++;                    /* I use < and not <= in the loop */
    return;
}
#endif

#if 0//change shenh
void make_edge_image (unsigned char *src, unsigned char *dst,
                      int width, int height, int edge)
{
    int i, j;
    unsigned char *p1, *p2, *p3, *p4;
    unsigned char *o1, *o2, *o3, *o4;

    /* center image */
    p1 = dst;
    o1 = src;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            *(p1 + i) = *(o1 + i);
        }
        p1 += width + (edge << 1);
        o1 += width;
    }

    /* left and right edges */
    p1 = dst - 1;
    o1 = src;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < edge; i++)
        {
            *(p1 - i) = *o1;
            *(p1 + width + i + 1) = *(o1 + width - 1);
        }
        p1 += width + (edge << 1);
        o1 += width;
    }

    /* top and bottom edges */
    p1 = dst;
    p2 = dst + (width + (edge << 1)) * (height - 1);
    o1 = src;
    o2 = src + width * (height - 1);
    for (j = 0; j < edge; j++)
    {
        p1 = p1 - (width + (edge << 1));
        p2 = p2 + (width + (edge << 1));
        for (i = 0; i < width; i++)
        {
            *(p1 + i) = *(o1 + i);
            *(p2 + i) = *(o2 + i);
        }
    }

    /* corners */
    p1 = dst - (width + (edge << 1)) - 1;
    p2 = p1 + width + 1;
    p3 = dst + (width + (edge << 1)) * (height) - 1;
    p4 = p3 + width + 1;

    o1 = src;
    o2 = o1 + width - 1;
    o3 = src + width * (height - 1);
    o4 = o3 + width - 1;
    for (j = 0; j < edge; j++)
    {
        for (i = 0; i < edge; i++)
        {
            *(p1 - i) = *o1;
            *(p2 + i) = *o2;
            *(p3 - i) = *o3;
            *(p4 + i) = *o4;
        }
        p1 = p1 - (width + (edge << 1));
        p2 = p2 - (width + (edge << 1));
        p3 = p3 + width + (edge << 1);
        p4 = p4 + width + (edge << 1);
    }

}
#endif

#if 0//change shenh
void interpolate_image (unsigned char *in, unsigned char *out, int width, int height)
/* only used for displayed interpolated frames, not reconstructed ones */
{

    int x, xx, y, w2;

    unsigned char *pp, *ii;

    w2 = 2 * width;

    /* Horizontally */
    pp = out;
    ii = in;
    for (y = 0; y < height - 1; y++)
    {
        for (x = 0, xx = 0; x < width - 1; x++, xx += 2)
        {
            *(pp + xx) = *(ii + x);
            *(pp + xx + 1) = ((unsigned int) (*(ii + x) + *(ii + x + 1)) ) >> 1;
            *(pp + w2 + xx) = ((unsigned int) (*(ii + x) + *(ii + x + width))) >> 1;
            *(pp + w2 + xx + 1) = ((unsigned int) (*(ii + x) + *(ii + x + 1) +
                                                   *(ii + x + width) + *(ii + x + width + 1))) >> 2;

        }
        *(pp + w2 - 2) = *(ii + width - 1);
        *(pp + w2 - 1) = *(ii + width - 1);
        *(pp + w2 + w2 - 2) = *(ii + width + width - 1);
        *(pp + w2 + w2 - 1) = *(ii + width + width - 1);
        pp += w2 << 1;
        ii += width;
    }

    /* last lines */
    for (x = 0, xx = 0; x < width - 1; x++, xx += 2)
    {
        *(pp + xx) = *(ii + x);
        *(pp + xx + 1) = ((unsigned int) (*(ii + x) + *(ii + x + 1) + 1 )) >> 1;
        *(pp + w2 + xx) = *(ii + x);
        *(pp + w2 + xx + 1) = ((unsigned int) (*(ii + x) + *(ii + x + 1) + 1 )) >> 1;
    }

    /* bottom right corner pels */
    *(pp + (width << 1) - 2) = *(ii + width - 1);
    *(pp + (width << 1) - 1) = *(ii + width - 1);
    *(pp + (width << 2) - 2) = *(ii + width - 1);
    *(pp + (width << 2) - 1) = *(ii + width - 1);

    return;
}

#endif


/**********************************************************************
 *
 *  Name:           Intra_AC_DC_Decode
 *  Description:    Intra Prediction in Advanced Intra Coding
 *
 *  Input:          store_qcoeff, Intra_AC_DC, position of MB, store_QP
 *
 *  Side effects:   change qcoeff to predicted qcoeff
 *
 *      Return:
 *
 *  Date:970717     Guy Cote <guyc@ee.ubc.ca>
 *
 ***********************************************************************/
#if 0

void Intra_AC_DC_Decode (short *store_qcoeff, int INTRA_AC_DC, int MBA, int xpos, int ypos, int comp, int newgob1)
{

    int A[8], B[8];
    int i, j, tempDC;
    short *Rec_C;
    short *rcoeff;

    static unsigned int  totaltick=0;

    errorstate=0;


    Rec_C = ld->block[comp];

    if (xpos == 0 && ypos == 0)
    {
        /* top left corner */
        (comp == 2 || comp == 3) ? fill_A (A, store_qcoeff, xpos, ypos, comp - 2) : fill_null (A);
        (comp == 1 || comp == 3) ? fill_B (B, store_qcoeff, xpos, ypos, comp - 1) : fill_null (B);
    }
    else
    {
        /* left border of picture */
        if (xpos == 0)
        {
            /* left edge of the picture */
            (comp == 2 || comp == 3) ? fill_A (A, store_qcoeff, xpos, ypos, comp - 2) :
            ((comp == 0 || comp == 1) && !(newgob1)) ? fill_A (A, store_qcoeff, xpos, ypos - 1, comp + 2) :
            ((comp == 4 || comp == 5) && !(newgob1)) ? fill_A (A, store_qcoeff, xpos, ypos - 1, comp) : fill_null (A);
            (comp == 1 || comp == 3) ? fill_B (B, store_qcoeff, xpos, ypos, comp - 1) : fill_null (B);
        }
        else
        {
            if (ypos == 0)
            {
                /* top border of picture */
                (comp == 2 || comp == 3) ? fill_A (A, store_qcoeff, xpos, ypos, comp - 2) : fill_null (A);
                (comp == 4 || comp == 5) ? fill_B (B, store_qcoeff, xpos - 1, ypos, comp) :
                (comp == 1 || comp == 3) ? fill_B (B, store_qcoeff, xpos, ypos, comp - 1) :
                fill_B (B, store_qcoeff, xpos - 1, ypos, comp + 1);
            }
            else
            {
                /* anywhere else in the picture, do not
                 * cross GOB boundary */
                (comp == 2 || comp == 3) ? fill_A (A, store_qcoeff, xpos, ypos, comp - 2) :
                ((comp == 0 || comp == 1) && !(newgob1)) ? fill_A (A, store_qcoeff, xpos, ypos - 1, comp + 2) :
                ((comp == 4 || comp == 5) && !(newgob1)) ? fill_A (A, store_qcoeff, xpos, ypos - 1, comp) : fill_null (A);

                (comp == 4 || comp == 5) ? fill_B (B, store_qcoeff, xpos - 1, ypos, comp) :
                (comp == 1 || comp == 3) ? fill_B (B, store_qcoeff, xpos, ypos, comp - 1) :
                fill_B (B, store_qcoeff, xpos - 1, ypos, comp + 1);
            }
        }
    }


    /* replace the qcoeff with the predicted values pcoeff */
    switch (INTRA_AC_DC)
    {
        case INTRA_MODE_DC:

            tempDC = Rec_C[0] + ((A[0] == 1024 && B[0] == 1024) ? 1024 :
                                 (A[0] == 1024) ? B[0] :
                                 (B[0] == 1024) ? A[0] : (A[0] + B[0]) / 2);
            for (i = 0; i < 8; i++)
                for (j = 0; j < 8; j++)
                    Rec_C[i * 8 + j] = clipAC (Rec_C[i * 8 + j]);
            Rec_C[0] = oddifyclipDC (tempDC);
            break;
        case INTRA_MODE_VERT_AC:
            tempDC = Rec_C[0] + A[0];
            for (i = 1; i < 8; i++)
            {
                rcoeff = &Rec_C[i];
                *rcoeff = clipAC (Rec_C[i] + A[i]);
            }
            for (i = 1; i < 8; i++)
                for (j = 0; j < 8; j++)
                    Rec_C[i * 8 + j] = clipAC (Rec_C[i * 8 + j]);
            Rec_C[0] = oddifyclipDC (tempDC);
            break;
        case INTRA_MODE_HORI_AC:
            tempDC = Rec_C[0] + B[0];
            for (i = 1; i < 8; i++)
                Rec_C[i * 8] = clipAC (Rec_C[i * 8] + B[i]);
            for (i = 0; i < 8; i++)
                for (j = 1; j < 8; j++)
                    Rec_C[i * 8 + j] = clipAC (Rec_C[i * 8 + j]);
            Rec_C[0] = oddifyclipDC (tempDC);
            break;
        default:
//        diag_printf ("Error in Prediction in Advanced Intra Coding\n");
            //exit (-1);
            errorstate=-1;
            return;
            break;
    }

    return;
}

/**********************************************************************
 *
 *  Name:           fill_null, fill_A, fill_B, oddifyclipDC, clipAC
 *                      and clipDC
 *  Description:    Fill values in predictor coefficients
 *                      Functions used in advanced intra coding mode
 *
 *  Input:          predictor qcoefficients
 *  Side effects:
 *
 *      Return:
 *
 *  Date:970717     Guy Cote <guyc@ee.ubc.ca>
 *
 ***********************************************************************/


void fill_null (int pred[])
{
    int j;

    pred[0] = 1024;
    for (j = 1; j < 8; j++)
    {
        pred[j] = 0;
    }
}

void fill_A (int pred[], short *store_qcoeff, int xpos, int ypos, int block)
{
    /* Fill first row of block at MB xpos, ypos, in pred[] */
    int j;
    for (j = 0; j < 8; j++)
    {
        pred[j] = *(store_qcoeff + (ypos * mb_width + xpos) * 384 + block * 64 + j);
    }
}

void fill_B (int pred[], short *store_qcoeff, int xpos, int ypos, int block)
{
    /* Fill first column of block at MB xpos, ypos, in pred[][i] */
    int j;
    for (j = 0; j < 8; j++)
    {
        pred[j] = *(store_qcoeff + (ypos * mb_width + xpos) * 384 + block * 64 + j * 8);
    }
}

int oddifyclipDC (int x)
{

    int result;


    (x % 2) ? (result = clipDC (x)) : (result = clipDC (x + 1));
    return result;
}
int clipAC (int x)
{
    int clipped;

    if (x > 2047)
        clipped = 2047;
    else if (x < -2048)
        clipped = -2048;
    else
        clipped = x;
    return clipped;

}
int clipDC (int x)
{
    int clipped;
    if (x > 2047)
        clipped = 2047;
    else if (x < 0)
        clipped = 0;
    else
        clipped = x;
    return clipped;

}
#endif

#if 0//change shenh
/**********************************************************************
 *
 *  Name:       EdgeFilter
 *  Description:    performs in the loop edge-filtering on
 *                      reconstructed frames
 *
 *  Input:          pointers to reconstructed frame and difference
 *                      image
 *  Returns:
 *  Side effects:   since neither the algorithm nor the routines
 *                      have been optimized for speed, the use of the
 *                      edge-filter slows down decoding speed
 *
 *  Date: 951129    Author: Gisle.Bjontegaard@fou.telenor.no
 *                        Karl.Lillevold@nta.no
 *  Date: 970820  Author: guyc@ee.ubc.ca
 *                        modified to implement annex J of H.263+
 *
 ***********************************************************************/


void edge_filter (unsigned char *lum, unsigned char *Cb, unsigned char *Cr,
                  int width, int height)
{

    /* Luma */
    horiz_edge_filter (lum, width, height, 0);
    if( errorstate) return;

    vert_edge_filter (lum, width, height, 0);
    if( errorstate) return;

    /* Chroma */
    horiz_edge_filter (Cb, width / 2, height / 2, 1);
    if( errorstate) return;
    vert_edge_filter (Cb, width / 2, height / 2, 1);
    if( errorstate) return;
    horiz_edge_filter (Cr, width / 2, height / 2, 1);
    if( errorstate) return;
    vert_edge_filter (Cr, width / 2, height / 2, 1);
    if( errorstate) return;

    /* that's it */
    return;
}


/***********************************************************************/


void horiz_edge_filter (unsigned char *rec, int width, int height, int chr)
{
    int i, j;
    int delta, d1, d2;
    int mbc, mbr, do_filter;
    int QP;
    int mbr_above;


    /* horizontal edges */
    for (j = 8; j < height; j += 8)
    {
        for (i = 0; i < width; i++)
        {
            if (!chr)
            {
                mbr = j >> 4;
                mbc = i >> 4;
                mbr_above = (j - 8) >> 4;
            }
            else
            {
                mbr = j >> 3;
                mbc = i >> 3;
                mbr_above = mbr - 1;
            }

            do_filter = coded_map[mbr + 1][mbc + 1] || coded_map[mbr_above + 1][mbc + 1];
            if (do_filter)
            {
                if (pb_frame)
                {
                    /* change shenh
                       QP = coded_map[mbr + 1][mbc + 1] ?
                       mmax (1, mmin (31, bquant_tab[bquant] * quant_map[mbr + 1][mbc + 1] / 4)) :
                       mmax (1, mmin (31, bquant_tab[bquant] * quant_map[mbr_above + 1][mbc + 1] / 4));
                    */
                    errorstate=-1;
                    return;

                }
                else
                    QP = coded_map[mbr + 1][mbc + 1] ? quant_map[mbr + 1][mbc + 1] : quant_map[mbr_above + 1][mbc + 1];
                if (chr && modified_quantization_mode)
                {
                    QP = MQ_chroma_QP_table[QP];
                }

                delta = (int) (((int) (*(rec + i + (j - 2) * width)) +
                                (int) (*(rec + i + (j - 1) * width) * (-4)) +
                                (int) (*(rec + i + (j) * width) * (4)) +
                                (int) (*(rec + i + (j + 1) * width) * (-1))) / 8.0);

                d1 = sign (delta) * mmax (0, ABS (delta) - mmax (0, 2 * (ABS (delta) - STRENGTH[QP - 1])));

                d2 = mmin (ABS (d1 / 2), mmax (-ABS (d1 / 2), (int) (((*(rec + i + (j - 2) * width) -
                                               *(rec + i + (j + 1) * width))) / 4)));

                *(rec + i + (j + 1) * width) += d2; /* D */
                *(rec + i + (j) * width) = mmin (255, mmax (0, (int) (*(rec + i + (j) * width)) - d1)); /* C */
                *(rec + i + (j - 1) * width) = mmin (255, mmax (0, (int) (*(rec + i + (j - 1) * width)) + d1)); /* B */
                *(rec + i + (j - 2) * width) -= d2; /* A */
            }
        }
    }
    return;
}

void vert_edge_filter (unsigned char *rec, int width, int height, int chr)
{
    int i, j;
    int delta, d1, d2;
    int mbc, mbr;
    int do_filter;
    int QP;
    int mbc_left;


    /* vertical edges */
    for (i = 8; i < width; i += 8)
    {
        for (j = 0; j < height; j++)
        {
            if (!chr)
            {
                mbr = j >> 4;
                mbc = i >> 4;
                mbc_left = (i - 8) >> 4;
            }
            else
            {
                mbr = j >> 3;
                mbc = i >> 3;
                mbc_left = mbc - 1;
            }
            do_filter = coded_map[mbr + 1][mbc + 1] || coded_map[mbr + 1][mbc_left + 1];

            if (do_filter)
            {
                if (pb_frame)
                {
                    /*change shenh
                      QP = coded_map[mbr + 1][mbc + 1] ?
                      mmax (1, mmin (31, bquant_tab[bquant] * quant_map[mbr + 1][mbc + 1] / 4)) :
                      mmax (1, mmin (31, bquant_tab[bquant] * quant_map[mbr + 1][mbc_left + 1] / 4));
                    */
                    errorstate=-1;
                    return;
                }
                else
                    QP = coded_map[mbr + 1][mbc + 1] ?
                         quant_map[mbr + 1][mbc + 1] : quant_map[mbr + 1][mbc_left + 1];
                if (chr && modified_quantization_mode)
                {
                    QP = MQ_chroma_QP_table[QP];
                }

                delta = (int) (((int) (*(rec + i - 2 + j * width)) +
                                (int) (*(rec + i - 1 + j * width) * (-4)) +
                                (int) (*(rec + i + j * width) * (4)) +
                                (int) (*(rec + i + 1 + j * width) * (-1))) / 8.0);

                d1 = sign (delta) * mmax (0, ABS (delta) -
                                          mmax (0, 2 * (ABS (delta) - STRENGTH[QP - 1])));

                d2 = mmin (ABS (d1 / 2), mmax (-ABS (d1 / 2),
                                               (int) ((*(rec + i - 2 + j * width) -
                                                       *(rec + i + 1 + j * width)) / 4)));

                *(rec + i + 1 + j * width) += d2; /* D */
                *(rec + i + j * width) = mmin (255, mmax (0, (int) (*(rec + i + j * width)) - d1)); /* C */
                *(rec + i - 1 + j * width) = mmin (255, mmax (0, (int) (*(rec + i - 1 + j * width)) + d1)); /* B */
                *(rec + i - 2 + j * width) -= d2; /* A */
            }
        }
    }
    return;
}

/**********************************************************************
 *
 *  Name:       PostFilter
 *  Description:    performs in the loop edge-filtering on
 *                      reconstructed frames
 *
 *  Input:          pointers to reconstructed frame and difference
 *                      image
 *  Returns:
 *  Side effects:   since neither the algorithm nor the routines
 *                      have been optimized for speed, the use of the
 *                      edge-filter slows down decoding speed
 *
 *
 *      Date: 971004    Author: guyc@ee.ubc.ca
 *
 *
 ***********************************************************************/


void PostFilter (unsigned char *lum, unsigned char *Cb, unsigned char *Cr,
                 int width, int height)
{

    /* Luma */
    horiz_post_filter (lum, width, height, 0);
    vert_post_filter (lum, width, height, 0);

    /* Chroma */
    horiz_post_filter (Cb, width / 2, height / 2, 1);
    vert_post_filter (Cb, width / 2, height / 2, 1);
    horiz_post_filter (Cr, width / 2, height / 2, 1);
    vert_post_filter (Cr, width / 2, height / 2, 1);

    /* that's it */
    return;
}


/***********************************************************************/


void horiz_post_filter (unsigned char *rec, int width, int height, int chr)
{
    int i, j;
    int delta, d1;
    int mbc, mbr;
    int QP;
    int mbr_above;


    /* horizontal edges */
    for (j = 8; j < height; j += 8)
    {
        for (i = 0; i < width; i++)
        {
            if (!chr)
            {
                mbr = j >> 4;
                mbc = i >> 4;
                mbr_above = (j - 8) >> 4;
            }
            else
            {
                mbr = j >> 3;
                mbc = i >> 3;
                mbr_above = mbr - 1;
            }


            if (pb_frame)
            {
                /*
                  QP = coded_map[mbr + 1][mbc + 1] ?
                  mmax (1, mmin (31, bquant_tab[bquant] * quant_map[mbr + 1][mbc + 1] / 4)) :
                  mmax (1, mmin (31, bquant_tab[bquant] * quant_map[mbr_above + 1][mbc + 1] / 4));
                */
                errorstate=-1;
                return;
            }
            else
                QP = coded_map[mbr + 1][mbc + 1] ?
                     quant_map[mbr + 1][mbc + 1] : quant_map[mbr_above + 1][mbc + 1];

            delta = (int) (((int) (*(rec + i + (j - 3) * width)) +
                            (int) (*(rec + i + (j - 2) * width)) +
                            (int) (*(rec + i + (j - 1) * width)) +
                            (int) (*(rec + i + (j) * width) * (-6)) +
                            (int) (*(rec + i + (j + 1) * width)) +
                            (int) (*(rec + i + (j + 2) * width)) +
                            (int) (*(rec + i + (j + 3) * width))) / 8.0);

            d1 = sign (delta) * mmax (0, ABS (delta) - mmax (0, 2 * (ABS (delta) - STRENGTH1[QP - 1])));

            /* Filter D */
            *(rec + i + (j) * width) += d1;
        }
    }
    return;
}

void vert_post_filter (unsigned char *rec, int width, int height, int chr)
{
    int i, j;
    int delta, d1;
    int mbc, mbr;
    int QP;
    int mbc_left;


    /* vertical edges */
    for (i = 8; i < width; i += 8)
    {
        for (j = 0; j < height; j++)
        {
            if (!chr)
            {
                mbr = j >> 4;
                mbc = i >> 4;
                mbc_left = (i - 8) >> 4;
            }
            else
            {
                mbr = j >> 3;
                mbc = i >> 3;
                mbc_left = mbc - 1;
            }

            if (pb_frame)
            {
                /*change shenh
                  QP = coded_map[mbr + 1][mbc + 1] ?
                  mmax (1, mmin (31, bquant_tab[bquant] * quant_map[mbr + 1][mbc + 1] / 4)) :
                  mmax (1, mmin (31, bquant_tab[bquant] * quant_map[mbr + 1][mbc_left + 1] / 4));
                */
                errorstate=-1;
                return;

            }
            else
                QP = coded_map[mbr + 1][mbc + 1] ?
                     quant_map[mbr + 1][mbc + 1] : quant_map[mbr + 1][mbc_left + 1];

            delta = (int) (((int) (*(rec + i - 3 + j * width)) +
                            (int) (*(rec + i - 2 + j * width)) +
                            (int) (*(rec + i - 1 + j * width)) +
                            (int) (*(rec + i + j * width) * (-6)) +
                            (int) (*(rec + i + 1 + j * width)) +
                            (int) (*(rec + i + 2 + j * width)) +
                            (int) (*(rec + i + 3 + j * width))) / 8.0);

            d1 = sign (delta) * mmax (0, ABS (delta) - mmax (0, 2 * (ABS (delta) - STRENGTH2[QP - 1])));

            /* Post Filter D */
            *(rec + i + j * width) += d1;
        }
    }
    return;
}
#endif

#if 0 //unspport now. shenh change.
/**********************************************************************
 *
 *  Name:             init_enhancement_layer
 *  Description:  intializes an enhancement layer when the first picture header
 *                in that layer is decoded
 *  Input:        layer number
 *  Returns:
 *  Side effects: allocates memory for previous and current enhancement
 *                layer picture as well as framed enhancement layer picture,
 *                and tmp forward and upward pictures (for bi-dir prediction),
 *                Y, Cb, and Cr components.
 *
 *  Date: 971102  Author: mikeg@ee.ubc.ca
 *
 *
 ***********************************************************************/
static void init_enhancement_layer (int layer)
{
    int cc, size;

    blk_cnt = 6;

    for (cc = 0; cc < 3; cc++)
    {
        if (cc == 0)
            size = coded_picture_width * coded_picture_height;
        else
            size = chrom_width * chrom_height;

        /* Used for bidirectional and direct prediction mode for true B
         * pictures, one for forward and one for backward. */
        if (!(prev_enhancement_frame[layer][cc] = (unsigned char *) h263_myMalloc (size)))
            error ("h263_myMalloc failed\n");

        if (!(current_enhancement_frame[layer][cc] = (unsigned char *) h263_myMalloc (size)))
            error ("h263_myMalloc failed\n");

        if (!(tmp_enhance_fwd[layer][cc] = (unsigned char *) h263_myMalloc (size)))
            error ("h263_myMalloc failed\n");

        if (!(tmp_enhance_up[layer][cc] = (unsigned char *) h263_myMalloc (size)))
            error ("h263_myMalloc failed\n");

    }

    for (cc = 0; cc < 3; cc++)
    {
        if (cc == 0)
        {
            size = (coded_picture_width + 64) * (coded_picture_height + 64);

            /* Stores framed version of previous inter-picture, luminance. */
            if (!(enhance_edgeframeorig[layer][cc] = (unsigned char *) h263_myMalloc (size)))
                error ("h263_myMalloc failed\n");
            enhance_edgeframe[layer][cc] = enhance_edgeframeorig[layer][cc] + (coded_picture_width + 64) * 32 + 32;
        }
        else
        {
            size = (chrom_width + 32) * (chrom_height + 32);

            /* Stores framed version of previous inter-picture, chrominance. */
            if (!(enhance_edgeframeorig[layer][cc] = (unsigned char *) h263_myMalloc (size)))
                error ("h263_myMalloc failed\n");
            enhance_edgeframe[layer][cc] = enhance_edgeframeorig[layer][cc] + (chrom_width + 32) * 16 + 16;
        }
    }

    sprintf (enhance_recon_file_name[enhancement_layer_num-2], "enhanced_%d.raw",
             (enhancement_layer_num-1));
    if ((enhance_recon_file_ptr[enhancement_layer_num-2] = fopen (
                enhance_recon_file_name[enhancement_layer_num-2], "wb")) == NULL)
    {
//        diag_printf ("Unable to open enhancement layer reconstruction file");
        exit (-1);
    }
}

/**********************************************************************
 *
 *  Name:         UpsampleReferenceLayerPicture
 *  Description:  calls UpsampleComponent for each picture component
 *                (Y,Cb, or Cr)
 *  Input:
 *  Returns:
 *  Side effects: allocates memory for spatially scaled enhancement
 *                layer picture Y, Cb, and Cr components.
 *
 *  Date: 971102  Author: mikeg@ee.ubc.ca
 *
 ***********************************************************************/
void UpsampleReferenceLayerPicture(void)
{

    int cc, size, x, y;

    for (cc = 0; cc < 3; cc++)
    {
        if (cc == 0)
        {
            size = coded_picture_width * coded_picture_height;
            x = ref_coded_picture_width;
            y = ref_coded_picture_height;
        }
        else
        {
            size = chrom_width * chrom_height;
            x = ref_chrom_width;
            y = ref_chrom_height;
        }

        if (!(upsampled_reference_frame[cc] = (unsigned char *) h263_myMalloc (size)))
            error ("h263_myMalloc failed\n");

        UpsampleComponent(upsampled_reference_frame[cc], current_frame[cc], x, y);
    }
}

/**********************************************************************
 *
 *  Name:         UpsampleComponent
 *  Description:  interpolates in horiz only, vert only or both
 *                vert and horiz depending on spatial scalability
 *                option in use
 *  Input:        pointers to  reference layer and enhancement layer
 *                picture components (Y,Cb,or Cr) and dimensions
 *                of reference layer picture
 *  Returns:
 *  Side effects:
 *
 *  Date: 971102  Author: mikeg@ee.ubc.ca
 *
 ***********************************************************************/
void UpsampleComponent (unsigned char *enhanced, unsigned char *base,
                        int horiz, int vert)
{
    int i,j;
    unsigned char *base_next, *enhanced_next, *enhanced_origin;

    enhanced_origin = enhanced;

    switch (scalability_mode)
    {
        case SPATIAL_SCALABILITY_H:

            /* Rows */
            for( j=0; j<vert; j++ )
            {
                /* First column of rows */
                *enhanced++ = *base;
                for( i=1; i<horiz; i++ )
                {
                    *enhanced++ = (3* *base +    *(base+1) + 2) >> 2;
                    *enhanced++ = (   *base + 3* *(base+1) + 2) >> 2;
                    base++;
                }
                /* Last column of rows */
                *enhanced++ = *base++;
            }

            break;

        case SPATIAL_SCALABILITY_V:

            /* First row */
            for( i=0 ; i<horiz; i++ )
            {
                *enhanced++ = *base++;
            }

            enhanced_next = enhanced + horiz;
            base          = base - horiz;
            base_next     = base + horiz;

            /* Rows */
            for( j=0; j<vert-1; j++ )
            {

                for( i=0; i<horiz; i++ )
                {
                    *enhanced++      = (3* *base +    *(base_next) + 2) >> 2;
                    *enhanced_next++ = (   *base + 3* *(base_next) + 2) >> 2;
                    base++;
                    base_next++;
                }
                enhanced      = enhanced + horiz;
                enhanced_next = enhanced + horiz;
            }

            /* Last row */
            for( i=0 ; i<horiz; i++ )
            {
                *enhanced++ = *base++;
            }

            break;

        case SPATIAL_SCALABILITY_HV:

            /* Top left corner pel */
            *enhanced++  = *base;
            /* First row */
            for( i=1 ; i<horiz; i++ )
            {
                *enhanced++ = (3* *base +    *(base+1) + 2) >> 2;
                *enhanced++ = (   *base + 3* *(base+1) + 2) >> 2;
                base++ ;
            }
            /* Top right corner pel */
            *enhanced++ = *base++;

            enhanced_next = enhanced + (horiz<<1);
            base          = base - horiz;
            base_next     = base + horiz;

            /* Rows */
            for( j=0; j<vert-1; j++ )
            {
                /* First column of rows */
                *enhanced++       = (3* *base +    *(base_next) + 2) >> 2;
                *enhanced_next++  = (   *base + 3* *(base_next) + 2) >> 2;
                for( i=1; i<horiz; i++ )
                {
                    *enhanced++      = (9* *base + 3* *(base+1) + 3* *base_next +
                                        *(base_next+1) + 8) >> 4;
                    *enhanced++      = (3* *base + 9* *(base+1) +    *base_next +
                                        3* *(base_next+1) + 8) >> 4;
                    *enhanced_next++ = (3* *base +    *(base+1) + 9* *base_next +
                                        3* *(base_next+1) + 8) >> 4;
                    *enhanced_next++ = (   *base + 3* *(base+1) + 3* *base_next +
                                           9* *(base_next+1) + 8) >> 4;
                    base++;
                    base_next++;
                }
                /* Last column of rows */
                *enhanced++      = (3* *base +    *(base_next) + 2) >> 2;
                *enhanced_next++ = (   *base + 3* *(base_next) + 2) >> 2;

                enhanced      = enhanced + (horiz<<1);
                enhanced_next = enhanced + (horiz<<1);
                base++;
                base_next++;
            }

            /* Bottom left corner pel */
            *enhanced++  = *base;
            /* Last row */
            for( i=1; i<horiz; i++ )
            {
                *enhanced++ = (3* *base +    *(base+1) + 2) >> 2 ;
                *enhanced++ = (   *base + 3* *(base+1) + 2) >> 2 ;
                base++ ;
            }
            /* Bottom right corner pel */
            *enhanced = *base;

            break;

        default:

            break;
    }
}
#endif

/**********************************************************************
 *
 *  Name:         DisplayPicture
 *  Description:  manages a one frame buffer for re-ordering frames prior
 *                to displaying or writing to a file.
 *  Input:        frame number
 *  Returns:
 *  Side effects:
 *
 *  Date: 971102  Author: mikeg@ee.ubc.ca
 *
 ***********************************************************************/
//void PictureDisplay(int *framenum)change shenh
#if 0
void PictureDisplay(unsigned char * outbuf)
{
    //unsigned char *frame_to_display[3];//change shenh
    //static int     last_frame_disposable = 0, display_buffer_empty = 1;//change shenh
//static int     buffered_frame_temp_ref = 0, display_framenum = 0;//change shenh
    //int            cc, size, update_buffered_frame;//change shenh

#if 0//change shenh
    if (display_buffer_empty)
    {
        /* Store current (initial) frame. */
        update_buffered_frame = ON;
    }
    else
    {
#if 0//change shenh
        if ( (temp_ref < buffered_frame_temp_ref) && (PCT_B == pict_type) )
        {
            /* Do not store current frame. */
            update_buffered_frame = OFF;

            /* Display current frame. */
            display_framenum = *framenum;

            frame_to_display[0] = current_frame[0];
            frame_to_display[1] = current_frame[1];
            frame_to_display[2] = current_frame[2];
        }
        else if ( (PCT_INTRA == pict_type) && (temp_ref < buffered_frame_temp_ref) )
#endif
            if ( (PCT_INTRA == pict_type) && (temp_ref < buffered_frame_temp_ref) )
            {
                /* INTRA refresh with temp_ref reset has occurred,
                 * display frame in buffer and start over */
                display_framenum = buffered_framenum;

                frame_to_display[0] = buffered_frame[0];
                frame_to_display[1] = buffered_frame[1];
                frame_to_display[2] = buffered_frame[2];

                /* Store current frame. */
                update_buffered_frame = ON;
            }
            else
            {
                /* Diplay frame in storage. */
                display_framenum = buffered_framenum;

                frame_to_display[0] = buffered_frame[0];
                frame_to_display[1] = buffered_frame[1];
                frame_to_display[2] = buffered_frame[2];

                /* Store current frame. */
                update_buffered_frame = ON;
            }
    }

    if (!display_buffer_empty)
    {
        /* Post Filter */
        //  if (post_filter) //shenh change
        //    PostFilter (frame_to_display[0], frame_to_display[1], frame_to_display[2],  coded_picture_width, coded_picture_height);

#if 0 //change shenh
        if (expand && outtype == T_X11)
        {
            /* display this image and keep frame already in storage where it is. */
            interpolate_image (frame_to_display[0], exnewframe[0],
                               coded_picture_width, coded_picture_height);
            interpolate_image (frame_to_display[1], exnewframe[1], chrom_width, chrom_height);
            interpolate_image (frame_to_display[2], exnewframe[2], chrom_width, chrom_height);

            storeframe (exnewframe, *framenum);
            /*shenh change
              if (save_frames)
              {
              save_frame (exnewframe, *framenum, recon_file_ptr);
              }*/
            if (pb_frame)
            {

                (*framenum)++;
                interpolate_image (bframe[0], exnewframe[0], coded_picture_width, coded_picture_height);
                interpolate_image (bframe[1], exnewframe[1], chrom_width, chrom_height);
                interpolate_image (bframe[2], exnewframe[2], chrom_width, chrom_height);
                storeframe (exnewframe, *framenum);
                /*shenh change
                  if (save_frames)
                  {
                  save_frame (exnewframe, *framenum, recon_file_ptr);
                  }*/
            }
        }
        else
        {
#endif
            storeframe (frame_to_display, *framenum);
            /*shenh change
              if (save_frames)
              {
              save_frame (frame_to_display, *framenum, recon_file_ptr);
              }*/
            if (pb_frame)
            {
                /*change shenh
                  (*framenum)++;
                  storeframe (bframe, *framenum);
                */
                errorstate=-1;
                return;

                /*shenh change
                  if (save_frames)
                  {
                  save_frame (bframe, *framenum, recon_file_ptr);
                  }*/
            }
            //}

        }

        /* After first decoded frame, the buffer is never again empty, until
         * flushed. */
        if (display_buffer_empty)
        {
            display_buffer_empty = 0;
        }
        if (update_buffered_frame)
        {
            buffered_frame_temp_ref += ( (temp_ref-buffered_frame_temp_ref) < 0 ) ?
                                       (temp_ref-buffered_frame_temp_ref+256) :
                                       (temp_ref-buffered_frame_temp_ref);
            buffered_framenum = *framenum;

            for (cc = 0; cc < 3; cc++)
            {
                if (cc == 0)
                    size = coded_picture_width * coded_picture_height;
                else
                    size = chrom_width * chrom_height;

                memcpy (buffered_frame[cc], current_frame[cc], size);
            }
        }
#else

    storeframe (current_frame, outbuf);

#endif

    }
#endif

#if 0
    void conceal_missing_gobs(int start_mb_row_missing, int number_of_mb_rows_missing)
    {
        /* counters */
        int xpos, ypos;

        int bx,by;
        int end = start_mb_row_missing + number_of_mb_rows_missing;

        errorstate=0;

        for (ypos = start_mb_row_missing; ypos < end; ypos++)
        {
            for (xpos = 0; xpos < mb_width; xpos++)
            {

                bx = 16 * xpos;
                by = 16 * ypos;

                /* Assume mode was Inter */
                modemap[ypos+1][xpos+1] = MODE_INTER;

                /* copy the motion vectors from the GOB above if present */
                /* (at ypos = 0, just use 0 MVs */
                if(!start_mb_row_missing)
                {
                    MV[0][0][ypos + 1][xpos + 1] = 0;
                    MV[1][0][ypos + 1][xpos + 1] = 0;
                }
                else
                {
                    MV[0][0][ypos + 1][xpos + 1] = MV[0][0][start_mb_row_missing][xpos + 1];
                    MV[1][0][ypos + 1][xpos + 1] = MV[1][0][start_mb_row_missing][xpos + 1];
                }

                /* motion compensation for P-frame */
                /* Always assume MBs were INTER coded */
                reconstruct (bx, by, 1, 0, 0, 0, 1);
                if(errorstate)return;
            }
        }
    }
#endif

    void cleandecoder ()
    {
        h263_freeAll();
    }

//move from tmndec.c  shenh
    void initdecoder ()
    {
        int i, size;
        // FILE *cleared;
        errorstate=0;

        if(init_tab_map ()!=0)//add shenh
        {
            errorstate=-1;
            return;
        }

        /* clip table */
#ifdef USE_INTERNAL_SRAM
        clp = (unsigned char *)CLP_START;
#else
        if (!(clp = (unsigned char *) h263_myMalloc (1024)))
        {
            //error ("h263_myMalloc failed\n");
            errorstate=-1;
            return;
        }
#endif

        clp += 384;

        for (i = -384; i < 640; i++)
            clp[i] = (i < 0) ? 0 : ((i > 255) ? 255 : i);

        /* MPEG-1 = TMN parameters */
        matrix_coefficients = 5;

        blk_cnt = 6;

        /* change shenh
           for (i=0; i<MAX_LAYERS; i++)
           enhancement_layer_init[i] = OFF;
        */

        size = coded_picture_width * coded_picture_height;
        if (!(next_I_P_frame[0] = (unsigned char *) h263_myMalloc (size*3/2)))//next yuv frame will putin. shenh
        {
            //error ("h263_myMalloc failed\n");
            errorstate=-1;
            return;
        }
        next_I_P_frame[1]=next_I_P_frame[0]+size;
        next_I_P_frame[2]=next_I_P_frame[0]+(size*5/4);

        if (!(current_frame[0] = (unsigned char *) h263_myMalloc (size*3/2)))//switch address with next_I_P_frame for output yuv.shenh
        {
            //error ("h263_myMalloc failed\n");
            errorstate=-1;
            return;
        }
        current_frame[1]=current_frame[0]+size;
        current_frame[2]=current_frame[0]+(size*5/4);

        prev_frame[0] = current_frame[0];
        prev_frame[1] = current_frame[1];
        prev_frame[2] = current_frame[2];

#if 0
        for (cc = 0; cc < 3; cc++)
        {
            if (cc == 0)
                size = coded_picture_width * coded_picture_height;
            else
                size = chrom_width * chrom_height;

            /* Used for bidirectional and direct prediction mode for true B
             * pictures, one for forward and one for backward. */
            if (!(next_I_P_frame[cc] = (unsigned char *) h263_myMalloc (size)))//next yuv frame will putin. shenh
            {
                //error ("h263_myMalloc failed\n");
                errorstate=-1;
                return;
            }

            /* change shenh
               if (!(prev_I_P_frame[cc] = (unsigned char *) h263_myMalloc (size)))
               {
               //error ("h263_myMalloc failed\n");
               errorstate=-1;
               return;
               }*/
            if (!(current_frame[cc] = (unsigned char *) h263_myMalloc (size)))//switch address with next_I_P_frame for output yuv.shenh
            {
                //error ("h263_myMalloc failed\n");
                errorstate=-1;
                return;
            }

            /* change shenh
               if (!(bframe[cc] = (unsigned char *) h263_myMalloc (size)))
               {
               //error ("h263_myMalloc failed\n");
               errorstate=-1;
               return;
               }
            */

            prev_frame[cc] = current_frame[cc];

            /* change shenh
               if (!(tmp_f[cc] = (unsigned char *) h263_myMalloc (size)))
               {
               //error ("h263_myMalloc failed\n");
               errorstate=-1;
               return;
               }

               if (!(tmp_b[cc] = (unsigned char *) h263_myMalloc (size)))
               {
               //error ("h263_myMalloc failed\n");
               errorstate=-1;
               return;
               }
            */

            /* Stores frame in display buffer */
            /*change shenh
              if (!(buffered_frame[cc] = (unsigned char *) h263_myMalloc (size)))
              {
              //error ("h263_myMalloc failed\n");
              errorstate=-1;
              return;
              }
            */
        }
#endif

#if 0//change shenh
        for (cc = 0; cc < 3; cc++)
        {
            if (cc == 0)
            {
                size = (coded_picture_width + 64) * (coded_picture_height + 64);

                /* Stores framed version of previous inter-picture, luminance. */
                if (!(edgeframeorig[cc] = (unsigned char *) h263_myMalloc (size)))
                {
                    //error ("h263_myMalloc failed\n");
                    errorstate=-1;
                    return;
                }
                edgeframe[cc] = edgeframeorig[cc] + (coded_picture_width + 64) * 32 + 32;

                /* Required for true B frames backward, bidir, and direct
                 * prediction. Stores future inter-picture, luminance */
                /* change shenh
                   if (!(nextedgeframeorig[cc] = (unsigned char *) h263_myMalloc (size)))
                   {
                   //error ("h263_myMalloc failed\n");
                   errorstate=-1;
                   return;
                   }
                   nextedgeframe[cc] = nextedgeframeorig[cc] + (coded_picture_width + 64) * 32 + 32;
                */
            }
            else
            {
                size = (chrom_width + 32) * (chrom_height + 32);

                /* Stores framed version of previous inter-picture, chrominance. */
                if (!(edgeframeorig[cc] = (unsigned char *) h263_myMalloc (size)))
                {
                    //error ("h263_myMalloc failed\n");
                    errorstate=-1;
                    return;
                }
                edgeframe[cc] = edgeframeorig[cc] + (chrom_width + 32) * 16 + 16;

                /* Required for true B frames backward, bidir, and direct
                 * prediction. Stores future inter-picture, chrominance. */
                /* change shenh
                   if (!(nextedgeframeorig[cc] = (unsigned char *) h263_myMalloc (size)))
                   {
                   //error ("h263_myMalloc failed\n");
                   errorstate=-1;
                   return;
                   }
                   nextedgeframe[cc] = nextedgeframeorig[cc] + (chrom_width + 32) * 16 + 16;
                */
            }
        }
#endif

        /* change shenh. expand==0
           if (expand)
           {
           for (cc = 0; cc < 3; cc++)
           {
           if (cc == 0)
           size = coded_picture_width * coded_picture_height * 4;
           else
           size = chrom_width * chrom_height * 4;

           if (!(exnewframe[cc] = (unsigned char *) h263_myMalloc (size)))
           error ("h263_myMalloc failed\n");
           }
           }*/

        /* Clear output file for concatenated storing */
        /* shenh change
           if (outtype == T_YUV_CONC)
           {
           if ((cleared = fopen (outputname, "wb")) == NULL)
           error ("couldn't clear outputfile\n");
           else
           fclose (cleared);
           }*/
        /* IDCT */
        //if (refidct) //change shenh
        //   init_idctref ();
        // else
        if(init_idct ()!=0)
        {
            errorstate=-1;
            return;
        }


        prev_mv_outside_frame = prev_sac = prev_adv_pred = prev_aic = prev_df = 0;
        prev_slice_struct = prev_rps = prev_isd = prev_aivlc = prev_mq = 0;
        prev_4mv = prev_long_vectors = prev_obmc = 0;
    }


    /**********************************************************************
     *
     *  Name:             flushdisplaybuffer
     *  Description:  empties last frame from display buffer when sequence
     *                fully decoded
     *  Input:        frame number
     *  Returns:
     *  Side effects:
     *
     *  Date: 971102  Author: mikeg@ee.ubc.ca
     *
     ***********************************************************************/
//move from tmndec.c shenh
#if 0//change shenh
    void flushdisplaybuffer (int framenum)
    {

        /* Ensure the dimensions are set properly for displaying the last picture in
         * the display buffer */
        coded_picture_width = horizontal_size = lines[base_source_format];
        coded_picture_height = vertical_size = pels[base_source_format];

        mb_width = horizontal_size / 16;
        mb_height = vertical_size / 16;
        chrom_width = coded_picture_width >> 1;
        chrom_height = coded_picture_height >> 1;

#if 0// change shenh
        if (expand && outtype == T_X11)
        {
            /* display this image and keep frame already in storage where it is. */
            interpolate_image (buffered_frame[0], exnewframe[0],
                               coded_picture_width, coded_picture_height);
            interpolate_image (buffered_frame[1], exnewframe[1], chrom_width, chrom_height);
            interpolate_image (buffered_frame[2], exnewframe[2], chrom_width, chrom_height);

            storeframe (exnewframe, framenum);
            /*shenh change
              if (save_frames)
              {
              save_frame (exnewframe, framenum, recon_file_ptr);
              }*/
        }
        else
        {
#endif
            storeframe (buffered_frame, framenum);
            /*shenh change
              if (save_frames)
              {
              save_frame (buffered_frame, framenum, recon_file_ptr);
              }*/
            //}
        }
#endif


