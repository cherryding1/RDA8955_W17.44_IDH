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

#include "at.h"
#include "at_module.h"
#include "at_cfg.h"
#include "at_setting_codec.h"
#include "fs/sf_api.h"

#define AT_CFG_FN_PATTERN "AT_CFG_%d.BIN"
#define AT_CFG_FN_LEN (SF_OBJ_NAME_LEN)
#define AT_CFG_NUM_MAX (1)

extern const at_setting_t gDefaultSettings;
at_setting_t gAtCurrentSetting; // current settings

static void POSSIBLY_UNUSED at_CfgFileName(uint8_t id, uint8_t *buf)
{
    sprintf(buf, AT_CFG_FN_PATTERN, id);
}

static bool POSSIBLY_UNUSED at_ReadCfgFile(const char *fn, at_setting_t *setting)
{
    int len = sf_safefile_data_size(fn);
    if (len < 0)
        return false;

    uint8_t *buffer = at_malloc(len);
    if (buffer == NULL)
        return false;

    int rdl = sf_safefile_read(fn, buffer, len);
    if (rdl < 0)
    {
        ATLOGE("AT CFG failed to read config file");
        goto free_failed;
    }

    int act = at_setting_decode(setting, buffer, rdl);
    if (act < 0)
        goto free_failed;

    at_free(buffer);
    return true;

free_failed:
    at_free(buffer);
    return false;
}

static bool POSSIBLY_UNUSED at_WriteCfgFile(char *filename, at_setting_t *setting)
{
    uint8_t *pb_buffer = NULL;
    unsigned length = 0;

    if (!at_setting_encode(setting, &pb_buffer, &length))
        goto free_failed;

    if (!sf_safefile_write(filename, pb_buffer, length))
    {
        ATLOGE("AT CFG failed to write config file");
        goto free_failed;
    }

    at_free(pb_buffer);
    return true;

free_failed:
    at_free(pb_buffer);
    return false;
}

// API to save at settings
bool at_CfgSetAtSettings(uint8_t set_id)
{
    if (set_id >= AT_CFG_NUM_MAX)
        return false;

    uint8_t filename[AT_CFG_FN_LEN];
    at_CfgFileName(set_id, filename);
    return at_WriteCfgFile(filename, &gAtCurrentSetting);
}

// API to get at settings
bool at_CfgGetAtSettings(uint8_t flag, uint8_t set_id)
{
    ATLOGV("AT CFG get, flag=%u, id=%u", flag, set_id);

    if (flag == MANUFACTURER_DEFALUT_SETING)
    {
        gAtCurrentSetting = gDefaultSettings;
        return true;
    }

    if (flag == USER_SETTING_1 && set_id < AT_CFG_NUM_MAX)
    {
        uint8_t filename[AT_CFG_FN_LEN];
        at_CfgFileName(set_id, filename);

        at_setting_t setting = gDefaultSettings;
        if (!at_ReadCfgFile(filename, &setting))
            goto failed;
        gAtCurrentSetting = setting;
        return true;
    }

failed:
    gAtCurrentSetting = gDefaultSettings;
    return false;
}

bool at_CfgInit(void)
{
    uint8_t filename[AT_CFG_FN_LEN];
    for (int n = 0; n < AT_CFG_NUM_MAX; n++)
    {
        at_CfgFileName(n, filename);
        sf_safefile_init(filename);
    }
    return true;
}
