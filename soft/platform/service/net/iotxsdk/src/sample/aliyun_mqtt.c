
#include "platform/aliyun_iot_platform_memory.h"
#include "utility/aliyun_iot_common_log.h"
#include "aliyun_iot_auth.h"
#include "system/aliyun_iot_device.h"
#include "lwip/opt.h"
#include "lwip/apps/mqtt.h"
#include "lwip/apps/mqtt_priv.h"

//The product and device information from IOT console
//TODO Please Enter your product and device information into the following macro.
#define PRODUCT_KEY         "TtTPwGtmHzX"
#define DEVICE_NAME         "rdaiot"
#define DEVICE_ID           "PbbdBVKhwCjAi8FKCPji"
#define DEVICE_SECRET       "9ZV9n23ZBIU5kIKenfe757ysoXar4oLt"

//This is the pre-defined topic
#define TOPIC_UPDATE         "/"PRODUCT_KEY"/"DEVICE_NAME"/update"
#define TOPIC_ERROR          "/"PRODUCT_KEY"/"DEVICE_NAME"/update/error"
#define TOPIC_GET            "/"PRODUCT_KEY"/"DEVICE_NAME"/rongzhang/get"

#define MSG_LEN_MAX         (1024)


static char my_msg[MSG_LEN_MAX];

static HANDLE aliyunmqtt_handle;


/**
 * @brief This is a callback function.
 *
 * @param [in] md: message pointer
 * @return none
 * @see none.
 * @note none.
 */
/*static void messageArrived(MessageData *md)
{
    uint32_t msg_len;
    MQTTMessage *message = md->message;

   if (message->payloadlen < MSG_LEN_MAX - 1) {
        msg_len = message->payloadlen;
   } else {
       ALIOT_LOG_INFO("message is too long to be stored, truncate it");
       msg_len = MSG_LEN_MAX - 1;
   }

    //copy the message to your buffer
    memcpy(my_msg, message->payload, msg_len);
*/


    /*
     * *********** Your Code ***********
     */



    //print the message
 //   ALIOT_LOG_DEBUG("msg = %s", my_msg);
//}


static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{

    ALIOT_LOG_DEBUG("mqtt_connection_status is %d the arg is %s", status, arg);
}

int mqtt_client()
{
    int rc = 0, ch = 0, msg_len, cnt;
    char msg_pub[128];
    err_t err;

    struct netif *mqtt_if = NULL;
    UINT8 nCID = startGprsLink("CMNET",0);
    while (mqtt_if = getGprsNetIf(nCID) == NULL) {
        sys_arch_printf("start_test_aliyun waiting link up ...");
        COS_Sleep(1000);
    }

    /* initialize device info */
    aliyun_iot_device_init();

    if (0 != aliyun_iot_set_device_info(PRODUCT_KEY, DEVICE_NAME, DEVICE_ID, DEVICE_SECRET))
    {
        sys_arch_printf("run aliyun_iot_set_device_info() error!");
        return -1;
    }

    /* Device AUTH */
    if (0 != aliyun_iot_auth(aliyun_iot_get_device_info(), aliyun_iot_get_user_info()))
    {
        ALIOT_LOG_INFO("run aliyun_iot_auth() error!");
        return 0;
    }


    /* start mqtt connect */
    struct mqtt_connect_client_info_t ci;
    aliot_user_info_pt puser_info;
    puser_info = aliyun_iot_get_user_info();
    ip_addr_t server_ip;
    char * host_name = puser_info->host_name;

    CFW_Gethostbyname(host_name, &server_ip, 1, 0x01);
    /* Setup an empty client info structure */
    memset(&ci, 0, sizeof(ci));
    ci.client_id = puser_info->client_id;
    ci.client_pass = puser_info->password;
    ci.client_user = puser_info->user_name;
    ci.keep_alive = 180;
    mqtt_client_t mqtt_client;
    mqtt_client.conn_state = 0;
    err = mqtt_client_connect(&mqtt_client, &server_ip, puser_info->port, mqtt_connection_cb, 0, &ci);
    /* For now just print the result code if something goes wrong*/
    if(err != ERR_OK) {
        sys_arch_printf("mqtt_connect return %d\n", err);
    }

    sys_arch_printf("out of demo!");
    return 0;
}

void start_test_aliyun()
{
    LWIP_DEBUGF(LWIP_DBG_TYPES_ON | LWIP_DBG_LEVEL_ALL,("singleThreadDemo Started..."));

	aliyunmqtt_handle = sys_thread_new("aliyun_test Task", mqtt_client, NULL, 4096, TCPIP_THREAD_PRIO+1);
    LWIP_DEBUGF(LWIP_DBG_TYPES_ON | LWIP_DBG_LEVEL_ALL,("singleThreadDemo creat ..."));
}

