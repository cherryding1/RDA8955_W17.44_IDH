/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *******************************************************************************/
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <time.h>

#include "MQTTSNPacket.h"
#include "lwip/sockets.h"
#include "netinet/in.h"
#include "lwip/opt.h"

#if !defined(SOCKET_ERROR)
    /** error in socket operation */
    #define SOCKET_ERROR -1
#endif

#define INVALID_SOCKET SOCKET_ERROR

#define mqsleep sleep
#define START_TIME_TYPE struct timeval
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#define min(a, b) ((a < b) ? a : b)

#define assert(a, b, c, d) myassert(__FILE__, __LINE__, a, b, c, d)
#define assert1(a, b, c, d, e) myassert(__FILE__, __LINE__, a, b, c, d, e)

int tests = 0;
int failures = 0;
START_TIME_TYPE global_start_time;
char output[3000];
char* cur_output = output;
int mysock = 0;

struct Options
{
    char* host;  
    int port;
    int verbose;
    int test_no;
} options =
{
    "47.95.235.10",
    1884,
    0,
    0,
};


/* TODO - unused - remove? static struct timeval start_time; */
START_TIME_TYPE start_clock(void)
{
    struct timeval start_time;
    gettimeofday(&start_time, NULL);
    return start_time;
}

void myassert(char* filename, int lineno, char* description, int value, char* format, ...)
{
    ++tests;
    if (!value)
    {
        va_list args;

        ++failures;
        sys_arch_printf("Assertion failed, file %s, line %d, description: %s\n", filename, lineno, description);

        cur_output += sprintf(cur_output, "<failure type=\"%s\">file %s, line %d </failure>\n",
                        description, filename, lineno);
    }
    else
        sys_arch_printf("Assertion succeeded, file %s, line %d, description: %s", filename, lineno, description);
}

int Socket_error(char* aString, int sock)
{
    int errno;

    if (errno != EINTR && errno != EAGAIN && errno != EINPROGRESS && errno != EWOULDBLOCK)
    {
        if (strcmp(aString, "shutdown") != 0 || (errno != ENOTCONN && errno != ECONNRESET))
        {
            int orig_errno = errno;
            char* errmsg = strerror(errno);

            sys_arch_printf("Socket error %d (%s) in %s for socket %d\n", orig_errno, errmsg, aString, sock);
        }
    }
    return errno;
}

int sendPacketBuffer(int asocket, char* host, int port, unsigned char* buf, int buflen)
{
    struct sockaddr_in cliaddr;
    int rc = 0;

    memset(&cliaddr, 0, sizeof(cliaddr));
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_addr.s_addr = inet_addr(host);
    cliaddr.sin_port = htons(port);

    if ((rc = sendto(asocket, buf, buflen, 0, (const struct sockaddr*)&cliaddr, sizeof(cliaddr))) == SOCKET_ERROR)
        Socket_error("sendto", asocket);
    else
        rc = 0;
    return rc;
}

int getdata(unsigned char* buf, int count)
{
    int rc = recvfrom(mysock, buf, count, 0, NULL, NULL);
    sys_arch_printf("received %d bytes count %d\n", rc, (int)count);
    return rc;
}

