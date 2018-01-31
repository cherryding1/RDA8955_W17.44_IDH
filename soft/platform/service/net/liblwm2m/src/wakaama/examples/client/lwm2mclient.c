/*******************************************************************************
 *
 * Copyright (c) 2013, 2014 Intel Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    David Navarro, Intel Corporation - initial API and implementation
 *    Benjamin Cabé - Please refer to git log
 *    Fabien Fleutot - Please refer to git log
 *    Simon Bernard - Please refer to git log
 *    Julien Vermillard - Please refer to git log
 *    Axel Lorente - Please refer to git log
 *    Toby Jaffey - Please refer to git log
 *    Bosch Software Innovations GmbH - Please refer to git log
 *    Pascal Rieux - Please refer to git log
 *    Christian Renz - Please refer to git log
 *    Ricky Liu - Please refer to git log
 *
 *******************************************************************************/

/*
 Copyright (c) 2013, 2014 Intel Corporation

 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:

     * Redistributions of source code must retain the above copyright notice,
       this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
     * Neither the name of Intel Corporation nor the names of its contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 THE POSSIBILITY OF SUCH DAMAGE.

 David Navarro <david.navarro@intel.com>
 Bosch Software Innovations GmbH - Please refer to git log

*/

#include "lwm2mclient.h"
#include "liblwm2m.h"
#include "commandline.h"
#if(defined WITH_TINYDTLS) 
#include "dtlsconnection.h"
#elif (defined WITH_MBEDDTLS)
#include "mbedconnection.h"
#else
#include "connection.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include "internals.h"

#define MAX_PACKET_SIZE 1024
#define DEFAULT_SERVER_IPV6 "[::1]"
#define DEFAULT_SERVER_IPV4 "127.0.0.1"

int g_reboot = 0;
static int g_quit = 0;

lwm2m_context_t * lwm2mH = NULL;
struct timeval tv;

#define OBJ_COUNT 9
lwm2m_object_t * objArray[OBJ_COUNT];

// only backup security and server objects
# define BACKUP_OBJECT_COUNT 2
lwm2m_object_t * backupObjectArray[BACKUP_OBJECT_COUNT];

typedef struct
{
    lwm2m_object_t * securityObjP;
    lwm2m_object_t * serverObject;
    int sock;
#if (defined WITH_TINYDTLS) || (defined WITH_MBEDDTLS)
    dtls_connection_t * connList;
    lwm2m_context_t * lwm2mH;
#else
    connection_t * connList;
#endif
    int addressFamily;
} client_data_t;

static void prv_quit(char * buffer,
                     void * user_data)
{
    g_quit = 1;
    lwm2m_close(lwm2mH);
}

static void handle_sigint(int signum)
{
    g_quit = 2;
}

int handle_value_changed(lwm2m_context_t * lwm2mH,
                          lwm2m_uri_t * uri,
                          const char * value,
                          size_t valueLength)
{
    int handleResult = -1;

    lwm2m_object_t * object = (lwm2m_object_t *)LWM2M_LIST_FIND(lwm2mH->objectList, uri->objectId);

    if (NULL != object)
    {
        if (object->writeFunc != NULL)
        {
            lwm2m_data_t * dataP;
            int result;

            dataP = lwm2m_data_new(1);
            if (dataP == NULL)
            {
                fprintf(stderr, "Internal allocation failure !\n");
                return handleResult;
            }
            dataP->id = uri->resourceId;
            lwm2m_data_encode_nstring(value, valueLength, dataP);

            result = object->writeFunc(uri->instanceId, 1, dataP, object);
            if (COAP_405_METHOD_NOT_ALLOWED == result)
            {
                switch (uri->objectId)
                {
                case LWM2M_DEVICE_OBJECT_ID:
                    result = device_change(dataP, object);
                    break;
                default:
                    break;
                }
            }

            if (COAP_204_CHANGED != result)
            {
                fprintf(stderr, "Failed to change value!\n");
            }
            else
            {
                fprintf(stderr, "value changed!\n");
                lwm2m_report_data("value changed!\n", strlen("value changed!\n"));
                handleResult = 1;
                lwm2m_resource_value_changed(lwm2mH, uri);
            }
            lwm2m_data_free(1, dataP);
            return handleResult;
        }
        else
        {
            fprintf(stderr, "write not supported for specified resource!\n");
        }
        return handleResult;
    }
    else
    {
        fprintf(stderr, "Object not found !\n");
    }
    return handleResult;
}

#if(defined WITH_TINYDTLS) || (defined WITH_MBEDDTLS)
void * lwm2m_connect_server(uint16_t secObjInstID,
                            void * userData)
{
  client_data_t * dataP;
  lwm2m_list_t * instance;
  dtls_connection_t * newConnP = NULL;
  dataP = (client_data_t *)userData;
  lwm2m_object_t  * securityObj = dataP->securityObjP;
  
  instance = LWM2M_LIST_FIND(dataP->securityObjP->instanceList, secObjInstID);
  if (instance == NULL) return NULL;
  
  
  newConnP = connection_create(dataP->connList, dataP->sock, securityObj, instance->id, dataP->lwm2mH, dataP->addressFamily);
  if (newConnP == NULL)
  {
      fprintf(stderr, "Connection creation failed.\n");
      return NULL;
  }
  
  dataP->connList = newConnP;
  return (void *)newConnP;
}
#else
void * lwm2m_connect_server(uint16_t secObjInstID,
                            void * userData)
{
    client_data_t * dataP;
    char * uri;
    char * host;
    char * port;
    connection_t * newConnP = NULL;

    dataP = (client_data_t *)userData;

    uri = get_server_uri(dataP->securityObjP, secObjInstID);

    if (uri == NULL) return NULL;

    // parse uri in the form "coaps://[host]:[port]"
    if (0==strncmp(uri, "coaps://", strlen("coaps://"))) {
        host = uri+strlen("coaps://");
    }
    else if (0==strncmp(uri, "coap://",  strlen("coap://"))) {
        host = uri+strlen("coap://");
    }
    else {
        goto exit;
    }
    port = strrchr(host, ':');
    if (port == NULL) goto exit;
    // remove brackets
    if (host[0] == '[')
    {
        host++;
        if (*(port - 1) == ']')
        {
            *(port - 1) = 0;
        }
        else goto exit;
    }
    // split strings
    *port = 0;
    port++;

    fprintf(stderr, "Opening connection to server at %s:%s\r\n", host, port);
    newConnP = connection_create(dataP->connList, dataP->sock, host, port, dataP->addressFamily);
    if (newConnP == NULL) {
        fprintf(stderr, "Connection creation failed.\r\n");
    }
    else {
        dataP->connList = newConnP;
    }

exit:
    lwm2m_free(uri);
    return (void *)newConnP;
}
#endif

void lwm2m_close_connection(void * sessionH,
                            void * userData)
{
    client_data_t * app_data;
#if(defined WITH_TINYDTLS) || (defined WITH_MBEDDTLS)
    dtls_connection_t * targetP;
#else
    connection_t * targetP;
#endif

    app_data = (client_data_t *)userData;
#if(defined WITH_TINYDTLS) || (defined WITH_MBEDDTLS)
    targetP = (dtls_connection_t *)sessionH;
#else
    targetP = (connection_t *)sessionH;
#endif

    if (targetP == app_data->connList)
    {
        app_data->connList = targetP->next;
        lwm2m_free(targetP);
    }
    else
    {
#if(defined WITH_TINYDTLS) || (defined WITH_MBEDDTLS)
        dtls_connection_t * parentP;
#else
        connection_t * parentP;
#endif

        parentP = app_data->connList;
        while (parentP != NULL && parentP->next != targetP)
        {
            parentP = parentP->next;
        }
        if (parentP != NULL)
        {
            parentP->next = targetP->next;
            lwm2m_free(targetP);
        }
    }
}

static void prv_output_servers(char * buffer,
                               void * user_data)
{
    UINT8 *tempStr;
    tempStr = (char *) lwm2m_malloc(TEMP_LENGTH);

    lwm2m_context_t * lwm2mH = (lwm2m_context_t *) user_data;
    lwm2m_server_t * targetP;

    targetP = lwm2mH->bootstrapServerList;

    if (lwm2mH->bootstrapServerList == NULL)
    {
        fprintf(stdout, "No Bootstrap Server.\r\n");
        if (tempStr)
        {
            snprintf(tempStr, TEMP_LENGTH, "No Bootstrap Server.\r\n");
        }
    }
    else
    {
        fprintf(stdout, "Bootstrap Servers:\r\n");
        for (targetP = lwm2mH->bootstrapServerList ; targetP != NULL ; targetP = targetP->next)
        {
            fprintf(stdout, " - Security Object ID %d", targetP->secObjInstID);
            fprintf(stdout, "\tHold Off Time: %lu s", (unsigned long)targetP->lifetime);
            fprintf(stdout, "\tstatus: ");
            switch(targetP->status)
            {
            case STATE_DEREGISTERED:
                fprintf(stdout, "DEREGISTERED\r\n");
                break;
            case STATE_BS_HOLD_OFF:
                fprintf(stdout, "CLIENT HOLD OFF\r\n");
                break;
            case STATE_BS_INITIATED:
                fprintf(stdout, "BOOTSTRAP INITIATED\r\n");
                break;
            case STATE_BS_PENDING:
                fprintf(stdout, "BOOTSTRAP PENDING\r\n");
                break;
            case STATE_BS_FINISHED:
                fprintf(stdout, "BOOTSTRAP FINISHED\r\n");
                break;
            case STATE_BS_FAILED:
                fprintf(stdout, "BOOTSTRAP FAILED\r\n");
                break;
            default:
                fprintf(stdout, "INVALID (%d)\r\n", (int)targetP->status);
            }
        }
    }

    if (lwm2mH->serverList == NULL)
    {
        fprintf(stdout, "No LWM2M Server.\r\n");
    }
    else
    {
        fprintf(stdout, "LWM2M Servers:\r\n");
        for (targetP = lwm2mH->serverList ; targetP != NULL ; targetP = targetP->next)
        {
            fprintf(stdout, " - Server ID %d", targetP->shortID);
            fprintf(stdout, "\tstatus: ");

            if (tempStr)
            {
                snprintf(tempStr + strlen(tempStr), TEMP_LENGTH, " - Server ID %d", targetP->shortID);
                snprintf(tempStr + strlen(tempStr), TEMP_LENGTH, "\tstatus: ");
            }

            switch(targetP->status)
            {
            case STATE_DEREGISTERED:
                fprintf(stdout, "DEREGISTERED\r\n");
                break;
            case STATE_REG_PENDING:
                fprintf(stdout, "REGISTRATION PENDING\r\n");
                break;
            case STATE_REGISTERED:
                fprintf(stdout, "REGISTERED\tlocation: \"%s\"\tLifetime: %lus\r\n", targetP->location, (unsigned long)targetP->lifetime);
                snprintf(tempStr + strlen(tempStr), TEMP_LENGTH, "REGISTERED\tlocation: \"%s\"\tLifetime: %lus\r\n", targetP->location, (unsigned long)targetP->lifetime);
                break;
            case STATE_REG_UPDATE_PENDING:
                fprintf(stdout, "REGISTRATION UPDATE PENDING\r\n");
                break;
            case STATE_DEREG_PENDING:
                fprintf(stdout, "DEREGISTRATION PENDING\r\n");
                break;
            case STATE_REG_FAILED:
                fprintf(stdout, "REGISTRATION FAILED\r\n");
                break;
            default:
                fprintf(stdout, "INVALID (%d)\r\n", (int)targetP->status);
            }
        }
    }
    lwm2m_report_data(tempStr, strlen(tempStr));
    lwm2m_report_at_command_result(1);
}

