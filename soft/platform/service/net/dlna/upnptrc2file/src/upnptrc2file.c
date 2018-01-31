/* 
* Copyright (c) 2006-2016 RDA Microelectronics, Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#include "ccond.h"
#include <stdio.h>
#include "clog.h"
#include "upnptrc2file.h"
#include "chttp.h"
#include "cssdp_server.h"

#define rda_push_trc2file
#if defined(__rda_debug_log__)
#define sprintf_trc_data			\
{								\
	do                        			\
	{							\
		va_list var;				\
		va_start(var, fmt);			\
		vsprintf(trcData,fmt,var);	\
		va_end(var);				\
		strncat(trcData,"\n",MAX_LOG_STRING);\
	} while (0);\
}

static FILE *fs_httpu = NULL,*fs_httpmu = NULL,*fs_ssdp = NULL,*fs_soap = NULL;
static CgMutex *(rda_trc_mutex) = NULL;
static char trcData[MAX_LOG_STRING] = {0,};
static BOOL rdaTrcInitFlag = FALSE;		

void rda_trc_file_init(void)
{	
	int i;
	char trc_file_path[RDA_TRC_PATH_LEN]={0,};
	
	if(0 != (access(RDA_DEFAULT_TRC_DIR,0)))
		mkdir(RDA_DEFAULT_TRC_DIR);

	_snprintf(trc_file_path, RDA_TRC_PATH_LEN,"%s%c%s",RDA_DEFAULT_TRC_DIR,'/',TRC_FILE_HTTPU);
	fs_httpu = fopen(trc_file_path, "w");
	memset(trc_file_path,0,RDA_TRC_PATH_LEN);

	_snprintf(trc_file_path, RDA_TRC_PATH_LEN,"%s%c%s",RDA_DEFAULT_TRC_DIR,'/',TRC_FILE_HTTPMU);
	fs_httpmu = fopen(trc_file_path, "w");
	memset(trc_file_path,0,RDA_TRC_PATH_LEN);
	
	_snprintf(trc_file_path, RDA_TRC_PATH_LEN,"%s%c%s",RDA_DEFAULT_TRC_DIR,'/',TRC_FILE_SSDP);
	fs_ssdp = fopen(trc_file_path, "w");
	memset(trc_file_path,0,RDA_TRC_PATH_LEN);

	_snprintf(trc_file_path, RDA_TRC_PATH_LEN,"%s%c%s",RDA_DEFAULT_TRC_DIR,'/',TRC_FILE_SOAP);
	fs_soap = fopen(trc_file_path, "w");
	rdaTrcInitFlag = TRUE;					
}

void rda_trc2file(unsigned short trc_file_enum)
{
	if(!rdaTrcInitFlag)
		rda_trc_file_init();
	switch(trc_file_enum)
	{
		case ENUM_TRC_FILE_HTTPU:
			fputs(trcData, fs_httpu);
			break;
		case ENUM_TRC_FILE_HTTPMU:
			fputs(trcData, fs_httpmu);
			break;
		case ENUM_TRC_FILE_SSDP:
			fputs(trcData, fs_ssdp);
			break;
		case ENUM_TRC_FILE_SOAP:
			fputs(trcData, fs_soap);
			break;
	}
	memset(trcData,0,MAX_LOG_STRING);
}

void rda_trace_httpu(const char *fmt,...)
{
	if (!rda_trc_mutex)
		rda_trc_mutex = cg_mutex_new();
	cg_mutex_lock(rda_trc_mutex);
	sprintf_trc_data;
	rda_trc2file(ENUM_TRC_FILE_HTTPU);
	cg_mutex_unlock(rda_trc_mutex);
}

void rda_trace_httpmu(const char *fmt,...)
{
	if (!rda_trc_mutex)
		rda_trc_mutex = cg_mutex_new();
	cg_mutex_lock(rda_trc_mutex);
	sprintf_trc_data;
	rda_trc2file(ENUM_TRC_FILE_HTTPMU);
	cg_mutex_unlock(rda_trc_mutex);
}

void rda_trace_ssdp(const char *fmt,...)
{
	if (!rda_trc_mutex)
		rda_trc_mutex = cg_mutex_new();
	cg_mutex_lock(rda_trc_mutex);
	sprintf_trc_data;
	rda_trc2file(ENUM_TRC_FILE_SSDP);
	cg_mutex_unlock(rda_trc_mutex);
}

void rda_trace_soap(const char *fmt,...)
{
	if (!rda_trc_mutex)
		rda_trc_mutex = cg_mutex_new();
	cg_mutex_lock(rda_trc_mutex);
	sprintf_trc_data;
	rda_trc2file(ENUM_TRC_FILE_SOAP);
	cg_mutex_unlock(rda_trc_mutex);
}

void rda_trc_file_close(void)
{
	if(NULL != fs_httpu)
		fclose(fs_httpu);
	if(NULL != fs_httpmu)
		fclose(fs_httpmu);
	if(NULL != fs_ssdp)
		fclose(fs_ssdp);
	if(NULL != fs_soap)
		fclose(fs_soap);
	rdaTrcInitFlag = FALSE;
}


#if defined(__rda_cust_http_server_debug__)
void rdadebug_log_cg_http_server_clientthread(CgHttpRequest *httpReq)
{
	char logBuff[1024*10]={0,};
	CgHttpHeader *header;
	
	sprintf(logBuff,"%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n",
					"===================ccg_http_request received Enter====================",
					"===================ccg_http_request dump 1 start======================",
					cg_string_getvalue(httpReq->content),
					cg_string_getvalue(httpReq->method),
					cg_string_getvalue(httpReq->uri),
					cg_string_getvalue(httpReq->version),
					cg_string_getvalue(httpReq->userAgent),
					"===================ccg_http_request dump 2 start======================"
	);
	for (header = cg_http_headerlist_gets(httpReq->headerList); header != NULL; header = cg_http_header_next(header)) 
	{
		strcat(logBuff,cg_http_header_getname(header));
		strcat(logBuff,cg_http_header_getvalue(header));
		strcat(logBuff,"\n");
	}
	
	strcat(logBuff,"===================ccg_http_request dump end==========================\n\n");
#ifdef rda_push_trc2file
	rda_trace_soap("%s",logBuff);
#else
	printf("%s",logBuff);
#endif
		
}
#endif

#if defined(__rda_cust_postsearchresponse_debug__)
void rdadebug_log_cg_upnp_device_postsearchresponse(CgUpnpSSDPPacket *ssdpPkt,CgUpnpSSDPResponse *ssdpRes)
{
	char logBuff[1024*10]={0,};
	CgString *ssdpMsg;
	char *localAddr;
	char *remoteAddr;
	int remotePort;

	localAddr = cg_upnp_ssdp_packet_getlocaladdress(ssdpPkt);
	remoteAddr = cg_upnp_ssdp_packet_getremoteaddress(ssdpPkt);
	remotePort = cg_upnp_ssdp_packet_getremoteport(ssdpPkt);
	ssdpMsg = cg_string_new();
	cg_upnp_ssdpresponse_tostring(ssdpRes, ssdpMsg);
	
	sprintf(logBuff,"%s\nlocalAddr: %s,remoteAddr: %s,remotePort: %d \n%s\n%s\n%s\n\n",
					"====================cg_upnp_ssdp_socket_postresponse Enter====================",
					localAddr,
					remoteAddr,
					remotePort,
					"====================cg_upnp_ssdp_socket_postresponse dump start===============",
					ssdpMsg->value,
					"====================cg_upnp_ssdp_socket_postresponse dump end	==============="
	);

	cg_string_delete(ssdpMsg);
#ifdef rda_push_trc2file
	rda_trace_httpu("%s",logBuff);
#else
	printf("%s",logBuff);
#endif
}
#endif	

#if defined(__rda_cust_ssdpmessagereceived_debug__)
void rdadebug_log_cg_upnp_device_ssdpmessagereceived(CgUpnpSSDPPacket *ssdpPkt)
{
	CgHttpHeader *header;
	char logBuff[1024*10]={0,};	
	{
		char * strusn;
		char *filter[]={	"ec1a59043284",
							"00000000-0000-0001-1000-ec1a59043284",
							NULL};
		int i=0;
		int needPrint=1;
		strusn=cg_upnp_ssdp_packet_getusn(ssdpPkt);
		if (strusn!=NULL)
		{
			do{
				if(((0 <= cg_strstr(strusn, filter[i])) ? TRUE : FALSE))
				{
					needPrint=0;
					break;
				}
				i++;
			}while(filter[i]!=NULL);
		}

		if (needPrint)
		{
			sprintf(logBuff,"%s\nremoteAddress: %s remotePort: %d\n%s\n%s\n",
							"===================cg_upnp_device_ssdpmessagereceived Enter===================",
							cg_upnp_ssdp_packet_getremoteaddress(ssdpPkt),
							cg_upnp_ssdp_packet_getremoteport(ssdpPkt),
							cg_string_getvalue(ssdpPkt->dgmPkt->data),
							"===================cg_upnp_device_ssdpmessagereceived dump start=============="
			);
			for (header = cg_http_headerlist_gets(ssdpPkt->headerList); header != NULL; header = cg_http_header_next(header)) 
			{
				strcat(logBuff,cg_http_header_getname(header));
				strcat(logBuff,cg_http_header_getvalue(header));
				strcat(logBuff,"\n");
			}			
			strcat(logBuff,"===================ccg_http_request dump end==========================\n\n");
		}
		else
		{
			/*		
			sprintf(logBuff,"%s\n",
							"===================cg_upnp_device_ssdpmessagereceived dump skip==============",
			);
			*/
		}
	}
