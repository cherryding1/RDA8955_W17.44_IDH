/**
 * @file
 * Application layered TCP/TLS connection API (to be used from TCPIP thread)
 *
 * @defgroup altcp_tls TLS layer
 * @ingroup altcp
 * This file contains function prototypes for a TLS layer.
 * A port to ARM mbedtls is provided in the apps/ tree
 * (LWIP_ALTCP_TLS_MBEDTLS option).
 */

/*
 * Copyright (c) 2017 Simon Goldschmidt
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Simon Goldschmidt <goldsimon@gmx.de>
 *
 */
#ifndef LWIP_HDR_ALTCP_TLS_H
#define LWIP_HDR_ALTCP_TLS_H

#include "lwip/opt.h"

#if LWIP_ALTCP /* don't build if not configured for use in lwipopts.h */
#include "lwip/apps/altcp_tls_mbedtls_opts.h"

#if LWIP_ALTCP_TLS

#include "lwip/altcp.h"
#include "lwip/altcp_tls.h"
#include "lwip/priv/altcp_priv.h"

#include "altcp_tls_mbedtls_structs.h"
#include "altcp_tls_mbedtls_mem.h"

/* @todo: which includes are really needed? */
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/certs.h"
#include "mbedtls/x509.h"
#include "mbedtls/ssl.h"
#include "mbedtls/net.h"
#include "mbedtls/error.h"
#include "mbedtls/debug.h"
#include "mbedtls/platform.h"
#include "mbedtls/memory_buffer_alloc.h"
#include "mbedtls/ssl_cache.h"

#include "mbedtls/ssl_internal.h" /* to call mbedtls_flush_output after ERR_MEM */

