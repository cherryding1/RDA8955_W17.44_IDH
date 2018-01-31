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



#ifndef _GSENSOR_M_H_
#define _GSENSOR_M_H_

#include "cs_types.h"
#include "hal_timers.h"

#include "tgt_gsensor_cfg.h"



// =============================================================================
//  MACROS    message struct
// =============================================================================
//rotate
#define    GS_MOTION_ROTATE_RIGHT        0x01
#define    GS_MOTION_ROTATE_LEFT         0x02
#define    GS_MOTION_ROTATE_UP           0x03
#define    GS_MOTION_ROTATE_DOWN         0x04
#define    GS_MOTION_ROTATE_UNKNOW       0x05

//shake:
#define    GS_MOTION_SHAKE_X_L_AXIS         0x11
#define    GS_MOTION_SHAKE_X_R_AXIS         0x12
// the orientation (up or down) correnspoing the sing (minus or plus) of g_cell on X axis
#define    GS_MOTION_SHAKE_Y_L_AXIS         0x13
#define    GS_MOTION_SHAKE_Y_R_AXIS         0x14
// the orientation (left or right) correnspoing the sing (minus or plus) of g_cell on Y axis
#define    GS_MOTION_SHAKE_Z_L_AXIS         0x15
#define    GS_MOTION_SHAKE_Z_R_AXIS         0x16

#define    GS_MOTION_SHAKE_XYZ_AXIS       0x17  // shake happened but the chip doesn't know which axis has a shake.
// the orientation (back or forward) correnspoing the sing (minus or plus) of g_cell on Z axis
#define    GS_SHAKE_NO_HAPPEN             0x18


//turnover:
#define    GS_MOTION_TURNOVER_FRONT      0x21  // the phone is lying on its front
#define    GS_MOTION_TURNOVER_BACK       0x22  // the phone is lying on its back   
#define    GS_MOTION_TURNOVER_UNKNOW     0x23  // nothing  

//tap:
#define    GS_MONTON_TAP_HAPPEN          0x31
#define    GS_MONTON_TAP_NO_HAPPEN       0x32

// =============================================================================
//      message struct
// =============================================================================


// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// GSENSOR_ERR_T
// -----------------------------------------------------------------------------
/// Error types for the FMD module.
// =============================================================================
typedef enum
{
    /// No error occured
    GSENSOR_ERR_NO,

    /// An attempt to access a busy resource failed
    /// (Resource use not possible at the time)
    GSENSOR_ERR_RESOURCE_BUSY,

    /// Attempt to open a resource already opened,
    /// (no need to open again to use).
    GSENSOR_ERR_ALREADY_OPENED,

    /// Timeout while trying to access the resource
    GSENSOR_ERR_RESOURCE_TIMEOUT,

    /// Invalid parameter
    GSENSOR_ERR_BAD_PARAMETER,

    /// The specified interface does not exist
    GSENSOR_ERR_NO_ITF,


    GSENSOR_ERR_QTY
} GSENSOR_ERR_T;

typedef enum
{
    SHANK_X_AXIS       = 0x4,
    SHANK_Y_AXIS       = 0x2,
    SHANK_Z_AXIS       = 0x1,
    SHANK_XYZ_AXIS     = 0x7,
    SHANK_X_QTY        =0xf
} GSENSOR_SHAKE_AXIS;

typedef enum {NOSHAKE, XSKPOS, XSKNEG, YSKPOS, YSKNEG, ZSKPOS, ZSKNEG} ShakeDir;


// ============================================================================
//  GLOBAL VARIABLES
// ============================================================================

// =============================================================================
// GSS_SAMPLE_DATA_T
// -----------------------------------------------------------------------------
///
///
///
// =============================================================================
typedef struct
{
    /// corresponding Pola
    UINT8 rotate;
    /// corresponding shake
    UINT8 shake;
    /// corresponding BaFro
    UINT8 tunover;
    /// tap
    UINT8 tap;
    ///  measures value of x axis
    INT8 x;
    ///  measures value of y axis
    INT8 y;
    ///  measures value of z axis
    INT8 z;

} GSS_SAMPLE_DATA_T;
// ============================================================================
//  GSS_CALLBACK_T
// ----------------------------------------------------------------------------
/// Type used to define the callback function that will be called by the
/// gsensor monitor driver
/// @param penState The state of the pen. See #TSD_PEN_STATE_T structure for details.
///
/// WARNING: this function is called form an IRQ context it should not be doing
/// any long processing, (like updating a screen display) but instead send a
/// message to a task.
// ============================================================================
typedef VOID (*GSS_CALLBACK_T)(GSS_SAMPLE_DATA_T* gsensor_data);

PUBLIC VOID gsensor_SetCallback(GSS_CALLBACK_T callback);

// ============================================================================
//  gsensor_Init
// ----------------------------------------------------------------------------
/// This function initializes the interrupter of gsensor module . The configuration of the
/// INT pin used of the gsensor stored in a GSENSOR_CONFIG_STRUCT_T struct .
/// @param enable_int is ture mean enable the interrupt
// ============================================================================

PUBLIC VOID gsensor_Init(BOOL enable_int);


//=============================================================================
// gsensor_filt_orientation
//-----------------------------------------------------------------------------
/// config the counts of filtering the portrail/landscapes samples.
/// @param counts
/// counts is 0 - 7 , deafult 4
//=============================================================================
VOID gsensor_filt_orientation(UINT8 count);

//=============================================================================
// gsensor_start_monitor
//-----------------------------------------------------------------------------
///
/// @param sample_period_ms is the period of detectiong the change of gsensor.
/// generally  35ms is recommend
//=============================================================================

GSENSOR_ERR_T gsensor_start_monitor(UINT32 sample_period_ms );
GSENSOR_ERR_T gsensor_stop_monitor(VOID);

//=============================================================================
// gsensor_set_shake_Axis
//-----------------------------------------------------------------------------
/// this function decide on which axis the shake will be detected.
/// @param x_axis:0   y_axis:1     z_axis:2    xyz_axis:3
///
//=============================================================================
//VOID gsensor_set_shake_Axis(GSENSOR_SHAKE_AXIS xyz);


//GSENSOR_ERR_T gsensor_Read_xyz(UINT8* x_adc, UINT8* y_adc,UINT8* z_adc);
//GSENSOR_ERR_T gsensor_enter_standby_mode(VOID);
//GSENSOR_ERR_T gsensor_enter_active_mode(VOID);


#endif //

