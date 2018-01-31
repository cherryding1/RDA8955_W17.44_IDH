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

#include "mmc_sndrec.h"
//#include "aud_analog.h"
#include "mci.h"
#include "mmc.h"
//#include "l1audio.h"
//#include "mmi_trace.h"
#include "ars_m.h"
#include "mmc_audiowav.h"
#include "hal_timers.h"
#include "fs.h"
#include "vpp_speech.h"

#include"mcip_debug.h"
#include"string.h"
#include"errorcode.h"
#include"dm.h"
#include"fmd_m.h"

extern BOOL Mic_open(VOID);
extern void Delay_for_audio(UINT32 time);

extern void aud_media_record_getdata(int32 event);

#ifdef FM_RADIO_RECORD
extern bool FMR_IsActive(void);
#endif


//#ifdef SOUND_RECORDER_SUPPORT
//#define FMR_IsActive() 0
//#endif
/*****maco define + ******/
#define WAVE_FILE_HEADER_SIZE (sizeof(WAVE_HEADER)+sizeof(WAVE_DATA_HEADER))
/*****maco define - ******/


PRIVATE  MCI_AUDIO_RECORD_BUFFER_PLAY_CALLBACK_T g_SndRecUserHandler =  NULL;

//app level define variable+
HANDLE recHandle = -1;
INT32 *sndRecBuffer;//[SNDRECBUF_LEN];// the whole record buffer
INT32 *sndRecRingBuffer;//[SNDRECBUF_LEN/2]; //half of the whole buffer for each half(consider the end as the second half) has a inturrupt

INT32 sndRecBufferLength;// the whole record buffer[SNDRECBUF_LEN];// the whole record buffer
ARS_REC_MODE_T mmc_Reccodec= 0xff;
mci_type_enum mmcRecFormat = -1;

static unsigned char g_RecordPlayOpen=0;
static unsigned char g_FMRecordpart=0xff;
//app level define variable-

//START record +
extern UINT32 g_audioRecBufStart;
extern UINT32 g_audioRecBufLen;
extern UINT32 g_audioRecBufOffset;
extern AUD_LEVEL_T audio_cfg;
extern AUD_ITF_T audioItf;
extern BOOL Speeching;
//char *g_pcm2mpeg;
//START record -

//MR122 to AMR+
#define MAX_SERIAL_SIZE 244 /* max. num. of serial bits */
/* frame size in serial bitstream file (frame type + serial stream + flags) */
#define SERIAL_FRAMESIZE (1+MAX_SERIAL_SIZE+5)  //250
#define L_FRAME      160  /* Frame size */

//MR122 to AMR-
const UINT16 IF1_475_table[] =
{
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,
    10,  11,  12,  13,  14,  15,  23,  24,  25,  26,
    27,  28,  48,  49,  61,  62,  82,  83,  47,  46,
    45,  44,  81,  80,  79,  78,  17,  18,  20,  22,
    77,  76,  75,  74,  29,  30,  43,  42,  41,  40,
    38,  39,  16,  19,  21,  50,  51,  59,  60,  63,
    64,  72,  73,  84,  85,  93,  94,  32,  33,  35,
    36,  53,  54,  56,  57,  66,  67,  69,  70,  87,
    88,  90,  91,  34,  55,  68,  89,  37,  58,  71,
    92,  31,  52,  65,  86,
};

const UINT16 IF1_515_table[] =
{
    7,   6,   5,   4,   3,   2,   1,   0,  15,  14,
    13,  12,  11,  10,  9,   8,  23,  24,  25,  26,
    27,  46,  65,  84, 45,  44,  43,  64,  63,  62,
    83,  82,  81, 102,101, 100,  42,  61,  80,  99,
    28,  47,  66,  85, 18,  41,  60,  79,  98,  29,
    48,  67,  17,  20, 22,  40,  59,  78,  97,  21,
    30,  49,  68,  86, 19,  16,  87,  39,  38,  58,
    57,  77,  35,  54, 73,  92,  76,  96,  95,  36,
    55,  74,  93,  32, 51,  33,  52,  70,  71,  89,
    90,  31,  50,  69, 88,  37,  56,  75,  94,  34,
    53,  72,  91,
};

