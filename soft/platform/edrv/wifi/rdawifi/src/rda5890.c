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


#include "drv_features.h"
#include "kal_release.h"
#include "reg_base.h"
#include "app_buff_alloc.h"
#include "msdc_def.h"
#include "sd_def.h"
#include "sdio_def.h"
#include "drv_comm.h"
#include "intrCtrl.h"
#include "sdio_sw.h"
#include "cache_sw.h"
#include "init.h"
#include "dma_hw.h"
#include "Drv_trc.h"

#include "rda5890_defs.h"
#include "rda5890.h"

#define IF_SDIO_SDIO2AHB_PKTLEN_L          0x00
#define IF_SDIO_SDIO2AHB_PKTLEN_H          0x01

#define IF_SDIO_AHB2SDIO_PKTLEN_L          0x02
#define IF_SDIO_AHB2SDIO_PKTLEN_H          0x03

#define IF_SDIO_FUN1_INT_MASK              0x04
#define IF_SDIO_FUN1_INT_PEND              0x05
#define IF_SDIO_FUN1_INT_STAT              0x06

#define   IF_SDIO_INT_AHB2SDIO             0x01
#define   IF_SDIO_INT_ERROR                0x04
#define   IF_SDIO_INT_SLEEP                0x10
#define   IF_SDIO_INT_AWAKE                0x20

#define IF_SDIO_FUN1_FIFO_WR               0x07
#define IF_SDIO_FUN1_FIFO_RD               0x08

#define IF_SDIO_FUN1_INT_TO_DEV            0x09

kal_char rda5890_dbg_str[200];
int rda5890_dbg_level = RDA5890_DL_TRACE;
int rda5890_dbg_area =  RDA5890_DA_MAIN |
                        RDA5890_DA_SDIO |
                        RDA5890_DA_WID;

#define RDA5890_SDIO_BLK_SIZE              512

__align(4) static kal_uint8 isr_in_databuf[1660];
__align(4) static kal_uint8 out_databuf[1660];

kal_char sdio_dbg_str[200];

void dump_buf(kal_uint8 *data, kal_uint16 len)
{
    kal_uint16 i, off = 0;

    for (i=0; i<len; i++)
    {
        if(i%8 == 0)
        {
            kal_sprintf(&sdio_dbg_str[off], "  ");
            off += 2;
        }
        kal_sprintf(&sdio_dbg_str[off], "%02x ", data[i]);
        off += 3;
        if((i+1)%16 == 0)
        {
            kal_sprintf(&sdio_dbg_str[off], "\n");
            kal_print(sdio_dbg_str);
            off = 0;
        }
    }
    kal_sprintf(&sdio_dbg_str[off], "\n");
    kal_print(sdio_dbg_str);
}

void rda5890_sdio_enable_irq(kal_bool enable)
{
    kal_uint8 int_mask;
    kal_bool result;

    RDA5890_DBGPRINT1(RDA5890_DA_SDIO, RDA5890_DL_NORM,
                      "rda5890_sdio_enable_irq, %s\n",
                      (enable == KAL_TRUE)?"ENABLE":"DISABLE");

    if(enable == KAL_TRUE)
        int_mask = 0x37;
    else
        int_mask = 0;

    result = SDIO_Register_WriteByte_Cmd52(SDIO_FUCN_1,
                                           IF_SDIO_FUN1_INT_MASK, int_mask);
    if (result != KAL_TRUE)
    {
        RDA5890_ERRPRINT("rda5890_sdio_enable_irq, cmd52 fail\n");
    }
}

void rda5890_sdio_set_blksize(kal_uint32 size)
{
    SDC_CMD_STATUS status = NO_ERROR;

    RDA5890_DBGPRINT1(RDA5890_DA_SDIO, RDA5890_DL_NORM,
                      "rda5890_sdio_set_blksize, size = %d\n", size);

    status = SDIO_configure_BLK_size(SDIO_FUCN_1, size);
    if (status != NO_ERROR)
    {
        RDA5890_ERRPRINT("rda5890_sdio_set_blksize, fail\n");
    }
}

