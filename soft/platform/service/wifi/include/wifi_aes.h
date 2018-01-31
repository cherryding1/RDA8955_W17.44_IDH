/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/



#ifndef WIFI_AES_H
#define WIFI_AES_H

void *aes_encrypt_init(const u8 *key, size_t len);
void aes_encrypt(void *ctx, const u8 *plain, u8 *crypt);
void aes_encrypt_deinit(void *ctx);
void *aes_decrypt_init(const u8 *key, size_t len);
void aes_decrypt(void *ctx, const u8 *crypt, u8 *plain);
void aes_decrypt_deinit(void *ctx);

#endif /* AES_H */
