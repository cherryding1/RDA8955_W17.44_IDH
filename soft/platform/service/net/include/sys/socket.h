#ifndef __SYS_SOCKET_H__
#define __SYS_SOCKET_H__
#include "sockets.h"
#define IP_PKTINFO 8
#define IPV6_PKTINFO 50
#define IPV6_MULTICAST_HOPS 18

#define SOMAXCONN  0xff

struct in6_pktinfo {
  struct in6_addr ipi6_addr;	/* src/dst IPv6 address */
  unsigned int ipi6_ifindex;	/* send/recv interface index */
};

#define IN6_IS_ADDR_MULTICAST(a) (((const uint8_t *) (a))[0] == 0xff)


#endif
