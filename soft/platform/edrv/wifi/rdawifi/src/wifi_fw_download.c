
/*
 * Copyright (c) 2014 Rdamicro Corporation
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
/* 91h, setup chip */

#define DECRYPT_FIRMWARE_RC4

enum HOST_DECRYPT_TYPE_T  //HCMD_DECRYPT_TYPE
{
    HOST_DECRYPT_RC4 = 1,
    HOST_DECRYPT_AES_CBC
};


#include "cs_types.h"
#include "string.h"

#include "sdio_config.h"
#include "wifi_config.h"
#include "edrvp_debug.h"
#include "sdio_debug.h"
extern VOID sxr_Sleep (UINT32 Period);

#define ETH_ALEN  6
u8 mac_addr[ETH_ALEN] ={0x0, 0x50,0xc2, 0x5e,0x10,0x66};

s32 wifi_start_scan_set(char *scan_ssid);

extern int firmware_wid_request(UINT8 *wid_req, unsigned short wid_req_len);
extern int sdio_getInputData(void);

/* ERP Protection type*/
typedef enum {
	G_SELF_CTS_PROT,
	G_RTS_CTS_PROT,
	NUM_G_PROTECTION_MODE
} G_PROTECTION_MODE;

typedef enum {
	B_ONLY_MODE = 0,
	G_ONLY_MODE,
	G_MIXED_11B_1_MODE,
	G_MIXED_11B_2_MODE,
	NUM_G_OPERATING_MODE
} G_OPERATING_MODE_T;

/*
 *  @user set ps mode for save power
 */
enum USER_PS_MODE_T {
	NO_POWERSAVE = 0,
	MIN_FAST_PS = 1,
	MAX_FAST_PS = 2,
	MIN_PSPOLL_PS = 3,
	MAX_PSPOLL_PS = 4
};

#define	FMW_HEADER_LEN          4
#define FMW_TYPE_MASK        0xF0	/* Firmware Host Type Mask */
#define FMWID_HEADER_LEN        7
static inline void put_le16(u8 *p, u16 v)
{
	p[0] = (u8)(v&0x000000ff);
	p[1] = (u8)((v >> 8)&0x000000ff);
}

static inline u16 get_le16(const u8 *p)
{
	return p[0] | (p[1] << 8);
}

static inline u32 get_le32(const u8 *p)
{
	return p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
}

static inline void put_le32(u8 *p, u32 v)
{
	p[0] = (u8)(v&0x000000ff);
	p[1] = (u8)((v >> 8)&0x000000ff);
	p[2] = (u8)((v >> 16)&0x000000ff);
	p[3] = (u8)((v >> 24)&0x000000ff);
}

static inline u32 get_be32(const u8 *p)
{
	return p[3] | (p[2] << 8) | (p[1] << 16) | (p[0] << 24);
}
static inline void put_be32(u8 *p, u32 v)
{
	p[3] = (u8)(v&0x000000ff);
	p[2] = (u8)((v >> 8)&0x000000ff);
	p[1] = (u8)((v >> 16)&0x000000ff);
	p[0] = (u8)((v >> 24)&0x000000ff);
}

/*
 * fill wid struct
 * return the length filled or -1
 *@buf buf to fill
 *@WID WID
 *@data the data of @WID, couldn't be NULL. WID_CHAR(&(u8 a)), WID_INT(&(u32 a)), WID_BIN(u8 *data)
 *@data_len length of the WID. WID_CHAR(1), WID_SHORT(2),  WID_INT(4),  WID_STR/BIN(n)
 *@is_query true:query_wid; query_wid @data @data_len could be NULL
 */
s32 wland_push_wid(u8 *buf, u16 WID, const void *data, u16 data_len, bool is_query)
{
	u8* offset = buf;
	WID_T type = wland_get_wid_type(WID);
	u16 wid_length = wland_get_wid_size(type, data_len);

	u8 checksum = 0;

	if (buf==NULL || (!is_query && data==NULL)) {
		MCD_TRACE(MCD_INFO_TRC, 0, "buf%p or data:%p NULL\n", buf, data);
		return -1;
	}

	if (wid_length<0 || (!is_query && wid_length!=data_len)) {
		MCD_TRACE(MCD_INFO_TRC, 0, "WID(0x%04x type:%d) data_len error. wid_length:%d data_len:%d\n",
			WID, type, wid_length, data_len);
		return -1;
	}

	//MCD_TRACE(MCD_INFO_TRC, 0, "WID:0x%04x type:%d wid_length:%d\n",
	//	WID, type, wid_length);

	put_le16(buf, WID);
	buf += 2;

	//query wid doesn't have length and data
	if (is_query)
		return buf-offset;

	switch (type) {
	case WID_CHAR:
		*buf = (u8)wid_length;
		*(buf+1) = *(u8 *)data;
		buf += 2;
		break;
	case WID_SHORT:
		*buf = (u8)wid_length;
		put_le16(buf+1, *(u16 *)data);//*(u16 *)(buf+1) = (u16)value;
		buf += 3;
		break;
	case WID_INT:
		*buf = (u8)wid_length;
		put_le32(buf+1, *(u32 *)data);//*(u32 *)(buf+1) = (u32)value;
		buf += 5;
		break;
	case WID_STR:
		*buf = (u8)wid_length;
		memcpy((char *)(buf+1), data, wid_length);
		buf += (1+wid_length);
		break;
	case WID_BIN:
		//MCD_TRACE(MCD_INFO_TRC, 0, " WID Binary, %08x\n", buf);
		put_le16(buf, wid_length);
		memcpy(buf+2, data, wid_length);
		while(data_len--)
			checksum += *(buf+2+data_len);
		*(buf+2+wid_length) = checksum;
		buf += (2+1+wid_length);
		//MCD_TRACE(MCD_INFO_TRC, 0, " WID Binary, %08x, wid_length is %d\n", buf, wid_length);
		break;
	case WID_UNKNOWN:
	default:
		return -1;
	}
	//MCD_TRACE(MCD_INFO_TRC, 0, "Done:%d\n", buf - offset);
	return buf - offset;
}

