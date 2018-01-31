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




#include "cs_types.h"
#include "camera_m.h"
#include "cam_motion_detect.h"
#include "app_ltlcom.h"
#include "kal_common_defs.h"
#include "med_utility.h"
#include "pmd_m.h"
#include "hal_timers.h"
#include "mcip_debug.h"
#include "oslmemory.h"
#include "sxr_tls.h"
#include "lcdd_m.h"


#define MAX_BUF_NUM 2
static unsigned short *pImgBuf[MAX_BUF_NUM];
static int imgBufIdx;

static int imgNumGet;

static unsigned int threshold;

#define MAX_ERRSUM_NUM 2

static int imgErrSumIdx;
static int imgErrSum[MAX_ERRSUM_NUM];

void(*motion_detect_callback)(int);

#define IMG_SIZE CAM_NPIX_QQVGA

#define MD_PERIOD 200 //ms

static char mdDelayNum;

static int MD_STATE;//0=close,1=open
static int MD_CAM_STATE;//0=close,1=open

extern UINT8  g_halCameraIfcChan ;

static int getErrSum(unsigned short *pImg1, unsigned short *pImg2, unsigned int pixelNum)
{
    int errSum=0;
    int err=0;
    //short *p1=pImg1;
    //short *p2=pImg2;
    unsigned char *p1=(unsigned char*)pImg1;
    unsigned char *p2=(unsigned char*)pImg2;
    unsigned int avgY;
    unsigned int scale;
    int i;

    //for rgb565-g
#if 0
    i=0;
    while(i<pixelNum)
    {
        err=(int)((((*p1)&0x07E0)-((*p2)&0x07E0))>>5);
        errSum+=((err>0)?(err):(-err));
        p1++;
        p2++;
        i++;
    }
#else//for yuv422-y
    i=0;
    avgY=0;
    while(i<pixelNum)
    {
        avgY+=(unsigned int)(*p1);
        err=(int)(*p1)-(int)(*p2);
        errSum+=((err>0)?(err):(-err));
        p1+=2;
        p2+=2;
        i++;
    }
    avgY=(avgY/pixelNum)-16;
    if(avgY<=0)avgY=1;
    //Y(16-232) 232-16=216
    scale=216/avgY;
    if(scale<1)scale=1;
    errSum=errSum*scale;
#endif
    diag_printf("errsum=%d avgY=%d scale=%d\n", errSum, avgY, scale);
    return errSum;
}

//for test
//static int errtime1, errtime2, timerperiod;//for test
//extern void VidRec_yuv2rgb(char* srcbuf,int src_w,int src_h,char* desbuf,int padding);
//short tmp565[160*120];
//test end


void do_motion_detect(void)
{
    int i;
    int md_work;
    //int preImgBufIdx;
    //preImgBufIdx=imgBufIdx-1;
    //if(preImgBufIdx<0)preImgBufIdx=MAX_BUF_NUM-1;

    //get one frame from camera


    //check buffer available
    if(imgNumGet<2)
    {
        imgNumGet+=1;
        imgBufIdx=1-imgBufIdx;
        camera_PrepareImageCaptureL(CAM_FORMAT_RGB565, IMG_SIZE, pImgBuf[imgBufIdx]);
//                camera_CaptureImage();
        return;
    }

#if 0
    {
        //test display
        LCDD_FBW_T lcdDraw;

        //yuv422->rgb565
        VidRec_yuv2rgb(pImgBuf[imgBufIdx], 160, 120, tmp565,0);

        lcdDraw.fb.colorFormat = LCDD_COLOR_FORMAT_RGB_565;
        lcdDraw.fb.buffer=tmp565;
        lcdDraw.fb.width = 160;
        lcdDraw.fb.height = 120;
        lcdDraw.roi.x= 0;
        lcdDraw.roi.y= 0;
        lcdDraw.roi.width =160;
        lcdDraw.roi.height=120;
        lcdd_Blit16(&lcdDraw, 0, 0);
    }
#endif

//errtime1=hal_TimGetUpTime();

    //do subtract and get err sum
    if(mdDelayNum==0)
        imgErrSum[imgErrSumIdx]=getErrSum(pImgBuf[imgBufIdx], pImgBuf[1-imgBufIdx], IMG_SIZE);
    else
    {
        imgErrSum[imgErrSumIdx]=0;
        mdDelayNum-=1;
    }

//errtime2=hal_TimGetUpTime();

    //check 2 err sum to callback
    md_work=0;
    for(i=0; i<MAX_ERRSUM_NUM; i++)
    {
        if(imgErrSum[i]>threshold)
            md_work+=1;
    }
    if(md_work==MAX_ERRSUM_NUM)
    {
        if(motion_detect_callback)
            motion_detect_callback(1);

        //do another md after some delay time
        mdDelayNum=5;
    }

    imgBufIdx=1-imgBufIdx;

    imgErrSumIdx+=1;
    if(imgErrSumIdx==MAX_ERRSUM_NUM)
        imgErrSumIdx=0;

    camera_PrepareImageCaptureL(CAM_FORMAT_RGB565, IMG_SIZE, pImgBuf[imgBufIdx]);
//    camera_CaptureImage();

}

