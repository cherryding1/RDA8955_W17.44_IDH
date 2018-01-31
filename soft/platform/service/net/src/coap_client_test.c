#ifdef LWIP_COAP_TEST

#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "cJSON.h"
#include "jsmn.h"


static const char *RESULT_STRING = "{\"resultCode\": \"0\", \"resultDesc\": \"Success\"}";
int coap_flag = 0;
int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
  if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
      strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
    return 0;
  }
  return -1;
}

void parseResultJsonString(char *jsonString)
{
  if (!jsonString)
  {
    sys_arch_printf("parseResultJsonString() - jsonString is NULL\n");
    return;
  }

  int i;
  int r;
  jsmn_parser p;
  jsmntok_t t[128]; /* We expect no more than 128 tokens */

  jsmn_init(&p);
  r = jsmn_parse(&p, jsonString, strlen(jsonString), t, sizeof(t)/sizeof(t[0]));
  if (r < 0) {
    sys_arch_printf("parseResultJsonString() - Failed to parse JSON: %d\n", r);
    return;
  }

  /* Assume the top-level element is an object */
  if (r < 1 || t[0].type != JSMN_OBJECT) {
    sys_arch_printf("parseResultJsonString() - Object expected\n");
    return;
  }

  char *resultCode;
  char *resultDesc;
  int len;
  /* Loop over all keys of the root object */
  for (i = 1; i < r; i++) {
    if (jsoneq(jsonString, &t[i], "resultCode") == 0) {
      len = t[i+1].end - t[i+1].start;
      resultCode= (char *)COS_MALLOC(len + 1);
      strncpy(resultCode, jsonString + t[i+1].start, len);

      i++;
    } else if (jsoneq(jsonString, &t[i], "resultDesc") == 0) {
      len = t[i+1].end - t[i+1].start;
      resultDesc= (char *)COS_MALLOC(len + 1);
      strncpy(resultDesc, jsonString + t[i+1].start, len);

      i++;
    } else {
      sys_arch_printf("parseResultJsonString() - Unexpected key: %.*s\n", t[i].end-t[i].start,
          jsonString + t[i].start);
    }
  }

  if (resultCode)
  {
    sys_arch_printf("parseResultJsonString() - resultCode: %s\n", resultCode);
  }

  if (resultDesc)
  {
    sys_arch_printf("parseResultJsonString() - resultDesc: %s\n", resultDesc);
  }
}


char *generateJsonString(char *regver, char *meid, char *modelsms,
    char *swver, char *SIM1Iccid, char *SIM1LteImsi)
{
  cJSON * cjson = NULL;
  cjson = cJSON_CreateObject();
  if (NULL == cjson)
  {
    sys_arch_printf("printJsonString() - cjson is NULL\n");
    return;
  }

  cJSON_AddStringToObject(cjson, "REGVER", regver);
  cJSON_AddStringToObject(cjson, "MEID", meid);
  cJSON_AddStringToObject(cjson, "MODELSMS", modelsms);
  cJSON_AddStringToObject(cjson, "SWVER", swver);
  cJSON_AddStringToObject(cjson, "SIM1ICCID", SIM1Iccid);
  cJSON_AddStringToObject(cjson, "SIM1LTEIMSI", SIM1LteImsi);

  char *out = cJSON_PrintUnformatted(cjson);
  if (out)
  {
    sys_arch_printf("printJsonString() - p: %s\n", out);
  }
}

int coap_PostJsonMethodTest(UINT8 nCID)
{
  int nResult = -1;

  char *out = generateJsonString("1", "aa", "aa", "aa", "aa", "aa");
  if (!out)
  {
    sys_arch_printf("coap_PostJsonMethodTest() - out is null\n");
    return -1;
  }
  coap_client_set_payload(out,strlen(out),0);
  char *argv[] = { "coap_client","-m","post",
    "-t","application/json",
    "-p","5683","coap://123.57.221.42:5683/large-post"};
  nResult = coap_client_main(8,argv);
  return nResult;
}

int coap_GetMethodTest(UINT8 nCID)
{
  int nResult = -1;
  char *argv[] = { "coap_client","-m","get",
    "-p","5683","coap://123.57.221.42"};
  nResult = coap_client_main(6,argv);
  return nResult;
}

int coap_PutMethodTest(UINT8 nCID)
{
  int nResult = -1;
  char *argv[] = { "coap_client","-m","put",
    "-t","text", 
    "-e","Coap put method demo",
    "-p","5683","coap://123.57.221.42:5683/large-update"};
  nResult = coap_client_main(10,argv);
  return nResult;
}

