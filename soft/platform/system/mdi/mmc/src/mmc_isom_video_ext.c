

/**************************
mmc_isom_video.c
for isomedia play (mp4 / 3gp)
author shenh
2008.2.26



***************************/
#include "global.h"

#ifdef MP4_3GP_SUPPORT

#include "cswtype.h"
#include "cs_types.h"
#include "cpu_share.h"
#include "vpp_audiojpeg_dec.h"
#include "mmc.h"

#include "hal_aif.h"
#include "aud_m.h"
#include "di.h"
#include "mci.h"

#include "mmc_timer.h"
#include "sxs_type.h"
#include "sxr_sbx.h"
#include "sxr_tls.h"

#include "mmc_isom_video.h"

#include "event.h"
#include "hal_timers.h"
#include "lcdd_m.h"
#include "hal_overlay.h"
#include "mcip_debug.h"

#include "mmc_audiompeg.h"

#ifdef MPEG4_SUPPORT
#include "xvid_decraw.h"
#endif

#ifdef H263_SUPPORT
#include "h263_global.h"
#endif
#include "mmi_jpeg_dec.h"

#if (CHIP_HAS_GOUDA == 1)
#include "hal_gouda.h"
#include "hal_speech.h"
#include "vpp_speech.h"
#endif

#ifdef RM_SUPPORT
#include "rm_dec.h"
#include "voc_cfg.h"
#endif

#ifdef DRA_SUPPORT
#include "DraExport.h"
#endif

#include "hal_sys.h"
#include "hal_clk.h"

#include "mdi_alg_common.h"
#include "dm.h"

//for mp4
typedef enum
{
    //Video player state.
    VID_PLAY_STATE_IDLE=0,
    VID_PLAY_STATE_PLAYING,
    VID_PLAY_STATE_PAUSE,

    //invalid after.
    VID_PLAY_STATE_INVALID
} VID_PLAY_STATE;

#define AAC_HEAD_SIZE 7
#define AMR_HEAD "#!AMR\n"
#define AMR_HEAD_SIZE 6

#define H264_START_CODE (0x01000000)
#define H264_START_CODE_SIZE 4

#define MAX_SLOW_NUM 2
#define AAC_FRAME_SAMPLE 1024
#define AMR_FRAME_SAMPLE 160
#define MP3_FRAME_SAMPLE_1152 1152
#define MP3_FRAME_SAMPLE_576 576
#define DRA_FRAME_SAMPLE_1024 1024
#define PCM_FRAME_SAMPLE 1024//user define

//NOTE:pcm pingpang buffer  must be less than 32KB
#ifdef RM_SUPPORT
#define PCM_BUF_AAC_FRAME_NUM (1*2)
#else
#define PCM_BUF_AAC_FRAME_NUM (1*2)
#endif
//#define PCM_BUF_AMR_FRAME_NUM (10*2)
#define PCM_BUF_AMR_FRAME_NUM (2*2)
#define PCM_BUF_MP3_FRAME_NUM (1*2)
#define PCM_BUF_PCM_FRAME_NUM (2*2)//user define
#define PCM_BUF_COOK_FRAME_NUM (2*2)
#define PCM_BUF_DRA_FRAME_NUM (2*2)

#define EXTRA_PCM_BUF_NUM 3 //use above half num as unit.

#define MAX_V_SAMPLEBUF_SIZE (1024*60) //1024*20 for QCIF .1024*40 for 240x180. will change with image width and height
#define MAX_V_SAMPLEBUF_SIZE2 (1024*150)
#define MAX_A_SAMPLEBUF_SIZE (1024*2*3)
#define MAX_A_SAMPLEBUF_MIRROR_SIZE 1536 //1024
#ifdef RM_SUPPORT
#define MAX_A_SAMPLEBUF_PIPE_SIZE (1024*6)
#else
#define MAX_A_SAMPLEBUF_PIPE_SIZE (1024*6)
#endif
#define PIPE_SPACE 4

#define SYNC_COEFF_SCALE_BIT 8
#define V_FPS_SCALE_BIT 8

#define SYNC_TIME_OFFSET 300 // time distance for next sync. (ms)

static unsigned char aacHead[AAC_HEAD_SIZE];

static volatile uint16 play_state=VID_PLAY_STATE_INVALID;


extern HAL_AIF_STREAM_T audioStream;
extern AUD_LEVEL_T audio_cfg;
extern AUD_ITF_T    audioItf;
extern MPEG_PLAY MpegPlayer;
#ifdef H264_SUPPORT
extern char *h264_mem_start;
#endif
extern const uint8 silence_frame475[];

#define USE_KEY_SYNC
#define USE_OVERLAY
//static uint8 USE_SLEEP;

#if (CSW_EXTENDED_API_LCD == 1)
static LCDD_FBW_T lcdDraw;
#endif

#if (CHIP_HAS_GOUDA == 1)
HAL_GOUDA_VID_LAYER_DEF_T Vid_Video;
static volatile uint32 RX_BUF_SWAP_ID;//pingpang
#endif

static IsomPlayGlobal *pIsomGlob;

#define VOC_WORK_NON 0
#define VOC_WORK_AUD 1
#define VOC_WORK_VID 2

static vpp_AudioJpeg_DEC_IN_T vocDecAud;
static volatile int16 VOC_VID;
static vpp_AudioJpeg_DEC_OUT_T vocDecStatus;
#define VOC_WORK_CONT (0x2000000)//total time=xxx ms
static volatile uint8 VOC_OPEN;
static volatile uint8 VOC_SETTING;
static volatile uint8 VOC_WORK;

VocWorkStruct vocWorkState;//for rm h264...
#ifdef VID_VOC_FREQ_SET
static HAL_SYS_VOC_FREQ_T VOC_FREQ_STATE = 0;
#endif
#ifdef VID_XCPU_FREQ_SET
static uint32 XCPU_FREQ_STATE;//0=normal
#endif

static volatile int16 VOC_AUD;
static volatile uint8 DISPLAY_INTERFACE;//0=not display time bar. 1=only display time bar. 2=display time bar with image area.0xff= control display area
static volatile uint16 AUD_DEC_FRM_NUM;
static volatile uint16 AUD_DEC_MIN;//voc decode AUD_DEC_MIN audio frame then decode 1 frame video. default 2, max half of the pcm buf. will be change in playing.
static volatile uint8 PCM_ISR_FLAG;
static volatile int16 AUD_DATA_REQ;//0 =no need, 1= need, 2=reposition && need
static volatile int16 VID_DEC_REQ;//+ need more decode; - too quick, skip decode
static volatile int16 SEND_EVENT_NUM;//avoid mail box full
static volatile int16 DISPLAY_SEND_EVENT_NUM;//avoid mail box full
static uint32 DTS_FLG;
static uint32 DTS_SampDelta;
static uint32 DTS_NextSampNode;
static uint32 cur_timer;
static uint16 MAX_KEEP_SKIP_F_NUM;
static uint16 keep_skip_frame_num;// B frame skip number continuous
//static uint16 skip_display_flag;
static UINT16 prebgcolor=0;

typedef struct
{
    volatile uint8 done;// 0=need dec, 1=dec done
    volatile uint8 *pBufPos;
    volatile uint32 timestamp;//record with pcm sample num.
} aud_extra_pcmbuf;
//static volatile uint8 AUD_EXTRA_BUF_DONE;// 0=need dec, 1=dec done
static aud_extra_pcmbuf aud_extra_pcmbuf_state[EXTRA_PCM_BUF_NUM];//more, pingpang or only one
static volatile uint8 A_EXBF_RD_ID;// will read from
static volatile uint8 A_EXBF_WT_ID;// will write to.
static int8 DISPLAY_STATE;// 1=in 0=out
static int8 AV_DEC_STATE;// 1=in 0=out

#define VID_ERR_FRAME_NUM 25 // max continuous error video frame number.
static uint16 VID_ERR_FRAME_REC;// record continuous error video frame number.

#ifdef USE_KEY_SYNC
static uint32 KEY_SAMP_NUM;
static int32 KEY_SAMP_TIME_WAIT;
static int32 KEY_SAMP_TIME_START;
#endif

static uint32 SYNC_NUM_COEFFICIENT;
static volatile uint32 LAST_SYNC_AUD_FRM_NUM;//reuse in rmvb for record pcm sample num.
static volatile uint32 pPcmBufPos;
static uint32 TIME_LINE_CUR; // for record time start for video sync without audio play.
static uint32 TIME_LINE_LAST;

#ifdef USE_OVERLAY
HAL_OVERLAY_ID_T VID_OVERLAY_ID;
#endif

#ifdef RM_SUPPORT
extern vpp_data_struct vpp_data_buffer[2];
extern volatile INT16  vpp_buffer_rd_id;
extern VPP_RMVB_CFG_T  vpp_rmvb_cfg;
static volatile uint8 needRA;
#endif

static volatile uint16 AUD_FADE_IN_FLAG;//zero frame num
static volatile uint16 AUD_FADE_OUT_FLAG;// 3 step. 3=pcm isr check and process fade out. 2=fade out will be play. 1=fade out over.

extern VOID mmc_MemCheck(BOOL bprintinfo);
extern char * mmc_MemMalloc(int32 nsize);
extern void mmc_MemFreeAll();
extern int mmc_MemGetFree();
static void vid_display(void) ;

static INT16 *g_VidBT8kPcmBuffer=NULL;
static  INT16  *g_VidBTDataAddress=0;
static INT32 g_VidBtOutputPcmBufSize =0;
static UINT32 g_VidBtcounterframe;
static INT16 g_VidBtNumOfOutSample=0;
static INT16 g_VidBtTotalFrame=0;
static UINT16 g_VidBtNout;
#define VID_BT_SCO_8K_OUTBUFF_SIZE   (8*1024)
static MPEG_INPUT  *my_input = &MpegPlayer.MpegInput;
static INT16 g_SbcEncEnable=0;

#ifdef RM_SUPPORT
extern void vpp_RMVBDecScheduleOneBlk(VPP_RMVB_CFG_T *dec_in_ptr);
#endif

extern INT32 ResampleInit(INT32 InSampleRate,INT32 OutSampleRate,INT32 FrameLength,INT32 NbChnanel);
extern INT32 ResampleOneFrame(INT16 *InBuf,INT16 * OutBuf);
extern VOID pmd_ReduceChargeCurrent(UINT8 low_current);

void (*fillAudioData)();//be call in video decoder
uint16 MBScale4AudRed=50;//MB number scale

int8 *pVid_ext_buf;//reuse mmi buf for video frame.(must be>= video decode resolution, example qvga 352x272x1.5)
//#ifdef VOCVID_CAUD_SUPPORT
//yuv buf for mjpeg
int8 *pVid_buf_Y[2];
int8 *pVid_buf_U[2];
int8 *pVid_buf_V[2];
int8 vid_buf_id;
//#endif

int8* pVid_SampleBuf[2];//frame sample buffer.only use in VOCVID and lowres(big frame size).
int8 vid_SampleBuf_id;
int vid_SampleBuf_BytesRemain;
int vid_SampleBuf_BytesUsed;
int vid_last_sample_num;

avframeinfohdstr avStrmFrmHdV;//keep last image frame header
avframeinfohdstr avStrmFrmHdA;//keep last audio frame header.(maybe n frame with one header)
unsigned int avStrmTimStpStartV;//record video start time stamp. wrap round when=0xffff ffff.
unsigned int avStrmTimStpStartA;//record audio start time stamp.wrap round when=0xffff ffff.
unsigned int avStrmCurPosA;//audio time line record with pcm sample num.
unsigned int avStrmCurPosV;//video time line record with pcm sample num.
unsigned int avStrmBaseClkA;//audio base clock for timestamp (exp. rtsp same with sample rate, cmmb 22500)
unsigned int avStrmBaseClkV;//video base clock for timestamo(exp.rtsp 90k, cmmb 22500)
unsigned int avStrmFrmTmScaleV;//video frame timestamp average scale.(fps=avStrmBaseClkV/avStrmFrmTmScaleV)
#ifdef DRA_SUPPORT
dra_frame_info DraFrameInfo;
int DraDownMixMode;
#endif

#define WRITE_AAC_HEAD(outbuf, sampLen)\
{\
    aacHead[3]&=0xfc;\
    aacHead[4]=0;\
    aacHead[5]&=0x1f;\
    aacHead[5]|=(((sampLen)&0x07)<<5);\
    aacHead[4]=(((sampLen)&0x07f8)>>3);\
    aacHead[3]|=(((sampLen)&0x1800)>>11);\
    memcpy((outbuf), aacHead, AAC_HEAD_SIZE);\
}

static uint32 tick1, tick2, totalTa,totalTv, counta, countv;//test for time

/*
get remain data lenght in pipe buffer.
assume that data cnsumeLen will not bigger than mirror.
*/
static int getDataLen()
{
    int dataInBuf;

    if(pIsomGlob->med_type==MCI_TYPE_AVSTRM)
    {
        avinfostr* avinfohdl= (avinfostr*)pIsomGlob->filehdl;
        return avstream_clt_getDataLen(avinfohdl->avstreamhdl_a);
    }

    //a_SampStart right, a_SampEnd left
    if(pIsomGlob->a_SampStart>pIsomGlob->a_SampEnd)
    {
        dataInBuf=pIsomGlob->a_SampBufPipeLen+(pIsomGlob->a_SampBufMirrorLen<<1)-(pIsomGlob->a_SampStart-pIsomGlob->a_SampEnd);

        if(pIsomGlob->a_SampEnd>=pIsomGlob->a_SampBuf+pIsomGlob->a_SampBufMirrorLen)
        {
            dataInBuf-=pIsomGlob->a_SampBufMirrorLen;
        }
        else
        {
            dataInBuf-=(pIsomGlob->a_SampEnd-pIsomGlob->a_SampBuf);
        }
    }
    else//a_SampStart left, a_SampEnd right
    {
        dataInBuf=pIsomGlob->a_SampEnd-pIsomGlob->a_SampStart;
    }

    return dataInBuf;
}

/*
get sample start position.
assume that data cnsumeLen will not bigger than mirror.
*/
static char* getDataPos()
{
    int8* startPos;
    uint8* ritMroStart;

    if(pIsomGlob->med_type==MCI_TYPE_AVSTRM)
    {
        avinfostr* avinfohdl= (avinfostr*)pIsomGlob->filehdl;
        return avstream_clt_getDataPos(avinfohdl->avstreamhdl_a);
    }

    // right mirror start position
    ritMroStart=pIsomGlob->a_SampBuf+pIsomGlob->a_SampBufPipeLen+pIsomGlob->a_SampBufMirrorLen;

    //sample start not in right mirror
    if(pIsomGlob->a_SampStart<ritMroStart)
    {
        startPos=pIsomGlob->a_SampStart;
    }
    else//sample start into the right mirror
    {
        uint8* lftMroPos;
        if(pIsomGlob->a_SampEnd<pIsomGlob->a_SampStart)
        {
            lftMroPos=pIsomGlob->a_SampBuf+(pIsomGlob->a_SampStart-ritMroStart);
            if(pIsomGlob->a_SampEnd>=lftMroPos)
            {
                startPos=lftMroPos;
            }
            else
                startPos=pIsomGlob->a_SampStart;
        }
        else
        {
            startPos=pIsomGlob->a_SampStart;
        }
    }

    return startPos;
}

/*
move sample start position.
assume that data cnsumeLen will not bigger than mirror.
*/
static void moveDataPos(int consumeLen)
{
    uint8* ritMroStart;
    uint8* moveStartPos;

    if(pIsomGlob->med_type==MCI_TYPE_AVSTRM)
    {
        avinfostr* avinfohdl= (avinfostr*)pIsomGlob->filehdl;
        avstream_clt_moveDataPos(avinfohdl->avstreamhdl_a,consumeLen);
        return;
    }

    // right mirror start position
    ritMroStart=pIsomGlob->a_SampBuf+pIsomGlob->a_SampBufPipeLen+pIsomGlob->a_SampBufMirrorLen;
    //sample start will move position
    moveStartPos=pIsomGlob->a_SampStart+consumeLen;

    if(moveStartPos<ritMroStart)
    {
        if(pIsomGlob->a_SampEnd<pIsomGlob->a_SampStart)
        {
            pIsomGlob->a_SampStart=moveStartPos;
        }
        else if(pIsomGlob->a_SampEnd>=moveStartPos)
        {
            pIsomGlob->a_SampStart=moveStartPos;
        }
        else
        {
            pIsomGlob->a_SampStart=pIsomGlob->a_SampEnd;
        }
    }
    else
    {
        uint8* lftMroPos;
        if(pIsomGlob->a_SampEnd<pIsomGlob->a_SampStart)
        {
            lftMroPos=pIsomGlob->a_SampBuf+(moveStartPos-ritMroStart);
            if(pIsomGlob->a_SampEnd>=lftMroPos)
            {
                pIsomGlob->a_SampStart=lftMroPos;
            }
            else
                pIsomGlob->a_SampStart=pIsomGlob->a_SampEnd;
        }
        else if(pIsomGlob->a_SampEnd>=moveStartPos)
        {
            pIsomGlob->a_SampStart=moveStartPos;
        }
        else
        {
            pIsomGlob->a_SampStart=pIsomGlob->a_SampEnd;
        }
    }
}

/*
copy data from buffer.
char* output,
unsigned int blkSize, //block size
unsigned int blkNum// block number
*/
static  int getData(unsigned char* output, unsigned int blkSize, unsigned int blkNum)
{
    int res;
    int getBlkNum;
    unsigned char *pTail;
    unsigned char *pOut;

    if(pIsomGlob->med_type==MCI_TYPE_AVSTRM)
    {
        avinfostr* avinfohdl= (avinfostr*)pIsomGlob->filehdl;
        return avstream_clt_getData(avinfohdl->avstreamhdl_a, output, blkSize, blkNum);
    }

    pTail=pIsomGlob->a_SampBuf+pIsomGlob->a_SampBufPipeLen+(pIsomGlob->a_SampBufMirrorLen<<1);
    pOut=output;
    getBlkNum=0;
    while(getDataLen()>=blkSize)
    {
        res=(int)(pTail-pIsomGlob->a_SampStart);
        if(res>=blkSize)
        {
            memcpy(pOut, pIsomGlob->a_SampStart, blkSize);
            moveDataPos(blkSize);
        }
        else
        {
            memcpy(pOut, pIsomGlob->a_SampStart, res);
            moveDataPos(res);

            memcpy(pOut+res, pIsomGlob->a_SampStart, blkSize-res);
            moveDataPos(blkSize-res);
        }

        pOut+=blkSize;
        getBlkNum++;
        if(getBlkNum==blkNum)break;
    }

    return getBlkNum;
}

/*
read data from bitstream and fill the out buffer full.
there have a temp buffer for bitstream,the size=mirror size
assume that the sample data will not bigger than mirror.
*/
static int readSamp(char *pBuf, int size)
{
    int res;
    int readSize;
    int8* pOut;
    uint8 useTmp;
    int8* pSamp;
    uint32 sampSize,gotbytes, gotcount;

    readSize=0;
    pOut=pBuf;

    if(pIsomGlob->a_SampTmpRemain>0)
    {
        if(pIsomGlob->a_SampTmpRemain>=size)
        {
            memcpy(pOut, pIsomGlob->a_SampTmpBufp, size);
            readSize=size;
        }
        else
        {
            memcpy(pOut, pIsomGlob->a_SampTmpBufp, pIsomGlob->a_SampTmpRemain);
            readSize=pIsomGlob->a_SampTmpRemain;
        }

        pOut+=readSize;
        pIsomGlob->a_SampTmpRemain-=readSize;
        pIsomGlob->a_SampTmpBufp+=readSize;
    }

    useTmp=0;
    while(readSize<size)
    {
        if(pIsomGlob->a_current_sample_num<pIsomGlob->a_total_sample_num)
        {
            if(pIsomGlob->med_type==MCI_TYPE_MP4 || pIsomGlob->med_type==MCI_TYPE_3GP)
            {
                if(pIsomGlob->a_type== A_TYPE_AAC)
                {
                    if((res = stbl_GetSampleSize(pIsomGlob->a_info , pIsomGlob->a_current_sample_num+1, &sampSize))!=GF_OK)
                    {
                        diag_printf("[VID] AUD GetSampleSize ERR! res=%d sample_num=%d \n",res, pIsomGlob->a_current_sample_num);
                        break;
                    }

                    if(readSize+AAC_HEAD_SIZE+sampSize<=size)
                    {
                        useTmp=0;
                        pSamp=pOut;
                    }
                    else if(AAC_HEAD_SIZE+sampSize<=pIsomGlob->a_SampBufMirrorLen)
                    {
                        useTmp=1;
                        pSamp=pIsomGlob->a_SampTmpBufp=pIsomGlob->a_SampTmpBuf;
                    }
                    else
                    {
                        diag_printf("[VID] AUD ERR! sample size=%d bigger than a_SampBufMirrorLen=%d",sampSize, pIsomGlob->a_SampBufMirrorLen);
                        break;
                    }

                    WRITE_AAC_HEAD(pSamp, AAC_HEAD_SIZE+sampSize);
                    pSamp+=AAC_HEAD_SIZE;

                    res=gf_isom_get_multSample((GF_MediaInformationBox *)pIsomGlob->a_info,
                                               pIsomGlob->a_current_sample_num+1,
                                               pSamp,
                                               sampSize,
                                               &gotbytes, &gotcount);

                    if(res || (gotbytes<=0))
                    {
                        diag_printf("[VID] AUD get_multSample. res=%d gotbytes=%d sampSize=%d sample_num=%d",res, gotbytes, sampSize, pIsomGlob->a_current_sample_num);
                        break;
                    }

                    if(useTmp==0)
                    {
                        readSize+=(AAC_HEAD_SIZE+gotbytes);
                        pOut+=(AAC_HEAD_SIZE+gotbytes);
                    }
                    else
                    {
                        pIsomGlob->a_SampTmpRemain=(AAC_HEAD_SIZE+gotbytes);
                        res=size-readSize;
                        if(res>pIsomGlob->a_SampTmpRemain)
                            res=pIsomGlob->a_SampTmpRemain;
                        memcpy(pOut, pIsomGlob->a_SampTmpBufp, res);
                        pIsomGlob->a_SampTmpRemain-=res;
                        pIsomGlob->a_SampTmpBufp+=res;
                        pOut+=res;
                        readSize+=res;
                    }
                    pIsomGlob->a_current_sample_num+=gotcount;

                }
                else if(pIsomGlob->a_type== A_TYPE_AMR || pIsomGlob->a_type== A_TYPE_MP3)
                {
                    if((res = stbl_GetSampleSize(pIsomGlob->a_info , pIsomGlob->a_current_sample_num+1, &sampSize))!=GF_OK)
                    {
                        diag_printf("[VID] AUD GetSampleSize ERR! res=%d sample_num=%d \n",res, pIsomGlob->a_current_sample_num);
                        break;
                    }

                    if(readSize+sampSize<=size)
                    {
                        useTmp=0;
                        pSamp=pOut;
                        sampSize=size-readSize;
                    }
                    else if(sampSize<=pIsomGlob->a_SampBufMirrorLen)
                    {
                        useTmp=1;
                        pSamp=pIsomGlob->a_SampTmpBufp=pIsomGlob->a_SampTmpBuf;
                        sampSize=pIsomGlob->a_SampBufMirrorLen;
                    }
                    else
                    {
                        diag_printf("[VID] AUD ERR! sample size=%d bigger than a_SampBufMirrorLen=%d",sampSize, pIsomGlob->a_SampBufMirrorLen);
                        break;
                    }

                    res=gf_isom_get_multSample((GF_MediaInformationBox *)pIsomGlob->a_info,
                                               pIsomGlob->a_current_sample_num+1,
                                               pSamp,
                                               sampSize,
                                               &gotbytes, &gotcount);

                    if(res || (gotbytes<=0))
                    {
                        diag_printf("[VID]AUD get_multSample res=%d gotbytes=%d sampSize=%d sample_num=%d",res, gotbytes, sampSize, pIsomGlob->a_current_sample_num);
                        break;
                    }

                    if(useTmp==0)
                    {
                        readSize+=gotbytes;
                        pOut+=gotbytes;
                    }
                    else
                    {
                        pIsomGlob->a_SampTmpRemain=gotbytes;
                        res=size-readSize;
                        if(res>pIsomGlob->a_SampTmpRemain)
                            res=pIsomGlob->a_SampTmpRemain;
                        memcpy(pOut, pIsomGlob->a_SampTmpBufp, res);
                        pIsomGlob->a_SampTmpRemain-=res;
                        pIsomGlob->a_SampTmpBufp+=res;
                        pOut+=res;
                        readSize+=res;
                    }
                    pIsomGlob->a_current_sample_num+=gotcount;

                }
            }

#ifdef RM_SUPPORT
            else if(pIsomGlob->med_type==MCI_TYPE_RM)
            {
                //int32 tt1,tt2;
//tt1=hal_TimGetUpTime();
                res= rm_getAudioData(pOut, size-readSize);
                if(res<=0)
                {
                    diag_printf("[VID] rm_getAudioData err res=%d\n", res);
                    res=0;
                }
                readSize+=res;
//tt2=hal_TimGetUpTime();
//diag_printf("ra read=%d",(tt2-tt1)*1000/16384);

                break;
            }
#endif
            else
            {
avi_audio:
                if((res=avi_read_audio(pIsomGlob->avi_p, pOut, size-readSize))<=0)
                {
                    diag_printf(  "[VID]avi_read_audio res =%d audio_posc=%d", res, pIsomGlob->avi_p->audio_posc);

                    if (pIsomGlob->avi_p->audio_chunks <= pIsomGlob->avi_p->audio_posc+1)
                    {
                        res=0;
                    }
                    else
                    {
                        pIsomGlob->a_current_sample_num=pIsomGlob->avi_p->audio_posc+1;
                        res =  avi_set_audio_position2(pIsomGlob->avi_p, pIsomGlob->a_current_sample_num);
                        if(res!=0)
                        {
                            diag_printf("[VID] avi_set_audio_position2 err! res=%d ", res);
                            //res=ISOM_ERR_AUDIO_READ_FAIL;
                            break;
                        }
                        goto avi_audio;
                    }
                }
                readSize+=res;
                pIsomGlob->a_current_sample_num=pIsomGlob->avi_p->audio_posc+1;
                break;
            }
        }
        else
        {
            break;
        }
    }

    return readSize;
}

static void fillSampBuf()
{
    uint8* lftMroEnd;
    uint8* ritMroStart;
    int32 fillSize;
    int32 readSize=0;

    if(pIsomGlob->med_type==MCI_TYPE_AVSTRM)
    {
        avinfostr *avinfohdl= (avinfostr*)pIsomGlob->filehdl;
        /*
        if(avinfohdl->avstream_srv_callback)
        {
            avinfohdl->avstream_srv_callback(0);
        }*/
        avstream_clt_checkagent(avinfohdl->avstreamhdl_a);
        return;
    }

    // left mirror end position
    lftMroEnd=pIsomGlob->a_SampBuf+pIsomGlob->a_SampBufMirrorLen;
    ritMroStart=lftMroEnd+pIsomGlob->a_SampBufPipeLen;

    while(1)
    {
        if(pIsomGlob->a_SampEnd+PIPE_SPACE<pIsomGlob->a_SampStart)
        {
            fillSize=pIsomGlob->a_SampStart-pIsomGlob->a_SampEnd-PIPE_SPACE;
            if(pIsomGlob->a_SampEnd>=lftMroEnd)
            {
                readSize=readSamp(pIsomGlob->a_SampEnd, fillSize);
                pIsomGlob->a_SampEnd+=readSize;

            }
            else
            {
                if(fillSize<=lftMroEnd-pIsomGlob->a_SampEnd)
                {
                    readSize=fillSize;
                    memcpy(pIsomGlob->a_SampEnd, ritMroStart+(pIsomGlob->a_SampEnd-pIsomGlob->a_SampBuf), readSize);
                    pIsomGlob->a_SampEnd+=readSize;
                }
                else
                {
                    readSize=lftMroEnd-pIsomGlob->a_SampEnd;
                    memcpy(pIsomGlob->a_SampEnd, ritMroStart+(pIsomGlob->a_SampEnd-pIsomGlob->a_SampBuf), readSize);
                    pIsomGlob->a_SampEnd+=readSize;

                    readSize=readSamp(pIsomGlob->a_SampEnd, fillSize-readSize);
                    pIsomGlob->a_SampEnd+=readSize;
                }
            }

            break;
        }
        else if(pIsomGlob->a_SampEnd>=pIsomGlob->a_SampStart)
        {
            if(pIsomGlob->a_SampEnd>=ritMroStart+pIsomGlob->a_SampBufMirrorLen)
            {
                if(pIsomGlob->a_SampStart>=pIsomGlob->a_SampBuf+PIPE_SPACE)
                {
                    pIsomGlob->a_SampEnd=pIsomGlob->a_SampBuf;
                    continue;
                }
                else
                    break;
            }

            fillSize=ritMroStart+pIsomGlob->a_SampBufMirrorLen-pIsomGlob->a_SampEnd;
            readSize=readSamp(pIsomGlob->a_SampEnd, fillSize);
            pIsomGlob->a_SampEnd+=readSize;

            if(readSize<fillSize)
                break;
        }
        else
            break;
    }
}

static void get_amr_mode(uint8 *pbuf, uint32 *pmode, uint32 *pframesize )
{
    //read amr ring header mode.
    //mode 8,15 and other will replace with 475 silence data and drop original data
    switch((pbuf[0] >> 3) & 0xF)
    {
        case 0:
            *pframesize = 13;
            *pmode = HAL_AMR475_DEC;
            //diag_printf("[VID] AMR mode 0, 4.75");
            break;
        case 1:
            *pframesize = 14;
            *pmode = HAL_AMR515_DEC;
            //diag_printf("[VID] AMR mode 1, 5.15");
            break;
        case 2:
            *pframesize = 16;
            *pmode = HAL_AMR59_DEC;
            //diag_printf("[VID] AMR mode 2, 5.9");
            break;
        case 3:
            *pframesize = 18;
            *pmode = HAL_AMR67_DEC;
            //diag_printf("[VID] AMR mode 3, 6.7");
            break;
        case 4:
            *pframesize = 20;
            *pmode = HAL_AMR74_DEC;
            //diag_printf("[VID] AMR mode 4, 7.4");
            break;
        case 5:
            *pframesize = 21;
            *pmode = HAL_AMR795_DEC;
            //diag_printf("[VID] AMR mode 5, 7.95");
            break;
        case 6:
            *pframesize = 27;
            *pmode = HAL_AMR102_DEC;
            //diag_printf("[VID] AMR mode 6, 10.2");
            break;
        case 7:
            *pframesize = 32;
            *pmode = HAL_AMR122_DEC;
            //diag_printf("[VID] AMR mode 7, 12.2");
            break;
        case 8:
            *pframesize = 6;
            *pmode = HAL_INVALID;
            diag_printf("[VID] AMR mode 8, SID");
            break;
        case 15:
            *pframesize = 1;
            *pmode = HAL_INVALID;
            diag_printf("[VID] AMR mode 15, NO DATA");
            break;

        default:
            //pIsomGlob->open_audio=0;
            *pframesize = 1;//move position 1byte
            *pmode = HAL_INVALID;
            diag_printf("[VID] WARNING! AMR head=%x , unsupport now!!! Audio Turn off!",pbuf[0]);
            break;
    }

}

/*
audio linear fade in
process couple of sample each time(assume 2 channel)
int16 *pPcm //buffer in
uint32 smpNum   // total sample count
*/

static void audFadeIn(int16 *pPcm, uint32 smpNum)
{
    int32 i;
    int32 pcm1,pcm2;
    int32 scale,scale2;
//int32 tt1,tt2;
//tt1=hal_TimGetUpTime();

    //32767=0x7FFF
    if(pPcm && smpNum>2)
    {
        smpNum>>=1;
        smpNum<<=1;
        scale=(32767+(smpNum>>1))/smpNum;
        if(scale==0)scale=1;
        for(i=0; i<smpNum; i+=2)
        {
            pcm1=(int32)*pPcm;
            pcm2=(int32)*(pPcm+1);
            scale2=i*scale;
            if(scale2>32767)break;
            *pPcm++=(int16)((pcm1*scale2)>>15);// 32767
            *pPcm++=(int16)((pcm2*scale2)>>15);// 32767
            //if(i>smpNum-64)
            //diag_printf("%d %d %d %d",pcm1,pcm2,*(pPcm-2),*(pPcm-1));
        }
    }
//tt2=hal_TimGetUpTime();
//diag_printf("fade in time=%d",(tt2-tt1));//*1000/16384
}

/*
audio linear fade out
process couple of sample each time(assume 2 channel)
int16 *pPcm //buffer in
uint32 smpNum   // total sample count
*/

static void audFadeOut(int16 *pPcm, uint32 smpNum)
{
    int32 i;
    int32 pcm1,pcm2;
    int32 scale,scale2;
//int32 tt1,tt2;
//tt1=hal_TimGetUpTime();

    //32767=0x7FFF
    if(pPcm && smpNum>2)
    {
        smpNum>>=1;
        smpNum<<=1;
        scale=(32767+(smpNum>>1))/smpNum;
        if(scale==0)scale=1;
        scale2=32767;
        for(i=0; i<smpNum; i+=2)
        {
            scale2=scale2-scale;

            if(scale2>0)
            {
                pcm1=(int32)*pPcm;
                pcm2=(int32)*(pPcm+1);
                *pPcm++=(int16)((pcm1*scale2)>>15);// 32767
                *pPcm++=(int16)((pcm2*scale2)>>15);// 32767
            }
            else
            {
                *pPcm++=0;
                *pPcm++=0;
            }

            //if(i>smpNum-64)
            //diag_printf("%d %d %d %d",pcm1,pcm2,*(pPcm-2),*(pPcm-1));
        }
    }
//tt2=hal_TimGetUpTime();
//diag_printf("fade out time=%d",(tt2-tt1));//*1000/16384
}

#if 0
static void fillAudioDataFun(void)
{
    fillSampBuf();

    if(pIsomGlob->a_type== A_TYPE_PCM && AUD_DEC_FRM_NUM>0)
    {
        int res;
        while(getDataLen()>0)
        {
            if(AUD_FADE_OUT_FLAG==0)
                res=getData((unsigned char*)pPcmBufPos, pIsomGlob->a_frameBytes, 1);
            else
            {
                moveDataPos(pIsomGlob->a_frameBytes);
                res=1;
            }

            if(res<=0)break;

            AUD_DEC_FRM_NUM-=(uint16)res;

            if((res>0)&&(pIsomGlob->a_channels==2)&&(audioItf==AUD_ITF_LOUD_SPEAKER))
            {
                uint32 i;
                INT16 *pPcm=(INT16 *)pPcmBufPos;
                uint32 sNum=pIsomGlob->a_frame_sample*2;

                for(i=0; i<sNum; i+=2)
                {
                    pPcm[i]=pPcm[i+1]=(pPcm[i]>>1)+(pPcm[i+1]>>1);
                }
            }

            pPcmBufPos+=pIsomGlob->a_frameBytes;
            fillSampBuf();

            if(AUD_DEC_FRM_NUM==0)break;
        }

        if(AUD_DEC_FRM_NUM>0)
        {
            memset((void*)pPcmBufPos, 0, pIsomGlob->a_frameBytes*AUD_DEC_FRM_NUM);
            AUD_DEC_FRM_NUM=0;
            diag_printf("fillAudioDataFun pcm over");
        }
    }
}
#endif

#ifdef RM_SUPPORT
//for ra c decode
int32 ra_nframe_dec(void)
{
    int32 res=0;
    if(aud_extra_pcmbuf_state[A_EXBF_WT_ID].done==0)
    {
        //int32 tt1,tt2;
        res=-1;
        AUD_DEC_FRM_NUM=0;

//tt1=hal_TimGetUpTime();

        while(AUD_DEC_FRM_NUM < pIsomGlob->a_HalfPcmBufFrame)
        {
            fillSampBuf();

            if(AUD_FADE_IN_FLAG==0)
            {
                if(getDataLen()>0)
                    res=Gecko2Decode(pIsomGlob->a_Gecko2DecHdl, getDataPos(), 0, (short*)pPcmBufPos);
                if(res!=0)
                {
                    memset((void*)pPcmBufPos,0,pIsomGlob->a_frameBytes);
                    diag_printf("[VID]AUD Gecko2Decode err! res=%d\n",res);
                    break;
                }
            }
            else
            {
                //mute frame
                if(AUD_FADE_IN_FLAG>1)
                    memset((uint8*)pPcmBufPos, 0, pIsomGlob->a_frameBytes);
                else
                {
                    audFadeIn((int16*)pPcmBufPos, pIsomGlob->a_frame_sample*pIsomGlob->a_channels);
                }
                AUD_FADE_IN_FLAG--;
            }

            moveDataPos(pIsomGlob->a_codeframeBytes);//const code frame bytes.
            pPcmBufPos+=pIsomGlob->a_frameBytes;

            AUD_DEC_FRM_NUM++;
        }

        aud_extra_pcmbuf_state[A_EXBF_WT_ID].done=1;
        A_EXBF_WT_ID+=1;
        if(A_EXBF_WT_ID>=EXTRA_PCM_BUF_NUM)A_EXBF_WT_ID=0;

        AUD_DEC_FRM_NUM=0;

        if(needRA)
        {
            if(AUD_FADE_OUT_FLAG==0)
                memcpy(PCM_ISR_FLAG ? (pIsomGlob->a_PcmBufHalfPos):(pIsomGlob->a_PcmBuf),
                       (void*)(aud_extra_pcmbuf_state[A_EXBF_RD_ID].pBufPos), pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame);
            aud_extra_pcmbuf_state[A_EXBF_RD_ID].done=0;
            A_EXBF_RD_ID+=1;
            if(A_EXBF_RD_ID>=EXTRA_PCM_BUF_NUM)A_EXBF_RD_ID=0;
            pPcmBufPos=(uint32)(aud_extra_pcmbuf_state[A_EXBF_WT_ID].pBufPos);
            needRA=0;
            //diag_printf("ra fill pcm");
        }
//tt2=hal_TimGetUpTime();
//diag_printf("ra time=%d",(tt2-tt1)*1000/16384);
    }

    return  res;
}
#endif


static int32 aud_nFrame_Sbc_enc(unsigned char *pInputBuf,unsigned char *pOutputBuf)
{

    SSHdl *my_ss_handle = MpegPlayer.MpegInput.ss_handle;
    bt_a2dp_audio_cap_struct * my_audio_config_handle=   MpegPlayer.MpegInput.audio_config_handle ;

    if (is_SSHdl_valid(my_ss_handle) && (my_audio_config_handle->codec_type ==1))//BT_A2DP_sbc
    {
        unsigned char *pstr,*pnd;
        int encoded,res = -1;
        int outputlen = 0,consumelen = 0;
        pstr = pInputBuf;
        pnd = pInputBuf + pIsomGlob->a_frameBytes;
        if (g_SbcEncEnable)
        {
            if (pIsomGlob->sbc.restart || (pIsomGlob->sbc.channels != pIsomGlob->a_channels)|| \
                    (pIsomGlob->a_sample_rate != pIsomGlob->sbc.samplerate))
            {
                pIsomGlob->sbc.inputlen = sbc_enc_cfg(pIsomGlob->a_channels,&pIsomGlob->sbc);
                switch (pIsomGlob->a_sample_rate)
                {
                    case 8000:
                    case 11025:
                    case 22050:
                    case 24000:
                        pIsomGlob->sbc.Resample = TRUE;
                        pIsomGlob->sbc.inputlen = sbc_resample_cfg(pIsomGlob->a_sample_rate,\
                                                  &pIsomGlob->sbc.frequency,&pIsomGlob->sbc.resampleup);
                        break;
                    case 16000:
                        pIsomGlob->sbc.frequency = SBC_FREQ_16000;
                        break;
                    case 32000:
                        pIsomGlob->sbc.frequency = SBC_FREQ_32000;
                        break;
                    case 44100:
                        pIsomGlob->sbc.frequency = SBC_FREQ_44100;
                        break;
                    case 48000:
                        pIsomGlob->sbc.frequency = SBC_FREQ_48000;
                        break;
                    default:
                        *((INT32*)pPcmBufPos)= 0;
                        diag_printf("ERROR SAMPLERATE");
                        return 0;
                }
                pIsomGlob->sbc.samplerate = pIsomGlob->a_sample_rate;
                pIsomGlob->sbc.channels  = pIsomGlob->a_channels;
                pIsomGlob->sbc.inputlen = pIsomGlob->a_channels * pIsomGlob->sbc.inputlen;
                pIsomGlob->sbc.restart = FALSE;
            }

            // diag_printf("Resample:%d,%d,%d,%d,%d",pIsomGlob->sbc.Resample,pIsomGlob->sbc.mono2dual,
            //  pIsomGlob->sbc.inputlen,pIsomGlob->sbc.mode,pIsomGlob->sbc.ResidualData );
            while ((pnd - pInputBuf)>=(pIsomGlob->sbc.inputlen*2))
            {
                res = sbc_encode(&pIsomGlob->sbc,(short*)pInputBuf,&pIsomGlob->sbc.inputlen,\
                                 pOutputBuf,2048,&encoded,&consumelen);
                if (res <= 0)
                {
                    diag_printf("sbc encode error %d",res);
                    encoded = 0;
                }
                pInputBuf += consumelen;
                pOutputBuf += encoded;
                outputlen += encoded;
            }
            pIsomGlob->sbc.ResidualData = pIsomGlob->a_frameBytes - (pInputBuf - pstr);
            // diag_printf("ResidualData:%d,%d,%d",pIsomGlob->sbc.ResidualData,outputlen,pIsomGlob->a_frameBytes);
            if (pIsomGlob->sbc.ResidualData > 0)
            {
                memcpy(pIsomGlob->sbc.extraBuf,pInputBuf,pIsomGlob->sbc.ResidualData);
            }
            else
            {
                pIsomGlob->sbc.ResidualData = 0;
            }
            //first 4bytes store the sbc data size.
            *((INT32*)pPcmBufPos)= outputlen;
            memcpy((void*)(pPcmBufPos+4),pIsomGlob->sbc.OutputBuffer,outputlen);
            //        diag_printf("sbc enc buf=%x len=%d\n",pPcmBufPos,*(int*)pPcmBufPos);
            return outputlen;
        }
        else
        {
            //first 4bytes store the sbc data size.
            *((INT32*)pPcmBufPos)= 0;
            diag_printf("skip encode sbc frame.\n");
            return 0;
        }
    }
	return 0;
}

/*
for audio c decode
return value:0=ok,-1=err,1=need more dec.
*/
static int32 aud_nframe_dec(void)
{
    int32 res= -1;
    uint8 *pStrm;
    uint32 byteLeft;

    if(AUD_DATA_REQ>0 && aud_extra_pcmbuf_state[A_EXBF_WT_ID].done==0)
    {
        int32 tt1,tt2;

        if( pIsomGlob->a_current_sample_num >= pIsomGlob->a_total_sample_num-1)
        {
            if(getDataLen()<32)
            {
                AUD_DATA_REQ=0;
                diag_printf("[VID] Audio Over!");
                return 0;
            }
        }

	tt1=hal_TimGetUpTime();

        if(AUD_DEC_FRM_NUM==0)//continue or new
            pPcmBufPos= (uint32)aud_extra_pcmbuf_state[A_EXBF_WT_ID].pBufPos;

        {


            res=-1;
//tt1=hal_TimGetUpTime();
            fillSampBuf();
//tt2=hal_TimGetUpTime();
//diag_printf("a fill t=%d",(tt2-tt1)*1000/16384);

            byteLeft=getDataLen();

            if(pIsomGlob->med_type==MCI_TYPE_AVSTRM)
            {
                if(avStrmFrmHdA.samplesize==0 && byteLeft > sizeof(avframeinfohdstr))
                {
                    avinfostr* avinfohdl= (avinfostr*)pIsomGlob->filehdl;
                    //res=getData((unsigned char*)&avStrmFrmHdA,sizeof(avframeinfohdstr),1);
                    res=avstream_clt_getheader(avinfohdl->avstreamhdl_a,&avStrmFrmHdA);
                    if(res==1)
                    {
                        if(AUD_DEC_FRM_NUM==0)
                        {
                            if(avStrmTimStpStartA!=0xFFFFFFFF)
                            {
                                aud_extra_pcmbuf_state[A_EXBF_WT_ID].timestamp=
                                    (uint32)((unsigned long long)(avStrmFrmHdA.timestamp -avStrmTimStpStartA)*(unsigned long long)pIsomGlob->a_sample_rate/(unsigned long long)avStrmBaseClkA);
                            }
                            else
                            {
                                aud_extra_pcmbuf_state[A_EXBF_WT_ID].timestamp=0;
                                avStrmTimStpStartA= avStrmFrmHdA.timestamp;
                            }
                        }
                        byteLeft-=sizeof(avframeinfohdstr);
                        //diag_printf("a ts %d sz %d lft %d",avStrmFrmHdA.timestamp,avStrmFrmHdA.samplesize,byteLeft);
                    }
                }

                if(byteLeft >= avStrmFrmHdA.samplesize)
                {
                    byteLeft=avStrmFrmHdA.samplesize;
                    avStrmFrmHdA.samplesize=0;
                }
                else
                    byteLeft=0;

            }

            if(byteLeft >0)
            {
                pStrm= getDataPos();

//tt1=hal_TimGetUpTime();
                switch(pIsomGlob->a_type)
                {
                    case A_TYPE_AAC:
                    {
                        uint32 oriByteLeft= byteLeft;
                        //diag_printf("buf %x pstrm %x %x %x %x left %d",pIsomGlob->a_SampBuf,pStrm[0],pStrm[1],pStrm[2],pStrm[3],byteLeft);
                        res=AACDecode(pIsomGlob->a_AacDecHdl,&pStrm,&byteLeft, (short*)pPcmBufPos);
                        pIsomGlob->a_codeframeBytes= oriByteLeft- byteLeft;
			if(res!=0 && pIsomGlob->a_codeframeBytes==0){
				INT32 offset;
				//for ADTS format
				//find first fake frame and skip it( 0xFFF).
				//make next decode from second frame
				offset= AACFindSyncWord(pStrm, byteLeft);
				if(offset >=0){ 
					pStrm += offset +1;
					byteLeft -= offset +1;
					offset = AACFindSyncWord(pStrm, byteLeft);
					if(offset >=0)
						byteLeft -= offset;
					else
						byteLeft=0;

					pIsomGlob->a_codeframeBytes= oriByteLeft- byteLeft;
				}else{
					pIsomGlob->a_codeframeBytes= byteLeft;
				}
				//pIsomGlob->a_codeframeBytes=4;
			}
                        //diag_printf("aac res=%d %d %x",res, pIsomGlob->a_codeframeBytes, *(uint32*)pPcmBufPos);
                        break;
                    }
                    case A_TYPE_MP3:
                    {
                        uint32 oriByteLeft= byteLeft;
                        //diag_printf("buf %x pstrm %x left %d",pIsomGlob->a_SampBuf,pStrm,byteLeft);
                        //res= MP3Decode(pIsomGlob->a_Mp3DecHdl,&pStrm,&byteLeft,(short*)pPcmBufPos,0);
                        res= MMF_Mp3_DecodeFrame(&pStrm, &byteLeft, (unsigned char*)pPcmBufPos, 0);
                        if(res>0)
                            res=0;
                        else
                            res=-1;
                        pIsomGlob->a_codeframeBytes= oriByteLeft- byteLeft;

                    }
                    break;
                    case A_TYPE_AMR:
                    {
                        uint32 oriByteLeft;
                        uint32 retry_num = 32;
                        uint32 retry_flag = 0;
                        uint32 retryFadeOut_flag = 1;
retry:
                        if(retry_flag)
                        {
                            if(retryFadeOut_flag)
                            {
                                audFadeOut((int16*) ((PCM_ISR_FLAG ? (pIsomGlob->a_PcmBufHalfPos):(pIsomGlob->a_PcmBuf))+ pIsomGlob->a_frameBytes*(pIsomGlob->a_HalfPcmBufFrame-1)),
                                           pIsomGlob->a_frame_sample*pIsomGlob->a_channels);
                                retryFadeOut_flag = 0;
                            }
                        }
                        oriByteLeft= byteLeft;

                        res= AmrFrameDec(pIsomGlob->a_AmrStruct,pStrm,(char*)pPcmBufPos,&byteLeft);
                        pIsomGlob->a_codeframeBytes= oriByteLeft- byteLeft;
                        if(pIsomGlob->a_codeframeBytes < 13)
                        {
                            if(retry_num>1)
                            {
                                moveDataPos(pIsomGlob->a_codeframeBytes);
                                fillSampBuf();
                                byteLeft=getDataLen();
                                pStrm= getDataPos();
                                retry_num--;
                                retry_flag = 1;
                                goto retry;
                            }
                        }

                        if(retry_flag)
                        {
                            audFadeIn((int16*)pPcmBufPos, pIsomGlob->a_frame_sample*pIsomGlob->a_channels);
                        }
                        if(res==1)res=0;
                        else res= -1;
                    }
                    break;

#ifdef RM_SUPPORT
                    case A_TYPE_COOK:
                        res=Gecko2Decode(pIsomGlob->a_Gecko2DecHdl, pStrm, 0, (short*)pPcmBufPos);
                        break;
#endif

#ifdef DRA_SUPPORT
                    case A_TYPE_DRA:
                    {
                        unsigned int outlen=pIsomGlob->a_frameBytes;
                        res=DRA_DecodeFrame(pIsomGlob->a_DraDecHdl,pStrm,byteLeft,(unsigned char*)pPcmBufPos,
                                            &outlen,DraDownMixMode,pIsomGlob->a_bit,&DraFrameInfo);
                        pIsomGlob->a_codeframeBytes= 0;
                    }
                    break;
#endif
                    case A_TYPE_PCM:

                        res=getData((unsigned char*)pPcmBufPos, pIsomGlob->a_frameBytes, 1);

                        if(res>0)
                        {
                            if((pIsomGlob->a_channels==2)&&(audioItf==AUD_ITF_LOUD_SPEAKER))
                            {
                                uint32 i;
                                INT16 *pPcm=(INT16 *)pPcmBufPos;
                                uint32 sNum=pIsomGlob->a_frame_sample*2;

                                for(i=0; i<sNum; i+=2)
                                {
                                    pPcm[i]=pPcm[i+1]=(pPcm[i]>>1)+(pPcm[i+1]>>1);
                                }
                            }
                            res= 0;
                        }
                        else
                            res= -1;
                        break;
                    default:
                        break;
                }
//tt2=hal_TimGetUpTime();
//diag_printf("a t=%d res %d %d %x",(tt2-tt1)*1000/16384,res,pIsomGlob->a_codeframeBytes,*(uint32*)pPcmBufPos);

            }


            if(res!=0)
            {
                memset((void*)pPcmBufPos,0,pIsomGlob->a_frameBytes);
                diag_printf("[VID]AUD dec err or data not enough! res=%d\n",res);
                res=-1;
                //break;
            }

            if(pIsomGlob->med_type==MCI_TYPE_AVSTRM)
            {
                pIsomGlob->a_codeframeBytes+=byteLeft;
            }

            //mute frame
            if(AUD_FADE_IN_FLAG>1)
            {
                memset((uint8*)pPcmBufPos, 0, pIsomGlob->a_frameBytes);
                AUD_FADE_IN_FLAG--;
            }
            else if(AUD_FADE_IN_FLAG==1)
            {
                audFadeIn((int16*)pPcmBufPos, pIsomGlob->a_frame_sample*pIsomGlob->a_channels);
                AUD_FADE_IN_FLAG= 0;
            }

            {
                INT16 *pPcmData = (INT16 *)pPcmBufPos;
                INT16 gain = aud_GetOutAlgGainDb2Val();
                int i;

                if(gain==0)
                {
                    memset((uint8*)pPcmBufPos, 0, pIsomGlob->a_frameBytes);
                }
                else if(gain != 16384)
                {
                    for(i = 0; i < (pIsomGlob->a_frameBytes>>1); i++)
                    {
                        pPcmData[i] = ((INT32)pPcmData[i] * gain) >>14;
                    }
                }
            }

            aud_nFrame_Sbc_enc((unsigned char *)pPcmBufPos,pIsomGlob->sbc.OutputBuffer);

            moveDataPos(pIsomGlob->a_codeframeBytes);//const code frame bytes.
            pPcmBufPos+=pIsomGlob->a_frameBytes;

            AUD_DEC_FRM_NUM++;

            if(pIsomGlob->med_type==MCI_TYPE_AVSTRM)
            {
                avinfostr *avinfohdl= (avinfostr*)pIsomGlob->filehdl;
                if((avinfohdl->avstreamhdl_a->ringbuflen -getDataLen()) > 2048 &&
                        avinfohdl->avstreamhdl_a->avstreamhdl_agent->bufupdate==0)
                {
                    avstream_clt_sendagent(avinfohdl->avstreamhdl_a);
                    if(avinfohdl->avstream_srv_callback)
                    {
                        avinfohdl->avstream_srv_callback(0);
                    }
                }
            }

        }

        //diag_printf("aud %d",AUD_DEC_FRM_NUM);

        if(AUD_DEC_FRM_NUM >= pIsomGlob->a_HalfPcmBufFrame)
        {
            aud_extra_pcmbuf_state[A_EXBF_WT_ID].done=1;
            A_EXBF_WT_ID+=1;
            if(A_EXBF_WT_ID>=EXTRA_PCM_BUF_NUM)A_EXBF_WT_ID=0;

            AUD_DEC_FRM_NUM=0;
            pPcmBufPos=0;

            if(VOC_AUD>0)
            {
                if(AUD_FADE_OUT_FLAG==0)
                    memcpy(PCM_ISR_FLAG ? (pIsomGlob->a_PcmBufHalfPos):(pIsomGlob->a_PcmBuf),
                           (void*)(aud_extra_pcmbuf_state[A_EXBF_RD_ID].pBufPos), pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame);
                aud_extra_pcmbuf_state[A_EXBF_RD_ID].done=0;
                A_EXBF_RD_ID+=1;
                if(A_EXBF_RD_ID>=EXTRA_PCM_BUF_NUM)A_EXBF_RD_ID=0;
                //pPcmBufPos=(uint32)(aud_extra_pcmbuf_state[A_EXBF_WT_ID].pBufPos);
                VOC_AUD--;
            }
        }

        if(aud_extra_pcmbuf_state[A_EXBF_WT_ID].done==0)res=1;//need more data

	tt2=hal_TimGetUpTime();
	totalTa+=(tt2-tt1);
	counta++;
    }

    return  res;
}


static void vid_pcmHalf_isr ()
{
    uint8 pcmUpdate=0;

    PCM_ISR_FLAG=0;

    //av sync
    if(VOC_AUD==0)
    {
        if(pIsomGlob->med_type!=MCI_TYPE_RM)
            pIsomGlob->a_current_frame_num+=pIsomGlob->a_HalfPcmBufFrame;
        else//reuse to record pcm sample
            pIsomGlob->a_current_frame_num+=pIsomGlob->a_frame_sample*pIsomGlob->a_HalfPcmBufFrame;

        if(pIsomGlob->med_type==MCI_TYPE_AVSTRM)
        {
            avStrmCurPosA+=pIsomGlob->a_frame_sample*pIsomGlob->a_HalfPcmBufFrame;
            //ajust when av different > 1 video frame
            //uint32 diffsmp= (avStrmFrmTmScaleV*pIsomGlob->a_sample_rate)/avStrmBaseClkV;
            if(avStrmCurPosA> avStrmCurPosV+pIsomGlob->v_period)
                VID_DEC_REQ= (int16)((avStrmCurPosA- avStrmCurPosV)/pIsomGlob->v_period);
            else if(avStrmCurPosA+pIsomGlob->v_period < avStrmCurPosV)
                VID_DEC_REQ= -(int16)((avStrmCurPosV- avStrmCurPosA)/pIsomGlob->v_period);

            pIsomGlob->current_time=(avStrmCurPosA/pIsomGlob->a_sample_rate)*1000;
            //diag_printf("sync a %d v %d",avStrmCurPosA,avStrmCurPosV);
        }
    }
    else//stop audio pos
    {
        VOC_AUD=0;
        LAST_SYNC_AUD_FRM_NUM= pIsomGlob->a_current_frame_num -pIsomGlob->a_sync_distance_frame_num;
    }

    //diag_printf("0 %d %d",pIsomGlob->a_current_frame_num,pIsomGlob->v_current_frame_num);

    if(AUD_FADE_OUT_FLAG==3)
    {
        //fade out  last frame
        audFadeOut((int16*) (pIsomGlob->a_PcmBufHalfPos+ pIsomGlob->a_frameBytes*(pIsomGlob->a_HalfPcmBufFrame-1)),
                   pIsomGlob->a_frame_sample*pIsomGlob->a_channels);
    }

    //if(getDataLen()>0)
    {
        if(pIsomGlob->dec_mode==1 &&(pIsomGlob->open_audio!=2 || pIsomGlob->med_type==MCI_TYPE_AVSTRM))
        {
            if(aud_extra_pcmbuf_state[A_EXBF_RD_ID].done==1)
            {
                if(AUD_FADE_OUT_FLAG==0)
                {
                    if( is_SSHdl_valid(MpegPlayer.MpegInput.ss_handle)&& (MpegPlayer.MpegInput.audio_config_handle->codec_type ==1)) //BT_A2DP_sbc
                    {
                        //PutData in isr is bad.fixme later.
                        int i;
                        for(i=0; i<pIsomGlob->a_HalfPcmBufFrame; i++)
                        {
                            int size;
                            int *p;

                            p= (int*)((aud_extra_pcmbuf_state[A_EXBF_RD_ID].pBufPos)+ i*pIsomGlob->a_frameBytes);
                            size= *p;
                            //      diag_printf("put sbc p=%x sz=%d\n",p,size);
                            p++;
                            if(size>0 && MpegPlayer.MpegInput.ss_handle->PutData(MpegPlayer.MpegInput.ss_handle,(UINT8*)p,size))
                            {
                                diag_printf("sbc data maybe overflow.\n");
                                return;
                            }
                        }
                    }
                    else
                    {
                        memcpy(pIsomGlob->a_PcmBuf,(VOID*)((uint32)(aud_extra_pcmbuf_state[A_EXBF_RD_ID].pBufPos)|0x20000000), pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame);
                    }
                }
                if(pIsomGlob->med_type==MCI_TYPE_AVSTRM && aud_extra_pcmbuf_state[A_EXBF_RD_ID].timestamp!=0)
                {
                    avStrmCurPosA= aud_extra_pcmbuf_state[A_EXBF_RD_ID].timestamp -pIsomGlob->a_frame_sample*pIsomGlob->a_HalfPcmBufFrame;
                    aud_extra_pcmbuf_state[A_EXBF_RD_ID].timestamp=0;
                }
                aud_extra_pcmbuf_state[A_EXBF_RD_ID].done=0;
                A_EXBF_RD_ID+=1;
                if(A_EXBF_RD_ID>=EXTRA_PCM_BUF_NUM)A_EXBF_RD_ID=0;
                //pPcmBufPos=(uint32)(aud_extra_pcmbuf_state[A_EXBF_WT_ID].pBufPos);
                pcmUpdate=1;
            }
            else
            {
                //pPcmBufPos=(uint32)(aud_extra_pcmbuf_state[A_EXBF_WT_ID].pBufPos);
                if(AUD_DATA_REQ) VOC_AUD=1;
                pcmUpdate=0;
            }
        }
        else
        {
            if(pIsomGlob->a_type== A_TYPE_COOK)
            {
#ifdef RM_SUPPORT
                if(aud_extra_pcmbuf_state[A_EXBF_RD_ID].done==1)
                {
                    if(AUD_FADE_OUT_FLAG==0)
                        memcpy(pIsomGlob->a_PcmBuf,(VOID*)((uint32)(aud_extra_pcmbuf_state[A_EXBF_RD_ID].pBufPos)|0x20000000), pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame);
                    aud_extra_pcmbuf_state[A_EXBF_RD_ID].done=0;
                    A_EXBF_RD_ID+=1;
                    if(A_EXBF_RD_ID>=EXTRA_PCM_BUF_NUM)A_EXBF_RD_ID=0;
                    pPcmBufPos=(uint32)(aud_extra_pcmbuf_state[A_EXBF_WT_ID].pBufPos);
                    pcmUpdate=1;
                }
                else
                {
                    needRA=1;
                    pcmUpdate=0;
                }
#endif

            }
            else if(pIsomGlob->a_type!= A_TYPE_PCM)
            {
                if(aud_extra_pcmbuf_state[A_EXBF_RD_ID].done==1)
                {
                    if(AUD_FADE_OUT_FLAG==0)
                    {
                        memcpy(pIsomGlob->a_PcmBuf, (VOID*)((uint32)(aud_extra_pcmbuf_state[A_EXBF_RD_ID].pBufPos)|0x20000000), pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame);
                        if((my_input->bt_sco==TRUE) &&(pIsomGlob->a_type != A_TYPE_AMR))
                        {
                            g_VidBTDataAddress = (INT16*)pIsomGlob->a_PcmBuf;
                            if(pIsomGlob->a_channels==2)
                            {
                                int i;
                                INT16 *PCM_p=(INT16 *)g_VidBTDataAddress;
                                for(i=0; i< g_VidBtOutputPcmBufSize ; i=i+2)
                                {
                                    PCM_p[i/2]=(PCM_p[i]>>1)+(PCM_p[i+1]>>1);
                                }
                            }

                            g_VidBtNout=ResampleOneFrame(g_VidBTDataAddress,g_VidBT8kPcmBuffer+g_VidBtcounterframe*g_VidBtNumOfOutSample);
                            g_VidBtcounterframe++;
                            if(g_VidBtcounterframe >= g_VidBtTotalFrame)
                                g_VidBtcounterframe = 0;
                            MCI_TRACE (TSTDOUT,0,"g_VidBtNout55555 = %d",g_VidBtNout);
                        }
                    }
#if 1
                    aud_extra_pcmbuf_state[A_EXBF_RD_ID].done=0;
                    A_EXBF_RD_ID+=1;
                    if(A_EXBF_RD_ID>=EXTRA_PCM_BUF_NUM)A_EXBF_RD_ID=0;

                    if(VOC_WORK==VOC_WORK_NON && VOC_SETTING==0 && getDataLen()>0)
                    {
                        //diag_printf("[VID]pcm call 1");
                        VOC_WORK=VOC_WORK_AUD;
                        pPcmBufPos=(uint32)(aud_extra_pcmbuf_state[A_EXBF_WT_ID].pBufPos);
#if( CHIP_HAS_GOUDA == 1 )
                        if(pIsomGlob->a_type != A_TYPE_AMR)
                        {
#endif
                            vocDecAud.inStreamBufAddr=(INT32 *)getDataPos();
                            vocDecAud.outStreamBufAddr=(INT32 *)pPcmBufPos;
                            vocDecAud.audioItf=audioItf;

                            /*if(pIsomGlob->v_type ==V_TYPE_H264 || pIsomGlob->v_type ==V_TYPE_RV
                                ||pIsomGlob->v_type ==V_TYPE_MPEG4
                                )
                                vpp_AVDecSetMode(vocDecAud.mode, 1, 0);//set video code invalide
                                */

                            if(vpp_AudioJpegDecScheduleOneFrame(&vocDecAud)!=HAL_ERR_NO)//aac 1024??
                            {
                                VOC_WORK=VOC_WORK_NON;
                                diag_printf("[VID]voc call err!");
                            }
                            //diag_printf("pcm call");
#if( CHIP_HAS_GOUDA == 1 )
                        }
                        else
                        {
                            HAL_SPEECH_DEC_IN_T *pVocDecAmr=vpp_SpeechGetRxCodBuffer();

                            get_amr_mode( getDataPos(), &(pIsomGlob->a_amr_mode), &(pIsomGlob->a_codeframeBytes));

                            if(pIsomGlob->a_amr_mode != HAL_INVALID)
                            {
                                getData((U8*)pVocDecAmr->decInBuf, pIsomGlob->a_codeframeBytes, 1);
                                pVocDecAmr->codecMode= pIsomGlob->a_amr_mode;
                            }
                            else
                            {
                                //write silence data
                                moveDataPos(pIsomGlob->a_codeframeBytes);
                                memcpy(pVocDecAmr->decInBuf, silence_frame475, 13);
                                pVocDecAmr->codecMode= HAL_AMR475_DEC;
                            }

                            // Set all the preprocessing modules
                            vpp_SpeechSetEncDecPocessingParams(/*audioItf*/AUD_ITF_LOUD_SPEAKER, audio_cfg.spkLevel, 0);
                            if(vpp_SpeechScheduleOneFrame(VPP_SPEECH_WAKEUP_SW_DEC) != HAL_ERR_NO)
                            {
                                VOC_WORK=VOC_WORK_NON;
                                diag_printf("[VID]voc call err!");
                            }
                        }
#endif

                    }
#endif
                    pcmUpdate=1;
                }
                else
                {
                    //diag_printf("[VID]pcm call 2");
                    if(VOC_WORK==VOC_WORK_NON && VOC_SETTING==0 && getDataLen()>0)
                    {
                        VOC_WORK=VOC_WORK_AUD;
                        VOC_AUD++;
                        //pPcmBufPos=(uint32)pIsomGlob->a_PcmBuf;
                        pPcmBufPos=(uint32)(aud_extra_pcmbuf_state[A_EXBF_WT_ID].pBufPos);

#if( CHIP_HAS_GOUDA == 1 )
                        if(pIsomGlob->a_type != A_TYPE_AMR)
                        {
#endif
                            vocDecAud.inStreamBufAddr=(INT32 *)getDataPos();
                            vocDecAud.outStreamBufAddr=(INT32 *)pPcmBufPos;
                            vocDecAud.audioItf=audioItf;

                            /*if(pIsomGlob->v_type ==V_TYPE_H264 || pIsomGlob->v_type ==V_TYPE_RV
                                ||pIsomGlob->v_type ==V_TYPE_MPEG4
                                )
                                vpp_AVDecSetMode(vocDecAud.mode, 1, 0);//set video code invalide
                                */

                            if(vpp_AudioJpegDecScheduleOneFrame(&vocDecAud)!=HAL_ERR_NO)//aac 1024??
                            {
                                VOC_WORK=VOC_WORK_NON;
                                diag_printf("[VID]voc call err!");
                            }
                            //diag_printf("pcm call");
#if( CHIP_HAS_GOUDA == 1 )
                        }
                        else
                        {
                            HAL_SPEECH_DEC_IN_T *pVocDecAmr=vpp_SpeechGetRxCodBuffer();

                            get_amr_mode( getDataPos(), &(pIsomGlob->a_amr_mode), &(pIsomGlob->a_codeframeBytes));

                            if(pIsomGlob->a_amr_mode != HAL_INVALID)
                            {
                                getData((U8*)pVocDecAmr->decInBuf, pIsomGlob->a_codeframeBytes, 1);
                                pVocDecAmr->codecMode= pIsomGlob->a_amr_mode;
                            }
                            else
                            {
                                //write silence data
                                moveDataPos(pIsomGlob->a_codeframeBytes);
                                memcpy(pVocDecAmr->decInBuf, silence_frame475, 13);
                                pVocDecAmr->codecMode= HAL_AMR475_DEC;
                            }

                            // Set all the preprocessing modules
                            vpp_SpeechSetEncDecPocessingParams(/*audioItf*/AUD_ITF_LOUD_SPEAKER, audio_cfg.spkLevel, 0);
                            if(vpp_SpeechScheduleOneFrame(VPP_SPEECH_WAKEUP_SW_DEC) != HAL_ERR_NO)
                            {
                                VOC_WORK=VOC_WORK_NON;
                                diag_printf("[VID]voc call err!");
                            }
                        }
#endif
                    }
                    else if(getDataLen()>0)
                    {
                        if(VOC_AUD!=0)
                            AUD_DEC_MIN=0;
                        //else
                        //  AUD_DEC_MIN=2;

                        VOC_AUD++;
                    }

                    pcmUpdate=0;
                }
            }
            else
            {
                pPcmBufPos=(uint32)pIsomGlob->a_PcmBuf;
                AUD_DEC_FRM_NUM=(uint16)pIsomGlob->a_HalfPcmBufFrame;
                pcmUpdate=1;
            }

        }

        if(AUD_FADE_OUT_FLAG>0)
        {
            memset((uint8*)pIsomGlob->a_PcmBuf, 0, pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame);
            if(AUD_FADE_OUT_FLAG>1)AUD_FADE_OUT_FLAG--;
            pcmUpdate=1;
        }

        if(SEND_EVENT_NUM==0 )
        {
            SEND_EVENT_NUM=1;
            mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_AUD_DEC_REQ);
            //diag_printf("0 pcm send");
        }
    }

    if(pcmUpdate==0)
    {
        //aud_StreamStop(audioItf);
        if(AUD_DATA_REQ==1 && pIsomGlob->dec_mode==0)AUD_DATA_REQ=2;
        memset(pIsomGlob->a_PcmBuf, 0, pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame);
        diag_printf("[VID]pcm isr, aud no data");
    }

}

static void vid_pcmEnd_isr ()
{
    long long syncF;
    uint8 pcmUpdate=0;

    PCM_ISR_FLAG=1;

    //av sync
    if(VOC_AUD==0)
    {
        if(pIsomGlob->med_type!=MCI_TYPE_RM)
            pIsomGlob->a_current_frame_num+=pIsomGlob->a_HalfPcmBufFrame;
        else//reuse to record pcm sample
            pIsomGlob->a_current_frame_num+=pIsomGlob->a_frame_sample*pIsomGlob->a_HalfPcmBufFrame;

        if(pIsomGlob->med_type==MCI_TYPE_AVSTRM)
        {
            avStrmCurPosA+=pIsomGlob->a_frame_sample*pIsomGlob->a_HalfPcmBufFrame;
            //ajust when av different >= 2 video frame
            //uint32 diffsmp= (avStrmFrmTmScaleV*pIsomGlob->a_sample_rate)/avStrmBaseClkV;

            if(avStrmCurPosA> avStrmCurPosV+pIsomGlob->v_period)
                VID_DEC_REQ= (int16)((avStrmCurPosA- avStrmCurPosV)/pIsomGlob->v_period);
            else if(avStrmCurPosA+pIsomGlob->v_period < avStrmCurPosV)
                VID_DEC_REQ= -(int16)((avStrmCurPosV- avStrmCurPosA)/pIsomGlob->v_period);

            pIsomGlob->current_time=(avStrmCurPosA/pIsomGlob->a_sample_rate)*1000;
            //diag_printf("sync a %d v %d",avStrmCurPosA,avStrmCurPosV);
        }

    }
    else//stop audio pos
    {
        VOC_AUD=0;
        LAST_SYNC_AUD_FRM_NUM= pIsomGlob->a_current_frame_num -pIsomGlob->a_sync_distance_frame_num;
    }

    {
        if(pIsomGlob->med_type!=MCI_TYPE_RM)
        {
            if(DTS_FLG ==0xFFFFFFFF)
            {
                if(pIsomGlob->med_type!=MCI_TYPE_AVSTRM)
                {
                    syncF=((long long)pIsomGlob->a_current_frame_num*(long long)SYNC_NUM_COEFFICIENT)>>(SYNC_COEFF_SCALE_BIT+V_FPS_SCALE_BIT);
                    VID_DEC_REQ=(int16)((int32)syncF-pIsomGlob->v_current_frame_num);
                    if(pIsomGlob->open_audio!=2)
                        pIsomGlob->current_time=(uint32)syncF*pIsomGlob->v_period;
                    else
                        pIsomGlob->current_time= ((long long)pIsomGlob->a_frame_sample*(long long)pIsomGlob->a_current_frame_num*1000)/(long long)pIsomGlob->a_sample_rate;

                }
            }
            else
            {
                pIsomGlob->current_time= ((long long)pIsomGlob->a_frame_sample*(long long)pIsomGlob->a_current_frame_num*1000+ (long long)(pIsomGlob->a_sample_rate>>1))/(long long)pIsomGlob->a_sample_rate;
                if(DTS_SampDelta)
                    VID_DEC_REQ= ((long long)pIsomGlob->current_time*(long long)pIsomGlob->v_timeScale - (long long)DTS_FLG*1000 )/((long long)DTS_SampDelta*1000);
            }
        }
#ifdef RM_SUPPORT
        else
        {
            syncF=((long long)pIsomGlob->a_current_frame_num*1000/(long long)pIsomGlob->a_sample_rate);
            VID_DEC_REQ=(int16)((syncF-(long long)rm_getLastFrameTimeStamp()+(long long)(pIsomGlob->v_period>>1))/(long long)pIsomGlob->v_period);
            pIsomGlob->current_time=(unsigned int)syncF;
        }
#endif


        if(pIsomGlob->a_current_frame_num-LAST_SYNC_AUD_FRM_NUM>= pIsomGlob->a_sync_distance_frame_num) //reuse a_sync_distance_frame_num as time bar update scale (500ms)
        {

            LAST_SYNC_AUD_FRM_NUM=pIsomGlob->a_current_frame_num;
            if(DISPLAY_INTERFACE!=0xff)
                DISPLAY_INTERFACE=1;
        }
    }

    if(AUD_FADE_OUT_FLAG==3)
    {
        //fade out  last frame
        audFadeOut((int16*) (pIsomGlob->a_PcmBuf+ pIsomGlob->a_frameBytes*(pIsomGlob->a_HalfPcmBufFrame-1)),
                   pIsomGlob->a_frame_sample*pIsomGlob->a_channels);
    }

    //if(getDataLen()>0)
    {
        if( pIsomGlob->dec_mode==1 && (pIsomGlob->open_audio!=2 || pIsomGlob->med_type==MCI_TYPE_AVSTRM))
        {
            if(aud_extra_pcmbuf_state[A_EXBF_RD_ID].done==1)
            {
                if(AUD_FADE_OUT_FLAG==0)
                {
                    if( is_SSHdl_valid(MpegPlayer.MpegInput.ss_handle)&& (MpegPlayer.MpegInput.audio_config_handle->codec_type ==1)) //BT_A2DP_sbc
                    {
                        //PutData in isr is bad.fixme later.
                        int i;
                        for(i=0; i<pIsomGlob->a_HalfPcmBufFrame; i++)
                        {
                            int size;
                            int *p;

                            p= (int*)((aud_extra_pcmbuf_state[A_EXBF_RD_ID].pBufPos)+ i*pIsomGlob->a_frameBytes);
                            size= *p;
                            //      diag_printf("put sbc p=%x sz=%d\n",p,size);
                            p++;
                            if(size>0 && MpegPlayer.MpegInput.ss_handle->PutData(MpegPlayer.MpegInput.ss_handle,(UINT8*)p,size))
                            {
                                diag_printf("sbc data maybe overflow.\n");
                                return;
                            }
                        }
                    }
                    else
                    {
                        memcpy(pIsomGlob->a_PcmBufHalfPos,(VOID*)((uint32)(aud_extra_pcmbuf_state[A_EXBF_RD_ID].pBufPos)|0x20000000), pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame);
                    }
                }

                if(pIsomGlob->med_type==MCI_TYPE_AVSTRM && aud_extra_pcmbuf_state[A_EXBF_RD_ID].timestamp!=0)
                {
                    avStrmCurPosA= aud_extra_pcmbuf_state[A_EXBF_RD_ID].timestamp -pIsomGlob->a_frame_sample*pIsomGlob->a_HalfPcmBufFrame;
                    aud_extra_pcmbuf_state[A_EXBF_RD_ID].timestamp=0;
                }
                aud_extra_pcmbuf_state[A_EXBF_RD_ID].done=0;
                A_EXBF_RD_ID+=1;
                if(A_EXBF_RD_ID>=EXTRA_PCM_BUF_NUM)A_EXBF_RD_ID=0;
                //pPcmBufPos=(uint32)(aud_extra_pcmbuf_state[A_EXBF_WT_ID].pBufPos);
                pcmUpdate=1;
            }
            else
            {
                //pPcmBufPos=(uint32)(aud_extra_pcmbuf_state[A_EXBF_WT_ID].pBufPos);
                if(AUD_DATA_REQ) VOC_AUD=1;
                pcmUpdate=0;
            }
        }
        else
        {
            if(pIsomGlob->a_type== A_TYPE_COOK)
            {

#ifdef RM_SUPPORT
                if(aud_extra_pcmbuf_state[A_EXBF_RD_ID].done==1)
                {
                    if(AUD_FADE_OUT_FLAG==0)
                        memcpy(pIsomGlob->a_PcmBufHalfPos,(VOID*)((uint32)(aud_extra_pcmbuf_state[A_EXBF_RD_ID].pBufPos)|0x20000000), pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame);
                    aud_extra_pcmbuf_state[A_EXBF_RD_ID].done=0;
                    A_EXBF_RD_ID+=1;
                    if(A_EXBF_RD_ID>=EXTRA_PCM_BUF_NUM)A_EXBF_RD_ID=0;
                    pPcmBufPos=(uint32)(aud_extra_pcmbuf_state[A_EXBF_WT_ID].pBufPos);
                    pcmUpdate=1;
                }
                else
                {
                    needRA=1;
                    pcmUpdate=0;
                }
#endif

            }
            else if(pIsomGlob->a_type!= A_TYPE_PCM)
            {
                if(aud_extra_pcmbuf_state[A_EXBF_RD_ID].done==1)
                {
                    if(AUD_FADE_OUT_FLAG==0)
                    {
                        memcpy(pIsomGlob->a_PcmBufHalfPos, (VOID*)((uint32)(aud_extra_pcmbuf_state[A_EXBF_RD_ID].pBufPos)|0x20000000), pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame);
                        if((my_input->bt_sco==TRUE) &&( pIsomGlob->a_type != A_TYPE_AMR))
                        {
                            g_VidBTDataAddress = (INT16*)pIsomGlob->a_PcmBufHalfPos;
                            if(pIsomGlob->a_channels==2)
                            {
                                int i;
                                INT16 *PCM_p=(INT16 *)g_VidBTDataAddress;
                                for(i=0; i<g_VidBtOutputPcmBufSize; i=i+2)
                                {
                                    PCM_p[i/2]=(PCM_p[i]>>1)+(PCM_p[i+1]>>1);
                                }
                            }

                            g_VidBtNout=ResampleOneFrame(g_VidBTDataAddress,g_VidBT8kPcmBuffer+g_VidBtcounterframe*g_VidBtNumOfOutSample);
                            g_VidBtcounterframe++;
                            if(g_VidBtcounterframe >= g_VidBtTotalFrame)
                                g_VidBtcounterframe = 0;
                            MCI_TRACE (TSTDOUT,0,"g_VidBtNout88888 = %d",g_VidBtNout);
                        }
                    }
#if 1
                    aud_extra_pcmbuf_state[A_EXBF_RD_ID].done=0;
                    A_EXBF_RD_ID+=1;
                    if(A_EXBF_RD_ID>=EXTRA_PCM_BUF_NUM)A_EXBF_RD_ID=0;

                    if(VOC_WORK==VOC_WORK_NON && VOC_SETTING==0 && getDataLen()>0)
                    {
                        VOC_WORK=VOC_WORK_AUD;

                        pPcmBufPos=(uint32)(aud_extra_pcmbuf_state[A_EXBF_WT_ID].pBufPos);

#if( CHIP_HAS_GOUDA == 1 )
                        if(pIsomGlob->a_type != A_TYPE_AMR)
                        {
#endif
                            vocDecAud.inStreamBufAddr=(INT32 *)getDataPos();
                            vocDecAud.outStreamBufAddr=(INT32 *)pPcmBufPos;
                            vocDecAud.audioItf=audioItf;

                            /*if(pIsomGlob->v_type ==V_TYPE_H264 || pIsomGlob->v_type ==V_TYPE_RV
                                ||pIsomGlob->v_type ==V_TYPE_MPEG4
                                )
                                vpp_AVDecSetMode(vocDecAud.mode, 1, 0);//set video code invalide
                                */

                            if(vpp_AudioJpegDecScheduleOneFrame(&vocDecAud)!=HAL_ERR_NO)//aac 1024??
                            {
                                VOC_WORK=VOC_WORK_NON;
                                diag_printf("[VID]voc call err!");
                            }
                            //diag_printf("pcm call");
#if( CHIP_HAS_GOUDA == 1 )
                        }
                        else
                        {
                            HAL_SPEECH_DEC_IN_T *pVocDecAmr=vpp_SpeechGetRxCodBuffer();

                            get_amr_mode( getDataPos(), &(pIsomGlob->a_amr_mode), &(pIsomGlob->a_codeframeBytes));

                            if(pIsomGlob->a_amr_mode != HAL_INVALID)
                            {
                                getData((U8*)pVocDecAmr->decInBuf, pIsomGlob->a_codeframeBytes, 1);
                                pVocDecAmr->codecMode= pIsomGlob->a_amr_mode;
                            }
                            else
                            {
                                //write silence data
                                moveDataPos(pIsomGlob->a_codeframeBytes);
                                memcpy(pVocDecAmr->decInBuf, silence_frame475, 13);
                                pVocDecAmr->codecMode= HAL_AMR475_DEC;
                            }

                            // Set all the preprocessing modules
                            vpp_SpeechSetEncDecPocessingParams(/*audioItf*/AUD_ITF_LOUD_SPEAKER, audio_cfg.spkLevel, 0);
                            if(vpp_SpeechScheduleOneFrame(VPP_SPEECH_WAKEUP_SW_DEC) != HAL_ERR_NO)
                            {
                                VOC_WORK=VOC_WORK_NON;
                                diag_printf("[VID]voc call err!");
                            }
                        }
#endif
                    }
#endif
                    pcmUpdate=1;
                }
                else
                {
                    if(VOC_WORK==VOC_WORK_NON && VOC_SETTING==0 && getDataLen()>0)
                    {
                        VOC_WORK=VOC_WORK_AUD;
                        VOC_AUD++;

                        //pPcmBufPos=(uint32)pIsomGlob->a_PcmBufHalfPos;
                        pPcmBufPos=(uint32)(aud_extra_pcmbuf_state[A_EXBF_WT_ID].pBufPos);

#if( CHIP_HAS_GOUDA == 1 )
                        if(pIsomGlob->a_type != A_TYPE_AMR)
                        {
#endif
                            vocDecAud.inStreamBufAddr=(INT32 *)getDataPos();
                            vocDecAud.outStreamBufAddr=(INT32 *)pPcmBufPos;
                            vocDecAud.audioItf=audioItf;

                            /*if(pIsomGlob->v_type ==V_TYPE_H264 || pIsomGlob->v_type ==V_TYPE_RV
                                ||pIsomGlob->v_type ==V_TYPE_MPEG4
                                )
                                vpp_AVDecSetMode(vocDecAud.mode, 1, 0);//set video code invalide
                                */

                            if(vpp_AudioJpegDecScheduleOneFrame(&vocDecAud)!=HAL_ERR_NO)//aac 1024??
                            {
                                VOC_WORK=VOC_WORK_NON;
                                diag_printf("[VID]voc call err!");
                            }
                            //diag_printf("pcm call");
#if( CHIP_HAS_GOUDA == 1 )
                        }
                        else
                        {
                            HAL_SPEECH_DEC_IN_T *pVocDecAmr=vpp_SpeechGetRxCodBuffer();

                            get_amr_mode( getDataPos(), &(pIsomGlob->a_amr_mode), &(pIsomGlob->a_codeframeBytes));

                            if(pIsomGlob->a_amr_mode != HAL_INVALID)
                            {
                                getData((U8*)pVocDecAmr->decInBuf, pIsomGlob->a_codeframeBytes, 1);
                                pVocDecAmr->codecMode= pIsomGlob->a_amr_mode;
                            }
                            else
                            {
                                //write silence data
                                moveDataPos(pIsomGlob->a_codeframeBytes);
                                memcpy(pVocDecAmr->decInBuf, silence_frame475, 13);
                                pVocDecAmr->codecMode= HAL_AMR475_DEC;
                            }

                            // Set all the preprocessing modules
                            vpp_SpeechSetEncDecPocessingParams(/*audioItf*/AUD_ITF_LOUD_SPEAKER, audio_cfg.spkLevel, 0);
                            if(vpp_SpeechScheduleOneFrame(VPP_SPEECH_WAKEUP_SW_DEC) != HAL_ERR_NO)
                            {
                                VOC_WORK=VOC_WORK_NON;
                                diag_printf("[VID]voc call err!");
                            }
                        }
#endif
                    }
                    else if(getDataLen()>0)
                    {
                        if(VOC_AUD!=0)
                            AUD_DEC_MIN=0;
                        //else
                        //  AUD_DEC_MIN=2;

                        VOC_AUD++;
                    }

                    pcmUpdate=0;
                }
            }
            else
            {
                pPcmBufPos=(uint32)pIsomGlob->a_PcmBufHalfPos;
                AUD_DEC_FRM_NUM=(uint16)pIsomGlob->a_HalfPcmBufFrame;
                pcmUpdate=1;

            }
        }

        if(AUD_FADE_OUT_FLAG>0)
        {
            memset((uint8*)pIsomGlob->a_PcmBufHalfPos, 0, pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame);
            if(AUD_FADE_OUT_FLAG>1)AUD_FADE_OUT_FLAG--;
            pcmUpdate=1;
        }

        if(SEND_EVENT_NUM==0 )
        {
            SEND_EVENT_NUM=1;
            mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_AUD_DEC_REQ);
            //diag_printf("1 pcm send");
        }
    }

    if(pcmUpdate==0)
    {
        //aud_StreamStop(audioItf);
        if(AUD_DATA_REQ==1 && pIsomGlob->dec_mode==0)AUD_DATA_REQ=2;
        memset(pIsomGlob->a_PcmBufHalfPos, 0, pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame);
        diag_printf("[VID]pcm isr, aud no data");
    }
}

static void vid_voc_isr(HAL_VOC_IRQ_STATUS_T * status )
{
    if(VOC_WORK==VOC_WORK_VID)
    {
	    //include h.264 h.263 mpeg4
	    VOC_VID -= 1;
	    //diag_printf("v isr VID %d",VOC_VID);
    }

    //check voc request for audio and video
    if(VOC_VID>0)
    {
	    //include h.264 h.263 mpeg4
	    //diag_printf("isr %d 2",VOC_WORK);
	    VOC_WORK=VOC_WORK_VID;
	    VOC_VID=0;
	    return;
    }

    //diag_printf("ISR %d end",VOC_WORK);
    VOC_WORK=VOC_WORK_NON;
}


//reset voc when loop error
void video_voc_reset(void)
{
    vpp_AudioJpegDecClose();
    sxr_Sleep(16);
    diag_printf("[VID]WARNING video_voc_reset happen! ");
    vpp_AudioJpegDecOpen(vid_voc_isr);
    switch(pIsomGlob->v_type)
    {

#ifdef RM_SUPPORT
        case V_TYPE_RV:
            vpp_AudioJpegSetReuseCode(1);//rmvb=1
            //vpp_AudioJpegDecSetCurMode(-1, -1);
            //vpp_AudioJpegDecSetInputMode(MMC_RM_DECODE);
            vpp_AVDecSetMode(MMC_RM_DECODE, 0, 0);
            break;
#endif
#ifdef H264_SUPPORT
        case V_TYPE_H264:
            vpp_AudioJpegSetReuseCode(2);//h264=2
            //vpp_AudioJpegDecSetCurMode(-1, -1);
            //vpp_AudioJpegDecSetInputMode(MMC_H264_DECODE);
            vpp_AVDecSetMode(MMC_H264_DECODE, 0, 0);
            break;
#endif
        case V_TYPE_MPEG4:
            vpp_AudioJpegSetReuseCode(3);//mpeg4=3
            vpp_AVDecSetMode(MMC_MPEG4_DECODE, 0, 0);
            break;
        case V_TYPE_H263:
            vpp_AudioJpegSetReuseCode(4);//h263=4
            vpp_AVDecSetMode(MMC_H263_DECODE, 0, 0);
            break;
        case V_TYPE_MJPEG:
            vpp_AudioJpegSetReuseCode(5);//jpeg=5
            vpp_AVDecSetMode(MMC_JPEG_DECODE, 0, 0);
            break;

        default:
            break;
    }
}

#ifdef H264_SUPPORT
static int video_dec_reset(void)
{
    video_voc_reset();
#ifdef H264_SUPPORT
    h264_decoder_close(pIsomGlob->pH264Hdl);
    pIsomGlob->pH264Hdl=h264_decoder_init();

    if(!pIsomGlob->pH264Hdl)
    {
        diag_printf("[VID] video_dec_reset err!  \n");
        return -1;
    }
#endif
    diag_printf("[VID] video_dec_reset happen!  \n");
    return 0;
}
#endif
/*
only do 90 clockwise,270 clockwise by lcd driver and make mirror.
4x4 each time.
w:image width to rotate. multiple of 4
h:image height to rotate. muliple of 4
stride:image stride. multiple of 4
pIn:input data address
pIn:output data address
half_w:flag to do pixel skip one by one with width direction.
sheen
*/
static void vid_rotate(int w,int h,int stride,char *pIn,char *pOut,int half_w)
{
	unsigned int *pSrc,*pDst;
	char *pSrcStart,*pDstStart;
	unsigned int d0,d1,d2,d3;
	int x,y;
	int src_offset=stride/4;
	int dst_offset=h/4;

	pDstStart=pOut+h-4; 
	if(half_w==0){
		for(y=0;y<h;y+=4){
			pSrcStart= pIn+ y*stride; 
			pDst= (unsigned int*)(pDstStart-y);
			for(x=0;x<w;x+=4){
				pSrc= (unsigned int*)(pSrcStart +x);
				d0=*pSrc;
				pSrc+=src_offset;
				d1=*pSrc;
				pSrc+=src_offset;
				d2=*pSrc;
				pSrc+=src_offset;
				d3=*pSrc;

				*pDst=(d3&0xff)|((d2&0xff)<<8)|((d1&0xff)<<16)|((d0&0xff)<<24);
				pDst+=dst_offset;
				*pDst=((d3&0xff00)>>8)|((d2&0xff00))|((d1&0xff00)<<8)|((d0&0xff00)<<16);
				pDst+=dst_offset;
				*pDst=((d3&0xff0000)>>16)|((d2&0xff0000)>>8)|((d1&0xff0000))|((d0&0xff0000)<<8);
				pDst+=dst_offset;
				*pDst=((d3&0xff000000)>>24)|((d2&0xff000000)>>16)|((d1&0xff000000)>>8)|((d0&0xff000000));
				pDst+=dst_offset;
			}
		}
	}else{
		for(y=0;y<h;y+=4){
			pSrcStart= pIn+ y*stride; 
			pDst= (unsigned int*)(pDstStart-y);
			for(x=0;x<w;x+=4){
				pSrc= (unsigned int*)(pSrcStart +x);
				d0=*pSrc;
				pSrc+=src_offset;
				d1=*pSrc;
				pSrc+=src_offset;
				d2=*pSrc;
				pSrc+=src_offset;
				d3=*pSrc;

				*pDst=(d3&0xff)|((d2&0xff)<<8)|((d1&0xff)<<16)|((d0&0xff)<<24);
				pDst+=dst_offset;
				*pDst=((d3&0xff0000)>>16)|((d2&0xff0000)>>8)|((d1&0xff0000))|((d0&0xff0000)<<8);
				pDst+=dst_offset;
			}
		}
	}	
}
/*
interface_mod:DISPLAY_INTERFACE
update_gouda:setup gouda or not.
sheen
*/
static void vid_setup_gouda_lcd(int interface_mod,int update_gouda)
{
	uint32 tmp= pIsomGlob->v_cutW_origin * pIsomGlob->v_cutH_origin;
	int32 half_w=(tmp>VDO_RSLUT_QVGA)?1:0;//make source half size but keep display size.
	uint8 *pRotY=pIsomGlob->v_ExtraRotateBuf;
	uint8 *pRotU=pRotY+ (half_w?(tmp>>1):tmp);
	uint8 *pRotV=pRotU+ (half_w?(tmp>>3):(tmp>>2));

	if(update_gouda){
		//soft rotate
		if((pIsomGlob->v_rotate!=Rotate_with_0) && (pIsomGlob->v_rotate_way==1)){//soft rotate
			//only do 90clockwise,270 clockwise by lcd driver make mirror
			int tk;
			tk=hal_TimGetUpTime();
			if(pIsomGlob->v_LastSkipRotateTk){
				//not skip at the first time.
				//rotate and display about <= 15fps (60*16)
				if(tk-pIsomGlob->v_LastSkipRotateTk <960){
					diag_printf("[VID]skip rotate.");
					return;
				}
			}
			pIsomGlob->v_LastSkipRotateTk= tk;

			vid_rotate(pIsomGlob->v_cutW_origin,pIsomGlob->v_cutH_origin,pIsomGlob->v_YuvInfo[3],(char*)(pIsomGlob->v_YuvInfo[0]+(uint32)pIsomGlob->v_cutY_origin*pIsomGlob->v_YuvInfo[3]+pIsomGlob->v_cutX_origin),pRotY,half_w);
			vid_rotate(pIsomGlob->v_cutW_origin>>1,pIsomGlob->v_cutH_origin>>1,pIsomGlob->v_YuvInfo[3]>>1,(char*)(pIsomGlob->v_YuvInfo[1]+(uint32)(pIsomGlob->v_cutY_origin>>1)*(pIsomGlob->v_YuvInfo[3]>>1)+(pIsomGlob->v_cutX_origin>>1)),pRotU,half_w);
			vid_rotate(pIsomGlob->v_cutW_origin>>1,pIsomGlob->v_cutH_origin>>1,pIsomGlob->v_YuvInfo[3]>>1,(char*)(pIsomGlob->v_YuvInfo[2]+(uint32)(pIsomGlob->v_cutY_origin>>1)*(pIsomGlob->v_YuvInfo[3]>>1)+(pIsomGlob->v_cutX_origin>>1)),pRotV,half_w);

                //tk=hal_TimGetUpTime();
                //diag_printf("rot t=%d",  (tk-pIsomGlob->v_LastSkipRotateTk)*1000/16384);
		}
		//setup gouda
		tmp=0;
		while(hal_GoudaIsActive())
		{
			tmp++;
			if(tmp>0x2000000)
			{
				diag_printf("[VID]gouda err!");
				break;
			}
		}
		hal_GoudaVidLayerClose();
		hal_GoudaSetBgColor(0);

		if((pIsomGlob->v_rotate==Rotate_with_0) ||(pIsomGlob->v_rotate_way==2)){//rotate by lcd or not rotate
			Vid_Video.addrY = (UINT32*)(pIsomGlob->v_YuvInfo[0]+(uint32)pIsomGlob->v_cutY_origin*pIsomGlob->v_YuvInfo[3]+pIsomGlob->v_cutX_origin);
			Vid_Video.addrU = (UINT32*)(pIsomGlob->v_YuvInfo[1]+(uint32)(pIsomGlob->v_cutY_origin>>1)*(pIsomGlob->v_YuvInfo[3]>>1)+(pIsomGlob->v_cutX_origin>>1));
			Vid_Video.addrV = (UINT32*)(pIsomGlob->v_YuvInfo[2]+(uint32)(pIsomGlob->v_cutY_origin>>1)*(pIsomGlob->v_YuvInfo[3]>>1)+(pIsomGlob->v_cutX_origin>>1));
			Vid_Video.stride = (UINT16)pIsomGlob->v_YuvInfo[3];
			Vid_Video.height = (UINT16)pIsomGlob->v_cutH_origin;
			Vid_Video.width = (UINT16)pIsomGlob->v_cutW_origin;
		}else if(pIsomGlob->v_rotate_way==1){//rotate by soft
			Vid_Video.addrY = (UINT32*)pRotY; 
			Vid_Video.addrU = (UINT32*)pRotU; 
			Vid_Video.addrV = (UINT32*)pRotV; 
			Vid_Video.stride = (UINT16)pIsomGlob->v_cutH_origin;
			Vid_Video.height = (UINT16)(half_w?(pIsomGlob->v_cutW_origin>>1):pIsomGlob->v_cutW_origin);
			Vid_Video.width = (UINT16)pIsomGlob->v_cutH_origin;
		}else{//roate by gouda
			//make image bottom right as the start address
			Vid_Video.addrY = (UINT32*)(pIsomGlob->v_YuvInfo[0]+(uint32)(pIsomGlob->v_cutY_origin+pIsomGlob->v_cutH_origin -1)*pIsomGlob->v_YuvInfo[3]+pIsomGlob->v_cutX_origin);
			Vid_Video.addrU = (UINT32*)(pIsomGlob->v_YuvInfo[1]+(uint32)(((pIsomGlob->v_cutY_origin+pIsomGlob->v_cutH_origin)>>1) -1)*(pIsomGlob->v_YuvInfo[3]>>1)+(pIsomGlob->v_cutX_origin>>1));
			Vid_Video.addrV = (UINT32*)(pIsomGlob->v_YuvInfo[2]+(uint32)(((pIsomGlob->v_cutY_origin+pIsomGlob->v_cutH_origin)>>1) -1)*(pIsomGlob->v_YuvInfo[3]>>1)+(pIsomGlob->v_cutX_origin>>1));
			//stride is not change,it's data stride
			Vid_Video.stride = (UINT16)pIsomGlob->v_YuvInfo[3];
			//height is after rotate
			Vid_Video.height= (UINT16)pIsomGlob->v_cutW_origin;
			//width is after rotate
			Vid_Video.width = (UINT16)pIsomGlob->v_cutH_origin;

		}

		hal_GoudaSetBlock(0);
		if(pIsomGlob->v_cutW>1 && pIsomGlob->v_cutH>1)
			hal_GoudaVidLayerOpen(&Vid_Video);
		else
			diag_printf("[VID]WARNING!gouda brPX brPY");

	}

	//setup lcd
	if((interface_mod==0) || (interface_mod==0xff)){//only display roi image area

		if((pIsomGlob->v_rotate==Rotate_with_0) || (pIsomGlob->v_rotate_way==2)){//rotate by lcd or not rotate
			lcdDraw.roi.x =pIsomGlob->v_lcd_start_x;
			lcdDraw.roi.y =pIsomGlob->v_lcd_start_y;
			lcdDraw.roi.width =pIsomGlob->v_cutW;
			lcdDraw.roi.height =pIsomGlob->v_cutH;
		}else if(pIsomGlob->v_rotate_way==1){//rotate by soft
			lcdDraw.roi.x =pIsomGlob->v_lcd_size_y -(pIsomGlob->v_lcd_start_y+pIsomGlob->v_cutH);
			lcdDraw.roi.y =pIsomGlob->v_lcd_start_x;
			lcdDraw.roi.width =pIsomGlob->v_cutH;
			lcdDraw.roi.height =pIsomGlob->v_cutW;
		}else{//rotate by gouda
			//same as soft rotate
			lcdDraw.roi.x =pIsomGlob->v_lcd_size_y -(pIsomGlob->v_lcd_start_y+pIsomGlob->v_cutH);
			lcdDraw.roi.y =pIsomGlob->v_lcd_start_x;
			lcdDraw.roi.width =pIsomGlob->v_cutH;
			lcdDraw.roi.height =pIsomGlob->v_cutW;
		}
	}else{
		//mode 1,2 only display time bar or with image
		lcdDraw.roi.x =0;
		lcdDraw.roi.y =0;
		if((pIsomGlob->v_rotate==Rotate_with_0) || (pIsomGlob->v_rotate_way==2)){//rotate by lcd or not rotate
			lcdDraw.roi.width =pIsomGlob->v_lcd_size_x;
			lcdDraw.roi.height =pIsomGlob->v_lcd_size_y;
		}else if(pIsomGlob->v_rotate_way==1){//rotate by soft
			lcdDraw.roi.width =pIsomGlob->v_lcd_size_y;
			lcdDraw.roi.height =pIsomGlob->v_lcd_size_x;
		}else{//rotate by gouda
			//same as soft rotate
			lcdDraw.roi.width =pIsomGlob->v_lcd_size_y;
			lcdDraw.roi.height =pIsomGlob->v_lcd_size_x;
		}
	}
/*
    diag_printf("[VID]interface =%d update gouda=%d\n",
		    interface_mod,update_gouda);
    diag_printf("[VID]gouda src y=%x u=%x v=%x\n",
                Vid_Video.addrY,Vid_Video.addrU,Vid_Video.addrV);
    diag_printf("[VID]gouda src w=%d h=%d stride=%d\n",
                Vid_Video.width,Vid_Video.height,Vid_Video.stride);
    diag_printf("[VID]lcdDraw roi x=%d y=%d w=%d h=%d\n",
                lcdDraw.roi.x,lcdDraw.roi.y,lcdDraw.roi.width,lcdDraw.roi.height);
*/

        if(lcdDraw.roi.width && lcdDraw.roi.height)
        {
		LCDD_ERR_T lcdRes=0;
            lcdRes=lcdd_Blit16(&lcdDraw, lcdDraw.roi.x, lcdDraw.roi.y);
            tmp=160;
            while (lcdRes!=LCDD_ERR_NO  && lcdRes != LCDD_ERR_INVALID_PARAMETER){
                if(tmp==0)
                {
		    diag_printf("[VID] lcd blit fail=%d",lcdRes);
                    break;
                }
                sxr_Sleep(1);

                lcdRes=lcdd_Blit16(&lcdDraw, lcdDraw.roi.x, lcdDraw.roi.y);
                tmp--;
            }
        }
}

static void vid_display(void)
{
	int update_gouda=1;
    if(pIsomGlob->bSkipFrame == TRUE )
    {
        pIsomGlob->bSkipFrame = FALSE;
	update_gouda= 0;
	if(DISPLAY_INTERFACE !=1){
		diag_printf("[VID]skip display");
		return;
	}
    }

    DISPLAY_STATE=1;

    if(DISPLAY_INTERFACE==0 || DISPLAY_INTERFACE==0xff)
    {
	    vid_setup_gouda_lcd(DISPLAY_INTERFACE,update_gouda);
    }
    else if(DISPLAY_INTERFACE==1)
    {
        DISPLAY_INTERFACE=0;

        /*update the panel buffer*/
        if(pIsomGlob->vid_draw_panel_callback)
            pIsomGlob->vid_draw_panel_callback(pIsomGlob->current_time);

	vid_setup_gouda_lcd(1,update_gouda);
    }
    else//DISPLAY_INTERFACE==2 display time bar with image area.
    {
        DISPLAY_INTERFACE=0;

	vid_setup_gouda_lcd(2,update_gouda);
    }

    DISPLAY_STATE=0;
}

/*
read next video frame to extra second sample buffer when voc decode current video frame.
only used in VOCVID and lowres==1(big frame size)
*/
static void vid_frame_read(void)
{
    int32 res;
//int32 tt1,tt2;//test
//tt1=hal_TimGetUpTime();

    vid_SampleBuf_BytesRemain-= vid_SampleBuf_BytesUsed;
    if ((pIsomGlob->v_type != V_TYPE_MPEG4)&&
            (pIsomGlob->v_type !=V_TYPE_MJPEG)&&
            (pIsomGlob->v_type !=V_TYPE_RV)&&
            (vid_SampleBuf_BytesUsed>0) &&
            (vid_SampleBuf_BytesRemain > 0) &&
            vid_SampleBuf_BytesRemain< pIsomGlob->v_SampleBufLen)
        memcpy(pVid_SampleBuf[vid_SampleBuf_id] , pVid_SampleBuf[vid_SampleBuf_id]+vid_SampleBuf_BytesUsed, vid_SampleBuf_BytesRemain);

    vid_SampleBuf_BytesUsed=0;
    if(vid_SampleBuf_BytesRemain<0)vid_SampleBuf_BytesRemain=0;

    if(pIsomGlob->v_current_sample_num+1<pIsomGlob->v_total_sample_num)
    {
        if(pIsomGlob->med_type==MCI_TYPE_MP4 || pIsomGlob->med_type==MCI_TYPE_3GP)
        {
            uint32 readLen;
            uint32 gotbytes;
            uint32 gotcount;

            if((res = stbl_GetSampleSize(pIsomGlob->v_info , pIsomGlob->v_current_sample_num+1, &readLen))!=GF_OK)
            {
                diag_printf("[VID] video stbl_GetSampleSize err res=%d \n",res);
                //AV_DEC_STATE=0;
                //return ISOM_ERR_VIDEO_READ_FAIL;
                vid_SampleBuf_BytesRemain = -1;
                return;
            }

            if(vid_SampleBuf_BytesRemain+readLen<pIsomGlob->v_SampleBufLen)
            {
                if(pIsomGlob->v_type == V_TYPE_MPEG4)
                {
                    if(pIsomGlob->v_LastFrmStates==1 && pIsomGlob->v_decSpeInfo &&
                            (vid_SampleBuf_BytesRemain+readLen+ pIsomGlob->v_decSpeInfo->dataLength)<pIsomGlob->v_SampleBufLen)
                    {

                        memcpy(pVid_SampleBuf[vid_SampleBuf_id], pIsomGlob->v_decSpeInfo->data, pIsomGlob->v_decSpeInfo->dataLength);
                        vid_SampleBuf_BytesRemain =pIsomGlob->v_decSpeInfo->dataLength;

                    }
                    else if(pIsomGlob->v_lowres==0)
                    {
                        if(readLen> pIsomGlob->v_width*pIsomGlob->v_height)
                            readLen= pIsomGlob->v_width*pIsomGlob->v_height;

                    }
                }

                res=gf_isom_get_multSample(pIsomGlob->v_info,
                                           pIsomGlob->v_current_sample_num+1,
                                           pVid_SampleBuf[vid_SampleBuf_id]+ vid_SampleBuf_BytesRemain,
                                           readLen,    &gotbytes, &gotcount);

                if(res || (gotbytes<=0))
                {
                    diag_printf("[VID] video gf_isom_get_multSample err res=%d \n",res);
                    //AV_DEC_STATE=0;
                    vid_SampleBuf_BytesRemain= -1;
                    //return ISOM_ERR_VIDEO_READ_FAIL;
                    return;
                }

                //replace the 4bytes frame size with start code for h.264 only.
                if(pIsomGlob->v_type==V_TYPE_H264)
                {
                    uint8 *p;
                    uint32 chunkPos,chunkSize;

                    chunkPos=0;
                    //may be multi chunk inside
                    do
                    {
                        p=( pVid_SampleBuf[vid_SampleBuf_id]+ vid_SampleBuf_BytesRemain+chunkPos);
                        chunkSize=0;
                        //get chunk size
                        chunkSize|=(*p)<<24;
                        chunkSize|=(*(p+1))<<16;
                        chunkSize|=(*(p+2))<<8;
                        chunkSize|=*(p+3);

                        //*((uint32*)p)=H264_START_CODE;
                        p[0]=0;
                        p[1]=0;
                        p[2]=0;
                        p[3]=1;//H264_START_CODE


                        chunkPos+=chunkSize+4;
                    }
                    while (chunkPos<gotbytes-4);

                }

                vid_SampleBuf_BytesRemain+=gotbytes;
                pIsomGlob->v_current_sample_num+=gotcount;
            }
            else
            {
                diag_printf("[VID]too big readLen=%d vid_SampleBuf_BytesRemain=%d \n",readLen, vid_SampleBuf_BytesRemain);
                //AV_DEC_STATE=0;
                vid_SampleBuf_BytesRemain = -1;
                //return ISOM_ERR_MEM_INSUFFICIENT;
                return;
            }
        }
        else if(pIsomGlob->med_type==MCI_TYPE_RM)
        {
            //to do ...
            vid_SampleBuf_BytesRemain=1;

        }
        else if(pIsomGlob->med_type==MCI_TYPE_AVSTRM)
        {
            avinfostr *avinfohdl= (avinfostr*)pIsomGlob->filehdl;
            //avframeinfohdstr frmhd;
            //int readLen=sizeof(avframeinfohdstr);
            /*
               if(avinfohdl->avstream_srv_callback)
               {
               avinfohdl->avstream_srv_callback(1);
               }*/

            avstream_clt_checkagent(avinfohdl->avstreamhdl_v);

            if(avStrmFrmHdV.samplesize==0)
            {
                avStrmCurPosV= (unsigned int)((unsigned long long)(avStrmFrmHdV.timestamp- avStrmTimStpStartV)*(unsigned long long)pIsomGlob->a_sample_rate/(unsigned long long)avStrmBaseClkV);
                //res=avstream_clt_getData(avinfohdl->avstreamhdl_v,(unsigned char*)&avStrmFrmHdV,readLen,1);
                res=avstream_clt_getheader(avinfohdl->avstreamhdl_v,&avStrmFrmHdV);
                //diag_printf("v ts %d sz %d",avStrmFrmHdV.timestamp, avStrmFrmHdV.samplesize);
            }
            else
            {
                //read head last time, but not read the frame samples.
                res=1;
            }

            if(res==1)
            {
                res=avstream_clt_getDataLen(avinfohdl->avstreamhdl_v);
                if(res >= avStrmFrmHdV.samplesize)
                {
                    if(vid_SampleBuf_BytesRemain+avStrmFrmHdV.samplesize<= pIsomGlob->v_SampleBufLen)
                    {

                        res=avstream_clt_getData(avinfohdl->avstreamhdl_v,pVid_SampleBuf[vid_SampleBuf_id] ,avStrmFrmHdV.samplesize,1);
                        if(res!=1)
                        {
                            diag_printf("[VID]avstream frame read fail res=%d \n",res);
                            //return ISOM_ERR_VIDEO_READ_FAIL;
                            vid_SampleBuf_BytesRemain= -1;
                            return;
                        }
                    }
                    else
                    {
                        avstream_clt_moveDataPos(avinfohdl->avstreamhdl_v, avStrmFrmHdV.samplesize);
                        diag_printf("[VID]avstream frame %d too big, skip!\n",avStrmFrmHdV.samplesize);
                    }
                    pIsomGlob->v_current_sample_num+=1;
                    vid_SampleBuf_BytesRemain+=avStrmFrmHdV.samplesize;
                    avStrmFrmHdV.samplesize=0;
                }
                else
                {
                    diag_printf("[VID]avstream frame data not enough, frm size=%d remain=%d buf size=%d\n",avStrmFrmHdV.samplesize,res,pIsomGlob->v_SampleBufLen);
                    //return ISOM_ERR_VIDEO_READ_FAIL;
                }
            }
            else
            {
                diag_printf("[VID]avstream fream header read fail res=%d \n",res);
                //return ISOM_ERR_VIDEO_READ_FAIL;
            }

            if((avinfohdl->avstreamhdl_v->ringbuflen -avstream_clt_getDataLen(avinfohdl->avstreamhdl_v)) > 10244
                    /*&& avinfohdl->avstreamhdl_v->avstreamhdl_agent->bufupdate==0*/)
            {
                avstream_clt_sendagent(avinfohdl->avstreamhdl_v);
                if(avinfohdl->avstream_srv_callback )
                {
                    avinfohdl->avstream_srv_callback(1);
                }
            }

        }
        else
        {
            int32 key_frame;
avi_video:

            res=avi_set_video_position(pIsomGlob->avi_p, pIsomGlob->v_current_sample_num);
            if(res!=0)
            {
                diag_printf("[VID] avi_set_video_position err!");
                //AV_DEC_STATE=0;
                //return ISOM_ERR_VIDEO_READ_FAIL;
                vid_SampleBuf_BytesRemain= -1;
                return;
            }

	    res=avi_frame_size(pIsomGlob->avi_p, pIsomGlob->v_current_sample_num);
            if(pIsomGlob->v_type == V_TYPE_MJPEG)
            {
		if(res<=608){
			//608 only jpg header size
			diag_printf("[VID] invalid MJPG size=%d",res);
			res=0;
		}
            }
	    if(res>0){
		    res=avi_read_frame(pIsomGlob->avi_p, pVid_SampleBuf[vid_SampleBuf_id]+vid_SampleBuf_BytesRemain,
                               pIsomGlob->v_SampleBufLen-vid_SampleBuf_BytesRemain,  &key_frame);
	    }else{
		res=0;
	    }
            if( res < 0 )
            {
                diag_printf( "[VID] avi video frame read err res=%d \n", res );
                //AV_DEC_STATE=0;
                //return ISOM_ERR_VIDEO_READ_FAIL;
                if (pIsomGlob->avi_p->video_frames <= pIsomGlob->v_current_sample_num + 1)
                {
                    vid_SampleBuf_BytesRemain= -1;
                    return;
                }
                else
                {
                    pIsomGlob->v_current_sample_num+=1;
                    goto avi_video;
                }
            }
            vid_SampleBuf_BytesRemain+=res;
            pIsomGlob->v_current_sample_num+=1;
        }
    }

//    tt2=hal_TimGetUpTime();
//    diag_printf("rd t=%d",  (tt2-tt1)*1000/16384);
    return;
}

static int32 vid_av_dec_frame(void)
{
    int32 res;

    if(play_state!=VID_PLAY_STATE_PLAYING)
    {
        if(play_state==VID_PLAY_STATE_PAUSE)
        {
            pIsomGlob->a_Pause_DecReq=1;
            diag_printf("[VID] vid_av_dec_frame pause state! \n");
            COS_KillTimerEX(GetMCITaskHandle(MBOX_ID_MMC), MMC_VID_PLY_TIMER_ID);
            return  ISOM_ERR_OK;
        }
        else
        {
            diag_printf("[VID] vid_av_dec_frame play_state= %d not match\n",play_state);
            COS_KillTimerEX(GetMCITaskHandle(MBOX_ID_MMC), MMC_VID_PLY_TIMER_ID);
            return  ISOM_ERR_PLAY_STATE_FAIL;
        }
    }

    /////////////////////////////////audio read//////////////////////////////

    vocWorkState.aud_nframe_dec=(void*)aud_nframe_dec;

    ///////////////////////////////////////video decode//////////////////////////////////////////////////////////
    //video_dec:
#if 1
    tick1=hal_TimGetUpTime();


    if(VID_DEC_REQ>0 ||pIsomGlob->med_type==MCI_TYPE_RM )
    {
        //int32 tk1,tk2;
        VID_DEC_REQ--;

        if(DTS_FLG !=0xFFFFFFFF)
        {
            DTS_FLG +=DTS_SampDelta;
            if( pIsomGlob->v_current_sample_num > DTS_NextSampNode)
            {
                uint32  dts_sampCount;
                uint32 lastSanpNum=( pIsomGlob->v_current_sample_num >0)? pIsomGlob->v_current_sample_num : 1;

                if((res=stbl_GetSampleDTS(pIsomGlob->v_info->sampleTable->TimeToSample, lastSanpNum,
                                          &DTS_FLG, &dts_sampCount, &DTS_SampDelta))==0)
                {
                    uint32 t;
                    //DTS_FLG = ((unsigned long long )DTS_FLG*1000+ (unsigned long long )(pIsomGlob->v_timeScale>>1))/pIsomGlob->v_timeScale;
                    DTS_NextSampNode = pIsomGlob->v_info->sampleTable->TimeToSample->r_FirstSampleInEntry + dts_sampCount -1;
                    //DTS_SampDelta = ((unsigned long long )dts_sampDelta*1000 + (pIsomGlob->v_timeScale>>1))/(unsigned long long )pIsomGlob->v_timeScale;
                    t= ((unsigned long long )DTS_SampDelta *16384+ (unsigned long long )(pIsomGlob->v_timeScale>>1))/(unsigned long long )pIsomGlob->v_timeScale;
                    if(t>0 && (t != cur_timer))
                    {
                        do
                        {
                            res=COS_KillTimerEX(GetMCITaskHandle(MBOX_ID_MMC), MMC_VID_PLY_TIMER_ID);
                        }
                        while(res!=TRUE);

                        res=COS_SetTimerEX(GetMCITaskHandle(MBOX_ID_MMC), MMC_VID_PLY_TIMER_ID, COS_TIMER_MODE_PERIODIC, t);
                        cur_timer=t;
                        diag_printf("[VID]change DTS_FLG=%d count=%d delta=%d timer %d", DTS_FLG, dts_sampCount, DTS_SampDelta,t);
                    }
                }
            }
        }

        if(pIsomGlob->med_type!=MCI_TYPE_AVSTRM)
            pIsomGlob->v_current_frame_num+=1;
        else
        {
            //unsigned long long temp;
            //temp=(unsigned long long)(((unsigned long long)avStrmFrmHdV.timestamp*(unsigned long long)pIsomGlob->v_fps)>>V_FPS_SCALE_BIT)/1000;
            pIsomGlob->v_current_frame_num= (int32)((avStrmFrmHdV.timestamp- avStrmTimStpStartV)/avStrmFrmTmScaleV);
        }

        //update play time when no audio.

        if(!pIsomGlob->open_audio)
        {
            uint32 timePos;
            uint32 timeOffset;

            timePos=hal_TimGetUpTime();
            timeOffset=((timePos-TIME_LINE_LAST)*1000)>>14;
            if(timeOffset>=SYNC_TIME_OFFSET)
            {
                unsigned long long temp;
                if(pIsomGlob->med_type!=MCI_TYPE_RM)
                {
                    pIsomGlob->current_time+=timeOffset;
                    // >>14 == /16384
                    temp=(unsigned long long)(((unsigned long long)pIsomGlob->current_time*(unsigned long long)pIsomGlob->v_fps)>>V_FPS_SCALE_BIT)/1000;
                    VID_DEC_REQ=(int16)(temp-pIsomGlob->v_current_frame_num);
                }

#ifdef RM_SUPPORT
                else
                {
                    pIsomGlob->current_time=rm_getLastFrameTimeStamp();
                    TIME_LINE_CUR+=timeOffset;
                    temp=((unsigned long long)TIME_LINE_CUR-(unsigned long long)pIsomGlob->current_time)/(unsigned long long)pIsomGlob->v_period;
                    VID_DEC_REQ=(int16)temp;
                }
#endif


                TIME_LINE_LAST=timePos;
                if(DISPLAY_INTERFACE!=0xff)
                    DISPLAY_INTERFACE=1;
            }
        }

#ifdef RM_SUPPORT
        if(pIsomGlob->med_type!=MCI_TYPE_RM)
        {
#endif

            if((pIsomGlob->v_current_frame_num+1)>=pIsomGlob->v_total_frame_num ||pIsomGlob->current_time >=  pIsomGlob->total_time)
            {
                COS_KillTimerEX(GetMCITaskHandle(MBOX_ID_MMC), MMC_VID_PLY_TIMER_ID);
                VID_DEC_REQ=0;

                mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_STOP_REQ);
                diag_printf("[VID] video frame over v_current_frame_num=%d  \n",pIsomGlob->v_current_frame_num);
                //AV_DEC_STATE=0;
                return ISOM_ERR_OK;
            }

#ifdef RM_SUPPORT
        }
        else
        {
            if(rm_getLastFrameTimeStamp() >= pIsomGlob->total_time-500)
            {
                COS_KillTimerEX(GetMCITaskHandle(MBOX_ID_MMC), MMC_VID_PLY_TIMER_ID);
                VID_DEC_REQ=0;

                mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_STOP_REQ);
                diag_printf("[VID] video frame over cur time=%d \n",rm_getLastFrameTimeStamp() );
                //AV_DEC_STATE=0;
                return ISOM_ERR_OK;
            }
        }
#endif

        //need more decode frame?
        if(VID_DEC_REQ>0)
        {
            if(pIsomGlob->med_type==MCI_TYPE_MP4 || pIsomGlob->med_type==MCI_TYPE_3GP ||
                    (pIsomGlob->med_type==MCI_TYPE_MJPG && pIsomGlob->v_type!=V_TYPE_MJPEG))
            {
#ifdef USE_KEY_SYNC
                if(KEY_SAMP_NUM==0 && VID_DEC_REQ>(pIsomGlob->v_fps>>V_FPS_SCALE_BIT))
                {
                    int16 frame_num_need=VID_DEC_REQ;//get VID_DEC_REQ avoid change.
                    //get next key frame for av sync
                    if(pIsomGlob->med_type==MCI_TYPE_MP4 || pIsomGlob->med_type==MCI_TYPE_3GP)
                        KEY_SAMP_NUM=gf_isom_get_nextKey(pIsomGlob->v_trak, pIsomGlob->v_current_frame_num+frame_num_need);
                    else if(pIsomGlob->med_type==MCI_TYPE_MJPG)
                        KEY_SAMP_NUM=avi_get_video_keyframe_num(pIsomGlob->avi_p, pIsomGlob->v_current_frame_num+frame_num_need);
                    KEY_SAMP_TIME_WAIT=(KEY_SAMP_NUM-(pIsomGlob->v_current_frame_num+frame_num_need)-2)*pIsomGlob->v_timerPeriod;
                    KEY_SAMP_TIME_START=hal_TimGetUpTime();

                    if(KEY_SAMP_TIME_WAIT>=0)
                    {
                        diag_printf("[VID]KEY_SAMP_NUM=%d v_current_sample_num=%d Ready!", KEY_SAMP_NUM, pIsomGlob->v_current_sample_num);
                    }
                    else
                    {
                        KEY_SAMP_NUM=0;
                        KEY_SAMP_TIME_WAIT=0;
                        KEY_SAMP_TIME_START=0;
                    }
                }

                if(KEY_SAMP_NUM>0 && (hal_TimGetUpTime()-KEY_SAMP_TIME_START>=KEY_SAMP_TIME_WAIT))
                {
                    if(KEY_SAMP_NUM>pIsomGlob->v_current_sample_num)
                    {
                        diag_printf("[VID]KEY_SAMP_NUM=%d v_current_sample_num=%d Go!", KEY_SAMP_NUM, pIsomGlob->v_current_sample_num);
                        pIsomGlob->v_current_sample_num=KEY_SAMP_NUM;
                        pIsomGlob->v_current_frame_num=KEY_SAMP_NUM-2;
                        VID_DEC_REQ=0;
                        if(DTS_FLG !=0xFFFFFFFF)
                        {
                            uint32  dts_sampCount;
                            uint32 lastSanpNum=( pIsomGlob->v_current_sample_num >0)? pIsomGlob->v_current_sample_num : 1;

                            if((res=stbl_GetSampleDTS(pIsomGlob->v_info->sampleTable->TimeToSample, lastSanpNum,
                                                      &DTS_FLG, &dts_sampCount, &DTS_SampDelta))==0)
                            {
                                //uint32 t;
                                //DTS_FLG = ((unsigned long long )DTS_FLG*1000+ (unsigned long long )(pIsomGlob->v_timeScale>>1))/pIsomGlob->v_timeScale;
                                DTS_NextSampNode = pIsomGlob->v_info->sampleTable->TimeToSample->r_FirstSampleInEntry + dts_sampCount -1;
                                //DTS_SampDelta = ((unsigned long long )dts_sampDelta*1000 + (pIsomGlob->v_timeScale>>1))/(unsigned long long )pIsomGlob->v_timeScale;
                                diag_printf("update DTS_FLG=%d count=%d delta=%d", DTS_FLG, dts_sampCount, DTS_SampDelta);
                            }

                        }
                    }
                    KEY_SAMP_NUM=0;
                    KEY_SAMP_TIME_WAIT=0;
                    KEY_SAMP_TIME_START=0;
                }
#endif
                if(SEND_EVENT_NUM==0 && VID_DEC_REQ>0)
                {
                    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_IMG_DEC_REQ);
                    SEND_EVENT_NUM=1;
                }
                diag_printf("[VID]VID_DEC_REQ=%d", VID_DEC_REQ);
            }

#ifdef RM_SUPPORT
            else if(pIsomGlob->med_type==MCI_TYPE_RM)
            {
                //rm to do ... simulate mp4/3gp
#ifdef USE_KEY_SYNC
                //int32 tk1,tk2;
                //tk1=hal_TimGetUpTime();

                if(KEY_SAMP_NUM==0 && VID_DEC_REQ>(pIsomGlob->v_fps>>V_FPS_SCALE_BIT))
                {
                    uint32 stamp1,stamp2;

                    //get cur real time
                    stamp1=(uint32)(((long long)pIsomGlob->a_current_frame_num+(long long)pIsomGlob->a_frame_sample*(long long)pIsomGlob->a_HalfPcmBufFrame)*1000/(long long)pIsomGlob->a_sample_rate);
                    //add sync time offset.
                    //seek and get next key time stamp.
                    stamp2=rm_getNextKeyTimeStamp(stamp1);

                    KEY_SAMP_NUM=stamp2;//recode the key frame time stamp.
                    KEY_SAMP_TIME_WAIT=((stamp2-stamp1)<<14)/1000; // *16384==<<14
                    KEY_SAMP_TIME_START=hal_TimGetUpTime();

                    if(KEY_SAMP_TIME_WAIT>=0)
                    {
                        diag_printf("[VID]stamp1=%d stamp2=%d wait=%d ms", stamp1,stamp2,(stamp2-stamp1));
                    }
                    else
                    {
                        KEY_SAMP_NUM=0;
                        KEY_SAMP_TIME_WAIT=0;
                        KEY_SAMP_TIME_START=0;
                    }
                }

                if(KEY_SAMP_NUM>0 && (hal_TimGetUpTime()-KEY_SAMP_TIME_START>=KEY_SAMP_TIME_WAIT))
                {
                    //diag_printf("[VID]last stamp=%d !",rm_getLastFrameTimeStamp());
                    if(KEY_SAMP_NUM>rm_getLastFrameTimeStamp())
                    {
                        diag_printf("[VID]KEY_SAMP_NUM=%d cur stamp=%d Go!", KEY_SAMP_NUM, rm_getLastFrameTimeStamp());
                        rm_seek_video(KEY_SAMP_NUM);
                        pIsomGlob->v_current_sample_num=KEY_SAMP_NUM/pIsomGlob->v_period;
                        pIsomGlob->v_current_frame_num=pIsomGlob->v_current_sample_num-2;
                        VID_DEC_REQ=0;
                    }

                    KEY_SAMP_NUM=0;
                    KEY_SAMP_TIME_WAIT=0;
                    KEY_SAMP_TIME_START=0;
                }

                //tk2=hal_TimGetUpTime();
                //diag_printf("rm key seek time=%d",  (tk2-tk1)*1000/16384);
#endif
                if(SEND_EVENT_NUM==0 && VID_DEC_REQ>0)
                {
                    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_IMG_DEC_REQ);
                    SEND_EVENT_NUM=1;
                }
                diag_printf("[VID]VID_DEC_REQ=%d", VID_DEC_REQ);
            }
#endif

            else if(pIsomGlob->med_type==MCI_TYPE_AVSTRM)
            {
                if(SEND_EVENT_NUM==0 && VID_DEC_REQ>0)
                {
                    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_IMG_DEC_REQ);
                    SEND_EVENT_NUM=1;
                }
                diag_printf("[VID]VID_DEC_REQ=%d", VID_DEC_REQ);
            }
            else
            {
                pIsomGlob->v_current_sample_num+=VID_DEC_REQ;
                pIsomGlob->v_current_frame_num+=VID_DEC_REQ;
                diag_printf("[VID]VID_DEC_REQ=%d", VID_DEC_REQ);
                VID_DEC_REQ=0;
            }
        }
        else if(pIsomGlob->open_audio==3)//audio pause
        {
            if((res=aud_StreamPause(audioItf, FALSE))!=AUD_ERR_NO)
            {
                diag_printf("[VID] ##WARNING##aud_StreamPause res=%d",res);
            }
            pIsomGlob->open_audio=1;
            diag_printf("[VID]resume audio play");
        }

#if 1

//vid_frame_read
        if(VID_DEC_REQ >= 0)
        {
            if(pVid_SampleBuf[1]!=0)
            {
                if((vid_last_sample_num+1) != pIsomGlob->v_current_sample_num || vid_SampleBuf_BytesRemain ==0 ||pIsomGlob->v_LastFrmStates==1)
                {
                    //be call when jump to key frame or not be invoke in last frame decoder.
                    while(hal_GoudaIsActive()) //wait unblock gouda done.
                    {
                        //diag_printf("[VID]gouda busy before read!");
                        sxr_Sleep(1);
                    }
                    vid_frame_read();
                    diag_printf("[VID]read frame outside video decoder!\n");
                }

                if(vid_SampleBuf_BytesRemain >=0 )
                {
                    pIsomGlob->v_SampleBuf= pVid_SampleBuf[vid_SampleBuf_id];
                    pIsomGlob->v_SampleBuf_BytesRemain= vid_SampleBuf_BytesRemain;
                    pIsomGlob->v_SampleBuf_BytesUsed= vid_SampleBuf_BytesUsed;
                    vid_SampleBuf_BytesRemain= 0;//can not get the consume before decode done.
                    vid_SampleBuf_BytesUsed= 0;
                    vid_SampleBuf_id= 1- vid_SampleBuf_id;
                    vid_last_sample_num= pIsomGlob->v_current_sample_num;
                }
                else
                {
                    diag_printf("[VID] last vid_frame_read fail! \n");
                    return ISOM_ERR_VIDEO_READ_FAIL;
                }
            }
            else
            {
                //set parameters
                vid_SampleBuf_id= 0;
                pVid_SampleBuf[0]= pIsomGlob->v_SampleBuf;
                vid_SampleBuf_BytesRemain = pIsomGlob->v_SampleBuf_BytesRemain;
                vid_SampleBuf_BytesUsed= pIsomGlob->v_SampleBuf_BytesUsed;
                while(hal_GoudaIsActive()) //wait unblock gouda done.
                {
                    //diag_printf("[VID]gouda busy before read!");
                    sxr_Sleep(1);
                }
                vid_frame_read();
                if(vid_SampleBuf_BytesRemain < 0 )
                {
                    diag_printf("[VID] vid_frame_read fail! \n");
                    return ISOM_ERR_VIDEO_READ_FAIL;
                }
                //update parameters
                pIsomGlob->v_SampleBuf= pVid_SampleBuf[0];
                pIsomGlob->v_SampleBuf_BytesRemain= vid_SampleBuf_BytesRemain;
                pIsomGlob->v_SampleBuf_BytesUsed= vid_SampleBuf_BytesUsed;
            }
        }

        //if(USE_SLEEP)
        //  COS_Sleep(1);//release cpu for other work
        if(pIsomGlob->v_SampleBuf_BytesRemain > 0)
        {
            switch(pIsomGlob->v_type)
            {
                case V_TYPE_H263:
                {
#ifdef H263_SUPPORT
                    int32 used_bytes;
                    int32 gob;
                    if(pIsomGlob->v_SampleBuf_BytesRemain>0)
                    {
                        //int32 tk1,tk2;
                        //tk1=hal_TimGetUpTime();

                        resetbits (pIsomGlob->v_SampleBuf+pIsomGlob->v_SampleBuf_BytesUsed, pIsomGlob->v_SampleBuf_BytesRemain);

                        gob= getheader ();
                        /*
                        if(errorstate || !gob )
                        {
                            diag_printf("[VID] h.263 getheader err gob=%d \n",gob);
                            diag_printf("[VID] h.263 getheader err !data use=%d \n", ld->rdptr - ld->rdbfr);
                            //AV_DEC_STATE=0;
                            //return  ISOM_ERR_INVALID_FORMAT;
                            pIsomGlob->bSkipFrame=TRUE;//only skip error frame.
                             pIsomGlob->v_SampleBuf_BytesUsed+=pIsomGlob->v_SampleBuf_BytesRemain;
                            break;
                        }*/

                        vocWorkState.pVOC_WORK=(UINT8*)&VOC_WORK;
                        vocWorkState.pVOC_VID=(int16*)&VOC_VID;
                        vocWorkState.pVOC_AUD =(int16*)&VOC_AUD;
                        vocWorkState.vid_display= vid_display;
                        vocWorkState.vid_voc_isr=NULL;//vid_voc_isr;
                        vocWorkState.aud_nframe_dec=(void*)aud_nframe_dec;
                        if(pVid_SampleBuf[1]!=0)
                            vocWorkState.vid_frame_read=(void*)vid_frame_read;
                        else
                            vocWorkState.vid_frame_read=NULL;

                        if(errorstate==0 && gob!=0 )
                            getpicture ((uint8*)pIsomGlob->v_YuvInfo, pIsomGlob->v_lcd_size_x, pIsomGlob->v_lcd_start_x, pIsomGlob->v_lcd_start_y, gob, NULL);
                        else
                        {
                            diag_printf("[VID] h.263 getheader err gob=%d \n",gob);
                            diag_printf("[VID] h.263 getheader err !data use=%d \n", ld->rdptr - ld->rdbfr);
                            errorstate=-1;
                            sxr_Sleep(16);
                        }

                        //tk2=hal_TimGetUpTime();
                        //diag_printf("h263 %d ms", (tk2-tk1)*1000/16384);

                        if(errorstate!=0)
                        {
                            diag_printf("[VID] h.263 getpicture err! \n");
                            //AV_DEC_STATE=0;
                            //return  ISOM_ERR_VIDEO_DEC_FAIL;

                            if(vocWorkState.vid_display)
                            {
                                vocWorkState.vid_display();
                                vocWorkState.vid_display= (void*)NULL;
                            }

                            pIsomGlob->bSkipFrame=TRUE;//only skip error frame.
                            pIsomGlob->v_SampleBuf_BytesUsed+=pIsomGlob->v_SampleBuf_BytesRemain;

                            VID_ERR_FRAME_REC+=1;
                            if(VID_ERR_FRAME_REC>5)
                            {
                                AUD_FADE_OUT_FLAG=0x7fff;
                                diag_printf("[VID] frame err >max!mute audio \n");
                                if(VID_ERR_FRAME_REC > VID_ERR_FRAME_NUM && pIsomGlob->med_type !=MCI_TYPE_AVSTRM)
                                {
                                    return ISOM_ERR_VIDEO_DEC_FAIL;
                                }
                            }
                            break;
                        }
                        else
                        {
                            if(VID_ERR_FRAME_REC>0)
                            {
                                VID_ERR_FRAME_REC--;
                                if(VID_ERR_FRAME_REC>3)
                                {
                                    VID_ERR_FRAME_REC=2;
                                }
                                if(VID_ERR_FRAME_REC==0 && AUD_FADE_OUT_FLAG>3)
                                {
                                    AUD_FADE_OUT_FLAG=3;
                                }
                                //keep 3 right frame
                                pIsomGlob->bSkipFrame=TRUE;//only skip error frame.
                            }
                        }

                        used_bytes=ld->rdptr - ld->rdbfr;
                        /* Update buffer pointers */

                        if(used_bytes > 0)
                        {
                            pIsomGlob->v_SampleBuf_BytesUsed+=used_bytes;
                        }
                    }

                    break;
#endif
                }
#ifdef H264_SUPPORT
                case V_TYPE_H264:
                {
                    int32 got_picture=0;
                    if(pIsomGlob->v_SampleBuf_BytesRemain>0)
                    {
                        //int32 tk1,tk2;
                        //tk1=hal_TimGetUpTime();

                        vocWorkState.pVOC_WORK=(UINT8*)&VOC_WORK;
                        vocWorkState.pVOC_VID=(int16*)&VOC_VID;
                        vocWorkState.pVOC_AUD =(int16*)&VOC_AUD;
                        vocWorkState.vid_display= vid_display;
                        vocWorkState.vid_voc_isr=NULL;
                        vocWorkState.aud_nframe_dec=(void*)aud_nframe_dec;
                        vocWorkState.vid_frame_read=NULL;

                        res=avcodec_decode_video(pIsomGlob->pH264Hdl->codec_context, pIsomGlob->pH264Hdl->frame_picture,
                                                 &got_picture, pIsomGlob->v_SampleBuf+pIsomGlob->v_SampleBuf_BytesUsed, pIsomGlob->v_SampleBuf_BytesRemain);
                        if(res<0 || !got_picture)
                        {
                            if(vocWorkState.vid_display)
                            {
                                vocWorkState.vid_display();
                                vocWorkState.vid_display=(void*) NULL;
                            }

                            if(h264_check_mem())
                            {
                                if(video_dec_reset()!=0)
                                    return ISOM_ERR_MALLOC_FAIL;
                            }

                            pIsomGlob->bSkipFrame=TRUE;//only skip error frame.
                            pIsomGlob->v_SampleBuf_BytesUsed+=pIsomGlob->v_SampleBuf_BytesRemain;
                            diag_printf("[VID] h.264 decode err! res=%d\n",res);
                            break;
                        }

                        //tk2=hal_TimGetUpTime();
                        //diag_printf("h264 %d ms tp=%d", (tk2-tk1)*1000/16384,pIsomGlob->pH264Hdl->frame_picture->pict_type);
                        //diag_printf("[VID] h.264 decode consume size=%d got_picture=%d type=%d\n",res,got_picture,pIsomGlob->pH264Hdl->frame_picture->pict_type);

                        pIsomGlob->v_SampleBuf_BytesUsed+=res;
                        pIsomGlob->v_YuvInfo[0]=(unsigned int)pIsomGlob->pH264Hdl->frame_picture->data[0];
                        pIsomGlob->v_YuvInfo[1]=(unsigned int)pIsomGlob->pH264Hdl->frame_picture->data[1];
                        pIsomGlob->v_YuvInfo[2]=(unsigned int)pIsomGlob->pH264Hdl->frame_picture->data[2];
                        pIsomGlob->v_YuvInfo[3]=pIsomGlob->pH264Hdl->frame_picture->linesize[0];
                        pIsomGlob->v_YuvInfo[4]=pIsomGlob->pH264Hdl->codec_context->height;
                    }
                    break;
                }
#endif
                case V_TYPE_MPEG4:
                {
#ifdef MPEG4_SUPPORT
                    int32 used_bytes;
                    xvid_dec_stats_t xvid_dec_stats;

                    if(pIsomGlob->v_SampleBuf_BytesRemain>0)
                    {

                        vocWorkState.vid_display= vid_display;//once
                        if(pVid_SampleBuf[1]!=0)
                            vocWorkState.vid_frame_read=(void*)vid_frame_read;//once
                        else
                            vocWorkState.vid_frame_read=NULL;

                        while(pIsomGlob->v_SampleBuf_BytesRemain > MIN_USEFUL_BYTES)
                        {

                            vocWorkState.pVOC_WORK=(UINT8*)&VOC_WORK;
                            vocWorkState.pVOC_VID=(int16*)&VOC_VID;
                            vocWorkState.pVOC_AUD =(int16*)&VOC_AUD;
                            //vocWorkState.vid_display= vid_display;
                            vocWorkState.vid_voc_isr=NULL;//vid_voc_isr;
                            vocWorkState.aud_nframe_dec=(void*)aud_nframe_dec;

                            used_bytes = dec_main(pIsomGlob->v_SampleBuf+pIsomGlob->v_SampleBuf_BytesUsed,
                                                  (uint8*)pIsomGlob->v_YuvInfo,
                                                  pIsomGlob->v_SampleBuf_BytesRemain, &xvid_dec_stats, NULL,
                                                  ((pIsomGlob->med_type ==MCI_TYPE_MJPG) ? TRUE : FALSE));

                            if(used_bytes == -1)//profile unfit
                            {
                                diag_printf("[VID] video decode err v_current_sample_num=%d \n",pIsomGlob->v_current_sample_num);
                                //AV_DEC_STATE=0;
                                //return ISOM_ERR_VIDEO_DEC_FAIL;
                                pIsomGlob->bSkipFrame=TRUE;//only skip error frame.
                                pIsomGlob->v_LastFrmStates=1;
                                pIsomGlob->v_SampleBuf_BytesUsed = pIsomGlob->v_SampleBuf_BytesRemain;


                                if(vocWorkState.vid_display)
                                {
                                    vocWorkState.vid_display();
                                    vocWorkState.vid_display= (void*)NULL;
                                }

                            }

                            if(used_bytes == -2)
                            {

                                if(vocWorkState.vid_display)
                                {
                                    vocWorkState.vid_display();
                                    vocWorkState.vid_display= NULL;
                                }

                                pIsomGlob->bSkipFrame = TRUE;
                                pIsomGlob->v_LastFrmStates=0;
                                pIsomGlob->v_SampleBuf_BytesUsed = pIsomGlob->v_SampleBuf_BytesRemain;
                            }

                            if(used_bytes > 0)
                            {
                                pIsomGlob->v_LastFrmStates=0;
                                if(xvid_dec_stats.type == XVID_TYPE_VOL)
                                {
                                    pIsomGlob->v_SampleBuf_BytesUsed+=used_bytes;
                                    pIsomGlob->v_SampleBuf_BytesRemain-=used_bytes;
                                    continue;
                                }
                                pIsomGlob->v_SampleBuf_BytesUsed+=pIsomGlob->v_SampleBuf_BytesRemain;
                            }

                            break;
                        }
                        if(pIsomGlob->v_SampleBuf_BytesRemain==MIN_USEFUL_BYTES) pIsomGlob->v_SampleBuf_BytesUsed=1;
                    }
                    else//for final 1 sample out
                    {

                        vocWorkState.pVOC_WORK=(UINT8*)&VOC_WORK;
                        vocWorkState.pVOC_VID=(int16*)&VOC_VID;
                        vocWorkState.pVOC_AUD =(int16*)&VOC_AUD;
                        vocWorkState.vid_display= vid_display;
                        vocWorkState.vid_voc_isr=NULL;//vid_voc_isr;
                        vocWorkState.aud_nframe_dec=(void*)aud_nframe_dec;
                        vocWorkState.vid_frame_read=NULL;

                        used_bytes = dec_main(NULL,  (uint8*)pIsomGlob->v_YuvInfo, -1, &xvid_dec_stats, NULL,
                                              ((pIsomGlob->med_type ==MCI_TYPE_MJPG) ? TRUE : FALSE));
                        if(vocWorkState.vid_display)
                        {
                            vocWorkState.vid_display();
                            vocWorkState.vid_display= (void*)NULL;
                        }

                        if(used_bytes == -2)
                        {
                            pIsomGlob->bSkipFrame = TRUE;
                        }
                    }

                    break;
#endif
                }
                case V_TYPE_MJPEG:
                {
                    DEC_UNIT jpgDec;
                    //int32 tk1,tk2;
                    //tk1=hal_TimGetUpTime();

                    vocWorkState.pVOC_WORK=(UINT8*)&VOC_WORK;
                    vocWorkState.pVOC_VID=(int16*)&VOC_VID;
                    vocWorkState.pVOC_AUD =(int16*)&VOC_AUD;
                    vocWorkState.vid_display= vid_display;
                    vocWorkState.vid_voc_isr=NULL;
                    vocWorkState.aud_nframe_dec=(void*)aud_nframe_dec;
                    vocWorkState.vid_frame_read=NULL;

                    vid_buf_id= 1-vid_buf_id;
                    res= MMF_JpegDecodePicture((long)((char*)(pIsomGlob->v_SampleBuf)),
                                               (char*) pVid_buf_Y[vid_buf_id],
                                               (pIsomGlob->v_width*pIsomGlob->v_height*3)>>1,
                                               pIsomGlob->v_width, pIsomGlob->v_height,
                                               0,0,
                                               pIsomGlob->v_width-1, pIsomGlob->v_height-1,
                                               &jpgDec, 2,fillAudioData);

                    //tk2=hal_TimGetUpTime();
                    //diag_printf("jpeg %d ms", (tk2-tk1)*1000/16384);

                    if(vocWorkState.vid_display)
                    {
                        vocWorkState.vid_display();
                        vocWorkState.vid_display= (void*)NULL;
                    }

                    pIsomGlob->v_YuvInfo[0]=(uint32)pVid_buf_Y[vid_buf_id];
                    pIsomGlob->v_YuvInfo[1]=(uint32)pVid_buf_U[vid_buf_id];
                    pIsomGlob->v_YuvInfo[2]=(uint32)pVid_buf_V[vid_buf_id];
                    pIsomGlob->v_YuvInfo[3]=(uint32)jpgDec.width;
                    pIsomGlob->v_YuvInfo[4]=(uint32)jpgDec.height;
                    pIsomGlob->v_SampleBuf_BytesUsed+=pIsomGlob->v_SampleBuf_BytesRemain;
                    if(res==FUNC_OK)
                    {
                        VID_ERR_FRAME_REC=0;
                    }
                    else
                    {
                        pIsomGlob->bSkipFrame = TRUE;
                        VID_ERR_FRAME_REC+=1;
                        if(VID_ERR_FRAME_REC<VID_ERR_FRAME_NUM)
                        {
                            diag_printf("[VID]MMF_JpegDecodePicture err!res=%d VID_ERR_FRAME_REC=%d", res,VID_ERR_FRAME_REC);
                        }
                        else
                        {
                            diag_printf("[VID]MMF_JpegDecodePicture err!res=%d VID_ERR_FRAME_REC=%d", res,VID_ERR_FRAME_REC);
                            //AV_DEC_STATE=0;
                            VID_ERR_FRAME_REC=0;
                            //return ISOM_ERR_VIDEO_DEC_FAIL;
                        }
                    }

                    break;
                }

#ifdef RM_SUPPORT
                case V_TYPE_RV:
                {
                    //VocWorkStruct vocState;
                    INT8 skip=0;
                    //int32 tk1,tk2;
                    //tk1=hal_TimGetUpTime();

                    vocWorkState.pVOC_WORK=(UINT8*)&VOC_WORK;
                    vocWorkState.pVOC_VID=(int16*)&VOC_VID;
                    vocWorkState.vid_voc_isr=vid_voc_isr;
                    vocWorkState.aud_nframe_dec=(void*)aud_nframe_dec;
                    vocWorkState.vid_frame_read=NULL;

                    if(keep_skip_frame_num<MAX_KEEP_SKIP_F_NUM && VID_DEC_REQ>0)
                    {
                        skip=1;
                    }

                    res=rm_getOneFrameVideo((INT8*)pIsomGlob->v_YuvInfo, /*&vocWorkState,*/ skip);
                    if(res==HXR_SKIP_BFRAME)
                    {
                        pIsomGlob->bSkipFrame = TRUE;
                        keep_skip_frame_num+=1;
                        diag_printf("[VID] skip B");
                    }
                    else if(res!=HXR_OK)
                    {
                        diag_printf("[VID] rm_getOneFrameVideo err!res=%d ", res);
                        //AV_DEC_STATE=0;
                        return ISOM_ERR_VIDEO_DEC_FAIL;
                    }
                    else
                    {
                        keep_skip_frame_num=0;
                    }

                    pIsomGlob->v_SampleBuf_BytesUsed=1;
                    //tk2=hal_TimGetUpTime();
                    //    diag_printf("rm frame time=%d",  (tk2-tk1)*1000/16384);

                    break;
                }
#endif

                default:
                    break;
            }
#endif
        }
        else
        {
            vid_display();
            pIsomGlob->bSkipFrame = TRUE;
            diag_printf("[VID]skip vid decode.VID_DEC_REQ %d",VID_DEC_REQ);
        }

        tick2=hal_TimGetUpTime();
        if(pIsomGlob->bSkipFrame != TRUE)
        {
            totalTv+=(tick2-tick1);
            countv++;
            //diag_printf("frame time=%d",  (tick2-tick1)*1000/16384);
        }

        sxr_Sleep(2);//COS_Sleep ms , sxr_Sleep tick 16384tick=1s 16tick=1ms
    }
#endif

    if(pIsomGlob->med_type==MCI_TYPE_AVSTRM &&
            (VID_DEC_REQ>(int16)(5*(pIsomGlob->v_fps>>V_FPS_SCALE_BIT))
             || VID_DEC_REQ < -(int16)(5*(pIsomGlob->v_fps>>V_FPS_SCALE_BIT))))
    {
        //unsync too much, clean  data
        avinfostr *avinfohdl= (avinfostr*)pIsomGlob->filehdl;
        diag_printf("[VID]avstrm unsync too much %d, reset data ! ",VID_DEC_REQ);

        avStrmFrmHdV.samplesize=0;
        avStrmFrmHdA.samplesize=0;
        avStrmCurPosV=avStrmCurPosA;
        VID_DEC_REQ=0;
        avstream_srv_reset(avinfohdl);
        if(avinfohdl->avstream_srv_callback )
        {
            avinfohdl->avstream_srv_callback(1);
        }

    }


    if(VID_DEC_REQ<=0 && pIsomGlob->med_type != MCI_TYPE_AVSTRM )
    {
        aud_nframe_dec();

        if(SEND_EVENT_NUM==0 && AUD_DATA_REQ>0 && aud_extra_pcmbuf_state[A_EXBF_WT_ID].done==0)
        {
            SEND_EVENT_NUM=1;
            mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_AUD_DEC_REQ);
        }
    }

    //AV_DEC_STATE=0;
    return ISOM_ERR_OK;
}



static int32 vid_export_info_from_parse(GF_ISOFile *file,uint8 open_audio)
{
    GF_DecoderConfig *temp_dcfg=NULL;
    u32 trackID, i,  m_type, m_stype;
    u32 track_count=0;
    u32 av_track=0;

    pIsomGlob->v_type= AV_TYPE_INVALID;
    pIsomGlob->a_type= AV_TYPE_INVALID;

    track_count = gf_isom_get_track_count(file);

    if(track_count<1)
    {
        //diag_printf("[VID] unsupport only video or only audio! track_count=%d\n", track_count);
        diag_printf("[VID] error track_count=%d\n", track_count);
        return ISOM_ERR_UNSUPPORT;
    }

    for(i=0; i<track_count/*MAX_TRACK_NUM*/; i++)
    {
        //trackID = gf_isom_get_track_by_id(file, i+1);
        trackID=i+1;

        m_type = gf_isom_get_media_type(file, trackID);//vide soun
        m_stype = gf_isom_get_media_subtype(file, trackID, 1);
        diag_printf("[VID] m_stype=%x  %d  trackID=%d \n",m_stype, m_stype, trackID);
        diag_printf("[VID] m_stype=%x  %d \n", GF_ISOM_BOX_TYPE_SAMR, GF_ISOM_BOX_TYPE_SAMR);
        if(m_type==GF_ISOM_MEDIA_VISUAL)
        {
            diag_printf("[VID]v m_type=%x trackID=%d\n",m_type,trackID);
            if(pIsomGlob->v_type != AV_TYPE_INVALID)
                continue;
        }
        else if(m_type==GF_ISOM_MEDIA_AUDIO)
        {
            diag_printf("[VID]a m_type=%x trackID=%d\n",m_type,trackID);
            if(pIsomGlob->a_type != AV_TYPE_INVALID)
                continue;
        }
        else
        {
            diag_printf("[VID]discard m_type=%x trackID=%d\n",m_type,trackID);
            continue;
        }

        temp_dcfg=NULL;

        if ((m_stype==GF_ISOM_BOX_TYPE_MP4V) ||(m_stype==GF_ISOM_BOX_TYPE_MP4A))
            temp_dcfg = gf_isom_get_decoder_config(file, trackID, 1);//esds

        if(m_type==GF_ISOM_MEDIA_VISUAL)
        {
            pIsomGlob->v_trackID=trackID;
            pIsomGlob->v_trak= gf_isom_get_track_from_file(file,trackID);
            if(!pIsomGlob->v_trak)
            {
                diag_printf("[VID]invalid gf_isom_get_track_from_file\n");
                continue;
            }

            if(pIsomGlob->v_trak->Media)
            {
                if(pIsomGlob->v_trak->Media->information)
                {
                    pIsomGlob->v_info=(GF_MediaInformationBox *)pIsomGlob->v_trak->Media->information;
                    pIsomGlob->v_info->lastSampleNumber=0;
                    pIsomGlob->v_info->lastFirstSample=0;
                    pIsomGlob->v_info->lastOffset=0;

                    if(!pIsomGlob->v_info->dataHandler)
                    {
                        if(pIsomGlob->v_trak->Media->mediaTrack)
                        {
                            if(pIsomGlob->v_trak->Media->mediaTrack->moov)
                            {
                                if(pIsomGlob->v_trak->Media->mediaTrack->moov->mov)
                                {
                                    if(pIsomGlob->v_trak->Media->mediaTrack->moov->mov->movieFileMap)
                                        pIsomGlob->v_info->dataHandler = pIsomGlob->v_trak->Media->mediaTrack->moov->mov->movieFileMap;
                                    else
                                    {
                                        diag_printf("[VID]invalid pIsomGlob->v_trak->Media 5\n");
                                        continue;
                                    }
                                }
                                else
                                {
                                    diag_printf("[VID]invalid pIsomGlob->v_trak->Media 4\n");
                                    continue;
                                }
                            }
                            else
                            {
                                diag_printf("[VID]invalid pIsomGlob->v_trak->Media 3\n");
                                continue;
                            }
                        }
                        else
                        {
                            diag_printf("[VID]invalid pIsomGlob->v_trak->Media 2\n");
                            continue;
                        }
                    }
                }
                else
                {
                    diag_printf("[VID]invalid pIsomGlob->v_trak->Media 1\n");
                    continue;
                }
            }
            else
            {
                diag_printf("[VID]invalid pIsomGlob->v_trak->Media 0\n");
                continue;
            }

            if(pIsomGlob->v_info->sampleTable)
            {
                if(pIsomGlob->v_info->sampleTable->TimeToSample)
                {
                    if(stbl_GetSampleDTScount(pIsomGlob->v_info->sampleTable->TimeToSample)>1)
                    {
                        DTS_FLG=0;
                        if(pIsomGlob->v_trak->Media->mediaHeader)
                        {
                            pIsomGlob->v_timeScale=pIsomGlob->v_trak->Media->mediaHeader->timeScale;
                        }
                        else
                        {
                            diag_printf("[VID]invalid mediaHeader");
                            continue;
                        }
                    }
                    else
                    {
                        DTS_FLG=0xFFFFFFFF;
                        pIsomGlob->v_timeScale=0;
                    }
                }
                else
                {
                    diag_printf("[VID]invalid TimeToSample");
                    continue;
                }
            }
            else
            {
                diag_printf("[VID]invalid sampleTable");
                continue;
            }

            pIsomGlob->total_time=gf_isom_get_sample_totaltime(file, pIsomGlob->v_trackID);
            pIsomGlob->v_fps= gf_isom_get_sample_FPS(file, pIsomGlob->v_trackID);//fps<<V_FPS_SCALE_BIT
            pIsomGlob->v_height= gf_isom_get_sample_height(file, pIsomGlob->v_trackID);
            pIsomGlob->v_width= gf_isom_get_sample_width(file, pIsomGlob->v_trackID);
            pIsomGlob->v_total_sample_num = gf_isom_get_sample_count(file, pIsomGlob->v_trackID);//total sample(frame) Num
            pIsomGlob->v_total_frame_num=pIsomGlob->v_total_sample_num;//will change
            pIsomGlob->v_total_keyframe_num=gf_isom_get_total_KeySamp_count(file, pIsomGlob->v_trackID);

        }
        else if (m_type==GF_ISOM_MEDIA_AUDIO)
        {
            pIsomGlob->a_trackID=trackID;
            pIsomGlob->a_trak= gf_isom_get_track_from_file(file,trackID);
            if(!pIsomGlob->a_trak)
            {
                diag_printf("[VID]invalid gf_isom_get_track_from_file\n");
                continue;
            }

            if(pIsomGlob->a_trak->Media)
            {
                if(pIsomGlob->a_trak->Media->information)
                {
                    pIsomGlob->a_info=(GF_MediaInformationBox *)pIsomGlob->a_trak->Media->information;
                    pIsomGlob->a_info->lastSampleNumber=0;
                    pIsomGlob->a_info->lastFirstSample=0;
                    pIsomGlob->a_info->lastOffset=0;

                    if(!pIsomGlob->a_info->dataHandler)
                    {
                        if(pIsomGlob->a_trak->Media->mediaTrack)
                        {
                            if(pIsomGlob->a_trak->Media->mediaTrack->moov)
                            {
                                if(pIsomGlob->a_trak->Media->mediaTrack->moov->mov)
                                {
                                    if(pIsomGlob->a_trak->Media->mediaTrack->moov->mov->movieFileMap)
                                        pIsomGlob->a_info->dataHandler = pIsomGlob->a_trak->Media->mediaTrack->moov->mov->movieFileMap;
                                    else
                                    {
                                        diag_printf("[VID]invalid Media 5\n");
                                        continue;
                                    }
                                }
                                else
                                {
                                    diag_printf("[VID]invalid Media 4\n");
                                    continue;
                                }
                            }
                            else
                            {
                                diag_printf("[VID]invalid Media 3\n");
                                continue;
                            }
                        }
                        else
                        {
                            diag_printf("[VID]invalid Media 2\n");
                            continue;
                        }
                    }
                }
                else
                {
                    diag_printf("[VID]invalid Media 1\n");
                    continue;
                }
            }
            else
            {
                diag_printf("[VID]invalid Media 0\n");
                continue;
            }

            pIsomGlob->a_sample_rate=gf_isom_get_sample_samplerate(file, pIsomGlob->a_trackID);
            pIsomGlob->a_bit=gf_isom_get_sample_bitspersample(file, pIsomGlob->a_trackID);
            if (temp_dcfg)
                pIsomGlob->a_bitrate=temp_dcfg->maxBitrate;
            pIsomGlob->a_channels=gf_isom_get_sample_channel_count(file, pIsomGlob->a_trackID);
            pIsomGlob->a_total_sample_num = gf_isom_get_sample_count(file, pIsomGlob->a_trackID);//total sample(chunk) Num
            //pIsomGlob->a_total_frame_num=pIsomGlob->a_total_sample_num;//will change
            pIsomGlob->a_total_frame_num=0;

        }

        if (temp_dcfg)
        {
            pIsomGlob->aacPara.is_aac=-1;
            pIsomGlob->aacPara.aac_type=0;

            switch (temp_dcfg->streamType)
            {
                case GF_STREAM_VISUAL:
                    switch (temp_dcfg->objectTypeIndication)
                    {
                        //Extracting MPEG-4 Visual stream to m4v ".m4v"
                        case 0x20:
                        {
                            //memcpy(pIsomGlob->v_SampleBuf,temp_dcfg->decoderSpecificInfo->data, temp_dcfg->decoderSpecificInfo->dataLength);
                            //pIsomGlob->v_SampleBuf_BytesRemain+=temp_dcfg->decoderSpecificInfo->dataLength;
                            pIsomGlob->v_decSpeInfo=temp_dcfg->decoderSpecificInfo;
                            diag_printf("[VID] MPEG4");
                        }
                        break;
                        //Extracting MPEG-4 AVC-H264 stream to h264".h264"
                        case 0x21:
                            //avccfg = gf_isom_avc_config_get(file, pIsomGlob->v_trackID, 1);
                            break;
                        //Extracting MPEG-1 Visual stream to h264".m1v"
                        case 0x6A:
                            break;
                        //Extracting MPEG-2 Visual stream to h264".m2v"
                        case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65:
                            break;
                        //Extracting JPEG image".jpg"
                        case 0x6C:
                            break;
                        //Extracting PNG image".png"
                        case 0x6D:
                            break;
                        default:
                            diag_printf("[VID]invalid objectTypeIndication %x\n",temp_dcfg->objectTypeIndication);
                            continue;
                    }
                    break;
                case GF_STREAM_AUDIO:
                    switch (temp_dcfg->objectTypeIndication)
                    {
                        //Extracting MPEG-2 AAC".aac"
                        case 0x66:
                        case 0x67:
                        case 0x68:
                            pIsomGlob->aacPara.is_aac = A_TYPE_MPEG2_AAC;
                            pIsomGlob->aacPara.aac_type = temp_dcfg->objectTypeIndication - 0x66;
                            gf_m4a_get_config(temp_dcfg->decoderSpecificInfo->data, temp_dcfg->decoderSpecificInfo->dataLength, &pIsomGlob->a_cfg);
                            diag_printf("[VID] MPEG2_AAC");
                            break;
                        //Extracting MPEG-4 AAC ".aac"
                        case 0x40:
                            pIsomGlob->aacPara.is_aac = A_TYPE_MPEG4_AAC;
                            gf_m4a_get_config(temp_dcfg->decoderSpecificInfo->data, temp_dcfg->decoderSpecificInfo->dataLength, &pIsomGlob->a_cfg);
                            pIsomGlob->aacPara.aac_type = pIsomGlob->a_cfg.base_object_type - 1;
                            diag_printf("[VID] MPEG4_AAC");
                            break;
                        //Extracting MPEG-1/2 Audio (MP3)".mp3"
                        case 0x69:
                        case 0x6B:
                            break;
                        default:
                            diag_printf("[VID] invalid temp_dcfg->objectTypeIndication=%x \n",temp_dcfg->objectTypeIndication);
                            continue;
                    }
                    break;
                default:
                    diag_printf("[VID] invalid temp_dcfg->streamType=%x \n",temp_dcfg->streamType);
                    continue;
            }

            if (temp_dcfg->streamType==GF_STREAM_AUDIO && pIsomGlob->aacPara.is_aac != -1)
            {
                GF_BitToInt bs;
                memset(aacHead, 0, AAC_HEAD_SIZE);
                bs.stream=aacHead;
                bs.current=0;
                bs.nbBits=0;

                gf_bs_bit_to_int(&bs, 0xFFF, 12);/*sync*/
                gf_bs_bit_to_int(&bs, (pIsomGlob->aacPara.is_aac==A_TYPE_MPEG2_AAC) ? 1 : 0, 1);/*mpeg2 aac*/
                gf_bs_bit_to_int(&bs, 0, 2); /*layer*/
                gf_bs_bit_to_int(&bs, 1, 1); /* protection_absent*/
                gf_bs_bit_to_int(&bs, pIsomGlob->aacPara.aac_type, 2);
                gf_bs_bit_to_int(&bs, pIsomGlob->a_cfg.base_sr_index, 4);
                gf_bs_bit_to_int(&bs, 0, 1);
                gf_bs_bit_to_int(&bs, pIsomGlob->a_cfg.nb_chan, 3);
                gf_bs_bit_to_int(&bs, 0, 4);
                //gf_bs_bit_to_int(&bs, AAC_HEAD_SIZE+dataLength, 13);
                gf_bs_bit_to_int(&bs, 0, 13);//13bit will be rewrite when every sample read
                gf_bs_bit_to_int(&bs, 0x7FF, 11);
                gf_bs_bit_to_int(&bs, 0, 2);
            }
            //gf_odf_desc_del((GF_Descriptor *) dcfg);
        }
#ifdef H263_SUPPORT
        else if(m_stype==GF_ISOM_SUBTYPE_3GP_H263) //add shenh
        {
            //return ISOM_ERR_UNSUPPORT;//change shenh
            pIsomGlob->h263Sp=gf_isom_get_h263_spec(file, pIsomGlob->v_trackID);
            if(!pIsomGlob->h263Sp) return ISOM_ERR_UNSUPPORT;
            diag_printf("[VID] H263");
        }
#endif
        else if(m_stype==GF_ISOM_SUBTYPE_AVC_H264)
        {
            pIsomGlob->h264Cfg=gf_isom_get_avc_config(file, pIsomGlob->v_trackID);
            if(!pIsomGlob->h264Cfg) return ISOM_ERR_UNSUPPORT;
            diag_printf("[VID] H264");
        }
        else if(m_stype==GF_ISOM_SUBTYPE_3GP_JPEG)
        {
            diag_printf("[VID] JPEG");
        }
        else if(m_stype==GF_ISOM_SUBTYPE_3GP_AMR)//add shenh
        {
            //return ISOM_ERR_UNSUPPORT;//change shenh
            //  pIsomGlob->amrSp=gf_isom_get_amr_spec(file, pIsomGlob->a_trackID);
            //  if(!pIsomGlob->amrSp) return ISOM_ERR_UNSUPPORT;
            diag_printf("[VID] AMR");
        }
        else if(m_stype==GF_ISOM_BOX_TYPE_MP3)
        {
            diag_printf("[VID] MP3");
        }
        else
        {
            /*
            udesc = gf_isom_get_generic_sample_description(file, trackID, 1);
            if (udesc) {
                dsi = udesc->extension_buf; udesc->extension_buf = NULL;
                dsi_size = udesc->extension_buf_size;
            }
            if (udesc) free(udesc);
            */
            diag_printf("[VID] invalid  m_stype=%x \n",m_stype);
            continue;//change shenh
        }

        if(m_type==GF_ISOM_MEDIA_VISUAL)
        {
            if(m_stype==GF_ISOM_BOX_TYPE_MP4V)
                pIsomGlob->v_type=V_TYPE_MPEG4;
            else if(m_stype==GF_ISOM_BOX_TYPE_S263)
                pIsomGlob->v_type=V_TYPE_H263;
            else if(m_stype==GF_ISOM_SUBTYPE_AVC_H264)
                pIsomGlob->v_type=V_TYPE_H264;
            else if(m_stype==GF_ISOM_BOX_TYPE_JPEG)
                pIsomGlob->v_type=V_TYPE_MJPEG;

        }
        else if (m_type==GF_ISOM_MEDIA_AUDIO)
        {
            if(m_stype==GF_ISOM_BOX_TYPE_MP4A)
                pIsomGlob->a_type=A_TYPE_AAC;
            else if(m_stype==GF_ISOM_BOX_TYPE_SAMR)
                pIsomGlob->a_type=A_TYPE_AMR;
            else if(m_stype==GF_ISOM_BOX_TYPE_MP3)
                pIsomGlob->a_type=A_TYPE_MP3;
        }

        av_track++;

        if(av_track>=2)
        {
            if(DTS_FLG !=0xFFFFFFFF)
            {
                //check invalid
                long long totalDuration1, totalDuration2, totalDuration3;
                long long timeScale1, timeScale2, timeScale3;
                //mvhd
                totalDuration1= file->moov->mvhd->duration;
                timeScale1= file->moov->mvhd->timeScale;
                //mdhd video
                totalDuration2= pIsomGlob->v_trak->Media->mediaHeader->duration;
                timeScale2 =pIsomGlob->v_trak->Media->mediaHeader->timeScale;
                //mdhd audio
                totalDuration3= pIsomGlob->a_trak->Media->mediaHeader->duration;
                timeScale3 =pIsomGlob->a_trak->Media->mediaHeader->timeScale;

                if(timeScale1 && timeScale2 && timeScale3)
                {
                    if( ABS((totalDuration2/timeScale2) - (totalDuration3/timeScale3)) -ABS((totalDuration1/timeScale1) - (totalDuration3/timeScale3)) >3 )
                    {
                        long long sampleCount;
                        DTS_FLG = 0xFFFFFFFF;//not use DTS
                        pIsomGlob->v_timeScale=0;

                        diag_printf("[VID]check totalDuration & timeScale for DTS. Old fps=%d",pIsomGlob->v_fps>>8);
                        diag_printf("[VID]mvhd dur=%d scale=%d",(uint32)totalDuration1,(uint32)timeScale1);
                        diag_printf("[VID]mdhd vid dur=%d scale=%d",(uint32) totalDuration2,(uint32) timeScale2);
                        diag_printf("[VID]mdhd aud dur=%d scale=%d ",(uint32)totalDuration3,(uint32)timeScale3);

                        if(totalDuration1 && pIsomGlob->v_trak->Media->information->sampleTable->SampleSize)
                        {
                            sampleCount= pIsomGlob->v_trak->Media->information->sampleTable->SampleSize->sampleCount;
                            pIsomGlob->v_fps= ((sampleCount<<8)*timeScale1+(totalDuration1>>1))/totalDuration1;
                        }
                        diag_printf("[VID]Not use DTS and new fps=%d",pIsomGlob->v_fps>>8);

                    }
                }
            }

            return ISOM_ERR_OK;
        }
    }

    if(pIsomGlob->v_type== AV_TYPE_INVALID )
    {
        diag_printf("[VID] unsupport  invalid video track!\n");
        return ISOM_ERR_UNSUPPORT;
    }
    else if(pIsomGlob->a_type== AV_TYPE_INVALID && open_audio !=0)
    {
        diag_printf("[VID] WARNING  invalid audio track!\n");
    }

    return ISOM_ERR_OK;
}

INT32 *G_VidSBCConstY=NULL;
extern const INT32 G_VppSBCConstX[];
extern U8 * gdi_layer_using_for_videoplayer(U8 index);
int32 mmc_vid_isom_open_file( HANDLE file_handle ,  uint8 file_mode, uint32 data_Len, int32 audio_path, uint8 open_audio, mci_type_enum type, void(*vid_play_finish_callback)(int32), void (*vid_draw_panel_callback)(uint32))
{

    int32 res=0,i;
    SSHdl *my_ss_handle = MpegPlayer.MpegInput.ss_handle;
    bt_a2dp_audio_cap_struct * my_audio_config_handle=   MpegPlayer.MpegInput.audio_config_handle ;

#ifdef VID_XCPU_FREQ_SET
    XCPU_FREQ_STATE=0;
#endif
#ifdef VID_VOC_FREQ_SET
    VOC_FREQ_STATE=0;
#endif
    pIsomGlob=0;
    /*for voc*/
    VOC_OPEN=0;
    VOC_VID=0;
    VOC_AUD=0;
    VOC_WORK=VOC_WORK_NON;
    VOC_SETTING=0;
    //file_mode=3;//test
    //type=MCI_TYPE_AVSTRM;//test
    //file_handle=avstream_srv_testinit(file_handle);//test

    if(play_state!=VID_PLAY_STATE_INVALID)
    {
        diag_printf("[VID] mmc_vid_isom_open_file err play_state= %d \n",play_state);
        res= ISOM_ERR_PLAY_STATE_FAIL;
        goto open_error;
    }
    pVid_SampleBuf[0]=0;
    pVid_SampleBuf[1]=0;
    //pIsomGlob=NULL;
    //save pre bgcolor
    prebgcolor=hal_GoudaGetBgColor();

    //malloc global parameters
    pIsomGlob=(IsomPlayGlobal*)mmc_MemMalloc(sizeof(IsomPlayGlobal));
    if(pIsomGlob)
        memset(pIsomGlob, 0, sizeof(IsomPlayGlobal));
    else
    {
        res=ISOM_ERR_MALLOC_FAIL;
        goto open_error;
    }

    pIsomGlob->med_type=type;

    if(vid_play_finish_callback)
        pIsomGlob->vid_play_finish_callback=vid_play_finish_callback;
    else
    {
        res=ISOM_ERR_PARAM_ERROR;
        goto open_error;
    }

    if(vid_draw_panel_callback)
        pIsomGlob->vid_draw_panel_callback=vid_draw_panel_callback;
    else
    {
        pIsomGlob->vid_draw_panel_callback=0;
        diag_printf("[VID]vid_draw_panel_callback NULL!!!");
        //res=ISOM_ERR_PARAM_ERROR;
        //goto open_error;
    }

    pIsomGlob->dec_mode=1;//
    diag_printf("[VID]voc video mode");

    diag_printf("[VID]file_mode=%d",file_mode);
    switch(file_mode)
    {
        case 0://file mode
#ifdef H264_SUPPORT
            h264_mem_start=0;
#endif
            break;
        case 1://whole file in bufffer mode
#ifdef H264_SUPPORT
            h264_mem_start=0;
#endif
            if(!file_handle || data_Len<=0)
            {
                res=ISOM_ERR_PARAM_ERROR;
                goto open_error;
            }
            break;
        case 2://http temp file mode
            http_fs_init(file_handle);
            break;
        case 3://av stream ringbuf mode

            if(!file_handle)
            {
                res=ISOM_ERR_PARAM_ERROR;
                goto open_error;
            }
            /*
            memset(&avStrmFrmHdV,0,sizeof(avframeinfohdstr));
            memset(&avStrmFrmHdA,0,sizeof(avframeinfohdstr));
            avStrmTimStpStartV=0xFFFFFFFF;
            avStrmTimStpStartA=0xFFFFFFFF;
            avStrmCurPosA=0;
            avStrmCurPosV=0;
            */
            avStrmBaseClkA=0;
            avStrmBaseClkV=0;
            avStrmFrmTmScaleV=0;
        default:
            break;
    }
    pIsomGlob->filehdl=file_handle;

    DTS_FLG=0xFFFFFFFF;


    /******************************************************************
                      parse  file
     ******************************************************************/
    switch(pIsomGlob->med_type)
    {
        case MCI_TYPE_MP4:
        case MCI_TYPE_3GP:
            res= gf_isom_open_file( file_handle, file_mode, data_Len, &pIsomGlob->file);
            if (res)
            {
                diag_printf( "[VID] Error opening file \n");
                res=ISOM_ERR_OPEN_PARSE_FILE_FAIL;
                goto open_error;
            }

            /******************************************************************
                               export  file
             ******************************************************************/

            if( (res=vid_export_info_from_parse(pIsomGlob->file,open_audio))!=ISOM_ERR_OK)
            {
                diag_printf("[VID] gf_media_export_info err res=%d \n",res);
                goto open_error;
            }

            break;
        case MCI_TYPE_MJPG:
            pIsomGlob->avi_p=NULL;
            pIsomGlob->avi_p=avi_open_input_file(file_handle, file_mode, data_Len, 1);
            if(pIsomGlob->avi_p==NULL)
            {
                diag_printf( "[VID]##ERROR## avi_open_input_file error %d \n",avi_get_error());
                res=ISOM_ERR_OPEN_FILE_FAIL;
                goto open_error;
            }

            if((uint32)*((uint32*)pIsomGlob->avi_p->compressor2)==AVI_FORMAT_MJPG)
            {
                diag_printf("[VID] MJPEG");
                pIsomGlob->v_type=V_TYPE_MJPEG;
            }
            else if(((uint32)*((uint32*)pIsomGlob->avi_p->compressor2)==AVI_FORMAT_XVID)
                    || ((uint32)*((uint32*)pIsomGlob->avi_p->compressor2)==AVI_FORMAT_XVID2))
            {
                diag_printf("[VID] XVID");
                pIsomGlob->v_type=V_TYPE_MPEG4;
            }
            else if((uint32)*((uint32*)pIsomGlob->avi_p->compressor2)==AVI_FORMAT_DX50)
            {
                diag_printf("[VID] DX50");
                pIsomGlob->v_type=V_TYPE_MPEG4;
            }
            else if((uint32)*((uint32*)pIsomGlob->avi_p->compressor2)==AVI_FORMAT_DIV3)
            {
                diag_printf("[VID] DIV3");
                pIsomGlob->v_type=V_TYPE_MPEG4;
            }
            else if(((uint32)*((uint32*)pIsomGlob->avi_p->compressor2)==AVI_FORMAT_DIVX)
                    || ((uint32)*((uint32*)pIsomGlob->avi_p->compressor2)==AVI_FORMAT_divx))
            {
                diag_printf("[VID] DIVX");
                pIsomGlob->v_type=V_TYPE_MPEG4;
            }
            else if((uint32)*((uint32*)pIsomGlob->avi_p->compressor2)==AVI_FORMAT_PM4V)
            {
                diag_printf("[VID] PM4V DIV4");
                pIsomGlob->v_type=V_TYPE_MPEG4;
            }
            else if((uint32)*((uint32*)pIsomGlob->avi_p->compressor2)==AVI_FORMAT_FMP4)
            {
                diag_printf("[VID] FMP4");
                pIsomGlob->v_type=V_TYPE_MPEG4;
            }
            else if((uint32)*((uint32*)pIsomGlob->avi_p->compressor2)==AVI_FORMAT_AVC1)
            {
                diag_printf("[VID] AVC1");
                pIsomGlob->v_type=V_TYPE_H264;
            }
            else if((uint32)*((uint32*)pIsomGlob->avi_p->compressor2)==0x33363248)
            {
                diag_printf("[VID] H263");
                pIsomGlob->v_type=V_TYPE_H263;
            }
            else
            {
                res=ISOM_ERR_UNSUPPORT;
                diag_printf("[VID] unsupport video type=%x \n",*(uint32*)pIsomGlob->avi_p->compressor2);
                goto open_error;
            }

            if(pIsomGlob->avi_p->a_fmt==AVI_FORMAT_AMR)
            {
                pIsomGlob->a_type=A_TYPE_AMR;
                diag_printf("[VID] AMR");
            }
            else if(pIsomGlob->avi_p->a_fmt==AVI_FORMAT_AMR_2)
            {
                //same as AMR, only FCC different.
                pIsomGlob->a_type=A_TYPE_AMR;
                diag_printf("[VID] AMR 2");
            }
            else if(pIsomGlob->avi_p->a_fmt==AVI_FORMAT_MP3)
            {
                pIsomGlob->a_type=A_TYPE_MP3;
                diag_printf("[VID] MP3");
            }
            else if(pIsomGlob->avi_p->a_fmt==AVI_FORMAT_MP2)
            {
                pIsomGlob->a_type=A_TYPE_MP3;
                diag_printf("[VID] MP2");
            }
            else if(pIsomGlob->avi_p->a_fmt==AVI_FORMAT_PCM)
            {
                pIsomGlob->a_type=A_TYPE_PCM;
                diag_printf("[VID] PCM");
            }
            else if(pIsomGlob->avi_p->a_fmt==AVI_FORMAT_ADPCM)
            {
                pIsomGlob->a_type=A_TYPE_ADPCM;
                //open_audio=0;// not support ADPCM now.
                //diag_printf("[VID] ADPCM");
                res=ISOM_ERR_UNSUPPORT;
                diag_printf("[VID] unsupport audio ADPCM now \n");
                goto open_error;
            }
            else
            {
                res=ISOM_ERR_UNSUPPORT;
                diag_printf("[VID] unsupport audio type=%x \n",pIsomGlob->avi_p->a_fmt);
                goto open_error;
            }
            pIsomGlob->a_channels=(uint16)pIsomGlob->avi_p->a_chans;
            pIsomGlob->a_sample_rate=pIsomGlob->avi_p->a_rate;
            pIsomGlob->a_bit=pIsomGlob->avi_p->a_bits;
            pIsomGlob->a_bitrate=pIsomGlob->avi_p->mp3rate;
            pIsomGlob->a_total_sample_num=pIsomGlob->avi_p->audio_chunks;
            pIsomGlob->a_total_frame_num=0;

            pIsomGlob->v_total_sample_num=pIsomGlob->avi_p->video_frames;
            pIsomGlob->v_total_frame_num=pIsomGlob->v_total_sample_num;
            pIsomGlob->v_fps=pIsomGlob->avi_p->fps;
            if(pIsomGlob->v_fps)
                pIsomGlob->total_time=pIsomGlob->v_total_sample_num*1000/pIsomGlob->v_fps;

            pIsomGlob->v_fps<<=V_FPS_SCALE_BIT;
            pIsomGlob->v_width=pIsomGlob->avi_p->width;
            pIsomGlob->v_height=pIsomGlob->avi_p->height;

            break;

#ifdef RM_SUPPORT
        case MCI_TYPE_RM:
        {
            rmInfo rm_Info;

#ifdef USE_OVERLAY
            VID_OVERLAY_ID=HAL_OVERLAY_INT_SRAM_ID_4;
            res=hal_OverlayLoad(VID_OVERLAY_ID);
            if(res!=HAL_ERR_NO)
                diag_printf( "[VID]RV HAL_OVERLAY_INT_SRAM_ID_4 hal_OverlayLoad  res=%d",res);
#endif

            res=rm_init(file_handle, &rm_Info);
            if(res != HXR_OK)
            {
                diag_printf( "[VID]##ERROR## rm_init error %d \n",res);
                res=ISOM_ERR_OPEN_FILE_FAIL;
                goto open_error;
            }

            pIsomGlob->total_time=rm_Info.rm_Duration;

            if(rm_Info.ra_Format==RA_CODEC_RAAC|| rm_Info.ra_Format==RA_CODEC_RACP)
            {
                //raac
                pIsomGlob->a_type=A_TYPE_AAC;

                //estimate the total frame number.
                if(rm_Info.ra_SampleRate)
                    pIsomGlob->a_total_sample_num=(uint32)((long long)rm_Info.rm_Duration*(long long)rm_Info.ra_SampleRate/(AAC_FRAME_SAMPLE*1000));
                else
                {
                    res=ISOM_ERR_PARAM_ERROR;
                    diag_printf("[VID] rm audio ra_SampleRate=0 ERR!\n");
                    goto open_error;
                }
                pIsomGlob->a_total_frame_num=pIsomGlob->a_total_sample_num;
                diag_printf("[VID] RM AAC\n");
            }
            else if(rm_Info.ra_Format==RA_CODEC_SIPR)
            {
                //real audio voice
                pIsomGlob->a_type=A_TYPE_RA;
                pIsomGlob->a_total_frame_num=pIsomGlob->a_total_sample_num=1;
                //res=ISOM_ERR_UNSUPPORT;
                diag_printf("[VID] unsupport rm audio type RA_INTERLEAVER_SIPR\n");
                //goto open_error;
            }
            else if(rm_Info.ra_Format==RA_CODEC_COOK )
            {
                uint32 codingDelay=0;

                //real audio 8 low bitrate (cook)
                pIsomGlob->a_type=A_TYPE_COOK;

                pIsomGlob->a_codeframeBytes=rm_Info.ra_BitsPerFrame>>3;
                if(rm_Info.ra_NumChannels)
                    pIsomGlob->a_frame_sample=rm_Info.ra_SamplePerFrame/rm_Info.ra_NumChannels;//256,512,1024

                //estimate the total frame number.
                if(rm_Info.ra_SampleRate && rm_Info.ra_SamplePerFrame)
                    pIsomGlob->a_total_sample_num=(uint32)((long long)rm_Info.rm_Duration*(long long)rm_Info.ra_SampleRate/((long long)rm_Info.ra_SamplePerFrame*1000));
                else
                {
                    res=ISOM_ERR_PARAM_ERROR;
                    diag_printf("[VID] rm audio ra_SampleRate=0 ERR!\n");
                    goto open_error;
                }
                pIsomGlob->a_total_frame_num=pIsomGlob->a_total_sample_num;

                pIsomGlob->a_Gecko2DecHdl=Gecko2InitDecoder(pIsomGlob->a_frame_sample,
                                          rm_Info.ra_NumChannels,
                                          rm_Info.ra_Regions,
                                          rm_Info.ra_BitsPerFrame,
                                          rm_Info.ra_SampleRate,
                                          rm_Info.ra_cplStart,
                                          rm_Info.ra_cplQBits,
                                          &codingDelay);
                diag_printf("[VID] RM low bitrate cook\n");
                diag_printf("[VID] cook. regions=%d bitPerFrame=%d cplStart=%d cplQBits=%d codingDelay=%d\n",
                            rm_Info.ra_Regions,rm_Info.ra_BitsPerFrame, rm_Info.ra_cplStart, rm_Info.ra_cplQBits,codingDelay);

                if(pIsomGlob->a_Gecko2DecHdl==0)
                {
                    res=ISOM_ERR_COOK_INIT_FAIL;
                    diag_printf("[VID] Gecko2InitDecoder fail!\n");
                    goto open_error;
                }
            }
            else if(rm_Info.ra_Format==RA_CODEC_ATRC)
            {
                //real audio 8 high bitrate(atrc)
                pIsomGlob->a_type=A_TYPE_RA;
                pIsomGlob->a_total_frame_num=pIsomGlob->a_total_sample_num=1;
                //res=ISOM_ERR_UNSUPPORT;
                diag_printf("[VID] unsupport rm audio type RA_INTERLEAVER_GENR \n");
                //goto open_error;
            }
            else
            {
                pIsomGlob->a_type=A_TYPE_RA;
                pIsomGlob->a_total_frame_num=pIsomGlob->a_total_sample_num=1;
                //res=ISOM_ERR_UNSUPPORT;
                diag_printf("[VID] unsupport rm audio type=%x \n", rm_Info.ra_Format);
                //goto open_error;
            }
            //pIsomGlob->a_type=A_TYPE_PCM;//for debug
            pIsomGlob->a_channels=(uint16) rm_Info.ra_NumChannels;
            pIsomGlob->a_sample_rate=rm_Info.ra_SampleRate;
            pIsomGlob->a_bit=rm_Info.ra_BitsPerSample;
            pIsomGlob->a_bitrate=0;

            if(rm_Info.rv_Format==HX_RV30VIDEO_ID || rm_Info.rv_Format==HX_RV40VIDEO_ID
                    || rm_Info.rv_Format==HX_RV89COMBO_ID)
            {
                pIsomGlob->v_type=V_TYPE_RV;
            }
            else
            {
                res=ISOM_ERR_UNSUPPORT;
                diag_printf("[VID] unsupport rm video type=%x \n", rm_Info.rv_Format);
                goto open_error;
            }
            //estimate the total frame number.
            pIsomGlob->v_total_sample_num=rm_Info.rm_Duration*rm_Info.rv_Fps/1000;
            pIsomGlob->v_total_frame_num=pIsomGlob->v_total_sample_num;
            pIsomGlob->v_fps=rm_Info.rv_Fps;
            pIsomGlob->v_fps<<=V_FPS_SCALE_BIT;
            pIsomGlob->v_width=rm_Info.rv_Width;
            pIsomGlob->v_height=rm_Info.rv_Height;
//tick1=hal_TimGetUpTime();

            res=rm_seek(pIsomGlob->total_time);//for init the seek table.
            if(res != HXR_OK)
            {
                diag_printf( "[VID]##ERROR## rm seek table init error %d \n",res);
                res=ISOM_ERR_OPEN_FILE_FAIL;
                goto open_error;
            }

//tick2=hal_TimGetUpTime();
//diag_printf("[VID]rm seek table init time=%d", (tick2-tick1)*1000/16384);
        }
        break;
#endif

        case MCI_TYPE_AVSTRM:
        {
            avinfostr *avinfohdl= (avinfostr*)file_handle;

            if(!avinfohdl->avstreamhdl_a && !avinfohdl->avstreamhdl_v)
            {
                res=ISOM_ERR_PARAM_ERROR;
                diag_printf("[VID] both audio and video stream NULL");
                goto open_error;
            }

            pIsomGlob->total_time =avinfohdl->totaltime;

            if(avinfohdl->avstreamhdl_v)
            {
                if(avinfohdl->fmt_v==V_TYPE_H263)
                {
                    diag_printf("[VID] H263");
                    pIsomGlob->v_type=V_TYPE_H263;
                }
                else if(avinfohdl->fmt_v==V_TYPE_MPEG4)
                {
                    diag_printf("[VID] MPEG4");
                    pIsomGlob->v_type=V_TYPE_MPEG4;
                }
                else if(avinfohdl->fmt_v==V_TYPE_H264)
                {
                    diag_printf("[VID] H264");
                    pIsomGlob->v_type=V_TYPE_H264;
                }
                else
                {
                    res=ISOM_ERR_UNSUPPORT;
                    diag_printf("[VID] unsupport video type=%x \n",avinfohdl->fmt_v);
                    goto open_error;
                }
                //pIsomGlob->v_fps=avinfohal->fps_v;
                //pIsomGlob->v_fps<<=V_FPS_SCALE_BIT;
                avStrmBaseClkV =avinfohdl->baseclock_v;
                avStrmFrmTmScaleV =avinfohdl->frmtimescale_v;
                diag_printf("[VID] avstrm avStrmBaseClkV %d, avStrmFrmTmScaleV %d",avStrmBaseClkV,avStrmFrmTmScaleV);
                if(!avStrmBaseClkV || !avStrmFrmTmScaleV)
                {
                    res=ISOM_ERR_PARAM_ERROR;
                    diag_printf("[VID] invalid baseclock_v or frmtimescale_v!");
                    goto open_error;
                }
                pIsomGlob->v_fps=(avStrmBaseClkV<<V_FPS_SCALE_BIT)/avStrmFrmTmScaleV;

                if(avinfohdl->totalfrm_v>0)
                    pIsomGlob->v_total_frame_num=pIsomGlob->v_total_sample_num=avinfohdl->totalfrm_v;
                else
                    pIsomGlob->v_total_frame_num=pIsomGlob->v_total_sample_num=(pIsomGlob->total_time/1000)*(avStrmBaseClkV/avStrmFrmTmScaleV);

                pIsomGlob->v_width=avinfohdl->width_v;
                pIsomGlob->v_height=avinfohdl->height_v;
            }
            else
            {
                diag_printf("[VID] no video stream!open_audio=%d",open_audio);
                if(open_audio!=0)
                    open_audio=2;
                pIsomGlob->v_fps=1<<V_FPS_SCALE_BIT;
                pIsomGlob->v_total_frame_num=0xffffffff;
            }

            if(avinfohdl->avstreamhdl_a)
            {
                if(avinfohdl->fmt_a==A_TYPE_AMR)
                {
                    diag_printf("[VID] AMR");
                    pIsomGlob->a_type=A_TYPE_AMR;
                }
                else if(avinfohdl->fmt_a==A_TYPE_AAC)
                {
                    diag_printf("[VID] AAC");
                    pIsomGlob->a_type=A_TYPE_AAC;
                }
                else if(avinfohdl->fmt_a==A_TYPE_MP3)
                {
                    diag_printf("[VID] MP3");
                    pIsomGlob->a_type=A_TYPE_MP3;
                }
                else if(avinfohdl->fmt_a==A_TYPE_DRA)
                {
                    diag_printf("[VID] DRA");
                    avinfohdl->channels_a=1;
                    avinfohdl->bits_a=16;
                    pIsomGlob->a_type=A_TYPE_DRA;
                }
                else
                {
                    res=ISOM_ERR_UNSUPPORT;
                    diag_printf("[VID] unsupport audio type=%x \n",avinfohdl->fmt_v);
                    goto open_error;
                }

                avStrmBaseClkA=avinfohdl->baseclock_a;
                diag_printf("[VID] avstrm avStrmBaseClkA %d",avStrmBaseClkA);
                if(!avStrmBaseClkA)
                {
                    res=ISOM_ERR_PARAM_ERROR;
                    diag_printf("[VID] invalid baseclock_a !");
                    goto open_error;
                }

                pIsomGlob->a_channels=(uint16)avinfohdl->channels_a;
                pIsomGlob->a_sample_rate=avinfohdl->samplerate_a;
                pIsomGlob->a_bit=avinfohdl->bits_a;
                pIsomGlob->a_bitrate=0;
                if(avinfohdl->totalfrm_a>0)
                    pIsomGlob->a_total_sample_num=avinfohdl->totalfrm_a;
                else
                    pIsomGlob->a_total_sample_num=0x7fffffff;
                pIsomGlob->a_total_frame_num=0;
            }
            else
            {
                diag_printf("[VID] no audio stream!");
                if(open_audio==2)
                {
                    res=ISOM_ERR_PARAM_ERROR;
                    diag_printf("[VID] invalid avstream!\n");
                    goto open_error;
                }
                open_audio=0;
            }
        }
        break;
        default:
            res=ISOM_ERR_UNSUPPORT;
            diag_printf("[VID] unsupport media type=%d \n",type);
            goto open_error;
    }

    switch(pIsomGlob->a_type)
    {
        case A_TYPE_AAC:
            pIsomGlob->a_vocDecType=MMC_AAC_DECODE;
            if(pIsomGlob->med_type==MCI_TYPE_MP4 || pIsomGlob->med_type==MCI_TYPE_3GP)
            {
                if(pIsomGlob->a_channels!=pIsomGlob->a_cfg.nb_chan)
                {
                    if(pIsomGlob->a_cfg.nb_chan!=0)
                        pIsomGlob->a_channels=pIsomGlob->a_cfg.nb_chan;
                    diag_printf("[VID] real channels pIsomGlob->a_cfg.nb_chan=%d !!!\n",pIsomGlob->a_cfg.nb_chan);
                }

                if(pIsomGlob->a_sample_rate!=pIsomGlob->a_cfg.base_sr)
                {
                    if(pIsomGlob->a_cfg.base_sr!=0)
                        pIsomGlob->a_sample_rate=pIsomGlob->a_cfg.base_sr;
                    diag_printf("[VID] real a_sample_rate pIsomGlob->a_cfg.sbr_sr=%d !!!\n",pIsomGlob->a_cfg.base_sr);
                }
            }

            pIsomGlob->a_frame_sample=AAC_FRAME_SAMPLE;
            pIsomGlob->a_frameBytes=(AAC_FRAME_SAMPLE*pIsomGlob->a_channels<<1);
            pIsomGlob->a_HalfPcmBufFrame=(PCM_BUF_AAC_FRAME_NUM>>1);
            pIsomGlob->a_SampBufPipeLen=MAX_A_SAMPLEBUF_PIPE_SIZE;
            pIsomGlob->a_SampBufMirrorLen=MAX_A_SAMPLEBUF_MIRROR_SIZE;

            if(!pIsomGlob->a_sample_rate)
            {
                diag_printf("[VID] err pIsomGlob->a_sample_rate=%d !!!\n",pIsomGlob->a_sample_rate);
                res=ISOM_ERR_PARAM_ERROR;
                goto open_error;
            }
            pIsomGlob->a_frame_Period=((AAC_FRAME_SAMPLE*1000)+(pIsomGlob->a_sample_rate>>1))/pIsomGlob->a_sample_rate;
            pIsomGlob->a_AacDecHdl= AACInitDecoder();
            if(!pIsomGlob->a_AacDecHdl)
            {
                diag_printf("[VID] aac decoder init err!\n");
                res=ISOM_ERR_AUDIO_DEC_FAIL;
                goto open_error;
            }

            break;
        case A_TYPE_AMR:
            pIsomGlob->a_vocDecType=MMC_AMR_DECODE;

            if(pIsomGlob->a_channels!=1 || pIsomGlob->a_sample_rate!=8000)
            {
                if(pIsomGlob->med_type==MCI_TYPE_MP4 || pIsomGlob->med_type==MCI_TYPE_3GP)
                {
                    //  if(pIsomGlob->amrSp->mode_set & 0x0080)
                    {
                        pIsomGlob->a_channels=1;
                        pIsomGlob->a_sample_rate=8000;
                        //      diag_printf("[VID] pIsomGlob->amrSp->mode_set=%x \n", pIsomGlob->amrSp->mode_set);
                    }
                    /*
                    else
                    {
                        pIsomGlob->a_channels=1;
                        pIsomGlob->a_sample_rate=8000;
                        diag_printf("[VID] WARNING! pIsomGlob->amrSp->mode_set=%x \n", pIsomGlob->amrSp->mode_set);
                        //res=ISOM_ERR_INVALID_FORMAT;
                        //goto open_error;
                    }
                    */
                }
                else
                {
                    diag_printf("[VID] warning!amr a_channels=%d a_sample_rate=%d\n", pIsomGlob->a_channels, pIsomGlob->a_sample_rate);
                    pIsomGlob->a_channels=1;
                    pIsomGlob->a_sample_rate=8000;
                }
            }

            pIsomGlob->a_frame_sample=AMR_FRAME_SAMPLE;
            pIsomGlob->a_frameBytes=(AMR_FRAME_SAMPLE*pIsomGlob->a_channels<<1);
            pIsomGlob->a_HalfPcmBufFrame=(PCM_BUF_AMR_FRAME_NUM>>1);
            pIsomGlob->a_SampBufPipeLen=MAX_A_SAMPLEBUF_PIPE_SIZE;
            pIsomGlob->a_SampBufMirrorLen=MAX_A_SAMPLEBUF_MIRROR_SIZE;

            if(!pIsomGlob->a_sample_rate)
            {
                diag_printf("[VID] err pIsomGlob->a_sample_rate=%d !!!\n",pIsomGlob->a_sample_rate);
                res=ISOM_ERR_PARAM_ERROR;
                goto open_error;
            }
            pIsomGlob->a_frame_Period=((AMR_FRAME_SAMPLE*1000)+(pIsomGlob->a_sample_rate>>1))/pIsomGlob->a_sample_rate;

            break;
        case A_TYPE_MP3:
            pIsomGlob->a_vocDecType=MMC_MP3_DECODE;

            if(pIsomGlob->a_sample_rate>=32000)
                pIsomGlob->a_frame_sample=MP3_FRAME_SAMPLE_1152;
            else
                pIsomGlob->a_frame_sample=MP3_FRAME_SAMPLE_576;
            pIsomGlob->a_frameBytes=(pIsomGlob->a_frame_sample*pIsomGlob->a_channels<<1);
            pIsomGlob->a_HalfPcmBufFrame=(PCM_BUF_MP3_FRAME_NUM>>1);
            pIsomGlob->a_SampBufPipeLen=MAX_A_SAMPLEBUF_PIPE_SIZE;
            pIsomGlob->a_SampBufMirrorLen=MAX_A_SAMPLEBUF_MIRROR_SIZE;

            if(!pIsomGlob->a_sample_rate)
            {
                diag_printf("[VID] err pIsomGlob->a_sample_rate=%d !!!\n",pIsomGlob->a_sample_rate);
                res=ISOM_ERR_PARAM_ERROR;
                goto open_error;
            }
            pIsomGlob->a_frame_Period=((pIsomGlob->a_frame_sample*1000)+(pIsomGlob->a_sample_rate>>1))/pIsomGlob->a_sample_rate;
            res= MMF_Mp3_OpenDecoder();
            if(res!=0)
            {
                diag_printf("[VID] mp3 decoder init err!\n");
                res=ISOM_ERR_AUDIO_DEC_FAIL;
                goto open_error;
            }

            break;
        case A_TYPE_PCM:
        case A_TYPE_ADPCM:
            //no voc decoder, input pcm directly.
            pIsomGlob->a_frame_sample=PCM_FRAME_SAMPLE;
            pIsomGlob->a_frameBytes=(pIsomGlob->a_frame_sample*pIsomGlob->a_channels<<1);
            pIsomGlob->a_HalfPcmBufFrame=(PCM_BUF_PCM_FRAME_NUM>>1);
            pIsomGlob->a_SampBufPipeLen=MAX_A_SAMPLEBUF_PIPE_SIZE;
            pIsomGlob->a_SampBufMirrorLen=MAX_A_SAMPLEBUF_MIRROR_SIZE;

            if(!pIsomGlob->a_sample_rate)
            {
                diag_printf("[VID] err pIsomGlob->a_sample_rate=%d !!!\n",pIsomGlob->a_sample_rate);
                res=ISOM_ERR_PARAM_ERROR;
                goto open_error;
            }
            pIsomGlob->a_frame_Period=((pIsomGlob->a_frame_sample*1000)+(pIsomGlob->a_sample_rate>>1))/pIsomGlob->a_sample_rate;

            break;
        case A_TYPE_COOK:
            //c decode. use like pcm
            //pIsomGlob->a_frame_sample get from rmvb.
            pIsomGlob->a_frameBytes=(pIsomGlob->a_frame_sample*pIsomGlob->a_channels<<1);
            pIsomGlob->a_HalfPcmBufFrame=(PCM_BUF_COOK_FRAME_NUM>>1);
            pIsomGlob->a_SampBufPipeLen=MAX_A_SAMPLEBUF_PIPE_SIZE;
            pIsomGlob->a_SampBufMirrorLen=MAX_A_SAMPLEBUF_MIRROR_SIZE;
            if(!pIsomGlob->a_sample_rate)
            {
                diag_printf("[VID] err pIsomGlob->a_sample_rate=%d !!!\n",pIsomGlob->a_sample_rate);
                res=ISOM_ERR_PARAM_ERROR;
                goto open_error;
            }
            pIsomGlob->a_frame_Period=((pIsomGlob->a_frame_sample*1000)+(pIsomGlob->a_sample_rate>>1))/pIsomGlob->a_sample_rate;

            break;
        case A_TYPE_RA:
            if(open_audio==2)
            {
                diag_printf("[VID] invalid audio! ");
                res=ISOM_ERR_UNSUPPORT;
                goto open_error;
            }

            open_audio=0;//not support ra.
            pIsomGlob->a_frame_sample=1;
            pIsomGlob->a_frameBytes=1;
            pIsomGlob->a_HalfPcmBufFrame=1;
            pIsomGlob->a_SampBufPipeLen=MAX_A_SAMPLEBUF_PIPE_SIZE;
            pIsomGlob->a_SampBufMirrorLen=MAX_A_SAMPLEBUF_MIRROR_SIZE;

            break;
#ifdef DRA_SUPPORT
        case A_TYPE_DRA:
        {
            //dra_cfg DraCfg;
            //int DraMemSize;
            pIsomGlob->a_frame_sample=DRA_FRAME_SAMPLE_1024;
            pIsomGlob->a_frameBytes=(pIsomGlob->a_frame_sample*pIsomGlob->a_channels*(pIsomGlob->a_bit>>3));
            pIsomGlob->a_HalfPcmBufFrame=(PCM_BUF_DRA_FRAME_NUM>>1);
            pIsomGlob->a_SampBufPipeLen=MAX_A_SAMPLEBUF_PIPE_SIZE;
            pIsomGlob->a_SampBufMirrorLen=MAX_A_SAMPLEBUF_MIRROR_SIZE;

            if(!pIsomGlob->a_sample_rate)
            {
                diag_printf("[VID] err pIsomGlob->a_sample_rate=%d !!!\n",pIsomGlob->a_sample_rate);
                res=ISOM_ERR_PARAM_ERROR;
                goto open_error;
            }
            pIsomGlob->a_frame_Period=((pIsomGlob->a_frame_sample*1000)+(pIsomGlob->a_sample_rate>>1))/pIsomGlob->a_sample_rate;

            memset(&DraFrameInfo,0,sizeof(dra_frame_info));
            DraDownMixMode=DXMODE_10;

            break;
        }
#endif
        default:

            //set as pcm
            pIsomGlob->a_type= A_TYPE_PCM;
            pIsomGlob->a_channels=2;
            pIsomGlob->a_sample_rate=8000;
            pIsomGlob->a_bit=16;
            pIsomGlob->a_bitrate=0;
            pIsomGlob->a_total_sample_num=1;
            pIsomGlob->a_total_frame_num=0;

            pIsomGlob->a_frame_sample=1;
            pIsomGlob->a_frameBytes=1;
            pIsomGlob->a_HalfPcmBufFrame=(PCM_BUF_PCM_FRAME_NUM>>1);
            pIsomGlob->a_SampBufPipeLen=MAX_A_SAMPLEBUF_PIPE_SIZE;
            pIsomGlob->a_SampBufMirrorLen=MAX_A_SAMPLEBUF_MIRROR_SIZE;
            pIsomGlob->a_frame_Period= 0;

            if(open_audio==2)
            {
                diag_printf("[VID] invalid audio! ");
                res=ISOM_ERR_UNSUPPORT;
                goto open_error;
            }

            open_audio= 0;
            diag_printf("[VID]WARNING invalid audio, be silent. \n");
            break;
    }

    if(!pIsomGlob->v_fps )
    {
        diag_printf("[VID] err pIsomGlob->v_fps=%d !!!\n",pIsomGlob->v_fps);
        res=ISOM_ERR_PARAM_ERROR;
        goto open_error;
    }

    if(pIsomGlob->med_type==MCI_TYPE_AVSTRM )
    {
        //record by pcm sample num or baseclock_v(without audio).
        if(open_audio==1)
            pIsomGlob->v_period=(avStrmFrmTmScaleV*pIsomGlob->a_sample_rate)/avStrmBaseClkV;
        else
            pIsomGlob->v_period=avStrmFrmTmScaleV;

        pIsomGlob->v_timerPeriod=(avStrmFrmTmScaleV*16384)/avStrmBaseClkV;
    }
    else
    {
        pIsomGlob->v_period=(uint32)((1000<<V_FPS_SCALE_BIT)+(pIsomGlob->v_fps>>1))/pIsomGlob->v_fps;
        if(!pIsomGlob->v_period)
        {
            diag_printf("[VID] err pIsomGlob->v_period=%d !!!\n",pIsomGlob->v_period);
            res=ISOM_ERR_PARAM_ERROR;
            goto open_error;
        }
        pIsomGlob->v_timerPeriod=(16384<<V_FPS_SCALE_BIT)/pIsomGlob->v_fps;
    }

    if(pIsomGlob->med_type!=MCI_TYPE_RM)
        pIsomGlob->a_sync_distance_frame_num=SYNC_TIME_OFFSET*pIsomGlob->a_sample_rate/(1000*pIsomGlob->a_frame_sample);//one sync / 500ms
    else//reuse a_sync_distance_frame_num to record pcm sample distance for rmvb
        pIsomGlob->a_sync_distance_frame_num=SYNC_TIME_OFFSET*pIsomGlob->a_sample_rate/1000;

    /******************
    pcm
    time=pIsomGlob->a_current_frame_num*(1000*pIsomGlob->a_frame_sample/pIsomGlob->a_sample_rate);
    video fram num=time/(1000/(pIsomGlob->v_fps>>V_FPS_SCALE_BIT));
                  =pIsomGlob->a_current_frame_num*(1000*pIsomGlob->a_frame_sample/pIsomGlob->a_sample_rate)/(1000/(pIsomGlob->v_fps>>V_FPS_SCALE_BIT))
                  =pIsomGlob->a_current_frame_num*pIsomGlob->a_frame_sample*(pIsomGlob->v_fps>>V_FPS_SCALE_BIT)/pIsomGlob->a_sample_rate;
                  =pIsomGlob->a_current_frame_num*SYNC_NUM_COEFFICIENT>>(V_FPS_SCALE_BIT+ SYNC_COEFF_SCALE_BIT)
    *******************/
    SYNC_NUM_COEFFICIENT=((pIsomGlob->a_frame_sample*pIsomGlob->v_fps<<SYNC_COEFF_SCALE_BIT)+(pIsomGlob->a_sample_rate>>1))/pIsomGlob->a_sample_rate;

    //Lily_Lcd_GetSize(&pIsomGlob->v_lcd_size_x, &pIsomGlob->v_lcd_size_y);
    pIsomGlob->v_lcd_size_x=MMC_LcdWidth;
    pIsomGlob->v_lcd_size_y=MMC_LcdHeight;
    pIsomGlob->v_display_width=pIsomGlob->v_width;//be resize
    pIsomGlob->v_display_height=pIsomGlob->v_height;

    //limit condition
    //if(open_audio!=2)
    {

        if( (UINT32)pIsomGlob->v_width * (UINT32)pIsomGlob->v_height > VDO_RSLUT_LIMIT_SIZE &&
                (UINT32)pIsomGlob->v_width * (UINT32)pIsomGlob->v_height <= VDO_RSLUT_LIMIT_SIZE_LOWRES &&
                (pIsomGlob->v_type==V_TYPE_MPEG4 || pIsomGlob->v_type==V_TYPE_H263))
        {
            diag_printf("[VID]enable lowres1,ori w=%d h=%d",pIsomGlob->v_width,pIsomGlob->v_height);
            pIsomGlob->v_lowres=1;
            pIsomGlob->v_width >>=1;
            pIsomGlob->v_height >>=1;
        }

        if( (UINT32)pIsomGlob->v_width * (UINT32)pIsomGlob->v_height <= VDO_RSLUT_LIMIT_SIZE)
        {
            //USE_SLEEP=0;
            //if(pIsomGlob->v_width>LIMIT_WIDTH_320 || pIsomGlob->v_height>LIMIT_HEIGHT_240)
            //  USE_SLEEP=1;

            /*default display size*/
            pIsomGlob->v_display_width=pIsomGlob->v_lcd_size_x;//be resize
            pIsomGlob->v_display_height=(pIsomGlob->v_lcd_size_x*3)>>2;//width : height= 4 : 3
            pIsomGlob->v_cutX=0;
            pIsomGlob->v_cutY=0;
            pIsomGlob->v_cutW=pIsomGlob->v_display_width;
            pIsomGlob->v_cutH=pIsomGlob->v_display_height;
            pIsomGlob->v_rotate=Rotate_with_0;

#if (CSW_EXTENDED_API_LCD == 1)
            lcdDraw.fb.buffer = NULL;
            lcdDraw.fb.width = pIsomGlob->v_display_width;
            lcdDraw.fb.height = pIsomGlob->v_display_height;
            lcdDraw.fb.colorFormat = LCDD_COLOR_FORMAT_RGB_565;
            lcdDraw.roi.x =0;
            lcdDraw.roi.y =0;
            lcdDraw.roi.width =pIsomGlob->v_display_width;
            lcdDraw.roi.height =pIsomGlob->v_display_height;

#if(CHIP_HAS_GOUDA == 1)
            Vid_Video.rotation = 0;//0=not rotate, 1= 90 clockwise
            Vid_Video.alpha = 255;
            Vid_Video.cKeyEn = FALSE;
            Vid_Video.cKeyColor = 0;
            Vid_Video.cKeyMask = HAL_GOUDA_CKEY_MASK_OFF;
            Vid_Video.depth = HAL_GOUDA_VID_LAYER_OVER_ALL;
            Vid_Video.fmt = HAL_GOUDA_IMG_FORMAT_IYUV;
            Vid_Video.width = (short)pIsomGlob->v_width;
            Vid_Video.height = (short)pIsomGlob->v_height;
	    Vid_Video.pos.tlPX = 0;
	    Vid_Video.pos.tlPY = 0;
	    Vid_Video.pos.brPX =pIsomGlob->v_display_width -1;
	    Vid_Video.pos.brPY =pIsomGlob->v_display_height -1;
#endif
#endif
        }
        else
        {
            diag_printf("[VID] ##WARNING## image size unsupport! \n");
            res=ISOM_ERR_VIDEO_UNSUPPORT_IMAGE_SIZE;
            goto open_error;
        }

#if   ( defined (CHIP_DIE_8955))
	pIsomGlob->v_rotate_way=0;//default rotate by gouda
#else
	pIsomGlob->v_rotate_way=1;//default rotate by soft.
#endif
	diag_printf("[VID]rotate way =%d",pIsomGlob->v_rotate_way);

	if(pIsomGlob->v_rotate_way==1){
		//rotate by soft and reuse gdi buffer for rotate.
		pVid_ext_buf=NULL;
		if((pIsomGlob->v_lcd_size_x*pIsomGlob->v_lcd_size_y) >= VDO_RSLUT_QVGA)
			pIsomGlob->v_ExtraRotateBuf= gdi_layer_using_for_videoplayer(1);//get external addr(mmi).
		if(!pIsomGlob->v_ExtraRotateBuf)
			pIsomGlob->v_ExtraRotateBuf=(unsigned char*)mmc_MemMalloc(VDO_RSLUT_QVGA*3/2);
		if(pIsomGlob->v_ExtraRotateBuf)
		{
			memset(pIsomGlob->v_ExtraRotateBuf, 0, VDO_RSLUT_QVGA*3/2);
		}else{
			res=ISOM_ERR_MALLOC_FAIL;
			goto open_error;
		}
	}else{
		//rotate by lcd and reuse gdi buffer for decode
		pVid_ext_buf= gdi_layer_using_for_videoplayer(1);//get external addr(mmi).
		if(pVid_ext_buf!=NULL)
		{
			//8bytes algin for gouda y addr.
			pVid_ext_buf+=8;
			pVid_ext_buf=(int8*)(((uint32)pVid_ext_buf>>3)<<3);
		}

		if((UINT32)pIsomGlob->v_width * (UINT32)pIsomGlob->v_height > VDO_RSLUT_QVGA){
			pVid_ext_buf=NULL;
		}
	}

    }

    if(pIsomGlob->v_width*pIsomGlob->v_height> 176*144 )
    {
        if((pIsomGlob->v_fps>>V_FPS_SCALE_BIT)>20)
            MAX_KEEP_SKIP_F_NUM=3;
        else if((pIsomGlob->v_fps>>V_FPS_SCALE_BIT)>15)
            MAX_KEEP_SKIP_F_NUM=2;
        else
            MAX_KEEP_SKIP_F_NUM=1;
    }
    else
    {
        MAX_KEEP_SKIP_F_NUM=1;
    }

    if(pIsomGlob->v_lowres==0)//no Y buf reuse 
        pIsomGlob->v_SampleBufLen=MAX_V_SAMPLEBUF_SIZE;
    else
        pIsomGlob->v_SampleBufLen=MAX_V_SAMPLEBUF_SIZE2;


    /*       Unsupport File:
     *       width*height > 320*240 && fps > 15
     *       width*height <= 320*240 && fps > 25
     * */
    if(pIsomGlob->v_width*pIsomGlob->v_height > VDO_RSLUT_QVGA)
    {
        if((pIsomGlob->v_fps>>V_FPS_SCALE_BIT) > 15)
        {
            diag_printf("[VID] ##WARNING## width*height > 320*240 fps=%d unsupport! \n", pIsomGlob->v_fps>>V_FPS_SCALE_BIT);
            res=ISOM_ERR_VIDEO_UNSUPPORT_FPS;
            goto open_error;
        }
    }
    else
    {
        if((pIsomGlob->v_fps>>V_FPS_SCALE_BIT) > 25)
        {
            diag_printf("[VID] ##WARNING## width*height <= 320*240 fps=%d unsupport! \n", pIsomGlob->v_fps>>V_FPS_SCALE_BIT);
            res=ISOM_ERR_VIDEO_UNSUPPORT_FPS;
            goto open_error;
        }
    }


    diag_printf("[VID] total_time=%d \n",pIsomGlob->total_time);
    diag_printf("[VID] v_type=%x \n",pIsomGlob->v_type);
    diag_printf("[VID] v_trackID=%d \n",pIsomGlob->v_trackID);
    diag_printf("[VID] v_total_sample_num=%d \n",pIsomGlob->v_total_sample_num);
    diag_printf("[VID] v_total_frame_num=%d \n",pIsomGlob->v_total_frame_num);
    diag_printf("[VID] v_total_keyframe_num=%d \n",pIsomGlob->v_total_keyframe_num);
    diag_printf("[VID] v_fps=%d \n",pIsomGlob->v_fps>>V_FPS_SCALE_BIT);
    diag_printf("[VID] v_period=%d \n",pIsomGlob->v_period);
    diag_printf("[VID] v_timerPeriod=%d \n",pIsomGlob->v_timerPeriod);
    diag_printf("[VID] DTS_FLG=%d \n", DTS_FLG);
    diag_printf("[VID] v_timeScale=%d \n",pIsomGlob->v_timeScale);
    diag_printf("[VID] v_lowres=%d \n",pIsomGlob->v_lowres);
    diag_printf("[VID] v_width=%d \n",pIsomGlob->v_width);
    diag_printf("[VID] v_height=%d \n",pIsomGlob->v_height);
    diag_printf("[VID] v_display_width=%d \n",pIsomGlob->v_display_width);
    diag_printf("[VID] v_display_height=%d \n",pIsomGlob->v_display_height);
    diag_printf("[VID] v_lcd_size_x=%d \n",pIsomGlob->v_lcd_size_x);
    diag_printf("[VID] v_lcd_size_y=%d \n",pIsomGlob->v_lcd_size_y);

    diag_printf("[VID] a_type=%x \n",pIsomGlob->a_type);
    diag_printf("[VID] a_trackID=%d \n",pIsomGlob->a_trackID);
    diag_printf("[VID] a_total_sample_num=%d \n",pIsomGlob->a_total_sample_num);
    diag_printf("[VID] a_total_frame_num=%d \n",pIsomGlob->a_total_frame_num);
    diag_printf("[VID] a_bitrate=%d \n",pIsomGlob->a_bitrate);
    diag_printf("[VID] a_sample_rate=%d \n",pIsomGlob->a_sample_rate);
    diag_printf("[VID] a_frame_Period=%d \n",pIsomGlob->a_frame_Period);
    diag_printf("[VID] a_bit=%d \n",pIsomGlob->a_bit);
    diag_printf("[VID] a_channels=%d \n",pIsomGlob->a_channels);
    diag_printf("[VID] a_frame_sample=%d \n",pIsomGlob->a_frame_sample);

    //malloc

    //initResample(pIsomGlob->v_lcd_size_x);//for resample, some row memory.
    //InitColorDither();//for yuv2rgb lookup table

    //if(open_audio!=2)
    {

	    pVid_SampleBuf[0]=(unsigned char*)mmc_MemMalloc(pIsomGlob->v_SampleBufLen);
	    if(pVid_SampleBuf[0])
	    {
		    memset(pVid_SampleBuf[0], 0, pIsomGlob->v_SampleBufLen);
	    }
	    else
	    {
		    res=ISOM_ERR_MALLOC_FAIL;
		    goto open_error;
	    }
	    if(pIsomGlob->v_lowres==1)
	    {
		    pVid_SampleBuf[1]=(unsigned char*)mmc_MemMalloc(pIsomGlob->v_SampleBufLen);
		    if(pVid_SampleBuf[1])
		    {
			    memset(pVid_SampleBuf[1], 0, pIsomGlob->v_SampleBufLen);
		    }
		    else
		    {
			    res=ISOM_ERR_MALLOC_FAIL;
			    goto open_error;
		    }
	    }
	    pIsomGlob->v_SampleBuf= pVid_SampleBuf[0];
	    vid_SampleBuf_id=0;
	    vid_SampleBuf_BytesRemain=0;
	    vid_SampleBuf_BytesUsed=0;
    }

    if(pIsomGlob->med_type!=MCI_TYPE_AVSTRM)
    {
        if(pIsomGlob->a_SampBufPipeLen+(pIsomGlob->a_SampBufMirrorLen<<1)>mmc_MemGetFree()-32)
        {
            res=ISOM_ERR_MEM_INSUFFICIENT;
            goto open_error;
        }
        pIsomGlob->a_SampBuf=(uint8*)mmc_MemMalloc(pIsomGlob->a_SampBufPipeLen+(pIsomGlob->a_SampBufMirrorLen<<1));
        if(pIsomGlob->a_SampBuf)
        {
            memset(pIsomGlob->a_SampBuf, 0, pIsomGlob->a_SampBufPipeLen+(pIsomGlob->a_SampBufMirrorLen<<1));
            pIsomGlob->a_SampStart=pIsomGlob->a_SampEnd=pIsomGlob->a_SampBuf+pIsomGlob->a_SampBufMirrorLen;
        }
        else
        {
            res=ISOM_ERR_MALLOC_FAIL;
            goto open_error;
        }

        if(pIsomGlob->a_SampBufMirrorLen>mmc_MemGetFree()-32)
        {
            res=ISOM_ERR_MEM_INSUFFICIENT;
            goto open_error;
        }
        pIsomGlob->a_SampTmpBuf=(uint8*)mmc_MemMalloc(pIsomGlob->a_SampBufMirrorLen);
        if(pIsomGlob->a_SampTmpBuf)
        {
            memset(pIsomGlob->a_SampTmpBuf, 0, pIsomGlob->a_SampBufMirrorLen);

            pIsomGlob->a_SampTmpBufp=pIsomGlob->a_SampTmpBuf;
        }
        else
        {
            res=ISOM_ERR_MALLOC_FAIL;
            goto open_error;
        }
    }

    //one more pIsomGlob->a_frameBytes for avoid overflow
    if(pIsomGlob->a_frameBytes*(pIsomGlob->a_HalfPcmBufFrame<<1)+pIsomGlob->a_frameBytes>mmc_MemGetFree()-32)
    {
        res=ISOM_ERR_MEM_INSUFFICIENT;
        goto open_error;
    }
    //malloc enough memory avoid the fake parameters
    if(pIsomGlob->a_channels==1)
    {
        if(pIsomGlob->a_type==A_TYPE_MP3 && pIsomGlob->a_frame_sample==MP3_FRAME_SAMPLE_576)
        {
            res=(pIsomGlob->a_frameBytes<<2)*pIsomGlob->a_HalfPcmBufFrame;
            pIsomGlob->a_PcmBuf=(uint8 *)mmc_MemMalloc(res<<1);
            pIsomGlob->a_ExtraPcmBuf=(uint8 *)mmc_MemMalloc(res*EXTRA_PCM_BUF_NUM);
        }
        else
        {
            res=(pIsomGlob->a_frameBytes<<1)*(pIsomGlob->a_HalfPcmBufFrame);
            pIsomGlob->a_PcmBuf=(uint8 *)mmc_MemMalloc(res<<1);
            pIsomGlob->a_ExtraPcmBuf=(uint8 *)mmc_MemMalloc(res*EXTRA_PCM_BUF_NUM);
        }
    }
    else if(pIsomGlob->a_channels==2)
    {
        if(pIsomGlob->a_type==A_TYPE_MP3 && pIsomGlob->a_frame_sample==MP3_FRAME_SAMPLE_576)
        {
            res=(pIsomGlob->a_frameBytes<<1)*(pIsomGlob->a_HalfPcmBufFrame);
            pIsomGlob->a_PcmBuf=(uint8 *)mmc_MemMalloc(res<<1);
            pIsomGlob->a_ExtraPcmBuf=(uint8 *)mmc_MemMalloc(res*EXTRA_PCM_BUF_NUM);
        }
        else
        {
            res=pIsomGlob->a_frameBytes*(pIsomGlob->a_HalfPcmBufFrame);
            pIsomGlob->a_PcmBuf=(uint8 *)mmc_MemMalloc(res<<1);
            pIsomGlob->a_ExtraPcmBuf=(uint8 *)mmc_MemMalloc(res*EXTRA_PCM_BUF_NUM);
        }
    }

    //diag_printf("pIsomGlob->a_PcmBuf=%x %x res=%d",pIsomGlob->a_PcmBuf,pIsomGlob->a_ExtraPcmBuf, res);

    if(pIsomGlob->a_PcmBuf && pIsomGlob->a_ExtraPcmBuf)
    {
        memset(pIsomGlob->a_PcmBuf, 0, pIsomGlob->a_frameBytes*(pIsomGlob->a_HalfPcmBufFrame<<1));
        pIsomGlob->a_PcmBufHalfPos=pIsomGlob->a_PcmBuf+pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame;
    }
    else
    {
        res=ISOM_ERR_MALLOC_FAIL;
        goto open_error;
    }

    //init extra buf
    for(i=0; i<EXTRA_PCM_BUF_NUM; i++)
    {
        aud_extra_pcmbuf_state[i].done=0;
        aud_extra_pcmbuf_state[i].pBufPos=pIsomGlob->a_ExtraPcmBuf+res*i;
        diag_printf("pBufPos=%x ",aud_extra_pcmbuf_state[i].pBufPos);
    }
    A_EXBF_RD_ID=0;
    A_EXBF_WT_ID=0;

    if(pIsomGlob->v_type==V_TYPE_MJPEG)
    {
        if(!pVid_ext_buf)
        {
            if(((pIsomGlob->v_width*pIsomGlob->v_height*3)>>1)>mmc_MemGetFree()-32)//for 2 yuv buffer
            {
                res=ISOM_ERR_MEM_INSUFFICIENT;
                goto open_error;
            }

            pVid_buf_Y[0]=(int8*)mmc_MemMalloc(((pIsomGlob->v_width*pIsomGlob->v_height*3)>>1)+8);

            //8bytes algin for gouda y addr.
            pVid_buf_Y[0]+=8;
            pVid_buf_Y[0]=(int8*)(((uint32)pVid_buf_Y[0]>>3)<<3);
        }
        else
        {
            pVid_buf_Y[0]=pVid_ext_buf;//reuse external addr.
        }

        if(((pIsomGlob->v_width*pIsomGlob->v_height*3)>>1)>mmc_MemGetFree()-32)//for 2 yuv buffer
        {
            res=ISOM_ERR_MEM_INSUFFICIENT;
            goto open_error;
        }

        pVid_buf_Y[1]=(int8*)mmc_MemMalloc(((pIsomGlob->v_width*pIsomGlob->v_height*3)>>1)+8);

        if(pVid_buf_Y[0] && pVid_buf_Y[1])
        {
            //8bytes algin for gouda y addr.
            pVid_buf_Y[1]+=8;
            pVid_buf_Y[1]=(int8*)(((uint32)pVid_buf_Y[1]>>3)<<3);

            memset(pVid_buf_Y[0], 0, (pIsomGlob->v_width*pIsomGlob->v_height*3)>>1);
            memset(pVid_buf_Y[1], 0, (pIsomGlob->v_width*pIsomGlob->v_height*3)>>1);
            pVid_buf_U[0]=pVid_buf_Y[0]+pIsomGlob->v_width*pIsomGlob->v_height;
            pVid_buf_V[0]=pVid_buf_U[0]+((pIsomGlob->v_width*pIsomGlob->v_height)>>2);
            pVid_buf_U[1]=pVid_buf_Y[1]+pIsomGlob->v_width*pIsomGlob->v_height;
            pVid_buf_V[1]=pVid_buf_U[1]+((pIsomGlob->v_width*pIsomGlob->v_height)>>2);
            vid_buf_id=0;
        }
        else
        {
            res=ISOM_ERR_MALLOC_FAIL;
            goto open_error;
        }
    }

    //if(open_audio!=2)
    {
#ifdef VID_VOC_FREQ_SET
        //get current voc frequency and set to 250M.
        //NOTE!!! 312M voc will make decoder out image error!!!
        if(file_mode!=3)
        {
            VOC_FREQ_STATE = hal_SysSetVocClock(HAL_SYS_VOC_FREQ_250M);
            if(VOC_FREQ_STATE >= HAL_SYS_VOC_FREQ_250M)
            {
                diag_printf("[VID]VOC freq = %d",VOC_FREQ_STATE);
                hal_SysSetVocClock(VOC_FREQ_STATE);
                VOC_FREQ_STATE =0;
            }
            else
            {
                diag_printf("[VID]VOC old freq = %d and set to 250M!",VOC_FREQ_STATE);
            }
        }
#endif

#ifdef VID_XCPU_FREQ_SET
        XCPU_FREQ_STATE= hal_SysGetFreq();
        if(XCPU_FREQ_STATE< HAL_SYS_VOC_FREQ_250M)
        {
            diag_printf("[VID]XCPU sys old freq = %d ",XCPU_FREQ_STATE);

            hal_SwRequestClk(FOURCC_VIDEO, HAL_SYS_VOC_FREQ_250M);
            XCPU_FREQ_STATE=    hal_SysGetFreq();
            diag_printf("[VID]XCPU sys new freq = %d ",XCPU_FREQ_STATE);
        }
        else
        {
            diag_printf("[VID]XCPU sys freq = %d",XCPU_FREQ_STATE);
            XCPU_FREQ_STATE=0;
        }
#endif
    }

    if(pIsomGlob->dec_mode==1 )
    {
        //voc only decode video(mpeg4/h263/h264/rm), xcpu decode audio.
        if((res=vpp_AudioJpegDecOpen(vid_voc_isr))!=HAL_ERR_NO)// will make a ISR.
        {
            diag_printf("[VID]##WARNING## vpp_AudioJpegDecOpen err res=%d \n" ,res);
            res=ISOM_ERR_VOC_OPEN_FAIL;
            goto open_error;
        }
        diag_printf("[VID]open VOC video and C audio decoder." ,res);

        switch(pIsomGlob->v_type)
        {

#ifdef RM_SUPPORT
            case V_TYPE_RV:
                vpp_AudioJpegSetReuseCode(1);//rmvb=1
                //vpp_AudioJpegDecSetCurMode(-1, -1);
                //vpp_AudioJpegDecSetInputMode(MMC_RM_DECODE);
                vpp_AVDecSetMode(MMC_RM_DECODE, 0, 0);
                break;
#endif
#ifdef H264_SUPPORT
            case V_TYPE_H264:
                vpp_AudioJpegSetReuseCode(2);//h264=2
                //vpp_AudioJpegDecSetCurMode(-1, -1);
                //vpp_AudioJpegDecSetInputMode(MMC_H264_DECODE);
                vpp_AVDecSetMode(MMC_H264_DECODE, 0, 0);
                break;
#endif
            case V_TYPE_MPEG4:
                vpp_AudioJpegSetReuseCode(3);//mpeg4=3
                vpp_AVDecSetMode(MMC_MPEG4_DECODE, 0, 0);
                break;
            case V_TYPE_H263:
                vpp_AudioJpegSetReuseCode(4);//h263=4
                vpp_AVDecSetMode(MMC_H263_DECODE, 0, 0);
                break;
            case V_TYPE_MJPEG:
                vpp_AudioJpegSetReuseCode(5);//jpeg=5
                vpp_AVDecSetMode(MMC_JPEG_DECODE, 0, 0);
                break;
            default:
                break;
        }
    }
    else if(pIsomGlob->med_type!=MCI_TYPE_AVSTRM)
    {
#if(CHIP_HAS_GOUDA == 1)
        if(pIsomGlob->a_type != A_TYPE_AMR)
        {
#endif
            if((res=vpp_AudioJpegDecOpen(vid_voc_isr))!=HAL_ERR_NO)// will make a ISR.
            {
                diag_printf("[VID]##WARNING## vpp_AudioJpegDecOpen err res=%d \n" ,res);
                res=ISOM_ERR_VOC_OPEN_FAIL;
                goto open_error;
            }

            vocDecAud.reset=1;
            vocDecAud.mode=pIsomGlob->a_vocDecType;
            vocDecAud.EQ_Type=-1;
            vocDecAud.BsbcEnable = 0;

#if(CHIP_HAS_GOUDA == 1)
            G_VidSBCConstY = (INT32*)mmc_MemMalloc(468*4);
            if( G_VidSBCConstY==NULL)
            {
                //mmc_MemFreeAll();
                g_VidBT8kPcmBuffer = NULL;
                return ISOM_ERR_MALLOC_FAIL;
            }
#endif
            if( is_SSHdl_valid(my_ss_handle)&& (my_audio_config_handle->codec_type == 1))      //BT_A2DP_sbc
            {
#if(CHIP_HAS_GOUDA == 1)
                vocDecAud.BsbcEnable = 1;
#endif
            }

#if(CHIP_HAS_GOUDA == 1)
        }
        else
        {
            VPP_SPEECH_AUDIO_CFG_T vppCfg;
            HAL_SPEECH_DEC_IN_T *pVocDecAmr=NULL;

            if ((res = vpp_SpeechOpen(vid_voc_isr, VPP_SPEECH_WAKEUP_SW_DEC)) == HAL_ERR_RESOURCE_BUSY)
            {
                diag_printf("[VID]##WARNING## vpp_SpeechOpen err res=%d \n" ,res);
                res=ISOM_ERR_VOC_OPEN_FAIL;
                goto open_error;
            }

            // VPP audio config
            vppCfg.echoEsOn = 0;
            vppCfg.echoEsVad = 0;
            vppCfg.echoEsDtd = 0;
            vppCfg.echoExpRel = 0;
            vppCfg.echoExpMu = 0;
            vppCfg.echoExpMin = 0;
            vppCfg.encMute = VPP_SPEECH_MUTE;
            vppCfg.decMute = VPP_SPEECH_UNMUTE;
            vppCfg.sdf = NULL;
            vppCfg.mdf = NULL;
            vppCfg.if1 = 1;// amr ring full mode must set 1.

            vpp_SpeechAudioCfg(&vppCfg);
            vpp_SpeechSetOutAlgGain(0);

            pVocDecAmr=vpp_SpeechGetRxCodBuffer();
            if(pVocDecAmr==NULL)
            {
                diag_printf("[VID]##ERROR## vpp_SpeechGetRxCodBuffer NULL!\n");
                res=ISOM_ERR_VOC_OPEN_FAIL;
                goto open_error;
            }

            pVocDecAmr->dtxOn = 0;
            pVocDecAmr->codecMode= HAL_INVALID;
            pVocDecAmr->decFrameType = 0;
            pVocDecAmr->bfi = 0;
            pVocDecAmr->sid = 0;
            pVocDecAmr->taf = 0;
            pVocDecAmr->ufi = 0;
            pVocDecAmr->reserved= 0;
            memset(pVocDecAmr->decInBuf, 0, HAL_SPEECH_FRAME_SIZE_COD_BUF);
        }
#endif
    }

    VOC_OPEN=1;

    fillAudioData=NULL;//not read audio in video decoder.

    //if(open_audio!=2)
    {
        switch(pIsomGlob->v_type)
        {
#ifdef MPEG4_SUPPORT
            case V_TYPE_MPEG4:
#ifdef USE_OVERLAY
                VID_OVERLAY_ID=HAL_OVERLAY_INT_SRAM_ID_1;
                res=hal_OverlayLoad(VID_OVERLAY_ID);
                if(res!=HAL_ERR_NO)
                    diag_printf( "[VID]MPEG4 HAL_OVERLAY_INT_SRAM_ID_1 hal_OverlayLoad  res=%d",res);
#endif

                res = dec_init(0, 0,pIsomGlob->v_lowres);

                if (res)
                {
                    res=ISOM_ERR_VIDEO_INIT_FAIL;
                    goto open_error;
                }

                if(pIsomGlob->v_decSpeInfo || pIsomGlob->med_type ==MCI_TYPE_MJPG ||pIsomGlob->med_type ==MCI_TYPE_AVSTRM)
                {
                    if(pIsomGlob->med_type ==MCI_TYPE_AVSTRM)
                    {
                        avinfostr *avinfohdl= (avinfostr*)pIsomGlob->filehdl;
                        unsigned char *pData= (unsigned char*)avstream_clt_getDataPos(avinfohdl->avstreamhdl_v) +sizeof(avframeinfohdstr);
                        int datalen= avstream_clt_getDataLen(avinfohdl->avstreamhdl_v)- sizeof(avframeinfohdstr);
                        if(pIsomGlob->v_SampleBufLen >= datalen)
                        {
                            memcpy(pIsomGlob->v_SampleBuf,pData,datalen);
                        }
                        else
                        {
                            memcpy(pIsomGlob->v_SampleBuf,pData,pIsomGlob->v_SampleBufLen);
                        }
                        pIsomGlob->v_SampleBuf_BytesRemain= pIsomGlob->v_SampleBufLen;
                    }
                    else if(pIsomGlob->med_type !=MCI_TYPE_MJPG)
                    {
                        if(pIsomGlob->v_decSpeInfo->dataLength<=pIsomGlob->v_SampleBufLen)
                        {
                            memcpy(pIsomGlob->v_SampleBuf, pIsomGlob->v_decSpeInfo->data, pIsomGlob->v_decSpeInfo->dataLength);
                            pIsomGlob->v_SampleBuf_BytesRemain+=pIsomGlob->v_decSpeInfo->dataLength;
                        }
                        else
                        {
                            diag_printf( "[VID]MPEG4 header=%d > buflen=%d ERR!",pIsomGlob->v_decSpeInfo->dataLength,
                                         pIsomGlob->v_SampleBufLen);
                            res=ISOM_ERR_MEM_INSUFFICIENT;
                            goto open_error;
                        }
                    }
                    else//mpeg4 in AVI
                    {
                        int32 key_frame;
                        pIsomGlob->v_current_sample_num=0;
                        pIsomGlob->v_SampleBuf_BytesRemain=0;
                        pIsomGlob->v_SampleBuf_BytesUsed=0;
                        do
                        {
                            res=avi_set_video_position(pIsomGlob->avi_p, pIsomGlob->v_current_sample_num);
                            if(res!=0)
                            {
                                diag_printf("[VID] avi_set_video_position err!");
                                res= ISOM_ERR_VIDEO_READ_FAIL;
                                goto open_error;
                            }
                            res=avi_read_frame(pIsomGlob->avi_p, pIsomGlob->v_SampleBuf, pIsomGlob->v_SampleBufLen,  &key_frame);
                            if( res < 0 )
                            {
                                diag_printf( "[VID]MJPG avi video frame read err res=%d \n", res );
                                res= ISOM_ERR_VIDEO_READ_FAIL;
                                goto open_error;
                            }
                            if(res==0)
                                pIsomGlob->v_current_sample_num+=1;
                            pIsomGlob->v_SampleBuf_BytesRemain+=res;
                        }
                        while(res==0 && pIsomGlob->v_current_sample_num < pIsomGlob->v_total_sample_num);
                    }

                    if(pIsomGlob->v_SampleBuf_BytesRemain>0)
                    {
                        int32 used_bytes;
                        xvid_dec_stats_t xvid_dec_stats;
                        vocWorkState.pVOC_WORK=(UINT8*)&VOC_WORK;
                        vocWorkState.pVOC_VID=(int16*)&VOC_VID;
                        vocWorkState.vid_display= NULL;
                        vocWorkState.vid_voc_isr=NULL;
                        vocWorkState.aud_nframe_dec=NULL;
                        vocWorkState.vid_frame_read=NULL;

                        used_bytes = dec_main(pIsomGlob->v_SampleBuf+pIsomGlob->v_SampleBuf_BytesUsed,
                                              (uint8*)pIsomGlob->v_YuvInfo,
                                              pIsomGlob->v_SampleBuf_BytesRemain, &xvid_dec_stats, NULL,
                                              ((pIsomGlob->med_type ==MCI_TYPE_MJPG) ? TRUE : FALSE));
                        diag_printf("[VID] dec_main head used_bytes=%d xvid_dec_stats.type=%d\n",used_bytes,xvid_dec_stats.type);
                        if(used_bytes == -1)//profile unfit
                        {
                            res= ISOM_ERR_VIDEO_DEC_FAIL;
                            goto open_error;
                        }
                        /* Resize image buffer if needed */
                        if(xvid_dec_stats.type == XVID_TYPE_VOL)
                        {
                            /* Check if old buffer is smaller */
                            if(pIsomGlob->v_width*pIsomGlob->v_height>=( xvid_dec_stats.data.vol.width>> pIsomGlob->v_lowres)*(xvid_dec_stats.data.vol.height>> pIsomGlob->v_lowres))
                            {
                                if(pIsomGlob->v_width!=(xvid_dec_stats.data.vol.width>> pIsomGlob->v_lowres))
                                {
                                    pIsomGlob->v_width=(xvid_dec_stats.data.vol.width >>pIsomGlob->v_lowres);
                                    pIsomGlob->v_height=(xvid_dec_stats.data.vol.height>>pIsomGlob->v_lowres);
                                    diag_printf("[VID] ##WARNING## real decode width=%d height=%d \n",xvid_dec_stats.data.vol.width, xvid_dec_stats.data.vol.height);
                                }
                            }
                            else
                            {
                                diag_printf("[VID] ##WARNING## real decode width=%d height=%d \n",xvid_dec_stats.data.vol.width, xvid_dec_stats.data.vol.height);
                                res= ISOM_ERR_PARAM_ERROR;
                                goto open_error;
                            }
                        }

                        /* Update buffer pointers */
                        if(used_bytes > 0)
                        {
                            pIsomGlob->v_SampleBuf_BytesUsed=pIsomGlob->v_SampleBuf_BytesRemain;
                        }

                    }
                }
                break;
#endif

#ifdef H263_SUPPORT
            case V_TYPE_H263:
            {
                uint32 readLen, gotbytes, gotcount;
                int32 gob;

#ifdef USE_OVERLAY
                VID_OVERLAY_ID=HAL_OVERLAY_INT_SRAM_ID_0;
                res=hal_OverlayLoad(VID_OVERLAY_ID);
                if(res!=HAL_ERR_NO)
                    diag_printf( "[VID]H263 HAL_OVERLAY_INT_SRAM_ID_1 hal_OverlayLoad  res=%d",res);
#endif

                //init h.263
                quiet = 0;
                errorstate=0;
                initbits(pIsomGlob->v_SampleBuf);
                if(errorstate)
                {
                    diag_printf("[VID] h.263 initbit err! \n");
                    res=ISOM_ERR_MALLOC_FAIL;
                    goto open_error;
                }

                initoptions (pIsomGlob->v_lowres);
                temp_ref = 0;
                prev_non_disposable_temp_ref =0;
                next_non_disposable_temp_ref = 0;

                coded_picture_width= pIsomGlob->v_width<< pIsomGlob->v_lowres;
                coded_picture_height=pIsomGlob->v_height<< pIsomGlob->v_lowres;

                pIsomGlob->v_SampleBuf_BytesUsed=0;

                if(pIsomGlob->med_type==MCI_TYPE_MP4 || pIsomGlob->med_type==MCI_TYPE_3GP)
                {
                    if((res = stbl_GetSampleSize(pIsomGlob->v_info , 1, &readLen))!=GF_OK)
                    {
                        diag_printf("[VID] video stbl_GetSampleSize err res=%d v_current_sample_num=%d \n",res, pIsomGlob->v_current_sample_num);
                        res=ISOM_ERR_VIDEO_READ_FAIL;
                        goto open_error;
                    }

                    if(pIsomGlob->v_SampleBuf_BytesRemain+readLen<pIsomGlob->v_SampleBufLen)
                    {
                        res=gf_isom_get_multSample(pIsomGlob->v_info,
                                                   pIsomGlob->v_current_sample_num+1,
                                                   pIsomGlob->v_SampleBuf+pIsomGlob->v_SampleBuf_BytesRemain,
                                                   readLen,    &gotbytes, &gotcount);

                        if(res || (gotbytes<=0))
                        {
                            diag_printf("[VID] video gf_isom_get_sample err res=%d \n",res);
                            res=ISOM_ERR_VIDEO_READ_FAIL;
                            goto open_error;
                        }
                    }
                    else
                    {
                        res=ISOM_ERR_MEM_INSUFFICIENT;
                        goto open_error;
                    }
                }
                else if(pIsomGlob->med_type==MCI_TYPE_AVSTRM)
                {
                    avinfostr *avinfohdl= (avinfostr*)pIsomGlob->filehdl;
                    avstream_clt_checkagent(avinfohdl->avstreamhdl_v);
                    res=avstream_clt_getDataLen(avinfohdl->avstreamhdl_v)-sizeof(avframeinfohdstr);
                    if(pIsomGlob->v_SampleBufLen< res)
                    {
                        readLen=pIsomGlob->v_SampleBufLen;
                    }
                    else
                    {
                        readLen=res;
                    }
                    memcpy(pIsomGlob->v_SampleBuf,(void*)avstream_clt_getDataPos(avinfohdl->avstreamhdl_v)+sizeof(avframeinfohdstr),readLen);
                    pIsomGlob->v_SampleBuf_BytesUsed=0;
                }

                resetbits (pIsomGlob->v_SampleBuf+pIsomGlob->v_SampleBuf_BytesUsed, readLen);

                gob=getheader ();
                if(errorstate)
                {
                    diag_printf("[VID] h.263 getheader err gob=%d \n",gob);
                    res=ISOM_ERR_INVALID_FORMAT;
                    goto open_error;
                }

                if (!gob )
                {
                    diag_printf("[VID] h.263 getheader err ! \n");
                    res=ISOM_ERR_INVALID_FORMAT;
                    goto open_error;
                }

                base_source_format = source_format;
                initdecoder ();
                if(errorstate)
                {
                    diag_printf("[VID] h.263 initdecoder err ! \n");
                    res=ISOM_ERR_MALLOC_FAIL;
                    goto open_error;
                }

                pIsomGlob->v_SampleBuf_BytesUsed=0;
                pIsomGlob->v_SampleBuf_BytesRemain=0;

                break;
            }
#endif
#ifdef H264_SUPPORT
            case V_TYPE_H264:
            {
                if(pIsomGlob->h264Cfg)
                {
                    //sps
                    if(pIsomGlob->h264Cfg->numOfSequenceParameterSets>0 && pIsomGlob->h264Cfg->sps)
                    {
                        for(i=0; i<pIsomGlob->h264Cfg->numOfSequenceParameterSets; i++)
                        {
                            if(pIsomGlob->h264Cfg->sps[i].dataLength+H264_START_CODE_SIZE+pIsomGlob->v_SampleBuf_BytesRemain<=pIsomGlob->v_SampleBufLen)
                            {
                                uint32 *p;
                                p=(uint32*)(pIsomGlob->v_SampleBuf+pIsomGlob->v_SampleBuf_BytesRemain);
                                *p=H264_START_CODE;
                                memcpy(pIsomGlob->v_SampleBuf+H264_START_CODE_SIZE+pIsomGlob->v_SampleBuf_BytesRemain,
                                       pIsomGlob->h264Cfg->sps[i].data, pIsomGlob->h264Cfg->sps[i].dataLength);
                                pIsomGlob->v_SampleBuf_BytesRemain+=pIsomGlob->h264Cfg->sps[i].dataLength+H264_START_CODE_SIZE;
                            }
                        }
                    }

                    //pps
                    if(pIsomGlob->h264Cfg->numOfPictureParameterSets>0 && pIsomGlob->h264Cfg->pps)
                    {
                        for(i=0; i<pIsomGlob->h264Cfg->numOfPictureParameterSets; i++)
                        {
                            if(pIsomGlob->h264Cfg->pps[i].dataLength+H264_START_CODE_SIZE+pIsomGlob->v_SampleBuf_BytesRemain<=pIsomGlob->v_SampleBufLen)
                            {
                                //uint32 *p;
                                uint8 *p;

                                //p=(uint32*)(pIsomGlob->v_SampleBuf+pIsomGlob->v_SampleBuf_BytesRemain);
                                p=(pIsomGlob->v_SampleBuf+pIsomGlob->v_SampleBuf_BytesRemain);

                                //p addr maybe not 4byte align.
                                p[0]=0;
                                p[1]=0;
                                p[2]=0;
                                p[3]=1;//H264_START_CODE

                                memcpy(pIsomGlob->v_SampleBuf+H264_START_CODE_SIZE+pIsomGlob->v_SampleBuf_BytesRemain,
                                       pIsomGlob->h264Cfg->pps[i].data, pIsomGlob->h264Cfg->pps[i].dataLength);

                                pIsomGlob->v_SampleBuf_BytesRemain+=pIsomGlob->h264Cfg->pps[i].dataLength+H264_START_CODE_SIZE;
                            }
                        }
                    }

                }

                pIsomGlob->pH264Hdl=h264_decoder_init();
                if(!pIsomGlob->pH264Hdl)
                {
                    diag_printf("[VID] h.264 h264_decoder_init err!  \n");
                    res=ISOM_ERR_MALLOC_FAIL;
                    goto open_error;
                }

                break;
            }
#endif
            case V_TYPE_MJPEG:
            {

                break;
            }
            case V_TYPE_RV:
            {
                break;
            }
            default:
                diag_printf("[VID] err! pIsomGlob->v_type=%x \n", pIsomGlob->v_type);
                res=ISOM_ERR_INVALID_FORMAT;
                goto open_error;
        }
    }

    mmc_MemCheck(TRUE);

    pIsomGlob->a_audio_path=audio_path;
    pIsomGlob->open_audio=open_audio;
    pIsomGlob->bSkipFrame = FALSE;

    pIsomGlob->v_current_frame_num= -1;//0=the first frame
    AV_DEC_STATE=0;
    DISPLAY_STATE=0;
    AUD_DEC_MIN=2;//default
    VID_ERR_FRAME_REC=0;
    AUD_DATA_REQ=0;

    //reduce charge current for video player.
    pmd_ReduceChargeCurrent(1);

    play_state=VID_PLAY_STATE_IDLE;

    diag_printf("[VID] mmc_vid_isom_open_file ok!\n",res);
    return ISOM_ERR_OK;

open_error:
    diag_printf("[VID] ##WARNING## mmc_vid_isom_open_file err res=%d\n",res);
#if 1
    mmc_vid_isom_close_file ();
    return res;
#else
    if(VOC_OPEN)
    {
        if(pIsomGlob->dec_mode==1 )
            vpp_AudioJpegDecClose();
        else
        {
#if(CHIP_HAS_GOUDA == 1)
            if(pIsomGlob->a_type != A_TYPE_AMR)
            {
#endif
                vpp_AudioJpegDecClose();
#if(CHIP_HAS_GOUDA == 1)
            }
            else
            {
                vpp_SpeechClose();
            }
#endif
        }
        VOC_OPEN=0;

    }
#ifdef VID_VOC_FREQ_SET
    if(VOC_FREQ_STATE)
    {
        hal_SysSetVocClock(VOC_FREQ_STATE);
        VOC_FREQ_STATE = 0;
    }
#endif

    if(pIsomGlob)
    {
        //destroy video decoder
        if( pIsomGlob->v_type == V_TYPE_MPEG4 )
        {
#ifdef MPEG4_SUPPORT
            dec_stop();
#endif
        }
        else if(pIsomGlob->v_type==V_TYPE_H264 )
        {
#ifdef H264_SUPPORT
            h264_decoder_close(pIsomGlob->pH264Hdl);
#endif
        }

        //destroy media box
        if(pIsomGlob->med_type==MCI_TYPE_MP4 || pIsomGlob->med_type==MCI_TYPE_3GP)
        {
            gf_isom_close_file(pIsomGlob->file);
        }

#ifdef RM_SUPPORT
        else if(pIsomGlob->med_type==MCI_TYPE_RM)
        {
            rm_destroy(0);
            if(pIsomGlob->a_type==A_TYPE_COOK && pIsomGlob->a_Gecko2DecHdl)
            {
                Gecko2FreeDecoder(pIsomGlob->a_Gecko2DecHdl);
            }

        }
#endif

        else if(pIsomGlob->med_type==MCI_TYPE_MJPG)
        {
            if(pIsomGlob->avi_p)
                avi_close_input_file(pIsomGlob->avi_p);
        }

        //destroy audio decoder

        if(pIsomGlob->a_AacDecHdl)
        {
            AACFreeDecoder(pIsomGlob->a_AacDecHdl);
            pIsomGlob->a_AacDecHdl=0;
        }
        else if(pIsomGlob->a_type == A_TYPE_MP3)
        {
            MMF_Mp3_CloseDecoder();
        }
        /*else if(pIsomGlob->a_Mp3DecHdl)
        {
            MP3FreeDecoder(pIsomGlob->a_Mp3DecHdl);
            pIsomGlob->a_Mp3DecHdl=0;
        }*/
        else if(pIsomGlob->a_AmrStruct)
        {
            AmrDecClose(pIsomGlob->a_AmrStruct);
            pIsomGlob->a_AmrStruct=0;
        }

#ifdef DRA_SUPPORT
        if(pIsomGlob->a_DraDecHdl)
        {
            DRA_Release(&pIsomGlob->a_DraDecHdl);
            pIsomGlob->a_DraDecHdl=0;
        }
#endif
    }
    //mmc_MemFreeAll();
    g_VidBT8kPcmBuffer = NULL;
    return res;
#endif
}

int32 mmc_vid_isom_play (uint16 lcd_start_x,  uint16 lcd_start_y)
{
    int i,j;
    int32 res=0;
    unsigned long long temp;
    uint32 aud_frame_samp;

#if(CHIP_HAS_GOUDA == 1)
    SSHdl *my_ss_handle = MpegPlayer.MpegInput.ss_handle;
    bt_a2dp_audio_cap_struct * my_audio_config_handle=   MpegPlayer.MpegInput.audio_config_handle ;
    bt_a2dp_sbc_codec_cap_struct *sbc_config_handle =  &my_audio_config_handle->codec_cap.sbc;

#endif

    totalTa=0;
    totalTv=0;
    counta=0;
    countv=0;

    if(play_state!=VID_PLAY_STATE_IDLE)
    {
        diag_printf("[VID] mmc_vid_isom_play err play_state= %d \n",play_state);
        res= ISOM_ERR_PLAY_STATE_FAIL;
        goto play_error;
    }

    //for audio
    if(!pIsomGlob->v_fps)
    {
        diag_printf("[VID] err pIsomGlob->v_fps=%d !!!\n",pIsomGlob->v_fps);
        res=ISOM_ERR_PARAM_ERROR;
        goto play_error;
    }

    if(pIsomGlob->a_type== A_TYPE_AAC)
        aud_frame_samp= AAC_FRAME_SAMPLE;
    else if(pIsomGlob->a_type== A_TYPE_AMR)
        aud_frame_samp= AMR_FRAME_SAMPLE;
    else if(pIsomGlob->a_type== A_TYPE_MP3)
    {
        if(pIsomGlob->a_sample_rate>=32000)
            aud_frame_samp= MP3_FRAME_SAMPLE_1152;
        else
            aud_frame_samp= MP3_FRAME_SAMPLE_576;
    }
    else if(pIsomGlob->a_type== A_TYPE_PCM)
        aud_frame_samp= PCM_FRAME_SAMPLE;
    else if(pIsomGlob->a_type== A_TYPE_ADPCM)
        aud_frame_samp=1;
    else if(pIsomGlob->a_type== A_TYPE_COOK)
        aud_frame_samp= pIsomGlob->a_frame_sample;
    else if(pIsomGlob->a_type== A_TYPE_RA)
        aud_frame_samp=1;
    else if(pIsomGlob->a_type== A_TYPE_DRA)
        aud_frame_samp= DRA_FRAME_SAMPLE_1024;
    else
    {
        diag_printf("[VID] err pIsomGlob->a_type=%d !!!\n",pIsomGlob->a_type);
        res=ISOM_ERR_UNSUPPORT;
        goto play_error;
    }
    MpegPlayer.MpegInput.bt_sco = FALSE;//sound quality is terrible, always close sco.
    if (is_SSHdl_valid(my_ss_handle)&& (my_audio_config_handle->codec_type == 1) \
            &&  pIsomGlob->dec_mode ==1)
    {
        int iRet = 0;
        iRet = sbc_init(&pIsomGlob->sbc, 0L);
        if(iRet != 0)
        {
            diag_printf("[VID] ####################### iRet = %d\n", iRet);
            goto play_error;
        }

        diag_printf("[VID] ####################### sbc_init ok!");
        pIsomGlob->sbc.mono2dual = DUAL2MONO;//2:dual->mono.
        g_SbcEncEnable =0;//not enc sbc in first half pcm buf,avoid error ch or samplerate.
        if (sbc_config_handle->alloc_method  == 0x01)
            pIsomGlob->sbc.allocation= SBC_AM_LOUDNESS;//
        else if (sbc_config_handle->alloc_method  == 0x02 )
            pIsomGlob->sbc.allocation= SBC_AM_SNR;//SBC_AM_LOUDNESS;//
        else
            diag_printf("[VID] WARNING! ##WARNING##ErrorSBC alloc_method!\n");
        if( sbc_config_handle->channel_mode == 0x01 )
            pIsomGlob->sbc.mode = SBC_MODE_JOINT_STEREO; // joint stereo
        else if( sbc_config_handle->channel_mode == 0x02 )
            pIsomGlob->sbc.mode = SBC_MODE_STEREO; // stereo
        else if( sbc_config_handle->channel_mode == 0x04 )
            pIsomGlob->sbc.mode = SBC_MODE_DUAL_CHANNEL; // dual
        else if( sbc_config_handle->channel_mode == 0x08 )
            pIsomGlob->sbc.mode= SBC_MODE_MONO; // mono
        else
            diag_printf("[VID] WARNING! ##WARNING##ErrorSBC channel mode!\n");
    }

    if(DTS_FLG==0xFFFFFFFF )
    {
        temp=((((unsigned long long)pIsomGlob->v_current_frame_num+1)*(unsigned long long)pIsomGlob->a_sample_rate)<<V_FPS_SCALE_BIT)/
             ((unsigned long long)pIsomGlob->v_fps*aud_frame_samp);
        if(pIsomGlob->a_type== A_TYPE_ADPCM || pIsomGlob->a_type== A_TYPE_RA)
            temp=0;
    }
    else
    {
        temp=((unsigned long long)pIsomGlob->current_time*(unsigned long long)pIsomGlob->a_sample_rate)/ ((unsigned long long)aud_frame_samp*1000);
        if(pIsomGlob->a_type== A_TYPE_ADPCM || pIsomGlob->a_type== A_TYPE_RA)
            temp=0;
    }

    pIsomGlob->a_current_frame_num=(uint32)temp;

    if((!pIsomGlob->v_total_frame_num ) || !pIsomGlob->total_time)
    {
        diag_printf("[VID] err v_total_frame_num=%d total_time=%d !!!\n",pIsomGlob->v_total_frame_num,pIsomGlob->total_time);
        res=ISOM_ERR_PARAM_ERROR;
        goto play_error;
    }

    if(DTS_FLG==0xFFFFFFFF)
    {

        pIsomGlob->a_current_sample_num = (uint32)((unsigned long long)pIsomGlob->a_total_sample_num*(unsigned long long)(pIsomGlob->v_current_frame_num+1)/
                                          ((unsigned long long)pIsomGlob->v_total_frame_num));

    }
    else if(pIsomGlob->open_audio)
    {
        uint32 dts_sampCount;
        uint32 DTS_pos,DTS_audio,DTS_SampDelta_audio;

        pIsomGlob->a_current_sample_num =(uint32)((unsigned long long)pIsomGlob->a_total_sample_num*(unsigned long long)(pIsomGlob->current_time)/
                                         ((unsigned long long)pIsomGlob->total_time));

        //get audio dts position
        DTS_pos= ((long long)DTS_FLG*(long long)pIsomGlob->a_trak->Media->mediaHeader->timeScale)/(long long)pIsomGlob->v_timeScale;
        while(pIsomGlob->a_current_sample_num <pIsomGlob->a_total_sample_num)
        {
            if((res=stbl_GetSampleDTS(pIsomGlob->a_info->sampleTable->TimeToSample, pIsomGlob->a_current_sample_num +1,
                                      &DTS_audio, &dts_sampCount, &DTS_SampDelta_audio))==0)
            {
                if(DTS_audio < DTS_pos)
                    pIsomGlob->a_current_sample_num++;
                else
                    break;
            }
            else
                break;
        }

        while(pIsomGlob->a_current_sample_num !=0)
        {
            if((res=stbl_GetSampleDTS(pIsomGlob->a_info->sampleTable->TimeToSample, pIsomGlob->a_current_sample_num +1,
                                      &DTS_audio, &dts_sampCount, &DTS_SampDelta_audio))==0)
            {
                if(DTS_audio > DTS_pos)
                    pIsomGlob->a_current_sample_num--;
                else
                    break;
            }
            else
                break;
        }

        diag_printf("[VID]sync AV with DTS a=%d v=%d Scale a=%d v=%d",DTS_audio,DTS_FLG,
                    pIsomGlob->a_trak->Media->mediaHeader->timeScale,pIsomGlob->v_trak->Media->mediaHeader->timeScale);
    }


    if(pIsomGlob->med_type==MCI_TYPE_MP4 || pIsomGlob->med_type==MCI_TYPE_3GP)
    {
        pIsomGlob->a_SampStart=pIsomGlob->a_SampEnd=pIsomGlob->a_SampBuf+pIsomGlob->a_SampBufMirrorLen;

        if(pIsomGlob->a_type== A_TYPE_AMR)
        {
            if(pIsomGlob->dec_mode==1 )
            {
                memcpy(pIsomGlob->a_SampTmpBuf, AMR_HEAD, AMR_HEAD_SIZE);
                pIsomGlob->a_SampTmpBufp=pIsomGlob->a_SampTmpBuf;
                pIsomGlob->a_SampTmpRemain=AMR_HEAD_SIZE;
            }
            else
            {
                pIsomGlob->a_SampTmpBufp=pIsomGlob->a_SampTmpBuf;
                pIsomGlob->a_SampTmpRemain=0;
            }

        }
        else
        {
            pIsomGlob->a_SampTmpBufp=pIsomGlob->a_SampTmpBuf;
            pIsomGlob->a_SampTmpRemain=0;
        }

    }

#ifdef RM_SUPPORT
    else if(pIsomGlob->med_type==MCI_TYPE_RM)
    {
        pIsomGlob->a_SampStart=pIsomGlob->a_SampEnd=pIsomGlob->a_SampBuf+pIsomGlob->a_SampBufMirrorLen;
        pIsomGlob->a_SampTmpBufp=pIsomGlob->a_SampTmpBuf;
        pIsomGlob->a_SampTmpRemain=0;

        fillSampBuf();
        temp=(unsigned long long)rm_getAudioTimeStampStart();
        temp=temp*(unsigned long long)(pIsomGlob->a_sample_rate)/1000;
        pIsomGlob->a_current_frame_num=(unsigned int)temp;//reuse in rmvb for record pcm sample num.
        diag_printf("[VID]audio time line pcm sample=%d", pIsomGlob->a_current_frame_num);
    }
#endif

    else if(pIsomGlob->med_type==MCI_TYPE_AVSTRM)
    {

        if(pIsomGlob->dec_mode==1 && /*pIsomGlob->open_audio!=2 &&*/ pIsomGlob->a_PcmBuf)//resue for amr init
        {
            memcpy(pIsomGlob->a_PcmBuf, AMR_HEAD, AMR_HEAD_SIZE);
            pIsomGlob->a_SampTmpBuf=pIsomGlob->a_PcmBuf;
        }

        memset(&avStrmFrmHdV,0,sizeof(avframeinfohdstr));
        memset(&avStrmFrmHdA,0,sizeof(avframeinfohdstr));
        avStrmTimStpStartV=0xFFFFFFFF;
        avStrmTimStpStartA=0xFFFFFFFF;
        avStrmCurPosA=0;
        avStrmCurPosV=0;
    }
    else
    {
        pIsomGlob->a_SampStart=pIsomGlob->a_SampEnd=pIsomGlob->a_SampBuf+pIsomGlob->a_SampBufMirrorLen;
        if(pIsomGlob->a_type== A_TYPE_AMR && pIsomGlob->a_current_sample_num!=0)
        {
            if(pIsomGlob->dec_mode==1 && pIsomGlob->open_audio!=2)
            {
                memcpy(pIsomGlob->a_SampTmpBuf, AMR_HEAD, AMR_HEAD_SIZE);
                pIsomGlob->a_SampTmpBufp=pIsomGlob->a_SampTmpBuf;
                pIsomGlob->a_SampTmpRemain=AMR_HEAD_SIZE;
            }
            else
            {
                pIsomGlob->a_SampTmpBufp=pIsomGlob->a_SampTmpBuf;
                pIsomGlob->a_SampTmpRemain=0;
            }
//#endif
        }
        else
        {
            pIsomGlob->a_SampTmpBufp=pIsomGlob->a_SampTmpBuf;
            pIsomGlob->a_SampTmpRemain=0;
        }

        res=avi_set_audio_position2(pIsomGlob->avi_p, pIsomGlob->a_current_sample_num);
        if(res!=0)
        {
            diag_printf("[VID] avi_set_audio_position2 err! res=%d ", res);
            res=ISOM_ERR_AUDIO_READ_FAIL;
            goto play_error;
        }

        if((pIsomGlob->dec_mode==0 || pIsomGlob->open_audio==2)
                &&pIsomGlob->a_type== A_TYPE_AMR && pIsomGlob->a_current_sample_num ==0)
        {
            fillSampBuf();
            //move amr header
            moveDataPos(AMR_HEAD_SIZE);
        }
    }


    //for amr c decoder
    if(pIsomGlob->open_audio!=2 || pIsomGlob->med_type==MCI_TYPE_AVSTRM)
    {
        if(pIsomGlob->a_type== A_TYPE_AMR)
        {
            int32 consume=0;
            if(pIsomGlob->med_type==MCI_TYPE_MJPG && pIsomGlob->a_current_sample_num ==0)
            {
                memcpy(pIsomGlob->a_SampTmpBuf, AMR_HEAD, AMR_HEAD_SIZE);
            }

            if(!pIsomGlob->a_AmrStruct)//avoid repeat init and malloc
                pIsomGlob->a_AmrStruct= AmrDecInit(pIsomGlob->a_SampTmpBuf, &consume);
            if(!pIsomGlob->a_AmrStruct)
            {
                diag_printf("[VID] AmrDecInit err! ");
                res=ISOM_ERR_AUDIO_DEC_FAIL;
                goto play_error;
            }
            pIsomGlob->a_SampTmpRemain=0;

            if(pIsomGlob->med_type==MCI_TYPE_MJPG && pIsomGlob->a_current_sample_num ==0)
            {
                fillSampBuf();
                //move amr header
                moveDataPos(AMR_HEAD_SIZE);
            }
        }
#ifdef DRA_SUPPORT
        else if(pIsomGlob->a_type== A_TYPE_DRA && pIsomGlob->med_type==MCI_TYPE_AVSTRM
                && !pIsomGlob->a_DraDecHdl)
        {
            dra_cfg DraCfg;
            dra_audio_info AudioInfo;
            avinfostr *avinfohdl= (avinfostr*)pIsomGlob->filehdl;

            DraCfg.initMode = 1;
            DraCfg.byteOrder =1;
            DraCfg.channel =pIsomGlob->a_channels;

            //malloc memory for DRA decode lib
            res = DRA_GetMemSize(DraCfg.channel,DraDownMixMode);
            if(res > 0)
            {
                pIsomGlob->a_DraDecHdl = (void *)mmc_MemMalloc(res);
                if(!pIsomGlob->a_DraDecHdl)
                {
                    diag_printf("[VID] pIsomGlob->a_DraDecHdl malloc fail!!\n");
                    res=ISOM_ERR_MALLOC_FAIL;
                    goto play_error;
                }
                diag_printf("[VID]DRA mem need=%d",res);
            }

            avstream_clt_checkagent(avinfohdl->avstreamhdl_a);

            res=-1;
            if(DraCfg.initMode==1 && (avstream_clt_getheader(avinfohdl->avstreamhdl_a,&avStrmFrmHdA)==1))
            {
                res = DRA_InitDecode(&pIsomGlob->a_DraDecHdl,&DraCfg,getDataPos(),avStrmFrmHdA.samplesize,&AudioInfo);
                diag_printf("[VID]DRA init a_DraDecHdl=%x",pIsomGlob->a_DraDecHdl);
            }

            if(res!=0)
            {
                diag_printf("[VID] DRA decoder init err!res=%d\n",res);
                res=ISOM_ERR_AUDIO_DEC_FAIL;
                goto play_error;
            }
        }
#endif
    }
    else
        vocDecAud.reset=1;

    //AUD_EXTRA_BUF_DONE=0;
    //diag_printf("%x",aud_extra_pcmbuf_state[0].pBufPos);
    //diag_printf("%x",aud_extra_pcmbuf_state[1].pBufPos);

    A_EXBF_RD_ID=0;
    A_EXBF_WT_ID=0;
    for(i=0; i<EXTRA_PCM_BUF_NUM; i++)
    {
        if(pIsomGlob->dec_mode==1 && (pIsomGlob->open_audio!=2 || pIsomGlob->med_type==MCI_TYPE_AVSTRM))
            aud_extra_pcmbuf_state[i].done=0;
        else
            aud_extra_pcmbuf_state[i].done=1;
    }

    VOC_AUD=0;

#ifdef RM_SUPPORT
    needRA=0;
#endif

#if 1
    if(pIsomGlob->open_audio)
    {
        AUD_FADE_IN_FLAG=(pIsomGlob->a_HalfPcmBufFrame+2);//zero pcm num

        for(i=0; i<(2+EXTRA_PCM_BUF_NUM); i++)
        {
            if(pIsomGlob->dec_mode==1 && (pIsomGlob->open_audio!=2 || pIsomGlob->med_type==MCI_TYPE_AVSTRM))
            {

                AUD_DEC_FRM_NUM=0;
                pPcmBufPos=0;
                AUD_DATA_REQ=1;
                for(j=0; j<pIsomGlob->a_HalfPcmBufFrame; j++)
                {
                    aud_nframe_dec();
                }
                g_SbcEncEnable =1;

                if(i<2)
                {
                    memcpy(pIsomGlob->a_PcmBuf +i*pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame,
                           (void*)aud_extra_pcmbuf_state[A_EXBF_RD_ID].pBufPos,
                           pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame);
                    aud_extra_pcmbuf_state[A_EXBF_RD_ID].done=0;
                    A_EXBF_RD_ID+=1;
                    if(A_EXBF_RD_ID>=EXTRA_PCM_BUF_NUM)A_EXBF_RD_ID=0;
                }

                switch(pIsomGlob->a_type)
                {
                    case A_TYPE_AAC:
                    {
                        AACFrameInfo aacFrameInfo;
                        AACGetLastFrameInfo(pIsomGlob->a_AacDecHdl, &aacFrameInfo);
                        //reuse vocDecStatus
                        vocDecStatus.nbChannel= aacFrameInfo.nChans;
                        vocDecStatus.SampleRate= aacFrameInfo.sampRateOut;
                        vocDecStatus.BitRate= aacFrameInfo.bitsPerSample;
                        vocDecStatus.output_len= aacFrameInfo.outputSamps*aacFrameInfo.bitsPerSample/8;
                        break;
                    }
                    case A_TYPE_MP3:
                    {
                        //MP3FrameInfo mp3FrameInfo;
                        //MP3GetLastFrameInfo(pIsomGlob->a_Mp3DecHdl, &mp3FrameInfo);
                        Mp3FrmInfo mp3FrameInfo;
                        MMF_Mp3_GetFrmInfo(&mp3FrameInfo);
                        //reuse vocDecStatus
                        vocDecStatus.nbChannel= mp3FrameInfo.nChans;
                        vocDecStatus.SampleRate= mp3FrameInfo.samprate;
                        vocDecStatus.BitRate= mp3FrameInfo.bitsPerSample;
                        vocDecStatus.output_len= mp3FrameInfo.outputSamps*mp3FrameInfo.bitsPerSample/8;
                    }
                    break;
                    case A_TYPE_AMR:
                        vocDecStatus.nbChannel= 1;
                        vocDecStatus.SampleRate= 8000;
                        vocDecStatus.BitRate= 16;
                        vocDecStatus.output_len= (AMR_FRAME_SAMPLE*2);
                        break;
                    case A_TYPE_COOK:
                    case A_TYPE_PCM:
                        //reuse vocDecStatus
                        vocDecStatus.nbChannel= pIsomGlob->a_channels;
                        vocDecStatus.SampleRate= pIsomGlob->a_sample_rate;
                        vocDecStatus.BitRate= pIsomGlob->a_bit;
                        vocDecStatus.output_len= pIsomGlob->a_frameBytes;
                        break;
#ifdef DRA_SUPPORT
                    case A_TYPE_DRA:
                        //reuse vocDecStatus
                        diag_printf( "[VID] DRA real nChannelNormal=%d nChannelLfe=%d fInstantBitrate=%d nSampleRate=%d ",
                                     DraFrameInfo.nChannelNormal,
                                     DraFrameInfo.nChannelLfe,
                                     DraFrameInfo.fInstantBitrate,
                                     DraFrameInfo.nSampleRate);
                        vocDecStatus.nbChannel= pIsomGlob->a_channels;
                        vocDecStatus.SampleRate= DraFrameInfo.nSampleRate;
                        vocDecStatus.BitRate= pIsomGlob->a_bit;
                        vocDecStatus.output_len= pIsomGlob->a_frameBytes;
                        break;
#endif
                    default:
                        break;
                }

                diag_printf( "[VID] AUD real channel=%d SampleRate=%d BitRate=%d output_len=%d ",
                             vocDecStatus.nbChannel,
                             vocDecStatus.SampleRate,
                             vocDecStatus.BitRate,
                             vocDecStatus.output_len);

                if(vocDecStatus.nbChannel !=pIsomGlob->a_channels)
                {
                    diag_printf( "[VID] warning! AUD channel not match");
                    if(vocDecStatus.nbChannel==1)
                    {
                        pIsomGlob->a_channels=1;
                        pIsomGlob->a_frameBytes>>=1;
                        pIsomGlob->a_PcmBufHalfPos=pIsomGlob->a_PcmBuf+pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame;
                    }
                    else if(vocDecStatus.nbChannel==2)
                    {
                        pIsomGlob->a_channels=2;
                        pIsomGlob->a_frameBytes<<=1;
                        pIsomGlob->a_PcmBufHalfPos=pIsomGlob->a_PcmBuf+pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame;
                    }
                    else
                    {
                        res=ISOM_ERR_AUDIO_DEC_FAIL;
                        goto play_error;
                    }
                }

                if(vocDecStatus.SampleRate !=pIsomGlob->a_sample_rate)
                {
                    diag_printf( "[VID] warning! AUD SampleRate not match");
                    if(vocDecStatus.SampleRate>=8000 && vocDecStatus.SampleRate<=48000)
                        pIsomGlob->a_sample_rate=vocDecStatus.SampleRate;
                    else
                    {
                        res=ISOM_ERR_AUDIO_DEC_FAIL;
                        goto play_error;
                    }

                    if(pIsomGlob->a_type==A_TYPE_MP3 )
                    {
                        if( pIsomGlob->a_frame_sample==MP3_FRAME_SAMPLE_576 && vocDecStatus.SampleRate>=32000)
                        {
                            pIsomGlob->a_frame_sample=MP3_FRAME_SAMPLE_1152;
                            pIsomGlob->a_frameBytes<<=1;
                            pIsomGlob->a_PcmBufHalfPos=pIsomGlob->a_PcmBuf+pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame;
                        }
                        else if( pIsomGlob->a_frame_sample==MP3_FRAME_SAMPLE_1152 && vocDecStatus.SampleRate<=24000)
                        {
                            pIsomGlob->a_frame_sample=MP3_FRAME_SAMPLE_576;
                            pIsomGlob->a_frameBytes>>=1;
                            pIsomGlob->a_PcmBufHalfPos=pIsomGlob->a_PcmBuf+pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame;
                        }
                    }
                }

                if(vocDecStatus.output_len !=pIsomGlob->a_frameBytes)
                {
                    diag_printf( "[VID] warning! AUD output_len not match");
                    res=ISOM_ERR_AUDIO_DEC_FAIL;
                    goto play_error;
                }
            }
            else
            {
                fillSampBuf();

                /*check AMR mode*/
                if(pIsomGlob->a_type== A_TYPE_AMR && i==0)
                {
                    uint8 *pH;
                    pH=getDataPos();

                }

                if(getDataLen()>0)
                {
                    //pPcmBufPos=(uint32)pIsomGlob->a_ExtraPcmBuf;
                    if(i==0)
                        pPcmBufPos=(uint32)pIsomGlob->a_PcmBuf;
                    else if(i==1)
                        pPcmBufPos=(uint32)pIsomGlob->a_PcmBufHalfPos;
                    else
                    {
                        //aud_extra_pcmbuf_state[i-2].done=0;
                        pPcmBufPos=(uint32)(aud_extra_pcmbuf_state[i-2].pBufPos);
                    }


                    if(pIsomGlob->a_type== A_TYPE_COOK)
                    {

#ifdef RM_SUPPORT
                        //int32 lostflag;
                        //if(i==0)lostflag=1;
                        //else lostflag=0;
                        tick1=hal_TimGetUpTime();

                        AUD_DEC_FRM_NUM=0;

                        while(AUD_DEC_FRM_NUM < pIsomGlob->a_HalfPcmBufFrame)
                        {
                            if(AUD_FADE_IN_FLAG==0)
                            {
                                res=Gecko2Decode(pIsomGlob->a_Gecko2DecHdl, getDataPos(), 0, (short*)pPcmBufPos);
                                if(res!=0)
                                {
                                    diag_printf("[VID]AUD Gecko2Decode err! res=%d\n",res);
                                    res=ISOM_ERR_AUDIO_DEC_FAIL;
                                    goto play_error;
                                }
                            }
                            else
                            {
                                //mute frame
                                if(AUD_FADE_IN_FLAG>1)
                                    memset((uint8*)pPcmBufPos, 0, pIsomGlob->a_frameBytes);
                                else
                                {
                                    audFadeIn((int16*)pPcmBufPos, pIsomGlob->a_frame_sample*pIsomGlob->a_channels);
                                }
                                AUD_FADE_IN_FLAG--;
                            }

                            moveDataPos(pIsomGlob->a_codeframeBytes);//const code frame bytes.
                            pPcmBufPos+=pIsomGlob->a_frameBytes;

                            AUD_DEC_FRM_NUM++;
                            //lostflag=0;

                        }

                        if(i>1)
                            aud_extra_pcmbuf_state[i-2].done=1;

                        AUD_DEC_FRM_NUM=0;

                        tick2=hal_TimGetUpTime();
                        diag_printf( "[VID] aud cook dec time=%d frame num=%d", (tick2-tick1)*1000/16384, pIsomGlob->a_HalfPcmBufFrame);
#endif

                    }
                    else if(pIsomGlob->a_type!= A_TYPE_PCM && pIsomGlob->a_type!= A_TYPE_ADPCM)
                    {
                        tick1=hal_TimGetUpTime();
                        AUD_DEC_FRM_NUM=0;

                        if(VOC_WORK==VOC_WORK_NON )
                        {


                            VOC_WORK=VOC_WORK_AUD;
                            aud_extra_pcmbuf_state[A_EXBF_WT_ID].done=0;
#if(CHIP_HAS_GOUDA == 1)
                            if(pIsomGlob->a_type!= A_TYPE_AMR)
                            {
#endif

                                vocDecAud.inStreamBufAddr=(INT32 *)getDataPos();
                                vocDecAud.outStreamBufAddr=(INT32 *)pPcmBufPos;
                                vocDecAud.audioItf=audioItf;

#if(CHIP_HAS_GOUDA == 1)
                                if( is_SSHdl_valid(my_ss_handle)&& (my_audio_config_handle->codec_type == 1))      //BT_A2DP_sbc
                                {

                                    int mode =SBC_MODE_JOINT_STEREO;
                                    vocDecAud.BsbcEnable = 1;

                                    if( sbc_config_handle->channel_mode == 0x01 )
                                        mode = SBC_MODE_JOINT_STEREO; // joint stereo
                                    else if( sbc_config_handle->channel_mode == 0x02 )
                                        mode = SBC_MODE_STEREO; // stereo
                                    else if( sbc_config_handle->channel_mode == 0x04 )
                                        mode = SBC_MODE_DUAL_CHANNEL; // dual
                                    else if( sbc_config_handle->channel_mode == 0x08 )
                                        mode= SBC_MODE_MONO; // mono
                                    else
                                        diag_printf("[VID] WARNING! ##WARNING##ErrorSBC channel mode!\n");

                                    vocDecAud.Source_width = mode;

                                    if( sbc_config_handle->alloc_method  == 0x01 )
                                        vocDecAud.Source_height= SBC_AM_LOUDNESS;//
                                    else if( sbc_config_handle->alloc_method  == 0x02 )
                                        vocDecAud.Source_height= SBC_AM_SNR;//SBC_AM_LOUDNESS;//
                                    else
                                        diag_printf("[VID] WARNING! ##WARNING##ErrorSBC alloc_method!\n");
                                    vocDecAud.Cut_width =    SBC_BIT_POOL;
                                    vocDecAud.inStreamUBufAddr = (INT32*)G_VppSBCConstX;
                                    vocDecAud.inStreamVBufAddr = G_VidSBCConstY;

                                }
#endif
                                /*if(pIsomGlob->v_type ==V_TYPE_H264 || pIsomGlob->v_type ==V_TYPE_RV
                                    ||pIsomGlob->v_type ==V_TYPE_MPEG4
                                    )
                                    vpp_AVDecSetMode(vocDecAud.mode, 1, 0);//set video code invalide
                                    */
                                vpp_AudioJpegDecScheduleOneFrame(&vocDecAud);
                                vocDecAud.reset=0;
#if(CHIP_HAS_GOUDA == 1)

                            }
                            else
                            {
                                HAL_SPEECH_DEC_IN_T *pVocDecAmr = vpp_SpeechGetRxCodBuffer();

                                get_amr_mode( getDataPos(), &(pIsomGlob->a_amr_mode), &(pIsomGlob->a_codeframeBytes));

                                if(i==0)
                                {
                                    int32 dropNum=pIsomGlob->a_HalfPcmBufFrame;
                                    //make start with silence
                                    while(dropNum)
                                    {
                                        moveDataPos(pIsomGlob->a_codeframeBytes);//drop one frame
                                        get_amr_mode( getDataPos(), &(pIsomGlob->a_amr_mode), &(pIsomGlob->a_codeframeBytes));
                                        dropNum--;
                                    }
                                    memset((void*)pPcmBufPos, 0, pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame);
                                    aud_extra_pcmbuf_state[A_EXBF_WT_ID].done=1;
                                    A_EXBF_WT_ID+=1;
                                    if(A_EXBF_WT_ID>=EXTRA_PCM_BUF_NUM)A_EXBF_WT_ID=0;
                                    VOC_WORK=VOC_WORK_NON;

                                }
                                else
                                {
                                    if(pIsomGlob->a_amr_mode != HAL_INVALID)
                                    {
                                        getData((U8*)pVocDecAmr->decInBuf, pIsomGlob->a_codeframeBytes, 1);
                                        pVocDecAmr->codecMode= pIsomGlob->a_amr_mode;
                                    }
                                    else
                                    {
                                        //write silence data
                                        moveDataPos(pIsomGlob->a_codeframeBytes);
                                        memcpy(pVocDecAmr->decInBuf, silence_frame475, 13);
                                        pVocDecAmr->codecMode= HAL_AMR475_DEC;
                                    }

                                    pVocDecAmr->dtxOn = 0;
                                    //pVocDecAmr->codecMode= pIsomGlob->a_amr_mode;
                                    pVocDecAmr->decFrameType = 0;
                                    pVocDecAmr->bfi = 0;
                                    pVocDecAmr->sid = 0;
                                    pVocDecAmr->taf = 0;
                                    pVocDecAmr->ufi = 0;
                                    pVocDecAmr->reserved= 0;
                                    RX_BUF_SWAP_ID=0;

                                    // Set all the preprocessing modules
                                    vpp_SpeechSetEncDecPocessingParams(/*audioItf*/AUD_ITF_LOUD_SPEAKER, audio_cfg.spkLevel, 0);
                                    vpp_SpeechScheduleOneFrame(VPP_SPEECH_WAKEUP_SW_DEC);
                                }
                            }
#endif
                        }

                        j = 0;
                        while(VOC_WORK )
                        {
                            j++;
                            if(j>=VOC_WORK_CONT)
                            {
                                diag_printf("[VID]AUD VOC_WORK_CONT end! \n");
                                res=ISOM_ERR_AUDIO_DEC_FAIL;
                                goto play_error;
                            }
                            //sxr_Sleep(1);
                        }

                        diag_printf("A_EXBF_WT_ID %d A_EXBF_RD_ID %d",A_EXBF_WT_ID,A_EXBF_RD_ID);
                        //if(i>1)
                        //  aud_extra_pcmbuf_state[i-2].done=1;
                        if(i<2)
                        {
                            //aud_extra_pcmbuf_state[A_EXBF_RD_ID].done=0;
                            A_EXBF_RD_ID+=1;
                            if(A_EXBF_RD_ID>=EXTRA_PCM_BUF_NUM)A_EXBF_RD_ID=0;
                        }

                        tick2=hal_TimGetUpTime();
                        diag_printf( "[VID]voc aud dec time=%d frame num=%d", (tick2-tick1)*1000/16384, pIsomGlob->a_HalfPcmBufFrame);
                        if(pIsomGlob->a_type!= A_TYPE_AMR)
                        {
                            diag_printf( "[VID] AUD real channel=%d SampleRate=%d BitRate=%d output_len=%d streamStatus=%d",
                                         vocDecStatus.nbChannel,
                                         vocDecStatus.SampleRate,
                                         vocDecStatus.BitRate,
                                         vocDecStatus.output_len,
                                         vocDecStatus.streamStatus);

                            if(vocDecStatus.nbChannel !=pIsomGlob->a_channels)
                            {
                                diag_printf( "[VID] warning! AUD channel not match");
                                if(vocDecStatus.nbChannel==1)
                                {
                                    pIsomGlob->a_channels=1;
                                    pIsomGlob->a_frameBytes>>=1;
                                    pIsomGlob->a_PcmBufHalfPos=pIsomGlob->a_PcmBuf+pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame;
                                }
                                else if(vocDecStatus.nbChannel==2)
                                {
                                    pIsomGlob->a_channels=2;
                                    pIsomGlob->a_frameBytes<<=1;
                                    pIsomGlob->a_PcmBufHalfPos=pIsomGlob->a_PcmBuf+pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame;
                                }
                                else
                                {
                                    res=ISOM_ERR_AUDIO_DEC_FAIL;
                                    goto play_error;
                                }
                            }
                            else if(vocDecStatus.SampleRate !=pIsomGlob->a_sample_rate)
                            {
                                diag_printf( "[VID] warning! AUD SampleRate not match");
                                if(vocDecStatus.SampleRate>=8000 && vocDecStatus.SampleRate<=48000)
                                    pIsomGlob->a_sample_rate=vocDecStatus.SampleRate;
                                else
                                {
                                    res=ISOM_ERR_AUDIO_DEC_FAIL;
                                    goto play_error;
                                }

                                if(pIsomGlob->a_type==A_TYPE_MP3 )
                                {
                                    if( pIsomGlob->a_frame_sample==MP3_FRAME_SAMPLE_576 && vocDecStatus.SampleRate>=32000)
                                    {
                                        pIsomGlob->a_frame_sample=MP3_FRAME_SAMPLE_1152;
                                        pIsomGlob->a_frameBytes<<=1;
                                        pIsomGlob->a_PcmBufHalfPos=pIsomGlob->a_PcmBuf+pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame;
                                    }
                                    else if( pIsomGlob->a_frame_sample==MP3_FRAME_SAMPLE_1152 && vocDecStatus.SampleRate<=24000)
                                    {
                                        pIsomGlob->a_frame_sample=MP3_FRAME_SAMPLE_576;
                                        pIsomGlob->a_frameBytes>>=1;
                                        pIsomGlob->a_PcmBufHalfPos=pIsomGlob->a_PcmBuf+pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame;
                                    }
                                }
                            }
                            else if(vocDecStatus.output_len !=pIsomGlob->a_frameBytes)
                            {
                                diag_printf( "[VID] warning! AUD output_len not match");
                                res=ISOM_ERR_AUDIO_DEC_FAIL;
                                goto play_error;
                            }
                        }
                    }
                    else if(pIsomGlob->a_type == A_TYPE_PCM)
                    {
                        if(i==0)
                            pPcmBufPos=(uint32)pIsomGlob->a_PcmBuf;
                        else if(i==1)
                        {
                            pPcmBufPos=(uint32)pIsomGlob->a_PcmBufHalfPos;
                            i=2;
                        }
                        AUD_DEC_FRM_NUM=pIsomGlob->a_HalfPcmBufFrame;
                        while(AUD_DEC_FRM_NUM)
                        {
                            res=getData(((unsigned char*)pPcmBufPos)+(pIsomGlob->a_HalfPcmBufFrame-(unsigned int)AUD_DEC_FRM_NUM)*pIsomGlob->a_frameBytes,
                                        pIsomGlob->a_frameBytes, (unsigned int)AUD_DEC_FRM_NUM);
                            if(res>0)
                            {
                                if((pIsomGlob->a_channels==2)&&(audioItf==AUD_ITF_LOUD_SPEAKER))
                                {
                                    INT16 *pPcm=(INT16 *)(((unsigned char*)pPcmBufPos)+(pIsomGlob->a_HalfPcmBufFrame-(unsigned int)AUD_DEC_FRM_NUM)*pIsomGlob->a_frameBytes);
                                    uint32 sNum=pIsomGlob->a_frame_sample*2*res;

                                    for(j=0; j<sNum; j+=2)
                                    {
                                        pPcm[j]=pPcm[j+1]=(pPcm[j]>>1)+(pPcm[j+1]>>1);
                                    }
                                }

                                AUD_DEC_FRM_NUM-=(uint16)res;
                                fillSampBuf();
                            }
                            else
                            {
                                diag_printf( "[VID] warning! AUD data end!");
                                AUD_DEC_FRM_NUM=0;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

#endif
    LAST_SYNC_AUD_FRM_NUM=pIsomGlob->a_current_frame_num;
    AUD_DEC_FRM_NUM=0;
    PCM_ISR_FLAG=0;
    VOC_WORK=VOC_WORK_NON;
    VOC_AUD=0;
    //AUD_EXTRA_BUF_DONE=1;
    //A_EXBF_RD_ID=0;
    //A_EXBF_WT_ID=0;
    //diag_printf("%x",aud_extra_pcmbuf_state[0].pBufPos);
    diag_printf("A_EXBF_WT_ID %d A_EXBF_RD_ID %d",A_EXBF_WT_ID,A_EXBF_RD_ID);
    //diag_printf(" %d %d %d",aud_extra_pcmbuf_state[0].done,aud_extra_pcmbuf_state[1].done,aud_extra_pcmbuf_state[2].done);

    fillAudioData=NULL;//not read audio in video decoder.

    //for video
    //if(pIsomGlob->open_audio!=2)
    {
        for(i=0; i<1; i++)
        {
            if(pIsomGlob->v_current_sample_num<pIsomGlob->v_total_sample_num)
            {
                pIsomGlob->v_SampleBuf_BytesRemain-=pIsomGlob->v_SampleBuf_BytesUsed;
                if ((pIsomGlob->v_type != V_TYPE_MPEG4)&&
                        (pIsomGlob->v_type !=V_TYPE_MJPEG)&&
                        (pIsomGlob->v_type !=V_TYPE_RV)&&
                        (pIsomGlob->v_SampleBuf_BytesUsed>0) &&
                        (pIsomGlob->v_SampleBuf_BytesRemain > 0) &&
                        pIsomGlob->v_SampleBuf_BytesRemain<pIsomGlob->v_SampleBufLen)
                    memcpy(pIsomGlob->v_SampleBuf , pIsomGlob->v_SampleBuf+pIsomGlob->v_SampleBuf_BytesUsed, pIsomGlob->v_SampleBuf_BytesRemain);

                pIsomGlob->v_SampleBuf_BytesUsed=0;
                if(pIsomGlob->v_SampleBuf_BytesRemain<0)pIsomGlob->v_SampleBuf_BytesRemain=0;

                if(pIsomGlob->med_type==MCI_TYPE_MP4 || pIsomGlob->med_type==MCI_TYPE_3GP)
                {
                    uint32 readLen;

                    if((res = stbl_GetSampleSize(pIsomGlob->v_info , pIsomGlob->v_current_sample_num+1, &readLen))!=GF_OK)
                    {
                        diag_printf("[VID] video stbl_GetSampleSize err res=%d v_current_sample_num=%d \n",res, pIsomGlob->v_current_sample_num);
                        res=ISOM_ERR_VIDEO_READ_FAIL;
                        goto play_error;
                    }

                    if(pIsomGlob->v_SampleBuf_BytesRemain+readLen<pIsomGlob->v_SampleBufLen)
                    {
                        uint32 gotbytes, gotcount;

                        if(pIsomGlob->v_type == V_TYPE_MPEG4)
                        {
                            if(pIsomGlob->v_LastFrmStates==1 && pIsomGlob->v_decSpeInfo &&
                                    (pIsomGlob->v_SampleBuf_BytesRemain+readLen+ pIsomGlob->v_decSpeInfo->dataLength)<pIsomGlob->v_SampleBufLen)
                            {
                                memcpy(pIsomGlob->v_SampleBuf, pIsomGlob->v_decSpeInfo->data, pIsomGlob->v_decSpeInfo->dataLength);
                                pIsomGlob->v_SampleBuf_BytesRemain =pIsomGlob->v_decSpeInfo->dataLength;
                            }
                            else if(pIsomGlob->v_lowres==0)
                            {
                                if(readLen> pIsomGlob->v_width*pIsomGlob->v_height)
                                    readLen= pIsomGlob->v_width*pIsomGlob->v_height;
                            }
                        }

                        res=gf_isom_get_multSample(pIsomGlob->v_info,
                                                   pIsomGlob->v_current_sample_num+1,
                                                   pIsomGlob->v_SampleBuf+pIsomGlob->v_SampleBuf_BytesRemain,
                                                   readLen,    &gotbytes, &gotcount);

                        if(res || (gotbytes<=0))
                        {
                            diag_printf("[VID] video gf_isom_get_sample err res=%d \n",res);
                            res=ISOM_ERR_VIDEO_READ_FAIL;
                            goto play_error;
                        }

                        //replace the 4bytes frame size with start code for h.264 only.
                        if(pIsomGlob->v_type==V_TYPE_H264)
                        {
                            uint8 *p;
                            uint32 chunkPos,chunkSize;

                            chunkPos=0;
                            //may be multi chunk
                            do
                            {
                                p=( pIsomGlob->v_SampleBuf+pIsomGlob->v_SampleBuf_BytesRemain+chunkPos);
                                chunkSize=0;
                                //get chunk size
                                chunkSize|=(*p)<<24;
                                chunkSize|=(*(p+1))<<16;
                                chunkSize|=(*(p+2))<<8;
                                chunkSize|=*(p+3);

                                //*((uint32*)p)=H264_START_CODE;
                                p[0]=0;
                                p[1]=0;
                                p[2]=0;
                                p[3]=1;//H264_START_CODE


                                chunkPos+=chunkSize+4;
                            }
                            while (chunkPos<gotbytes-4);
                        }

                        pIsomGlob->v_current_sample_num+=gotcount;
                        pIsomGlob->v_SampleBuf_BytesRemain+=gotbytes;
                    }
                    else
                    {
                        res=ISOM_ERR_MEM_INSUFFICIENT;
                        goto play_error;
                    }
                }
                else if(pIsomGlob->med_type==MCI_TYPE_MJPG)
                {
                    int32 key_frame;
                    res=avi_set_video_position(pIsomGlob->avi_p, pIsomGlob->v_current_sample_num);
                    if(res!=0)
                    {
                        diag_printf("[VID] avi_set_video_position err!res=%d", res);
                        res=ISOM_ERR_VIDEO_READ_FAIL;
                        goto play_error;
                    }

		    res=avi_frame_size(pIsomGlob->avi_p, pIsomGlob->v_current_sample_num);
                    if(pIsomGlob->v_type == V_TYPE_MJPEG)
                    {
			if(res<=608){
				//608 only jpg header size
				diag_printf("[VID] invalid MJPG size=%d",res);
				res=0;
			}
                    }

		    if(res>0){
			    res=avi_read_frame(pIsomGlob->avi_p, pIsomGlob->v_SampleBuf+pIsomGlob->v_SampleBuf_BytesRemain,
                                       pIsomGlob->v_SampleBufLen-pIsomGlob->v_SampleBuf_BytesRemain,  &key_frame);
		    }else{
			    res=0;
		    }
                    if( res < 0 )
                    {
                        diag_printf( "[VID] avi video frame read err res=%d \n", res );
                        res=ISOM_ERR_VIDEO_READ_FAIL;
                        goto play_error;
                    }

                    pIsomGlob->v_current_sample_num+=1;
                    pIsomGlob->v_SampleBuf_BytesRemain+=res;
                }
                else if(pIsomGlob->med_type==MCI_TYPE_AVSTRM)
                {
                    avinfostr *avinfohdl= (avinfostr*)pIsomGlob->filehdl;
                    //avframeinfohdstr frmhd;
                    //int readLen=sizeof(avframeinfohdstr);
                    avstream_clt_checkagent(avinfohdl->avstreamhdl_v);
                    //res=avstream_clt_getData(avinfohdl->avstreamhdl_v,(unsigned char*)&avStrmFrmHdV,readLen,1);
                    res=avstream_clt_getheader(avinfohdl->avstreamhdl_v,&avStrmFrmHdV);
                    if(res!=1)
                    {
                        diag_printf("[VID]avstream fream header read fail res=%d \n",res);
                        res=ISOM_ERR_VIDEO_READ_FAIL;
                        goto play_error;
                    }

                    if(avStrmTimStpStartV==0xFFFFFFFF)
                        avStrmTimStpStartV=avStrmFrmHdV.timestamp;

                    if(pIsomGlob->open_audio==1 && avinfohdl->avtimelineflag==1 && avStrmTimStpStartV!=0xFFFFFFFF && avStrmTimStpStartA!=0xFFFFFFFF)
                    {
                        //av same time line for time stamp reference.
                        if(avStrmTimStpStartV>avStrmTimStpStartA)
                        {
                            avStrmCurPosV= (unsigned int)((unsigned long long)(avStrmTimStpStartV- avStrmTimStpStartA)*(unsigned long long)pIsomGlob->a_sample_rate/(unsigned long long)avStrmBaseClkV);
                            avStrmTimStpStartV=avStrmTimStpStartA;
                        }
                    }

                    res=avstream_clt_getDataLen(avinfohdl->avstreamhdl_v);
                    if(res >= avStrmFrmHdV.samplesize && (pIsomGlob->v_SampleBuf_BytesRemain+avStrmFrmHdV.samplesize<pIsomGlob->v_SampleBufLen))
                    {
                        res=avstream_clt_getData(avinfohdl->avstreamhdl_v,pIsomGlob->v_SampleBuf ,avStrmFrmHdV.samplesize,1);
                        if(res!=1)
                        {
                            diag_printf("[VID]avstream fream read fail res=%d \n",res);
                            res=ISOM_ERR_VIDEO_READ_FAIL;
                            goto play_error;
                        }
                        pIsomGlob->v_current_sample_num+=1;
                        pIsomGlob->v_SampleBuf_BytesRemain+=avStrmFrmHdV.samplesize;
                        avStrmFrmHdV.samplesize=0;
                    }
                    else
                    {
                        diag_printf("[VID]avstream fream data not enough, frm size=%d remain=%d buf size=%d\n",avStrmFrmHdV.samplesize,res,pIsomGlob->v_SampleBufLen);
                        res=ISOM_ERR_VIDEO_READ_FAIL;
                        goto play_error;
                    }
                }
                else
                {
                    //rm to do...
                    pIsomGlob->v_SampleBuf_BytesRemain=1;
                }
            }

            if(pIsomGlob->v_SampleBuf_BytesRemain>0)
            {
                //int32 used_bytes;
                if(pIsomGlob->v_type== V_TYPE_MPEG4)
                {
#ifdef MPEG4_SUPPORT
                    int32 used_bytes;
                    xvid_dec_stats_t xvid_dec_stats;
                    while(pIsomGlob->v_SampleBuf_BytesRemain > MIN_USEFUL_BYTES)
                    {

                        vocWorkState.pVOC_WORK=(UINT8*)&VOC_WORK;
                        vocWorkState.pVOC_VID=(int16*)&VOC_VID;
                        vocWorkState.pVOC_AUD =(int16*)&VOC_AUD;
                        vocWorkState.vid_display= NULL;
                        vocWorkState.vid_voc_isr=NULL;
                        vocWorkState.aud_nframe_dec=NULL;
                        vocWorkState.vid_frame_read=NULL;

                        used_bytes = dec_main(pIsomGlob->v_SampleBuf+pIsomGlob->v_SampleBuf_BytesUsed,
                                              (uint8*)pIsomGlob->v_YuvInfo,
                                              pIsomGlob->v_SampleBuf_BytesRemain, &xvid_dec_stats, NULL,
                                              ((pIsomGlob->med_type ==MCI_TYPE_MJPG) ? TRUE : FALSE));

                        if(used_bytes == -1 ||pIsomGlob->v_LastFrmStates==1)//profile unfit
                        {
                            diag_printf("[VID] video decode err v_current_sample_num=%d \n",pIsomGlob->v_current_sample_num);
                            pIsomGlob->bSkipFrame = TRUE;
                            pIsomGlob->v_LastFrmStates=1;
                            pIsomGlob->v_SampleBuf_BytesUsed=pIsomGlob->v_SampleBuf_BytesRemain;
                            res=ISOM_ERR_VIDEO_DEC_FAIL;
                            goto play_error;
                        }

                        if(used_bytes == -2)
                        {
                            // to skip a B-frame
                            pIsomGlob->bSkipFrame = TRUE;
                            pIsomGlob->v_LastFrmStates=0;
                            pIsomGlob->v_SampleBuf_BytesUsed=pIsomGlob->v_SampleBuf_BytesRemain;
                        }

                        if(used_bytes > 0)
                        {
                            pIsomGlob->v_LastFrmStates=0;
                            if(xvid_dec_stats.type == XVID_TYPE_VOL)
                            {
                                pIsomGlob->v_SampleBuf_BytesUsed+=used_bytes;
                                pIsomGlob->v_SampleBuf_BytesRemain-=used_bytes;
                                continue;
                            }

                            pIsomGlob->v_SampleBuf_BytesUsed+=pIsomGlob->v_SampleBuf_BytesRemain;
                        }

                        break;
                    }
                    if(pIsomGlob->v_SampleBuf_BytesRemain==MIN_USEFUL_BYTES) pIsomGlob->v_SampleBuf_BytesUsed=1;
#endif
                }
#ifdef H263_SUPPORT
                else if(pIsomGlob->v_type== V_TYPE_H263)
                {
                    int32 used_bytes;
                    int32 gob;
                    resetbits (pIsomGlob->v_SampleBuf+pIsomGlob->v_SampleBuf_BytesUsed, pIsomGlob->v_SampleBuf_BytesRemain);

                    gob= getheader ();
                    if(errorstate)
                    {
                        diag_printf("[VID] h.263 getheader err gob=%d \n",gob);
                        res= ISOM_ERR_INVALID_FORMAT;
                        goto play_error;
                    }

                    if (!gob )
                    {
                        diag_printf("[VID] h.263 getheader err !data use=%d \n", ld->rdptr - ld->rdbfr);
                        res= ISOM_ERR_INVALID_FORMAT;
                        goto play_error;
                    }

                    vocWorkState.pVOC_WORK=(UINT8*)&VOC_WORK;
                    vocWorkState.pVOC_VID=(int16*)&VOC_VID;
                    vocWorkState.pVOC_AUD =(int16*)&VOC_AUD;
                    vocWorkState.vid_display= NULL;
                    vocWorkState.vid_voc_isr=NULL;
                    vocWorkState.aud_nframe_dec=NULL;
                    vocWorkState.vid_frame_read=NULL;

                    getpicture ((uint8*)pIsomGlob->v_YuvInfo, pIsomGlob->v_lcd_size_x, pIsomGlob->v_lcd_start_x, pIsomGlob->v_lcd_start_y, gob, NULL);
                    if(errorstate!=0)
                    {
                        diag_printf("[VID] h.263 getpicture err! \n");
                        res= ISOM_ERR_VIDEO_DEC_FAIL;
                        goto play_error;
                    }

                    used_bytes=ld->rdptr - ld->rdbfr;
                    /* Update buffer pointers */

                    if(used_bytes > 0)
                    {
                        pIsomGlob->v_SampleBuf_BytesUsed+=used_bytes;
                    }
                }
#endif
#ifdef H264_SUPPORT
                else if( pIsomGlob->v_type==V_TYPE_H264)
                {
                    int32 got_picture=0;

                    vocWorkState.pVOC_WORK=(UINT8*)&VOC_WORK;
                    vocWorkState.pVOC_VID=(int16*)&VOC_VID;
                    vocWorkState.pVOC_AUD =(int16*)&VOC_AUD;
                    vocWorkState.vid_display= NULL;
                    vocWorkState.vid_voc_isr=NULL;
                    vocWorkState.aud_nframe_dec=NULL;
                    vocWorkState.vid_frame_read=NULL;

                    diag_printf("pIsomGlob->v_SampleBuf_BytesRemain=%d",pIsomGlob->v_SampleBuf_BytesRemain);

                    res=avcodec_decode_video(pIsomGlob->pH264Hdl->codec_context, pIsomGlob->pH264Hdl->frame_picture,
                                             &got_picture, pIsomGlob->v_SampleBuf+pIsomGlob->v_SampleBuf_BytesUsed, pIsomGlob->v_SampleBuf_BytesRemain);
                    if(res<0 || !got_picture)
                    {
                        diag_printf("[VID] h.264 decode err! res=%d\n",res);
                        res= ISOM_ERR_VIDEO_DEC_FAIL;
                        goto play_error;
                    }
                    pIsomGlob->v_SampleBuf_BytesUsed+=res;
                    diag_printf("[VID] h.264 decode consume size=%d got_picture=%d type=%d\n",res,got_picture,pIsomGlob->pH264Hdl->frame_picture->pict_type);
                    pIsomGlob->v_YuvInfo[0]=(unsigned int)pIsomGlob->pH264Hdl->frame_picture->data[0];
                    pIsomGlob->v_YuvInfo[1]=(unsigned int)pIsomGlob->pH264Hdl->frame_picture->data[1];
                    pIsomGlob->v_YuvInfo[2]=(unsigned int)pIsomGlob->pH264Hdl->frame_picture->data[2];
                    pIsomGlob->v_YuvInfo[3]=pIsomGlob->pH264Hdl->frame_picture->linesize[0];
                    pIsomGlob->v_YuvInfo[4]=pIsomGlob->pH264Hdl->codec_context->height;
                }
#endif
                else if(pIsomGlob->v_type== V_TYPE_MJPEG)
                {
                    DEC_UNIT jpgDec;

                    vocWorkState.pVOC_WORK=(UINT8*)&VOC_WORK;
                    vocWorkState.pVOC_VID=(int16*)&VOC_VID;
                    vocWorkState.pVOC_AUD =(int16*)&VOC_AUD;
                    vocWorkState.vid_display= NULL;
                    vocWorkState.vid_voc_isr=NULL;
                    vocWorkState.aud_nframe_dec=NULL;
                    vocWorkState.vid_frame_read=NULL;

                    vid_buf_id= 1-vid_buf_id;
                    res= MMF_JpegDecodePicture((long)((char*)pIsomGlob->v_SampleBuf), (char*) pVid_buf_Y[vid_buf_id],
                                               (pIsomGlob->v_width*pIsomGlob->v_height*3)>>1,
                                               pIsomGlob->v_width, pIsomGlob->v_height,
                                               0,0,
                                               pIsomGlob->v_width-1, pIsomGlob->v_height-1,
                                               &jpgDec, 2,0);
                    if(res!=FUNC_OK)
                    {
                        diag_printf("[VID] Mjpeg MMF_JpegDecodePicture err!res=%d", res);
                        //res= ISOM_ERR_VIDEO_DEC_FAIL;
                        //goto play_error;
                        pIsomGlob->bSkipFrame = TRUE;
                    }
                    pIsomGlob->v_YuvInfo[0]=(uint32)pVid_buf_Y[vid_buf_id];
                    pIsomGlob->v_YuvInfo[1]=(uint32)pVid_buf_U[vid_buf_id];
                    pIsomGlob->v_YuvInfo[2]=(uint32)pVid_buf_V[vid_buf_id];
                    pIsomGlob->v_YuvInfo[3]=(uint32)jpgDec.width;
                    pIsomGlob->v_YuvInfo[4]=(uint32)jpgDec.height;
                    pIsomGlob->v_SampleBuf_BytesUsed+=pIsomGlob->v_SampleBuf_BytesRemain;
                }

#ifdef RM_SUPPORT
                else if( pIsomGlob->v_type==V_TYPE_RV)
                {
                    //VocWorkStruct vocState;
                    vocWorkState.pVOC_WORK=(UINT8*)&VOC_WORK;
                    vocWorkState.pVOC_VID=(int16*)&VOC_VID;
                    vocWorkState.vid_voc_isr=vid_voc_isr;
                    vocWorkState.aud_nframe_dec=NULL;
                    vocWorkState.vid_frame_read=NULL;

                    res=rm_getOneFrameVideo((INT8*)pIsomGlob->v_YuvInfo, /*&vocWorkState,*/ 0);
                    if(res==HXR_SKIP_BFRAME)
                    {
                        pIsomGlob->bSkipFrame = TRUE;
                    }
                    else if(res!=HXR_OK)
                    {
                        diag_printf("[VID] rm_getOneFrameVideo err!res=%d ", res);
                        res= ISOM_ERR_VIDEO_DEC_FAIL;
                        goto play_error;
                    }

                    pIsomGlob->v_SampleBuf_BytesUsed=1;
                }
#endif

                else
                {
                    diag_printf("[VID] err pIsomGlob->v_type=%d !!!\n",pIsomGlob->v_type);
                    res=ISOM_ERR_UNSUPPORT;
                    goto play_error;
                }
            }
            else
            {

                pIsomGlob->bSkipFrame = TRUE;

            }

            if( pIsomGlob->bSkipFrame == FALSE )
            {
                pIsomGlob->v_ImageBuf_HalfFlag=i;
            }

        }
    }

    diag_printf("[VID] v_current_frame_num= %d v_current_sample_num=%d \n",pIsomGlob->v_current_frame_num, pIsomGlob->v_current_sample_num);

#if 1
    if(audioItf!=AUD_ITF_EAR_PIECE&&audioItf!=AUD_ITF_LOUD_SPEAKER)
    {
        // Initial cfg
        diag_printf("audioItf=%d invalid. set to AUD_ITF_LOUD_SPEAKER");
        audioItf = AUD_ITF_LOUD_SPEAKER;
    }

    audioStream.startAddress = (UINT32*)pIsomGlob->a_PcmBuf;
    audioStream.length= pIsomGlob->a_frameBytes*(pIsomGlob->a_HalfPcmBufFrame<<1);
    audioStream.channelNb = pIsomGlob->a_channels;
    audioStream.voiceQuality = FALSE;
    audioStream.playSyncWithRecord = FALSE;
    audioStream.halfHandler = vid_pcmHalf_isr;
    audioStream.endHandler = vid_pcmEnd_isr;

    switch (pIsomGlob->a_sample_rate)
    {
        case 48000:
            audioStream.sampleRate = HAL_AIF_FREQ_48000HZ;
            break;
        case 44100:
            audioStream.sampleRate = HAL_AIF_FREQ_44100HZ;
            break;
        case 32000:
            audioStream.sampleRate = HAL_AIF_FREQ_32000HZ;
            break;
        case 24000:
            audioStream.sampleRate = HAL_AIF_FREQ_24000HZ;
            break;
        case 22050:
            audioStream.sampleRate = HAL_AIF_FREQ_22050HZ;
            break;
        case 16000:
            audioStream.sampleRate = HAL_AIF_FREQ_16000HZ;
            break;
        case 12000:
            audioStream.sampleRate = HAL_AIF_FREQ_12000HZ;
            break;
        case 11025:
            audioStream.sampleRate = HAL_AIF_FREQ_11025HZ;
            break;
        case 8000:
            audioStream.sampleRate = HAL_AIF_FREQ_8000HZ;
            break;
        default:
            diag_printf( "##WARNING## invalid a_sample_rate=%d\n",pIsomGlob->a_sample_rate);
            res= ISOM_ERR_INVALID_FORMAT;
            goto play_error;
    }

#if 1
    if(pIsomGlob->open_audio)
    {
        diag_printf( "[VID]open audio, audioItf=%d",audioItf);
        if((MpegPlayer.MpegInput.bt_sco==TRUE)&&(pIsomGlob->a_type != A_TYPE_AMR))
        {
            g_VidBtNumOfOutSample=ResampleInit(pIsomGlob->a_sample_rate, HAL_AIF_FREQ_8000HZ, pIsomGlob->a_frameBytes*pIsomGlob->a_bit/8, pIsomGlob->a_channels);
            g_VidBtTotalFrame = pIsomGlob->a_HalfPcmBufFrame;
            if(g_VidBT8kPcmBuffer == NULL)
                g_VidBT8kPcmBuffer=(INT16 *)mmc_MemMalloc(g_VidBtTotalFrame*g_VidBtNumOfOutSample*pIsomGlob->a_bit);

            if(!g_VidBT8kPcmBuffer)
            {
                res= ISOM_ERR_MALLOC_FAIL;
                goto play_error;

            }
            memset(g_VidBT8kPcmBuffer, 0, VID_BT_SCO_8K_OUTBUFF_SIZE);

            g_VidBtOutputPcmBufSize =pIsomGlob->a_frameBytes;
            //g_VidBtNumOfOutSample=ResampleInit(pIsomGlob->a_frame_sample, HAL_AIF_FREQ_8000HZ, VID_BT_SCO_8K_OUTBUFF_SIZE>>1, audioStream.channelNb);
            //pIsomGlob->a_sample_rate = HAL_AIF_FREQ_8000HZ;
            audioStream.startAddress  = (UINT32*) g_VidBT8kPcmBuffer;//pAudioPlayer->MpegOutput.pcm.Buffer;
            audioStream.length        = g_VidBtTotalFrame*g_VidBtNumOfOutSample*pIsomGlob->a_bit/8;
            audioStream.sampleRate    = HAL_AIF_FREQ_8000HZ;//HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_8000HZ;
            audioStream.channelNb     = HAL_AIF_MONO;//HAL_AIF_STEREO;//HAL_AIF_MONO;
            audioStream.voiceQuality   = TRUE;
            audioItf=AUD_ITF_BLUETOOTH;
        }
        else if((MpegPlayer.MpegInput.bt_sco==TRUE)&&(pIsomGlob->a_type == A_TYPE_AMR))
        {
            audioItf=AUD_ITF_BLUETOOTH;
            audioStream.sampleRate    = HAL_AIF_FREQ_8000HZ;//HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_8000HZ;
            audioStream.channelNb     = HAL_AIF_MONO;//HAL_AIF_STEREO;//HAL_AIF_MONO;
            audioStream.voiceQuality   = TRUE;
        }
        if((res=aud_StreamStart(audioItf, &audioStream, &audio_cfg))!=AUD_ERR_NO)
        {
            diag_printf( "[VID]##WARNING## first aud_StreamStart fail res= %d \n",res);
            aud_StreamStop(audioItf);
            if((res=aud_StreamStart(audioItf, &audioStream, &audio_cfg))!=AUD_ERR_NO)
            {
                diag_printf( "[VID]##ERROR## aud_StreamStart res= %d \n",res);
                res=ISOM_ERR_PCM_OPEN_FAIL;
                goto play_error;
            }
        }
        AUD_DATA_REQ=1;
    }
    else
    {
        // record the start time for time bar and video sync
        if(pIsomGlob->med_type!=MCI_TYPE_RM)
            TIME_LINE_LAST=hal_TimGetUpTime();
        else
        {
            TIME_LINE_CUR=pIsomGlob->current_time;
            TIME_LINE_LAST=hal_TimGetUpTime();
        }
        AUD_DATA_REQ=0;
    }
#endif

#endif

#if 1 //display video or not

    //res=COS_SetTimerEX(GetMCITaskHandle(MBOX_ID_MMC), MMC_VID_PLY_TIMER_ID, COS_TIMER_MODE_PERIODIC, pIsomGlob->v_timerPeriod);
    //if(pIsomGlob->open_audio!=2)
    {
        cur_timer= pIsomGlob->v_timerPeriod;
        if(DTS_FLG==0xFFFFFFFF)
        {
            res=COS_SetTimerEX(GetMCITaskHandle(MBOX_ID_MMC), MMC_VID_PLY_TIMER_ID, COS_TIMER_MODE_PERIODIC, pIsomGlob->v_timerPeriod);
        }
        else
        {
            uint32  dts_sampCount;
            uint32 lastSanpNum=( pIsomGlob->v_current_sample_num -1 >0)? (pIsomGlob->v_current_sample_num -1) : 1;
            if((res=stbl_GetSampleDTS(pIsomGlob->v_info->sampleTable->TimeToSample, lastSanpNum,
                                      &DTS_FLG, &dts_sampCount, &DTS_SampDelta))==0)
            {
                uint32 t;
                if(!pIsomGlob->v_timeScale)
                {
                    diag_printf("[VID] err pIsomGlob->v_timeScale=%d !!!\n",pIsomGlob->v_timeScale);
                    res=ISOM_ERR_PARAM_ERROR;
                    goto play_error;
                }
                //DTS_FLG = ((unsigned long long )DTS_FLG*1000+ (unsigned long long )(pIsomGlob->v_timeScale>>1))/pIsomGlob->v_timeScale;
                DTS_NextSampNode = pIsomGlob->v_info->sampleTable->TimeToSample->r_FirstSampleInEntry + dts_sampCount -1;
                //DTS_SampDelta = ((unsigned long long )dts_sampDelta*1000 + (pIsomGlob->v_timeScale>>1))/(unsigned long long )pIsomGlob->v_timeScale;
                t= ((unsigned long long )DTS_SampDelta*16384 + (pIsomGlob->v_timeScale>>1))/(unsigned long long )pIsomGlob->v_timeScale;
                if(t>0)
                    res=COS_SetTimerEX(GetMCITaskHandle(MBOX_ID_MMC), MMC_VID_PLY_TIMER_ID, COS_TIMER_MODE_PERIODIC, t);
                else
                    res=COS_SetTimerEX(GetMCITaskHandle(MBOX_ID_MMC), MMC_VID_PLY_TIMER_ID, COS_TIMER_MODE_PERIODIC, pIsomGlob->v_timerPeriod);
                diag_printf("[VID]DTS_FLG=%d count=%d delta=%d", DTS_FLG, dts_sampCount, DTS_SampDelta);
            }
            else
            {
                res=COS_SetTimerEX(GetMCITaskHandle(MBOX_ID_MMC), MMC_VID_PLY_TIMER_ID, COS_TIMER_MODE_PERIODIC, pIsomGlob->v_timerPeriod);
                DTS_NextSampNode =0;
                diag_printf("[VID]##WARNING## DTS err !\n");
            }

        }
        diag_printf( "[VID]timer set result= %d \n",res);
    }

    VID_DEC_REQ=0;
    SEND_EVENT_NUM=0;
    DISPLAY_SEND_EVENT_NUM=0;
    keep_skip_frame_num=0;
    //skip_display_flag=0;

#ifdef USE_KEY_SYNC
    KEY_SAMP_NUM=0;
    KEY_SAMP_TIME_WAIT=0;
    KEY_SAMP_TIME_START=0;
#endif

#endif

    vid_last_sample_num= pIsomGlob->v_current_sample_num;
    vid_SampleBuf_id= 0;
    vid_SampleBuf_BytesRemain= 0;
    vid_SampleBuf_BytesUsed= 0;

    play_state=VID_PLAY_STATE_PLAYING;
    diag_printf("[VID] mmc_vid_isom_play OK!");
    return ISOM_ERR_OK;

play_error:
    diag_printf("[VID] ##WARNING## mmc_vid_isom_play err res= %d \n",res);
    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_STOP_REQ);
    return res;
}

int32 mmc_vid_isom_seek ( uint32    time, int16 time_mode, int16 lcd_start_x, int16 lcd_start_y)
{
    int32 res;
    int32 used_bytes;
    unsigned long long temp;
	
	
#ifdef __USE_LCD_FMARK__
	while(hal_GoudaIsActive());
#endif
    if((play_state!=VID_PLAY_STATE_IDLE) && (play_state!=VID_PLAY_STATE_PAUSE) )
    {
        diag_printf("[VID] mmc_vid_isom_seek err play_state= %d \n",play_state);
        return  ISOM_ERR_PLAY_STATE_FAIL;
    }


    //pIsomGlob->v_lcd_start_x=lcd_start_x;//set by mmc_vid_isom_set_mode
    //pIsomGlob->v_lcd_start_y=lcd_start_y;
    pIsomGlob->v_lcd_end_x=pIsomGlob->v_lcd_start_x+pIsomGlob->v_display_width-1;
    pIsomGlob->v_lcd_end_y=pIsomGlob->v_lcd_start_y+pIsomGlob->v_display_height-1;

    if(time_mode==SEEK_TIME_MODE_OFFSET)
    {
        pIsomGlob->current_time+=time;
    }
    else if(time_mode==SEEK_TIME_MODE_ABSOLUTE)//default
    {
        pIsomGlob->current_time=time;
    }
    diag_printf("[VID] v_lcd_start_x=%d v_lcd_end_x=%d",pIsomGlob->v_lcd_start_x, pIsomGlob->v_lcd_end_x);
    diag_printf("[VID] v_lcd_start_y=%d v_lcd_end_y=%d",pIsomGlob->v_lcd_start_y, pIsomGlob->v_lcd_end_y);
    diag_printf("[VID] seek time=%d \n",pIsomGlob->current_time);

    if(pIsomGlob->current_time<0)
    {
        pIsomGlob->current_time=0;
    }
    else if(pIsomGlob->current_time>pIsomGlob->total_time)
    {
        pIsomGlob->current_time=pIsomGlob->total_time;
    }

    temp=(unsigned long long)pIsomGlob->current_time*(unsigned long long)pIsomGlob->v_fps/(1000<<V_FPS_SCALE_BIT);
    if(DTS_FLG!=0xFFFFFFFF )
    {
        uint32  dts_sampCount, samptime;

        while(temp !=0)
        {
            if((res=stbl_GetSampleDTS(pIsomGlob->v_info->sampleTable->TimeToSample, temp +1,
                                      &DTS_FLG, &dts_sampCount, &DTS_SampDelta))==0)
            {
                samptime= ((unsigned long long )DTS_FLG*1000+ (unsigned long long )(pIsomGlob->v_timeScale>>1))/pIsomGlob->v_timeScale;
                if(samptime > pIsomGlob->current_time)
                    temp--;
                else
                    break;
            }
        }

        while(temp <pIsomGlob->v_total_sample_num)
        {
            if((res=stbl_GetSampleDTS(pIsomGlob->v_info->sampleTable->TimeToSample, temp +1,
                                      &DTS_FLG, &dts_sampCount, &DTS_SampDelta))==0)
            {
                samptime= ((unsigned long long )DTS_FLG*1000+ (unsigned long long )(pIsomGlob->v_timeScale>>1))/pIsomGlob->v_timeScale;
                if(samptime < pIsomGlob->current_time)
                    temp++;
                else
                    break;
            }
        }
    }
    pIsomGlob->v_current_frame_num=(uint32)temp;
    pIsomGlob->v_current_sample_num=pIsomGlob->v_current_frame_num;//wiil change (frame per sample ?)

    //get key frame number
    if(pIsomGlob->v_current_sample_num<pIsomGlob->v_total_sample_num)
    {
        if(pIsomGlob->med_type==MCI_TYPE_MP4 || pIsomGlob->med_type==MCI_TYPE_3GP)
            pIsomGlob->v_current_sample_num=gf_isom_get_keySampleNumber(pIsomGlob->v_trak, pIsomGlob->v_current_sample_num );
        else if(pIsomGlob->med_type==MCI_TYPE_MJPG && pIsomGlob->v_type!=V_TYPE_MJPEG)
            pIsomGlob->v_current_sample_num=avi_get_video_keyframe_num(pIsomGlob->avi_p, pIsomGlob->v_current_sample_num);
    }
    else
    {
        pIsomGlob->v_current_sample_num=(pIsomGlob->v_total_sample_num==0)?0:(pIsomGlob->v_total_sample_num-1);
        if(pIsomGlob->med_type==MCI_TYPE_MP4 || pIsomGlob->med_type==MCI_TYPE_3GP)
            pIsomGlob->v_current_sample_num=gf_isom_get_keySampleNumber(pIsomGlob->v_trak, pIsomGlob->v_current_sample_num );
        else if(pIsomGlob->med_type==MCI_TYPE_MJPG && pIsomGlob->v_type!=V_TYPE_MJPEG)
            pIsomGlob->v_current_sample_num=avi_get_video_keyframe_num(pIsomGlob->avi_p, pIsomGlob->v_current_sample_num);
    }

    //update seek frame number and time
    pIsomGlob->v_current_frame_num=pIsomGlob->v_current_sample_num;//wiil change (frame per sample ?)
    if(pIsomGlob->med_type==MCI_TYPE_MP4 || pIsomGlob->med_type==MCI_TYPE_3GP
            || (pIsomGlob->med_type==MCI_TYPE_MJPG && pIsomGlob->v_type!=V_TYPE_MJPEG))
    {
        if(!pIsomGlob->v_fps)
        {
            diag_printf("[VID] err pIsomGlob->v_fps=%d !!!\n",pIsomGlob->v_fps);
            return ISOM_ERR_PARAM_ERROR;
        }
        if(DTS_FLG==0xFFFFFFFF)
        {
            temp=(((unsigned long long)pIsomGlob->v_current_frame_num*1000)<<V_FPS_SCALE_BIT)/(unsigned long long)pIsomGlob->v_fps;
            pIsomGlob->current_time=(uint32)temp;
        }
        else
        {
            uint32  dts_sampCount;
            if((res=stbl_GetSampleDTS(pIsomGlob->v_info->sampleTable->TimeToSample, pIsomGlob->v_current_sample_num +1,
                                      &DTS_FLG, &dts_sampCount, &DTS_SampDelta))==0)
            {
                pIsomGlob->current_time= ((unsigned long long )DTS_FLG*1000+ (unsigned long long )(pIsomGlob->v_timeScale>>1))/pIsomGlob->v_timeScale;
            }
        }
    }
    diag_printf("[VID] new position key framenum=%d \n", pIsomGlob->v_current_frame_num);
    diag_printf("[VID] new position current_time=%d \n", pIsomGlob->current_time);

    pIsomGlob->v_SampleBuf_BytesRemain-=pIsomGlob->v_SampleBuf_BytesUsed;
    if ((pIsomGlob->v_SampleBuf_BytesUsed>0) &&(pIsomGlob->v_SampleBuf_BytesRemain > 0) && pIsomGlob->v_SampleBuf_BytesRemain<pIsomGlob->v_SampleBufLen)
        memcpy(pIsomGlob->v_SampleBuf , pIsomGlob->v_SampleBuf+pIsomGlob->v_SampleBuf_BytesUsed, pIsomGlob->v_SampleBuf_BytesRemain);

    pIsomGlob->v_SampleBuf_BytesUsed=0;
    if(pIsomGlob->v_SampleBuf_BytesRemain<0)pIsomGlob->v_SampleBuf_BytesRemain=0;

    tick1=hal_TimGetUpTime();
    switch(pIsomGlob->med_type)
    {
        case MCI_TYPE_3GP:
        case MCI_TYPE_MP4:
        {
            uint32 readLen, gotbytes, gotcount,lastkey;
            while(1)
            {
                if((res = stbl_GetSampleSize(pIsomGlob->v_info , pIsomGlob->v_current_sample_num+1, &readLen))!=GF_OK)
                {
                    diag_printf("[VID] ERR stbl_GetSampleSize err res=%d v_current_sample_num=%d \n",res, pIsomGlob->v_current_sample_num);
                    if(pIsomGlob->v_current_sample_num> 0)
                    {
                        lastkey= pIsomGlob->v_current_sample_num;
                        do
                        {
                            pIsomGlob->v_current_sample_num -=1;
                            res= gf_isom_get_keySampleNumber(pIsomGlob->v_trak, pIsomGlob->v_current_sample_num );
                        }
                        while(res >= lastkey && pIsomGlob->v_current_sample_num >0);

                        if(pIsomGlob->v_current_sample_num!=0)
                            pIsomGlob->v_current_sample_num= res;
                        continue;
                    }
                    else
                        return ISOM_ERR_VIDEO_READ_FAIL;

                }

                if(pIsomGlob->v_SampleBuf_BytesRemain+readLen<pIsomGlob->v_SampleBufLen)
                {
                    if(pIsomGlob->v_type == V_TYPE_MPEG4)
                    {
                        if(pIsomGlob->v_LastFrmStates==1 && pIsomGlob->v_decSpeInfo &&
                                (pIsomGlob->v_SampleBuf_BytesRemain+readLen+ pIsomGlob->v_decSpeInfo->dataLength)<pIsomGlob->v_SampleBufLen)
                        {

                            memcpy(pIsomGlob->v_SampleBuf, pIsomGlob->v_decSpeInfo->data, pIsomGlob->v_decSpeInfo->dataLength);
                            pIsomGlob->v_SampleBuf_BytesRemain =pIsomGlob->v_decSpeInfo->dataLength;

                        }
                        else if(pIsomGlob->v_lowres==0)
                        {
                            if(readLen> pIsomGlob->v_width*pIsomGlob->v_height)
                                readLen= pIsomGlob->v_width*pIsomGlob->v_height;

                        }
                    }

                    res=gf_isom_get_multSample(pIsomGlob->v_info,
                                               pIsomGlob->v_current_sample_num+1,
                                               pIsomGlob->v_SampleBuf+pIsomGlob->v_SampleBuf_BytesRemain,
                                               readLen,    &gotbytes, &gotcount);

                    if(res || (gotbytes<=0))
                    {
                        diag_printf("[VID] ERR video gf_isom_get_sample  res=%d gotbytes=%d smp num=%d\n",res, gotbytes, pIsomGlob->v_current_sample_num);

                        if(pIsomGlob->v_current_sample_num> 0)
                        {
                            lastkey= pIsomGlob->v_current_sample_num;

                            do
                            {
                                pIsomGlob->v_current_sample_num -=1;
                                res= gf_isom_get_keySampleNumber(pIsomGlob->v_trak, pIsomGlob->v_current_sample_num );
                            }
                            while(res >= lastkey && pIsomGlob->v_current_sample_num >0);

                            if(pIsomGlob->v_current_sample_num!=0)
                                pIsomGlob->v_current_sample_num= res;
                            pIsomGlob->v_SampleBuf_BytesRemain=0;

                            continue;
                        }
                        else
                            return ISOM_ERR_VIDEO_READ_FAIL;

                    }
                    else
                    {

                        //replace the 4bytes frame size with start code for h.264 only.
                        if(pIsomGlob->v_type==V_TYPE_H264)
                        {
                            uint8 *p;
                            uint32 chunkPos,chunkSize;

                            chunkPos=0;
                            //may be multi chunk
                            do
                            {
                                p=( pIsomGlob->v_SampleBuf+pIsomGlob->v_SampleBuf_BytesRemain+chunkPos);
                                chunkSize=0;
                                //get chunk size
                                chunkSize|=(*p)<<24;
                                chunkSize|=(*(p+1))<<16;
                                chunkSize|=(*(p+2))<<8;
                                chunkSize|=*(p+3);

                                //*((uint32*)p)=H264_START_CODE;
                                p[0]=0;
                                p[1]=0;
                                p[2]=0;
                                p[3]=1;//H264_START_CODE


                                chunkPos+=chunkSize+4;
                            }
                            while (chunkPos<gotbytes-4);
                        }

                        pIsomGlob->v_current_sample_num+=gotcount;
                        pIsomGlob->v_SampleBuf_BytesRemain+=gotbytes;
                    }

                    break;
                }
                else
                {
                    diag_printf("[VID] err MEM_INSUFFICIENT readLen=%d \n", readLen);
                    return ISOM_ERR_MEM_INSUFFICIENT;
                }
            }
        }
        break;

        case MCI_TYPE_MJPG:
        {
            int32 key_frame;
            do
            {
                res=avi_set_video_position(pIsomGlob->avi_p, pIsomGlob->v_current_sample_num);
                if(res!=0)
                {
                    diag_printf("[VID] avi_set_video_position err!");
                    //return ISOM_ERR_VIDEO_READ_FAIL;
                    break;
                }

		res=avi_frame_size(pIsomGlob->avi_p, pIsomGlob->v_current_sample_num);
                if(pIsomGlob->v_type == V_TYPE_MJPEG)
                {
		    if(res<=608){
			    //608 only jpg header size
			    diag_printf("[VID] invalid MJPG size=%d",res);
			    res=0;
		    }
                }

		if(res>0){
			res=avi_read_frame(pIsomGlob->avi_p, pIsomGlob->v_SampleBuf, pIsomGlob->v_SampleBufLen,  &key_frame);
		}else{
			res=0;
		}
                if( res < 0 )
                {
                    diag_printf( "[VID]MJPG avi video frame read err res=%d \n", res );
                    //return ISOM_ERR_VIDEO_READ_FAIL;
                    break;
                }
                pIsomGlob->v_current_sample_num+=1;
                pIsomGlob->v_SampleBuf_BytesRemain+=res;
            }
            while(res==0 && pIsomGlob->v_current_sample_num < pIsomGlob->v_total_sample_num);

            break;
        }

#ifdef RM_SUPPORT
        case MCI_TYPE_RM:
        {
            res=rm_seek( time);

            if(res!=HXR_OK)
            {
                diag_printf("[VID]rm_seek err res=%d\n", res);
                return ISOM_ERR_VIDEO_READ_FAIL;
            }
            //get real time offset.
            pIsomGlob->current_time=rm_getKeyTimeStamp();
            while(pIsomGlob->current_time<time)
            {
                time=pIsomGlob->current_time;
                res=rm_seek( pIsomGlob->current_time);
                if(res!=HXR_OK)
                {
                    diag_printf("[VID]rm_seek err res=%d\n", res);
                    return ISOM_ERR_VIDEO_READ_FAIL;
                }

                pIsomGlob->current_time=rm_getKeyTimeStamp();

                if(pIsomGlob->current_time<=0)break;
            }
            temp=(unsigned long long)pIsomGlob->current_time*(unsigned long long)pIsomGlob->v_fps/(1000<<V_FPS_SCALE_BIT);
            pIsomGlob->v_current_frame_num=(uint32)temp;
            pIsomGlob->v_current_sample_num=pIsomGlob->v_current_frame_num;

            pIsomGlob->v_SampleBuf_BytesRemain=1;
        }
        break;
#endif

        case MCI_TYPE_AVSTRM:
        {
            avinfostr *avinfohdl= (avinfostr*)pIsomGlob->filehdl;
            //avframeinfohdstr frmhd;
            //int readLen=sizeof(avframeinfohdstr);
            avstream_clt_checkagent(avinfohdl->avstreamhdl_v);
            //res=avstream_clt_getData(avinfohdl->avstreamhdl_v,(unsigned char*)&avStrmFrmHdV,readLen,1);
            res=avstream_clt_getheader(avinfohdl->avstreamhdl_v,&avStrmFrmHdV);
            if(res!=1)
            {
                diag_printf("[VID]avstream fream header read fail res=%d \n",res);
                return ISOM_ERR_VIDEO_READ_FAIL;
            }

            res=avstream_clt_getDataLen(avinfohdl->avstreamhdl_v);
            if(res >= avStrmFrmHdV.samplesize && (pIsomGlob->v_SampleBuf_BytesRemain+avStrmFrmHdV.samplesize<pIsomGlob->v_SampleBufLen))
            {
                res=avstream_clt_getData(avinfohdl->avstreamhdl_v,pIsomGlob->v_SampleBuf ,avStrmFrmHdV.samplesize,1);
                if(res!=1)
                {
                    diag_printf("[VID]avstream fream read fail res=%d \n",res);
                    return ISOM_ERR_VIDEO_READ_FAIL;
                }
                pIsomGlob->v_current_sample_num+=1;
                pIsomGlob->v_SampleBuf_BytesRemain+=avStrmFrmHdV.samplesize;
                avStrmFrmHdV.samplesize=0;
            }
            else
            {
                diag_printf("[VID]avstream fream data not enough, frm size=%d remain=%d buf size=%d\n",avStrmFrmHdV.samplesize,res,pIsomGlob->v_SampleBufLen);
                return ISOM_ERR_VIDEO_READ_FAIL;
            }
        }
        break;
        default:
            diag_printf("[VID]  err! pIsomGlob->v_type=%d \n",pIsomGlob->v_type);
            return ISOM_ERR_INVALID_FORMAT;
    }

    tick2=hal_TimGetUpTime();
    diag_printf("[VID]video read time=%d", (tick2-tick1)*1000/16384);
    tick1=hal_TimGetUpTime();

    pIsomGlob->bSkipFrame = FALSE;
    fillAudioData=NULL;//not read audio in video decoder.

    if(pIsomGlob->v_SampleBuf_BytesRemain > 0)
    {
        switch(pIsomGlob->v_type)
        {
#ifdef MPEG4_SUPPORT
            case V_TYPE_MPEG4:
                //decode mpeg4
            {
                xvid_dec_stats_t xvid_dec_stats;
                // read new data

                //if(pIsomGlob->v_SampleBuf_BytesRemain<=MIN_USEFUL_BYTES)
                //    return ISOM_ERR_DATA_NONE;

                while(pIsomGlob->v_SampleBuf_BytesRemain > MIN_USEFUL_BYTES)
                {

                    vocWorkState.pVOC_WORK=(UINT8*)&VOC_WORK;
                    vocWorkState.pVOC_VID=(int16*)&VOC_VID;
                    vocWorkState.pVOC_AUD =(int16*)&VOC_AUD;
                    vocWorkState.vid_display= NULL;
                    vocWorkState.vid_voc_isr=NULL;
                    vocWorkState.aud_nframe_dec=NULL;
                    vocWorkState.vid_frame_read=NULL;

                    used_bytes = dec_main(pIsomGlob->v_SampleBuf+pIsomGlob->v_SampleBuf_BytesUsed,
                                          (uint8*)pIsomGlob->v_YuvInfo,
                                          pIsomGlob->v_SampleBuf_BytesRemain, &xvid_dec_stats, NULL,
                                          ((pIsomGlob->med_type ==MCI_TYPE_MJPG) ? TRUE : FALSE));

                    diag_printf("[VID] used_bytes=%d xvid_dec_stats.type=%d \n", used_bytes, xvid_dec_stats.type);

                    if(used_bytes == -1)//profile unfit
                    {
                        //return ISOM_ERR_VIDEO_DEC_FAIL;
                        pIsomGlob->bSkipFrame = TRUE;
                        pIsomGlob->v_LastFrmStates=1;
                        pIsomGlob->v_SampleBuf_BytesUsed=pIsomGlob->v_SampleBuf_BytesRemain;
                        diag_printf("[VID]ERR frame %d discard display\n",pIsomGlob->v_current_sample_num);
                    }
                    if(used_bytes == -2)
                    {
                        pIsomGlob->bSkipFrame = TRUE;
                        pIsomGlob->v_LastFrmStates=0;
                        pIsomGlob->v_SampleBuf_BytesUsed=pIsomGlob->v_SampleBuf_BytesRemain;
                    }

                    /* Update buffer pointers */
                    if(used_bytes > 0)
                    {
                        pIsomGlob->v_LastFrmStates=0;
                        if(xvid_dec_stats.type == XVID_TYPE_VOL)
                        {
                            pIsomGlob->v_SampleBuf_BytesUsed+=used_bytes;
                            pIsomGlob->v_SampleBuf_BytesRemain-=used_bytes;
                            continue;
                        }
                        else if(xvid_dec_stats.type != XVID_TYPE_IVOP && xvid_dec_stats.type != XVID_TYPE_PVOP)
                        {
                            pIsomGlob->bSkipFrame = TRUE;
                            pIsomGlob->v_LastFrmStates=1;
                            diag_printf("[VID]invalid frame type, discard display\n");
                        }

                        pIsomGlob->v_SampleBuf_BytesUsed+=pIsomGlob->v_SampleBuf_BytesRemain;
                    }

                    break;
                }
                if(pIsomGlob->v_SampleBuf_BytesRemain==MIN_USEFUL_BYTES) pIsomGlob->v_SampleBuf_BytesUsed=1;

            }
            break;
#endif

#ifdef H263_SUPPORT
            case V_TYPE_H263:
            {
                //decode h.263
                int32 gob;

                resetbits (pIsomGlob->v_SampleBuf+pIsomGlob->v_SampleBuf_BytesUsed, pIsomGlob->v_SampleBuf_BytesRemain);

                gob= getheader ();

                if(errorstate)
                {
                    diag_printf("[VID] h.263 getheader err gob=%d \n",gob);
                    return ISOM_ERR_INVALID_FORMAT;
                }

                if (!gob )
                {
                    diag_printf("[VID] h.263 getheader err !data use=%d \n", ld->rdptr - ld->rdbfr);
                    return ISOM_ERR_INVALID_FORMAT;
                    //continue;
                }


                vocWorkState.pVOC_WORK=(UINT8*)&VOC_WORK;
                vocWorkState.pVOC_VID=(int16*)&VOC_VID;
                vocWorkState.pVOC_AUD =(int16*)&VOC_AUD;
                vocWorkState.vid_display= NULL;
                vocWorkState.vid_voc_isr=NULL;
                vocWorkState.aud_nframe_dec=NULL;
                vocWorkState.vid_frame_read=NULL;


                getpicture ((uint8*)pIsomGlob->v_YuvInfo, pIsomGlob->v_lcd_size_x, pIsomGlob->v_lcd_start_x, pIsomGlob->v_lcd_start_y, gob, NULL);

                if(errorstate!=0)
                {
                    diag_printf("[VID] h.263 getpicture err! \n");
                    return ISOM_ERR_VIDEO_DEC_FAIL;
                }

                used_bytes=ld->rdptr - ld->rdbfr;
                /* Update buffer pointers */

                if(used_bytes > 0)
                {
                    pIsomGlob->v_SampleBuf_BytesUsed+=used_bytes;
                }

                break;
            }
#endif

#ifdef H264_SUPPORT
            case V_TYPE_H264:
            {
                int32 got_picture=0;
                //VocWorkStruct vocState;

                vocWorkState.pVOC_WORK=(UINT8*)&VOC_WORK;
                vocWorkState.pVOC_VID=(int16*)&VOC_VID;
                vocWorkState.pVOC_AUD =(int16*)&VOC_AUD;
                vocWorkState.vid_display= NULL;
                vocWorkState.vid_voc_isr=NULL;
                vocWorkState.aud_nframe_dec=NULL;
                vocWorkState.vid_frame_read=NULL;

                diag_printf("pIsomGlob->v_SampleBuf_BytesRemain=%d",pIsomGlob->v_SampleBuf_BytesRemain);
                res=avcodec_decode_video(pIsomGlob->pH264Hdl->codec_context, pIsomGlob->pH264Hdl->frame_picture,
                                         &got_picture, pIsomGlob->v_SampleBuf+pIsomGlob->v_SampleBuf_BytesUsed, pIsomGlob->v_SampleBuf_BytesRemain);
                if(res<0 || !got_picture)
                {
                    diag_printf("[VID] h.264 decode err! res=%d\n",res);
                    return ISOM_ERR_VIDEO_DEC_FAIL;
                }
                pIsomGlob->v_SampleBuf_BytesUsed+=res;
                diag_printf("[VID] h.264 decode consume size=%d got_picture=%d type=%d\n",res,got_picture, pIsomGlob->pH264Hdl->frame_picture->pict_type);
                pIsomGlob->v_YuvInfo[0]=(unsigned int)pIsomGlob->pH264Hdl->frame_picture->data[0];
                pIsomGlob->v_YuvInfo[1]=(unsigned int)pIsomGlob->pH264Hdl->frame_picture->data[1];
                pIsomGlob->v_YuvInfo[2]=(unsigned int)pIsomGlob->pH264Hdl->frame_picture->data[2];
                pIsomGlob->v_YuvInfo[3]=pIsomGlob->pH264Hdl->frame_picture->linesize[0];
                pIsomGlob->v_YuvInfo[4]=pIsomGlob->pH264Hdl->codec_context->height;
                diag_printf("y addr=%x",pIsomGlob->v_YuvInfo[0]);
                diag_printf("u addr=%x",pIsomGlob->v_YuvInfo[1]);
                diag_printf("v addr=%x",pIsomGlob->v_YuvInfo[2]);
                break;
            }
#endif
            case V_TYPE_MJPEG:
            {
                DEC_UNIT jpgDec;


                vocWorkState.pVOC_WORK=(UINT8*)&VOC_WORK;
                vocWorkState.pVOC_VID=(int16*)&VOC_VID;
                vocWorkState.pVOC_AUD =(int16*)&VOC_AUD;
                vocWorkState.vid_display= NULL;
                vocWorkState.vid_voc_isr=NULL;
                vocWorkState.aud_nframe_dec=NULL;
                vocWorkState.vid_frame_read=NULL;

                vid_buf_id=0;
                res= MMF_JpegDecodePicture((long)((char*)pIsomGlob->v_SampleBuf), (char*) pVid_buf_Y[vid_buf_id],
                                           (pIsomGlob->v_width*pIsomGlob->v_height*3)>>1,
                                           pIsomGlob->v_width, pIsomGlob->v_height,
                                           0,0,
                                           pIsomGlob->v_width-1, pIsomGlob->v_height-1,
                                           &jpgDec, 2,0);

                if(res!=FUNC_OK)
                {
                    diag_printf("[VID] Mjpeg MMF_JpegDecodePicture err!res=%d %x", res, *(int*)pIsomGlob->v_SampleBuf);
                    //return ISOM_ERR_VIDEO_DEC_FAIL;
                    pIsomGlob->bSkipFrame = TRUE;
                }

                pIsomGlob->v_YuvInfo[0]=(uint32)pVid_buf_Y[vid_buf_id];
                pIsomGlob->v_YuvInfo[1]=(uint32)pVid_buf_U[vid_buf_id];
                pIsomGlob->v_YuvInfo[2]=(uint32)pVid_buf_V[vid_buf_id];
                pIsomGlob->v_YuvInfo[3]=(uint32)jpgDec.width;
                pIsomGlob->v_YuvInfo[4]=(uint32)jpgDec.height;
                pIsomGlob->v_SampleBuf_BytesUsed+=pIsomGlob->v_SampleBuf_BytesRemain;

                break;
            }

#ifdef RM_SUPPORT
            case V_TYPE_RV:
            {
                int32 i;
                //VocWorkStruct vocState;
                vocWorkState.pVOC_WORK=(UINT8*)&VOC_WORK;
                vocWorkState.pVOC_VID=(int16*)&VOC_VID;
                vocWorkState.vid_voc_isr=vid_voc_isr;
                vocWorkState.aud_nframe_dec=NULL;
                vocWorkState.vid_frame_read=NULL;

                //first output may be last frame. do twice to output key frame
                i=2;
                while(i>0)
                {
                    res=rm_getOneFrameVideo((INT8*)pIsomGlob->v_YuvInfo, /*&vocWorkState,*/ 0);

                    if(res!=HXR_OK)
                    {
                        diag_printf("[VID] rm_getOneFrameVideo err!res=%d ", res);
                        return ISOM_ERR_VIDEO_DEC_FAIL;
                    }

                    if(rm_getLastFrameTimeStamp() == pIsomGlob->current_time)break;

                    i--;
                }
                pIsomGlob->v_SampleBuf_BytesUsed=1;

                break;
            }
#endif

            default:
                diag_printf("[VID]  err! pIsomGlob->v_type=%d \n",pIsomGlob->v_type);
                return ISOM_ERR_INVALID_FORMAT;
        }
    }
    else
    {
        pIsomGlob->bSkipFrame = TRUE;
    }

    tick2=hal_TimGetUpTime();
    diag_printf("[VID]video decode time=%d", (tick2-tick1)*1000/16384);
    tick1=hal_TimGetUpTime();

    pIsomGlob->v_ImageBuf_HalfFlag=1;

    if(DISPLAY_INTERFACE!=0xff){
	    if( pIsomGlob->bSkipFrame == FALSE)
		    DISPLAY_INTERFACE=2;
	    else
		    DISPLAY_INTERFACE=1;
    }

    //skip_display_flag=0;
    vid_display();
#if(CHIP_HAS_GOUDA == 1)
    while(hal_GoudaIsActive())//wait unblock gouda done.
    {
        diag_printf("[VID]gouda busy!");
        sxr_Sleep(160);
    }
    hal_GoudaSetBlock(1);
#endif

    tick2=hal_TimGetUpTime();
    diag_printf("[VID]display time=%d",  (tick2-tick1)*1000/16384);


    play_state=VID_PLAY_STATE_IDLE;
    diag_printf("[VID] mmc_vid_isom_seek OK!");
    //asm("break 1");
    return ISOM_ERR_OK;


}

int32 mmc_vid_isom_pause (void)
{
    int res;
    int i;

    if(play_state!=VID_PLAY_STATE_PLAYING)
    {
        mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_STOP_REQ);
        diag_printf("[VID]##WARNING##mmc_vid_isom_pause play_state= %d \n",play_state);
        return  ISOM_ERR_PLAY_STATE_FAIL;
    }

    play_state=VID_PLAY_STATE_PAUSE;

    do
    {
        res=COS_KillTimerEX(GetMCITaskHandle(MBOX_ID_MMC), MMC_VID_PLY_TIMER_ID);
        diag_printf("[VID]kill timer res=%d", res);
    }
    while(res!=TRUE);

    if(pIsomGlob->open_audio)
    {
        AUD_FADE_OUT_FLAG=3;
        i=0;
        //fade out
        while(AUD_FADE_OUT_FLAG !=1)
        {
            sxr_Sleep(320);//about 20ms
            if(i>20)
                break;
            i++;
        }

        if((res=aud_StreamPause(audioItf, TRUE))!=AUD_ERR_NO)
        {
            diag_printf("[VID] ##WARNING##aud_StreamPause res=%d",res);
        }
        AUD_DATA_REQ=0;
        AUD_FADE_OUT_FLAG=0;
    }
    diag_printf("[VID] a_Pause_Pos= %x \n",pIsomGlob->a_Pause_Pos);

    //play_state=VID_PLAY_STATE_PAUSE;

    //wait voc work over
    i=0;
    while(AV_DEC_STATE ||DISPLAY_STATE || VOC_WORK )
    {
        i++;
        if(i>=VOC_WORK_CONT)
        {
            diag_printf("[VID]PAUSE WARNING! AUD VOC_WORK_CONT end! \n");
            break;
        }
        sxr_Sleep(1600);
    }
    return ISOM_ERR_OK;
}

int32 mmc_vid_isom_resume (void)
{
    int32 res=0;

    if(play_state!=VID_PLAY_STATE_PAUSE)
    {
        diag_printf("[VID]##WARNING##mmc_vid_isom_resume play_state= %d \n",play_state);
        res=ISOM_ERR_PLAY_STATE_FAIL;
        goto resume_error;
    }

    if(pIsomGlob->a_Pause_DecReq==1)
    {

        pIsomGlob->a_Pause_DecReq=0;
    }
    if(pIsomGlob->open_audio)
    {
        int32 i,j;
        uint8 rd_id=A_EXBF_RD_ID;

        AUD_FADE_IN_FLAG=(pIsomGlob->a_HalfPcmBufFrame+2);//zero pcm num
        //mute current pcm play buffer
        if(PCM_ISR_FLAG==0)
        {
            memset((uint8*)pIsomGlob->a_PcmBufHalfPos, 0, pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame);
        }
        else
        {
            memset((uint8*)pIsomGlob->a_PcmBuf, 0, pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame);
        }
        //mute or fade in extern buffer
        for(i=0; i<EXTRA_PCM_BUF_NUM; i++)
        {
            if(aud_extra_pcmbuf_state[rd_id].done==1)
            {
                j=0;
                while(j<pIsomGlob->a_HalfPcmBufFrame)
                {
                    if(AUD_FADE_IN_FLAG>1)
                    {
                        memset((uint8*)(aud_extra_pcmbuf_state[rd_id].pBufPos+ pIsomGlob->a_frameBytes*j), 0, pIsomGlob->a_frameBytes);
                        AUD_FADE_IN_FLAG--;
                    }
                    else if(AUD_FADE_IN_FLAG==1)
                    {
                        audFadeIn((int16*)(aud_extra_pcmbuf_state[rd_id].pBufPos+ pIsomGlob->a_frameBytes*j),
                                  pIsomGlob->a_frame_sample*pIsomGlob->a_channels);
                        AUD_FADE_IN_FLAG=0;
                    }

                    j++;
                }
            }
            else
                break;

            rd_id+=1;
            if(rd_id>=EXTRA_PCM_BUF_NUM)rd_id=0;
        }


        if((res=aud_StreamPause(audioItf, FALSE))!=AUD_ERR_NO)
        {
            diag_printf( "##WARNING## aud_StreamPause err result= %d \n",res);
            res=ISOM_ERR_PCM_OPEN_FAIL;
            goto resume_error;
        }
        AUD_DATA_REQ=1;
    }


    if(DTS_FLG ==0xFFFFFFFF)
        COS_SetTimerEX(GetMCITaskHandle(MBOX_ID_MMC), MMC_VID_PLY_TIMER_ID, COS_TIMER_MODE_PERIODIC, pIsomGlob->v_timerPeriod);
    else
    {
        uint32 t;
        t= ((unsigned long long )DTS_SampDelta *16384+ (unsigned long long )(pIsomGlob->v_timeScale>>1))/(unsigned long long )pIsomGlob->v_timeScale;
        if(t>0)
            COS_SetTimerEX(GetMCITaskHandle(MBOX_ID_MMC), MMC_VID_PLY_TIMER_ID, COS_TIMER_MODE_PERIODIC, t);
        else
            res=COS_SetTimerEX(GetMCITaskHandle(MBOX_ID_MMC), MMC_VID_PLY_TIMER_ID, COS_TIMER_MODE_PERIODIC, pIsomGlob->v_timerPeriod);
    }


    SEND_EVENT_NUM=0;
    DISPLAY_SEND_EVENT_NUM=0;

    play_state=VID_PLAY_STATE_PLAYING;
    diag_printf("[VID] mmc_vid_isom_resume");

    return ISOM_ERR_OK;
resume_error:

    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_STOP_REQ);
    diag_printf("[VID] mmc_vid_isom_resume err res=%d\n", res);
    return res;

}
int32 mmc_vid_isom_stop(void)
{
    int res;
    int i;


    if(play_state>=VID_PLAY_STATE_INVALID)
    {
        diag_printf("[VID]mmc_vid_isom_stop play_state=%d ERR!", play_state);
        return ISOM_ERR_PLAY_STATE_FAIL;
    }
    else if( play_state==VID_PLAY_STATE_IDLE)
    {
        diag_printf("[VID] mmc_vid_isom_stop play_state= %d \n",play_state);
        return ISOM_ERR_OK;
    }

    do
    {
        res=COS_KillTimerEX(GetMCITaskHandle(MBOX_ID_MMC), MMC_VID_PLY_TIMER_ID);
        diag_printf("[VID]kill timer res=%d", res);
    }
    while(res!=TRUE);

    VID_ERR_FRAME_REC=0;
    VID_DEC_REQ=0;
#ifdef USE_KEY_SYNC
    KEY_SAMP_NUM=0;
    KEY_SAMP_TIME_WAIT=0;
    KEY_SAMP_TIME_START=0;
#endif
    if(pIsomGlob)
    {
        if(pIsomGlob->open_audio)
        {
            AUD_FADE_OUT_FLAG=3;
            i=0;
            //fade out
            while(AUD_FADE_OUT_FLAG !=1)
            {
                sxr_Sleep(320);//about 20ms
                if(i>20)
                    break;
                i++;
            }

            if((res=aud_StreamStop(audioItf))!=AUD_ERR_NO)
            {
                diag_printf("[VID] ##WARNING##aud_StreamStop res=%d",res);
            }
            AUD_DATA_REQ=0;
            AUD_FADE_OUT_FLAG=0;
        }

        PCM_ISR_FLAG=0;
        pIsomGlob->a_Pause_DecReq=0;
    }
#if(CHIP_HAS_GOUDA == 1)
    while(hal_GoudaIsActive())
    {
        diag_printf("[VID]gouda busy!");
        sxr_Sleep(160);
    }
    hal_GoudaSetBlock(1);
#endif

    diag_printf("[VID] mmc_vid_isom_stop");

    if(counta)
        diag_printf("[VID] avg  a_read,dec =%d ms ", (totalTa*1000+counta*(16384/2))/(counta*16384));
    if(countv)
        diag_printf("[VID] avg v_dec=%d ms ", (totalTv*1000+countv*(16384/2))/(countv*16384));


    play_state=VID_PLAY_STATE_IDLE;

    //wait voc work over
    i=0;
    while(AV_DEC_STATE ||DISPLAY_STATE || VOC_WORK )
    {
        i++;
        if(i>=VOC_WORK_CONT)
        {
            diag_printf("[VID]STOP WARNING! AUD VOC_WORK_CONT end! \n");
            break;
        }
        sxr_Sleep(1600);
    }
    return ISOM_ERR_OK;
}
int32 mmc_vid_isom_close_file (void)
{
    int32 res;
    if(play_state!=VID_PLAY_STATE_IDLE)
    {
        diag_printf("[VID] mmc_vid_isom_close_file err play_state= %d \n",play_state);
        mmc_vid_isom_stop();
    }

    if(VOC_OPEN)
    {
        if(pIsomGlob->dec_mode==1 )
            vpp_AudioJpegDecClose();
        else
        {
#if(CHIP_HAS_GOUDA == 1)
            if(pIsomGlob->a_type != A_TYPE_AMR)
            {
#endif
                vpp_AudioJpegDecClose();
#if(CHIP_HAS_GOUDA == 1)
            }
            else
            {
                vpp_SpeechClose();
            }
#endif
        }
        VOC_OPEN=0;
    }
#ifdef VID_VOC_FREQ_SET
    if(VOC_FREQ_STATE)
    {
        hal_SysSetVocClock(VOC_FREQ_STATE);
        VOC_FREQ_STATE = 0;
    }
#endif

#ifdef VID_XCPU_FREQ_SET
    if(XCPU_FREQ_STATE)
    {
        //free xcpu resource

        hal_SwReleaseClk(FOURCC_VIDEO);

        XCPU_FREQ_STATE=0;
    }
#endif

    if(pIsomGlob)
    {
        //destroy video decoder
        if( pIsomGlob->v_type == V_TYPE_MPEG4)
        {
#ifdef MPEG4_SUPPORT
            dec_stop();
#endif
        }
        else if(pIsomGlob->v_type==V_TYPE_H264 )
        {
#ifdef H264_SUPPORT
            h264_decoder_close(pIsomGlob->pH264Hdl);
#endif
        }

        //destroy media box
        if(pIsomGlob->med_type==MCI_TYPE_MP4 || pIsomGlob->med_type==MCI_TYPE_3GP)
        {
            gf_isom_close_file(pIsomGlob->file);
        }

#ifdef RM_SUPPORT
        else if(pIsomGlob->med_type==MCI_TYPE_RM)
        {
            rm_destroy(0);
            if(pIsomGlob->a_type==A_TYPE_COOK && pIsomGlob->a_Gecko2DecHdl)
            {
                Gecko2FreeDecoder(pIsomGlob->a_Gecko2DecHdl);
            }
        }
#endif

        else
        {
            if(pIsomGlob->avi_p)
                avi_close_input_file(pIsomGlob->avi_p);
        }

        //destroy audio decoder
        if(pIsomGlob->open_audio!=2 || pIsomGlob->med_type==MCI_TYPE_AVSTRM)
        {
            if(pIsomGlob->a_AacDecHdl)
            {
                AACFreeDecoder(pIsomGlob->a_AacDecHdl);
                pIsomGlob->a_AacDecHdl=0;
            }
            else if(pIsomGlob->a_type == A_TYPE_MP3)
            {
                MMF_Mp3_CloseDecoder();
            }
            /*else if(pIsomGlob->a_Mp3DecHdl)
            {
                MP3FreeDecoder(pIsomGlob->a_Mp3DecHdl);
                pIsomGlob->a_Mp3DecHdl=0;
            }*/
            else if(pIsomGlob->a_AmrStruct)
            {
                AmrDecClose(pIsomGlob->a_AmrStruct);
                pIsomGlob->a_AmrStruct=0;
            }
#ifdef DRA_SUPPORT
            else if(pIsomGlob->a_DraDecHdl)
            {
                DRA_Release(&pIsomGlob->a_DraDecHdl);
                pIsomGlob->a_DraDecHdl=0;
            }
#endif
        }

    }

#if(CHIP_HAS_GOUDA == 1)
    while(hal_GoudaIsActive())
    {
        diag_printf("[VID]close gouda busy!");
        sxr_Sleep(160);
    }
    hal_GoudaSetBlock(1);
    hal_GoudaVidLayerClose();
    if(hal_GoudaGetBgColor()==0)
        hal_GoudaSetBgColor(prebgcolor);

#endif

    play_state=VID_PLAY_STATE_INVALID;

    //wait video work over
    while(AV_DEC_STATE )
    {
        sxr_Sleep(1600);
        diag_printf("[VID]AV_DEC_STATE busy!");
    }

    //avstreamstr_srv_testclose(pIsomGlob->filehdl);//test
    mmc_MemFreeAll();
    g_VidBT8kPcmBuffer = NULL;
    pIsomGlob=NULL;
    pVid_SampleBuf[0]=0;
    pVid_SampleBuf[1]=0;

    lcdd_SetDirDefault();
    //set default charge current.
    pmd_ReduceChargeCurrent(0);

    diag_printf("[VID]  mmc_vid_isom_close_file  \n");
    //play_state=VID_PLAY_STATE_INVALID;

#ifdef USE_OVERLAY
    res=hal_OverlayUnload(VID_OVERLAY_ID);
    if(res!=HAL_ERR_NO)
        diag_printf( "[VID]H263 VID_OVERLAY_ID=%d hal_OverlayUnload  res=%d", VID_OVERLAY_ID, res);
#endif

    return ISOM_ERR_OK;
}

int32 mmc_vid_isom_get_info( IsomPlayInfo* pInfo)
{
    if(play_state>=VID_PLAY_STATE_INVALID)
    {
        diag_printf("[VID]mmc_vid_isom_get_info play_state=%d ERR!", play_state);
        return ISOM_ERR_PLAY_STATE_FAIL;
    }

    /*get info*/
    pInfo->total_time=(pIsomGlob->total_time<500)?0:(pIsomGlob->total_time-500);
    pInfo->current_time= pIsomGlob->current_time;
    pInfo->total_frame_num=pIsomGlob->v_total_frame_num;
    pInfo->image_height=pIsomGlob->v_height;
    pInfo->image_width=pIsomGlob->v_width;
    pInfo->aud_channel_no = pIsomGlob->a_channels;
    pInfo->aud_sample_rate = pIsomGlob->a_sample_rate;
    pInfo->a_type   =  pIsomGlob->a_type;
    switch(pIsomGlob->a_type)
    {
        case A_TYPE_AAC:
        case A_TYPE_AMR:
        case A_TYPE_MP3:
        case A_TYPE_PCM:
        case A_TYPE_COOK:
            pInfo->track = MDI_VIDEO_TRACK_A_ONLY;
            break;
        default:
            pInfo->track = MDI_VIDEO_TRACK_NONE;
            break;
    }

    switch(pIsomGlob->v_type)
    {

        case V_TYPE_H263:
        case V_TYPE_MPEG4:
        case V_TYPE_MJPEG:
            //case V_TYPE_AVI_XVID:
            //case V_TYPE_AVI_DX50:
            //case V_TYPE_AVI_DIV3:

            if(pInfo->track == MDI_VIDEO_TRACK_A_ONLY)
                pInfo->track =  MDI_VIDEO_TRACK_AV;
            else
                pInfo->track =  MDI_VIDEO_TRACK_V_ONLY;
            break;
        default:
            break;
    }

    return ISOM_ERR_OK;
}

int32 mmc_vid_isom_set_mode( IsomPlayMode* pMode)
{
    int32 cleanBufFlag=0;

#if 1
    if(play_state>=VID_PLAY_STATE_INVALID)
    {
        diag_printf("[VID]mmc_vid_isom_set_mode play_state=%d ERR!", play_state);
        return ISOM_ERR_PLAY_STATE_FAIL;
    }

    if((pMode->rotate&0x03) !=pIsomGlob->v_rotate || pMode->zoom_width != pIsomGlob->v_display_width ||
            pMode->zoom_height != pIsomGlob->v_display_height )
    {
        cleanBufFlag=1;
    }

    /*set mode*/
    if((pMode->rotate&0x03)==Rotate_with_0)
    {
	if(pIsomGlob->v_rotate_way==2)//rotate by lcd
		lcdd_SetDirDefault();

        pIsomGlob->v_rotate=Rotate_with_0;
	//xy from lcd top left to bottom right
        pIsomGlob->v_lcd_start_x=pMode->lcd_start_x;
        pIsomGlob->v_lcd_start_y=pMode->lcd_start_y;
        pIsomGlob->v_lcd_size_x=MMC_LcdWidth;
        pIsomGlob->v_lcd_size_y=MMC_LcdHeight;
    }
    else
    {

	if(pIsomGlob->v_rotate_way==2)//rotate by lcd
		lcdd_SetDirRotation();

        pIsomGlob->v_rotate=pMode->rotate&0x03;
	//xy from lcd top right to bottom left.
        pIsomGlob->v_lcd_start_x=pMode->lcd_start_x;
        pIsomGlob->v_lcd_start_y=pMode->lcd_start_y;
        pIsomGlob->v_lcd_size_x=MMC_LcdHeight;
        pIsomGlob->v_lcd_size_y=MMC_LcdWidth;
    }

    pIsomGlob->v_display_width=pMode->zoom_width;
    pIsomGlob->v_display_height=pMode->zoom_height;
    pIsomGlob->v_cutX=pMode->cutX;
    pIsomGlob->v_cutY=pMode->cutY;
    pIsomGlob->v_cutW=pMode->cutW;
    pIsomGlob->v_cutH=pMode->cutH;

    if(pIsomGlob->v_lcd_start_x<0)pIsomGlob->v_lcd_start_x=0;
    if(pIsomGlob->v_lcd_start_x>=pIsomGlob->v_lcd_size_x)pIsomGlob->v_lcd_start_x=pIsomGlob->v_lcd_size_x;
    if(pIsomGlob->v_lcd_start_y<0)pIsomGlob->v_lcd_start_y=0;
    if(pIsomGlob->v_lcd_start_y>=pIsomGlob->v_lcd_size_y)pIsomGlob->v_lcd_start_y=pIsomGlob->v_lcd_size_y;

    if(pIsomGlob->v_rotate==Rotate_with_0)
    {
        if(pIsomGlob->v_cutX<0)pIsomGlob->v_cutX=0;
        if(pIsomGlob->v_cutX>=pIsomGlob->v_display_width)pIsomGlob->v_cutX=pIsomGlob->v_display_width;
        if(pIsomGlob->v_cutY<0)pIsomGlob->v_cutY=0;
        if(pIsomGlob->v_cutY>=pIsomGlob->v_display_height)pIsomGlob->v_cutY=pIsomGlob->v_display_height;
        if(pIsomGlob->v_cutW<0)pIsomGlob->v_cutW=0;
        if(pIsomGlob->v_cutW>=pIsomGlob->v_display_width)pIsomGlob->v_cutW=pIsomGlob->v_display_width;
        if(pIsomGlob->v_cutH<0)pIsomGlob->v_cutH=0;
        if(pIsomGlob->v_cutH>=pIsomGlob->v_display_height)pIsomGlob->v_cutH=pIsomGlob->v_display_height;

        if(pIsomGlob->v_lcd_start_x+pIsomGlob->v_cutW >= pIsomGlob->v_lcd_size_x)
            pIsomGlob->v_cutW= pIsomGlob->v_lcd_size_x-pIsomGlob->v_lcd_start_x;
        if(pIsomGlob->v_lcd_start_y+pIsomGlob->v_cutH >= pIsomGlob->v_lcd_size_y)
            pIsomGlob->v_cutH= pIsomGlob->v_lcd_size_y-pIsomGlob->v_lcd_start_y;

    }
    else
    {
        if(pIsomGlob->v_cutX<0)pIsomGlob->v_cutX=0;
        if(pIsomGlob->v_cutX>=pIsomGlob->v_display_width)pIsomGlob->v_cutX=pIsomGlob->v_display_width;
        if(pIsomGlob->v_cutY<0)pIsomGlob->v_cutY=0;
        if(pIsomGlob->v_cutY>=pIsomGlob->v_display_height)pIsomGlob->v_cutY=pIsomGlob->v_display_height;
        if(pIsomGlob->v_cutW<0)pIsomGlob->v_cutW=0;
        if(pIsomGlob->v_cutW>=pIsomGlob->v_display_width)pIsomGlob->v_cutW=pIsomGlob->v_display_width;
        if(pIsomGlob->v_cutH<0)pIsomGlob->v_cutH=0;
        if(pIsomGlob->v_cutH>=pIsomGlob->v_display_height)pIsomGlob->v_cutH=pIsomGlob->v_display_height;

        if(pIsomGlob->v_lcd_start_x+pIsomGlob->v_cutW >= pIsomGlob->v_lcd_size_x)
            pIsomGlob->v_cutW= pIsomGlob->v_lcd_size_x-pIsomGlob->v_lcd_start_x;
        if(pIsomGlob->v_lcd_start_y+pIsomGlob->v_cutH >= pIsomGlob->v_lcd_size_y)
            pIsomGlob->v_cutH= pIsomGlob->v_lcd_size_y-pIsomGlob->v_lcd_start_y;

    }

    if(pIsomGlob->v_display_width>0 && pIsomGlob->v_display_height>0)
    {
	    if(pIsomGlob->v_rotate_way==2){//rotate by lcd
		    pIsomGlob->v_cutX_origin=(int16)((uint32)pIsomGlob->v_width*(uint32)pIsomGlob->v_cutX/(uint32)pIsomGlob->v_display_width);
		    pIsomGlob->v_cutX_origin=(pIsomGlob->v_cutX_origin+0x3)&0xFFF8;//multiple of 8 for YUV
		    pIsomGlob->v_cutY_origin=(int16)((uint32)pIsomGlob->v_height*(uint32)pIsomGlob->v_cutY/(uint32)pIsomGlob->v_display_height);
		    pIsomGlob->v_cutY_origin=(pIsomGlob->v_cutY_origin+0x1)&0xFFFC;//multiple of 4 for YUV
		    pIsomGlob->v_cutW_origin=(int16)((uint32)(pIsomGlob->v_width)*(uint32)(pIsomGlob->v_cutW)/(uint32)(pIsomGlob->v_display_width));
		    if(pIsomGlob->v_cutX_origin+pIsomGlob->v_cutW_origin > pIsomGlob->v_width)
			    pIsomGlob->v_cutW_origin=(pIsomGlob->v_width&0xFFF8)-pIsomGlob->v_cutX_origin;
		    pIsomGlob->v_cutW_origin=(pIsomGlob->v_cutW_origin)&0xFFF8;//multiple of 8 for YUV
		    pIsomGlob->v_cutH_origin=(int16)((uint32)pIsomGlob->v_height*(uint32)pIsomGlob->v_cutH/(uint32)pIsomGlob->v_display_height);
		    if(pIsomGlob->v_cutY_origin+pIsomGlob->v_cutH_origin > pIsomGlob->v_height)
			    pIsomGlob->v_cutH_origin=(pIsomGlob->v_height&0xFFFC)-pIsomGlob->v_cutY_origin;
		    pIsomGlob->v_cutH_origin=(pIsomGlob->v_cutH_origin)&0xFFFC;//multiple of 4 for YUV
	    }else{ //gouda or soft rotate
		    pIsomGlob->v_cutX_origin=(int16)((uint32)pIsomGlob->v_width*(uint32)pIsomGlob->v_cutX/(uint32)pIsomGlob->v_display_width);
		    pIsomGlob->v_cutX_origin=(pIsomGlob->v_cutX_origin+0x1)&0xFFFC;//multiple of 4 for YUV
		    pIsomGlob->v_cutY_origin=(int16)((uint32)pIsomGlob->v_height*(uint32)pIsomGlob->v_cutY/(uint32)pIsomGlob->v_display_height);
		    //pIsomGlob->v_cutY_origin=(pIsomGlob->v_cutY_origin+0x3)&0xFFF8;//multiple of 8 for Y
		    pIsomGlob->v_cutW_origin=(int16)((uint32)(pIsomGlob->v_width)*(uint32)(pIsomGlob->v_cutW)/(uint32)(pIsomGlob->v_display_width));
		    if(pIsomGlob->v_cutX_origin+pIsomGlob->v_cutW_origin > pIsomGlob->v_width)
			    pIsomGlob->v_cutW_origin=pIsomGlob->v_width-pIsomGlob->v_cutX_origin;
		    pIsomGlob->v_cutW_origin=(pIsomGlob->v_cutW_origin)&0xFFF8;//multiple of 8 for Y, 4 for UV
		    pIsomGlob->v_cutH_origin=(int16)((uint32)pIsomGlob->v_height*(uint32)pIsomGlob->v_cutH/(uint32)pIsomGlob->v_display_height);
		    if(pIsomGlob->v_cutY_origin+pIsomGlob->v_cutH_origin > pIsomGlob->v_height)
			    pIsomGlob->v_cutH_origin=pIsomGlob->v_height-pIsomGlob->v_cutY_origin;
		    pIsomGlob->v_cutH_origin=(pIsomGlob->v_cutH_origin)&0xFFF8;//multiple of 8 for Y,4 for UV
	    }

    }
    pIsomGlob->v_LastSkipRotateTk= 0;

#if (CSW_EXTENDED_API_LCD == 1)
	//setup gouda
    Vid_Video.depth = HAL_GOUDA_VID_LAYER_OVER_ALL;

    if((pMode->rotate&0x0C)==4)
        Vid_Video.depth = HAL_GOUDA_VID_LAYER_BETWEEN_2_1;
    else if((pMode->rotate&0x0C)==8)
        Vid_Video.depth = HAL_GOUDA_VID_LAYER_BETWEEN_1_0;
    else if((pMode->rotate&0x0C)==12)
        Vid_Video.depth = HAL_GOUDA_VID_LAYER_BEHIND_ALL;

    if((pIsomGlob->v_rotate==Rotate_with_0)||(pIsomGlob->v_rotate_way!=0)){//not rotate or not by gouda.
	    Vid_Video.rotation = 0;//0=not rotate, 1= 90 clockwise
    }else{//rotate by gouda
	    Vid_Video.rotation = 1;//0=not rotate, 1= 90 clockwise
    }

    if((pIsomGlob->v_rotate==Rotate_with_0) ||(pIsomGlob->v_rotate_way==2)){//rotate by lcd or not rotate
	    Vid_Video.pos.tlPX = pIsomGlob->v_lcd_start_x;
	    Vid_Video.pos.tlPY = pIsomGlob->v_lcd_start_y;
	    Vid_Video.pos.brPX =pIsomGlob->v_lcd_start_x+pIsomGlob->v_cutW-1;
	    Vid_Video.pos.brPY =pIsomGlob->v_lcd_start_y+pIsomGlob->v_cutH-1;
	    //setup lcd
	    lcdDraw.fb.width = pIsomGlob->v_lcd_size_x;
	    lcdDraw.fb.height = pIsomGlob->v_lcd_size_y;

    }else if(pIsomGlob->v_rotate_way==1){//rotate by soft
	    //horizontal lcd xy to vertical lcd xy.
	    //target area to scale
	    Vid_Video.pos.tlPX = pIsomGlob->v_lcd_size_y-(pIsomGlob->v_lcd_start_y+pIsomGlob->v_cutH);
	    Vid_Video.pos.tlPY = pIsomGlob->v_lcd_start_x;
	    Vid_Video.pos.brPX = pIsomGlob->v_lcd_size_y- pIsomGlob->v_lcd_start_y-1;
	    Vid_Video.pos.brPY =pIsomGlob->v_lcd_start_x+pIsomGlob->v_cutW-1;
	    //setup lcd
	    lcdDraw.fb.width = pIsomGlob->v_lcd_size_y;
	    lcdDraw.fb.height = pIsomGlob->v_lcd_size_x;

    }else{//roate by gouda
	    //same as soft rotate
	    Vid_Video.pos.tlPX = pIsomGlob->v_lcd_size_y-(pIsomGlob->v_lcd_start_y+pIsomGlob->v_cutH);
	    Vid_Video.pos.tlPY = pIsomGlob->v_lcd_start_x;
	    Vid_Video.pos.brPX = pIsomGlob->v_lcd_size_y- pIsomGlob->v_lcd_start_y-1;
	    Vid_Video.pos.brPY =pIsomGlob->v_lcd_start_x+pIsomGlob->v_cutW-1;
	    lcdDraw.fb.width = pIsomGlob->v_lcd_size_y;
	    lcdDraw.fb.height = pIsomGlob->v_lcd_size_x;
	
    }

#endif


    if((pMode->rotate&0x10)==0x10)//bit4 control display area
	    DISPLAY_INTERFACE=0xff;
    else
	    DISPLAY_INTERFACE=2;

#endif

    diag_printf("[VID]mmc_vid_isom_set_mode lcd_start_x=%d  lcd_start_y=%d \n zoom_width=%d  zoom_height=%d rotate=%d \n",
                pMode->lcd_start_x, pMode->lcd_start_y, pMode->zoom_width,pMode->zoom_height, pMode->rotate);
    diag_printf("[VID]cutX=%d cutY=%d cutW=%d cutH=%d\n",
                pMode->cutX,pMode->cutY,pMode->cutW,pMode->cutH);

    diag_printf("[VID]real lcd_start_x=%d  lcd_start_y=%d \n zoom_width=%d  zoom_height=%d rotate=%d \n",
                pIsomGlob->v_lcd_start_x, pIsomGlob->v_lcd_start_y, pIsomGlob->v_display_width,pIsomGlob->v_display_height, pIsomGlob->v_rotate);
    diag_printf("[VID]cutX=%d cutY=%d cutW=%d cutH=%d\n",
                pIsomGlob->v_cutX,pIsomGlob->v_cutY,pIsomGlob->v_cutW,pIsomGlob->v_cutH);
    diag_printf("[VID]origin cutX=%d cutY=%d cutW=%d cutH=%d\n",
                pIsomGlob->v_cutX_origin,pIsomGlob->v_cutY_origin,pIsomGlob->v_cutW_origin,pIsomGlob->v_cutH_origin);

    diag_printf("[VID]gouda pos tlx=%d tly=%d brx=%d bry=%d\n",
                Vid_Video.pos.tlPX,Vid_Video.pos.tlPY,Vid_Video.pos.brPX,Vid_Video.pos.brPY);
    diag_printf("[VID]lcdDraw fb w=%d h=%d\n",
                lcdDraw.fb.width,lcdDraw.fb.height);

    return ISOM_ERR_OK;
}


int32 Video_IsomUserMsg(COS_EVENT *pVidEv)
{
    int32 res;

    AV_DEC_STATE=1;

    switch(pVidEv->nEventId)
    {
        case MSG_VID_AUD_DEC_REQ:
            SEND_EVENT_NUM=0;

            res=vid_av_dec_frame();

            if(res!=ISOM_ERR_OK && res!=ISOM_ERR_PLAY_STATE_FAIL)
            {
                mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_STOP_REQ);
                diag_printf("[VID] UserMsg vid_audio_dec_frame err=%d \n", res);
            }

            break;

        case MSG_VID_IMG_DEC_REQ:
            SEND_EVENT_NUM=0;

            res=vid_av_dec_frame();

            if(res!=ISOM_ERR_OK && res!=ISOM_ERR_PLAY_STATE_FAIL)
            {
                mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_STOP_REQ);
                diag_printf("[VID] UserMsg vid_video_dec_frame err=%d \n", res);
            }

            break;

        case EV_TIMER:

            switch(pVidEv->nParam1)
            {
                case MMC_VID_PLY_TIMER_ID:
                    VID_DEC_REQ++;

                    if(SEND_EVENT_NUM==0)
                    {
                        res = vid_av_dec_frame();

                        if(res!=ISOM_ERR_OK && res!=ISOM_ERR_PLAY_STATE_FAIL)
                        {
                            mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_STOP_REQ);
                            diag_printf("[VID] UserMsg vid_video_dec_frame err=%d \n", res);
                        }
                    }
                    //else
                    //  diag_printf( "[VID]Skip Timer");
                    break;

                default:

                    break;
            }
            break;

        case MSG_VID_DISPLAY_REQ:
            //vid_display();
            break;
        case MSG_VID_STOP_REQ:
            //  if(play_state!=VID_PLAY_STATE_IDLE)
        {
            if(pIsomGlob->open_audio)
                aud_StreamStop(audioItf);

            COS_KillTimerEX(GetMCITaskHandle(MBOX_ID_MMC), MMC_VID_PLY_TIMER_ID);

            pIsomGlob->current_time=0;
            pIsomGlob->vid_play_finish_callback(0);
        }
        break;

        default:
            break;
    }

    AV_DEC_STATE=0;

    return ISOM_ERR_OK;

}

#endif

