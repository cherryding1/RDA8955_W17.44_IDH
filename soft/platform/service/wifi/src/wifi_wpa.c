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


#include "wifi_os.h"
#include "wifi_common.h"
#include "wifi_wpa_common.h"
#include "wifi_eapol_common.h"
#include "ts.h"
#include "wifi_defs.h"
#include"wifi_ieee802_11_defs.h"
#include "wifi_peerkey.h"
#include "wifi_sha1.h"
#include "sxs_io.h"
#include "sockets.h"

struct wpa_gtk_data
{
    wpa_alg alg;
    int tx, key_rsc_len, keyidx;
    u8 gtk[32];
    int gtk_len;
};

int  test_rda5890_set_StrWid(unsigned short wid, unsigned char *val, unsigned char len);
static int ieee80211w_set_keys(WPA_SM_T *sm,  struct wpa_eapol_ie_parse *ie);

static int wpa_parse_generic(const u8 *pos, const u8 *end,
                             struct wpa_eapol_ie_parse *ie)
{
    if (pos[1] == 0)
        return 1;

    if (pos[1] >= 6 &&
            RSN_SELECTOR_GET(pos + 2) == WPA_OUI_TYPE &&
            pos[2 + WPA_SELECTOR_LEN] == 1 &&
            pos[2 + WPA_SELECTOR_LEN + 1] == 0)
    {
        ie->wpa_ie = pos;
        ie->wpa_ie_len = pos[1] + 2;
        return 0;
    }

    if (pos + 1 + RSN_SELECTOR_LEN < end &&
            pos[1] >= RSN_SELECTOR_LEN + PMKID_LEN &&
            RSN_SELECTOR_GET(pos + 2) == RSN_KEY_DATA_PMKID)
    {
        ie->pmkid = pos + 2 + RSN_SELECTOR_LEN;
        return 0;
    }

    if (pos[1] > RSN_SELECTOR_LEN + 2 &&
            RSN_SELECTOR_GET(pos + 2) == RSN_KEY_DATA_GROUPKEY)
    {
        ie->gtk = pos + 2 + RSN_SELECTOR_LEN;
        ie->gtk_len = pos[1] - RSN_SELECTOR_LEN;
        return 0;
    }

    if (pos[1] > RSN_SELECTOR_LEN + 2 &&
            RSN_SELECTOR_GET(pos + 2) == RSN_KEY_DATA_MAC_ADDR)
    {
        ie->mac_addr = pos + 2 + RSN_SELECTOR_LEN;
        ie->mac_addr_len = pos[1] - RSN_SELECTOR_LEN;
        return 0;
    }

#ifdef CONFIG_PEERKEY
    if (pos[1] > RSN_SELECTOR_LEN + 2 &&
            RSN_SELECTOR_GET(pos + 2) == RSN_KEY_DATA_SMK)
    {
        ie->smk = pos + 2 + RSN_SELECTOR_LEN;
        ie->smk_len = pos[1] - RSN_SELECTOR_LEN;
        return 0;
    }

    if (pos[1] > RSN_SELECTOR_LEN + 2 &&
            RSN_SELECTOR_GET(pos + 2) == RSN_KEY_DATA_NONCE)
    {
        ie->nonce = pos + 2 + RSN_SELECTOR_LEN;
        ie->nonce_len = pos[1] - RSN_SELECTOR_LEN;
        return 0;
    }

    if (pos[1] > RSN_SELECTOR_LEN + 2 &&
            RSN_SELECTOR_GET(pos + 2) == RSN_KEY_DATA_LIFETIME)
    {
        ie->lifetime = pos + 2 + RSN_SELECTOR_LEN;
        ie->lifetime_len = pos[1] - RSN_SELECTOR_LEN;
        return 0;
    }

    if (pos[1] > RSN_SELECTOR_LEN + 2 &&
            RSN_SELECTOR_GET(pos + 2) == RSN_KEY_DATA_ERROR)
    {
        ie->error = pos + 2 + RSN_SELECTOR_LEN;
        ie->error_len = pos[1] - RSN_SELECTOR_LEN;
        return 0;
    }
#endif /* CONFIG_PEERKEY */

#ifdef CONFIG_IEEE80211W
    if (pos[1] > RSN_SELECTOR_LEN + 2 &&
            RSN_SELECTOR_GET(pos + 2) == RSN_KEY_DATA_IGTK)
    {
        ie->igtk = pos + 2 + RSN_SELECTOR_LEN;
        ie->igtk_len = pos[1] - RSN_SELECTOR_LEN;
        return 0;
    }
#endif /* CONFIG_IEEE80211W */

    return 0;
}


/**
 * wpa_supplicant_parse_ies - Parse EAPOL-Key Key Data IEs
 * @buf: Pointer to the Key Data buffer
 * @len: Key Data Length
 * @ie: Pointer to parsed IE data
 * Returns: 0 on success, -1 on failure
 */
int wpa_supplicant_parse_ies(const u8 *buf, size_t len,
                             struct wpa_eapol_ie_parse *ie)
{
    const u8 *pos, *end;
    int ret = 0;

    os_memset(ie, 0, sizeof(*ie));
    for (pos = buf, end = pos + len; pos + 1 < end; pos += 2 + pos[1])
    {
        if (pos[0] == 0xdd &&
                ((pos == buf + len - 1) || pos[1] == 0))
        {
            /* Ignore padding */
            break;
        }
        if (pos + 2 + pos[1] > end)
        {
            CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(3),  TSTR("WPA: EAPOL-Key Key Data underflow (ie=%d len=%d pos=%d)",0x08700022),
                      pos[0], pos[1], (int) (pos - buf));
            //wpa_hexdump_key(MSG_DEBUG, "WPA: Key Data",
            //      buf, len);
            ret = -1;
            break;
        }
        if (*pos == WLAN_EID_RSN)
        {
            ie->rsn_ie = pos;
            ie->rsn_ie_len = pos[1] + 2;
#ifdef CONFIG_IEEE80211R
        }
        else if (*pos == WLAN_EID_MOBILITY_DOMAIN)
        {
            ie->mdie = pos;
            ie->mdie_len = pos[1] + 2;
#endif /* CONFIG_IEEE80211R */
        }
        else if (*pos == WLAN_EID_VENDOR_SPECIFIC)
        {
            ret = wpa_parse_generic(pos, end, ie);
            if (ret < 0)
                break;
            if (ret > 0)
            {
                ret = 0;
                break;
            }
        }
        else
        {
            //wpa_hexdump(MSG_DEBUG, "WPA: Unrecognized EAPOL-Key "
            //      "Key Data IE", pos, 2 + pos[1]);
        }
    }

    return ret;
}
#define MD5_MAC_LEN 16
int wpa_eapol_key_mic(const u8 *key, int ver, const u8 *buf, size_t len,
                      u8 *mic)
{
    u8 hash[SHA1_MAC_LEN];
    CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(1),  TSTR("wpa_eapol_key_mic: ver: 0x%x",0x08700023), ver);

    switch (ver)
    {
        case WPA_KEY_INFO_TYPE_HMAC_MD5_RC4:
            hmac_md5(key, 16, buf, len, mic);
            break;
        case WPA_KEY_INFO_TYPE_HMAC_SHA1_AES:
            hmac_sha1(key, 16, buf, len, hash);
            os_memcpy(mic, hash, MD5_MAC_LEN);
            break;
#if defined(CONFIG_IEEE80211R) || defined(CONFIG_IEEE80211W)
        case WPA_KEY_INFO_TYPE_AES_128_CMAC:
            return omac1_aes_128(key, buf, len, mic);
#endif /* CONFIG_IEEE80211R || CONFIG_IEEE80211W */
        default:
            return -1;
    }

    return 0;
}

int wpa_supplicant_verify_eapol_key_mic(WPA_SM_T   *sm,   struct wpa_eapol_key *key,  u16 ver, const u8 *buf, size_t len)
{
    u8 mic[16];
    int ok = 0;

    os_memcpy(mic, key->key_mic, 16);
    if (sm->tptk_set)
    {
        os_memset(key->key_mic, 0, 16);
        wpa_eapol_key_mic(sm->tptk.kck, ver, buf, len,
                          key->key_mic);
        if (os_memcmp(mic, key->key_mic, 16) != 0)
        {
            CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0), TSTR("WPA: Invalid EAPOL-Key MIC when using TPTK - ignoring TPTK",0x08700024));
        }
        else
        {
            ok = 1;
            sm->tptk_set = 0;
            sm->ptk_set = 1;
            os_memcpy(&sm->ptk, &sm->tptk, sizeof(sm->ptk));
        }
    }

    if (!ok && sm->ptk_set)
    {
        os_memset(key->key_mic, 0, 16);
        wpa_eapol_key_mic(sm->ptk.kck, ver, buf, len,
                          key->key_mic);
        if (os_memcmp(mic, key->key_mic, 16) != 0)
        {
            CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0),  TSTR("WPA: Invalid EAPOL-Key MIC - dropping packet",0x08700025));
            return -1;
        }
        ok = 1;
    }

    if (!ok)
    {
        CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0), TSTR("WPA: Could not verify EAPOL-Key MIC - dropping packet",0x08700026));
        return -1;
    }

    os_memcpy(sm->rx_replay_counter, key->replay_counter,
              WPA_REPLAY_COUNTER_LEN);
    sm->rx_replay_counter_set = 1;
    return 0;
}

void wpa_pmk_to_ptk(const u8 *pmk, size_t pmk_len, const char *label,
                    const u8 *addr1, const u8 *addr2,
                    const u8 *nonce1, const u8 *nonce2,
                    u8 *ptk, size_t ptk_len, int use_sha256)
{
    u8 data[2 * ETH_ALEN + 2 * WPA_NONCE_LEN];

    if (os_memcmp(addr1, addr2, ETH_ALEN) < 0)
    {
        os_memcpy(data, addr1, ETH_ALEN);
        os_memcpy(data + ETH_ALEN, addr2, ETH_ALEN);
    }
    else
    {
        os_memcpy(data, addr2, ETH_ALEN);
        os_memcpy(data + ETH_ALEN, addr1, ETH_ALEN);
    }

    if (os_memcmp(nonce1, nonce2, WPA_NONCE_LEN) < 0)
    {
        os_memcpy(data + 2 * ETH_ALEN, nonce1, WPA_NONCE_LEN);
        os_memcpy(data + 2 * ETH_ALEN + WPA_NONCE_LEN, nonce2,
                  WPA_NONCE_LEN);
    }
    else
    {
        os_memcpy(data + 2 * ETH_ALEN, nonce2, WPA_NONCE_LEN);
        os_memcpy(data + 2 * ETH_ALEN + WPA_NONCE_LEN, nonce1,
                  WPA_NONCE_LEN);
    }

#ifdef CONFIG_IEEE80211W
    if (use_sha256)
        sha256_prf(pmk, pmk_len, label, data, sizeof(data),
                   ptk, ptk_len);
    else
#endif /* CONFIG_IEEE80211W */
        sha1_prf(pmk, pmk_len, label, data, sizeof(data), ptk,
                 ptk_len);

    //  wpa_printf(MSG_DEBUG, "WPA: PTK derivation - A1=" MACSTR " A2=" MACSTR,
    //         MAC2STR(addr1), MAC2STR(addr2));
    //  wpa_hexdump_key(MSG_DEBUG, "WPA: PMK", pmk, pmk_len);
    //  wpa_hexdump_key(MSG_DEBUG, "WPA: PTK", ptk, ptk_len);
}

static int wpa_derive_ptk( WPA_SM_T *sm, const unsigned char *src_addr,
                           const struct wpa_eapol_key *key,
                           struct wpa_ptk *ptk)
{
    size_t ptk_len = (sm->pairwise_cipher == WPA_CIPHER_CCMP ? 48 : 64);
#ifdef CONFIG_IEEE80211R
    if (wpa_key_mgmt_ft(sm->key_mgmt))
        return wpa_derive_ptk_ft(sm, src_addr, key, ptk, ptk_len);
#endif /* CONFIG_IEEE80211R */
    CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0), TSTR("wpa_derive_ptk:",0x08700027));
    CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(1), TSTR("sm->pmk len %d, data:",0x08700028), sm->pmk_len);
    wifi_dump(sm->pmk, sm->pmk_len);
    CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0), TSTR("sm->own_addr:",0x08700029));
    wifi_dump(sm->own_addr, 6);
    CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0), TSTR(" sm->bssid:",0x0870002a));
    wifi_dump( sm->bssid, 6);
    CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0), TSTR(" sm->snonce:",0x0870002b));
    wifi_dump( sm->snonce, 32);
    CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0), TSTR(" key->key_nonce:",0x0870002c));
    wifi_dump(key->key_nonce, 32);

    CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(2), TSTR(" ptk addr: 0x%x;  len:%d ",0x0870002d), ptk, ptk_len);




    wpa_pmk_to_ptk(sm->pmk, sm->pmk_len, "Pairwise key expansion",
                   sm->own_addr, sm->bssid, sm->snonce, key->key_nonce,
                   (u8 *) ptk, ptk_len,
                   wpa_key_mgmt_sha256(sm->key_mgmt));
    return 0;
}

static int wpa_supplicant_get_pmk( WPA_SM_T *sm,
                                   const unsigned char *src_addr,
                                   const u8 *pmkid)
{
#if 0
    int abort_cached = 0;

    if (pmkid && !sm->cur_pmksa)
    {
        /* When using drivers that generate RSN IE, wpa_supplicant may
         * not have enough time to get the association information
         * event before receiving this 1/4 message, so try to find a
         * matching PMKSA cache entry here. */
        sm->cur_pmksa = pmksa_cache_get(sm->pmksa, src_addr, pmkid);
        if (sm->cur_pmksa)
        {
            CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0), TSTR("RSN: found matching PMKID from PMKSA cache",0x0870002e));
        }
        else
        {
            CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0), TSTR("RSN: no matching PMKID found",0x0870002f));
            abort_cached = 1;
        }
    }

    if (pmkid && sm->cur_pmksa &&
            os_memcmp(pmkid, sm->cur_pmksa->pmkid, PMKID_LEN) == 0)
    {
        //  wpa_hexdump(MSG_DEBUG, "RSN: matched PMKID", pmkid, PMKID_LEN);
        wpa_sm_set_pmk_from_pmksa(sm);
        //wpa_hexdump_key(MSG_DEBUG, "RSN: PMK from PMKSA cache",
        //          sm->pmk, sm->pmk_len);
        eapol_sm_notify_cached(sm->eapol);
#ifdef CONFIG_IEEE80211R
        sm->xxkey_len = 0;
#endif /* CONFIG_IEEE80211R */
    }
    else if (wpa_key_mgmt_wpa_ieee8021x(sm->key_mgmt) && sm->eapol)
    {
        int res, pmk_len;
        pmk_len = PMK_LEN;
        res = eapol_sm_get_key(sm->eapol, sm->pmk, PMK_LEN);
        if (res)
        {
            /*
             * EAP-LEAP is an exception from other EAP methods: it
             * uses only 16-byte PMK.
             */
            res = eapol_sm_get_key(sm->eapol, sm->pmk, 16);
            pmk_len = 16;
        }
        else
        {
#ifdef CONFIG_IEEE80211R
            u8 buf[2 * PMK_LEN];
            if (eapol_sm_get_key(sm->eapol, buf, 2 * PMK_LEN) == 0)
            {
                os_memcpy(sm->xxkey, buf + PMK_LEN, PMK_LEN);
                sm->xxkey_len = PMK_LEN;
                os_memset(buf, 0, sizeof(buf));
            }
#endif /* CONFIG_IEEE80211R */
        }
        if (res == 0)
        {
            wpa_hexdump_key(MSG_DEBUG, "WPA: PMK from EAPOL state "
                            "machines", sm->pmk, pmk_len);
            sm->pmk_len = pmk_len;
            pmksa_cache_add(sm->pmksa, sm->pmk, pmk_len, src_addr,
                            sm->own_addr, sm->network_ctx,
                            sm->key_mgmt);
            if (!sm->cur_pmksa && pmkid &&
                    pmksa_cache_get(sm->pmksa, src_addr, pmkid))
            {
                wpa_printf(MSG_DEBUG, "RSN: the new PMK "
                           "matches with the PMKID");
                abort_cached = 0;
            }
        }
        else
        {
            wpa_msg(sm->ctx->ctx, MSG_WARNING,
                    "WPA: Failed to get master session key from "
                    "EAPOL state machines");
            wpa_msg(sm->ctx->ctx, MSG_WARNING,
                    "WPA: Key handshake aborted");
            if (sm->cur_pmksa)
            {
                wpa_printf(MSG_DEBUG, "RSN: Cancelled PMKSA "
                           "caching attempt");
                sm->cur_pmksa = NULL;
                abort_cached = 1;
            }
            else if (!abort_cached)
            {
                return -1;
            }
        }
    }

    if (abort_cached && wpa_key_mgmt_wpa_ieee8021x(sm->key_mgmt))
    {
        /* Send EAPOL-Start to trigger full EAP authentication. */
        u8 *buf;
        size_t buflen;

        wpa_printf(MSG_DEBUG, "RSN: no PMKSA entry found - trigger "
                   "full EAP authentication");
        buf = wpa_sm_alloc_eapol(sm, IEEE802_1X_TYPE_EAPOL_START,
                                 NULL, 0, &buflen, NULL);
        if (buf)
        {
            wpa_sm_ether_send(sm, sm->bssid, ETH_P_EAPOL,
                              buf, buflen);
            os_free(buf);
        }

        return -1;
    }
#endif
    return 0;
}
UINT8 random_buf[128];
int rand();
void srand(unsigned int init);

void init_randomBuf(void)
{
    UINT8 *tmp;
    tmp = random_buf;
    UINT8 t = 0;
    for(t = 0; t < 128; t++)
    {
        tmp[t] = rand() & 0xff;
    }
}

void init_randomBufEx(void)
{
    UINT8 *tmp;
    tmp = random_buf;
    UINT8 t = 0;
    UINT32 startTime ;
    startTime = hal_TimGetUpTime();

    for(t = 0; t < 128; t++)
    {
        startTime = startTime + t * 100 + t * 10 + t;
        srand(startTime);
        tmp[t] = rand() & 0xff;
    }
}
int os_get_random(unsigned char *buf, size_t len)
{
    memcpy(buf, random_buf, len);

    return  0;
}
u8 *supp_alloc_eapol( u8 type, const void *data,
                      u16 data_len, size_t *msg_len, void **data_pos)
{
    struct ieee802_1x_hdr *hdr;

    CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(2), TSTR("SUPP: (type=%d data_len=%d)",0x08700030),    type, data_len);

    *msg_len = sizeof(*hdr) + data_len;
    hdr = os_malloc(*msg_len);
    if (hdr == NULL)
        return NULL;

    hdr->version = 1;
    hdr->type = type;
    hdr->length = host_to_be16(data_len);

    if (data)
        os_memcpy(hdr + 1, data, data_len);
    else
        os_memset(hdr + 1, 0, data_len);

    if (data_pos)
        *data_pos = hdr + 1;
    CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(2), TSTR("data_pos: 0x%x, har: 0x%x",0x08700031),   *data_pos, hdr);

    return (u8 *) hdr;
}

WPA_SM_T sm_cxt ;
void setWpaAddr(UINT8 *dest, UINT8 *src, UINT8 *ssid)
{
    memcpy(sm_cxt.bssid, dest, 6);
    memcpy(sm_cxt.own_addr, src, 6);
    strcpy(sm_cxt.ssid, ssid);
	sm_cxt.ssid_len = strlen(ssid);

}

int wifi_mac_data_txReq(UINT8 *data, unsigned short len,UINT8 isDataMalloc);
static inline int wpa_sm_ether_send(WPA_SM_T *sm, const u8 *dest,
                                    u16 proto, const u8 *buf, size_t len)
{

    UINT16 offset = 0;
    UINT8 *eapoldata ;
    eapoldata = COS_MALLOC(len + 14);
    SUL_MemCopy8 (&eapoldata[offset], sm->bssid, 6);
    offset += 6;
    SUL_MemCopy8 (&eapoldata[offset], sm->own_addr, 6);
    offset += 6;
    eapoldata[offset] = 0x88;
    offset++;
    eapoldata[offset] = 0x8e;
    offset++;
    SUL_MemCopy8 (&eapoldata[offset], buf, len);
    //   SUL_MemSet8(&eapoldata[offset-35],0xcc,5);
    /*
    if(sm_cxt.send_buf[0][1664] ==0)
    {
    memset(&sm_cxt.send_buf[0][0],0x00,1665)
      SUL_MemCopy8 (&sm_cxt.send_buf[0][0],eapoldata,14 + len);
    sm_cxt.send_buf[0][1664] = 1;
    }else{
        CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0), TSTR("wpa_sm_ether_send,another key waiting to be sent!!",0x08700032));

    }
    // test_rda5890_data_tx(eapoldata,14 + len);
    */
    memset(&sm_cxt.send_buf[0][0], 0x00, 1665);
    SUL_MemCopy8 (&sm_cxt.send_buf[0][0], eapoldata, 14 + len);
#ifdef USB_WIFI_SUPPORT
#else

    wifi_mac_data_txReq(&sm_cxt.send_buf[0][0], 14 + len,0);
#endif

    COS_FREE(eapoldata);
    return 0;
}
void wpa_eapol_key_send(WPA_SM_T *sm, const u8 *kck,
                        int ver, const u8 *dest, u16 proto,
                        u8 *msg, size_t msg_len, u8 *key_mic)
{
    int res = 0;
    if (is_zero_ether_addr(dest) && is_zero_ether_addr(sm->bssid))
    {
        /*
         * Association event was not yet received; try to fetch
         * BSSID from the driver.
         */
        //  if (wpa_sm_get_bssid(sm, sm->bssid) < 0) {
        CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0),  TSTR("WPA: Failed to read BSSID for EAPOL-Key destination address",0x08700033));
        //  } else {
        //  dest = sm->bssid;
        //  CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0), TSTR("WPA: Use BSSID () as the destination for EAPOL-Key",0x08700034) );
        //  }
    }
    if (key_mic)
        res = wpa_eapol_key_mic(kck, ver, msg, msg_len, key_mic);
    CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(1),  TSTR("wpa_eapol_key_mic, res:%d",0x08700035), res);

    //wpa_hexdump(MSG_MSGDUMP, "WPA: TX EAPOL-Key", msg, msg_len);
    wpa_sm_ether_send(sm, dest, proto, msg, msg_len);
    //eapol_sm_notify_tx_eapol_key(sm->eapol);
    CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(1),  TSTR("wpa_eapol_key_send, msg:0x%x",0x08700036), msg);

    COS_FREE(msg);
}


