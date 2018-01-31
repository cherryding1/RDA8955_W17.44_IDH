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

#include "uctls_m.h"
#include "urndis_callback.h"
typedef HAL_USB_CALLBACK_EP_TYPE_T MsgBody_t;
#include "itf_msg.h"
#include "hal_usb.h"

#include "sxr_ops.h"

#include "urndis_m.h"
#include "urndis_control_m.h"
#include "urndisp_debug.h"
#include "netif/ethernet.h"
#include "string.h"

extern UINT8                        g_urndisMbx;

#define RNDIS_MAX_CONFIGS	1

static rndis_params rndis_per_dev_params[RNDIS_MAX_CONFIGS];

/* Driver Version */
static const __le32 rndis_driver_version = cpu_to_le32(1);


PRIVATE UINT8    g_urndisIntrEp;
//max ctrl out msg len is less than HAL_USB_MPS * 2
PRIVATE UINT8 USB_UCBSS_INTERNAL g_urndisCtrlOut[HAL_USB_MPS * 2];
//oid_supported_list is the largest data reply,so max ctrl in msg len is sizeof(oid_supported_list) + sizeof(rndis_query_cmplt_type)
PRIVATE UINT8 USB_UCBSS_INTERNAL g_urndisCtrlIn[sizeof(rndis_resp_t) + sizeof(oid_supported_list) + sizeof(rndis_query_cmplt_type)];
PRIVATE UINT8 USB_UCBSS_INTERNAL g_urndisEPIntrBuf[RESPONSE_AVAILABLE_LEN];
PRIVATE rndis_resp_t *response;
PRIVATE UINT8 isSendEncap;

PRIVATE VOID rndis_intr_msg_send(VOID)
{
    Msg_t * MsgPtr ;
    UINT16     Len    = sizeof( Msg_t );
    MsgPtr = (Msg_t*) sxr_Malloc(Len);
    MsgPtr->B = URNDIS_MSG_SEND_INTR;
    if(g_urndisMbx != 0xff)
    {
        sxr_Send(MsgPtr, g_urndisMbx, SXR_SEND_MSG);
    }
}

PRIVATE rndis_resp_t *rndis_add_response(int configNr, u32 length)
{
    response = (rndis_resp_t *)g_urndisCtrlIn;
    response->buf = (u8 *)(response + 1);
    response->length = length;
    return response;
}

#if 0
PRIVATE void rndis_free_response(void)
{
    response = NULL;
}
#endif

