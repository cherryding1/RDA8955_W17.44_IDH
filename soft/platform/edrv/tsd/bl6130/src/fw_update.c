#include "cs_types.h"
#include "sxr_ops.h"
#include "sxs_io.h"
#include "sxr_tls.h"
#include "pmd_m.h"
#include "hal_gpio.h"
#include "hal_host.h"
#include "hal_i2c.h"
#include "i2c_master.h"
#include "tsdp_debug.h"
#include "tsd_config.h"
#include "tsdp_calib.h"
#include "tsd_m.h"
#include "tgt_tsd_cfg.h"

#include "fw_update.h"
#include "bl_ts.h"
#include <stdarg.h>

#include "gpio_i2c.h"
#include "lcdd.h"


#ifdef WIN32
//#include <platform_def.h>
#endif


#define	  FLASH_CHECKSUM_DELAY_TIME			    100
#define   FLASH_ARG_SIZE                        1024
#define   ARGUMENT_OFFSET                       0x200
#define   FW_I2C_ADDR						    (CTP_SLAVE_ADDR) // 7bit
#define   FLASH_I2C_ADDR					    (0x2c)//7bit addr
#define   FLASH_WSIZE						    128 //256//0x128
#define   FLASH_RSIZE						    64//0x128

#define   ARGUMENT_MAX_COUNT                    20
#define   ARGUMENT_DUM_SIZE                     512

//end

extern UINT8 gpio_i2c_write_bytes(UINT32 addr, UINT8 *data, UINT8 datalen);
extern UINT8 gpio_i2c_read_bytes(UINT32 addr,UINT8 *datas, UINT8 datalen);

extern HAL_ERR_T hal_I2cGetData_ext(UINT32 slaveAddr,   UINT8 memAddr,UINT8* pData, UINT8 length);

TSD_CONFIG_T*	g_tsdConfig_bl = NULL;

uint8_t  g_pj_id = 0;
uint8_t  g_pj_id_init = 0;


/********************************************/
uint8_t   g_tp_id = 0;
uint8_t   g_fw_id = 0xff;
uint8_t   g_arg_id = 0xff;
uint8_t   g_bin_fw_id = 0xff;
uint8_t   g_bin_arg_id = 0xff;
uint16_t  g_fw_checksum = !BIN_CHECKSUM;
uint16_t  g_bin_checksum = BIN_CHECKSUM;
uint8_t   g_bin_type = BIN_TYPE;
uint8_t   g_down_type = DOWN_LOAD_TYPE;

/********************************************/


#ifndef _WIN32

#if 0
int8_t bl_i2c_write(uint8_t i2c_addr, uint8_t *data, uint16_t len) 
{
	HAL_ERR_T halErr = HAL_ERR_RESOURCE_BUSY;

    halErr = hal_I2cOpen(g_tsdConfig_bl->i2cBusId);
    halErr = hal_I2cSendRawByte(g_tsdConfig_bl->i2cBusId, (i2c_addr << 1), I2C_MASTER_STA | I2C_MASTER_WR);
    for(int i = 0; i < len - 1; i++, data++)
    {
        halErr = hal_I2cSendRawByte(g_tsdConfig_bl->i2cBusId, *data, I2C_MASTER_WR);
    }
    halErr = hal_I2cSendRawByte(g_tsdConfig_bl->i2cBusId, *data, I2C_MASTER_WR | I2C_MASTER_STO);
	hal_I2cClose(g_tsdConfig_bl->i2cBusId);

	return halErr;

}

#else

int8_t bl_i2c_write(uint8_t i2c_addr, uint8_t *data, uint16_t len) 
{
    HAL_ERR_T halErr;

    gpio_i2c_open();
    halErr = gpio_i2c_write_data(i2c_addr, NULL, 0, data, len);
    if (halErr != HAL_ERR_NO)
    {
        hal_HstSendEvent(0x5827e004);
    }
    gpio_i2c_close();

    return halErr;
}
#endif

