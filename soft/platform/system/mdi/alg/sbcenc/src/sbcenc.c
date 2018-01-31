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


#ifdef _VC_DEBUG_
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <io.h>
#include <process.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <sys/stat.h>

#include "sbc.h"


#if __BYTE_ORDER == __LITTLE_ENDIAN
#define COMPOSE_ID(a,b,c,d) ((a) | ((b)<<8) | ((c)<<16) | ((d)<<24))
//#define LE_SHORT(v)       (v)
//#define LE_INT(v)     (v)
//#define BE_SHORT(v)       bswap_16(v)
//#define BE_INT(v)     bswap_32(v)
#elif __BYTE_ORDER == __BIG_ENDIAN
#define COMPOSE_ID(a,b,c,d) ((d) | ((c)<<8) | ((b)<<16) | ((a)<<24))
//#define LE_SHORT(v)       bswap_16(v)
//#define LE_INT(v)     bswap_32(v)
//#define BE_SHORT(v)       (v)
//#define BE_INT(v)     (v)
#else
#error "Wrong endian"
#endif

#define AU_MAGIC        COMPOSE_ID('.','s','n','d')

#define AU_FMT_ULAW     1
#define AU_FMT_LIN8     2
#define AU_FMT_LIN16        3

FILE *enc_out;

struct au_header
{
    uint32_t magic;     /* '.snd' */
    uint32_t hdr_size;  /* size of header (min 24) */
    uint32_t data_size; /* size of data */
    uint32_t encoding;  /* see to AU_FMT_XXXX */
    uint32_t sample_rate;   /* sample rate */
    uint32_t channels;  /* number of channels (voices) */
};

static int __read(int fd, char *buf, int count)
{
    int len, pos = 0;

    while (count > 0)
    {
        len = read(fd, buf + pos, count);
        if (len <= 0)
            return len;

        count -= len;
        pos   += len;
    }

    return pos;
}

static size_t __write(int fd, short *buf, size_t count)
{
    size_t len, pos = 0;

    while (count > 0)
    {
        len = write(fd, buf + pos, count);
        if (len <= 0)
            return len;

        count -= len;
        pos   += len;
    }

    return pos;
}


void  decode(char *infilename, char* outfilename)
{
    sbc_t sbc;
    unsigned char  input[2048],output[2048];
    int output_len,consume_len,left_len,tmp_len;
    FILE*  fp=fopen(infilename,"rb");
    FILE*  fpout=fopen(outfilename,"wb");
    static  int frame=0;

    //init
    sbc_init(&sbc,0);
    fread(input,1,2048,fp);
    left_len = 2048;
    consume_len = 0;

    do
    {
        tmp_len = sbc_decode(&sbc, input+consume_len, left_len, output, 2048, &output_len);
        if (tmp_len==-1)
        {

            memcpy(input,input+consume_len,left_len);
            tmp_len=fread(input+left_len,1,consume_len,fp);
            if (tmp_len<=0)
            {
                break;
            }
            consume_len = 0;
            left_len = tmp_len - consume_len;
        }
        else
        {
            consume_len+=tmp_len;
            left_len = 2048 - consume_len;
            fwrite(output,1,output_len,fpout);

            frame++;
            printf("decode %d frame\n ",frame);
        }




    }
    while(1);

    printf("decode %d frames totally",frame);
    fclose(fp);
    fclose(fpout);
    sbc_finish(&sbc);

}


static void encode(char *filename, int subbands, int joint)
{
    unsigned char  input[2048], output[2048];
    sbc_t sbc;
    int fd, len, encoded,j;
    int ch, sb, blk, input_len;
    FILE *fp;
    static int frame=0;
    int framecount = 0;
    if((pcm_file=fopen("pcm_orig.dat","wb"))==NULL)
    {
        printf("cannot open pcm_file");
        exit(1);
    }

    if((enc_sbc_file=fopen("enc_sbc.dat","wb"))==NULL)
    {
        printf("cannot open enc_sbc_file");
        exit(1);
    }

    if((sb_sample_f_file=fopen("sb_sample_f.dat","wb"))==NULL)
    {
        printf("cannot open sb_sample_f_file");
        exit(1);
    }

    if((scale_factor_file=fopen("scale_factor.dat","wb"))==NULL)
    {
        printf("cannot open scale_factor_file");
        exit(1);
    }

    if((bits_file=fopen("bits.dat","wb"))==NULL)
    {
        printf("cannot open bits_file");
        exit(1);
    }

    if (strcmp(filename, "-"))
    {
        fp= fopen(filename, "rb");
        if(!fp)
        {
            fprintf(stderr, "Can't open file %s: %s\n", filename, strerror(errno));
            return;
        }
    }
    else
        fd = fileno(stdin);

    sbc_init(&sbc, 0L);

    sbc.endian = SBC_LE;

    ch = sbc.mode == SBC_MODE_MONO ? 1 : 2;
    sb = sbc.subbands ? 8 : 4;
    blk = 4 + (sbc.blocks * 4);
    input_len = ch*sb*blk*2;

    while((len = fread(input, 1, input_len, fp))==input_len)//for(k=0;k<53;k++)
    {

        frame++;
        if (len == 0)
            break;

        if (len < 0)
        {
            perror("Can't read audio data");
            break;
        }

        len = sbc_encode(&sbc, input, input_len, output, sizeof(output), &encoded);

        len = encoded;
        fwrite(output,1,len,enc_out);
        fflush(enc_out);
        framecount++;
        if (framecount == 72)
            printf("framecount:%d",framecount);


        for (j = 0; j < len; j++)
        {
            fprintf(enc_sbc_file, "%x\n",output[j]); //only for test
        }

        if (len == 0)
            break;

        if (len < 0 || len != encoded)
        {
            perror("Can't write SBC output");
            break;
        }
    }

    printf("total frame=%d",frame);

    fclose(scale_factor_file);
    fclose(bits_file);

    fclose(fp);
    sbc_finish(&sbc);
}


static void usage(void)
{
    printf("SBC encoder utility version \n");//%s\n", VERSION);
    printf("Copyright (c) 2004-2008  Marcel Holtmann\n\n");

    printf("Usage:\n"
           "\tsbcenc [options] file(s)\n"
           "\n");

    printf("Options:\n"
           "\t-h, --help           Display help\n"
           "\t-v, --verbose        Verbose mode\n"
           "\t-s, --subbands       Number of subbands to use (4 or 8)\n"
           "\t-j, --joint          Joint stereo\n"
           "\n");
}
int main(int argc, char *argv[])
{

    int opt=0, verbose = 0, subbands = 8, joint = 0;


    if (argc < 1)
    {
        usage();
        exit(1);
    }


    if (atoi(argv[3])==ENCODE_SBC)
    {
        //encode   pcm_raw.raw   pcm_lr.sbc  0
        if((enc_out=fopen(argv[2],"wb"))==NULL)
        {
            printf("cannot open outfile");
            exit(1);
        }

        encode(argv[1], subbands, joint);
        fclose(enc_out);
    }
    else
    {
        //decode     pcm_lr.sbc pcm.raw  1
        decode(argv[1],argv[2]);
    }


    return 0;
}
#endif