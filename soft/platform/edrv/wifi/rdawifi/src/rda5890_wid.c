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

#include <linux/module.h>
#include <linux/dcache.h>
#include <linux/debugfs.h>
#include <linux/delay.h>
#include <linux/mm.h>
#include <linux/string.h>
#include <linux/etherdevice.h>
#include <net/iw_handler.h>

#include "rda5890_defs.h"
#include "rda5890_dev.h"
#include "rda5890_wid.h"
#include "rda5890_wext.h"
#include "rda5890_txrx.h"

/* for both Query and Write */
int rda5890_wid_request(struct rda5890_private *priv,
                        char *wid_req, unsigned short wid_req_len,
                        char *wid_rsp, unsigned short *wid_rsp_len)
{
    int ret = 0;
    int timeleft = 0;
    char data_buf[RDA5890_MAX_WID_LEN + 2];
    unsigned short data_len;

    RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_DEBUG,
                   "%s >>>\n", __func__);

    mutex_lock(&priv->wid_lock);

    priv->wid_rsp = wid_rsp;
    priv->wid_rsp_len = *wid_rsp_len;
    priv->wid_pending = 1;

    data_len = wid_req_len + 2;
    data_buf[0] = (char)(data_len&0xFF);
    data_buf[1] = (char)((data_len>>8)&0x0F);
    data_buf[1] |= 0x40;  // for Request(Q/W) 0x4
    memcpy(data_buf + 2, wid_req, wid_req_len);

    init_completion(&priv->wid_done);

    ret = rda5890_host_to_card(priv, data_buf, data_len);
    if (ret)
    {
        RDA5890_ERRP("host_to_card send failed, ret = %d\n", ret);
        priv->wid_pending = 0;
        goto out;
    }

    timeleft = wait_for_completion_timeout(&priv->wid_done, HZ * 10);
    if (timeleft == 0)
    {
        RDA5890_ERRP("respose timeout\n");
        priv->wid_pending = 0;
        ret = -EFAULT;
        goto out;
    }

    *wid_rsp_len = priv->wid_rsp_len;

out:
    mutex_unlock(&priv->wid_lock);

    RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_DEBUG,
                   "%s <<<\n", __func__);

    return ret;
}

void rda5890_wid_response(struct rda5890_private *priv,
                          char *wid_rsp, unsigned short wid_rsp_len)
{
    RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_DEBUG,
                   "%s >>>\n", __func__);

    if (!priv->wid_pending)
    {
        RDA5890_ERRP("no wid pending\n");
        return;
    }

    if (wid_rsp_len > priv->wid_rsp_len)
    {
        RDA5890_ERRP("not enough space for wid response, size = %d, buf = %d\n",
                     wid_rsp_len, priv->wid_rsp_len);
        complete(&priv->wid_done);
        return;
    }

    memcpy(priv->wid_rsp, wid_rsp, wid_rsp_len);
    priv->wid_rsp_len = wid_rsp_len;
    priv->wid_pending = 0;

    RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_DEBUG,
                   "%s <<<\n", __func__);

    complete(&priv->wid_done);
}

void rda5890_wid_status(struct rda5890_private *priv,
                        char *wid_status, unsigned short wid_status_len)
{
    char mac_status;

    RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_DEBUG,
                   "%s >>>\n", __func__);

    mac_status = wid_status[7];
    if (mac_status == MAC_CONNECTED)
    {
        RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_CRIT,
                       "MAC CONNECTED\n");
        priv->connect_status = MAC_CONNECTED;
        netif_carrier_on(priv->dev);
        netif_wake_queue(priv->dev);
        cancel_delayed_work(&priv->assoc_done_work);
        queue_delayed_work(priv->work_thread, &priv->assoc_done_work, 0);
    }
    else if (mac_status == MAC_DISCONNECTED)
    {
        RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_CRIT,
                       "MAC DISCONNECTED\n");
        priv->connect_status = MAC_DISCONNECTED;
        netif_carrier_off(priv->dev);
        rda5890_indicate_disconnected(priv);
    }
    else
    {
        RDA5890_ERRP("Invalid MAC Status 0x%02x\n", mac_status);
    }

    RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_DEBUG,
                   "%s <<<\n", __func__);
}