const UINT16 IF1_59_table[] =
{
    0,   1,   4,   5,   3,   6,   7,   2,  13,  15,
    8,   9,  11,  12,  14,  10,  16,  28,  74,  29,
    75,  27,  73,  26,  72,  30,  76,  51,  97,  50,
    71,  96, 117,  31,  77,  52,  98,  49,  70,  95,
    116,  53,  99,  32,  78,  33,  79,  48,  69,  94,
    115,  47,  68,  93, 114,  46,  67,  92, 113,  19,
    21,  23,  22,  18,  17,  20,  24, 111,  43,  89,
    110,  64,  65,  44,  90,  25,  45,  66,  91, 112,
    54, 100,  40,  61,  86, 107,  39,  60,  85, 106,
    36,  57,  82, 103,  35,  56,  81, 102,  34,  55,
    80, 101,  42,  63,  88, 109,  41,  62,  87, 108,
    38,  59,  84, 105,  37,  58,  83, 104,
};
const UINT16 IF1_67_table[] =
{
    0,   1,   4,   3,   5,   6,  13,   7,   2,   8,
    9,  11,  15,  12,  14,  10,  28,  82,  29,  83,
    27,  81, 26,  80,  30,  84,  16,  55, 109,  56,
    110,  31, 85,  57, 111,  48,  73, 102, 127,  32,
    86,  51,  76, 105, 130,  52,  77, 106, 131,  58,
    112, 33,  87,  19,  23,  53,  78, 107, 132,  21,
    22,  18,  17,  20,  24,  25,  50,  75, 104, 129,
    47,  72, 101, 126,  54,  79, 108, 133,  46,  71,
    100, 125, 128, 103,  74,  49,  45,  70,  99, 124,
    42,  67,  96, 121,  39,  64,  93, 118,  38,  63,
    92, 117,  35,  60,  89, 114,  34,  59,  88, 113,
    44,  69,  98, 123,  43,  68,  97, 122,  41,  66,
    95, 120,  40,  65,  94, 119,  37,  62,  91, 116,
    36,  61,  90, 115,
};
const UINT16 IF1_74_table[] =
{
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,
    10,  11,  12,  13,  14,  15,  16,  26,  87,  27,
    88,  28,  89,  29,  90,  30,  91,  51,  80, 112,
    141,  52,  81, 113, 142,  54,  83, 115, 144,  55,
    84, 116, 145,  58, 119,  59, 120,  21,  22,  23,
    17,  18,  19,  31,  60,  92, 121,  56,  85, 117,
    146,  20,  24,  25,  50,  79, 111, 140,  57,  86,
    118, 147,  49,  78, 110, 139,  48,  77,  53,  82,
    114, 143, 109, 138,  47,  76, 108, 137,  32,  33,
    61,  62,  93,  94, 122, 123,  41,  42,  43,  44,
    45,  46,  70,  71,  72,  73,  74,  75, 102, 103,
    104, 105, 106, 107, 131, 132, 133, 134, 135, 136,
    34,  63,  95, 124,  35,  64,  96, 125,  36,  65,
    97, 126,  37,  66,  98, 127,  38,  67,  99, 128,
    39,  68, 100, 129,  40,  69, 101, 130,
};
const UINT16 IF1_795_table[] =
{
    8,   7,   6,   5,   4,   3,   2,  14,  16,   9,
    10,  12,  13,  15,  11,  17,  20,  22,  24,  23,
    19,  18,  21,  56,  88, 122, 154,  57,  89, 123,
    155,  58,  90, 124, 156,  52,  84, 118, 150,  53,
    85, 119, 151,  27,  93,  28,  94,  29,  95,  30,
    96,  31,  97,  61, 127,  62, 128,  63, 129,  59,
    91, 125, 157,  32,  98,  64, 130,   1,   0,  25,
    26,  33,  99,  34, 100,  65, 131,  66, 132,  54,
    86, 120, 152,  60,  92, 126, 158,  55,  87, 121,
    153, 117, 116, 115,  46,  78, 112, 144,  43,  75,
    109, 141,  40,  72, 106, 138,  36,  68, 102, 134,
    114, 149, 148, 147, 146,  83,  82,  81,  80,  51,
    50,  49,  48,  47,  45,  44,  42,  39,  35,  79,
    77,  76,  74,  71,  67, 113, 111, 110, 108, 105,
    101, 145, 143, 142, 140, 137, 133,  41,  73, 107,
    139,  37,  69, 103, 135,  38,  70, 104, 136,
};
const UINT16 IF1_102_table[] =
{
    7,   6,   5,   4,   3,   2,   1,   0,  16,  15,
    14,  13,  12,  11,  10,   9,   8,  26,  27,  28,
    29,  30,  31, 115, 116, 117, 118, 119, 120,  72,
    73, 161, 162,  65,  68,  69, 108, 111, 112, 154,
    157, 158, 197, 200, 201,  32,  33, 121, 122,  74,
    75, 163, 164,  66, 109, 155, 198,  19,  23,  21,
    22,  18,  17,  20,  24,  25,  37,  36,  35,  34,
    80,  79,  78,  77, 126, 125, 124, 123, 169, 168,
    167, 166,  70,  67,  71, 113, 110, 114, 159, 156,
    160, 202, 199, 203,  76, 165,  81,  82,  92,  91,
    93,  83,  95,  85,  84,  94, 101, 102,  96, 104,
    86, 103,  87,  97, 127, 128, 138, 137, 139, 129,
    141, 131, 130, 140, 147, 148, 142, 150, 132, 149,
    133, 143, 170, 171, 181, 180, 182, 172, 184, 174,
    173, 183, 190, 191, 185, 193, 175, 192, 176, 186,
    38,  39,  49,  48,  50,  40,  52,  42,  41,  51,
    58,  59,  53,  61,  43,  60,  44,  54, 194, 179,
    189, 196, 177, 195, 178, 187, 188, 151, 136, 146,
    153, 134, 152, 135, 144, 145, 105,  90, 100, 107,
    88, 106,  89,  98,  99,  62,  47,  57,  64,  45,
    63,  46,  55,  56,
};
const UINT16 IF1_122_table[] =
{
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,
    10,  11,  12,  13,  14,  23,  15,  16,  17,  18,
    19,  20,  21,  22,  24,  25,  26,  27,  28,  38,
    141,  39, 142,  40, 143,  41, 144,  42, 145,  43,
    146,  44, 147,  45, 148,  46, 149,  47,  97, 150,
    200,  48,  98, 151, 201,  49,  99, 152, 202,  86,
    136, 189, 239,  87, 137, 190, 240,  88, 138, 191,
    241,  91, 194,  92, 195,  93, 196,  94, 197,  95,
    198,  29,  30,  31,  32,  33,  34,  35,  50, 100,
    153, 203,  89, 139, 192, 242,  51, 101, 154, 204,
    55, 105, 158, 208,  90, 140, 193, 243,  59, 109,
    162, 212,  63, 113, 166, 216,  67, 117, 170, 220,
    36,  37,  54,  53,  52,  58,   57,  56,  62,  61,
    60,  66,  65,  64,  70,  69,   68, 104, 103, 102,
    108, 107, 106, 112, 111, 110, 116, 115, 114, 120,
    119, 118, 157, 156, 155, 161, 160, 159, 165, 164,
    163, 169, 168, 167, 173, 172, 171, 207, 206, 205,
    211, 210, 209, 215, 214, 213, 219, 218, 217, 223,
    222, 221,  73,  72,  71,  76,  75,  74,  79,  78,
    77,  82,  81,  80,  85,  84,  83, 123, 122, 121,
    126, 125, 124, 129, 128, 127, 132, 131, 130, 135,
    134, 133, 176, 175, 174, 179, 178, 177, 182, 181,
    180, 185, 184, 183, 188, 187, 186, 226, 225, 224,
    229, 228, 227, 232, 231, 230, 235, 234, 233, 238,
    237, 236,  96, 199,
};


const UINT8 AmrHead_table[8] =
{
    4,  12,  20,  28,  36,  44,   52,  60,
};

UINT16 IF1Table_size[8] =
{
    95, 103, 118, 134, 148, 159, 204, 244,
};

