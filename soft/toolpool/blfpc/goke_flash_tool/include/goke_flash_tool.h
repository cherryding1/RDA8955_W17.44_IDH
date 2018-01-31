#ifndef _GOKE_FLASH_TOOL_H_
#define _GOKE_FLASH_TOOL_H_

#include <stdint.h>
#include <stdbool.h>

typedef void (*BurnProgressCb_t)(uint32_t current, uint32_t total, unsigned long param);

bool gkft_init(uint8_t uart_id, uint32_t img_size, bool need32k);
void gkft_destroy();
bool gkft_fill_data(const void *data, uint32_t size);
bool gkft_burn_image(BurnProgressCb_t cb, unsigned long cbp);
bool gkft_open_gps();
bool gkft_close_gps();
const void *gkft_image_data();

#endif // _GOKE_FLASH_TOOL_H_