/* NDIS Functions */
PRIVATE int gen_ndis_query_resp(int configNr, u32 OID, u8 *buf,
			       unsigned buf_len, rndis_resp_t *r)
{
	int retval = -ENOTSUPP;
	u32 length = 4;	/* usually */
	__le32 *outbuf;
	int i, count;
	rndis_query_cmplt_type *resp;
	//struct net_device *net;
	struct rtnl_link_stats64 temp;
	const struct rtnl_link_stats64 *stats;

	if (!r) return -ENOMEM;
	resp = (rndis_query_cmplt_type *)r->buf;

	if (!resp) return -ENOMEM;

	if (buf_len && rndis_debug > 1) {
		pr_debug("query OID %08x value, len %d:\n", OID, buf_len);
		for (i = 0; i < buf_len; i += 16) {
			pr_debug("%03d: %08x %08x %08x %08x\n", i,
				get_unaligned_le32(&buf[i]),
				get_unaligned_le32(&buf[i + 4]),
				get_unaligned_le32(&buf[i + 8]),
				get_unaligned_le32(&buf[i + 12]));
		}
	}

	/* response goes here, right after the header */
	outbuf = (__le32 *)&resp[1];
	resp->InformationBufferOffset = cpu_to_le32(16);

	//net = rndis_per_dev_params[configNr].dev;
	//stats = dev_get_stats(net, &temp);
    temp.rx_dropped = 0;
    temp.rx_errors = 0;
    temp.tx_dropped = 0;
    temp.tx_errors = 0;
    temp.rx_frame_errors = 0;
    stats = &temp;

	switch (OID) {

	/* general oids (table 4-1) */

	/* mandatory */
	case RNDIS_OID_GEN_SUPPORTED_LIST:
		pr_debug("%s: RNDIS_OID_GEN_SUPPORTED_LIST\n", __func__);
		length = sizeof(oid_supported_list);
		count  = length / sizeof(u32);
		for (i = 0; i < count; i++)
			outbuf[i] = cpu_to_le32(oid_supported_list[i]);
		retval = 0;
		break;

	/* mandatory */
	case RNDIS_OID_GEN_HARDWARE_STATUS:
		pr_debug("%s: RNDIS_OID_GEN_HARDWARE_STATUS\n", __func__);
		/* Bogus question!
		 * Hardware must be ready to receive high level protocols.
		 * BTW:
		 * reddite ergo quae sunt Caesaris Caesari
		 * et quae sunt Dei Deo!
		 */
		*outbuf = cpu_to_le32(0);
		retval = 0;
		break;

	/* mandatory */
	case RNDIS_OID_GEN_MEDIA_SUPPORTED:
		pr_debug("%s: RNDIS_OID_GEN_MEDIA_SUPPORTED\n", __func__);
		*outbuf = cpu_to_le32(rndis_per_dev_params[configNr].medium);
		retval = 0;
		break;

	/* mandatory */
	case RNDIS_OID_GEN_MEDIA_IN_USE:
		pr_debug("%s: RNDIS_OID_GEN_MEDIA_IN_USE\n", __func__);
		/* one medium, one transport... (maybe you do it better) */
		*outbuf = cpu_to_le32(rndis_per_dev_params[configNr].medium);
		retval = 0;
		break;

	/* mandatory */
	case RNDIS_OID_GEN_MAXIMUM_FRAME_SIZE:
		pr_debug("%s: RNDIS_OID_GEN_MAXIMUM_FRAME_SIZE\n", __func__);
		//if (rndis_per_dev_params[configNr].dev) {
			*outbuf = cpu_to_le32(
				rndis_per_dev_params[configNr].mtu);
			retval = 0;
		//}
		break;

	/* mandatory */
	case RNDIS_OID_GEN_LINK_SPEED:
		if (rndis_debug > 1)
			pr_debug("%s: RNDIS_OID_GEN_LINK_SPEED\n", __func__);
		if (rndis_per_dev_params[configNr].media_state
				== RNDIS_MEDIA_STATE_DISCONNECTED)
			*outbuf = cpu_to_le32(0);
		else
			*outbuf = cpu_to_le32(
				rndis_per_dev_params[configNr].speed);
		retval = 0;
		break;

	/* mandatory */
	case RNDIS_OID_GEN_TRANSMIT_BLOCK_SIZE:
		pr_debug("%s: RNDIS_OID_GEN_TRANSMIT_BLOCK_SIZE\n", __func__);
		//if (rndis_per_dev_params[configNr].dev) {
			*outbuf = cpu_to_le32(
				rndis_per_dev_params[configNr].mtu);
			retval = 0;
		//}
		break;

	/* mandatory */
	case RNDIS_OID_GEN_RECEIVE_BLOCK_SIZE:
		pr_debug("%s: RNDIS_OID_GEN_RECEIVE_BLOCK_SIZE\n", __func__);
		//if (rndis_per_dev_params[configNr].dev) {
			*outbuf = cpu_to_le32(
				rndis_per_dev_params[configNr].mtu);
			retval = 0;
		//}
		break;

	/* mandatory */
	case RNDIS_OID_GEN_VENDOR_ID:
		pr_debug("%s: RNDIS_OID_GEN_VENDOR_ID\n", __func__);
		*outbuf = cpu_to_le32(
			rndis_per_dev_params[configNr].vendorID);
		retval = 0;
		break;

	/* mandatory */
	case RNDIS_OID_GEN_VENDOR_DESCRIPTION:
		pr_debug("%s: RNDIS_OID_GEN_VENDOR_DESCRIPTION\n", __func__);
		if (rndis_per_dev_params[configNr].vendorDescr) {
			length = strlen(rndis_per_dev_params[configNr].
					vendorDescr);
			memcpy(outbuf,
				rndis_per_dev_params[configNr].vendorDescr,
				length);
		} else {
			outbuf[0] = 0;
		}
		retval = 0;
		break;

	case RNDIS_OID_GEN_VENDOR_DRIVER_VERSION:
		pr_debug("%s: RNDIS_OID_GEN_VENDOR_DRIVER_VERSION\n", __func__);
		/* Created as LE */
		*outbuf = rndis_driver_version;
		retval = 0;
		break;

	/* mandatory */
	case RNDIS_OID_GEN_CURRENT_PACKET_FILTER:
		URNDIS_TRACE(URNDIS_INFO_TRC, 0, "query: RNDIS_OID_GEN_CURRENT_PACKET_FILTER\n");
		*outbuf = cpu_to_le32(*rndis_per_dev_params[configNr].filter);
		retval = 0;
		break;

	/* mandatory */
	case RNDIS_OID_GEN_MAXIMUM_TOTAL_SIZE:
		pr_debug("%s: RNDIS_OID_GEN_MAXIMUM_TOTAL_SIZE\n", __func__);
		*outbuf = cpu_to_le32(RNDIS_MAX_TOTAL_SIZE);
		retval = 0;
		break;

	/* mandatory */
	case RNDIS_OID_GEN_MEDIA_CONNECT_STATUS:
		if (rndis_debug > 1)
			pr_debug("%s: RNDIS_OID_GEN_MEDIA_CONNECT_STATUS\n", __func__);
		*outbuf = cpu_to_le32(rndis_per_dev_params[configNr]
						.media_state);
		retval = 0;
		break;

	case RNDIS_OID_GEN_PHYSICAL_MEDIUM:
		pr_debug("%s: RNDIS_OID_GEN_PHYSICAL_MEDIUM\n", __func__);
		*outbuf = cpu_to_le32(0);
		retval = 0;
		break;

	/* The RNDIS specification is incomplete/wrong.   Some versions
	 * of MS-Windows expect OIDs that aren't specified there.  Other
	 * versions emit undefined RNDIS messages. DOCUMENT ALL THESE!
	 */
	case RNDIS_OID_GEN_MAC_OPTIONS:		/* from WinME */
		pr_debug("%s: RNDIS_OID_GEN_MAC_OPTIONS\n", __func__);
		*outbuf = cpu_to_le32(
			  RNDIS_MAC_OPTION_RECEIVE_SERIALIZED
			| RNDIS_MAC_OPTION_FULL_DUPLEX);
		retval = 0;
		break;

	/* statistics OIDs (table 4-2) */

	/* mandatory */
	case RNDIS_OID_GEN_XMIT_OK:
		if (rndis_debug > 1)
			pr_debug("%s: RNDIS_OID_GEN_XMIT_OK\n", __func__);
		if (stats) {
			*outbuf = cpu_to_le32(stats->tx_packets
				- stats->tx_errors - stats->tx_dropped);
			retval = 0;
		}
		break;

	/* mandatory */
	case RNDIS_OID_GEN_RCV_OK:
		if (rndis_debug > 1)
			pr_debug("%s: RNDIS_OID_GEN_RCV_OK\n", __func__);
		if (stats) {
			*outbuf = cpu_to_le32(stats->rx_packets
				- stats->rx_errors - stats->rx_dropped);
			retval = 0;
		}
		break;

	/* mandatory */
	case RNDIS_OID_GEN_XMIT_ERROR:
		if (rndis_debug > 1)
			pr_debug("%s: RNDIS_OID_GEN_XMIT_ERROR\n", __func__);
		if (stats) {
			*outbuf = cpu_to_le32(stats->tx_errors);
			retval = 0;
		}
		break;

	/* mandatory */
	case RNDIS_OID_GEN_RCV_ERROR:
		if (rndis_debug > 1)
			pr_debug("%s: RNDIS_OID_GEN_RCV_ERROR\n", __func__);
		if (stats) {
			*outbuf = cpu_to_le32(stats->rx_errors);
			retval = 0;
		}
		break;

	/* mandatory */
	case RNDIS_OID_GEN_RCV_NO_BUFFER:
		pr_debug("%s: RNDIS_OID_GEN_RCV_NO_BUFFER\n", __func__);
		if (stats) {
			*outbuf = cpu_to_le32(stats->rx_dropped);
			retval = 0;
		}
		break;

	/* ieee802.3 OIDs (table 4-3) */

	/* mandatory */
	case RNDIS_OID_802_3_PERMANENT_ADDRESS:
		pr_debug("%s: RNDIS_OID_802_3_PERMANENT_ADDRESS\n", __func__);
		//if (rndis_per_dev_params[configNr].dev) {
			length = ETH_ALEN;
			memcpy(outbuf,
				rndis_per_dev_params[configNr].host_mac,
				length);
			retval = 0;
		//}
		break;

	/* mandatory */
	case RNDIS_OID_802_3_CURRENT_ADDRESS:
		pr_debug("%s: RNDIS_OID_802_3_CURRENT_ADDRESS\n", __func__);
		//if (rndis_per_dev_params[configNr].dev) {
			length = ETH_ALEN;
			memcpy(outbuf,
				rndis_per_dev_params [configNr].host_mac,
				length);
			retval = 0;
		//}
		break;

	/* mandatory */
	case RNDIS_OID_802_3_MULTICAST_LIST:
		pr_debug("%s: RNDIS_OID_802_3_MULTICAST_LIST\n", __func__);
		/* Multicast base address only */
		*outbuf = cpu_to_le32(0xE0000000);
		retval = 0;
		break;

	/* mandatory */
	case RNDIS_OID_802_3_MAXIMUM_LIST_SIZE:
		pr_debug("%s: RNDIS_OID_802_3_MAXIMUM_LIST_SIZE\n", __func__);
		/* Multicast base address only */
		*outbuf = cpu_to_le32(1);
		retval = 0;
		break;

	case RNDIS_OID_802_3_MAC_OPTIONS:
		pr_debug("%s: RNDIS_OID_802_3_MAC_OPTIONS\n", __func__);
		*outbuf = cpu_to_le32(0);
		retval = 0;
		break;

	/* ieee802.3 statistics OIDs (table 4-4) */

	/* mandatory */
	case RNDIS_OID_802_3_RCV_ERROR_ALIGNMENT:
		pr_debug("%s: RNDIS_OID_802_3_RCV_ERROR_ALIGNMENT\n", __func__);
		if (stats) {
			*outbuf = cpu_to_le32(stats->rx_frame_errors);
			retval = 0;
		}
		break;

	/* mandatory */
	case RNDIS_OID_802_3_XMIT_ONE_COLLISION:
		pr_debug("%s: RNDIS_OID_802_3_XMIT_ONE_COLLISION\n", __func__);
		*outbuf = cpu_to_le32(0);
		retval = 0;
		break;

	/* mandatory */
	case RNDIS_OID_802_3_XMIT_MORE_COLLISIONS:
		pr_debug("%s: RNDIS_OID_802_3_XMIT_MORE_COLLISIONS\n", __func__);
		*outbuf = cpu_to_le32(0);
		retval = 0;
		break;

	default:
		pr_warning("%s: query unknown OID 0x%08X\n",
			 __func__, OID);
	}
	if (retval < 0)
		length = 0;

	resp->InformationBufferLength = cpu_to_le32(length);
	r->length = length + sizeof(*resp);
	resp->MessageLength = cpu_to_le32(r->length);
	return retval;
}

