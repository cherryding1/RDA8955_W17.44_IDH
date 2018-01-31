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



#include <linux/moduleparam.h>
#include <linux/delay.h>
#include <linux/etherdevice.h>
#include <linux/netdevice.h>
#include <linux/if_arp.h>
#include <linux/kthread.h>
#include <linux/kfifo.h>
#include <linux/stddef.h>

#include <net/iw_handler.h>
#include <net/ieee80211.h>

#include "rda5890_defs.h"
#include "rda5890_dev.h"
#include "rda5890_ioctl.h"
#include "rda5890_wid.h"
#include "rda5890_wext.h"
#include "rda5890_txrx.h"

int rda5890_sleep_flags = RDA_SLEEP_ENABLE | RDA_SLEEP_PREASSO;

static void rda5890_init_pm(struct rda5890_private *priv)
{
    if (rda5890_sleep_flags & RDA_SLEEP_ENABLE)
        rda5890_set_pm_mode(priv, 2);
    if (rda5890_sleep_flags & RDA_SLEEP_PREASSO)
        rda5890_set_preasso_sleep(priv, 0x01000020);
}

/**
 *  @brief This function opens the ethX or mshX interface
 *
 *  @param dev     A pointer to net_device structure
 *  @return        0 or -EBUSY if monitor mode active
 */
static int rda5890_dev_open(struct net_device *dev)
{
    struct rda5890_private *priv = (struct rda5890_private *) dev->priv ;
    int ret = 0;

    RDA5890_DBGLAP(RDA5890_DA_ETHER, RDA5890_DL_DEBUG,
                   "%s >>>\n", __func__);

    if (priv->connect_status == MAC_CONNECTED)
        netif_carrier_on(dev);
    else
        netif_carrier_off(dev);

    RDA5890_DBGLAP(RDA5890_DA_ETHER, RDA5890_DL_DEBUG,
                   "%s <<<\n", __func__);
    return ret;
}

/**
 *  @brief This function closes the ethX interface
 *
 *  @param dev     A pointer to net_device structure
 *  @return        0
 */
static int rda5890_eth_stop(struct net_device *dev)
{
    //struct rda5890_private *priv = (struct rda5890_private *) dev->priv;

    RDA5890_DBGLAP(RDA5890_DA_ETHER, RDA5890_DL_DEBUG,
                   "%s >>>\n", __func__);

    netif_stop_queue(dev);

    RDA5890_DBGLAP(RDA5890_DA_ETHER, RDA5890_DL_DEBUG,
                   "%s <<<\n", __func__);
    return 0;
}

static void rda5890_tx_timeout(struct net_device *dev)
{
    //struct rda5890_private *priv = (struct rda5890_private *) dev->priv;

    RDA5890_DBGLAP(RDA5890_DA_ETHER, RDA5890_DL_DEBUG,
                   "%s >>>\n", __func__);

    RDA5890_DBGLAP(RDA5890_DA_ETHER, RDA5890_DL_DEBUG,
                   "%s <<<\n", __func__);
}

/**
 *  @brief This function returns the network statistics
 *
 *  @param dev     A pointer to struct lbs_private structure
 *  @return        A pointer to net_device_stats structure
 */
static struct net_device_stats *rda5890_get_stats(struct net_device *dev)
{
    struct rda5890_private *priv = (struct rda5890_private *) dev->priv;

    RDA5890_DBGLAP(RDA5890_DA_ETHER, RDA5890_DL_DEBUG,
                   "%s >>>\n", __func__);

    RDA5890_DBGLAP(RDA5890_DA_ETHER, RDA5890_DL_DEBUG,
                   "%s <<<\n", __func__);
    return &priv->stats;
}

static int rda5890_set_mac_address(struct net_device *dev, void *addr)
{
    int ret = 0;
    struct rda5890_private *priv = (struct rda5890_private *) dev->priv;
    struct sockaddr *phwaddr = addr;

    RDA5890_DBGLAP(RDA5890_DA_ETHER, RDA5890_DL_DEBUG,
                   "%s >>>\n", __func__);

    ret = rda5890_set_mac_addr(priv, phwaddr->sa_data);
    if (ret)
    {
        goto done;
    }
    memcpy(priv->dev->dev_addr, phwaddr->sa_data, ETH_ALEN);

done:
    RDA5890_DBGLAP(RDA5890_DA_ETHER, RDA5890_DL_DEBUG,
                   "%s <<<\n", __func__);
    return ret;
}

