//------------------------------------------------------------------------------
//  $Log: sm_trs.h,v $
//  Revision 1.4  2006/05/22 19:35:41  aba
//  Update of the copyright
//
//  Revision 1.3  2006/03/09 07:38:12  aba
//  Trace updating
//
//  Revision 1.2  2005/12/29 17:39:17  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (C) 2003 2004 2005 2006

================================================================================
*/

/*
================================================================================
  File       : sm_trs.h
--------------------------------------------------------------------------------

  Scope      : Header file for sm_trs.c

  History    :
--------------------------------------------------------------------------------
  Aug 02 04  |  PCA   | Creation
================================================================================
*/

#ifndef __SM_TRS_H__
#define __SM_TRS_H__
#include "sxr_ops.h"
#include "cmn_defs.h"
#include "sxs_cfg.h"

#define SM_ACTIVATE_PDP_HEADER_SIZE 17

// This macro ignore the repetition; the unknown IE which are not  comprehension required
// and the IE with syntax error
#define M_GET_TLV_DATA(_PtIEI,_LenDest, _PtDest,_PtOrg, _MinLen, _MaxLen, _PtLastByte) \
{                                                             \
  u8 j;                                                       \
  _LenDest = 0;                                               \
  if (_PtOrg > _PtLastByte)                                   \
    return SM_NO_ERROR;                                       \
  while (1)                                                   \
  {                                                           \
    if (*_PtOrg == *_PtIEI)                                   \
    {                                                         \
      _PtOrg++;                                               \
      if (_PtOrg > _PtLastByte)                               \
        return SM_NO_ERROR;                                   \
      if (*_PtOrg < _MinLen)                                  \
        return 96;                                            \
      if (*_PtOrg > _MaxLen)                                  \
        _LenDest = _MaxLen;                                   \
      else                                                    \
        _LenDest = *_PtOrg;                                   \
      if (_PtOrg + _LenDest > _PtLastByte)                    \
      {                                                       \
        _LenDest = 0;                                         \
        return SM_NO_ERROR;                                   \
      }                                                       \
      memcpy(_PtDest,_PtOrg+1,_LenDest);                      \
      _PtOrg += 1 + *_PtOrg;                                  \
      break;                                                  \
    }                                                         \
    else                                                      \
    { /* Test if this IEI is one of the following IEI */      \
      for (j=1; *(_PtIEI+j) != NO_MORE_IEI;j++)               \
      {                                                       \
        if (*(_PtIEI+j) < 0x10)                               \
        {                                                     \
          if (*(_PtIEI+j) == ((*_PtOrg)>> 4))                 \
            break;                                            \
        }                                                     \
        else                                                  \
        {                                                     \
          if (*(_PtIEI+j) == *_PtOrg)                         \
            break;                                            \
        }                                                     \
      }                                                       \
      if (*(_PtIEI+j) == NO_MORE_IEI)                         \
      { /* This is an unknown IEI */                          \
        if (((*_PtOrg) & 0xF0) == 0)                          \
          return 96;                                          \
        else                                                  \
        { /* Ignore IE */                                     \
          if (((*_PtOrg) & 0x80) == 0)                        \
          { /* TLV */                                         \
            _PtOrg += 2 + *(_PtOrg+1);                        \
          }                                                   \
          else                                                \
          { /* TV type 1 or T type 2 */                       \
            _PtOrg++;                                         \
          }                                                   \
          if (_PtOrg > _PtLastByte)                           \
            return SM_NO_ERROR;                               \
        }                                                     \
      }                                                       \
      else                                                    \
      {                                                       \
        break;                                                \
      }                                                       \
    }                                                         \
  }                                                           \
}

