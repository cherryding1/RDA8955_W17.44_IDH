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



#include "mmf_jpegamrring_avi_enc.h"
#include "mmc_videorecord.h"
#include "string.h"
#include "mcip_debug.h"

int32 favi;
DWORD frames;
//int amr_block;//only for some fps video 5 10 ..
int video_i/*,audio_i*/;
int bitrate_tmp,fps_tmp,width_tmp,height_tmp;
uint16 Audio_SampleRate;    //48k,44.1k,32k,22.05k,24k,16k,12k,11.025k,8k
uint8  Audio_Bits;      //16bit or 8bit
uint8  Audio_Channel;
long jpg_sz_64;
long aud_sz_64;
JpegAmr_Avi_Init Avi_Init;
uint32 RemainEndBytes;//idx1+size

AVI_video_index *avi_viidx;     //bao debug: design the index table according 500 frame
//AVI_audio_index *avi_auidx;     //bao debug: design the index table according 500 frame
unsigned int idx_video_offset;
unsigned int idx_audio_offset;
//unsigned int idx_audio_size;

typedef long off64_t;


////for test////////////////////
#ifdef AVI_TEST
/*
uint8 jpegbuf[]={
    #include "11.tab"
    };
uint8 amrbuf[]={
    #include "shenhua.tab"
    };
*/
uint8 video_b[10480];
uint8 video_b2[10480];
uint8 audio_b[326];
uint8 avi_out[10480];
uint8 avi_head[AVI_HEAD_SIZE];
uint8 avi_end[86];
int8 fn[255];
#endif
//////////////////////////////





/******
copy data memery out
******/
void avi_memcpy(uint32 *pOutBuf, uint8 *pInBuf,uint32 size)
{
    //printf("shenh memcpy oubuf adrr=%x ,inbuf adrr=%x ,size=%d \n",*pOutBuf,pInBuf,size);
    memcpy((void*)(*pOutBuf), (const void*)pInBuf, (int)size);
    (*pOutBuf)+=size;
}

/*
spc: printing 4 byte word in little-endian fmt
*/
void print_quartet(uint8* pBuf, unsigned int i)
{
    /*
        int8 temp[4];
        temp[0]=i % 0x100;
        i /= 0x100;
        temp[1]=i % 0x100;
        i /= 0x100;
        temp[2]=i % 0x100;
        i /= 0x100;
        temp[3]=i % 0x100;
    //  FS_Write(favi,temp,4);
        avi_memcpy((uint32 *)pBuf, temp, 4);
    */
    int8* p_temp=(uint8*)*((uint32 *)pBuf);
    p_temp[0]=(uint8)(i & 0xff);

    p_temp[1]=(uint8)((i >>8)& 0xff);

    p_temp[2]=(uint8)((i >>16)& 0xff);

    p_temp[3]=(uint8)((i >>24)& 0xff);

    (*((uint32 *)pBuf))+=4;

}

/**********************
step 1
 init avi parameters
 input: JpegAmr_Avi_Init
 return: FUNC_OK
 shenh
2007.9.19
***********************/
int16 MMF_Avi_Enc_Init(JpegAmr_Avi_Init* pAvi_Init)
{
    bitrate_tmp = pAvi_Init->Amr_BitRate;
    fps_tmp =  pAvi_Init->Video_fps;
    width_tmp =  pAvi_Init->Image_Width;
    height_tmp =  pAvi_Init->Image_Height;
    Audio_Bits=pAvi_Init->Audio_Bits;
    Audio_SampleRate=pAvi_Init->Audio_SampleRate;
    Audio_Channel=pAvi_Init->Audio_Channel;

    video_i =0;
    //audio_i = 0;
    idx_video_offset=0;
    idx_audio_offset=0;
    //idx_audio_size=0;

    frames = 0;
    RemainEndBytes=8;

    //amr_block = bitrate_tmp / fps_tmp / 8;
    //amr_block = 32*VIDREC_AMR_FRAME_NUM_IN_A_PACK;              // bao debug: bytes per audio frame

    jpg_sz_64 = 0;
    aud_sz_64=0;

    diag_printf("shenh avi init over\n");
    return FUNC_OK;
}

