#ifdef LWIP_MQTT_TEST
#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "lwip/apps/mqtt.h"
#include "sockets.h"
#include "lwip/apps/mqtt_priv.h"

#define PROCESS_IN_MQTT_THREAD  1

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status);
static void mqtt_sub_request_cb(void *arg, err_t result);
static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len);
static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags);
static void mqtt_pub_request_cb(void *arg, err_t result);

mqtt_client_t static_client;

#define EV_MQTT_CONNECT_CB  1
#define EV_MQTT_SUB_REQ_CB  2
#define EV_MQTT_INCOMING_PUB_CB 3
#define EV_MQTT_INCOMING_DATA_CB 4
#define EV_MQTT_PUB_REQ_CB 5

#define EV_MQTT_CONNECT   10
#define EV_MQTT_DISCONNECT   11
#define EV_MQTT_PUBLISH   12
#define EV_MQTT_UNPUBLISH   13

static HANDLE mqtt_handle;

void example_do_connect(mqtt_client_t *client)
{
  struct mqtt_connect_client_info_t ci;
  ip_addr_t server_ip;
  err_t err;
  
  /* Setup an empty client info structure */
  memset(&ci, 0, sizeof(ci));
  
  /* Minimal amount of information required is client identifier, so set it here */ 
  ci.client_id = "lwip_test";
  ci.keep_alive = 120;
  char hostName[] = "test.mosquitto.org";
  if (CFW_Gethostbyname(hostName, &server_ip, 1, 0x01) != RESOLV_COMPLETE)
    return;

  /* Initiate client and connect to server, if this fails immediately an error code is returned
     otherwise mqtt_connection_cb will be called with connection result after attempting 
     to establish a connection with the server. 
     For now MQTT version 3.1.1 is always used */
  
  err = mqtt_client_connect(client, &server_ip, MQTT_PORT, mqtt_connection_cb, 0, &ci);
  
  /* For now just print the result code if something goes wrong*/
  if(err != ERR_OK) {
    sys_arch_printf("mqtt_connect return %d\n", err);
  }
}

static void mqtt_connection_cb_process(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
    err_t err;
    if(status == MQTT_CONNECT_ACCEPTED) {
      sys_arch_printf("mqtt_connection_cb: Successfully connected\n");
      
      /* Setup callback for incoming publish requests */
      mqtt_set_inpub_callback(client, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, arg);
      
      /* Subscribe to a topic named "subtopic" with QoS level 1, call mqtt_sub_request_cb with result */ 
      err = mqtt_subscribe(client, "#", 1, mqtt_sub_request_cb, arg);
    
      if(err != ERR_OK) {
        sys_arch_printf("mqtt_subscribe return: %d\n", err);
      }
    }else if(status == MQTT_CONNECT_DISCONNECTED) {
      sys_arch_printf("mqtt_connection_cb: Disconnected, reason: %d\n", status);
      example_do_connect(client);
    }else {
      sys_arch_printf("mqtt_connection_cb: Other, reason: %d\n", status);
      /* Its more nice to be connected, so try to reconnect */
      example_do_connect(client);
    }
}

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
#ifdef PROCESS_IN_MQTT_THREAD
    COS_EVENT ev={0};
    ev.nEventId = EV_MQTT_CONNECT_CB;
    ev.nParam1 = (int32_t)client;
    ev.nParam2 = (int32_t)arg;
    ev.nParam3 = (int32_t)status;
    COS_SendEvent(mqtt_handle,&ev,COS_WAIT_FOREVER,COS_EVENT_PRI_NORMAL);
#else
    mqtt_connection_cb_process(client,arg,status);
#endif
}
static void mqtt_sub_request_cb_process(void *arg, err_t result)
{
  /* Just print the result code here for simplicity, 
     normal behaviour would be to take some action if subscribe fails like 
     notifying user, retry subscribe or disconnect from server */
  sys_arch_printf("Subscribe result: %d\n", result);
}

