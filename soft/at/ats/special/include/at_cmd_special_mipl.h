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

#ifdef AT_MIPL_SUPPORT

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
 *                                          INCLUDES
 **************************************************************************************************/
#include "at_common.h"



#ifdef __cplusplus
}
#endif

VOID AT_MIP_CmdFunc_MIPLCR(AT_CMD_PARA *pParam);
VOID AT_MIP_CmdFunc_MIPLDEL(AT_CMD_PARA *pParam);
VOID AT_MIP_CmdFunc_MIPLOPEN(AT_CMD_PARA *pParam);
VOID AT_MIP_CmdFunc_MIPLCLOSE(AT_CMD_PARA *pParam);
VOID AT_MIP_CmdFunc_MIPLCHG(AT_CMD_PARA *pParam);
VOID AT_MIP_CmdFunc_MIPLSND(AT_CMD_PARA *pParam);
VOID AT_MIP_CmdFunc_MIPLRCV(AT_CMD_PARA *pParam);
VOID AT_MIP_CmdFunc_MIPLNFY(AT_CMD_PARA *pParam);
VOID AT_MIP_CmdFunc_MIPLVER(AT_CMD_PARA *pParam);

typedef enum mipl_type_t
{
    MIPL_READ = 0,
    MIPL_WRITE,
    MIPL_EXECUTE,
    MIPL_OBSERVE,
} MIPL_TYPE_T;

#endif
