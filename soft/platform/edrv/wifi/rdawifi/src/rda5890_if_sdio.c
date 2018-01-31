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
#include <linux/firmware.h>
#include <linux/netdevice.h>
#include <linux/delay.h>
#include <linux/mmc/card.h>
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/sdio_ids.h>

#include "rda5890_defs.h"
#include "rda5890_dev.h"
#include "rda5890_if_sdio.h"
#include "rda5890_wid.h"
#include "rda5890_debugfs.h"

int rda5890_dbg_level = RDA5890_DL_TRACE;
int rda5890_dbg_area = RDA5890_DA_MAIN
                       | RDA5890_DA_SDIO
                       | RDA5890_DA_ETHER
                       | RDA5890_DA_WID
                       //| RDA5890_DA_WEXT
                       //| RDA5890_DA_TXRX
                       //| RDA5890_DA_PM
                       ;

/* Module parameters */
module_param_named(debug_level, rda5890_dbg_level, int, 0644);
module_param_named(debug_area, rda5890_dbg_area, int, 0644);
int sdio_test_flag = 0;
module_param_named(sdio_test, sdio_test_flag, int, 0644);
module_param_named(sleep_flags, rda5890_sleep_flags, int, 0644);

#define SDIO_VENDOR_ID_RDA5890        0x5449
#define SDIO_DEVICE_ID_RDA5890        0x0145

static const struct sdio_device_id if_sdio_ids[] =
{
    { SDIO_DEVICE(SDIO_VENDOR_ID_RDA5890, SDIO_DEVICE_ID_RDA5890) },
    { /* end: all zeroes */                     },
};

MODULE_DEVICE_TABLE(sdio, if_sdio_ids);

struct if_sdio_packet
{
    struct if_sdio_packet   *next;
    u16         nb;
    u8          buffer[0] __attribute__((aligned(4)));
};

struct if_sdio_card
{
    struct sdio_func    *func;
    struct rda5890_private  *priv;

    u8          buffer[65536];

    spinlock_t      lock;
    struct if_sdio_packet   *packets;
    struct work_struct  packet_worker;
};

int if_sdio_card_to_host(struct if_sdio_card *card)
{
    int ret = 0;
    struct rda5890_private *priv = card->priv;
    u8 size_l, size_h;
    u16 size, chunk;

    RDA5890_DBGLAP(RDA5890_DA_SDIO, RDA5890_DL_DEBUG,
                   "%s >>>\n", __func__);

    size_l = sdio_readb(card->func, IF_SDIO_AHB2SDIO_PKTLEN_L, &ret);
    if (ret)
    {
        RDA5890_ERRP("read PKTLEN_L reg fail\n");
        goto out;
    }

    size_h = sdio_readb(card->func, IF_SDIO_AHB2SDIO_PKTLEN_H, &ret);
    if (ret)
    {
        RDA5890_ERRP("read PKTLEN_H reg fail\n");
        goto out;
    }

    size = (size_l | ((size_h & 0x7f) << 8)) * 4;
    if (size < 4)
    {
        RDA5890_ERRP("invalid packet size (%d bytes) from firmware\n", size);
        ret = -EINVAL;
        goto out;
    }

    /* alignment is handled on firmside */
    //chunk = sdio_align_size(card->func, size);
    chunk = size;

    RDA5890_DBGLAP(RDA5890_DA_SDIO, RDA5890_DL_NORM,
                   "if_sdio_card_to_host, size = %d, aligned size = %d\n", size, chunk);

    /* TODO: handle multiple packets here */
    ret = sdio_readsb(card->func, card->buffer, IF_SDIO_FUN1_FIFO_RD, chunk);
    if (ret)
    {
        RDA5890_ERRP("sdio_readsb fail, ret = %d\n", ret);
        goto out;
    }

    RDA5890_DBGLAP(RDA5890_DA_SDIO, RDA5890_DL_VERB,
                   "if_sdio_card_to_host, read done\n");

    if (sdio_test_flag)
    {
        rda5890_sdio_test_card_to_host(card->buffer, chunk);
        goto out;
    }

    /* TODO: this chunk size need to be handled here */
    rda5890_card_to_host(priv, card->buffer, chunk);

out:
    RDA5890_DBGLAP(RDA5890_DA_SDIO, RDA5890_DL_DEBUG,
                   "if_sdio_card_to_host >>>\n");
    return ret;
}

