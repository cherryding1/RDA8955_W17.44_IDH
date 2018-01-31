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
*	File: csocket.h
*
*	Revision:
*
*	01/17/05
*		- first revision
*	04/03/06 Theo Beisch
*		- added WSAGetLastError support
*		- changed socket_cleanup to return
*		  value to enable final Winsock close 
******************************************************************/

#ifndef _CG_NET_CSOCKET_H_
#define _CG_NET_CSOCKET_H_

#include "http_types.h"
#include "http_if.h"
#include "cstring.h"

#include "cinterface.h"

#if defined(CG_USE_OPENSSL)
#ifdef MBEDTLS
#include "mbedtls/ssl.h"
#include <mbedtls/net.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#else
#include <openssl/ssl.h>
#include "error-crypt.h"
#endif

//#include "error-crypt.h"
#endif

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

#define CG_NET_SOCKET_NONE 0

#define CG_NET_SOCKET_STREAM 0x01
#define CG_NET_SOCKET_DGRAM 0x02

#define CG_NET_SOCKET_CLIENT 1
#define CG_NET_SOCKET_SERVER 2

#define CG_NET_SOCKET_MAXHOST 32
#define CG_NET_SOCKET_MAXSERV 32

#if defined(CG_USE_OPENSSL)

/**** HTTPS certificate type ****/
#define CG_HTTPS_CA_CRT 0
#define CG_HTTPS_CLIENT_CRT 1
#define CG_HTTPS_CLIENT_PRIVATE_KEY 2