static int wpa_supplicant_validate_ie(struct wpa_sm *sm,
                                      const unsigned char *src_addr,
                                      struct wpa_eapol_ie_parse *ie)
{
#if 0
    if (sm->ap_wpa_ie == NULL && sm->ap_rsn_ie == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0),  TSTR("WPA: No WPA/RSN IE for this AP known. Trying to get from scan results",0x08700037));
        if (wpa_sm_get_beacon_ie(sm) < 0)
        {
            CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0), TSTR("WPA: Could not find AP from the scan results",0x08700038));
        }
        else
        {
            CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0),  TSTR("WPA: Found the current AP from updated scan results",0x08700039));
        }
    }

    if (ie->wpa_ie == NULL && ie->rsn_ie == NULL &&
            (sm->ap_wpa_ie || sm->ap_rsn_ie))
    {
        wpa_report_ie_mismatch(sm, "IE in 3/4 msg does not match "
                               "with IE in Beacon/ProbeResp (no IE?)",
                               src_addr, ie->wpa_ie, ie->wpa_ie_len,
                               ie->rsn_ie, ie->rsn_ie_len);
        return -1;
    }

    if ((ie->wpa_ie && sm->ap_wpa_ie &&
            (ie->wpa_ie_len != sm->ap_wpa_ie_len ||
             os_memcmp(ie->wpa_ie, sm->ap_wpa_ie, ie->wpa_ie_len) != 0)) ||
            (ie->rsn_ie && sm->ap_rsn_ie &&
             (ie->rsn_ie_len != sm->ap_rsn_ie_len ||
              os_memcmp(ie->rsn_ie, sm->ap_rsn_ie, ie->rsn_ie_len) != 0)))
    {
        wpa_report_ie_mismatch(sm, "IE in 3/4 msg does not match "
                               "with IE in Beacon/ProbeResp",
                               src_addr, ie->wpa_ie, ie->wpa_ie_len,
                               ie->rsn_ie, ie->rsn_ie_len);
        return -1;
    }

    if (sm->proto == WPA_PROTO_WPA &&
            ie->rsn_ie && sm->ap_rsn_ie == NULL && sm->rsn_enabled)
    {
        wpa_report_ie_mismatch(sm, "Possible downgrade attack "
                               "detected - RSN was enabled and RSN IE "
                               "was in msg 3/4, but not in "
                               "Beacon/ProbeResp",
                               src_addr, ie->wpa_ie, ie->wpa_ie_len,
                               ie->rsn_ie, ie->rsn_ie_len);
        return -1;
    }

#ifdef CONFIG_IEEE80211R
    if (wpa_key_mgmt_ft(sm->key_mgmt))
    {
        struct rsn_mdie *mdie;
        /* TODO: verify that full MDIE matches with the one from scan
         * results, not only mobility domain */
        mdie = (struct rsn_mdie *) (ie->mdie + 2);
        if (ie->mdie == NULL || ie->mdie_len < 2 + sizeof(*mdie) ||
                os_memcmp(mdie->mobility_domain, sm->mobility_domain,
                          MOBILITY_DOMAIN_ID_LEN) != 0)
        {
            wpa_printf(MSG_DEBUG, "FT: MDIE in msg 3/4 did not "
                       "match with the current mobility domain");
            return -1;
        }
    }
#endif /* CONFIG_IEEE80211R */
#endif
    return 0;
}
int wpa_supplicant_send_2_of_4(WPA_SM_T *sm, const unsigned char *dst,
                               const struct wpa_eapol_key *key,
                               int ver, const u8 *nonce,
                               const u8 *wpa_ie, size_t wpa_ie_len,
                               struct wpa_ptk *ptk)
{
    size_t rlen;
    struct wpa_eapol_key *reply;
    u8 *rbuf;

    if (wpa_ie == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0), TSTR("WPA: No wpa_ie set - cannot generate msg 2/4",0x0870003a));
        return -1;
    }

    //wpa_hexdump(MSG_DEBUG, "WPA: WPA IE for msg 2/4", wpa_ie, wpa_ie_len);


    rbuf = supp_alloc_eapol(IEEE802_1X_TYPE_EAPOL_KEY,
                            NULL, sizeof(*reply) + wpa_ie_len,
                            &rlen, (void *) &reply);
    //rbuf = wpa_sm_alloc_eapol(sm, IEEE802_1X_TYPE_EAPOL_KEY,
    //                NULL, sizeof(*reply) + wpa_ie_len,
    //                &rlen, (void *) &reply);
    if (rbuf == NULL)
        return -1;

    reply->type = sm->proto == WPA_PROTO_RSN ?
                  EAPOL_KEY_TYPE_RSN : EAPOL_KEY_TYPE_WPA;
    WPA_PUT_BE16(reply->key_info,
                 ver | WPA_KEY_INFO_KEY_TYPE | WPA_KEY_INFO_MIC);
    if (sm->proto == WPA_PROTO_RSN)
        WPA_PUT_BE16(reply->key_length, 0);
    else
        os_memcpy(reply->key_length, key->key_length, 2);
    os_memcpy(reply->replay_counter, key->replay_counter,
              WPA_REPLAY_COUNTER_LEN);

    WPA_PUT_BE16(reply->key_data_length, wpa_ie_len);
    os_memcpy(reply + 1, wpa_ie, wpa_ie_len);

    os_memcpy(reply->key_nonce, nonce, WPA_NONCE_LEN);

    CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0),  TSTR("WPA: Sending EAPOL-Key 2/4",0x0870003b));
    wpa_eapol_key_send(sm, ptk->kck, ver, dst, ETH_P_EAPOL,
                       rbuf, rlen, reply->key_mic);

    return 0;
}

void wpa_supplicant_process_1_of_4(WPA_SM_T *sm,
                                   const unsigned char *src_addr,
                                   struct wpa_eapol_key *key,
                                   u16 ver)
{
    struct wpa_eapol_ie_parse ie;
    struct wpa_ptk *ptk;
    u8 buf[8];

    //  if (wpa_sm_get_network_ctx(sm) == NULL) {
    //  CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0),  TSTR("WPA: No SSID info found (msg 1 of 4).",0x0870003c));
    //  return;
    //}

    //  wpa_sm_set_state(sm, WPA_4WAY_HANDSHAKE);
    //  CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(1),  TSTR("WPA: RX message 1 of 4-Way Handshake from  (ver=%d)",0x0870003d), MAC2STR(src_addr), ver);
    memcpy(sm->anonce, key->key_nonce, WPA_NONCE_LEN);
    os_memset(&ie, 0, sizeof(ie));

#ifndef CONFIG_NO_WPA2
    if (sm->proto == WPA_PROTO_RSN)
    {
        /* RSN: msg 1/4 should contain PMKID for the selected PMK */
        const u8 *_buf = (const u8 *) (key + 1);
        size_t len = WPA_GET_BE16(key->key_data_length);
        //wpa_hexdump(MSG_DEBUG, "RSN: msg 1/4 key data", _buf, len);
        wpa_supplicant_parse_ies(_buf, len, &ie);
        if (ie.pmkid)
        {
            //wpa_hexdump(MSG_DEBUG, "RSN: PMKID from "
            //      "Authenticator", ie.pmkid, PMKID_LEN);
        }
    }
#endif /* CONFIG_NO_WPA2 */

    if (wpa_supplicant_get_pmk(sm, src_addr, ie.pmkid))
        goto failed;

    if (sm->renew_snonce)
    {
        if (os_get_random(sm->snonce, WPA_NONCE_LEN))
        {
            //wpa_msg(sm->ctx->ctx, MSG_WARNING,
            //"WPA: Failed to get random data for SNonce");
            goto failed;
        }
        sm->renew_snonce = 0;
        //wpa_hexdump(MSG_DEBUG, "WPA: Renewed SNonce",
        //      sm->snonce, WPA_NONCE_LEN);
    }

    /* Calculate PTK which will be stored as a temporary PTK until it has
     * been verified when processing message 3/4. */

    /******************************/
    /*
    UINT8 test_snonce[32] ={0x79,0xe0,0xdc,0xd5,0x01,0x2d,0x7f,0x88,0x36,0x12,0xfc,0xcd,0x5f,0x4b,0x3d,0xb3,0x5c,0x21,0x82,0xc1,0xb9,0x85,0xa2,0x71,0xb4,0xda,0x04,0x87,0x2d,0x41,0x36,0xa0};
    memcpy(sm->snonce,test_snonce,WPA_NONCE_LEN);
    UINT8 test_knonce[32] ={0xa2,0x83,0x2b,0x60,0x60,0xa8,0xc3,0xa9,0x05,0xec,0xdb,0xea,0xb0,0x43,0x54,0xfb,0x76,0xfc,0x93,0x06,0x49,0xfd,0xad,0x20,0x8d,0xdc,0x99,0x50,0x91,0x8d,0xf6,0x65};
    memcpy(key->key_nonce,test_knonce,WPA_NONCE_LEN);
        UINT8 test_ownaddr[6] ={0x00,0x50,0xc2, 0x5e, 0x20, 0x01};

        memcpy(sm->own_addr, test_ownaddr, 6);
    key->replay_counter[7] = 0x01;
    */
    //UINT8 psk[] = "wl_test123456";
    //memset(sm->pmk,0,32);
    //memcpy(sm->pmk, psk, strlen(psk));
    //sm->pmk_len= 32;//strlen(psk);
    /******************************/

    ptk = &sm->tptk;
    wpa_derive_ptk(sm, src_addr, key, ptk);
    /* Supplicant: swap tx/rx Mic keys */
    os_memcpy(buf, ptk->u.auth.tx_mic_key, 8);
    os_memcpy(ptk->u.auth.tx_mic_key, ptk->u.auth.rx_mic_key, 8);
    os_memcpy(ptk->u.auth.rx_mic_key, buf, 8);
    sm->tptk_set = 1;

    if (wpa_supplicant_send_2_of_4(sm, sm->bssid, key, ver, sm->snonce,
                                   sm->assoc_wpa_ie, sm->assoc_wpa_ie_len,
                                   ptk))
        goto failed;

    os_memcpy(sm->anonce, key->key_nonce, WPA_NONCE_LEN);
    return;

failed:
    return;
    //wpa_sm_deauthenticate(sm, WLAN_REASON_UNSPECIFIED);
}


void peerkey_rx_eapol_4way(WPA_SM_T *sm, struct wpa_peerkey *peerkey,
                           struct wpa_eapol_key *key, u16 key_info, u16 ver)
{
#if 0
    if ((key_info & (WPA_KEY_INFO_MIC | WPA_KEY_INFO_ACK)) ==
            (WPA_KEY_INFO_MIC | WPA_KEY_INFO_ACK))
    {
        /* 3/4 STK 4-Way Handshake */
        wpa_supplicant_process_stk_3_of_4(sm, peerkey, key, ver);
    }
    else if (key_info & WPA_KEY_INFO_ACK)
    {
        /* 1/4 STK 4-Way Handshake */
        wpa_supplicant_process_stk_1_of_4(sm, peerkey, key, ver);
    }
    else if (key_info & WPA_KEY_INFO_SECURE)
    {
        /* 4/4 STK 4-Way Handshake */
        wpa_supplicant_process_stk_4_of_4(sm, peerkey, key, ver);
    }
    else
    {
        /* 2/4 STK 4-Way Handshake */
        wpa_supplicant_process_stk_2_of_4(sm, peerkey, key, ver);
    }
#endif
}


void peerkey_rx_eapol_smk(WPA_SM_T *sm, const u8 *src_addr,
                          struct wpa_eapol_key *key, size_t extra_len,
                          u16 key_info, u16 ver)
{
#if 0
    if (key_info & WPA_KEY_INFO_ERROR)
    {
        /* SMK Error */
        wpa_supplicant_process_smk_error(sm, src_addr, key, extra_len);
    }
    else if (key_info & WPA_KEY_INFO_ACK)
    {
        /* SMK M2 */
        wpa_supplicant_process_smk_m2(sm, src_addr, key, extra_len,
                                      ver);
    }
    else
    {
        /* SMK M4 or M5 */
        wpa_supplicant_process_smk_m45(sm, src_addr, key, extra_len,
                                       ver);
    }
#endif
}

static int wpa_supplicant_process_1_of_2_rsn(struct wpa_sm *sm,
        const u8 *keydata,
        size_t keydatalen,
        u16 key_info,
        struct wpa_gtk_data *gd)
{
    CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0), TSTR("wpa_supplicant_process_1_of_2_rsn , but no process!!!!!!!!!!!",0x0870003e) );
#if 1
    int maxkeylen;
    struct wpa_eapol_ie_parse ie;

    //wpa_hexdump(MSG_DEBUG, "RSN: msg 1/2 key data", keydata, keydatalen);
    wpa_supplicant_parse_ies(keydata, keydatalen, &ie);
    if (ie.gtk && !(key_info & WPA_KEY_INFO_ENCR_KEY_DATA))
    {
        CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0), TSTR("WPA: GTK IE in unencrypted key data",0x0870003f));
        return -1;
    }
    if (ie.gtk == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0), TSTR("WPA: No GTK IE in Group Key msg 1/2",0x08700040));
        return -1;
    }
    maxkeylen = gd->gtk_len = ie.gtk_len - 2;

    if (wpa_supplicant_check_group_cipher(sm->group_cipher,
                                          gd->gtk_len, maxkeylen,
                                          &gd->key_rsc_len, &gd->alg))
        return -1;

    //wpa_hexdump(MSG_DEBUG, "RSN: received GTK in group key handshake",
    //      ie.gtk, ie.gtk_len);
    gd->keyidx = ie.gtk[0] & 0x3;
    gd->tx = wpa_supplicant_gtk_tx_bit_workaround(sm,
             !!(ie.gtk[0] & BIT(2)));
    if (ie.gtk_len - 2 > sizeof(gd->gtk))
    {
        CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0), TSTR("RSN: Too long GTK in GTK IE ",0x08700041),
                  "(len=%lu)", (unsigned long) ie.gtk_len - 2);
        return -1;
    }
    os_memcpy(gd->gtk, ie.gtk + 2, ie.gtk_len - 2);

    if (ieee80211w_set_keys(sm, &ie) < 0)
        CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0), TSTR("RSN: Failed to configure IGTK",0x08700042));
#endif
    return 0;
}


static int wpa_supplicant_process_1_of_2_wpa(struct wpa_sm *sm,
        const struct wpa_eapol_key *key,
        size_t keydatalen, int key_info,
        size_t extra_len, u16 ver,
        struct wpa_gtk_data *gd)
{
    CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0), TSTR("wpa_supplicant_process_1_of_2_wpa , but no process!!!!!!!!!!!",0x08700043) );

    size_t maxkeylen;
    u8 ek[32];

    gd->gtk_len = WPA_GET_BE16(key->key_length);
    maxkeylen = keydatalen;
    if (keydatalen > extra_len)
    {
        CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(2), TSTR("WPA: Truncated EAPOL-Key packet: key_data_length=%lu > extra_len=%lu",0x08700044),
                  (unsigned long) keydatalen,
                  (unsigned long) extra_len);
        return -1;
    }
    if (ver == WPA_KEY_INFO_TYPE_HMAC_SHA1_AES)
    {
        if (maxkeylen < 8)
        {
            CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(1), TSTR("WPA: Too short maxkeylen (%lu)",0x08700045),
                      (unsigned long) maxkeylen);
            return -1;
        }
        maxkeylen -= 8;
    }

    if (wpa_supplicant_check_group_cipher(sm->group_cipher,
                                          gd->gtk_len, maxkeylen,
                                          &gd->key_rsc_len, &gd->alg))
        return -1;

    gd->keyidx = (key_info & WPA_KEY_INFO_KEY_INDEX_MASK) >>
                 WPA_KEY_INFO_KEY_INDEX_SHIFT;
    if (ver == WPA_KEY_INFO_TYPE_HMAC_MD5_RC4)
    {
        os_memcpy(ek, key->key_iv, 16);
        os_memcpy(ek + 16, sm->ptk.kek, 16);
        if (keydatalen > sizeof(gd->gtk))
        {
            CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(1), TSTR("WPA: RC4 key data too long (%lu)",0x08700046),
                      (unsigned long) keydatalen);
            return -1;
        }
        os_memcpy(gd->gtk, key + 1, keydatalen);
        //  rc4_skip(ek, 32, 256, gd->gtk, keydatalen);
    }
    else if (ver == WPA_KEY_INFO_TYPE_HMAC_SHA1_AES)
    {
        if (keydatalen % 8)
        {
            CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(1), TSTR("WPA: Unsupported AES-WRAP len %lu",0x08700047), (unsigned long) keydatalen);
            return -1;
        }
        if (maxkeylen > sizeof(gd->gtk))
        {
            CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(2), TSTR("WPA: AES-WRAP key data too long (keydatalen=%lu maxkeylen=%lu)",0x08700048),
                      (unsigned long) keydatalen,
                      (unsigned long) maxkeylen);
            return -1;
        }
        if (aes_unwrap(sm->ptk.kek, maxkeylen / 8,
                       (const u8 *) (key + 1), gd->gtk))
        {
            CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0), TSTR("WPA: AES unwrap failed - could not decrypt GTK",0x08700049));
            return -1;
        }
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(1), TSTR("WPA: Unsupported key_info type %d",0x0870004a),
                  ver);
        return -1;
    }
    gd->tx = wpa_supplicant_gtk_tx_bit_workaround(
                 sm, !!(key_info & WPA_KEY_INFO_TXRX));
    return 0;
}

static int wpa_supplicant_send_2_of_2(struct wpa_sm *sm,
                                      const struct wpa_eapol_key *key,
                                      int ver, u16 key_info)
{
    size_t rlen;
    struct wpa_eapol_key *reply;
    u8 *rbuf;

    rbuf = supp_alloc_eapol(IEEE802_1X_TYPE_EAPOL_KEY, NULL,
                            sizeof(*reply), &rlen, (void *) &reply);
    if (rbuf == NULL)
        return -1;

    reply->type = sm->proto == WPA_PROTO_RSN ?
                  EAPOL_KEY_TYPE_RSN : EAPOL_KEY_TYPE_WPA;
    key_info &= WPA_KEY_INFO_KEY_INDEX_MASK;
    key_info |= ver | WPA_KEY_INFO_MIC | WPA_KEY_INFO_SECURE;
    WPA_PUT_BE16(reply->key_info, key_info);
    if (sm->proto == WPA_PROTO_RSN)
        WPA_PUT_BE16(reply->key_length, 0);
    else
        os_memcpy(reply->key_length, key->key_length, 2);
    os_memcpy(reply->replay_counter, key->replay_counter,
              WPA_REPLAY_COUNTER_LEN);

    WPA_PUT_BE16(reply->key_data_length, 0);

    CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0), TSTR("WPA: Sending EAPOL-Key 2/2",0x0870004b));
    wpa_eapol_key_send(sm, sm->ptk.kck, ver, sm->bssid, ETH_P_EAPOL,
                       rbuf, rlen, reply->key_mic);

    return 0;
}

static void wpa_supplicant_process_1_of_2(WPA_SM_T *sm,
        const unsigned char *src_addr,
        const struct wpa_eapol_key *key,
        int extra_len, u16 ver)
{
#if 1
    u16 key_info, keydatalen;
    int rekey, ret;
    struct wpa_gtk_data gd;

    os_memset(&gd, 0, sizeof(gd));

    //rekey = wpa_sm_get_state(sm) == WPA_COMPLETED;
    CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(1), TSTR("WPA: RX message 1 of Group Key Handshake from  (ver=%d)",0x0870004c),  ver);

    key_info = WPA_GET_BE16(key->key_info);
    keydatalen = WPA_GET_BE16(key->key_data_length);

    if (sm->proto == WPA_PROTO_RSN)
    {
        ret = wpa_supplicant_process_1_of_2_rsn(sm,
                                                (const u8 *) (key + 1),
                                                keydatalen, key_info,
                                                &gd);
    }
    else
    {
        ret = wpa_supplicant_process_1_of_2_wpa(sm, key, keydatalen,
                                                key_info, extra_len,
                                                ver, &gd);
    }

    //wpa_sm_set_state(sm, WPA_GROUP_HANDSHAKE);

    if (ret)
        goto failed;

    if (wpa_supplicant_install_gtk(sm, &gd, key->key_rsc) ||
            wpa_supplicant_send_2_of_2(sm, key, ver, key_info))
        goto failed;
    /*
        if (rekey) {
            wpa_msg(sm->ctx->ctx, MSG_INFO, "WPA: Group rekeying "
                "completed with " MACSTR " [GTK=%s]",
                MAC2STR(sm->bssid), wpa_cipher_txt(sm->group_cipher));
            wpa_sm_cancel_auth_timeout(sm);
            wpa_sm_set_state(sm, WPA_COMPLETED);
        } else {
            wpa_supplicant_key_neg_complete(sm, sm->bssid,
                            key_info &
                            WPA_KEY_INFO_SECURE);
        }*/
    CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(1),  TSTR("in wpa_supplicant_process_1_of_2, ketsettoMac: 0x%x ",0x0870004d), sm->keysettoMac);

    if(sm->keysettoMac == 0x03)
    {
        ip_WifiConnectedInd();
    }

    return;