#if 1
int8_t bl_i2c_read(uint8_t i2c_addr, uint8_t *data, uint16_t len) 
{
    HAL_ERR_T halErr = HAL_ERR_RESOURCE_BUSY;

    halErr = hal_I2cOpen(g_tsdConfig_bl->i2cBusId);
//    halErr = hal_I2cSendRawByte(g_tsdConfig->i2cBusId, (i2c_addr << 1), I2C_MASTER_STA | I2C_MASTER_WR);
//    halErr = hal_I2cSendRawByte(g_tsdConfig->i2cBusId, addr, I2C_MASTER_WR);
    halErr = hal_I2cSendRawByte(g_tsdConfig_bl->i2cBusId, (i2c_addr << 1) | 0x01, I2C_MASTER_STA | I2C_MASTER_WR);
    for(int i = 0; i < len - 1; i++, data++)
    {
        (*data) = hal_I2cReadRawByte(g_tsdConfig_bl->i2cBusId, I2C_MASTER_RD);
    }
    (*data) = hal_I2cReadRawByte(g_tsdConfig_bl->i2cBusId, I2C_MASTER_RD | I2C_MASTER_ACK | I2C_MASTER_STO);
	hal_I2cClose(g_tsdConfig_bl->i2cBusId);

	return halErr;	
}
#else

int8_t bl_i2c_read(uint8_t i2c_addr, uint8_t *data, uint16_t len) 
{
    gpio_i2c_open();
    gpio_i2c_read_data(i2c_addr, 0, 0, data, len);
    gpio_i2c_close();
    return HAL_ERR_NO;
}

#endif


#endif

static uint8_t bl_read_fw(uint8_t i2c_addr,uint8_t reg_addr,uint8_t *buf,uint16_t len)
{
	uint8_t ret =0;



	ret = bl_i2c_write(i2c_addr, &reg_addr, 1);
	if(ret)
	{
		return ret;
	}
	ret = bl_i2c_read(i2c_addr, buf, len);
	return ret;
}

static uint8_t bl_write_flash(uint16_t flash_start_addr,uint8_t *flash_data,uint16_t data_len)
{
	uint8_t ret =0;
	uint8_t cmd_buf[6];
	uint16_t flash_end_addr;



	flash_end_addr = flash_start_addr + data_len -1;
	cmd_buf[0] = WRITE_MAIN_CMD;
	cmd_buf[1] = ~WRITE_MAIN_CMD;
	cmd_buf[2] = flash_start_addr >> 0x08;
	cmd_buf[3] = flash_start_addr & 0xff;
	cmd_buf[4] = flash_end_addr >> 0x08;
	cmd_buf[5] = flash_end_addr & 0xff;
	ret = bl_i2c_write(FLASH_I2C_ADDR, cmd_buf, 6);
	
	if(ret)
	{
		return ret;
	}
	ret = bl_i2c_write(FLASH_I2C_ADDR,flash_data,data_len);
	
	return ret;
}

uint8_t bl_write_flash_ex(uint16_t flash_start_addr,uint8_t* flash_data,uint16_t data_size)
{
	uint32_t i;
	uint16_t addr,size,len;



	addr = flash_start_addr;
	for(i = 0;i < data_size;)
	{
		size = data_size - i;	
		if(size > FLASH_WSIZE)
		{			
			len = FLASH_WSIZE;		
		}
		else
		{			
			len = size;		
		}
		if(bl_write_flash(addr, flash_data + i, len))
		{		
			bl_log_trace("bl_write_flash return 1");
			return 1;	
		}		
		addr += len;
		i += len;
		MDELAY(5);//???μ??5MS
	}
	return 0;
}