/*
 * analysis wid struct
 * return the length analysised or -1
 *@buf buf to analysis
 *@WID return WID, must be !NULL
 *@data buf to return data. must be !NULL
 *@data_len return data_len. must be !NULL
 */
s32 wland_pull_wid(const u8 *buf, u16 buf_len, u16 *WID, const u8 **data, u16 *data_len)
{
	const u8 *offset = buf;
	WID_T type;
	u16 wid_length;
	u8 checksum = 0;

	if (buf==NULL || WID==NULL || data==NULL || data_len==NULL) {
		MCD_TRACE(MCD_INFO_TRC, 0, "input paramters error\n");
		return -1;
	}
	*WID = *data_len = 0;
	*data = NULL;

	*WID = get_le16(buf);
	buf += 2;
	type = wland_get_wid_type(*WID);

	if (buf > offset + buf_len) {
		MCD_TRACE(MCD_INFO_TRC, 0, "data outof buf_len\n");
		return (offset+buf_len) - buf;
	} else if (buf == offset + buf_len)
		return buf_len;

	MCD_TRACE(MCD_INFO_TRC, 0, "WID:0x%04x type:%d\n",*WID, type);

	switch (type) {
	case WID_CHAR:
		wid_length = (u16)(*buf);
 		if (wid_length != 1) {
			MCD_TRACE(MCD_INFO_TRC, 0, "WID(0x%04x) dlength(%d) \n",*WID, wid_length);
			return -1;
		}
		*data_len = wid_length;
		*data = buf + 1;
		buf += 2;
		break;
	case WID_SHORT:
		wid_length = (u16)(*buf);
 		if (wid_length != 1) {
			MCD_TRACE(MCD_INFO_TRC, 0, "WID(0x%04x) dlength(%d)\n",*WID, wid_length);
			return -1;
		}
		*data_len = wid_length;
		*data = buf + 1;
		buf += 3;
		break;
	case WID_INT:
		wid_length = (u16)(*buf);
		*data_len = wid_length;
 		if (wid_length != 4) {
			MCD_TRACE(MCD_INFO_TRC, 0, "WID(0x%04x) dlength(%d)\n",*WID, wid_length);
			return -1;
		}
		*data = buf + 1;
		buf += 5;
		break;
	case WID_STR:
		wid_length = (u16)(*buf);
		*data_len = wid_length;
		*data = buf+1;
		//memcpy(data, buf+1, wid_length);
		buf += (1+wid_length);
		break;
	case WID_BIN:
		//buf += 2;//only for binary_wid. word alignment
		//wid_length = get_le16(buf);
		wid_length = get_le16(buf) & 0x3fff;
		*data_len = wid_length;
		*data = buf+2;
		MCD_TRACE(MCD_INFO_TRC, 0, "%d:%d\n", buf-offset+2+1+wid_length, buf_len);
		if (buf-offset+2+1+wid_length > buf_len) {
			MCD_TRACE(MCD_INFO_TRC, 0, "data outof buf_len\n");
			return (buf_len - (buf-offset+2+1+wid_length));
		}

		while(wid_length--)
			checksum += *(buf+2+wid_length);
		wid_length = *data_len;
		if ( *(buf+2+wid_length) != checksum)  {
			MCD_TRACE(MCD_INFO_TRC, 0, "BIN WID(0x%04x) checksum error. fw (%u) checksum(%u)\n",
				*WID, *(buf+2+wid_length), checksum);
			return -1;
		}
		buf += (2+1+wid_length);
		break;
	case WID_UNKNOWN:
	default:
		return -1;
	}

	if (buf > offset + buf_len) {
		MCD_TRACE(MCD_INFO_TRC, 0, "data outof buf_len\n");
		return (offset+buf_len) - buf;
	} else
		return buf - offset;
}

static UINT8 wid_buf_0[1536];
UINT8 *wid_buf = wid_buf_0 + 2;

