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
#include "h263_global.h"

/* private prototypes */
static void getpicturehdr _ANSI_ARGS_ ((void));

/* decode headers from one input stream until an End of Sequence or
 * picture start code is found */

int getheader ()
{
    unsigned int code, gob;
    errorstate=0;
    /* look for startcode */
    startcode ();
    code = getbits (PSC_LENGTH);
    gob = getbits (5);
    if (gob == SE_CODE)
        return 0;

    if (gob == 0)// 00000 为新图像的开始头
    {
        //开始读新的图像头
        getpicturehdr ();
        if(errorstate)return 0;
        //  if (syntax_arith_coding)    /* reset decoder after receiving */
        //    decoder_reset ();         /* fixed length PSC string */
        if (syntax_arith_coding)
        {
            errorstate=-1;
//          diag_printf("SAC not support. shenh");
            return 0;
        }

    }

    return gob + 1;
}


/* align to start of next startcode */

void startcode ()
{
    /* search for new picture start code */
    while (showbits (PSC_LENGTH) != 1l)
        flushbits (1);
}

/* decode picture header */

static void getpicturehdr ()
{
    int pos, pei, tmp;
    int TRPI = 0;
    int BCI = 0;
    int clock_conversion_code = 0;
    int clock_divisor = 0;
    int extended_temporal_reference = 0;

    errorstate=0;

    UFEP = 0;

    pos = ld->bitcnt;

    temp_ref = getbits (8);
    if (trd < 0)
        trd += 256;

    tmp = getbits (1);            /* always "1" */
    //if (!tmp)
    //  if (!quiet)
    //   diag_printf ("warning: spare in picture header should be \"1\"\n");
    tmp = getbits (1);            /* always "0" */
//  if (tmp)
//   if (!quiet)
//     diag_printf ("warning: H.261 distinction bit should be \"0\"\n");
    tmp = getbits (1);            /* split_screen_indicator */
    if (tmp)
    {
//    if (!quiet)
//      diag_printf ("error: split-screen not supported in this version\n");
        //exit (-1);
        errorstate=-1;
        return;
    }
    tmp = getbits (1);            /* document_camera_indicator */
// if (tmp)
    //  if (!quiet)
    //    diag_printf ("warning: document camera indicator not supported in this version\n");

    tmp = getbits (1);            /* freeze_picture_release */
// if (tmp)
    //  if (!quiet)
    //    diag_printf ("warning: frozen picture not supported in this version\n");
    tmp = getbits (3);//size qcif cif .....
    if (tmp == 0)
    {
//    if (!quiet)
//      diag_printf ("error: source format 000 is reserved and not used in this version\n");
        //exit (-1);
        errorstate=-1;
        return;
    }
    if (tmp == EXTENDED_PTYPE)
    {
        plus_type = 1;
        UFEP = getbits (3);
        if (UFEP == 1)
        {
            /* OPPTYPE */
            source_format = getbits (3);

            /* optional custom picture clock frequency */
            optional_custom_PCF = getbits (1);
            if (optional_custom_PCF)
            {
//        if (!quiet)
//          diag_printf ("error: Optional custom picture clock frequency is not supported in this version\n");
                //exit (-1);
                errorstate=-1;
                return;
            }
            mv_outside_frame = getbits (1);
            long_vectors = (mv_outside_frame ? 1 : 0);
            syntax_arith_coding = getbits (1);
            adv_pred_mode = getbits (1);
            mv_outside_frame = (adv_pred_mode ? 1 : mv_outside_frame);
            overlapping_MC = (adv_pred_mode ? 1 : 0);
            use_4mv = (adv_pred_mode ? 1 : 0);
            pb_frame = 0;
            advanced_intra_coding = getbits (1);
            if (advanced_intra_coding)//add shenh
            {
//        if (!quiet)
//          diag_printf ("error: advanced_intra_coding is not supported for calloc. shenh\n");
                //exit (-1);
                errorstate=-1;
                return;
            }

            deblocking_filter_mode = getbits (1);
            mv_outside_frame = (deblocking_filter_mode ? 1 : mv_outside_frame);
            use_4mv = (deblocking_filter_mode ? 1 : use_4mv);

            slice_structured_mode = getbits (1);
            if (slice_structured_mode)
            {
//        if (!quiet)
//          diag_printf ("error: Slice structured mode is not supported in this version\n");
                //exit (-1);
                errorstate=-1;
                return;
            }
            reference_picture_selection_mode = getbits (1);
            if (reference_picture_selection_mode)//add shenh
            {
//        if (!quiet)
//          diag_printf ("error: ANNEX N Reference Picture Selection mode is not supported for h263_myMalloc\n");
                //exit (-1);
                errorstate=-1;
                return;
            }
            independently_segmented_decoding_mode = getbits (1);
            if (independently_segmented_decoding_mode)
            {
//        if (!quiet)
//          diag_printf ("error: Independently segmented decoding mode is not supported in this version\n");
                //exit (-1);
                errorstate=-1;
                return;
            }
            alternative_inter_VLC_mode = getbits (1);
            modified_quantization_mode = getbits (1);
            tmp = getbits (4);
            if (tmp != 8)
            {
                /* OPPTYPE : bit15=1, bit16,bit17,bit18=0 */
//        if (!quiet)
//          diag_printf ("error: The last 4 bits of OPPTYPE is expected to be 1000\n");
                //exit (-1);
                errorstate=-1;
                return;
            }
        }
        if ((UFEP == 1) || (UFEP == 0))
        {
            if (UFEP == 0)
            {
                if (scalability_mode >= 3)
                {
                    horizontal_size = lines[base_source_format];
                    vertical_size = pels[base_source_format];

                    mb_width = horizontal_size / 16;
                    mb_height = vertical_size / 16;

                    /* Need to store previous (reference layer) values
                     * for interpolation purposes, as the new values,
                     * i.e. of the spatially scaled layer, depend on
                     * the type of spatial scalability in use. */
                    if(coded_picture_width != horizontal_size || coded_picture_height != vertical_size)//shenh add
                    {
//          diag_printf ("error: real h263 horizontal_size=%d vertical_size=%d \n", horizontal_size, vertical_size);
                        //exit (-1);
                        errorstate=-1;
                        return;
                    }
                    ref_coded_picture_width = coded_picture_width = horizontal_size;
                    ref_coded_picture_height = coded_picture_height = vertical_size;
                    ref_chrom_width = chrom_width = coded_picture_width >> 1;
                    ref_chrom_height = chrom_height = coded_picture_height >> 1;

                    source_format = base_source_format;
                }
            }

            /* MMPTYPE */
            pict_type = getbits (3);
            if (pict_type == PCT_IPB)
                pb_frame = IM_PB_FRAMES;
            else
                pb_frame = 0;

            if (PCT_B == pict_type)
            {
                true_B_frame = ON;
                /* Allow motion over picture boundaries, regardless of whether or
                 * not UMV is turned on. */
                mv_outside_frame = 1;
                true_b_trb = temp_ref - prev_non_disposable_temp_ref;
                if (true_b_trb < 0)
                    true_b_trb += 256;
            }
            else
            {
                true_B_frame = OFF;
                prev_non_disposable_temp_ref = next_non_disposable_temp_ref;
                next_non_disposable_temp_ref = temp_ref;
                trd = temp_ref - prev_non_disposable_temp_ref;
                if (trd < 0)
                    trd += 256;
            }

            reference_picture_resampling_mode = getbits (1);
            if (reference_picture_resampling_mode)
            {
//        if (!quiet)
//          diag_printf ("error: Reference picture resampling mode is not supported in this version\n");
                //exit (-1);
                errorstate=-1;
                return;
            }
            reduced_resolution_update_mode = getbits (1);
            if (reduced_resolution_update_mode)
            {
//        if (!quiet)
//          diag_printf ("error: Reduced resolution update mode is not supported in this version\n");
                //exit (-1);
                errorstate=-1;
                return;
            }
            rtype = getbits (1);      /* rounding type */
            tmp = getbits (3);
            if (tmp != 1)
            {
                /* MPPTYPE : bit7,bit8=0  bit9=1 */
                if (!quiet)
                    //exit (-1);
                    errorstate=-1;
                return;
            }
        }
        else
        {
            /* UFEP is neither 001 nor 000 */
//      if (!quiet)
//        diag_printf ("error: UFEP should be either 001 or 000.\n");
            //exit (-1);
            errorstate=-1;
            return;
        }

        tmp = getbits (1);
        if (tmp)
        {
//      if (!quiet)
//        diag_printf ("error: CPM not supported in this version\n");
            //exit (-1);
            errorstate=-1;
            return;
        }

        if (UFEP && (source_format == SF_CUSTOM))
        {
            /* Read custom picture format */
            CP_PAR_code = getbits (4);
            if (CP_PAR_code != PAR_CIF)
            {
//        if (!quiet)
//        {
//          diag_printf ("error: only 12:11 pixel aspect ratio supported ");
//        }
                //exit (-1);
                errorstate=-1;
                return;
            }
            tmp=getbits (9);
            horizontal_size = (tmp + 1 ) * 4;
            tmp = getbits (1);
            if (!tmp)
            {
//        if (!quiet)
//          diag_printf ("error: The 14th bit of Custom Picture Format(CPFMT) should be 1\n");
//    //exit (-1);
                errorstate=-1;
                return;
            }
            tmp = getbits (9);
            vertical_size = tmp * 4;
            if ((horizontal_size%16) || (vertical_size%16))
            {
//        if(!quiet)
//        {
//          diag_printf ("error: only factor of 16 custom source format supported\n");
//        }
                //exit (-1);
                errorstate=-1;
                return;
            }

            if (CP_PAR_code == EXTENDED_PAR)
            {
                PAR_width = getbits (8);
                PAR_height = getbits (8);
            }
        }

        if (source_format != SF_CUSTOM)
        {
            horizontal_size = lines[source_format];
            vertical_size = pels[source_format];

        }
        mb_width = horizontal_size / 16;
        mb_height = vertical_size / 16;

        /* Need to store previous (reference layer) values
         * for interpolation purposes, as the new values,
         * i.e. of the spatially scaled layer, depend on
         * the type of spatial scalability in use. */
        ref_coded_picture_width = coded_picture_width;
        coded_picture_width = horizontal_size;

        ref_coded_picture_height = coded_picture_height;
        coded_picture_height = vertical_size;

        ref_chrom_width = chrom_width;
        chrom_width = coded_picture_width >> 1;

        ref_chrom_height = chrom_height;
        chrom_height = coded_picture_height >> 1;

        if (optional_custom_PCF)
        {
            if (UFEP)
            {
                clock_conversion_code = getbits (1);
                clock_divisor = getbits (7);
                CP_clock_frequency = 18000000 / (clock_divisor * (8 + clock_conversion_code)) ;
            }
            /* regardless of the value of UFEP */
            extended_temporal_reference = getbits (2);
            temp_ref = (extended_temporal_reference << 8) + temp_ref;

            if (PCT_B == pict_type)
            {
                true_b_trb = temp_ref - prev_non_disposable_temp_ref;
            }
            else
            {
                trd = temp_ref - prev_non_disposable_temp_ref;
            }

            if (trd < 0)
                trd += 1024;
        }
        if (UFEP && long_vectors)
        {
            if (getbits(1))
            {
                unlimited_unrestricted_motion_vectors = 0;
            }
            else
            {
                flushbits(1);
                unlimited_unrestricted_motion_vectors = 1;
            }
        }
        if (UFEP && slice_structured_mode)
        {
            SSS_rectangular_slices = getbits (1);
            SSS_arbitary_slice_ordering = getbits (1);
        }

        if ((pict_type == PCT_B) || (pict_type == PCT_EI) || (pict_type == PCT_EP))
        {
            /* optional temporal, SNR and spatial scalability mode in use */
            enhancement_layer_num = getbits (4);
            if (UFEP)
            {
                reference_layer_number = getbits (4);
            }
            if (1 != enhancement_layer_num)
            {
                if (source_format != base_source_format)
                {
                    if (source_format != SF_CUSTOM)
                    {
                        scalability_mode = SPATIAL_SCALABILITY_HV;
                    }
                    else if (coded_picture_width != ref_coded_picture_width)
                    {
                        scalability_mode = SPATIAL_SCALABILITY_H;
                    }
                    else
                    {
                        scalability_mode = SPATIAL_SCALABILITY_V;
                    }
                }
                else
                {
                    scalability_mode = SNR_SCALABILITY;
                }
            }
        }
        else
        {
            enhancement_layer_num = reference_layer_number = 1;
        }

        if (reference_picture_selection_mode)
        {
            if (reference_picture_selection_mode)//add shenh
            {
//        if (!quiet)
//          diag_printf ("error: ANNEX N Reference Picture Selection mode is not supported for h263_myMalloc\n");
                //exit (-1);
                errorstate=-1;
                return;
            }
            if (UFEP)
            {
                MF_of_reference_picture_selection = getbits (3);
            }
            TRPI = getbits (1);
            if (TRPI)
            {
                /* temporal reference for prediction exists */

                temporal_reference_for_prediction = getbits (10);
            }
            if (showbits (1))
            {
                /* backward channel message exists */
                /* BCM(backward channel message) is not implemented */
                flushbits (1);
//        if (!quiet)
//          diag_printf ("error: BCM(backward channel message) is not implemented in this version\n");
                //exit (-1);
                errorstate=-1;
                return;
            }
            else
            {
                /* BCM is not present */
                BCI = getbits (2);
            }
        }
        if (reference_picture_resampling_mode)
        {
            /* reading RPRP info is not implemented */
//      if (!quiet)
//        diag_printf ("error: RPRP reading is not implemented in this version\n");
            //exit (-1);
            errorstate=-1;
            return;
        }
        pic_quant = getbits (5);
        quant = pic_quant;

    }
    else
    {
        plus_type = 0;
        rtype = 0;
        source_format = tmp;
        enhancement_layer_num = reference_layer_number = 1;

        horizontal_size = lines[source_format];
        vertical_size = pels[source_format];

        mb_width = horizontal_size / 16;
        mb_height = vertical_size / 16;

        /* Need to store previous (reference layer) values
         * for interpolation purposes, as the new values,
         * i.e. of the spatially scaled layer, depend on
         * the type of spatial scalability in use. */
        if(coded_picture_width != horizontal_size || coded_picture_height != vertical_size)//shenh add
        {
//      diag_printf ("error: real h263 horizontal_size=%d vertical_size=%d \n", horizontal_size, vertical_size);
            //exit (-1);
            errorstate=-1;
            return;
        }
        ref_coded_picture_width = coded_picture_width = horizontal_size;
        ref_coded_picture_height = coded_picture_height = vertical_size;
        ref_chrom_width = chrom_width = coded_picture_width >> 1;
        ref_chrom_height = chrom_height = coded_picture_height >> 1;

        true_B_frame = OFF;
        prev_non_disposable_temp_ref = next_non_disposable_temp_ref;
        next_non_disposable_temp_ref = temp_ref;
        trd = temp_ref - prev_non_disposable_temp_ref;
        if (trd < 0)
            trd += 256;

        pict_type = getbits (1);
        mv_outside_frame = getbits (1);
        long_vectors = (mv_outside_frame ? 1 : 0);
        syntax_arith_coding = getbits (1);
        adv_pred_mode = getbits (1);
        mv_outside_frame = (adv_pred_mode ? 1 : mv_outside_frame);
        overlapping_MC = (adv_pred_mode ? 1 : 0);
        use_4mv = (adv_pred_mode ? 1 : 0);
        pb_frame = getbits (1);

        pic_quant = getbits (5);
        quant = pic_quant;


        tmp = getbits (1);
        if (tmp)
        {
//      if (!quiet)
//        diag_printf ("error: CPM not supported in this version\n");
            //exit (-1);
            errorstate=-1;
            return;
        }
    }

    if (pb_frame)
    {
        if (optional_custom_PCF)
        {
            trb = getbits (5);
        }
        else
        {
            trb = getbits (3);
        }

        bquant = getbits (2);
    }
    else
    {
        trb = 0;
    }


    pei = getbits (1);

pspare:
    if (pei)
    {
        /* extra info for possible future backward compatible additions */
        getbits (8);                /* not used */
        pei = getbits (1);
        if (pei)
            goto pspare;              /* keep on reading pspare until pei=0 */
    }

#if 0//change shenh
    if (verbose > 0)
    {
        /* $printf("picture header (byte %d)\n",(pos>>3)-4);$ */
        if (verbose > 1)
        {
//      diag_printf ("  temp_ref=%d\n", temp_ref);
            /* $printf("  pict_type=%d\n",pict_type); diag_printf("
             * source_format=%d\n", source_format); diag_printf("
             * quant=%d\n",quant); if (syntax_arith_coding) diag_printf("  SAC coding
             * mode used \n"); if (mv_outside_frame) diag_printf("  unrestricted
             * motion vector mode used\n"); if (adv_pred_mode) diag_printf("
             * advanced prediction mode used\n");$ */
            if (pb_frame)
            {
                /* $printf("  pb-frames mode used\n");$ */
//        diag_printf ("  trb=%d\n", trb);
                /* $printf("  bquant=%d\n", bquant);$ */
            }
        }
    }
#endif
}

