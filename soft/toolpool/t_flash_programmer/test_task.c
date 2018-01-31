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

#include "reftest_m.h"
#include "fmg_m.h"

#include "sxr_ops.h"
#include "sxs_io.h"
#include "key_defs.h"


#include "hal_dma.h"
#include "hal_sys.h"
#include "hal_aif.h"

#include "hal_timers.h"

#include "stdlib.h"
#include "stdio.h"

#include "pmd_m.h"
#include "fs.h"
#include "tupgrade.h"
#include "sxr_usrmem.h"

UINT8 testTaskId;
UINT8 testMbx;

extern VOID fmg_Fillrect(UINT16 startX, UINT16 startY, UINT16 endX, UINT16 endY,UINT32 color);
extern char* strcpy(char* to, const char* from);
extern char* strcat(char *s,const char *append);
extern void hal_SysSoftReset();

/* ----------------- This function generates a message for the test task
						when a key event is detected */
void reftest_KeyCallback(UINT8 id,  HAL_KEY_STATE_T status)
{
    Msg_t * MsgPtr ;
    UINT16     Len    = sizeof( Msg_t );

    MsgPtr = (Msg_t*) sxr_Malloc(Len);
    MsgPtr->H.Id = MMI_KEYPRESSED + status;
    MsgPtr->B = tgt_GetKeyCode(id);
    sxr_Send (MsgPtr,testMbx, SXR_SEND_MSG);

    if (MsgPtr->H.Id == MMI_KEY_UP) {
        hal_PwlGlow(0, 0, 0, 0);
    } else {
        hal_PwlGlow(255, 0, 0, 0);
    }
}

#define COLOR_BLACK     0x0000
#define COLOR_WHITE     0xFFFF
#define COLOR_RED       0xF800
#define COLOR_GREEN     0x05E0
#define COLOR_BLUE      0x001F

#if 1


void t_update_showPrograss(UINT32 progress)
{
    //UINT32 startx = g_fmgPrintfState.curX,starty=g_fmgPrintfState.curY,endx=g_fmgPrintfState.curX+100,endy=g_fmgPrintfState.curY+10;
    UINT32 startx = 10;
    UINT32 starty=100;
    UINT32 endx;
    UINT32 endy;
    //UINT32 cnt = 0;


    endx=startx+100;
    endy=starty+20;

    if(progress == 0)
    {
        fmg_Fillrect(startx,starty,endx,endy,COLOR_RED);
    }
    else
    {
        fmg_Fillrect(startx,starty,startx+progress,endy,COLOR_GREEN);
    }
    if(progress == 100)
    {
        sxs_fprintf(TSTDOUT,"Finish!!!");

    }

}
#endif
/* Test task description */

void testTaskInit()
{
    UINT32        evt;
    Msg_t	        *msg = NULL;
    UINT8          button;
    UINT32        select_num = 0;
    INT32          err_code = 0;

    reftest_Init();
    sxs_fprintf(TSTDOUT,"reftest test task started");

    // We don't want to be bothered by the watchdog in
    // this test.
    hal_TimWatchDogClose();

    //init file system
    err_code = T_UP_FsInit();
    fmg_PrintfClrScr();
    //fmg_Printf("\n");
    err_code = T_UP_Main(t_update_showPrograss);
    // fmg_Printf("Press key 0 to list update files.\n");
    hal_SysSoftReset();

    while (1)
    {
        msg = sxr_Wait(&evt, testMbx);


        if(msg != NULL)
        {

            if (msg->H.Id == MMI_KEY_DOWN)
            {
                button = msg->B;
                if (button != KP_NB)
                {
                    //sxs_fprintf(TSTDOUT,"button %c \n",button);
                    //fmg_PrintfSetXY(2,10);
                    //fmg_Printf("Button %c\n ",button);
                }
                if(err_code == 0)
                {
                    select_num = 0;
                    switch (button)
                    {
                    case KP_0:
                        break;

                    case KP_1:
                        select_num = 1;
                        break;

                    case KP_2:
                        select_num = 2;
                        break;

                    case KP_3:
                        select_num = 3;
                        break;

                    case KP_4:
                        select_num = 3;
                        break;
                    case KP_5:
                        select_num = 3;
                        break;
                    case KP_6:
                        select_num = 6;
                        break;
                    case KP_7:
                        select_num = 7;
                        break;
                    case KP_8:
                        select_num = 8;
                        //  T_UP_Verify();
                        break;
                    case KP_9:
                        select_num = 9;
                        hal_SysSoftReset();
                        break;
                    case KP_SR:

                        break;

                    case KP_SL:

                        fmg_Printf("SeekDw        \n");
                        sxs_fprintf(TSTDOUT, "SeekDw");
                        break;

                    case KP_LT:
                        break;

                    case KP_DW:
                        break;

                    case KP_RT:
                        break;

                    case KP_UP:
                        break;

                    case KP_VU:
                        break;

                    case KP_VD:
                        break;

                    case KP_OK:
                    {

                    }
                    break;

                    default:
                        break;

                    }
                }

            }

            sxr_Free(msg);
        }
        else
        {
            sxs_fprintf(TSTDOUT,"event 0x%x \n",evt);
        }
    }

}