static int gen_ndis_set_resp(u8 configNr, u32 OID, u8 *buf, u32 buf_len,
			     rndis_resp_t *r)
{
	rndis_set_cmplt_type *resp;
	int i, retval = -ENOTSUPP;
	struct rndis_params *params;

	if (!r)
		return -ENOMEM;
	resp = (rndis_set_cmplt_type *)r->buf;
	if (!resp)
		return -ENOMEM;

	if (buf_len && rndis_debug > 1) {
		pr_debug("set OID %08x value, len %d:\n", OID, buf_len);
		for (i = 0; i < buf_len; i += 16) {
			pr_debug("%03d: %08x %08x %08x %08x\n", i,
				get_unaligned_le32(&buf[i]),
				get_unaligned_le32(&buf[i + 4]),
				get_unaligned_le32(&buf[i + 8]),
				get_unaligned_le32(&buf[i + 12]));
		}
	}

	params = &rndis_per_dev_params[configNr];
	switch (OID) {
	case RNDIS_OID_GEN_CURRENT_PACKET_FILTER:

		/* these NDIS_PACKET_TYPE_* bitflags are shared with
		 * cdc_filter; it's not RNDIS-specific
		 * NDIS_PACKET_TYPE_x == USB_CDC_PACKET_TYPE_x for x in:
		 *	PROMISCUOUS, DIRECTED,
		 *	MULTICAST, ALL_MULTICAST, BROADCAST
		 */
		*params->filter = (u16)get_unaligned_le32(buf);
		URNDIS_TRACE(URNDIS_INFO_TRC, 0, "set: RNDIS_OID_GEN_CURRENT_PACKET_FILTER %08x\n",
			*params->filter);

		/* this call has a significant side effect:  it's
		 * what makes the packet flow start and stop, like
		 * activating the CDC Ethernet altsetting.
		 */
		retval = 0;
		if (*params->filter) {
			params->state = RNDIS_DATA_INITIALIZED;
			//netif_carrier_on(params->dev);
			//if (netif_running(params->dev))
			//	netif_wake_queue(params->dev);
		} else {
			params->state = RNDIS_INITIALIZED;
			//netif_carrier_off(params->dev);
			//netif_stop_queue(params->dev);
		}
		break;

	case RNDIS_OID_802_3_MULTICAST_LIST:
		/* I think we can ignore this */
		pr_debug("%s: RNDIS_OID_802_3_MULTICAST_LIST\n", __func__);
		retval = 0;
		break;

	default:
		pr_warning("%s: set unknown OID 0x%08X, size %d\n",
			 __func__, OID, buf_len);
	}

	return retval;
}

