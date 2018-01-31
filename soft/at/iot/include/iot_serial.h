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

#ifndef _IOT_SERIAL_H
#define _IOT_SERIAL_H
#include "iot_api.h"

class IOT_SERIAL
{
public:
    IOT_SERIAL();
//        bool begin(UART_BAUDRATE nRat);
    bool println(char *nBuf);
    bool print(char *nBuf);
    void delay(int nTime);
private:
    int index;
};
IOT_SERIAL::IOT_SERIAL()
{
    IOT_trace("------IOT_SERIAL");
}

//bool IOT_SERIAL::begin(UART_BAUDRATE nRat)
//{
//    //return CFW_SmsInitReady(CFW_SIM_0);
//    return ;
//}
bool IOT_SERIAL::println(char *nBuf)
{
//    return CFW_SmsInitReady(CFW_SIM_0);
    return TRUE;
}
bool IOT_SERIAL::print(char *nBuf)
{
//    return CFW_SmsInitReady(CFW_SIM_0);
    return TRUE;
}
void IOT_SERIAL::delay(int nTime)
{
//   hal_TimDelay(nTime);
    return;
}

IOT_SERIAL Serial;
#endif

