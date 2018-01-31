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
#include <string.h>

#include "voc2_library.h"

#include "vppp_audiojpegenc_common.h"
#include "vpp_audiojpegenc.h"
#include "vppp_audiojpegenc_asm_map.h"
#include "vppp_audiojpegenc_asm.h"


VPP_audiojpeg_enc_DEC_OUT_T  g_vppaudiojpeg_encDecStatus;
VPP_audiojpeg_enc_DEC_IN_T   g_vppaudiojpeg_encDecIn;
int Dec_inStreamBufAddr_Jpeg;
int Dec_inStreamBufAddr;
int Dec_inStreamBufStartAddr;
int Dec_inStreamBufEndAddr;

void VoC_BCPU_IRQ_handler() {;}   //Xuml for audiojpeg_enc_voc_decoder
//#define BINARY_SIM


int *Test_Sequence=NULL;
int Test_Sequence_Size=0;


static int mp3_out[]=
{
//#include "..\work\m71.tab"//momo
#include "..\work\a3.tab"//stereo
};
const int amrout[]=
{
    //#include "jout.tab"
    //#include "carphone_out.tab"
#include "..\work\amr_out.tab"
};
const int jpegout[]=
{
#include "..\work\test.tab"
};


void VoC_load_extern(int * source, int word_size, int word_start_addr);



int Input_tab_offset=0;

int intsLeft, nRead,eofReached;
char *outfileName;
FILE *outfile;
FILE *infile;

int i_xuml;
int framecount=0;
int endframe=600;
int ErrorFrameCount=0;
Codecmode Multimedia_Mode;
short outbuf0[2048];
int coef[1024];

#ifdef ROTATE_TEST

int width,height; //w mutiple of 8, h mulitple of 8
int dst_width,dst_height;
int mb_width, mb_height;
int direction;  //1: flip right 90; -1: flip left 90
int stride;

#endif



static int Coolsand_FillReadBuffer(int bufSize, int intsLeft)
{
    int nRead=Test_Sequence_Size-Input_tab_offset;
    bufSize=Input_buffer_size;

    /* move last, small chunk from end of buffer to start, then fill with new data */
    memmove(RAM_EXTERN+((Dec_inStreamBufStartAddr&0x3fffffff)>>2),
            RAM_EXTERN+((Dec_inStreamBufAddr&0x3fffffff)>>2), intsLeft*4);

    if(nRead<bufSize-intsLeft)
    {
        VoC_load_extern(Test_Sequence+Input_tab_offset, nRead,
                        ((Dec_inStreamBufStartAddr&0x3fffffff)>>2)+intsLeft);
        /* zero-pad to avoid finding false sync word after last frame (from old data in readBuf) */
        memset(RAM_EXTERN+((Dec_inStreamBufStartAddr&0x3fffffff)>>2)+intsLeft+nRead,
               0, (bufSize - intsLeft - nRead)*4);

    }
    else
    {
        VoC_load_extern(Test_Sequence+Input_tab_offset, bufSize-intsLeft,
                        ((Dec_inStreamBufStartAddr&0x3fffffff)>>2)+intsLeft);
        nRead=bufSize-intsLeft;
    }

    Dec_inStreamBufAddr=(Input_buffer_START<<2)|0x00000000;
    Input_tab_offset=Input_tab_offset+nRead;

    return nRead;
}


void write_header(int quality_select,short width,short height)
{

    fwrite(head1,20,1,outfile);// memcpy(&RAM_EXTERN[RAME_HEADER1_ADDR],head1,20);

    if (quality_select==0)
        fwrite(head_q_l,136,1,outfile);//memcpy(&RAM_EXTERN[RAME_HEAD_Q_ADDR],head_q_l,136);
    else
        fwrite(head_q_h,136,1,outfile);//memcpy(&RAM_EXTERN[RAME_HEAD_Q_ADDR],head_q_h,136);


    head_size_128_411[3]=height>>8;
    head_size_128_411[4]=height&0xff;
    head_size_128_411[5]=width>>8;
    head_size_128_411[6]=width&0xff;



    if (width<=350)
    {
        head_size_128_411[9] = 0x21;//head_size_128_411[9] = 0x21;//
    }
    else if (width<=700)
    {
        head_size_128_411[9] = 0x21;//422
    }
    else if(width==1280 && height==1024)
    {
        head_size_128_411[9] = 0x21;//422
    }
    else
    {
        head_size_128_411[9] = 0x21;//422
        //  printf("wrong size\n");
        //  return;
    }

    fwrite(head_size_128_411,10,1,outfile);

    fwrite(head_4,441,1,outfile);//memcpy(&RAM_EXTERN[RAME_HEAD4_ADDR],head_4,443);


    // memcpy(RAM_EXTERN,Buff,TOTAL422_SIZE);
    fflush(outfile);// fwrite(RAM_EXTERN,607,1,pout);
    // return 0;
}