kal_bool rda5890_sdio_data_write(kal_uint8 *data, kal_uint32 len)
{
    kal_bool result;
    kal_uint8 byte;
    kal_uint32 len_32 = (len + 3) >> 2;
    kal_uint32 batch, bytes_to_write, bytes_written;

    RDA5890_DBGPRINT1(RDA5890_DA_SDIO, RDA5890_DL_NORM,
                      "rda5890_sdio_data_write, len = %d\n", len);

    memcpy(out_databuf, data, len);

    /* TODO: fix not 4-bytes align issue here */
    byte = (kal_uint8)(len_32 & 0xff);
    result = SDIO_Register_WriteByte_Cmd52(SDIO_FUCN_1,
                                           IF_SDIO_SDIO2AHB_PKTLEN_L, byte);
    if (result != KAL_TRUE)
    {
        RDA5890_ERRPRINT("rda5890_sdio_data_write, cmd52 L fail\n");
        goto out;
    }

    byte = (kal_uint8)((len_32 >> 8) & 0xff);
    byte |= 0x80;
    result = SDIO_Register_WriteByte_Cmd52(SDIO_FUCN_1,
                                           IF_SDIO_SDIO2AHB_PKTLEN_H, byte);
    if (result != KAL_TRUE)
    {
        RDA5890_ERRPRINT("rda5890_sdio_data_write, cmd52 H fail\n");
        goto out;
    }

    bytes_to_write = (len_32 << 2);
    bytes_written = 0;
    while(bytes_to_write)
    {
        batch = (bytes_to_write > RDA5890_SDIO_BLK_SIZE)?
                RDA5890_SDIO_BLK_SIZE:bytes_to_write;

        if (batch == RDA5890_SDIO_BLK_SIZE)
            result = SDIO_Data_Write(SDIO_FUCN_1,
                                     IF_SDIO_FUN1_FIFO_WR, out_databuf + bytes_written,
                                     SDIO_FIX, 1, KAL_TRUE);
        else
            result = SDIO_Data_Write(SDIO_FUCN_1,
                                     IF_SDIO_FUN1_FIFO_WR, out_databuf + bytes_written,
                                     SDIO_FIX, batch, KAL_FALSE);
        if (result != KAL_TRUE)
        {
            RDA5890_ERRPRINT("rda5890_sdio_data_write, cmd53 fail\n");
            goto out;
        }
        bytes_to_write -= batch;
        bytes_written += batch;
    }

out:
    return result;
}

kal_bool rda5890_sdio_data_read(kal_uint8 *data, kal_uint32 *len)
{
    kal_bool result;
    kal_uint8 byte;
    kal_uint32 len_32 = 0;
    kal_uint32 batch, bytes_to_read, bytes_read;

    result = SDIO_Register_ReadByte_Cmd52(SDIO_FUCN_1,
                                          IF_SDIO_AHB2SDIO_PKTLEN_L, &byte);
    if (result != KAL_TRUE)
    {
        RDA5890_ERRPRINT("rda5890_sdio_data_read, cmd52 L fail\n");
        goto out;
    }
    len_32 |= byte;

    result = SDIO_Register_ReadByte_Cmd52(SDIO_FUCN_1,
                                          IF_SDIO_AHB2SDIO_PKTLEN_H, &byte);
    if (result != KAL_TRUE)
    {
        RDA5890_ERRPRINT("rda5890_sdio_data_read, cmd52 H fail\n");
        goto out;
    }
    len_32 |= (byte << 8);

    bytes_to_read = len_32 << 2;
    bytes_read = 0;
    while(bytes_to_read)
    {
        batch = (bytes_to_read > RDA5890_SDIO_BLK_SIZE)?
                RDA5890_SDIO_BLK_SIZE:bytes_to_read;

        if (batch == RDA5890_SDIO_BLK_SIZE)
            result = SDIO_Data_Read(SDIO_FUCN_1,
                                    IF_SDIO_FUN1_FIFO_RD, data + bytes_read,
                                    SDIO_FIX, 1, KAL_TRUE);
        else
            result = SDIO_Data_Read(SDIO_FUCN_1,
                                    IF_SDIO_FUN1_FIFO_RD, data + bytes_read,
                                    SDIO_FIX, batch, KAL_FALSE);
        if (result != KAL_TRUE)
        {
            RDA5890_ERRPRINT("rda5890_sdio_data_read, cmd53 fail\n");
            goto out;
        }
        bytes_to_read -= batch;
        bytes_read += batch;
    }

    RDA5890_DBGPRINT1(RDA5890_DA_SDIO, RDA5890_DL_NORM,
                      "rda5890_sdio_data_read, len = %d\n", bytes_read);

    *len = bytes_read;
out:
    return result;
}

