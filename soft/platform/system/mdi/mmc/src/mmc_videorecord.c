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





#include "mmc_videorecord.h"
#include "hal_error.h"
#include "fs.h"
#include "event.h"
#include "mmc_timer.h"
#include "vpp_audiojpegenc.h"
#include "mmf_jpegamrring_avi_enc.h"
#include "mmc_camera.h" // chenhe add for get a new filename
#include "mci.h"
#include "dm.h"

#include "ars_m.h"
#include "camera_m.h"
#include "hal_timers.h"
#include "lcdd_m.h"
#include "mcip_debug.h"
#include "string.h"
#include "fs_asyn.h"
//#include "hal_overlay.h"

#include "global.h"
#include "calib_m.h"

#if (CHIP_HAS_GOUDA == 1)

#include "hal_gouda.h"
#include "sxr_tls.h"

#include "sxr_tim.h"

#ifndef DONOT_SUPPORT_DUAL_BUFFER
//#define VID_REC_CAP_DUAL_BUF
#endif
//mutiple of 5 ,2 or 3.
#define VSRECBUF_LEN            (600)
#define VSRECBUF_TRANS_LEN (300)

#define AVI_SIZE_MIN (1000)
#define YUV2RGB_ASM
#define PCM_REC_BUF_SIZE  (160*2*10)
#define AMR_TRANS_COUNT (5)

extern void set_mmc_camera_preview();
extern BOOL mcd_IsHighCapacityCard();

VIDREC_CONTEXT g_VidRecCntx;
FS_INFO g_FsVidRecInfo;

uint8* vidrec_pAvi_Buff;
uint8* vidrec_avi_out;
uint8* RecPcmBuffer;
uint8* AMR_buffer;
UINT8* AMR_buffer_trans;
UINT8* AMR_buffer_trans_cur;
uint8* vidrec_JpegBuff0;
uint8* vidrec_JpegBuff1;
uint8* linebuff_asm;

int8* gp_VidRec_Prebuf;

int32 vidrec_VocFirstSchedule;
int32 vidrec_AviNuminBuff;

volatile mmcModeStatus videoRecordState = VidrecStopStatus;

uint8 vidrec_amr_first_frame[32* 30 + 6];
uint8 vidrec_avi_head[AVI_HEAD_SIZE];
uint8* vidrec_avi_end;

volatile uint8 *AMR_buffer_tail;
volatile uint8 *AMR_buffer_heard;
volatile uint8 *vidrec_pJpegAviIn;
volatile uint8 *vidrec_pJpegVocOut;

volatile uint32* vidrec_pJpeg_sizeVoc=NULL;
volatile uint32* vidrec_pJpeg_sizeAvi=NULL;

volatile int32 Msg_Wait_Num = 0;
volatile int32 AMR_Data_Store = 0;
//volatile int32 Vidrec_Sensor_Seq = 0;
volatile int32 Cam_Data_Valid = -1;
//volatile int32 Voc_JpegEnc_Busy = 0;
//volatile int32 Voc_AmrEnc_Busy = 0;

#define BIT_JPEG_ENC_REQ        0
#define BIT_JPEG_ENC_BUSY       1
#define BIT_AMR_ENC_REQ         2
#define BIT_AMR_ENC_BUSY        3
//bit1:Jpeg encode busy, bit0:Jpeg encode request
//bit3:AMR encode busy, bit2:AMR encode request
volatile uint8 Voc_Busy = 0;

volatile int32 Voc_Interrupt_From = -1;
volatile int32 Voc_Amr_Count = 0;
volatile int32 Pcm_StartPosition=0;
volatile int32 Voc_Jpg_Need = 0;
volatile int32 Voc_Amr_Need = 0;
volatile int32 Voc_JpegOut_Seq = 0;
volatile int32 VR_Init=0;
volatile int32 VR_Amr_Init = 0;

volatile uint32 Avi_Out_Index=0;
volatile uint32 g_AMR_DataInCount=0;

uint32 F_tick =0;
uint32 L_tick = 0;
uint32 frame_cont =0;
int32 vidrec_Jpeg0_size;
int32 vidrec_Jpeg1_size;

extern UINT16* sensorbuf;
#ifdef VID_REC_CAP_DUAL_BUF
static UINT16* sensorbuf_dual[2]= {NULL,NULL};
//write id.0 for sensorbuf,1 for sensorbuf_dual
volatile UINT16 sensorbuf_w_id;
#endif
extern UINT16* blendBuffer;
//extern CAM_SIZE_T MY_NPIX;
extern uint16 startPos;
//extern uint16 srcwidth;
//extern uint16 srcheight;
extern uint16 cutwidth;
extern uint16 cutheight;
extern uint16  bset;
extern int video_i;
extern uint32 cam_wbValue;
extern AUD_ITF_T audioItf;
extern AVI_video_index* avi_viidx;  // bao debug: design the index table according 500 frame
extern volatile CAM_INFO_STRUCT  CameraInfo;

UINT32 gVidrecTimeout = 0;

#if (CSW_EXTENDED_API_LCD == 1)
extern LCDD_FBW_T displayFbw;
#endif

UINT16* g_vidBuf = NULL;

static int32 vidrec_AmrEncFirstSchedule = 0;
extern void VidRec_bilinear_zoom(char* srcbuf,int src_w,int src_h,int cut_w,int cut_h,char* decbuf,int dec_w,int dec_h,char* linebuff);

void setbit(uint8* num, int bit)
{
    //diag_printf ("set bit before, *num:0x%x , bit:%d", *num, bit);
    *num |= (1<<bit);
    //diag_printf ("set bit after, *num:0x%x , bit:%d", *num, bit);
}

void clearbit(uint8* num, int bit)
{
    //diag_printf ("clear bit before, *num:0x%x , bit:%d", *num, bit);
    *num &= ~(1<<bit);
    //diag_printf ("clear bit after, *num:0x%x , bit:%d", *num, bit);
}

bool testbit(uint8* num, int bit)
{
    //diag_printf ("test bit, *num:0x%x , bit:%d", *num, bit);
    return *num &= (1<<bit);
}


#if defined(__CAMERA_FULL_OSD__)
UINT16* rotateBuf = NULL;
UINT8 vidRec_rotate(UINT16* inBuf, UINT16* outBuf, IMG_ROTATE_E rotate, UINT16 width, UINT16 height)
{
    UINT32 imgSize = width*height;
    volatile BOOL step4 = (width&3)||(height&3);
    volatile BOOL step8 = (width&7)||(height&7);

    if (!inBuf || !outBuf || !width || !height || (rotate >= ROTATE_MAX))
        return 0;

    inBuf = (UINT16*)(((UINT32)inBuf)&(~0x20000000));
    hal_SysInvalidateCache(inBuf, imgSize<<1);
    switch (rotate)
    {
        case ROTATE_0:
            break;

        case ROTATE_90:
        {
            UINT16 x = 0;
            UINT16 y = 0;
            UINT32 idx = 0;
            UINT32 pos = 0;

            do
            {
                y = idx / width;
                x = idx % width;
                pos = x*height + height - y - 1;

                outBuf[pos] = inBuf[idx++];
                pos += height;

                outBuf[pos] = inBuf[idx++];
                pos += height;

                if (!step4)
                {
                    outBuf[pos] = inBuf[idx++];
                    pos += height;

                    outBuf[pos] = inBuf[idx++];
                    pos += height;
                }

                if (!step8)
                {
                    outBuf[pos] = inBuf[idx++];
                    pos += height;

                    outBuf[pos] = inBuf[idx++];
                    pos += height;

                    outBuf[pos] = inBuf[idx++];
                    pos += height;

                    outBuf[pos] = inBuf[idx++];
                    pos += height;
                }

            }
            while(idx < imgSize);
        }
        break;

        case ROTATE_180:
        {
            UINT32 idx = 0;

            do
            {
                outBuf[imgSize-idx-1] = inBuf[idx];
                idx++;

                outBuf[imgSize-idx-1] = inBuf[idx];
                idx++;

                if (!step4)
                {
                    outBuf[imgSize-idx-1] = inBuf[idx];
                    idx++;

                    outBuf[imgSize-idx-1] = inBuf[idx];
                    idx++;
                }

                if (!step8)
                {
                    outBuf[imgSize-idx-1] = inBuf[idx];
                    idx++;

                    outBuf[imgSize-idx-1] = inBuf[idx];
                    idx++;

                    outBuf[imgSize-idx-1] = inBuf[idx];
                    idx++;

                    outBuf[imgSize-idx-1] = inBuf[idx];
                    idx++;
                }
            }
            while(idx < imgSize);
        }
        break;

        case ROTATE_270:
        {
            UINT16 x = 0;
            UINT16 y = 0;
            UINT32 idx = 0;
            UINT32 pos = 0;

            do
            {
                y = idx / width;
                x = idx % width;
                pos = (width-x-1)*height + y;

                outBuf[pos] = inBuf[idx++];
                pos -= height;

                outBuf[pos] = inBuf[idx++];
                pos -= height;

                if (!step4)
                {
                    outBuf[pos] = inBuf[idx++];
                    pos -= height;

                    outBuf[pos] = inBuf[idx++];
                    pos -= height;
                }

                if (!step8)
                {
                    outBuf[pos] = inBuf[idx++];
                    pos -= height;

                    outBuf[pos] = inBuf[idx++];
                    pos -= height;

                    outBuf[pos] = inBuf[idx++];
                    pos -= height;

                    outBuf[pos] = inBuf[idx++];
                    pos -= height;
                }
            }
            while(idx < imgSize);
        }
        break;
    }

    return 1;
}
#endif

void vidRec_Callback()
{
    uint32 dtick=0;
    int TDuration = 1;

    if(videoRecordState == VidrecStopStatus)
        return;

    frame_cont++;
    if(F_tick == 0)
    {
        F_tick = hal_TimGetUpTime();
    }
    if(frame_cont >= ((mcd_IsHighCapacityCard() == TRUE) ? VIDREC_AVI_VIDEO_FRAME_RATE : (VIDREC_AVI_VIDEO_FRAME_RATE>>1))* TDuration)
    {
        L_tick = hal_TimGetUpTime();


        if(L_tick < F_tick )
        {
            dtick = L_tick + (0xffffffff-F_tick);
        }
        else
            dtick = L_tick - F_tick;

        if(dtick >= HAL_TICK1S * TDuration)
        {
            F_tick = L_tick;
            dtick = dtick-HAL_TICK1S * TDuration;
            gVidrecTimeout = gVidrecTimeout - dtick/TDuration/((mcd_IsHighCapacityCard() == TRUE) ? VIDREC_AVI_VIDEO_FRAME_RATE : (VIDREC_AVI_VIDEO_FRAME_RATE>>1));

        }
        else
        {
            F_tick = L_tick;
            dtick = HAL_TICK1S * TDuration- dtick;
            gVidrecTimeout = gVidrecTimeout + dtick/TDuration/((mcd_IsHighCapacityCard() == TRUE) ? VIDREC_AVI_VIDEO_FRAME_RATE : (VIDREC_AVI_VIDEO_FRAME_RATE>>1));
        }

        frame_cont=0;
    }
    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VIDREC_AVI_OUTPUT_REQ);
    if((gVidrecTimeout>0x7FFFFFFF) ||(gVidrecTimeout==0))
    {
        gVidrecTimeout=16384/((mcd_IsHighCapacityCard() == TRUE) ? VIDREC_AVI_VIDEO_FRAME_RATE : (VIDREC_AVI_VIDEO_FRAME_RATE>>1));
    }
    sxr_StartFunctionTimer(gVidrecTimeout, vidRec_Callback, NULL, 0);
}

void VidRec_EncodeFrameIsr(HAL_VOC_IRQ_STATUS_T* irqStatus)
{
    MCI_TRACE(MCI_CAMERA_TRC, 0, "[MMC_videorecord]VidRec_EncodeFrameIsr: %d",hal_TimGetUpTime());
    MCI_TRACE (MCI_VIDREC_TRC,0,"------------Voc_Interrupt_From=%d,Voc_Jpg_Need=%d,Voc_Amr_Need =%d,Voc_Amr_Count=%d",Voc_Interrupt_From,Voc_Jpg_Need,Voc_Amr_Need,Voc_Amr_Count);

    if ((vidrec_VocFirstSchedule != 0)&&(videoRecordState == VidrecRecordStatus))
    {
        if(Voc_Interrupt_From == 0)
        {
            vpp_AudioJpeg_ENC_OUT_T VocOutputStatus;

            Voc_Interrupt_From = -1;
            Avi_Out_Index += 1;

            vpp_AudioJpegEncStatus(&VocOutputStatus);
            *vidrec_pJpeg_sizeVoc = VocOutputStatus.output_len;// + 607;

            if(Voc_JpegOut_Seq == 0)
            {
                Voc_JpegOut_Seq = 1;
                vidrec_pJpegVocOut = vidrec_JpegBuff0;
                vidrec_pJpegAviIn = vidrec_JpegBuff1;
                vidrec_pJpeg_sizeVoc = &vidrec_Jpeg0_size;
                vidrec_pJpeg_sizeAvi = &vidrec_Jpeg1_size;
            }
            else
            {
                Voc_JpegOut_Seq = 0;
                vidrec_pJpegVocOut = vidrec_JpegBuff1;
                vidrec_pJpegAviIn = vidrec_JpegBuff0;
                vidrec_pJpeg_sizeVoc = &vidrec_Jpeg1_size;
                vidrec_pJpeg_sizeAvi = &vidrec_Jpeg0_size;
            }
            //Voc_JpegEnc_Busy = 0;
            clearbit(&Voc_Busy, BIT_JPEG_ENC_BUSY);
        }
        else if (Voc_Interrupt_From == 1)
        {
            Voc_Interrupt_From = -1;
            //Voc_AmrEnc_Busy = 0;
            clearbit(&Voc_Busy, BIT_AMR_ENC_BUSY);
            AMR_Data_Store++;

            if(Voc_Amr_Count >= AMR_TRANS_COUNT)
            {
                Voc_Amr_Count = 0;
                Voc_Amr_Need = 0;
            }
        }

        if (Voc_Jpg_Need == 1)
        {
            //Voc_JpegEnc_Busy = 1;
            setbit(&Voc_Busy, BIT_JPEG_ENC_BUSY);
            Voc_Interrupt_From = 0;
            Voc_Jpg_Need = 0;

#ifdef __CAMERA_FULL_OSD__
            if (CameraInfo.img_rotate != 0)
            {
                VidRec_JPEGEncode((int32 *)(rotateBuf),
                                  g_VidRecCntx.settingInfo.imageWidth,
                                  g_VidRecCntx.settingInfo.imageHeight);
            }
            else
#endif
            {
                VidRec_JPEGEncode((int32 *)(g_vidBuf),
                                  g_VidRecCntx.settingInfo.imageWidth,
                                  g_VidRecCntx.settingInfo.imageHeight);
            }

        }
        else if (Voc_Amr_Need > 0)
        {
            VidRec_Pcm2Amr();
        }
        else
        {
            MCI_TRACE (MCI_VIDREC_TRC,0,"------------VOC IDLE!,Voc_Amr_Need=%d",Voc_Amr_Need);
        }

    }
    else
        vidrec_VocFirstSchedule = 1;

}