void rda5890_card_to_host(struct rda5890_private *priv,
                          char *packet, unsigned short packet_len)
{
    unsigned char rx_type;
    unsigned short rx_length;
    unsigned char msg_type;

    RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_DEBUG,
                   "%s >>>\n", __func__);

    rx_type = (unsigned char)packet[1]&0xF0;
    rx_length = (unsigned short)(packet[0] + ((packet[1]&0x0f) << 8));

    if (rx_length > packet_len)
    {
        RDA5890_ERRP("packet_len %d less than header specified length %d\n",
                     packet_len, rx_length);
        goto out;
    }

    if( rx_type == 0x30 )
    {
        RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_NORM,
                       "Message Packet\n");
        msg_type = packet[2];
        if(msg_type == 'I')
        {
            RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_NORM,
                           "Indication Message\n");
            rda5890_wid_status(priv, packet + 2, rx_length - 2);
        }
        else if (msg_type == 'R')
        {
            RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_NORM,
                           "WID Message\n");
            rda5890_wid_response(priv, packet + 2, rx_length - 2);
        }
        else
        {
            //RDA5890_ERRP("Invalid Message Type '%c'\n", msg_type);
        }
    }
    else if(rx_type == 0x20)
    {
        RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_NORM,
                       "Data Packet\n");
        rda5890_data_rx(priv, packet + 2, rx_length - 2);
    }
    else
    {
        RDA5890_ERRP("Invalid Packet Type 0x%02x\n", rx_type);
    }

out:
    RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_DEBUG,
                   "%s <<<\n", __func__);
}

int rda5890_host_to_card(struct rda5890_private *priv,
                         char *packet, unsigned short packet_len)
{
    int ret = 0;

    RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_DEBUG,
                   "%s >>>\n", __func__);

    ret = priv->hw_host_to_card(priv, packet, packet_len);

    RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_DEBUG,
                   "%s >>>\n", __func__);

    return ret;
}

/*
 * check wid response header
 * if no error, return the pointer to the payload
 * support only 1 wid per packet
 */
int rda5890_check_wid_response(char *wid_rsp, unsigned short wid_rsp_len,
                               unsigned short wid, char wid_msg_id,
                               char payload_len, char **ptr_payload)
{
    unsigned short rsp_len;
    unsigned short rsp_wid;
    unsigned char msg_len;

    if (wid_rsp[0] != 'R')
    {
        RDA5890_ERRP("wid_rsp[0] != 'R'\n");
        return -EINVAL;
    }

    if (wid_rsp[1] != wid_msg_id)
    {
        RDA5890_ERRP("wid_msg_id not match\n");
        return -EINVAL;
    }

    if (wid_rsp_len < 4)
    {
        RDA5890_ERRP("wid_rsp_len < 4\n");
        return -EINVAL;
    }

    rsp_len = wid_rsp[2] | (wid_rsp[3] << 8);
    if (wid_rsp_len != rsp_len)
    {
        RDA5890_ERRP("wid_rsp_len not match, %d != %d\n", wid_rsp_len, rsp_len);
        return -EINVAL;
    }

    if (wid_rsp_len < 7)
    {
        RDA5890_ERRP("wid_rsp_len < 7\n");
        return -EINVAL;
    }

    rsp_wid = wid_rsp[4] | (wid_rsp[5] << 8);
    if (wid != rsp_wid)
    {
        RDA5890_ERRP("wid not match, 0x%04x != 0x%04x\n", wid, rsp_wid);
        return -EINVAL;
    }

    msg_len = wid_rsp[6];
    if (wid_rsp_len != msg_len + 7)
    {
        RDA5890_ERRP("msg_len not match, %d + 7 != %d\n", msg_len, wid_rsp_len);
        return -EINVAL;
    }

    if (payload_len != msg_len)
    {
        RDA5890_ERRP("payload_len not match, %d  != %d\n", msg_len, payload_len);
        return -EINVAL;
    }

    *ptr_payload = wid_rsp + 7;

    return 0;
}

/*
 * check wid status response
 */
int rda5890_check_wid_status(char *wid_rsp, unsigned short wid_rsp_len,
                             char wid_msg_id)
{
    int ret;
    unsigned short wid = WID_STATUS;
    char *ptr_payload;
    char status_val;

    ret = rda5890_check_wid_response(wid_rsp, wid_rsp_len, wid, wid_msg_id,
                                     1, &ptr_payload);
    if (ret)
    {
        RDA5890_ERRP("rda5890_check_wid_status, check_wid_response fail\n");
        return ret;
    }

    status_val = ptr_payload[0];
    if (status_val != WID_STATUS_SUCCESS)
    {
        RDA5890_ERRP("check_wid_status NOT success, status = %d\n", status_val);
        return -EINVAL;
    }

    return 0;
}