kal_bool rda5890_sdio_data_read_isr(kal_uint8 *data, kal_uint32 *len)
{
    kal_bool result;
    kal_uint8 byte;
    kal_uint32 len_32 = 0;
    kal_uint32 batch, bytes_to_read, bytes_read;

    result = SDIO_Register_ReadByte_Cmd52_isr(SDIO_FUCN_1,
             IF_SDIO_AHB2SDIO_PKTLEN_L, &byte);
    if (result != KAL_TRUE)
    {
        RDA5890_ERRPRINT("rda5890_sdio_data_read_isr, cmd52 L fail\n");
        goto out;
    }
    len_32 |= byte;

    result = SDIO_Register_ReadByte_Cmd52_isr(SDIO_FUCN_1,
             IF_SDIO_AHB2SDIO_PKTLEN_H, &byte);
    if (result != KAL_TRUE)
    {
        RDA5890_ERRPRINT("rda5890_sdio_data_read_isr, cmd52 H fail\n");
        goto out;
    }
    len_32 |= (byte << 8);

    bytes_to_read = len_32 << 2;
    bytes_read = 0;
    while(bytes_to_read)
    {
        batch = (bytes_to_read > RDA5890_SDIO_BLK_SIZE)?
                RDA5890_SDIO_BLK_SIZE:bytes_to_read;

        if (batch == RDA5890_SDIO_BLK_SIZE)
            result = SDIO_Data_Read_isr(SDIO_FUCN_1,
                                        IF_SDIO_FUN1_FIFO_RD, data + bytes_read,
                                        SDIO_FIX, 1, KAL_TRUE);
        else
            result = SDIO_Data_Read_isr(SDIO_FUCN_1,
                                        IF_SDIO_FUN1_FIFO_RD, data + bytes_read,
                                        SDIO_FIX, batch, KAL_FALSE);
        if (result != KAL_TRUE)
        {
            RDA5890_ERRPRINT("rda5890_sdio_data_read_isr, cmd53 fail\n");
            goto out;
        }
        bytes_to_read -= batch;
        bytes_read += batch;
    }

    RDA5890_DBGPRINT1(RDA5890_DA_SDIO, RDA5890_DL_NORM,
                      "rda5890_sdio_data_read_isr, len = %d\n", bytes_read);

    *len = bytes_read;

out:
    return result;
}

void rda5890_sdio_isr(void)
{
    kal_bool result;
    kal_uint8 int_status = 0;
    kal_uint32 rx_len;

    result = SDIO_Register_ReadByte_Cmd52_isr(SDIO_FUCN_1,
             IF_SDIO_FUN1_INT_STAT, &int_status);
    RDA5890_DBGPRINT1(RDA5890_DA_SDIO, RDA5890_DL_VERB,
                      "rda5890_sdio_isr, int_status = 0x%02x\n", int_status);

    if (int_status & IF_SDIO_INT_AHB2SDIO)
    {
        RDA5890_DBGPRINT(RDA5890_DA_SDIO, RDA5890_DL_NORM,
                         "rda5890_sdio_isr, INT_RX\n");
        rda5890_sdio_data_read_isr(isr_in_databuf, &rx_len);
        //dump_buf(isr_in_databuf, rx_len);
        rda5890_card_to_host(isr_in_databuf, rx_len);
    }

    if (int_status & IF_SDIO_INT_ERROR)
    {
        SDIO_Register_WriteByte_Cmd52_isr(SDIO_FUCN_1,
                                          IF_SDIO_FUN1_INT_PEND, IF_SDIO_INT_ERROR);
        RDA5890_DBGPRINT(RDA5890_DA_SDIO, RDA5890_DL_NORM,
                         "rda5890_sdio_isr, INT_ERROR\n");
    }

    if (int_status & IF_SDIO_INT_SLEEP)
    {
        SDIO_Register_WriteByte_Cmd52_isr(SDIO_FUCN_1,
                                          IF_SDIO_FUN1_INT_PEND, IF_SDIO_INT_SLEEP);
        RDA5890_DBGPRINT(RDA5890_DA_SDIO, RDA5890_DL_NORM,
                         "rda5890_sdio_isr, INT_SLEEP\n");
    }

    if (int_status & IF_SDIO_INT_AWAKE)
    {
        SDIO_Register_WriteByte_Cmd52_isr(SDIO_FUCN_1,
                                          IF_SDIO_FUN1_INT_PEND, IF_SDIO_INT_AWAKE);
        RDA5890_DBGPRINT(RDA5890_DA_SDIO, RDA5890_DL_NORM,
                         "rda5890_sdio_isr, INT_AWAKE\n");
    }
}

