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




#ifndef LCD_IF_H
#define LCD_IF_H
#include "kal_non_specific_general_types.h"

#ifdef MMI_ON_HARDWARE_P        // In order to run on emulator
#include "csw_lcd.h"
#else
#ifndef ONLY_AT_SUPPORT
#ifndef FPGA
typedef unsigned char           kal_uint8;
#endif
#endif
typedef void (*lcd_func_type)(void);
#endif

#define LCD_TOTAL_LAYER                            4
#define MAIN_LCD    1
#define SUB_LCD        2

typedef enum
{
    LCD_IDLE_STATE=0,
    LCD_INITIAL_STATE,
    LCD_STANDBY_STATE,
    LCD_SLEEP_STATE,
    LCD_SW_UPDATE_STATE,
//add by WeiD

    LCD_MEM_UPDATE_STATE,
    LCD_JPEG_VIDEO_UPDATE_STATE,
    LCD_JPEG_VIDEO_CMD_QUEUE_STATE,

    LCD_CAMERA_ACTIVE_STATE,
    LCD_CAMERA_ACTIVE_UPDATE_STATE,
    LCD_CAMERA_UPDATE_STATE,
    LCD_CAMERA_CMD_QUEUE_STATE,
//  #endif

    LCD_HW_UPDATE_STATE,
    LCD_DC_UPDATE_STATE,
    LCD_HW_CMD_QUEUE_STATE,
    LCD_DC_CMD_QUEUE_STATE,
    LCD_WAIT_LAST_UPDATE_STATE,
//  #endif
//#endif
//add by WeiD

    MAX_LCD_OPERATION_STATE
}   LCD_OPERATION_STATE_ENUM;


#define LCD_LAYER0          0
#define LCD_LAYER1          1
#define LCD_LAYER2          2
#define LCD_LAYER3          3
#define LCD_LAYER4          4
#define LCD_LAYER5          5

#define LCD_GAMMA_TABLE0    0
#define LCD_GAMMA_TABLE1    1
#define LCD_GAMMA_TABLE2    2

#define LCD_LAYER0_ENABLE       0x80000000
#define LCD_LAYER1_ENABLE       0x40000000
#define LCD_LAYER2_ENABLE       0x20000000
#define LCD_LAYER3_ENABLE       0x10000000
#define LCD_LAYER4_ENABLE       0x08000000
#define LCD_LAYER5_ENABLE       0x04000000

#define LCD_UPDATE_MODULE_MMI               0
#define LCD_UPDATE_MODULE_MEDIA                1
#define LCD_UPDATE_MODULE_JPEG_VIDEO                  2
#define LCD_UPDATE_MODULE_EXT_CAMERA           3


#define LCD_CMD_COMPLETE_EVENT          0x00000004





#define LCD_SW_TRIGGER_MODE         0       /* LCD SW trigger with frame buffer */

/* wufasong added 2007.06.13 */
#define LCD_WM_CONT_MODE            0
#define LCD_WM_BLOCK_MODE           1


#define LCD_DRIVING_2MA 0
#define LCD_DRIVING_4MA 1
#define LCD_DRIVING_6MA 2
#define LCD_DRIVING_8MA 3

typedef struct
{
    kal_bool    layer_update_queue;     /* lcd layer parameter queue is full or not */
    kal_bool    source_key_enable;      /* enable/disable source key for specified layer */
    kal_bool        color_palette_enable;   /* enable/disable color palette for specified layer */
    kal_bool        opacity_enable;         /* enable/disable opacity for specified layer */
    kal_uint8   source_color_format;
    kal_uint8   color_palette_select;   /* selection of color palette table */
    kal_uint8   opacity_value;              /* opacity value for specified layer */
    kal_uint8   rotate_value;               /* rotate select for specified layer */
    kal_uint16  x_offset;                   /* x axis offset from main window for specified layer */
    kal_uint16  y_offset;                   /* y axis offset from main widnow for specified layer */
    kal_uint16  row_number;                 /* layer buffer height of specified layer */
    kal_uint16  column_number;              /* layer buffer width of specified layer */
    kal_uint32  source_key;                 /* source key color in RGB565 format for specified layer */
    kal_uint32  frame_buffer_address;   /* frame buffer start address of specified layer */
} lcd_layer_struct;

typedef struct
{
    kal_uint8 module_id;                /* module ID that request frame buffer update */
    kal_uint8 lcd_id;                   /* which lcd will be updated (MAIN_LCD or SUB_LCD) */
    kal_bool    block_mode_flag;        /* block/unblock upper layer AP or not */
    kal_uint8 fb_update_mode;       /* frame buffer update mode (SW_TRIGGER, HW_TRIGGER or DIRECT_COUPLE) */
    kal_uint16 lcm_start_x;         /* the start x axis of LCM to be updated*/
    kal_uint16 lcm_start_y;         /* the start y axis of LCM to be updated*/
    kal_uint16 lcm_end_x;           /* the end x axis of LCM to be updated*/
    kal_uint16 lcm_end_y;           /* the end y axis of LCM to be updated*/
    kal_uint16 roi_offset_x;        /* the ROI window offset x from main window */
    kal_uint16 roi_offset_y;        /* the ROI window offset y from main window */
    kal_uint32 update_layer;        /* the updated layer */
    kal_uint32 hw_update_layer; /* which layer will be applied by hw trigger or direct couple
 */
    kal_uint8   rotate_value;       /* rotate select for hardware update layer, used for MPEG4 decode
 */



    void (* lcd_block_mode_cb)(void);
}   lcd_frame_update_struct;

