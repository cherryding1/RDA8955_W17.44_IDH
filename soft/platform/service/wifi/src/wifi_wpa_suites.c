#include "csw.h"
#include "base_prv.h"
#include "sxs_io.h"
#include "wifi_config.h"
#include "wifi_wpa_common.h"
#include "wifi_ieee802_11_defs.h"
#include "wifi_defs.h"

#define WIFI_TS_ID_SXS  9

#define MCD_TRACE(level, tstmap, format, ...) \
	CSW_TRACE(_CSW|TLEVEL(WIFI_TS_ID_SXS)|TDB|TNB_ARG(2), TSTXT(TSTR(format,0x0870000c)), format);

#define MCD_INFO_TRC 0

static int rsn_selector_to_bitfield(const u8 *s)
{
    if (RSN_SELECTOR_GET(s) == RSN_CIPHER_SUITE_NONE)
        return WPA_CIPHER_NONE;
    if (RSN_SELECTOR_GET(s) == RSN_CIPHER_SUITE_WEP40)
        return WPA_CIPHER_WEP40;
    if (RSN_SELECTOR_GET(s) == RSN_CIPHER_SUITE_TKIP)
        return WPA_CIPHER_TKIP;
    if (RSN_SELECTOR_GET(s) == RSN_CIPHER_SUITE_CCMP)
        return WPA_CIPHER_CCMP;
    if (RSN_SELECTOR_GET(s) == RSN_CIPHER_SUITE_WEP104)
        return WPA_CIPHER_WEP104;
    return 0;
}

static int rsn_key_mgmt_to_bitfield(const u8 *s)
{
    if (RSN_SELECTOR_GET(s) == RSN_AUTH_KEY_MGMT_UNSPEC_802_1X)
        return WPA_KEY_MGMT_IEEE8021X;
    if (RSN_SELECTOR_GET(s) == RSN_AUTH_KEY_MGMT_PSK_OVER_802_1X)
        return WPA_KEY_MGMT_PSK;
    return 0;
}

static int wpa_selector_to_bitfield(const u8 *s)
{
    if (RSN_SELECTOR_GET(s) == WPA_CIPHER_SUITE_NONE)
        return WPA_CIPHER_NONE;
    if (RSN_SELECTOR_GET(s) == WPA_CIPHER_SUITE_WEP40)
        return WPA_CIPHER_WEP40;
    if (RSN_SELECTOR_GET(s) == WPA_CIPHER_SUITE_TKIP)
        return WPA_CIPHER_TKIP;
    if (RSN_SELECTOR_GET(s) == WPA_CIPHER_SUITE_CCMP)
        return WPA_CIPHER_CCMP;
    if (RSN_SELECTOR_GET(s) == WPA_CIPHER_SUITE_WEP104)
        return WPA_CIPHER_WEP104;
    return 0;
}

static int wpa_key_mgmt_to_bitfield(const u8 *s)
{
    if (RSN_SELECTOR_GET(s) == WPA_AUTH_KEY_MGMT_UNSPEC_802_1X)
        return WPA_KEY_MGMT_IEEE8021X;
    if (RSN_SELECTOR_GET(s) == WPA_AUTH_KEY_MGMT_PSK_OVER_802_1X)
        return WPA_KEY_MGMT_PSK;
    if (RSN_SELECTOR_GET(s) == WPA_AUTH_KEY_MGMT_NONE)
        return WPA_KEY_MGMT_WPA_NONE;
    return 0;
}
int wpa_parse_wpa_ie_rsn(const u8 *rsn_ie, u32 rsn_ie_len,
             struct wpa_ie_data *data)
{
    const struct rsn_ie_hdr *hdr;
    const u8 *pos;
    int left;
    int i, count;

    if (rsn_ie_len == 0) {
        /* No RSN IE - fail silently */
        return -1;
    }

    if (rsn_ie_len < sizeof(struct rsn_ie_hdr)) {
        MCD_TRACE(MCD_INFO_TRC, 0," ie len too short %lu!\r\n", (unsigned long) rsn_ie_len);
        return -1;
    }

    hdr = (const struct rsn_ie_hdr *) rsn_ie;

    if (hdr->elem_id != WLAN_EID_RSN ||
        hdr->len != rsn_ie_len - 2 ||
        WPA_GET_LE16(hdr->version) != RSN_VERSION) {
        MCD_TRACE(MCD_INFO_TRC, 0," malformed ie or unknown version!\r\n");
        return -2;
    }

    pos = (const u8 *) (hdr + 1);
    left = rsn_ie_len - sizeof(*hdr);

