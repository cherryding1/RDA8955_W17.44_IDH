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

#ifndef _RDA5890_IOCTL_H_
#define _RDA5890_IOCTL_H_

#include "linux/sockios.h"

#define IOCTL_RDA5890_BASE                SIOCDEVPRIVATE
#define IOCTL_RDA5890_GET_MAGIC           IOCTL_RDA5890_BASE
#define IOCTL_RDA5890_GET_DRIVER_VER      (IOCTL_RDA5890_BASE + 1)
#define IOCTL_RDA5890_MAC_GET_FW_VER      (IOCTL_RDA5890_BASE + 2)
#define IOCTL_RDA5890_MAC_WID             (IOCTL_RDA5890_BASE + 3)
#define IOCTL_RDA5890_SET_WAPI_ASSOC_IE   (IOCTL_RDA5890_BASE + 4)
#define IOCTL_RDA5890_GET_WAPI_ASSOC_IE   (IOCTL_RDA5890_BASE + 5)

#define RDA5890_MAGIC 0x5890face

#endif /* _RDA5890_IOCTL_H_  */