void VidRec_Pcm2Amr(void)
{
    setbit(&Voc_Busy, BIT_AMR_ENC_REQ);
    //if ((Voc_JpegEnc_Busy == 0) && (Voc_AmrEnc_Busy == 0))//&&(!((PCM_BUF_INT=1)&&(VOC_AMRENC_BUSY))))
    if (Voc_Busy == (1<<BIT_AMR_ENC_REQ))
    {
        vpp_AudioJpeg_ENC_IN_T   audio_amrenc;

        setbit(&Voc_Busy, BIT_AMR_ENC_BUSY);
        clearbit(&Voc_Busy, BIT_AMR_ENC_REQ);
        if(AMR_Data_Store<0)
            AMR_Data_Store = 0;
        //Voc_AmrEnc_Busy = 1;
        Voc_Interrupt_From = 1;
        Voc_Amr_Count++;
        //Voc_Amr_Need=1;

        audio_amrenc.inStreamBufAddr = (INT32*)(RecPcmBuffer+(Pcm_StartPosition*PCM_REC_BUF_SIZE/2)+(Voc_Amr_Count-1)*320);
        audio_amrenc.outStreamBufAddr = (INT32*)AMR_buffer_tail;
        AMR_buffer_tail += 32;

        if((AMR_buffer_tail - AMR_buffer) >= 32 * VSRECBUF_LEN)
            AMR_buffer_tail = AMR_buffer;

        audio_amrenc.mode = MMC_AMR_ENCODE;
        audio_amrenc.channel_num = 1;
        audio_amrenc.SampleRate = HAL_AIF_FREQ_8000HZ;
        audio_amrenc.reset = 0;

        if (VR_Amr_Init == 0)
        {
            VR_Amr_Init = 1;
            audio_amrenc.reset = 1;
        }

        if(HAL_ERR_NO != vpp_AudioJpegEncScheduleOneFrame(&audio_amrenc))
        {
            //Voc_AmrEnc_Busy = 0;
            clearbit(&Voc_Busy, BIT_AMR_ENC_BUSY);
            diag_printf ("Failed to call VOC in VidRec_Pcm2Amr :%d th", Voc_Amr_Count);
        }
        MCI_TRACE(MCI_CAMERA_TRC, 0, "[MMC_videorecord]AMR: %d",hal_TimGetUpTime());

    }
    else
    {
        clearbit(&Voc_Busy, BIT_AMR_ENC_REQ);
        //Voc_Amr_Need=1;
        /*MCI_TRACE (MCI_VIDREC_TRC,0,
                "------%d pcm2amr need waiting,Voc_JpegEnc_Busy=%d,Voc_AmrEnc_Busy=%d",
                Voc_Amr_Need, Voc_JpegEnc_Busy, Voc_AmrEnc_Busy);*/
        MCI_TRACE (MCI_VIDREC_TRC,0,   "------%d pcm2amr need waiting,Voc_Busy=%d", Voc_Amr_Need, Voc_Busy);
    }
}

error_type vidrec_EncodeRun(VIDREC_CONTEXT* amr_in)
{
    int32 count = 0;
    UINT8* ptmp = NULL;

    // ////////////////// JPEG encode a frame finish ////////////////////////////////////

    amr_in->JPEGEncodeFrameCnt++;

    //  check if finish the whole video recored
    if (((g_VidRecCntx.curFileSize + VIDREC_CHECK_AVI_FILE_SIZE) >= (g_VidRecCntx.settingInfo.fileSizePermit<<10))
            && (g_VidRecCntx.settingInfo.fileSizePermit > 0))
    {
        diag_printf("[Vidrec Encode Run] file size is reach, file size = %d \n", g_VidRecCntx.curFileSize);
        VidRec_RecordStop();
        g_VidRecCntx.vidrec_callback(MDI_RES_VDOREC_REACH_SIZE_LIMIT);
        diag_printf("Video Record Finished, REACH_SIZE_LIMIT!\n");
        return MDI_RES_VDOREC_REACH_SIZE_LIMIT;
    }

    {
        int32 avi_frame_length = 0;
        JpegAmr_Avi_Enc avi_enc_info;
        static uint8 amrHead[6] = {0x23, 0x21, 0x41, 0x4d, 0x52, 0x0a};

        /*************avi frame integrate***************************/
        if ((videoRecordState == VidrecRecordStatus) &&
                (g_VidRecCntx.JPEGEncodeFrameCnt > 0) &&
                (g_VidRecCntx.settingInfo.IfRecordAudio//&& g_AMR_DataInCount--)
                 || ((!g_VidRecCntx.settingInfo.IfRecordAudio)
                     && (g_VidRecCntx.RGBBufStatus[0] == RGB_BUF_NULL))))
        {
            // interrupt disabled
            UINT32 status = hal_SysEnterCriticalSection();

            if ((g_VidRecCntx.settingInfo.IfRecordAudio == TRUE) && (AMR_Data_Store>0))
            {
                UINT32 size = AMR_Data_Store<<5;
                UINT8* pEnd = AMR_buffer + (VSRECBUF_LEN<<5);

                if ((size + AMR_buffer_trans_cur) >= (AMR_buffer_trans + (VSRECBUF_TRANS_LEN<<5)))
                    AMR_buffer_trans_cur = AMR_buffer_trans;

                ptmp = AMR_buffer_trans_cur;

                if (size + AMR_buffer_heard > pEnd)
                {
                    UINT32 size1 = (pEnd - AMR_buffer_heard);
                    UINT32 size2 = size - size1;
                    memcpy(AMR_buffer_trans_cur, AMR_buffer_heard, size1);
                    AMR_buffer_trans_cur += size1;
                    memcpy(AMR_buffer_trans_cur, AMR_buffer, size2);
                    AMR_buffer_heard = AMR_buffer + size2;
                    AMR_buffer_trans_cur += size2;
                }
                else
                {
                    memcpy(AMR_buffer_trans_cur, AMR_buffer_heard, size);
                    AMR_buffer_heard += size;
                    AMR_buffer_trans_cur += size;
                }

                g_VidRecCntx.pingInDataBlockNum = size;
                AMR_Data_Store = 0;
            }
            else
            {
                g_VidRecCntx.pingInDataBlockNum = 0;
            }

            // interrupt enabled
            hal_SysExitCriticalSection(status);

            if(vidrec_AviNuminBuff < AVI_BUF_NUM)
            {
                avi_enc_info.Avi_outBuf = vidrec_avi_out;

                vidrec_avi_out += (AVI_BUF_SIZE<<10);
                if(vidrec_avi_out > (vidrec_pAvi_Buff+((AVI_BUF_SIZE*(AVI_BUF_NUM-1))<<10)))
                    vidrec_avi_out = vidrec_pAvi_Buff;

                vidrec_AviNuminBuff++;
            }
            else
            {
                diag_printf("######discard avi,buffer overflow!vidrec_AviNuminBuff =%d,AMR_Data_Store=%d",
                            vidrec_AviNuminBuff,
                            AMR_Data_Store);
                return NO_ERRO;
            }

            //avi_enc_info.Avi_outBuf = vidrec_avi_out;
            avi_enc_info.Jpeg_Frame_Size = * vidrec_pJpeg_sizeAvi;
            avi_enc_info.Jpeg_inBuf = (uint8*) vidrec_pJpegAviIn + 1;

            if (g_VidRecCntx.AVIFrameCnt == 0)
            {
                memcpy((void*)vidrec_amr_first_frame, (const void*)amrHead, 6);
                memcpy((void*)(vidrec_amr_first_frame + 6), (const void*)ptmp, (g_VidRecCntx.pingInDataBlockNum));

                avi_enc_info.Amr_Data_Size = (g_VidRecCntx.pingInDataBlockNum) + 6;
                avi_enc_info.Amr_inBuf = vidrec_amr_first_frame;
            }
            else
            {
                avi_enc_info.Amr_Data_Size = (g_VidRecCntx.pingInDataBlockNum);
                avi_enc_info.Amr_inBuf =(uint8*) ptmp;
            }

            avi_frame_length = MMF_Avi_Enc_Frame(&avi_enc_info);
            /****************have no much enough amr data to integrate this frame, just discard this jpeg*********************/
            MCI_TRACE (MCI_VIDREC_TRC,0,
                       "------vidrec_EncodeRun: JPEG_Size 0x%x AMR_Size 0x%x AVI_Size 0x%x",
                       avi_enc_info.Jpeg_Frame_Size, avi_enc_info.Amr_Data_Size, avi_frame_length);

            g_VidRecCntx.AVIFrameCnt++;
            g_VidRecCntx.curFileSize += avi_frame_length + VIDREC_AVI_SINGLE_INDEX_LEN;

            /*
                        AMR_Data_Store -= g_VidRecCntx.pingInDataBlockNum;
                        AMR_buffer_heard += g_VidRecCntx.pingInDataBlockNum;
                        if (AMR_buffer_heard >= (AMR_buffer+(VSRECBUF_LEN<<5)))
                        {
                            AMR_buffer_heard = AMR_buffer;
                        }
            */

            //MCI_TRACE (MCI_VIDREC_TRC,0,"------begin write %d byte from:%x,at:%d\n",avi_frame_length,avi_enc_info.Avi_outBuf,hal_TimGetUpTime());
            //count = FS_Write(g_VidRecCntx.fileHandle, vidrec_avi_out, (int32)avi_frame_length);
            count = avi_frame_length;
            MCI_TRACE(MCI_CAMERA_TRC, 0, "[MMC_videorecord]write file begin: %d",hal_TimGetUpTime());
            if(0 != FS_AsynWriteReq(g_VidRecCntx.fileHandle, avi_enc_info.Avi_outBuf, (int32)avi_frame_length,(FS_ASNY_FUNCPTR)VidRec_FS_AsynWrite_CB))
            {
                diag_printf("------FS write fail!");
                VidRec_RecordStop();
                g_VidRecCntx.vidrec_callback(MDI_RES_VDOREC_ERR_FAILED);
                return MDI_RES_VDOREC_ERR_FAILED;
            }

            g_FsVidRecInfo.iUsedSize +=(UINT64)avi_frame_length;
            if ((g_FsVidRecInfo.iUsedSize + AVI_TF_MIN*1024) >= g_FsVidRecInfo.iTotalSize)
            {
                VidRec_RecordStop();
                g_VidRecCntx.vidrec_callback(MDI_RES_VDOREC_DISK_FULL);
                diag_printf("Video Record Finished, DISK_FULL!\n");
                return MDI_RES_VDOREC_DISK_FULL;
            }

            if(video_i >=VIDREC_AVI_FRAME_MAX_COUNT)
            {
                VidRec_RecordStop();
                g_VidRecCntx.vidrec_callback(MDI_RES_VDOREC_REACH_SIZE_LIMIT);
                diag_printf("Video Record File Reach Maximum!\n");
                return MDI_RES_VDOREC_REACH_SIZE_LIMIT;
            }
        }
    }

    amr_in->VocState = VIDREC_VOC_IDLE;

    //MCI_TRACE (MCI_VIDREC_TRC,0,"******* end to write avi: %d--*********",hal_TimGetUpTime());
    return NO_ERRO;
}

error_type VidRec_Init(uint8* pFileName, VIDREC_CONTEXT* amr_in)
{
    JpegAmr_Avi_Init Avi_Init_Info;
    HAL_ERR_T voc_ret = 0;
    CALIB_CALIBRATION_T* calibPtr = calib_GetPointers();

    /**************init avi file info*********************************/
    Avi_Init_Info.Amr_BitRate      = 12800;
    Avi_Init_Info.Audio_Bits         = 16;
    Avi_Init_Info.Audio_Channel   = 1;
    Avi_Init_Info.Audio_SampleRate = 8000;
    Avi_Init_Info.Image_Height    = g_VidRecCntx.settingInfo.imageHeight;
    Avi_Init_Info.Image_Width     = g_VidRecCntx.settingInfo.imageWidth;
    Avi_Init_Info.Video_fps   = (mcd_IsHighCapacityCard() == TRUE)?VIDREC_AVI_VIDEO_FRAME_RATE:(VIDREC_AVI_VIDEO_FRAME_RATE>>1);
    if (MMF_Avi_Enc_Init(&Avi_Init_Info) == FUNC_OK)
        MCI_TRACE (MCI_VIDREC_TRC,0,"Avi Encoder init ok\n");

    // init video record context
    //amr_in->VppCodecMode       = AMR_ENCODE;
    amr_in->AMREncodeUnitCnt  = 0;
    amr_in->AVIFrameCnt = 0;
    amr_in->curFileSize     = AVI_HEAD_SIZE + VIDREC_AVI_INDEX_TABLE_HEAD_LEN;
    amr_in->lastRecordPeriod = 0;
    amr_in->JPEGEncodeFrameCnt = 0; // set init value as 0!
    amr_in->RGBBufStatus[0] = RGB_BUF_NULL;
    amr_in->RGBBufStatus[1] = RGB_BUF_NULL;
    amr_in->VocState        = VIDREC_VOC_IDLE;

    vidrec_VocFirstSchedule = 0;
    vidrec_AmrEncFirstSchedule = 0; // first schedule Voc, reset = 1.
    voc_ret = vpp_AudioJpegEncOpen(VidRec_EncodeFrameIsr);

    switch (voc_ret)
    {
        case HAL_ERR_NO:
            break;

        case HAL_ERR_RESOURCE_RESET:
        case HAL_ERR_RESOURCE_BUSY:
        case HAL_ERR_RESOURCE_TIMEOUT:
        case HAL_ERR_RESOURCE_NOT_ENABLED:
        case HAL_ERR_BAD_PARAMETER:
        case HAL_ERR_UART_RX_OVERFLOW:
        case HAL_ERR_UART_TX_OVERFLOW:
        case HAL_ERR_UART_PARITY:
        case HAL_ERR_UART_FRAMING:
        case HAL_ERR_UART_BREAK_INT:
        case HAL_ERR_TIM_RTC_NOT_VALID:
        case HAL_ERR_TIM_RTC_ALARM_NOT_ENABLED:
        case HAL_ERR_TIM_RTC_ALARM_NOT_DISABLED:
        case HAL_ERR_COMMUNICATION_FAILED:
        case HAL_ERR_QTY:
            diag_printf("fail to call vpp_AMREncOpen(), result: %d \n", voc_ret);
            return ERROR_AMROPENFAIL;

        default:
            break;
    }

    vpp_AudioJpegEncSetInAlgGain(calibPtr->bb->audio[audioItf].audioGains.inGainsRecord.alg);

    return NO_ERRO;
}

void Mic_RecMsgHandle_Half(VOID)
{
    MCI_TRACE (MCI_VIDREC_TRC,0,"Mic_Interrupt_half start!Msg_Wait_Num=%d",Msg_Wait_Num);

    if ((videoRecordState == VidrecRecordStatus) && (vidrec_VocFirstSchedule > 0) )
    {
        Pcm_StartPosition = 0;
        Voc_Amr_Need = 1;
        Voc_Amr_Count = 0;
        //if(Msg_Wait_Num <= 0)
        VidRec_Pcm2Amr();
    }

    MCI_TRACE (MCI_VIDREC_TRC,0,"Mic_Interrupt_half over!");
}

void Mic_RecMsgHandle_End(VOID)
{
    MCI_TRACE (MCI_VIDREC_TRC,0,"Mic_Interrupt_end start!Msg_Wait_Num=%d",Msg_Wait_Num);

    if ((videoRecordState == VidrecRecordStatus) && (vidrec_VocFirstSchedule > 0) )
    {
        Pcm_StartPosition = 1;
        Voc_Amr_Need = 1;
        Voc_Amr_Count = 0;
        //if(Msg_Wait_Num <= 0)
        VidRec_Pcm2Amr();
    }

    MCI_TRACE (MCI_VIDREC_TRC,0,"Mic_Interrupt_end over!");
}