/*
static char* ca_pem_default="-----BEGIN CERTIFICATE-----\n\
MIICvTCCAiagAwIBAgIJAN9BSz3o74bZMA0GCSqGSIb3DQEBCwUAMHYxCzAJBgNV\n\
BAYTAkNOMQswCQYDVQQIDAJUSjEQMA4GA1UEBwwHdGlhbmppbjEMMAoGA1UECgwD\n\
UkRBMQwwCgYDVQQLDANSREExDDAKBgNVBAMMA1JEQTEeMBwGCSqGSIb3DQEJARYP\n\
bW1AcmRhbWljcm8uY29tMB4XDTE3MDkxODA4NDYzMFoXDTE3MTIxNzA4NDYzMFow\n\
djELMAkGA1UEBhMCQ04xCzAJBgNVBAgMAlRKMRAwDgYDVQQHDAd0aWFuamluMQww\n\
CgYDVQQKDANSREExDDAKBgNVBAsMA1JEQTEMMAoGA1UEAwwDUkRBMR4wHAYJKoZI\n\
hvcNAQkBFg9tbUByZGFtaWNyby5jb20wgZ8wDQYJKoZIhvcNAQEBBQADgY0AMIGJ\n\
AoGBAK0wt//LO9JYRKd52vSCcpCX/KthEhqzXWvZZB2biqgwQWEYWjOpRDDoPlzU\n\
WbM0uq8aLTn9U/TLX+JXQYbfZM8rqds5Xn2sonyo1y+v+GFB0PG+DzNFlYx79c5v\n\
6VXaPLrZ3mHd41kRTBTKHKb6hAnySLg8XbztmHeX86+I7yBhAgMBAAGjUzBRMB0G\n\
A1UdDgQWBBSYiBRGalbh0/iTiBDFoXpbIsnePjAfBgNVHSMEGDAWgBSYiBRGalbh\n\
0/iTiBDFoXpbIsnePjAPBgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3DQEBCwUAA4GB\n\
AJx9ecP7VRJr0hTo5Vls8MFKlLDBi8EV8avsNvd3wSkZ1M/0mL3nAUYUBanpQAvO\n\
T7P9yeQGgUbIfYgczfeNDHCCkWNRxVANMba+KiUuCpxzH+aeKh3ccTlyLP907UNY\n\
lObAMWNspJ4kMm+vS9p2Jlj1gZCtgDl9N+dveViZn3PN\n\
-----END CERTIFICATE-----";


static char* cert_pem_default="Certificate:\n\
    Data:\n\
        Version: 3 (0x2)\n\
        Serial Number: 11 (0xb)\n\
    Signature Algorithm: sha256WithRSAEncryption\n\
        Issuer: C=CN, ST=TJ, L=tianjin, O=RDA, OU=RDA, CN=RDA/emailAddress=mm@rdamicro.com\n\
        Validity\n\
            Not Before: Sep 15 00:00:00 2017 GMT\n\
            Not After : Sep 15 00:00:00 2018 GMT\n\
        Subject: C=CN, ST=TJ, O=RDA, OU=RDA, CN=RDA/emailAddress=mm@rdamicro.com\n\
        Subject Public Key Info:\n\
            Public Key Algorithm: rsaEncryption\n\
                Public-Key: (1024 bit)\n\
                Modulus:\n\
                    00:ce:d2:55:22:66:c6:6a:39:a9:04:44:bc:5f:34:\n\
                    9f:2a:d9:f4:98:a9:05:ba:c1:15:31:6b:ed:b6:29:\n\
                    82:06:06:c8:a6:f5:97:73:f7:c9:b7:ce:74:aa:75:\n\
                    c2:4f:5e:9d:16:50:fc:9e:fd:8e:c5:32:8a:0c:41:\n\
                    93:d2:b4:28:7f:a4:24:3f:36:3c:69:fc:b7:00:87:\n\
                    ec:98:e4:6f:4f:0e:fa:6a:fe:bd:22:40:bb:17:4d:\n\
                    49:3f:e3:86:78:ae:31:21:74:bd:f5:ed:cb:68:ad:\n\
                    65:e8:c9:35:42:20:19:8b:1e:dc:e0:32:f2:0b:fe:\n\
                    d4:bf:dc:f3:74:d3:54:ac:4f\n\
                Exponent: 65537 (0x10001)\n\
        X509v3 extensions:\n\
            X509v3 Basic Constraints:\n\ 
                CA:FALSE\n\
            Netscape Comment:\n\ 
                OpenSSL Generated Certificate\n\
            X509v3 Subject Key Identifier:\n\ 
                FE:64:B3:C3:9C:14:D7:F4:DF:33:7F:34:DA:0D:A6:F8:34:73:5B:6B\n\
            X509v3 Authority Key Identifier:\n\ 
                keyid:98:88:14:46:6A:56:E1:D3:F8:93:88:10:C5:A1:7A:5B:22:C9:DE:3E\n\
\n\
    Signature Algorithm: sha256WithRSAEncryption\n\
         7a:a6:71:d0:56:32:bb:c3:66:f8:49:03:b0:27:e7:91:b6:f4:\n\
         e1:fd:05:6a:ed:2c:0c:2a:3f:45:cf:c5:43:c0:0c:ff:9f:f1:\n\
         9a:07:8c:e5:93:94:2d:e7:aa:9f:d4:1e:b0:47:8e:85:2f:33:\n\
         c5:66:4a:e3:7e:ec:d6:58:d9:ea:55:f1:f3:15:89:f8:a9:69:\n\
         f6:74:04:ef:76:12:03:fe:b2:3c:a7:02:4e:cb:18:25:84:c2:\n\
         e5:4c:26:bb:d0:86:1e:0b:5c:0a:75:ef:51:ae:2d:d3:7f:d9:\n\
         b7:4a:a5:ef:e1:dd:e2:e3:0f:6a:cb:e8:ab:82:c2:51:5e:6d:\n\
         01:08\n\
-----BEGIN CERTIFICATE-----\n\
MIICyzCCAjSgAwIBAgIBCzANBgkqhkiG9w0BAQsFADB2MQswCQYDVQQGEwJDTjEL\n\
MAkGA1UECAwCVEoxEDAOBgNVBAcMB3RpYW5qaW4xDDAKBgNVBAoMA1JEQTEMMAoG\n\
A1UECwwDUkRBMQwwCgYDVQQDDANSREExHjAcBgkqhkiG9w0BCQEWD21tQHJkYW1p\n\
Y3JvLmNvbTAeFw0xNzA5MTUwMDAwMDBaFw0xODA5MTUwMDAwMDBaMGQxCzAJBgNV\n\
BAYTAkNOMQswCQYDVQQIDAJUSjEMMAoGA1UECgwDUkRBMQwwCgYDVQQLDANSREEx\n\
DDAKBgNVBAMMA1JEQTEeMBwGCSqGSIb3DQEJARYPbW1AcmRhbWljcm8uY29tMIGf\n\
MA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDO0lUiZsZqOakERLxfNJ8q2fSYqQW6\n\
wRUxa+22KYIGBsim9Zdz98m3znSqdcJPXp0WUPye/Y7FMooMQZPStCh/pCQ/Njxp\n\
/LcAh+yY5G9PDvpq/r0iQLsXTUk/44Z4rjEhdL317ctorWXoyTVCIBmLHtzgMvIL\n\
/tS/3PN001SsTwIDAQABo3sweTAJBgNVHRMEAjAAMCwGCWCGSAGG+EIBDQQfFh1P\n\
cGVuU1NMIEdlbmVyYXRlZCBDZXJ0aWZpY2F0ZTAdBgNVHQ4EFgQU/mSzw5wU1/Tf\n\
M3802g2m+DRzW2swHwYDVR0jBBgwFoAUmIgURmpW4dP4k4gQxaF6WyLJ3j4wDQYJ\n\
KoZIhvcNAQELBQADgYEAeqZx0FYyu8Nm+EkDsCfnkbb04f0Fau0sDCo/Rc/FQ8AM\n\
/5/xmgeM5ZOULeeqn9QesEeOhS8zxWZK437s1ljZ6lXx8xWJ+Klp9nQE73YSA/6y\n\
PKcCTssYJYTC5Uwmu9CGHgtcCnXvUa4t03/Zt0ql7+Hd4uMPasvoq4LCUV5tAQg=\n\
-----END CERTIFICATE-----";

static char* key_pem_default="-----BEGIN RSA PRIVATE KEY-----\n\
MIICWwIBAAKBgQDO0lUiZsZqOakERLxfNJ8q2fSYqQW6wRUxa+22KYIGBsim9Zdz\n\
98m3znSqdcJPXp0WUPye/Y7FMooMQZPStCh/pCQ/Njxp/LcAh+yY5G9PDvpq/r0i\n\
QLsXTUk/44Z4rjEhdL317ctorWXoyTVCIBmLHtzgMvIL/tS/3PN001SsTwIDAQAB\n\
AoGAdfn1JlKKDD4n849iBMz3nnIF3K9N3WIWvMJcLvUuflGQU+z0H+rQ3lCerD3Q\n\
QYYvBXPdJrRspIBXzvX5ZM/v6RkczRO3tWSs1O+hUlgpBCS67IoTvQo7bANWxbfn\n\
eQvXfVafNkfPAIMDA2FlETXVoTMjoWEWJLKc4pnq+sHnQQECQQDrMgrD8VnLC5k2\n\
WuF/o0mLx1D6e8OZTeHIuepEhLyZwS/Gkeiu/GNaOk0FJEO9qpZs24BMlJn71+cd\n\
8Hrj7m3xAkEA4R3FA8N33ipTBLiLOolSR1QSeiVhvFm36MsYYg2BqJ7+kTcxoLlh\n\
IIbjOizONecn6Xeepg2kWhTcULIVuzt+PwJAbpc5OKlj73WktUpGfh3plzBBg4Zu\n\
ksdsQ1YoMc+4XxPXwycYoi9LCEIhtSk4MRblZIpR6bB2N1Vt90X8gvC6AQJAPF2l\n\
RRsXe6XR2UQ7YZkNTT9ILST1DnLolC5R/Yktqa/pB7vLPRQ47WX+bM0zBtVTqJGB\n\
I3opBlbttV3r4edUNQJAHsgXxIpcVDlsIeL+tghkRZqvkYkd+HriKDQMgOGX00Z/\n\
kokwpK8OTd65XmiaZ3FtqcZLUwVJ3Sdwcb9hyNySPw==\n\
-----END RSA PRIVATE KEY-----";
*/
#endif