PRIVATE INT32 rndis_control_msg_parser(u8 configNr, u8 *buf)
{
    UINT32 MsgType, MsgLength, RequestID;
    UINT32 *tmp = (UINT32 *)buf;
	struct rndis_params *params;

    MsgType = *tmp;
    MsgLength = *(tmp + 1);
    RequestID = *(tmp + 2);
    //URNDIS_TRACE(URNDIS_INFO_TRC, 0, "MsgType=%d MsgLength=%d RequestID=%02X\n",MsgType,MsgLength,RequestID);
    URNDIS_TRACE(URNDIS_INFO_TRC, 0, "RequestID=%04X\n",RequestID);

	if (configNr >= RNDIS_MAX_CONFIGS)
		return -ENOTSUPP;
	params = &rndis_per_dev_params[configNr];

	/* NOTE: RNDIS is *EXTREMELY* chatty ... Windows constantly polls for
	 * rx/tx statistics and link status, in addition to KEEPALIVE traffic
	 * and normal HC level polling to see if there's any IN traffic.
	 */

	/* For USB: responses may take up to 10 seconds */
    switch(MsgType)
    {
        case RNDIS_MSG_INIT:
        {
            rndis_init_cmplt_type *resp;
            rndis_resp_t *r;
            r = rndis_add_response(0, sizeof(rndis_init_cmplt_type));

            rndis_init_msg_type *init_msg = (rndis_init_msg_type *)buf;

            resp = (rndis_init_cmplt_type *)r->buf;

            resp->MessageType = cpu_to_le32(RNDIS_MSG_INIT_C);
            resp->MessageLength = cpu_to_le32(52);
            resp->RequestID = init_msg->RequestID; /* Still LE in msg buffer */
            resp->Status = cpu_to_le32(RNDIS_STATUS_SUCCESS);
            resp->MajorVersion = cpu_to_le32(RNDIS_MAJOR_VERSION);
            resp->MinorVersion = cpu_to_le32(RNDIS_MINOR_VERSION);
            resp->DeviceFlags = cpu_to_le32(RNDIS_DF_CONNECTIONLESS);
            resp->Medium = cpu_to_le32(RNDIS_MEDIUM_802_3);
            resp->MaxPacketsPerTransfer = cpu_to_le32(params->max_pkt_per_xfer);
            resp->MaxTransferSize = cpu_to_le32(params->max_pkt_per_xfer *
                (params->mtu
                + sizeof(struct eth_hdr)
                + sizeof(struct rndis_packet_msg_type)
                + 22));
            resp->PacketAlignmentFactor = cpu_to_le32(0);
            resp->AFListOffset = cpu_to_le32(0);
            resp->AFListSize = cpu_to_le32(0);

            params->state = RNDIS_INITIALIZED;

            rndis_intr_msg_send();
            break;
        }
        case RNDIS_MSG_HALT:
            params->state = RNDIS_UNINITIALIZED;
            isSendEncap = 0;
            break;
        case RNDIS_MSG_QUERY:
        {
            rndis_query_cmplt_type *resp;
            rndis_resp_t *r;

            /*
            * we need more memory:
            * gen_ndis_query_resp expects enough space for
            * rndis_query_cmplt_type followed by data.
            * oid_supported_list is the largest data reply
            */
            r = rndis_add_response(0,
                sizeof(oid_supported_list) + sizeof(rndis_query_cmplt_type));

            rndis_query_msg_type *query_msg = (rndis_query_msg_type *)buf;

            resp = (rndis_query_cmplt_type *)r->buf;

            resp->MessageType = cpu_to_le32(RNDIS_MSG_QUERY_C);
            resp->RequestID = query_msg->RequestID; /* Still LE in msg buffer */
            if (gen_ndis_query_resp(0, le32_to_cpu(query_msg->OID),
                    le32_to_cpu(query_msg->InformationBufferOffset)
                            + 8 + (u8 *)query_msg,
                    le32_to_cpu(query_msg->InformationBufferLength),
                    r)) {
                /* OID not supported */
                resp->Status = cpu_to_le32(RNDIS_STATUS_NOT_SUPPORTED);
                resp->MessageLength = cpu_to_le32(sizeof *resp);
                resp->InformationBufferLength = cpu_to_le32(0);
                resp->InformationBufferOffset = cpu_to_le32(0);
            } else
                resp->Status = cpu_to_le32(RNDIS_STATUS_SUCCESS);

            rndis_intr_msg_send();
            break;
        }
        case RNDIS_MSG_SET:
        {
            u32 BufLength, BufOffset;
            rndis_set_cmplt_type *resp;
            rndis_resp_t *r;

            r = rndis_add_response(0, sizeof(rndis_set_cmplt_type));

            rndis_set_msg_type *set_msg = (rndis_set_msg_type *)buf;
            BufLength = le32_to_cpu(set_msg->InformationBufferLength);
            BufOffset = le32_to_cpu(set_msg->InformationBufferOffset);

            resp = (rndis_set_cmplt_type *)r->buf;

            resp->MessageType = cpu_to_le32(RNDIS_MSG_SET_C);
            resp->MessageLength = cpu_to_le32(16);
            resp->RequestID = set_msg->RequestID; /* Still LE in msg buffer */
            if (gen_ndis_set_resp(0, le32_to_cpu(set_msg->OID),
                    ((u8 *)set_msg) + 8 + BufOffset, BufLength, r))
                resp->Status = cpu_to_le32(RNDIS_STATUS_NOT_SUPPORTED);
            else
                resp->Status = cpu_to_le32(RNDIS_STATUS_SUCCESS);

            rndis_intr_msg_send();
            break;
        }
        case RNDIS_MSG_RESET:
        {
            rndis_reset_cmplt_type *resp;
            rndis_resp_t *r;

            r = rndis_add_response(0, sizeof(rndis_reset_cmplt_type));

            resp = (rndis_reset_cmplt_type *)r->buf;

            resp->MessageType = cpu_to_le32(RNDIS_MSG_RESET_C);
            resp->MessageLength = cpu_to_le32(16);
            resp->Status = cpu_to_le32(RNDIS_STATUS_SUCCESS);
            /* resent information */
            resp->AddressingReset = cpu_to_le32(1);

            rndis_intr_msg_send();
            break;
        }
        case RNDIS_MSG_KEEPALIVE:
        {
            rndis_keepalive_cmplt_type *resp;
            rndis_resp_t *r;

            r = rndis_add_response(0, sizeof(rndis_keepalive_cmplt_type));

            rndis_keepalive_msg_type *keepalive_msg = (rndis_keepalive_msg_type *)buf;

            resp = (rndis_keepalive_cmplt_type *)r->buf;

            resp->MessageType = cpu_to_le32(RNDIS_MSG_KEEPALIVE_C);
            resp->MessageLength = cpu_to_le32(16);
            resp->RequestID = keepalive_msg->RequestID; /* Still LE in msg buffer */
            resp->Status = cpu_to_le32(RNDIS_STATUS_SUCCESS);

            rndis_intr_msg_send();
            break;
        }
        default:
            break;
    }
    return 0;
}

