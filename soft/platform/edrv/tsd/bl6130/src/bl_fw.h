#ifndef _BL_FW_H_
#define _BL_FW_H_

extern unsigned char fwbin[];

#define FW_SIZE 0x7600
#define BIN_CHECKSUM 0xfffe
#define BL5460 0xFF
#define BL5461 0x01
#define BL5463 0x02
#define BL6XX0 0x20
#define CHIP_TYPE BL6XX0 //chip_id
#define FW_VERSION 8 //fw_version
#define AR_VERSION 9 //argument_version
#endif