static void mqtt_sub_request_cb(void *arg, err_t result)
{
#ifdef PROCESS_IN_MQTT_THREAD
    COS_EVENT ev={0};
    ev.nEventId = EV_MQTT_SUB_REQ_CB;
    ev.nParam1 = (int32_t)arg;
    ev.nParam2 = (int32_t)result;
    COS_SendEvent(mqtt_handle,&ev,COS_WAIT_FOREVER,COS_EVENT_PRI_NORMAL);
#else
    mqtt_sub_request_cb_process(arg,result);
#endif

}


/* The idea is to demultiplex topic and create some reference to be used in data callbacks
   Example here uses a global variable, better would be to use a member in arg
   If RAM and CPU budget allows it, the easiest implementation might be to just take a copy of
   the topic string and use it in mqtt_incoming_data_cb
*/
static int inpub_id;
static void mqtt_incoming_publish_cb_process(void *arg, const char *topic, u32_t tot_len)
{
  sys_arch_printf("Incoming publish at topic %s with total length %u\n", topic, (unsigned int)tot_len);

  /* Decode topic string into a user defined reference */
  if(strcmp(topic, "print_payload") == 0) {
    inpub_id = 0;
  } else if(topic[0] == 'A') {
    /* All topics starting with 'A' might be handled at the same way */
    inpub_id = 1;
  } else {
    /* For all other topics */
    inpub_id = 2;
  }
}

static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len)
{
#ifdef PROCESS_IN_MQTT_THREAD
    COS_EVENT ev={0};
    ev.nEventId = EV_MQTT_INCOMING_PUB_CB;
    ev.nParam1 = (int32_t)arg;
    ev.nParam2 = (int32_t)topic;
    ev.nParam3 = (int32_t)tot_len;
    COS_SendEvent(mqtt_handle,&ev,COS_WAIT_FOREVER,COS_EVENT_PRI_NORMAL);
#else
    mqtt_incoming_publish_cb_process(arg,topic,tot_len);
#endif
}
static void mqtt_incoming_data_cb_process(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
  sys_arch_printf("Incoming publish payload with length %d, flags %u\n", len, (unsigned int)flags);

  if(flags & MQTT_DATA_FLAG_LAST) {
    /* Last fragment of payload received (or whole part if payload fits receive buffer
       See MQTT_VAR_HEADER_BUFFER_LEN)  */

    /* Call function or do action depending on reference, in this case inpub_id */
    if(inpub_id == 0) {
      /* Don't trust the publisher, check zero termination */
      if(data[len-1] == 0) {
        sys_arch_printf("mqtt_incoming_data_cb: %s\n", (const char *)data);
      }
    } else if(inpub_id == 1) {
      /* Call an 'A' function... */
    } else {
      sys_arch_printf("mqtt_incoming_data_cb: Ignoring payload...\n");
    }
  } else {
    /* Handle fragmented payload, store in buffer, write to file or whatever */
  }
}

typedef struct _indata_t {
    const u8_t *data;
    u16_t len;
} in_data_t;

static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
#ifdef PROCESS_IN_MQTT_THREAD
    in_data_t *indata = (in_data_t *)COS_MALLOC(sizeof(in_data_t));
    indata->data = data;
    indata->len = len;
    COS_EVENT ev={0};
    ev.nEventId = EV_MQTT_INCOMING_DATA_CB;
    ev.nParam1 = (int32_t)arg;
    ev.nParam2 = (int32_t)indata;
    ev.nParam3 = (int32_t)flags;
    COS_SendEvent(mqtt_handle,&ev,COS_WAIT_FOREVER,COS_EVENT_PRI_NORMAL);
#else
    mqtt_incoming_data_cb_process(arg,data,len,flags);
#endif
}

void example_publish(mqtt_client_t *client, void *arg)
{
  const char *pub_payload= "PubSubHubLubJub";
  err_t err;
  u8_t qos = 2; /* 0 1 or 2, see MQTT specification */
  u8_t retain = 0; /* No don't retain such crappy payload... */
  u8_t dup = 0; /* The DUP flag MUST be set to 0 for all QoS 0 messages */
  err = mqtt_publish(client, "pub_topic", pub_payload, strlen(pub_payload), dup, qos, retain, mqtt_pub_request_cb, arg);
  if(err != ERR_OK) {
    sys_arch_printf("Publish err: %d\n", err);
  }
}