int32 VidRec_PreviewStart(MMC_VDOREC_SETTING_STRUCT* previewPara, void(*vid_rec_finish_ind)(uint16 msg_result) )
{
    VOID* InvalideAddress = (VOID*)convertaddr(NULL);

    MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT , 0,"[MMC_VIDREC]VidRec_PreviewStart!");

    if ((videoRecordState == VidrecStopStatus) || (videoRecordState == NoneStatus))
    {
        videoRecordState = VidrecPreviewStatus;

        memset(&g_VidRecCntx, 0x00, sizeof(VIDREC_CONTEXT));
        *(&(g_VidRecCntx.settingInfo)) = *previewPara;
        g_VidRecCntx.vidrec_callback = vid_rec_finish_ind;

        MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT, 0, "[MMC_VIDREC] img_h %d img_w %d",
                  g_VidRecCntx.settingInfo.imageHeight, g_VidRecCntx.settingInfo.imageWidth);

        //vidrec_pAvi_Buff: to store the AVI frame which composed of both MJPEG and AMR
        vidrec_pAvi_Buff = (uint8*)convertaddr(mmc_MemMalloc((AVI_BUF_SIZE*AVI_BUF_NUM)<<10));
        if(vidrec_pAvi_Buff == InvalideAddress)
        {
            MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT, 0, "[MMC_VIDREC_ERROR]NO ENOUGH MEMORY!");
            mmc_MemFreeAll();
            return MDI_RES_VDOREC_ERR_MEMORY_NOT_ENOUGH;
        }
        vidrec_avi_out = vidrec_pAvi_Buff;

#if (GALLITE_IS_8805 == 1)
        blendBuffer =
            (UINT16*)convertaddr((mmc_MemMalloc((previewPara->previewWidth*previewPara->previewHeight)<<1)));
        if (blendBuffer == InvalideAddress)
        {
            mmc_MemFreeAll();
            return MDI_RES_VDOREC_ERR_MEMORY_NOT_ENOUGH;
        }

        linebuff_asm = (int8* )mmc_MemMalloc(320<<2);
        if(linebuff_asm==NULL)
        {
            MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT , 0, "[MMC_VIDREC_ERROR]NO ENOUGH MEMORY!");
            VidRec_PreviewStop();
            return MDI_RES_VDOREC_ERR_MEMORY_NOT_ENOUGH;
        }
#endif
        // avi_viidx: to store the avi indexes
        avi_viidx = (AVI_video_index *)mmc_MemMalloc((int32)VIDREC_AVI_FRAME_MAX_COUNT*sizeof(AVI_video_index));
        // RecPcmBuffer: to store the PCM data, 8k sample rate, 16kB/s = 320bytes/20ms
        RecPcmBuffer = (uint8 *)convertaddr(mmc_MemMalloc((int32)PCM_REC_BUF_SIZE*sizeof(uint8)));
        // AMR_buffer: to store the AMR frame, 20 ms/f = 32 bytes/f
        AMR_buffer = (uint8 *)convertaddr((mmc_MemMalloc((VSRECBUF_LEN+VSRECBUF_TRANS_LEN)<<5)));
        if(avi_viidx == NULL || RecPcmBuffer == InvalideAddress || AMR_buffer == InvalideAddress)
        {
            MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT , 0,"[MMC_VIDREC_ERROR]NO ENOUGH MEMORY!");
            mmc_MemFreeAll();
            return MDI_RES_VDOREC_ERR_MEMORY_NOT_ENOUGH;
        }
        AMR_buffer_heard = AMR_buffer_tail = AMR_buffer;
        // AMR_buffer_trans: AMR transfer buffer for writing AVI file
        AMR_buffer_trans = AMR_buffer + (VSRECBUF_LEN<<5);
        AMR_buffer_trans_cur = AMR_buffer_trans;

        Msg_Wait_Num = 0;
        AMR_Data_Store = 0;
        //Vidrec_Sensor_Seq = 0;

        if(MCI_CamPowerUp(1, NULL) != 0)
        {
            diag_printf(" MCI_CamPowerUp is failed\n");
            return MDI_RES_VDOREC_ERR_FAILED;
        }

#ifdef VID_REC_CAP_DUAL_BUF
        sensorbuf_dual[0]= sensorbuf;
        sensorbuf_dual[1]=
            (UINT16*)convertaddr((mmc_MemMalloc((CameraInfo.prev_width*CameraInfo.prev_height)<<1)));
        if (sensorbuf_dual[1] == InvalideAddress)
        {
            MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT , 0, "[MMC_VIDREC_ERROR]sensorbuf_dual NO ENOUGH MEMORY!");
            mmc_MemFreeAll();
            return MDI_RES_VDOREC_ERR_MEMORY_NOT_ENOUGH;
        }
        sensorbuf_w_id=0;
        g_vidBuf = sensorbuf_dual[sensorbuf_w_id];
#else
        g_vidBuf = sensorbuf;
#endif

        vidrec_VideoSampleStart(previewPara);

#ifdef __CAMERA_FULL_OSD__
        if (CameraInfo.img_rotate != ROTATE_0)
        {
            rotateBuf = (UINT16*)convertaddr(mmc_MemMalloc((CameraInfo.prev_width*CameraInfo.prev_height)<<1));
            if (rotateBuf == InvalideAddress)
            {
                MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT , 0,"[MMC_VIDREC_ERROR]NO ENOUGH MEMORY!");
                VidRec_PreviewStop();
                return MDI_RES_VDOREC_ERR_MEMORY_NOT_ENOUGH;
            }
            memset((UINT8*)rotateBuf, 0, ((CameraInfo.prev_width*CameraInfo.prev_height)<<1));
        }
#endif

        // vidrec_JpegBuff0: to store the MJPEG output from VoC
        vidrec_JpegBuff0 = (int8* )convertaddr((mmc_MemMalloc((JPEG_BUF_SIZE*JPEG_BUF_NUM)<<10)));
        if(vidrec_JpegBuff0 == InvalideAddress)
        {
            MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT , 0,"[MMC_VIDREC_ERROR]NO ENOUGH MEMORY!");
            VidRec_PreviewStop();
            return MDI_RES_VDOREC_ERR_MEMORY_NOT_ENOUGH;
        }
        vidrec_JpegBuff1 = vidrec_JpegBuff0 + (JPEG_BUF_SIZE<<10);

        // vidrec_avi_end: to store the end info of the AVI file
        vidrec_avi_end = (uint8*)mmc_MemMalloc(30<<10); // 30k
        if(vidrec_avi_end == NULL)
        {
            MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT , 0,"[MMC_VIDREC_ERROR]NO ENOUGH MEMORY!");
            VidRec_PreviewStop();
            return MDI_RES_VDOREC_ERR_MEMORY_NOT_ENOUGH;
        }
        MCI_TRACE (MCI_VIDREC_TRC,0,"vidrec_avi_out = %p \n",vidrec_avi_out);
        MCI_TRACE (MCI_VIDREC_TRC,0,"avi_viidx = %p \n",avi_viidx);
        MCI_TRACE (MCI_VIDREC_TRC,0,"RecPcmBuffer = %p \n",RecPcmBuffer);
        MCI_TRACE (MCI_VIDREC_TRC,0,"AMR_buffer_heard = %p, AMR_buffer_tail = %p, AMR_buffer = %p \n", AMR_buffer_heard, AMR_buffer_tail, AMR_buffer);
        MCI_TRACE (MCI_VIDREC_TRC,0,"[MMC_VIDREC]vidrec_JpegBuff0 = 0x%x",vidrec_JpegBuff0);

        return MDI_RES_VDOREC_SUCCEED;
    }

    MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT , 0,"[MMC_VIDREC]VidRec is already working!");
    return MDI_RES_VDOREC_ERR_FAILED;
}

int32 VidRec_PreviewStop(void)
{
    MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT , 0,"[MMC_VIDREC]VidRec_PreviewStop!");

    if (videoRecordState == VidrecPreviewStatus)
    {
        videoRecordState = VidrecStopStatus;
        MCI_CamPowerDown();
    }

    return NO_ERRO;
}

// Open mic for video recoder zhangl 2008-11-03
AUD_ERR_T VidRec_MicStart(VOID)
{
    AUD_LEVEL_T audioCfg;
    AUD_ERR_T audErr;
    HAL_AIF_STREAM_T pcmStream;

    // Set audio interface
    audioCfg.spkLevel = AUD_SPK_MUTE;
    audioCfg.micLevel = AUD_MIC_ENABLE;
    audioCfg.sideLevel = AUD_SIDE_VOL_3;//AUD_SIDE_MUTE;
    audioCfg.toneLevel = 0;

    // PCM Audio stream, input of VPP
    pcmStream.startAddress = (UINT32*)RecPcmBuffer;
    pcmStream.length = PCM_REC_BUF_SIZE;//sizeof(HAL_SPEECH_PCM_BUF_T);
    pcmStream.sampleRate = HAL_AIF_FREQ_8000HZ;
    pcmStream.channelNb = HAL_AIF_MONO;
    pcmStream.voiceQuality = TRUE;
    pcmStream.playSyncWithRecord = FALSE;
    pcmStream.halfHandler = Mic_RecMsgHandle_Half; // Mechanical interaction with VPP's VOC
    pcmStream.endHandler = Mic_RecMsgHandle_End;//NULL; // Mechanical interaction with VPP's VOC

    if(audioItf!=AUD_ITF_EAR_PIECE&&audioItf!=AUD_ITF_RECEIVER)
    {
        // Initial cfg
        audioItf = AUD_ITF_RECEIVER;
    }

    audErr = aud_StreamRecord(audioItf, &pcmStream, &audioCfg);

    if(audErr == AUD_ERR_NO)
        MCI_TRACE (MCI_VIDREC_TRC,0,"----audio recoder of the vr is started: ok-----");
    else
        MCI_TRACE (MCI_VIDREC_TRC|TSTDOUT,0,"[MMC_VIDREC_ERROR]audio recoder of the video recorder is started: failure with %d-----",audErr);

    return audErr;
}
/*+ zhouqin modify for video record in secord card20110530*/
#if defined(DUAL_TFLASH_SUPPORT)
extern BOOL mmi_vdorec_check_storage(void);
#endif
/*- zhouqin modify for video record in secord card20110530*/
int32 VidRec_RecordStart(HANDLE filehandle)
{
    MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT , 0, "[MMC_VIDREC]VidRec_RecordStart!");
    F_tick = 0;
    L_tick = 0;
    //Audio_one_minute_interrupt = 0;
    gVidrecTimeout = 16384/((mcd_IsHighCapacityCard() == TRUE) ? VIDREC_AVI_VIDEO_FRAME_RATE : (VIDREC_AVI_VIDEO_FRAME_RATE>>1));

    //gVidrecTimeout = gVidrecTimeout*96/100;
    g_VidRecCntx.fileHandle = filehandle;
    if (videoRecordState == VidrecPreviewStatus)
    {
        /*******initialization of  the file process ********/

        /*+ zhouqin modify for video record in secord card20110530*/
#if defined(DUAL_TFLASH_SUPPORT)
        if(mmi_vdorec_check_storage()== TRUE)
            FS_GetFSInfo("mmc1", &g_FsVidRecInfo);
        else
#endif
            /*- zhouqin modify for video record in second card20110530*/
            FS_GetFSInfo("mmc0", &g_FsVidRecInfo);

        if ((g_FsVidRecInfo.iUsedSize + AVI_TF_MIN*1024) >= g_FsVidRecInfo.iTotalSize)
        {
            videoRecordState = VidrecStopStatus;

            MCI_CamPowerDown();

            diag_printf("Video Record Finished, DISK_FULL!\n");
            return MDI_RES_VDOREC_REACH_DISK_LIMIT;
        }

        memset(vidrec_avi_head, 0x00, AVI_HEAD_SIZE);

        FS_Write(g_VidRecCntx.fileHandle, vidrec_avi_head, AVI_HEAD_SIZE);

        //Voc_AmrEnc_Busy = 0;
        //Voc_JpegEnc_Busy = 0;
        clearbit(&Voc_Busy, BIT_JPEG_ENC_REQ);
        clearbit(&Voc_Busy, BIT_JPEG_ENC_BUSY);
        clearbit(&Voc_Busy, BIT_AMR_ENC_REQ);
        clearbit(&Voc_Busy, BIT_AMR_ENC_BUSY);

        // ////////////////// AVI info init, video record context init and voc init  //////////////////////
        if (VidRec_Init("", &g_VidRecCntx) != NO_ERRO)
            return MDI_RES_VDOREC_ERR_FAILED;

        g_AMR_DataInCount = 0;

        if (g_VidRecCntx.settingInfo.IfRecordAudio)
        {
            diag_printf( "----sound recorder is started----\n" );
            VR_Init = 0;
            VR_Amr_Init = 0;
            Voc_Amr_Count = 0;
            memset(RecPcmBuffer, 0x0, PCM_REC_BUF_SIZE*sizeof(uint8));
            memset(AMR_buffer, 0, 32*VSRECBUF_LEN*sizeof(uint8));
            VidRec_MicStart();
        }
        else
        {
            diag_printf( "----sound recorder is off----\n" );
            // fill the AMR output buffer with 0
            memset(AMR_buffer, 0, 32*VSRECBUF_LEN*sizeof(uint8));
        }

        videoRecordState = VidrecRecordStatus;
        Avi_Out_Index = 0;
        vidrec_AviNuminBuff = 0;
        Voc_JpegOut_Seq = 0;
        vidrec_pJpegVocOut = vidrec_JpegBuff1;
        vidrec_pJpegAviIn = vidrec_JpegBuff0;
        vidrec_pJpeg_sizeVoc = &vidrec_Jpeg1_size;
        vidrec_pJpeg_sizeAvi = &vidrec_Jpeg0_size;

        g_VidRecCntx.recordStartTime = hal_TimGetUpTime();//cyg_current_time(); changde by zhangl
        //   test_video=0;

        sxr_StartFunctionTimer(gVidrecTimeout, vidRec_Callback, NULL, 0);
        return MDI_RES_VDOREC_SUCCEED;
    }

    MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT , 0, "[MMC_VIDREC]VidRec: not in previewing state!");
    return MDI_RES_VDOREC_ERR_FAILED;
}

int32 VidRec_RecordPause(void)
{
    if (videoRecordState == VidrecRecordStatus)
    {
//        videoRecordState = VidrecPauseStatus;
        sxr_StopFunctionTimer(vidRec_Callback);

        if (g_VidRecCntx.settingInfo.IfRecordAudio)
        {
            aud_StreamPause(audioItf, TRUE);
        }

        videoRecordState = VidrecPauseStatus;

        g_VidRecCntx.VocState = VIDREC_VOC_IDLE;

        g_VidRecCntx.RGBBufStatus[0] = RGB_BUF_NULL;
        g_VidRecCntx.RGBBufStatus[1] = RGB_BUF_NULL;

        g_VidRecCntx.lastRecordPeriod +=
            (hal_TimGetUpTime()/*cyg_current_time()*/ - g_VidRecCntx.recordStartTime) / VIDREC_DEFAULT_FREQ_TICK_COUNT_ONE_SECOND;
    }

    return MDI_RES_VDOREC_SUCCEED;
}