/*
camera isr.
*/
static void md_CamCallback(unsigned char id, unsigned char ActiveBufNum)
{
    ilm_struct *ilm_ptr = NULL;

    ilm_ptr = allocate_ilm(MOD_MED);
    ilm_ptr->src_mod_id = MOD_MED;
    ilm_ptr->dest_mod_id = MOD_MED;
    ilm_ptr->sap_id = MED_SAP;

    ilm_ptr->msg_id = (msg_type_t)MSG_ID_MEDIA_CAM_MD_REQ;
    ilm_ptr->local_para_ptr =NULL;
    ilm_ptr->peer_buff_ptr = NULL;

    msg_send_ext_queue(ilm_ptr);
}


/*
motion detect open API.
MD_LEVEL sensLevel: motion detect sensitive level.(high/mid/low) default=low
md_callback: call back function pointer. work when motion detect ture.
*/
int motion_detect_open(MD_LEVEL sensLevel, void(*md_callback)(int32))
{
    int res;
    int i;

    UINT16 ispOutWidth = 0, ispOutHeight = 0;

    hal_HstSendEvent(0xffff0010);
    hal_HstSendEvent(hal_TimGetUpTime());
    if (IMG_SIZE == CAM_NPIX_VGA)
    {
        ispOutWidth = 640;
        ispOutHeight = 480;
    }
    else if (IMG_SIZE == CAM_NPIX_QVGA)
    {
        ispOutWidth = 320;
        ispOutHeight = 240;
    }
    else if (IMG_SIZE == CAM_NPIX_QQVGA)
    {
        ispOutWidth = 160;
        ispOutHeight = 120;
    }
    else
    {
        diag_printf("motion_detect_open: Unsupported ISP Out Size");
        res=-1;
        goto MD_OPEN_ERR;
    }

    if(MD_STATE)
    {
        diag_printf("motion_detect_open err MD_STATE=1!");
        return 0;
    }

    switch(sensLevel)
    {
        case SENSE_HIGH:
            threshold=40*IMG_SIZE;
            break;
        case SENSE_MID:
            threshold=55*IMG_SIZE;
            break;
        case SENSE_LOW:
            threshold=140*IMG_SIZE;
            break;
        default://default=low
            threshold=70*IMG_SIZE;
            break;
    }

    if(md_callback)
        motion_detect_callback=md_callback;
    else
    {
        diag_printf("motion_detect_open md_callback ERROR!");
        res=-1;
        goto MD_OPEN_ERR;
    }

    //malloc and initital
    pImgBuf[0]=0;
    pImgBuf[0]=OslMalloc(IMG_SIZE*sizeof(short)*MAX_BUF_NUM);//for rgb565 or yuv422
    pImgBuf[1]=pImgBuf[0]+IMG_SIZE;
    //pImgBuf[2]=pImgBuf[1]+IMG_SIZE;

    diag_printf("pImgBuf[0]=%x sensLevel=%d", pImgBuf[0], sensLevel);

    if(!pImgBuf[1])
    {
        diag_printf("motion_detect_open malloc ERROR!");
        res=-1;
        goto MD_OPEN_ERR;
    }
    imgBufIdx=0;

    imgNumGet=0;

    for(i=0; i<MAX_ERRSUM_NUM; i++)
        imgErrSum[i]=0;
    imgErrSumIdx=0;

    mdDelayNum=0;

    //open camera
    pmd_EnablePower(PMD_POWER_CAMERA, TRUE);

    //  if(g_halCameraIfcChan != 0xff)
    {
        hal_CameraStopXfer(1);
    }

    camera_Reserve(md_CamCallback);

    //camera_SetFrameRate(7);

    camera_PowerOn(IMG_SIZE, ispOutWidth, ispOutHeight, CAM_FORMAT_RGB565);
    MD_CAM_STATE=1;

    hal_TimDelay(25*16);// delay 25ms
    if(camera_GetId()== 0)
    {
        diag_printf("motion_detect_open camera ERROR!");
        motion_detect_close();
        res=-1;
        goto MD_OPEN_ERR;
    }

    camera_SetContrastL(4);
    camera_SetBrightnessL(3);
    camera_SetSpecialEffect(CAM_SPE_NORMAL);
    camera_SetNightMode(0);
    camera_SetWhiteBalanceL(CAM_WB_AUTO);


    //open timer
    //res=COS_SetTimerEX(GetMCITaskHandle(MBOX_ID_MMC), MMC_MD_TIMER_ID, COS_TIMER_MODE_PERIODIC, 150*16384/1000);

    camera_PrepareImageCaptureL(CAM_FORMAT_RGB565, IMG_SIZE, pImgBuf[0]);
    camera_CaptureImage();


    MD_STATE=1;

    diag_printf("motion_detect_open ok!");
    hal_HstSendEvent(0xffff0011);
    hal_HstSendEvent(hal_TimGetUpTime());

    return 0;

MD_OPEN_ERR:

    motion_detect_close();
    diag_printf("motion_detect_open err!res=%d", res);
    hal_HstSendEvent(0xffff0012);
    hal_HstSendEvent(hal_TimGetUpTime());
    hal_HstSendEvent(res);
    return res;
}

