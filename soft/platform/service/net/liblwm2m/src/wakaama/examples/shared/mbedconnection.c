/*******************************************************************************
 *
 * Copyright (c) 2015 Intel Corporation and others.
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
 *    Christian Renz - Please refer to git log
 *
 *******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "mbedconnection.h"
#include "commandline.h"
#include "internals.h"

#define COAP_PORT "5683"
#define COAPS_PORT "5684"
#define URI_LENGTH 256

dtls_context_t * dtlsContext;

#define MAX_PACKET_SIZE 1024
/********************* Security Obj Helpers **********************/
char * security_get_uri(lwm2m_object_t * obj, int instanceId, char * uriBuffer, int bufferSize){
    int size = 1;
    lwm2m_data_t * dataP = lwm2m_data_new(size);
    dataP->id = 0; // security server uri

    obj->readFunc(instanceId, &size, &dataP, obj);
    if (dataP != NULL &&
            dataP->type == LWM2M_TYPE_STRING &&
            dataP->value.asBuffer.length > 0)
    {
        if (bufferSize > dataP->value.asBuffer.length){
            memset(uriBuffer,0,dataP->value.asBuffer.length+1);
            strncpy(uriBuffer,dataP->value.asBuffer.buffer,dataP->value.asBuffer.length);
            lwm2m_data_free(size, dataP);
            return uriBuffer;
        }
    }
    lwm2m_data_free(size, dataP);
    return NULL;
}

int64_t security_get_mode(lwm2m_object_t * obj, int instanceId){
    int64_t mode;
    int size = 1;
    lwm2m_data_t * dataP = lwm2m_data_new(size);
    dataP->id = 2; // security mode

    obj->readFunc(instanceId, &size, &dataP, obj);
    if (0 != lwm2m_data_decode_int(dataP,&mode))
    {
        lwm2m_data_free(size, dataP);
        return mode;
    }

    lwm2m_data_free(size, dataP);
    LOG("Unable to get security mode : use not secure mode");
    return LWM2M_SECURITY_MODE_NONE;
}

char * security_get_public_id(lwm2m_object_t * obj, int instanceId, int * length){
    int size = 1;
    lwm2m_data_t * dataP = lwm2m_data_new(size);
    dataP->id = 3; // public key or id

    obj->readFunc(instanceId, &size, &dataP, obj);
    if (dataP != NULL &&
        dataP->type == LWM2M_TYPE_OPAQUE)
    {
        char * buff;

        buff = (char*)lwm2m_malloc(dataP->value.asBuffer.length);
        if (buff != 0)
        {
            memcpy(buff, dataP->value.asBuffer.buffer, dataP->value.asBuffer.length);
            *length = dataP->value.asBuffer.length;
        }
        lwm2m_data_free(size, dataP);

        return buff;
    } else {
        return NULL;
    }
}


char * security_get_secret_key(lwm2m_object_t * obj, int instanceId, int * length){
    int size = 1;
    lwm2m_data_t * dataP = lwm2m_data_new(size);
    dataP->id = 5; // secret key
    LOG("security_get_secret_key");
    obj->readFunc(instanceId, &size, &dataP, obj);
    if (dataP != NULL &&
        dataP->type == LWM2M_TYPE_OPAQUE)
    {
        char * buff;
        buff = (char*)malloc(dataP->value.asBuffer.length);
        if (buff != 0)
        {
            memcpy(buff, dataP->value.asBuffer.buffer, dataP->value.asBuffer.length);
			*length = dataP->value.asBuffer.length;
        }
        lwm2m_data_free(size, dataP);
        return buff;
    } else {
        return NULL;
    }
}

/********************* Security Obj Helpers Ends **********************/

