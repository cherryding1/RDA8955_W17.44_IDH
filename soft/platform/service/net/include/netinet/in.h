#ifndef _NETINET_IN_H
#define _NETINET_IN_H   1

#include <stdint.h>
#include <sys/socket.h>

/* Likewise, for IPv6.  */
struct ipv6_mreq
  {
    /* IPv6 multicast address of group */
    struct in6_addr ipv6mr_multiaddr;

    /* local interface */
    unsigned int ipv6mr_interface;
  };
#define IPV6_MULTICAST_IF  17
#define IPV6_JOIN_GROUP             20
#define IP_ADD_MEMBERSHIP           35

#endif