/*
motion detect close API.
close camera.
*/
int motion_detect_close()
{
    int i;

    motion_detect_callback=0;
    //close camera
    if(MD_CAM_STATE)
        camera_PowerOff();

    //free memory
    if(pImgBuf[0])
        OslMfree(pImgBuf[0]);
    pImgBuf[0]=0;
    pImgBuf[1]=0;
    //pImgBuf[2]=0;
    imgBufIdx=0;

    imgNumGet=0;

    for(i=0; i<MAX_ERRSUM_NUM; i++)
        imgErrSum[i]=0;
    imgErrSumIdx=0;


    threshold=0;
    mdDelayNum=0;
    MD_STATE=0;
    MD_CAM_STATE=0;

    diag_printf("motion_detect_close ok!");
    return 0;
}


#if 0
void main()
{
    int i;
    int res;
    FILE *pIn;
    pIn=fopen("F:\\源码\\motion_detect\\160x120.yuv", "rb");

    motion_detect_open(SENSE_HIGH, 1);

    res=fread(pImgBuf[imgBufIdx], 1, IMG_SIZE, pIn);
    imgBufIdx+=1;
    fseek(pIn, IMG_SIZE/2, SEEK_CUR);

    i=1;
    while(1)
    {
        res=fread(pImgBuf[imgBufIdx], 1, IMG_SIZE, pIn);
        if(res!=IMG_SIZE)break;

        do_motion_detect();

        fseek(pIn, IMG_SIZE/2, SEEK_CUR);
        printf("frame %d\n",i);
        i++;
    }

    motion_detect_close();
}
#endif

