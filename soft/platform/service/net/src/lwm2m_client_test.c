#ifdef LWIP_LIBWM2M_TEST

#include "lwip/ip_addr.h"

#define LIBWM2M_THREAD_STACKSIZE        4096
#define LIBWM2M_THREAD_PRIO             (TCPIP_THREAD_PRIO + 3)

static HANDLE libwm2m_handle;
static int g_lwm2client_on;

static void test_lwm2m(void *arg)
{
    if (g_lwm2client_on)
    {
        g_lwm2client_on = 1;
        return;
    }
    g_lwm2client_on = 1;

    struct netif *libwm2m_if = NULL;
    UINT8 nCID = startGprsLink("CMNET",0);
    while (libwm2m_if = getGprsNetIf(0, nCID) == NULL) {
        sys_arch_printf("test_lwm2m() - waiting link up ...");
        COS_Sleep(1000);
    }
    sys_arch_printf("test_lwm2m() - gprs link up success");

#if (defined WITH_TINYDTLS) || (defined WITH_MBEDDTLS)
    char *argv[] = { "lwm2mclient", "-n", "RDA_lwm2m", "-h", "leshan.eclipse.org", "-i", "rdaidentity", "-s", "0123456789abcdef", "-p", "5684", "-4" };
#else
    char *argv[] = { "lwm2mclient", "-n", "RDA_lwm2m_test", "-h", "leshan.eclipse.org", "-4" };
#endif
    lwm2mclient_main(sizeof(argv)/sizeof(argv[0]), argv);
}

void start_libwm2m_test()
{
    libwm2m_handle = sys_thread_new("liblwm2m_thread", test_lwm2m, NULL, LIBWM2M_THREAD_STACKSIZE, LIBWM2M_THREAD_PRIO);
}

#endif