/* Called when publish is complete either with sucess or failure */
static void mqtt_pub_request_cb_process(void *arg, err_t result)
{
  if(result != ERR_OK) {
    sys_arch_printf("Publish result: %d\n", result);
  }
}

static void mqtt_pub_request_cb(void *arg, err_t result)
{
#ifdef PROCESS_IN_MQTT_THREAD
    COS_EVENT ev={0};
    ev.nEventId = EV_MQTT_PUB_REQ_CB;
    ev.nParam1 = (int32_t)arg;
    ev.nParam2 = (int32_t)result;
    COS_SendEvent(mqtt_handle,&ev,COS_WAIT_FOREVER,COS_EVENT_PRI_NORMAL);
#else
    mqtt_pub_request_cb_process(arg,result);
#endif
}

static void test_mqtt(void *arg)
{
    COS_EVENT event = { 0 };
    struct netif *mqtt_if = NULL;
#ifdef WIFI_SUPPORT
    mqtt_if = startWifiLink("FreeAndroid","rdadroid");
#else
    UINT8 nCID = startGprsLink("CMNET",0);
    while (mqtt_if = getGprsNetIf(0,nCID) == NULL) {
        sys_arch_printf("start_test_mqtt waiting link up ...");
        COS_Sleep(1000);
    }
#endif
    example_do_connect(&static_client);
#ifdef PROCESS_IN_MQTT_THREAD
    for (;;) {
        COS_WaitEvent(mqtt_handle, &event, COS_WAIT_FOREVER);
        if(event.nEventId == EV_MQTT_CONNECT_CB) {
            mqtt_client_t *client = (mqtt_client_t *)event.nParam1;
            void *arg = (void *)event.nParam2;
            mqtt_connection_status_t status = (mqtt_connection_status_t)event.nParam3;
            mqtt_connection_cb_process(client,arg,status);
        }else if (event.nEventId == EV_MQTT_SUB_REQ_CB) {
            void *arg = (void *)event.nParam1;
            err_t result = (err_t)event.nParam2;
            mqtt_sub_request_cb_process(arg,result);
        }else if (event.nEventId == EV_MQTT_INCOMING_PUB_CB) {
            void *arg = (void *)event.nParam1;
            const char *topic = (const char *)event.nParam2;
            u32_t tot_len = (u32_t)event.nParam3;
            mqtt_incoming_publish_cb_process(arg,topic,tot_len);
        }else if (event.nEventId == EV_MQTT_INCOMING_DATA_CB) {
            void *arg = (void *)event.nParam1;
            in_data_t *in_data = (in_data_t *)event.nParam2;
            u8_t flags = (u8_t)event.nParam3;
            u8_t *data = in_data->data;
            u16_t len = in_data->len;
            mqtt_incoming_data_cb_process(arg,data,len,flags);
            COS_FREE(in_data);
        }else if (event.nEventId == EV_MQTT_PUB_REQ_CB) {        
            void *arg = (void *)event.nParam1;
            err_t result = (err_t)event.nParam2;
            mqtt_pub_request_cb_process(arg,result);
        }else if (event.nEventId == EV_MQTT_CONNECT) {
            example_do_connect(&static_client);
        }else if (event.nEventId == EV_MQTT_DISCONNECT) {
            mqtt_disconnect(&static_client);
        }else if (event.nEventId == EV_MQTT_PUBLISH) {
            example_publish(&static_client,NULL);
        }else if (event.nEventId == EV_MQTT_UNPUBLISH) {
            
        }
    }
#else
    COS_Sleep(10000);
    example_publish(&static_client,NULL);
    COS_Sleep(10000);
    mqtt_disconnect(&static_client);
    while (1)
        COS_Sleep(10000);
#endif
}

#define MQTT_THREAD_STACKSIZE			1024
#define MQTT_THREAD_PRIO               (TCPIP_THREAD_PRIO-1)

void start_test_mqtt()
{
	mqtt_handle = sys_thread_new("mqtt_thread", test_mqtt, NULL, MQTT_THREAD_STACKSIZE, MQTT_THREAD_PRIO);
}
#endif