static int rda5890_generic_get_uchar(struct rda5890_private *priv,
                                     unsigned short wid, unsigned char *val)
{
    int ret;
    char wid_req[6];
    unsigned short wid_req_len = 6;
    char wid_rsp[32];
    unsigned short wid_rsp_len = 32;
    char wid_msg_id = priv->wid_msg_id++;
    char *ptr_payload;

    wid_req[0] = 'Q';
    wid_req[1] = wid_msg_id;

    wid_req[2] = (char)(wid_req_len&0x00FF);
    wid_req[3] = (char)((wid_req_len&0xFF00) >> 8);

    wid_req[4] = (char)(wid&0x00FF);
    wid_req[5] = (char)((wid&0xFF00) >> 8);

    ret = rda5890_wid_request(priv, wid_req, wid_req_len, wid_rsp, &wid_rsp_len);
    if (ret)
    {
        RDA5890_ERRP("rda5890_wid_request fail, ret = %d\n", ret);
        goto out;
    }

    ret = rda5890_check_wid_response(wid_rsp, wid_rsp_len, wid, wid_msg_id,
                                     sizeof(unsigned char), &ptr_payload);
    if (ret)
    {
        RDA5890_ERRP("check_wid_response fail, ret = %d\n", ret);
        goto out;
    }

    memcpy(val, ptr_payload, sizeof(unsigned char));

out:
    return ret;
}

int rda5890_generic_set_uchar(struct rda5890_private *priv,
                              unsigned short wid, unsigned char val)
{
    int ret;
    char wid_req[7 + sizeof(unsigned char)];
    unsigned short wid_req_len = 7 + sizeof(unsigned char);
    char wid_rsp[32];
    unsigned short wid_rsp_len = 32;
    char wid_msg_id = priv->wid_msg_id++;

    wid_req[0] = 'W';
    wid_req[1] = wid_msg_id;

    wid_req[2] = (char)(wid_req_len&0x00FF);
    wid_req[3] = (char)((wid_req_len&0xFF00) >> 8);

    wid_req[4] = (char)(wid&0x00FF);
    wid_req[5] = (char)((wid&0xFF00) >> 8);

    wid_req[6] = (char)(sizeof(unsigned char));
    memcpy(wid_req + 7, &val, sizeof(unsigned char));

    ret = rda5890_wid_request(priv, wid_req, wid_req_len, wid_rsp, &wid_rsp_len);
    if (ret)
    {
        RDA5890_ERRP("rda5890_wid_request fail, ret = %d\n", ret);
        goto out;
    }

    ret = rda5890_check_wid_status(wid_rsp, wid_rsp_len, wid_msg_id);
    if (ret)
    {
        RDA5890_ERRP("check_wid_status fail, ret = %d\n", ret);
        goto out;
    }

out:
    return ret;
}

#if 0
static int rda5890_generic_get_ushort(struct rda5890_private *priv,
                                      unsigned short wid, unsigned short *val)
{
    int ret;
    char wid_req[6];
    unsigned short wid_req_len = 6;
    char wid_rsp[32];
    unsigned short wid_rsp_len = 32;
    char wid_msg_id = priv->wid_msg_id++;
    char *ptr_payload;

    wid_req[0] = 'Q';
    wid_req[1] = wid_msg_id;

    wid_req[2] = (char)(wid_req_len&0x00FF);
    wid_req[3] = (char)((wid_req_len&0xFF00) >> 8);

    wid_req[4] = (char)(wid&0x00FF);
    wid_req[5] = (char)((wid&0xFF00) >> 8);

    ret = rda5890_wid_request(priv, wid_req, wid_req_len, wid_rsp, &wid_rsp_len);
    if (ret)
    {
        RDA5890_ERRP("rda5890_wid_request fail, ret = %d\n", ret);
        goto out;
    }

    ret = rda5890_check_wid_response(wid_rsp, wid_rsp_len, wid, wid_msg_id,
                                     sizeof(unsigned short), &ptr_payload);
    if (ret)
    {
        RDA5890_ERRP("check_wid_response fail, ret = %d\n", ret);
        goto out;
    }

    memcpy(val, ptr_payload, sizeof(unsigned short));

out:
    return ret;
}

int rda5890_generic_set_ushort(struct rda5890_private *priv,
                               unsigned short wid, unsigned short val)
{
    int ret;
    char wid_req[7 + sizeof(unsigned short)];
    unsigned short wid_req_len = 7 + sizeof(unsigned short);
    char wid_rsp[32];
    unsigned short wid_rsp_len = 32;
    char wid_msg_id = priv->wid_msg_id++;

    wid_req[0] = 'W';
    wid_req[1] = wid_msg_id;

    wid_req[2] = (char)(wid_req_len&0x00FF);
    wid_req[3] = (char)((wid_req_len&0xFF00) >> 8);

    wid_req[4] = (char)(wid&0x00FF);
    wid_req[5] = (char)((wid&0xFF00) >> 8);

    wid_req[6] = (char)(sizeof(unsigned short));
    memcpy(wid_req + 7, &val, sizeof(unsigned short));

    ret = rda5890_wid_request(priv, wid_req, wid_req_len, wid_rsp, &wid_rsp_len);
    if (ret)
    {
        RDA5890_ERRP("rda5890_wid_request fail, ret = %d\n", ret);
        goto out;
    }

    ret = rda5890_check_wid_status(wid_rsp, wid_rsp_len, wid_msg_id);
    if (ret)
    {
        RDA5890_ERRP("check_wid_status fail, ret = %d\n", ret);
        goto out;
    }

out:
    return ret;
}
#endif

