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







#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <math.h>
#ifndef WIN32
//#include <sys/time.h>
#else
#include <time.h>
#endif

//#include "xvid.h"
#include "image.h"
#include "mpeg4_global.h"
#include "xvid_decraw.h"
#include "malloc.h"

/*****************************************************************************
 *               Global vars in module and constants
 ****************************************************************************/

//#define USE_PNM 0
//#define USE_TGA 1

static int XDIM = 0;
static int YDIM = 0;
//static int ARG_SAVEDECOUTPUT = 0;
//static int ARG_SAVEMPEGSTREAM = 0;
//static char *ARG_INPUTFILE = NULL;
//static char *ARG_OUTPUTFILE = NULL;
//static int CSP = XVID_CSP_I420;
//static int BPP = 1;
//static int FORMAT = USE_PNM;

//static char filepath[256] = "./";
static void *dec_handle = NULL;

//#define BUFFER_SIZE (2*176*144)

//static const int display_buffer_bytes = 0;

//#define MIN_USEFUL_BYTES 1
//static int num = 0;
//FILE * f;
//#define uint16 unsigned int
//unsigned char out_buffer[50688] = {0};//QCIF-RGB565: 176*144*2
//#define MP4_SIZE (599338)
//unsigned char dongwl_mp4[]=
//{
//  #include "carphone_ion.tab"
//};
//int dwl_display=0;
//extern int frame_number;//test by longyl
/*****************************************************************************
 *               Local prototypes
 ****************************************************************************/

// double msecond();
/*
static int dec_init(int use_assembler, int debug_level);
static int dec_main(unsigned char *istream,
                    unsigned char *ostream,
                    int istream_size,
                    xvid_dec_stats_t *xvid_dec_stats);
static int dec_stop();
*/
//static void usage();
//static int write_image(int8 *prefix, uint8 *image);
//static int write_pnm(int8 *filename, uint8 *image);
//static int write_tga(int8 *filename, uint8*image);

const int8 * type2str(int type)
{
    if (type==XVID_TYPE_IVOP)
        return "I";
    if (type==XVID_TYPE_PVOP)
        return "P";
    if (type==XVID_TYPE_BVOP)
        return "B";
    return "S";
}

/*****************************************************************************
 *        Main program
 ****************************************************************************/
#if 0
int xvid_dec(void)
{
    unsigned char *mp4_buffer = NULL;
    unsigned char *mp4_ptr    = NULL;
//  unsigned char *out_buffer = NULL;
    int useful_bytes;
    xvid_dec_stats_t xvid_dec_stats;

    int status;
    int use_assembler = 0;
    int debug_level = 0;

    char filename[256];

    FILE *in_file;
    int filenr;
    int i;
    /*****************************************************************************
     * Command line parsing
     ****************************************************************************/
//  frame_number = 0;//test by longyl

    unsigned int main_dectime;

    /*****************************************************************************
     * Values checking
     ****************************************************************************/

#if 0
    CSP = XVID_CSP_RGB565;
    BPP = 2;
    FORMAT = USE_TGA;
#endif
    /*****************************************************************************
     *        Memory allocation
     ****************************************************************************/
    mp4_buffer =dongwl_mp4;
    /* Memory for encoded mp4 stream */
    mp4_ptr = mp4_buffer;

    /*****************************************************************************
     *        XviD PART  Start
     ****************************************************************************/

    status = dec_init(use_assembler, debug_level);
    if (status)
    {
        goto release_all;
    }


    /*****************************************************************************
     *                           Main loop
     ****************************************************************************/

    /* Fill the buffer */
    useful_bytes =MP4_SIZE;

    filenr = 0;
    mp4_ptr = mp4_buffer;

    do
    {
        int used_bytes = 0;

        /* This loop is needed to handle VOL/NVOP reading */
        do
        {

//          time_measure_start();

            /* Update mp4_ptr */
            used_bytes = dec_main(mp4_ptr, out_buffer, useful_bytes, &xvid_dec_stats);

//          main_dectime = (time_measure_end())/30000;
            /* read new data */

//          temp_ind += 1;
            /*          if (temp_ind>80){
                            put_string("dongwl-end", 10);
                            put_enter();
                            while(1);
                        }
            */

            /*
                        put_string("decode frame ",13);
                        put_init_data(temp_ind);
                        put_string(" end, time(ms): ",16);
                        put_init_data(main_dectime);
                        put_enter();
            */

            if(used_bytes == -1)//profile unfit
                goto release_all;
            /* Resize image buffer if needed */
            if(xvid_dec_stats.type == XVID_TYPE_VOL)
            {
                /* Check if old buffer is smaller */
                if(XDIM*YDIM < xvid_dec_stats.data.vol.width*xvid_dec_stats.data.vol.height)
                {

                    /* Copy new witdh and new height from the vol structure */
                    XDIM = xvid_dec_stats.data.vol.width;
                    YDIM = xvid_dec_stats.data.vol.height;
                }
            }

            /* Update buffer pointers */
            if(used_bytes > 0)
            {
                mp4_ptr += used_bytes;
                useful_bytes -= used_bytes;
            }
        }
        while (xvid_dec_stats.type <= 0 && useful_bytes > MIN_USEFUL_BYTES);

        /* Check if there is a negative number of useful bytes left in buffer
         * This means we went too far */
        if(useful_bytes < 0)
            break;

//      dongwl_mp4display( out_buffer );
#if 0
        time_measure_start();

        dongwl_mp4display( out_buffer );
        dwl_display= (time_measure_end())/30000;
        put_string("  display ",10);
        put_init_data(dwl_display);
        put_enter();
#endif
        filenr++;
        num = filenr;

    }
    while (useful_bytes>MIN_USEFUL_BYTES );

    useful_bytes = 0; /* Empty buffer */

    /*****************************************************************************
     *     Flush decoder buffers
     ****************************************************************************/

    do
    {

        /* Fake vars */
        int used_bytes;

        do
        {

            used_bytes = dec_main(NULL, out_buffer, -1, &xvid_dec_stats);

        }
        while(used_bytes>=0 && xvid_dec_stats.type <= 0);

        if (used_bytes < 0)     /* XVID_ERR_END */
        {
            break;
        }

        /* Save output frame if required */

        filenr++;

    }
    while(1);

    /*****************************************************************************
     *      XviD PART  Stop
     ****************************************************************************/

release_all:
    if (dec_handle)
    {
        status = dec_stop();
    }

free_all_memory:

    return(0);
}
#endif