#if defined(BTRON) || defined(TENGINE)
typedef W CSOCKET;
#elif defined(ITRON)
typedef ER CSOCKET;
#elif defined(RDA_HTTP_LWIP)
typedef int CSOCKET;
#elif !defined(WIN32) && !defined(__CYGWIN__)
//typedef INT8 CSOCKET;
#endif

#define CG_SOCKET_LF '\n'

#define CG_NET_SOCKET_DGRAM_RECV_BUFSIZE 512
#define CG_NET_SOCKET_DGRAM_ANCILLARY_BUFSIZE 512
#define CG_NET_SOCKET_MULTICAST_DEFAULT_TTL 4
#define CG_NET_SOCKET_AUTO_IP_NET 0xa9fe0000
#define CG_NET_SOCKET_AUTO_IP_MASK 0xffff0000 

#if defined(ITRON)
#define CG_NET_SOCKET_WINDOW_BUFSIZE 4096
#endif

/****************************************
* Define (SocketList)
****************************************/

#if defined(ITRON) || defined(RDA_HTTP)
#define CG_NET_USE_SOCKET_LIST 1
#endif

/****************************************
* Data Type
****************************************/

#if defined(CG_NET_USE_SOCKET_LIST)
#include "clist.h"
#endif

typedef struct _CgSocket {
#if defined(CG_NET_USE_SOCKET_LIST)
	BOOL headFlag;
	struct _CgSocket *prev;
	struct _CgSocket *next;
#endif
	CSOCKET id;
	int type;
	int direction;
	CgString *ipaddr;
	int port;
	BOOL *runable;
#if defined(ITRON)
	UH *sendWinBuf;
	UH *recvWinBuf;
#endif
#if defined(CG_USE_OPENSSL)
    #ifdef MBEDTLS
	mbedtls_ssl_context* ssl;
    mbedtls_net_context server_fd;
	mbedtls_entropy_context entropy;
	mbedtls_ctr_drbg_context ctr_drbg;
	mbedtls_ssl_config conf;
	mbedtls_x509_crt cacert;
	mbedtls_x509_crt clicert;
    mbedtls_pk_context pkey;
	#else
	SSL_CTX* ctx;
	SSL* ssl;
	#endif

#endif
} CgSocket, CgSocketList;

