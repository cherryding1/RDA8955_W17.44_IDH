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



#ifndef WIFI_AES_WRAP_H
#define WIFI_AES_WRAP_H

int __must_check aes_wrap(const u8 *kek, int n, const u8 *plain, u8 *cipher);
int __must_check aes_unwrap(const u8 *kek, int n, const u8 *cipher, u8 *plain);
int __must_check omac1_aes_128_vector(const u8 *key, size_t num_elem,
                                      const u8 *addr[], const size_t *len,
                                      u8 *mac);
int __must_check omac1_aes_128(const u8 *key, const u8 *data, size_t data_len,
                               u8 *mac);
int __must_check aes_128_encrypt_block(const u8 *key, const u8 *in, u8 *out);
int __must_check aes_128_ctr_encrypt(const u8 *key, const u8 *nonce,
                                     u8 *data, size_t data_len);
int __must_check aes_128_eax_encrypt(const u8 *key,
                                     const u8 *nonce, size_t nonce_len,
                                     const u8 *hdr, size_t hdr_len,
                                     u8 *data, size_t data_len, u8 *tag);
int __must_check aes_128_eax_decrypt(const u8 *key,
                                     const u8 *nonce, size_t nonce_len,
                                     const u8 *hdr, size_t hdr_len,
                                     u8 *data, size_t data_len, const u8 *tag);
int __must_check aes_128_cbc_encrypt(const u8 *key, const u8 *iv, u8 *data,
                                     size_t data_len);
int __must_check aes_128_cbc_decrypt(const u8 *key, const u8 *iv, u8 *data,
                                     size_t data_len);

#endif /* AES_WRAP_H */