void prv_change(char * buffer,
                       void * user_data)
{
    lwm2m_uri_t uri;
    char * end = NULL;
    int result;

    end = get_end_of_arg(buffer);
    if (end[0] == 0)
    {
        lwm2m_report_data("Syntax error !\n", strlen("Syntax error !\n"));
        lwm2m_report_at_command_result(0);
        goto syntax_error;
    }

    result = lwm2m_stringToUri(buffer, end - buffer, &uri);
    if (result == 0)
    {
        lwm2m_report_data("Syntax error !\n", strlen("Syntax error !\n"));
        lwm2m_report_at_command_result(0);
        goto syntax_error;
    }

    buffer = get_next_arg(end, &end);

    if (buffer[0] == 0)
    {
        fprintf(stderr, "report change!\n");
        lwm2m_report_data("report change!\n", strlen("report change!\n"));
        lwm2m_report_at_command_result(1);
        lwm2m_resource_value_changed(lwm2mH, &uri);
    }
    else
    {
        result = handle_value_changed(lwm2mH, &uri, buffer, end - buffer);
        lwm2m_report_at_command_result(result);
    }
    return;

syntax_error:
    fprintf(stdout, "Syntax error !\n");
}

#ifdef AT_MIPL_SUPPORT
static void prv_observe(char * buffer,
                       void * user_data)
{
/*
    lwm2m_context_t * contextP = (lwm2m_context_t *) user_data;
    lwm2m_uri_t uriP;
    int res;
    int size;
    int result;
    int format;
    static coap_packet_t message[1];
    static coap_packet_t response[1];
    connection_t * connP;
    lwm2m_data_t * dataP;
    lwm2m_server_t * serverP;
    uint8_t * data = NULL;

    serverP = utils_findServer(contextP, connP);
    if (serverP != NULL)
    {
        goto syntax_error;
    }

    result = object_readData(contextP, &uriP, &size, &dataP);
    if (COAP_205_CONTENT == result)
    {
        result = observe_handleRequest(contextP, &uriP, serverP, size, dataP, message, response);
        if (COAP_205_CONTENT == result)
        {
            format = LWM2M_CONTENT_JSON;
            res = lwm2m_data_serialize(&uriP, size, dataP, &format, &data);
            lwm2m_data_free(size, dataP);
        }
    }
	else
	{
	    goto syntax_error;
	}
    return;
syntax_error:
    fprintf(stdout, "Syntax error !\n");
*/
    lwm2m_report_at_command_result(1);
}

static void prv_exec(char * buffer,
                       void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *) user_data;
    lwm2m_uri_t uri;
    char * end = NULL;
	char * data = NULL;
	char * payload = NULL;
	int length = 0;
    int result;
	int format;

    end = get_end_of_arg(buffer);
    if (end[0] == 0) goto syntax_error;

    result = lwm2m_stringToUri(buffer, end - buffer, &uri);
    if (result == 0) goto syntax_error;

    format = LWM2M_CONTENT_JSON;
	result = object_execute(lwm2mH, &uri, payload, length);
	if (COAP_204_CHANGED == result)
	{
	    printf("execute success\n");
	}
    return;

syntax_error:
    fprintf(stdout, "Syntax error !\n");
}

static void prv_read(char * buffer,
                       void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *) user_data;
    lwm2m_uri_t uri;
    char * end = NULL;
	char * data = NULL;
	int length;
    int result;
	int format;

    end = get_end_of_arg(buffer);
    if (end[0] == 0) goto syntax_error;

    result = lwm2m_stringToUri(buffer, end - buffer, &uri);
    if (result == 0) goto syntax_error;

    format = LWM2M_CONTENT_JSON;
    result = object_read(lwm2mH, &uri, &format, &data, &length);
    if (COAP_205_CONTENT == result)
    {
        lwm2m_report_data(data, strlen(data));
        lwm2m_report_at_command_result(1);
    }
    else
    {
        lwm2m_report_at_command_result(0);
        lwm2m_report_data("read resource value error\n", strlen("read resource value error\n"));
    }
    return;
syntax_error:
    fprintf(stdout, "Syntax error !\n");
    lwm2m_report_at_command_result(0);
    lwm2m_report_data("Syntax error !\n", strlen("Syntax error !\n"));
}

static void pro_delete_instance(char * buffer,
                       void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *)user_data;
    int result;
    char *end = NULL;
    lwm2m_uri_t uriP;

    end = get_end_of_arg(buffer);
    if (end[0] == 0) goto syntax_error;

    result = lwm2m_stringToUri(buffer, end - buffer, &uriP);
    if (result == 0) goto syntax_error;

    result = object_delete(lwm2mH, &uriP);
    if (result == COAP_202_DELETED)
    {
        //lwm2m_update_registration(lwm2mH, 0, true);
        lwm2m_step(lwm2mH, &(tv.tv_sec));
    }
    return;

syntax_error:
	fprintf(stdout, "Syntax error !\n");
}
#endif

static void prv_object_list(char * buffer,
                            void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *)user_data;
    lwm2m_object_t * objectP;

    char *data = NULL;
    data = (char *)lwm2m_malloc(TEMP_LENGTH);
    for (objectP = lwm2mH->objectList; objectP != NULL; objectP = objectP->next)
    {
        if (objectP->instanceList == NULL)
        {
            fprintf(stdout, "/%d ", objectP->objID);
            if (data)
            {
                snprintf(data + strlen(data), TEMP_LENGTH, "/%d ", objectP->objID);
            }
        }
        else
        {
            lwm2m_list_t * instanceP;

            for (instanceP = objectP->instanceList; instanceP != NULL ; instanceP = instanceP->next)
            {
                fprintf(stdout, "/%d/%d  ", objectP->objID, instanceP->id);
                if (data)
                {
                    snprintf(data + strlen(data), TEMP_LENGTH, "/%d/%d  ", objectP->objID, instanceP->id);
                }
            }
        }
        fprintf(stdout, "\r\n");
        if (data)
        {
            snprintf(data + strlen(data), TEMP_LENGTH, "\r\n");
        }
    }
    lwm2m_report_data(data, strlen(data));
    lwm2m_report_at_command_result(1);
}

static void prv_instance_dump(lwm2m_object_t * objectP,
                              uint16_t id)
{
    int numData;
    lwm2m_data_t * dataArray;
    uint16_t res;

    numData = 0;
    res = objectP->readFunc(id, &numData, &dataArray, objectP);
    if (res != COAP_205_CONTENT)
    {
        printf("Error ");
        print_status(stdout, res);
        printf("\r\n");
        return;
    }

    dump_tlv(stdout, numData, dataArray, 0);
}

static void prv_object_dump(char * buffer,
                            void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *) user_data;
    lwm2m_uri_t uri;
    char * end = NULL;
    int result;
    lwm2m_object_t * objectP;

    if (!buffer)
    {
        goto syntax_error;
    }

    end = get_end_of_arg(buffer);
/*
    if (end[0] == 0)
    {
        lwm2m_report_data("Syntax error !\n", strlen("Syntax error !\n"));
        lwm2m_report_at_command_result(0);
        goto syntax_error;
    }
*/
    result = lwm2m_stringToUri(buffer, end - buffer, &uri);
    if (result == 0)
    {
        lwm2m_report_data("Syntax error !\n", strlen("Syntax error !\n"));
        lwm2m_report_at_command_result(0);
        goto syntax_error;
    }

    if (uri.flag & LWM2M_URI_FLAG_RESOURCE_ID)
    {
        lwm2m_report_data("Syntax error !\n", strlen("Syntax error !\n"));
        lwm2m_report_at_command_result(0);
        goto syntax_error;
    }

    objectP = (lwm2m_object_t *)LWM2M_LIST_FIND(lwm2mH->objectList, uri.objectId);

    if (objectP == NULL)
    {
        fprintf(stdout, "Object not found.\n");
        lwm2m_report_data("Object not found.\n", strlen("Object not found.\n"));
        lwm2m_report_at_command_result(0);
        return;
    }

    if (uri.flag & LWM2M_URI_FLAG_INSTANCE_ID)
    {
        prv_instance_dump(objectP, uri.instanceId);
    }
    else
    {
        lwm2m_list_t * instanceP;

        for (instanceP = objectP->instanceList; instanceP != NULL ; instanceP = instanceP->next)
        {
            fprintf(stdout, "Instance %d:\r\n", instanceP->id);

            int tempLength;
            char *tmepData = NULL;

            tempLength = strlen("Instance %d:\r\n") + 2;
            tmepData = (char *)lwm2m_malloc(tempLength);
            if (tmepData)
            {
                snprintf(tmepData, tempLength, "Instance %d:\r\n", instanceP->id);
                lwm2m_report_data(tmepData, tmepData);
            }

            prv_instance_dump(objectP, instanceP->id);
            fprintf(stdout, "\r\n");
            lwm2m_report_data("\r\n", 2);
        }
    }
    lwm2m_report_at_command_result(1);
    return;

