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

#ifndef __IOT_CONNECT_BASE_H__
#define __IOT_CONNECT_BASE_H__

#include "iot_api.h"
#include "cs_types.h"
class CIOTConnectBase
{
public:
    CIOTConnectBase()
    {
        IOT_trace("----CIOTConnectBase::init construct");
    };
    ~CIOTConnectBase()
    {
    };

    BOOL Connect()
    {
        IOT_trace("----CIOTConnectBase::Connect");
    };

    BOOL DisConnect()
    {
        IOT_trace("----CIOTConnectBase::DisConnect");
    };

    BOOL open()
    {
        IOT_trace("----CIOTConnectBase::open");
    };
    BOOL close()
    {
        IOT_trace("----CIOTConnectBase::close");
    };

    BOOL read()
    {
        IOT_trace("----CIOTConnectBase::read");
    };
    BOOL write()
    {
        IOT_trace("----CIOTConnectBase::write");
    };

    BOOL SetState()
    {
        IOT_trace("----CIOTConnectBase::SetState");
    };
    BOOL GetState()
    {
        IOT_trace("----CIOTConnectBase::GetState");
    };

    enum
    {
        STATE_READY,
        STATE_SLEEP,
        STATE_ON_READY
    };

private:
    UINT8 m_State;
};

#endif /* __IOT_CONNECT_BASE_H__ */