const UINT32 SNDRECBUF_LEN_AMR[8] =
{
    SNDRECBUF_LEN_AMR475,
    SNDRECBUF_LEN_AMR515,
    SNDRECBUF_LEN_AMR59,
    SNDRECBUF_LEN_AMR67,
    SNDRECBUF_LEN_AMR74,
    SNDRECBUF_LEN_AMR795,
    SNDRECBUF_LEN_AMR102,
    SNDRECBUF_LEN_AMR122,

};

static const UINT16   frame_bytes[8]= {12,13,15,17,19,20,26,31};
const UINT16   Voc_frame_bytes[8] =
{
    HAL_SPEECH_FRAME_SIZE_AMR475 ,
    HAL_SPEECH_FRAME_SIZE_AMR515,
    HAL_SPEECH_FRAME_SIZE_AMR59,
    HAL_SPEECH_FRAME_SIZE_AMR67,
    HAL_SPEECH_FRAME_SIZE_AMR74,
    HAL_SPEECH_FRAME_SIZE_AMR795,
    HAL_SPEECH_FRAME_SIZE_AMR102,
    HAL_SPEECH_FRAME_SIZE_AMR122,
};

const UINT16 *IF1_table[] =
{
    IF1_475_table,
    IF1_515_table,
    IF1_59_table,
    IF1_67_table,
    IF1_74_table,
    IF1_795_table,
    IF1_102_table,
    IF1_122_table,
};

extern const uint8  *Silence_table[];

void    Amr2AmrRing( UINT16  mode,    UINT8 * Amrsource , UINT8 * amrDesbuffer, UINT32 frameCnt)
{

    UINT16 i,j;
    UINT16 t;
    UINT8 a;

    UINT8 * source ;
    UINT8 *amrbuffer;
    UINT8  sorcerbit[SERIAL_FRAMESIZE];
    UINT8  destinbit[SERIAL_FRAMESIZE];

    UINT8  *serial = sorcerbit;
    UINT8  *dest = destinbit;
    UINT32 frame_count= frameCnt;
    const UINT16 * IF1_mode_table = IF1_table[mode];


    source = Amrsource;
    amrbuffer = amrDesbuffer;
    MCI_TRACE (MCI_AUDIO_TRC,0,"frame_count=%d, mode=%d", frame_count, mode);
// frame_count--;
    while (frame_count--)
    {
        serial = sorcerbit;
        dest = destinbit;
        *amrbuffer++ = AmrHead_table[mode];


        for(i = 0; i < frame_bytes[mode]-1; i++)
        {
            a = source[i];
            for(j = 0; j < 8; j++)
            {
                *serial++ = (a & 1);
                a >>= 1;
            }
        }

        t = IF1Table_size[mode]-(frame_bytes[mode]-1)*8;

        a = source[frame_bytes[mode]-1];

        for(i = 0; i < t; i++)
        {
            *serial++ = (a & 1);
            a >>= 1;
        }


        for(i = 0; i < IF1Table_size[mode]; i++)
        {
            dest[i] = sorcerbit[IF1_mode_table[i]];
        }

        for (j=0; j < frame_bytes[mode]-1; j++)
        {
            amrbuffer[j] =0;
            for (i=0; i<8; i++)
            {
                amrbuffer[j]|=  *dest++ <<(7- i);
            }
        }

        //  t = bits_no[mode]-(frame_bytes[mode]-1)*8;
        amrbuffer[frame_bytes[mode]-1] =0;

        for (i=0; i<t; i++)
        {
            amrbuffer[frame_bytes[mode]-1]|=*dest++ <<(t - i);
        }

        source = source+ Voc_frame_bytes[mode];
        amrbuffer = amrDesbuffer +  (frameCnt-frame_count)*(frame_bytes[mode]+1);

    }


}




VOID Amr122ToAmrRing(UINT32 *Amr122Buff, char *AmrRingBuff,UINT32 frameCnt)
{

    UINT16 new_speech[256];  /* Pointer to new speech data        */

    UINT16 Myring[250];
    UINT8 temp[32];
    UINT32 i, j;
    UINT32 serial, ref_serial;
    UINT16 a;
    UINT32 ref_tab[15];
    UINT32 frame_count= frameCnt;
// static UINT32 test;
    /*-----------------------------------------------------------------------*
     * Process speech frame by frame                                         *
     *-----------------------------------------------------------------------*/
//MCI_TRACE (MCI_AUDIO_TRC,0,"Amr122ToAmrRing,frameCnt is %d",frameCnt);

    while (frame_count--)
    {
        if (Amr122Buff != NULL)
        {
            for (i = 0; i < 8; ++i)     //read one frame data to ref_tab; 8 word
            {
                ref_tab[i] = *(Amr122Buff++);//read_tab_line(file_serial, &eof_flag);
            }
        }
        else
        {
            //ass(1);
            MCI_TRACE (MCI_AUDIO_TRC,0,"Amr122ToAmrRing,error 1");
        }
        for (i = 0; i < 8; i++)
        {
            serial = ref_tab[i];

            for (j = 0; j < 32; j++)
            {
                ref_serial = serial;
                a          = ref_serial & 1;
                serial     = serial >> 1;
                new_speech[i* 32 + j] = a;
            }
        }

        for (i = 0; i < SERIAL_FRAMESIZE; i++)
            Myring[i] = 0;

        for (i = 0; i < 244; i++)
        {
            Myring[i] = new_speech[IF1_122_table[i]];
        }

        Myring[244] = 0;
        Myring[245] = 0;
        Myring[246] = 0;
        Myring[247] = 0;

        temp[0] = 0x3c;
        for (j = 1; j < 32; j++)
        {
            temp[j] = 0;
            for (i = 0; i < 8; i++)
            {
                temp[j] |= (Myring[(j - 1) * 8 + i] << (7 - i));
            }
        }

#if 0
        if (!test++) // head
        {
            char head[6] = { 0x23, 0x21, 0x41, 0x4d, 0x52, 0x0a };

            //fwrite(head, 1, 6, amr_ring);
            memcpy(AmrRingBuff,head,6);
            AmrRingBuff += 6;
        }
#endif

        memcpy(AmrRingBuff,temp,32);
        AmrRingBuff += 32;

    }
    return ;
}

