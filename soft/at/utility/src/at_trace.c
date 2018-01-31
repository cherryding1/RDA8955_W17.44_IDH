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



#include "at.h" // includes "trace.h" header file.
#include "sxs_io.h"

/*
Below are the trace switches. You can change it's value to control the output of any kind
of trace information.
**/

UINT32 AT_OutputText(UINT16 nIndex, PCSTR fmt, ...)
{
    if (nIndex > 16)
        return ERR_NOT_SUPPORT;

    char uart_buf[256];
    va_list ap;

    va_start (ap, fmt);
    vsnprintf(uart_buf, sizeof(uart_buf), fmt, ap);
    va_end (ap);

    if (nIndex == 0x00)
    {
        // Forced trace
        SXS_TRACE(_AT | TNB_ARG(0) | TSTDOUT, uart_buf);
    }
    else
    {
        // Regular trace
        SXS_TRACE(_AT | TNB_ARG(0) | TLEVEL(nIndex), uart_buf);
    }
    return ERR_SUCCESS;
}

#ifdef DEBUG
void AT_TC_MEMBLOCK(UINT16 nIndex, UINT8 *buffer, UINT16 len, UINT8 radix)
{
    INT32 i            = 0;
    CHAR s[4]          = { 0 };
    CHAR line_buff[64] = { 0 }; // Temporary buffer for print trace information.

    AT_TC(nIndex, "==================TRACE MEMORY BLOCK=====================>>");

    // Just print part of the data if the data length overrun the temporary buffer size.

    // ///////////////////////////////////////////////////////////////////////////
    // the buffer of vsprintf is not more than 256(the max number chars of each line), so
    // the size of the buffer to need to print is not limited, line_buffer size can be less
    // than 256 each line and it can be used circularly, modify by wangqunyang 2008.06.02
    // ///////////////////////////////////////////////////////////////////////////
    /*
       if (len > sizeof(line_buff))
       len = sizeof(line_buff);
     */

    for (i = 0; i < len; i++)
    {
        if (radix == 10)
            AT_Sprintf(s, "%3d ", buffer[i]);
        else if (radix == 16)
            AT_Sprintf(s, "%02x ", buffer[i]);
        else  // default case
            AT_Sprintf(s, "%02x ", buffer[i]);

        /* we can print 10 chars each line or any others being less than
           ** line_buff size and in this function max size of line_buff is 64 */
        if (i != 0 && i % 10 == 0)
        {
            AT_TC(nIndex, "%s", line_buff);
            memset(line_buff, 0, sizeof(line_buff));
        }

        strcat(line_buff, s);
    }

    if (strlen(line_buff) > 0)
        AT_TC(nIndex, "%s", line_buff);

    AT_TC(nIndex, "==================TRACE MEMORY BLOCK=====================<<");
}
#endif