static uint8_t bl_read_flash(uint16_t flash_start_addr,uint8_t *flash_data,uint16_t data_len)
{
	uint8_t ret =0;
	uint8_t cmd_buf[6];
	uint16_t flash_end_addr;



	flash_end_addr = flash_start_addr + data_len -1;
	cmd_buf[0] = READ_MAIN_CMD;
	cmd_buf[1] = ~READ_MAIN_CMD;
	cmd_buf[2] = flash_start_addr >> 0x08;
	cmd_buf[3] = flash_start_addr & 0xff;
	cmd_buf[4] = flash_end_addr >> 0x08;
	cmd_buf[5] = flash_end_addr & 0xff;
	ret = bl_i2c_write(FLASH_I2C_ADDR,cmd_buf,6);
	if(ret)
	{
		return ret;
	}
	ret = bl_i2c_read(FLASH_I2C_ADDR,flash_data,data_len);
	return ret;
}

/*static*/ uint8_t bl_erase_flash(void)
{
	uint8_t cmd[2] = {ERASE_ALL_MAIN_CMD, ~ERASE_ALL_MAIN_CMD};
	return bl_i2c_write(FLASH_I2C_ADDR, cmd,2);
}

static uint8_t bl_erase_flash_page(uint8_t start_page, uint8_t end_page)
{
	//uint8_t cmd[4] = {ERASE_SECTOR_MAIN_CMD, ~ERASE_SECTOR_MAIN_CMD, start_page, end_page};
	uint8_t cmd[4] = {ERASE_SECTOR_MAIN_CMD, ~ERASE_SECTOR_MAIN_CMD, 0, 0};

	cmd[2] = start_page;
	cmd[3] = end_page;
	
	return bl_i2c_write(FLASH_I2C_ADDR, cmd, 4);
}


uint8_t bl_get_flash_checksum(uint16_t *flash_checksum)
{
	uint8_t  buf[3];
	uint8_t  checksum_ready =0;
	uint8_t  retry = 5;
	uint8_t  ret =0;



	buf[0] = CHECKSUM_CAL_REG;
	buf[1] = CHECKSUM_CAL;
	while(retry--)
	{
		ret = bl_i2c_write(FW_I2C_ADDR,buf,2);
		if(!ret)
			break;
		MDELAY(30);
	}

	bl_log_trace("bl_get_flash_checksum 1");
	if(ret)
	{
		return ret;
	}

	bl_log_trace("bl_get_flash_checksum 1.1");
	MDELAY(FLASH_CHECKSUM_DELAY_TIME);
	ret = bl_read_fw(FW_I2C_ADDR,CHECKSUM_REG,buf,3);
	if(ret)
	{
		return ret;
	}

	bl_log_trace("bl_get_flash_checksum 2");
	checksum_ready = buf[0];
	retry = 5;
	while((retry--) && (checksum_ready != CHECKSUM_READY))
	{
		MDELAY(50);
		
		ret = bl_read_fw(FW_I2C_ADDR, CHECKSUM_REG, buf, 3);
		if(ret)
		{
			return ret;
		}
		checksum_ready = buf[0];
	}

	bl_log_trace("bl_get_flash_checksum 3");
	if(checksum_ready != CHECKSUM_READY)
	{
		return 1;		
	}

	bl_log_trace("bl_get_flash_checksum buf[] hex : %x%x", buf[2], buf[1]);
	*flash_checksum = (buf[1]<<8)+buf[2];
	return 0;
}

static  uint16_t bl_strlen(uint8_t* p_str)
{
	uint16_t  len = 0;



	while(*p_str++ != 0)
		len++;
	return len;
}

static uint8_t  find_bin_arg_offset(uint8_t* p_fw, uint16_t max_len, uint8_t* p_key, uint16_t* p_offset)
{
	uint8_t   ret = 1;
	uint16_t  i = 0; 
	uint16_t  j = 0;
	uint16_t  keylen = bl_strlen(p_key);

	

	for(i = 0; i < max_len; i++)
	{
		for(j = 0; p_fw[i + j] == p_key[j]; j++);
		if(j >= keylen)
		{
			*p_offset = i;
			ret = 0;
			break;
		}
	}
	if(ret)
	{
		*p_offset = 0;
	}
	return ret;
}

