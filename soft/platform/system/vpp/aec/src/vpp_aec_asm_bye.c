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




#include "vppp_aec_asm_common.h"
#include "vppp_aec_asm_map.h"
#include "vppp_aec_asm_sections.h"

#include "voc2_library.h"
#include "voc2_define.h"

// VoC_directive: PARSER_OFF
#include <stdio.h>
// VoC_directive: PARSER_ON


#if 0

voc_word  GLOBAL_BYE,x
voc_short GLOBAL_BYE2,y

#endif


// ************************************************
//   Function:     vpp_HelloBye
//   Description:  Say "Bye"
//   Inputs:
//   Outputs:
//   Used :        all
//   Version 1.0   Create by Yordan  24/12/2008
//   **********************************************

void vpp_HelloBye(void)
{

#if 0

    voc_short LOCAL_BYE[4],x
    voc_word  LOCAL_BYE2,y

#endif


// VoC_directive: PARSER_OFF
    printf("The VPP Hello says Bye\n");
// VoC_directive: PARSER_ON


    VoC_lw16i(REG6,VPP_HELLO_SAYS_BYE);
    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_NOP();
    VoC_sw32_d(REG67,GLOBAL_HELLO_STATUS);

    // for test purposes
    VoC_push16(RA,DEFAULT);
    VoC_jal(vpp_HelloByeTest);
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();

    VoC_return;
}

void vpp_HelloByeTest(void)
{
    // for test purposes
    VoC_sw32_d(REG67,LOCAL_BYE2);
    VoC_sw32_d(REG67,LOCAL_BYE);
    VoC_sw32_d(REG67,LOCAL_BYE+2);

    VoC_return;
}