/**********************
step 2
 put jpeg and amr into avi
 input: JpegAmr_Avi_Enc
 return: AviFrame_size
 shenh
2007.9.19
***********************/
int32 MMF_Avi_Enc_Frame(JpegAmr_Avi_Enc* pAvi_Enc)
{
    uint8 *pOutBuf;
    uint32 AviFrame_size;
    int32 remnant;


    /*************jpeg***********************/
    //video data
    pOutBuf=pAvi_Enc->Avi_outBuf;
    avi_memcpy((uint32 *)((uint8 *)&pOutBuf), "00db", 4);
    remnant = ((pAvi_Enc->Jpeg_Frame_Size+ 3)&0xfffffffc) - pAvi_Enc->Jpeg_Frame_Size;
    print_quartet((uint8 *)&pOutBuf, pAvi_Enc->Jpeg_Frame_Size + remnant);

    avi_viidx[video_i].video_size = pAvi_Enc->Jpeg_Frame_Size + remnant;
    jpg_sz_64 += (pAvi_Enc->Jpeg_Frame_Size + remnant);
    if(video_i == 0)
    {
        //avi_viidx[video_i].video_offset = 4;
        idx_video_offset=4;
    }
    else
    {
        //avi_viidx[video_i].video_offset = avi_auidx[audio_i-1].audio_offset + avi_auidx[audio_i-1].audio_size + 8;
        //idx_video_offset= idx_audio_offset+ idx_audio_size+ 8;
        idx_video_offset= idx_audio_offset+ avi_viidx[video_i-1].audio_size+ 8;
    }

    avi_memcpy( (uint32 *)((uint8 *)&pOutBuf), pAvi_Enc->Jpeg_inBuf, pAvi_Enc->Jpeg_Frame_Size);
    //printf("shenh jpg size res=%d\n",pAvi_Enc->Jpeg_Frame_Size);
    avi_memcpy((uint32 *)((uint8 *)&pOutBuf), pAvi_Enc->Jpeg_inBuf, (uint32)remnant);
    //printf("shenh jpg size2 res=%d\n",remnant);

    //video_i ++;
    //frames ++;

    /**************amr**************************/
    avi_memcpy((uint32 *)((uint8 *)&pOutBuf), "01wb", 4);
    print_quartet((uint8 *)&pOutBuf,pAvi_Enc->Amr_Data_Size);
    avi_memcpy((uint32 *)((uint8 *)&pOutBuf), pAvi_Enc->Amr_inBuf, pAvi_Enc->Amr_Data_Size);
    //printf("shenh aud size res=%d\n",pAvi_Enc->Amr_Data_Size);

    avi_viidx[video_i].audio_size = pAvi_Enc->Amr_Data_Size;
    aud_sz_64+=pAvi_Enc->Amr_Data_Size;
    //avi_auidx[audio_i].audio_offset = avi_viidx[video_i-1].video_offset + avi_viidx[video_i-1].video_size + 8;
    //idx_audio_size = pAvi_Enc->Amr_Data_Size;
    idx_audio_offset = idx_video_offset + avi_viidx[video_i].video_size + 8;
    //audio_i ++;

    video_i ++;
    frames ++;

    RemainEndBytes+=16*2;
    AviFrame_size=pOutBuf-pAvi_Enc->Avi_outBuf;

    //printf("shenh avi enc over\n");
    return AviFrame_size;
}

