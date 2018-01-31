/* Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
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

#ifndef _AT_UTILS_H_
#define _AT_UTILS_H_

#include "cos.h"
#include "sxs_io.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Macro can trace caller address
#define at_malloc(size) COS_Malloc(size, COS_MMI_HEAP)
#define at_free(ptr) COS_Free(ptr)
#define at_realloc(ptr, size) COS_Realloc(ptr, size)
#define at_memcpy(dest, src, size) memcpy(dest, src, size)

// Callback and timer to be executed in AT task
void at_TaskCallback(COS_CALLBACK_FUNC_T func, void *param);
void at_TaskCallbackNotif(COS_CALLBACK_FUNC_T func, void *param);
void at_StopTaskCallback(COS_CALLBACK_FUNC_T func, void *param);
void at_StartCallbackTimer(unsigned ms, COS_CALLBACK_FUNC_T callback, void *param);
void at_StopCallbackTimer(COS_CALLBACK_FUNC_T callback, void *param);

bool at_CmuxCheckFcs(const uint8_t *data, unsigned length, uint8_t crc);
uint8_t at_CmuxCalcFcs(const uint8_t *data, unsigned length);

int at_DtmfChar2Tone(uint8_t c);
int at_DtmfTone2Char(uint8_t c);

#define AT_TRACE_ID (TID(__AT) | TLEVEL(1))
#define ATLOGV(...) COS_TRACE_V(AT_TRACE_ID, __VA_ARGS__)
#define ATLOGD(...) COS_TRACE_D(AT_TRACE_ID, __VA_ARGS__)
#define ATLOGI(...) COS_TRACE_I(AT_TRACE_ID, __VA_ARGS__)
#define ATLOGW(...) COS_TRACE_W(AT_TRACE_ID, __VA_ARGS__)
#define ATLOGE(...) COS_TRACE_E(AT_TRACE_ID, __VA_ARGS__)
#define ATLOGSV(tsmap, ...) COS_TRACE_V(AT_TRACE_ID | (tsmap), __VA_ARGS__)
#define ATLOGSD(tsmap, ...) COS_TRACE_D(AT_TRACE_ID | (tsmap), __VA_ARGS__)
#define ATLOGSI(tsmap, ...) COS_TRACE_I(AT_TRACE_ID | (tsmap), __VA_ARGS__)
#define ATLOGSW(tsmap, ...) COS_TRACE_W(AT_TRACE_ID | (tsmap), __VA_ARGS__)
#define ATLOGSE(tsmap, ...) COS_TRACE_E(AT_TRACE_ID | (tsmap), __VA_ARGS__)
#define ATDUMP(data, size) COS_DUMP(AT_TRACE_ID, data, size);
#define ATDUMP16(data, size) COS_DUMP16(AT_TRACE_ID, data, size);
#define ATDUMP32(data, size) COS_DUMP32(AT_TRACE_ID, data, size);

#define ATUARTLOG(v, ...) COS_TRACE_##v(TID(__AT) | TLEVEL(1), __VA_ARGS__)
#define ATUARTLOGS(v, tsmap, ...) COS_TRACE_##v(TID(__AT) | TLEVEL(1) | (tsmap), __VA_ARGS__)

#define ATUATLOG(v, ...) COS_TRACE_##v(TID(__AT) | TLEVEL(1), __VA_ARGS__)
#define ATUATLOGS(v, tsmap, ...) COS_TRACE_##v(TID(__AT) | TLEVEL(1) | (tsmap), __VA_ARGS__)

#define ATDISPLOG(v, ...) COS_TRACE_##v(TID(__AT) | TLEVEL(1), __VA_ARGS__)
#define ATDISPLOGS(v, tsmap, ...) COS_TRACE_##v(TID(__AT) | TLEVEL(1) | (tsmap), __VA_ARGS__)

#define ATCMDLOG(v, ...) COS_TRACE_##v(TID(__AT) | TLEVEL(2), __VA_ARGS__)
#define ATCMDLOGS(v, tsmap, ...) COS_TRACE_##v(TID(__AT) | TLEVEL(2) | (tsmap), __VA_ARGS__)

#define ATDATALOG(v, ...) COS_TRACE_##v(TID(__AT) | TLEVEL(2), __VA_ARGS__)
#define ATDATALOGS(v, tsmap, ...) COS_TRACE_##v(TID(__AT) | TLEVEL(2) | (tsmap), __VA_ARGS__)

#endif