    if (left >= RSN_SELECTOR_LEN) {
        data->group_cipher = rsn_selector_to_bitfield(pos);
        pos += RSN_SELECTOR_LEN;
        left -= RSN_SELECTOR_LEN;
    } else if (left > 0) {
        MCD_TRACE(MCD_INFO_TRC, 0,"ie length mismatch, %u too much\r\n", left);
        return -3;
    }

    if (left >= 2) {
        data->pairwise_cipher = 0;
        count = WPA_GET_LE16(pos);
        pos += 2;
        left -= 2;
        if (count == 0 || left < count * RSN_SELECTOR_LEN) {
            MCD_TRACE(MCD_INFO_TRC, 0,"ie count botch (pairwise) count %u left %u\r\n", count, left);
            return -4;
        }
        for (i = 0; i < count; i++) {
            data->pairwise_cipher |= rsn_selector_to_bitfield(pos);
            pos += RSN_SELECTOR_LEN;
            left -= RSN_SELECTOR_LEN;
        }
    } else if (left == 1) {
        MCD_TRACE(MCD_INFO_TRC, 0,"ie too short (for key mgmt)\r\n");
        return -5;
    }

    if (left >= 2) {
        data->key_mgmt = 0;
        count = WPA_GET_LE16(pos);
        pos += 2;
        left -= 2;
        if (count == 0 || left < count * RSN_SELECTOR_LEN) {
            MCD_TRACE(MCD_INFO_TRC, 0,"ie count botch (key mgmt), count %u left %u\r\n", count, left);
            return -6;
        }
        for (i = 0; i < count; i++) {
            data->key_mgmt |= rsn_key_mgmt_to_bitfield(pos);
            pos += RSN_SELECTOR_LEN;
            left -= RSN_SELECTOR_LEN;
        }
    } else if (left == 1) {
        MCD_TRACE(MCD_INFO_TRC, 0, "ie too short (for capabilities)\r\n");
        return -7;
    }

    if (left >= 2) {
	char * capabileities_0 = pos;
        data->capabilities = WPA_GET_LE16(pos);
		capabileities_0[0]  &= (~0x0c);
        pos += 2;
        left -= 2;
    }

    if (left >= 2) {
        data->num_pmkid = WPA_GET_LE16(pos);
        pos += 2;
        left -= 2;
        if (left < (int) data->num_pmkid * PMKID_LEN) {
            MCD_TRACE(MCD_INFO_TRC, 0,"PMKID underflow (num_pmkid=%lu left=%d)\r\n", (unsigned long) data->num_pmkid, left);
            data->num_pmkid = 0;
            return -9;
        } else {
            data->pmkid = pos;
            pos += data->num_pmkid * PMKID_LEN;
            left -= data->num_pmkid * PMKID_LEN;
        }
    }

    if (left > 0) {
        MCD_TRACE(MCD_INFO_TRC, 0,"ie has %u trailing bytes - ignored\r\n", left);
    }

    return 0;
}

int wpa_parse_wpa_ie_wpa(const u8 *wpa_ie, size_t wpa_ie_len,
             struct wpa_ie_data *data)
{
    const struct wpa_ie_hdr *hdr;
    const u8 *pos;
    int left;
    int i, count;

    if (wpa_ie_len == 0) {
        /* No WPA IE - fail silently */
        return -1;
    }

    if (wpa_ie_len < sizeof(struct wpa_ie_hdr)) {
        MCD_TRACE(MCD_INFO_TRC, 0,"ie len too short %lu\r\n",(unsigned long) wpa_ie_len);
        return -1;
    }

    hdr = (const struct wpa_ie_hdr *) wpa_ie;

    if (hdr->elem_id != WLAN_EID_VENDOR_SPECIFIC ||
        hdr->len != wpa_ie_len - 2 ||
        RSN_SELECTOR_GET(hdr->oui) != WPA_OUI_TYPE ||
        WPA_GET_LE16(hdr->version) != WPA_VERSION) {
        MCD_TRACE(MCD_INFO_TRC, 0,"malformed ie or unknown version\r\n");
        return -2;
    }

    pos = (const u8 *) (hdr + 1);
    left = wpa_ie_len - sizeof(*hdr);

    if (left >= WPA_SELECTOR_LEN) {
        data->group_cipher = wpa_selector_to_bitfield(pos);
        pos += WPA_SELECTOR_LEN;
        left -= WPA_SELECTOR_LEN;
    } else if (left > 0) {
        MCD_TRACE(MCD_INFO_TRC, 0,"ie length mismatch, %u too much\r\n",left);
        return -3;
    }