/**********************
step 4
 at last put head data into avi
 input: pAvi_Head_Buf
 return: FUNC_OK
 shenh
2007.9.19
***********************/
int16 MMF_Avi_Enc_Head(uint8* pAvi_Head_Buf)
{
    //  int i;
    //  long movi_sz;
    off64_t riff_sz_64;
    DWORD per_usec = 1;
    //const off64_t MAX_RIFF_SZ = 0x7fffffff;
    //DWORD riff_sz;
    uint8 *pOutBuf=pAvi_Head_Buf;

    struct AVI_list_hdrl hdrl =
    {
        /* header */
        {
            {'L', 'I', 'S', 'T'},
            LILEND4(sizeof(struct AVI_list_hdrl) - 8),
            {'h', 'd', 'r', 'l'}
        },

        /* chunk avih */
        {'a', 'v', 'i', 'h'},
        LILEND4(sizeof(struct AVI_avih)),
        {
            LILEND4(per_usec),

            //LILEND4(1000000 * (jpg_sz_64/frames) / per_usec),
            LILEND4(40*1024),   //bao debug: max bytes per second

            LILEND4(0),
            LILEND4(AVIF_ISINTERLEAVED|AVIF_HASINDEX),
            LILEND4(frames),
            LILEND4(0),
            LILEND4(2),
            LILEND4(0),
            LILEND4(width_tmp),
            LILEND4(height_tmp),
            {*(DWORD *)"cool", *(DWORD*)"sand", LILEND4(0), LILEND4(0)}
        },
        /* list strl */
        {
            {
                {'L', 'I', 'S', 'T'},
                LILEND4(sizeof(struct AVI_list_strl_video) - 8),
                {'s', 't', 'r', 'l'}
            },

            /* chunk strh */
            {'s', 't', 'r', 'h'},
            LILEND4(sizeof(struct AVI_strh)),
            {
                {'v', 'i', 'd', 's'},
                {'M', 'J', 'P', 'G'},
                LILEND4(1),
                LILEND4(0),
                LILEND4(0),
                LILEND4(per_usec),
                LILEND4(1000000),
                LILEND4(0),
                LILEND4(frames),
                LILEND4(0),
                LILEND4(0xffffffff),
                LILEND4(0)
            },

            /* chunk strf */
            {'s', 't', 'r', 'f'},
            LILEND4(sizeof(struct AVI_strf_video)),
            {
                LILEND4(sizeof(struct AVI_strf_video)),
                LILEND4(width_tmp),
                LILEND4(height_tmp),
                LILEND4(1 + 24*256*256),
                {'M', 'J', 'P', 'G'},
                LILEND4(width_tmp * height_tmp * 3),
                LILEND4(0),
                LILEND4(0),
                LILEND4(0),
                LILEND4(0)
            }
        },
        {
            {
                {'L', 'I', 'S', 'T'},
                LILEND4(sizeof(struct AVI_list_strl_audio) - 8),
                {'s', 't', 'r', 'l'}
            },

            /* chunk strh */
            {'s', 't', 'r', 'h'},
            LILEND4(sizeof(struct AVI_strh)),
            {
                {'a', 'u', 'd', 's'},

                {1,0,0,0},
                LILEND4(1),
                LILEND4(0),
                LILEND4(0),
                LILEND4(0),
                LILEND4(0),
                LILEND4(0),
                LILEND4(0),
                LILEND4(0),
                LILEND4(0xFFFFFFFF),
                LILEND4(2)
            },

            /* chunk strf */
            {'s', 't', 'r', 'f'},
            LILEND4(sizeof(struct AVI_strf_audio)),
            {
                LILEND2(0xFE),
                LILEND2(1),
                LILEND4(8000),
                LILEND4(1600),
                LILEND2(2),
                LILEND2(16)
            }
        }
    };

    /* getting image, and hence, riff sizes */
    //riff_sz_64 = sizeof(struct AVI_list_hdrl) + 4 + 4 + jpg_sz_64
    //+ 8*frames + 8 + 8 + 16*frames + amr_block*frames + 16 + 8;
    riff_sz_64 = 4 + sizeof(struct AVI_list_hdrl) + 12 + 8*frames + jpg_sz_64 + 8*frames + aud_sz_64
                 + 8 + 2*16*frames;      //bao debug: size of file, not include 'RIFF' and size

    //per_usec = 1000000/fps_tmp;
    per_usec = VIDREC_AVI_AUDIO_FRAME_INTERVAL*1000;        //bao debug: us per frame

    /* printing AVI riff hdr */
    avi_memcpy((uint32 *)((uint8 *)&pOutBuf), "RIFF", 4);
    print_quartet((uint8 *)&pOutBuf,riff_sz_64);
    avi_memcpy((uint32 *)((uint8 *)&pOutBuf), "AVI ", 4);

    /* list hdrl */
    hdrl.avih.us_per_frame = LILEND4(per_usec);

    //hdrl.avih.max_bytes_per_sec = LILEND4(1000000 * (jpg_sz_64/frames) / per_usec);
    hdrl.avih.max_bytes_per_sec = 40*1024;  //bao debug: max bytes per second

    hdrl.avih.tot_frames = LILEND4(frames);
    hdrl.avih.width = LILEND4(width_tmp);
    hdrl.avih.height = LILEND4(height_tmp);
    hdrl.strl_video.strh.scale = LILEND4(1);

    hdrl.strl_video.strh.rate = LILEND4(fps_tmp);

    hdrl.strl_video.strh.length = LILEND4(frames);
    hdrl.strl_video.strh.sample_sz = LILEND4(frames);
    hdrl.strl_video.strh.left=LILEND2(0);
    hdrl.strl_video.strh.top=LILEND2(0);
    hdrl.strl_video.strh.right=LILEND2(width_tmp);
    hdrl.strl_video.strh.bottom=LILEND2(height_tmp);
    hdrl.strl_video.strf_video.width = LILEND4(width_tmp);
    hdrl.strl_video.strf_video.height = LILEND4(height_tmp);
    hdrl.strl_video.strf_video.image_sz = LILEND4(width_tmp * height_tmp * 3);
    hdrl.strl_audio.strh.scale=LILEND4(1);

    hdrl.strl_audio.strh.rate=LILEND4(bitrate_tmp/16);    //bao debug: ?
    //hdrl.strl_audio.strh.length=LILEND4((bitrate_tmp/16)*frames/fps_tmp);
    hdrl.strl_audio.strh.length=LILEND4( 32*VIDREC_AMR_FRAME_NUM_IN_A_PACK*frames );    //bao debug: audio stream data length

    //  hdrl.strl_video.list_odml.frames = LILEND4(frames); /*  */
    avi_memcpy((uint32 *)((uint8 *) &pOutBuf), (uint8*)&hdrl, sizeof(hdrl));

    /* list movi */
    avi_memcpy((uint32 *)((uint8 *)&pOutBuf), "LIST", 4);
    print_quartet((uint8 *)&pOutBuf,jpg_sz_64 + 8*frames + 4 + aud_sz_64+  8*frames);
    avi_memcpy((uint32 *)((uint8 *)&pOutBuf), "movi", 4);

    diag_printf("shenh avi head over\n");
    return FUNC_OK;
}

