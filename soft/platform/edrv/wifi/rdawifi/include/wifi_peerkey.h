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



#ifndef WIFI_PEERKEY_H
#define WIFI_PEERKEY_H

#define PEERKEY_MAX_IE_LEN 80
struct wpa_peerkey
{
    struct wpa_peerkey *next;
    int initiator; /* whether this end was initator for SMK handshake */
    u8 addr[ETH_ALEN]; /* other end MAC address */
    u8 inonce[WPA_NONCE_LEN]; /* Initiator Nonce */
    u8 pnonce[WPA_NONCE_LEN]; /* Peer Nonce */
    u8 rsnie_i[PEERKEY_MAX_IE_LEN]; /* Initiator RSN IE */
    size_t rsnie_i_len;
    u8 rsnie_p[PEERKEY_MAX_IE_LEN]; /* Peer RSN IE */
    size_t rsnie_p_len;
    u8 smk[PMK_LEN];
    int smk_complete;
    u8 smkid[PMKID_LEN];
    u32 lifetime;
    os_time_t expiration;
    int cipher; /* Selected cipher (WPA_CIPHER_*) */
    u8 replay_counter[WPA_REPLAY_COUNTER_LEN];
    int replay_counter_set;
    int use_sha256; /* whether AKMP indicate SHA256-based derivations */

    struct wpa_ptk stk, tstk;
    int stk_set, tstk_set;
};


#ifdef CONFIG_PEERKEY

int peerkey_verify_eapol_key_mic(struct wpa_sm *sm,
                                 struct wpa_peerkey *peerkey,
                                 struct wpa_eapol_key *key, u16 ver,
                                 const u8 *buf, size_t len);
void peerkey_rx_eapol_4way(struct wpa_sm *sm, struct wpa_peerkey *peerkey,
                           struct wpa_eapol_key *key, u16 key_info, u16 ver);
void peerkey_rx_eapol_smk(struct wpa_sm *sm, const u8 *src_addr,
                          struct wpa_eapol_key *key, size_t extra_len,
                          u16 key_info, u16 ver);
void peerkey_deinit(struct wpa_sm *sm);

#else /* CONFIG_PEERKEY */

static inline int
peerkey_verify_eapol_key_mic(struct wpa_sm *sm,
                             struct wpa_peerkey *peerkey,
                             struct wpa_eapol_key *key, u16 ver,
                             const u8 *buf, size_t len)
{
    return -1;
}

static inline void
peerkey_rx_eapol_4way(struct wpa_sm *sm, struct wpa_peerkey *peerkey,
                      struct wpa_eapol_key *key, u16 key_info, u16 ver)
{
}

static inline void
peerkey_rx_eapol_smk(struct wpa_sm *sm, const u8 *src_addr,
                     struct wpa_eapol_key *key, size_t extra_len,
                     u16 key_info, u16 ver)
{
}

static inline void peerkey_deinit(struct wpa_sm *sm)
{
}

#endif /* CONFIG_PEERKEY */

#endif /* PEERKEY_H */