/* wufasong added 2007.06.13 the follow struct needed by media task's source code. */
typedef struct
{
    kal_uint8   fb_update_mode;     /* frame buffer update mode (SW_TRIGGER, HW_TRIGGER or DIRECT_COUPLE) */

    kal_uint8   block_mode;             /* block write out or not */
    kal_uint16  dest_block_width;       /* x pitch of block write operation */
    kal_uint32  dest_buffer_address;    /* the start address of desination buffer for LCD memory write out */
    kal_uint32  dest_buffer_size;
    kal_uint16  roi_offset_x;           /* x offset of interest area from dest buffer */
    kal_uint16  roi_offset_y;           /* y offset of interest area from dest buffer */
    kal_uint16  roi_width;              /* dest image width */
    kal_uint16  roi_height;             /* dest image height */
    kal_uint32  update_layer;
    kal_uint32  hw_update_layer;

    kal_uint8   hw_trigger_src;         /* LCD_HW_TRIGGER_IBW1 or LCD_HW_TRIGGER_IBW2 that will trigger LCD */
    kal_uint32  roi_background_color;   /* background color of memory otuput buffer */
    kal_uint8   memory_data_format;     /* output data format */
}   lcd_frame_update_to_mem_struct;

extern LCD_OPERATION_STATE_ENUM main_lcd_operation_state;
extern lcd_frame_update_struct main_lcd_fb_update_para;
#ifdef DUAL_LCD
extern LCD_OPERATION_STATE_ENUM sub_lcd_operation_state;
extern lcd_frame_update_struct sub_lcd_fb_update_para;
#endif
extern volatile kal_uint8 current_update_lcd;
extern kal_uint8    lcd_sleep_mode_handler;
extern kal_uint8 lcd_rotate_value;
extern kal_uint32 current_lcd_hw_update_layer;
extern kal_bool lcd_window_swap_flag;

#ifdef __LCD_ESD_RECOVERY__
kal_bool lcd_ESD_check(void);
#endif
kal_bool get_lcd_status(void);
void lcd_power_up(void);
void lcd_power_down(void);

void swap_lcd_layer_window(kal_uint32 swap_layer);
void set_lcd_layer_roate(kal_uint32 rotate_layer,kal_uint8 rotate_value);
void resume_lcd_layer_rotate(void);

kal_bool config_lcd_layer_window(kal_uint8 lcd_layer,lcd_layer_struct *layer_data);
kal_bool config_lcd_roi_window(kal_uint16 roi_offset_x, kal_uint16 roi_offset_y, kal_uint16 roi_column,
                               kal_uint16 roi_row);
void set_lcd_color_palette(kal_uint8 color_palette_select,kal_uint32 *color_palette_addr_ptr,
                           kal_uint8 start_index, kal_uint8 number_of_color);
void config_lcd_layer_offset(kal_uint8 lcd_layer, kal_uint16 layer_offset_x, kal_uint16 layer_offset_y);
void lcd_init(kal_uint8 lcd_id, kal_uint16 background_color);
void lcd_sleep_in(kal_uint8 lcd_id);
void lcd_sleep_out(kal_uint8 lcd_id);
void lcd_partial_on(kal_uint8 lcd_id, kal_uint16 start_line, kal_uint16 end_line);
void lcd_partial_off(kal_uint8 lcd_id);
kal_uint8 lcd_partial_display_align_line(kal_uint8 lcd_id);
void lcd_bright_level(kal_uint8 lcd_id, kal_uint8 bright_level);
void lcd_power_on(kal_uint8 lcd_id, kal_bool on);
void lcd_screen_on(kal_uint8 lcd_id, kal_bool on);
kal_uint8 lcd_get_parameter(kal_uint8 lcd_id,lcd_func_type type);
void lcd_get_size(kal_uint8 lcd_id, kal_uint16 *lcd_width, kal_uint16 *lcd_height);
void lcd_set_bias(kal_uint8 lcd_id, kal_uint8 *bias);
void lcd_set_contrast(kal_uint8 lcd_id, kal_uint8 *contrast);
void lcd_set_linerate(kal_uint8 lcd_id, kal_uint8 *linereate);
void lcd_set_linerate(kal_uint8 lcd_id, kal_uint8 *linereate);
void lcd_set_temp_compensate(kal_uint8 lcd_id, kal_uint8 *compensate);
void lcd_fb_update(lcd_frame_update_struct *lcd_para);
//void lcd_fb_update_to_memory(lcd_frame_update_to_mem_struct *lcd_para);
kal_uint32 get_lcd_frame_buffer_address(void);
kal_uint32 set_lcd_frame_buffer_address(kal_uint32 new_addr);
void reset_lcd_if(void);
void assert_lcd_fb_update(kal_uint8 lcd_id, kal_uint16 start_x, kal_uint16 start_y, kal_uint16 end_x,
                          kal_uint16 end_y,kal_uint16 roi_offset_x, kal_uint16 roi_offset_y, kal_uint32 update_layer);
void lcd_dma_slow_down(kal_uint8 level);
void lcd_dma_recovery(void);
void set_lcd_driving_current(kal_uint8 current);
void lcd_system_init(void);
#if defined(GDI_USING_LCD_ROTATE) || defined(GDI_USING_LAYER_ROTATE)

void lcd_set_rotate(kal_uint8 lcd_id, kal_uint8 rotate_value);
#endif

extern void lcd_lock_all_blt_operation(void);
extern void lcd_unlock_all_blt_operation(void);

#endif /* LCD_IF_H */