/*****************************note***************************************/
/*move cenhe fix from aud_medit.c to here,                                                              */
/*write 25 silence amr frames to avoid quick stop record can not create valid amr file problem,*/
/* temp fix, we will analysis and fix it (wang rui)                                                        */
/***********************************************************************/

INT32 sndRec_writeAMRSilence(HANDLE fhd, UINT16  mode)
{
    INT32 vi;
    INT32 len;

    //write 25 silence amr122 frames to avoid quick stop record can not create valid amr file problem
    for(vi = 0; vi < 25; vi++)
    {
        len = FS_Write(fhd,(UINT8 *)(Silence_table[mode]),frame_bytes[mode]+1);
        if(len!= (frame_bytes[mode]+1)) return -1;
    }

    return 0;
}



INT32 sndRec_writeWaveHeader(HANDLE fhd)
{
    UINT32 fileSize = 0;
    INT32 result=-1;
    WAVE_HEADER waveFormatHeader;
    WAVE_DATA_HEADER waveDataHeader;



    fileSize = (INT32)FS_GetFileSize(fhd);
    result=(INT32)FS_Seek(fhd,0,FS_SEEK_SET);
    MCI_TRACE (MCI_AUDIO_TRC,0,"sndRec_writeWaveHeader,fileSize is %d,result is %d",fileSize,result);
    if(ERR_SUCCESS != result || fileSize < 0)
    {
        return -1;
    }


    //wav header
    memcpy(waveFormatHeader.szRiff,"RIFF",4);
    waveFormatHeader.dwFileSize = fileSize- 8;
    memcpy(waveFormatHeader.szWaveFmt,"WAVEfmt ",8);
    waveFormatHeader.dwFmtSize = 16;
    waveFormatHeader.wFormatTag = 1;
#if (defined(CHIP_DIE_8955) && defined(FM_RECORD_WITH_NEW_PATH))
#ifdef __MMI_FM_RADIO__
    if(FMR_IsActive())
    {
        waveFormatHeader.nChannels = HAL_AIF_STEREO;
        waveFormatHeader.nSamplesPerSec = 44100;
    }
    else
#endif
    {
        waveFormatHeader.nChannels = HAL_AIF_MONO;
        waveFormatHeader.nSamplesPerSec = 8000;
    }
#else
    waveFormatHeader.nChannels = HAL_AIF_MONO;
    waveFormatHeader.nSamplesPerSec = 8000;
#endif

    waveFormatHeader.wBitsPerSample = 16;
    waveFormatHeader.nAvgBytesPerSec = waveFormatHeader.nChannels*waveFormatHeader.wBitsPerSample*waveFormatHeader.nSamplesPerSec/8;
    waveFormatHeader.nBlockAlign = waveFormatHeader.nChannels*waveFormatHeader.wBitsPerSample/8;
    //write wav header
    result=FS_Write(fhd,(UINT8 *)&waveFormatHeader,sizeof(WAVE_HEADER));
    if(result!=sizeof(WAVE_HEADER))
    {
        return -1;
    }

    //wav data header
    memcpy(waveDataHeader.szData,"data",4);
    waveDataHeader.dwDataSize = fileSize - 44;
    //write wav data header
    result=FS_Write(fhd,(UINT8 *)&waveDataHeader,sizeof(WAVE_DATA_HEADER));
    if(result!=sizeof(WAVE_DATA_HEADER))
    {
        return -1;
    }

    return 0;
}

AUD_ITF_T   rec_audioItf ;
//common-
INT32 Mmc_MicStartRecord(UINT32 *pBuffer, UINT32 nBufferSize,ARS_REC_MODE_T codec,BOOL loop_mode,ARS_USER_HANDLER_T recHanlder)
{
    UINT32 ret_val;
    //CSW_PROFILE_FUNCTION_ENTER(DM_MicStartRecord);

    if(pBuffer == NULL)
        return FALSE;
    if((nBufferSize == 0) || (codec > ARS_REC_MODE_DAF))
    {
        return FALSE;
    }
    mmc_Reccodec = codec;
    //hal_audio_MicOpen();
    //mic_record = TRUE;
    Mic_open();

    g_audioRecBufStart = (UINT32) pBuffer;
    g_audioRecBufLen = nBufferSize*4;
    g_audioRecBufOffset = 0;

    ARS_ENC_STREAM_T stream;
    switch (mmc_Reccodec)
    {
        case ARS_REC_MODE_DAF://mp3
        {

            stream.voiceQuality = FALSE;
            stream.sampleRate = HAL_AIF_FREQ_48000HZ;
            stream.channelNb = HAL_AIF_MONO;
            break;
        }
        case ARS_REC_MODE_PCM://wav
        {
            stream.voiceQuality = TRUE;
            stream.sampleRate = HAL_AIF_FREQ_8000HZ;
            stream.channelNb = HAL_AIF_MONO;
            break;
        }
        default://amr
        {
            stream.voiceQuality = TRUE;
            stream.sampleRate = HAL_AIF_FREQ_8000HZ;
            stream.channelNb = HAL_AIF_MONO;
            break;
        }
    }
    stream.startAddress = (UINT32)pBuffer;
    stream.length =  nBufferSize*4;
    stream.mode = codec;

    if (loop_mode)
    {
        // The mmi is not expecting any IRQ in loop mode
        stream.handler = recHanlder;
    }
    else
    {
#if 1 //modified by chenhe ,080814
        stream.handler =recHanlder;
#else
        stream.handler = record_end_Handler;
#endif
    }
    ARS_AUDIO_CFG_T  arsAudioCfg;

    arsAudioCfg.micLevel = audio_cfg.micLevel;
    arsAudioCfg.spkLevel = AUD_SPK_MUTE;
    arsAudioCfg.filter = NULL;

    int32 ars_err_temp=0;

    if(audioItf!=AUD_ITF_EAR_PIECE&&audioItf!=AUD_ITF_RECEIVER)
    {
        // Initial cfg
        audioItf = AUD_ITF_RECEIVER;
    }
    rec_audioItf=audioItf;

    if(!Speeching)
    {
        arsAudioCfg.micLevel = AUD_MIC_VOL_RECORD;
        audio_cfg.micLevel = AUD_MIC_VOL_RECORD;
    }

    ars_err_temp=(int32)ars_Record(audioItf, &stream, &arsAudioCfg, loop_mode);


    ret_val = (ars_err_temp == ARS_ERR_NO);

    Delay_for_audio(1);

    //CSW_PROFILE_FUNCTION_EXIT(DM_MicStartRecord);
    if(ret_val)
    {

        MCI_TRACE (MCI_AUDIO_TRC,0,"----Mmc_MicStartRecord is started: ok---!");

        return TRUE;
    }
    else
    {
        MCI_TRACE (MCI_AUDIO_TRC,0,"----Mmc_MicStartRecord is started: failure and error is %d---!",ars_err_temp);
        return -1;

    }

}