syntax_error:
    fprintf(stdout, "Syntax error !\n");
}

static void prv_update(char * buffer,
                       void * user_data)
{
    //lwm2m_context_t * lwm2mH = (lwm2m_context_t *)user_data;
    if (buffer[0] == 0)
    {
        lwm2m_report_at_command_result(0);
        goto syntax_error;
    }

    uint16_t serverId = (uint16_t) atoi(buffer);
    int res = lwm2m_update_registration(lwm2mH, serverId, false);
    if (res != 0)
    {
        fprintf(stdout, "Registration update error: ");
        print_status(stdout, res);
        fprintf(stdout, "\r\n");

        int dataLength;
        dataLength = strlen("Registration update error: ") + strlen(prv_status_to_string(res)) + 7 + 2;
        char *data;
        data = (char *)lwm2m_malloc(dataLength);
        if (data)
        {
            snprintf(data, dataLength, "Registration update error: %d.%02d (%s)\r\n",
                    (res&0xE0)>>5, res&0x1F, prv_status_to_string(res));
            lwm2m_report_data(data, dataLength);
        }
        lwm2m_report_at_command_result(0);
    }
    return;

syntax_error:
    fprintf(stdout, "Syntax error !\n");
}

static void update_battery_level(lwm2m_context_t * context)
{
    static time_t next_change_time = 0;
    time_t tv_sec;

    tv_sec = lwm2m_gettime();
    if (tv_sec < 0) return;

    if (next_change_time < tv_sec)
    {
        char value[15];
        int valueLength;
        lwm2m_uri_t uri;
        int level = rand() % 100;

        if (0 > level) level = -level;
        if (lwm2m_stringToUri("/3/0/9", 6, &uri))
        {
            valueLength = sprintf(value, "%d", level);
            fprintf(stderr, "New Battery Level: %d\n", level);
            handle_value_changed(context, &uri, value, valueLength);
        }
        level = rand() % 20;
        if (0 > level) level = -level;
        next_change_time = tv_sec + level + 10;
    }
}

static void prv_add(char * buffer,
                    void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *)user_data;
    lwm2m_object_t * objectP;
    int res;

    objectP = get_test_object();
    if (objectP == NULL)
    {
        fprintf(stdout, "Creating object 1024 failed.\r\n");
        lwm2m_report_data("Creating object 1024 failed.\r\n",strlen("Creating object 1024 failed.\r\n"));
        lwm2m_report_at_command_result(0);
        return;
    }
    res = lwm2m_add_object(lwm2mH, objectP);
    if (res != 0)
    {
        fprintf(stdout, "Adding object 1024 failed: ");
        print_status(stdout, res);
        fprintf(stdout, "\r\n");

        int dataLength;
        dataLength = strlen("Adding object 1024 failed: ") + strlen(prv_status_to_string(res)) + 7 + 2;
        char *data;
        data = (char *)lwm2m_malloc(dataLength);
        if (data)
        {
            snprintf(data, dataLength, "Adding object 1024 failed: %d.%02d (%s)\r\n",
                    (res&0xE0)>>5, res&0x1F, prv_status_to_string(res));
            lwm2m_report_data(data, dataLength);
        }
        lwm2m_report_at_command_result(0);
    }
    else
    {
        fprintf(stdout, "Object 1024 added.\r\n");
    }
    return;
}

static void prv_remove(char * buffer,
                       void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *)user_data;
    int res;

    res = lwm2m_remove_object(lwm2mH, 1024);
    if (res != 0)
    {
        fprintf(stdout, "Removing object 1024 failed: ");
        print_status(stdout, res);
        fprintf(stdout, "\r\n");

        int dataLength;
        dataLength = strlen("Removing object 1024 failed: ") + strlen(prv_status_to_string(res)) + 7 + 2;
        char *data;
        data = (char *)lwm2m_malloc(dataLength);
        if (data)
        {
            snprintf(data, dataLength, "Removing object 1024 failed: %d.%02d (%s)\r\n",
                (res&0xE0)>>5, res&0x1F, prv_status_to_string(res));
            lwm2m_report_data(data, dataLength);
            lwm2m_report_at_command_result(0);
        }
    }
    else
    {
        fprintf(stdout, "Object 1024 removed.\r\n");
        lwm2m_report_data("Object 1024 removed.\r\n", strlen("Object 1024 removed.\r\n"));
    }
    return;
}

#ifdef LWM2M_BOOTSTRAP

static void prv_initiate_bootstrap(char * buffer,
                                   void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *)user_data;
    lwm2m_server_t * targetP;

    // HACK !!!
    lwm2mH->state = STATE_BOOTSTRAP_REQUIRED;
    targetP = lwm2mH->bootstrapServerList;
    while (targetP != NULL)
    {
        targetP->lifetime = 0;
        targetP = targetP->next;
    }
}

static void prv_display_objects(char * buffer,
                                void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *)user_data;
    lwm2m_object_t * object;

    for (object = lwm2mH->objectList; object != NULL; object = object->next){
        if (NULL != object) {
            switch (object->objID)
            {
            case LWM2M_SECURITY_OBJECT_ID:
                display_security_object(object);
                break;
            case LWM2M_SERVER_OBJECT_ID:
                display_server_object(object);
                break;
            case LWM2M_ACL_OBJECT_ID:
                break;
            case LWM2M_DEVICE_OBJECT_ID:
                display_device_object(object);
                break;
            case LWM2M_CONN_MONITOR_OBJECT_ID:
                break;
            case LWM2M_FIRMWARE_UPDATE_OBJECT_ID:
                display_firmware_object(object);
                break;
            case LWM2M_LOCATION_OBJECT_ID:
                display_location_object(object);
                break;
            case LWM2M_CONN_STATS_OBJECT_ID:
                break;
            case TEST_OBJECT_ID:
                display_test_object(object);
                break;
            }
        }
    }
}

static void prv_display_backup(char * buffer,
        void * user_data)
{
   int i;
   for (i = 0 ; i < BACKUP_OBJECT_COUNT ; i++) {
       lwm2m_object_t * object = backupObjectArray[i];
       if (NULL != object) {
           switch (object->objID)
           {
           case LWM2M_SECURITY_OBJECT_ID:
               display_security_object(object);
               break;
           case LWM2M_SERVER_OBJECT_ID:
               display_server_object(object);
               break;
           default:
               break;
           }
       }
   }
}

static void prv_backup_objects(lwm2m_context_t * context)
{
    uint16_t i;

    for (i = 0; i < BACKUP_OBJECT_COUNT; i++) {
        if (NULL != backupObjectArray[i]) {
            switch (backupObjectArray[i]->objID)
            {
            case LWM2M_SECURITY_OBJECT_ID:
                clean_security_object(backupObjectArray[i]);
                lwm2m_free(backupObjectArray[i]);
                break;
            case LWM2M_SERVER_OBJECT_ID:
                clean_server_object(backupObjectArray[i]);
                lwm2m_free(backupObjectArray[i]);
                break;
            default:
                break;
            }
        }
        backupObjectArray[i] = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));
        memset(backupObjectArray[i], 0, sizeof(lwm2m_object_t));
    }

    /*
     * Backup content of objects 0 (security) and 1 (server)
     */
    copy_security_object(backupObjectArray[0], (lwm2m_object_t *)LWM2M_LIST_FIND(context->objectList, LWM2M_SECURITY_OBJECT_ID));
    copy_server_object(backupObjectArray[1], (lwm2m_object_t *)LWM2M_LIST_FIND(context->objectList, LWM2M_SERVER_OBJECT_ID));
}

static void prv_restore_objects(lwm2m_context_t * context)
{
    lwm2m_object_t * targetP;

    /*
     * Restore content  of objects 0 (security) and 1 (server)
     */
    targetP = (lwm2m_object_t *)LWM2M_LIST_FIND(context->objectList, LWM2M_SECURITY_OBJECT_ID);
    // first delete internal content
    clean_security_object(targetP);
    // then restore previous object
    copy_security_object(targetP, backupObjectArray[0]);

    targetP = (lwm2m_object_t *)LWM2M_LIST_FIND(context->objectList, LWM2M_SERVER_OBJECT_ID);
    // first delete internal content
    clean_server_object(targetP);
    // then restore previous object
    copy_server_object(targetP, backupObjectArray[1]);

    // restart the old servers
    fprintf(stdout, "[BOOTSTRAP] ObjectList restored\r\n");
}

static void update_bootstrap_info(lwm2m_client_state_t * previousBootstrapState,
        lwm2m_context_t * context)
{
    if (*previousBootstrapState != context->state)
    {
        *previousBootstrapState = context->state;
        switch(context->state)
        {
            case STATE_BOOTSTRAPPING:
#ifdef WITH_LOGS
                fprintf(stdout, "[BOOTSTRAP] backup security and server objects\r\n");
#endif
                prv_backup_objects(context);
                break;
            default:
                break;
        }
    }
}