static  uint8_t check_bin(uint8_t* p_fw, uint16_t* arg_offset)
{
	uint8_t check_str[] = "chp_cfg_mark";
	uint8_t check_len = (uint8_t)bl_strlen(check_str);
	uint8_t i = 0;
	uint8_t ret = 0;
	uint8_t strkey[] = "UFOUFOUFO";
	

	

	while(*p_fw++ == *(check_str + i))
	{
		i++;
		if(i >= check_len)
			return ARG_TYPE;
	}
	ret = find_bin_arg_offset((uint8_t*)fwbin, FW_SIZE, strkey, arg_offset);
	if(ret)
	{
		return BIN_TYPE;
	}
	return BIN_ARG_TYPE;
}


uint8_t bl_get_arg_order(uint8_t type, uint8_t tp_id, uint16_t offset, uint8_t* p_arg_order)
{
	uint16_t i = 0;
	uint8_t* p_fw = (uint8_t*)fwbin;


	if(type == BIN_TYPE)
	{
		*p_arg_order = 0;
	}
	else if(type == BIN_ARG_TYPE)
	{
		for(i = 0; i < ARGUMENT_MAX_COUNT; i++)
		{
			p_fw = (uint8_t*)(fwbin + offset + ARGUMENT_DUM_SIZE + i * FLASH_ARG_SIZE);  
			if(p_fw[PJ_ID_OFFSET] == tp_id)
			{
				*p_arg_order = (uint8_t)i;
				break;
			}
		}
		if(i == ARGUMENT_MAX_COUNT)
		{
			*p_arg_order = 0;
			return 1;
		}
	}
	else
	{
		for(i = 0; i < ARGUMENT_MAX_COUNT; i++)
		{
			p_fw = (uint8_t*)(fwbin + i * FLASH_ARG_SIZE);  
			if(p_fw[PJ_ID_OFFSET] == tp_id)
			{
				*p_arg_order = (uint8_t)i;
				break;
			}
		}
		if(i == ARGUMENT_MAX_COUNT)
		{
			*p_arg_order = 0;
			return 1;
		}
	}

	return 0;
}

uint8_t bl_memcmp(uint8_t* p_src, uint8_t* p_dst, uint16_t len)
{
	while(len && *(p_src + len - 1) == *(p_dst + len - 1))
	{
		len--;
	}
	return len > 0;
}

uint8_t bl_update_argument(uint8_t type,  uint16_t offset, uint8_t arg_order)
{
	uint32_t  i = 0;
	uint8_t   strkey[] = "UFOUFOUFO";
	uint16_t  size = 0;
	uint8_t*  p_fw = (uint8_t*)fwbin;
	uint8_t   vertify_buf[FLASH_RSIZE] = {0};
	MDELAY(5);

	if(bl_erase_flash_page(1, 2))
	{
		return 1;
	}
	MDELAY(50);
	if(type == BIN_ARG_TYPE)
	{
		p_fw = (uint8_t*)(fwbin + offset + ARGUMENT_DUM_SIZE + arg_order * FLASH_ARG_SIZE);
	}
	else if (type == BIN_TYPE)
	{
		p_fw = (uint8_t*)(fwbin + ARGUMENT_OFFSET);
	}
	else
	{
		p_fw = (uint8_t*)(fwbin + arg_order * FLASH_ARG_SIZE);
	}

	if(bl_write_flash_ex(ARGUMENT_OFFSET, p_fw, FLASH_ARG_SIZE))
	{
		return 1;
	}

	for(i = 0; i < FLASH_ARG_SIZE;)
	{
		size = FLASH_ARG_SIZE - i;
		if(size > FLASH_RSIZE)
		{
			size = FLASH_RSIZE;
		}
		if(bl_read_flash(ARGUMENT_OFFSET + i, vertify_buf, FLASH_RSIZE))
		{
			return 1;
		}
		if(bl_memcmp(p_fw + i, vertify_buf, FLASH_RSIZE))
		{
			return 1;
		}
		i += size;
	}

	MDELAY(5);
	return 0;
}