static int if_sdio_wakeup_card(struct if_sdio_card *card)
{
    struct rda5890_private *priv = card->priv;
    int ret = 0;

    sdio_claim_host(card->func);
    sdio_writeb(card->func, 1, IF_SDIO_FUN1_INT_TO_DEV, &ret);
    atomic_set(&priv->sleep_flag, 0);
    sdio_release_host(card->func);
    if (ret)
    {
        RDA5890_ERRP("write FUN1_INT_TO_DEV reg fail\n");
        goto out;
    }

    // wait 15ms, hardware need 13ms to wakeup
    mdelay(15);
out:
    return ret;
}

static void if_sdio_host_to_card_worker(struct work_struct *work)
{
    struct if_sdio_card *card;
    struct rda5890_private *priv;
    struct if_sdio_packet *packet;
    int ret;
    unsigned long flags;
    u16 size;
    u8 size_l, size_h;
#define SDIO_HOST_WRITE_BATCH_SIZE   512
    u16 bytes_left, offset, batch;

    card = container_of(work, struct if_sdio_card, packet_worker);
    priv = card->priv;

    while (1)
    {
        spin_lock_irqsave(&card->lock, flags);
        packet = card->packets;
        if (packet)
            card->packets = packet->next;
        spin_unlock_irqrestore(&card->lock, flags);

        if (!packet)
            break;

        RDA5890_DBGLAP(RDA5890_DA_SDIO, RDA5890_DL_DEBUG,
                       "if_sdio_host_to_card_worker, send one packet, size = %d\n", packet->nb);

        /* check sleep status first */
        if (atomic_read(&priv->sleep_flag))
        {
            /* device is sleeping, wakeup here */
            RDA5890_DBGLAP(RDA5890_DA_PM, RDA5890_DL_TRACE,
                           "if_sdio_host_to_card_worker, sleeping, wakeup now\n");

            ret = if_sdio_wakeup_card(card);
            if (ret)
            {
                RDA5890_ERRP("wakeup card fail\n");
                goto out;
            }
            RDA5890_DBGLAP(RDA5890_DA_PM, RDA5890_DL_TRACE,
                           "if_sdio_host_to_card_worker, wakeup done\n");
        }

        sdio_claim_host(card->func);

        /* write length */
        size = packet->nb/4;
        size_l = (u8)(size & 0xff);
        size_h = (u8)((size >> 8) & 0x7f);
        size_h |= 0x80;

        sdio_writeb(card->func, size_l, IF_SDIO_SDIO2AHB_PKTLEN_L, &ret);
        if (ret)
        {
            RDA5890_ERRP("write PKTLEN_L reg fail\n");
            goto release;
        }
        sdio_writeb(card->func, size_h, IF_SDIO_SDIO2AHB_PKTLEN_H, &ret);
        if (ret)
        {
            RDA5890_ERRP("write PKTLEN_H reg fail\n");
            goto release;
        }

        /* write data */
        bytes_left = packet->nb;
        offset = 0;
        while(bytes_left)
        {
            batch = (bytes_left < SDIO_HOST_WRITE_BATCH_SIZE)?
                    bytes_left:SDIO_HOST_WRITE_BATCH_SIZE;
            ret = sdio_writesb(card->func, IF_SDIO_FUN1_FIFO_WR,
                               packet->buffer + offset, batch);
            if (ret)
            {
                RDA5890_ERRP("sdio_writesb fail, ret = %d\n", ret);
                goto release;
            }
            RDA5890_DBGLAP(RDA5890_DA_SDIO, RDA5890_DL_DEBUG,
                           "write batch %d, offset = %d\n", batch, offset);
            offset += batch;
            bytes_left -= batch;
        }

        RDA5890_DBGLAP(RDA5890_DA_SDIO, RDA5890_DL_DEBUG,
                       "if_sdio_host_to_card_worker, send one packet done\n");
release:
        sdio_release_host(card->func);
out:
        kfree(packet);
    }
}

/*******************************************************************/
/* RDA5890 callbacks                                              */
/*******************************************************************/

