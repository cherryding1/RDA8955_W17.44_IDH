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
#include "iot_sms.h"
#include "iot_serial.h"
#include "iot_api.h"

class IOT_MAIN
{
public:
    IOT_MAIN();
    void Event_Handler(UINT32 id, void *msg);

private:
    IOT_GPRS *IOT_Gprs;
    IOT_SMS  *IOT_Sms;
    IOT_SERIAL  *IOT_Serial;
};

IOT_MAIN::IOT_MAIN()
{
    IOT_GPRS *IOT_Gprs = new IOT_GPRS();
    IOT_SMS  *IOT_Sms = new IOT_SMS();
    IOT_SERIAL  *IOT_Serial = new IOT_SERIAL();
}

void IOT_MAIN::Event_Handler(UINT32 id, void *msg)
{
    IOT_trace("IOT get nEventId = %x", id);
    IOT_Sms->send_sms("136xxx", "test");
}

IOT_MAIN *IOT_Main = NULL;

extern "C" {
    void IOT_Event_Handler(UINT32 id, void *msg)
    {
        IOT_trace("IOT_Event_Handler");
        if (IOT_Main == NULL)
            IOT_Main = new IOT_MAIN();

        IOT_Main->Event_Handler(id, msg);
    }
}

