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

#include <csw.h>
#include <stdkey.h>
#include <event.h>
#include <base_prv.h>
#include <ts.h>
#include<sxr_tksd.h>
#include<sxr_tim.h>
#include "cos.h"
#include "csw_csp.h"

void  COS_CleanDCache()
{
    asm volatile(".set noreorder");
    asm volatile(".align 4");
    asm volatile("cache 2,0");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile(".set reorder");
}
void  COS_CleanICache()
{
    asm volatile(".set noreorder");
    asm volatile(".align 4");
    asm volatile("cache 1,0");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile(".set reorder");

}
void  COS_CleanALLCach()
{
    asm volatile(".set noreorder");
    asm volatile(".align 4");
    asm volatile("cache 0,0");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile(".set reorder");
}