kal_bool rda5890_sdio_init(void)
{
    SDC_CMD_STATUS status;
    kal_bool result = KAL_TRUE;

    MSDC_Initialize();
    status = SDIO_Initialize();
    if (status != NO_ERROR)
    {
        RDA5890_ERRPRINT("SDIO_Initialize fail\n");
        result = KAL_FALSE;
        goto out;
    }

    /* set blk size */
    status = SDIO_configure_BLK_size(SDIO_FUCN_1, RDA5890_SDIO_BLK_SIZE);
    if (status != NO_ERROR)
    {
        RDA5890_ERRPRINT("SDIO_configure_BLK_size 1 fail\n");
        result = KAL_FALSE;
        goto out;
    }

    /* setup interrupt */
    SDIO_int_registration(SDIO_FUCN_1, rda5890_sdio_isr);
    status = SDIO_enable_IO_Int(SDIO_FUCN_0, KAL_TRUE); // Master Interrupt Enable
    if (status != NO_ERROR)
    {
        RDA5890_ERRPRINT("SDIO_enable_IO_Int 0 fail\n");
        result = KAL_FALSE;
        goto out;
    }
    status = SDIO_enable_IO_Int(SDIO_FUCN_1, KAL_TRUE);
    if (status != NO_ERROR)
    {
        RDA5890_ERRPRINT("SDIO_enable_IO_Int 1 fail\n");
        result = KAL_FALSE;
        goto out;
    }
    rda5890_sdio_enable_irq(KAL_TRUE);

    /* init wid */
    rda5890_wid_init();

    RDA5890_DBGPRINT(RDA5890_DA_SDIO, RDA5890_DL_TRACE,
                     "rda5890_sdio_init, done\n");

out:
    return result;
}

/* Testing Functions */
void SDIO_test_isr(void)
{
    kal_bool result;
    kal_uint8 int_status = 0;
    kal_uint32 rx_len;

    result = SDIO_Register_ReadByte_Cmd52_isr(SDIO_FUCN_1,
             IF_SDIO_FUN1_INT_STAT, &int_status);
    kal_sprintf(sdio_dbg_str, "rda5890_sdio_isr, int_status = 0x%02x\n", result);
    kal_print(sdio_dbg_str);

    if (int_status & IF_SDIO_INT_AHB2SDIO)
    {
        kal_print("rda5890_sdio_isr, INT_RX\n");
        rda5890_sdio_data_read_isr(isr_in_databuf, &rx_len);
        dump_buf(isr_in_databuf, rx_len);
    }

    if (int_status & IF_SDIO_INT_ERROR)
    {
        SDIO_Register_WriteByte_Cmd52_isr(SDIO_FUCN_1,
                                          IF_SDIO_FUN1_INT_PEND, IF_SDIO_INT_ERROR);
        kal_print("rda5890_sdio_isr, INT_ERROR\n");
    }

    if (int_status & IF_SDIO_INT_SLEEP)
    {
        SDIO_Register_WriteByte_Cmd52_isr(SDIO_FUCN_1,
                                          IF_SDIO_FUN1_INT_PEND, IF_SDIO_INT_SLEEP);
        kal_print("rda5890_sdio_isr, INT_SLEEP\n");
    }

    if (int_status & IF_SDIO_INT_AWAKE)
    {
        SDIO_Register_WriteByte_Cmd52_isr(SDIO_FUCN_1,
                                          IF_SDIO_FUN1_INT_PEND, IF_SDIO_INT_AWAKE);
        kal_print("rda5890_sdio_isr, INT_AWAKE\n");
    }
}

void SDIO_test_init(kal_uint32 mode)
{
    SDC_CMD_STATUS status;
    kal_bool result = KAL_TRUE;

    kal_print("SDIO_test_init >>>\n");

    MSDC_Initialize();
    status = SDIO_Initialize();
    if (status != NO_ERROR)
    {
        kal_print("SDIO_Initialize fail\n");
        result = KAL_FALSE;
        goto out;
    }

    /* set blk size */
    status = SDIO_configure_BLK_size(SDIO_FUCN_1, 512);
    if (status != NO_ERROR)
    {
        kal_print("SDIO_configure_BLK_size 1 fail\n");
        result = KAL_FALSE;
        goto out;
    }

    /* setup interrupt */
    SDIO_int_registration(SDIO_FUCN_1, SDIO_test_isr);
    status = SDIO_enable_IO_Int(SDIO_FUCN_0, KAL_TRUE); // Master Interrupt Enable
    if (status != NO_ERROR)
    {
        kal_print("SDIO_enable_IO_Int 0 fail\n");
        result = KAL_FALSE;
        goto out;
    }
    status = SDIO_enable_IO_Int(SDIO_FUCN_1, KAL_TRUE);
    if (status != NO_ERROR)
    {
        kal_print("SDIO_enable_IO_Int 1 fail\n");
        result = KAL_FALSE;
        goto out;
    }
    rda5890_sdio_enable_irq(KAL_TRUE);

out:
    kal_sprintf(sdio_dbg_str, "SDIO_test_init <<<, result = %d\n", result);
    kal_print(sdio_dbg_str);
}