static int rda5890_generic_get_ulong(struct rda5890_private *priv,
                                     unsigned short wid, unsigned long *val)
{
    int ret;
    char wid_req[6];
    unsigned short wid_req_len = 6;
    char wid_rsp[32];
    unsigned short wid_rsp_len = 32;
    char wid_msg_id = priv->wid_msg_id++;
    char *ptr_payload;

    wid_req[0] = 'Q';
    wid_req[1] = wid_msg_id;

    wid_req[2] = (char)(wid_req_len&0x00FF);
    wid_req[3] = (char)((wid_req_len&0xFF00) >> 8);

    wid_req[4] = (char)(wid&0x00FF);
    wid_req[5] = (char)((wid&0xFF00) >> 8);

    ret = rda5890_wid_request(priv, wid_req, wid_req_len, wid_rsp, &wid_rsp_len);
    if (ret)
    {
        RDA5890_ERRP("rda5890_wid_request fail, ret = %d\n", ret);
        goto out;
    }

    ret = rda5890_check_wid_response(wid_rsp, wid_rsp_len, wid, wid_msg_id,
                                     sizeof(unsigned long), &ptr_payload);
    if (ret)
    {
        RDA5890_ERRP("check_wid_response fail, ret = %d\n", ret);
        goto out;
    }

    memcpy(val, ptr_payload, sizeof(unsigned long));

out:
    return ret;
}

int rda5890_generic_set_ulong(struct rda5890_private *priv,
                              unsigned short wid, unsigned long val)
{
    int ret;
    char wid_req[7 + sizeof(unsigned long)];
    unsigned short wid_req_len = 7 + sizeof(unsigned long);
    char wid_rsp[32];
    unsigned short wid_rsp_len = 32;
    char wid_msg_id = priv->wid_msg_id++;

    wid_req[0] = 'W';
    wid_req[1] = wid_msg_id;

    wid_req[2] = (char)(wid_req_len&0x00FF);
    wid_req[3] = (char)((wid_req_len&0xFF00) >> 8);

    wid_req[4] = (char)(wid&0x00FF);
    wid_req[5] = (char)((wid&0xFF00) >> 8);

    wid_req[6] = (char)(sizeof(unsigned long));
    memcpy(wid_req + 7, &val, sizeof(unsigned long));

    ret = rda5890_wid_request(priv, wid_req, wid_req_len, wid_rsp, &wid_rsp_len);
    if (ret)
    {
        RDA5890_ERRP("rda5890_wid_request fail, ret = %d\n", ret);
        goto out;
    }

    ret = rda5890_check_wid_status(wid_rsp, wid_rsp_len, wid_msg_id);
    if (ret)
    {
        RDA5890_ERRP("check_wid_status fail, ret = %d\n", ret);
        goto out;
    }

out:
    return ret;
}

int rda5890_generic_get_str(struct rda5890_private *priv,
                            unsigned short wid, unsigned char *val, unsigned char len)
{
    int ret;
    char wid_req[6];
    unsigned short wid_req_len = 6;
    char wid_rsp[RDA5890_MAX_WID_LEN];
    unsigned short wid_rsp_len = RDA5890_MAX_WID_LEN;
    char wid_msg_id = priv->wid_msg_id++;
    char *ptr_payload;

    wid_req[0] = 'Q';
    wid_req[1] = wid_msg_id;

    wid_req[2] = (char)(wid_req_len&0x00FF);
    wid_req[3] = (char)((wid_req_len&0xFF00) >> 8);

    wid_req[4] = (char)(wid&0x00FF);
    wid_req[5] = (char)((wid&0xFF00) >> 8);

    ret = rda5890_wid_request(priv, wid_req, wid_req_len, wid_rsp, &wid_rsp_len);
    if (ret)
    {
        RDA5890_ERRP("rda5890_wid_request fail, ret = %d\n", ret);
        goto out;
    }

    ret = rda5890_check_wid_response(wid_rsp, wid_rsp_len, wid, wid_msg_id,
                                     (char)len, &ptr_payload);
    if (ret)
    {
        RDA5890_ERRP("check_wid_response fail, ret = %d\n", ret);
        goto out;
    }

    memcpy(val, ptr_payload, len);

out:
    return ret;
}

