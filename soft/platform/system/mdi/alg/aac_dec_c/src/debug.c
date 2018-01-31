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





#include "wrapper.h"

#if !defined (_DEBUG)

void DebugMemCheckInit(void)
{
}

void DebugMemCheckStartPoint(void)
{
}

void DebugMemCheckEndPoint(void)
{
}

void DebugMemCheckFree(void)
{
}

#elif defined (_WIN32) && !defined (_WIN32_WCE)

#include <stdio.h>
#include <crtdbg.h>

#ifdef FORTIFY
#include "fortify.h"
#else
static  _CrtMemState oldState, newState, stateDiff;
#endif

void DebugMemCheckInit(void)
{
    int tmpDbgFlag;

    /* Send all reports to STDOUT */
    _CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );
    _CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT );
    _CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );
    _CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDOUT );
    _CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE );
    _CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDOUT );

    tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
    tmpDbgFlag |= _CRTDBG_CHECK_ALWAYS_DF;
    _CrtSetDbgFlag(tmpDbgFlag);
}

void DebugMemCheckStartPoint(void)
{
#ifdef FORTIFY
    Fortify_EnterScope();
#else
    _CrtMemCheckpoint(&oldState);
#endif
}

void DebugMemCheckEndPoint(void)
{
#ifdef FORTIFY
    Fortify_LeaveScope();
#else
    _CrtMemCheckpoint(&newState);
    _CrtMemDifference(&stateDiff, &oldState, &newState);
    _CrtMemDumpStatistics(&stateDiff);
#endif
}

void DebugMemCheckFree(void)
{
    printf("\n");
    if (!_CrtDumpMemoryLeaks())
        printf("Memory leak test:      no leaks\n");

    if (!_CrtCheckMemory())
        printf("Memory integrity test: error!\n");
    else
        printf("Memory integrity test: okay\n");
}

#elif defined (__arm) && defined (__ARMCC_VERSION)

void DebugMemCheckInit(void)
{
}

void DebugMemCheckStartPoint(void)
{
}

void DebugMemCheckEndPoint(void)
{
}

void DebugMemCheckFree(void)
{
}

#elif defined(__GNUC__) && (defined(__POWERPC__) || defined(__powerpc__))

void DebugMemCheckInit(void)
{
}

void DebugMemCheckStartPoint(void)
{
}

void DebugMemCheckEndPoint(void)
{
}

void DebugMemCheckFree(void)
{
}

#endif