    if (left >= 2) {
        data->pairwise_cipher = 0;
        count = WPA_GET_LE16(pos);
        pos += 2;
        left -= 2;
        if (count == 0 || left < count * WPA_SELECTOR_LEN) {
            MCD_TRACE(MCD_INFO_TRC, 0,"ie count botch (pairwise), count %u left %u\r\n", count, left);
            return -4;
        }
        for (i = 0; i < count; i++) {
            data->pairwise_cipher |= wpa_selector_to_bitfield(pos);
            pos += WPA_SELECTOR_LEN;
            left -= WPA_SELECTOR_LEN;
        }
    } else if (left == 1) {
        MCD_TRACE(MCD_INFO_TRC, 0,"ie too short (for key mgmt)\r\n");
        return -5;
    }

    if (left >= 2) {
        data->key_mgmt = 0;
        count = WPA_GET_LE16(pos);
        pos += 2;
        left -= 2;
        if (count == 0 || left < count * WPA_SELECTOR_LEN) {
            MCD_TRACE(MCD_INFO_TRC, 0,"ie count botch (key mgmt), count %u left %u\r\n", count, left);
            return -6;
        }
        for (i = 0; i < count; i++) {
            data->key_mgmt |= wpa_key_mgmt_to_bitfield(pos);
            pos += WPA_SELECTOR_LEN;
            left -= WPA_SELECTOR_LEN;
        }
    } else if (left == 1) {
        MCD_TRACE(MCD_INFO_TRC, 0,"ie too short (for capabilities)\r\n");
        return -7;
    }

    if (left >= 2) {
        data->capabilities = WPA_GET_LE16(pos);
        pos += 2;
        left -= 2;
    }

    if (left > 0) {
        MCD_TRACE(MCD_INFO_TRC, 0,"ie has %u trailing bytes - ignored\r\n", left);
    }

    return 0;
}
/*
int wpa_parse_wpa_ie(const u8 *wpa_ie, size_t wpa_ie_len,
             struct wpa_ie_data *data)
{
    if (wpa_ie_len >= 1 && wpa_ie[0] == WLAN_EID_RSN)
        return wpa_parse_wpa_ie_rsn(wpa_ie, wpa_ie_len, data);
    else
        return wpa_parse_wpa_ie_wpa(wpa_ie, wpa_ie_len, data);
}
*/

const u8 * wpa_bss_get_vendor_ie(const u8 *bss, size_t wpa_ie_len, u32 vendor_type)
{
    const u8 *end, *pos;

    pos = (const u8 *) bss;
    end = pos + wpa_ie_len;

    while (pos + 1 < end) {
        if (pos + 2 + pos[1] > end)
            break;
        if (pos[0] == WLAN_EID_VENDOR_SPECIFIC && pos[1] >= 4 &&
            vendor_type == WPA_GET_BE32(&pos[2]))
            return pos;
        pos += 2 + pos[1];
    }

    return NULL;
}

const u8 * wpa_bss_get_ie(const u8 *bss, size_t wpa_ie_len, u8 ie)
{
    const u8 *end, *pos;

    pos = (const u8 *)bss;
    end = pos + wpa_ie_len;

    while (pos + 1 < end) {
        if (pos + 2 + pos[1] > end)
            break;
        if (pos[0] == ie)
            return pos;
        pos += 2 + pos[1];
    }

    return NULL;
}


extern wifi_cxt_t wifi_context;

extern WPA_SM_T sm_cxt ;