/**********************
step 3
  put idx1 end data into avi
 input: JpegAmr_Avi_End
 return: Avi_End_Return
 shenh
2007.9.19
***********************/
int16 MMF_Avi_Enc_End(JpegAmr_Avi_End* pAvi_End, Avi_End_Return* pAvi_End_Re)
{
    int i=0;
    uint8 *pOutBuf;
    int frame_pos=0;
    int Out_num=0;
    //unsigned int a_size=0;

    pOutBuf = pAvi_End->pAvi_End_Buf;

    //printf("shenh in end RemainEndBytes=%d pAvi_End->Buf_size=%d  \n",RemainEndBytes,pAvi_End->Buf_size);
    if(RemainEndBytes==(8+16*frames*2))
    {
        avi_memcpy((uint32 *)((uint8 *)&pOutBuf), "idx1", 4);
        print_quartet((uint8 *)&pOutBuf,16 * frames * 2);
        if((Out_num=(pAvi_End->Buf_size-8)/32)>=frames)
        {
            Out_num=frames;
        }
        frame_pos=0;
        pAvi_End_Re->OutByte_size=Out_num*32+8;
        RemainEndBytes-=pAvi_End_Re->OutByte_size;
        //printf("shenh 1 frame_pos=%d Out_num=%d \n",frame_pos,Out_num);
    }
    else
    {
        if((Out_num=pAvi_End->Buf_size/32)>=(RemainEndBytes/32))
        {
            Out_num=RemainEndBytes/32;
        }
        frame_pos=((8+16*frames*2)-RemainEndBytes-8)/32;
        pAvi_End_Re->OutByte_size=Out_num*32;
        RemainEndBytes-=pAvi_End_Re->OutByte_size;
        //printf("shenh 2 frame_pos=%d Out_num=%d \n",frame_pos,Out_num);
    }

    i=frame_pos+Out_num;
    for (; frame_pos < i; frame_pos++)
    {
        avi_memcpy((uint32 *)((uint8 *)&pOutBuf),"00db", 4);
        print_quartet((uint8 *)&pOutBuf,16);
        //print_quartet(&pOutBuf,(unsigned int)(avi_viidx[frame_pos].video_offset));
        /*        if(frame_pos!=1)
                    a_size=idx_audio_size;
                else
                    a_size=idx_audio_size+6;
        */
        if(frame_pos>0)
        {
            //idx_video_offset= idx_audio_offset+ a_size+ 8;
            idx_video_offset= idx_audio_offset+ avi_viidx[frame_pos-1].audio_size+ 8;
        }
        else
        {
            idx_video_offset=4;
        }
        print_quartet((uint8 *)&pOutBuf, idx_video_offset);
        print_quartet((uint8 *)&pOutBuf,(unsigned int)(avi_viidx[frame_pos].video_size));

        avi_memcpy((uint32 *)((uint8 *)&pOutBuf),"01wb", 4);
        print_quartet((uint8 *)&pOutBuf,16);
        //print_quartet(&pOutBuf,(unsigned int)(avi_auidx[frame_pos].audio_offset));
        //print_quartet(&pOutBuf,(unsigned int)(avi_auidx[frame_pos].audio_size));
        idx_audio_offset = idx_video_offset + (unsigned int)avi_viidx[frame_pos].video_size + 8;
        /*
                if(frame_pos>0)
                {
                    a_size = idx_audio_size;
                }
                else
                {
                    a_size = idx_audio_size+6;
                }
        */
        print_quartet((uint8 *)&pOutBuf, idx_audio_offset);
        print_quartet((uint8 *)&pOutBuf, avi_viidx[frame_pos].audio_size);
    }
    pAvi_End_Re->RemainByte_size=RemainEndBytes;
    //printf("shenh idx data ,RemainEndBytes=%d frames=%d \n",RemainEndBytes,frames);
    return FUNC_OK;
}