struct wland_dcmd {
	u8 wid_msg_type;	/*
				 *'Q'  -- request  for wid query
				 *'W'  -- request  for wid write
				 *'R'  -- response for write or query
				 *'I'  -- mac status indication
				 *'N'  -- network info: scan AP list
				 */

	u8 wid_msg_id;		/* message id     */
	u16 wid_msg_length;	/* message length */
};
static u8 msg_id = 0;

#if defined DECRYPT_FIRMWARE_RC4 || defined DECRYPT_FIRMWARE_AES
static s32 wifi_SdioSetDecrypt(
	u32 addr, u32 len, u8 method)
{
	UINT8 *buf_start = wid_buf + 4;

	UINT8 *buf = buf_start;
	struct wland_dcmd *msg = (struct wland_dcmd *)wid_buf;
	s32 ret = 0;
	u16 data_len;


	MCD_TRACE(MCD_INFO_TRC, 0, "Enter wifi_SdioSetDecrypt(), method %d\n",method );


	ret = wland_push_wid(buf, WID_MEMORY_ADDRESS, &addr, sizeof(addr),0 );
	if (ret < 0) {
		MCD_TRACE(MCD_INFO_TRC, 0, "put wid error\n");
		return -1;
	}
	buf += ret;

	ret = wland_push_wid(buf, WID_MEMORY_LENGTH, &len, sizeof(len), 0);
	if (ret < 0) {
		MCD_TRACE(MCD_INFO_TRC, 0, "put wid error\n");
		return -1;
	}
	buf += ret;

	ret = wland_push_wid(buf, WID_BOOTROM_DECRYPT, &method, sizeof(method), 0);
	if (ret < 0) {
		MCD_TRACE(MCD_INFO_TRC, 0, "put wid error\n");
		return -1;
	}
	buf += ret;

	MCD_TRACE(MCD_INFO_TRC, 0, "valid data length in wid_buf is %d\n", (int)(buf - wid_buf));


	data_len = buf - buf_start;

	msg->wid_msg_id = 0;
	msg->wid_msg_type = 0x57;
	msg->wid_msg_length = data_len + 4;

	ret = firmware_wid_request(wid_buf_0, data_len + FMW_HEADER_LEN);
	if (ret < 0)
		MCD_TRACE(MCD_INFO_TRC, 0, "WID Result Failed\n");

	MCD_TRACE(MCD_INFO_TRC, 0, "firmware_wid_request for decrypt Done(err:%d)\n", ret);
	sdio_getInputData();

//	mutex_unlock(&drvr->proto_block);

	return ret;
}
#endif

extern UINT8 localMac[6] ;

