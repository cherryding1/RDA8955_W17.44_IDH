#ifndef    _FW_UPDATE_H_
#define    _FW_UPDATE_H_

#include   "bl_fw.h"

#define		PJ_ID_OFFSET		44
#define     ARG_ID_OFFSET       43
#define     FW_VER_ID_OFFSET    42
#define     BIN_MAX_SIZE        0x8000



enum BL_FLASH_CMD 
{	
	ERASE_SECTOR_MAIN_CMD	= 0X06,	
	ERASE_ALL_MAIN_CMD	= 0X09,	
	RW_REGISTER_CMD		= 0X0a,
	READ_MAIN_CMD		= 0X0D,
	WRITE_MAIN_CMD		= 0X0F,	
	WRITE_RAM_CMD		= 0X11,	
	READ_RAM_CMD		= 0X12,
};

enum FW_REG
{
	WORK_MODE_REG		= 0X00,	
	CHECKSUM_REG		= 0x3f,
	CHECKSUM_CAL_REG	= 0x8a,
	AC_REG			    = 0X8b,
	RESOLUTION_REG		= 0X98,
	LPM_REG			    = 0Xa5,
	PROXIMITY_REG		= 0Xb0,
	PROXIMITY_FLAG_REG	= 0Xb1,
	PJ_ID_REG		    = 0Xb6,	
	TP_ID_REG           = 0x33,
};

enum checksum
{
	CHECKSUM_READY		= 0x01,
	CHECKSUM_CAL		= 0xaa,
};

enum  ARRAY_DATA_TYPE
{
    BIN_ARG_TYPE = 0x00,
	BIN_TYPE,
	ARG_TYPE,
} ;

enum DOWN_TYPE
{
	DOWN_LOAD_TYPE,
	UPDATE_ARG_TYPE,
	ERASE_TYPE,
};


uint8_t  bl_update_fw(void);
uint8_t  bl_update_argument(uint8_t type,  uint16_t offset, uint8_t arg_order);
uint8_t  bl_get_flash_checksum(uint16_t *flash_checksum);
uint8_t  bl_get_bin_checksum(uint8_t type, uint16_t offset, uint8_t arg_order, uint16_t* p_bin_checksum);
uint8_t  bl_download_flash(uint8_t type, uint16_t offset, uint8_t arg_order);

#endif



