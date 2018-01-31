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

#ifndef __REG_DEFAULT_H3__
#define __REG_DEFAULT_H3__
const char *default_registry =
    "[REG_HKEY_SYSTEM_ROOT/Time]\n"\
    "\"TM_FORMAT\"=hex:1a,02,00,00\n"\
    "\"TM_ZONE\"=hex:00\n"\
    "\"TM_ALARM\"=hex:01\n"\
    "\"TM_BASE\"=hex:80,43,6D,38\n"\
    "\"TM_TIME\"=hex:80,43,6D,38\n"\
    "\"TM_ALARM1\"=hex:00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"TM_ALARM2\"=hex:00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"TM_ALARM3\"=hex:00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"TM_ALARM4\"=hex:00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"TM_ALARM5\"=hex:00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"TM_ALARM6\"=hex:00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"TM_ALARM7\"=hex:00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"TM_ALARM8\"=hex:00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"TM_ALARM9\"=hex:00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"TM_ALARM10\"=hex:00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"TM_ALARM11\"=hex:00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"TM_ALARM12\"=hex:00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"TM_ALARM13\"=hex:00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"TM_ALARM14\"=hex:00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"TM_ALARM15\"=hex:00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "[REG_HKEY_SYSTEM_ROOT/FS]\n"\
    "\"PSTATUS\"=int8:0\n"\
    "[REG_HKEY_CURRENT_COMM/configure]\n"\
    "\"SMS_PARAM\"=hex:00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "[REG_HKEY_CURRENT_COMM/cm]\n"\
    "\"PUBLIC\"=int32:3\n"\
    "\"CCWA_SIM0\"=int8:1\n"\
    "\"CCWA_SIM1\"=int8:1\n"\
    "\"CCWA_SIM2\"=int8:1\n"\
    "\"CLIR_SIM0\"=int8:0\n"\
    "\"CLIR_SIM1\"=int8:0\n"\
    "\"CLIR_SIM2\"=int8:0\n"\
    "\"COLP_SIM0\"=int8:0\n"\
    "\"COLP_SIM1\"=int8:0\n"\
    "\"COLP_SIM2\"=int8:0\n"\
    "[REG_HKEY_CURRENT_COMM/general]\n"\
    "\"MEMORYINFO\"=hex:00,00,00,00,00,00,00,00,00,00,00,00,00,00,01,01,01,01,00,00,00,00,00,00,00,00,00,00\n"\
    "\"IND_EVENT\"=hex:ff,ff\n"\
    "\"IMSI_SIM0\"=hex:00,ff,ff,ff,ff,ff,ff,ff,ff,ff\n"\
    "\"IMSI_SIM1\"=hex:00,ff,ff,ff,ff,ff,ff,ff,ff,ff\n"\
    "\"IMSI_SIM2\"=hex:00,ff,ff,ff,ff,ff,ff,ff,ff,ff\n"\
    "[REG_HKEY_CURRENT_COMM/nw]\n"\
    "\"NW_SIM0_WORK_MODE\"=int8:0\n"\
    "\"NW_SIM0_FLIGHT_MODE\"=int32:1\n"\
    "\"NW_SIM0_FREQ_BAND\"=int8:76\n"\
    "\"NW_SIM1_WORK_MODE\"=int8:0\n"\
    "\"NW_SIM1_FLIGHT_MODE\"=int32:1\n"\
    "\"NW_SIM1_FREQ_BAND\"=int8:76\n"\
    "\"NW_SIM2_WORK_MODE\"=int8:0\n"\
    "\"NW_SIM2_FLIGHT_MODE\"=int32:1\n"\
    "\"NW_SIM2_FREQ_BAND\"=int8:76\n"\
    "\"NW_SIM0_PLMN\"=hex:00,00,00\n"\
    "\"NW_SIM1_PLMN\"=hex:00,00,00\n"\
    "\"NW_SIM2_PLMN\"=hex:00,00,00\n"\
    "\"NW_SIM0_BA_COUNT\"=int8:0\n"\
    "\"NW_SIM1_BA_COUNT\"=int8:0\n"\
    "\"NW_SIM2_BA_COUNT\"=int8:0\n"\
    "\"NW_SIM0_ARFCN_LIST\"=hex:00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"NW_SIM1_ARFCN_LIST\"=hex:00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"NW_SIM2_ARFCN_LIST\"=hex:00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"NW_SIM0_STORE_LAST\"=hex:00,00\n"\
    "\"NW_SIM1_STORE_LAST\"=hex:00,00\n"\
    "\"NW_SIM2_STORE_LAST\"=hex:00,00\n"\
    "\"GPRS_UP_SUM_SIM0\"=int32:0\n"\
    "\"GPRS_DWN_SUM_SIM0\"=int32:0\n"\
    "\"GPRS_UP_SUM_SIM1\"=int32:0\n"\
    "\"GPRS_DWN_SUM_SIM1\"=int32:0\n"\
    "\"GPRS_UP_SUM_SIM2\"=int32:0\n"\
    "\"GPRS_DWN_SUM_SIM2\"=int32:0\n"\
    "\"GPRS_UP_SUM_SIM3\"=int32:0\n"\
    "\"GPRS_DWN_SUM_SIM3\"=int32:0\n"\
    "[REG_HKEY_CURRENT_COMM/sim]\n"\
    "\"SimCardSwitch\"=int8:0\n"\
    "\"SoftReset\"=int8:0\n"\
    "\"IsFirstPowerOn\"=int8:1\n"\
    "[REG_HKEY_CURRENT_COMM/sms]\n"\
    "\"SMS_STORAGE_0\"=int8:2\n"\
    "\"SMS_STORAGE_1\"=int8:2\n"\
    "\"SMS_STORAGE_2\"=int8:2\n"\
    "\"SMS_SIM0_OVER_IND\"=int8:0\n"\
    "\"FULL_FOR_NEW_SIM0\"=int8:0\n"\
    "\"DEF_PARAM_IND_SIM0\"=int8:0\n"\
    "\"SMS_SIM0_PARAM1\"=hex:AD,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"SMS_SIM0_PARAM2\"=hex:AD,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"SMS_SIM0_PARAM3\"=hex:AD,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"SMS_SIM0_PARAM4\"=hex:AD,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"SMS_SIM0_PARAM5\"=hex:AD,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"SMS_SIM0_OPTION\"=int8:4\n"\
    "\"SMS_SIM0_OPTSTORAGE\"=int8:2\n"\
    "\"SMS_SIM1_OVER_IND\"=int8:0\n"\
    "\"FULL_FOR_NEW_SIM1\"=int8:0\n"\
    "\"DEF_PARAM_IND_SIM1\"=int8:0\n"\
    "\"SMS_SIM1_PARAM1\"=hex:AD,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"SMS_SIM1_PARAM2\"=hex:AD,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"SMS_SIM1_PARAM3\"=hex:AD,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"SMS_SIM1_PARAM4\"=hex:AD,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"SMS_SIM1_PARAM5\"=hex:AD,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"SMS_SIM1_OPTION\"=int8:4\n"\
    "\"SMS_SIM1_OPTSTORAGE\"=int8:2\n"\
    "\"SMS_SIM2_OVER_IND\"=int8:0\n"\
    "\"FULL_FOR_NEW_SIM2\"=int8:0\n"\
    "\"DEF_PARAM_IND_SIM2\"=int8:0\n"\
    "\"SMS_SIM2_PARAM1\"=hex:AD,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"SMS_SIM2_PARAM2\"=hex:AD,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"SMS_SIM2_PARAM3\"=hex:AD,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"SMS_SIM2_PARAM4\"=hex:AD,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"SMS_SIM2_PARAM5\"=hex:AD,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"SMS_SIM2_OPTION\"=int8:4\n"\
    "\"SMS_SIM2_OPTSTORAGE\"=int8:2\n"\
    "[REG_HKEY_CURRENT_COMM/emod]\n"\
    "\"EMOD_PARAM\"=hex:00\n"\
    "\"EMOD_NWNM\"=hex:ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff\n"\
    "\"EMOD_SIGN\"=int8:0\n"\
    "\"EMOD_SIMNM_1\"=hex:ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff\n"\
    "\"EMOD_SIMNM_2\"=hex:ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff\n"\
    "[REG_HKEY_CURRENT_COMM/TCPIP]\n"\
    "\"DNS_ADR1\"=\"211.136.17.107\"\n"\
    "\"DNS_ADR2\"=\"211.136.17.107\"\n"\
    "[REG_HKEY_CURRENT_COMM/JMETOC]\n"\
    "\"APN\"=\"cmnet\"\n"\
    "\"USER\"=\"wap\"\n"\
    "\"PWD\"=\"wap\"\n"\
    "\"PROXY_ADR\"=hex:0a,00,00,ac\n"\
    "\"PROXY_PORT\"=hex:50,00\n"\
    "\"SIM_APN_NUM\"=hex:00\n";

#endif//__REG_DEFAULT_H3__





