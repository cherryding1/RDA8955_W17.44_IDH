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

#ifndef _MMC_DRAW_LCD_H_
#define _MMC_DRAW_LCD_H_
#include "lp_lcd_driver.h"
#include "lp_ct810reg.h"
#include "global_macros.h"

//#define LP_LCM_TYPE_R61505U
//#define LP_LCM_TYPE_S6D0144
#define LCD_WAIT_READY()        {\
    uint32 status;  \
    do{             \
        status = LUPINE_RD_REG(LCD_STATE_REG);  \
    }while(0 == (LCD_STATE_RDY & status));\
}
#define LCD_CS_SE(cs)   {\
    uint32 reg;     \
    /* set gpio mode: 0, 1, 17, 18*/    \
    LUPINE_WR_REG(GPIO_MODE, LUPINE_RD_REG(GPIO_MODE) & 0xfff9fffc);\
    cs &= 0x00000070;       \
    LCD_WAIT_READY();   \
    reg = LUPINE_RD_REG(LCD_CFG_REG);   \
    LUPINE_WR_REG(LCD_CFG_REG, reg & (~(cs)));\
}

#define LCD_CS_DE(cs)   {\
    uint32 reg;         \
    cs &= 0x00000070;   \
    LCD_WAIT_READY();   \
    reg = LUPINE_RD_REG(LCD_CFG_REG);   \
    LUPINE_WR_REG(LCD_CFG_REG, reg | cs);   \
}

#define LCD_APB_CMD_DIRECT_WRITE(data)  {\
    uint32 reg = 1u | 0 | 0 | (1u<<9) | (0);    \
    LUPINE_WR_REG(LCD_CTRL_REG, reg);   \
    LCD_WAIT_READY();   \
    LUPINE_WR_REG(LCD_DATA_OUT, data);  \
    LCD_WAIT_READY();   \
}

#define LCD_APB_DATA_DIRECT_WRITE(data) {\
    uint32 reg = 1u | (1u<<4) | 0 | (1u<<9) | (0);\
    LUPINE_WR_REG(LCD_CTRL_REG, reg);   \
    LCD_WAIT_READY();   \
    LUPINE_WR_REG(LCD_DATA_OUT, data);  \
    LCD_WAIT_READY();   \
}
#if defined LP_LCM_TYPE_R61505U
#define DRAWPOINT_START(startx,endx,starty,endy)    {\
    uint32 cs=0x00000010;   \
    LCD_CS_SE(cs);  \
    lcd_set_window(startx,endx,starty,endy);    \
    lcd_dev_set_entrymode(7, 0);        \
    lcd_issue_command(0x0020, startx);  \
    lcd_issue_command(0x0021, starty);  \
    LCD_APB_CMD_DIRECT_WRITE((uint32)0x0022);\
}
#elif defined LP_LCM_TYPE_S6D0144
#define DRAWPOINT_START(startx,endx,starty,endy)    {\
    uint32 cs=0x00000010;   \
    LCD_CS_SE(cs);  \
    lcd_set_window(startx,endx,starty,endy);    \
    lcd_dev_set_entrymode(7, 0);        \
    lcd_issue_command(0x21, (starty&0x00ff)<<8 |(startx&0x00ff) );  \
    LCD_APB_CMD_DIRECT_WRITE((uint32)0x0022);\
}
#endif
#define DRAWPOINT_LCD(data) {\
    LCD_APB_DATA_DIRECT_WRITE((uint32)(data));\
}

#define DRAWPOINT_DONE()    {\
    uint32 cs=0x00000010;   \
    LCD_CS_DE(cs);  \
}

#endif