int connectDisconnect(struct Options options)
{
    MQTTSNPacket_connectData data = MQTTSNPacket_connectData_initializer;
    int rc = 0;
    unsigned char buf[100];
    int buflen = sizeof(buf);
    int len = 0;

    mysock = socket(AF_INET, SOCK_DGRAM, 0);
    if (mysock == INVALID_SOCKET)
        rc = Socket_error("socket", mysock);

    data.clientID.cstring = "RDA_mqttsn_test";

    data.cleansession = 0;

    rc = MQTTSNSerialize_connect(buf, buflen, &data);
    assert("good rc from serialize connect", rc > 0, "rc was %d\n", rc);

    rc = sendPacketBuffer(mysock, options.host, options.port, buf, rc);
    assert("good rc from sendPacketBuffer", rc == 0, "rc was %d\n", rc);

    /* wait for connack */
    if (MQTTSNPacket_read(buf, buflen, getdata) == MQTTSN_CONNACK)
    {
        int connack_rc = -1;

        if (MQTTSNDeserialize_connack(&connack_rc, buf, buflen) != 1 || connack_rc != 0)
        {
            sys_arch_printf("Unable to connect, return code %d\n", connack_rc);
            goto exit;
        }
        else 
            sys_arch_printf("connected rc %d\n", connack_rc);
    }
    else
        goto exit;

    len = MQTTSNSerialize_disconnect(buf, buflen, 0);
    rc = sendPacketBuffer(mysock, options.host, options.port, buf, len);

    rc = shutdown(mysock, SHUT_WR);
    rc = close(mysock);

exit:
    return rc;
}

int test1(struct Options options)
{

    global_start_time = start_clock();
    failures = 0;
    sys_arch_printf( "Starting test 1 - reconnection");

    connectDisconnect(options);
    connectDisconnect(options);

    sys_arch_printf( "TEST1: test %s. %d tests run, %d failures.",
            (failures == 0) ? "passed" : "failed", tests, failures);
    return failures;
}


int connectSubscribeDisconnect(struct Options options)
{
    MQTTSNPacket_connectData data = MQTTSNPacket_connectData_initializer;
    int rc = 0;
    unsigned char buf[100];
    int buflen = sizeof(buf);
    int len = 0;
    int msgid = 1;
    MQTTSN_topicid topic;
    unsigned short topicid;

    mysock = socket(AF_INET, SOCK_DGRAM, 0);
    if (mysock == INVALID_SOCKET)
        rc = Socket_error("socket", mysock);

    data.clientID.cstring = "RDA_mqttsn_test";
    data.cleansession = 0;

    rc = MQTTSNSerialize_connect(buf, buflen, &data);
    assert("good rc from serialize connect", rc > 0, "rc was %d\n", rc);

    rc = sendPacketBuffer(mysock, options.host, options.port, buf, rc);
    assert("good rc from sendPacketBuffer", rc == 0, "rc was %d\n", rc);

    /* wait for connack */
    if (MQTTSNPacket_read(buf, buflen, getdata) == MQTTSN_CONNACK)
    {
        int connack_rc = -1;

        rc = MQTTSNDeserialize_connack(&connack_rc, buf, buflen);
        assert("Good rc from deserialize connack", rc == 1, "rc was %d\n", rc);
        assert("Good rc from connect", connack_rc == 0, "connack_rc was %d\n", rc);
        if (connack_rc != 0)
            goto exit;
    }
    else
        goto exit;

    /* subscribe */
    sys_arch_printf("Subscribing\n");
    topic.type = MQTTSN_TOPIC_TYPE_NORMAL;
    topic.data.long_.name = "substopic";
    topic.data.long_.len = strlen(topic.data.long_.name);
    len = MQTTSNSerialize_subscribe(buf, buflen, 0, 2, msgid, &topic);
    rc = sendPacketBuffer(mysock, options.host, options.port, buf, len);

    if (MQTTSNPacket_read(buf, buflen, getdata) == MQTTSN_SUBACK)     /* wait for suback */
    {
        unsigned short submsgid;
        int granted_qos;
        unsigned char returncode;

        rc = MQTTSNDeserialize_suback(&granted_qos, &topicid, &submsgid, &returncode, buf, buflen);
        if (granted_qos != 2 || returncode != 0)
        {
            sys_arch_printf("granted qos != 2, %d return code %d\n", granted_qos, returncode);
            goto exit;
        }
        else
            sys_arch_printf("suback topic id %d\n", topicid);
    }
    else
        goto exit;
    
    sys_arch_printf("Publishing\n");
    /* publish with short name */
    topic.type = MQTTSN_TOPIC_TYPE_NORMAL;
    topic.data.id = topicid;
    ++msgid;
    unsigned char dup = 0;
    int qos = 1;
    unsigned char retained = 0;
    unsigned char* payload = (unsigned char*)"mypayload";
    int payloadlen = strlen((char*)payload);
    len = MQTTSNSerialize_publish(buf, buflen, dup, qos, retained, msgid,
            topic, payload, payloadlen);
    rc = sendPacketBuffer(mysock,options.host, options.port, buf, len);

    /* wait for puback */
    if (MQTTSNPacket_read(buf, buflen, getdata) == MQTTSN_PUBACK)
    {
        unsigned short packet_id, topic_id;
        unsigned char returncode;

        if (MQTTSNDeserialize_puback(&topic_id, &packet_id, &returncode, buf, buflen) != 1
                        || returncode != MQTTSN_RC_ACCEPTED)
            sys_arch_printf("Unable to publish, return code %d\n", returncode);
        else
            sys_arch_printf("puback received, msgid %d topic id %d\n", packet_id, topic_id);
    }
    else
        goto exit;

    sys_arch_printf("Receive publish\n");
    if (MQTTSNPacket_read(buf, buflen, getdata) == MQTTSN_PUBLISH)
    {
        unsigned short packet_id;
        int qos, payloadlen;
        unsigned char* payload;
        unsigned char dup, retained;
        MQTTSN_topicid pubtopic;

        if (MQTTSNDeserialize_publish(&dup, &qos, &retained, &packet_id, &pubtopic,
                &payload, &payloadlen, buf, buflen) != 1)
            sys_arch_printf("Error deserializing publish\n");
        else
            sys_arch_printf("publish received, id %d qos %d\n", packet_id, qos);

        if (qos == 1)
        {
            len = MQTTSNSerialize_puback(buf, buflen, pubtopic.data.id, packet_id, MQTTSN_RC_ACCEPTED);
            rc = sendPacketBuffer(mysock,options.host, options.port, buf, len);
            if (rc == 0)
                sys_arch_printf("puback sent\n");
        }
    }
    else
        goto exit;

    len = MQTTSNSerialize_disconnect(buf, buflen, 0);
    rc = sendPacketBuffer(mysock, options.host, options.port, buf, len);

    rc = shutdown(mysock, SHUT_WR);
    rc = close(mysock);

exit:
    return rc;
}