VOID rndis_control_send_intr(VOID)
{
    sxr_Sleep(200 MS_WAITING);

    UINT32 *tmp = (UINT32 *)g_urndisEPIntrBuf;
    *tmp = RESPONSE_AVAILABLE;
    *(tmp + 1) = 0;

    hal_UsbSend(g_urndisIntrEp, g_urndisEPIntrBuf, RESPONSE_AVAILABLE_LEN, 0);
}

PRIVATE HAL_USB_CALLBACK_RETURN_T
rndisIntrEpCallback(HAL_USB_CALLBACK_EP_TYPE_T   type,
                     HAL_USB_SETUP_T*             setup)
{
    //URNDIS_TRACE(URNDIS_INFO_TRC, 0, "rndisIntrEpCallback\n");
    return(HAL_USB_CALLBACK_RETURN_OK);
}

PRIVATE HAL_USB_CALLBACK_RETURN_T
rndisControlCallback(HAL_USB_CALLBACK_EP_TYPE_T   type,
                      HAL_USB_SETUP_T*             setup)
{
    //URNDIS_TRACE(URNDIS_INFO_TRC, 0, "rndisControlCallback,type=%d\n",type);
    switch(type)
    {
        case HAL_USB_CALLBACK_TYPE_CMD:
            //URNDIS_TRACE(URNDIS_INFO_TRC, 0, "ctrl r=%d l=%d\n",setup->request,setup->lenght);
            if(setup->request == USB_CDC_SEND_ENCAPSULATED_COMMAND)
            {
                INT32 ret;
                isSendEncap = 1;
                if(setup->lenght <= HAL_USB_MPS)
                {
                    ret = hal_UsbRecv(0, g_urndisCtrlOut, HAL_USB_MPS, 0);
                }
                else
                {
                    ret = hal_UsbRecv(0, g_urndisCtrlOut, HAL_USB_MPS * 2, 0);
                }
            }
            else if(setup->request == USB_CDC_GET_ENCAPSULATED_RESPONSE)
            {
                isSendEncap = 0;
                hal_UsbSend(0, response->buf, response->length, 1);
            }
            return(HAL_USB_CALLBACK_RETURN_OK);
        case HAL_USB_CALLBACK_TYPE_DATA_CMD:
            //URNDIS_TRACE(URNDIS_INFO_TRC, 0, "ctrl is=%d\n",isSendEncap);
            if(isSendEncap)
            {
                rndis_control_msg_parser(0, g_urndisCtrlOut);
            }
            else
            {
                //rndis_free_response();
            }
            return(HAL_USB_CALLBACK_RETURN_OK);
        case HAL_USB_CALLBACK_TYPE_RECEIVE_END:
            break;
        case HAL_USB_CALLBACK_TYPE_TRANSMIT_END:
            break;
        case HAL_USB_CALLBACK_TYPE_ENABLE:
            break;
        case HAL_USB_CALLBACK_TYPE_DISABLE:
            break;
    }
    return(HAL_USB_CALLBACK_RETURN_KO);
}