/* get the GOB header */
/* author: <guyc@ece.ubc.ca */
void getgobheader()
{
    // int BCI = 0;
    errorstate=0;
    /* GFID is not allowed to change unless PTYPE in picture header
    * changes */
    gfid = getbits (2);//图像的PTYPE改变，gfid(gob frame ID)才会改变
    /* NB: in error-prone environments the decoder can use this
    * value to determine whether a picture header where the PTYPE
    * has changed, has been lost */
    /* we now use it and assume that a picture change is *
    * from INTRA to INTER or INTER to INTRA only */
    if (gfid != pgfid && newgob)
    {
        if (pict_type == PCT_INTRA)
            pict_type = PCT_INTER;
        else if (pict_type == PCT_INTER)
            pict_type = PCT_INTRA;
    }

    quant = getbits (5);//块组量化信息1-31
    /* Support for Annex N at the GOB level */

#if 0
    if (reference_picture_selection_mode)
    {
        //add shenh
//  if (!quiet)
//  diag_printf ("error: ANNEX N Reference Picture Selection mode is not supported for h263_myMalloc\n");
        errorstate=-1;
        return;
#if 0
        TRI = getbits (1);
        if (TRI)
        {
            /* Temporal reference of GOB */
            temp_ref = optional_custom_PCF?getbits (10):getbits(8);
        }
        /* Temporal Reference for Prediction indication bit */
        TRPI = getbits (1);
        if (TRPI)
        {
            /* temporal reference for prediction exists */
            temporal_reference_for_prediction = getbits (10);
        }
        if (showbits (1))
        {
            /* backward channel message exists */
            /* BCM(backward channel message) is not implemented */
            flushbits (1);
//  if (!quiet)
//  diag_printf ("error: BCM(backward channel message) is not implemented in this version\n");
            //exit (-1);
            errorstate=-1;
            return;
        }
        else
        {
            /* BCM is not present */
            BCI = getbits (2);
        }
#endif
    }
#endif

    pgfid = gfid;
}
void initoptions ()
{
    mv_outside_frame = 0;
    syntax_arith_coding = 0;
    adv_pred_mode = 0;
    pb_frame = 0;
    long_vectors = 0;

    plus_type = 0;
    optional_custom_PCF = 0;
    advanced_intra_coding = 0;
    deblocking_filter_mode = 0;
    slice_structured_mode = 0;
    reference_picture_selection_mode = 0;
    independently_segmented_decoding_mode = 0;
    alternative_inter_VLC_mode = 0;
    modified_quantization_mode = 0;
    reduced_resolution_update_mode = 0;
    reference_picture_resampling_mode = 0;
    rtype = 0;
}