static void rda5890_set_multicast_list(struct net_device *dev)
{
    //struct rda5890_private *priv = dev->priv;

    RDA5890_DBGLAP(RDA5890_DA_ETHER, RDA5890_DL_DEBUG,
                   "%s >>>\n", __func__);

    //schedule_work(&priv->mcast_work);

    RDA5890_DBGLAP(RDA5890_DA_ETHER, RDA5890_DL_DEBUG,
                   "%s <<<\n", __func__);
}

/**
 *  @brief This function checks the conditions and sends packet to IF
 *  layer if everything is ok.
 *
 *  @param priv    A pointer to struct lbs_private structure
 *  @param skb     A pointer to skb which includes TX packet
 *  @return        0 or -1
 */
int rda5890_hard_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
    struct rda5890_private *priv = dev->priv;

    return rda5890_data_tx(priv, skb, dev);
}

static int rda5890_ioctl(struct net_device *dev, struct ifreq *rq, int cmd)
{
    int ret = 0;
    struct rda5890_private *priv = netdev_priv(dev);
    unsigned long value;
    char in_buf[MAX_CMD_LEN + 4], out_buf[MAX_CMD_LEN + 4];
    unsigned short in_len, out_len, out_len_ret;

    RDA5890_DBGLAP(RDA5890_DA_ETHER, RDA5890_DL_DEBUG,
                   "%s >>>, cmd = %d\n", __func__, cmd);

    switch (cmd)
    {
        case IOCTL_RDA5890_GET_MAGIC:
            RDA5890_DBGLAP(RDA5890_DA_ETHER, RDA5890_DL_DEBUG,
                           "IOCTL_RDA5890_GET_MAGIC\n");
            value = RDA5890_MAGIC;
            if (copy_to_user(rq->ifr_data, &value, sizeof(value)))
                ret = -EFAULT;
            break;
        case IOCTL_RDA5890_GET_DRIVER_VER:
            RDA5890_DBGLAP(RDA5890_DA_ETHER, RDA5890_DL_DEBUG,
                           "IOCTL_RDA5890_GET_DRIVER_VER\n");
            value = RDA5890_SDIOWIFI_VER_MAJ << 16 |
                    RDA5890_SDIOWIFI_VER_MIN << 8 |
                    RDA5890_SDIOWIFI_VER_BLD;
            if (copy_to_user(rq->ifr_data, &value, sizeof(value)))
                ret = -EFAULT;
            break;
        case IOCTL_RDA5890_MAC_GET_FW_VER:
            RDA5890_DBGLAP(RDA5890_DA_ETHER, RDA5890_DL_DEBUG,
                           "IOCTL_RDA5890_MAC_GET_FW_VER\n");
            ret = rda5890_get_fw_ver(priv, &value);
            if (ret)
                break;
            if (copy_to_user(rq->ifr_data, &value, sizeof(value)))
                ret = -EFAULT;
            break;
        case IOCTL_RDA5890_MAC_WID:
            RDA5890_DBGLAP(RDA5890_DA_ETHER, RDA5890_DL_DEBUG,
                           "IOCTL_RDA5890_MAC_WID\n");
            if (copy_from_user(in_buf, rq->ifr_data, 4))
            {
                ret = -EFAULT;
                break;
            }
            in_len = (unsigned short)(in_buf[0] + (in_buf[1] << 8));
            out_len = (unsigned short)(in_buf[2] + (in_buf[3] << 8));
            RDA5890_DBGLAP(RDA5890_DA_ETHER, RDA5890_DL_DEBUG,
                           " in_len = %d, out_len = %d\n", in_len, out_len);
            if (copy_from_user(in_buf, rq->ifr_data, 4 + in_len))
            {
                ret = -EFAULT;
                break;
            }
            out_len_ret = MAX_CMD_LEN;
            ret = rda5890_wid_request(priv, in_buf + 4, in_len, out_buf + 4, &out_len_ret);
            if (ret)
            {
                RDA5890_ERRP("rda5890_wid_request, ret = %d\n", ret);
                break;
            }
            RDA5890_DBGLAP(RDA5890_DA_ETHER, RDA5890_DL_DEBUG,
                           "  out_len_ret = %d\n", out_len_ret);
            if (out_len_ret > out_len)
            {
                RDA5890_ERRP("No enough buf for wid response\n");
                ret = -ENOMEM;
                break;
            }
            out_buf[2] = (char)(out_len_ret&0x00FF);
            out_buf[3] = (char)((out_len_ret&0xFF00) >> 8);
            if (copy_to_user(rq->ifr_data, out_buf, 4 + out_len_ret))
                ret = -EFAULT;
            break;
        case IOCTL_RDA5890_SET_WAPI_ASSOC_IE:
            if(copy_from_user(in_buf, rq->ifr_data, 100))
            {
                ret = -EFAULT;
                break;
            }
            RDA5890_DBGLAP(RDA5890_DA_WID, RDA5890_DL_TRACE,
                           "IOCTL_RDA5890_SET_WAPI_ASSOC_IE is  %x %x %x %x \n",
                           in_buf[0], in_buf[1],in_buf[2],in_buf[3]);
            rda5890_generic_set_str(priv, WID_WAPI_ASSOC_IE, in_buf ,100);
            break;
        case IOCTL_RDA5890_GET_WAPI_ASSOC_IE:
            rda5890_generic_get_str(priv, WID_WAPI_ASSOC_IE, out_buf, 100);
            if (copy_to_user(rq->ifr_data, out_buf, 100))
                ret = -EFAULT;
            break;
        default:
            RDA5890_ERRP("unknown cmd 0x%x\n", cmd);
            ret = -EFAULT;
            break;
    }

    RDA5890_DBGLAP(RDA5890_DA_ETHER, RDA5890_DL_DEBUG,
                   "%s <<<\n", __func__);
    return ret;
}