INT32 Mmc_FmStartRecord(UINT32 *pBuffer, UINT32 nBufferSize,ARS_REC_MODE_T codec,BOOL loop_mode,ARS_USER_HANDLER_T recHanlder)
{
    AUD_ITF_T itf=AUD_ITF_FM;

    if(pBuffer == NULL)
        return FALSE;
    if((nBufferSize == 0) || (codec > ARS_REC_MODE_DAF))
    {
        return FALSE;
    }

    mmc_Reccodec = codec;

    g_audioRecBufStart = (UINT32) pBuffer;
    g_audioRecBufLen = nBufferSize*4;
    g_audioRecBufOffset = 0;
    g_FMRecordpart = 0xff;
    FMD_ERR_T fmdError = FMD_ERR_NO;
    FMD_I2S_CFG_T fmdI2sCfg= {0};

    ARS_ENC_STREAM_T stream;
    switch (mmc_Reccodec)
    {
        case ARS_REC_MODE_DAF://mp3
        {
            stream.voiceQuality = FALSE;
            stream.sampleRate = HAL_AIF_FREQ_44100HZ;
#if (defined(CHIP_DIE_8955) && defined(FM_RECORD_WITH_NEW_PATH))
            stream.channelNb = HAL_AIF_STEREO;
#else
            stream.channelNb = HAL_AIF_MONO;
#endif

            fmdI2sCfg.freq=FMD_FREQ_48000HZ;
            break;
        }
        case ARS_REC_MODE_PCM://wav
        {
            stream.voiceQuality = TRUE;
            stream.sampleRate = HAL_AIF_FREQ_8000HZ;
            stream.channelNb = HAL_AIF_MONO;

#if (defined(CHIP_DIE_8955) && defined(FM_RECORD_WITH_NEW_PATH))
            stream.sampleRate = HAL_AIF_FREQ_44100HZ;
            stream.channelNb = HAL_AIF_STEREO;
            stream.voiceQuality = FALSE;
            fmdI2sCfg.freq=FMD_FREQ_44100HZ;
#endif

            break;
        }
        default://amr
        {
            stream.voiceQuality = TRUE;
            stream.sampleRate = HAL_AIF_FREQ_8000HZ;
            stream.channelNb = HAL_AIF_MONO;

            fmdI2sCfg.freq=FMD_FREQ_8000HZ;
            break;
        }
    }
    stream.startAddress = (UINT32)pBuffer;
    stream.length =  nBufferSize*4;
    stream.mode = codec;
    stream.handler = recHanlder;

    fmdI2sCfg.slave=TRUE;
    fmdI2sCfg.dataSigned = TRUE;
#if 0

    if (loop_mode)
    {
        // The mmi is not expecting any IRQ in loop mode
        stream.handler = recHanlder;
    }
    else
    {
        stream.handler =recHanlder;
    }
#endif

    ARS_AUDIO_CFG_T  arsAudioCfg;
    arsAudioCfg.spkLevel = audio_cfg.spkLevel;
    arsAudioCfg.filter = NULL;

#ifdef FM_LINEIN_RECORD

    arsAudioCfg.micLevel = AUD_MIC_VOL_RECORD;
#else
    arsAudioCfg.micLevel = AUD_MIC_MUTE;


    fmdError= fmd_I2sOpen( fmdI2sCfg);

    if (fmdError != FMD_ERR_NO)
    {
        MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT, 0,     "[MCI]fmd_I2sOpen FAILED");
        return AUD_ERR_RESOURCE_BUSY;
    }
    else
    {
        MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT, 0,     "[MCI]fmd_I2sOpen SUCCESS");
    }
#endif

    rec_audioItf = itf;

#if (defined(CHIP_DIE_8955) && defined(FM_RECORD_WITH_NEW_PATH))
    if(codec == ARS_REC_MODE_PCM || codec == ARS_REC_MODE_DAF)
        ars_EnFMRecNewPath(TRUE);
    else
        ars_EnFMRecNewPath(FALSE);
#endif

    ARS_ERR_T ars_ret = ars_Record(itf, &stream, &arsAudioCfg, loop_mode);

    Delay_for_audio(1);

    if(ars_ret == ARS_ERR_NO)
    {
        MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"----Mmc_FmStartRecord is started: ok---!");
        return TRUE;
    }
    else
    {
        MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"----Mmc_FmStartRecord is started: failure and error is %d---!",ars_ret);
        return FALSE;
    }
}

void sndrec_halfHandler()
{
    MCI_TRACE (MCI_AUDIO_TRC,0,"sndrec_halfHandler");

    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC),SNDREC_DATA_PART_HALF);
}

void sndrec_endHandler()
{
    MCI_TRACE (MCI_AUDIO_TRC,0,"sndrec_endHandler");

    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC),SNDREC_DATA_PART_END);
}

void sndrec_DrvHandler(ARS_STATUS_T status)
{
    MCI_TRACE (MCI_AUDIO_TRC,0,"sndrec_DrvHandler,status is %d",status);

    if(status ==ARS_STATUS_MID_BUFFER_REACHED)
    {
        ars_ReadData(sndRecBufferLength*2);
        sndrec_halfHandler();
    }
    else if(status ==ARS_STATUS_END_BUFFER_REACHED)
    {
        ars_ReadData(sndRecBufferLength*2);
        sndrec_endHandler();
    }
}