failed:
    return;
    //  wpa_sm_deauthenticate(sm, WLAN_REASON_UNSPECIFIED);
#endif
}

int wpa_supplicant_send_4_of_4(WPA_SM_T *sm, const unsigned char *dst,
                               const struct wpa_eapol_key *key,
                               u16 ver, u16 key_info,
                               const u8 *kde, size_t kde_len,
                               struct wpa_ptk *ptk)
{
    size_t rlen;
    struct wpa_eapol_key *reply;
    u8 *rbuf;

    if (kde)
    {
        CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0),  TSTR("WPA: KDE for msg 4/4",0x0870004e));
        wifi_dump(kde, kde_len);
    }

    rbuf = supp_alloc_eapol( IEEE802_1X_TYPE_EAPOL_KEY, NULL,
                             sizeof(*reply) + kde_len,
                             &rlen, (void *) &reply);
    if (rbuf == NULL)
        return -1;
    CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(1),  TSTR("WPA: Sending EAPOL-Key 4/4, sm->proto 0x%x",0x0870004f), sm->proto );
    reply->type = sm->proto == WPA_PROTO_RSN ?
                  EAPOL_KEY_TYPE_RSN : EAPOL_KEY_TYPE_WPA;
    key_info &= WPA_KEY_INFO_SECURE;
    key_info |= ver | WPA_KEY_INFO_KEY_TYPE | WPA_KEY_INFO_MIC;
    WPA_PUT_BE16(reply->key_info, key_info);
    CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(1),  TSTR("WPA: Sending EAPOL-Key 4/4, sm->proto 0x%x",0x08700050), sm->proto );
    if (sm->proto == WPA_PROTO_RSN)
        WPA_PUT_BE16(reply->key_length, 0);
    else
        os_memcpy(reply->key_length, key->key_length, 2);
    os_memcpy(reply->replay_counter, key->replay_counter,
              WPA_REPLAY_COUNTER_LEN);

    WPA_PUT_BE16(reply->key_data_length, kde_len);
    if (kde)
        os_memcpy(reply + 1, kde, kde_len);

    CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(2),  TSTR("WPA: Sending EAPOL-Key 4/4,  reply->key_info:0x%x 0x%x",0x08700051), reply->key_info[0], reply->key_info[1]);
    wifi_dump(rbuf, rlen);

    wpa_eapol_key_send(sm, ptk->kck, ver, dst, ETH_P_EAPOL,
                       rbuf, rlen, reply->key_mic);

    return 0;
}

static int ieee80211w_set_keys(WPA_SM_T *sm,
                               struct wpa_eapol_ie_parse *ie)
{
#ifdef CONFIG_IEEE80211W
    if (sm->mgmt_group_cipher != WPA_CIPHER_AES_128_CMAC)
        return 0;

    if (ie->igtk)
    {
        const struct wpa_igtk_kde *igtk;
        u16 keyidx;
        if (ie->igtk_len != sizeof(*igtk))
            return -1;
        igtk = (const struct wpa_igtk_kde *) ie->igtk;
        keyidx = WPA_GET_LE16(igtk->keyid);
        /*CSW_TRACE(9,  "WPA: IGTK keyid %d pn %02x%02x%02x%02x%02x%02x", keyidx, (igtk->pn));*/
        //wpa_hexdump_key(MSG_DEBUG, "WPA: IGTK",
        //      igtk->igtk, WPA_IGTK_LEN);
        if (keyidx > 4095)
        {
            CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(1), TSTR("WPA: Invalid IGTK KeyID %d",0x08700052),
                      keyidx);
            return -1;
        }
        if (wpa_sm_set_key(sm, WPA_ALG_IGTK,
                           (u8 *) "\xff\xff\xff\xff\xff\xff",
                           keyidx, 0, igtk->pn, sizeof(igtk->pn),
                           igtk->igtk, WPA_IGTK_LEN) < 0)
        {
            CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0),  TSTR("WPA: Failed to configure IGTK to the driver",0x08700053));
            return -1;
        }
    }

    return 0;
#else /* CONFIG_IEEE80211W */
    return 0;
#endif /* CONFIG_IEEE80211W */
}

int wpa_supplicant_gtk_tx_bit_workaround(const struct wpa_sm *sm,
        int tx)
{
    if (tx && sm->pairwise_cipher != WPA_CIPHER_NONE)
    {
        /* Ignore Tx bit for GTK if a pairwise key is used. One AP
         * seemed to set this bit (incorrectly, since Tx is only when
         * doing Group Key only APs) and without this workaround, the
         * data connection does not work because wpa_supplicant
         * configured non-zero keyidx to be used for unicast. */
        CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0),  TSTR("WPA: Tx bit set for GTK, but pairwise keys are used - ignore Tx bit",0x08700054));
        return 0;
    }
    return tx;
}

static const char *wpa_cipher_txt(int cipher)
{
    switch (cipher)
    {
        case WPA_CIPHER_NONE:
            return "NONE";
        case WPA_CIPHER_WEP40:
            return "WEP-40";
        case WPA_CIPHER_WEP104:
            return "WEP-104";
        case WPA_CIPHER_TKIP:
            return "TKIP";
        case WPA_CIPHER_CCMP:
            return "CCMP";
        default:
            return "UNKNOWN";
    }
}


int wpa_supplicant_check_group_cipher(int group_cipher,
                                      int keylen, int maxkeylen,
                                      int *key_rsc_len, wpa_alg *alg)
{
    int ret = 0;

    switch (group_cipher)
    {
        case WPA_CIPHER_CCMP:
            if (keylen != 16 || maxkeylen < 16)
            {
                ret = -1;
                break;
            }
            *key_rsc_len = 8;
            *alg = WPA_ALG_CCMP;
            break;
        case WPA_CIPHER_TKIP:
            if (keylen != 32 || maxkeylen < 32)
            {
                ret = -1;
                break;
            }
            *key_rsc_len = 6;
            *alg = WPA_ALG_TKIP;
            break;
        case WPA_CIPHER_WEP104:
            if (keylen != 13 || maxkeylen < 13)
            {
                ret = -1;
                break;
            }
            *key_rsc_len = 0;
            *alg = WPA_ALG_WEP;
            break;
        case WPA_CIPHER_WEP40:
            if (keylen != 5 || maxkeylen < 5)
            {
                ret = -1;
                break;
            }
            *key_rsc_len = 0;
            *alg = WPA_ALG_WEP;
            break;
        default:
            CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(1), TSTR("WPA: Unsupported Group Cipher %d",0x08700055),
                      group_cipher);
            return -1;
    }

    if (ret < 0 )
    {
        CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(3)|TSMAP(1),  TSTR("WPA: Unsupported %s Group Cipher key length %d (%d).",0x08700056),
                  wpa_cipher_txt(group_cipher), keylen, maxkeylen);
    }

    return ret;
}
int wifi_mac_key_set_Req(UINT32 key_type);

static inline int wpa_sm_set_key(struct wpa_sm *sm, wpa_alg alg,
                                 const u8 *addr, int key_idx, int set_tx,
                                 const u8 *seq, size_t seq_len,
                                 const u8 *key, size_t key_len)
{
    CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0),  TSTR("set key to MAC.",0x08700057));
    UINT8 *tmpdata;
    UINT8 *ptr;
    UINT8 len = key_len + 6 + 1; //6 for bssid, 1 for key len
    tmpdata = os_malloc(len);
    memset(tmpdata, 0x00, len);
    ptr = tmpdata;
    memcpy(tmpdata, addr, 6);
    tmpdata += 6;
    *tmpdata = key_len;
    tmpdata++;
    memcpy(tmpdata, key, key_len);

    memcpy(sm_cxt.set_key, ptr, len);
    sm_cxt.set_key_len = len;
    wifi_mac_key_set_Req(0x301B);
    //test_rda5890_set_StrWid(0x301B,ptr, len );



    /*
        WPA_ASSERT(sm->ctx->set_key);
        return sm->ctx->set_key(sm->ctx->ctx, alg, addr, key_idx, set_tx,
                    seq, seq_len, key, key_len);
                    */
    return 0;
}
static inline int wpa_sm_set_ptk(    const u8 *addr, const u8 *key, size_t key_len)
{
    CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0),  TSTR("set ptk to MAC.",0x08700058));
    UINT8 *tmpdata;
    UINT8 *ptr;
    UINT8 len = key_len + 6 + 1; //6 for bssid, 1 for key len
    tmpdata = os_malloc(len);
    memset(tmpdata, 0x00, len);
    ptr = tmpdata;
    memcpy(tmpdata, addr, 6);
    tmpdata += 6;
    *tmpdata = key_len;
    tmpdata++;
    memcpy(tmpdata, key, 16);
    if(key_len > 16)
    {

        memcpy(tmpdata + 16, key + 24, 8);
        memcpy(tmpdata + 24, key + 16, 8);

    }
    CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(2),  TSTR("wpa_sm_set_ptk, ptr:0x%x; len:%d",0x08700059), ptr, len);
    wifi_dump(ptr, len);
    memcpy(sm_cxt.set_ptk, ptr, len);
    sm_cxt.set_ptk_len = len;
    wifi_mac_key_set_Req(0x301B);

    //test_rda5890_set_StrWid(0x301B,ptr, len );

    COS_FREE(ptr);

    /*
        WPA_ASSERT(sm->ctx->set_key);
        return sm->ctx->set_key(sm->ctx->ctx, alg, addr, key_idx, set_tx,
                    seq, seq_len, key, key_len);
                    */
    return 0;
}
static inline int wpa_sm_set_gtk(struct wpa_sm *sm, wpa_alg alg,
                                 const u8 *addr, int key_idx, int set_tx,
                                 const u8 *seq, size_t seq_len,
                                 const u8 *key, size_t key_len)
{
    CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(1),  TSTR("set gtk to MAC.klen:%d",0x0870005a), key_len);
    UINT8 *tmpdata;
    UINT8 *ptr;
    UINT8 len = key_len + seq_len + 6 + 1 + 1; //6 for bssid, 1 for key len
    tmpdata = os_malloc(len);
    memset(tmpdata, 0x00, len);
    ptr = tmpdata;
    memcpy(tmpdata, addr, 6);
    tmpdata += 6;
    memcpy(tmpdata, seq, seq_len);
    tmpdata += seq_len;
    *tmpdata = (U8)key_idx;
    tmpdata++;

    *tmpdata = key_len;
    tmpdata++;
    memcpy(tmpdata, key, 16);



    /* swap TX MIC and RX MIC, rda5890 need RX MIC to be ahead */
    if(key_len > 16)
    {
        //memcpy(key_str + 16 + 16, key + 16, key_len - 16);
#if 0
        CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0),  TSTR("set gtk to MAC.no swap",0x0870005b));

        memcpy(tmpdata + 16 , key + 16, 8);
        memcpy(tmpdata + 24, key + 24, 8);
#else
        CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0),  TSTR("set gtk to MAC. swap",0x0870005c));

        memcpy(tmpdata + 16 , key + 24, 8);
        memcpy(tmpdata + 24, key + 16, 8);
#endif
    }
    CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(2),  TSTR("wpa_sm_set_gtk, ptr:0x%x; len:%d",0x0870005d), ptr, len);
    wifi_dump(ptr, len);

    memcpy(sm_cxt.set_gtk, ptr, len);
    sm_cxt.set_gtk_len = len;

    wifi_mac_key_set_Req(0x301c);

    //test_rda5890_set_StrWid(0x301c,ptr, len );
    COS_FREE(ptr);


    /*
        WPA_ASSERT(sm->ctx->set_key);
        return sm->ctx->set_key(sm->ctx->ctx, alg, addr, key_idx, set_tx,
                    seq, seq_len, key, key_len);
                    */
    return 0;
}
int wpa_supplicant_install_gtk(struct wpa_sm *sm,
                               const struct wpa_gtk_data *gd,
                               const u8 *key_rsc)
{
    const u8 *_gtk = gd->gtk;
    u8 gtk_buf[32];
    u8 null_rsc[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

    //wpa_hexdump_key(MSG_DEBUG, "WPA: Group Key", gd->gtk, gd->gtk_len);
    CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(3),  TSTR("WPA: Installing GTK to the driver (keyidx=%d tx=%d len=%d).",0x0870005e), gd->keyidx, gd->tx,
              gd->gtk_len);
    //wpa_hexdump(MSG_DEBUG, "WPA: RSC", key_rsc, gd->key_rsc_len);
    if (sm->group_cipher == WPA_CIPHER_TKIP)
    {
        /* Swap Tx/Rx keys for Michael MIC */
        os_memcpy(gtk_buf, gd->gtk, 16);
        os_memcpy(gtk_buf + 16, gd->gtk + 24, 8);
        os_memcpy(gtk_buf + 24, gd->gtk + 16, 8);
        _gtk = gtk_buf;
    }
    CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(2), TSTR("WPA: set GTK to the driver (Group only). pair: 0x%x; group: 0x%x",0x0870005f), sm->pairwise_cipher, sm->group_cipher);

    if (sm->pairwise_cipher == WPA_CIPHER_NONE)
    {
        if (wpa_sm_set_gtk(sm, gd->alg,
                           sm->bssid,
                           gd->keyidx, 1, key_rsc, gd->key_rsc_len,
                           _gtk, gd->gtk_len) < 0)
        {
            CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0), TSTR("WPA: Failed to set GTK to the driver (Group only).",0x08700060));
            return -1;
        }
    }
    else if (wpa_sm_set_gtk(sm, gd->alg,
                            sm->bssid,
                            gd->keyidx, gd->tx, null_rsc, 8,
                            _gtk, gd->gtk_len) < 0)
    {
        CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(3),  TSTR("WPA: Failed to set GTK to the driver (alg=%d keylen=%d keyidx=%d)",0x08700061),
                  gd->alg, gd->gtk_len, gd->keyidx);
        return -1;
    }
    sm->keysettoMac |= (0x01 << 1);

    return 0;
}

int wpa_supplicant_pairwise_gtk(struct wpa_sm *sm,
                                const struct wpa_eapol_key *key,
                                const u8 *gtk, size_t gtk_len,
                                int key_info)
{
#ifndef CONFIG_NO_WPA2
    struct wpa_gtk_data gd;

    /*
     * IEEE Std 802.11i-2004 - 8.5.2 EAPOL-Key frames - Figure 43x
     * GTK KDE format:
     * KeyID[bits 0-1], Tx [bit 2], Reserved [bits 3-7]
     * Reserved [bits 0-7]
     * GTK
     */

    os_memset(&gd, 0, sizeof(gd));
    //wpa_hexdump_key(MSG_DEBUG, "RSN: received GTK in pairwise handshake",
    //      gtk, gtk_len);
    //
    if (gtk_len < 2 || gtk_len - 2 > sizeof(gd.gtk))
        return -1;

    gd.keyidx = gtk[0] & 0x3;
    gd.tx = wpa_supplicant_gtk_tx_bit_workaround(sm,
            !!(gtk[0] & BIT(2)));
    gtk += 2;
    gtk_len -= 2;

    os_memcpy(gd.gtk, gtk, gtk_len);
    gd.gtk_len = gtk_len;

    if (wpa_supplicant_check_group_cipher(sm->group_cipher,
                                          gtk_len, gtk_len,
                                          &gd.key_rsc_len, &gd.alg) ||
            wpa_supplicant_install_gtk(sm, &gd, key->key_rsc))
    {
        CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0),  TSTR("RSN: Failed to install GTK",0x08700062));
        return -1;
    }

    //wpa_supplicant_key_neg_complete(sm, sm->bssid,
    //          key_info & WPA_KEY_INFO_SECURE);
    return 0;
#else /* CONFIG_NO_WPA2 */
    return -1;
#endif /* CONFIG_NO_WPA2 */
}
static int wpa_supplicant_install_ptk(struct wpa_sm *sm,
                                      const struct wpa_eapol_key *key)
{



    int keylen, rsclen;
    wpa_alg alg;
    const u8 *key_rsc;
    u8 null_rsc[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

    CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0), TSTR("WPA: Installing PTK to the driver.",0x08700063));

    switch (sm->pairwise_cipher)
    {
        case WPA_CIPHER_CCMP:
            alg = WPA_ALG_CCMP;
            keylen = 16;
            rsclen = 6;
            break;
        case WPA_CIPHER_TKIP:
            alg = WPA_ALG_TKIP;
            keylen = 32;
            rsclen = 6;
            break;
        case WPA_CIPHER_NONE:
            CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0), TSTR("WPA: Pairwise Cipher Suite:NONE - do not use pairwise keys",0x08700064));
            return 0;
        default:
            CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(1), TSTR("WPA: Unsupported pairwise cipher %d",0x08700065),  sm->pairwise_cipher);
            return -1;
    }

    if (sm->proto == WPA_PROTO_RSN)
    {
        key_rsc = null_rsc;
    }
    else
    {
        key_rsc = key->key_rsc;
        //wpa_hexdump(MSG_DEBUG, "WPA: RSC", key_rsc, rsclen);
    }

    if(wpa_sm_set_ptk(sm->bssid, (u8 *) sm->ptk.tk1, keylen) < 0)
    {
        //if (wpa_sm_set_key(sm, alg, sm->bssid, 0, 1, key_rsc, rsclen,
        //         (u8 *) sm->ptk.tk1, keylen) < 0) {
        CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(3)|TSMAP(4), TSTR("WPA: Failed to set PTK to the driver (alg=%d keylen=%d bssid=%s)",0x08700066),
                  alg, keylen, (sm->bssid));
        return -1;
    }
    sm->keysettoMac |= (0x01);
    /*
        if (sm->wpa_ptk_rekey) {
            eloop_cancel_timeout(wpa_sm_rekey_ptk, sm, NULL);
            eloop_register_timeout(sm->wpa_ptk_rekey, 0, wpa_sm_rekey_ptk,
                           sm, NULL);
        }
    */
    return 0;
}
VOID ip_WifiConnectedInd(void);

static void wpa_supplicant_process_3_of_4(WPA_SM_T *sm,
        const struct wpa_eapol_key *key,
        u16 ver)
{
#if 1
    u16 key_info, keylen, len;
    const u8 *pos;
    struct wpa_eapol_ie_parse ie;

    //wpa_sm_set_state(sm, WPA_4WAY_HANDSHAKE);
    CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(2)|TSMAP(1), TSTR("WPA: RX message 3 of 4-Way Handshake from :%s (ver=%d)",0x08700067), (sm->bssid), ver);

    key_info = WPA_GET_BE16(key->key_info);

    pos = (const u8 *) (key + 1);
    len = WPA_GET_BE16(key->key_data_length);
    //  CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(2), TSTR("before wpa_supplicant_parse_ies, pos: 0x%x, len: %d",0x08700068),pos,len);
    //  wifi_dump(pos,len);

    //wpa_hexdump(MSG_DEBUG, "WPA: IE KeyData", pos, len);
    wpa_supplicant_parse_ies(pos, len, &ie);
    if (ie.gtk && !(key_info & WPA_KEY_INFO_ENCR_KEY_DATA))
    {
        CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0), TSTR("WPA: GTK IE in unencrypted key data",0x08700069));
        goto failed;
    }
#ifdef CONFIG_IEEE80211W
    if (ie.igtk && !(key_info & WPA_KEY_INFO_ENCR_KEY_DATA))
    {
        CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0),  TSTR("WPA: IGTK KDE in unencrypted key data",0x0870006a));
        goto failed;
    }

    if (ie.igtk && ie.igtk_len != sizeof(struct wpa_igtk_kde))
    {
        CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(1),  TSTR("WPA: Invalid IGTK KDE length %lu",0x0870006b),  (unsigned long) ie.igtk_len);
        goto failed;
    }
#endif /* CONFIG_IEEE80211W */

    //if (wpa_supplicant_validate_ie(sm, sm->bssid, &ie) < 0)
    //  goto failed;

    if (os_memcmp(sm->anonce, key->key_nonce, WPA_NONCE_LEN) != 0)
    {
        //CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0),  TSTR("WPA: ANonce from message 1 of 4-Way ",0x0870006c)
        //     "Handshake differs from 3 of 4-Way Handshake - drop"
        //     " packet (src=" MACSTR ")", MAC2STR(sm->bssid));
        goto failed;
    }

    keylen = WPA_GET_BE16(key->key_length);
    switch (sm->pairwise_cipher)
    {
        case WPA_CIPHER_CCMP:
            if (keylen != 16)
            {
                CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(1),  TSTR("WPA: Invalid CCMP key length %d (src=)",0x0870006d),  keylen, (sm->bssid));
                goto failed;
            }
            break;
        case WPA_CIPHER_TKIP:
            if (keylen != 32)
            {
                CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(2)|TSMAP(2),  TSTR("WPA: Invalid TKIP key length %d (src=%s",0x0870006e),  keylen, (sm->bssid));
                goto failed;
            }
            break;
    }

    if (wpa_supplicant_send_4_of_4(sm, sm->bssid, key, ver, key_info,
                                   NULL, 0, &sm->ptk))
    {
        goto failed;
    }

    /* SNonce was successfully used in msg 3/4, so mark it to be renewed
     * for the next 4-Way Handshake. If msg 3 is received again, the old
     * SNonce will still be used to avoid changing PTK. */
    sm->renew_snonce = 1;

    if (key_info & WPA_KEY_INFO_INSTALL)
    {
        if (wpa_supplicant_install_ptk(sm, key))
            goto failed;
    }

    if (key_info & WPA_KEY_INFO_SECURE)
    {
        //  wpa_sm_mlme_setprotection(
        //      sm, sm->bssid, MLME_SETPROTECTION_PROTECT_TYPE_RX,
        //      MLME_SETPROTECTION_KEY_TYPE_PAIRWISE);
        //  eapol_sm_notify_portValid(sm->eapol, TRUE);
    }
    //wpa_sm_set_state(sm, WPA_GROUP_HANDSHAKE);

    if (ie.gtk &&
            wpa_supplicant_pairwise_gtk(sm, key,
                                        ie.gtk, ie.gtk_len, key_info) < 0)
    {
        CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0),  TSTR("RSN: Failed to configure GTK",0x0870006f));
        goto failed;
    }

    if (ieee80211w_set_keys(sm, &ie) < 0)
    {
        CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0), TSTR("RSN: Failed to configure IGTK",0x08700070));
        goto failed;
    }

    CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(2),  TSTR("in wpa_supplicant_process_3_of_4, ie.gtk: 0x%x , len: %d",0x08700071), ie.gtk, ie.gtk_len);
    wifi_dump(ie.gtk, ie.gtk_len);
    CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(1),  TSTR("in wpa_supplicant_process_3_of_4, ketsettoMac: 0x%x ",0x08700072), sm->keysettoMac);

    if(sm->keysettoMac == 0x03)
    {
        ip_WifiConnectedInd();
    }
    return;

