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



#ifndef SHA1_H
#define SHA1_H



#define SHA1_MAC_LEN 20

typedef struct SHA1Context
{
    u32 state[5];
    u32 count[2];
    unsigned char buffer[64];
} SHA1_CTX;

void hmac_sha1_vector(const u8 *key, size_t key_len, size_t num_elem,
                      const u8 *addr[], const size_t *len, u8 *mac);
void hmac_sha1(const u8 *key, size_t key_len, const u8 *data, size_t data_len,
               u8 *mac);
void sha1_prf(const u8 *key, size_t key_len, const char *label,
              const u8 *data, size_t data_len, u8 *buf, size_t buf_len);
void sha1_t_prf(const u8 *key, size_t key_len, const char *label,
                const u8 *seed, size_t seed_len, u8 *buf, size_t buf_len);
int __must_check tls_prf(const u8 *secret, size_t secret_len,
                         const char *label, const u8 *seed, size_t seed_len,
                         u8 *out, size_t outlen);
void pbkdf2_sha1(const char *passphrase, const char *ssid, size_t ssid_len,
                 int iterations, u8 *buf, size_t buflen);

#ifdef CONFIG_CRYPTO_INTERNAL
struct SHA1Context;

void SHA1Init(struct SHA1Context *context);
void SHA1Update(struct SHA1Context *context, const void *data, u32 len);
void SHA1Final(unsigned char digest[20], struct SHA1Context *context);
#endif /* CONFIG_CRYPTO_INTERNAL */

#endif /* SHA1_H */
