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


#ifndef SHEEN_VC_DEBUG
#include "global.h"
#include "mcip_debug.h"
#endif

#ifdef MP4_3GP_SUPPORT

#include "isomedia_dev.h"
#include "list.h"

extern GF_BitStream *bs_glob;//add shenh
extern uint8 *pBufGlob;//temp buf. 512bytes .add shenh

GF_Err gf_isom_parse_movie_boxes(GF_ISOFile *mov)
{
    GF_Box *a;
    u64 totSize;
    GF_Err e = GF_OK;
    totSize = 0;

    /*while we have some data, parse our boxes*/
    while (gf_bs_available(mov->movieFileMap->bs) > 8)
    {

        e = gf_isom_parse_box(&a, mov->movieFileMap->bs);
        if(e==GF_INVALID_BOX)//sheen
        {
            totSize += (uint32)a;//recode the size.sheen
            continue;//skip this box
        }
        if (e >= 0)
        {
            e = GF_OK;
        }
        else
        {
            if(mov->moov && mov->mdat)
                break;
            else
                return e;
        }

        switch (a->type)
        {
            /*MOOV box*/
            case GF_ISOM_BOX_TYPE_MOOV:
                if (mov->moov) return GF_ISOM_INVALID_FILE;
                mov->moov = (GF_MovieBox *)a;
                /*set our pointer to the movie*/
                mov->moov->mov = mov;
                e = gf_list_add2(mov->TopBoxes, a);//change shenh
                if (e) return e;
                totSize += a->size;
                break;

            /*META box*/
//      case GF_ISOM_BOX_TYPE_META:
//          if (mov->meta) return GF_ISOM_INVALID_FILE;
//          mov->meta = (GF_MetaBox *)a;
//          e = gf_list_add(mov->TopBoxes, a);
//          if (e) return e;
//          totSize += a->size;
//          break;

            /*we only keep the MDAT in READ for dump purposes*/
            case GF_ISOM_BOX_TYPE_MDAT:
                totSize += a->size;
                mov->mdat = (GF_MediaDataBox *) a;;
                e = gf_list_add2(mov->TopBoxes, mov->mdat);//change shenh
                if (e) return e;
                break;
            case GF_ISOM_BOX_TYPE_FTYP:
                /*ONE AND ONLY ONE FTYP*/
                if (mov->brand)
                {
                    return GF_ISOM_INVALID_FILE;
                }
                mov->brand = (GF_FileTypeBox *)a;
                totSize += a->size;
                e = gf_list_add2(mov->TopBoxes, a);//change shenh
                break;

            case GF_ISOM_BOX_TYPE_PDIN:
                /*ONE AND ONLY ONE PDIN*/
                if (mov->pdin)
                {
                    return GF_ISOM_INVALID_FILE;
                }
                mov->pdin = (GF_ProgressiveDownloadBox *) a;
                totSize += a->size;
                e = gf_list_add2(mov->TopBoxes, a);//change shenh
                break;
            default:
                totSize += a->size;
                e = gf_list_add2(mov->TopBoxes, a);//change shenh
                break;
        }
        if(a && a->type==GF_ISOM_BOX_TYPE_MDAT && mov->movieFileMap->bs->mp4_fs_mode==2)
        {
            break;
        }
    }
    /*we need at least moov or meta*/
    if (!mov->moov ) return GF_ISOM_INVALID_FILE;
    /*we MUST have movie header*/
    if (mov->moov && !mov->moov->mvhd) return GF_ISOM_INVALID_FILE;
    return GF_OK;
}

GF_ISOFile *gf_isom_new_movie()
{
    //diag_printf("mmc_MemMalloc .... gf_isom_new_movie \n");
    GF_ISOFile *mov = (GF_ISOFile*)mmc_MemMalloc(sizeof(GF_ISOFile));
    if (mov == NULL)
    {
        return NULL;
    }
    memset(mov, 0, sizeof(GF_ISOFile));

    /*init the boxes*/
    mov->TopBoxes = gf_list_new();
    if (!mov->TopBoxes)
    {
        //free(mov);change shenh
        return NULL;
    }

    //add shenh
    mov->TopBoxes->slots= (void **)mmc_MemMalloc(MAX_TOPBOX_SLOT_NUM*sizeof(void*));
    if (!mov->TopBoxes->slots)
    {
        //free(mov);change shenh
        return NULL;
    }
    memset(mov->TopBoxes->slots, 0, MAX_TOPBOX_SLOT_NUM*sizeof(void*));
    //

    return mov;
}

