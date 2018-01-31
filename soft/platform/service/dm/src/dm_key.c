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

#include <cswtype.h>
#include <base.h>
#include "chip_id.h"

#include "hal_key.h"
#include "tgt_m.h"
#include "hal_pwm.h"

#include "csw_csp.h"

#include <cos.h>
#include "key_defs.h"
#include <stdkey.h>
#include <base_prv.h>
#include <sul.h>
#include "dm_prv.h"
#include "ts.h"
#ifdef _USED_TSC_KEY_
#include "tsd_m.h"
#endif
#include "csw_mem_prv.h"

#define BUTTON(row,col) (((row)<<8) | (col))

extern UINT16 g_PreKeyCode;
extern UINT16 g_PreKeyPressTime;

typedef struct _KEY_SCAN_STATUS
{
    UINT32     LastButton;
    UINT32     PendingButton;
    UINT8      nSame;          // current number of identical scans
    UINT8      padding;
    UINT16      nPressTime;

    // Debouncing parameters
    UINT32     nStable;        // Number of stable scans before returning
} KEY_SCAN_STATUS;

#ifdef __INFONES_SUPPORT__
/* Pad state */
extern DWORD dwPad1;
extern DWORD dwPad2;
extern DWORD dwSystem;
extern UINT32 InfoNES_LoadFrame_Ex;
void poll_event(U8 key,hal_KeyState_t status)
{
    switch(status)
    {
        case HAL_KEY_DOWN: //key down
            switch(key)
            {
                case KP_RT:   dwPad1 |= (1<<7); break;
                case KP_LT:    dwPad1 |= (1<<6); break;
                case KP_DW:    dwPad1 |= (1<<5); break;
                case KP_UP:      dwPad1 |= (1<<4); break;
                case KP_OK:    dwPad1 |= (1<<3); break;   /* Start */
                case KP_SL:    dwPad1 |= (1<<2); break;   /* Select */
                case KP_7:     dwPad1 |= (1<<1); break;   /* 'B' */
                case KP_STAR:  dwPad1 |= (1<<0); break;   /* 'A' */
                // case KP_POUND: InfoNES_LoadFrame_Ex = !InfoNES_LoadFrame_Ex;
                case KP_SR: InfoNES_Quit(); break; /* exit */
                case KP_END: InfoNES_Quit(); break; /* exit */
            } /* keydown */
            break;

        case HAL_KEY_UP: //key up
            switch(key)
            {
                case KP_RT:   dwPad1 &=~(1<<7); break; //r
                case KP_LT:    dwPad1 &=~(1<<6); break; //l
                case KP_DW:    dwPad1 &=~(1<<5); break; //down
                case KP_UP:    dwPad1 &=~(1<<4); break; //up
                case KP_OK:    dwPad1 &=~(1<<3); break;  /* Start */
                case KP_SL:    dwPad1 &=~(1<<2); break;  /* Select */
                case KP_7:     dwPad1 &=~(1<<1); break;  /* 'B' */
                case KP_STAR:  dwPad1 &=~(1<<0); break;  /* 'A' */
            } /* keyup */
            break;
    }
}
#endif //__INFONES_SUPPORT__

void DM_KeyHandler(UINT8 button, hal_KeyState_t status)
{
    UINT8 row = (button == HAL_KEY_ON_OFF) ? HAL_KEY_ON_OFF : button / 8;
    UINT8 col = (button == HAL_KEY_ON_OFF) ? HAL_KEY_ON_OFF : button % 8;
    UINT8 key = tgt_GetKeyCode(button);

    COS_EVENT ev;
#ifdef __INFONES_SUPPORT__
    if(((row == 255) && (col == 255)) || (button == HAL_KEY_ON_OFF))
        poll_event(KP_END,status);
    else
        poll_event(key,status);
#endif //__INFONES_SUPPORT__
    ev.nParam1 = DM_GetStdKey(key);  // SK_BUTTON
    ev.nParam2 = ((row << 8) | col); // (row and column)
    ev.nParam3 = key;       // Key value from target
    //hal_HstSendEvent(0x00000000);
    //hal_HstSendEvent(row);
    //hal_HstSendEvent(col);
    //hal_HstSendEvent(0x00000001);

    if (((row == 255) && (col == 255)) || (button == HAL_KEY_ON_OFF))
    {
        if(status == HAL_KEY_DOWN )
        {
            ev.nEventId = EV_KEY_DOWN;
            ev.nParam1    = SK_POWER_OFF;
            DM_SendKeyMessage(&ev);
        }
        else if(status == HAL_KEY_UP)
        {
            ev.nEventId = EV_KEY_UP;
            ev.nParam1    = SK_POWER_OFF;
            DM_SendKeyMessage(&ev);
        }
    }
    else
    {
        if (status == HAL_KEY_DOWN)
        {
            ev.nEventId = EV_KEY_DOWN;
            DM_SendKeyMessage(&ev);
            CSW_TRACE(_CSW|TLEVEL(1)|TDB|TNB_ARG(3), TSTR("dm: DM_KeyHandler ev.nParam1 =%x  2 = %x 3= %c X \n",0x0820000d),
                      ev.nParam1, ev.nParam2, ev.nParam3);
        }
        else if (status == HAL_KEY_UP)
        {
            ev.nEventId = EV_KEY_UP;
            DM_SendKeyMessage(&ev);
            CSW_TRACE(_CSW|TLEVEL(1)|TDB|TNB_ARG(3), TSTR("dm; DM_KeyHandler up dmkey = %x rowcol = %x tgtkey = %c\n",0x0820000e),
                      ev.nParam1, ev.nParam2, ev.nParam3);

        }
    }



}