static int rda5890_init_adapter(struct rda5890_private *priv)
{
    int ret = 0;
    size_t bufsize;
    int i;

    mutex_init(&priv->lock);
    mutex_init(&priv->wid_lock);
    atomic_set(&priv->sleep_flag, 0);
    priv->wid_pending = 0;
    priv->wid_msg_id = 0;

    /* Allocate buffer to store the BSSID list */
    bufsize = RDA5890_MAX_NETWORK_NUM * sizeof(struct bss_descriptor);
    priv->networks = kzalloc(bufsize, GFP_KERNEL);
    if (!priv->networks)
    {
        RDA5890_ERRP("Out of memory allocating beacons\n");
        ret = -1;
        goto out;
    }

    /* Initialize scan result lists */
    INIT_LIST_HEAD(&priv->network_free_list);
    INIT_LIST_HEAD(&priv->network_list);
    for (i = 0; i < RDA5890_MAX_NETWORK_NUM; i++)
    {
        list_add_tail(&priv->networks[i].list,
                      &priv->network_free_list);
    }
    priv->scan_running = 0;

    /* Initialize delayed work workers and thread */
    priv->work_thread = create_singlethread_workqueue("rda5890_worker");
    INIT_DELAYED_WORK(&priv->scan_work, rda5890_scan_worker);
    INIT_DELAYED_WORK(&priv->assoc_work, rda5890_assoc_worker);
    INIT_DELAYED_WORK(&priv->assoc_done_work, rda5890_assoc_done_worker);

    /* Initialize status */
    priv->connect_status = MAC_DISCONNECTED;
    memset(&priv->curbssparams, 0, sizeof(priv->curbssparams));
    memset(&priv->wstats, 0, sizeof(priv->wstats));

    /* Initialize sec related status */
    priv->assoc_flags = 0;
    priv->secinfo.auth_mode = IW_AUTH_ALG_OPEN_SYSTEM;
    memset(&priv->wep_keys, 0, sizeof(priv->wep_keys));
    memset(&priv->wpa_ie[0], 0, sizeof(priv->wpa_ie));
    priv->wpa_ie_len = 0;

out:
    return ret;
}

static void rda5890_free_adapter(struct rda5890_private *priv)
{
    cancel_delayed_work_sync(&priv->assoc_done_work);
    cancel_delayed_work_sync(&priv->assoc_work);
    cancel_delayed_work_sync(&priv->scan_work);
    destroy_workqueue(priv->work_thread);

    kfree(priv->networks);
    priv->networks = NULL;
}