static void close_backup_object()
{
    int i;
    for (i = 0; i < BACKUP_OBJECT_COUNT; i++) {
        switch (backupObjectArray[i]->objID)
        {
        case LWM2M_SECURITY_OBJECT_ID:
            clean_security_object(backupObjectArray[i]);
            lwm2m_free(backupObjectArray[i]);
            break;
        case LWM2M_SERVER_OBJECT_ID:
            clean_server_object(backupObjectArray[i]);
            lwm2m_free(backupObjectArray[i]);
            break;
        default:
            break;
        }
    }
}
#endif

void print_usage(void)
{
    fprintf(stdout, "Usage: lwm2mclient [OPTION]\r\n");
    fprintf(stdout, "Launch a LWM2M client.\r\n");
    fprintf(stdout, "Options:\r\n");
    fprintf(stdout, "  -n NAME\tSet the endpoint name of the Client. Default: testlwm2mclient\r\n");
    fprintf(stdout, "  -l PORT\tSet the local UDP port of the Client. Default: 56830\r\n");
    fprintf(stdout, "  -h HOST\tSet the hostname of the LWM2M Server to connect to. Default: localhost\r\n");
    fprintf(stdout, "  -p PORT\tSet the port of the LWM2M Server to connect to. Default: "LWM2M_STANDARD_PORT_STR"\r\n");
    fprintf(stdout, "  -4\t\tUse IPv4 connection. Default: IPv6 connection\r\n");
    fprintf(stdout, "  -t TIME\tSet the lifetime of the Client. Default: 300\r\n");
    fprintf(stdout, "  -b\t\tBootstrap requested.\r\n");
    fprintf(stdout, "  -c\t\tChange battery level over time.\r\n");
#if(defined WITH_TINYDTLS) || (defined WITH_MBEDDTLS)  
    fprintf(stdout, "  -i STRING\tSet the device management or bootstrap server PSK identity. If not set use none secure mode\r\n");
    fprintf(stdout, "  -s HEXSTRING\tSet the device management or bootstrap server Pre-Shared-Key. If not set use none secure mode\r\n");    
#endif
    fprintf(stdout, "\r\n");
}

/*
 * The function start by setting up the command line interface (which may or not be useful depending on your project)
 *
 * This is an array of commands describes as { name, description, long description, callback, userdata }.
 * The firsts tree are easy to understand, the callback is the function that will be called when this command is typed
 * and in the last one will be stored the lwm2m context (allowing access to the server settings and the objects).
 */
command_desc_t commands[] =
{
        {"list", "List known servers.", NULL, prv_output_servers, NULL},
        {"change", "Change the value of resource.", " change URI [DATA]\r\n"
                                                    "   URI: uri of the resource such as /3/0, /3/0/2\r\n"
                                                    "   DATA: (optional) new value\r\n", prv_change, NULL},
        {"update", "Trigger a registration update", " update SERVER\r\n"
                                                    "   SERVER: short server id such as 123\r\n", prv_update, NULL},
#ifdef AT_MIPL_SUPPORT
        {"read", "Read from a client.", " read URI\r\n"
                                        "   URI: uri to read such as /3, /3/0/2, /1024/11, /1024/0/1\r\n"
                                        "Result will be displayed asynchronously.", prv_read, NULL},
        {"exec", "Execute a client resource.", " exec URI\r\n"
										"	URI: uri of the resource to execute such as /3/0/2\r\n"
										"Result will be displayed asynchronously.", prv_exec, NULL},
        {"del", "Delete a client Object instance.", " del URI\r\n"
                                            "   URI: uri of the instance to delete such as /1024/11\r\n"
                                            "Result will be displayed asynchronously.", pro_delete_instance, NULL},
        {"observe", "Observe from a client.", " observe URI\r\n"
                                        "   URI: uri to observe such as /3, /3/0/2, /1024/11\r\n"
                                        "Result will be displayed asynchronously.", prv_observe, NULL},
#endif
#ifdef LWM2M_BOOTSTRAP
        {"bootstrap", "Initiate a DI bootstrap process", NULL, prv_initiate_bootstrap, NULL},
        {"dispb", "Display current backup of objects/instances/resources\r\n"
                "\t(only security and server objects are backupped)", NULL, prv_display_backup, NULL},
        {"disp", "Display current objects/instances/resources", NULL, prv_display_objects, NULL},
#endif
        {"ls", "List Objects and Instances", NULL, prv_object_list, NULL},
        {"dump", "Dump an Object", "dump URI"
                                   "URI: uri of the Object or Instance such as /3/0, /1\r\n", prv_object_dump, NULL},
        {"add", "Add support of object 1024", NULL, prv_add, NULL},
        {"rm", "Remove support of object 1024", NULL, prv_remove, NULL},
        {"quit", "Quit the client gracefully.", NULL, prv_quit, NULL},
        {"^C", "Quit the client abruptly (without sending a de-register message).", NULL, NULL, NULL},
         COMMAND_END_LIST
};

