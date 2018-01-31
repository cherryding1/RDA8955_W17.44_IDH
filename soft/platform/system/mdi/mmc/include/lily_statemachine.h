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





#ifndef _LILY_STATEMACHINE_H
#define _LILY_STATEMACHINE_H
#include "cs_types.h"

#define STM_USE_BIT

//#define ENABLE_LILY_SLEEP

#ifdef STM_USE_BIT


typedef enum
{
    STA_MAC_MOD_IDLE,
    STA_MAC_MOD_SLEEP,
    STA_MAC_MOD_AUD_PLAY,
    STA_MAC_MOD_AUD_PAUSE,
    STA_MAC_MOD_AUDREC,
    STA_MAC_MOD_IMG,
    STA_MAC_MOD_CAM,
    STA_MAC_MOD_FS,
    STA_MAC_MOD_VID,
    STA_MAC_MOD_VIDREC,
    STA_MAC_MOD_USB,
    STA_MAC_MOD_FMREC,
    STA_MAC_MOD_FM,
    STA_MAC_MOD_NONE,
} STA_MAC_MODULE;

typedef enum
{
    STA_RES_OK,
    STA_RES_FAIL
} STA_RESULT;

#define STATE_BIT1 (1L)



#define STM_MOD_IDLE (STATE_BIT1)
#define STM_MOD_SLEEP (STATE_BIT1<<(STA_MAC_MOD_SLEEP))
#define STM_MOD_AUD_PLAY (STATE_BIT1<<(STA_MAC_MOD_AUD_PLAY))
#define STM_MOD_AUD_PAUSE (STATE_BIT1<<(STA_MAC_MOD_AUD_PAUSE))
#define STM_MOD_AUDREC (STATE_BIT1<<(STA_MAC_MOD_AUDREC))
#define STM_MOD_IMG (STATE_BIT1<<(STA_MAC_MOD_IMG))
#define STM_MOD_CAM (STATE_BIT1<<(STA_MAC_MOD_CAM))
#define STM_MOD_FS (STATE_BIT1<<(STA_MAC_MOD_FS))
#define STM_MOD_VID (STATE_BIT1<<(STA_MAC_MOD_VID))
#define STM_MOD_VIRECD (STATE_BIT1<<(STA_MAC_MOD_VIDREC))
#define STM_MOD_USB (STATE_BIT1<<(STA_MAC_MOD_USB))
#define STM_MOD_FMREC (STATE_BIT1<<(STA_MAC_MOD_FMREC))
#define STM_MOD_FM (STATE_BIT1<<(STA_MAC_MOD_FM))









#else
#define STATE_MACHINE_IDLE_BASE (0x0)
#define STATE_MACHINE_MEDIA_BASE (STATE_MACHINE_IDLE_BASE+10)
#define STATE_MACHINE_USB_BASE (STATE_MACHINE_MEDIA_BASE+200)


typedef enum
{
    STA_MAC_MOD_IDLE,
    STA_MAC_MOD_SLEEP,
    STA_MAC_MOD_AUD,
    STA_MAC_MOD_AUDREC,
    STA_MAC_MOD_IMG,
    STA_MAC_MOD_CAM,
    STA_MAC_MOD_FS,
    STA_MAC_MOD_VID,
    STA_MAC_MOD_VIDREC,
    STA_MAC_MOD_USB,
    STA_MAC_MOD_NONE,
} STA_MAC_MODULE;




































typedef enum
{
    STA_RES_OK,
    STA_RES_FAIL
} STA_RESULT;





typedef enum
{
    STA_MAC_IDLE = STATE_MACHINE_IDLE_BASE,
    STA_MAC_NO_CARD,
    STA_MAC_SLEEP,

    /*********all  is media State,start****/
    /*audio module states*/
    STA_MAC_MEDIA_AUD_PLAY ,//= STATE_MACHINE_MEDIA_BASE,
    STA_MAC_MEDIA_AUD_PAUSE,
    STA_MAC_MEDIA_AUD_RESUME,
    STA_MAC_MEDIA_AUD_STOP,
    /*********all  is media State,end****/

    /********image start    ***/
    STA_MAC_MEDIA_IMG_DECODE,
    STA_MAC_MEDIA_IMG_STOP,
    /********image end  ***/
    /********camera start   ***/
    STA_MAC_MEDIA_CAM_POWERON,
    STA_MAC_MEDIA_CAM_PREVIEWON,
    STA_MAC_MEDIA_CAM_PREVIEWOFF,
    STA_MAC_MEDIA_CAM_CAPTURE,
    STA_MAC_MEDIA_CAM_POWEROFF,
    /********camera end ***/
    STA_MAC_USB,// = STATE_MACHINE_USB_BASE,
    STA_MAC_MAX
} STA_MAC_STATES;

typedef struct
{
    uint16 permissionNum;
    const uint16 *permission;
    uint16 forbiddanceNum;
    const uint16 *forbiddance;
} STA_MAC_MANAGE_LIST;

typedef struct
{
    uint16 parallelState1;
    uint16 parallelState2;
    uint16 parallelState3;
} STA_MAC_STATES_RECORD;













//function declaration
STA_MAC_STATES lily_stmTransferTmpstate(uint16 *state);
void lily_stateMachineInit(void);
boolean lily_getModuleState(uint16 module);

#endif


STA_RESULT lily_changeToState(uint16 module);
STA_RESULT lily_exitModule(uint16 module);



#endif //_LILY_STATEMACHINE_H
