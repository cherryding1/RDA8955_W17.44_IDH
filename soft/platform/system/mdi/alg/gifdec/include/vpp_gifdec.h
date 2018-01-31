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
















#ifndef VPP_GIFDEC_H
#define VPP_GIFDEC_H

#include "cs_types.h"
#include "hal_voc.h"

typedef struct
{
    UINT16 error_code;
    UINT16 dec_finished;
} VPP_GIFDEC_STATUS_T;

typedef struct
{
    S32 dest;
    S32 dest_pitch_jump;

    S16 *want_sx_table;
    S16 *want_sx_table_end;
    S16 *next_want_sx;

    S16 *want_sy_table;
    S16 want_sx_table_size;
    S16 want_sy_table_size;

    S16 bitsperpixels;
    S16 src_height_range;   /* Source */
    S16 dest_height_range;  /* Destination */
    S16 offset_dx;
    S16 offset_dy;

    S16 want_dx1, want_dx2; /* wanted rendering area */
    S16 want_dy1, want_dy2;

    S16 want_sx;    /* current wanted point (want_x,want_y) */
    S16 want_sy;

    S16 next_want_dy;

    S16 dir_x;
    S16 dir_y;

    /* for non resize used */
    S16 want_start_sx, want_start_sy;
    S16 want_end_sx, want_end_sy;

} gdi_resizer_struct_voc;


typedef struct
{
    UINT16  GCT[256];       // global color table (must be 4 byte aligned)
    UINT32 shadow_adrs;
    UINT16 shadow_width,shadow_height;

    // image info
    UINT32 img_adrs;
    UINT32 img_size;
    UINT16  width;      // image width
    UINT16  height;     // image height
    UINT16  x;              // image top
    UINT16  y;              // image left

    UINT16 transparent_enable; //it will overwrite the TC flag of the GCE
    UINT16 transparent_index;
    UINT16 is_interlace;
    UINT16 temp;
    UINT32 buffer_offset;
    UINT32 resizer;
} VPP_GIFDEC_CFG_T;



typedef struct
{
    U16 CUR_POS;
    U16 CACHE_DEBUG[1024];
} CACHE_PROFILE_T;

// ============================================================================
// VPP_GIFDEC_MODE_T
// ----------------------------------------------------------------------------
/// VPP GIFDEC code mode enumerator
// ============================================================================

typedef enum
{
    VPP_GIFDEC_MODE_NO = -1,
    VPP_GIFDEC_MODE_HI =  0,
    VPP_GIFDEC_MODE_BYE

} VPP_GIFDEC_MODE_T;

// ============================================================================
// VPP_GIFDEC_WAKEUP_MODE_T
// ----------------------------------------------------------------------------
/// Defines the order in which the tasks are executed an the way these
/// tasks are started
// ============================================================================

typedef enum
{
    /// No wakeup enabled
    VPP_GIFDEC_WAKEUP_NO = 0,
    /// Wake on software event SOF0.
    VPP_GIFDEC_WAKEUP_SW0,
    /// Wake on hardware event IFC0.
    VPP_GIFDEC_WAKEUP_HW0

} VPP_GIFDEC_WAKEUP_MODE_T;

// ============================================================================
// VPP_PNGDEC_ERROR_T
// ----------------------------------------------------------------------------
/// VPP PNGDEC error enumerator
// ============================================================================

typedef enum
{
    /// There is no error
    VPP_GIFDEC_ERROR_NO                  =  0,
    /// There is error
    VPP_GIFDEC_ERROR_YES                 = -1

} VPP_PNGDEC_ERROR_T;



PUBLIC HAL_ERR_T vpp_GifDecOpen(HAL_VOC_IRQ_HANDLER_T vocIrqHandler,
                                VPP_GIFDEC_WAKEUP_MODE_T wakeupMode);

PUBLIC VOID vpp_GifDecClose(VOID);

PUBLIC VOID vpp_GifDecStatus(VPP_GIFDEC_STATUS_T * pStatus);
PUBLIC VOID vpp_GifDecProfile(CACHE_PROFILE_T *profile);
PUBLIC VOID vpp_GifDecCfg(VPP_GIFDEC_CFG_T* pCfg);

PUBLIC HAL_ERR_T vpp_GifDecSchedule(VOID);

PUBLIC VOID vpp_GifDecTest();

#endif
