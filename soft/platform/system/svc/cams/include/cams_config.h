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



#ifndef _CAMS_CONFIG_H_
#define _CAMS_CONFIG_H_


// =============================================================================
// CAMS_CONFIG_T
// -----------------------------------------------------------------------------
/// Configuration structure for CAMS.
/// Defines parameters such as the sensor orientation, etc.
// =============================================================================
typedef struct
{
    /// Sensor orientation, as the parameter needed by
    /// cams_ResizeVgaToScreen to display the image in the
    /// right direction.
    UINT32 sensorOrientation;
} CAMS_CONFIG_T;


#endif // _CAMS_CONFIG_H_