int wpa_supplicant_set_suites(u8 *bss_wpa, size_t wpa_ie_len, u8 *bss_rsn, size_t rsn_ie_len)
{
	struct wpa_ie_data ie_data;
	memset(&ie_data, 0, sizeof(ie_data));
	MCD_TRACE(MCD_INFO_TRC, 0,"set_suites, dot11_mode:%x, wpa_ie_len:%d, rsn_ie_len:%d\n",
		wifi_context.dot11_mode, wpa_ie_len, rsn_ie_len);

	wifi_context.auth_type = OPEN_SYSTEM;

	if (rsn_ie_len) {
		if (wifi_context.dot11_mode == NO_ENCRYPT)
			return -1;
		sm_cxt.proto = WPA_PROTO_RSN;
		sm_cxt.rsn_enabled = 1;
		wifi_context.dot11_mode |= WPA2;
		wpa_parse_wpa_ie_rsn(bss_rsn, bss_rsn[1]+2, &ie_data);
	} else if (wpa_ie_len) {
		if (wifi_context.dot11_mode == NO_ENCRYPT)
			return -2;
		sm_cxt.proto = WPA_PROTO_WPA;
		wifi_context.dot11_mode |= WPA;
		wpa_parse_wpa_ie_wpa(bss_wpa, bss_wpa[1]+2, &ie_data);
	}

	MCD_TRACE(MCD_INFO_TRC, 0,"set_suites, pariwise_cipher:%x, group_cipher:%x\n", ie_data.pairwise_cipher, ie_data.group_cipher);

	if (ie_data.pairwise_cipher & WPA_CIPHER_CCMP)
		wifi_context.dot11_mode |= AES;
	else if (ie_data.pairwise_cipher & WPA_CIPHER_TKIP)
		wifi_context.dot11_mode |= TKIP;

	if (ie_data.group_cipher & WPA_CIPHER_CCMP)
	        wifi_context.dot11_mode |= AES;
	else if (ie_data.group_cipher & WPA_CIPHER_TKIP)
		wifi_context.dot11_mode |= TKIP;


	if (wifi_context.dot11_mode == ENCRYPT_ENABLED) {
		u8 pw_len = strlen(wifi_context.cur_pwd);
		wifi_context.auth_type = SHARED_KEY;
		if (pw_len==5 || pw_len==10)
			wifi_context.dot11_mode |= WEP;
		else if (pw_len==13 || pw_len==26)
			wifi_context.dot11_mode |= (WEP | WEP_EXTENDED);
		else {
			MCD_TRACE(MCD_INFO_TRC, 0,"wpa_supplicant_set_suites, wep pw length error.%d\n", pw_le);
			return -3;
		}
	}
	MCD_TRACE(MCD_INFO_TRC, 0, "wifi_context.dot11_mode =%02x\n", wifi_context.dot11_mode);

	sm_cxt.pairwise_cipher  = ie_data.pairwise_cipher;
	sm_cxt.group_cipher  = ie_data.group_cipher;

	if(sm_cxt.proto == WPA_PROTO_RSN)
		get_assoRsnIe(sm_cxt.assoc_wpa_ie, &(sm_cxt.assoc_wpa_ie_len));
	else if(sm_cxt.proto == WPA_PROTO_WPA)
		get_assoWpaIe(sm_cxt.assoc_wpa_ie, &(sm_cxt.assoc_wpa_ie_len));

	sm_cxt.key_mgmt = ie_data.key_mgmt;

	return 0;
}

int rda5890_network_dot11_mode(u8 *bss_wpa, size_t wpa_ie_len, u8 *bss_rsn, size_t rsn_ie_len)
{
	struct wpa_ie_data ie_data;
    UINT8 dot11_mode = NO_ENCRYPT;
	memset(&ie_data, 0, sizeof(ie_data));

	if (rsn_ie_len) {
		dot11_mode |= WPA2|ENCRYPT_ENABLED;
		wpa_parse_wpa_ie_rsn(bss_rsn, bss_rsn[1]+2, &ie_data);
	} else if (wpa_ie_len) {
		dot11_mode |= WPA|ENCRYPT_ENABLED;
		wpa_parse_wpa_ie_wpa(bss_wpa, bss_wpa[1]+2, &ie_data);
	}

	if (ie_data.pairwise_cipher & WPA_CIPHER_CCMP)
		dot11_mode |= AES|ENCRYPT_ENABLED;
	else if (ie_data.pairwise_cipher & WPA_CIPHER_TKIP)
		dot11_mode |= TKIP|ENCRYPT_ENABLED;

	if (ie_data.group_cipher & WPA_CIPHER_CCMP)
        dot11_mode |= AES|ENCRYPT_ENABLED;
	else if (ie_data.group_cipher & WPA_CIPHER_TKIP)
		dot11_mode |= TKIP|ENCRYPT_ENABLED;

    sys_arch_printf("rda5890 dot11_mode : %x\n",dot11_mode);
    // 0:open 1:wep 2:wpa_psk(tkip) 3:wpa2_psk(aes) 4:wpa_wpa2_psk

    if(0 == dot11_mode)
        return 0;
    else if((0 != (dot11_mode & TKIP)) && (0 != (dot11_mode & AES)))
        return 4;
    else if((0 != (dot11_mode & TKIP)) && (0 == (dot11_mode & AES)))
        return 2;
    else if((0 == (dot11_mode & TKIP)) && (0 != (dot11_mode & AES)))
        return 3;
    else if((0 == (dot11_mode & WEP)) || (0 == (dot11_mode & WEP_EXTENDED)))
        return 1;
    else
	    return dot11_mode;
}


