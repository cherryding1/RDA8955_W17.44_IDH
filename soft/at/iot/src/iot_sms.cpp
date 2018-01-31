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
#include  "iot_sms.h"
#include  "iot_api.h"

IOT_SMS::IOT_SMS()
{
    IOT_trace("------IOT_SMS--------");
    return ;
}

bool IOT_SMS::ready()
{
    return TRUE;
}
bool IOT_SMS::available()
{
    return TRUE;
}
bool IOT_SMS::remoteNumber(char *nBuf, int nLen)
{
    return TRUE;
}
int IOT_SMS::read()
{
    return NULL;
}
bool IOT_SMS::flush()
{
    return TRUE;
}

bool IOT_SMS::handler_sms(char *number, char *sms)
{
    //handler sms
    return TRUE;
}

bool IOT_SMS::send_sms(char *number, char *sms)
{
    IOT_trace("------send_sms--------");
//    CFW_SmsSendPresetMessage();
    return TRUE;
}