int32 VidRec_RecordResume(void)
{
    if (videoRecordState == VidrecPauseStatus)
    {
        if (g_VidRecCntx.settingInfo.IfRecordAudio)
        {
            // begint to sound sample
 //           AMR_buffer_heard = AMR_buffer_tail = AMR_buffer;
   //         AMR_buffer_trans_cur = AMR_buffer_trans;
     //       Avi_Out_Index = 0;
       //     AMR_Data_Store = 0;
            //Voc_AmrEnc_Busy = 0;
            //Voc_JpegEnc_Busy = 0;
            //clearbit(&Voc_Busy, BIT_JPEG_ENC_BUSY);
            //clearbit(&Voc_Busy, BIT_AMR_ENC_BUSY);
            aud_StreamPause(audioItf, FALSE);
        }

        // begin to count time
        g_VidRecCntx.recordStartTime = hal_TimGetUpTime();//cyg_current_time(); changed by zhangl
        videoRecordState = VidrecRecordStatus;
        F_tick = 0;
        L_tick = 0;
/*
        if((gVidrecTimeout>0x7FFFFFFF) ||(gVidrecTimeout==0))
        {
            gVidrecTimeout=16384/((mcd_IsHighCapacityCard() == TRUE) ? VIDREC_AVI_VIDEO_FRAME_RATE : (VIDREC_AVI_VIDEO_FRAME_RATE>>1));
        }
*/
        sxr_StartFunctionTimer(gVidrecTimeout, vidRec_Callback, NULL, 0);
    }

    return MDI_RES_VDOREC_SUCCEED;
}

int32 VidRec_RecordGetTime(void)
{
    int32 fps;
    int32 time;
    if(videoRecordState == VidrecStopStatus)
        return 0;

    fps   = (mcd_IsHighCapacityCard() == TRUE)?VIDREC_AVI_VIDEO_FRAME_RATE:(VIDREC_AVI_VIDEO_FRAME_RATE>>1);
    time= (g_VidRecCntx.JPEGEncodeFrameCnt*1000)/fps;
    //MCI_TRACE(MCI_VIDREC_TRC, 0, "[MMC_VIDREC]time=%d",time);
    return time;
}

int32 VidRec_RecordStop(void)
{
    if ((videoRecordState == VidrecRecordStatus) || (videoRecordState == VidrecPauseStatus))
    {
        videoRecordState = VidrecStopStatus;

        MCI_TRACE (MCI_VIDREC_TRC,0, "[COMM]: GET A MESSAGE - REC_STOP_SAMPLE_REQ%d", g_VidRecCntx.settingInfo.IfRecordAudio);

        if (g_VidRecCntx.settingInfo.IfRecordAudio)
        {
            MCI_TRACE (MCI_VIDREC_TRC,0, "[COMM]: GET A MESSAGE - REC_STOP_SAMPLE_REQ");

            if(aud_StreamStop( audioItf ) != 0)
                MCI_TRACE (MCI_VIDREC_TRC,0,"aud_StreamStop close failed");

        }

        //bset = 2;
        //mmc_SetCurrMode(MMC_MODE_IDLE);
        //Lily_Camera_PreviewOff();
        //camera_PowerOff();

        g_AMR_DataInCount = 0;
        vpp_AudioJpegEncClose();
        VidRec_AVIFileClose(&g_VidRecCntx);

        sxr_StopFunctionTimer(vidRec_Callback);
        gVidrecTimeout = 0;
        //mmc_MemFreeAll();
        MCI_CamPowerDown();
    }

    return MDI_RES_VDOREC_SUCCEED;
}

void VidRec_JPEGEncode(int32* buf_in,short width,short height)
{
    vpp_AudioJpeg_ENC_IN_T VocMP4EncIn;

#ifdef __CAMERA_FULL_OSD__
    if (CameraInfo.img_rotate&1)
        VocMP4EncIn.SampleRate = (cutwidth<<16)|cutheight;
    else
#endif
        VocMP4EncIn.SampleRate = (cutheight<<16)|cutwidth;

    VocMP4EncIn.mode = MMC_MJPEG_ENCODE;  // update ram according to global  mode
    VocMP4EncIn.imag_width = width;//VIDEO_RECORD_DISP_WIDTH;
    VocMP4EncIn.imag_height = height;//VIDEO_RECORD_DISP_HEIGHT;

#if defined(VDOREC_MODE_VGA)
    VocMP4EncIn.imag_quality = 0;
#else
    VocMP4EncIn.imag_quality = 1;
#endif
    g_VidRecCntx.VocState = VIDREC_JPEG_ENC_BUSY;

    VocMP4EncIn.inStreamBufAddr = (INT32*)buf_in;   //gp_VideoRecordAFrameRGBBuf;
    VocMP4EncIn.outStreamBufAddr = (INT32*)(vidrec_pJpegVocOut+ 608);
#ifdef __CAMERA_FULL_OSD__
    VocMP4EncIn.channel_num = 0;
#else
    VocMP4EncIn.channel_num = 1;
#endif
    if (VR_Init==0)
    {
        VR_Init = 1;
        VocMP4EncIn.reset = 1;
    }

    if(HAL_ERR_NO != vpp_AudioJpegEncScheduleOneFrame(&VocMP4EncIn))
    {
        //Voc_JpegEnc_Busy = 0;
        clearbit(&Voc_Busy, BIT_JPEG_ENC_BUSY);
        diag_printf ("[MMC_VIDREC_ERROR]Failed to call VOC in VidRec_JPEGEncode");
    }
    VPP_WRITE_JPEGHeadr(VocMP4EncIn.imag_quality, width, height, (uint8*)vidrec_pJpegVocOut);

    MCI_TRACE(MCI_CAMERA_TRC, 0, "[MMC_videorecord]JPEG: %d",hal_TimGetUpTime());
}


void VidRec_FS_AsynWrite_CB(void* pParam)
{
    FS_ASYN_READ_RESULT *sRsp ;
    MCI_TRACE(MCI_CAMERA_TRC, 0, "[MMC_videorecord]write file end: %d",hal_TimGetUpTime());

    sRsp = (FS_ASYN_READ_RESULT*)pParam;

    if(sRsp->iResult >= 0)
    {
        vidrec_AviNuminBuff--;
    }
    else //fs write file
    {
        vidrec_AviNuminBuff = 0;

        diag_printf("[MMC_VIDREC_ERROR]FS write fail!");
        VidRec_RecordStop();
        g_VidRecCntx.vidrec_callback(MDI_RES_VDOREC_ERR_FAILED);
    }
    return;
}

error_type VidRec_AVIFileClose(VIDREC_CONTEXT* amr_in)
{
    JpegAmr_Avi_End Avi_End_Input_Info;
    Avi_End_Return Avi_End_Output_Info;
    int32 count;

    /************write avi end data****************************/
    Avi_End_Input_Info.Buf_size = 512;  // VIDREC_AVI_FRAME_MAX_COUNT * 32 + 8;
    Avi_End_Input_Info.pAvi_End_Buf = vidrec_avi_end;
    Avi_End_Output_Info.OutByte_size = 0;
    Avi_End_Output_Info.RemainByte_size = 0;

    while(vidrec_AviNuminBuff > 0)
    {
        diag_printf(" vidrec_AviNuminBuff =%d\n",vidrec_AviNuminBuff);
        sxr_Sleep(100);
    }

    do
    {
        if (MMF_Avi_Enc_End(&Avi_End_Input_Info, &Avi_End_Output_Info) == FUNC_OK)
            FS_Write((int32)amr_in->fileHandle, vidrec_avi_end, (int32)Avi_End_Output_Info.OutByte_size);
    }
    while (Avi_End_Output_Info.RemainByte_size > 0);


    /*************seek and write avi head********************************/
    count = (INT32)FS_Seek((int32)amr_in->fileHandle, 0, FS_SEEK_SET);
    MMF_Avi_Enc_Head(vidrec_avi_head);
    count = FS_Write((int32)amr_in->fileHandle, vidrec_avi_head, AVI_HEAD_SIZE);

    // here should call the callback function to delete it
    if(amr_in->curFileSize < AVI_SIZE_MIN)
    {
        diag_printf(" amr_in->curFileSize =%d\n", amr_in->curFileSize);

        g_VidRecCntx.vidrec_callback((uint16)MDI_RES_VDOREC_ERR_FILE_SIZE);
    }

    return NO_ERRO;
}

void VidRec_CallbackFun(UINT8 id,  UINT8 ActiveBufNum)
{
    Cam_Data_Valid = id;
    MCI_TRACE (MCI_VIDREC_TRC, 0,
               "[MMC_VIDREC]VidRec_CallbackFun TIME:%d,NUM:%d,ID:%d", hal_TimGetUpTime(), Msg_Wait_Num, id);

    //send message for preview and capture the next picture
    if(Msg_Wait_Num <= 0)
    {
#ifdef VID_REC_CAP_DUAL_BUF
        g_vidBuf = sensorbuf_dual[sensorbuf_w_id];
        sensorbuf_w_id =1-sensorbuf_w_id;
#endif
        mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VIDREC_SENSOR_IMAGE_READY);
        Msg_Wait_Num++;
    }
    else
    {
        MCI_TRACE (MCI_VIDREC_TRC, 0,
                   "[MMC_VIDREC]one sensor image is discarded,Msg_Wait_Num=%d",Msg_Wait_Num);
    }

#ifdef VID_REC_CAP_DUAL_BUF
    //if Msg_Wait_Num>0 just rewrite last sensor buf
    camera_PrepareImageCaptureL(CAM_FORMAT_RGB565, CameraInfo.prev_size,sensorbuf_dual[sensorbuf_w_id]);
#endif

}

void    VidRec_FrameOutput(void)
{
    error_type avi_inte_result=NO_ERRO;

    if (videoRecordState != VidrecRecordStatus) return;

    do
    {
        MCI_TRACE (MCI_VIDREC_TRC,0,"[MMC_VIDREC]AMR_Data_Store:%d", AMR_Data_Store);
        if(Avi_Out_Index > 0)
        {
            if(NO_ERRO != (avi_inte_result = vidrec_EncodeRun(&g_VidRecCntx)))
            {
                MCI_TRACE (MCI_VIDREC_TRC|TSTDOUT, 0, "[MMC_VIDREC_ERROR]vidrec_EncodeRun Error:%d", avi_inte_result);
                g_VidRecCntx.vidrec_callback((uint16)avi_inte_result);
                return;
            }
        }
        else
        {
            //MCI_TRACE (MCI_VIDREC_TRC|TSTDOUT,0,"[MMC_VIDREC]THE FISRST FRAME IS BAD!");
            MCI_TRACE (MCI_VIDREC_TRC|TSTDOUT,0,"[MMC_VIDREC]Video data not ready!");
            AMR_Data_Store = -1 ; //discard the amr_frame before the first jpeg
            AMR_buffer_tail = AMR_buffer;
            break;
        }
        sxr_Sleep(48);//release cpu for MMI task
    }
    while(AMR_Data_Store >= 10 && !testbit(&Voc_Busy,BIT_JPEG_ENC_BUSY));
    //}while(AMR_Data_Store >= 10 && Voc_JpegEnc_Busy == 0);
}

void VidRec_UserMsg(COS_EVENT *pnMsg)
{
    switch (pnMsg->nEventId)
    {
        case  MSG_VIDREC_SENSOR_IMAGE_READY:
            MCI_TRACE (MCI_VIDREC_TRC,0,"MSG_VIDREC_SENSOR_IMAGE_READY");
#ifdef _CAM_SPECIAL_GC6113_
            mmc_camResize((UINT8*)sensorbuf, ((CameraInfo.prev_width*CameraInfo.prev_height)<<3), (UINT8*)sensorbuf);
#endif
            VidRec_PreCap();
            break;

        case MSG_VIDREC_AVI_OUTPUT_REQ:
            MCI_TRACE (MCI_VIDREC_TRC,0,"MSG_VIDREC_AVI_OUTPUT_REQ");
            VidRec_FrameOutput();
            break;

        case EV_TIMER:
            switch(pnMsg->nParam1)
            {
                case MMC_CAMERA_TIMER_ID:
                    MCI_TRACE(MCI_VIDREC_TRC, 0,"[MMC_VIDREC]Camera_UserMsg  EV_TIMER");
                    if(bset < 2)//bset==2 means the camera is power off
                        Lily_Camera_SetEffect(CAM_PARAM_WB,(uint32)g_VidRecCntx.settingInfo.whiteBalance);
                    break;
                default:
                    MCI_TRACE(MCI_VIDREC_TRC, 0,"[MMC_VIDREC]Invalid timer ID in camera");
                    break;
            }
            break;

        default:
            break;
    }
}