void SDIO_test_write_cmd52(kal_uint8 func, kal_uint32 addr, kal_uint8 data)
{
    kal_bool result;

    kal_sprintf(sdio_dbg_str,
                "SDIO_test_write_cmd52 >>>, func = %d, addr = 0x%x, data = 0x%x\n", func, addr, data);
    kal_print(sdio_dbg_str);

    result = SDIO_Register_WriteByte_Cmd52(func, addr, data);

    kal_sprintf(sdio_dbg_str,
                "SDIO_test_write_cmd52 <<<, result = %d\n", result);
    kal_print(sdio_dbg_str);
}

void SDIO_test_read_cmd52(kal_uint8 func, kal_uint32 addr, kal_uint8 *data)
{
    kal_bool result;

    kal_sprintf(sdio_dbg_str,
                "SDIO_test_read_cmd52 >>>, func = %d, addr = 0x%x\n", func, addr);
    kal_print(sdio_dbg_str);

    result = SDIO_Register_ReadByte_Cmd52(func, addr, data);

    kal_sprintf(sdio_dbg_str,
                "SDIO_test_read_cmd52 <<<, result = %d, data = %d\n", result, *data);
    kal_print(sdio_dbg_str);
}

void SDIO_test_reg32_write(kal_uint32 addr, kal_uint32 data)
{
    SDC_CMD_STATUS status = NO_ERROR;
    kal_bool result;

    kal_sprintf(sdio_dbg_str,
                "SDIO_test_reg32_write >>>, addr = 0x%x, data = 0x%x\n", addr, data);
    kal_print(sdio_dbg_str);

    result = SDIO_Register_Write(SDIO_FUCN_1, addr, data, SDIO_INC);
    if (result != KAL_TRUE)
    {
        status = ERR_ERRORS;
        goto ret;
    }

ret:
    kal_sprintf(sdio_dbg_str,
                "SDIO_test_reg32_write <<<, status = %d\n", status);
    kal_print(sdio_dbg_str);
}

void SDIO_test_reg32_read(kal_uint32 addr, kal_uint32 *data)
{
    SDC_CMD_STATUS status = NO_ERROR;
    kal_bool result;

    kal_sprintf(sdio_dbg_str,
                "SDIO_test_reg32_read >>>, addr = 0x%x\n", addr);
    kal_print(sdio_dbg_str);

    result = SDIO_Register_Read(SDIO_FUCN_1, addr, data, SDIO_INC);
    if (result != KAL_TRUE)
    {
        status = ERR_ERRORS;
        goto ret;
    }

ret:
    kal_sprintf(sdio_dbg_str,
                "SDIO_test_reg32_read <<<, status = %d, data = 0x%x\n", status, *data);
    kal_print(sdio_dbg_str);
}

void SDIO_test_data_write(kal_uint8 *data, kal_uint32 len)
{
    kal_bool result;

    kal_sprintf(sdio_dbg_str,
                "SDIO_test_data_write >>>, len = 0x%x\n", len);
    kal_print(sdio_dbg_str);

    result = rda5890_sdio_data_write(data, len);

    kal_sprintf(sdio_dbg_str,
                "SDIO_test_data_write <<<, result = %d\n", result);
    kal_print(sdio_dbg_str);
}

void SDIO_test_data_read(kal_uint8 *data, kal_uint32 *len)
{
    kal_bool result;

    kal_sprintf(sdio_dbg_str,
                "SDIO_test_data_read >>>\n");
    kal_print(sdio_dbg_str);

    result = rda5890_sdio_data_read(data, len);

    kal_sprintf(sdio_dbg_str,
                "SDIO_test_data_read <<<, result = %d, len = %d\n", result, *len);
    kal_print(sdio_dbg_str);
}