int send_data(dtls_connection_t *connP,
                    uint8_t * buffer,
                    size_t length)
{
    int nbSent;
    size_t offset;

#ifdef WITH_LOGS
    char s[INET6_ADDRSTRLEN];
    in_port_t port;

    s[0] = 0;

    if (AF_INET == connP->addr.sin6_family)
    {
        struct sockaddr_in *saddr = (struct sockaddr_in *)&connP->addr;
        inet_ntop(saddr->sin_family, &saddr->sin_addr, s, INET6_ADDRSTRLEN);
        port = saddr->sin_port;
    }
    else if (AF_INET6 == connP->addr.sin6_family)
    {
        struct sockaddr_in6 *saddr = (struct sockaddr_in6 *)&connP->addr;
        inet_ntop(saddr->sin6_family, &saddr->sin6_addr, s, INET6_ADDRSTRLEN);
        port = saddr->sin6_port;
    }

    LOG_ARG("Sending %d bytes to [%s]:%hu\r\n", length, s, ntohs(port));

#endif

    offset = 0;
    while (offset != length)
    {
        nbSent = sendto(connP->sock, buffer + offset, length - offset, 0, (struct sockaddr *)&(connP->addr), connP->addrLen);
        if (nbSent == -1) return -1;
        offset += nbSent;
    }
    connP->lastSend = lwm2m_gettime();
    return 0;

}

/*
 * Write at most 'len' characters
 */
int mbeddtls_net_send( void *connP, const unsigned char *buf, size_t len )
{
	LOG("mbeddtls_net_send enter");
	int ret;
	dtls_connection_t* ctx = (dtls_connection_t *) connP;
	int fd = ctx->sock;
	
	if( fd < 0 )
		return( MBEDTLS_ERR_NET_INVALID_CONTEXT );

    int nbSent;
    size_t offset;

    //char s[INET6_ADDRSTRLEN];add LWIP_IPV4_ON/LWIP_IPV6_ON
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

    s[0] = 0;
#ifdef LWIP_IPV4_ON

    if (AF_INET == ctx->addr.sin_family)
    {
        struct sockaddr_in *saddr = (struct sockaddr_in *)&ctx->addr;
        inet_ntop(saddr->sin_family, &saddr->sin_addr, s, INET_ADDRSTRLEN);
        port = saddr->sin_port;
    }
#endif
#ifdef LWIP_IPV6_ON
    if (AF_INET6 == ctx->addr6.sin6_family)
    {
        struct sockaddr_in6 *saddr = (struct sockaddr_in6 *)&ctx->addr;
        inet_ntop(saddr->sin6_family, &saddr->sin6_addr, s, INET6_ADDRSTRLEN);
        port = saddr->sin6_port;
    }
#endif
    LOG_ARG("Sending %d bytes\r\n", len);

    offset = 0;
    while (offset != len)
    {
        nbSent = sendto(fd, buf + offset, len - offset, 0, (struct sockaddr *)&(ctx->addr), ctx->addrLen);
        if (nbSent == -1) return -1;
        offset += nbSent;
    }
	LOG_ARG("return len = %d",len);
    return nbSent;
}

/*
 * Read at most 'len' characters
 */