INT32 Mmc_sndRecStart(HANDLE fhd,U8 quality, mci_type_enum format, MCI_AUDIO_FILE_RECORD_CALLBACK_T callback,  MCI_AUDIO_RECORD_BUFFER_PLAY_CALLBACK_T usercallback)
{
    INT32 result;
    U8 snd_quality;
    recHandle = fhd;
    snd_quality = quality;
    MCI_TRACE (MCI_AUDIO_TRC,0,"Mmc_sndRecStart format:%d",format);
    g_RecordPlayOpen=1;

    sndRecBuffer=(INT32 *)mmc_MemMalloc(SNDRECBUF_LEN*4 + 1024);//need extra buffer for mp3enc.
    sndRecRingBuffer=(INT32 *)mmc_MemMalloc(SNDRECBUF_LEN*2);

    if(NULL == sndRecBuffer|| NULL == sndRecRingBuffer)
    {
        MCI_TRACE (MCI_AUDIO_TRC,0,"func=Mmc_sndRecStart: Memory allocation failure!");
        g_SndRecUserHandler = NULL;
        recHandle = -1;
        mmc_MemFreeAll();
        return -1;
    }

    g_SndRecUserHandler = usercallback;

    mmcRecFormat = format;
    switch(mmcRecFormat)
    {
        case MCI_TYPE_WAV_DVI_ADPCM:
        {
            if(g_SndRecUserHandler != NULL)
            {
                if(fhd == 0) sndRecBufferLength= SNDRECBUF_LEN;
                else if(fhd < 2000)
                {
                    sndRecBufferLength= 2000;

                }
                else
                {
                    sndRecBufferLength=fhd>SNDRECBUF_LEN? SNDRECBUF_LEN: fhd ;
                }
            }
            else
                sndRecBufferLength=SNDRECBUF_LEN;

            if(g_SndRecUserHandler  == NULL)
            {
                result=sndRec_writeWaveHeader(recHandle);
                //  result = FS_Write(recHandle,waveHeader,WAVE_FILE_HEADER_SIZE);
                if(result<0)
                {
                    MCI_TRACE (MCI_AUDIO_TRC,0,"sndRec_writeWaveHeader error!");
                    g_SndRecUserHandler = NULL;
                    recHandle = -1;
                    return -1;
                }

                result=(INT32)FS_Seek(recHandle,0,FS_SEEK_END);

                MCI_TRACE (MCI_AUDIO_TRC,0,"FS_seek pos:%d",result);

                if(result<0 )
                {
                    MCI_TRACE (MCI_AUDIO_TRC,0,"FS_seek error!");
                    g_SndRecUserHandler = NULL;
                    recHandle = -1;
                    return -1;
                }
            }

#ifdef FM_RADIO_RECORD
            if(FMR_IsActive())
                result = Mmc_FmStartRecord(sndRecBuffer,sndRecBufferLength,ARS_REC_MODE_PCM,TRUE,sndrec_DrvHandler);
            else
#endif
                result = Mmc_MicStartRecord(sndRecBuffer,sndRecBufferLength,ARS_REC_MODE_PCM,TRUE,sndrec_DrvHandler);

            if(result<0)
            {
                g_SndRecUserHandler = NULL;
                recHandle = -1;
                return -1;
            }
            break;
        }
        case MCI_TYPE_DAF:
        {

            sndRecBufferLength = 7680;//SNDRECBUF_LEN; //word
            // g_pcm2mpeg = (char*)mmc_MemMalloc(1152*4);//if lack of memory,try to use pcmbuf_overlay.

#ifdef FM_RADIO_RECORD
            if (FMR_IsActive())
            {
#if (defined(CHIP_DIE_8955) && defined(FM_RECORD_WITH_NEW_PATH))
                sndRecBufferLength = 7524; // for fm record with new data path.
#endif
                result = Mmc_FmStartRecord(sndRecBuffer,sndRecBufferLength,
                                           ARS_REC_MODE_DAF,TRUE,sndrec_DrvHandler);
            }
            else
#endif
                result = Mmc_MicStartRecord(sndRecBuffer,sndRecBufferLength,
                                            ARS_REC_MODE_DAF,TRUE,sndrec_DrvHandler);
            if (result<0)
            {
                g_SndRecUserHandler = NULL;
                recHandle = -1;
                return -1;
            }

            break;
        }
        default:

            if(snd_quality> 7) snd_quality = 7;
            // TODO: do not know why sndRec_writeAMRSilence is added,
            // and it causes noise when appending an amr file. Just Disable it.
            //if(g_SndRecUserHandler  == NULL)
            //sndRec_writeAMRSilence(fhd, snd_quality);
            sndRecBufferLength=SNDRECBUF_LEN_AMR[snd_quality];

#ifdef FM_RADIO_RECORD
            if(FMR_IsActive())
                result = Mmc_FmStartRecord(sndRecBuffer,sndRecBufferLength,snd_quality,TRUE,sndrec_DrvHandler);
            else
#endif
                result = Mmc_MicStartRecord(sndRecBuffer,sndRecBufferLength,snd_quality,TRUE,sndrec_DrvHandler);

            if(result<0)
            {
                g_SndRecUserHandler = NULL;
                recHandle = -1;
                return -1;
            }
            break;
    }
    mmc_EntryMode(MMC_MODE_RECORD,format);
    return 0;
}

INT32 Mmc_sndRecStop(void)
{
    INT32 result = AUD_ERR_NO;

    MCI_TRACE (MCI_AUDIO_TRC,0,"Mmc_sndRecStop!");

    MCI_TRACE (MCI_AUDIO_TRC,0,"rec_audioItf=%d!", rec_audioItf);


    g_RecordPlayOpen=0;
#ifdef __MMI_FM_RADIO__
    if(FMR_IsActive())
        result = ars_RecordStop();
    else
#endif
        result = ars_Stop();

    switch(mmcRecFormat)
    {
        case MCI_TYPE_WAV_DVI_ADPCM:
            if(0xff == g_FMRecordpart)
            {
                memcpy(sndRecRingBuffer,sndRecBuffer,sndRecBufferLength*2);
                if(g_SndRecUserHandler != NULL)
                {
                    g_SndRecUserHandler((UINT8 *)sndRecRingBuffer, sndRecBufferLength*2, (unsigned int*)&result);
                }
                else
                    result=FS_Write(recHandle, (UINT8 *)sndRecRingBuffer, sndRecBufferLength*2);
            }
            sndRec_writeWaveHeader(recHandle);
#ifdef CHIP_DIE_8955
            ars_EnFMRecNewPath(FALSE);
            break;
        case MCI_TYPE_DAF:
#endif //CHIP_DIE_8955
            break;
        default:
            break;
    }
    // mmc_MemFreeAll();
    mmc_ExitMode();
    g_SndRecUserHandler = NULL;
    recHandle = -1;
    return result;
}