failed:
    return;
    //wpa_sm_deauthenticate(sm, WLAN_REASON_UNSPECIFIED);
#endif
}


static int wpa_supplicant_decrypt_key_data(WPA_SM_T *sm,
        struct wpa_eapol_key *key, u16 ver)
{
    CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0), TSTR("wpa_supplicant_decrypt_key_data begin",0x08700073));

#if 1
    u16 keydatalen = WPA_GET_BE16(key->key_data_length);

    //CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0),  TSTR("RSN: encrypted key data",0x08700074));
    //wifi_dump(  (u8 *) (key + 1), keydatalen);
    if (!sm->ptk_set)
    {
        CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0),  TSTR("WPA: PTK not available, cannot decrypt EAPOL-Key key data.",0x08700075));
        return -1;
    }

    /* Decrypt key data here so that this operation does not need
     * to be implemented separately for each message type. */
    if (ver == WPA_KEY_INFO_TYPE_HMAC_MD5_RC4)
    {
        u8 ek[32];
        os_memcpy(ek, key->key_iv, 16);
        os_memcpy(ek + 16, sm->ptk.kek, 16);
        rc4_skip(ek, 32, 256, (u8 *) (key + 1), keydatalen);
    }
    else if (ver == WPA_KEY_INFO_TYPE_HMAC_SHA1_AES ||
             ver == WPA_KEY_INFO_TYPE_AES_128_CMAC)
    {
        u8 *buf;
        if (keydatalen % 8)
        {
            CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(1), TSTR("WPA: Unsupported AES-WRAP len %d",0x08700076), keydatalen);
            return -1;
        }
        keydatalen -= 8; /* AES-WRAP adds 8 bytes */
        buf = os_malloc(keydatalen);
        if (buf == NULL)
        {
            CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0),  TSTR("WPA: No memory for AES-UNWRAP buffer",0x08700077));
            return -1;
        }
        if (aes_unwrap(sm->ptk.kek, keydatalen / 8,
                       (u8 *) (key + 1), buf))
        {
            os_free(buf);
            CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0), TSTR("WPA: AES unwrap failed - could not decrypt EAPOL-Key key data",0x08700078));
            return -1;
        }
        os_memcpy(key + 1, buf, keydatalen);
        os_free(buf);
        WPA_PUT_BE16(key->key_data_length, keydatalen);
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(1), TSTR("WPA: Unsupported key_info type %d",0x08700079),  ver);
        return -1;
    }
    //CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0),  TSTR("WPA: decrypted EAPOL-Key key data",0x0870007a));
    //  wifi_dump(  (u8 *) (key + 1), keydatalen);
#endif
    return 0;
}


int wpa_sm_rx_eapol( WPA_SM_T *sm, const u8 *src_addr, const u8 *buf, size_t len)
{
    size_t plen, data_len, extra_len;
    struct ieee802_1x_hdr *hdr;
    struct wpa_eapol_key *key;
    u16 key_info, ver;
    u8 *tmp;
    int ret = -1;
    struct wpa_peerkey *peerkey = NULL;

#ifdef CONFIG_IEEE80211R
    sm->ft_completed = 0;
#endif /* CONFIG_IEEE80211R */

    if (len < sizeof(struct ieee802_1x_hdr) + sizeof(struct wpa_eapol_key ))
    {
        CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(2), TSTR("WPA: EAPOL frame too short to be a WPA ,EAPOL-Key (len %lu, expecting at least %lu)",0x0870007b),
                  (unsigned long) len,
                  (unsigned long) sizeof(struct ieee802_1x_hdr) + sizeof(struct wpa_eapol_key ));
        return 0;
    }

    tmp = COS_MALLOC(len);
    if (tmp == NULL)
        return -1;
    memcpy(tmp, buf, len);

    hdr = (struct ieee802_1x_hdr *) tmp;
    key = (struct wpa_eapol_key *) (hdr + 1);
    plen = ntohs(hdr->length);
    data_len = plen + sizeof(*hdr);
    CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(4), TSTR("IEEE 802.1X RX: version=%d type=%d length=%lu, buf len:%d",0x0870007c),  hdr->version, hdr->type, (unsigned long) plen, len);

    if (hdr->version < EAPOL_VERSION)
    {
        /* TODO: backwards compatibility */
    }
    if (hdr->type != IEEE802_1X_TYPE_EAPOL_KEY)
    {
        CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(1), TSTR("WPA: EAPOL frame (type %u) discarded, not a Key frame",0x0870007d), hdr->type);
        ret = 0;
        goto out;
    }
    if (plen > len - sizeof(*hdr) || plen < sizeof(*key))
    {
        CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(2), TSTR("WPA: EAPOL frame payload size %lu ,invalid (frame size %lu)",0x0870007e),
                  (unsigned long) plen, (unsigned long) len);
        ret = 0;
        goto out;
    }

    if (key->type != EAPOL_KEY_TYPE_WPA && key->type != EAPOL_KEY_TYPE_RSN)
    {
        CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(1), TSTR("WPA: EAPOL-Key type (%d) unknown,discarded",0x0870007f), key->type);
        ret = 0;
        goto out;
    }
    //wpa_eapol_key_dump(key);

    //  eapol_sm_notify_lower_layer_success(sm->eapol, 0);
    //wpa_hexdump(MSG_MSGDUMP, "WPA: RX EAPOL-Key", tmp, len);
    if (data_len < len)
    {
        CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(1), TSTR("WPA: ignoring %lu bytes after the IEEE 802.1X data",0x08700080), (unsigned long) len - data_len);
    }
    key_info = WPA_GET_BE16(key->key_info);
    ver = key_info & WPA_KEY_INFO_TYPE_MASK;
    if (ver != WPA_KEY_INFO_TYPE_HMAC_MD5_RC4 &&
#if defined(CONFIG_IEEE80211R) || defined(CONFIG_IEEE80211W)
            ver != WPA_KEY_INFO_TYPE_AES_128_CMAC &&
#endif /* CONFIG_IEEE80211R || CONFIG_IEEE80211W */
            ver != WPA_KEY_INFO_TYPE_HMAC_SHA1_AES)
    {
#define WPA_GET_BE16(a) ((u16) (((a)[0] << 8) | (a)[1])) "WPA: Unsupported EAPOL-Key descriptor version %d.", ver);
        goto out;
    }

#ifdef CONFIG_IEEE80211R
    if (wpa_key_mgmt_ft(sm->key_mgmt))
    {
        /* IEEE 802.11r uses a new key_info type (AES-128-CMAC). */
        if (ver != WPA_KEY_INFO_TYPE_AES_128_CMAC)
        {
            wpa_printf(MSG_INFO, "FT: AP did not use "
                       "AES-128-CMAC.");
            goto out;
        }
    }
    else
#endif /* CONFIG_IEEE80211R */
#ifdef CONFIG_IEEE80211W
        if (wpa_key_mgmt_sha256(sm->key_mgmt))
        {
            if (ver != WPA_KEY_INFO_TYPE_AES_128_CMAC)
            {
                wpa_printf(MSG_INFO, "WPA: AP did not use the "
                           "negotiated AES-128-CMAC.");
                goto out;
            }
        }
        else
#endif /* CONFIG_IEEE80211W */
#if 0
            if (sm->pairwise_cipher == WPA_CIPHER_CCMP &&
                    ver != WPA_KEY_INFO_TYPE_HMAC_SHA1_AES)
            {
                CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(1), TSTR("WPA: CCMP is used, but EAPOL-Key descriptor version (%d) is not 2.",0x08700081), ver);
                if (sm->group_cipher != WPA_CIPHER_CCMP &&
                        !(key_info & WPA_KEY_INFO_KEY_TYPE))
                {
                    /* Earlier versions of IEEE 802.11i did not explicitly
                     * require version 2 descriptor for all EAPOL-Key
                     * packets, so allow group keys to use version 1 if
                     * CCMP is not used for them. */
                    wpa_printf(MSG_INFO, "WPA: Backwards compatibility: "
                               "allow invalid version for non-CCMP group "
                               "keys");
                }
                else
                    goto out;
            }
#endif
#ifdef CONFIG_PEERKEY
    for (peerkey = sm->peerkey; peerkey; peerkey = peerkey->next)
    {
        if (os_memcmp(peerkey->addr, src_addr, ETH_ALEN) == 0)
            break;
    }

    if (!(key_info & WPA_KEY_INFO_SMK_MESSAGE) && peerkey)
    {
        if (!peerkey->initiator && peerkey->replay_counter_set &&
                os_memcmp(key->replay_counter, peerkey->replay_counter,
                          WPA_REPLAY_COUNTER_LEN) <= 0)
        {
            wpa_printf(MSG_WARNING, "RSN: EAPOL-Key Replay "
                       "Counter did not increase (STK) - dropping "
                       "packet");
            goto out;
        }
        else if (peerkey->initiator)
        {
            u8 _tmp[WPA_REPLAY_COUNTER_LEN];
            os_memcpy(_tmp, key->replay_counter,
                      WPA_REPLAY_COUNTER_LEN);
            inc_byte_array(_tmp, WPA_REPLAY_COUNTER_LEN);
            if (os_memcmp(_tmp, peerkey->replay_counter,
                          WPA_REPLAY_COUNTER_LEN) != 0)
            {
                wpa_printf(MSG_DEBUG, "RSN: EAPOL-Key Replay "
                           "Counter did not match (STK) - "
                           "dropping packet");
                goto out;
            }
        }
    }

    if (peerkey && peerkey->initiator && (key_info & WPA_KEY_INFO_ACK))
    {
        wpa_printf(MSG_INFO, "RSN: Ack bit in key_info from STK peer");
        goto out;
    }
#endif /* CONFIG_PEERKEY */
    /*
        if (!peerkey && sm->rx_replay_counter_set &&
            os_memcmp(key->replay_counter, sm->rx_replay_counter,
                  WPA_REPLAY_COUNTER_LEN) <= 0) {
            wpa_printf(MSG_WARNING, "WPA: EAPOL-Key Replay Counter did not"
                   " increase - dropping packet");
            goto out;
        }
    */
    if (!(key_info & (WPA_KEY_INFO_ACK | WPA_KEY_INFO_SMK_MESSAGE))
#ifdef CONFIG_PEERKEY
            && (peerkey == NULL || !peerkey->initiator)
#endif /* CONFIG_PEERKEY */
       )
    {
        CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0), TSTR("WPA: No Ack bit in key_info",0x08700082));
        goto out;
    }

    if (key_info & WPA_KEY_INFO_REQUEST)
    {
        CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0), TSTR("WPA: EAPOL-Key with Request bit - dropped",0x08700083));
        goto out;
    }

    if ((key_info & WPA_KEY_INFO_MIC) && !peerkey &&
            wpa_supplicant_verify_eapol_key_mic(sm, key, ver, tmp, data_len))
        goto out;

#ifdef CONFIG_PEERKEY
    if ((key_info & WPA_KEY_INFO_MIC) && peerkey &&
            peerkey_verify_eapol_key_mic(sm, peerkey, key, ver, tmp, data_len))
        goto out;
#endif /* CONFIG_PEERKEY */

    extra_len = data_len - sizeof(*hdr) - sizeof(*key);
    UINT16 extra_lent = ((key->key_data_length)[0] << 8) | (key->key_data_length)[1];
    if (extra_lent > extra_len)
    {

        //if (WPA_GET_BE16(key->key_data_length) > extra_len) {
        //wpa_msg(sm->ctx->ctx, MSG_INFO, "WPA: Invalid EAPOL-Key "
        //  "frame - key_data overflow (%d > %lu)",
        //  WPA_GET_BE16(key->key_data_length),
        //  (unsigned long) extra_len);
        goto out;
    }
    extra_len = ((key->key_data_length)[0] << 8) | (key->key_data_length)[1];
    //extra_len = WPA_GET_BE16(key->key_data_length);

    if (sm->proto == WPA_PROTO_RSN &&
            (key_info & WPA_KEY_INFO_ENCR_KEY_DATA))
    {
        if (wpa_supplicant_decrypt_key_data(sm, key, ver))
            goto out;
        //extra_len = WPA_GET_BE16(key->key_data_length);
        extra_len = (key->key_data_length[0] << 8) | key->key_data_length[1];

    }

    if (key_info & WPA_KEY_INFO_KEY_TYPE)
    {
        if (key_info & WPA_KEY_INFO_KEY_INDEX_MASK)
        {
            CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0), TSTR("WPA: Ignored EAPOL-Key (Pairwise) with non-zero key index",0x08700084));
            goto out;
        }
        if (peerkey)
        {
            /* PeerKey 4-Way Handshake */
            //peerkey_rx_eapol_4way(sm, peerkey, key, key_info, ver);
        }
        else if (key_info & WPA_KEY_INFO_MIC)
        {
            /* 3/4 4-Way Handshake */
            wpa_supplicant_process_3_of_4(sm, key, ver);
        }
        else
        {
            /* 1/4 4-Way Handshake */
            wpa_supplicant_process_1_of_4(sm, src_addr, key,
                                          ver);
        }
    }
    else if (key_info & WPA_KEY_INFO_SMK_MESSAGE)
    {
        /* PeerKey SMK Handshake */
        peerkey_rx_eapol_smk(sm, src_addr, key, extra_len, key_info,
                             ver);
    }
    else
    {
        if (key_info & WPA_KEY_INFO_MIC)
        {
            /* 1/2 Group Key Handshake */
            wpa_supplicant_process_1_of_2(sm, src_addr, key,
                                          extra_len, ver);
        }
        else
        {
            CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0), TSTR("WPA: EAPOL-Key (Group) without Mic bit - dropped",0x08700085));
        }
    }

    ret = 1;

out:
    os_free(tmp);
    return ret;
}

void pbkdf2_sha1(const char *passphrase, const char *ssid, size_t ssid_len,
                 int iterations, u8 *buf, size_t buflen);

void get_assoRsnIe(UINT8 *ie, UINT8 *len);
extern  UINT32 startTime_ex ;
VOID setWifiSmPmk(UINT8 *psswd, UINT8 len)
{
    CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(1)|TSMAP(1), TSTR("WPA: set setWifiSmPmk, pass: %s",0x08700086), psswd);
    UINT32 startTime ;

    memset(sm_cxt.pmk, 0x0, PMK_LEN);
    CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(2)|TSMAP(1), TSTR("WPA: set setWifiSmPmk, ssid: %s, len: %d",0x08700087), sm_cxt.ssid,  strlen(sm_cxt.ssid));
    wifi_dump(sm_cxt.ssid, strlen(sm_cxt.ssid));
    wifi_dump(psswd, len);

    startTime_ex = hal_TimGetUpTime();

    pbkdf2_sha1(psswd, sm_cxt.ssid, strlen(sm_cxt.ssid), 4096, sm_cxt.pmk, PMK_LEN);
    startTime_ex = hal_TimGetUpTime() - startTime_ex;

    //  CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(1), TSTR("WPA: set setWifiSmPmk, %d, pmk:",0x08700088),startTime);
    wifi_dump(sm_cxt.pmk, PMK_LEN);
    sm_cxt.pmk_len = PMK_LEN;
    //memcpy(sm_cxt.pmk, psswd,len);

}

void initWifiWapSm(void)
{
    memset(&sm_cxt, 0x00, sizeof(WPA_SM_T));
    sm_cxt.renew_snonce = 1;
    //init_randomBuf();
    init_randomBufEx();
    CSW_TRACE(_CSW|TLEVEL(9)|TDB|TNB_ARG(0), TSTR("WPA: set init_randomBuf:",0x08700089));
    wifi_dump(random_buf, 128);


}
void get_assoWpaIe(UINT8 *ie, UINT8 *len);
void setWifiSmField(u8 *wifi_password)
{
    setWifiSmPmk(wifi_password, strlen(wifi_password));




}
void wifi_inputEapolPkt(const u8 *src_addr, const u8 *buf, size_t len)
{
    wpa_sm_rx_eapol( &sm_cxt, src_addr, buf, len);

}


void test_alg(void)
{

}
#if 0

#include "includes.h"

#include "common.h"
#include "rc4.h"
#include "aes_wrap.h"
#include "wpa.h"
#include "eloop.h"
#include "eapol_supp/eapol_supp_sm.h"
#include "preauth.h"
#include "pmksa_cache.h"
#include "wpa_i.h"
#include "wpa_ie.h"
#include "peerkey.h"
#include "ieee802_11_defs.h"


/**
 * wpa_cipher_txt - Convert cipher suite to a text string
 * @cipher: Cipher suite (WPA_CIPHER_* enum)
 * Returns: Pointer to a text string of the cipher suite name
 */
static const char *wpa_cipher_txt(int cipher)
{
    switch (cipher)
    {
        case WPA_CIPHER_NONE:
            return "NONE";
        case WPA_CIPHER_WEP40:
            return "WEP-40";
        case WPA_CIPHER_WEP104:
            return "WEP-104";
        case WPA_CIPHER_TKIP:
            return "TKIP";
        case WPA_CIPHER_CCMP:
            return "CCMP";
        default:
            return "UNKNOWN";
    }
}


/**
 * wpa_key_mgmt_txt - Convert key management suite to a text string
 * @key_mgmt: Key management suite (WPA_KEY_MGMT_* enum)
 * @proto: WPA/WPA2 version (WPA_PROTO_*)
 * Returns: Pointer to a text string of the key management suite name
 */
static const char *wpa_key_mgmt_txt(int key_mgmt, int proto)
{
    switch (key_mgmt)
    {
        case WPA_KEY_MGMT_IEEE8021X:
            return proto == WPA_PROTO_RSN ?
                   "WPA2/IEEE 802.1X/EAP" : "WPA/IEEE 802.1X/EAP";
        case WPA_KEY_MGMT_PSK:
            return proto == WPA_PROTO_RSN ?
                   "WPA2-PSK" : "WPA-PSK";
        case WPA_KEY_MGMT_NONE:
            return "NONE";
        case WPA_KEY_MGMT_IEEE8021X_NO_WPA:
            return "IEEE 802.1X (no WPA)";
#ifdef CONFIG_IEEE80211R
        case WPA_KEY_MGMT_FT_IEEE8021X:
            return "FT-EAP";
        case WPA_KEY_MGMT_FT_PSK:
            return "FT-PSK";
#endif /* CONFIG_IEEE80211R */
#ifdef CONFIG_IEEE80211W
        case WPA_KEY_MGMT_IEEE8021X_SHA256:
            return "WPA2-EAP-SHA256";
        case WPA_KEY_MGMT_PSK_SHA256:
            return "WPA2-PSK-SHA256";
#endif /* CONFIG_IEEE80211W */
        default:
            return "UNKNOWN";
    }
}


/**
 * wpa_eapol_key_send - Send WPA/RSN EAPOL-Key message
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @kck: Key Confirmation Key (KCK, part of PTK)
 * @ver: Version field from Key Info
 * @dest: Destination address for the frame
 * @proto: Ethertype (usually ETH_P_EAPOL)
 * @msg: EAPOL-Key message
 * @msg_len: Length of message
 * @key_mic: Pointer to the buffer to which the EAPOL-Key MIC is written
 */
void wpa_eapol_key_send(struct wpa_sm *sm, const u8 *kck,
                        int ver, const u8 *dest, u16 proto,
                        u8 *msg, size_t msg_len, u8 *key_mic)
{
    if (is_zero_ether_addr(dest) && is_zero_ether_addr(sm->bssid))
    {
        /*
         * Association event was not yet received; try to fetch
         * BSSID from the driver.
         */
        if (wpa_sm_get_bssid(sm, sm->bssid) < 0)
        {
            wpa_printf(MSG_DEBUG, "WPA: Failed to read BSSID for "
                       "EAPOL-Key destination address");
        }
        else
        {
            dest = sm->bssid;
            wpa_printf(MSG_DEBUG, "WPA: Use BSSID (" MACSTR
                       ") as the destination for EAPOL-Key",
                       MAC2STR(dest));
        }
    }
    if (key_mic)
        wpa_eapol_key_mic(kck, ver, msg, msg_len, key_mic);
    wpa_hexdump(MSG_MSGDUMP, "WPA: TX EAPOL-Key", msg, msg_len);
    wpa_sm_ether_send(sm, dest, proto, msg, msg_len);
    eapol_sm_notify_tx_eapol_key(sm->eapol);
    os_free(msg);
}


/**
 * wpa_sm_key_request - Send EAPOL-Key Request
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @error: Indicate whether this is an Michael MIC error report
 * @pairwise: 1 = error report for pairwise packet, 0 = for group packet
 *
 * Send an EAPOL-Key Request to the current authenticator. This function is
 * used to request rekeying and it is usually called when a local Michael MIC
 * failure is detected.
 */
