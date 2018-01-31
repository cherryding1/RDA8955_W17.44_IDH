/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: chttp_server.cpp
*
*	Revision:
*
*	02/07/05
*		- first revision
*	10/30/05
*		- Thanks for Makela Aapo <aapo.makela@nokia.com>
*		- Fixed a memory leak problem in the http client thread.
*	16-Jan-06 Aapo Makela
*		- Changed to do a better keepalive check
*
******************************************************************/

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "chttp.h"
#include "cthread.h"
#include "clog.h"
#include "cstring.h"
#include "event.h"


#if defined(HAVE_UNAME) || defined(TARGET_OS_MAC) || defined(TARGET_OS_IPHONE)
#include <sys/utsname.h>
#endif

/****************************************
* cg_http_server_new
****************************************/

CgHttpServer *cg_http_server_new()
{
	CgHttpServer *httpServer;

	cg_log_debug_l4("Entering...\n");

	httpServer = (CgHttpServer *)malloc(sizeof(CgHttpServer));

	if ( NULL != httpServer )
	{
		cg_list_node_init((CgList *)httpServer);

		httpServer->sock = NULL;
		httpServer->acceptThread = NULL;
		httpServer->listener = NULL;

		/**** Thanks for Makela Aapo (10/31/05) ****/
		httpServer->clientThreads = NULL;

		cg_http_server_setuserdata(httpServer, NULL);

		cg_http_server_settimeout(httpServer, CG_HTTP_SERVER_READ_TIMEOUT);

		/* Mutex */
		httpServer->mutex = cg_mutex_new();
	}

	cg_log_debug_l4("Leaving...\n");

	return httpServer;
}

/****************************************
* cg_http_server_delete
****************************************/

