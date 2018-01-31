/*
 * aliyun_iot_common_net.h
 *
 *  Created on: May 5, 2017
 *      Author: qibiao.wqb
 */

#ifndef ALIYUN_IOT_COMMON_NET_H
#define ALIYUN_IOT_COMMON_NET_H

#include "aliyun_iot_network_ssl.h"
#include "cs_types.h"
#include "stdint.h"

typedef struct {
    char *pHostAddress;     ///< Pointer to a host name string
    char *pHostPort;        ///< destination port
    char *pPubKey;          ///< Pointer to a ca string
}ConnectParams;

#ifndef intptr_t
//typedef unsigned int   intptr_t;
#endif

/**
 * @brief The structure of network connection(TCP or SSL).
 *   The user has to allocate memory for this structure.
 */

typedef struct Network_t Network_t;

struct Network_t
{
    char *port; //todo to integer.
    char *pHostAddress;
    char *ca_crt;       //TCP connection when the value is NULL; SSL connection when the value is NOT NULL
    intptr_t handle;    //connection handle

    int (*aliyun_read)(Network_t *, char *, uint32_t, uint32_t);      /**< Read data from server function pointer. */
    int (*aliyun_write)(Network_t *, char *, uint32_t, uint32_t);     /**< Send data to server function pointer. */
    int (*aliyun_disconnect)(Network_t *);                  /**< Disconnect the network function pointer.此函数close socket后需要初始化为-1，如果为-1则不再执行close操作*/
    intptr_t (*aliyun_connect)(Network_t *);
};


int aliyun_iot_net_read(Network_t * pNetwork, char *buffer, uint32_t len, uint32_t timeout_ms);
int aliyun_iot_net_write(Network_t * pNetwork, char *buffer, uint32_t len, uint32_t timeout_ms);
int aliyun_iot_net_disconnect(Network_t * pNetwork);
intptr_t aliyun_iot_net_connect(Network_t * pNetwork);
int aliyun_iot_net_init(Network_t * pNetwork, char *host, char *port, char *ca_crt);

#endif /* ALIYUN_IOT_COMMON_NET_H */