void VidRec_PreCap(void)
{
    MCI_TRACE (MCI_VIDREC_TRC,0,"[MMC_VIDREC]VidRec_PreCap! tick %d", hal_TimGetUpTime());

#define VALID_STATUS 0

    if (videoRecordState == VidrecStopStatus) return;

    camera_cut_image(g_vidBuf, g_vidBuf, CameraInfo.prev_width, CameraInfo.prev_height, cutwidth, cutheight);
    if ((videoRecordState == VidrecRecordStatus) && (vidrec_VocFirstSchedule > 0))
    {
#ifdef __CAMERA_FULL_OSD__
        if (CameraInfo.img_rotate != ROTATE_0)
        {
            vidRec_rotate(g_vidBuf, rotateBuf, CameraInfo.img_rotate, cutwidth, cutheight);
        }
#endif
        // the operation related to video recoder is to be added next
        if(Cam_Data_Valid == VALID_STATUS)
        {
            setbit(&Voc_Busy, BIT_JPEG_ENC_REQ);
            //if (Voc_AmrEnc_Busy == 0 && Voc_JpegEnc_Busy == 0)
            if (Voc_Busy == (1<<BIT_JPEG_ENC_REQ))
            {
                //Voc_JpegEnc_Busy = 1;
                setbit(&Voc_Busy, BIT_JPEG_ENC_BUSY);
                clearbit(&Voc_Busy, BIT_JPEG_ENC_REQ);
                Voc_Interrupt_From = 0;
                Voc_Jpg_Need = 0;
#ifdef __CAMERA_FULL_OSD__
                if (CameraInfo.img_rotate != ROTATE_0)
                {
                    VidRec_JPEGEncode((int32*)(rotateBuf),
                                      g_VidRecCntx.settingInfo.imageWidth,
                                      g_VidRecCntx.settingInfo.imageHeight);
                }
                else
#endif
                {
                    VidRec_JPEGEncode((int32*)(g_vidBuf),
                                      g_VidRecCntx.settingInfo.imageWidth,
                                      g_VidRecCntx.settingInfo.imageHeight);
                }
            }
            else
            {
                /*MCI_TRACE (MCI_VIDREC_TRC, 0, "[MMC_VIDREC_WARN]1 sensor image waiting,Voc_JpegEnc_Busy=%d,Voc_AmrEnc_Busy=%d",
                            Voc_JpegEnc_Busy, Voc_AmrEnc_Busy);*/
                clearbit(&Voc_Busy, BIT_JPEG_ENC_REQ);
                MCI_TRACE (MCI_VIDREC_TRC, 0, "[MMC_VIDREC_WARN]1 sensor image waiting,Voc_Busy=%d", Voc_Busy);
                Voc_Jpg_Need = 1;
            }
        }
    }

    if( Cam_Data_Valid == VALID_STATUS )
    {
        LCDD_ERR_T err;
        HAL_GOUDA_VID_LAYER_DEF_T   gouda_vid_def = {0, };

#if (GALLITE_IS_8805 == 1)
        VidRec_bilinear_zoom((uint8*)((uint32)g_vidBuf),
                             (int)srcwidth, (int)srcheight, (int)cutwidth, (int)cutheight, (uint8*)blendBuffer,
                             (int)CameraInfo.prev_width, (int)CameraInfo.prev_height, linebuff_asm);
        gouda_vid_def.addrY = (UINT32*)(blendBuffer);
        gouda_vid_def.height = CameraInfo.prev_height;
        gouda_vid_def.width = CameraInfo.prev_width;
        gouda_vid_def.stride = CameraInfo.prev_width*2;
#else
        gouda_vid_def.addrY = (UINT32*)(g_vidBuf);
        gouda_vid_def.height =  cutheight;
        gouda_vid_def.width = cutwidth;
        gouda_vid_def.stride = cutwidth<<1;
#endif
#ifdef __CAMERA_FULL_OSD__
        gouda_vid_def.fmt = HAL_GOUDA_IMG_FORMAT_RGB565;
#else
        gouda_vid_def.fmt = HAL_GOUDA_IMG_FORMAT_YUYV;
#endif
        gouda_vid_def.alpha = 0xFF;
        gouda_vid_def.cKeyColor = 0;
        gouda_vid_def.cKeyEn = FALSE;
        gouda_vid_def.cKeyMask = HAL_GOUDA_CKEY_MASK_OFF;
#ifdef __CAMERA_FULL_OSD__
        gouda_vid_def.depth = HAL_GOUDA_VID_LAYER_BETWEEN_1_0;
        //gouda_vid_def.depth = HAL_GOUDA_VID_LAYER_BETWEEN_2_1;
#else
        gouda_vid_def.depth = HAL_GOUDA_VID_LAYER_BETWEEN_2_1;
#endif
        gouda_vid_def.pos.tlPX = CameraInfo.prev_win.start_x;
        gouda_vid_def.pos.tlPY = CameraInfo.prev_win.start_y;
        gouda_vid_def.pos.brPX = CameraInfo.prev_win.start_x + CameraInfo.prev_win.width - 1;
        gouda_vid_def.pos.brPY = CameraInfo.prev_win.start_y + CameraInfo.prev_win.height - 1;

        while(hal_GoudaIsActive());
        hal_GoudaVidLayerClose();
        hal_GoudaVidLayerOpen(&gouda_vid_def);

#if defined(__PRJ_WITH_SPILCD__) && !defined(__PRJ_WITH_SPICAM__)
        extern U16 simulator_Merge_buffer[];
        displayFbw.fb.buffer = simulator_Merge_buffer;
#else
        displayFbw.fb.buffer = NULL;
#endif

        // Display the blended image until lcdd is available
        err = lcdd_Blit16(&displayFbw, displayFbw.roi.x, displayFbw.roi.y);
        while (LCDD_ERR_NO != err)
        {
            sxr_Sleep(10);
            err = lcdd_Blit16(&displayFbw, displayFbw.roi.x, displayFbw.roi.y);
        }
    }

    MCI_TRACE (MCI_VIDREC_TRC,0,"[MMC_VIDREC]finished preview a frame! tick %d", hal_TimGetUpTime());

    //hal_HstSendEvent(0xdcdc6002);
   // hal_HstSendEvent(hal_TimGetUpTime());

#ifndef VID_REC_CAP_DUAL_BUF
#ifdef _CAM_SPECIAL_GC6113_
    camera_PrepareImageCaptureL(CAM_FORMAT_RGB565, (CameraInfo.prev_size<<2), g_vidBuf);
#else
    camera_PrepareImageCaptureL(CAM_FORMAT_RGB565, CameraInfo.prev_size, g_vidBuf);
#endif
#endif

    Msg_Wait_Num--;
}


int32 vidrec_VideoSampleStart(MMC_VDOREC_SETTING_STRUCT* previewPara)
{
    CAM_PREVIEW_STRUCT cam_para_temp = {0,};

    cam_para_temp.image_height = previewPara->imageHeight;
    cam_para_temp.image_width = previewPara->imageWidth;
    cam_para_temp.start_x = previewPara->previewStartX;
    cam_para_temp.start_y = previewPara->previewStartY;
    cam_para_temp.end_x = previewPara->previewWidth + previewPara->previewStartX - 1;
    cam_para_temp.end_y = previewPara->previewHeight + previewPara->previewStartY - 1;
    cam_para_temp.nightmode = previewPara->nightMode;
    cam_para_temp.imageQuality = previewPara->encodeQuality;
    cam_para_temp.factor = previewPara->zoomFactor;
    cam_para_temp.specialEffect = previewPara->specialEffect;
    cam_para_temp.whiteBlance = previewPara->whiteBalance;
    cam_para_temp.contrast = previewPara->contrast;
    cam_para_temp.brightNess = previewPara->brightnessLevel;

    set_mmc_camera_preview();
    MCI_CamPreviewOpen(&cam_para_temp);

    return 0;
}


#else // (CHIP_HAS_GOUDA != 1)


#define  VSRECBUF_LEN 900 //mutiple of 5 ,2 or 3.
#define AVI_SIZE_MIN 1000

volatile  int32 Voc_JpegEnc_Busy=0;
volatile int32 Voc_AmrEnc_Busy=0;
volatile int32 Voc_Jpg_Need=0;
volatile int32 Voc_Amr_Need=0;
volatile int32 Voc_Interrupt_From=-1;//0: VOC interrupt from jpgenc  and 1: VOC interrupt from amrenc
volatile int32 Voc_Amr_Count=0;
volatile int32 Pcm_StartPosition=0;
volatile uint32 Avi_Out_Index=0;
volatile uint32 Jpeg_Out_Index=0;
volatile int32 Effect_Set_Valid=0;
volatile int32 Msg_Wait_Num=0;
volatile int32 Vidrec_Sensor_Seq=0;
volatile int32 Voc_JpegOut_Seq=0;

volatile uint16 * sensor_data_buf;
int8* gp_VidRec_Prebuf;
uint8 * linebuff_asm;
volatile int32   Cam_Data_Valid=-1;
volatile uint32 g_AMR_DataInCount=0;
volatile int32 VR_Init=0;
volatile int32 VR_Amr_Init=0;

#define PCM_REC_BUF_SIZE  160 * 2  * 10
#define AMR_TRANS_COUNT 5
#define AVI_TF_MIN 650 //for 32minutes video index

#ifdef VIDREC_USE_QVGA_SCREEN
#define VIDEO_RECORD_FRAME_RATE 6
#else
#define VIDEO_RECORD_FRAME_RATE 10
#endif

uint8 *AMR_buffer;
volatile uint8 *AMR_buffer_tail;
volatile uint8 *AMR_buffer_heard;
volatile int32 AMR_Data_Store=0;

uint8* RecPcmBuffer;

extern AVI_video_index* avi_viidx;  // bao debug: design the index table according 500 frame
extern uint16  *sensorbuf;//[640*480];
extern uint16* blendBuffer;
extern int video_i;
extern uint32 cam_wbValue;
extern AUD_ITF_T   audioItf;

// //////////////////////////////////////////////////////////////////////////////
// EXTERN FUNCTION PROTOTYPES
// //////////////////////////////////////////////////////////////////////////////
// TODO..

// //////////////////////////////////////////////////////////////////////////////
// DEFINE CONSTANTS
// //////////////////////////////////////////////////////////////////////////////
// video recorder default settings
const MMC_VDOREC_SETTING_STRUCT g_VidrecDefaultSetting =
{
    0,  // previewStartX;
    0,  // previewStartY;
    VIDEO_RECORD_MJPEG_WIDTH, // previewWidth;
    VIDEO_RECORD_MJPEG_HEIGHT,  // previewHeight;

    0,  // zoomFactor;
    CAM_EV_ZERO,  // exposureVerify;

    CAM_WB_AUTO,  // whiteBalance;
    CAM_EFFECT_ENC_NORMAL,  // specialEffect;
    FALSE,  // nightMode;

    TRUE, // IfRecordAudio;
    0,  // fileSizePermit;
    0,  // recordTimePermit;
    CAM_JPG_QTY_HIGH, // encodeQuality;

    0 // reserved;
};

// //////////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES
// //////////////////////////////////////////////////////////////////////////////
// TODO...
VIDREC_CONTEXT g_VidRecCntx;
FS_INFO g_FsVidRecInfo;
int32* vidrec_inbuf;
uint8* vidrec_pAvi_Buff;
uint8* vidrec_avi_out;
uint8* vidrec_pAvi_FS;
int32 vidrec_AviNuminBuff;
uint8* vidrec_JpegBuff0;
uint8* vidrec_JpegBuff1;
volatile uint8* vidrec_pJpegVocOut;
volatile uint8* vidrec_pJpegAviIn;

int32 vidrec_Jpeg0_size;
int32 vidrec_Jpeg1_size;
volatile uint32* vidrec_pJpeg_sizeVoc=NULL;
volatile uint32* vidrec_pJpeg_sizeAvi=NULL;

uint8* vidrec_avi_end;
uint8 vidrec_avi_head[AVI_HEAD_SIZE];
uint8 vidrec_amr_first_frame[32* 30 + 6];
volatile mmcModeStatus videoRecordState = VidrecStopStatus;
int32 vidrec_VocFirstSchedule;

// //////////////////////////////////////////////////////////////////////////////
// LOCAL VARIABLES
// //////////////////////////////////////////////////////////////////////////////
// TODO...
static int32 vidrec_AmrEncFirstSchedule = 0;

extern volatile   CAM_INFO_STRUCT  CameraInfo;
extern volatile   uint16  srcwidth,srcheight;
extern volatile   uint16  cutwidth,cutheight;
extern uint16  *sensorbuf;//[640*480];
extern uint16  *sensorbuf1;
extern uint16  *sensorbuf2;
extern uint16  *blendBuffer;//[MMC_LcdWidth*MMC_LcdHeight];
extern uint16  *backbuffer;
extern CAM_SIZE_T   MY_NPIX;
#if (CSW_EXTENDED_API_LCD == 1)
extern LCDD_FBW_T displayFbw;
#endif
extern volatile uint16  bset;
extern int Lily_Camera_PreviewOff(void);
extern void sxr_Sleep( UINT32 );

void VidRec_EncodeFrameIsr(HAL_VOC_IRQ_STATUS_T* irqStatus)
{
    MCI_TRACE (MCI_VIDREC_TRC,0,"------------Voc_Interrupt_From=%d,Voc_Jpg_Need=%d,Voc_Amr_Need =%d,Voc_Amr_Count=%d",Voc_Interrupt_From,Voc_Jpg_Need,Voc_Amr_Need,Voc_Amr_Count);

    if ((vidrec_VocFirstSchedule != 0)&&(videoRecordState == VidrecRecordStatus))
    {

        if(Voc_Interrupt_From==0)
        {
            Voc_Interrupt_From=-1;
            Avi_Out_Index+=1;
            vpp_AudioJpeg_ENC_OUT_T VocOutputStatus;
            vpp_AudioJpegEncStatus(&VocOutputStatus);
            *vidrec_pJpeg_sizeVoc=VocOutputStatus.output_len;// + 607;
            if(Voc_JpegOut_Seq==0)
            {
                Voc_JpegOut_Seq=1;
                vidrec_pJpegVocOut=vidrec_JpegBuff0;
                vidrec_pJpegAviIn =vidrec_JpegBuff1;
                vidrec_pJpeg_sizeVoc = &vidrec_Jpeg0_size;
                vidrec_pJpeg_sizeAvi = &vidrec_Jpeg1_size;
            }
            else
            {
                Voc_JpegOut_Seq=0;
                vidrec_pJpegVocOut = vidrec_JpegBuff1;
                vidrec_pJpegAviIn = vidrec_JpegBuff0;
                vidrec_pJpeg_sizeVoc = &vidrec_Jpeg1_size;
                vidrec_pJpeg_sizeAvi = &vidrec_Jpeg0_size;
            }
            Voc_JpegEnc_Busy=0;

            ///////////voc processing is over and try to check if there's waiting one!//////////////

            if (Voc_Amr_Need>0)
                VidRec_Pcm2Amr();
        }
        else if (Voc_Interrupt_From==1)
        {
            Voc_Interrupt_From=-1;
            Voc_AmrEnc_Busy=0;
            AMR_Data_Store++;
            if(Voc_Amr_Count>=AMR_TRANS_COUNT)
            {
                Voc_Amr_Count=0;
                Voc_Amr_Need=0;
            }

            if (Voc_Jpg_Need==1&&AMR_Data_Store>=5)
            {
                Voc_JpegEnc_Busy=1;
                Voc_Interrupt_From=0;
                Voc_Jpg_Need=0;
                VidRec_JPEGEncode((int32 *)(sensor_data_buf+srcwidth*((srcheight-cutheight)>>1)+((srcwidth-cutwidth)>>1)),g_VidRecCntx.settingInfo.imageWidth,g_VidRecCntx.settingInfo.imageHeight);
            }
            else if (Voc_Amr_Need>0)
            {
                VidRec_Pcm2Amr();
            }
            else
            {
                ;
                MCI_TRACE (MCI_VIDREC_TRC,0,"------------VOC IDLE!,Voc_Amr_Need=%d",Voc_Amr_Need);
            }
        }
    }
    else
        vidrec_VocFirstSchedule = 1;
    //MCI_TRACE (MCI_VIDREC_TRC,0,"------------Voc_Interrupt over!");
}

error_type VidRec_Init(uint8* pFileName, VIDREC_CONTEXT* amr_in)
{
    JpegAmr_Avi_Init Avi_Init_Info;
    HAL_ERR_T voc_ret = 0;
    CALIB_CALIBRATION_T* calibPtr = calib_GetPointers();

    /**************init avi file info*********************************/
    Avi_Init_Info.Amr_BitRate      = 12800;
    Avi_Init_Info.Audio_Bits       = 16;
    Avi_Init_Info.Audio_Channel    = 1;
    Avi_Init_Info.Audio_SampleRate = 8000;
    Avi_Init_Info.Image_Height     = g_VidRecCntx.settingInfo.imageHeight;
    Avi_Init_Info.Image_Width      = g_VidRecCntx.settingInfo.imageWidth;
    Avi_Init_Info.Video_fps        = VIDREC_AVI_VIDEO_FRAME_RATE;
    if (MMF_Avi_Enc_Init(&Avi_Init_Info) == FUNC_OK)
        MCI_TRACE (MCI_VIDREC_TRC,0,"Avi Encoder init ok\n");

    // init video record context
    //amr_in->VppCodecMode       = AMR_ENCODE;
    amr_in->AMREncodeUnitCnt   = 0;
    amr_in->AVIFrameCnt        = 0;
    amr_in->curFileSize        = AVI_HEAD_SIZE + VIDREC_AVI_INDEX_TABLE_HEAD_LEN;
    amr_in->lastRecordPeriod   = 0;
    amr_in->JPEGEncodeFrameCnt = 0; // set init value as 0!
    amr_in->RGBBufStatus[0]       = RGB_BUF_NULL;
    amr_in->RGBBufStatus[1]       = RGB_BUF_NULL;
    amr_in->VocState              = VIDREC_VOC_IDLE;

    vidrec_VocFirstSchedule    = 0;
    vidrec_AmrEncFirstSchedule = 0; // first schedule Voc, reset = 1.
    voc_ret                    = vpp_AudioJpegEncOpen(VidRec_EncodeFrameIsr);

    switch (voc_ret)
    {
        case HAL_ERR_NO:
            break;

        case HAL_ERR_RESOURCE_RESET:
        case HAL_ERR_RESOURCE_BUSY:
        case HAL_ERR_RESOURCE_TIMEOUT:
        case HAL_ERR_RESOURCE_NOT_ENABLED:
        case HAL_ERR_BAD_PARAMETER:
        case HAL_ERR_UART_RX_OVERFLOW:
        case HAL_ERR_UART_TX_OVERFLOW:
        case HAL_ERR_UART_PARITY:
        case HAL_ERR_UART_FRAMING:
        case HAL_ERR_UART_BREAK_INT:
        case HAL_ERR_TIM_RTC_NOT_VALID:
        case HAL_ERR_TIM_RTC_ALARM_NOT_ENABLED:
        case HAL_ERR_TIM_RTC_ALARM_NOT_DISABLED:
        case HAL_ERR_COMMUNICATION_FAILED:
        case HAL_ERR_QTY:
            diag_printf("fail to call vpp_AMREncOpen(), result: %d \n", voc_ret);
            return ERROR_AMROPENFAIL;
        default:
            break;
    }

    vpp_AudioJpegEncSetInAlgGain(calibPtr->bb->audio[audioItf].audioGains.inGainsRecord.alg);

    return NO_ERRO;
}