/* peak (theoretical) bulk transfer rate in bits-per-second */
PRIVATE int bitrate(int speed)
{
	if (speed == USB_SPEED_SUPER)
		return 13 * 1024 * 8 * 1000 * 8;
	else if (speed == USB_SPEED_HIGH)
		return 13 * 512 * 8 * 1000 * 8;
	else
		return 19 * 64 * 1 * 1000 * 8;
}

const u8 host_macaddr[] = {0x32, 0x5F, 0x86, 0x7F, 0xD8, 0x5F};
PRIVATE u16 filter = 0;
PRIVATE int rndis_init(void)
{
    u8 i;
    for (i = 0; i < RNDIS_MAX_CONFIGS; i++) {
        rndis_per_dev_params[i].state = RNDIS_UNINITIALIZED;
        rndis_per_dev_params[i].medium = RNDIS_MEDIUM_802_3;
        rndis_per_dev_params[i].speed = bitrate(USB_SPEED_FULL) / 100;
        rndis_per_dev_params[i].media_state
                = RNDIS_MEDIA_STATE_CONNECTED;
        rndis_per_dev_params[i].host_mac = host_macaddr;
        rndis_per_dev_params[i].filter = &filter;
        rndis_per_dev_params[i].mtu = MAX_MTU;
        rndis_per_dev_params[i].vendorID = 8;
        rndis_per_dev_params[i].vendorDescr = "Rdamicro";
        rndis_per_dev_params[i].max_pkt_per_xfer = 3;
    }
    return 0;
}

