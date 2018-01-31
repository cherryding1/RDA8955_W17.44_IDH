#ifdef AT_COAP_SUPPORT

#include "sockets.h"

struct addrinfo {
    int               ai_flags;      /* Input flags. */
    int               ai_family;     /* Address family of socket. */
    int               ai_socktype;   /* Socket type. */
    int               ai_protocol;   /* Protocol of socket. */
    socklen_t         ai_addrlen;    /* Length of socket address. */
    struct sockaddr  *ai_addr;       /* Socket address of socket. */
    char             *ai_canonname;  /* Canonical name of service location. */
    struct addrinfo  *ai_next;       /* Pointer to next in list. */
};


/* Socket protocol types (TCP/UDP/RAW) */
#define SOCK_STREAM     1
#define SOCK_DGRAM      2
#define SOCK_RAW        3

#define AI_PASSIVE          1
#define AI_CANONNAME        2
#define AI_NUMERICHOST      4

#define AF_UNSPEC       0
#define AF_INET         2
#define PF_INET         AF_INET
#define PF_UNSPEC       AF_UNSPEC

// #define stderr 12
#define AT_COAP_ADDRLEN 50

VOID AT_COAP_CmdFunc_START(AT_CMD_PARA *pParam);

#endif