///////////// test demo/////////////
#ifdef AVI_TEST
void video_record_test()
{
    int i;
    HANDLE fjpg;
    HANDLE faud;
    HANDLE favi;

    uint32 jpg_size;
    uint32 jpg2_size;
    JpegAmr_Avi_Init  test_Avi_Init;
    JpegAmr_Avi_Enc  test_avi_enc;
    JpegAmr_Avi_End  test_Avi_End;
    Avi_End_Return     test_Avi_End_Re;
    int result, res;

    uint8 *paudio_b=audio_b;
    uint8 *pvideo_b=video_b;
    uint8 *pvideo_b2=video_b2;

    /*********mount*******************/
    result = FS_Mount("","",0,0);
    if(result!=0)
    {
        printf("mount fail!\n");
        return -1;
    }
    result=FS_MountRoot("");
    if( result > 0)
    {
        OutPutFileName(result);
    }
    printf("shenh mount,result=%d \n",result);


    /************open jpeg and amr data*****************************/
    //jpeg 1
    if((fjpg = FS_Open("qzx.jpeg", FS_O_RDONLY, 0))>= 0)
    {
        printf( "shenh open jpg file OK \n");
    }
    else
        printf( "shenh open jpg file fail=%x \n",fjpg);

    jpg_size=(uint32)FS_Seek((int32)fjpg, 0,FS_SEEK_END );
    printf( "shenh  jpg size=%d \n",jpg_size);
    FS_Seek((int32)fjpg, 0, FS_SEEK_SET);

    if((result=FS_Read((int32)fjpg,pvideo_b,jpg_size))>0)
    {
        printf( "shenh read  jpg file ok=%d \n", result);
    }
    else
        printf( "shenh read  jpg file err result=%d \n", result);

    FS_Close(fjpg);
    //jpeg 2
    if((fjpg = FS_Open("qzx2.jpeg", FS_O_RDONLY, 0))>=0)
    {
        printf( "shenh open jpg2 file OK \n");
    }
    else
        printf( "shenh open jpg2 file fail=%x \n",fjpg);

    jpg2_size=(uint32)FS_Seek((int32)fjpg, 0,FS_SEEK_END );
    printf( "shenh  jpg2 size=%d \n",jpg2_size);
    FS_Seek((int32)fjpg, 0, FS_SEEK_SET);

    if((result=FS_Read((int32)fjpg,pvideo_b2,jpg2_size))>0)
    {
        printf( "shenh read  jpg2 file ok=%d \n", result);
    }
    else
        printf( "shenh read  jpg2 file err result=%d \n", result);

    FS_Close(fjpg);

    //amr
    if((faud=FS_Open("qzx.amr", FS_O_RDONLY, 0))>=0 )
    {
        printf("shenh open amr file OK faud=%x\n",faud);
    }
    else
        printf( "shenh open amr file fail=%x \n",faud);

    /***********creat avi file and seek head size**********************/

    if((favi=FS_Create("test.avi", 0))>0 )
    {
        printf("shenh creat avi  file OK favi=%x \n",favi);
    }
    else
        printf( "shenh creat avi file fail=%x \n",favi);

    memset(avi_head,0x00, AVI_HEAD_SIZE);
    printf("shenh begin write 1 \n");
    result=FS_Write((int32)favi, avi_head, AVI_HEAD_SIZE);
    printf( "shenh head write result=%d \n",result);

    /**************init avi enc*********************************/
    test_Avi_Init.Amr_BitRate=12800;
    test_Avi_Init.Audio_Bits=16;
    test_Avi_Init.Audio_Channel=1;
    test_Avi_Init.Audio_SampleRate=8000;
    test_Avi_Init.Image_Height=96;
    test_Avi_Init.Image_Width=128;
    test_Avi_Init.Video_fps=5;
    if(MMF_Avi_Enc_Init(&test_Avi_Init)==FUNC_OK)
        printf("shenh init ok\n");

    /*************write jpeg and amr***************************/
    for(i=0; i<100; i++)
    {
        //jpeg data
        if((i%2)==0)
        {
            test_avi_enc.Jpeg_Frame_Size=jpg_size;
            test_avi_enc.Jpeg_inBuf=pvideo_b;
        }
        else
        {
            test_avi_enc.Jpeg_Frame_Size=jpg2_size;
            test_avi_enc.Jpeg_inBuf=pvideo_b2;
        }

        //amr data
        if(i==0)
        {
            result=FS_Read(faud,paudio_b,(1600/test_Avi_Init.Video_fps)+6);
            printf( "shenh read aud file ok=%d\n", result);
        }
        else
        {
            result=FS_Read(faud,paudio_b,1600/test_Avi_Init.Video_fps);
            printf( "shenh read aud file size=%d\n", result);
        }

        test_avi_enc.Amr_Data_Size=result;
        test_avi_enc.Amr_inBuf=paudio_b;
        test_avi_enc.Avi_outBuf=avi_out;

        printf("shenh Avi_outBuf= %x Jpeg_inBuf=%x Amr_inBuf =%x \n",avi_out,test_avi_enc.Jpeg_inBuf,paudio_b);
        result=MMF_Avi_Enc_Frame(&test_avi_enc);
        res=FS_Write((int32) favi, avi_out, (int32)result);
        printf("shenh write avi result=%d res =%d\n",result,res);

        //FS_Close(fjpg);
    }

    /************write end data****************************/
    test_Avi_End.Buf_size=sizeof(avi_end);
    test_Avi_End.pAvi_End_Buf=avi_end;
    printf("shenh test_Avi_End.pAvi_End_Buf= %x  Buf_size=%d \n",test_Avi_End.pAvi_End_Buf,test_Avi_End.Buf_size);
    do
    {
        printf("shenh  RemainEndBytes=%d\n", RemainEndBytes);
        if(MMF_Avi_Enc_End(&test_Avi_End, &test_Avi_End_Re)==FUNC_OK)
            printf("shenh avi end ok");
        res=FS_Write((int32) favi, avi_end, (int32)test_Avi_End_Re.OutByte_size);
        printf( "shenh end write =%d ,RemainByte_size=%d \n",res,test_Avi_End_Re.RemainByte_size);
    }
    while(test_Avi_End_Re.RemainByte_size);

    /*************seek and write avi head********************************/
    result=(int)FS_Seek((int32)favi, 0, FS_SEEK_SET);
    printf( "shenh head seek result=%d \n",result);
    if(MMF_Avi_Enc_Head(avi_head)==FUNC_OK)
        printf("shenh head data ok \n");

    res=FS_Write((int32) favi, avi_head, AVI_HEAD_SIZE);
    printf( "shenh head write =%d \n",res);

    FS_Close(faud);
    FS_Close(favi);
}
#endif

