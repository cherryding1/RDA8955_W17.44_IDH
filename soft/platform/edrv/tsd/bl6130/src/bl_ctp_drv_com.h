/* Copyright Statement:
 *
 * (C) 2005-2016  All rights reserved.
 *
 * 
 */

#ifndef _BL_CTP_H_
#define _BL_CTP_H_
#include "bl_type_def.h"
#include <assert.h>

//#include "bl_bsp_ctp.h"
#define boolean  BOOL


#ifdef __cplusplus
extern "C"
{
#endif

#define CTP_MAX_POINTS                  2
#define CTP_PATTERN                     0xAA
#ifndef WIN32
#define     bl_log_trace kal_prompt_trace
#else
#define     bl_log_trace(log,...)
#endif


typedef enum 
{
    CTP_COMMAND_GET_VERSION,
    CTP_COMMAND_GET_CONFIG,
    CTP_COMMAND_LOAD_INT_CONFIG,
    CTP_COMMAND_LOAD_EXT_CONFIG,
    CTP_COMMAND_GET_DIFF_DATA,
    CTP_COMMAND_GET_FW_BUFFER,
    CTP_COMMAND_DO_FW_UPDATE
} ctp_control_command_enum ;


typedef struct 
{
    uint16_t resolution;    // CTP_RESOLTION,
    uint16_t threshold;     //  CTP_THRESHOLD,
    uint16_t report_interval;
    uint16_t idle_time_interval;
    uint16_t sleep_time_interval;
    uint16_t gesture_active_distance;
    uint16_t ms_calibration[128];
} ctp_parameters_struct;// ctp_get_set_enum

typedef struct 
{
    int8_t    vendor[8];
    int8_t    product[8];
    int8_t    firmware_version[8];
} ctp_custom_information_struct;

typedef enum 
{
	PEN_DOWN = 0,       /*0*/
	PEN_UP,             /*1*/
	PEN_MOVE,           /*2*/
	PEN_LONGTAP,        /*3*/
	PEN_REPEAT,         /*4*/
	PEN_ABORT,          /*5*/
	TP_UNKNOWN_EVENT,   /*6*/
	STROKE_MOVE,        /*7*/
	STROKE_STATICAL,    /*8*/
	STROKE_HOLD,        /*9*/
	PEN_LONGTAP_HOLD,   /*10*/
	PEN_REPEAT_HOLD,    /*11*/
	STROKE_DOWN_,       /*12*/
	STROKE_LONGTAP_,    /*13*/
	STROKE_UP_,         /*14*/
	STROKE_DOWN = 0xc0,     /*0*/ 
	STROKE_LONGTAP = 0x7e, /*8*/
	STROKE_UP = 0x7f        /*127*/
} touch_panel_event_enum;

typedef struct 
{
	// Touch_Panel_Event_enum
	touch_panel_event_enum event;
	/*coordinate point, not diff*/
	uint16_t x;
	uint16_t y;
	uint8_t weight;
	uint8_t area;
} tp_single_event;

typedef struct 
{
	uint16_t         model;     // Single/Dual/Triple/Four/Five/All gesture
	uint16_t         padding;   //currently use for check the structure format correctness, 0xAA
	uint32_t         time_stamp;
	tp_single_event  points[CTP_MAX_POINTS];
}TouchPanelMultipleEventStruct;


typedef enum 
{
    CTP_ACTIVE_MODE = 0x4B,
    CTP_IDLE_MODE = 0x08,
    CTP_SLEEP_MODE = 0x10,
	CTP_NORMAL_OPERATION = 0x0B,
	CTP_SYSTEM_INFORMATION = 0x1B,
	CTP_GESTURE_DETECTION_MODE = 0x40,
	CTP_MULTIPLE_POINT_MODE = 0x80,
	CTP_DEBUG_MODE,
    CTP_FIRMWARE_UPDATE,
    CTP_FM_ENABLE,
    CTP_FM_DISABLE,
} ctp_device_mode_enum;

typedef enum 
{
    CTP_UP   = 0,
    CTP_DOWN,
} ctp_pen_state_enum;

typedef struct 
{
	boolean (*ctp_init)(void);
    boolean (*ctp_set_device_mode)(ctp_device_mode_enum);
	ctp_pen_state_enum (*ctp_hisr)(void);
	boolean (*ctp_get_data)(TouchPanelMultipleEventStruct *);
	boolean (*ctp_parameters)(ctp_parameters_struct *, uint32_t, uint32_t);
    void (*ctp_power_on)(boolean);
	uint32_t (*ctp_command)(ctp_control_command_enum, void*, void*);
} ctp_customize_function_struct;

/******** funtion extern **************/
extern void ctp_delay_ms(uint16_t delay);
void ctp_delay_us(uint32_t time);

#ifdef __cplusplus
}
#endif


#endif /*_BL_CTP_H_*/

