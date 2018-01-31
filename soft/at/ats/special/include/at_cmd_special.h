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



#ifndef _AT_CMD_EX_H_
#define _AT_CMD_EX_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
 *                                          INCLUDES
 **************************************************************************************************/
#  include "at_common.h"

/**************************************************************************************************
 *                                         CONSTANTS
 **************************************************************************************************/

/**************************************************************************************************
 *                                          MACROS
 **************************************************************************************************/

/**************************************************************************************************
 *                                         TYPEDEFS
 **************************************************************************************************/

/**************************************************************************************************
 *                                     GLOBAL VARIABLES
 **************************************************************************************************/

/**************************************************************************************************
 *                                     FUNCTIONS - API
 **************************************************************************************************/

/*
 * AT^MONI
 */
VOID AT_EX_CmdFunc_MONI(AT_CMD_PARA *pParam);

/*
 * AT^MONI
 */
VOID AT_EX_TimerFunc_MONI(AT_CMD_PARA *pParam);

/*
 * AT^MONIEND
 */
VOID AT_EX_CmdFunc_MONIEND(AT_CMD_PARA *pParam);

/*
 * AT^MONIEND
 */
VOID AT_EX_TimerFunc_MONIEND(AT_CMD_PARA *pParam);

/*
 * AT^MONP
 */
VOID AT_EX_CmdFunc_MONP(AT_CMD_PARA *pParam);

/*
 * AT^MONP
 */
VOID AT_EX_TimerFunc_MONP(AT_CMD_PARA *pParam);

/*
 * AT^MONPEND
 */
VOID AT_EX_CmdFunc_MONPEND(AT_CMD_PARA *pParam);

/*
 * AT^MONPEND
 */
VOID AT_EX_TimerFunc_MONPEND(AT_CMD_PARA *pParam);

/*
 * AT^SCID
 */
VOID AT_EX_CmdFunc_SCID(AT_CMD_PARA *pParam);

/*
 * AT^SBBI
 */
VOID AT_EX_CmdFunc_SBBI(AT_CMD_PARA *pParam);

/*
 * AT^SBBR
 */
VOID AT_EX_CmdFunc_SBBR(AT_CMD_PARA *pParam);

/*
 * AT^NBBR
 */
VOID AT_EX_CmdFunc_NBBR(AT_CMD_PARA *pParam);

/*
 * AT^SBBW
 */
VOID AT_EX_CmdFunc_SBBW(AT_CMD_PARA *pParam);

/*
 * AT^NBBW
 */
VOID AT_EX_CmdFunc_NBBW(AT_CMD_PARA *pParam);

/*
 * AT^DREG
 */
VOID AT_EX_CmdFunc_DREG(AT_CMD_PARA *pParam);

/*
 * AT^DPBK
 */
VOID AT_EX_CmdFunc_DPBK(AT_CMD_PARA *pParam);

/*
 * AT^DFFS
 */
VOID AT_EX_CmdFunc_DFFS(AT_CMD_PARA *pParam);

/*
 * AT^DSMS
 */
VOID AT_EX_CmdFunc_DSMS(AT_CMD_PARA *pParam);

/*
 * AT^PRDU
 */
VOID AT_EX_CmdFunc_PRDU(AT_CMD_PARA *pParam);

/*
 * AT^AUST
 */
VOID AT_EX_CmdFunc_AUST(AT_CMD_PARA *pParam);

/*
 * AT^AUEND
 */
VOID AT_EX_CmdFunc_AUEND(AT_CMD_PARA *pParam);

/*
 * AT^SAIC
 */
VOID AT_EX_CmdFunc_SAIC(AT_CMD_PARA *pParam);

/*
 * AT^SBC
 */
VOID AT_EX_CmdFunc_SBC(AT_CMD_PARA *pParam);

/*
 * AT^CBCM
 */
VOID AT_EX_CmdFunc_CBCM(AT_CMD_PARA *pParam);

/*
 * AT^SCTM
 */
VOID AT_EX_CmdFunc_SCTM(AT_CMD_PARA *pParam);

/*
 * A/
 */
VOID AT_EX_CmdFunc_RCMD(AT_CMD_PARA *pParam);

/*
 * AT+CMUT
 */
VOID AT_CC_CmdFunc_CLVL(AT_CMD_PARA *pParam);
VOID AT_CC_CmdFunc_VGR(AT_CMD_PARA *pParam);
VOID AT_CC_CmdFunc_CMUT(AT_CMD_PARA *pParam);

/*
 *
 */
VOID AT_EX_AsyncEventProcess(COS_EVENT *pEvent);

/**************************************************************************************************
**************************************************************************************************/

#ifdef __cplusplus
}
#endif
#endif
