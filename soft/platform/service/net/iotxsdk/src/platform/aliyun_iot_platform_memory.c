#include "platform/aliyun_iot_platform_memory.h"
#include "lwip/opt.h"

void *aliyun_iot_memory_malloc(uint32_t size)
{
    return mem_malloc(size);
}

void aliyun_iot_memory_free(void *ptr)
{
    mem_free(ptr);
    return;
}