INT32 Mmc_sndRecPause(void)
{
    //aud_AnalogStreamPause(AUD_ITF_LOUD_SPEAKER,TRUE);
    return ars_Pause(TRUE);

}

INT32 Mmc_sndRecResume(void)
{
    //aud_AnalogStreamPause(AUD_ITF_LOUD_SPEAKER,FALSE);
    ARS_ERR_T ret;
    ret = ars_Pause(FALSE);
    if(audio_cfg.micLevel == AUD_MIC_VOL_RECORD)
        audio_cfg.micLevel = AUD_MIC_ENABLE;

    return ret;
}
void sndRec_msgHandle(SNDREC_DATA_PART part)
{
    UINT32 AMRRing_frame;
    UINT32 WriteFramebuffLen=0;
    MCI_TRACE (MCI_AUDIO_TRC,0,"sndRec_msgHandle!");
    g_FMRecordpart = part;
    if (g_RecordPlayOpen==1)
    {
        MCI_TRACE (MCI_AUDIO_TRC,0,"sndRec_msgHandle,part is %d,mmcRecFormat is %d",part, mmcRecFormat);
        int32 result = -1;
        if (part == SNDREC_DATA_PART_HALF)
        {
            switch(mmcRecFormat)
            {
                case MCI_TYPE_AMR:
                {
                    AMRRing_frame = (sndRecBufferLength*2)/Voc_frame_bytes[mmc_Reccodec];
                    WriteFramebuffLen = AMRRing_frame*(frame_bytes[mmc_Reccodec]+1);

                    Amr2AmrRing(mmc_Reccodec,(UINT8 *)sndRecBuffer,
                                (UINT8 *)sndRecRingBuffer,AMRRing_frame);
                    int32 timebegin=hal_TimGetUpTime();

                    if(g_SndRecUserHandler != NULL)
                    {
                        g_SndRecUserHandler((UINT8 *)sndRecRingBuffer,
                                            WriteFramebuffLen, (unsigned int*)&result);
                    }
                    else
                        result=FS_Write(recHandle, (UINT8 *)sndRecRingBuffer, WriteFramebuffLen); //AMRRing_buffLen

                    int32 timeend=hal_TimGetUpTime();
                    // MCI_TRACE (MCI_AUDIO_TRC,0,"AMR half timeend = %d",timeend);
                    MCI_TRACE (MCI_AUDIO_TRC,0,"AMR half time = %dms",((timeend-timebegin)*1000)/16384);
                }
                break;
                case MCI_TYPE_WAV_DVI_ADPCM:
                case MCI_TYPE_DAF:
                {
                    WriteFramebuffLen = sndRecBufferLength*2;

                    int32 timebegin=hal_TimGetUpTime();
                    if(g_SndRecUserHandler != NULL)
                    {
                        memcpy(sndRecRingBuffer,sndRecBuffer,WriteFramebuffLen);

                        g_SndRecUserHandler((UINT8 *)sndRecRingBuffer,
                                            WriteFramebuffLen, (unsigned int*)&result);
                    }
                    else
                    {
                        result=FS_Write(recHandle,
                                        (UINT8 *)sndRecBuffer, WriteFramebuffLen);

                    }
                    int32 timeend=hal_TimGetUpTime();
                    //MCI_TRACE (MCI_AUDIO_TRC,0,"half timeend = %d",timeend);
                    MCI_TRACE (MCI_AUDIO_TRC,0,"half time = %dms",((timeend-timebegin)*1000)/16384);
                    break;

                }
                default:
                    break;
            }
        }
        else if (part == SNDREC_DATA_PART_END)
        {
            switch(mmcRecFormat)
            {
                case MCI_TYPE_AMR:
                {
                    AMRRing_frame = (sndRecBufferLength*2)/Voc_frame_bytes[mmc_Reccodec];
                    WriteFramebuffLen = AMRRing_frame*(frame_bytes[mmc_Reccodec]+1);

                    Amr2AmrRing(mmc_Reccodec, (UINT8 *)(sndRecBuffer+sndRecBufferLength/2),
                                (UINT8 *)sndRecRingBuffer,AMRRing_frame);

                    int32 timebegin=hal_TimGetUpTime();
                    if (g_SndRecUserHandler != NULL)
                    {
                        g_SndRecUserHandler((UINT8 *)sndRecRingBuffer,
                                            WriteFramebuffLen, (unsigned int*)&result);
                    }
                    else
                    {
                        result=FS_Write(recHandle,
                                        (UINT8 *)sndRecRingBuffer, WriteFramebuffLen);//AMRRing_buffLen
                    }
                    int32 timeend=hal_TimGetUpTime();
                    //  MCI_TRACE (MCI_AUDIO_TRC,0,"AMR end timeend = %d",timeend);
                    MCI_TRACE (MCI_AUDIO_TRC,0,"AMR end time = %dms",((timeend-timebegin)*1000)/16384);
                }
                break;
                case MCI_TYPE_WAV_DVI_ADPCM:
                case MCI_TYPE_DAF:
                {
                    WriteFramebuffLen = sndRecBufferLength*2;

                    int32 timebegin=hal_TimGetUpTime();
                    if(g_SndRecUserHandler != NULL)
                    {
                        memcpy(sndRecRingBuffer,
                               (UINT8 *)(sndRecBuffer+sndRecBufferLength/2),WriteFramebuffLen);

                        g_SndRecUserHandler((UINT8 *)sndRecRingBuffer,
                                            WriteFramebuffLen, (unsigned int*)&result);
                    }
                    else
                    {
                        result=FS_Write(recHandle,
                                        (UINT8 *)(sndRecBuffer+sndRecBufferLength/2), WriteFramebuffLen);

                    }
                    int32 timeend=hal_TimGetUpTime();
                    // MCI_TRACE (MCI_AUDIO_TRC,0," end timeend = %d",timeend);
                    MCI_TRACE (MCI_AUDIO_TRC,0,"end time = %dms",((timeend-timebegin)*1000)/16384);
                }
                default:
                    break;
            }
        }
        else
        {

            MCI_TRACE (MCI_AUDIO_TRC,0,"unknown message !");
        }

        if(result!=WriteFramebuffLen)
        {
            MCI_TRACE (MCI_AUDIO_TRC,0,"sndRec_msgHandle,write error");
            switch(mmcRecFormat)
            {
                case MCI_TYPE_AMR:
                case MCI_TYPE_DAF:
                    ars_Stop();
                    if(result == ERR_FS_DISK_FULL)
                        MCI_AudioRecordFinished(MCI_ERR_DISK_FULL);
                    else
                        MCI_AudioRecordFinished(MCI_ERR_ERROR);
                    break;
                case MCI_TYPE_WAV_DVI_ADPCM:
                    ars_Stop();
                    if (g_SndRecUserHandler == NULL)
                        sndRec_writeWaveHeader(recHandle);
                    MCI_AudioRecordFinished(MCI_ERR_ERROR);
                    break;
                default:
                    break;
            }
            Mmc_sndRecStop();
        }
        COS_Sleep(10);
    }
    else
    {
        MCI_TRACE (MCI_AUDIO_TRC,0,"record has been closed!");

    }

}