static int wland_preinit_cmds_91h(u8 *pMac)
{

	UINT8 *buf_start = wid_buf + 4;

	UINT8 *buf = buf_start;
	struct wland_dcmd *msg = (struct wland_dcmd *)wid_buf;
	s32 ret = 0;
	u8  val = 0;
	u16 data_len;

	MCD_TRACE(MCD_INFO_TRC, 0, "Enter  wland_preinit_cmds_91h()\n");

	ret = wland_push_wid(buf, WID_MAC_ADDR, pMac, ETH_ALEN, 0);
	if (ret < 0) {
		MCD_TRACE(MCD_INFO_TRC, 0, "put wid error\n");
		return -1;
	}
	buf += ret;
/*	action.category = 0x07;
	action.action = 0x02;
	memset(action.bssid, 0, ETH_ALEN);
	action.tid = 0x00;
	action.max_msdu = 0x10;
	action.ack_policy = 0x00;
	action.ba_policy = 0x01;
	action.buff_size = cpu_to_be16(0x10);
	action.ba_timeout = cpu_to_be16(0x00);
	action.add_ba_timeout = cpu_to_be16(0x00);

	re = wland_push_wid(buf, WID_11N_P_ACTION_REQ, &action, sizeof(action), 0);
	if (ret < 0) {
		WLAND_ERR("put wid error\n");
		goto done;
	}
	buf += ret;
*/
	val = 1;
	ret = wland_push_wid(buf, WID_QOS_ENABLE, &val, sizeof(val), 0);
	if (ret < 0) {
		MCD_TRACE(MCD_INFO_TRC, 0, "put wid error\n");
		goto done;
	}
	buf += ret;


	val = 1;
	ret = wland_push_wid(buf, WID_11N_IMMEDIATE_BA_ENABLED, &val, sizeof(val), 0);
	if (ret < 0) {
		MCD_TRACE(MCD_INFO_TRC, 0, "put wid error\n");
		goto done;
	}
	buf += ret;


	val = WIFI_LISTEN_INTERVAL;
	ret = wland_push_wid(buf, WID_LISTEN_INTERVAL, &val, sizeof(val), 0);
	if (ret < 0) {
		MCD_TRACE(MCD_INFO_TRC, 0, "put wid error\n");
		goto done;
	}
	buf += ret;

	val = WIFI_LINK_LOSS_THRESHOLD_91H;
	ret = wland_push_wid(buf, WID_LINK_LOSS_THRESHOLD, &val, sizeof(val), 0);
	if (ret < 0) {
		MCD_TRACE(MCD_INFO_TRC, 0, "put wid error\n");
		goto done;
	}
	buf += ret;

	val = 0;
	ret = wland_push_wid(buf, WID_QOS_ENABLE, &val, sizeof(val), 0);
	if (ret < 0) {
		MCD_TRACE(MCD_INFO_TRC, 0, "put wid error\n");
		goto done;
	}
	buf += ret;

	val = 0;
	ret = wland_push_wid(buf, WID_11N_IMMEDIATE_BA_ENABLED, &val, sizeof(val), 0);
	if (ret < 0) {
		MCD_TRACE(MCD_INFO_TRC, 0, "put wid error\n");
		goto done;
	}
	buf += ret;

	val = 0;
	ret = wland_push_wid(buf, WID_11N_ENABLE, &val, sizeof(val), 0);
	if (ret < 0) {
		MCD_TRACE(MCD_INFO_TRC, 0, "put wid error\n");
		goto done;
	}
	buf += ret;

	val = 0;
	ret = wland_push_wid(buf, WID_2040_ENABLE, &val, sizeof(val), 0);
	if (ret < 0) {
		MCD_TRACE(MCD_INFO_TRC, 0, "put wid error\n");
		goto done;
	}
	buf += ret;

#if 0
	val = 1;
	ret = wland_push_wid(buf, WID_11N_SHORT_GI_ENABLE, &val, sizeof(val), 0);
	if (ret < 0) {
		MCD_TRACE(MCD_INFO_TRC, 0, "put wid error\n");
		goto done;
	}
	buf += ret;


	val = 1;
	ret = wland_push_wid(buf, WID_RIFS_MODE, &val, sizeof(val), 0);
	if (ret < 0) {
		MCD_TRACE(MCD_INFO_TRC, 0, "put wid error\n");
		goto done;
	}
	buf += ret;

	val = G_RTS_CTS_PROT;
	ret = wland_push_wid(buf, WID_11N_ERP_PROT_TYPE, &val, sizeof(val), 0);
	if (ret < 0) {
		MCD_TRACE(MCD_INFO_TRC, 0, "put wid error\n");
		goto done;
	}
	buf += ret;

	val = G_MIXED_11B_2_MODE;
	ret = wland_push_wid(buf, WID_11G_OPERATING_MODE, &val, sizeof(val), 0);
	if (ret < 0) {
		MCD_TRACE(MCD_INFO_TRC, 0, "put wid error\n");
		goto done;
	}
	buf += ret;

	val = 0;
	ret = wland_push_wid(buf, WID_11N_AMPDU_TID, &val, sizeof(val), 0);
	if (ret < 0) {
		MCD_TRACE(MCD_INFO_TRC, 0, "put wid error\n");
		goto done;
	}
	buf += ret;

#endif
#if 0
	val = 1;
	ret = wland_push_wid(buf, WID_2040_ENABLE, &val, sizeof(val), 0);
	if (ret < 0) {
		WLAND_ERR("put wid error\n");
		goto done;
	}
	buf += ret;
#endif

#ifdef WLAND_POWER_MANAGER
	if (ifp->drvr->sleep_flags & WLAND_SLEEP_ENABLE) {
		val = MAX_FAST_PS;
		ret = wland_push_wid(buf, WID_POWER_MANAGEMENT, &val, sizeof(val), 0);
		if (ret < 0) {
			MCD_TRACE(MCD_INFO_TRC, 0, "put wid error\n");
			goto done;
		}
		buf += ret;

		/*
		if (ifp->drvr->sleep_flags & WLAND_SLEEP_PREASSO) {
			u32Val = WIFI_PREASSO_SLEEP;
			err = wland_fil_set_cmd_data(ifp, WID_PREASSO_SLEEP, &u32Val,
				sizeof(u32Val));
			if (err < 0)
				goto done;
		}
		val = 1;
		sdio_claim_host(sdiodev->func);
		err = wland_sdioh_request_byte(sdiodev, SDIOH_WRITE,
			URSDIO_FUNC1_INT_TO_DEVICE, &val);
		sdio_release_host(sdiodev->func);
		if (err) {
			WLAND_ERR("Write URSDIO_FUNC1_INT_TO_DEVICE failed!\n");
		}
		*/
	}
#endif /* WLAND_POWER_MANAGER */

	data_len = buf - buf_start;

	msg->wid_msg_id = 0;
	msg->wid_msg_type = 0x57;
	msg->wid_msg_length = data_len + 4;

    	sxs_Dump(MCD_INFO_TRC, 0,wid_buf, (int)(data_len + FMW_HEADER_LEN));

	MCD_TRACE(MCD_INFO_TRC, 0, " preinit valid data length wid_buf is %d\n", (int)(buf - wid_buf));
	ret = firmware_wid_request(wid_buf_0, data_len + FMW_HEADER_LEN);
	if (ret < 0)
		MCD_TRACE(MCD_INFO_TRC, 0, "preinit 5991h WID Result Failed\n");

	//sdio_getInputData();
done:
//	mutex_unlock(&drvr->proto_block);
	MCD_TRACE(MCD_INFO_TRC, 0, " preinit 5991h Done(err:%d)\n", ret);
	return ret;
}