uint8_t bl_download_flash(uint8_t type, uint16_t offset, uint8_t arg_order)
{
	uint8_t*  p_fw = (uint8_t*)fwbin;
	uint16_t  down_size = FW_SIZE;
	uint8_t   ret = 1;


	bl_log_trace("bl_download_flash coming");
	MDELAY(5);
	if(bl_erase_flash())
	{
		bl_log_trace("bl_erase_flash fail");
		return 1;
	}

	bl_log_trace("bl_erase_flash ok");
	MDELAY(200);
	if(type == BIN_TYPE)
	{
		down_size = FW_SIZE;
		if(bl_write_flash_ex(0, p_fw, down_size))
		{
			return 1;
		}
	}
	else if(type == BIN_ARG_TYPE)
	{
		down_size = offset;
		if(bl_write_flash_ex(0, p_fw, ARGUMENT_OFFSET))
		{
			return 1;
		}

		bl_log_trace("bl_download_flash 1");
		p_fw = (uint8_t*)(fwbin + offset + ARGUMENT_DUM_SIZE + arg_order * FLASH_ARG_SIZE);
		if(bl_write_flash_ex(ARGUMENT_OFFSET, p_fw, FLASH_ARG_SIZE))
		{
			return 1;
		}

		bl_log_trace("bl_download_flash 2");
		p_fw = (uint8_t*)(fwbin + ARGUMENT_OFFSET + FLASH_ARG_SIZE);
		if(bl_write_flash_ex(ARGUMENT_OFFSET + FLASH_ARG_SIZE, p_fw, down_size - ARGUMENT_OFFSET - FLASH_ARG_SIZE))
		{
			return 1;
		}
	}

	bl_log_trace("bl_download_flash ok");
	MDELAY(5);
	return 0;
}


int bl_get_pjid_fwid_argid(unsigned char *buf)
{
	uint8_t ret = 0;
	uint8_t reg = PJ_ID_REG;

	ret = bl_i2c_write(FW_I2C_ADDR, &reg, 1);
	bl_log_trace("bl_get_pjid_fwid_argid write return %d", ret);
	
	if(ret)
	{
		return ret;
	}
	ret = bl_i2c_read(FW_I2C_ADDR, buf, 3);
	bl_log_trace("bl_get_pjid_fwid_argid read return %d", ret);

	return ret;
}

uint8_t  bl_get_tp_id(uint8_t* p_id)
{
	uint8_t ret = 1;
	uint8_t reg = TP_ID_REG;
	uint8_t retry = 1;//5;
	uint8_t buf[7] = { 0 };


	while(retry--)
	{
		ret = bl_i2c_write(FW_I2C_ADDR, &reg, 1);
		bl_log_trace("bl_get_tp_id bl_i2c_write return %d", ret);
		
		if(!ret)
		{
			ret = bl_i2c_read(FW_I2C_ADDR, buf, 7);
		}
		bl_log_trace("bl_get_tp_id bl_i2c_read return %d", ret);

		if(!ret)
			break;
		MDELAY(30);
	}
				
	if(!ret)
	{
		*p_id = buf[6];
	}
	else
	{
		*p_id = 0xff;
	}

	bl_log_trace("bl_get_tp_id buf[6]  is 0x%x", buf[6] );
	
	return ret;
}

uint8_t bl_get_bin_fwid_argid(uint8_t type,  uint16_t offset, uint8_t arg_order, uint8_t* p_arg_id, uint8_t* p_fw_ver_id)
{
	uint8_t   strkey[] = "UFOUFOUFO";
	uint8_t*  p_fw = (uint8_t*)fwbin;
	uint8_t   ret = 1;


	if(type == BIN_ARG_TYPE)
	{
		p_fw = (uint8_t*)(fwbin + offset + ARGUMENT_DUM_SIZE + arg_order * FLASH_ARG_SIZE);
	}
	else if(type == BIN_TYPE)
	{
		p_fw = (uint8_t*)(fwbin + ARGUMENT_OFFSET);
	}
	else 
	{
		p_fw = (uint8_t*)(fwbin + arg_order * FLASH_ARG_SIZE);
	}
	*p_arg_id = p_fw[ARG_ID_OFFSET];
	*p_fw_ver_id = p_fw[FW_VER_ID_OFFSET];
	return 0;
}

