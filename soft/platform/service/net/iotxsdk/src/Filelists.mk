
COMMONFILES=$(ALIYUNDIR)/utility/aliyun_iot_common_hexdump.c \
	$(ALIYUNDIR)/utility/aliyun_iot_common_hmac.c \
	$(ALIYUNDIR)/utility/aliyun_iot_common_httpclient.c \
	$(ALIYUNDIR)/utility/aliyun_iot_common_jsonparser.c \
	$(ALIYUNDIR)/utility/aliyun_iot_common_list.c \
	$(ALIYUNDIR)/utility/aliyun_iot_common_log.c \
	$(ALIYUNDIR)/utility/aliyun_iot_common_md5.c \
	$(ALIYUNDIR)/utility/aliyun_iot_common_sha1.c \
	$(ALIYUNDIR)/utility/aliyun_iot_common_net.c \
      $(ALIYUNDIR)/utility/aliyun_iot_common_timer.c


MATTPACKET=$(ALIYUNDIR)/mqtt/MQTTConnectClient.c \
	$(ALIYUNDIR)/mqtt/MQTTConnectServer.c \
	$(ALIYUNDIR)/mqtt/MQTTDeserializePublish.c \
	$(ALIYUNDIR)/mqtt/MQTTFormat.c \
	$(ALIYUNDIR)/mqtt/MQTTPacket.c \
	$(ALIYUNDIR)/mqtt/MQTTSerializePublish.c \
      $(ALIYUNDIR)/mqtt/MQTTSubscribeClient.c \
      $(ALIYUNDIR)/mqtt/MQTTSubscribeServer.c \
      $(ALIYUNDIR)/mqtt/MQTTUnsubscribeClient.c \
      $(ALIYUNDIR)/mqtt/MQTTUnsubscribeServer.c \
      $(ALIYUNDIR)/mqtt/aliyun_iot_mqtt_client.c 

AUTH=$(ALIYUNDIR)/auth/aliyun_iot_auth.c

PLATFORM=$(ALIYUNDIR)/platform/aliyun_iot_platform_network.c \
         $(ALIYUNDIR)/platform/aliyun_iot_platform_memory.c \
         $(ALIYUNDIR)/platform/aliyun_iot_network_ssl.c

SYSTEM=$(ALIYUNDIR)/system/aliyun_iot_ca.c \
       $(ALIYUNDIR)/system/aliyun_iot_device.c

EXAMPLE=$(ALIYUNDIR)/sample/aliyun_mqtt.c

ALIYUNALLFILES=$(COMMONFILES)  \
               $(AUTH)   \
               $(PLATFORM)  \
               $(EXAMPLE) \
               $(SYSTEM)