void wpa_sm_key_request(struct wpa_sm *sm, int error, int pairwise)
{
    size_t rlen;
    struct wpa_eapol_key *reply;
    int key_info, ver;
    u8 bssid[ETH_ALEN], *rbuf;

    if (wpa_key_mgmt_ft(sm->key_mgmt) || wpa_key_mgmt_sha256(sm->key_mgmt))
        ver = WPA_KEY_INFO_TYPE_AES_128_CMAC;
    else if (sm->pairwise_cipher == WPA_CIPHER_CCMP)
        ver = WPA_KEY_INFO_TYPE_HMAC_SHA1_AES;
    else
        ver = WPA_KEY_INFO_TYPE_HMAC_MD5_RC4;

    if (wpa_sm_get_bssid(sm, bssid) < 0)
    {
        wpa_printf(MSG_WARNING, "Failed to read BSSID for EAPOL-Key "
                   "request");
        return;
    }

    rbuf = wpa_sm_alloc_eapol(sm, IEEE802_1X_TYPE_EAPOL_KEY, NULL,
                              sizeof(*reply), &rlen, (void *) &reply);
    if (rbuf == NULL)
        return;

    reply->type = sm->proto == WPA_PROTO_RSN ?
                  EAPOL_KEY_TYPE_RSN : EAPOL_KEY_TYPE_WPA;
    key_info = WPA_KEY_INFO_REQUEST | ver;
    if (sm->ptk_set)
        key_info |= WPA_KEY_INFO_MIC;
    if (error)
        key_info |= WPA_KEY_INFO_ERROR;
    if (pairwise)
        key_info |= WPA_KEY_INFO_KEY_TYPE;
    WPA_PUT_BE16(reply->key_info, key_info);
    WPA_PUT_BE16(reply->key_length, 0);
    os_memcpy(reply->replay_counter, sm->request_counter,
              WPA_REPLAY_COUNTER_LEN);
    inc_byte_array(sm->request_counter, WPA_REPLAY_COUNTER_LEN);

    WPA_PUT_BE16(reply->key_data_length, 0);

    wpa_printf(MSG_INFO, "WPA: Sending EAPOL-Key Request (error=%d "
               "pairwise=%d ptk_set=%d len=%lu)",
               error, pairwise, sm->ptk_set, (unsigned long) rlen);
    wpa_eapol_key_send(sm, sm->ptk.kck, ver, bssid, ETH_P_EAPOL,
                       rbuf, rlen, key_info & WPA_KEY_INFO_MIC ?
                       reply->key_mic : NULL);
}


static int wpa_supplicant_get_pmk(struct wpa_sm *sm,
                                  const unsigned char *src_addr,
                                  const u8 *pmkid)
{
    int abort_cached = 0;

    if (pmkid && !sm->cur_pmksa)
    {
        /* When using drivers that generate RSN IE, wpa_supplicant may
         * not have enough time to get the association information
         * event before receiving this 1/4 message, so try to find a
         * matching PMKSA cache entry here. */
        sm->cur_pmksa = pmksa_cache_get(sm->pmksa, src_addr, pmkid);
        if (sm->cur_pmksa)
        {
            wpa_printf(MSG_DEBUG, "RSN: found matching PMKID from "
                       "PMKSA cache");
        }
        else
        {
            wpa_printf(MSG_DEBUG, "RSN: no matching PMKID found");
            abort_cached = 1;
        }
    }

    if (pmkid && sm->cur_pmksa &&
            os_memcmp(pmkid, sm->cur_pmksa->pmkid, PMKID_LEN) == 0)
    {
        wpa_hexdump(MSG_DEBUG, "RSN: matched PMKID", pmkid, PMKID_LEN);
        wpa_sm_set_pmk_from_pmksa(sm);
        wpa_hexdump_key(MSG_DEBUG, "RSN: PMK from PMKSA cache",
                        sm->pmk, sm->pmk_len);
        eapol_sm_notify_cached(sm->eapol);
#ifdef CONFIG_IEEE80211R
        sm->xxkey_len = 0;
#endif /* CONFIG_IEEE80211R */
    }
    else if (wpa_key_mgmt_wpa_ieee8021x(sm->key_mgmt) && sm->eapol)
    {
        int res, pmk_len;
        pmk_len = PMK_LEN;
        res = eapol_sm_get_key(sm->eapol, sm->pmk, PMK_LEN);
        if (res)
        {
            /*
             * EAP-LEAP is an exception from other EAP methods: it
             * uses only 16-byte PMK.
             */
            res = eapol_sm_get_key(sm->eapol, sm->pmk, 16);
            pmk_len = 16;
        }
        else
        {
#ifdef CONFIG_IEEE80211R
            u8 buf[2 * PMK_LEN];
            if (eapol_sm_get_key(sm->eapol, buf, 2 * PMK_LEN) == 0)
            {
                os_memcpy(sm->xxkey, buf + PMK_LEN, PMK_LEN);
                sm->xxkey_len = PMK_LEN;
                os_memset(buf, 0, sizeof(buf));
            }
#endif /* CONFIG_IEEE80211R */
        }
        if (res == 0)
        {
            wpa_hexdump_key(MSG_DEBUG, "WPA: PMK from EAPOL state "
                            "machines", sm->pmk, pmk_len);
            sm->pmk_len = pmk_len;
            pmksa_cache_add(sm->pmksa, sm->pmk, pmk_len, src_addr,
                            sm->own_addr, sm->network_ctx,
                            sm->key_mgmt);
            if (!sm->cur_pmksa && pmkid &&
                    pmksa_cache_get(sm->pmksa, src_addr, pmkid))
            {
                wpa_printf(MSG_DEBUG, "RSN: the new PMK "
                           "matches with the PMKID");
                abort_cached = 0;
            }
        }
        else
        {
            wpa_msg(sm->ctx->ctx, MSG_WARNING,
                    "WPA: Failed to get master session key from "
                    "EAPOL state machines");
            wpa_msg(sm->ctx->ctx, MSG_WARNING,
                    "WPA: Key handshake aborted");
            if (sm->cur_pmksa)
            {
                wpa_printf(MSG_DEBUG, "RSN: Cancelled PMKSA "
                           "caching attempt");
                sm->cur_pmksa = NULL;
                abort_cached = 1;
            }
            else if (!abort_cached)
            {
                return -1;
            }
        }
    }

    if (abort_cached && wpa_key_mgmt_wpa_ieee8021x(sm->key_mgmt))
    {
        /* Send EAPOL-Start to trigger full EAP authentication. */
        u8 *buf;
        size_t buflen;

        wpa_printf(MSG_DEBUG, "RSN: no PMKSA entry found - trigger "
                   "full EAP authentication");
        buf = wpa_sm_alloc_eapol(sm, IEEE802_1X_TYPE_EAPOL_START,
                                 NULL, 0, &buflen, NULL);
        if (buf)
        {
            wpa_sm_ether_send(sm, sm->bssid, ETH_P_EAPOL,
                              buf, buflen);
            os_free(buf);
        }

        return -1;
    }

    return 0;
}


/**
 * wpa_supplicant_send_2_of_4 - Send message 2 of WPA/RSN 4-Way Handshake
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @dst: Destination address for the frame
 * @key: Pointer to the EAPOL-Key frame header
 * @ver: Version bits from EAPOL-Key Key Info
 * @nonce: Nonce value for the EAPOL-Key frame
 * @wpa_ie: WPA/RSN IE
 * @wpa_ie_len: Length of the WPA/RSN IE
 * @ptk: PTK to use for keyed hash and encryption
 * Returns: 0 on success, -1 on failure
 */
int wpa_supplicant_send_2_of_4(struct wpa_sm *sm, const unsigned char *dst,
                               const struct wpa_eapol_key *key,
                               int ver, const u8 *nonce,
                               const u8 *wpa_ie, size_t wpa_ie_len,
                               struct wpa_ptk *ptk)
{
    size_t rlen;
    struct wpa_eapol_key *reply;
    u8 *rbuf;

    if (wpa_ie == NULL)
    {
        wpa_printf(MSG_WARNING, "WPA: No wpa_ie set - cannot "
                   "generate msg 2/4");
        return -1;
    }

    wpa_hexdump(MSG_DEBUG, "WPA: WPA IE for msg 2/4", wpa_ie, wpa_ie_len);

    rbuf = wpa_sm_alloc_eapol(sm, IEEE802_1X_TYPE_EAPOL_KEY,
                              NULL, sizeof(*reply) + wpa_ie_len,
                              &rlen, (void *) &reply);
    if (rbuf == NULL)
        return -1;

    reply->type = sm->proto == WPA_PROTO_RSN ?
                  EAPOL_KEY_TYPE_RSN : EAPOL_KEY_TYPE_WPA;
    WPA_PUT_BE16(reply->key_info,
                 ver | WPA_KEY_INFO_KEY_TYPE | WPA_KEY_INFO_MIC);
    if (sm->proto == WPA_PROTO_RSN)
        WPA_PUT_BE16(reply->key_length, 0);
    else
        os_memcpy(reply->key_length, key->key_length, 2);
    os_memcpy(reply->replay_counter, key->replay_counter,
              WPA_REPLAY_COUNTER_LEN);

    WPA_PUT_BE16(reply->key_data_length, wpa_ie_len);
    os_memcpy(reply + 1, wpa_ie, wpa_ie_len);

    os_memcpy(reply->key_nonce, nonce, WPA_NONCE_LEN);

    wpa_printf(MSG_DEBUG, "WPA: Sending EAPOL-Key 2/4");
    wpa_eapol_key_send(sm, ptk->kck, ver, dst, ETH_P_EAPOL,
                       rbuf, rlen, reply->key_mic);

    return 0;
}




static void wpa_sm_start_preauth(void *eloop_ctx, void *timeout_ctx)
{
    struct wpa_sm *sm = eloop_ctx;
    rsn_preauth_candidate_process(sm);
}


static void wpa_supplicant_key_neg_complete(struct wpa_sm *sm,
        const u8 *addr, int secure)
{
    wpa_msg(sm->ctx->ctx, MSG_INFO, "WPA: Key negotiation completed with "
            MACSTR " [PTK=%s GTK=%s]", MAC2STR(addr),
            wpa_cipher_txt(sm->pairwise_cipher),
            wpa_cipher_txt(sm->group_cipher));
    wpa_sm_cancel_auth_timeout(sm);
    wpa_sm_set_state(sm, WPA_COMPLETED);

    if (secure)
    {
        wpa_sm_mlme_setprotection(
            sm, addr, MLME_SETPROTECTION_PROTECT_TYPE_RX_TX,
            MLME_SETPROTECTION_KEY_TYPE_PAIRWISE);
        eapol_sm_notify_portValid(sm->eapol, TRUE);
        if (wpa_key_mgmt_wpa_psk(sm->key_mgmt))
            eapol_sm_notify_eap_success(sm->eapol, TRUE);
        /*
         * Start preauthentication after a short wait to avoid a
         * possible race condition between the data receive and key
         * configuration after the 4-Way Handshake. This increases the
         * likelyhood of the first preauth EAPOL-Start frame getting to
         * the target AP.
         */
        eloop_register_timeout(1, 0, wpa_sm_start_preauth, sm, NULL);
    }

    if (sm->cur_pmksa && sm->cur_pmksa->opportunistic)
    {
        wpa_printf(MSG_DEBUG, "RSN: Authenticator accepted "
                   "opportunistic PMKSA entry - marking it valid");
        sm->cur_pmksa->opportunistic = 0;
    }

#ifdef CONFIG_IEEE80211R
    if (wpa_key_mgmt_ft(sm->key_mgmt))
    {
        /* Prepare for the next transition */
        wpa_ft_prepare_auth_request(sm);
    }
#endif /* CONFIG_IEEE80211R */
}


static void wpa_sm_rekey_ptk(void *eloop_ctx, void *timeout_ctx)
{
    struct wpa_sm *sm = eloop_ctx;
    wpa_printf(MSG_DEBUG, "WPA: Request PTK rekeying");
    wpa_sm_key_request(sm, 0, 1);
}