static int if_sdio_host_to_card(struct rda5890_private *priv,
                                u8 *buf, u16 nb)
{
    int ret = 0;
    struct if_sdio_card *card;
    struct if_sdio_packet *packet, *cur;
    u16 size;
    unsigned long flags;

    RDA5890_DBGLAP(RDA5890_DA_SDIO, RDA5890_DL_DEBUG,
                   "%s >>>\n", __func__);

    card = priv->card;

    if (nb > (65536 - sizeof(struct if_sdio_packet)))
    {
        ret = -EINVAL;
        goto out;
    }

    //size = sdio_align_size(card->func, nb);
    size = nb;
    if(size%4)
    {
        size += (4-size%4);
    }

    RDA5890_DBGLAP(RDA5890_DA_SDIO, RDA5890_DL_NORM,
                   "if_sdio_host_to_card, size = %d, aligned size = %d\n", nb, size);

    packet = kzalloc(sizeof(struct if_sdio_packet) + size,
                     GFP_ATOMIC);
    if (!packet)
    {
        ret = -ENOMEM;
        goto out;
    }

    packet->next = NULL;
    packet->nb = size;

    memcpy(packet->buffer, buf, nb);

    spin_lock_irqsave(&card->lock, flags);

    if (!card->packets)
        card->packets = packet;
    else
    {
        cur = card->packets;
        while (cur->next)
            cur = cur->next;
        cur->next = packet;
    }

    spin_unlock_irqrestore(&card->lock, flags);

    schedule_work(&card->packet_worker);

    RDA5890_DBGLAP(RDA5890_DA_SDIO, RDA5890_DL_DEBUG,
                   "%s <<<\n", __func__);

    ret = 0;

out:
    return ret;
}

/*******************************************************************/
/* SDIO callbacks                                                  */
/*******************************************************************/

static void if_sdio_interrupt(struct sdio_func *func)
{
    int ret = 0;
    struct if_sdio_card *card;
    struct rda5890_private *priv;
    u8 status;

    RDA5890_DBGLAP(RDA5890_DA_SDIO, RDA5890_DL_DEBUG,
                   "%s >>>\n", __func__);

    card = sdio_get_drvdata(func);
    priv = card->priv;

    status = sdio_readb(card->func, IF_SDIO_FUN1_INT_STAT, &ret);
    if (ret)
        goto out;
    RDA5890_DBGLAP(RDA5890_DA_SDIO, RDA5890_DL_VERB,
                   "if_sdio_interrupt, status = 0x%02x\n", status);

    if (status & IF_SDIO_INT_AHB2SDIO)
        if_sdio_card_to_host(card);

    if (status & IF_SDIO_INT_ERROR)
    {
        sdio_writeb(card->func, IF_SDIO_INT_ERROR, IF_SDIO_FUN1_INT_PEND, &ret);
        if (ret)
        {
            RDA5890_ERRP("write FUN1_INT_STAT reg fail\n");
            goto out;
        }

        RDA5890_DBGLAP(RDA5890_DA_SDIO, RDA5890_DL_TRACE,
                       "%s, INT_ERROR\n", __func__);
    }

    if (status & IF_SDIO_INT_SLEEP)
    {
        atomic_inc(&priv->sleep_flag);

        sdio_writeb(card->func, IF_SDIO_INT_SLEEP, IF_SDIO_FUN1_INT_PEND, &ret);
        if (ret)
        {
            RDA5890_ERRP("write FUN1_INT_STAT reg fail\n");
            goto out;
        }

        RDA5890_DBGLAP(RDA5890_DA_PM, RDA5890_DL_NORM,
                       "%s, device sleep\n", __func__);
    }

    if (status & IF_SDIO_INT_AWAKE)
    {
        atomic_set(&priv->sleep_flag, 0);

        sdio_writeb(card->func, IF_SDIO_INT_AWAKE, IF_SDIO_FUN1_INT_PEND, &ret);
        if (ret)
        {
            RDA5890_ERRP("write FUN1_INT_STAT reg fail\n");
            goto out;
        }

        RDA5890_DBGLAP(RDA5890_DA_PM, RDA5890_DL_NORM,
                       "%s, device awake\n", __func__);
    }

out:
    RDA5890_DBGLAP(RDA5890_DA_SDIO, RDA5890_DL_DEBUG,
                   "%s <<<\n", __func__);
}