int coap_PostMethodTest(UINT8 nCID)
{
  int nResult = -1;
  char *argv[] = { "coap_client","-m","post",
    "-t","text",
    "-e","Coap post method demo",
    "-p","5683","coap://123.57.221.42:5683/large-update"};
  nResult = coap_client_main(10,argv);
  return nResult;
}

int coap_DeleteMethodTest(UINT8 nCID)
{
  int nResult = -1;
  char *argv[] = { "coap_client","-m","delete",
    "-p","5683","coap://123.57.221.42:5683/obs"};
  nResult = coap_client_main(6,argv);
  return nResult;
}

int coaps_GetMethodTest(UINT8 nCID)
{
  int nResult = -1;
  char *argv[] = { "coaps_client","-m","get",
  	"-k","sesame","-u","password",//secretPSK  Client_identity
    "-p","5684","coaps://123.57.221.42/"};
  nResult = coap_client_main(10,argv);
  return nResult;
}

int coaps_PutMethodTest(UINT8 nCID)
{
  int nResult = -1;
  char *argv[] = { "coaps_client","-m","put",
    "-t","text", 
    "-e","Coaps put method demo",
    "-k","sesame","-u","password",//secretPSK  Client_identity
    "-p","5684","coaps://123.57.221.42/large-update"};
  nResult = coap_client_main(14,argv);
  return nResult;
}

int coaps_PostMethodTest(UINT8 nCID)
{
  int nResult = -1;
  char *argv[] = { "coaps_client","-m","post",
    "-t","text",
    "-k","sesame","-u","password",
    "-e","Coaps post method demo",
    "-p","5684","coaps://123.57.221.42/large-create"};
  nResult = coap_client_main(14,argv);
  return nResult;
}

int coaps_DeleteMethodTest(UINT8 nCID)
{
  int nResult = -1;
  char *argv[] = { "coaps_client","-m","delete",
  	"-k","sesame","-u","password",
  	"-p","5684","coaps://123.57.221.42/create1"};
  nResult = coap_client_main(10,argv);
  return nResult;
}

static HANDLE coap_handle;
#define COAP_THREAD_STACKSIZE			4096
#define COAP_THREAD_PRIO               (TCPIP_THREAD_PRIO+2)

static void test_coap(void *arg)
{
    int ret = -2;
    struct netif *coap_if = NULL;
    UINT8 nCID = startGprsLink("uniwap",0);
    while (coap_if = getGprsNetIf(0,nCID) == NULL) {
        sys_arch_printf("test_coap waiting link up ...");
        COS_Sleep(1000);
    }
    ret = coap_GetMethodTest(nCID);
    sys_arch_printf("test_coap get ret = %d", ret);
    ret = coap_PostMethodTest(nCID);
    sys_arch_printf("test_coap post ret = %d", ret);
    coap_PostJsonMethodTest(nCID);
    sys_arch_printf("test_coap post json ret = %d", ret);
    ret = coap_PutMethodTest(nCID);
    sys_arch_printf("test_coap put ret = %d", ret);
    ret = coap_DeleteMethodTest(nCID);
    sys_arch_printf("test_coap connect delete ret = %d", ret);
    char *argv[] = { "coap_client","-p","5683","coap://californium.eclipse.org:5683/"};
    ret = coap_client_main(4,argv);

	//test coaps
    ret = coaps_GetMethodTest(nCID);
    sys_arch_printf("test_coaps get ret = %d", ret);
	COS_Sleep(2000);
	ret = coaps_PutMethodTest(nCID);
    sys_arch_printf("test_coaps put ret = %d", ret);
	COS_Sleep(2000);
    ret = coaps_PostMethodTest(nCID);
    sys_arch_printf("test_coaps post ret = %d", ret);
	COS_Sleep(2000);
    ret = coaps_DeleteMethodTest(nCID);
    sys_arch_printf("test_coaps connect delete ret = %d", ret);

    COS_StopTask(coap_handle);
    
}

void start_coap_test()
{
	if(coap_flag == 1){
        COS_DeleteTask(coap_handle);
        coap_flag = 0;
    }
    if(coap_flag == 0){
		coap_handle = sys_thread_new("coap_handle", test_coap, NULL, COAP_THREAD_STACKSIZE, COAP_THREAD_PRIO);
		coap_flag = 1;
    }
}

#endif
