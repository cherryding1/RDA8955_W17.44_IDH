/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Simon Goldschmidt
 *
 */
#ifndef LWIP_HDR_LWIPOPTS_H
#define LWIP_HDR_LWIPOPTS_H

#include "sxs_io.h"
#include <cswtype.h>
#include "csw.h"
#include "cfw.h"
#include "event.h"
#define CFW_TCPIP_TS_ID_SXS CFW_GPRS_TS_ID
#define MMI_TS_ID_SXS  (_MMI | TLEVEL(1))
#define CFW_TCPIPT_TS_ID_SXS CFW_GPRS_TS_ID

#ifdef LWIP_IPV6_ON
#define LWIP_IPV6                   1
#else
#ifdef LTE_NBIOT_SUPPORT
#define LWIP_IPV6                   1
#else
#define LWIP_IPV6                   0
#endif //LTE_NBIOT_SUPPORT
#endif //LWIP_IPV6_ON

#ifdef LWIP_IPV4_ON
#define LWIP_IPV4                   1
#else
#ifdef LTE_NBIOT_SUPPORT
#define LWIP_IPV4                   1
#else
#define LWIP_IPV4                   0
#endif //LTE_NBIOT_SUPPORT
#endif //LWIP_IPV4_ON


#ifndef SXS_NO_PRINTF
#ifdef LWIP_DEBUG_ON
#define LWIP_DEBUG                  1
#else
#define LWIP_DEBUG                  0
#endif
#else
#define LWIP_DEBUG                  0
#endif

#define PPP_SUPPORT                 1


#define PPP_SERVER                  PPP_SUPPORT
#define CCP_SUPPORT                 PPP_SUPPORT
#define PPP_PROTOCOLNAME            1
#define PRINTPKT_SUPPORT            1
#define PPP_NOTIFY_PHASE            1
#define IP_FORWARD                  1
#define PAP_SUPPORT                     1
#define CHAP_SUPPORT                    1
#define EAP_SUPPORT                     1
#define MPPE_SUPPORT                    1
#define LWIP_USE_EXTERNAL_MBEDTLS       1
#define LWIP_TCP_KEEPALIVE          1
#ifdef LTE_NBIOT_SUPPORT
#define LWIP_IPV6_DUP_DETECT_ATTEMPTS   0
#endif

/** LWIP_ALTCP_TLS_MBEDTLS==1: use mbedTLS for TLS support for altcp API
 * mbedtls include directory must be reachable via include search path
 */

#define LWIP_ALTCP_TLS_MBEDTLS                        1
/** LWIP_ALTCP==1: enable the altcp API
 * altcp is an abstraction layer that prevents applications linking against the
 * tcp.h functions but provides the same functionality. It is used to e.g. add
 * SSL/TLS or proxy-connect support to an application written for the tcp callback
 * API without that application knowing the protocol details.
 * Applications written against the altcp API are directly linked against the
 * tcp callback API for LWIP_ALTCP==0, but then cannot use layered protocols.
 */

#define LWIP_ALTCP                      1
/** LWIP_ALTCP_TLS==1: enable TLS support for altcp API.
 * This needs a port of the functions in altcp_tls.h to a TLS library.
 * A port to ARM mbedtls is provided with lwIP, see apps/altcp_tls/ directory
 * and LWIP_ALTCP_TLS_MBEDTLS option.
 */

#define LWIP_ALTCP_TLS                  1



/* Prevent having to link sys_arch.c (we don't test the API layers in unit tests) */
#define NO_SYS                          0
#define LWIP_PROVIDE_ERRNO

#define SYS_LIGHTWEIGHT_PROT            1
#define LWIP_NETCONN                    0
#define LWIP_SOCKET                     1
#define LWIP_DNS                        1
#define LWIP_RAW                        1
#define LWIP_NETIF_API                  0
#define LWIP_TCPIP_CORE_LOCKING         0
#define LWIP_TCPIP_CORE_LOCKING_INPUT   0
#define LWIP_ARP                        1
#define LWIP_TIMERS                     0
/* Enable DHCP to test it, disable UDP checksum to easier inject packets */
#define LWIP_DHCP                       1
#define LWIP_IPV6_MLD                   1
#ifdef LTE_NBIOT_SUPPORT
#define MEMP_MEM_MALLOC                 0
#else
#define MEMP_MEM_MALLOC                 1
#endif
#define MEM_LIBC_MALLOC                 1
#define mem_clib_free COS_FREE
#define mem_clib_malloc COS_MALLOC
#define mem_clib_calloc

#define malloc COS_MALLOC

#define LWIP_SO_RCVTIMEO                0
#define LWIP_SO_RCVBUF                  1
#define LWIP_MPU_COMPATIBLE             1

#define MEM_SIZE                        81920


/* Minimal changes to opt.h required for tcp unit tests: */
#if 0
#define TCP_SND_QUEUELEN                40
#define MEMP_NUM_TCP_SEG                TCP_SND_QUEUELEN
#define TCP_SND_BUF                     (12 * TCP_MSS)
#define TCP_WND                         (10 * TCP_MSS)
#define LWIP_WND_SCALE                  1
#define TCP_RCV_SCALE                   0
#define PBUF_POOL_SIZE                  40 /* pbuf tests need ~200KByte */
#define MEMP_NUM_SYS_TIMEOUT            16
#else
#define TCP_MSS                         1440
#define TCP_WND                         (16 * TCP_MSS)
#define TCP_SND_BUF                     (8 * TCP_MSS)
#define MEMP_NUM_TCP_SEG                TCP_SND_QUEUELEN
#define LWIP_WND_SCALE                  1
#define TCP_RCV_SCALE                   0
#define PBUF_POOL_SIZE                  40
#endif
#define LWIP_STATS                      0
#define LWIP_ICMP                       1

#define MEM_ALIGNMENT                   4
#define ETH_PAD_SIZE                    0

/* Enable IGMP and MDNS for MDNS tests */
#define LWIP_IGMP                       0
#define LWIP_MDNS_RESPONDER             0
#define LWIP_NUM_NETIF_CLIENT_DATA      (LWIP_MDNS_RESPONDER)

/* Minimal changes to opt.h required for etharp unit tests: */
#define ETHARP_SUPPORT_STATIC_ENTRIES   0

#define USE_CUSTOMER_THREAD         !LWIP_TIMERS
#define TCPIP_THREAD_STACKSIZE  2048
#define TCPIP_THREAD_PRIO   BAL_CFW_ADV_TASK_PRIORITY

#if !LWIP_DEBUG
#define tcp_debug_print_pcbs()
#define sys_arch_printf(format, ...)
#define sys_arch_assert(x)
#define LWIP_DBG_ALL    0
#define LWIP_NOASSERT   1
#else
#define LWIP_DBG_ALL    LWIP_DBG_ON
#define TCP_DEBUG_PCB_LISTS 1
#endif

#ifndef _TIME_T_DEFINED
#define _TIME_T_DEFINED
typedef long         time_t;
#endif

#define LWIP_HOOK_IP4_ROUTE_SRC(dest, src) ip4_src_route(dest,src)

#endif /* LWIP_HDR_LWIPOPTS_H */