PRIVATE HAL_USB_INTERFACE_DESCRIPTOR_T*
urndis_ControlOpen(CONST URNDIS_CONTROL_PARAMETERS_T*  cfg)
{
    HAL_USB_EP_DESCRIPTOR_T** epList;

    epList = (HAL_USB_EP_DESCRIPTOR_T**)
             sxr_Malloc(sizeof(HAL_USB_EP_DESCRIPTOR_T*)*2);

    g_urndisIntrEp = HAL_USB_EP_DIRECTION_IN(cfg->intrEpNum);
    rndis_init();

    epList[0] = uctls_NewInterruptEpDescriptor(g_urndisIntrEp, 255, rndisIntrEpCallback);
    epList[1] = 0;

    URNDIS_TRACE(URNDIS_INFO_TRC, 0, "URNDIS control Open\n");
    return uctls_NewInterfaceDescriptor(USB_CLASS_WIRELESS_CONTROLLER, 0x01, 0x03, 0x00,
                                        epList, "RNDIS Communications Control", rndisControlCallback);
}

PRIVATE VOID urndis_ControlClose(VOID)
{
    URNDIS_TRACE(URNDIS_INFO_TRC, 0, "URNDIS control Close\n");
}

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

PUBLIC CONST URNDIS_CONTROL_CALLBACK_T g_urndisControlCallback =
{
    .open  = urndis_ControlOpen,
    .close = urndis_ControlClose
};