error_type VidRec_AVIFileClose(VIDREC_CONTEXT* amr_in)
{
    JpegAmr_Avi_End Avi_End_Input_Info;
    Avi_End_Return Avi_End_Output_Info;
    int32 count;
//  int16 res = 0;
//  uint32 resultCount=0;

    /************write avi end data****************************/
    Avi_End_Input_Info.Buf_size         = 512;  // VIDREC_AVI_FRAME_MAX_COUNT * 32 + 8;
    Avi_End_Input_Info.pAvi_End_Buf     = vidrec_avi_end;
    Avi_End_Output_Info.OutByte_size    = 0;
    Avi_End_Output_Info.RemainByte_size = 0;

    while(vidrec_AviNuminBuff>0)
    {
        diag_printf(" vidrec_AviNuminBuff =%d\n",vidrec_AviNuminBuff);
        sxr_Sleep(100);
    }
    do
    {
        if (MMF_Avi_Enc_End(&Avi_End_Input_Info, &Avi_End_Output_Info) == FUNC_OK)
            FS_Write((int32)amr_in->fileHandle, vidrec_avi_end, (int32)Avi_End_Output_Info.OutByte_size);
    }
    while (Avi_End_Output_Info.RemainByte_size > 0);
    /*************seek and write avi head********************************/
    count = (INT32)FS_Seek((int32)amr_in->fileHandle, 0, FS_SEEK_SET);
    MMF_Avi_Enc_Head(vidrec_avi_head);
    count = FS_Write((int32)amr_in->fileHandle, vidrec_avi_head, AVI_HEAD_SIZE);

// here should call the callback function to delete it
    if(amr_in->curFileSize <AVI_SIZE_MIN)
    {
        diag_printf(" amr_in->curFileSize =%d\n",amr_in->curFileSize);
        g_VidRecCntx.vidrec_callback((uint16)MDI_RES_VDOREC_ERR_FILE_SIZE);
    }
    return NO_ERRO;
}

int32 VidRec_PreviewStop(void)
{
    if (videoRecordState == VidrecPreviewStatus)
    {
        videoRecordState = VidrecStopStatus;
        MCI_CamPowerDown();
    }
    return NO_ERRO;
}

int32 VidRec_RecordStop(void)
{
    if ((videoRecordState == VidrecRecordStatus) || (videoRecordState == VidrecPauseStatus))
    {
        videoRecordState = VidrecStopStatus;

        MCI_TRACE (MCI_VIDREC_TRC,0, "[COMM]: GET A MESSAGE - REC_STOP_SAMPLE_REQ%d", g_VidRecCntx.settingInfo.IfRecordAudio);

        if (g_VidRecCntx.settingInfo.IfRecordAudio)
        {
            MCI_TRACE (MCI_VIDREC_TRC,0, "[COMM]: GET A MESSAGE - REC_STOP_SAMPLE_REQ");

            if(aud_StreamStop(audioItf)!=0)
                MCI_TRACE (MCI_VIDREC_TRC,0,"aud_StreamStop close failed");

        }

        bset=2;
        mmc_SetCurrMode(MMC_MODE_IDLE);
        Lily_Camera_PreviewOff();
        camera_PowerOff();

        g_AMR_DataInCount = 0;
        vpp_AudioJpegEncClose();
        VidRec_AVIFileClose(&g_VidRecCntx);
        mmc_MemFreeAll();


//    camInPreviewState = FALSE;
    }

    //EVENT_Free(GetLastEvent());
    return MDI_RES_VDOREC_SUCCEED;
}

error_type vidrec_EncodeRun(VIDREC_CONTEXT* amr_in)
{

    int32 count=0;
    // ////////////////// JPEG encode a frame finish ////////////////////////////////////

    amr_in->JPEGEncodeFrameCnt++;

    //  check if finish the whole video recored
    if (((g_VidRecCntx.curFileSize + VIDREC_CHECK_AVI_FILE_SIZE) >=
            g_VidRecCntx.settingInfo.fileSizePermit* 1024)&&(g_VidRecCntx.settingInfo.fileSizePermit > 0))
    {
        diag_printf("[Vidrec Encode Run] file size is reach, file size = %d \n", g_VidRecCntx.curFileSize);
        VidRec_RecordStop();
        g_VidRecCntx.vidrec_callback(MDI_RES_VDOREC_REACH_SIZE_LIMIT);
        diag_printf("Video Record Finished, REACH_SIZE_LIMIT!\n");
        return MDI_RES_VDOREC_REACH_SIZE_LIMIT;
    }

    {
        int32 avi_frame_length = 0;
        JpegAmr_Avi_Enc avi_enc_info;
        static uint8 amrHead[6] = {0x23, 0x21, 0x41, 0x4d, 0x52, 0x0a};

        /*************avi frame integrate***************************/
        if ((videoRecordState == VidrecRecordStatus) &&
                (g_VidRecCntx.JPEGEncodeFrameCnt > 0) &&
                (g_VidRecCntx.settingInfo.IfRecordAudio//&& g_AMR_DataInCount--)
                 || ((!g_VidRecCntx.settingInfo.IfRecordAudio) && (g_VidRecCntx.RGBBufStatus[0] == RGB_BUF_NULL))))
        {

            if (!g_VidRecCntx.settingInfo.IfRecordAudio)
            {
                //diag_printf( "[No sound]: AVI frame package now. \n" );
                g_VidRecCntx.pingInDataBlockNum = VIDREC_AMR_FRAME_NUM_IN_A_PACK;
                //memset(vidrec_outbuf,0x00,VIDREC_AMRENC_OUTBUF_SIZE);
            }

            /**********************VIDREC_AMR_FRAME_MAX_CNT_IN_A_PACK is just for test of zhangl**************************/

            g_VidRecCntx.pingInDataBlockNum = VIDREC_AMR_FRAME_NUM_IN_A_PACK;

            if(AMR_Data_Store<5)
            {
                diag_printf("######discard one jpeg AMR_Data_Store=%d",AMR_Data_Store);
                return NO_ERRO;
            }

            if(vidrec_AviNuminBuff<6)
            {
                avi_enc_info.Avi_outBuf = vidrec_avi_out;
                vidrec_avi_out +=20*1024;
                if(vidrec_avi_out >(vidrec_pAvi_Buff+100*1024))
                    vidrec_avi_out = vidrec_pAvi_Buff;
                vidrec_AviNuminBuff++;
//              MCI_TRACE (MCI_VIDREC_TRC,0," ++vidrec_AviNuminBuff =%d \n",vidrec_AviNuminBuff);

            }
            else
            {
                diag_printf("######discard avi,buffer overflow!vidrec_AviNuminBuff =%d,AMR_Data_Store=%d",vidrec_AviNuminBuff,AMR_Data_Store);
                AMR_Data_Store-=g_VidRecCntx.pingInDataBlockNum;
                AMR_buffer_heard += g_VidRecCntx.pingInDataBlockNum * 32;
                if (AMR_buffer_heard >= (AMR_buffer+VSRECBUF_LEN*32))
                {
                    AMR_buffer_heard = AMR_buffer;
                }
                return NO_ERRO;
            }

            //avi_enc_info.Avi_outBuf = vidrec_avi_out;
            avi_enc_info.Jpeg_Frame_Size = * vidrec_pJpeg_sizeAvi;
            avi_enc_info.Jpeg_inBuf      =(uint8*) vidrec_pJpegAviIn + 1;

            if (g_VidRecCntx.AVIFrameCnt == 0)
            {
                memcpy((void*)vidrec_amr_first_frame, (const void*)amrHead, 6);
                memcpy((void*)(vidrec_amr_first_frame + 6), (const void*)AMR_buffer_heard, g_VidRecCntx.pingInDataBlockNum* 32);

                avi_enc_info.Amr_Data_Size = g_VidRecCntx.pingInDataBlockNum * 32 + 6;
                avi_enc_info.Amr_inBuf     = vidrec_amr_first_frame;
            }
            else
            {
                avi_enc_info.Amr_Data_Size = g_VidRecCntx.pingInDataBlockNum * 32;
                avi_enc_info.Amr_inBuf =(uint8*) AMR_buffer_heard;
            }
            /****************have no much enough amr data to integrate this frame, just discard this jpeg*********************/

            avi_frame_length = MMF_Avi_Enc_Frame(&avi_enc_info);
            g_VidRecCntx.AVIFrameCnt++;
            g_VidRecCntx.curFileSize += avi_frame_length + VIDREC_AVI_SINGLE_INDEX_LEN;

            AMR_Data_Store-=g_VidRecCntx.pingInDataBlockNum;
            AMR_buffer_heard += g_VidRecCntx.pingInDataBlockNum * 32;
            if (AMR_buffer_heard >= (AMR_buffer+VSRECBUF_LEN*32))
            {
                AMR_buffer_heard = AMR_buffer;
            }


            //MCI_TRACE (MCI_VIDREC_TRC,0,"------begin write %d byte from:%x,at:%d\n",avi_frame_length,avi_enc_info.Avi_outBuf,hal_TimGetUpTime());
            //count = FS_Write(g_VidRecCntx.fileHandle, vidrec_avi_out, (int32)avi_frame_length);
            count =avi_frame_length;
            MCI_TRACE(MCI_CAMERA_TRC, 0, "[MMC_videorecord]write file begin0: %d",hal_TimGetUpTime());

            if(0 != FS_AsynWriteReq(g_VidRecCntx.fileHandle, avi_enc_info.Avi_outBuf, (int32)avi_frame_length,(FS_ASNY_FUNCPTR)VidRec_FS_AsynWrite_CB))
            {
                diag_printf("------FS write fail!");
                vidrec_AviNuminBuff--;
                VidRec_RecordStop();
                g_VidRecCntx.vidrec_callback(MDI_RES_VDOREC_ERR_FAILED);
                return MDI_RES_VDOREC_ERR_FAILED;
            }

            g_FsVidRecInfo.iUsedSize += (UINT64) avi_frame_length;
            if ((g_FsVidRecInfo.iUsedSize + AVI_TF_MIN*1024)>= g_FsVidRecInfo.iTotalSize)
            {
                VidRec_RecordStop();
                g_VidRecCntx.vidrec_callback(MDI_RES_VDOREC_DISK_FULL);
                diag_printf("Video Record Finished, DISK_FULL!\n");
                return MDI_RES_VDOREC_DISK_FULL;
            }
            if(video_i >=VIDREC_AVI_FRAME_MAX_COUNT)
            {
                VidRec_RecordStop();
                g_VidRecCntx.vidrec_callback(MDI_RES_VDOREC_REACH_SIZE_LIMIT);
                diag_printf("Video Record File Reach Maximum!\n");
                return MDI_RES_VDOREC_REACH_SIZE_LIMIT;
            }

            if (count !=avi_frame_length)
            {
                diag_printf("Video Record Finished, Write file error!\n");
                VidRec_RecordStop();
                g_VidRecCntx.vidrec_callback(MDI_RES_VDOREC_ERR_FAILED);
                return MDI_RES_VDOREC_ERR_FAILED;
            }

        }
    }
    amr_in->VocState = VIDREC_VOC_IDLE;

    //MCI_TRACE (MCI_VIDREC_TRC,0,"******* end to write avi: %d--*********",hal_TimGetUpTime());
    return NO_ERRO;
}

extern void set_mmc_camera_preview();

int32 VidRec_PreviewStart(MMC_VDOREC_SETTING_STRUCT* previewPara, void(*vid_rec_finish_ind)(uint16 msg_result) )
{
    VOID*  InvalideAddress = (VOID*)convertaddr(NULL);

    MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT , 0,"[MMC_VIDREC]VidRec_PreviewStart!");

    if ((videoRecordState == VidrecStopStatus) || (videoRecordState == NoneStatus)) // actually VidrecStopStatus = NoneStatus
    {
        videoRecordState = VidrecPreviewStatus;

        // copy user's setting to video recorder's context, and save it.
        memset(&g_VidRecCntx, 0x00, sizeof(VIDREC_CONTEXT));
        *(&(g_VidRecCntx.settingInfo)) = *previewPara;
        g_VidRecCntx.vidrec_callback=vid_rec_finish_ind;

        /*****************************************************************************/
//mmc_memory allocation:
// 1. vidrec_avi_out    --30k
// 2. avi_viidx     --37.5k
// 3. RecPcmBuffer  --6.25k
// 4. AMR_buffer        --32k
// 5. sensorbuf     --300k
// 6. backbuffer        --150k
// 7. vidrec_JpegBuff       --40k
// 8. linebuff_asm      -- 1.25k
// 9. gp_VidRec_Prebuf --previewWidth* previewHeight*2 QVGA Screen:84.375k
//10.vidrec_avi_end --30k
        /*----------------------------------------------*/
//total:        751.375k < 800k
        /*****************************************************************************/

        vidrec_pAvi_Buff = (uint8*)convertaddr(mmc_MemMalloc(120* 1024));
        if(vidrec_pAvi_Buff==InvalideAddress)
        {
            MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT , 0,"[MMC_VIDREC_ERROR]NO ENOUGH MEMORY!");
            mmc_MemFreeAll();
            return MDI_RES_VDOREC_ERR_MEMORY_NOT_ENOUGH;
        }

        vidrec_avi_out = vidrec_pAvi_Buff;

        avi_viidx = (AVI_video_index *)mmc_MemMalloc((int32)VIDREC_AVI_FRAME_MAX_COUNT * sizeof(AVI_video_index));
        RecPcmBuffer = (uint8 *)convertaddr(mmc_MemMalloc((int32)PCM_REC_BUF_SIZE * sizeof(uint8)));

        AMR_buffer = (uint8 *)convertaddr((mmc_MemMalloc(32*VSRECBUF_LEN)));

        if(avi_viidx==NULL||RecPcmBuffer==InvalideAddress||AMR_buffer==InvalideAddress)
        {
            MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT , 0,"[MMC_VIDREC_ERROR]NO ENOUGH MEMORY!");
            mmc_MemFreeAll();
            return MDI_RES_VDOREC_ERR_MEMORY_NOT_ENOUGH;
        }

        AMR_buffer_heard = AMR_buffer_tail = AMR_buffer;

        Msg_Wait_Num=0;
        Effect_Set_Valid=0;
        AMR_Data_Store=0;
        Vidrec_Sensor_Seq=0;

        if(MCI_CamPowerUp(1,(MMC_CAMERA_CB)NULL)!=0)
        {
            MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT , 0,"[MMC_VIDREC_ERROR]MCI_CamPowerUp FAILE!!");
            return MDI_RES_VDOREC_ERR_FAILED;
        }

        lily_transfer_data();


        vidrec_JpegBuff0 = (int8* )convertaddr((mmc_MemMalloc(40* 1024)));

        if(vidrec_JpegBuff0==InvalideAddress)
        {
            MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT , 0,"[MMC_VIDREC_ERROR]NO ENOUGH MEMORY!");
            VidRec_PreviewStop();
            return MDI_RES_VDOREC_ERR_MEMORY_NOT_ENOUGH;
        }


        vidrec_JpegBuff1 = vidrec_JpegBuff0+20*1024;

        linebuff_asm=(int8* )mmc_MemMalloc(320*4);
        gp_VidRec_Prebuf=(int8* )convertaddr(mmc_MemMalloc(previewPara->previewWidth* previewPara->previewHeight*2));

        if(linebuff_asm==NULL||gp_VidRec_Prebuf==InvalideAddress)
        {
            MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT , 0,"[MMC_VIDREC_ERROR]NO ENOUGH MEMORY!");
            VidRec_PreviewStop();
            return MDI_RES_VDOREC_ERR_MEMORY_NOT_ENOUGH;
        }


        vidrec_avi_end =(uint8*)mmc_MemMalloc(30*1024);// (uint8*)mmc_MemMalloc( mmc_MemGetFree()- 1024);//VIDREC_AVI_FRAME_MAX_COUNT* 32 + 8  );  // 225*3k

        if(vidrec_avi_end==NULL)
        {
            MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT , 0,"[MMC_VIDREC_ERROR]NO ENOUGH MEMORY!");
            VidRec_PreviewStop();
            return MDI_RES_VDOREC_ERR_MEMORY_NOT_ENOUGH;
        }

        MCI_TRACE (MCI_VIDREC_TRC,0,"[MMC_VIDREC]vidrec_avi_out = 0x%x",vidrec_avi_out);
        MCI_TRACE (MCI_VIDREC_TRC,0,"[MMC_VIDREC]avi_viidx =0x%x",avi_viidx);
        MCI_TRACE (MCI_VIDREC_TRC,0,"[MMC_VIDREC]RecPcmBuffer = 0x%x",RecPcmBuffer);
        MCI_TRACE (MCI_VIDREC_TRC,0,"[MMC_VIDREC]AMR_buffer_heard = 0x%x,AMR_buffer_tail = 0x%x,AMR_buffer = 0x%x",AMR_buffer_heard,AMR_buffer_tail,AMR_buffer);
        MCI_TRACE (MCI_VIDREC_TRC,0,"[MMC_VIDREC]vidrec_JpegBuff0 = 0x%x",vidrec_JpegBuff0);
        vidrec_VideoSampleStart(previewPara);

        return MDI_RES_VDOREC_SUCCEED;
    }
    return MDI_RES_VDOREC_ERR_FAILED;
}