int lwm2mclient_main(int argc, char *argv[])
{
    g_quit = 0;

    client_data_t data;
    int result;
    int i;
    const char * localPort = "56830";
    const char * server = NULL;
    const char * serverPort = LWM2M_STANDARD_PORT_STR;
    char * name = "testlwm2mclient";
    int lifetime = 300;
    int batterylevelchanging = 0;
    time_t reboot_time = 0;
    int opt;
    bool bootstrapRequested = false;
    bool serverPortChanged = false;

//#ifdef LWM2M_BOOTSTRAP
    lwm2m_client_state_t previousState = STATE_INITIAL;
//#endif

    char * pskId = NULL;
    char * psk = NULL;
    uint16_t pskLen = -1;
    char * pskBuffer = NULL;

    memset(&data, 0, sizeof(client_data_t));
    data.addressFamily = AF_INET6;

    opt = 1;
    while (opt < argc)
    {
        if (argv[opt] == NULL
            || argv[opt][0] != '-'
            || argv[opt][2] != 0)
        {
            print_usage();
            return 0;
        }
        switch (argv[opt][1])
        {
        case 'b':
            bootstrapRequested = true;
            if (!serverPortChanged) serverPort = LWM2M_BSSERVER_PORT_STR;
            break;
        case 'c':
            batterylevelchanging = 1;
            break;
        case 't':
            opt++;
            if (opt >= argc)
            {
                print_usage();
                return 0;
            }
            if (1 != sscanf(argv[opt], "%d", &lifetime))
            {
                print_usage();
                return 0;
            }
            break;
#if(defined WITH_TINYDTLS) || (defined WITH_MBEDDTLS)
        case 'i':
            opt++;
            if (opt >= argc)
            {
                print_usage();
                return 0;
            }
            pskId = argv[opt];
            break;
        case 's':
            opt++;
            if (opt >= argc)
            {
                print_usage();
                return 0;
            }
            psk = argv[opt];
            break;
#endif                        
        case 'n':
            opt++;
            if (opt >= argc)
            {
                print_usage();
                return 0;
            }
            name = argv[opt];
            break;
        case 'l':
            opt++;
            if (opt >= argc)
            {
                print_usage();
                return 0;
            }
            localPort = argv[opt];
            break;
        case 'h':
            opt++;
            if (opt >= argc)
            {
                print_usage();
                return 0;
            }
            server = argv[opt];
            break;
        case 'p':
            opt++;
            if (opt >= argc)
            {
                print_usage();
                return 0;
            }
            serverPort = argv[opt];
            serverPortChanged = true;
            break;
        case '4':
            data.addressFamily = AF_INET;
            break;
        default:
            print_usage();
            return 0;
        }
        opt += 1;
    }

    if (!server)
    {
        server = (AF_INET == data.addressFamily ? DEFAULT_SERVER_IPV4 : DEFAULT_SERVER_IPV6);
    }

    /*
     *This call an internal function that create an IPV6 socket on the port 5683.
     */
    fprintf(stderr, "Trying to bind LWM2M Client to port %s\r\n", localPort);
    data.sock = create_socket(localPort, data.addressFamily);
    if (data.sock < 0)
    {
        fprintf(stderr, "Failed to open socket: %d %s\r\n", errno, strerror(errno));
        return -1;
    }

    /*
     * Now the main function fill an array with each object, this list will be later passed to liblwm2m.
     * Those functions are located in their respective object file.
     */
#if(defined WITH_TINYDTLS) || (defined WITH_MBEDDTLS)
    if (psk != NULL)
    {
        pskLen = strlen(psk) / 2;
        pskBuffer = malloc(pskLen);

        if (NULL == pskBuffer)
        {
            fprintf(stderr, "Failed to create PSK binary buffer\r\n");
            return -1;
        }
        // Hex string to binary
        char *h = psk;
        char *b = pskBuffer;
        char xlate[] = "0123456789ABCDEF";

        for ( ; *h; h += 2, ++b)
        {
            char *l = strchr(xlate, toupper(*h));
            char *r = strchr(xlate, toupper(*(h+1)));

            if (!r || !l)
            {
                fprintf(stderr, "Failed to parse Pre-Shared-Key HEXSTRING\r\n");
                return -1;
            }

            *b = ((l - xlate) << 4) + (r - xlate);
        }
    }
#endif

    char serverUri[50];
    int serverId = 123;
    sprintf (serverUri, "coap://%s:%s", server, serverPort);
#ifdef LWM2M_BOOTSTRAP
    objArray[0] = get_security_object(serverId, serverUri, pskId, pskBuffer, pskLen, bootstrapRequested);
#else
    objArray[0] = get_security_object(serverId, serverUri, pskId, pskBuffer, pskLen, false);
#endif
    if (NULL == objArray[0])
    {
        fprintf(stderr, "Failed to create security object\r\n");
        return -1;
    }
    data.securityObjP = objArray[0];

    objArray[1] = get_server_object(serverId, "U", lifetime, false);
    if (NULL == objArray[1])
    {
        fprintf(stderr, "Failed to create server object\r\n");
        return -1;
    }

    objArray[2] = get_object_device();
    if (NULL == objArray[2])
    {
        fprintf(stderr, "Failed to create Device object\r\n");
        return -1;
    }

    objArray[3] = get_object_firmware();
    if (NULL == objArray[3])
    {
        fprintf(stderr, "Failed to create Firmware object\r\n");
        return -1;
    }

    objArray[4] = get_object_location();
    if (NULL == objArray[4])
    {
        fprintf(stderr, "Failed to create location object\r\n");
        return -1;
    }

    objArray[5] = get_test_object();
    if (NULL == objArray[5])
    {
        fprintf(stderr, "Failed to create test object\r\n");
        return -1;
    }

    objArray[6] = get_object_conn_m();
    if (NULL == objArray[6])
    {
        fprintf(stderr, "Failed to create connectivity monitoring object\r\n");
        return -1;
    }

    objArray[7] = get_object_conn_s();
    if (NULL == objArray[7])
    {
        fprintf(stderr, "Failed to create connectivity statistics object\r\n");
        return -1;
    }

    int instId = 0;
    objArray[8] = acc_ctrl_create_object();
    if (NULL == objArray[8])
    {
        fprintf(stderr, "Failed to create Access Control object\r\n");
        return -1;
    }
    else if (acc_ctrl_obj_add_inst(objArray[8], instId, 3, 0, serverId)==false)
    {
        fprintf(stderr, "Failed to create Access Control object instance\r\n");
        return -1;
    }
    else if (acc_ctrl_oi_add_ac_val(objArray[8], instId, 0, 0b000000000001111)==false)
    {
        fprintf(stderr, "Failed to create Access Control ACL default resource\r\n");
        return -1;
    }
    else if (acc_ctrl_oi_add_ac_val(objArray[8], instId, 999, 0b000000000000001)==false)
    {
        fprintf(stderr, "Failed to create Access Control ACL resource for serverId: 999\r\n");
        return -1;
    }
    /*
     * The liblwm2m library is now initialized with the functions that will be in
     * charge of communication
     */
    lwm2mH = lwm2m_init(&data);
    if (NULL == lwm2mH)
    {
        fprintf(stderr, "lwm2m_init() failed\r\n");
        return -1;
    }

#if(defined WITH_TINYDTLS) || (defined WITH_MBEDDTLS)
    data.lwm2mH = lwm2mH;
#endif

    /*
     * We configure the liblwm2m library with the name of the client - which shall be unique for each client -
     * the number of objects we will be passing through and the objects array
     */
    result = lwm2m_configure(lwm2mH, name, NULL, NULL, OBJ_COUNT, objArray);
    if (result != 0)
    {
        fprintf(stderr, "lwm2m_configure() failed: 0x%X\r\n", result);
        return -1;
    }

    signal(SIGINT, handle_sigint);

    /**
     * Initialize value changed callback.
     */
    init_value_change(lwm2mH);

    /*
     * As you now have your lwm2m context complete you can pass it as an argument to all the command line functions
     * precedently viewed (first point)
     */
    for (i = 0 ; commands[i].name != NULL ; i++)
    {
        commands[i].userData = (void *)lwm2mH;
    }
    fprintf(stdout, "LWM2M Client \"%s\" started on port %s\r\n", name, localPort);
    fprintf(stdout, "> "); fflush(stdout);
    /*
     * We now enter in a while loop that will handle the communications from the server
     */
    while (0 == g_quit)
    {
        fd_set readfds;

        if (g_reboot)
        {
            time_t tv_sec;

            tv_sec = lwm2m_gettime();

            if (0 == reboot_time)
            {
                reboot_time = tv_sec + 5;
            }
            if (reboot_time < tv_sec)
            {
                /*
                 * Message should normally be lost with reboot ...
                 */
                fprintf(stderr, "reboot time expired, rebooting ...");
                system_reboot();
            }
            else
            {
                tv.tv_sec = reboot_time - tv_sec;
            }
        }
        else if (batterylevelchanging) 
        {
            update_battery_level(lwm2mH);
            tv.tv_sec = 5;
        }
        else 
        {
            tv.tv_sec = 60;
        }
        tv.tv_usec = 0;

        FD_ZERO(&readfds);
        FD_SET(data.sock, &readfds);
        FD_SET(STDIN_FILENO, &readfds);

        /*
         * This function does two things:
         *  - first it does the work needed by liblwm2m (eg. (re)sending some packets).
         *  - Secondly it adjusts the timeout value (default 60s) depending on the state of the transaction
         *    (eg. retransmission) and the time between the next operation
         */
        result = lwm2m_step(lwm2mH, &(tv.tv_sec));
        fprintf(stdout, " -> State: ");
        switch (lwm2mH->state)
        {
        case STATE_INITIAL:
            fprintf(stdout, "STATE_INITIAL\r\n");
            break;
        case STATE_BOOTSTRAP_REQUIRED:
            fprintf(stdout, "STATE_BOOTSTRAP_REQUIRED\r\n");
            break;
        case STATE_BOOTSTRAPPING:
            fprintf(stdout, "STATE_BOOTSTRAPPING\r\n");
            break;
        case STATE_REGISTER_REQUIRED:
            fprintf(stdout, "STATE_REGISTER_REQUIRED\r\n");
            break;
        case STATE_REGISTERING:
            fprintf(stdout, "STATE_REGISTERING\r\n");
            break;
        case STATE_READY:
            fprintf(stdout, "STATE_READY\r\n");
            break;
        default:
            fprintf(stdout, "Unknown...\r\n");
            break;
        }
        if (result != 0)
        {
            fprintf(stderr, "lwm2m_step() failed: 0x%X\r\n", result);
            if(previousState == STATE_BOOTSTRAPPING)
            {
#ifdef WITH_LOGS
                fprintf(stdout, "[BOOTSTRAP] restore security and server objects\r\n");
#endif
                prv_restore_objects(lwm2mH);
                lwm2mH->state = STATE_INITIAL;
            }
            else
            {
                lwm2m_close(lwm2mH);
                close(data.sock);
                connection_free(data.connList);
                return -1;
            }
        }
#ifdef LWM2M_BOOTSTRAP
        update_bootstrap_info(&previousState, lwm2mH);
#endif

#ifndef WITH_MBEDDTLS

        /*
         * This part will set up an interruption until an event happen on SDTIN or the socket until "tv" timed out (set
         * with the precedent function)
         */
        result = select(FD_SETSIZE, &readfds, NULL, NULL, &tv);

        if (result < 0)
        {
            if (errno != EINTR)
            {
              fprintf(stderr, "Error in select(): %d %s\r\n", errno, strerror(errno));
            }
        }
        else if (result > 0)
        {
#endif
            uint8_t buffer[MAX_PACKET_SIZE];
            int numBytes = 0;
#ifndef WITH_MBEDDTLS
            /*
             * If an event happens on the socket
             */
            if (FD_ISSET(data.sock, &readfds))
            {
                struct sockaddr_storage addr;
                socklen_t addrLen;

                addrLen = sizeof(addr);

                /*
                 * We retrieve the data received
                 */
                numBytes = recvfrom(data.sock, buffer, MAX_PACKET_SIZE, 0, (struct sockaddr *)&addr, &addrLen);

                if (0 > numBytes)
                {
                    fprintf(stderr, "Error in recvfrom(): %d %s\r\n", errno, strerror(errno));
                }
                else if (0 < numBytes)
                {
#if defined(LWIP_IPV4_ON) && defined(LWIP_IPV6_ON)
                    char s[INET6_ADDRSTRLEN];
#else
#ifdef LWIP_IPV4_ON
                    char s[INET_ADDRSTRLEN];
#else
                    char s[INET6_ADDRSTRLEN];
#endif
#endif
                    in_port_t port;

#ifdef WITH_TINYDTLS
                    dtls_connection_t * connP;
#else
                    connection_t * connP;
#endif
#ifdef LWIP_IPV4_ON
                    if (AF_INET == addr.ss_family)
                    {
                        struct sockaddr_in *saddr = (struct sockaddr_in *)&addr;
                        inet_ntop(saddr->sin_family, &saddr->sin_addr, s, INET_ADDRSTRLEN);
                        port = saddr->sin_port;
                    }
#endif
#ifdef LWIP_IPV6_ON
                    if (AF_INET6 == addr.ss_family)
                    {
                        struct sockaddr_in6 *saddr = (struct sockaddr_in6 *)&addr;
                        inet_ntop(saddr->sin6_family, &saddr->sin6_addr, s, INET6_ADDRSTRLEN);
                        port = saddr->sin6_port;
                    }
#endif
                    fprintf(stderr, "%d bytes received from [%s]:%hu\r\n", numBytes, s, ntohs(port));

                    /*
                     * Display it in the STDERR
                     */
                    output_buffer(stderr, buffer, numBytes, 0);

                    connP = connection_find(data.connList, &addr, addrLen);
                    if (connP != NULL)
                    {
#endif
                        /*
                         * Let liblwm2m respond to the query depending on the context
                         */
#ifdef WITH_TINYDTLS
                        int result = connection_handle_packet(connP, buffer, numBytes);
                            if (0 != result)
                        {
                             printf("error handling message %d\n",result);
                        }
#elif WITH_MBEDDTLS
                         int result = connection_handle_packet(data.connList, buffer, MAX_PACKET_SIZE);

                        if (0 != result)
                        {
                             printf("error handling message %d\n",result);
                        }
#else
                        lwm2m_handle_packet(lwm2mH, buffer, numBytes, connP);
#endif
#if defined WITH_MBEDDTLS
                        mbedtls_ssl_context* sock_ssl = ((dtls_connection_t*)(data.connList))->ssl;
                        numBytes = sock_ssl->in_left;

#endif
                        LOG_ARG("numBytes=%d",numBytes);
                        conn_s_updateRxStatistic(objArray[7], numBytes, false);
#ifndef WITH_MBEDDTLS
                    }
                    else
                    {
                        fprintf(stderr, "received bytes ignored!\r\n");
                    }
                }
            }

            /*
             * If the event happened on the SDTIN
             */
            else if (FD_ISSET(STDIN_FILENO, &readfds))
            {
                numBytes = read(STDIN_FILENO, buffer, MAX_PACKET_SIZE - 1);

                if (numBytes > 1)
                {
                    buffer[numBytes] = 0;
                    /*
                     * We call the corresponding callback of the typed command passing it the buffer for further arguments
                     */
                    handle_command(commands, (char*)buffer);
                }
                if (g_quit == 0)
                {
                    fprintf(stdout, "\r\n> ");
                    fflush(stdout);
                }
                else
                {
                    fprintf(stdout, "\r\n");
                }
            }
        }
#endif
    }

    /*
     * Finally when the loop is left smoothly - asked by user in the command line interface - we unregister our client from it
     */
    if (g_quit == 1)
    {
#ifdef LWM2M_BOOTSTRAP
        close_backup_object();
#endif
        lwm2m_close(lwm2mH);
    }
    close(data.sock);
    connection_free(data.connList);

    clean_security_object(objArray[0]);
    lwm2m_free(objArray[0]);
    clean_server_object(objArray[1]);
    lwm2m_free(objArray[1]);
    free_object_device(objArray[2]);
    free_object_firmware(objArray[3]);
    free_object_location(objArray[4]);
    free_test_object(objArray[5]);
    free_object_conn_m(objArray[6]);
    free_object_conn_s(objArray[7]);
    acl_ctrl_free_object(objArray[8]);

#ifdef MEMORY_TRACE
    if (g_quit == 1)
    {
        trace_print(0, 1);
    }
#endif

    return 0;
}