#ifdef __cplusplus
extern "C" {
#endif

/** @ingroup altcp_tls
 * ALTCP_TLS configuration handle, content depends on port (e.g. mbedtls) 
 */
struct altcp_tls_config;

/** @ingroup altcp_tls
 * Create an ALTCP_TLS server configuration handle
 */
struct altcp_tls_config *altcp_tls_create_config_server_privkey_cert(const u8_t *privkey, size_t privkey_len,
                            const u8_t *privkey_pass, size_t privkey_pass_len,
                            const u8_t *cert, size_t cert_len);

/** @ingroup altcp_tls
 * Create an ALTCP_TLS client configuration handle
 */
 struct altcp_tls_config *altcp_tls_create_config_client(char *altcp_ca_pem,char *altcp_cert_pem,char *altcp_key_pem);

err_t altcp_mbedtls_verify(altcp_mbedtls_state_t *state);

/** @ingroup altcp_tls
 * Create new ALTCP_TLS layer
 */
struct altcp_pcb *altcp_tls_new(struct altcp_tls_config* config, struct altcp_pcb *inner_pcb);

#if LWIP_ALTCP && LWIP_ALTCP_TLS

static char* cre_ca_pem2 ="-----BEGIN CERTIFICATE-----\n\
MIIC8DCCAlmgAwIBAgIJAOD63PlXjJi8MA0GCSqGSIb3DQEBBQUAMIGQMQswCQYD\n\
VQQGEwJHQjEXMBUGA1UECAwOVW5pdGVkIEtpbmdkb20xDjAMBgNVBAcMBURlcmJ5\n\
MRIwEAYDVQQKDAlNb3NxdWl0dG8xCzAJBgNVBAsMAkNBMRYwFAYDVQQDDA1tb3Nx\n\
dWl0dG8ub3JnMR8wHQYJKoZIhvcNAQkBFhByb2dlckBhdGNob28ub3JnMB4XDTEy\n\
MDYyOTIyMTE1OVoXDTIyMDYyNzIyMTE1OVowgZAxCzAJBgNVBAYTAkdCMRcwFQYD\n\
VQQIDA5Vbml0ZWQgS2luZ2RvbTEOMAwGA1UEBwwFRGVyYnkxEjAQBgNVBAoMCU1v\n\
c3F1aXR0bzELMAkGA1UECwwCQ0ExFjAUBgNVBAMMDW1vc3F1aXR0by5vcmcxHzAd\n\
BgkqhkiG9w0BCQEWEHJvZ2VyQGF0Y2hvby5vcmcwgZ8wDQYJKoZIhvcNAQEBBQAD\n\
gY0AMIGJAoGBAMYkLmX7SqOT/jJCZoQ1NWdCrr/pq47m3xxyXcI+FLEmwbE3R9vM\n\
rE6sRbP2S89pfrCt7iuITXPKycpUcIU0mtcT1OqxGBV2lb6RaOT2gC5pxyGaFJ+h\n\
A+GIbdYKO3JprPxSBoRponZJvDGEZuM3N7p3S/lRoi7G5wG5mvUmaE5RAgMBAAGj\n\
UDBOMB0GA1UdDgQWBBTad2QneVztIPQzRRGj6ZHKqJTv5jAfBgNVHSMEGDAWgBTa\n\
d2QneVztIPQzRRGj6ZHKqJTv5jAMBgNVHRMEBTADAQH/MA0GCSqGSIb3DQEBBQUA\n\
A4GBAAqw1rK4NlRUCUBLhEFUQasjP7xfFqlVbE2cRy0Rs4o3KS0JwzQVBwG85xge\n\
REyPOFdGdhBY2P1FNRy0MDr6xr+D2ZOwxs63dG1nnAnWZg7qwoLgpZ4fESPD3PkA\n\
1ZgKJc2zbSQ9fCPxt2W3mdVav66c6fsb7els2W2Iz7gERJSX\n\
-----END CERTIFICATE-----";

static char* cre_cert_pem2 = "-----BEGIN CERTIFICATE-----\n\
MIIDKDCCApGgAwIBAgIBADANBgkqhkiG9w0BAQUFADCBkDELMAkGA1UEBhMCR0Ix\n\
FzAVBgNVBAgMDlVuaXRlZCBLaW5nZG9tMQ4wDAYDVQQHDAVEZXJieTESMBAGA1UE\n\
CgwJTW9zcXVpdHRvMQswCQYDVQQLDAJDQTEWMBQGA1UEAwwNbW9zcXVpdHRvLm9y\n\
ZzEfMB0GCSqGSIb3DQEJARYQcm9nZXJAYXRjaG9vLm9yZzAeFw0xNzA2MTIwNzU1\n\
MThaFw0xNzA5MTAwNzU1MThaMIGCMQswCQYDVQQGEwJjbjEQMA4GA1UECAwHVGlh\n\
bmppbjEQMA4GA1UEBwwHVGlhbmppbjEMMAoGA1UECgwDUkRBMQ0wCwYDVQQLDART\n\
UFJEMREwDwYDVQQDDAhjcmV0aGRvbTEfMB0GCSqGSIb3DQEJARYQY3JldGhkb21A\n\
MTYzLmNvbTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAOHG08slcUHa\n\
Kscnn/MIivr/ghsNr6M46ufKnwif1loTgr800VTykoiPo1gdILL1Fl9SBHdWsHYP\n\
wtg2H/pcLbXlJbW4SjZdrcHIz1KbkytI616/t6gUALEsfWiDcbLRp+FgWUgvVn2C\n\
uzcFv1EkiC034wme9rdoo6eQuPg5lo4iOwlKBW4QZQSyXo7UQ8hdGO/RJaIqgcQ7\n\
7gm9eUCGAnaP2NskUUjOMR4rUwNrhFbTnO0K8mwbTqn09tiaauT//3xGwTdbJSzW\n\
AtPJSd9umda3TA9RmqIQKkcwRalWdr5Bs01QZCjdVHZ47EhO+ssEQHEIkM4IHcCr\n\
akj4EwitjX0CAwEAAaMaMBgwCQYDVR0TBAIwADALBgNVHQ8EBAMCBeAwDQYJKoZI\n\
hvcNAQEFBQADgYEAX7P6PK3Rb73ErN4oCMcDh/YOG9U8/vwYF5qLV1uTrnXJasJF\n\
a4F8sPI5wGeyIJ/SAxz7SdU7bDXRRfp97khLyixgThJEhDumgbcHWQI4fMTvUO0q\n\
92rcyDHg/hWjatB+3Bls+RTe0unBvlwtZyABvVBQZG6gJ6wI92iOTNUCRYw=\n\
-----END CERTIFICATE-----";

static char* cre_key_pem2 = "-----BEGIN RSA PRIVATE KEY-----\n\
MIIEogIBAAKCAQEA4cbTyyVxQdoqxyef8wiK+v+CGw2vozjq58qfCJ/WWhOCvzTR\n\
VPKSiI+jWB0gsvUWX1IEd1awdg/C2DYf+lwtteUltbhKNl2twcjPUpuTK0jrXr+3\n\
qBQAsSx9aINxstGn4WBZSC9WfYK7NwW/USSILTfjCZ72t2ijp5C4+DmWjiI7CUoF\n\
bhBlBLJejtRDyF0Y79EloiqBxDvuCb15QIYCdo/Y2yRRSM4xHitTA2uEVtOc7Qry\n\
bBtOqfT22Jpq5P//fEbBN1slLNYC08lJ326Z1rdMD1GaohAqRzBFqVZ2vkGzTVBk\n\
KN1UdnjsSE76ywRAcQiQzggdwKtqSPgTCK2NfQIDAQABAoIBADba6wx5fWoFis02\n\
eGtBERtoxwCxs5HRu2kz1q3uv9LE2MiCGbYs9aNOWt6GvTId0CC6ubERE/CrZajI\n\
hb57dPOzyBS9wPIykov5sT2d5QToQ5+SrxmcOorRh2rEPPS4cbSVhzu9nKMEB16X\n\
++LIIHn0ESp+P+1R9Ck7RWWyAD1p6N+h98wuxL8FEqH+Fj2v4IH/dxyrsuv+W+xq\n\
eJazH9MOJBnzo3VOIdwih1s64CbVsqBAmxUWqCADAZv0fRK6ug9uTWKWDEZROsxd\n\
rchFotrGcjzOiSl2+qkdSH/imqWiVk+Dg7J0xFX0+IAU5HoSgejWDtpxvGEUY1Zn\n\
csgImUECgYEA8reCO8kpQJBnfPacUXu5SULcqaIwcdMqKT0tGuUTxLkWMHhVDT+u\n\
C7fnBMG/4Hz/YtJgvKg5bY0FiHEHQ+u2Jg6T0m2hh4XFMWxiJneyDcS1HkN5Y2Vq\n\
mfDzUomX26fLrt6JqzkoTlYhMxVUr3dpGYnJDWB6rj7MvyMuboEsdFECgYEA7iH8\n\
JQ7DZ06wwMeWRMepe8D0qNJJt1QBSnAUnvgsT+/UgisnaVHqF2Au7lOkKG335S8t\n\
BrYsK2E2uXRSlcaJEaYHOrkkb6Y81gIwKtmOvZ071QduVKiSZcdPp2WZXLAIRijP\n\
cvO4EN7WqbtbEjAr+iuoaDunqR2RGsaxhg4Q120CgYBj1tR9XzvW0Q+guZiHdAs9\n\
dYFrTcn7qE75c66wF3bQPwo/md/yeTVQoK8op8ITx5cNwVgFgnVM+OtDuqqwZ68f\n\
bHOl5W7CXmZE9D5TcH0PaNDPyBWQq1D0TcbpNqv+sMD6B5Q9mF9aCdfSOX4CR242\n\
ERVgqUUNk8uI/CHHVt9AYQKBgDVueLYT7vJSLMTEw8nhBv1WmU78CEBTYArExUu6\n\
Ia85//PJHQm7sIvYMA+XwuCzdAj/odm8rIJ7SJ0qJZGnv3+vQjYB7tQCd+oJCafD\n\
swyGqDMvfKkQPfEKXKwR1ux+Lw0XAR0o5GccqQvilxeoqWVzwFDK+d6vXKymYpTS\n\
uSKtAoGAOlTQFtVFjkWGDLc/8Kww611Wd4OyT0Z2jphDFrKH8XUT9QYY9PTUmreO\n\
pjCVSM5JrBuGg/MjzmCtcndtxBUx6WbwcCnQoFTbEJwm9V1OBUn3+fh30GV+wAV+\n\
zYVCGiDJ18zwEfnUNGktElm+3ShYYmt/ym7fpdNjIOIOF2Z8Ln0=\n\
-----END RSA PRIVATE KEY-----";


static char* cre_ca_pem1="-----BEGIN CERTIFICATE-----\n\
MIIDuTCCAqGgAwIBAgIJALcohqWBlcK7MA0GCSqGSIb3DQEBCwUAMHMxCzAJBgNV\n\
BAYTAkNIMQswCQYDVQQIDAJUSjELMAkGA1UEBwwCVEoxDDAKBgNVBAoMA1JEQTEM\n\
MAoGA1UECwwDUkRBMQwwCgYDVQQDDANSREExIDAeBgkqhkiG9w0BCQEWEXRlc3RA\n\
cmRhbWljcm8uY29tMB4XDTE3MDMxNTAzMTAwOVoXDTE4MDMxNTAzMTAwOVowczEL\n\
MAkGA1UEBhMCQ0gxCzAJBgNVBAgMAlRKMQswCQYDVQQHDAJUSjEMMAoGA1UECgwD\n\
UkRBMQwwCgYDVQQLDANSREExDDAKBgNVBAMMA1JEQTEgMB4GCSqGSIb3DQEJARYR\n\
dGVzdEByZGFtaWNyby5jb20wggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIB\n\
AQCuhmzPzbNXN9JyL8kRV5pDrFDtzvKis2lTbOGwYMCLwpYZTOruWxAyB232q3ri\n\
fBvDqTh2kvtsAw9pfvdjsvGzXFJzh+hQaPpDOY6eSMEkHUqcXUx8WeS4Yl5O4CPv\n\
UwhCja4Xl2Z9QVTAvXhJGJbwl0i1tfQVHm3R5F+S6ZiqsIA81QF6ZFTPjqMfevDJ\n\
oGnM3K21QgbplfNRfesDL7rznecokd9VOsuuOpR+CiwgjctvjfCTFbFOwmR7DIs/\n\
Q5Qp69DaEW4HOAILzu8rD1Gy77Yuk/0GZg2Zv0cPFPrzHZj4QhGJpnjF7w+8p+1E\n\
kcIuy1/bCY529qmbgYsAMu3lAgMBAAGjUDBOMB0GA1UdDgQWBBRSnbbcqSGw98ou\n\
X1cvE9F+fuNSuzAfBgNVHSMEGDAWgBRSnbbcqSGw98ouX1cvE9F+fuNSuzAMBgNV\n\
HRMEBTADAQH/MA0GCSqGSIb3DQEBCwUAA4IBAQCZ1fudBTKF120LDSdMsduGZvaJ\n\
QbkHeN39IzlFtYzgFmB49In+EaUvgG4nUyjH1bUXcGthbz07KDVBs4MrJdXHY+/C\n\
en77/lEJP1Xq9Tz/yBZMEAcFcltVItCwsCevuGCAURDU9EOqj2SxrufpA7gxT/QO\n\
ALALsM8mT98Lfx0ix4OZfOnzlIIhOy8hTXVcvC4Yk29TQOSoqcBfzIR7rr/PH5Tr\n\
lghC1l11VmruTT2aPFbJ2I1hR/UXIeEo6PkBHl6r2i4jPtL9TcIN/wOqSLzYr/B4\n\
miH8oswCfpFFxiyeSOw2hiRGykzC/t12Qx/1m2icAd81dNyiy0m6FUsCwpbo\n\
-----END CERTIFICATE-----";

static char* cre_cert_pem1="Certificate:\n\
    Data:\n\
        Version: 3 (0x2)\n\
        Serial Number: 5 (0x5)\n\
    Signature Algorithm: sha256WithRSAEncryption\n\
        Issuer: C=CH, ST=TJ, L=TJ, O=RDA, OU=RDA, CN=RDA/emailAddress=test@rdamicro.com\n\
        Validity\n\
            Not Before: Mar 15 03:11:42 2017 GMT\n\
            Not After : Mar 15 03:11:42 2018 GMT\n\
        Subject: C=CH, ST=TJ, O=RDA, OU=RDA, CN=RDA/emailAddress=test@rdamicro.com\n\
        Subject Public Key Info:\n\
            Public Key Algorithm: rsaEncryption\n\
                Public-Key: (1024 bit)\n\
                Modulus:\n\
                    00:ba:66:09:09:01:b8:8f:7e:16:77:4d:07:7c:d7:\n\
                    4b:68:ba:76:e1:4d:cd:b8:a9:b0:b6:f6:77:83:d4:\n\
                    40:68:38:eb:de:1e:e2:96:a5:ec:ba:bb:07:82:79:\n\
                    9f:c8:52:04:0a:74:22:57:b6:f8:54:84:a0:ea:be:\n\
                    9a:45:19:c8:8a:3d:95:31:30:0a:ef:ae:d8:37:fd:\n\
                    e1:ac:b9:03:2f:e9:96:23:c8:1f:78:fb:be:d8:7a:\n\
                    c1:a3:5c:a3:35:da:de:b0:e3:01:88:c0:dd:93:8c:\n\
                    33:2e:06:d5:4e:8c:59:e5:a1:d9:e2:ba:ce:c1:ab:\n\
                    0c:2e:46:0c:ca:26:90:1b:e3\n\
                Exponent: 65537 (0x10001)\n\
        X509v3 extensions:\n\
            X509v3 Basic Constraints:\n\ 
                CA:FALSE\n\
            Netscape Comment: \n\
                OpenSSL Generated Certificate\n\
            X509v3 Subject Key Identifier: \n\
                46:34:FD:9D:CF:0C:BD:80:29:07:C4:C3:97:E9:21:8B:FE:E7:BE:91\n\
            X509v3 Authority Key Identifier:\n\ 
                keyid:52:9D:B6:DC:A9:21:B0:F7:CA:2E:5F:57:2F:13:D1:7E:7E:E3:52:BB\n\
	\n\
    Signature Algorithm: sha256WithRSAEncryption\n\
         64:93:70:fe:79:ff:9b:1a:0d:1f:49:fb:a4:ce:db:c6:21:97:\n\
         e5:42:54:0e:3a:9a:b3:68:b7:30:57:8c:ad:33:97:68:65:4d:\n\
         18:94:cb:bd:b3:e4:ad:47:42:eb:17:15:7f:a6:d2:f0:aa:3d:\n\
         59:f1:bc:48:5d:f2:56:1a:21:a0:3e:99:f6:15:b5:c1:8f:d4:\n\
         57:2f:c4:7f:57:b1:74:2c:d4:7a:7d:e6:c0:7c:7c:b3:71:2b:\n\
         ab:9b:d0:a2:dc:0d:7f:c4:07:d9:2d:07:d0:90:5c:3d:80:75:\n\
         45:c8:3b:fe:b6:55:ef:e7:09:ce:4b:e9:e8:17:99:b2:fb:b2:\n\
         b8:72:1e:00:5e:89:18:6a:b1:fe:ca:b0:f1:e4:3a:1c:65:fa:\n\
         53:07:e0:82:ff:73:70:36:db:fb:58:13:b8:f5:17:07:55:6e:\n\
         d8:1c:fa:ff:3a:2a:7b:49:32:8a:2d:d1:41:3f:83:52:a2:30:\n\
         05:f3:1d:34:ac:47:d7:81:16:46:42:d0:92:dd:a7:e5:c0:67:\n\
         47:42:a5:f8:4e:95:83:f3:f6:d4:9b:02:18:28:e0:80:e6:99:\n\
         57:d6:4e:44:a7:9c:e8:29:c3:5e:ac:3b:da:7c:a2:ff:a5:74:\n\
         1a:c7:19:df:2b:5f:dc:37:2d:84:5e:13:22:9a:0e:c0:08:b3:\n\
         5b:64:09:72\n\
-----BEGIN CERTIFICATE-----\n\
MIIDSzCCAjOgAwIBAgIBBTANBgkqhkiG9w0BAQsFADBzMQswCQYDVQQGEwJDSDEL\n\
MAkGA1UECAwCVEoxCzAJBgNVBAcMAlRKMQwwCgYDVQQKDANSREExDDAKBgNVBAsM\n\
A1JEQTEMMAoGA1UEAwwDUkRBMSAwHgYJKoZIhvcNAQkBFhF0ZXN0QHJkYW1pY3Jv\n\
LmNvbTAeFw0xNzAzMTUwMzExNDJaFw0xODAzMTUwMzExNDJaMGYxCzAJBgNVBAYT\n\
AkNIMQswCQYDVQQIDAJUSjEMMAoGA1UECgwDUkRBMQwwCgYDVQQLDANSREExDDAK\n\
BgNVBAMMA1JEQTEgMB4GCSqGSIb3DQEJARYRdGVzdEByZGFtaWNyby5jb20wgZ8w\n\
DQYJKoZIhvcNAQEBBQADgY0AMIGJAoGBALpmCQkBuI9+FndNB3zXS2i6duFNzbip\n\
sLb2d4PUQGg4694e4pal7Lq7B4J5n8hSBAp0Ile2+FSEoOq+mkUZyIo9lTEwCu+u\n\
2Df94ay5Ay/pliPIH3j7vth6waNcozXa3rDjAYjA3ZOMMy4G1U6MWeWh2eK6zsGr\n\
DC5GDMomkBvjAgMBAAGjezB5MAkGA1UdEwQCMAAwLAYJYIZIAYb4QgENBB8WHU9w\n\
ZW5TU0wgR2VuZXJhdGVkIENlcnRpZmljYXRlMB0GA1UdDgQWBBRGNP2dzwy9gCkH\n\
xMOX6SGL/ue+kTAfBgNVHSMEGDAWgBRSnbbcqSGw98ouX1cvE9F+fuNSuzANBgkq\n\
hkiG9w0BAQsFAAOCAQEAZJNw/nn/mxoNH0n7pM7bxiGX5UJUDjqas2i3MFeMrTOX\n\
aGVNGJTLvbPkrUdC6xcVf6bS8Ko9WfG8SF3yVhohoD6Z9hW1wY/UVy/Ef1exdCzU\n\
en3mwHx8s3Erq5vQotwNf8QH2S0H0JBcPYB1Rcg7/rZV7+cJzkvp6BeZsvuyuHIe\n\
AF6JGGqx/sqw8eQ6HGX6Uwfggv9zcDbb+1gTuPUXB1Vu2Bz6/zoqe0kyii3RQT+D\n\
UqIwBfMdNKxH14EWRkLQkt2n5cBnR0Kl+E6Vg/P21JsCGCjggOaZV9ZORKec6CnD\n\
Xqw72nyi/6V0GscZ3ytf3DcthF4TIpoOwAizW2QJcg==\n\
-----END CERTIFICATE-----";

static char* cre_key_pem1="-----BEGIN RSA PRIVATE KEY-----\n\
MIICXQIBAAKBgQC6ZgkJAbiPfhZ3TQd810tounbhTc24qbC29neD1EBoOOveHuKW\n\
pey6uweCeZ/IUgQKdCJXtvhUhKDqvppFGciKPZUxMArvrtg3/eGsuQMv6ZYjyB94\n\
+77YesGjXKM12t6w4wGIwN2TjDMuBtVOjFnlodnius7BqwwuRgzKJpAb4wIDAQAB\n\
AoGAYmCJ8jv7SZh1iP+ZrRUAEsUfCyiCJC1DPD12tJgoahlou3G3i2UFPUSK4R0J\n\
7LMEuPO0UHEcPZdGSr7QMxo5kX1hjLVQc9gVBh8pbP636iFIaET9JUnaqRCPb438\n\
b2KFX1Khpj9M9UESxuKpZZrMhi5LixPSxmWipzSNAK5ZDlkCQQDqzvfK1JpoRFO3\n\
iAmUY/XW3/RomjtNJ8WRQ3+G9Hq75/0HcJyr8Cu7qlwdiEPPNiJI8dvUdBTBa+6U\n\
qoqNY9dXAkEAyziaBNy9jfpf42J7BSU2OvwoxpYixXAhHukuwXhOpFXFqFPsJC4B\n\
p9eqTABP0BBIhLOHCps6H9jF3iE/2O3EVQJBAKyU7Im3E7o5JjMKGMhpZCLHvpM3\n\
cQi3h73KF6xwetkrnM1gFrdFMk+f2k6WfgiJ6E8m/QU9QJgdpWC9rFJZpMcCQQDJ\n\
HAKPvUN5NcWe6N6FgQXh6ZkQrZNLgGPGi0zR+ZUZ3Cf3JNYUF9NbnnnRLJ3Pcc3S\n\
RTfmryceTQ/De3QL4YDhAkBSxUJ0CLAdwbGt2ePCfnBKvaz3kbiNxCl+Lp0rICZ/\n\
m/Di9hQf7K6+bNn+OMvR6c52PXahzjfK8RfufcQNEYZG\n\
-----END RSA PRIVATE KEY-----";

#endif



#ifdef __cplusplus
}
#endif

#endif /* LWIP_ALTCP_TLS */
#endif /* LWIP_ALTCP */
#endif /* LWIP_HDR_ALTCP_TLS_H */
