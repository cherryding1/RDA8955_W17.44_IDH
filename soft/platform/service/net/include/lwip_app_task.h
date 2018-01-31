
typedef enum app_type_t
{
    LWIP_APP_NONE,
    LWIP_APP_COAP,
    LWIP_APP_LWM2M,
    LWIP_APP_MIPL_LWM2M,
    LWIP_APP_COUNT,
} APP_TYPE_T;

void start_lwip_app_task();