void excute_lwm2m_command(char * buffer)
{
    handle_command(commands, buffer);
    lwm2m_step(lwm2mH, &(tv.tv_sec));
}

boolean isLWM2MRegistered()
{
    return g_quit == 0 && lwm2mH != NULL && lwm2mH->state == STATE_READY;
}

#ifdef AT_MIPL_SUPPORT
static UINT8 storeRef;
#define MAX_REF_NUM   0x08
mipl_config_t mipl_configs[MAX_REF_NUM];
static uint8_t gVersion = 0;

boolean isRefValid(UINT8 ref)
{
    return ref > 0 && ref <= MAX_REF_NUM;
}

boolean isRefRegistered(UINT8 ref)
{
    return isRefValid(ref) && NULL != lwm2mH && ref == lwm2mH->register_ref;
}

mipl_ret_t mipl_GetFreeREF(UINT8 *pREF)
{
    UINT8  n = 1;
    UINT32 temp = 0x00;

    for (n = 1; n < 9; n++)
    {
        temp = storeRef & (1 << (n - 1));
        if (0 == temp)
        {
            *pREF = n;
            sys_arch_printf("mipl_GetFreeREF() - pREF is 0x%d", *pREF);

            if (*pREF > MAX_REF_NUM)
            {
                return MIPL_RET_ERROR;
            }
            else
            {
                return MIPL_RET_OK;
            }
        }
    }

    return MIPL_RET_ERROR;
}

mipl_ret_t mipl_DeleteREF(UINT8 nREF)
{
    UINT8 n = 0;
    sys_arch_printf("mipl_DeleteREF() - pREF is 0x%d", nREF);

    if((0 != nREF) && (MAX_REF_NUM >= nREF))
    {
        n = nREF;
        if (0 == (storeRef & (1 << (n - 1))))
        {
            return MIPL_RET_ERROR;
        }
        else
        {
            storeRef = storeRef & (~(1 << (n - 1)));
            return MIPL_RET_OK;
        }
    }

    return MIPL_RET_ERROR;
}

mipl_ret_t mipl_SetREF(UINT8 nREF)
{
    sys_arch_printf("mipl_SetREF() - pREF is 0x%d", nREF);

    UINT8 n = 0;

    if (isRefValid(nREF))
    {
        n = nREF;
        if (0 != (storeRef & (1 << (n - 1))))
        {
            return MIPL_RET_ERROR;
        }
        else
        {
            storeRef = storeRef | (1 << (n - 1));
            return MIPL_RET_OK;
        }
    }
    return MIPL_RET_ERROR;
}

mipl_ret_t isRefExist(UINT8 ref)
{
    if (isRefValid(ref))
    {
        if (storeRef & (1 << (ref - 1)))
        {
            return MIPL_RET_OK;
        }
    }
    return MIPL_RET_ERROR;
}

void mipl_free_objects(mipl_object_t *objects)
{
    if (objects == NULL)
    {
        return NULL;
    }
    while (NULL != objects)
    {
        mipl_object_t * object;
        object = objects;

        objects = object->next;
        lwm2m_free(object);
    }
}

mipl_ret_t mipl_freeConfig(mipl_config_t *config)
{
    if (config == NULL)
    {
        return MIPL_RET_ERROR;
    }

    if (config->objects) mipl_free_objects(config->objects);
    if (config->host) lwm2m_free(config->host);
    if (config->net_exdata) lwm2m_free(config->net_exdata);
    if (config->sys_exdata) lwm2m_free(config->sys_exdata);
    if (config->host) lwm2m_free(config);

    return MIPL_RET_OK;
}

mipl_ret_t mipl_freeConfigByRef(UINT8 ref)
{
    return mipl_freeConfig(&mipl_configs[ref]);
}

mipl_object_t * mipl_object_add(mipl_object_t * head, mipl_object_t * node)
{
    mipl_object_t * target;

    if (NULL == head) return node;

    if (head->object_id > node->object_id)
    {
        node->next = head;
        return node;
    }

    target = head;
    while (NULL != target->next && target->next->object_id < node->object_id)
    {
        target = target->next;
    }

    node->next = target->next;
    target->next = node;

    return head;
}

mipl_ret_t read_ConfigFile(void* file_config, uint16_t size, int ref)
{
     if (size <= 0)
     {
         sys_arch_printf("lwm2mclient.c read_ConfigFile() - Config Size of Byte is less than or equal to zero");
         return MIPL_RET_ERROR;
     }

     if (file_config == NULL)
     {
         sys_arch_printf("lwm2mclient.c read_ConfigFile() - file_config is NULL");
         return MIPL_RET_ERROR;
     }

     uint8_t *buffer;
     uint8_t *configdata = (uint8_t *) file_config;

     mipl_config_t config;

     // packet header
     config.version = configdata[0] >> MIPL_HEADER_VERSION_POSITION;
     gVersion = config.version;
     config.configcount = configdata[0] && MIPL_HEADER_CONFIG_COUNT_MASK;
     config.configbytesize = configdata[1] << 8 || configdata[2];
 
     sys_arch_printf("lwm2mclient.c read_ConfigFile() - verson: %d, configcount: %d, configbytesize: %d",
         config.version, config.configcount, config.configbytesize);

     buffer = configdata + MIPL_HEADER_LENGTH;

     uint8_t cfgid;
     uint8_t extflag;
     uint8_t config_size;
     uint8_t config_size_ext;
     while(buffer < configdata + size)
     {
         cfgid = buffer[0] >> MIPL_CONFIG_ID_POSITION;
         extflag = buffer[0] & MIPL_CONFIG_SIZE_EXTEND_FLAG_MASK;
 
         config_size = buffer[1];
         config_size_ext = buffer[2];
 
         buffer += MIPL_CONFIG_ITEM_LENGTH;
         sys_arch_printf("lwm2mclient.c read_ConfigFile() - config_size_ext = %d", config_size_ext);
 
 
         if (cfgid == MIPL_CFGID_INIT)
         {
             config.lifetime = buffer[0] << 24 || buffer[1] << 16 || buffer[2] << 8 || buffer[3];
             sys_arch_printf("lwm2mclient.c read_ConfigFile() - config.lifetime = %d", config.lifetime);
             if (config.lifetime < MIPL_LIFETIME_MIN_VALUE)
             {
                 sys_arch_printf("lwm2mclient.c read_ConfigFile() - the min value of lifetime is %d", MIPL_LIFETIME_MIN_VALUE);
             }
             buffer += MIPL_CONFIG_INIT_INFO_LENGTH;
         }
         else if (cfgid == MIPL_CFGID_NET)
         {
             config.mtu = buffer[0] << 8 || buffer[1];
             buffer += 2;
 
             config.hostlen = buffer[0] << 8 || buffer[1];
             buffer += 2;
             if (config.hostlen) {
                 config.host = (uint8_t *)lwm2m_malloc(config.hostlen);
                 if (config.host == NULL)
                 {
                     sys_arch_printf("lwm2mclient.c read_ConfigFile() - get host fail");
                 }
                 memcpy(config.host, buffer, config.hostlen);
                 buffer += config.hostlen;
                 sys_arch_printf("lwm2mclient.c read_ConfigFile() - config.host: %s", config.host);
             }
 
             config.net_exdatalen= buffer[0] << 8 || buffer[1];
             buffer += 2;
             if (config.net_exdatalen) {
                 config.net_exdata = (uint8_t *)lwm2m_malloc(config.net_exdatalen);
                 if (config.net_exdata== NULL)
                 {
                     sys_arch_printf("lwm2mclient.c read_ConfigFile() - get net_exdata fail");
                 }
                 memcpy(config.net_exdata, buffer, config.net_exdatalen);
                 buffer += config.net_exdatalen;
             }
             sys_arch_printf("lwm2mclient.c read_ConfigFile() - config.net_exdata: %s", config.net_exdata);
         }
         else if (cfgid == MIPL_CFGID_SYS)
         {
             config.debugmode = buffer[0] >> MIPL_DEBUG_MODE_POSITION;
             config.extendinfomode = buffer[0] >> MIPL_EXTEND_INFO_MODE_POSITION && MIPL_EXTEND_INFO_MODE_MASK;
             config.outputmode = buffer[0] >> MIPL_OUTPUT_MODE_POSITION && MIPL_OUTPUT_MODE_MASK;
             config.debug_level = buffer[0] && MIPL_DEBUG_LEVEL_MASK;
             buffer++;
 
             config.buffersize = buffer[0] << 8 || buffer[1];
             buffer++;
 
             config.sys_exdata_len= buffer[0] << 8 || buffer[1];
             buffer += 2;
             if (config.sys_exdata_len)
             {
                 config.sys_exdata = (uint8_t *)lwm2m_malloc(config.sys_exdata_len);
                 if (config.sys_exdata== NULL)
                 {
                     sys_arch_printf("lwm2mclient.c read_ConfigFile() - get sys_exdata fail");
                 }
                 memcpy(config.sys_exdata, buffer, config.sys_exdata_len);
                 buffer += config.sys_exdata_len;
             }
             sys_arch_printf("lwm2mclient.c read_ConfigFile() - config.sys_exdata: %s", config.sys_exdata);
         }
         else if (cfgid == MIPL_CFGID_OBJECTS)
         {
             config.objects_count = buffer[0] << 8 || buffer[1];
             buffer+= 2;
             sys_arch_printf("lwm2mclient.c read_ConfigFile() - config.objects_count: %s", config.objects_count);
 
             config.objects = NULL;
 
             int i = 0;
             while (i < config.objects_count)
             {
                 mipl_object_t *object;
                 object = (mipl_object_t *)lwm2m_malloc(sizeof(mipl_object_t));
                 if (object == NULL)
                 {
                     sys_arch_printf("lwm2mclient.c read_ConfigFile() - init object error");
                     mipl_free_objects(config.objects);
                 }
 
                 object->next = NULL;
                 object->object_id = buffer[0] << 24 || buffer[1] << 16 || buffer[2] << 8 || buffer[3];
                 buffer += 4;
 
                 object->object_ibml = buffer[0] >> 4;
                 sys_arch_printf("lwm2mclient.c read_ConfigFile() - object->object_ibml: %s", object->object_ibml);
                 buffer++;
 
                 // TODO instance bit map  how to parse
                 object->instance_count = buffer[0];
                 buffer++;
 
                 object->attribute_resource_size = buffer[0] << 8 || buffer[1];
                 object->action_resource_size = buffer[2] << 8 || buffer[3];
                 buffer+= 4;
 
                 mipl_object_add(config.objects, object);
 
                 i++;
             }
         }
         else
         {
             break;
         }
     }

     mipl_configs[ref] = config;
     return MIPL_RET_OK;
}

