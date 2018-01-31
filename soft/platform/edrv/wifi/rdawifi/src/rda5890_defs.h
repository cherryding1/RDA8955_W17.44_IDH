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

#ifndef _RDA5890_DEFS_H_
#define _RDA5890_DEFS_H_

#include <linux/spinlock.h>

#define RDA5890_SDIOWIFI_VER_MAJ     0
#define RDA5890_SDIOWIFI_VER_MIN     3
#define RDA5890_SDIOWIFI_VER_BLD     1

#define DEBUG

extern int rda5890_dbg_level;
extern int rda5890_dbg_area;

typedef enum
{
    RDA5890_DL_ALL   = 0,
    RDA5890_DL_CRIT  = 1,
    RDA5890_DL_TRACE = 2,
    RDA5890_DL_NORM  = 3,
    RDA5890_DL_DEBUG = 4,
    RDA5890_DL_VERB  = 5,
} RDA5890_DBG_LEVEL;

#define RDA5890_DA_MAIN            (1 << 0)
#define RDA5890_DA_SDIO            (1 << 1)
#define RDA5890_DA_ETHER           (1 << 2)
#define RDA5890_DA_WID             (1 << 3)
#define RDA5890_DA_WEXT            (1 << 4)
#define RDA5890_DA_TXRX            (1 << 5)
#define RDA5890_DA_PM              (1 << 6)
#define RDA5890_DA_ALL             0x0000007f

#define RDA5890_LOG "RDA5890: "

#ifdef DEBUG
#define RDA5890_DBGLA(area, lvl)                                             \
    (((lvl)<=rda5890_dbg_level) && ((area)&rda5890_dbg_area))
#define RDA5890_DBGLAP(area,lvl, x...)                                       \
    do{                                                                  \
        if (((lvl)<=rda5890_dbg_level) && ((area)&rda5890_dbg_area)) \
            printk(KERN_INFO RDA5890_LOG x );                    \
    }while(0)
#define RDA5890_DBGP(x...)                                                   \
    do{                                                                  \
        printk(KERN_INFO RDA5890_LOG x );                            \
    }while(0)
#else
#define RDA5890_DBGLA(area, lvl)    0
#define RDA5890_DBGLAP(area,lvl, x...)  do {} while (0)
#define RDA5890_DBGP(x...)  do {} while (0)
#endif

#define RDA5890_ERRP(fmt, args...)                                          \
    do{                                                                 \
        printk(KERN_ERR RDA5890_LOG "%s: "fmt, __func__, ## args ); \
    }while(0)

#endif