static s32 wifi_SdioSendBulk(const void *buffer, u32 buflen , u32 addr )
{
	unsigned int ret;
	UINT8 *buf_start = wid_buf + 4;

	UINT8 *buf = buf_start;
	struct wland_dcmd *msg = (struct wland_dcmd *)wid_buf;
	u16 data_len;
	MCD_TRACE(MCD_INFO_TRC, 0, "Enter, addr:%x, buflen is %d\n", addr, buflen);

//	memset(wid_buf, 0 , sizeof(wid_buf));

	ret = wland_push_wid(buf, WID_MEMORY_ADDRESS, &addr, sizeof(addr), 0);
	if (ret < 0) {
		MCD_TRACE(MCD_INFO_TRC, 0, "put wid error\n");
		return -1;
	}
	buf += ret;

	ret = wland_push_wid(buf, WID_MEMORY_LENGTH, &buflen, sizeof(u32), 0);
	if (ret < 0) {
		MCD_TRACE(MCD_INFO_TRC, 0, "put wid error\n");
		return -1;
	}
	buf += ret;

	ret = wland_push_wid(buf, WID_MEMORY_ACCESS, buffer, buflen, 0);
	if (ret < 0) {
		MCD_TRACE(MCD_INFO_TRC, 0, "put wid error\n");
		return -1;
	}
	buf += ret;
	data_len = buf - buf_start;

	msg->wid_msg_id = msg_id++;
	msg->wid_msg_type = 0x57;
	msg->wid_msg_length = data_len + 4;

  //  	sxs_Dump(MCD_INFO_TRC, 0,wid_buf, (int)(buf-wid_buf));

	MCD_TRACE(MCD_INFO_TRC, 0, "valid data length in wid_buf is %d\n", (int)(buf - wid_buf));
	ret = firmware_wid_request(wid_buf_0, data_len + FMW_HEADER_LEN);
	if (ret < 0)
		MCD_TRACE(MCD_INFO_TRC, 0, "WID Result Failed\n");

	MCD_TRACE(MCD_INFO_TRC, 0, "write FW msg ID is %d\n", msg_id);

//	if (msg_id >= 210)
	sdio_getInputData();

	MCD_TRACE(MCD_INFO_TRC, 0, "firmware_wid_request() Done(err:%d)\n", ret);
	return ret;
}

static int wifi_SdioWriteFirmware(
	const char *pBuf, int size , u32 addr)
{
	s32 err = 0;
	u32 sent, dllen;
	u32 sendlen;
	const void *dlpos;

	///TODO: The 10 length??
//	u16 bulkchunk = (1536 - 25 -1024 );
	u16 bulkchunk = (1536 - 25 );
	bulkchunk = bulkchunk-(bulkchunk%4);
		//1536 - sizeof(struct wland_dcmd) -
		 //sizeof(struct wland_wid_cmd) - 10 - 8;
msg_id = 0;

	dlpos = (void *)pBuf;
	dllen = size;
//	dllen = 400;

	MCD_TRACE(MCD_INFO_TRC, 0, "Enter, data_addr:%p,data_len:%u\n",
		dlpos, dllen);
	sent = 0;

	while (sent != dllen) {
		if (dllen-sent < bulkchunk)
			sendlen = dllen-sent;
		else
			sendlen = bulkchunk;
#ifdef debug_time
		{
			struct timespec t;
			jiffies_to_timespec(jiffies,&t);
			MCD_TRACE(MCD_INFO_TRC, 0, "%lu, %lu\n", t.tv_sec, t.tv_nsec);
		}
#endif
		MCD_TRACE(MCD_INFO_TRC, 0, "send one pkt, addr:0x%08x, dlpos:%p, sendlen:%u(all:%u)\n",
			addr, dlpos, sent, dllen);
		if (wifi_SdioSendBulk(dlpos, sendlen, addr) < 0) {
			MCD_TRACE(MCD_INFO_TRC, 0, "wland_sdio_send_bulk failed");
			err = -1;
			break;
		}
//		return err;
		addr += sendlen;
		dlpos += sendlen;
		sent += sendlen;
	}

	MCD_TRACE(MCD_INFO_TRC, 0, "Done(err=%d)\n", err);
	return err;
}