typedef struct _CgDatagramPacket {
	CgString *data;
	CgString *localAddress;
	int localPort;
	CgString *remoteAddress;
	int remotePort;
} CgDatagramPacket;

/****************************************
* Function (Socket)
****************************************/

void cg_socket_startup();
void cg_socket_cleanup();

CgSocket *cg_socket_new(int type);
#define cg_socket_stream_new() cg_socket_new(CG_NET_SOCKET_STREAM)
#define cg_socket_dgram_new() cg_socket_new(CG_NET_SOCKET_DGRAM)
int cg_socket_delete(CgSocket *socket);

void cg_socket_setid(CgSocket *socket, CSOCKET value);
#define cg_socket_getid(socket) (socket->id)

#define cg_socket_settype(socket, value) (socket->type = value)
#define cg_socket_gettype(socket) (socket->type)
#define cg_socket_issocketstream(socket) ((socket->type & CG_NET_SOCKET_STREAM) ? TRUE : FALSE)
#define cg_socket_isdatagramstream(socket) ((socket->type & CG_NET_SOCKET_DGRAM) ? TRUE : FALSE)

#define cg_socket_setdirection(socket, value) (socket->direction = value)
#define cg_socket_getdirection(socket) (socket->direction)
#define cg_socket_isclient(socket) ((socket->direction == CG_NET_SOCKET_CLIENT) ? TRUE : FALSE)
#define cg_socket_isserver(socket) ((socket->direction == CG_NET_SOCKET_SERVER) ? TRUE : FALSE)

#define cg_socket_setaddress(socket, value) cg_string_setvalue(socket->ipaddr, value)
#define cg_socket_setport(socket, value) (socket->port = value)
#define cg_socket_getaddress(socket) cg_string_getvalue(socket->ipaddr)
#define cg_socket_getport(socket) (socket->port)

BOOL cg_socket_isbound(CgSocket *socket);
BOOL cg_socket_close(CgSocket *socket);

BOOL cg_socket_listen(CgSocket *socket);