static int wpa_supplicant_install_ptk(struct wpa_sm *sm,
                                      const struct wpa_eapol_key *key)
{
    int keylen, rsclen;
    wpa_alg alg;
    const u8 *key_rsc;
    u8 null_rsc[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

    wpa_printf(MSG_DEBUG, "WPA: Installing PTK to the driver.");

    switch (sm->pairwise_cipher)
    {
        case WPA_CIPHER_CCMP:
            alg = WPA_ALG_CCMP;
            keylen = 16;
            rsclen = 6;
            break;
        case WPA_CIPHER_TKIP:
            alg = WPA_ALG_TKIP;
            keylen = 32;
            rsclen = 6;
            break;
        case WPA_CIPHER_NONE:
            wpa_printf(MSG_DEBUG, "WPA: Pairwise Cipher Suite: "
                       "NONE - do not use pairwise keys");
            return 0;
        default:
            wpa_printf(MSG_WARNING, "WPA: Unsupported pairwise cipher %d",
                       sm->pairwise_cipher);
            return -1;
    }

    if (sm->proto == WPA_PROTO_RSN)
    {
        key_rsc = null_rsc;
    }
    else
    {
        key_rsc = key->key_rsc;
        wpa_hexdump(MSG_DEBUG, "WPA: RSC", key_rsc, rsclen);
    }

    if (wpa_sm_set_key(sm, alg, sm->bssid, 0, 1, key_rsc, rsclen,
                       (u8 *) sm->ptk.tk1, keylen) < 0)
    {
        wpa_printf(MSG_WARNING, "WPA: Failed to set PTK to the "
                   "driver (alg=%d keylen=%d bssid=" MACSTR ")",
                   alg, keylen, MAC2STR(sm->bssid));
        return -1;
    }

    if (sm->wpa_ptk_rekey)
    {
        eloop_cancel_timeout(wpa_sm_rekey_ptk, sm, NULL);
        eloop_register_timeout(sm->wpa_ptk_rekey, 0, wpa_sm_rekey_ptk,
                               sm, NULL);
    }

    return 0;
}


static int wpa_supplicant_check_group_cipher(int group_cipher,
        int keylen, int maxkeylen,
        int *key_rsc_len, wpa_alg *alg)
{
    int ret = 0;

    switch (group_cipher)
    {
        case WPA_CIPHER_CCMP:
            if (keylen != 16 || maxkeylen < 16)
            {
                ret = -1;
                break;
            }
            *key_rsc_len = 6;
            *alg = WPA_ALG_CCMP;
            break;
        case WPA_CIPHER_TKIP:
            if (keylen != 32 || maxkeylen < 32)
            {
                ret = -1;
                break;
            }
            *key_rsc_len = 6;
            *alg = WPA_ALG_TKIP;
            break;
        case WPA_CIPHER_WEP104:
            if (keylen != 13 || maxkeylen < 13)
            {
                ret = -1;
                break;
            }
            *key_rsc_len = 0;
            *alg = WPA_ALG_WEP;
            break;
        case WPA_CIPHER_WEP40:
            if (keylen != 5 || maxkeylen < 5)
            {
                ret = -1;
                break;
            }
            *key_rsc_len = 0;
            *alg = WPA_ALG_WEP;
            break;
        default:
            wpa_printf(MSG_WARNING, "WPA: Unsupported Group Cipher %d",
                       group_cipher);
            return -1;
    }

    if (ret < 0 )
    {
        wpa_printf(MSG_WARNING, "WPA: Unsupported %s Group Cipher key "
                   "length %d (%d).",
                   wpa_cipher_txt(group_cipher), keylen, maxkeylen);
    }

    return ret;
}


struct wpa_gtk_data
{
    wpa_alg alg;
    int tx, key_rsc_len, keyidx;
    u8 gtk[32];
    int gtk_len;
};


static int wpa_supplicant_install_gtk(struct wpa_sm *sm,
                                      const struct wpa_gtk_data *gd,
                                      const u8 *key_rsc)
{
    const u8 *_gtk = gd->gtk;
    u8 gtk_buf[32];

    wpa_hexdump_key(MSG_DEBUG, "WPA: Group Key", gd->gtk, gd->gtk_len);
    wpa_printf(MSG_DEBUG, "WPA: Installing GTK to the driver "
               "(keyidx=%d tx=%d len=%d).", gd->keyidx, gd->tx,
               gd->gtk_len);
    wpa_hexdump(MSG_DEBUG, "WPA: RSC", key_rsc, gd->key_rsc_len);
    if (sm->group_cipher == WPA_CIPHER_TKIP)
    {
        /* Swap Tx/Rx keys for Michael MIC */
        os_memcpy(gtk_buf, gd->gtk, 16);
        os_memcpy(gtk_buf + 16, gd->gtk + 24, 8);
        os_memcpy(gtk_buf + 24, gd->gtk + 16, 8);
        _gtk = gtk_buf;
    }
    if (sm->pairwise_cipher == WPA_CIPHER_NONE)
    {
        if (wpa_sm_set_key(sm, gd->alg,
                           (u8 *) "\xff\xff\xff\xff\xff\xff",
                           gd->keyidx, 1, key_rsc, gd->key_rsc_len,
                           _gtk, gd->gtk_len) < 0)
        {
            wpa_printf(MSG_WARNING, "WPA: Failed to set "
                       "GTK to the driver (Group only).");
            return -1;
        }
    }
    else if (wpa_sm_set_key(sm, gd->alg,
                            (u8 *) "\xff\xff\xff\xff\xff\xff",
                            gd->keyidx, gd->tx, key_rsc, gd->key_rsc_len,
                            _gtk, gd->gtk_len) < 0)
    {
        wpa_printf(MSG_WARNING, "WPA: Failed to set GTK to "
                   "the driver (alg=%d keylen=%d keyidx=%d)",
                   gd->alg, gd->gtk_len, gd->keyidx);
        return -1;
    }

    return 0;
}


static int wpa_supplicant_gtk_tx_bit_workaround(const struct wpa_sm *sm,
        int tx)
{
    if (tx && sm->pairwise_cipher != WPA_CIPHER_NONE)
    {
        /* Ignore Tx bit for GTK if a pairwise key is used. One AP
         * seemed to set this bit (incorrectly, since Tx is only when
         * doing Group Key only APs) and without this workaround, the
         * data connection does not work because wpa_supplicant
         * configured non-zero keyidx to be used for unicast. */
        wpa_printf(MSG_INFO, "WPA: Tx bit set for GTK, but pairwise "
                   "keys are used - ignore Tx bit");
        return 0;
    }
    return tx;
}


static int wpa_supplicant_pairwise_gtk(struct wpa_sm *sm,
                                       const struct wpa_eapol_key *key,
                                       const u8 *gtk, size_t gtk_len,
                                       int key_info)
{
#ifndef CONFIG_NO_WPA2
    struct wpa_gtk_data gd;

    /*
     * IEEE Std 802.11i-2004 - 8.5.2 EAPOL-Key frames - Figure 43x
     * GTK KDE format:
     * KeyID[bits 0-1], Tx [bit 2], Reserved [bits 3-7]
     * Reserved [bits 0-7]
     * GTK
     */

    os_memset(&gd, 0, sizeof(gd));
    wpa_hexdump_key(MSG_DEBUG, "RSN: received GTK in pairwise handshake",
                    gtk, gtk_len);

    if (gtk_len < 2 || gtk_len - 2 > sizeof(gd.gtk))
        return -1;

    gd.keyidx = gtk[0] & 0x3;
    gd.tx = wpa_supplicant_gtk_tx_bit_workaround(sm,
            !!(gtk[0] & BIT(2)));
    gtk += 2;
    gtk_len -= 2;

    os_memcpy(gd.gtk, gtk, gtk_len);
    gd.gtk_len = gtk_len;

    if (wpa_supplicant_check_group_cipher(sm->group_cipher,
                                          gtk_len, gtk_len,
                                          &gd.key_rsc_len, &gd.alg) ||
            wpa_supplicant_install_gtk(sm, &gd, key->key_rsc))
    {
        wpa_printf(MSG_DEBUG, "RSN: Failed to install GTK");
        return -1;
    }

    wpa_supplicant_key_neg_complete(sm, sm->bssid,
                                    key_info & WPA_KEY_INFO_SECURE);
    return 0;
#else /* CONFIG_NO_WPA2 */
    return -1;
#endif /* CONFIG_NO_WPA2 */
}


static int ieee80211w_set_keys(struct wpa_sm *sm,
                               struct wpa_eapol_ie_parse *ie)
{
#ifdef CONFIG_IEEE80211W
    if (sm->mgmt_group_cipher != WPA_CIPHER_AES_128_CMAC)
        return 0;

    if (ie->igtk)
    {
        const struct wpa_igtk_kde *igtk;
        u16 keyidx;
        if (ie->igtk_len != sizeof(*igtk))
            return -1;
        igtk = (const struct wpa_igtk_kde *) ie->igtk;
        keyidx = WPA_GET_LE16(igtk->keyid);
        wpa_printf(MSG_DEBUG, "WPA: IGTK keyid %d "
                   "pn %02x%02x%02x%02x%02x%02x",
                   keyidx, MAC2STR(igtk->pn));
        wpa_hexdump_key(MSG_DEBUG, "WPA: IGTK",
                        igtk->igtk, WPA_IGTK_LEN);
        if (keyidx > 4095)
        {
            wpa_printf(MSG_WARNING, "WPA: Invalid IGTK KeyID %d",
                       keyidx);
            return -1;
        }
        if (wpa_sm_set_key(sm, WPA_ALG_IGTK,
                           (u8 *) "\xff\xff\xff\xff\xff\xff",
                           keyidx, 0, igtk->pn, sizeof(igtk->pn),
                           igtk->igtk, WPA_IGTK_LEN) < 0)
        {
            wpa_printf(MSG_WARNING, "WPA: Failed to configure IGTK"
                       " to the driver");
            return -1;
        }
    }

    return 0;
#else /* CONFIG_IEEE80211W */
    return 0;
#endif /* CONFIG_IEEE80211W */
}


static void wpa_report_ie_mismatch(struct wpa_sm *sm,
                                   const char *reason, const u8 *src_addr,
                                   const u8 *wpa_ie, size_t wpa_ie_len,
                                   const u8 *rsn_ie, size_t rsn_ie_len)
{
    wpa_msg(sm->ctx->ctx, MSG_WARNING, "WPA: %s (src=" MACSTR ")",
            reason, MAC2STR(src_addr));

    if (sm->ap_wpa_ie)
    {
        wpa_hexdump(MSG_INFO, "WPA: WPA IE in Beacon/ProbeResp",
                    sm->ap_wpa_ie, sm->ap_wpa_ie_len);
    }
    if (wpa_ie)
    {
        if (!sm->ap_wpa_ie)
        {
            wpa_printf(MSG_INFO, "WPA: No WPA IE in "
                       "Beacon/ProbeResp");
        }
        wpa_hexdump(MSG_INFO, "WPA: WPA IE in 3/4 msg",
                    wpa_ie, wpa_ie_len);
    }

    if (sm->ap_rsn_ie)
    {
        wpa_hexdump(MSG_INFO, "WPA: RSN IE in Beacon/ProbeResp",
                    sm->ap_rsn_ie, sm->ap_rsn_ie_len);
    }
    if (rsn_ie)
    {
        if (!sm->ap_rsn_ie)
        {
            wpa_printf(MSG_INFO, "WPA: No RSN IE in "
                       "Beacon/ProbeResp");
        }
        wpa_hexdump(MSG_INFO, "WPA: RSN IE in 3/4 msg",
                    rsn_ie, rsn_ie_len);
    }

    wpa_sm_disassociate(sm, WLAN_REASON_IE_IN_4WAY_DIFFERS);
}


static int wpa_supplicant_validate_ie(struct wpa_sm *sm,
                                      const unsigned char *src_addr,
                                      struct wpa_eapol_ie_parse *ie)
{
    if (sm->ap_wpa_ie == NULL && sm->ap_rsn_ie == NULL)
    {
        wpa_printf(MSG_DEBUG, "WPA: No WPA/RSN IE for this AP known. "
                   "Trying to get from scan results");
        if (wpa_sm_get_beacon_ie(sm) < 0)
        {
            wpa_printf(MSG_WARNING, "WPA: Could not find AP from "
                       "the scan results");
        }
        else
        {
            wpa_printf(MSG_DEBUG, "WPA: Found the current AP from "
                       "updated scan results");
        }
    }

    if (ie->wpa_ie == NULL && ie->rsn_ie == NULL &&
            (sm->ap_wpa_ie || sm->ap_rsn_ie))
    {
        wpa_report_ie_mismatch(sm, "IE in 3/4 msg does not match "
                               "with IE in Beacon/ProbeResp (no IE?)",
                               src_addr, ie->wpa_ie, ie->wpa_ie_len,
                               ie->rsn_ie, ie->rsn_ie_len);
        return -1;
    }

    if ((ie->wpa_ie && sm->ap_wpa_ie &&
            (ie->wpa_ie_len != sm->ap_wpa_ie_len ||
             os_memcmp(ie->wpa_ie, sm->ap_wpa_ie, ie->wpa_ie_len) != 0)) ||
            (ie->rsn_ie && sm->ap_rsn_ie &&
             (ie->rsn_ie_len != sm->ap_rsn_ie_len ||
              os_memcmp(ie->rsn_ie, sm->ap_rsn_ie, ie->rsn_ie_len) != 0)))
    {
        wpa_report_ie_mismatch(sm, "IE in 3/4 msg does not match "
                               "with IE in Beacon/ProbeResp",
                               src_addr, ie->wpa_ie, ie->wpa_ie_len,
                               ie->rsn_ie, ie->rsn_ie_len);
        return -1;
    }

    if (sm->proto == WPA_PROTO_WPA &&
            ie->rsn_ie && sm->ap_rsn_ie == NULL && sm->rsn_enabled)
    {
        wpa_report_ie_mismatch(sm, "Possible downgrade attack "
                               "detected - RSN was enabled and RSN IE "
                               "was in msg 3/4, but not in "
                               "Beacon/ProbeResp",
                               src_addr, ie->wpa_ie, ie->wpa_ie_len,
                               ie->rsn_ie, ie->rsn_ie_len);
        return -1;
    }

#ifdef CONFIG_IEEE80211R
    if (wpa_key_mgmt_ft(sm->key_mgmt))
    {
        struct rsn_mdie *mdie;
        /* TODO: verify that full MDIE matches with the one from scan
         * results, not only mobility domain */
        mdie = (struct rsn_mdie *) (ie->mdie + 2);
        if (ie->mdie == NULL || ie->mdie_len < 2 + sizeof(*mdie) ||
                os_memcmp(mdie->mobility_domain, sm->mobility_domain,
                          MOBILITY_DOMAIN_ID_LEN) != 0)
        {
            wpa_printf(MSG_DEBUG, "FT: MDIE in msg 3/4 did not "
                       "match with the current mobility domain");
            return -1;
        }
    }
#endif /* CONFIG_IEEE80211R */

    return 0;
}


/**
 * wpa_supplicant_send_4_of_4 - Send message 4 of WPA/RSN 4-Way Handshake
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @dst: Destination address for the frame
 * @key: Pointer to the EAPOL-Key frame header
 * @ver: Version bits from EAPOL-Key Key Info
 * @key_info: Key Info
 * @kde: KDEs to include the EAPOL-Key frame
 * @kde_len: Length of KDEs
 * @ptk: PTK to use for keyed hash and encryption
 * Returns: 0 on success, -1 on failure
 */
int wpa_supplicant_send_4_of_4(struct wpa_sm *sm, const unsigned char *dst,
                               const struct wpa_eapol_key *key,
                               u16 ver, u16 key_info,
                               const u8 *kde, size_t kde_len,
                               struct wpa_ptk *ptk)
{
    size_t rlen;
    struct wpa_eapol_key *reply;
    u8 *rbuf;

    if (kde)
        wpa_hexdump(MSG_DEBUG, "WPA: KDE for msg 4/4", kde, kde_len);

    rbuf = wpa_sm_alloc_eapol(sm, IEEE802_1X_TYPE_EAPOL_KEY, NULL,
                              sizeof(*reply) + kde_len,
                              &rlen, (void *) &reply);
    if (rbuf == NULL)
        return -1;

    reply->type = sm->proto == WPA_PROTO_RSN ?
                  EAPOL_KEY_TYPE_RSN : EAPOL_KEY_TYPE_WPA;
    key_info &= WPA_KEY_INFO_SECURE;
    key_info |= ver | WPA_KEY_INFO_KEY_TYPE | WPA_KEY_INFO_MIC;
    WPA_PUT_BE16(reply->key_info, key_info);
    if (sm->proto == WPA_PROTO_RSN)
        WPA_PUT_BE16(reply->key_length, 0);
    else
        os_memcpy(reply->key_length, key->key_length, 2);
    os_memcpy(reply->replay_counter, key->replay_counter,
              WPA_REPLAY_COUNTER_LEN);

    WPA_PUT_BE16(reply->key_data_length, kde_len);
    if (kde)
        os_memcpy(reply + 1, kde, kde_len);

    wpa_printf(MSG_DEBUG, "WPA: Sending EAPOL-Key 4/4");
    wpa_eapol_key_send(sm, ptk->kck, ver, dst, ETH_P_EAPOL,
                       rbuf, rlen, reply->key_mic);

    return 0;
}


static void wpa_supplicant_process_3_of_4(struct wpa_sm *sm,
        const struct wpa_eapol_key *key,
        u16 ver)
{
    u16 key_info, keylen, len;
    const u8 *pos;
    struct wpa_eapol_ie_parse ie;

    wpa_sm_set_state(sm, WPA_4WAY_HANDSHAKE);
    wpa_printf(MSG_DEBUG, "WPA: RX message 3 of 4-Way Handshake from "
               MACSTR " (ver=%d)", MAC2STR(sm->bssid), ver);

    key_info = WPA_GET_BE16(key->key_info);

    pos = (const u8 *) (key + 1);
    len = WPA_GET_BE16(key->key_data_length);
    wpa_hexdump(MSG_DEBUG, "WPA: IE KeyData", pos, len);
    wpa_supplicant_parse_ies(pos, len, &ie);
    if (ie.gtk && !(key_info & WPA_KEY_INFO_ENCR_KEY_DATA))
    {
        wpa_printf(MSG_WARNING, "WPA: GTK IE in unencrypted key data");
        goto failed;
    }
#ifdef CONFIG_IEEE80211W
    if (ie.igtk && !(key_info & WPA_KEY_INFO_ENCR_KEY_DATA))
    {
        wpa_printf(MSG_WARNING, "WPA: IGTK KDE in unencrypted key "
                   "data");
        goto failed;
    }

    if (ie.igtk && ie.igtk_len != sizeof(struct wpa_igtk_kde))
    {
        wpa_printf(MSG_WARNING, "WPA: Invalid IGTK KDE length %lu",
                   (unsigned long) ie.igtk_len);
        goto failed;
    }
#endif /* CONFIG_IEEE80211W */

    if (wpa_supplicant_validate_ie(sm, sm->bssid, &ie) < 0)
        goto failed;

    if (os_memcmp(sm->anonce, key->key_nonce, WPA_NONCE_LEN) != 0)
    {
        wpa_printf(MSG_WARNING, "WPA: ANonce from message 1 of 4-Way "
                   "Handshake differs from 3 of 4-Way Handshake - drop"
                   " packet (src=" MACSTR ")", MAC2STR(sm->bssid));
        goto failed;
    }

    keylen = WPA_GET_BE16(key->key_length);
    switch (sm->pairwise_cipher)
    {
        case WPA_CIPHER_CCMP:
            if (keylen != 16)
            {
                wpa_printf(MSG_WARNING, "WPA: Invalid CCMP key length "
                           "%d (src=" MACSTR ")",
                           keylen, MAC2STR(sm->bssid));
                goto failed;
            }
            break;
        case WPA_CIPHER_TKIP:
            if (keylen != 32)
            {
                wpa_printf(MSG_WARNING, "WPA: Invalid TKIP key length "
                           "%d (src=" MACSTR ")",
                           keylen, MAC2STR(sm->bssid));
                goto failed;
            }
            break;
    }

    if (wpa_supplicant_send_4_of_4(sm, sm->bssid, key, ver, key_info,
                                   NULL, 0, &sm->ptk))
    {
        goto failed;
    }

    /* SNonce was successfully used in msg 3/4, so mark it to be renewed
     * for the next 4-Way Handshake. If msg 3 is received again, the old
     * SNonce will still be used to avoid changing PTK. */
    sm->renew_snonce = 1;

    if (key_info & WPA_KEY_INFO_INSTALL)
    {
        if (wpa_supplicant_install_ptk(sm, key))
            goto failed;
    }

    if (key_info & WPA_KEY_INFO_SECURE)
    {
        wpa_sm_mlme_setprotection(
            sm, sm->bssid, MLME_SETPROTECTION_PROTECT_TYPE_RX,
            MLME_SETPROTECTION_KEY_TYPE_PAIRWISE);
        eapol_sm_notify_portValid(sm->eapol, TRUE);
    }
    wpa_sm_set_state(sm, WPA_GROUP_HANDSHAKE);

    if (ie.gtk &&
            wpa_supplicant_pairwise_gtk(sm, key,
                                        ie.gtk, ie.gtk_len, key_info) < 0)
    {
        wpa_printf(MSG_INFO, "RSN: Failed to configure GTK");
        goto failed;
    }

    if (ieee80211w_set_keys(sm, &ie) < 0)
    {
        wpa_printf(MSG_INFO, "RSN: Failed to configure IGTK");
        goto failed;
    }

    return;

failed:
    wpa_sm_deauthenticate(sm, WLAN_REASON_UNSPECIFIED);
}


static int wpa_supplicant_process_1_of_2_rsn(struct wpa_sm *sm,
        const u8 *keydata,
        size_t keydatalen,
        u16 key_info,
        struct wpa_gtk_data *gd)
{
    int maxkeylen;
    struct wpa_eapol_ie_parse ie;

    wpa_hexdump(MSG_DEBUG, "RSN: msg 1/2 key data", keydata, keydatalen);
    wpa_supplicant_parse_ies(keydata, keydatalen, &ie);
    if (ie.gtk && !(key_info & WPA_KEY_INFO_ENCR_KEY_DATA))
    {
        wpa_printf(MSG_WARNING, "WPA: GTK IE in unencrypted key data");
        return -1;
    }
    if (ie.gtk == NULL)
    {
        wpa_printf(MSG_INFO, "WPA: No GTK IE in Group Key msg 1/2");
        return -1;
    }
    maxkeylen = gd->gtk_len = ie.gtk_len - 2;

    if (wpa_supplicant_check_group_cipher(sm->group_cipher,
                                          gd->gtk_len, maxkeylen,
                                          &gd->key_rsc_len, &gd->alg))
        return -1;

    wpa_hexdump(MSG_DEBUG, "RSN: received GTK in group key handshake",
                ie.gtk, ie.gtk_len);
    gd->keyidx = ie.gtk[0] & 0x3;
    gd->tx = wpa_supplicant_gtk_tx_bit_workaround(sm,
             !!(ie.gtk[0] & BIT(2)));
    if (ie.gtk_len - 2 > sizeof(gd->gtk))
    {
        wpa_printf(MSG_INFO, "RSN: Too long GTK in GTK IE "
                   "(len=%lu)", (unsigned long) ie.gtk_len - 2);
        return -1;
    }
    os_memcpy(gd->gtk, ie.gtk + 2, ie.gtk_len - 2);

    if (ieee80211w_set_keys(sm, &ie) < 0)
        wpa_printf(MSG_INFO, "RSN: Failed to configure IGTK");

    return 0;
}


static int wpa_supplicant_process_1_of_2_wpa(struct wpa_sm *sm,
        const struct wpa_eapol_key *key,
        size_t keydatalen, int key_info,
        size_t extra_len, u16 ver,
        struct wpa_gtk_data *gd)
{
    size_t maxkeylen;
    u8 ek[32];

    gd->gtk_len = WPA_GET_BE16(key->key_length);
    maxkeylen = keydatalen;
    if (keydatalen > extra_len)
    {
        wpa_printf(MSG_INFO, "WPA: Truncated EAPOL-Key packet:"
                   " key_data_length=%lu > extra_len=%lu",
                   (unsigned long) keydatalen,
                   (unsigned long) extra_len);
        return -1;
    }
    if (ver == WPA_KEY_INFO_TYPE_HMAC_SHA1_AES)
    {
        if (maxkeylen < 8)
        {
            wpa_printf(MSG_INFO, "WPA: Too short maxkeylen (%lu)",
                       (unsigned long) maxkeylen);
            return -1;
        }
        maxkeylen -= 8;
    }

    if (wpa_supplicant_check_group_cipher(sm->group_cipher,
                                          gd->gtk_len, maxkeylen,
                                          &gd->key_rsc_len, &gd->alg))
        return -1;

    gd->keyidx = (key_info & WPA_KEY_INFO_KEY_INDEX_MASK) >>
                 WPA_KEY_INFO_KEY_INDEX_SHIFT;
    if (ver == WPA_KEY_INFO_TYPE_HMAC_MD5_RC4)
    {
        os_memcpy(ek, key->key_iv, 16);
        os_memcpy(ek + 16, sm->ptk.kek, 16);
        if (keydatalen > sizeof(gd->gtk))
        {
            wpa_printf(MSG_WARNING, "WPA: RC4 key data "
                       "too long (%lu)",
                       (unsigned long) keydatalen);
            return -1;
        }
        os_memcpy(gd->gtk, key + 1, keydatalen);
        rc4_skip(ek, 32, 256, gd->gtk, keydatalen);
    }
    else if (ver == WPA_KEY_INFO_TYPE_HMAC_SHA1_AES)
    {
        if (keydatalen % 8)
        {
            wpa_printf(MSG_WARNING, "WPA: Unsupported AES-WRAP "
                       "len %lu", (unsigned long) keydatalen);
            return -1;
        }
        if (maxkeylen > sizeof(gd->gtk))
        {
            wpa_printf(MSG_WARNING, "WPA: AES-WRAP key data "
                       "too long (keydatalen=%lu maxkeylen=%lu)",
                       (unsigned long) keydatalen,
                       (unsigned long) maxkeylen);
            return -1;
        }
        if (aes_unwrap(sm->ptk.kek, maxkeylen / 8,
                       (const u8 *) (key + 1), gd->gtk))
        {
            wpa_printf(MSG_WARNING, "WPA: AES unwrap "
                       "failed - could not decrypt GTK");
            return -1;
        }
    }
    else
    {
        wpa_printf(MSG_WARNING, "WPA: Unsupported key_info type %d",
                   ver);
        return -1;
    }
    gd->tx = wpa_supplicant_gtk_tx_bit_workaround(
                 sm, !!(key_info & WPA_KEY_INFO_TXRX));
    return 0;
}


static int wpa_supplicant_send_2_of_2(struct wpa_sm *sm,
                                      const struct wpa_eapol_key *key,
                                      int ver, u16 key_info)
{
    size_t rlen;
    struct wpa_eapol_key *reply;
    u8 *rbuf;

    rbuf = wpa_sm_alloc_eapol(sm, IEEE802_1X_TYPE_EAPOL_KEY, NULL,
                              sizeof(*reply), &rlen, (void *) &reply);
    if (rbuf == NULL)
        return -1;

    reply->type = sm->proto == WPA_PROTO_RSN ?
                  EAPOL_KEY_TYPE_RSN : EAPOL_KEY_TYPE_WPA;
    key_info &= WPA_KEY_INFO_KEY_INDEX_MASK;
    key_info |= ver | WPA_KEY_INFO_MIC | WPA_KEY_INFO_SECURE;
    WPA_PUT_BE16(reply->key_info, key_info);
    if (sm->proto == WPA_PROTO_RSN)
        WPA_PUT_BE16(reply->key_length, 0);
    else
        os_memcpy(reply->key_length, key->key_length, 2);
    os_memcpy(reply->replay_counter, key->replay_counter,
              WPA_REPLAY_COUNTER_LEN);

    WPA_PUT_BE16(reply->key_data_length, 0);

    wpa_printf(MSG_DEBUG, "WPA: Sending EAPOL-Key 2/2");
    wpa_eapol_key_send(sm, sm->ptk.kck, ver, sm->bssid, ETH_P_EAPOL,
                       rbuf, rlen, reply->key_mic);

    return 0;
}


static void wpa_supplicant_process_1_of_2(struct wpa_sm *sm,
        const unsigned char *src_addr,
        const struct wpa_eapol_key *key,
        int extra_len, u16 ver)
{
    u16 key_info, keydatalen;
    int rekey, ret;
    struct wpa_gtk_data gd;

    os_memset(&gd, 0, sizeof(gd));

    rekey = wpa_sm_get_state(sm) == WPA_COMPLETED;
    wpa_printf(MSG_DEBUG, "WPA: RX message 1 of Group Key Handshake from "
               MACSTR " (ver=%d)", MAC2STR(src_addr), ver);

    key_info = WPA_GET_BE16(key->key_info);
    keydatalen = WPA_GET_BE16(key->key_data_length);

    if (sm->proto == WPA_PROTO_RSN)
    {
        ret = wpa_supplicant_process_1_of_2_rsn(sm,
                                                (const u8 *) (key + 1),
                                                keydatalen, key_info,
                                                &gd);
    }
    else
    {
        ret = wpa_supplicant_process_1_of_2_wpa(sm, key, keydatalen,
                                                key_info, extra_len,
                                                ver, &gd);
    }

    wpa_sm_set_state(sm, WPA_GROUP_HANDSHAKE);

    if (ret)
        goto failed;

    if (wpa_supplicant_install_gtk(sm, &gd, key->key_rsc) ||
            wpa_supplicant_send_2_of_2(sm, key, ver, key_info))
        goto failed;

    if (rekey)
    {
        wpa_msg(sm->ctx->ctx, MSG_INFO, "WPA: Group rekeying "
                "completed with " MACSTR " [GTK=%s]",
                MAC2STR(sm->bssid), wpa_cipher_txt(sm->group_cipher));
        wpa_sm_cancel_auth_timeout(sm);
        wpa_sm_set_state(sm, WPA_COMPLETED);
    }
    else
    {
        wpa_supplicant_key_neg_complete(sm, sm->bssid,
                                        key_info &
                                        WPA_KEY_INFO_SECURE);
    }
    return;

failed:
    wpa_sm_deauthenticate(sm, WLAN_REASON_UNSPECIFIED);
}


static int wpa_supplicant_verify_eapol_key_mic(struct wpa_sm *sm,
        struct wpa_eapol_key *key,
        u16 ver,
        const u8 *buf, size_t len)
{
    u8 mic[16];
    int ok = 0;

    os_memcpy(mic, key->key_mic, 16);
    if (sm->tptk_set)
    {
        os_memset(key->key_mic, 0, 16);
        wpa_eapol_key_mic(sm->tptk.kck, ver, buf, len,
                          key->key_mic);
        if (os_memcmp(mic, key->key_mic, 16) != 0)
        {
            wpa_printf(MSG_WARNING, "WPA: Invalid EAPOL-Key MIC "
                       "when using TPTK - ignoring TPTK");
        }
        else
        {
            ok = 1;
            sm->tptk_set = 0;
            sm->ptk_set = 1;
            os_memcpy(&sm->ptk, &sm->tptk, sizeof(sm->ptk));
        }
    }

    if (!ok && sm->ptk_set)
    {
        os_memset(key->key_mic, 0, 16);
        wpa_eapol_key_mic(sm->ptk.kck, ver, buf, len,
                          key->key_mic);
        if (os_memcmp(mic, key->key_mic, 16) != 0)
        {
            wpa_printf(MSG_WARNING, "WPA: Invalid EAPOL-Key MIC "
                       "- dropping packet");
            return -1;
        }
        ok = 1;
    }

    if (!ok)
    {
        wpa_printf(MSG_WARNING, "WPA: Could not verify EAPOL-Key MIC "
                   "- dropping packet");
        return -1;
    }

    os_memcpy(sm->rx_replay_counter, key->replay_counter,
              WPA_REPLAY_COUNTER_LEN);
    sm->rx_replay_counter_set = 1;
    return 0;
}


/* Decrypt RSN EAPOL-Key key data (RC4 or AES-WRAP) */
static int wpa_supplicant_decrypt_key_data(struct wpa_sm *sm,
        struct wpa_eapol_key *key, u16 ver)
{
    u16 keydatalen = WPA_GET_BE16(key->key_data_length);

    wpa_hexdump(MSG_DEBUG, "RSN: encrypted key data",
                (u8 *) (key + 1), keydatalen);
    if (!sm->ptk_set)
    {
        wpa_printf(MSG_WARNING, "WPA: PTK not available, "
                   "cannot decrypt EAPOL-Key key data.");
        return -1;
    }

    /* Decrypt key data here so that this operation does not need
     * to be implemented separately for each message type. */
    if (ver == WPA_KEY_INFO_TYPE_HMAC_MD5_RC4)
    {
        u8 ek[32];
        os_memcpy(ek, key->key_iv, 16);
        os_memcpy(ek + 16, sm->ptk.kek, 16);
        rc4_skip(ek, 32, 256, (u8 *) (key + 1), keydatalen);
    }
    else if (ver == WPA_KEY_INFO_TYPE_HMAC_SHA1_AES ||
             ver == WPA_KEY_INFO_TYPE_AES_128_CMAC)
    {
        u8 *buf;
        if (keydatalen % 8)
        {
            wpa_printf(MSG_WARNING, "WPA: Unsupported "
                       "AES-WRAP len %d", keydatalen);
            return -1;
        }
        keydatalen -= 8; /* AES-WRAP adds 8 bytes */
        buf = os_malloc(keydatalen);
        if (buf == NULL)
        {
            wpa_printf(MSG_WARNING, "WPA: No memory for "
                       "AES-UNWRAP buffer");
            return -1;
        }
        if (aes_unwrap(sm->ptk.kek, keydatalen / 8,
                       (u8 *) (key + 1), buf))
        {
            os_free(buf);
            wpa_printf(MSG_WARNING, "WPA: AES unwrap failed - "
                       "could not decrypt EAPOL-Key key data");
            return -1;
        }
        os_memcpy(key + 1, buf, keydatalen);
        os_free(buf);
        WPA_PUT_BE16(key->key_data_length, keydatalen);
    }
    else
    {
        wpa_printf(MSG_WARNING, "WPA: Unsupported key_info type %d",
                   ver);
        return -1;
    }
    wpa_hexdump_key(MSG_DEBUG, "WPA: decrypted EAPOL-Key key data",
                    (u8 *) (key + 1), keydatalen);
    return 0;
}


/**
 * wpa_sm_aborted_cached - Notify WPA that PMKSA caching was aborted
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 */
void wpa_sm_aborted_cached(struct wpa_sm *sm)
{
    if (sm && sm->cur_pmksa)
    {
        wpa_printf(MSG_DEBUG, "RSN: Cancelling PMKSA caching attempt");
        sm->cur_pmksa = NULL;
    }
}


static void wpa_eapol_key_dump(const struct wpa_eapol_key *key)
{
#ifndef CONFIG_NO_STDOUT_DEBUG
    u16 key_info = WPA_GET_BE16(key->key_info);

    wpa_printf(MSG_DEBUG, "  EAPOL-Key type=%d", key->type);
    wpa_printf(MSG_DEBUG, "  key_info 0x%x (ver=%d keyidx=%d rsvd=%d %s"
               "%s%s%s%s%s%s%s)",
               key_info, key_info & WPA_KEY_INFO_TYPE_MASK,
               (key_info & WPA_KEY_INFO_KEY_INDEX_MASK) >>
               WPA_KEY_INFO_KEY_INDEX_SHIFT,
               (key_info & (BIT(13) | BIT(14) | BIT(15))) >> 13,
               key_info & WPA_KEY_INFO_KEY_TYPE ? "Pairwise" : "Group",
               key_info & WPA_KEY_INFO_INSTALL ? " Install" : "",
               key_info & WPA_KEY_INFO_ACK ? " Ack" : "",
               key_info & WPA_KEY_INFO_MIC ? " MIC" : "",
               key_info & WPA_KEY_INFO_SECURE ? " Secure" : "",
               key_info & WPA_KEY_INFO_ERROR ? " Error" : "",
               key_info & WPA_KEY_INFO_REQUEST ? " Request" : "",
               key_info & WPA_KEY_INFO_ENCR_KEY_DATA ? " Encr" : "");
    wpa_printf(MSG_DEBUG, "  key_length=%u key_data_length=%u",
               WPA_GET_BE16(key->key_length),
               WPA_GET_BE16(key->key_data_length));
    wpa_hexdump(MSG_DEBUG, "  replay_counter",
                key->replay_counter, WPA_REPLAY_COUNTER_LEN);
    wpa_hexdump(MSG_DEBUG, "  key_nonce", key->key_nonce, WPA_NONCE_LEN);
    wpa_hexdump(MSG_DEBUG, "  key_iv", key->key_iv, 16);
    wpa_hexdump(MSG_DEBUG, "  key_rsc", key->key_rsc, 8);
    wpa_hexdump(MSG_DEBUG, "  key_id (reserved)", key->key_id, 8);
    wpa_hexdump(MSG_DEBUG, "  key_mic", key->key_mic, 16);
#endif /* CONFIG_NO_STDOUT_DEBUG */
}


/**
 * wpa_sm_rx_eapol - Process received WPA EAPOL frames
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @src_addr: Source MAC address of the EAPOL packet
 * @buf: Pointer to the beginning of the EAPOL data (EAPOL header)
 * @len: Length of the EAPOL frame
 * Returns: 1 = WPA EAPOL-Key processed, 0 = not a WPA EAPOL-Key, -1 failure
 *
 * This function is called for each received EAPOL frame. Other than EAPOL-Key
 * frames can be skipped if filtering is done elsewhere. wpa_sm_rx_eapol() is
 * only processing WPA and WPA2 EAPOL-Key frames.
 *
 * The received EAPOL-Key packets are validated and valid packets are replied
 * to. In addition, key material (PTK, GTK) is configured at the end of a
 * successful key handshake.
 */
int wpa_sm_rx_eapol(struct wpa_sm *sm, const u8 *src_addr,
                    const u8 *buf, size_t len)
{
    size_t plen, data_len, extra_len;
    struct ieee802_1x_hdr *hdr;
    struct wpa_eapol_key *key;
    u16 key_info, ver;
    u8 *tmp;
    int ret = -1;
    struct wpa_peerkey *peerkey = NULL;

#ifdef CONFIG_IEEE80211R
    sm->ft_completed = 0;
#endif /* CONFIG_IEEE80211R */

    if (len < sizeof(*hdr) + sizeof(*key))
    {
        wpa_printf(MSG_DEBUG, "WPA: EAPOL frame too short to be a WPA "
                   "EAPOL-Key (len %lu, expecting at least %lu)",
                   (unsigned long) len,
                   (unsigned long) sizeof(*hdr) + sizeof(*key));
        return 0;
    }

    tmp = os_malloc(len);
    if (tmp == NULL)
        return -1;
    os_memcpy(tmp, buf, len);

    hdr = (struct ieee802_1x_hdr *) tmp;
    key = (struct wpa_eapol_key *) (hdr + 1);
    plen = be_to_host16(hdr->length);
    data_len = plen + sizeof(*hdr);
    wpa_printf(MSG_DEBUG, "IEEE 802.1X RX: version=%d type=%d length=%lu",
               hdr->version, hdr->type, (unsigned long) plen);

    if (hdr->version < EAPOL_VERSION)
    {
        /* TODO: backwards compatibility */
    }
    if (hdr->type != IEEE802_1X_TYPE_EAPOL_KEY)
    {
        wpa_printf(MSG_DEBUG, "WPA: EAPOL frame (type %u) discarded, "
                   "not a Key frame", hdr->type);
        ret = 0;
        goto out;
    }
    if (plen > len - sizeof(*hdr) || plen < sizeof(*key))
    {
        wpa_printf(MSG_DEBUG, "WPA: EAPOL frame payload size %lu "
                   "invalid (frame size %lu)",
                   (unsigned long) plen, (unsigned long) len);
        ret = 0;
        goto out;
    }

    if (key->type != EAPOL_KEY_TYPE_WPA && key->type != EAPOL_KEY_TYPE_RSN)
    {
        wpa_printf(MSG_DEBUG, "WPA: EAPOL-Key type (%d) unknown, "
                   "discarded", key->type);
        ret = 0;
        goto out;
    }
    wpa_eapol_key_dump(key);

    eapol_sm_notify_lower_layer_success(sm->eapol, 0);
    wpa_hexdump(MSG_MSGDUMP, "WPA: RX EAPOL-Key", tmp, len);
    if (data_len < len)
    {
        wpa_printf(MSG_DEBUG, "WPA: ignoring %lu bytes after the IEEE "
                   "802.1X data", (unsigned long) len - data_len);
    }
    key_info = WPA_GET_BE16(key->key_info);
    ver = key_info & WPA_KEY_INFO_TYPE_MASK;
    if (ver != WPA_KEY_INFO_TYPE_HMAC_MD5_RC4 &&
#if defined(CONFIG_IEEE80211R) || defined(CONFIG_IEEE80211W)
            ver != WPA_KEY_INFO_TYPE_AES_128_CMAC &&
#endif /* CONFIG_IEEE80211R || CONFIG_IEEE80211W */
            ver != WPA_KEY_INFO_TYPE_HMAC_SHA1_AES)
    {
        wpa_printf(MSG_INFO, "WPA: Unsupported EAPOL-Key descriptor "
                   "version %d.", ver);
        goto out;
    }

#ifdef CONFIG_IEEE80211R
    if (wpa_key_mgmt_ft(sm->key_mgmt))
    {
        /* IEEE 802.11r uses a new key_info type (AES-128-CMAC). */
        if (ver != WPA_KEY_INFO_TYPE_AES_128_CMAC)
        {
            wpa_printf(MSG_INFO, "FT: AP did not use "
                       "AES-128-CMAC.");
            goto out;
        }
    }
    else
#endif /* CONFIG_IEEE80211R */
#ifdef CONFIG_IEEE80211W
        if (wpa_key_mgmt_sha256(sm->key_mgmt))
        {
            if (ver != WPA_KEY_INFO_TYPE_AES_128_CMAC)
            {
                wpa_printf(MSG_INFO, "WPA: AP did not use the "
                           "negotiated AES-128-CMAC.");
                goto out;
            }
        }
        else
#endif /* CONFIG_IEEE80211W */
            if (sm->pairwise_cipher == WPA_CIPHER_CCMP &&
                    ver != WPA_KEY_INFO_TYPE_HMAC_SHA1_AES)
            {
                wpa_printf(MSG_INFO, "WPA: CCMP is used, but EAPOL-Key "
                           "descriptor version (%d) is not 2.", ver);
                if (sm->group_cipher != WPA_CIPHER_CCMP &&
                        !(key_info & WPA_KEY_INFO_KEY_TYPE))
                {
                    /* Earlier versions of IEEE 802.11i did not explicitly
                     * require version 2 descriptor for all EAPOL-Key
                     * packets, so allow group keys to use version 1 if
                     * CCMP is not used for them. */
                    wpa_printf(MSG_INFO, "WPA: Backwards compatibility: "
                               "allow invalid version for non-CCMP group "
                               "keys");
                }
                else
                    goto out;
            }

#ifdef CONFIG_PEERKEY
    for (peerkey = sm->peerkey; peerkey; peerkey = peerkey->next)
    {
        if (os_memcmp(peerkey->addr, src_addr, ETH_ALEN) == 0)
            break;
    }

    if (!(key_info & WPA_KEY_INFO_SMK_MESSAGE) && peerkey)
    {
        if (!peerkey->initiator && peerkey->replay_counter_set &&
                os_memcmp(key->replay_counter, peerkey->replay_counter,
                          WPA_REPLAY_COUNTER_LEN) <= 0)
        {
            wpa_printf(MSG_WARNING, "RSN: EAPOL-Key Replay "
                       "Counter did not increase (STK) - dropping "
                       "packet");
            goto out;
        }
        else if (peerkey->initiator)
        {
            u8 _tmp[WPA_REPLAY_COUNTER_LEN];
            os_memcpy(_tmp, key->replay_counter,
                      WPA_REPLAY_COUNTER_LEN);
            inc_byte_array(_tmp, WPA_REPLAY_COUNTER_LEN);
            if (os_memcmp(_tmp, peerkey->replay_counter,
                          WPA_REPLAY_COUNTER_LEN) != 0)
            {
                wpa_printf(MSG_DEBUG, "RSN: EAPOL-Key Replay "
                           "Counter did not match (STK) - "
                           "dropping packet");
                goto out;
            }
        }
    }

    if (peerkey && peerkey->initiator && (key_info & WPA_KEY_INFO_ACK))
    {
        wpa_printf(MSG_INFO, "RSN: Ack bit in key_info from STK peer");
        goto out;
    }
#endif /* CONFIG_PEERKEY */

    if (!peerkey && sm->rx_replay_counter_set &&
            os_memcmp(key->replay_counter, sm->rx_replay_counter,
                      WPA_REPLAY_COUNTER_LEN) <= 0)
    {
        wpa_printf(MSG_WARNING, "WPA: EAPOL-Key Replay Counter did not"
                   " increase - dropping packet");
        goto out;
    }

    if (!(key_info & (WPA_KEY_INFO_ACK | WPA_KEY_INFO_SMK_MESSAGE))
#ifdef CONFIG_PEERKEY
            && (peerkey == NULL || !peerkey->initiator)
#endif /* CONFIG_PEERKEY */
       )
    {
        wpa_printf(MSG_INFO, "WPA: No Ack bit in key_info");
        goto out;
    }

    if (key_info & WPA_KEY_INFO_REQUEST)
    {
        wpa_printf(MSG_INFO, "WPA: EAPOL-Key with Request bit - "
                   "dropped");
        goto out;
    }

    if ((key_info & WPA_KEY_INFO_MIC) && !peerkey &&
            wpa_supplicant_verify_eapol_key_mic(sm, key, ver, tmp, data_len))
        goto out;

#ifdef CONFIG_PEERKEY
    if ((key_info & WPA_KEY_INFO_MIC) && peerkey &&
            peerkey_verify_eapol_key_mic(sm, peerkey, key, ver, tmp, data_len))
        goto out;
#endif /* CONFIG_PEERKEY */

    extra_len = data_len - sizeof(*hdr) - sizeof(*key);

    if (WPA_GET_BE16(key->key_data_length) > extra_len)
    {
        wpa_msg(sm->ctx->ctx, MSG_INFO, "WPA: Invalid EAPOL-Key "
                "frame - key_data overflow (%d > %lu)",
                WPA_GET_BE16(key->key_data_length),
                (unsigned long) extra_len);
        goto out;
    }
    extra_len = WPA_GET_BE16(key->key_data_length);

    if (sm->proto == WPA_PROTO_RSN &&
            (key_info & WPA_KEY_INFO_ENCR_KEY_DATA))
    {
        if (wpa_supplicant_decrypt_key_data(sm, key, ver))
            goto out;
        extra_len = WPA_GET_BE16(key->key_data_length);
    }

    if (key_info & WPA_KEY_INFO_KEY_TYPE)
    {
        if (key_info & WPA_KEY_INFO_KEY_INDEX_MASK)
        {
            wpa_printf(MSG_WARNING, "WPA: Ignored EAPOL-Key "
                       "(Pairwise) with non-zero key index");
            goto out;
        }
        if (peerkey)
        {
            /* PeerKey 4-Way Handshake */
            peerkey_rx_eapol_4way(sm, peerkey, key, key_info, ver);
        }
        else if (key_info & WPA_KEY_INFO_MIC)
        {
            /* 3/4 4-Way Handshake */
            wpa_supplicant_process_3_of_4(sm, key, ver);
        }
        else
        {
            /* 1/4 4-Way Handshake */
            wpa_supplicant_process_1_of_4(sm, src_addr, key,
                                          ver);
        }
    }
    else if (key_info & WPA_KEY_INFO_SMK_MESSAGE)
    {
        /* PeerKey SMK Handshake */
        peerkey_rx_eapol_smk(sm, src_addr, key, extra_len, key_info,
                             ver);
    }
    else
    {
        if (key_info & WPA_KEY_INFO_MIC)
        {
            /* 1/2 Group Key Handshake */
            wpa_supplicant_process_1_of_2(sm, src_addr, key,
                                          extra_len, ver);
        }
        else
        {
            wpa_printf(MSG_WARNING, "WPA: EAPOL-Key (Group) "
                       "without Mic bit - dropped");
        }
    }

    ret = 1;

out:
    os_free(tmp);
    return ret;
}


#ifdef CONFIG_CTRL_IFACE
static int wpa_cipher_bits(int cipher)
{
    switch (cipher)
    {
        case WPA_CIPHER_CCMP:
            return 128;
        case WPA_CIPHER_TKIP:
            return 256;
        case WPA_CIPHER_WEP104:
            return 104;
        case WPA_CIPHER_WEP40:
            return 40;
        default:
            return 0;
    }
}


static u32 wpa_key_mgmt_suite(struct wpa_sm *sm)
{
    switch (sm->key_mgmt)
    {
        case WPA_KEY_MGMT_IEEE8021X:
            return (sm->proto == WPA_PROTO_RSN ?
                    RSN_AUTH_KEY_MGMT_UNSPEC_802_1X :
                    WPA_AUTH_KEY_MGMT_UNSPEC_802_1X);
        case WPA_KEY_MGMT_PSK:
            return (sm->proto == WPA_PROTO_RSN ?
                    RSN_AUTH_KEY_MGMT_PSK_OVER_802_1X :
                    WPA_AUTH_KEY_MGMT_PSK_OVER_802_1X);
#ifdef CONFIG_IEEE80211R
        case WPA_KEY_MGMT_FT_IEEE8021X:
            return RSN_AUTH_KEY_MGMT_FT_802_1X;
        case WPA_KEY_MGMT_FT_PSK:
            return RSN_AUTH_KEY_MGMT_FT_PSK;
#endif /* CONFIG_IEEE80211R */
#ifdef CONFIG_IEEE80211W
        case WPA_KEY_MGMT_IEEE8021X_SHA256:
            return RSN_AUTH_KEY_MGMT_802_1X_SHA256;
        case WPA_KEY_MGMT_PSK_SHA256:
            return RSN_AUTH_KEY_MGMT_PSK_SHA256;
#endif /* CONFIG_IEEE80211W */
        case WPA_KEY_MGMT_WPA_NONE:
            return WPA_AUTH_KEY_MGMT_NONE;
        default:
            return 0;
    }
}


static u32 wpa_cipher_suite(struct wpa_sm *sm, int cipher)
{
    switch (cipher)
    {
        case WPA_CIPHER_CCMP:
            return (sm->proto == WPA_PROTO_RSN ?
                    RSN_CIPHER_SUITE_CCMP : WPA_CIPHER_SUITE_CCMP);
        case WPA_CIPHER_TKIP:
            return (sm->proto == WPA_PROTO_RSN ?
                    RSN_CIPHER_SUITE_TKIP : WPA_CIPHER_SUITE_TKIP);
        case WPA_CIPHER_WEP104:
            return (sm->proto == WPA_PROTO_RSN ?
                    RSN_CIPHER_SUITE_WEP104 : WPA_CIPHER_SUITE_WEP104);
        case WPA_CIPHER_WEP40:
            return (sm->proto == WPA_PROTO_RSN ?
                    RSN_CIPHER_SUITE_WEP40 : WPA_CIPHER_SUITE_WEP40);
        case WPA_CIPHER_NONE:
            return (sm->proto == WPA_PROTO_RSN ?
                    RSN_CIPHER_SUITE_NONE : WPA_CIPHER_SUITE_NONE);
        default:
            return 0;
    }
}


#define RSN_SUITE "%02x-%02x-%02x-%d"
#define RSN_SUITE_ARG(s) \
((s) >> 24) & 0xff, ((s) >> 16) & 0xff, ((s) >> 8) & 0xff, (s) & 0xff

/**
 * wpa_sm_get_mib - Dump text list of MIB entries
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @buf: Buffer for the list
 * @buflen: Length of the buffer
 * Returns: Number of bytes written to buffer
 *
 * This function is used fetch dot11 MIB variables.
 */
int wpa_sm_get_mib(struct wpa_sm *sm, char *buf, size_t buflen)
{
    char pmkid_txt[PMKID_LEN * 2 + 1];
    int rsna, ret;
    size_t len;

    if (sm->cur_pmksa)
    {
        wpa_snprintf_hex(pmkid_txt, sizeof(pmkid_txt),
                         sm->cur_pmksa->pmkid, PMKID_LEN);
    }
    else
        pmkid_txt[0] = '\0';

    if ((wpa_key_mgmt_wpa_psk(sm->key_mgmt) ||
            wpa_key_mgmt_wpa_ieee8021x(sm->key_mgmt)) &&
            sm->proto == WPA_PROTO_RSN)
        rsna = 1;
    else
        rsna = 0;

    ret = os_snprintf(buf, buflen,
                      "dot11RSNAOptionImplemented=TRUE\n"
                      "dot11RSNAPreauthenticationImplemented=TRUE\n"
                      "dot11RSNAEnabled=%s\n"
                      "dot11RSNAPreauthenticationEnabled=%s\n"
                      "dot11RSNAConfigVersion=%d\n"
                      "dot11RSNAConfigPairwiseKeysSupported=5\n"
                      "dot11RSNAConfigGroupCipherSize=%d\n"
                      "dot11RSNAConfigPMKLifetime=%d\n"
                      "dot11RSNAConfigPMKReauthThreshold=%d\n"
                      "dot11RSNAConfigNumberOfPTKSAReplayCounters=1\n"
                      "dot11RSNAConfigSATimeout=%d\n",
                      rsna ? "TRUE" : "FALSE",
                      rsna ? "TRUE" : "FALSE",
                      RSN_VERSION,
                      wpa_cipher_bits(sm->group_cipher),
                      sm->dot11RSNAConfigPMKLifetime,
                      sm->dot11RSNAConfigPMKReauthThreshold,
                      sm->dot11RSNAConfigSATimeout);
    if (ret < 0 || (size_t) ret >= buflen)
        return 0;
    len = ret;

    ret = os_snprintf(
              buf + len, buflen - len,
              "dot11RSNAAuthenticationSuiteSelected=" RSN_SUITE "\n"
              "dot11RSNAPairwiseCipherSelected=" RSN_SUITE "\n"
              "dot11RSNAGroupCipherSelected=" RSN_SUITE "\n"
              "dot11RSNAPMKIDUsed=%s\n"
              "dot11RSNAAuthenticationSuiteRequested=" RSN_SUITE "\n"
              "dot11RSNAPairwiseCipherRequested=" RSN_SUITE "\n"
              "dot11RSNAGroupCipherRequested=" RSN_SUITE "\n"
              "dot11RSNAConfigNumberOfGTKSAReplayCounters=0\n"
              "dot11RSNA4WayHandshakeFailures=%u\n",
              RSN_SUITE_ARG(wpa_key_mgmt_suite(sm)),
              RSN_SUITE_ARG(wpa_cipher_suite(sm, sm->pairwise_cipher)),
              RSN_SUITE_ARG(wpa_cipher_suite(sm, sm->group_cipher)),
              pmkid_txt,
              RSN_SUITE_ARG(wpa_key_mgmt_suite(sm)),
              RSN_SUITE_ARG(wpa_cipher_suite(sm, sm->pairwise_cipher)),
              RSN_SUITE_ARG(wpa_cipher_suite(sm, sm->group_cipher)),
              sm->dot11RSNA4WayHandshakeFailures);
    if (ret >= 0 && (size_t) ret < buflen)
        len += ret;

    return (int) len;
}
#endif /* CONFIG_CTRL_IFACE */


static void wpa_sm_pmksa_free_cb(struct rsn_pmksa_cache_entry *entry,
                                 void *ctx, int replace)
{
    struct wpa_sm *sm = ctx;

    if (sm->cur_pmksa == entry ||
            (sm->pmk_len == entry->pmk_len &&
             os_memcmp(sm->pmk, entry->pmk, sm->pmk_len) == 0))
    {
        wpa_printf(MSG_DEBUG, "RSN: removed current PMKSA entry");
        sm->cur_pmksa = NULL;

        if (replace)
        {
            /* A new entry is being added, so no need to
             * deauthenticate in this case. This happens when EAP
             * authentication is completed again (reauth or failed
             * PMKSA caching attempt). */
            return;
        }

        os_memset(sm->pmk, 0, sizeof(sm->pmk));
        wpa_sm_deauthenticate(sm, WLAN_REASON_UNSPECIFIED);
    }
}


/**
 * wpa_sm_init - Initialize WPA state machine
 * @ctx: Context pointer for callbacks; this needs to be an allocated buffer
 * Returns: Pointer to the allocated WPA state machine data
 *
 * This function is used to allocate a new WPA state machine and the returned
 * value is passed to all WPA state machine calls.
 */
struct wpa_sm *wpa_sm_init(struct wpa_sm_ctx *ctx)
{
    struct wpa_sm *sm;

    sm = os_zalloc(sizeof(*sm));
    if (sm == NULL)
        return NULL;
    sm->renew_snonce = 1;
    sm->ctx = ctx;

    sm->dot11RSNAConfigPMKLifetime = 43200;
    sm->dot11RSNAConfigPMKReauthThreshold = 70;
    sm->dot11RSNAConfigSATimeout = 60;

    sm->pmksa = pmksa_cache_init(wpa_sm_pmksa_free_cb, sm, sm);
    if (sm->pmksa == NULL)
    {
        wpa_printf(MSG_ERROR, "RSN: PMKSA cache initialization "
                   "failed");
        os_free(sm);
        return NULL;
    }

    return sm;
}


/**
 * wpa_sm_deinit - Deinitialize WPA state machine
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 */
void wpa_sm_deinit(struct wpa_sm *sm)
{
    if (sm == NULL)
        return;
    pmksa_cache_deinit(sm->pmksa);
    eloop_cancel_timeout(wpa_sm_start_preauth, sm, NULL);
    eloop_cancel_timeout(wpa_sm_rekey_ptk, sm, NULL);
    os_free(sm->assoc_wpa_ie);
    os_free(sm->ap_wpa_ie);
    os_free(sm->ap_rsn_ie);
    os_free(sm->ctx);
    peerkey_deinit(sm);
    os_free(sm);
}


/**
 * wpa_sm_notify_assoc - Notify WPA state machine about association
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @bssid: The BSSID of the new association
 *
 * This function is called to let WPA state machine know that the connection
 * was established.
 */
void wpa_sm_notify_assoc(struct wpa_sm *sm, const u8 *bssid)
{
    int clear_ptk = 1;

    if (sm == NULL)
        return;

    wpa_printf(MSG_DEBUG, "WPA: Association event - clear replay counter");
    os_memcpy(sm->bssid, bssid, ETH_ALEN);
    os_memset(sm->rx_replay_counter, 0, WPA_REPLAY_COUNTER_LEN);
    sm->rx_replay_counter_set = 0;
    sm->renew_snonce = 1;
    if (os_memcmp(sm->preauth_bssid, bssid, ETH_ALEN) == 0)
        rsn_preauth_deinit(sm);

#ifdef CONFIG_IEEE80211R
    if (wpa_ft_is_completed(sm))
    {
        wpa_supplicant_key_neg_complete(sm, sm->bssid, 1);

        /* Prepare for the next transition */
        wpa_ft_prepare_auth_request(sm);

        clear_ptk = 0;
    }
#endif /* CONFIG_IEEE80211R */

    if (clear_ptk)
    {
        /*
         * IEEE 802.11, 8.4.10: Delete PTK SA on (re)association if
         * this is not part of a Fast BSS Transition.
         */
        wpa_printf(MSG_DEBUG, "WPA: Clear old PTK");
        sm->ptk_set = 0;
        sm->tptk_set = 0;
    }
}


/**
 * wpa_sm_notify_disassoc - Notify WPA state machine about disassociation
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 *
 * This function is called to let WPA state machine know that the connection
 * was lost. This will abort any existing pre-authentication session.
 */
void wpa_sm_notify_disassoc(struct wpa_sm *sm)
{
    rsn_preauth_deinit(sm);
    if (wpa_sm_get_state(sm) == WPA_4WAY_HANDSHAKE)
        sm->dot11RSNA4WayHandshakeFailures++;
}


/**
 * wpa_sm_set_pmk - Set PMK
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @pmk: The new PMK
 * @pmk_len: The length of the new PMK in bytes
 *
 * Configure the PMK for WPA state machine.
 */
void wpa_sm_set_pmk(struct wpa_sm *sm, const u8 *pmk, size_t pmk_len)
{
    if (sm == NULL)
        return;

    sm->pmk_len = pmk_len;
    os_memcpy(sm->pmk, pmk, pmk_len);

#ifdef CONFIG_IEEE80211R
    /* Set XXKey to be PSK for FT key derivation */
    sm->xxkey_len = pmk_len;
    os_memcpy(sm->xxkey, pmk, pmk_len);
#endif /* CONFIG_IEEE80211R */
}


/**
 * wpa_sm_set_pmk_from_pmksa - Set PMK based on the current PMKSA
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 *
 * Take the PMK from the current PMKSA into use. If no PMKSA is active, the PMK
 * will be cleared.
 */
void wpa_sm_set_pmk_from_pmksa(struct wpa_sm *sm)
{
    if (sm == NULL)
        return;

    if (sm->cur_pmksa)
    {
        sm->pmk_len = sm->cur_pmksa->pmk_len;
        os_memcpy(sm->pmk, sm->cur_pmksa->pmk, sm->pmk_len);
    }
    else
    {
        sm->pmk_len = PMK_LEN;
        os_memset(sm->pmk, 0, PMK_LEN);
    }
}


/**
 * wpa_sm_set_fast_reauth - Set fast reauthentication (EAP) enabled/disabled
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @fast_reauth: Whether fast reauthentication (EAP) is allowed
 */
void wpa_sm_set_fast_reauth(struct wpa_sm *sm, int fast_reauth)
{
    if (sm)
        sm->fast_reauth = fast_reauth;
}


/**
 * wpa_sm_set_scard_ctx - Set context pointer for smartcard callbacks
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @scard_ctx: Context pointer for smartcard related callback functions
 */
void wpa_sm_set_scard_ctx(struct wpa_sm *sm, void *scard_ctx)
{
    if (sm == NULL)
        return;
    sm->scard_ctx = scard_ctx;
    if (sm->preauth_eapol)
        eapol_sm_register_scard_ctx(sm->preauth_eapol, scard_ctx);
}


/**
 * wpa_sm_set_config - Notification of current configration change
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @config: Pointer to current network configuration
 *
 * Notify WPA state machine that configuration has changed. config will be
 * stored as a backpointer to network configuration. This can be %NULL to clear
 * the stored pointed.
 */
void wpa_sm_set_config(struct wpa_sm *sm, struct rsn_supp_config *config)
{
    if (!sm)
        return;

    if (config)
    {
        sm->network_ctx = config->network_ctx;
        sm->peerkey_enabled = config->peerkey_enabled;
        sm->allowed_pairwise_cipher = config->allowed_pairwise_cipher;
        sm->proactive_key_caching = config->proactive_key_caching;
        sm->eap_workaround = config->eap_workaround;
        sm->eap_conf_ctx = config->eap_conf_ctx;
        if (config->ssid)
        {
            os_memcpy(sm->ssid, config->ssid, config->ssid_len);
            sm->ssid_len = config->ssid_len;
        }
        else
            sm->ssid_len = 0;
        sm->wpa_ptk_rekey = config->wpa_ptk_rekey;
    }
    else
    {
        sm->network_ctx = NULL;
        sm->peerkey_enabled = 0;
        sm->allowed_pairwise_cipher = 0;
        sm->proactive_key_caching = 0;
        sm->eap_workaround = 0;
        sm->eap_conf_ctx = NULL;
        sm->ssid_len = 0;
        sm->wpa_ptk_rekey = 0;
    }
    if (config == NULL || config->network_ctx != sm->network_ctx)
        pmksa_cache_notify_reconfig(sm->pmksa);
}


/**
 * wpa_sm_set_own_addr - Set own MAC address
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @addr: Own MAC address
 */
void wpa_sm_set_own_addr(struct wpa_sm *sm, const u8 *addr)
{
    if (sm)
        os_memcpy(sm->own_addr, addr, ETH_ALEN);
}


/**
 * wpa_sm_set_ifname - Set network interface name
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @ifname: Interface name
 * @bridge_ifname: Optional bridge interface name (for pre-auth)
 */
void wpa_sm_set_ifname(struct wpa_sm *sm, const char *ifname,
                       const char *bridge_ifname)
{
    if (sm)
    {
        sm->ifname = ifname;
        sm->bridge_ifname = bridge_ifname;
    }
}


/**
 * wpa_sm_set_eapol - Set EAPOL state machine pointer
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @eapol: Pointer to EAPOL state machine allocated with eapol_sm_init()
 */
void wpa_sm_set_eapol(struct wpa_sm *sm, struct eapol_sm *eapol)
{
    if (sm)
        sm->eapol = eapol;
}


/**
 * wpa_sm_set_param - Set WPA state machine parameters
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @param: Parameter field
 * @value: Parameter value
 * Returns: 0 on success, -1 on failure
 */
int wpa_sm_set_param(struct wpa_sm *sm, enum wpa_sm_conf_params param,
                     unsigned int value)
{
    int ret = 0;

    if (sm == NULL)
        return -1;

    switch (param)
    {
        case RSNA_PMK_LIFETIME:
            if (value > 0)
                sm->dot11RSNAConfigPMKLifetime = value;
            else
                ret = -1;
            break;
        case RSNA_PMK_REAUTH_THRESHOLD:
            if (value > 0 && value <= 100)
                sm->dot11RSNAConfigPMKReauthThreshold = value;
            else
                ret = -1;
            break;
        case RSNA_SA_TIMEOUT:
            if (value > 0)
                sm->dot11RSNAConfigSATimeout = value;
            else
                ret = -1;
            break;
        case WPA_PARAM_PROTO:
            sm->proto = value;
            break;
        case WPA_PARAM_PAIRWISE:
            sm->pairwise_cipher = value;
            break;
        case WPA_PARAM_GROUP:
            sm->group_cipher = value;
            break;
        case WPA_PARAM_KEY_MGMT:
            sm->key_mgmt = value;
            break;
#ifdef CONFIG_IEEE80211W
        case WPA_PARAM_MGMT_GROUP:
            sm->mgmt_group_cipher = value;
            break;
#endif /* CONFIG_IEEE80211W */
        case WPA_PARAM_RSN_ENABLED:
            sm->rsn_enabled = value;
            break;
        default:
            break;
    }

    return ret;
}


/**
 * wpa_sm_get_param - Get WPA state machine parameters
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @param: Parameter field
 * Returns: Parameter value
 */
unsigned int wpa_sm_get_param(struct wpa_sm *sm, enum wpa_sm_conf_params param)
{
    if (sm == NULL)
        return 0;

    switch (param)
    {
        case RSNA_PMK_LIFETIME:
            return sm->dot11RSNAConfigPMKLifetime;
        case RSNA_PMK_REAUTH_THRESHOLD:
            return sm->dot11RSNAConfigPMKReauthThreshold;
        case RSNA_SA_TIMEOUT:
            return sm->dot11RSNAConfigSATimeout;
        case WPA_PARAM_PROTO:
            return sm->proto;
        case WPA_PARAM_PAIRWISE:
            return sm->pairwise_cipher;
        case WPA_PARAM_GROUP:
            return sm->group_cipher;
        case WPA_PARAM_KEY_MGMT:
            return sm->key_mgmt;
#ifdef CONFIG_IEEE80211W
        case WPA_PARAM_MGMT_GROUP:
            return sm->mgmt_group_cipher;
#endif /* CONFIG_IEEE80211W */
        case WPA_PARAM_RSN_ENABLED:
            return sm->rsn_enabled;
        default:
            return 0;
    }
}


/**
 * wpa_sm_get_status - Get WPA state machine
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @buf: Buffer for status information
 * @buflen: Maximum buffer length
 * @verbose: Whether to include verbose status information
 * Returns: Number of bytes written to buf.
 *
 * Query WPA state machine for status information. This function fills in
 * a text area with current status information. If the buffer (buf) is not
 * large enough, status information will be truncated to fit the buffer.
 */
int wpa_sm_get_status(struct wpa_sm *sm, char *buf, size_t buflen,
                      int verbose)
{
    char *pos = buf, *end = buf + buflen;
    int ret;

    ret = os_snprintf(pos, end - pos,
                      "pairwise_cipher=%s\n"
                      "group_cipher=%s\n"
                      "key_mgmt=%s\n",
                      wpa_cipher_txt(sm->pairwise_cipher),
                      wpa_cipher_txt(sm->group_cipher),
                      wpa_key_mgmt_txt(sm->key_mgmt, sm->proto));
    if (ret < 0 || ret >= end - pos)
        return pos - buf;
    pos += ret;
    return pos - buf;
}


/**
 * wpa_sm_set_assoc_wpa_ie_default - Generate own WPA/RSN IE from configuration
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @wpa_ie: Pointer to buffer for WPA/RSN IE
 * @wpa_ie_len: Pointer to the length of the wpa_ie buffer
 * Returns: 0 on success, -1 on failure
 */
int wpa_sm_set_assoc_wpa_ie_default(struct wpa_sm *sm, u8 *wpa_ie,
                                    size_t *wpa_ie_len)
{
    int res;

    if (sm == NULL)
        return -1;

    res = wpa_gen_wpa_ie(sm, wpa_ie, *wpa_ie_len);
    if (res < 0)
        return -1;
    *wpa_ie_len = res;

    wpa_hexdump(MSG_DEBUG, "WPA: Set own WPA IE default",
                wpa_ie, *wpa_ie_len);

    if (sm->assoc_wpa_ie == NULL)
    {
        /*
         * Make a copy of the WPA/RSN IE so that 4-Way Handshake gets
         * the correct version of the IE even if PMKSA caching is
         * aborted (which would remove PMKID from IE generation).
         */
        sm->assoc_wpa_ie = os_malloc(*wpa_ie_len);
        if (sm->assoc_wpa_ie == NULL)
            return -1;

        os_memcpy(sm->assoc_wpa_ie, wpa_ie, *wpa_ie_len);
        sm->assoc_wpa_ie_len = *wpa_ie_len;
    }

    return 0;
}


/**
 * wpa_sm_set_assoc_wpa_ie - Set own WPA/RSN IE from (Re)AssocReq
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @ie: Pointer to IE data (starting from id)
 * @len: IE length
 * Returns: 0 on success, -1 on failure
 *
 * Inform WPA state machine about the WPA/RSN IE used in (Re)Association
 * Request frame. The IE will be used to override the default value generated
 * with wpa_sm_set_assoc_wpa_ie_default().
 */
int wpa_sm_set_assoc_wpa_ie(struct wpa_sm *sm, const u8 *ie, size_t len)
{
    if (sm == NULL)
        return -1;

    os_free(sm->assoc_wpa_ie);
    if (ie == NULL || len == 0)
    {
        wpa_printf(MSG_DEBUG, "WPA: clearing own WPA/RSN IE");
        sm->assoc_wpa_ie = NULL;
        sm->assoc_wpa_ie_len = 0;
    }
    else
    {
        wpa_hexdump(MSG_DEBUG, "WPA: set own WPA/RSN IE", ie, len);
        sm->assoc_wpa_ie = os_malloc(len);
        if (sm->assoc_wpa_ie == NULL)
            return -1;

        os_memcpy(sm->assoc_wpa_ie, ie, len);
        sm->assoc_wpa_ie_len = len;
    }

    return 0;
}


/**
 * wpa_sm_set_ap_wpa_ie - Set AP WPA IE from Beacon/ProbeResp
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @ie: Pointer to IE data (starting from id)
 * @len: IE length
 * Returns: 0 on success, -1 on failure
 *
 * Inform WPA state machine about the WPA IE used in Beacon / Probe Response
 * frame.
 */
int wpa_sm_set_ap_wpa_ie(struct wpa_sm *sm, const u8 *ie, size_t len)
{
    if (sm == NULL)
        return -1;

    os_free(sm->ap_wpa_ie);
    if (ie == NULL || len == 0)
    {
        wpa_printf(MSG_DEBUG, "WPA: clearing AP WPA IE");
        sm->ap_wpa_ie = NULL;
        sm->ap_wpa_ie_len = 0;
    }
    else
    {
        wpa_hexdump(MSG_DEBUG, "WPA: set AP WPA IE", ie, len);
        sm->ap_wpa_ie = os_malloc(len);
        if (sm->ap_wpa_ie == NULL)
            return -1;

        os_memcpy(sm->ap_wpa_ie, ie, len);
        sm->ap_wpa_ie_len = len;
    }

    return 0;
}


/**
 * wpa_sm_set_ap_rsn_ie - Set AP RSN IE from Beacon/ProbeResp
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @ie: Pointer to IE data (starting from id)
 * @len: IE length
 * Returns: 0 on success, -1 on failure
 *
 * Inform WPA state machine about the RSN IE used in Beacon / Probe Response
 * frame.
 */
int wpa_sm_set_ap_rsn_ie(struct wpa_sm *sm, const u8 *ie, size_t len)
{
    if (sm == NULL)
        return -1;

    os_free(sm->ap_rsn_ie);
    if (ie == NULL || len == 0)
    {
        wpa_printf(MSG_DEBUG, "WPA: clearing AP RSN IE");
        sm->ap_rsn_ie = NULL;
        sm->ap_rsn_ie_len = 0;
    }
    else
    {
        wpa_hexdump(MSG_DEBUG, "WPA: set AP RSN IE", ie, len);
        sm->ap_rsn_ie = os_malloc(len);
        if (sm->ap_rsn_ie == NULL)
            return -1;

        os_memcpy(sm->ap_rsn_ie, ie, len);
        sm->ap_rsn_ie_len = len;
    }

    return 0;
}


/**
 * wpa_sm_parse_own_wpa_ie - Parse own WPA/RSN IE
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @data: Pointer to data area for parsing results
 * Returns: 0 on success, -1 if IE is not known, or -2 on parsing failure
 *
 * Parse the contents of the own WPA or RSN IE from (Re)AssocReq and write the
 * parsed data into data.
 */
int wpa_sm_parse_own_wpa_ie(struct wpa_sm *sm, struct wpa_ie_data *data)
{
    if (sm == NULL || sm->assoc_wpa_ie == NULL)
    {
        wpa_printf(MSG_DEBUG, "WPA: No WPA/RSN IE available from "
                   "association info");
        return -1;
    }
    if (wpa_parse_wpa_ie(sm->assoc_wpa_ie, sm->assoc_wpa_ie_len, data))
        return -2;
    return 0;
}
#endif