int gf_isom_open_file(HANDLE file_handle, uint8 file_mode, uint32 data_Len, GF_ISOFile **movie)
{
    GF_Err e;
    GF_ISOFile *mov = gf_isom_new_movie();
    if(!mov)return -1;
    bs_glob=NULL;
    bs_glob=(GF_BitStream*)mmc_MemMalloc(sizeof(GF_BitStream));
    if(!bs_glob)return -1;
    memset(bs_glob, 0, sizeof(GF_BitStream));
    pBufGlob=(unsigned char*)mmc_MemMalloc(BUFGLOB_SIZE);
    if(!pBufGlob)return -1;
    memset(pBufGlob, 0, BUFGLOB_SIZE);

    *movie=mov;

    e = gf_isom_datamap_new( file_handle, file_mode, data_Len, &mov->movieFileMap);
    if (e)
    {
        return -1;
    }
    //mov->fileName = strdup(fileName);  change shenh
    /*
    mov->fileName=mmc_MemMalloc(strlen(fileName));
    if(!mov->fileName)return -1;
    memset(mov->fileName, 0, strlen(fileName));
    strcpy(mov->fileName, fileName);
    */

    e=gf_isom_parse_movie_boxes(mov);

    if(e==GF_ISOM_INCOMPLETE_FILE)
    {
        diag_printf("[VID] WARNING gf_isom_open_file incomplete file!\n");
        return 0;
    }
    else if (e)
    {
        return -1;
    }

    return  0;
}

int gf_isom_close_file(GF_ISOFile * movie)//add shenh
{
    GF_FileDataMap *tmp;
    if(!movie)
    {
        diag_printf("gf_isom_close_file movie NULL\n");
        return -1;
    }
    if(!movie->movieFileMap)
    {
        diag_printf("gf_isom_close_file movieFileMap NULL\n");
        return -1;
    }

    tmp = (GF_FileDataMap *)movie->movieFileMap;

    /*
    tmp->stream==0
    if(tmp->stream)
    {
        diag_printf("gf_isom_close_file stream NULL\n");
        return -1;
    }*/

    //diag_printf("**********CLOSE FILE IN VIDEO********tmp->stream:%d",tmp->stream);
//  FS_Close((INT32)tmp->stream);

    return  0;
}

GF_TrackBox *gf_isom_get_track(GF_MovieBox *moov, uint32 trackNumber)
{
    GF_TrackBox *trak;
    if (!moov || !trackNumber || (trackNumber > gf_list_count(moov->trackList))) return NULL;
    trak = (GF_TrackBox*)gf_list_get(moov->trackList, trackNumber - 1);
    return trak;

}

GF_TrackBox *gf_isom_get_track_from_file(GF_ISOFile *movie, uint32 trackNumber)
{
    GF_TrackBox *trak;
    if (!movie) return NULL;
    trak = gf_isom_get_track(movie->moov, trackNumber);
    if (!trak) movie->LastError = GF_BAD_PARAM;
    return trak;
}

#endif

#ifdef SHEEN_VC_DEBUG
//#if 0

void *mmc_MemMalloc(int size)
{
    static i=0,tot=0;
    printf("i=%d size=%d tot=%d \n",i ,size, tot);
    i++;
    tot+=size;
    return malloc(size);
}

