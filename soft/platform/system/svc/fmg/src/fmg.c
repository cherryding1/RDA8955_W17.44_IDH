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



#include "cs_types.h"
#include <stdarg.h>
#include "fmg_m.h"
#include "lcdd_m.h"

#include "fmgp_debug.h"
#ifndef NO_FMG_DISPLAY
// =============================================================================
//  MACROS
// =============================================================================

#define MAX_INT_LEN     32
#define BACKGROUND      0xffff  // Was 0, black ?
#define FMG_PIXEL_CH_X  8
#define FMG_PIXEL_CH_Y  10
#define NBLANK          2       // Number of blank lines to maintain


#define NCHAN_CHAR      3   // This goes with the font.dat file, so don't put in .h defines 
#define NCHAN_CHAR_16   1   // This goes with the font.dat file, so don't put in .h defines

// =============================================================================
//  TYPES
// =============================================================================

typedef struct
{
    UINT16 curX;
    UINT16 curY;
    UINT16 maxX;
    UINT16 maxY;
    UINT16 rstX;
    UINT16 rstY;
    BOOL   init;
} FMG_PRINTF_STATE_T;



// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

FMG_PRINTF_STATE_T  g_fmgPrintfState = {0,0,0,0,0,0,FALSE};

CONST UINT16 FMG_ASCII_16[(8*10*128)*NCHAN_CHAR_16] =
{
#include "lucidasans_8x10_16bits.dat"
};



// =============================================================================
//  FUNCTIONS
// =============================================================================
EXPORT INT32 vsprintf(CHAR* buf, CONST CHAR* fmt, va_list args);

// TODO describes
PRIVATE VOID fmg_PrintfClearline(UINT8 nLine);
#endif //NO_FMG_DISPLAY
// =============================================================================
// fmg_PrintfInit
// -----------------------------------------------------------------------------
/// Initializes the printf mechanisms.
/// @param rstX Initial x coordinate - Beginning of any new line.
/// @param rstY Initial y coordinate - First line of a new screen.
/// @param graphX Printable zone last pixel x coordinate. This and graphY are the
/// only coordinates expressed in pixels. All the other ones are in characters.
/// @param graphY Printable zone last pixel y coordinate. This and graphX are the
/// only coordinates expressed in pixels. All the other ones are in characters.
// =============================================================================
PUBLIC UINT32 fmg_PrintfInit(UINT16 rstX, UINT16 rstY, UINT16 graphX, UINT16 graphY)
{
#ifndef NO_FMG_DISPLAY
    g_fmgPrintfState.curX = 0;
    g_fmgPrintfState.curY = 0;

    g_fmgPrintfState.maxX = (graphX/FMG_PIXEL_CH_X);
    g_fmgPrintfState.maxY = (graphY/FMG_PIXEL_CH_Y);

    g_fmgPrintfState.rstX = rstX;
    g_fmgPrintfState.rstY = rstY;

    g_fmgPrintfState.init = TRUE;
#endif  //NO_FMG_DISPLAY
    return 0;
}





// =============================================================================
// fmg_PrintfString
// -----------------------------------------------------------------------------
/// Print a string starting at the current cursor position.
/// @param pString String to print.
// =============================================================================
PUBLIC UINT32 fmg_PrintfString(UINT8* pString)
{
#ifndef NO_FMG_DISPLAY
    UINT32 chCnt;

    FMG_ASSERT(g_fmgPrintfState.init, "FMG not initialized");
    chCnt = 0;
    while (pString[chCnt] != 0)
    {
        fmg_PutChar(pString[chCnt]);
        ++chCnt;
    }
#endif //NO_FMG_DISPLAY
    return 0;
}



// =============================================================================
// fmg_PutChar
// -----------------------------------------------------------------------------
/// Display one char at the current cursor position.
/// @param chr Charater to display.
// =============================================================================
PUBLIC VOID fmg_PutChar(UINT8 chr)
{
#ifndef NO_FMG_DISPLAY
    CONST UINT16 *pCharSrc;
    UINT32 idx;
    UINT16 clrLine;

    if (chr == 0x0d)
    {
        g_fmgPrintfState.curX = g_fmgPrintfState.rstX;
    }
    else if (chr == 0x0a)
    {
        g_fmgPrintfState.curX = g_fmgPrintfState.rstX;
        ++g_fmgPrintfState.curY;
        if (g_fmgPrintfState.curY == g_fmgPrintfState.maxY)
        {
            g_fmgPrintfState.curY = g_fmgPrintfState.rstY;
        }
        clrLine = (g_fmgPrintfState.curY);
        for (idx = 0; idx < NBLANK; ++idx)
        {
            fmg_PrintfClearline(clrLine);
            ++clrLine;
            if (clrLine == g_fmgPrintfState.maxY)
            {
                clrLine = g_fmgPrintfState.rstY;
            }
        }
    }
    else
        // Actual character to draw
    {
        UINT16 startx = g_fmgPrintfState.curX*FMG_PIXEL_CH_X;
        UINT16 starty = g_fmgPrintfState.curY*FMG_PIXEL_CH_Y;
        LCDD_FBW_T frameBufferWin;

        pCharSrc = &FMG_ASCII_16[(chr*FMG_PIXEL_CH_X*FMG_PIXEL_CH_Y) * NCHAN_CHAR_16];

        frameBufferWin.fb.buffer = (UINT16*) pCharSrc;
        frameBufferWin.fb.width = FMG_PIXEL_CH_X;
        frameBufferWin.fb.height = FMG_PIXEL_CH_Y;
        frameBufferWin.fb.colorFormat = LCDD_COLOR_FORMAT_RGB_565;

        frameBufferWin.roi.x=0;
        frameBufferWin.roi.y=0;
        frameBufferWin.roi.width=FMG_PIXEL_CH_X;
        frameBufferWin.roi.height=FMG_PIXEL_CH_Y;


        while (LCDD_ERR_NO != lcdd_Blit16(&frameBufferWin, startx, starty));
//        hal_HstSendEvent(0x7777);



        ++g_fmgPrintfState.curX;
        if (g_fmgPrintfState.curX == g_fmgPrintfState.maxX)
        {
            g_fmgPrintfState.curX = g_fmgPrintfState.rstX;
            ++g_fmgPrintfState.curY;
            if (g_fmgPrintfState.curY == g_fmgPrintfState.maxY)
            {
                g_fmgPrintfState.curY = g_fmgPrintfState.rstY;
            }
        }
    }


    // Check wrap conditions
#endif //NO_FMG_DISPLAY
}

