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

#include "iot_gprs.h"
#include "iot_api.h"

IOT_GPRS::IOT_GPRS()
{
    IOT_trace("------IOT_GPRS--------");
    m_Index = 0;
    return ;
}

IOT_GPRS::~IOT_GPRS()
{
    IOT_trace("------~IOT_GPRS--------");
    m_Index = 0;
}

bool IOT_GPRS::attachGPRS()
{
    IOT_trace("------~attachGPRS--------");
    return TRUE;
}
bool IOT_GPRS::println(char *nBuf)
{
    return TRUE;
}
bool IOT_GPRS::connect(char *nURL,int nLen)
{
    IOT_trace("------connect--------");
    return Connect();
}
void IOT_GPRS::available()
{
}
void IOT_GPRS::read()
{

}