static int if_sdio_probe(struct sdio_func *func,
                         const struct sdio_device_id *id)
{
    struct if_sdio_card *card;
    struct rda5890_private *priv;
    struct if_sdio_packet *packet;
    int ret = 0;

    RDA5890_DBGLAP(RDA5890_DA_SDIO, RDA5890_DL_DEBUG,
                   "%s >>>\n", __func__);

    printk(KERN_INFO "RDA5890: SDIO card attached\n");

    card = kzalloc(sizeof(struct if_sdio_card), GFP_KERNEL);
    if (!card)
    {
        RDA5890_ERRP("kzalloc fail\n");
        return -ENOMEM;
    }

    card->func = func;
    spin_lock_init(&card->lock);
    INIT_WORK(&card->packet_worker, if_sdio_host_to_card_worker);

    sdio_claim_host(func);

    ret = sdio_enable_func(func);
    if (ret)
    {
        RDA5890_ERRP("sdio_enable_func fail, ret = %d\n", ret);
        goto release;
    }

    ret = sdio_claim_irq(func, if_sdio_interrupt);
    if (ret)
    {
        RDA5890_ERRP("sdio_claim_irq fail, ret = %d\n", ret);
        goto disable;
    }
    sdio_release_host(func);

    sdio_set_drvdata(func, card);

    priv = rda5890_add_card(card, &func->dev);
    if (!priv)
    {
        RDA5890_ERRP("rda5890_add_card fail\n");
        ret = -ENOMEM;
        goto release_int;
    }

    rda5890_debugfs_init_one(priv);

    card->priv = priv;

    priv->card = card;
    priv->hw_host_to_card = if_sdio_host_to_card;

    /*
     * Enable interrupts now that everything is set up
     */
    sdio_claim_host(func);
    sdio_writeb(func, 0x37, IF_SDIO_FUN1_INT_MASK, &ret);
    sdio_release_host(func);
    if (ret)
    {
        RDA5890_ERRP("enable func interrupt fail\n");
        goto remove_card;
    }

    if (sdio_test_flag)
    {
        RDA5890_DBGLAP(RDA5890_DA_SDIO, RDA5890_DL_TRACE,
                       "SDIO TESTING MODE\n");
        goto done;
    }

    msleep(1000);

    ret = rda5890_start_card(priv);
    if (ret)
    {
        RDA5890_ERRP("rda5890_start_card fail, ret = %d\n", ret);
        goto remove_card;
    }

done:
    printk(KERN_INFO "RDA5890: SDIO card started\n");

out:
    RDA5890_DBGLAP(RDA5890_DA_SDIO, RDA5890_DL_DEBUG,
                   "%s <<<\n", __func__);
    return ret;

remove_card:
    rda5890_remove_card(priv);
release_int:
    sdio_claim_host(func);
    sdio_release_irq(func);
disable:
    sdio_disable_func(func);
release:
    sdio_release_host(func);

    while (card->packets)
    {
        packet = card->packets;
        card->packets = card->packets->next;
        kfree(packet);
    }

    kfree(card);
    goto out;
}

static void if_sdio_remove(struct sdio_func *func)
{
    struct if_sdio_card *card;
    struct if_sdio_packet *packet;

    RDA5890_DBGLAP(RDA5890_DA_SDIO, RDA5890_DL_DEBUG,
                   "%s >>>\n", __func__);

    printk(KERN_INFO "RDA5890: SDIO card detached\n");

    card = sdio_get_drvdata(func);

    rda5890_stop_card(card->priv);
    rda5890_debugfs_remove_one(card->priv);
    rda5890_remove_card(card->priv);
    kfree(card->priv);

    sdio_claim_host(func);
    sdio_release_irq(func);
    sdio_disable_func(func);
    sdio_release_host(func);

    while (card->packets)
    {
        packet = card->packets;
        card->packets = card->packets->next;
        kfree(packet);
    }

    kfree(card);

    printk(KERN_INFO "RDA5890: SDIO card removed\n");

    RDA5890_DBGLAP(RDA5890_DA_SDIO, RDA5890_DL_DEBUG,
                   "%s <<<\n", __func__);
}

static struct sdio_driver if_sdio_driver =
{
    .name       = "rda5890_sdio",
    .id_table   = if_sdio_ids,
    .probe      = if_sdio_probe,
    .remove     = if_sdio_remove,
};

/*******************************************************************/
/* Module functions                                                */
/*******************************************************************/

static int __init if_sdio_init_module(void)
{
    int ret = 0;

    printk(KERN_INFO "\nRDA5890 SDIO WIFI Driver\n");
    printk(KERN_INFO "Ver: %d.%d.%d\n\n",
           RDA5890_SDIOWIFI_VER_MAJ,
           RDA5890_SDIOWIFI_VER_MIN,
           RDA5890_SDIOWIFI_VER_BLD);

    rda5890_debugfs_init();

    ret = sdio_register_driver(&if_sdio_driver);

    return ret;
}

static void __exit if_sdio_exit_module(void)
{
    sdio_unregister_driver(&if_sdio_driver);

    rda5890_debugfs_remove();
}

module_init(if_sdio_init_module);
module_exit(if_sdio_exit_module);

MODULE_DESCRIPTION("RDA5890 SDIO WLAN Driver");
MODULE_AUTHOR("lwang");
MODULE_LICENSE("GPL");
