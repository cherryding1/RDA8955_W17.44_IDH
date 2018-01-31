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



#if (CSW_EXTENDED_API_LCD == 1)

#include "mci_lcd.h"

// This is bad - this is coming from the MMI
// and the platform should be independant of the MMI
EXPORT UINT32 get_lcd_frame_buffer_address(VOID);

// ============================================================================
// Init_DI
// ----------------------------------------------------------------------------
// ============================================================================
VOID Init_DI(UINT32 background, VOID **unused)
{
    mci_LcdInit(background);
}

// ============================================================================
// mci_LcdBlockWrite
// ----------------------------------------------------------------------------
VOID BlockWrite_DI(UINT16 startx, UINT16 starty, UINT16 endx, UINT16 endy)
{
#ifdef __PRJ_WITH_SPILCD__
    mci_LcdBlockWrite((UINT16 *)(get_lcd_frame_buffer_address() | 0xa0000000), startx, starty, endx, endy);
#else
    mci_LcdBlockWrite((UINT16 *)get_lcd_frame_buffer_address(), startx, starty, endx, endy);
#endif
}

UINT8 partial_display_align_DI(VOID)
{
    return 0;
}

VOID lcd_update_image_DI(UINT16 startx, UINT16 starty, UINT16 Width, UINT16 Height)
{
}

VOID lcd_chipinit_DI(VOID)
{
}

#endif // CSW_EXTENDED_API_LCD