/*****************************************************************************
 *              output functions
 ****************************************************************************/
#if 0

static int write_image(char *prefix, unsigned char *image)
{
    int i;

    if(!f)
    {
        f = fopen(prefix,"wb");
    }
    fwrite(image, 1, XDIM*YDIM, f);

    for (i=0; i<YDIM/2; i++)
    {
        fwrite(image+XDIM*YDIM + i*XDIM/2, 1, XDIM/2, f);
    }
    for (i=0; i<YDIM/2; i++)
    {
        fwrite(image+5*XDIM*YDIM/4 + i*XDIM/2, 1, XDIM/2, f);
    }
    return(0);
}

#endif

/*****************************************************************************
 * Routines for decoding: init decoder, use, and stop decoder
 ****************************************************************************/

/* init decoder before first run */
int
dec_init(int use_assembler, int debug_level)
{
    int ret;
    xvid_gbl_init_t   xvid_gbl_init;
    xvid_dec_create_t xvid_dec_create;

    /* Reset the structure with zeros */
    memset(&xvid_gbl_init, 0, sizeof(xvid_gbl_init_t));
    memset(&xvid_dec_create, 0, sizeof(xvid_dec_create_t));
    dec_handle=NULL;

    /*------------------------------------------------------------------------
     * XviD core initialization
     *----------------------------------------------------------------------*/

    /* Version */
    xvid_gbl_init.version = XVID_VERSION;

    xvid_gbl_init.cpu_flags = XVID_CPU_FORCE;

    xvid_gbl_init.debug = debug_level;

    xvid_global(NULL, 0, &xvid_gbl_init, NULL);

    /*------------------------------------------------------------------------
     * XviD encoder initialization
     *----------------------------------------------------------------------*/

    /* Version */
    xvid_dec_create.version = XVID_VERSION;

    /*
     * Image dimensions -- set to 0, xvidcore will resize when ever it is
     * needed
     */
    xvid_dec_create.width = 0;
    xvid_dec_create.height = 0;

    ret = xvid_decore(NULL, XVID_DEC_CREATE, &xvid_dec_create, NULL);
    if(ret!=0) return ret;

    dec_handle = xvid_dec_create.handle;
    return(ret);
}

/* decode one frame  */
int
dec_main(uint8 *istream,
         uint8 *ostream,
         int istream_size,
         xvid_dec_stats_t *xvid_dec_stats)
{

    int ret;
    xvid_dec_frame_t xvid_dec_frame;

    /* Reset all structures */
    memset(&xvid_dec_frame, 0, sizeof(xvid_dec_frame_t));
    memset(xvid_dec_stats, 0, sizeof(xvid_dec_stats_t));

    /* Set version */
    xvid_dec_frame.version = XVID_VERSION;
    xvid_dec_stats->version = XVID_VERSION;

    /* No general flags to set */
    xvid_dec_frame.general          = 0;

    /* Input stream */
    xvid_dec_frame.bitstream        = istream;
    xvid_dec_frame.length           = istream_size;

    /* Output frame structure */
    xvid_dec_frame.output.plane[0]  = ostream;
    xvid_dec_frame.output.stride[0] = XDIM;
//  xvid_dec_frame.output.csp = CSP;

    ret = xvid_decore(dec_handle, XVID_DEC_DECODE, &xvid_dec_frame, xvid_dec_stats);

    if(xvid_dec_stats->type == XVID_TYPE_VOL)
    {
        /* Check if old buffer is smaller */
        if(XDIM*YDIM < xvid_dec_stats->data.vol.width*xvid_dec_stats->data.vol.height)
        {

            /* Copy new witdh and new height from the vol structure */
            XDIM = xvid_dec_stats->data.vol.width;
            YDIM = xvid_dec_stats->data.vol.height;
        }
    }
    return(ret);
}

/* close decoder to release resources */
int
dec_stop()
{
    int ret;
    ret=0;
    if(dec_handle)
    {
        ret = xvid_decore(dec_handle, XVID_DEC_DESTROY, NULL, NULL);
        dec_handle=NULL;
    }
    xvid_freeAll();

    return(ret);
}

