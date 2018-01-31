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

#ifndef __MMC_VDOCHAT_H__
#define __MMC_VDOCHAT_H__


//declaration for customer
int   mmc_vdochat_powerOn(BOOL bIsRGB);
void mmc_vdochat_powerOff(void);

void mmc_vdochat_previewStart(UINT32 startX,UINT32 startY,UINT32 preW,UINT32 preH);
void mmc_vdochat_previewStop(void);

UINT8* mmc_vdochat_getData(UINT16 width, UINT16 height);

typedef enum
{
    //STOP  MESSAGE
    VDOCHAT_PRIVIEW=0x30,
    VDOCHAT_SETUP=0x31,
    VDOCHAT_ERR =  0x32,

} VDOCHAT_MSGID;

//declaration for ourself
void vdochat_Callback(UINT8 id,  UINT8 ActiveBufNum);
void vdochat_usrmsgHandle(COS_EVENT *pnMsg) ;

#endif//__MMC_VDOCHAT_H__

