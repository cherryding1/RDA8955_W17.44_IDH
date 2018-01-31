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



#ifndef _AT_CMD_SPECIAL_CALL_REC_H_
#define _AT_CMD_SPECIAL_CALL_REC_H_

//#include "callsdefs.h"

//#ifdef MMI_ON_HARDWARE_P
//#include "l4c_common_enum.h"
//#endif


//#define CALL_LOG_NUMBER_STRUCT   l4c_number_struct
//#define CALL_LOG_NAME_STRUCT     l4_name_struct

#ifdef __MMI_MULTI_SIM__
#define MMI_CHIS_DEST_MOD   (MOD_L4C + chis_p->nSimId)
#else
#define MMI_CHIS_DEST_MOD   MOD_L4C
#endif
#define MAX_CC_ADDR_LEN          41
#define MAX_CALL_HISTORY_ENTRIES    20
#define MAX_PB_NAME_LENGTH      30
#define ENCODING_LENGTH         2
#define MAX_PHB_LN_ENTRY 20
#define TOT_SIZE_OF_CALLS_LIST   MAX_PHB_LN_ENTRY
#ifdef MODEM_CALL_RECORDER
extern VOID AT_AUDIO_CmdFunc_Call_Record(AT_CMD_PARA *pParam);
#endif
#endif
