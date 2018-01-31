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

#ifndef __IOT_AT_GPRS_H__
#define __IOT_AT_GPRS_H__

#include "IOTConnectBase.h"
class IOT_GPRS : public CIOTConnectBase
{
public:
    IOT_GPRS();
    ~IOT_GPRS();

    bool attachGPRS();
    bool connect(char *nURL,int nLen);
    bool println(char *nBuf);
    void available();
    void read();
private:
    int m_Index;
};
#endif /* __IOT_AT_GPRS_H__ */