int rda5890_generic_set_str(struct rda5890_private *priv,
                            unsigned short wid, unsigned char *val, unsigned char len)
{
    int ret;
    char wid_req[RDA5890_MAX_WID_LEN];
    unsigned short wid_req_len = 7 + len;
    char wid_rsp[RDA5890_MAX_WID_LEN];
    unsigned short wid_rsp_len = RDA5890_MAX_WID_LEN;
    char wid_msg_id = priv->wid_msg_id++;

    wid_req[0] = 'W';
    wid_req[1] = wid_msg_id;

    wid_req[2] = (char)(wid_req_len&0x00FF);
    wid_req[3] = (char)((wid_req_len&0xFF00) >> 8);

    wid_req[4] = (char)(wid&0x00FF);
    wid_req[5] = (char)((wid&0xFF00) >> 8);

    wid_req[6] = (char)(len);
    memcpy(wid_req + 7, val, len);

    ret = rda5890_wid_request(priv, wid_req, wid_req_len, wid_rsp, &wid_rsp_len);
    if (ret)
    {
        RDA5890_ERRP("rda5890_wid_request fail, ret = %d\n", ret);
        goto out;
    }

    ret = rda5890_check_wid_status(wid_rsp, wid_rsp_len, wid_msg_id);
    if (ret)
    {
        RDA5890_ERRP("check_wid_status fail, ret = %d\n", ret);
        goto out;
    }

out:
    return ret;
}

int rda5890_check_wid_response_unknown_len(
    char *wid_rsp, unsigned short wid_rsp_len,
    unsigned short wid, char wid_msg_id,
    char *payload_len, char **ptr_payload)
{
    unsigned short rsp_len;
    unsigned short rsp_wid;
    unsigned char msg_len;

    if (wid_rsp[0] != 'R')
    {
        RDA5890_ERRP("wid_rsp[0] != 'R'\n");
        return -EINVAL;
    }

    if (wid_rsp[1] != wid_msg_id)
    {
        RDA5890_ERRP("wid_msg_id not match\n");
        return -EINVAL;
    }

    if (wid_rsp_len < 4)
    {
        RDA5890_ERRP("wid_rsp_len < 4\n");
        return -EINVAL;
    }

    rsp_len = wid_rsp[2] | (wid_rsp[3] << 8);
    if (wid_rsp_len != rsp_len)
    {
        RDA5890_ERRP("wid_rsp_len not match, %d != %d\n", wid_rsp_len, rsp_len);
        return -EINVAL;
    }

    if (wid_rsp_len < 7)
    {
        RDA5890_ERRP("wid_rsp_len < 7\n");
        return -EINVAL;
    }

    rsp_wid = wid_rsp[4] | (wid_rsp[5] << 8);
    if (wid != rsp_wid)
    {
        RDA5890_ERRP("wid not match, 0x%04x != 0x%04x\n", wid, rsp_wid);
        return -EINVAL;
    }

    msg_len = wid_rsp[6];
    if (wid_rsp_len != msg_len + 7)
    {
        RDA5890_ERRP("msg_len not match, %d + 7 != %d\n", msg_len, wid_rsp_len);
        return -EINVAL;
    }

    *payload_len = msg_len;

    *ptr_payload = wid_rsp + 7;

    return 0;
}

int rda5890_get_str_unknown_len(struct rda5890_private *priv,
                                unsigned short wid, unsigned char *val, unsigned char *len)
{
    int ret;
    char wid_req[6];
    unsigned short wid_req_len = 6;
    char wid_rsp[RDA5890_MAX_WID_LEN];
    unsigned short wid_rsp_len = RDA5890_MAX_WID_LEN;
    char wid_msg_id = priv->wid_msg_id++;
    char *ptr_payload;

    wid_req[0] = 'Q';
    wid_req[1] = wid_msg_id;

    wid_req[2] = (char)(wid_req_len&0x00FF);
    wid_req[3] = (char)((wid_req_len&0xFF00) >> 8);

    wid_req[4] = (char)(wid&0x00FF);
    wid_req[5] = (char)((wid&0xFF00) >> 8);

    ret = rda5890_wid_request(priv, wid_req, wid_req_len, wid_rsp, &wid_rsp_len);
    if (ret)
    {
        RDA5890_ERRP("rda5890_wid_request fail, ret = %d\n", ret);
        goto out;
    }

    ret = rda5890_check_wid_response_unknown_len(
              wid_rsp, wid_rsp_len, wid, wid_msg_id, (char*)len, &ptr_payload);
    if (ret)
    {
        RDA5890_ERRP("check_wid_response fail, ret = %d\n", ret);
        goto out;
    }

    memcpy(val, ptr_payload, *len);

out:
    return ret;
}