void cg_http_server_delete(CgHttpServer *httpServer)
{
	cg_log_debug_l4("Entering...\n");

	cg_http_server_stop(httpServer);
	cg_http_server_close(httpServer);

	if (httpServer->mutex)
		cg_mutex_delete(httpServer->mutex);

	cg_list_remove((CgList *)httpServer);

	free(httpServer);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_server_delete
****************************************/

BOOL cg_http_server_open(CgHttpServer *httpServer, int bindPort, char *bindAddr)
{
	cg_log_debug_l4("Entering...\n");

	if (cg_http_server_isopened(httpServer) == TRUE)
		return FALSE;

	httpServer->sock = cg_socket_stream_new();
	if (cg_socket_bind(httpServer->sock, bindPort, bindAddr, TRUE, FALSE) == FALSE) {
		cg_socket_delete(httpServer->sock);
		httpServer->sock = NULL;
		return FALSE;
	}
	if (cg_socket_listen(httpServer->sock) == FALSE) {
		cg_socket_delete(httpServer->sock);
		httpServer->sock = NULL;
		return FALSE;
	}

	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* cg_http_server_delete
****************************************/

BOOL cg_http_server_close(CgHttpServer *httpServer)
{
	cg_log_debug_l4("Entering...\n");

	cg_http_server_stop(httpServer);

	if (httpServer->sock != NULL) {
		//cg_socket_close(httpServer->sock);
		cg_socket_delete(httpServer->sock);
		httpServer->sock = NULL;
	}

	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* cg_http_server_thread
****************************************/

typedef struct _CgHttpServerClientData {
	CgSocket *clientSock;
	CgHttpServer *httpServer;
	BOOL runOneTime;
} CgHttpServerClientData;

static CgHttpServerClientData *cg_http_server_clientdata_new(CgHttpServer *httpServer, CgSocket *clientSock)
{
	CgHttpServerClientData *clientData;

	cg_log_debug_l4("Entering...\n");

	clientData = (CgHttpServerClientData *)malloc(sizeof(CgHttpServerClientData));

	if ( NULL != clientData )
	{
		clientData->httpServer = httpServer;
		clientData->clientSock = clientSock;
	}

	cg_log_debug_l4("Leaving...\n");

	return clientData;
}

static void cg_http_server_clientdata_delete(CgHttpServerClientData *clientData)
{
	cg_log_debug_l4("Entering...\n");

	free(clientData);

	cg_log_debug_l4("Leaving...\n");
}

static void cg_http_server_clientthread(CgThread *thread)
{
	CgHttpServerClientData *clientData;
	CgHttpServer *httpServer;
	CgSocket *clientSock;
	void *httpServerUserData;
	CgHttpRequest *httpReq;
	char *version = NULL;

	cg_log_debug_l4("Entering...\n");

	clientData = (CgHttpServerClientData *)cg_thread_getuserdata(thread);
	httpServer = clientData->httpServer;
	clientSock = clientData->clientSock;
	httpServerUserData = cg_http_server_getuserdata(httpServer);

	httpReq = cg_http_request_new();
	cg_http_request_setsocket(httpReq, clientSock);

	/**** Thanks for Makela Aapo (10/31/05) ****/
	while (cg_http_request_read(httpReq, clientSock) == TRUE && cg_thread_isrunnable(thread) == TRUE) 
	{
		/* Check some validity of the request */
		version = cg_http_request_getversion(httpReq);
		if (cg_strcmp(version, CG_HTTP_VER11) == 0)
		{
			/* According to HTTP/1.1 spec, we must not tolerate
			   HTTP/1.1 request without HOST-header */
			if (cg_http_request_gethost(httpReq) == NULL)
			{
				cg_http_request_postbadrequest(httpReq);
				continue;
			}
		}

		if (httpServer->listener != NULL) 
		{	
            cg_http_request_setuserdata(httpReq, httpServerUserData);
			httpServer->listener(httpReq);
		}

		/* Close connection according to HTTP version and headers */
		if (cg_strcmp(version, CG_HTTP_VER10) == 0)
		{
			/* Terminate connection after HTTP/1.0 request */
			break;
		}

		if(clientData->runOneTime == TRUE)
		{
			break;
		}

		/* We are having HTTP/1.1 or better => terminate, if requested */
		/*if (cg_http_request_iskeepaliveconnection(httpReq) == FALSE)
		{
			break;
		}*/
	}

	{
		COS_EVENT ev;
		ev.nEventId = EV_HTTP_CLIENT_THREAD_EXIT_IND;
		ev.nParam1 = (UINT32)clientSock;
		ev.nParam2 = (UINT32)httpServer;
		COS_SendEvent(MOD_SSDP_ADVERTISE, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
	}

	cg_log_debug_s("Dropping HTTP client\n");
	cg_http_request_delete(httpReq);

	//cg_socket_delete(clientSock);

	cg_http_server_clientdata_delete(clientData);
	cg_thread_setuserdata(thread, NULL);

    // This code frequently crashes. mutex lock referencing free'd memory.
 #if !defined(RDA_DLNA)
	cg_http_server_lock(httpServer);
	cg_thread_remove(thread);
	cg_http_server_unlock(httpServer);
#endif

	cg_log_debug_l4("Leaving...\n");

	cg_thread_delete(thread);
}

/****************************************
* cg_http_server_thread
****************************************/
static void cg_http_server_thread(CgThread *thread)
{
	CgHttpServer *httpServer;
	CgThread *httpClientThread;
	CgHttpServerClientData *clientData;
	CgSocket *serverSock;
	CgSocket *clientSock;

	cg_log_debug_l4("Entering...\n");

	httpServer = (CgHttpServer *)cg_thread_getuserdata(thread);

	if (cg_http_server_isopened(httpServer) == FALSE)
		return;

	serverSock = httpServer->sock;
	cg_socket_settimeout(serverSock, 0xffffffff);
#if !defined(RDA_DLNA)
	while (cg_thread_isrunnable(thread) == TRUE) 
#endif
	{
		clientSock = cg_socket_stream_new();
		httpServer->listeningSock = clientSock;
		hal_HstSendEvent(SYS_EVENT, 0x08270014);
		if (cg_socket_accept(serverSock, clientSock) == FALSE) 
		{
			hal_HstSendEvent(SYS_EVENT, 0x08270011);
			cg_socket_delete(clientSock);
		}
		else
		{
			hal_HstSendEvent(SYS_EVENT, 0x08270012);

			cg_socket_settimeout(clientSock, 15);  //15s //cg_http_server_gettimeout(httpServer));
			clientData = cg_http_server_clientdata_new(httpServer, clientSock);
			
			clientData->runOneTime = FALSE;
			httpClientThread = cg_thread_new();
			cg_thread_setaction(httpClientThread, cg_http_server_clientthread);
			cg_thread_setuserdata(httpClientThread, clientData);
			
#if !defined(RDA_DLNA)	
			/**** Thanks for Makela Aapo (10/31/05) ****/
			cg_http_server_lock(httpServer);
			cg_threadlist_add(httpServer->clientThreads, httpClientThread);
			cg_http_server_unlock(httpServer);
#endif
			cg_thread_start(httpClientThread);
			{
				COS_EVENT event;
				event.nEventId = EV_HTTP_SERVER_SHIFT_START_REQ;
				event.nParam1 = (UINT32)httpServer;
				COS_SendEvent(MOD_HTTP_SERVER_SHIFT, &event, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
			}

			cg_http_server_clientthread(httpClientThread);
			httpServer->listeningSock = NULL;
			hal_HstSendEvent(SYS_EVENT, 0x08270013);
		}
	}

	cg_log_debug_l4("Leaving...\n");
}

static void cg_http_server_shift_thread(CgThread *thread)
{
	CgHttpServer *httpServer;
	CgThread *httpClientThread;
	CgHttpServerClientData *clientData;
	CgSocket *serverSock;
	CgSocket *clientSock;

	cg_log_debug_l4("Entering...\n");

	httpServer = (CgHttpServer *)cg_thread_getuserdata(thread);

	if (cg_http_server_isopened(httpServer) == FALSE)
		return;

	serverSock = httpServer->sock;
	while (cg_thread_isrunnable(thread) == TRUE)
	{
		clientSock = cg_socket_stream_new();
		cg_socket_settimeout(serverSock, 20); // 10s wait at most.
		if (cg_socket_accept(serverSock, clientSock) == FALSE) 
		{
			hal_HstSendEvent(SYS_EVENT, 0x08270001);
			cg_socket_delete(clientSock);
			if(!httpServer->listeningSock && httpServer->playing == FALSE)
			{
				COS_EVENT ev;
				ev.nEventId = EV_HTTP_SERVER_START_REQ;
				ev.nParam1 = (UINT32)httpServer->acceptThread;
				COS_SendEvent(MOD_HTTP_SERVER, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);

				ev.nEventId = EV_SSDP_ADVERTISE_START_REQ;
				COS_SendEvent(MOD_SSDP_ADVERTISE, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
				break;
			}
			break;
		}
		else
		{
			cg_socket_settimeout(clientSock, 0xffffffff); //cg_http_server_gettimeout(httpServer));
			clientData = cg_http_server_clientdata_new(httpServer, clientSock);

			clientData->runOneTime = TRUE;
			httpClientThread = cg_thread_new();
			cg_thread_setaction(httpClientThread, cg_http_server_clientthread);
			cg_thread_setuserdata(httpClientThread, clientData);

			
#if !defined(RDA_DLNA)	
			/**** Thanks for Makela Aapo (10/31/05) ****/
			cg_http_server_lock(httpServer);
			cg_threadlist_add(httpServer->clientThreads, httpClientThread);
			cg_http_server_unlock(httpServer);
#else
			cg_thread_start(httpClientThread);
			cg_http_server_clientthread(httpClientThread);	
#endif
		}
	}

	cg_thread_delete(thread);
	cg_log_debug_l4("Leaving...\n");
}



/****************************************
* cg_http_server_start
****************************************/

BOOL cg_http_server_start(CgHttpServer *httpServer)
{
	cg_log_debug_l4("Entering...\n");

	if (httpServer->acceptThread != NULL)
		return FALSE;

	httpServer->acceptThread = cg_thread_new();
	cg_thread_setaction(httpServer->acceptThread, cg_http_server_thread);
	cg_thread_setuserdata(httpServer->acceptThread, httpServer);

	/**** Thanks for Makela Aapo (10/31/05) ****/
	httpServer->clientThreads = cg_threadlist_new();

	if (cg_thread_start(httpServer->acceptThread) == FALSE) {
		cg_thread_delete(httpServer->acceptThread);
		httpServer->acceptThread = NULL;

		/**** Thanks for Makela Aapo (10/31/05) ****/
		cg_threadlist_delete(httpServer->clientThreads);
		httpServer->clientThreads = NULL;

		return FALSE;
	}

	{
		COS_EVENT ev;
		ev.nEventId = EV_HTTP_SERVER_START_REQ;
		ev.nParam1 = (UINT32)httpServer->acceptThread;
		COS_SendEvent(MOD_HTTP_SERVER, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
	}
	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* cg_http_server_stop
****************************************/

BOOL cg_http_server_stop(CgHttpServer *httpServer)
{
	cg_log_debug_l4("Entering...\n");

	if (httpServer->acceptThread != NULL) {
		cg_thread_stop(httpServer->acceptThread);
		cg_thread_delete(httpServer->acceptThread);
		httpServer->acceptThread = NULL;
	}
	/**** Thanks for Makela Aapo (10/31/05) ****/
	if (httpServer->clientThreads != NULL) {
		cg_threadlist_stop(httpServer->clientThreads);
		cg_threadlist_delete(httpServer->clientThreads);
		httpServer->clientThreads = NULL;
	}

	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* cg_http_server_setlistener
****************************************/

void cg_http_server_setlistener(CgHttpServer *httpServer, CG_HTTP_LISTENER listener)
{
	cg_log_debug_l4("Entering...\n");

	httpServer->listener = listener;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_getservername()
****************************************/

char *cg_http_getservername(char *buf, int bufSize)
{
#if defined(WIN32) && !defined(ITRON)
	OSVERSIONINFO verInfo;
	cg_strcpy(buf, "Platform 1.0");
	ZeroMemory(&verInfo, sizeof(OSVERSIONINFO));
	verInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (GetVersionEx(&verInfo)) {
		#if defined(HAVE_SNPRINTF)
		snprintf(buf, bufSize, "Windows %d.%d", verInfo.dwMajorVersion, verInfo.dwMinorVersion);
		#else
		sprintf(buf, "Windows %d.%d", verInfo.dwMajorVersion, verInfo.dwMinorVersion);
		#endif
	}
#elif defined(BTRON)
	T_VER verInfo;
	cg_strcpy(buf, "Platform 1.0");
	if (get_ver(&verInfo) == 0) {
		#if defined(HAVE_SNPRINTF)
		snprintf(buf, bufSize, "BTRON %hd", verInfo.spver);
		#else
		sprintf(buf, "BTRON %hd", verInfo.spver);
		#endif
	}
#elif defined(ITRON)
	cg_strcpy(buf, "uITRON 4.0");
#elif defined(TENGINE)
	cg_strcpy(buf, "T-Engine 1.0");
#elif defined(HAVE_UNAME) || defined(TARGET_OS_MAC) || defined(TARGET_OS_IPHONE)
	struct utsname unameBuf;
	cg_strcpy(buf, "Platform 1.0");
	if (uname(&unameBuf) == 0) {
		#if defined(HAVE_SNPRINTF)
		snprintf(buf, bufSize, "%s %s", unameBuf.sysname, unameBuf.release);
		#else
		sprintf(buf, "%s %s", unameBuf.sysname, unameBuf.release);
		#endif
	}
#else
	cg_strcpy(buf, "Platform 1.0");
#endif

	cg_log_debug_l4("Entering...\n");

	return buf;

	cg_log_debug_l4("Leaving...\n");
}

void BAL_HTTP_ServerTask_Shift(void* pData)
{
    COS_EVENT ev;

	while(1)
	{
		COS_WaitEvent(MOD_HTTP_SERVER_SHIFT, &ev, COS_WAIT_FOREVER);
		switch(ev.nEventId)
		{
		case EV_HTTP_SERVER_SHIFT_START_REQ:
			{
				CgThread *pThread = cg_thread_new();
				hal_HstSendEvent(SYS_EVENT, 0x08070051);
				
				cg_thread_setaction(pThread, cg_http_server_shift_thread);
				cg_thread_setuserdata(pThread, (CgThread*)ev.nParam1);
				cg_thread_start(pThread);
				cg_http_server_shift_thread(pThread);
			}
			break;
		default:
			break;
		}
	}
}

void BAL_HTTP_ServerTask(void* pData)
{
    COS_EVENT ev;
	CgHttpServer *httpServer = NULL;
	while(1)
	{
		COS_WaitEvent(MOD_HTTP_SERVER, &ev, COS_WAIT_FOREVER);
		switch(ev.nEventId)
		{
		case EV_HTTP_SERVER_START_REQ:
		case EV_HTTP_SERVER_LOOP_REQ:	
			{
				httpServer = cg_thread_getuserdata((CgThread*)ev.nParam1);
				cg_http_server_thread((CgThread*)ev.nParam1);
			}
			break;
		default:
			break;
		}
	}
		
}