#ifdef rda_push_trc2file
	rda_trace_ssdp("%s",logBuff);
#else	
	printf("%s",logBuff);
#endif
}
#endif

#if defined(__rda_cust_httpmu_recv_debug__)
void rdadebug_log_cg_upnp_httpmu_socket_recv(CgUpnpSSDPPacket *ssdpPkt)
{
	CgDatagramPacket *dgmPkt;
	char *ssdpData;
	char logBuff[1024*10]={0,};

	dgmPkt = cg_upnp_ssdp_packet_getdatagrampacket(ssdpPkt);
	ssdpData = cg_socket_datagram_packet_getdata(dgmPkt);
	{
		char *filter[]={	"ec1a59043284",
							"00000000-0000-0001-1000-ec1a59043284",
							NULL};
		int i=0;
		int needPrint=1;
		{
			do{
				if(((0 <= cg_strstr(ssdpData, filter[i])) ? TRUE : FALSE))
				{
					needPrint=0;
					break;
				}
				i++;
			}while(filter[i]!=NULL);
		}
		if (needPrint)
		{
			sprintf(logBuff,"%s\nremoteAddress: %s remotePort: %d\n%s\n%s\n%s\n\n",
							"=====================cg_upnp_httpmu_socket_recv  Enter========================",
							cg_upnp_ssdp_packet_getremoteaddress(ssdpPkt),
							cg_upnp_ssdp_packet_getremoteport(ssdpPkt),
							"=====================cg_upnp_httpmu_socket_recv dump start====================",
							ssdpData,
							"=====================cg_upnp_httpmu_socket_recv dump end  ====================");
		}
		else
		{
		/*
			sprintf(logBuff,"%s\n%s\n",
							"=====================cg_upnp_httpmu_socket_recv skip display==================\n",
							ssdpData);
		*/					
		}
	}
#ifdef rda_push_trc2file
	rda_trace_httpmu("%s",logBuff);
#else		
	printf("%s",logBuff);
#endif
}
#endif