int rda5890_start_scan(struct rda5890_private *priv)
{
    int ret;
    char wid_req[12];
    unsigned short wid_req_len = 12;
    char wid_rsp[32];
    unsigned short wid_rsp_len = 32;
    unsigned short wid;
    char wid_msg_id = priv->wid_msg_id++;

    wid_req[0] = 'W';
    wid_req[1] = wid_msg_id;

    wid_req[2] = (char)(wid_req_len&0x00FF);
    wid_req[3] = (char)((wid_req_len&0xFF00) >> 8);

    wid = WID_SITE_SURVEY;
    wid_req[4] = (char)(wid&0x00FF);
    wid_req[5] = (char)((wid&0xFF00) >> 8);

    wid_req[6] = (char)(0x01);
    wid_req[7] = (char)(0x01);

    wid = WID_START_SCAN_REQ;
    wid_req[8] = (char)(wid&0x00FF);
    wid_req[9] = (char)((wid&0xFF00) >> 8);

    wid_req[10] = (char)(0x01);
    wid_req[11] = (char)(0x01);

    ret = rda5890_wid_request(priv, wid_req, wid_req_len, wid_rsp, &wid_rsp_len);
    if (ret)
    {
        RDA5890_ERRP("rda5890_wid_request fail, ret = %d\n", ret);
        goto out;
    }

    ret = rda5890_check_wid_status(wid_rsp, wid_rsp_len, wid_msg_id);
    if (ret)
    {
        RDA5890_ERRP("check_wid_status fail, ret = %d\n", ret);
        goto out;
    }

out:
    return ret;
}

int rda5890_get_fw_ver(struct rda5890_private *priv, unsigned long *fw_ver)
{
    int ret;

    ret = rda5890_generic_get_ulong(priv, WID_SYS_FW_VER, fw_ver);
    if (ret)
    {
        goto out;
    }

    RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_TRACE,
                   "Get FW_VER 0x%08lx\n", *fw_ver);

out:
    return ret;
}

int rda5890_get_mac_addr(struct rda5890_private *priv, unsigned char *mac_addr)
{
    int ret;

    ret = rda5890_generic_get_str(priv, WID_MAC_ADDR, mac_addr, ETH_ALEN);
    if (ret)
    {
        goto out;
    }

    RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_TRACE,
                   "STA MAC Address [%02x:%02x:%02x:%02x:%02x:%02x]\n",
                   mac_addr[0], mac_addr[1], mac_addr[2],
                   mac_addr[3], mac_addr[4], mac_addr[5]);
out:
    return ret;
}

/* support only one bss per packet for now */
int rda5890_get_scan_results(struct rda5890_private *priv,
                             struct rda5890_bss_descriptor *bss_desc)
{
    int ret;
    int count;
    unsigned char len;
    unsigned char buf[sizeof(struct rda5890_bss_descriptor)*RDA5890_MAX_NETWORK_NUM + 2];

    ret = rda5890_get_str_unknown_len(priv, WID_SITE_SURVEY_RESULTS,
                                      buf, &len);
    if (ret)
    {
        return ret;
    }
    RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_TRACE,
                   "Get Scan Result, len = %d\n", len);

    if ((len - 2) % sizeof(struct rda5890_bss_descriptor))
    {
        RDA5890_ERRP("Scan Result len not correct, %d\n", len);
        return -EINVAL;
    }

    count = (len - 2) / sizeof(struct rda5890_bss_descriptor);

    memcpy(bss_desc, buf + 2, (len - 2));

    return count;
}

int rda5890_get_bssid(struct rda5890_private *priv, unsigned char *bssid)
{
    int ret;

    ret = rda5890_generic_get_str(priv, WID_BSSID, bssid, ETH_ALEN);
    if (ret)
    {
        goto out;
    }

    RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_TRACE,
                   "Get BSSID [%02x:%02x:%02x:%02x:%02x:%02x]\n",
                   bssid[0], bssid[1], bssid[2],
                   bssid[3], bssid[4], bssid[5]);
out:
    return ret;
}

int rda5890_get_channel(struct rda5890_private *priv, unsigned char *channel)
{
    int ret;

    ret = rda5890_generic_get_uchar(priv, WID_CURRENT_CHANNEL, channel);
    if (ret)
    {
        goto out;
    }

    RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_TRACE,
                   "Get Channel %d\n", *channel);

out:
    return ret;
}

int rda5890_get_rssi(struct rda5890_private *priv, unsigned char *rssi)
{
    int ret;

    ret = rda5890_generic_get_uchar(priv, WID_RSSI, rssi);
    if (ret)
    {
        goto out;
    }

    RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_TRACE,
                   "Get RSSI %d\n", *(char*)rssi);

out:
    return ret;
}