uint8_t bl_get_bin_checksum(uint8_t type, uint16_t offset, uint8_t arg_order, uint16_t* p_bin_checksum)
{
	uint32_t  i = 0;
	uint8_t   strkey[] = "UFOUFOUFO";
	uint8_t*  p_fw = (uint8_t*)fwbin;
	uint16_t  bin_checksum = 0;
	uint8_t   ret = 1;


	
	if(type == BIN_ARG_TYPE)
	{
		for(i = 0; i < ARGUMENT_OFFSET; i++)
		{
			bin_checksum += p_fw[i];
		}
		p_fw = (uint8_t*)(fwbin + offset + ARGUMENT_DUM_SIZE + arg_order * FLASH_ARG_SIZE);
		for(i = 0; i < FLASH_ARG_SIZE; i++)
		{
			bin_checksum += p_fw[i];
		}
		p_fw = (uint8_t*)(fwbin + ARGUMENT_OFFSET + FLASH_ARG_SIZE);
		for(i = 0; i < (uint32_t)(offset - ARGUMENT_OFFSET - FLASH_ARG_SIZE); i++)
		{
			bin_checksum += p_fw[i];
		}
		for(i = offset; i < BIN_MAX_SIZE; i++)
		{
			bin_checksum += 0xff;
		}
		*p_bin_checksum = bin_checksum;
		
	}
	else if(type == BIN_TYPE)
	{
		for(i = 0; i < FW_SIZE; i++)
		{
			bin_checksum += p_fw[i];
		}
		for(i = FW_SIZE; i < BIN_MAX_SIZE; i++)
		{
			bin_checksum += 0xff;
		}
	}
	*p_bin_checksum = bin_checksum;
	return 0;
}

