#ifdef LWIP_HTTP_TEST

#include "cJSON.h"
#include "jsmn.h"
#include "clog.h"

char *httpgenerateJsonString(char *regver, char *meid, char *modelsms,
    char *swver, char *SIM1Iccid, char *SIM1LteImsi)
{
  cJSON * cjson = NULL;
  cjson = cJSON_CreateObject();
  if (NULL == cjson)
  {
    cg_log_debug_s("JSON error ++ NULL");
    return;
  }
  
  cJSON_AddStringToObject(cjson, "REGVER", regver);
  cJSON_AddStringToObject(cjson, "MEID", meid);
  cJSON_AddStringToObject(cjson, "MODELSMS", modelsms);
  cJSON_AddStringToObject(cjson, "SWVER", swver);
  cJSON_AddStringToObject(cjson, "SIM1ICCID", SIM1Iccid);
  cJSON_AddStringToObject(cjson, "SIM1LTEIMSI", SIM1LteImsi);

  char* out = cJSON_PrintUnformatted(cjson);
  cg_log_debug_s("JSON++ %s",out);
  return out;
}
#endif