#if 0
// Painful to implement (seems to rely on the current active
// window and we cannot get it easily ..., and all the LCDD API
// function use rect as parameters.
// Moreover, I don't get what this function really does.
// It clears the new line ? A next line ?
// It shouldn't be to difficult to define the proper rect
// if we know which line to erase ...
VOID Util_clearnewline()
{
//  Util_LCM_Set_Active_Win_AU((g_fmgPrintfState.maxX*FMG_PIXEL_CH_X),
    //  FMG_PIXEL_CH_Y*NBLANK);
    //blank out

    Util_LCM_GRAM_ClearWin_AU(BACKGROUND);
    lcdd_SetActiveWindow(
        g_fmgPrintfState.curX*FMG_PIXEL_CH_X,
        g_fmgPrintfState.curY*FMG_PIXEL_CH_Y,      // y pixel offset
        (g_fmgPrintfState.curX+1)*FMG_PIXEL_CH_X - 1,
        (g_fmgPrintfState.curY+1)*FMG_PIXEL_CH_Y - 1);
}
#endif



// =============================================================================
// fmg_PrintfSetXY
// -----------------------------------------------------------------------------
/// Set the cursor to the (xPos, yPos) point. (Coordinates are expressed as number
/// of characters).
/// @param xPos X coordinate.
/// @param yPos Y coordinate.
// =============================================================================
PUBLIC VOID fmg_PrintfSetXY(UINT8 xPos, UINT8 yPos)
{
#ifndef NO_FMG_DISPLAY
    g_fmgPrintfState.curX = xPos;
    g_fmgPrintfState.curY = yPos;
#endif
}


#ifndef NO_FMG_DISPLAY
PRIVATE VOID fmg_PrintfClearline(UINT8 nLine)
{
//  Util_LCM_Set_Active_Win_AU(
//      (g_fmgPrintfState.rstX * FMG_PIXEL_CH_X),
//      (g_fmgPrintfState.maxX-g_fmgPrintfState.rstX) * FMG_PIXEL_CH_X)-1,
//  (nLine * FMG_PIXEL_CH_Y),
//  Util_LCM_GRAM_ClearWin_AU(0);
//  CT_graph_setactivewindow(FMG_PIXEL_CH_X, FMG_PIXEL_CH_Y);
}
#endif


// =============================================================================
// fmg_PrintfClrScr
// -----------------------------------------------------------------------------
/// Clear the screen used to display the printfed text.
// =============================================================================
PUBLIC VOID fmg_PrintfClrScr(VOID)
{
#ifndef NO_FMG_DISPLAY
    LCDD_ROI_T regionOfInterrest;

    UINT16 startx = g_fmgPrintfState.rstX * FMG_PIXEL_CH_X;
    UINT16 starty = g_fmgPrintfState.rstY * FMG_PIXEL_CH_Y;
    UINT16 endx = (g_fmgPrintfState.maxX) * FMG_PIXEL_CH_X-1;
    UINT16 endy = (g_fmgPrintfState.maxY) * FMG_PIXEL_CH_Y-1;

    regionOfInterrest.x=startx;
    regionOfInterrest.y=starty;
    regionOfInterrest.width=endx-startx+1;
    regionOfInterrest.height=endy-starty+1;

    while (LCDD_ERR_NO != lcdd_FillRect16(&regionOfInterrest, BACKGROUND));

    g_fmgPrintfState.curX = g_fmgPrintfState.rstX;
    g_fmgPrintfState.curY = g_fmgPrintfState.rstY;
#endif //NO_FMG_DISPLAY
}

PUBLIC VOID fmg_Fillrect(UINT16 startX, UINT16 startY, UINT16 endX, UINT16 endY,UINT32 color)
{
#ifndef NO_FMG_DISPLAY
    LCDD_ROI_T regionOfInterrest;


    regionOfInterrest.x=startX;
    regionOfInterrest.y=startY;
    regionOfInterrest.width=endX-startX+1;
    regionOfInterrest.height=endY-startY+1;

    while (LCDD_ERR_NO != lcdd_FillRect16(&regionOfInterrest, color));
#endif //NO_FMG_DISPLAY
}


// =============================================================================
// fmg_Printf
// -----------------------------------------------------------------------------
/// Classical printf function.
// =============================================================================
PUBLIC UINT32 fmg_Printf(CONST CHAR* fmt, ...)
{
#ifndef NO_FMG_DISPLAY
    va_list args;
    char msg[256];
    int i;

    va_start(args, fmt);
    i = vsprintf(msg, fmt, args);
    va_end(args);
    fmg_PrintfString(msg);
    return i;
#else
    return 0;
#endif //   NO_FMG_DISPLAY

}
