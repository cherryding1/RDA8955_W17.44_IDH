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


#ifndef  _ANALOG_TV_H_
#define  _ANALOG_TV_H_

typedef struct
{
    UINT32 SrcWidth;
    UINT32 Srcheight;
    UINT32 colGrabStart;
    UINT32 colGrabEnd;
    UINT32 rowGrabStart;
    UINT32 rowGrabEnd;
    UINT32 ispOutputStartPos;
    UINT32 ispOutputWidth;
    UINT32 ispOutputheight;
} ATV_ISP_SETTING_T;

typedef enum
{
    /// Normal behaviour, no error
    ATVD_ERR_NO,

    /// The camera is busy
    ATVD_ERR_RESOURCE_BUSY,

    /// There is no camera device to open
    ATVD_ERR_NO_DEVICE,

    /// Unsupported parameter,
    ATVD_ERR_UNSUPPORTED_PARAM,

    /// Unsupported value
    ATVD_ERR_UNSUPPORTED_VALUE,

    ATVD_ERR_QTY
} ATVD_ERR_T;


// =============================================================================
// ATVD_FREQ_T
// -----------------------------------------------------------------------------
/// ATV audio stream sample rate
// =============================================================================
typedef enum
{
    ATVD_FREQ_8000HZ  =  0,
    ATVD_FREQ_11025HZ,
    ATVD_FREQ_12000HZ,
    ATVD_FREQ_16000HZ,
    ATVD_FREQ_22050HZ,
    ATVD_FREQ_24000HZ,
    ATVD_FREQ_32000HZ,
    ATVD_FREQ_44100HZ,
    ATVD_FREQ_48000HZ
}  ATVD_FREQ_T;

// ===========================================================
// ATVD_I2S_CFG_T
// -----------------------------------------------------------------------------
/// This type describes the initial values for #atvd_I2sOpen
// ============================================================
typedef struct
{

    /// If TRUE, FM is used as the slave
    BOOL                slave;

    /// Configure LRCK polarity.
    ///0 = high level on LRCK means right channel, low level means left.
    ///1 = high level on LRCK means left channel, low level means right.
    BOOL                polarity;

    ///0 = use normal sclk internally
    ///1 = inverted sclk internally
    BOOL                edge;

    /// I2S LRCK Frequency
    ATVD_FREQ_T          freq;

    ///1 = Output signed 16-bit audio data
    BOOL                dataSigned;

    ///1 = Invert LRCK when master
    BOOL                wsInv;

    ///1 = Invert SCLK when master
    BOOL                slckInv;

    ///1 = Left channel data delay 1T
    BOOL                leftDelay;

    ///1 = Right channel data delay 1T
    BOOL                rightDelay;

} ATVD_I2S_CFG_T;

// ---------------------------------------------------------------------------
// TV AREA DEFINISTIONS
// ---------------------------------------------------------------------------
typedef enum
{
    TV_AREA_CHINA         = 0,
    TV_AREA_CHINA_SHENZHEN,
    TV_AREA_CHINA_TAIWAN,
    TV_AREA_WESTERNEUROP,
    TV_AREA_AMERICA,
    TV_AREA_CHINA_MACAO,
    TV_AREA_INDIA,
    TV_AREA_PAKISTAN,
    TV_AREA_INDONESIA,
    TV_AREA_CHINA_HONGKONG,
    TV_AREA_YEMEN,  // 10
    TV_AREA_BAHRAIN,
    TV_AREA_ABU_DHABI,
    TV_AREA_KUWAIT,
    TV_AREA_THAI,
    TV_AREA_PORTUGAL,
    TV_AREA_SPAIN,
    TV_AREA_PHILIPINES,
    TV_AREA_MALAYSIA,
    TV_AREA_VIETNAM,
    TV_AREA_BRAZIL,  // 20
    TV_AREA_UK,
    TV_AREA_SOUTH_AFRICA,
    TV_AREA_TURKEY,
    TV_AREA_SINGAPORE,
    TV_AREA_CAMBODIA,
    TV_AREA_LAOS,
    TV_AREA_AFGHANISTA,
    TV_AREA_CANADA,
    TV_AREA_KOREA,
    TV_AREA_MEXICO,    // 30
    TV_AREA_CHILE,
    TV_AREA_VENEZUELA,
    TV_AREA_JAPAN,
    TV_AREA_ARGENTINA,
    TV_AREA_BURMA,     // 35
    TV_AREA_RUSSIA,

    TV_AREA_FACTORYMODE,
    TV_AREA_NUM
} ATVD_AREA_T;


typedef UINT16  *ANATV_IMAGE_T;

typedef VOID (*ANATV_IRQ_HANDLER_T)(UINT8 id, UINT8 ActiveBufNum);


PUBLIC ATVD_ERR_T atvd_Open(VOID);
PUBLIC ATVD_ERR_T atvd_Close(VOID);
PUBLIC uint8 atvd_Is625Lines(void);
PUBLIC ATVD_ERR_T atvd_SetTestPattern(void);
PUBLIC int16 atvd_GetRSSI(void);
PUBLIC uint32 atvd_LumaAdjust(uint8 nLevel);
PUBLIC uint32 atvd_ChromAdjust(uint8 nLevel);
PUBLIC uint32 atvd_SetVolume(uint8 nLevel);
PUBLIC ATVD_ERR_T atvd_I2sOpen(ATVD_I2S_CFG_T atvdI2sCfg);
PUBLIC ATVD_ERR_T atvd_I2sClose(void);
PUBLIC ATVD_ERR_T atvd_SetTVArea(uint32 cTVArea);
PUBLIC uint8 atvd_SetChannel(uint32 nChannel);
PUBLIC uint8 atvd_ScanChannel(uint32 nChannel);
PUBLIC uint8 atvd_GetSignalLevel(void);
PUBLIC uint8 atvd_GetTotalChannels(void);

#endif