void Mic_RecMsgHandle_Half(VOID)
{
    MCI_TRACE (MCI_VIDREC_TRC,0,"Mic_Interrupt_half start!Msg_Wait_Num=%d",Msg_Wait_Num);
    if ((videoRecordState == VidrecRecordStatus) && (vidrec_VocFirstSchedule > 0) )
    {
        Pcm_StartPosition=0;
        Voc_Amr_Need=1;
        Voc_Amr_Count=0;
        if(Msg_Wait_Num<=0)
            VidRec_Pcm2Amr();
    }
    MCI_TRACE (MCI_VIDREC_TRC,0,"Mic_Interrupt_half over!");
}
void Mic_RecMsgHandle_End(VOID)
{
    MCI_TRACE (MCI_VIDREC_TRC,0,"Mic_Interrupt_end start!Msg_Wait_Num=%d",Msg_Wait_Num);
    if ((videoRecordState == VidrecRecordStatus) && (vidrec_VocFirstSchedule > 0) )
    {
        Pcm_StartPosition=1;
        Voc_Amr_Need=1;
        Voc_Amr_Count=0;
        if(Msg_Wait_Num<=0)
            VidRec_Pcm2Amr();
    }
    MCI_TRACE (MCI_VIDREC_TRC,0,"Mic_Interrupt_end over!");
}

// Open mic for video recoder zhangl 2008-11-03

AUD_ERR_T VidRec_MicStart(VOID)
{
    AUD_LEVEL_T audioCfg;
    AUD_ERR_T audErr;
    HAL_AIF_STREAM_T pcmStream;

    // Set audio interface
    audioCfg.spkLevel = AUD_SPK_MUTE;
    audioCfg.micLevel = AUD_MIC_ENABLE;
    audioCfg.sideLevel = AUD_SIDE_VOL_3;//AUD_SIDE_MUTE;
    audioCfg.toneLevel = 0;

    // PCM Audio stream, input of VPP
    pcmStream.startAddress = (UINT32*)RecPcmBuffer;
    pcmStream.length = PCM_REC_BUF_SIZE;//sizeof(HAL_SPEECH_PCM_BUF_T);
    pcmStream.sampleRate = HAL_AIF_FREQ_8000HZ;
    pcmStream.channelNb = HAL_AIF_MONO;
    pcmStream.voiceQuality = TRUE;
    pcmStream.playSyncWithRecord = FALSE;
    pcmStream.halfHandler = Mic_RecMsgHandle_Half; // Mechanical interaction with VPP's VOC
    pcmStream.endHandler = Mic_RecMsgHandle_End;//NULL; // Mechanical interaction with VPP's VOC

    audErr = aud_StreamRecord(audioItf, &pcmStream, &audioCfg);

    if(audErr==AUD_ERR_NO)
        MCI_TRACE (MCI_VIDREC_TRC,0,"----audio recoder of the vr is started: ok-----");
    else
        MCI_TRACE (MCI_VIDREC_TRC|TSTDOUT,0,"[MMC_VIDREC_ERROR]audio recoder of the video recorder is started: failure with %d-----",audErr);

    return audErr;
}

/*+ zhouqin modify for video record in secord card20110530*/
#if defined(DUAL_TFLASH_SUPPORT)
extern BOOL mmi_vdorec_check_storage(void);
#endif
/*- zhouqin modify for video record in secord card20110530*/
int32 VidRec_RecordStart(HANDLE filehandle)
{

    g_VidRecCntx.fileHandle = filehandle;
    if (videoRecordState == VidrecPreviewStatus)
    {
        MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT , 0,"[MMC_VIDREC]VidRec_RecordStart!");

        /*******initialization of  the file process ********/

        /*+ zhouqin modify for video record in secord card20110530*/
#if defined(DUAL_TFLASH_SUPPORT)
        if(mmi_vdorec_check_storage()== TRUE)
            FS_GetFSInfo("mmc1", &g_FsVidRecInfo);
        else
#endif
            /*- zhouqin modify for video record in secord card20110530*/
            FS_GetFSInfo("mmc0", &g_FsVidRecInfo);

        if ((g_FsVidRecInfo.iUsedSize + AVI_TF_MIN*1024) >= g_FsVidRecInfo.iTotalSize)
        {
            videoRecordState = VidrecStopStatus;

            MCI_CamPowerDown();

            diag_printf("Video Record Finished, DISK_FULL!\n");
            g_VidRecCntx.vidrec_callback(MDI_RES_VDOREC_DISK_FULL);
            return MDI_RES_VDOREC_REACH_DISK_LIMIT;
        }

        memset(vidrec_avi_head, 0x00, AVI_HEAD_SIZE);

        FS_Write(g_VidRecCntx.fileHandle, vidrec_avi_head, AVI_HEAD_SIZE);

        Voc_AmrEnc_Busy=0;
        Voc_JpegEnc_Busy=0;

        // ////////////////// AVI info init, video record context init and voc init  //////////////////////
        if (VidRec_Init("", &g_VidRecCntx) != NO_ERRO)
            return MDI_RES_VDOREC_ERR_FAILED;

        g_AMR_DataInCount = 0;

        if (g_VidRecCntx.settingInfo.IfRecordAudio)
        {
            diag_printf( "----sound recorder is started----\n" );
            VR_Init=0;
            VR_Amr_Init=0;
            Voc_Amr_Count=0;
            memset(RecPcmBuffer, 0x0, PCM_REC_BUF_SIZE*sizeof(uint8));
            memset(AMR_buffer, 0, 32*VSRECBUF_LEN*sizeof(uint8));
            VidRec_MicStart();

        }
        else
        {
            // fill the AMR output buffer with 0
            memset(AMR_buffer, 0, 32*VSRECBUF_LEN*sizeof(uint8));

        }

        videoRecordState = VidrecRecordStatus;
        Avi_Out_Index=0;
        vidrec_AviNuminBuff=0;
        Voc_JpegOut_Seq=0;
        vidrec_pJpegVocOut = vidrec_JpegBuff1;
        vidrec_pJpegAviIn = vidrec_JpegBuff0;
        vidrec_pJpeg_sizeVoc = &vidrec_Jpeg1_size;
        vidrec_pJpeg_sizeAvi = &vidrec_Jpeg0_size;

        g_VidRecCntx.recordStartTime = hal_TimGetUpTime();//cyg_current_time(); changde by zhangl
        //   test_video=0;
        return MDI_RES_VDOREC_SUCCEED;
    }
    return MDI_RES_VDOREC_ERR_FAILED;
}

int32 VidRec_RecordPause(void)
{
    if (videoRecordState == VidrecRecordStatus)
    {
        videoRecordState = VidrecPauseStatus;

        if (g_VidRecCntx.settingInfo.IfRecordAudio)
        {

            aud_StreamPause(audioItf, TRUE);
        }

        g_VidRecCntx.VocState = VIDREC_VOC_IDLE;

        g_VidRecCntx.RGBBufStatus[0] = RGB_BUF_NULL;
        g_VidRecCntx.RGBBufStatus[1] = RGB_BUF_NULL;

        g_VidRecCntx.lastRecordPeriod +=
            (hal_TimGetUpTime()/*cyg_current_time()*/ - g_VidRecCntx.recordStartTime) / VIDREC_DEFAULT_FREQ_TICK_COUNT_ONE_SECOND;
    }

    return MDI_RES_VDOREC_SUCCEED;
}

int32 VidRec_RecordResume(void)
{
    if (videoRecordState == VidrecPauseStatus)
    {
        if (g_VidRecCntx.settingInfo.IfRecordAudio)
        {
            // begint to sound sample
            AMR_buffer_heard = AMR_buffer_tail = AMR_buffer;
            Avi_Out_Index=0;
            AMR_Data_Store=0;
            Voc_AmrEnc_Busy=0;
            Voc_JpegEnc_Busy=0;
            aud_StreamPause(audioItf, FALSE);
        }

        // begin to count time
        g_VidRecCntx.recordStartTime = hal_TimGetUpTime();//cyg_current_time(); changed by zhangl
        videoRecordState = VidrecRecordStatus;
    }

    return MDI_RES_VDOREC_SUCCEED;
}

void VidRec_Pcm2Amr(void)
{
    if ((Voc_JpegEnc_Busy==0)&&Voc_AmrEnc_Busy==0)//&&(!((PCM_BUF_INT=1)&&(VOC_AMRENC_BUSY))))
    {
        vpp_AudioJpeg_ENC_IN_T   audio_amrenc;

        Voc_AmrEnc_Busy=1;
        Voc_Interrupt_From=1;
        Voc_Amr_Count++;
        //Voc_Amr_Need=1;

        audio_amrenc.inStreamBufAddr=(INT32*)(RecPcmBuffer+(Pcm_StartPosition*PCM_REC_BUF_SIZE/2)+(Voc_Amr_Count-1)*320);
        audio_amrenc.outStreamBufAddr=(INT32*)AMR_buffer_tail;
        AMR_buffer_tail+=32;
        if((AMR_buffer_tail-AMR_buffer)>=32*VSRECBUF_LEN)
            AMR_buffer_tail=AMR_buffer;

        audio_amrenc.mode=MMC_AMR_ENCODE;
        audio_amrenc.channel_num = 1;
        audio_amrenc.SampleRate= HAL_AIF_FREQ_8000HZ;
        audio_amrenc.reset=0;
        if (VR_Amr_Init==0)
        {
            VR_Amr_Init=1;
            audio_amrenc.reset=1;
        }

        if(HAL_ERR_NO!=vpp_AudioJpegEncScheduleOneFrame(&audio_amrenc))
        {
            Voc_AmrEnc_Busy=0;
            diag_printf ("Failed to call VOC in VidRec_Pcm2Amr :%d th",Voc_Amr_Count);
        }
        MCI_TRACE(MCI_CAMERA_TRC, 0, "[MMC_videorecord]AMR1: %d",hal_TimGetUpTime());
    }
    else
    {
        //Voc_Amr_Need=1;
        MCI_TRACE (MCI_VIDREC_TRC,0,"------%d pcm2amr need waiting,Voc_JpegEnc_Busy=%d,Voc_AmrEnc_Busy=%d",Voc_Amr_Need,Voc_JpegEnc_Busy,Voc_AmrEnc_Busy);
    }

}

extern volatile int32 sensorready;

void VidRec_CallbackFun(UINT8 id,  UINT8 ActiveBufNum)
{

    Cam_Data_Valid=id;

    MCI_TRACE (MCI_VIDREC_TRC,0,"[MMC_VIDREC]VidRec_CallbackFun TIME:%d,NUM:%d,ID:%d",hal_TimGetUpTime(),Msg_Wait_Num,id);
    sensorready = 1;
    //send message for preview and capture the next picture
    if(Msg_Wait_Num<=0)
    {
        mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VIDREC_SENSOR_IMAGE_READY);
        Msg_Wait_Num++;
    }
    else
    {
        MCI_TRACE (MCI_VIDREC_TRC,0,"[MMC_VIDREC]one sensor image is discarded,Msg_Wait_Num=%d",Msg_Wait_Num);
    }


    //start to capture image data to sensorbuf
    if( Vidrec_Sensor_Seq==0)
    {
        camera_PrepareImageCaptureL(CAM_FORMAT_RGB565, MY_NPIX, sensorbuf);
        Vidrec_Sensor_Seq=1;
    }
    else
    {
        camera_PrepareImageCaptureL(CAM_FORMAT_RGB565, MY_NPIX, sensorbuf1);
        Vidrec_Sensor_Seq=0;
    }
    camera_CaptureImage();

// diag_printf("12:the last four bytes of vidrec_avi_out :%x \n",*((U32*)(((U32)vidrec_avi_out|0xa0000000)+50* 1024)));
}


extern void VidRec_yuv2rgb(char* srcbuf,int src_w,int src_h,char* desbuf,int padding);
extern void VidRec_yuv2rgb_blend(char* srcbuf,int src_w,int src_h,char* desbuf,char*backbuf,int padding);
extern void VidRec_bilinear_zoom(char* srcbuf,int src_w,int src_h,int cut_w,int cut_h,char* decbuf,int dec_w,int dec_h,char* linebuff);



void VidRec_UserMsg(COS_EVENT *pnMsg)
{
    switch (pnMsg->nEventId)
    {
        case  MSG_VIDREC_SENSOR_IMAGE_READY:
            //MCI_TRACE (MCI_VIDREC_TRC,0,"MSG_VIDREC_SENSOR_IMAGE_READY");
            VidRec_PreCap();
            break;
        case MSG_VIDREC_AVI_OUTPUT_REQ:
            //MCI_TRACE (MCI_VIDREC_TRC,0,"MSG_VIDREC_AVI_OUTPUT_REQ");
            VidRec_FrameOutput();
            break;
        case EV_TIMER:
            switch(pnMsg->nParam1)
            {
                case MMC_CAMERA_TIMER_ID:
                    MCI_TRACE(MCI_VIDREC_TRC, 0,"[MMC_VIDREC]Camera_UserMsg  EV_TIMER");
                    if(bset<2)//bset==2 means the camera is power off
                        Lily_Camera_SetEffect(CAM_PARAM_WB,(uint32)g_VidRecCntx.settingInfo.whiteBalance);
                    break;
                default:
                    MCI_TRACE(MCI_VIDREC_TRC, 0,"[MMC_VIDREC]Invalid timer ID in camera");
                    break;
            }
            break;
        default:
            break;
    }

}