void SDIO_test_set_blksize(kal_uint32 size)
{
    SDC_CMD_STATUS status = NO_ERROR;

    kal_sprintf(sdio_dbg_str,
                "SDIO_test_set_blksize >>>, size = %d\n", size);
    kal_print(sdio_dbg_str);

    status = SDIO_configure_BLK_size(SDIO_FUCN_1, size);

    kal_sprintf(sdio_dbg_str,
                "SDIO_test_set_blksize <<<, status = %d\n", status);
    kal_print(sdio_dbg_str);
}

void SDIO_test_data_write_blk(kal_uint8 *data, kal_uint32 len)
{
    SDC_CMD_STATUS status = NO_ERROR;
    kal_bool result;
    kal_uint8 byte;
    kal_uint32 len_32 = len >> 2;

    kal_sprintf(sdio_dbg_str,
                "SDIO_test_data_write >>>, len = %d\n", len);
    kal_print(sdio_dbg_str);

    /* TODO: fix not 4-bytes align issue here */
    byte = (kal_uint8)(len_32 & 0xff);
    result = SDIO_Register_WriteByte_Cmd52(SDIO_FUCN_1,
                                           IF_SDIO_SDIO2AHB_PKTLEN_L, byte);
    if (result != KAL_TRUE)
    {
        status = ERR_ERRORS;
        goto ret;
    }
    kal_sprintf(sdio_dbg_str,
                "  Write len_l, byte = %d\n", byte);
    kal_print(sdio_dbg_str);

    byte = (kal_uint8)((len_32 >> 8) & 0xff);
    byte |= 0x80;
    result = SDIO_Register_WriteByte_Cmd52(SDIO_FUCN_1,
                                           IF_SDIO_SDIO2AHB_PKTLEN_H, byte);
    if (result != KAL_TRUE)
    {
        status = ERR_ERRORS;
        goto ret;
    }
    kal_sprintf(sdio_dbg_str,
                "  Write len_h, byte = %d\n", byte);
    kal_print(sdio_dbg_str);

    result = SDIO_Data_Write(SDIO_FUCN_1,
                             IF_SDIO_FUN1_FIFO_WR, data, SDIO_FIX, 1, KAL_TRUE);
    if (result != KAL_TRUE)
    {
        status = ERR_ERRORS;
        goto ret;
    }

ret:
    kal_sprintf(sdio_dbg_str,
                "SDIO_test_data_write <<<, status = %d\n", status);
    kal_print(sdio_dbg_str);
}

void SDIO_test_data_read_blk(kal_uint8 *data, kal_uint32 *len)
{
    SDC_CMD_STATUS status = NO_ERROR;
    kal_bool result;
    kal_uint8 byte;
    kal_uint32 len_32 = 0;

    kal_sprintf(sdio_dbg_str,
                "SDIO_test_data_read >>>\n");
    kal_print(sdio_dbg_str);

    result = SDIO_Register_ReadByte_Cmd52(SDIO_FUCN_1,
                                          IF_SDIO_AHB2SDIO_PKTLEN_L, &byte);
    if (result != KAL_TRUE)
    {
        status = ERR_ERRORS;
        goto ret;
    }
    kal_sprintf(sdio_dbg_str,
                "  Read len_l, byte = %d\n", byte);
    kal_print(sdio_dbg_str);
    len_32 |= byte;

    result = SDIO_Register_ReadByte_Cmd52(SDIO_FUCN_1,
                                          IF_SDIO_AHB2SDIO_PKTLEN_H, &byte);
    if (result != KAL_TRUE)
    {
        status = ERR_ERRORS;
        goto ret;
    }
    kal_sprintf(sdio_dbg_str,
                "  Read len_h, byte = %d\n", byte);
    kal_print(sdio_dbg_str);
    len_32 |= (byte << 8);

    *len = len_32 << 2;
    result = SDIO_Data_Read(SDIO_FUCN_1,
                            IF_SDIO_FUN1_FIFO_RD, data, SDIO_FIX, 1, KAL_TRUE);
    if (result != KAL_TRUE)
    {
        status = ERR_ERRORS;
        goto ret;
    }

ret:
    kal_sprintf(sdio_dbg_str,
                "SDIO_test_data_read <<<, status = %d, len = %d\n", status, *len);
    kal_print(sdio_dbg_str);
}

void SDIO_debug_read_msdc_reg(kal_uint32 addr, kal_uint32 *data)
{
    *data = *(volatile kal_uint32*)(MSDC_ADRS + addr);
    kal_sprintf(sdio_dbg_str,
                "SDIO_debug_read_msdc_reg, addr = 0x%x, data = 0x%x\n", addr, *data);
    kal_print(sdio_dbg_str);
}