static int wifi_SdioDownloadImage(
	const char *pBuf, int size , u32 addr)
{
	s32 ret = -1;
#if defined(CHECK_FIRMWARE_CRC32) || defined(CHECK_FIRMWARE_MD5) || defined(CHECK_FIRMWARE_SHA1)
	u8 result[20] = {0};
#endif
#ifdef CHECK_FIRMWARE_MD5
	u8 md5_result[MD5_DIGEST_LENGTH];
#endif
#ifdef CHECK_FIRMWARE_SHA1
	u8 sha1_result[SHA1_DIGEST_SIZE];
#endif
        MCD_TRACE(MCD_INFO_TRC, 0,  " wifi_SdioDownloadImage  entered \n");

	ret = wifi_SdioWriteFirmware(pBuf, size, addr);
	if (ret < 0) {
		MCD_TRACE(MCD_INFO_TRC, 0,  "wland_sdio_write_firmware failed!\n");
		goto err;
	}

//	goto err;

#ifdef CHECK_FIRMWARE_CRC32
	WLAND_DBG(TRAP, INFO, "check firmware with crc32\n");
	ret = wland_sdio_check_firmware(ifp, addr,
		fw_entry->size, HOST_CHECKSUM_CRC, result, CRC32_DIGEST_SIZE);
	if (ret < 0) {
		ret = -1;
		WLAND_ERR("crc32 check firmware failed\n");
		goto err;
	} else if(*(u32 *)result != crc32(0xffffffff, fw_entry->data, fw_entry->size)) {
		ret = -2;
		WLAND_ERR("crc32 check result error.fw ret:%x, driver ret:%x\n",
			*(u32 *)result, crc32(0xffffffff, fw_entry->data, fw_entry->size));
		goto err;
	}
#endif

#ifdef CHECK_FIRMWARE_MD5
	WLAND_DBG(TRAP, INFO, "check firmware with md5. result:%p\n", result);
	ret = wland_sdio_check_firmware(ifp, addr,
		fw_entry->size, HOST_CHECKSUM_MD5, result, MD5_DIGEST_LENGTH);
	if (ret < 0) {
		ret = -1;
		WLAND_ERR("md5 check firmware failed\n");
		goto err;
	}

	if (wland_hash("md5", fw_entry->data, fw_entry->size, md5_result) < 0) {
		ret = -1;
		WLAND_ERR("count md5 failed\n");
		goto err;
	} else if(memcmp(result, md5_result, MD5_DIGEST_LENGTH)) {
		ret = -2;
		WLAND_ERR("md5 check result error. return result:%08x %08x %08x %08x"
			"my_result:%08x %08x %08x %08x\n",
			*(u32*)(result+0), *(u32*)(result+4), *(u32*)(result+8),*(u32*)(result+12),
			*(u32*)(md5_result+0), *(u32*)(md5_result+4), *(u32*)(md5_result+8),
			*(u32*)(md5_result+12));
		goto err;
	}
#endif

#ifdef CHECK_FIRMWARE_SHA1
		WLAND_DBG(TRAP, INFO, "check firmware with sh1\n");
		ret = wland_sdio_check_firmware(ifp, addr,
			fw_entry->size, HOST_CHECKSUM_SHA1, result, SHA1_DIGEST_SIZE);
		if (ret < 0) {
			ret = -1;
			WLAND_ERR("sh1 check firmware failed\n");
			goto err;
		}

		if (wland_hash("sha1", fw_entry->data, fw_entry->size, sha1_result) < 0) {
			ret = -1;
			WLAND_ERR("count sh1 failed\n");
			goto err;
		} else if(memcmp(result, sha1_result, SHA1_DIGEST_SIZE)) {
			ret = -2;
			WLAND_ERR("sh1 check result error. return result:%04x%04x%04x%04x%04x"
				"my_result:%04x%04x%04x%04x%04x\n",
				*(u32*)(result+0), *(u32*)(result+4), *(u32*)(result+8),*(u32*)(result+12),*(u32*)(result+16),
				*(u32*)(sha1_result+0), *(u32*)(sha1_result+4), *(u32*)(sha1_result+8),
				*(u32*)(sha1_result+12), *(u32*)(sha1_result+16));
			goto err;
		}
#endif

#ifdef DECRYPT_FIRMWARE_RC4
	MCD_TRACE(MCD_INFO_TRC, 0, "wland_sdio_set_decrypt\n");
	ret =  wifi_SdioSetDecrypt(addr, size, HOST_DECRYPT_RC4);
	if (ret < 0) {
		ret = -1;
        	MCD_TRACE(MCD_INFO_TRC, 0, "wland_sdio_set_decrypt:%d failed\n", HOST_DECRYPT_RC4);
		goto err;
	}
#elif defined DECRYPT_FIRMWARE_AES
	WLAND_DBG(TRAP, INFO, "wland_sdio_set_decrypt\n");
	ret = wland_sdio_set_decrypt(ifp, addr, fw_entry->size, HOST_DECRYPT_AES_CBC);
	if (ret < 0) {
		ret = -1;
		WLAND_ERR("wland_sdio_set_decrypt:%d failed\n", HOST_DECRYPT_AES_CBC);
		goto err;
	}
#endif
err:
	return ret;

}

