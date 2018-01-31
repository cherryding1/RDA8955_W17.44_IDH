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

#include "reftest_m.h"

#include "memd_m.h"
#include "lcdd_m.h"
#include "fmg_m.h"

#include "tgt_memd_cfg.h"

#define NCHAN_16                1
#define IMAGE_WIDTH             128
#define IMAGE_HEIGHT            34

UINT16 g_logoRgb[(IMAGE_WIDTH*IMAGE_HEIGHT)*NCHAN_16] =
{
#include "logo_128x60_16.dat"
};




//--------- initialises LCD + keypad for reference tests
PUBLIC VOID reftest_Init(VOID)
{
    HAL_KEY_IRQ_MASK_T keyMask;
    LCDD_SCREEN_INFO_T screenInfo;
    LCDD_ROI_T roi;
    LCDD_FBW_T frameBufferWin;

    // Setup the display
    lcdd_Open();
    lcdd_SetStandbyMode(FALSE);
    lcdd_GetScreenInfo(&screenInfo);

    roi.x=0;
    roi.y=0;
    roi.width=screenInfo.width;
    roi.height=screenInfo.height;

    while (LCDD_ERR_NO != lcdd_FillRect16(&roi, 0xffff));

    // Draw a rda logo
    frameBufferWin.fb.buffer = (UINT16*)  g_logoRgb;
    frameBufferWin.fb.width =  IMAGE_WIDTH;
    frameBufferWin.fb.height = IMAGE_HEIGHT;
    frameBufferWin.fb.colorFormat = LCDD_COLOR_FORMAT_RGB_565;

    frameBufferWin.roi.x=0;
    frameBufferWin.roi.y=0;
    frameBufferWin.roi.width= IMAGE_WIDTH;
    frameBufferWin.roi.height= IMAGE_HEIGHT;

    while (LCDD_ERR_NO != lcdd_Blit16(&frameBufferWin,0,0));

    lcdd_SetBrightness(6);

    fmg_PrintfInit(0, 8, screenInfo.width, screenInfo.height);

    // Setup the keypad
    hal_KeyOpen(240, 16); //8); // dbn = 15ms, itl = 120 ms
    hal_KeyIrqSetHandler(reftest_KeyCallback);
    keyMask.onUp = 1;
    keyMask.onPressed = 1;
    keyMask.onDown = 1;
    hal_KeyResetIrq();
    hal_KeyIrqSetMask(keyMask);
}

// This function waits until the OK key is pressed
PUBLIC VOID reftest_WaitAnyKey()
{
    BOOL testStarted = 0;
    UINT32     evt[4];
    Msg_t	*msg;

    fmg_PrintfSetXY(2,8);
    fmg_Printf("Press any key\n");

    while (testStarted==0)
    {
        msg = sxr_Wait(evt, testMbx);
        if (msg->H.Id == MMI_KEY_DOWN)
        {
            testStarted = 1;
            sxs_fprintf(TSTDOUT,"starting test");
            fmg_PrintfSetXY(2,8);
            fmg_Printf("Starting test\n");
        }
        sxr_Free(msg);
    }

}