void VoC_XCPU_IRQ_handler()
{
    static int  mpc_cc = 0;
    static int cc=0;
    int i;
    char *desty,*destu,*destv;
    printf("frame:%d\n",framecount);
    //  if(framecount==3)
    //  {
    //      printf("ok\n");
    //  }
#ifndef ROTATE_TEST
    if(framecount)
    {

        if(g_vppaudiojpeg_encDecStatus.ErrorStatus==0)
        {
            //test the output
            switch(Multimedia_Mode)
            {
                case MP3_ENCODE:
                    vpp_audiojpeg_encDecStatus(&g_vppaudiojpeg_encDecStatus);

                    printf(" consumedLen:%d\n",g_vppaudiojpeg_encDecStatus.consumedLen);
                    printf(" output_len:%d\n",g_vppaudiojpeg_encDecStatus.output_len);
                    printf(" mode:%d\n",g_vppaudiojpeg_encDecStatus.mode);

                    Dec_inStreamBufAddr=((Dec_inStreamBufAddr&0x3fffffff)+(g_vppaudiojpeg_encDecStatus.consumedLen))|0x00000000;
                    intsLeft = ((Dec_inStreamBufEndAddr&0x3fffffff)-(Dec_inStreamBufAddr&0x3fffffff))>>2;

                    for (i=0; i<g_vppaudiojpeg_encDecStatus.output_len/4; i++)
                    {
                        if (mp3_out[mpc_cc+i]!=RAM_EXTERN[Output_buffer_START+i])
                        {
                            printf("mp3 wrong");
                        }
                    }
                    printf("\n");
                    mpc_cc += g_vppaudiojpeg_encDecStatus.output_len/4;

                    break;
                case AMR_ENCODE:
                    vpp_audiojpeg_encDecStatus(&g_vppaudiojpeg_encDecStatus);

                    printf(" consumedLen:%d\n",g_vppaudiojpeg_encDecStatus.consumedLen);
                    printf(" output_len:%d\n",g_vppaudiojpeg_encDecStatus.output_len);
                    printf(" mode:%d\n",g_vppaudiojpeg_encDecStatus.mode);

                    Dec_inStreamBufAddr=((Dec_inStreamBufAddr&0x3fffffff)+(g_vppaudiojpeg_encDecStatus.consumedLen))|0x00000000;
                    intsLeft = ((Dec_inStreamBufEndAddr&0x3fffffff)-(Dec_inStreamBufAddr&0x3fffffff))>>2;

//              for (i=0;i<8;i++)
//              {
//
//                  if (amrout[cc + i] != RAM_EXTERN[Output_buffer_START+i])
//                  {
//                      printf("amr wrong");
//                  }
//              }
//              printf("\n");

                    cc += 8;
                    if (outfile)
                    {
                        fwrite(&RAM_EXTERN[Output_buffer_START],1,g_vppaudiojpeg_encDecStatus.output_len,outfile);
                    }
                    fflush(outfile);
                    break;
                case MJPEG_ENCODE:

                    vpp_audiojpeg_encDecStatus(&g_vppaudiojpeg_encDecStatus);

                    printf(" consumedLen:%d\n",g_vppaudiojpeg_encDecStatus.consumedLen);
                    printf(" output_len:%d\n",g_vppaudiojpeg_encDecStatus.output_len);
                    printf(" mode:%d\n",g_vppaudiojpeg_encDecStatus.mode);
                    {
                        unsigned char* p_out = (unsigned char*)&RAM_EXTERN[Output_buffer_START];


//                  printf("jpeg test......\n");
//                  for (i=0;i<(g_vppaudiojpeg_encDecStatus.output_len-607);i++)
//                  {
//                      if (*p_out++!=jpegout[607+i])
//                      {
//                          printf("jpeg wrong");
//                      }
//                  }
//                  printf("\n");

                        write_header(g_vppaudiojpeg_encDecIn.imag_quality,g_vppaudiojpeg_encDecIn.imag_width,g_vppaudiojpeg_encDecIn.imag_height);
                        fwrite(&RAM_EXTERN[Output_buffer_START],1,g_vppaudiojpeg_encDecStatus.output_len-607,outfile);
                        fflush(outfile);
                        //  return;
                    }
                    break;
                default:
                    break;
            }


        }
        else
        {
            ErrorFrameCount++;
            printf("[MMC_audiojpeg_enc]##WARNING##Error Frame!:%d\n",ErrorFrameCount);
            if(ErrorFrameCount>20)
            {
                printf("[MMC_audiojpeg_enc]##WARNING##ErrorFrameCount>20!\n");
                return;
            }
        }


    }
    else
    {
        char amr122ring_head[6]= {0x23,0x21,0x41,0x4d,0x52,0x0a};
        if (Multimedia_Mode==AMR_ENCODE)
        {
            fwrite(amr122ring_head,1,6,outfile);
        }
        g_vppaudiojpeg_encDecIn.reset=1;
    }

//  if (framecount)
//  {
//      if (Multimedia_Mode==MJPEG_ENCODE) //else endframe=600;
//      {
//          Multimedia_Mode=AMR_ENCODE;
//      }
//      else if(Multimedia_Mode==AMR_ENCODE)
//      {
//          Multimedia_Mode=MJPEG_ENCODE;
//      }
//  }

    if (Multimedia_Mode==MJPEG_ENCODE) //else endframe=600;
    {
        endframe=1;
        memmove(&RAM_EXTERN[0x20000/2],Test_Sequence, Test_Sequence_Size*4);
        Dec_inStreamBufAddr_Jpeg=(0x20000/2<<2)|0x00000000;
    }
    else if(Multimedia_Mode==AMR_ENCODE)
    {
        endframe=20;
    }
    else if(Multimedia_Mode==MP3_ENCODE)
    {
        endframe=7;
    }
    else
    {
        printf("Error CODEC Mode!");
        return;
    }



    /* somewhat arbitrary trigger to refill buffer - should always be enough for a full frame */
    if (Multimedia_Mode!=MJPEG_ENCODE&&intsLeft < Input_buffer_size/2 && !eofReached)
    {
        nRead = Coolsand_FillReadBuffer(Input_buffer_size, intsLeft);
        if (nRead == 0)
            eofReached = 1;
    }

    g_vppaudiojpeg_encDecIn.mode=Multimedia_Mode;

    g_vppaudiojpeg_encDecIn.inStreamBufAddr=Dec_inStreamBufAddr;
    if (Multimedia_Mode==MJPEG_ENCODE)
    {
        g_vppaudiojpeg_encDecIn.inStreamBufAddr=Dec_inStreamBufAddr_Jpeg;
    }
    g_vppaudiojpeg_encDecIn.outStreamBufAddr=(Output_buffer_START)<<2;
    g_vppaudiojpeg_encDecIn.SampleRate=(480<<16)|640;//(96<<16)|128;//(120<<16)|160;
    g_vppaudiojpeg_encDecIn.BitRate=128;
    g_vppaudiojpeg_encDecIn.imag_width=320;
    g_vppaudiojpeg_encDecIn.imag_height=240;
    g_vppaudiojpeg_encDecIn.imag_quality=1;
    g_vppaudiojpeg_encDecIn.channel_num = 0;//jpeg:rgb:0; yuv:1;   mp3:1-single  2-double
    g_vppaudiojpeg_encDecIn.reset=1;

    if (g_vppaudiojpeg_encDecStatus.streamStatus!=VPP_AudioJpeg_Enc_STREAM_STOP&&(framecount<endframe))
    {
        vpp_audiojpeg_encDecScheduleOneFrame(&g_vppaudiojpeg_encDecIn);
    }
#else


    if(Multimedia_Mode==YUV_ROTATE)
    {

        if(framecount)
        {
            desty=(char*)&RAM_EXTERN[0x20000];
            destu=(char*)(desty+dst_width*dst_height);
            destv=(char*)(desty+dst_width*dst_height*5/4);
            fwrite(desty,1,dst_width*dst_height,outfile);
            fwrite(destu,1,dst_width*dst_height/4,outfile);
            fwrite(destv,1,dst_width*dst_height/4,outfile);
            fflush(outfile);

        }

        endframe=100;

        fread(&RAM_EXTERN[0x8000],4,stride*height/2,infile);

        memset(&RAM_EXTERN[0x20000],0,dst_width*dst_height*3/2);

        RAM_X[(YUV_ROTATE_X_INPUT_ADDR-RAM_X_BEGIN_ADDR)/2]=0X8000*4;
        RAM_X[(YUV_ROTATE_X_OUTPUT_YADDR-RAM_X_BEGIN_ADDR)/2]=0x20000*4;
        RAM_X[(YUV_ROTATE_X_OUTPUT_UADDR-RAM_X_BEGIN_ADDR)/2]=0x20000*4+dst_width*dst_height;
        RAM_X[(YUV_ROTATE_X_OUTPUT_VADDR-RAM_X_BEGIN_ADDR)/2]=0x20000*4+dst_width*dst_height*5/4;
        RAM_X[(YUV_ROTATE_X_Srcwidth-RAM_X_BEGIN_ADDR)/2]=(INT32)(width|height<<16);

        RAM_X[(YUV_ROTATE_X_Direction-RAM_X_BEGIN_ADDR)/2]=(INT32)((direction & 0xffff)|stride<<16);

        if(framecount<endframe)
        {
            VoC_set_event(CFG_WAKEUP_SOF0);
        }

    }
    else if (Multimedia_Mode==RGB565_ROTATE)
    {
        if(framecount)
        {
            desty=(char*)&RAM_EXTERN[0x20000];
            fwrite(desty,1,dst_width*dst_height*2,outfile);
            fflush(outfile);
        }

        endframe=200;

        fread(&RAM_EXTERN[0x8000],2,stride*height,infile); //input buf
        memset(&RAM_EXTERN[0x20000],0,dst_width*dst_height*2); //output buf

        RAM_X[(RGB_ROTATE_X_INPUT_ADDR-RAM_X_BEGIN_ADDR)/2]=0x8000*4;
        RAM_X[(RGB_ROTATE_X_OUTPUT_ADDR-RAM_X_BEGIN_ADDR)/2]= 0x20000*4;
        RAM_X[(RGB_ROTATE_X_srcwidth-RAM_X_BEGIN_ADDR)/2]=(INT32)(width|height<<16);

        RAM_X[(RGB_ROTATE_X_direction-RAM_X_BEGIN_ADDR)/2]=(INT32)((direction & 0xffff)|stride<<16);


        if(framecount<endframe)
        {
            VoC_set_event(CFG_WAKEUP_SOF0);
        }


    }
    else
    {
        ;
    }

#endif

    if (framecount)
    {
        VoC_counter_stats_minmax(framecount);
    }

    framecount++;

    return;
}