s32 wifi_SdioRunFirmware(u32 addr)
{
	UINT8 *buf_start = wid_buf + 4;

	UINT8 *buf = buf_start;
	struct wland_dcmd *msg = (struct wland_dcmd *)wid_buf;
	s32 ret = 0;
	u8 result;
	u16 data_len;

	MCD_TRACE(MCD_INFO_TRC, 0, "Enter  wifi_SdioRunFirmware()  addr:%x \n", addr);

	/* ensure firmware statue  ??????*/
/*	ret = wland_fil_get_cmd_data(ifp, WID_BOOTROM_START_APP, &result, 1);
	if (ret < 0 || result != 1) {
		MCD_TRACE(MCD_INFO_TRC, 0, "query firmware state error\n");
		return -1;
	}
*/

	ret = wland_push_wid(buf, WID_MEMORY_ADDRESS, &addr, sizeof(addr), 0);
	if (ret < 0) {
		MCD_TRACE(MCD_INFO_TRC, 0, "put wid error\n");
		return -1;
	}
	buf += ret;

	result = 1;
	ret = wland_push_wid(buf, WID_BOOTROM_START_APP, &result, sizeof(u8), 0);
	if (ret < 0) {
		MCD_TRACE(MCD_INFO_TRC, 0, "put wid error\n");
		return -1;
	}
	buf += ret;

	MCD_TRACE(MCD_INFO_TRC, 0, "valid data length in wid_buf is %d\n", (int)(buf - wid_buf));

	data_len = buf - buf_start;

	msg->wid_msg_id = 0;
	msg->wid_msg_type = 0x57;
	msg->wid_msg_length = data_len + 4;

	ret = firmware_wid_request(wid_buf_0, data_len + FMW_HEADER_LEN);
	if (ret < 0)
		MCD_TRACE(MCD_INFO_TRC, 0, "WID Result Failed\n");

	MCD_TRACE(MCD_INFO_TRC, 0, "firmware_wid_request() Done(err:%d)\n", ret);

	return ret;
}

extern  const unsigned char code_rda5991h_start[];
extern  const unsigned char data_rda5991h_start[];
extern  char code_rda5991h_size[];
extern  char data_rda5991h_size[];

#define FIRMWARE_ADDRESS(sec) sec##_start
#define FIRMWARE_SIZE(sec)    ((u32)sec##_size)


#define RDA5991H_CODE_ADDR 0x100000
#define RDA5991H_DATA_ADDR 0x180000
extern u8 fw_done;

int wifi_FirmwareDownload(void)
{

	int err =  -19 ; //-ENODEV;
        MCD_TRACE(MCD_INFO_TRC, 0,  " code size= %d, data size= %d\n",
			FIRMWARE_SIZE(code_rda5991h), FIRMWARE_SIZE(data_rda5991h));
        MCD_TRACE(MCD_INFO_TRC, 0,  " wifi_FWImageDownload\n");

	/*
	 * sta mode
	 */
	err = wifi_SdioDownloadImage(FIRMWARE_ADDRESS(code_rda5991h),
			FIRMWARE_SIZE(code_rda5991h), RDA5991H_CODE_ADDR);
	if (err < 0) {
		MCD_TRACE(MCD_INFO_TRC, 0,  "download code firmware failed!\n");
		goto fail;
	}

	err = wifi_SdioDownloadImage(FIRMWARE_ADDRESS(data_rda5991h),
			FIRMWARE_SIZE(data_rda5991h), RDA5991H_DATA_ADDR);

	if (err < 0) {
		MCD_TRACE(MCD_INFO_TRC, 0,  "download data firmware failed!\n");
		goto fail;
	}

	err = wifi_SdioRunFirmware(RDA5991H_CODE_ADDR);
	if (err < 0) {
		MCD_TRACE(MCD_INFO_TRC, 0,  "run firmware failed!\n");
		goto fail;
	}
	fw_done =  TRUE;

        sxr_Sleep(16384*1);

	memcpy(localMac, mac_addr, 6);
	err = wland_preinit_cmds_91h(localMac);
	if (err < 0) {
		MCD_TRACE(MCD_INFO_TRC, 0,  "preinit cmds failed!\n");
		goto fail;
	}

fail:
	MCD_TRACE(MCD_INFO_TRC, 0,  "Done(err:%d)\n", err);

	return err;
}