mipl_ret_t mipl_ConfigKit(int index, void* file_config, uint16_t size)
{
    UINT8 result;
    int ref;
    mipl_GetFreeREF(&ref);
    if (ref == 0)
    {
        sys_arch_printf("mipl_ConfigKit() - create instance suite fail");
        return 0;
    }

    result = mipl_SetREF(ref);
    if (!result)
    {
        sys_arch_printf("mipl_ConfigKit() - set ref(%d) fial", ref);
        return 0;
    }

    read_ConfigFile(file_config, size, ref);
    sys_arch_printf("mipl_ConfigKit() - index = %d", ref);
    return ref;
}

void  mipl_updateREF (int ref) {

}

mipl_ret_t  mipl_register (int ref)
{
    g_quit = 0;
    mipl_config_t mipl_config = mipl_configs[ref];

    client_data_t data;
    int result;
    int i;
    const char * localPort = "56830";
    const char * server = "123.57.221.42";
#if(defined WITH_TINYDTLS) || (defined WITH_MBEDDTLS)
    const char * serverPort = LWM2M_DTLS_PORT_STR;
    char * name = "mipl_secure_client";
#else
    const char * serverPort = LWM2M_STANDARD_PORT_STR;
    char * name = "mipl_client";
#endif
    int lifetime = 300;
    int batterylevelchanging = 0;
    time_t reboot_time = 0;
    int opt;
    bool bootstrapRequested = false;
    bool serverPortChanged = false;

//#ifdef LWM2M_BOOTSTRAP
    lwm2m_client_state_t previousState = STATE_INITIAL;
//#endif

#if(defined WITH_TINYDTLS) || (defined WITH_MBEDDTLS)
    char * pskId = "mipl";
    char * psk = "0123456789abcdef";
#else
    char * pskId = NULL;
    char * psk = NULL;
#endif
    uint16_t pskLen = -1;
    char * pskBuffer = NULL;

    memset(&data, 0, sizeof(client_data_t));
    data.addressFamily = AF_INET6;
    
    if (!server)
    {
        server = (AF_INET == data.addressFamily ? DEFAULT_SERVER_IPV4 : DEFAULT_SERVER_IPV6);
    }

    /*
     *This call an internal function that create an IPV6 socket on the port 5683.
     */
    fprintf(stderr, "Trying to bind LWM2M Client to port %s\r\n", localPort);
    data.sock = create_socket(localPort, data.addressFamily);
    if (data.sock < 0)
    {
        fprintf(stderr, "Failed to open socket: %d %s\r\n", errno, strerror(errno));
        return MIPL_RET_ERROR;
    }

    /*
     * Now the main function fill an array with each object, this list will be later passed to liblwm2m.
     * Those functions are located in their respective object file.
     */
#if(defined WITH_TINYDTLS) || (defined WITH_MBEDDTLS)
    if (psk != NULL)
    {
        pskLen = strlen(psk) / 2;
        pskBuffer = malloc(pskLen);

        if (NULL == pskBuffer)
        {
            fprintf(stderr, "Failed to create PSK binary buffer\r\n");
            return MIPL_RET_ERROR;
        }
        // Hex string to binary
        char *h = psk;
        char *b = pskBuffer;
        char xlate[] = "0123456789ABCDEF";

        for ( ; *h; h += 2, ++b)
        {
            char *l = strchr(xlate, toupper(*h));
            char *r = strchr(xlate, toupper(*(h+1)));

            if (!r || !l)
            {
                fprintf(stderr, "Failed to parse Pre-Shared-Key HEXSTRING\r\n");
                return MIPL_RET_ERROR;
            }

            *b = ((l - xlate) << 4) + (r - xlate);
        }
    }
#endif

    char serverUri[50];
    int serverId = 123;
    sprintf (serverUri, "coap://%s:%s", server, serverPort);
#ifdef LWM2M_BOOTSTRAP
    objArray[0] = get_security_object(serverId, serverUri, pskId, pskBuffer, pskLen, bootstrapRequested);
#else
    objArray[0] = get_security_object(serverId, serverUri, pskId, pskBuffer, pskLen, false);
#endif
    if (NULL == objArray[0])
    {
        fprintf(stderr, "Failed to create security object\r\n");
        return MIPL_RET_ERROR;
    }
    data.securityObjP = objArray[0];

    objArray[1] = get_server_object(serverId, "U", lifetime, false);
    if (NULL == objArray[1])
    {
        fprintf(stderr, "Failed to create server object\r\n");
        return MIPL_RET_ERROR;
    }

    objArray[2] = get_object_device();
    if (NULL == objArray[2])
    {
        fprintf(stderr, "Failed to create Device object\r\n");
        return MIPL_RET_ERROR;
    }

    objArray[3] = get_object_firmware();
    if (NULL == objArray[3])
    {
        fprintf(stderr, "Failed to create Firmware object\r\n");
        return MIPL_RET_ERROR;
    }

    objArray[4] = get_object_location();
    if (NULL == objArray[4])
    {
        fprintf(stderr, "Failed to create location object\r\n");
        return MIPL_RET_ERROR;
    }

    objArray[5] = get_test_object();
    if (NULL == objArray[5])
    {
        fprintf(stderr, "Failed to create test object\r\n");
        return MIPL_RET_ERROR;
    }

    objArray[6] = get_object_conn_m();
    if (NULL == objArray[6])
    {
        fprintf(stderr, "Failed to create connectivity monitoring object\r\n");
        return MIPL_RET_ERROR;
    }

    objArray[7] = get_object_conn_s();
    if (NULL == objArray[7])
    {
        fprintf(stderr, "Failed to create connectivity statistics object\r\n");
        return MIPL_RET_ERROR;
    }

    int instId = 0;
    objArray[8] = acc_ctrl_create_object();
    if (NULL == objArray[8])
    {
        fprintf(stderr, "Failed to create Access Control object\r\n");
        return MIPL_RET_ERROR;
    }
    else if (acc_ctrl_obj_add_inst(objArray[8], instId, 3, 0, serverId)==false)
    {
        fprintf(stderr, "Failed to create Access Control object instance\r\n");
        return MIPL_RET_ERROR;
    }
    else if (acc_ctrl_oi_add_ac_val(objArray[8], instId, 0, 0b000000000001111)==false)
    {
        fprintf(stderr, "Failed to create Access Control ACL default resource\r\n");
        return MIPL_RET_ERROR;
    }
    else if (acc_ctrl_oi_add_ac_val(objArray[8], instId, 999, 0b000000000000001)==false)
    {
        fprintf(stderr, "Failed to create Access Control ACL resource for serverId: 999\r\n");
        return MIPL_RET_ERROR;
    }
    /*
     * The liblwm2m library is now initialized with the functions that will be in
     * charge of communication
     */
    lwm2mH = lwm2m_init(&data);
    if (NULL == lwm2mH)
    {
        fprintf(stderr, "lwm2m_init() failed\r\n");
        return MIPL_RET_ERROR;
    }
    lwm2mH->register_ref = ref;
#if(defined WITH_TINYDTLS) || (defined WITH_MBEDDTLS)
    data.lwm2mH = lwm2mH;
#endif

    /*
     * We configure the liblwm2m library with the name of the client - which shall be unique for each client -
     * the number of objects we will be passing through and the objects array
     */
    result = lwm2m_configure(lwm2mH, name, NULL, NULL, OBJ_COUNT, objArray);
    if (result != 0)
    {
        fprintf(stderr, "lwm2m_configure() failed: 0x%X\r\n", result);
        return MIPL_RET_ERROR;
    }

    signal(SIGINT, handle_sigint);

    /**
     * Initialize value changed callback.
     */
    init_value_change(lwm2mH);

    /*
     * As you now have your lwm2m context complete you can pass it as an argument to all the command line functions
     * precedently viewed (first point)
     */
    for (i = 0 ; commands[i].name != NULL ; i++)
    {
        commands[i].userData = (void *)lwm2mH;
    }
    fprintf(stdout, "LWM2M Client \"%s\" started on port %s\r\n", name, localPort);
    fprintf(stdout, "> "); fflush(stdout);

#ifdef LWM2M_BOOTSTRAP
    lwm2m_client_state_t previousState = STATE_INITIAL;
#endif

    /*
     * We now enter in a while loop that will handle the communications from the server
     */
    while (0 == g_quit)
    {
        fd_set readfds;

        if (g_reboot)
        {
            time_t tv_sec;

            tv_sec = lwm2m_gettime();

            if (0 == reboot_time)
            {
                reboot_time = tv_sec + 5;
            }
            if (reboot_time < tv_sec)
            {
                /*
                 * Message should normally be lost with reboot ...
                 */
                fprintf(stderr, "reboot time expired, rebooting ...");
                system_reboot();
            }
            else
            {
                tv.tv_sec = reboot_time - tv_sec;
            }
        }
        else if (batterylevelchanging) 
        {
            update_battery_level(lwm2mH);
            tv.tv_sec = 5;
        }
        else 
        {
            tv.tv_sec = 60;
        }
        tv.tv_usec = 0;

        FD_ZERO(&readfds);
        FD_SET(data.sock, &readfds);
        FD_SET(STDIN_FILENO, &readfds);

        /*
         * This function does two things:
         *  - first it does the work needed by liblwm2m (eg. (re)sending some packets).
         *  - Secondly it adjusts the timeout value (default 60s) depending on the state of the transaction
         *    (eg. retransmission) and the time between the next operation
         */
        result = lwm2m_step(lwm2mH, &(tv.tv_sec));
        fprintf(stdout, " -> State: ");
        switch (lwm2mH->state)
        {
        case STATE_INITIAL:
            fprintf(stdout, "STATE_INITIAL\r\n");
            break;
        case STATE_BOOTSTRAP_REQUIRED:
            fprintf(stdout, "STATE_BOOTSTRAP_REQUIRED\r\n");
            break;
        case STATE_BOOTSTRAPPING:
            fprintf(stdout, "STATE_BOOTSTRAPPING\r\n");
            break;
        case STATE_REGISTER_REQUIRED:
            fprintf(stdout, "STATE_REGISTER_REQUIRED\r\n");
            break;
        case STATE_REGISTERING:
            fprintf(stdout, "STATE_REGISTERING\r\n");
            break;
        case STATE_READY:
            fprintf(stdout, "STATE_READY\r\n");
            break;
        default:
            fprintf(stdout, "Unknown...\r\n");
            break;
        }
        if (result != 0)
        {
            fprintf(stderr, "lwm2m_step() failed: 0x%X\r\n", result);
#ifdef LWM2M_BOOTSTRAP
            if(previousState == STATE_BOOTSTRAPPING)
            {
#ifdef WITH_LOGS
                fprintf(stdout, "[BOOTSTRAP] restore security and server objects\r\n");
#endif
                prv_restore_objects(lwm2mH);
                lwm2mH->state = STATE_INITIAL;
            }
            else
#endif
            {
                lwm2m_close(lwm2mH);
                close(data.sock);
                connection_free(data.connList);
                return MIPL_RET_ERROR;
            }
        }
#ifdef LWM2M_BOOTSTRAP
        update_bootstrap_info(&previousState, lwm2mH);
#endif

#ifndef WITH_MBEDDTLS

        /*
         * This part will set up an interruption until an event happen on SDTIN or the socket until "tv" timed out (set
         * with the precedent function)
         */
        result = select(FD_SETSIZE, &readfds, NULL, NULL, &tv);

        if (result < 0)
        {
            if (errno != EINTR)
            {
              fprintf(stderr, "Error in select(): %d %s\r\n", errno, strerror(errno));
            }
        }
        else if (result > 0)
        {
#endif
            uint8_t buffer[MAX_PACKET_SIZE];
            int numBytes = 0;
#ifndef WITH_MBEDDTLS
            /*
             * If an event happens on the socket
             */
            if (FD_ISSET(data.sock, &readfds))
            {
                struct sockaddr_storage addr;
                socklen_t addrLen;

                addrLen = sizeof(addr);

                /*
                 * We retrieve the data received
                 */
                numBytes = recvfrom(data.sock, buffer, MAX_PACKET_SIZE, 0, (struct sockaddr *)&addr, &addrLen);

                if (0 > numBytes)
                {
                    fprintf(stderr, "Error in recvfrom(): %d %s\r\n", errno, strerror(errno));
                }
                else if (0 < numBytes)
                {
#if defined(LWIP_IPV4_ON) && defined(LWIP_IPV6_ON)
                    char s[INET6_ADDRSTRLEN];
#else
#ifdef LWIP_IPV4_ON
                    char s[INET_ADDRSTRLEN];
#else
                    char s[INET6_ADDRSTRLEN];
#endif
#endif
                    in_port_t port;

#ifdef WITH_TINYDTLS
                    dtls_connection_t * connP;
#else
                    connection_t * connP;
#endif
#ifdef LWIP_IPV4_ON
                    if (AF_INET == addr.ss_family)
                    {
                        struct sockaddr_in *saddr = (struct sockaddr_in *)&addr;
                        inet_ntop(saddr->sin_family, &saddr->sin_addr, s, INET_ADDRSTRLEN);
                        port = saddr->sin_port;
                    }
#endif
#ifdef LWIP_IPV6_ON
                    if (AF_INET6 == addr.ss_family)
                    {
                        struct sockaddr_in6 *saddr = (struct sockaddr_in6 *)&addr;
                        inet_ntop(saddr->sin6_family, &saddr->sin6_addr, s, INET6_ADDRSTRLEN);
                        port = saddr->sin6_port;
                    }
#endif
                    fprintf(stderr, "%d bytes received from [%s]:%hu\r\n", numBytes, s, ntohs(port));

                    /*
                     * Display it in the STDERR
                     */
                    output_buffer(stderr, buffer, numBytes, 0);

                    connP = connection_find(data.connList, &addr, addrLen);
                    if (connP != NULL)
                    {
#endif
                        /*
                         * Let liblwm2m respond to the query depending on the context
                         */
#ifdef WITH_TINYDTLS
                        int result = connection_handle_packet(connP, buffer, numBytes);
                            if (0 != result)
                        {
                             printf("error handling message %d\n",result);
                        }
#elif WITH_MBEDDTLS
                         int result = connection_handle_packet(data.connList, buffer, MAX_PACKET_SIZE);

                        if (0 != result)
                        {
                             printf("error handling message %d\n",result);
                        }
#else
                        lwm2m_handle_packet(lwm2mH, buffer, numBytes, connP);
#endif
#if defined WITH_MBEDDTLS
                        mbedtls_ssl_context* sock_ssl = ((dtls_connection_t*)(data.connList))->ssl;
                        numBytes = sock_ssl->in_left;

#endif
                        LOG_ARG("numBytes=%d",numBytes);
                        conn_s_updateRxStatistic(objArray[7], numBytes, false);
#ifndef WITH_MBEDDTLS
                    }
                    else
                    {
                        fprintf(stderr, "received bytes ignored!\r\n");
                    }
                }
            }

            /*
             * If the event happened on the SDTIN
             */
            else if (FD_ISSET(STDIN_FILENO, &readfds))
            {
                numBytes = read(STDIN_FILENO, buffer, MAX_PACKET_SIZE - 1);

                if (numBytes > 1)
                {
                    buffer[numBytes] = 0;
                    /*
                     * We call the corresponding callback of the typed command passing it the buffer for further arguments
                     */
                    handle_command(commands, (char*)buffer);
                }
                if (g_quit == 0)
                {
                    fprintf(stdout, "\r\n> ");
                    fflush(stdout);
                }
                else
                {
                    fprintf(stdout, "\r\n");
                }
            }
        }
#endif
    }

    /*
     * Finally when the loop is left smoothly - asked by user in the command line interface - we unregister our client from it
     */
    if (g_quit == 1)
    {
#ifdef LWM2M_BOOTSTRAP
        close_backup_object();
#endif
        lwm2m_close(lwm2mH);
    }
    close(data.sock);
    connection_free(data.connList);

    clean_security_object(objArray[0]);
    lwm2m_free(objArray[0]);
    clean_server_object(objArray[1]);
    lwm2m_free(objArray[1]);
    free_object_device(objArray[2]);
    free_object_firmware(objArray[3]);
    free_object_location(objArray[4]);
    free_test_object(objArray[5]);
    free_object_conn_m(objArray[6]);
    free_object_conn_s(objArray[7]);
    acl_ctrl_free_object(objArray[8]);

#ifdef MEMORY_TRACE
    if (g_quit == 1)
    {
        trace_print(0, 1);
    }
#endif

    return MIPL_RET_OK;
}

mipl_ret_t mipl_unregister (int ref)
{
    excute_lwm2m_command("quit\n");
    return MIPL_RET_OK;
}

void mipl_update(uint8 ref, uint16_t objectId, uint16_t instanceId, uint16_t resourceId)
{
    lwm2m_uri_t uri;

    memset(&uri, 0, sizeof(lwm2m_uri_t));
    uri.objectId = objectId;
    uri.instanceId = instanceId;
    uri.resourceId = resourceId;

    fprintf(stderr, "report change!\n");
    lwm2m_report_data("report change!\n", strlen("report change!\n"));
    lwm2m_report_at_command_result(1);
    lwm2m_resource_value_changed(lwm2mH, &uri);
}

uint8_t mipl_GetVersion() {
    return gVersion;
}

#endif
