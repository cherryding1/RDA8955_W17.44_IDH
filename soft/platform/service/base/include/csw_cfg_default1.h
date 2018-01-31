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

#ifndef __REG_DEFAULT_H1__
#define __REG_DEFAULT_H1__
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
    "\"CCWA\"=int8:1\n"\
    "\"CLIR\"=int8:0\n"\
    "\"COLP\"=int8:0\n"\
    "[REG_HKEY_CURRENT_COMM/general]\n"\
    "\"MEMORYINFO\"=hex:00,00,00,00,00,00,00,00,00,00,00,00,00,00,01,01,00,00,00,00,00,00,00,00\n"\
    "\"IND_EVENT\"=hex:ff,ff\n"\
    "\"IMSI\"=hex:00,ff,ff,ff,ff,ff,ff,ff,ff,ff\n"\
    "[REG_HKEY_CURRENT_COMM/nw]\n"\
    "\"NW_WORK_MODE\"=int8:0\n"\
    "\"NW_FLIGHT_MODE\"=int8:0\n"\
    "\"NW_FREQ_BAND\"=int8:76\n"\
    "\"NW_SIM0_PLMN\"=hex:00,00,00\n"\
    "\"NW_SIM0_BA_COUNT\"=int8:0\n"\
    "\"NW_SIM0_ARFCN_LIST\"=hex:00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"NW_SIM0_STORE_LAST\"=hex:00,00\n"\
    "\"GPRS_UP_SUM_SIM0\"=int32:0\n"\
    "\"GPRS_DWN_SUM_SIM0\"=int32:0\n"\
    "[REG_HKEY_CURRENT_COMM/sim]\n"\
    "\"SimCardSwitch\"=int8:0\n"\
    "\"SoftReset\"=int8:0\n"\
    "\"IsFirstPowerOn\"=int8:1\n"\
    "[REG_HKEY_CURRENT_COMM/sms]\n"\
    "\"SMS_STORAGE\"=int8:2\n"\
    "\"SMS_OVER_IND\"=int8:0\n"\
    "\"FULL_FOR_NEW\"=int8:0\n"\
    "\"DEF_PARAM_IND\"=int8:0\n"\
    "\"SMS_PARAM1\"=hex:00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"SMS_PARAM2\"=hex:00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"SMS_PARAM3\"=hex:00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"SMS_PARAM4\"=hex:00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"SMS_PARAM5\"=hex:00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"SMS_STORAGEINFO1\"=hex:00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"SMS_STORAGEINFO2\"=hex:00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"SMS_STORAGEINFO3\"=hex:00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"SMS_STORAGEINFO4\"=hex:00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00\n"\
    "\"SMS_OPTION\"=int8:4\n"\
    "\"SMS_OPT_STORAGE\"=int8:2\n"\
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
    "\"SIM_APN_NUM\"=hex:00\n"\
    "[REG_HKEY_CURRENT_COMM/DBS]\n"\
    "\"SEED\"=\"0000000\"\n";

#endif//__REG_DEFAULT_H1__