int rda5890_set_mac_addr(struct rda5890_private *priv, unsigned char *mac_addr)
{
    int ret;

    ret = rda5890_generic_set_str(priv, WID_MAC_ADDR, mac_addr, ETH_ALEN);
    if (ret)
    {
        goto out;
    }

    RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_TRACE,
                   "Set STA MAC Address [%02x:%02x:%02x:%02x:%02x:%02x]\n",
                   mac_addr[0], mac_addr[1], mac_addr[2],
                   mac_addr[3], mac_addr[4], mac_addr[5]);
out:
    return ret;
}

int rda5890_set_ssid(struct rda5890_private *priv,
                     unsigned char *ssid, unsigned char ssid_len)
{
    int ret;

    RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_TRACE,
                   "Set SSID: %s, len = %d\n", ssid, ssid_len);

    ret = rda5890_generic_set_str(priv, WID_SSID, ssid, ssid_len);
    if (ret)
    {
        goto out;
    }

    RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_TRACE,
                   "Set SSID Done\n");

out:
    return ret;
}

int rda5890_set_imode(struct rda5890_private *priv, unsigned char imode)
{
    int ret;

    RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_TRACE,
                   "Set IMode 0x%02x\n", imode);

    ret = rda5890_generic_set_uchar(priv, WID_802_11I_MODE, imode);
    if (ret)
    {
        goto out;
    }

    RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_TRACE,
                   "Set IMode Done\n");

out:
    return ret;
}

int rda5890_set_authtype(struct rda5890_private *priv, unsigned char authtype)
{
    int ret;

    RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_TRACE,
                   "Set AuthType 0x%02x\n", authtype);

    ret = rda5890_generic_set_uchar(priv, WID_AUTH_TYPE, authtype);
    if (ret)
    {
        goto out;
    }

    RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_TRACE,
                   "Set AuthType Done\n");

out:
    return ret;
}

int rda5890_set_wepkey(struct rda5890_private *priv,
                       unsigned short index, unsigned char *key, unsigned char key_len)
{
    int ret;
    unsigned char key_str[26 + 1]; // plus 1 for debug print
    unsigned char key_str_len;

    if (key_len == KEY_LEN_WEP_40)
    {
        sprintf(key_str, "%02x%02x%02x%02x%02x\n",
                key[0], key[1], key[2], key[3], key[4]);
        key_str_len = 10;
        key_str[key_str_len] = '\0';
    }
    else if (key_len == KEY_LEN_WEP_104)
    {
        sprintf(key_str, "%02x%02x%02x%02x%02x"
                "%02x%02x%02x%02x%02x"
                "%02x%02x%02x\n",
                key[0], key[1], key[2], key[3], key[4],
                key[5], key[6], key[7], key[8], key[9],
                key[10], key[11], key[12]);
        key_str_len = 26;
        key_str[key_str_len] = '\0';
    }
    else
    {
        RDA5890_ERRP("Error in WEP Key length %d\n", key_len);
        ret = -EINVAL;
        goto out;
    }

    RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_TRACE,
                   "Set WEP KEY[%d]: %s\n", index, key_str);

    ret = rda5890_generic_set_str(priv,
                                  (WID_WEP_KEY_VALUE0 + index), key_str, key_str_len);
    if (ret)
    {
        goto out;
    }

    RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_TRACE,
                   "Set WEP KEY[%d] Done\n", index);

out:
    return ret;
}

static void dump_key(unsigned char *key, unsigned char key_len)
{
    RDA5890_DBGP("%02x %02x %02x %02x  %02x %02x %02x %02x\n",
                 key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7]);
    RDA5890_DBGP("%02x %02x %02x %02x  %02x %02x %02x %02x\n",
                 key[8], key[9], key[10], key[11], key[12], key[13], key[14], key[15]);
    if (key_len > 16)
        RDA5890_DBGP("%02x %02x %02x %02x  %02x %02x %02x %02x\n",
                     key[16], key[17], key[18], key[19], key[20], key[21], key[22], key[23]);
    if (key_len > 24)
        RDA5890_DBGP("%02x %02x %02x %02x  %02x %02x %02x %02x\n",
                     key[24], key[25], key[26], key[27], key[28], key[29], key[30], key[31]);
}

