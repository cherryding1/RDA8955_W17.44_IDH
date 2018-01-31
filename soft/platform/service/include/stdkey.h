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























#ifndef __STDKEY_H__
#define __STDKEY_H__

//
// Standard key definition:
//
#define SK_NONE              0x00

#define SK_BACKSPACE         0x01
#define SK_SPACE             0x02
#define SK_ESCAPE            0x03
#define SK_MENU              0x04

#define SK_HOME              0x10
#define SK_END               0x11
#define SK_INSERT            0x12
#define SK_DELETE            0x13 //Delete key   
#define SK_PAGEUP            0x14
#define SK_PAGEDOWN          0x15

#define SK_UP                0x16 //UP arrow key   
#define SK_DOWN              0x17 //DOWN arrow key   
#define SK_LEFT              0x18 //LEFT arrow key   
#define SK_RIGHT             0x19 //RIGHT arrow key   
#define SK_SELECT            0x1A //Select/action key, enters a carriage return in multiple-line fields by pressing the Select key.   

#define SK_CTL_LEFT          0x20 //Left Soft control key   
#define SK_CTL_RIGHT         0x21 //Right Soft control key

#define SK_PHONE_SEND        0x30
#define SK_PHONE_END         0x31

#define SK_CONTRAST_DEC      0x40
#define SK_CONTRAST_INC      0x41
#define SK_VOLUME_DEC        0x42
#define SK_VOLUME_INC        0x43
#define SK_BRIGHTNESS_DEC    0x44
#define SK_BRIGHTNESS_INC    0x45
#define SK_BACKLIGHT_ON      0x46
#define SK_BACKLIGHT_OFF     0x47
#define SK_DICTAPHONE_PLAY   0x48
#define SK_DICTAPHONE_STOP   0x49
#define SK_DICTAPHONE_RECORD 0x4A
#define SK_POWER_OFF         0x4B

#define SK_NUM0              0x50
#define SK_NUM1              0x51
#define SK_NUM2              0x52
#define SK_NUM3              0x53
#define SK_NUM4              0x54
#define SK_NUM5              0x55
#define SK_NUM6              0x56
#define SK_NUM7              0x57
#define SK_NUM8              0x58
#define SK_NUM9              0x59
#define SK_STAR              0x5A //* key
#define SK_HASH              0x5B //# key

#define SK_APP1              0x61
#define SK_APP2              0x62
#define SK_APP3              0x63
#define SK_APP4              0x64
#define SK_APP5              0x65
#define SK_APP6              0x66
#define SK_APP7              0x67
#define SK_APP8              0x68
#define SK_APP9              0x69
#define SK_APP10             0x6A
#define SK_APP11             0x6B
#define SK_APP12             0x6C
#define SK_APP13             0x6D
#define SK_APP14             0x6E
#define SK_APP15             0x6F

#define SK_ALL               0xFF


// FIXME Those two macros are not used
// and conflict within the MMI --> beheaded
//#define KEY_NORMAL_PRESS     0
//#define KEY_LONG_PRESS       1
// Merged from MMI firmware headers
#define SK_LONG_PRESS       1  //simon add
#define MAX_KEYS_NUM    44  // simon add
#endif // _H_