/**************************************************************
        image from sensor is 160*120, the selected view&record range is
        92*120, the followint rotate function will clockwise rotate the
        selected image for LCD size 320*240,dedicated for project N9580
        created by liyongjian  08162009
***************************************************************/
void yuv422_rotate(uint16 * sourcebuf,uint16 * rotatedbuf)
{
    uint8* psrc;
    uint8* pdes;
    uint8 *psrcline;
    uint8 *pdesrow;
    int i,j;

    psrc = (uint8*)((uint32)sourcebuf&0xdfffffff) + (34*2);
    pdes = (uint8*)(rotatedbuf + 160*105+20);

    for(i=0; i<60; i++)
    {
        psrcline = psrc + 160*2*i*2;
        pdesrow= pdes+2*i*2;
        for(j=0; j<46; j++)
        {
            *pdesrow = *psrcline;                                           //y
            *(pdesrow-160*2+1) = *(pdesrow+1) = *(psrcline+1);              //u
            *(pdesrow-160*2) = *(psrcline+2);                               //y
            *(pdesrow+3) = *(pdesrow-160*2+3) = *(psrcline+3);              //v
            *(pdesrow+2) = *(psrcline+160*2);                               //y
//              *(pdesrow-160*2+1) = *(psrcline+1);
            *(pdesrow-160*2+2) = *(psrcline+160*2+2);                       //y
//              *(pdesrow+3) = *(psrcline+3);

            psrcline +=2*2;
            pdesrow -= 160*2*2;
        }
    }

}

void VidRec_PreCap(void)
{

    //MCI_TRACE (MCI_VIDREC_TRC,0,"[MMC_VIDREC]VidRec_PreCap TIME: %d,Cam_Data_Valid=%d",hal_TimGetUpTime(),Cam_Data_Valid);
    MCI_TRACE (MCI_VIDREC_TRC,0,"[MMC_VIDREC]VidRec_PreCap!");

#if (CSW_EXTENDED_API_LCD == 0)
    uint32  local;
#endif
    uint32 count=0;

    // if video recorder is stopped, as follow not exec.
    if (videoRecordState == VidrecStopStatus) return;

    if( Vidrec_Sensor_Seq==0)
    {
        sensor_data_buf=sensorbuf;
    }
    else
    {
        sensor_data_buf=sensorbuf1;
    }


    count = (CameraInfo.start_y)*MMC_LcdWidth;

    if(g_VidRecCntx.settingInfo.lcdrotate==0)
    {
        cutwidth  = (srcwidth*10/(10+(CameraInfo.previewZoom<<1)))&0xfffffffc;
        cutheight = (srcheight*10/(10+(CameraInfo.previewZoom<<1)))&0xfffffffe;
    }
    else
    {
        //MCI_TRACE (MCI_VIDREC_TRC,0,"----VidRec_rotate time begin: %d---",hal_TimGetUpTime());
        yuv422_rotate((uint16*)sensor_data_buf,sensorbuf2);
        sensor_data_buf = sensorbuf2;
        cutwidth  = (120*10/(10+(CameraInfo.previewZoom<<1)))&0xfffffffc;
        cutheight = (90*10/(10+(CameraInfo.previewZoom<<1)))&0xfffffffe;
        //MCI_TRACE (MCI_VIDREC_TRC,0,"----VidRec_rotate time end: %d---%d,%d,%d",hal_TimGetUpTime(),MMC_LcdWidth,MMC_LcdHeight,cutheight);
        CameraInfo.start_x = 0;
        CameraInfo.start_y = 15;
    }

    if ((videoRecordState == VidrecRecordStatus) && (vidrec_VocFirstSchedule > 0) )
    {
        if(Cam_Data_Valid==0)
        {
            if (Voc_AmrEnc_Busy==0 && Voc_JpegEnc_Busy ==0)
            {
                Voc_JpegEnc_Busy=1;
                Voc_Interrupt_From=0;
                Voc_Jpg_Need=0;

                VidRec_JPEGEncode((int32 *)(sensor_data_buf+srcwidth*((srcheight-cutheight)>>1)+((srcwidth-cutwidth)>>1)),CameraInfo.image_width,CameraInfo.image_height);

            }
            else
            {
                MCI_TRACE (MCI_VIDREC_TRC,0,"[MMC_VIDREC_WARN]1 sensor image waiting,Voc_JpegEnc_Busy=%d,Voc_AmrEnc_Busy=%d",Voc_JpegEnc_Busy,Voc_AmrEnc_Busy);
                Voc_Jpg_Need=1;
            }
        }

    }
    //MCI_TRACE (MCI_VIDREC_TRC,0,"VidRec_preview begin: %d",hal_TimGetUpTime());

    if((Cam_Data_Valid==0)&&(Effect_Set_Valid==0))
    {
        LCDD_ERR_T err;
        //zoom and yuv2rgg565 for preview display
        VidRec_bilinear_zoom((uint8*)((uint32)sensor_data_buf&0xdfffffff),(int)srcwidth,(int)srcheight,(int)cutwidth,(int)cutheight,(uint8*)gp_VidRec_Prebuf,(int)CameraInfo.preview_width,(int)CameraInfo.preview_height,linebuff_asm);
#if (CSW_EXTENDED_API_LCD == 1)
        if(g_VidRecCntx.settingInfo.lcdrotate==0)
        {

            //display video recorder's info bar
            displayFbw.fb.buffer = (UINT16*) backbuffer;
            displayFbw.fb.width = MMC_LcdWidth;
            displayFbw.fb.height = MMC_LcdHeight;
            displayFbw.fb.colorFormat = LCDD_COLOR_FORMAT_RGB_565;
            displayFbw.roi.width = MMC_LcdWidth;
            displayFbw.roi.height = MMC_LcdHeight-(CameraInfo.preview_height+CameraInfo.start_y);
            displayFbw.roi.x = 0;
            displayFbw.roi.y = CameraInfo.start_y+CameraInfo.preview_height;

            err = lcdd_Blit16(&displayFbw,0,CameraInfo.start_y+CameraInfo.preview_height);
            while (LCDD_ERR_NO != err)
            {
                sxr_Sleep(10);
                err = lcdd_Blit16(&displayFbw,0 ,CameraInfo.start_y+CameraInfo.preview_height);
            }
            VidRec_yuv2rgb_blend((int8*)gp_VidRec_Prebuf,(int)CameraInfo.preview_width,(int)CameraInfo.preview_height,(int8*)(blendBuffer+count),(int8*)(backbuffer+count),(MMC_LcdWidth-(int)CameraInfo.preview_width)*2);
            displayFbw.fb.buffer = (UINT16*) blendBuffer;
            displayFbw.fb.width = MMC_LcdWidth;
            displayFbw.fb.height = MMC_LcdHeight;
            displayFbw.fb.colorFormat = LCDD_COLOR_FORMAT_RGB_565;
            displayFbw.roi.width = CameraInfo.preview_width;
            displayFbw.roi.height = CameraInfo.preview_height;
            displayFbw.roi.x = 0;
            displayFbw.roi.y = CameraInfo.start_y;

            // Display the blended image until lcdd is available
            err = lcdd_Blit16(&displayFbw,0,CameraInfo.start_y);
            while (LCDD_ERR_NO != err)
            {
                sxr_Sleep(10);
                err = lcdd_Blit16(&displayFbw,0 ,CameraInfo.start_y);
            }
        }
        else
        {
            VidRec_yuv2rgb((int8*)gp_VidRec_Prebuf,(int)CameraInfo.preview_width,(int)CameraInfo.preview_height,(int8*)(backbuffer+count),(MMC_LcdWidth-(int)CameraInfo.preview_width)*2);
            displayFbw.fb.buffer = (UINT16*) backbuffer;
            displayFbw.fb.width = MMC_LcdWidth;
            displayFbw.fb.height = MMC_LcdHeight;
            displayFbw.fb.colorFormat = LCDD_COLOR_FORMAT_RGB_565;
            displayFbw.roi.width = MMC_LcdWidth;
            displayFbw.roi.height = MMC_LcdHeight;
            displayFbw.roi.x = 0;
            displayFbw.roi.y = 0;

            // Display the blended image until lcdd is available
            err = lcdd_Blit16(&displayFbw,0,0);
            while (LCDD_ERR_NO != err)
            {
                sxr_Sleep(10);
                err = lcdd_Blit16(&displayFbw,0 ,0);
            }
        }

#else
        diag_printf ("[MMC_VIDREC]need some new LCD display codes!\n");
        local=get_lcd_frame_buffer_address();
        set_lcd_frame_buffer_address((kal_uint32)blendBuffer);
        previewstatus = 0;
        MainLCD->BlockWrite(0,0,MMC_LcdWidth-1,MMC_LcdHeight-1);
        previewstatus = 1;
        set_lcd_frame_buffer_address(local);
#endif
    }

    if ((videoRecordState == VidrecRecordStatus) && (vidrec_VocFirstSchedule > 0) )
    {
        mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VIDREC_AVI_OUTPUT_REQ);
    }

    Effect_Set_Valid=0;
    Msg_Wait_Num--;

    //MCI_TRACE (MCI_VIDREC_TRC,0,"VidRec_PreCap end: %d",hal_TimGetUpTime());
}



void    VidRec_FrameOutput(void)
{
    error_type avi_inte_result=NO_ERRO;
    do
    {
        MCI_TRACE (MCI_VIDREC_TRC,0,"[MMC_VIDREC]AMR_Data_Store:%d",AMR_Data_Store);
        if(Avi_Out_Index>0)
        {
            if(NO_ERRO!=(avi_inte_result=vidrec_EncodeRun(&g_VidRecCntx)))
            {
                MCI_TRACE (MCI_VIDREC_TRC|TSTDOUT,0,"[MMC_VIDREC_ERROR]vidrec_EncodeRun Error:%d",avi_inte_result);
                g_VidRecCntx.vidrec_callback((uint16)avi_inte_result);
                return;
            }
        }
        else
        {
            MCI_TRACE (MCI_VIDREC_TRC|TSTDOUT,0,"[MMC_VIDREC]THE FISRST FRAME IS BAD!");
            break;
        }
    }
    while(AMR_Data_Store>=10&&Voc_JpegEnc_Busy==0);
}



void VidRec_JPEGEncode(int32* buf_in,short width,short height)
{
    vpp_AudioJpeg_ENC_IN_T VocMP4EncIn;

    VocMP4EncIn.SampleRate=(cutheight<<16)|cutwidth;
    VocMP4EncIn.mode        = MMC_MJPEG_ENCODE;  // update ram according to global  mode
    VocMP4EncIn.imag_width  = width;//VIDEO_RECORD_DISP_WIDTH;
    VocMP4EncIn.imag_height = height;//VIDEO_RECORD_DISP_HEIGHT;
    g_VidRecCntx.settingInfo.encodeQuality=1;

    if (g_VidRecCntx.settingInfo.encodeQuality == VID_REC_QTY_LOW)
    {
        VocMP4EncIn.imag_quality = 0;
    }
    else
    {
        VocMP4EncIn.imag_quality = 1;
    }
    g_VidRecCntx.VocState = VIDREC_JPEG_ENC_BUSY;




    VocMP4EncIn.inStreamBufAddr  = (INT32*)buf_in;//gp_VideoRecordAFrameRGBBuf;

    VocMP4EncIn.outStreamBufAddr = (INT32*)(vidrec_pJpegVocOut+ 608);


    VocMP4EncIn.channel_num = 1;
    if (VR_Init==0)
    {
        VR_Init=1;
        VocMP4EncIn.reset=1;
    }
    if(HAL_ERR_NO!=vpp_AudioJpegEncScheduleOneFrame(&VocMP4EncIn))
    {
        Voc_JpegEnc_Busy=0;
        diag_printf ("[MMC_VIDREC_ERROR]Failed to call VOC in VidRec_JPEGEncode");
    }

    VPP_WRITE_JPEGHeadr(VocMP4EncIn.imag_quality, width,height,(uint8*)vidrec_pJpegVocOut);
    MCI_TRACE(MCI_CAMERA_TRC, 0, "[MMC_videorecord]JPEGENCODER1: %d",hal_TimGetUpTime());
}



int32 vidrec_VideoSampleStart(MMC_VDOREC_SETTING_STRUCT* previewPara)
{
    CAM_PREVIEW_STRUCT cam_para_temp;

//  MCI_TRACE (MCI_VIDREC_TRC,0,"previewStartX:%d--previewStartY:%d",previewPara->previewStartX,previewPara->previewStartY);
//  MCI_TRACE (MCI_VIDREC_TRC,0,"previewWidth:%d--previewHeight:%d",previewPara->previewWidth,previewPara->previewHeight);
//  MCI_TRACE (MCI_VIDREC_TRC,0,"imageWidth:%d--imageHeight:%d",previewPara->imageWidth,previewPara->imageHeight);
//  MCI_TRACE (MCI_VIDREC_TRC,0,"nightMode:%d--encodeQuality:%d",previewPara->nightMode,previewPara->encodeQuality);
//  MCI_TRACE (MCI_VIDREC_TRC,0,"zoomFactor:%d--specialEffect:%d",previewPara->zoomFactor,previewPara->specialEffect);
//  MCI_TRACE (MCI_VIDREC_TRC,0,"whiteBalance:%d--contrast:%d--brightnessLevel:%d",previewPara->whiteBalance,previewPara->contrast,previewPara->brightnessLevel);


    cam_para_temp.image_height=previewPara->imageHeight;
    cam_para_temp.image_width=previewPara->imageWidth;
    cam_para_temp.start_x=previewPara->previewStartX;
    cam_para_temp.start_y=previewPara->previewStartY;
    cam_para_temp.end_x=previewPara->previewWidth + previewPara->previewStartX-1;
    cam_para_temp.end_y=previewPara->previewHeight + previewPara->previewStartY-1;
    cam_para_temp.nightmode=previewPara->nightMode;
    cam_para_temp.imageQuality=previewPara->encodeQuality;
    cam_para_temp.factor=previewPara->zoomFactor;
    cam_para_temp.specialEffect=previewPara->specialEffect;
    cam_para_temp.whiteBlance=previewPara->whiteBalance;
    cam_para_temp.contrast = previewPara->contrast;
    cam_para_temp.brightNess = previewPara->brightnessLevel;


    set_mmc_camera_preview();
    MCI_CamPreviewOpen(&cam_para_temp);

    return 0;
}

void VidRec_FS_AsynWrite_CB(void* pParam)
{
    FS_ASYN_READ_RESULT  *sRsp ;

    MCI_TRACE(MCI_CAMERA_TRC, 0, "[MMC_videorecord]write file end1: %d",hal_TimGetUpTime());
    sRsp = (FS_ASYN_READ_RESULT*)pParam;
    if(sRsp->iResult >= 0)
    {
        vidrec_AviNuminBuff--;
    }
    else //fs write file
    {
        vidrec_AviNuminBuff = 0;

        diag_printf("[MMC_VIDREC_ERROR]FS write fail!");
        VidRec_RecordStop();
        g_VidRecCntx.vidrec_callback(MDI_RES_VDOREC_ERR_FAILED);
    }
    return;
}

#endif // (CHIP_HAS_GOUDA != 1)