int rda5890_set_ptk(struct rda5890_private *priv,
                    unsigned char *key, unsigned char key_len)
{
    int ret;
    unsigned char key_str[32 + ETH_ALEN + 1];
    unsigned char key_str_len = key_len + ETH_ALEN + 1;

    RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_TRACE,
                   "Set PTK: len = %d\n", key_len);
    if (RDA5890_DBGLA(RDA5890_DA_WID, RDA5890_DL_VERB))
        dump_key(key, key_len);

    if (priv->connect_status != MAC_CONNECTED)
    {
        RDA5890_ERRP("Adding PTK while not connected\n");
        ret = -EINVAL;
        goto out;
    }

    /*----------------------------------------*/
    /*    STA Addr  | KeyLength |   Key       */
    /*----------------------------------------*/
    /*       6      |     1     |  KeyLength  */
    /*----------------------------------------*/

    /*---------------------------------------------------------*/
    /*                      key                                */
    /*---------------------------------------------------------*/
    /* Temporal Key    | Rx Micheal Key    |   Tx Micheal Key  */
    /*---------------------------------------------------------*/
    /*    16 bytes     |      8 bytes      |       8 bytes     */
    /*---------------------------------------------------------*/

    memcpy(key_str, priv->curbssparams.bssid, ETH_ALEN);
    key_str[6] = key_len;
    memcpy(key_str + 7, key, 16);

    /* swap TX MIC and RX MIC, rda5890 need RX MIC to be ahead */
    if(key_len > 16)
    {
        memcpy(key_str + 7 + 16, key + 24, 8);
        memcpy(key_str + 7 + 24, key + 16, 8);
    }

    if(priv->is_wapi)
        ret = rda5890_generic_set_str(priv,
                                      WID_ADD_WAPI_PTK, key_str, key_str_len);
    else
        ret = rda5890_generic_set_str(priv,
                                      WID_ADD_PTK, key_str, key_str_len);
    if (ret)
    {
        goto out;
    }

    RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_TRACE,
                   "Set PTK Done\n");

out:
    return ret;
}

int rda5890_set_gtk(struct rda5890_private *priv, unsigned char key_id,
                    unsigned char *key_rsc, unsigned char key_rsc_len,
                    unsigned char *key, unsigned char key_len)
{
    int ret;
    unsigned char key_str[32 + ETH_ALEN + 8 + 2];
    unsigned char key_str_len = key_len + ETH_ALEN + 8 + 2;

    /*---------------------------------------------------------*/
    /*    STA Addr  | KeyRSC | KeyID | KeyLength |   Key       */
    /*---------------------------------------------------------*/
    /*       6      |   8    |   1   |     1     |  KeyLength  */
    /*---------------------------------------------------------*/

    /*-------------------------------------*/
    /*                      key            */
    /*-------------------------------------*/
    /* Temporal Key    | Rx Micheal Key    */
    /*-------------------------------------*/
    /*    16 bytes     |      8 bytes      */
    /*-------------------------------------*/

    RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_TRACE,
                   "Set GTK: len = %d\n", key_len);
    if (RDA5890_DBGLA(RDA5890_DA_WID, RDA5890_DL_VERB))
        dump_key(key, key_len);

    if (priv->connect_status != MAC_CONNECTED)
    {
        RDA5890_ERRP("Adding PTK while not connected\n");
        ret = -EINVAL;
        goto out;
    }

    memcpy(key_str, priv->curbssparams.bssid, ETH_ALEN);
    memcpy(key_str + 6, key_rsc, key_rsc_len);
    key_str[14] = key_id;
    key_str[15] = key_len;
    memcpy(key_str + 16, key, 16);

    /* swap TX MIC and RX MIC, rda5890 need RX MIC to be ahead */
    if(key_len > 16)
    {
        //memcpy(key_str + 16 + 16, key + 16, key_len - 16);
        memcpy(key_str + 16 + 16, key + 24, 8);
        memcpy(key_str + 16 + 24, key + 16, 8);
    }

    if(priv->is_wapi)
        ret = rda5890_generic_set_str(priv,
                                      WID_ADD_WAPI_RX_GTK, key_str, key_str_len);
    else
        ret = rda5890_generic_set_str(priv,
                                      WID_ADD_RX_GTK, key_str, key_str_len);
    if (ret)
    {
        goto out;
    }

    RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_TRACE,
                   "Set GTK Done\n");

out:
    return ret;
}

int rda5890_set_pm_mode(struct rda5890_private *priv, unsigned char pm_mode)
{
    int ret;

    RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_TRACE,
                   "Set PM Mode 0x%02x\n", pm_mode);

    ret = rda5890_generic_set_uchar(priv, WID_POWER_MANAGEMENT, pm_mode);
    if (ret)
    {
        goto out;
    }

    RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_TRACE,
                   "Set PM Mode Done\n");

out:
    return ret;
}

int rda5890_set_preasso_sleep(struct rda5890_private *priv, unsigned int preasso_sleep)
{
    int ret;

    RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_TRACE,
                   "Set Preasso Sleep 0x%08x\n", preasso_sleep);

    ret = rda5890_generic_set_ulong(priv, WID_PREASSO_SLEEP, preasso_sleep);
    if (ret)
    {
        goto out;
    }

    RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_TRACE,
                   "Set Preasso Sleep Done\n");

out:
    return ret;
}