#define M_GET_TV_DATA(_PtIEI,_Dest,_PtOrg, _PtLastByte)       \
{                                                             \
  u8 j;                                                       \
  if (_PtOrg > _PtLastByte)                                   \
    return SM_NO_ERROR;                                       \
  while (1)                                                   \
  {                                                           \
    if ((*_PtIEI < 0x10) && (*_PtIEI == (*_PtOrg>> 4)))       \
    {                                                         \
      _Dest = (*_PtOrg++) & 0xF;                              \
      break;                                                  \
    }                                                         \
    else if ((*_PtIEI > 0x0F) && (*_PtIEI == *_PtOrg))        \
    {                                                         \
      _PtOrg++;                                               \
      if (_PtOrg > _PtLastByte)                               \
        return SM_NO_ERROR;                                   \
      _Dest = *_PtOrg++;                                      \
      break;                                                  \
    }                                                         \
    else                                                      \
    { /* Test if this IEI is one of the following IEI */      \
      for (j=1; *(_PtIEI+j) != NO_MORE_IEI;j++)               \
      {                                                       \
        if (*(_PtIEI+j) < 0x10)                               \
        {                                                     \
          if ((*(_PtIEI+j)) == ((*_PtOrg)>> 4) )              \
            break;                                            \
        }                                                     \
        else                                                  \
        {                                                     \
          if ((*(_PtIEI+j)) == *_PtOrg)                       \
            break;                                            \
        }                                                     \
      }                                                       \
      if (*(_PtIEI+j) == NO_MORE_IEI)                         \
      { /* This is an unknown IEI */                          \
        if (((*_PtOrg) & 0xF0) == 0)                          \
          return 96;                                          \
        else                                                  \
        { /* Ignore IE */                                     \
          if (((*_PtOrg) & 0x80) == 0)                        \
          { /* TLV */                                         \
            SXS_TRACE(SM_DEC_MSG_TRC,"unknown optional TLV IE"); \
            _PtOrg += 2 + *(_PtOrg+1);                        \
          }                                                   \
          else                                                \
          { /* TV type 1 or T type 2 */                       \
            SXS_TRACE(SM_DEC_MSG_TRC,"unknown optional TV or T IE"); \
            _PtOrg++;                                         \
          }                                                   \
          if (_PtOrg > _PtLastByte)                           \
            return SM_NO_ERROR;                               \
        }                                                     \
      }                                                       \
      else                                                    \
      {                                                       \
        break;                                                \
      }                                                       \
    }                                                         \
  }                                                           \
}

#define M_GET_LV_DATA(_LenDest,_PtDest, _PtOrg, _MinLen, _MaxLen, _PtLastByte)\
{                                                             \
  if (*_PtOrg < _MinLen)                                      \
    return 96;                                                \
  if (*_PtOrg > _MaxLen)                                      \
  {                                                           \
    _LenDest = _MaxLen;                                       \
  }                                                           \
  else                                                        \
  {                                                           \
    _LenDest = *_PtOrg;                                       \
  }                                                           \
  if (_PtOrg + _LenDest > _PtLastByte)                        \
    return 96;                                                \
  memcpy(_PtDest,_PtOrg+1,_LenDest);                          \
  _PtOrg += 1 + *_PtOrg;                                      \
}

#define M_SET_LV_DATA(_PtDest, _Len, _PtOrg)                  \
{                                                             \
  if (_Len != 0)                                              \
  {                                                           \
    *_PtDest++ = _Len;                                        \
    memcpy(_PtDest,_PtOrg,_Len);                              \
    _PtDest += _Len;                                          \
  }                                                           \
}

#define M_SET_TV_DATA(_IEI, _PtDest, _Value)                  \
{                                                             \
  *_PtDest++ = _IEI;                                          \
  *_PtDest++ = _Value;                                        \
}

#define M_SET_TLV_DATA(_IEI, _PtDest, _Len, _PtOrg)           \
{                                                             \
  if (_Len != 0)                                              \
  {                                                           \
    *_PtDest++ = _IEI;                                        \
    *_PtDest++ = _Len;                                        \
    memcpy(_PtDest,_PtOrg,_Len);                              \
    _PtDest += _Len;                                          \
  }                                                           \
}

// STATUS ERROR
#define SM_NO_ERROR                            0

#define SM_INVALID_TI_VALUE                   81
#define SM_INVALID_MANDATORY_INFORMATION      96
#define SM_UNKNOWN_MSG_TYPE                   97
#define SM_MSG_TYPE_NOT_COMPATIBLE_WITH_STATE 98

extern u8   sm_PdpInactive_CtxActivate (void);
extern u8   sm_PdpActivePending_T3380  (void);
extern void sm_MmpDataInd              (void);

//added by Akui for 221 on 20090313.
extern void sm_StartTimer (u32 Period, u32 Id, void *Data, u8 ReStart, u8 Mbx);
extern void sm_StopTimer (u32 Id, void *Data, u8 Mbx);
#endif