/*
 * Connect non-cleansession, subscribe, disconnect.
 * Then reconnect non-cleansession.
 */
int test2(struct Options options)
{

    global_start_time = start_clock();
    failures = 0;
    sys_arch_printf("Starting test 2 - clientid free");

    connectSubscribeDisconnect(options);
    connectDisconnect(options);

    sys_arch_printf("TEST2: test %s. %d tests run, %d failures.",
            (failures == 0) ? "passed" : "failed", tests, failures);
    return failures;
}


int main_test(void)
{
    COS_EVENT event = { 0 };
    struct netif *mqtt_if = NULL;
    UINT8 nCID = startGprsLink("CMNET",0);
    while (mqtt_if = getGprsNetIf(0,nCID) == NULL) {
        sys_arch_printf("start_test_mqttsn waiting link up ...");
        COS_Sleep(1000);
    }

    int rc = 0;
     int (*tests[])() = {NULL, test1, test2};

     if (options.test_no == 0)
    { /* run all the tests */
            for (options.test_no = 1; options.test_no < ARRAY_SIZE(tests); ++options.test_no)
            rc += tests[options.test_no](options); /* return number of failures.  0 = test succeeded */
    }
    else
            rc = tests[options.test_no](options); /* run just the selected test */

    if (rc == 0)
        sys_arch_printf( "verdict pass");
    else
        sys_arch_printf("verdict fail");

    return rc;
}

static HANDLE mqtt_handle;
void start_test_mqttsn()
{
    mqtt_handle = sys_thread_new("mqttsn_thread", main_test, NULL, 2048, TCPIP_THREAD_PRIO-1);
}