int main(int argc, char **argv)
{
    int i;
    for (i=0; i<RAM_X_SIZE/2; i++)
    {
        RAM_X[i]=0x55555555;//0xfffffffe;//0x55555555;//0xaaaaaaaa;
    }

    for (i=0; i<RAM_Y_SIZE/2; i++)
    {
        RAM_Y[i]=0x55555555;//0xfffffffe;//0x55555555;//0xbbbbbbbb;
    }
    for (i=0; i<RAM_E_SIZE/2; i++)
    {
        RAM_EXTERN[i]=0xcccccccc;
    }

#ifndef ROTATE_TEST

    outfileName = argv[argc-1];

    /* open output file */
    outfile = fopen(outfileName, "wb");
    if (!outfile)
    {
        printf(" *** Error opening output file %s ***\n", outfileName);
        return -1;
    }

    /* decode one audiojpeg_enc frame */
    Multimedia_Mode=MJPEG_ENCODE;//AMR_ENCODE MP3_ENCODE  MJPEG_ENCODE
#else
    infile=fopen("352x288.yuv422","rb");
    //infile=fopen("352x288.rgb565","rb");
    if(NULL==infile)
    {
        printf("open infile error!\n");
        exit(-1);
    }

    outfile=fopen("test.yuv420","wb");
    //outfile=fopen("test.rgb565","wb");
    if(outfile==NULL)
    {
        printf("open outfile error!\n");
        exit(-1);
    }

    width=352;
    height=288;

    direction=-1;

    dst_height=width;
    dst_width= height;

    mb_width=width>>3;
    mb_height=height>>3;

    stride=352;

    Multimedia_Mode=YUV_ROTATE;
    //Multimedia_Mode=RGB565_ROTATE;

#endif


    /* init after decoding first frame */
    intsLeft = 0;
    eofReached = 0;


    vpp_audiojpeg_encDecOpen(Multimedia_Mode);


    vpp_audiojpeg_encDecClose();

#ifdef ROTATE_TEST
    fclose(infile);
#endif

    fclose(outfile);
    VoC_counter_print(framecount);
    printf("\n");

    return 0;
}