uint8_t bl_update_fw(void)
{
	uint8_t   retry = 3;
	uint8_t   ret = 1;
	uint8_t   buf[3] = {0xff};	
	uint8_t   tp_id = 0;
	uint8_t   fw_id = 0xff;
	uint8_t   arg_id = 0xff;
	uint8_t   bin_fw_id = 0xff;
	uint8_t   bin_arg_id = 0xff;
	uint16_t  fw_checksum = !BIN_CHECKSUM;
	uint16_t  bin_checksum = BIN_CHECKSUM;
	uint8_t   bin_type = BIN_TYPE;
	uint8_t   down_type = DOWN_LOAD_TYPE;
	uint16_t  find_arg_offset = 0;
	uint8_t   arg_order_id = 0;

	hal_HstSendEvent(0xbbbb0000);
	buf[1] = 0xff;
	while(retry--)
	{
		ret = bl_get_pjid_fwid_argid(buf);
		if(!ret)
			break;
		MDELAY(30);
	}
	bl_log_trace("bl_get_pjid_fwid_argid return %d", ret);
	
	if(!ret)
	{
		fw_id = buf[0];
		arg_id = buf[1];
		g_pj_id = buf[2];
		retry = 3;
		while(retry--)
		{
			ret = bl_get_tp_id(&tp_id);
			if(!ret)
				break;
			MDELAY(30);
		}
	}
	
	bin_type = check_bin((uint8_t*)fwbin, &find_arg_offset);
	bl_get_arg_order(bin_type, tp_id, find_arg_offset, &arg_order_id);
	if(bin_type != ARG_TYPE)
	{
		bl_get_bin_checksum(bin_type ,find_arg_offset, arg_order_id, &bin_checksum);
	}
	if(!ret)
	{
		bl_get_bin_fwid_argid(bin_type, find_arg_offset, arg_order_id,  &bin_arg_id, &bin_fw_id);
		if(tp_id == 0xff)
		{
			down_type = ERASE_TYPE;
			ret = 1;
		}
		else if(fw_id < bin_fw_id || (fw_id == 0xff && bin_fw_id != fw_id))
		{
			down_type = DOWN_LOAD_TYPE;
			ret = 1;
		}
		else if(arg_id < bin_arg_id || (arg_id == 0xff && bin_arg_id != arg_id) || tp_id != g_pj_id)
		{
			down_type = UPDATE_ARG_TYPE;
			ret = 1;
		}
	}
	else
	{
		down_type = DOWN_LOAD_TYPE;
	}

	//debug
	g_pj_id_init = g_pj_id;
	g_fw_id = fw_id;
	g_arg_id = arg_id;
	g_bin_fw_id = bin_fw_id;
	g_bin_arg_id = bin_arg_id;
	g_bin_type = bin_type;
	g_down_type = down_type;


	bl_log_trace("bl_update_fw print tp information");
	bl_log_trace( "fw_id : %d",  fw_id);
	bl_log_trace( "arg_id : %d",  arg_id);
	bl_log_trace( "g_pj_id : %d",  g_pj_id);
	bl_log_trace( "tp_id : %d", tp_id);

	bl_log_trace( "bin_fw_id : %d",  bin_fw_id);
	bl_log_trace( "bin_arg_id : %d",  bin_arg_id);

    bl_log_trace( "bin_type : %d",  bin_type);
	bl_log_trace( "down_type : %d",  down_type);

	bl_log_trace( "find_arg_offset : %d", find_arg_offset);
	bl_log_trace( "arg_order_id : %d", arg_order_id);
	
    bl_log_trace( "ret : %d",  ret);
	
	retry = 3;
	while((retry--) && ret)
	{
		hal_HstSendEvent(0xbbbb0001);
		SET_INTUP_LOW;
		MDELAY(5);
		if(bin_type == ARG_TYPE)
		{
			if(down_type == ERASE_TYPE)
			{
				ret = bl_erase_flash();
				MDELAY(200);
			}
			else
			{
				ret = bl_update_argument(bin_type, find_arg_offset, arg_order_id);
				down_type = UPDATE_ARG_TYPE;
			}
		}
		else
		{
			if(down_type == UPDATE_ARG_TYPE)
			{
				ret = bl_update_argument(bin_type, find_arg_offset, arg_order_id);
			}
			else  if(down_type == DOWN_LOAD_TYPE)
			{
				ret = bl_download_flash(bin_type, find_arg_offset, arg_order_id);
			}
			else if(down_type == ERASE_TYPE)
			{
				ret = bl_erase_flash();
				MDELAY(200);
			}
		}

		bl_log_trace("update ret is %d", ret );
		hal_HstSendEvent(0xbbbb0002);
		if(1 == ret)
		{				
			continue;	
		}
		SET_INTUP_HIGH;
		MDELAY(50);
		if(down_type == UPDATE_ARG_TYPE || down_type == ERASE_TYPE)
		{
			hal_HstSendEvent(0xbbbb0003);
			return ret;
		}
		bl_get_flash_checksum(&fw_checksum);

		bl_log_trace("(fw_checksum, bin_checksum) : (%x, %x) ", fw_checksum, bin_checksum);
		if(fw_checksum == bin_checksum)
		{
			bl_log_trace("fw_checksum == bin_checksum");
			hal_HstSendEvent(0xbbbb0004);
			g_pj_id = tp_id;
			ret = 0;
			break;
		}
		else
		{
			bl_log_trace("fw_checksum != bin_checksum");
			hal_HstSendEvent(0xbbbb0005);
			ret = 1;
		}
		
	}
	if(1 == ret)
	{
		SET_INTUP_HIGH;
	}
	return ret;
}