void main()
{
    int res;
    int i;
    FILE *fileIn, *fileOut;
    GF_ISOFile *file;
    GF_AVCConfig *avc;
    GF_DecoderConfig *temp_dcfg;
    GF_TrackBox *v_trak;
    GF_TrackBox *a_trak;
    int readLen;
    char *buf;
    unsigned char*pBuf;
    int bufPos=0;
    unsigned int chunkPos;
    unsigned int chunksize;
    unsigned int a_sample_rate, a_channels;
    uint32  dts, dts_sampCount,dts_sampDelta;

    int *p;
    int gotbytes, gotcount;
    unsigned int is_aac;
    unsigned int aac_type;
    GF_M4ADecSpecInfo a_cfg;
    unsigned char aacHead[7];
    int trknumV=2;
    int trknumA=1;
    int file_mode=0;
    unsigned int m_type;
    unsigned int timestamp;
    unsigned int syncword=0xAAAAAAAA;

#define OUT_V
//#define IS_MPEG4
#define IS_H264
//#define OUT_A
//#define IS_AAC

    fileIn=fopen("F:\\测试文件\\264\\少女时代2_h264.mp4","rb");
    //fileIn=fopen("H:\\yea.mp4","rb");
    fileOut=fopen("F:\\测试文件\\264\\少女时代2.264","wb");
    buf=malloc(100*1024);

    if(file_mode==2)
    {
        res=http_fs_parsehead(fileIn);
        printf("head size %d\n",res-1024);
        http_fs_init(fileIn);
    }

    res=gf_isom_open_file(fileIn,file_mode,0, &file );

    m_type = gf_isom_get_media_type(file, 1);
    if(m_type==GF_ISOM_MEDIA_VISUAL)
    {
        trknumA=2;
        trknumV=1;
    }

    a_sample_rate=gf_isom_get_sample_samplerate(file, trknumA);
    a_channels=gf_isom_get_sample_channel_count(file, trknumA);
    gf_isom_get_sample_height(file, trknumV);
    gf_isom_get_sample_width(file, trknumV);
    gf_isom_get_sample_FPS(file, trknumV);
    gf_isom_get_sample_count(file,trknumV);



#ifdef IS_H264//h.264
    avc=gf_isom_get_avc_config(file, trknumV);

    p=(int*)buf;
    p[0]=(int)0x01000000;//0x00000001
    bufPos=4;
    memcpy(buf+bufPos, avc->sps[0].data, avc->sps[0].dataLength);
    bufPos+=avc->sps[0].dataLength;

    *(buf+bufPos)=0;
    *(buf+bufPos+1)=0;
    *(buf+bufPos+2)=0;
    *(buf+bufPos+3)=1;
    bufPos+=4;
    memcpy(buf+bufPos, avc->pps[0].data, avc->pps[0].dataLength);
    bufPos+=avc->pps[0].dataLength;
#endif

#ifdef IS_MPEG4//mpeg4
    temp_dcfg = gf_isom_get_decoder_config(file, trknumV, 1);
    memcpy(buf, temp_dcfg->decoderSpecificInfo->data, temp_dcfg->decoderSpecificInfo->dataLength);
    bufPos+= temp_dcfg->decoderSpecificInfo->dataLength;

#endif

    v_trak=gf_isom_get_track_from_file(file,trknumV);

    if(stbl_GetSampleDTScount(v_trak->Media->information->sampleTable->TimeToSample)>1)
    {
        //dts
        int timescale=v_trak->Media->mediaHeader->timeScale;
    }

    stbl_GetSampleDTS(v_trak->Media->information->sampleTable->TimeToSample, 5, &dts, &dts_sampCount, &dts_sampDelta);


    v_trak->Media->information->lastSampleNumber=0;
    v_trak->Media->information->lastFirstSample=0;
    v_trak->Media->information->lastOffset=0;
    if(!v_trak->Media->information->dataHandler)
        v_trak->Media->information->dataHandler = v_trak->Media->mediaTrack->moov->mov->movieFileMap;
#ifdef OUT_A//audio
    a_trak=gf_isom_get_track_from_file(file,trknumA);

    if(stbl_GetSampleDTScount(a_trak->Media->information->sampleTable->TimeToSample)>1)
    {
        //dts
        int timescale=a_trak->Media->mediaHeader->timeScale;
    }
    stbl_GetSampleDTS(a_trak->Media->information->sampleTable->TimeToSample, 5, &dts, &dts_sampCount, &dts_sampDelta);

    if(!a_trak->Media->information->dataHandler)
        a_trak->Media->information->dataHandler = a_trak->Media->mediaTrack->moov->mov->movieFileMap;

    {
        UINT32 totalDuration1, totalDuration2, totalDuration3;
        UINT32 timeScale1, timeScale2, timeScale3;
        //mvhd
        totalDuration1= file->moov->mvhd->duration;
        timeScale1= file->moov->mvhd->timeScale;
        //mdhd video
        totalDuration2= v_trak->Media->mediaHeader->duration;
        timeScale2 =v_trak->Media->mediaHeader->timeScale;
        //mdhd audio
        totalDuration3= a_trak->Media->mediaHeader->duration;
        timeScale3 =a_trak->Media->mediaHeader->timeScale;
    }

#ifdef IS_AAC//aac
    temp_dcfg = gf_isom_get_decoder_config(file, trknumA,1);

    switch (temp_dcfg->objectTypeIndication)
    {
        //Extracting MPEG-2 AAC".aac"
        case 0x66:
        case 0x67:
        case 0x68:
            is_aac = 0;
            aac_type = temp_dcfg->objectTypeIndication - 0x66;
            gf_m4a_get_config(temp_dcfg->decoderSpecificInfo->data, temp_dcfg->decoderSpecificInfo->dataLength, &a_cfg);
            //diag_printf("[VID] MPEG2_AAC");
            break;
        //Extracting MPEG-4 AAC ".aac"
        case 0x40:
            is_aac = 1;
            gf_m4a_get_config(temp_dcfg->decoderSpecificInfo->data, temp_dcfg->decoderSpecificInfo->dataLength, &a_cfg);
            aac_type = a_cfg.base_object_type - 1;
            //diag_printf("[VID] MPEG4_AAC");
            break;
        //Extracting MPEG-1/2 Audio (MP3)".mp3"
        case 0x69:
        case 0x6B:
            break;
        default:
            //diag_printf("[VID] unsupport temp_dcfg->objectTypeIndication=%x \n",temp_dcfg->objectTypeIndication);
            //return ISOM_ERR_UNSUPPORT;
            break;
    }

    if (temp_dcfg->streamType==GF_STREAM_AUDIO)
    {
        GF_BitToInt bs;
        memset(aacHead, 0, 7);
        bs.stream=aacHead;
        bs.current=0;
        bs.nbBits=0;

        gf_bs_bit_to_int(&bs, 0xFFF, 12);/*sync*/
        gf_bs_bit_to_int(&bs, (is_aac==0) ? 1 : 0, 1);/*mpeg2 aac*/
        gf_bs_bit_to_int(&bs, 0, 2); /*layer*/
        gf_bs_bit_to_int(&bs, 1, 1); /* protection_absent*/
        gf_bs_bit_to_int(&bs, aac_type, 2);
        gf_bs_bit_to_int(&bs, a_cfg.base_sr_index, 4);
        gf_bs_bit_to_int(&bs, 0, 1);
        gf_bs_bit_to_int(&bs, a_cfg.nb_chan, 3);
        gf_bs_bit_to_int(&bs, 0, 4);
        //gf_bs_bit_to_int(&bs, AAC_HEAD_SIZE+dataLength, 13);
        gf_bs_bit_to_int(&bs, 0, 13);//13bit will be rewrite when every sample read
        gf_bs_bit_to_int(&bs, 0x7FF, 11);
        gf_bs_bit_to_int(&bs, 0, 2);
    }
#endif

#endif

    i=1;
    //i=gf_isom_get_keySampleNumber(v_trak,50);
    timestamp=0;
    while(1)
    {

#ifdef OUT_V// video
        ////get one frame
        stbl_GetSampleSize(v_trak->Media->information , i, &readLen);

        res=gf_isom_get_multSample(v_trak->Media->information,
                                   i,
                                   buf+bufPos,
                                   readLen,    &gotbytes, &gotcount);
        if(res!=0 || gotbytes<=0)
            break;
        //if(i<1805)
        //  {i++;continue;}
#endif

#ifdef OUT_A//audio

        res=stbl_GetSampleSize(a_trak->Media->information , i, &readLen);
        if(res)break;

#ifdef IS_AAC//aac
        aacHead[3]&=0xfc;
        aacHead[4]=0;
        aacHead[5]&=0x1f;
        aacHead[5]|=(((readLen+7)&0x07)<<5);
        aacHead[4]=(((readLen+7)&0x07f8)>>3);
        aacHead[3]|=(((readLen+7)&0x1800)>>11);
        memcpy((buf+bufPos), aacHead, 7);
        bufPos+=7;
#endif

        res=gf_isom_get_multSample(a_trak->Media->information,
                                   i,
                                   buf+bufPos,
                                   readLen,    &gotbytes, &gotcount);
#endif

#ifdef IS_H264//h.264
        //replace frame size with start code.
        chunkPos=0;
        do
        {
            pBuf=(buf+bufPos+chunkPos);
            chunksize=0;

            chunksize|=(*pBuf)<<24;
            chunksize|=(*(pBuf+1))<<16;
            chunksize|=(*(pBuf+2))<<8;
            chunksize|=*(pBuf+3);

            *(pBuf)=0;
            *(pBuf+1)=0;
            *(pBuf+2)=0;
            *(pBuf+3)=1;

            chunkPos+=chunksize+4;
        }
        while (chunkPos<gotbytes-4);
#endif

        bufPos+=gotbytes;
        /////////////get frame end
#if 0//time,size,header.
        fwrite(&syncword,1,4,fileOut);
        fwrite(&timestamp,1,4,fileOut);
        fwrite(&gotbytes,1,4,fileOut);
        timestamp+=450;//15fps,90k baseclock
#endif
        fwrite(buf,1,bufPos,fileOut);
        //fwrite(buf,1,32,fileOut);
        printf("frame %d %d\n",i,readLen);
        bufPos=0;
        i++;
        //if(i>100)break;
    }
    gf_isom_close_file(file);
    fclose(fileIn);
    fclose(fileOut);
}
#endif