#if defined(__rda_cust_httpu_recv_debug__)
void rdadebug_log_cg_upnp_httpu_socket_recv(CgUpnpSSDPPacket *ssdpPkt)
{
	CgDatagramPacket *dgmPkt;
	char *ssdpData;
	char logBuff[1024*10]={0,};	

	dgmPkt = cg_upnp_ssdp_packet_getdatagrampacket(ssdpPkt);
	ssdpData = cg_socket_datagram_packet_getdata(dgmPkt);
	{
		char *filter[]={	"ec1a59043284",
		"00000000-0000-0001-1000-ec1a59043284",
		NULL};
		int i=0;
		int needPrint=1;
		{
			do{
				if(((0 <= cg_strstr(ssdpData, filter[i])) ? TRUE : FALSE))
				{
					needPrint=0;
					break;
				}
				i++;
			}while(filter[i]!=NULL);
		}
		if (needPrint)
		{
			sprintf(logBuff,"%s\nremoteAddress: %s remotePort: %d\n%s\n%s\n%s\n\n",
				"=====================cg_upnp_httpu_socket_recv  Enter=========================",
				cg_upnp_ssdp_packet_getremoteaddress(ssdpPkt),
				cg_upnp_ssdp_packet_getremoteport(ssdpPkt),
				"=====================cg_upnp_httpu_socket_recv dump start=====================",
				ssdpData,
				"=====================cg_upnp_httpu_socket_recv dump end  =====================");
		}
		else
		{
			/*
			sprintf(logBuff,"%s\n%s\n",
			"=====================cg_upnp_httpu_socket_recv skip display===================\n",
			ssdpData);
			*/					
		}
	}
#ifdef rda_push_trc2file
	rda_trace_httpu("%s",logBuff);
#else
	printf("%s",logBuff);
#endif	
}
#endif

#endif