/**
 * @brief This function adds the card. it will probe the
 * card, allocate the lbs_priv and initialize the device.
 *
 *  @param card    A pointer to card
 *  @return        A pointer to struct lbs_private structure
 */
struct rda5890_private *rda5890_add_card(void *card, struct device *dmdev)
{
    struct net_device *dev = NULL;
    struct rda5890_private *priv = NULL;

    RDA5890_DBGLAP(RDA5890_DA_ETHER, RDA5890_DL_DEBUG,
                   "%s >>>\n", __func__);

    /* Allocate an Ethernet device and register it */
    dev = alloc_etherdev(sizeof(struct rda5890_private));
    if (!dev)
    {
        RDA5890_ERRP("alloc_etherdev failed\n");
        goto done;
    }
    priv = dev->priv;

    if (rda5890_init_adapter(priv))
    {
        RDA5890_ERRP("rda5890_init_adapter failed\n");
        goto err_init_adapter;
    }

    priv->dev = dev;
    priv->card = card;

    /* Setup the OS Interface to our functions */
    dev->open = rda5890_dev_open;
    dev->hard_start_xmit = rda5890_hard_start_xmit;
    dev->stop = rda5890_eth_stop;
    dev->set_mac_address = rda5890_set_mac_address;
    dev->tx_timeout = rda5890_tx_timeout;
    dev->get_stats = rda5890_get_stats;
    dev->watchdog_timeo = 5 * HZ;
    dev->do_ioctl = rda5890_ioctl;
    //dev->ethtool_ops = &lbs_ethtool_ops;
#ifdef  WIRELESS_EXT
    dev->wireless_handlers = (struct iw_handler_def *)&rda5890_wext_handler_def;
#endif
    dev->flags |= IFF_BROADCAST | IFF_MULTICAST;
    dev->set_multicast_list = rda5890_set_multicast_list;

    SET_NETDEV_DEV(dev, dmdev);

    goto done;

err_init_adapter:
    rda5890_free_adapter(priv);
    free_netdev(dev);
    priv = NULL;

done:
    RDA5890_DBGLAP(RDA5890_DA_ETHER, RDA5890_DL_DEBUG,
                   "%s <<<\n", __func__);
    return priv;
}

void rda5890_remove_card(struct rda5890_private *priv)
{
    struct net_device *dev = priv->dev;

    RDA5890_DBGLAP(RDA5890_DA_ETHER, RDA5890_DL_DEBUG,
                   "%s >>>\n", __func__);

    dev = priv->dev;

    rda5890_free_adapter(priv);

    priv->dev = NULL;
    free_netdev(dev);

    RDA5890_DBGLAP(RDA5890_DA_ETHER, RDA5890_DL_DEBUG,
                   "%s <<<\n", __func__);
}

int rda5890_start_card(struct rda5890_private *priv)
{
    struct net_device *dev = priv->dev;
    int ret = 0;
    unsigned char mac_addr[ETH_ALEN];

    RDA5890_DBGLAP(RDA5890_DA_ETHER, RDA5890_DL_DEBUG,
                   "%s >>>\n", __func__);

    ret = rda5890_get_mac_addr(priv, mac_addr);
    if (ret)
    {
        goto done;
    }
    memcpy(priv->dev->dev_addr, mac_addr, ETH_ALEN);

    if (register_netdev(dev))
    {
        RDA5890_ERRP("register_netdev failed\n");
        goto done;
    }

    rda5890_indicate_disconnected(priv);

    rda5890_init_pm(priv);
done:
    RDA5890_DBGLAP(RDA5890_DA_ETHER, RDA5890_DL_DEBUG,
                   "%s <<<\n", __func__);
    return ret;
}

void rda5890_stop_card(struct rda5890_private *priv)
{
    struct net_device *dev = priv->dev;

    RDA5890_DBGLAP(RDA5890_DA_ETHER, RDA5890_DL_DEBUG,
                   "%s >>>\n", __func__);

    rda5890_indicate_disconnected(priv);

    unregister_netdev(dev);

    RDA5890_DBGLAP(RDA5890_DA_ETHER, RDA5890_DL_DEBUG,
                   "%s <<<\n", __func__);
}

