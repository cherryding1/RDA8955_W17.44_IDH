//==============================================================================
// File       : SMC_PROC.H
//------------------------------------------------------------------------------
// Scope      :
// History    :
//------------------------------------------------------------------------------
// 2006/05/22 | aba | Copyrigth is updated
// 2006/01/05 | aba | update max cp data retrans to 1
// 2005/12/29 | pca | Log keyword added to all files
//  Feb 23 04  |  ABA   | Creation
//==============================================================================

#ifndef __SMC_PROC_H__
#define __SMC_PROC_H__

#include "sxr_ops.h"
#include "cmn_defs.h"

// 15秒是语音信道发送的时间，还需要加上网络返回回复的时间
// 单纯信令信道发送时间稍快，
//#define TC1M_DURATION (15 SECOND)
#define TC1M_DURATION (18 SECOND)  // 20070905
#define MAX_CP_DATA_RETRANS 2

#endif  // __SMR_PROC_H__
