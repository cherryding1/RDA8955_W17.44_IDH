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
#ifndef UPNP_TRC2FILE_GPROT_H
#define UPNP_TRC2FILE_GPROT_H
#ifdef __rda_debug_log__
void rda_trc2file(unsigned short trc_file_enum);
void rda_trc_file_init(void);
void rda_trace_httpu(const char *fmt,...);
void rda_trace_httpmu(const char *fmt,...);
void rda_trace_ssdp(const char *fmt,...);
void rda_trace_soap(const char *fmt,...);
void rda_trc_file_close(void);
#ifdef __rda_cust_http_server_debug__
void rdadebug_log_cg_http_server_clientthread(CgHttpRequest *httpReq);
#endif
#ifdef __rda_cust_ssdpmessagereceived_debug__
void rdadebug_log_cg_upnp_device_ssdpmessagereceived(CgUpnpSSDPPacket *ssdpPkt);
#endif
#ifdef __rda_cust_postsearchresponse_debug__
void rdadebug_log_cg_upnp_device_postsearchresponse(CgUpnpSSDPPacket *ssdpPkt,CgUpnpSSDPResponse *ssdpRes);
#endif
#if defined(__rda_cust_httpmu_recv_debug__)
void rdadebug_log_cg_upnp_httpmu_socket_recv(CgUpnpSSDPPacket *ssdpPkt);
#endif
#if defined(__rda_cust_httpu_recv_debug__)
void rdadebug_log_cg_upnp_httpu_socket_recv(CgUpnpSSDPPacket *ssdpPkt);
#endif
#endif
#endif//UPNP_TRC2FILE_GPROT_H