BOOL cg_socket_bind(CgSocket *sock, int bindPort, char *bindAddr, BOOL bindFlag, BOOL reuseFlag);
BOOL cg_socket_accept(CgSocket *sock, CgSocket *clientSock);
BOOL cg_socket_connect(CgSocket *sock, char *addr, int port);
int cg_socket_read(CgSocket *sock, char *buffer, int bufferLen);
int cg_socket_write(CgSocket *sock, char *buffer, int bufferLen);
int cg_socket_readline(CgSocket *sock, char *buffer, int bufferLen);
long cg_socket_skip(CgSocket *sock, long skipLen);

int cg_socket_sendto(CgSocket *sock, char *addr, int port, char *data, int dataeLen);
int cg_socket_recv(CgSocket *sock, CgDatagramPacket *dgmPkt);

int cg_socket_getlasterror();

/****************************************
* Function (Multicast)
****************************************/

BOOL cg_socket_joingroup(CgSocket *sock, char *mcastAddr, char *ifAddr);

/****************************************
* Function (Option)
****************************************/

BOOL cg_socket_setreuseaddress(CgSocket *socket, BOOL flag);
BOOL cg_socket_setmulticastttl(CgSocket *sock,  int ttl);
BOOL cg_socket_settimeout(CgSocket *sock, int sec);

/****************************************
* Function (DatagramPacket)
****************************************/

CgDatagramPacket *cg_socket_datagram_packet_new();
void cg_socket_datagram_packet_delete(CgDatagramPacket *dgmPkt);

#define cg_socket_datagram_packet_setdata(dgmPkt, value) cg_string_setvalue(dgmPkt->data, value)
#define cg_socket_datagram_packet_getdata(dgmPkt) cg_string_getvalue(dgmPkt->data)

#define cg_socket_datagram_packet_setlocaladdress(dgmPkt, addr) cg_string_setvalue(dgmPkt->localAddress, addr)
#define cg_socket_datagram_packet_getlocaladdress(dgmPkt) cg_string_getvalue(dgmPkt->localAddress)
#define cg_socket_datagram_packet_setlocalport(dgmPkt, port) (dgmPkt->localPort = port)
#define cg_socket_datagram_packet_getlocalport(dgmPkt) (dgmPkt->localPort)
#define cg_socket_datagram_packet_setremoteaddress(dgmPkt, addr) cg_string_setvalue(dgmPkt->remoteAddress, addr)
#define cg_socket_datagram_packet_getremoteaddress(dgmPkt) cg_string_getvalue(dgmPkt->remoteAddress)
#define cg_socket_datagram_packet_setremoteport(dgmPkt, port) (dgmPkt->remotePort = port)
#define cg_socket_datagram_packet_getremoteport(dgmPkt) (dgmPkt->remotePort)

void cg_socket_datagram_packet_copy(CgDatagramPacket *dstDgmPkt, CgDatagramPacket *srcDgmPkt);

/****************************************
* Function (SSLSocket)
****************************************/

#if defined(CG_USE_OPENSSL)
#define CG_NET_SOCKET_SSL 0x0100
#define cg_socket_ssl_new() cg_socket_new(CG_NET_SOCKET_STREAM | CG_NET_SOCKET_SSL)
#define cg_socket_isssl(socket) ((socket->type & CG_NET_SOCKET_SSL) ? TRUE : FALSE)
#endif

/****************************************
* Function (SocketList)
****************************************/

#if defined(CG_NET_USE_SOCKET_LIST)

#define cg_socket_next(sock) (CgSocket *)cg_list_next((CgList *)sock)

CgSocketList *cg_socketlist_new();
void cg_socketlist_delete(CgSocketList *sockList);

#define cg_socketlist_clear(sockList) cg_list_clear((CgList *)sockList, (CG_LIST_DESTRUCTORFUNC)cg_socket_delete)
#define cg_socketlist_size(sockList) cg_list_size((CgList *)sockList)
#define cg_socketlist_gets(sockList) (CgSocket *)cg_list_next((CgList *)sockList)
#define cg_socketlist_add(sockList, sock) cg_list_add((CgList *)sockList, (CgList *)sock)

#endif

#ifdef  __cplusplus
}
#endif

#endif