#define SoundRecord_Format 7

INT32 Mmc_SoundStartRecord(HANDLE fhd)
{
    UINT32 ret_val;
    ARS_ENC_STREAM_T stream;
    ARS_AUDIO_CFG_T  arsAudioCfg;
    int32 ars_err_temp=0;
    recHandle = fhd;

    sndRecBuffer=(INT32 *)mmc_MemMalloc(SNDRECBUF_LEN*4 + 1024);
    sndRecRingBuffer=(INT32 *)mmc_MemMalloc(SNDRECBUF_LEN*2);

    if(NULL == sndRecBuffer|| NULL == sndRecRingBuffer)
    {
        recHandle = -1;
        mmc_MemFreeAll();
        return -1;
    }
    sndRecBufferLength=SNDRECBUF_LEN_AMR[SoundRecord_Format];
    mmcRecFormat = MCI_TYPE_AMR;
    g_RecordPlayOpen=1;
    mmc_EntryMode(MMC_MODE_RECORD,MCI_TYPE_AMR);
    Mic_open();
    mmc_Reccodec = SoundRecord_Format;
    g_audioRecBufStart = (UINT32)sndRecBuffer;
    g_audioRecBufLen = sndRecBufferLength*4;
    g_audioRecBufOffset = 0;
    
    stream.voiceQuality = TRUE;
    stream.sampleRate = HAL_AIF_FREQ_8000HZ;
    stream.channelNb = HAL_AIF_MONO;
    stream.startAddress = (UINT32)sndRecBuffer;
    stream.length =  sndRecBufferLength*4;
    stream.mode = SoundRecord_Format;
    stream.handler = sndrec_DrvHandler;

    arsAudioCfg.micLevel = AUD_MIC_VOL_RECORD;
    arsAudioCfg.spkLevel = AUD_SPK_MUTE;
    arsAudioCfg.filter = NULL;

    audioItf = AUD_ITF_RECEIVER;
    audio_cfg.micLevel = AUD_MIC_VOL_RECORD;

    ars_err_temp=(int32)ars_Record(audioItf, &stream, &arsAudioCfg, TRUE);

    ret_val = (ars_err_temp == ARS_ERR_NO);
    if(ret_val)
    {
        return TRUE;
    }
    else
    {
        g_RecordPlayOpen=0;
        ars_Stop();
        mmcRecFormat = 0;
        mmc_ExitMode();
		recHandle = -1;
        return -1;
    }
}

INT32 Mmc_SoundStopRecord(void)
{
    INT32 result = AUD_ERR_NO;

    g_RecordPlayOpen=0;
    result = ars_Stop();
    mmcRecFormat = 0;
    mmc_ExitMode();
    if(result<0)
        return -1;
    else
        return TRUE;
}

/*
INT32 Mmc_FmRecStart(HANDLE fhd,mci_type_enum format)
{
    INT32 result;

    recHandle = fhd;
    MCI_TRACE (MCI_AUDIO_TRC,0,"Mmc_FmRecStart format:%d",format);
       g_RecordPlayOpen=1;

    sndRecBuffer=(INT32 *)mmc_MemMalloc(SNDRECBUF_LEN*4);
    sndRecRingBuffer=(INT32 *)mmc_MemMalloc(SNDRECBUF_LEN*2);

    mmcRecFormat = format;
    switch(mmcRecFormat)
    {
        case MCI_TYPE_WAV_DVI_ADPCM:
            sndRecBufferLength=SNDRECBUF_LEN;

            result=sndRec_writeWaveHeader(recHandle);

            if(result<0)
            {
                MCI_TRACE (MCI_AUDIO_TRC,0,"sndRec_writeWaveHeader error!");
                return -1;
            }

            result=FS_Seek(recHandle,0,FS_SEEK_END);

            MCI_TRACE (MCI_AUDIO_TRC,0,"FS_seek pos:%d",result);

            if(result<0 )
            {
                MCI_TRACE (MCI_AUDIO_TRC,0,"FS_seek error!");
                return -1;
            }

            result = Mmc_FmStartRecord(sndRecBuffer,sndRecBufferLength,DM_DICTAPHONE_PCM,TRUE,sndrec_DrvHandler);

            if(result<0)
            {
                return -1;
            }
            break;
        default:
            sndRecBufferLength=SNDRECBUF_LEN_AMR;
            result = Mmc_FmStartRecord(sndRecBuffer,sndRecBufferLength,DM_DICTAPHONE_MR122,TRUE,sndrec_DrvHandler);
            if(result<0)
            {
                return -1;
            }
            break;
    }
    mmc_EntryMode(MMC_MODE_RECORD,format);

    return 0;
}

*/