int mbeddtls_net_recv( void *ctx, unsigned char *buffer, size_t len )
{
	LOG("mbeddtls_net_recv enter");
	int ret;
	int fd = ((dtls_connection_t *) ctx)->sock;
	struct timeval tv;
	tv.tv_sec = 60;
	tv.tv_usec = 0;
	if( fd < 0 )
        return( MBEDTLS_ERR_NET_INVALID_CONTEXT );
	fd_set readfds;

	FD_ZERO(&readfds);
	FD_SET(fd, &readfds);

    /*
     * This part will set up an interruption until an event happen on SDTIN or the socket until "tv" timed out (set
     * with the precedent function)
     */
    int result = select(FD_SETSIZE, &readfds, NULL, NULL, &tv);

    if (result < 0)
    {
       
        LOG( "Error in select(): %d %s\r\n");
        return result;
    }
    else if (result > 0)
	{
        int numBytes;
		LOG( "select read event happend");

        /*
         * If an event happens on the socket
         */
        if (FD_ISSET(fd, &readfds))
        {
            struct sockaddr_storage addr;
            socklen_t addrLen;
            addrLen = sizeof(addr);

            /*
             * We retrieve the data received
             */
            numBytes = recvfrom(fd, buffer, MAX_PACKET_SIZE, 0, (struct sockaddr *)&addr, &addrLen);

            if (0 > numBytes)
            {
                LOG( "Error in recvfrom(): %d %s\r\n");
            }
            else if (0 < numBytes)
            {
				//char s[INET6_ADDRSTRLEN];add LWIP_IPV4_ON/LWIP_IPV6_ON
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
#ifdef LWIP_IPV4_ON
                if (AF_INET == addr.ss_family)
                {
                    struct sockaddr_in *saddr = (struct sockaddr_in *)&addr;
                    inet_ntop(saddr->sin_family, &saddr->sin_addr, s, INET6_ADDRSTRLEN);
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
                LOG_ARG( "%d bytes received from [%s]:%hu\r\n", numBytes, s, ntohs(port));
            }
			return numBytes;
		}
		LOG("no read event happend on this socket");
		return -1;
    }
}

dtls_context_t * get_dtls_context(dtls_connection_t * connList) {

	uint32_t flags; 
	int ret;
	const char *pers ="lwm2mclient"; 
	LOG("Enterring get_dtls_context");
	int i =0;
	//mbedtls_x509_crt_init( &connList->clicert);

    //mbedtls_pk_init( &connList->pkey);

	mbedtls_net_init(&connList->server_fd);

	connList->ssl = malloc(sizeof(mbedtls_ssl_context));
	mbedtls_ssl_init(connList->ssl);

	mbedtls_ssl_config_init(&connList->conf);

	mbedtls_x509_crt_init(&connList->cacert);

	mbedtls_ctr_drbg_init(&connList->ctr_drbg);

	mbedtls_entropy_init(&connList->entropy);

	if(( ret = mbedtls_ctr_drbg_seed( &connList->ctr_drbg, mbedtls_entropy_func, &connList->entropy,
	    (const unsigned char *) pers, strlen( pers ) ) ) != 0 )
    {
	    LOG_ARG("mbedtls_ctr_drbg_seed failed...,ret=%d\n",ret);
	    return ret;
	}

    connList->server_fd.fd = connList->sock;
	if( ( ret = mbedtls_ssl_config_defaults( &connList->conf,
	        MBEDTLS_SSL_IS_CLIENT,
	        MBEDTLS_SSL_TRANSPORT_DATAGRAM,
	        MBEDTLS_SSL_PRESET_DEFAULT ) ) != 0 )
	{
	    LOG_ARG("mbedtls_ssl_config_defaults failed ret = %d\n",ret);
	    return ret;
	}
	ret = mbedtls_net_set_block( &connList->server_fd );
	mbedtls_timing_delay_context timer;
	 mbedtls_ssl_set_timer_cb( connList->ssl, &timer, mbedtls_timing_set_delay,
                                        mbedtls_timing_get_delay );

	int length = 0;
	int id_len = 0;
	char* psk = security_get_secret_key(connList->securityObj,connList->securityInstId,&length);

	char* psk_id = security_get_public_id(connList->securityObj,connList->securityInstId,&id_len);

	if( ( ret = mbedtls_ssl_conf_psk( &connList->conf, psk, length,
                         psk_id,
                         id_len ) ) != 0 )
    {
	    LOG_ARG( " failed! mbedtls_ssl_conf_psk returned %d\n\n", ret );
	    return ret;
    }
	/* OPTIONAL is not optimal for security,
	* but makes interop easier in this simplified example */
	mbedtls_ssl_conf_authmode( &connList->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
	mbedtls_ssl_conf_ca_chain( &connList->conf, &connList->cacert, NULL );
	mbedtls_ssl_conf_rng( &connList->conf, mbedtls_ctr_drbg_random, &connList->ctr_drbg );
	//mbedtls_ssl_conf_dbg( &conf, my_debug, stdout );

	if( ( ret = mbedtls_ssl_setup( connList->ssl, &connList->conf ) ) != 0 )
	{
	    LOG_ARG("mbedtls_ssl_setup failed ret = %d\n",ret);
	    return ret;
	}
	mbedtls_ssl_set_bio( connList->ssl, connList, mbeddtls_net_send, mbeddtls_net_recv, NULL );
	/*
	* 4. Handshake
	*/
	while( ( ret = mbedtls_ssl_handshake( connList->ssl ) ) != 0 )
	{
		if( ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE )
		{
			LOG_ARG( " failed ! mbedtls_ssl_handshake returned %x\n\n", ret );
			return ret;
		}
	} 

	LOG( " ok\n" );

}

int get_port(struct sockaddr *x)
{
#ifdef LWIP_IPV4_ON
   if (x->sa_family == AF_INET)
   {
       return ((struct sockaddr_in *)x)->sin_port;
   }
#endif
#ifdef LWIP_IPV6_ON
   if (x->sa_family == AF_INET6) {
       return ((struct sockaddr_in6 *)x)->sin6_port;
   }
#endif
   printf("non IPV4 or IPV6 address\n");
   return  -1;

}

int create_socket(const char * portStr, int ai_family)
{
    int s = -1;
    struct addrinfo hints;
    struct addrinfo *res;
    struct addrinfo *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = ai_family;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    if (0 != getaddrinfo(NULL, portStr, &hints, &res))
    {
        return -1;
    }

    for(p = res ; p != NULL && s == -1 ; p = p->ai_next)
    {
        s = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (s >= 0)
        {
            if (-1 == bind(s, p->ai_addr, p->ai_addrlen))
            {
                close(s);
                s = -1;
            }
        }
    }

    freeaddrinfo(res);

    return s;
}

dtls_connection_t * connection_new_incoming(dtls_connection_t * connList,
                                       int sock,
                                       const struct sockaddr * addr,
                                       size_t addrLen)
{
    dtls_connection_t * connP;

    connP = (dtls_connection_t *)malloc(sizeof(dtls_connection_t));
    if (connP != NULL)
    {
        connP->sock = sock;
        memcpy(&(connP->addr), addr, addrLen);
        connP->addrLen = addrLen;
        connP->next = connList;
        LOG("new_incoming");
        connP->dtlsSession = (session_t *)malloc(sizeof(session_t));
#ifdef LWIP_IPV6_ON
				connP->dtlsSession->addr.sin6 = connP->addr6;
#else
				connP->dtlsSession->addr.sin = connP->addr;
#endif

        connP->dtlsSession->size = connP->addrLen;
        connP->lastSend = lwm2m_gettime();
    }

    return connP;
}

dtls_connection_t * connection_create(dtls_connection_t * connList,
                                 int sock,
                                 lwm2m_object_t * securityObj,
                                 int instanceId,
                                 lwm2m_context_t * lwm2mH,
                                 int addressFamily)
{
    struct addrinfo hints;
    struct addrinfo *servinfo = NULL;
    struct addrinfo *p;
    int s;
    struct sockaddr *sa;
    socklen_t sl;
    dtls_connection_t * connP = NULL;
    char uriBuf[URI_LENGTH];
    char * uri;
    char * host;
    char * port;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = addressFamily;
    hints.ai_socktype = SOCK_DGRAM;

    uri = security_get_uri(securityObj, instanceId, uriBuf, URI_LENGTH);
    if (uri == NULL) return NULL;

    // parse uri in the form "coaps://[host]:[port]"
    char * defaultport;
    if (0 == strncmp(uri, "coaps://", strlen("coaps://")))
    {
        host = uri+strlen("coaps://");
        defaultport = COAPS_PORT;
    }
    else if (0 == strncmp(uri, "coap://", strlen("coap://")))
    {
        host = uri+strlen("coap://");
        defaultport = COAP_PORT;
    }
    else
    {
        return NULL;
    }
    port = strrchr(host, ':');
    if (port == NULL)
    {
        port = defaultport;
    }
    else
    {
        // remove brackets
        if (host[0] == '[')
        {
            host++;
            if (*(port - 1) == ']')
            {
                *(port - 1) = 0;
            }
            return NULL;
        }
        // split strings
        *port = 0;
        port++;
    }

    if (0 != getaddrinfo(host, port, &hints, &servinfo) || servinfo == NULL) return NULL;

    // we test the various addresses
    s = -1;
    for(p = servinfo ; p != NULL && s == -1 ; p = p->ai_next)
    {
        s = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (s >= 0)
        {
            sa = p->ai_addr;
            sl = p->ai_addrlen;
            if (-1 == connect(s, p->ai_addr, p->ai_addrlen))
            {
                LOG("failed");
                close(s);
                s = -1;
            }
        }
    }
    if (s >= 0)
    {
        connP = connection_new_incoming(connList, sock, sa, sl);		
        close(s);
        connP->sock_id = s;
        // do we need to start tinydtls?
        if (connP != NULL)
        {   LOG(" connP != NULL");
            connP->securityObj = securityObj;
            connP->securityInstId = instanceId;
            connP->lwm2mH = lwm2mH;

            if (security_get_mode(connP->securityObj,connP->securityInstId)
                     != LWM2M_SECURITY_MODE_NONE)
            {
                LOG(" security_get_mode dtls");
                connP->dtlsContext = get_dtls_context(connP);
            }
            else
            {
                LOG(" security_get_mode none");
                // no dtls session
                connP->dtlsSession = NULL;
            }
        }
    }

    if (NULL != servinfo) free(servinfo);
    return connP;
}

void connection_free(dtls_connection_t * connList)
{
    while (connList != NULL)
    {
        dtls_connection_t * nextP;

        nextP = connList->next;
        free(connList);

        connList = nextP;
    }

    mbedtls_net_free( &connList->server_fd );
    mbedtls_x509_crt_free( &connList->cacert );
    mbedtls_ssl_free( connList->ssl );
	free(connList->ssl);
    mbedtls_ssl_config_free( &connList->conf );
    mbedtls_ctr_drbg_free(&connList->ctr_drbg );
    mbedtls_entropy_free( &connList->entropy );
}

int connection_send(dtls_connection_t *connP, uint8_t * buffer, size_t length){
    if (connP->dtlsSession == NULL) {
        // no security
        LOG("no security");
        if ( 0 != send_data(connP, buffer, length)) {
            return -1 ;
        }
    } else {
		if (-1 == mbedtls_ssl_write(connP->ssl,buffer,length)) {
            return -1;
        }
    }
    LOG("connection_send success");
    return 0;
}

int connection_handle_packet(dtls_connection_t *connP, uint8_t * buffer, size_t numBytes){

    if (connP->dtlsSession != NULL)
    {
        LOG("security mode");

		int result =mbedtls_ssl_read(connP->ssl,buffer,numBytes);
		if (result <= 0) {
             LOG_ARG("error dtls handling message %d\n",result);
			 return result;
        }
		LOG_ARG("after mbedtls_ssl_read,relsult=%d",result);
		lwm2m_handle_packet(connP->lwm2mH, buffer, result, (void*)connP);
        
    } else {
        // no security, just give the plaintext buffer to liblwm2m
        lwm2m_handle_packet(connP->lwm2mH, buffer, numBytes, (void*)connP);
        return 0;
    }
}

uint8_t lwm2m_buffer_send(void * sessionH,
                          uint8_t * buffer,
                          size_t length,
                          void * userdata)
{
    dtls_connection_t * connP = (dtls_connection_t*) sessionH;

    if (connP == NULL)
    {
        LOG_ARG( "#> failed sending %lu bytes, missing connection\r\n", length);
        return COAP_500_INTERNAL_SERVER_ERROR ;
    }

    if (-1 == connection_send(connP, buffer, length))
    {
        LOG_ARG("#> failed sending %lu bytes\r\n", length);
        return COAP_500_INTERNAL_SERVER_ERROR ;
    }
    LOG("lwm2m_buffer_send success");
    return COAP_NO_ERROR;
}

bool lwm2m_session_is_equal(void * session1,
                            void * session2,
                            void * userData)
{
    return (session1 == session2);
}