#ifdef _USED_TSC_KEY_
VOID DM_tsc_KeyHandler(UINT8 button, TSD_PEN_STATE_T status)
{
    COS_EVENT ev;
    UINT8 key = tgt_GetKeyCode(button);

    ev.nParam1 = 0xff;  // SK_BUTTON
    ev.nParam2 = 0xff;   // (row and column)
    ev.nParam3 = key;       // Key value from target
    //hal_HstSendEvent(0x00000000);
    //hal_HstSendEvent(row);
    //hal_HstSendEvent(col);
    //hal_HstSendEvent(0x00000001);

    if (status == TSD_PEN_DOWN)
    {
        ev.nEventId = EV_KEY_DOWN;
        DM_SendKeyMessage(&ev);
        CSW_TRACE(_CSW|TLEVEL(1)|TDB|TNB_ARG(3), TSTR("dm: DM_KeyHandler ev.nParam1 =%x  2 = %x 3= %c X \n",0x0820000f),
                  ev.nParam1, ev.nParam2, ev.nParam3);
    }
    else if (status == TSD_PEN_UP)
    {
        ev.nEventId = EV_KEY_UP;
        DM_SendKeyMessage(&ev);
        CSW_TRACE(_CSW|TLEVEL(1)|TDB|TNB_ARG(3), TSTR("dm; DM_KeyHandler up dmkey = %x rowcol = %x tgtkey = %c\n",0x08200010),
                  ev.nParam1, ev.nParam2, ev.nParam3);
    }
}
#endif

BOOL DM_KeyInit()
{
#ifdef _USED_TSC_KEY_
    tsd_KeyOpen(&DM_tsc_KeyHandler);
#endif

    hal_KeyMask_t keyMask;

    hal_key_Open(4, 4, 50);
    hal_key_IrqSetHandler(DM_KeyHandler);
    keyMask.onUp = 1;
    keyMask.onPressed = 0;
    keyMask.onDown = 1;
    hal_key_IrqSetMask(keyMask); // Modem2G's API use the structure directly

    return TRUE;
}

BOOL DM_KeyExit()
{
    hal_key_Close();
    return TRUE;
}


UINT8 DM_GetStdKey(UINT8 key)
{

    switch (key)
    {
        case KP_VU:
            return (SK_VOLUME_INC);     // Volume UP

        case KP_VD:
            return (SK_VOLUME_DEC);     // Volume Dwn

        case KP_UP:
            return (SK_UP);             // UP

        case KP_DW:
            return (SK_DOWN);             // DOWN

        case KP_RT:
            return (SK_RIGHT);         // RIGHT

        case KP_LT:
            return (SK_LEFT);             // LEFT

        case KP_1:
            return (SK_NUM1);          // 0x31+0x20

        case KP_2:
            return (SK_NUM2);             // 0x32+0x20

        case KP_3:
            return (SK_NUM3);             // 0x33+0x20

        case KP_4:
            return (SK_NUM4);             // 0x34+0x20

        case KP_5:
            return (SK_NUM5);             // 0x35+0x20

        case KP_6:
            return (SK_NUM6);             // 0x36+0x20

        case KP_7:
            return (SK_NUM7);             // 0x37+0x20

        case KP_8:
            return (SK_NUM8);             // 0x38+0x20

        case KP_9:
            return (SK_NUM9);             // 0x39+0x20

        case KP_STAR:
            return (SK_STAR);             // 0x3A+0x20

        case KP_0:
            return (SK_NUM0);             // 0x30+0x20

        case KP_POUND:
            return (SK_HASH);             // 0x3B+0x20

        case KP_SR:
            return (SK_CTL_RIGHT);     //    SOFT RIGHT

        case KP_SL:
            return (SK_CTL_LEFT);         // SOFT_LEFT

        case KP_END:
            return (SK_PHONE_END);     // Phone End

        case KP_SND:
            return (SK_PHONE_SEND);     // Phone Send

        case KP_OK:
            return (SK_SELECT);         // 0x01

        case KP_DEL:
            return (SK_DELETE);         // C

        case KP_BACK:
            return (SK_BACKSPACE);     // Backspace

        case KP_SC:
            return (SK_MENU);             // 0x04

        default:
            break;
    }

    return (SK_NONE);
}