s32 wifi_start_scan_set(char *scan_ssid)
{
	UINT8 *buf_start = wid_buf + 4;

	UINT8 *buf = buf_start;
	struct wland_dcmd *msg = (struct wland_dcmd *)wid_buf;
	s32 ret = 0;
	u16 data_len;

	u16 wid_msg_len = FMW_HEADER_LEN;
	WID_T wid;
	WID_TYPE_T type;
	u8 size;
    u16 time = 20;

	MCD_TRACE(MCD_INFO_TRC, 0, "Enter  wifi_start_scan_set() scan\n");

	//memset(prot->buf, '\0', sizeof(prot->buf));

	wid = WID_SITE_SURVEY;
	type = wland_get_wid_type(wid);
	size = wland_get_wid_size(type, 1);
	buf[0] = (u8) (wid & 0x00FF);
	buf[1] = (u8) ((wid & 0xFF00) >> 8);
	buf[2] = size;
	buf[3] = SITE_SURVEY_ALL_CH;
	buf += (size + 3);
	wid_msg_len += (size + 3);


	wid = WID_START_SCAN_REQ;
	type = wland_get_wid_type(wid);
	size = wland_get_wid_size(type, 1);
	buf[0] = (u8) (wid & 0x00FF);
	buf[1] = (u8) ((wid & 0xFF00) >> 8);
	buf[2] = size;
	buf[3] = DEFAULT_SCAN;
	buf += (size + 3);
	wid_msg_len += (size + 3);

	wid = WID_SCAN_TYPE;
	type = wland_get_wid_type(wid);
	size = wland_get_wid_size(type, 1);
	buf[0] = (u8) (wid & 0x00FF);
	buf[1] = (u8) ((wid & 0xFF00) >> 8);
	buf[2] = size;
	buf[3] = ACTIVE_SCAN_TYPE;
	buf += (size + 3);
	wid_msg_len += (size + 3);

    ret = wland_push_wid(buf, WID_ACTIVE_SCAN_TIME, &time, 2, FALSE);
	if (ret < 0) {
		MCD_TRACE(MCD_INFO_TRC, 0, "put wid error\n");
		return -1;
	}
	buf += ret;
	wid_msg_len += ret;

	if (scan_ssid && strlen(scan_ssid)) {
		ret = wland_push_wid(buf, WID_SSID, scan_ssid, strlen(scan_ssid), FALSE);
	} else {
			size = 0;
			ret = wland_push_wid(buf, WID_SSID, &size, 1, FALSE);
	}
	if (ret < 0) {
		MCD_TRACE(MCD_INFO_TRC, 0, "put wid error\n");
		return -1;
	}
	buf += ret;
	wid_msg_len += ret;

	data_len = buf - buf_start;

	msg->wid_msg_id = 0;
	msg->wid_msg_type = 0x57;
	msg->wid_msg_length = data_len + 4;

	ret = firmware_wid_request(wid_buf_0, data_len + FMW_HEADER_LEN);
	if (ret < 0)
		MCD_TRACE(MCD_INFO_TRC, 0, " scan WID Result Failed\n");


	MCD_TRACE(MCD_INFO_TRC, 0, "Done scan\n");

	return ret;
}

s32 wifi_start_sscan_set(u8 channel, u16 time)
{
	UINT8 *buf_start = wid_buf + 4;

	UINT8 *buf = buf_start;
	struct wland_dcmd *msg = (struct wland_dcmd *)wid_buf;
	s32 ret = 0;
	u16 data_len;

	u16 wid_msg_len = FMW_HEADER_LEN;
	WID_T wid;
	WID_TYPE_T type;
	u8 size;

	MCD_TRACE(MCD_INFO_TRC, 0, "Enter  wifi_start_sscan_set()\n");

	//memset(prot->buf, '\0', sizeof(prot->buf));

	wid = WID_START_SCAN_REQ;
	type = wland_get_wid_type(wid);
	size = wland_get_wid_size(type, 1);
	buf[0] = (u8) (wid & 0x00FF);
	buf[1] = (u8) ((wid & 0xFF00) >> 8);
	buf[2] = size;
	buf[3] = USER_SCAN;
	buf += (size + 3);
	wid_msg_len += (size + 3);

	wid = WID_SCAN_TYPE;
	type = wland_get_wid_type(wid);
	size = wland_get_wid_size(type, 1);
	buf[0] = (u8) (wid & 0x00FF);
	buf[1] = (u8) ((wid & 0xFF00) >> 8);
	buf[2] = size;
	buf[3] = PASSIVE_SCAN_TYPE;
	buf += (size + 3);
	wid_msg_len += (size + 3);

	wid = WID_SITE_SURVEY;
	type = wland_get_wid_type(wid);
	size = wland_get_wid_size(type, 1);
	buf[0] = (u8) (wid & 0x00FF);
	buf[1] = (u8) ((wid & 0xFF00) >> 8);
	buf[2] = size;
	buf[3] = SITE_SURVEY_1CH;
	buf += (size + 3);
	wid_msg_len += (size + 3);

	wid = WID_CURRENT_CHANNEL;
	type = wland_get_wid_type(wid);
	size = wland_get_wid_size(type, 1);
	buf[0] = (u8) (wid & 0x00FF);
	buf[1] = (u8) ((wid & 0xFF00) >> 8);
	buf[2] = size;
	buf[3] = channel;
	buf += (size + 3);
	wid_msg_len += (size + 3);

    ret = wland_push_wid(buf, WID_PASSIVE_SCAN_TIME, &time, 2, FALSE);
	if (ret < 0) {
		MCD_TRACE(MCD_INFO_TRC, 0, "put wid error\n");
		return -1;
	}
	buf += ret;
	wid_msg_len += ret;

	data_len = buf - buf_start;

	msg->wid_msg_id = 0;
	msg->wid_msg_type = 0x57;
	msg->wid_msg_length = data_len + 4;

	ret = firmware_wid_request(wid_buf_0, data_len + FMW_HEADER_LEN);
	if (ret < 0)
		MCD_TRACE(MCD_INFO_TRC, 0, " scan WID Result Failed\n");


	MCD_TRACE(MCD_INFO_TRC, 0, "Done sscan\n");

	return ret;
}

