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





#ifndef __SAP_IO_H__
#define __SAP_IO_H__

typedef enum {__SXR, __PAL, __L1A, __L1S, __LAP, __RLU, __RLD,
              __LLC, __MM,  __CC,  __SS,  __SMS,  __SM, __SND, __API,
              __MMI, __SIM, __AT , __M2A, __STT, __RRI, __RRD, __RLP,
              SXS_NB_ID
             } sxs_TraceId_t;

#define _SXR    TID(__SXR)
#define _PAL    TID(__PAL)
#define _L1A    TID(__L1A)
#define _L1S    TID(__L1S)
#define _LAP    TID(__LAP)
#define _RLU    TID(__RLU)
#define _RLD    TID(__RLD)
#define _LLC    TID(__LLC)
#define _MM     TID(__MM)
#define _CC     TID(__CC)
#define _SS     TID(__SS)
#define _SMS    TID(__SMS)
#define _SM     TID(__SM)
#define _SND    TID(__SND)
#define _API    TID(__API)
#define _MMI    TID(__MMI)
#define _SIM    TID(__SIM)
#define _AT     TID(__AT)
#define _M2A    TID(__M2A)
#define _STT    TID(__STT)
#define _RRI    TID(__RRI)
#define _RRD    TID(__RRD)
#define _RLP    TID(__RLP)

#ifdef __SXS_IO_VAR__

const ascii *const sxs_TraceDesc [SXS_NB_ID] =
{
    "SXR", "PAL", "L1A", "L1S", "LAP", "RLU", "RLD",
    "LLC", "MM ", "CC ", "SS ", "SMS", "SM ", "SND", "API",
    "MMI", "SIM", "AT ", "M2A", "STT", "RRI", "RRD", "RLP"
};

#else
extern const u8 *const sxs_TraceDesc [SXS_NB_ID];
#endif



#endif
