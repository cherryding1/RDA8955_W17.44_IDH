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


#ifndef _FS_ASYN_H_
#define _FS_ASYN_H_

// FS asynchronization api error code defining.

// Base
#define ERR_FS_ASYN_BASE                           100000

// Success.
#define ERR_FS_ASYN_SUCCESS                     0

// Parameter is invalid.
#define ERR_FS_ASYN_PARA_INVALID            ERR_FS_ASYN_BASE + 1

// Malloc failed.
#define ERR_FS_ASYN_MALLOC_FAILED         ERR_FS_ASYN_BASE + 2

// Send event failed.
#define ERR_FS_ASYN_SENDEVENT_FAILED   ERR_FS_ASYN_BASE + 3

// fs asyn task not start.
#define ERR_FS_ASYN_TASK_NOT_START      ERR_FS_ASYN_BASE + 4
// Event Id define.

// Base.
#define FS_ASYN_EVENT_ID_BASE     0x1000

// Read request.
#define FS_ASYN_EVENT_ID_READ_REQ       (FS_ASYN_EVENT_ID_BASE + 1)

// Write request.
#define FS_ASYN_EVENT_ID_WRITE_REQ      (FS_ASYN_EVENT_ID_BASE + 2)

// GetFsInfo request.
#define FS_ASYN_EVENT_ID_GETFSINFO_REQ     (FS_ASYN_EVENT_ID_BASE + 3)


// Callback function define.
typedef VOID (*FS_ASNY_FUNCPTR)(void *);

// fs asyn read requst stucture.
typedef struct _fs_asyn_read_req
{
    INT32 iFd;                                      // File desciption,received call the function FS_Open.
    UINT8 *pBuff;                                // Point to a buffer of to read.
    UINT32 uSize;                               // the size of to read.
    FS_ASNY_FUNCPTR pCBFunc;          // call back fuction, it shall been called when completed reading.
} FS_ASYN_READ_REQ;

// fs asyn write requst stucture.
typedef struct _fs_asyn_write_req
{
    INT32 iFd;                                     // File desciption,received call the function FS_Open.
    UINT8 *pBuff;                               // Point to a buffer of to write.
    UINT32 uSize;                              // the size of to write.
    FS_ASNY_FUNCPTR pCBFunc;       // call back fuction, it shall been called when completed writing.
} FS_ASYN_WRITE_REQ;

// fs asyn getfsinfo requst stucture.
typedef struct _fs_asyn_getfsinfo_req
{
    UINT8 pDevName[16];                               // Point to a buffer of device name.
    FS_ASNY_FUNCPTR pCBFunc;       // call back fuction, it shall been called when completed.
} FS_ASYN_GETFSINFO_REQ;

// fs asyn read result structure.
typedef struct _fs_asyn_read_result
{
    UINT32   uSize;                      // size of really readed.
    UINT8   *pBuff;                        // point to the buffer of to read.
    INT32    iResult;                       // read option process result.if success, it is zero,else,it is the return value of FS_Read.
} FS_ASYN_READ_RESULT;

// fs asyn write result structure.
typedef struct _fs_asyn_write_result
{
    UINT32   uSize;                     // size of really writed.
    UINT8   *pBuff;                        // point to the buffer of to write.
    INT32    iResult;                        // write option process result.if success, it is zero,else,it is the error code  of FS_Write.
} FS_ASYN_WRITE_RESULT;

// fs asyn getfsinfo result structure.
typedef struct _fs_asyn_getfsinfo_result
{
    UINT64 iTotalSize;                     // Total size
    UINT64 iUsedSize;                     // Has used  size
    INT32    iResult;                        // GetFsInfo option process result.if success, it is zero,else,it is the error code  of FS_GetFsInfo.
} FS_ASYN_GETFSINFO_RESULT;

//fs asyn task entry.
VOID BAL_AsynFsTask(void *pData);


//-----------------------------------------------------------------------------------------
// FS_AsynReadReq
// Function:
//    Asynchronization read a file.
// Parameter:
//     iFd[in]:  File description,received FS_Open.
//     pBuff[in,out]: point to a buffer to read.
//     uSize[in]: read size.
//     pCallback[in]:callback function.
// Return value:
//     Upon successed completion ,ERR_FS_ASYN_SUCCESS shall be return, else the err code shall be return.
//-----------------------------------------------------------------------------------------
INT32 FS_AsynReadReq(INT32 iFd, UINT8 *pBuff, UINT32 uSize, FS_ASNY_FUNCPTR pCallback);

//-----------------------------------------------------------------------------------------
// FS_AsynWriteReq
// Function:
//    Asynchronization write a file.
// Parameter:
//     iFd[in]:  File description,received FS_Open.
//     pBuff[in,out]: point to a buffer to write.
//     uSize[in]: write size.
//     pCallback[in]:callback function.
// Return value:
//     Upon successed completion ,ERR_FS_ASYN_SUCCESS shall be return, else the err code shall be return.
//-----------------------------------------------------------------------------------------
INT32 FS_AsynWriteReq(INT32 iFd, UINT8 *pBuff, UINT32 uSize, FS_ASNY_FUNCPTR pCallback);

//-----------------------------------------------------------------------------------------
// FS_AsynGetFsInfoReq
// Function:
//    Asynchronization Get fs info.
// Parameter:
//     pDevName[in]:  Device name.
//     pCallback[in]:callback function.
// Return value:
//     Upon successed completion ,ERR_FS_ASYN_SUCCESS shall be return, else the err code shall be return.
//-----------------------------------------------------------------------------------------
INT32 FS_AsynGetFsInfoReq(UINT8 *pDevName, FS_ASNY_FUNCPTR pCallback);

#endif
