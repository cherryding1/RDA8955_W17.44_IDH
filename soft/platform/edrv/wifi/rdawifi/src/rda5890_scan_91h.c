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


#ifdef  __SCAN_INFO__
#include "cs_types.h"

#include "sdio_config.h"
#include "wifi_config.h"
//#include "wifi_eapol_common.h"
//#include "wifi_wpa_common.h"
//#include "wifi_peerkey.h"
#include "tgt_mcd_cfg.h"
#include "sdio_m.h"
#include "sdio_debug.h"
//#include "mcdp_sdmmc.h"

//#include "hal_sdmmc.h"
#include "hal_sys.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "sxr_tls.h"
#include "sdmmc2.h"
#include "hal_sdio.h"
/*
#include "ifc_service.h"
#include "lp_timer_service.h"
*/
//#include "wifi_os.h"

//#include "wifi_common.h"

#include "sxs_io.h"
#include "sxr_sbx.h"
#include "cos.h"
#include "event.h"
#include "base_prv.h"
#include "wifi_ieee802_11_defs.h"
#include "rda5990_scan.h"
//#define SCAN_RESULT_DEBUG

//added by xiongzhi for wapi
#ifndef FCS_LEN
#define FCS_LEN                 4
#endif


#ifndef min
#define min(a,b)        ((a) < (b) ? (a) : (b))
#endif

struct bss_info_t
{
    unsigned char bss_infos[sizeof(RDA5990_BSS_INFO)*RDA5890_MAX_NETWORK_NUM ];
    UINT8 count;
} bss_list;
/* Element ID  of various Information Elements */
typedef enum {ISSID         = 0,   /* Service Set Identifier   */
              ISUPRATES     = 1,   /* Supported Rates          */
              IFHPARMS      = 2,   /* FH parameter set         */
              IDSPARMS      = 3,   /* DS parameter set         */
              ICFPARMS      = 4,   /* CF parameter set         */
              ITIM          = 5,   /* Traffic Information Map  */
              IIBPARMS      = 6,   /* IBSS parameter set       */
              ICTEXT        = 16,  /* Challenge Text           */
              IERPINFO      = 42,  /* ERP Information          */
              IEXSUPRATES   = 50,   /* Extended Supported Rates */
              IWAPI               =68
             } ELEMENTID_T;

/* Capability Information field bit assignments  */
typedef enum {ESS           = 0x01,   /* ESS capability               */
              IBSS          = 0x02,   /* IBSS mode                    */
              POLLABLE      = 0x04,   /* CF Pollable                  */
              POLLREQ       = 0x08,   /* Request to be polled         */
              PRIVACY       = 0x10,   /* WEP encryption supported     */
              SHORTPREAMBLE  = 0x20,   /* Short Preamble is supported  */
              SHORTSLOT      = 0x400,  /* Short Slot is supported      */
              PBCC          = 0x40,   /* PBCC                         */
              CHANNELAGILITY = 0x80,   /* Channel Agility              */
              SPECTRUM_MGMT = 0x100,  /* Spectrum Management          */
              DSSS_OFDM      = 0x2000  /* DSSS-OFDM                    */
             } CAPABILITY_T;



/* BSS type */
typedef enum {INFRASTRUCTURE  = 1,
              INDEPENDENT     = 2,
              ANY_BSS         = 3
             } BSSTYPE_T;



unsigned char* get_ie_elem(unsigned char* msa, ELEMENTID_T elm_id,
                           unsigned short rx_len,unsigned short tag_param_offset)
{
    unsigned short index = 0;

    /*************************************************************************/
    /*                       Beacon Frame - Frame Body                       */
    /* --------------------------------------------------------------------- */
    /* |Timestamp |BeaconInt |CapInfo |SSID |SupRates |DSParSet |TIM elm   | */
    /* --------------------------------------------------------------------- */
    /* |8         |2         |2       |2-34 |3-10     |3        |4-256     | */
    /* --------------------------------------------------------------------- */
    /*                                                                       */
    /*************************************************************************/

    index = tag_param_offset;

    /* Search for the TIM Element Field and return if the element is found */
    while(index < (rx_len - FCS_LEN))
    {
        if(msa[index] == elm_id)
        {
            return(&msa[index]);
        }
        else
        {
            index += (2 + msa[index + 1]);
        }
    }

    return(0);
}

/* This function extracts the 'from ds' bit from the MAC header of the input */
/* frame.                                                                    */
/* Returns the value in the LSB of the returned value.                       */
unsigned char get_from_ds(unsigned char* header)
{
    return ((header[1] & 0x02) >> 1);
}

/* This function extracts the 'to ds' bit from the MAC header of the input   */
/* frame.                                                                    */
/* Returns the value in the LSB of the returned value.                       */
unsigned char get_to_ds(unsigned char* header)
{
    return (header[1] & 0x01);
}

/* This function extracts the MAC Address in 'address1' field of the MAC     */
/* header and updates the MAC Address in the allocated 'addr' variable.      */
void get_address1(unsigned char* msa, unsigned char* addr)
{
    memcpy(addr, msa + 4, 6);
}

/* This function extracts the MAC Address in 'address2' field of the MAC     */
/* header and updates the MAC Address in the allocated 'addr' variable.      */
void get_address2(unsigned char* msa, unsigned char* addr)
{
    memcpy(addr, msa + 10, 6);
}

/* This function extracts the MAC Address in 'address3' field of the MAC     */
/* header and updates the MAC Address in the allocated 'addr' variable.      */
void get_address3(unsigned char* msa, unsigned char* addr)
{
    memcpy(addr, msa + 16, 6);
}

/* This function extracts the BSSID from the incoming WLAN packet based on   */
/* the 'from ds' bit, and updates the MAC Address in the allocated 'addr'    */
/* variable.                                                                 */
void get_BSSID(unsigned char* data, unsigned char* bssid)
{
    if(get_from_ds(data) == 1)
        get_address2(data, bssid);
    else if(get_to_ds(data) == 1)
        get_address1(data, bssid);
    else
        get_address3(data, bssid);
}

extern int is_same_network(struct bss_descriptor *src,
                           struct bss_descriptor *dst);
extern void clear_bss_descriptor(struct bss_descriptor *bss);
UINT8* get_free_bss_info_node(void);

int is_ap_support_11b(unsigned char* rates)
{
    int i = 0;
    for(i = 0; i <= MAX_RATES; i ++)
    {
        if(rates[i] == 0x8C)
            return 0;
    }
    return 1;
}


void show_bss_info_node(void);
RDA5890_BSS_NETWORK_INFO* find_bss_info_node_Mac(UINT8 * mac);
UINT8 network_info_time =0;
struct bss_descs_t
{
    unsigned char bss_descs[sizeof(RDA5890_BSS_DESC)*RDA5890_MAX_NETWORK_NUM ];
#ifdef __SCAN_INFO__
    unsigned char bss_info[sizeof(RDA5890_BSS_NETWORK_INFO)*RDA5890_MAX_NETWORK_NUM ];
#endif
    UINT8 count;
};
extern struct bss_descs_t bss_Rst;

static UINT32 wifi_ScanSingleRsp(RDA5890_BSS_DESC * bss_desc, UINT8 index)
{
    MCD_TRACE(MCD_INFO_TRC, 0, "wifi_ScanSingleRsp\n");
    COS_EVENT ev;

    ev.nEventId = EV_CFW_WIFI_SCAN_SINGLE_RSP;
    ev.nParam1  = bss_desc;
    ev.nParam2  = index;
    COS_SendEvent(COS_GetDefaultMmiTaskHandle(DEFAULT_MMI_TASK_ID), &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);

    return 0;
}
extern int rda5890_network_dot11_mode(u8 *bss_wpa, size_t wpa_ie_len, u8 *bss_rsn, size_t rsn_ie_len);

void rda5890_network_information(UINT8 *info, unsigned short info_len)
{
    //hal_HstSendEvent(0x01130002);
    network_info_time++;


    RDA5890_BSS_NETWORK_INFO* bss;
    RDA5890_BSS_NETWORK_INFO*   tmpbss;
    RDA5890_BSS_NETWORK_INFO add_bss;
    bss = &add_bss;
    tmpbss = (RDA5890_BSS_NETWORK_INFO*)NULL;

    unsigned char  *pos, *end, *p;
    unsigned char n_ex_rates = 0, got_basic_rates = 0, n_basic_rates = 0;

    unsigned char* msa = &info[9];
    unsigned short msa_len = info[6] | (info[7] << 8);


    if((msa_len - 1 + 9 ) != info_len)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"rda5890_network_information verify lengh feild failed,  msa_len: %d,  info_len:%d , \n", msa_len, info_len);
    }

    memset(bss, 0, sizeof (RDA5890_BSS_NETWORK_INFO));
	bss->rssi = info[8];
    msa_len -= 1; // has rssi

    get_BSSID(msa, bss->bssid);

    end = msa + msa_len;

    //mac head
    pos = msa + 24;
    //time stamp
    pos += 8;
    //beacon
    bss->beaconperiod = *(pos) | (*(pos + 1) << 8);
    pos += 2 ;
    //capability
    bss->capability = *(pos) | (*(pos + 1) << 8);
    pos += 2;

    if (bss->capability & WLAN_CAPABILITY_IBSS)
        bss->mode = INDEPENDENT;
    // bss->mode = IW_MODE_ADHOC;
    else
        bss->mode = INFRASTRUCTURE;
    // bss->mode = IW_MODE_INFRA;

    /* process variable IE */
    // UINT8* tmp
    while( (pos + 2 <= end)&&((pos+ 2-msa) <=info_len))
    {
        if (pos + pos[1] > end)
        {
#ifdef SCAN_RESULT_DEBUG
            MCD_TRACE(MCD_INFO_TRC, 0,"process_bss: error in processing IE, "
                      "bytes left < IE length\n");
#endif
            break;
        }

        switch (pos[0])
        {

            case WLAN_EID_SSID:
                bss->ssid_len = min( IW_ESSID_MAX_SIZE, pos[1]);
                memcpy(bss->ssid, pos + 2, bss->ssid_len);
				bss->ssid[bss->ssid_len] = '\0';
#ifdef SCAN_RESULT_DEBUG
                MCD_TRACE(MCD_INFO_TRC, 0,"got SSID IE:  len %d %d\n",   bss->ssid_len, pos[1]);
                showString((UINT8*)bss->ssid);

#endif
                break;

            case WLAN_EID_SUPP_RATES:
                n_basic_rates = min( MAX_RATES, pos[1]);
                memcpy(bss->rates, pos + 2, n_basic_rates);
                got_basic_rates = 1;
#ifdef SCAN_RESULT_DEBUG
                MCD_TRACE(MCD_INFO_TRC, 0,"got RATES IE\n");
#endif
                break;

            case WLAN_EID_FH_PARAMS:
#ifdef SCAN_RESULT_DEBUG
                MCD_TRACE(MCD_INFO_TRC, 0,"got FH IE\n");
#endif
                break;

            case WLAN_EID_DS_PARAMS:
#ifdef SCAN_RESULT_DEBUG
                MCD_TRACE(MCD_INFO_TRC, 0,"got DS IE\n");
#endif
                bss->channel = pos[2];
                break;

            case WLAN_EID_CF_PARAMS:
#ifdef SCAN_RESULT_DEBUG
                MCD_TRACE(MCD_INFO_TRC, 0,"got CF IE\n");
#endif
                break;

            case WLAN_EID_IBSS_PARAMS:
#ifdef SCAN_RESULT_DEBUG
                MCD_TRACE(MCD_INFO_TRC, 0,"got IBSS IE\n");
#endif
                break;

            case WLAN_EID_COUNTRY:
                //  pcountryinfo = (struct ieee_ie_country_info_set *) pos;
#ifdef SCAN_RESULT_DEBUG
                MCD_TRACE(MCD_INFO_TRC, 0,"got COUNTRY IE\n");
#endif
                break;

            case WLAN_EID_EXT_SUPP_RATES:
                /* only process extended supported rate if data rate is
                 * already found. Data rate IE should come before
                 * extended supported rate IE
                 */
#ifdef SCAN_RESULT_DEBUG
                MCD_TRACE(MCD_INFO_TRC, 0,"got RATESEX IE\n");
#endif
                if (!got_basic_rates)
                {
#ifdef SCAN_RESULT_DEBUG
                    MCD_TRACE(MCD_INFO_TRC, 0,"... but ignoring it\n");
#endif
                    break;
                }

                n_ex_rates = pos[1];
                if (n_basic_rates + n_ex_rates > MAX_RATES)
                    n_ex_rates = MAX_RATES - n_basic_rates;

                p = bss->rates + n_basic_rates;
                memcpy(p, pos + 2, n_ex_rates);
                break;

            //case WLAN_EID_GENERIC:
            case  WLAN_EID_VENDOR_SPECIFIC:
                if (pos[1] >= 4 &&
                        pos[2] == 0x00 && pos[3] == 0x50 &&
                        pos[4] == 0xf2 && pos[5] == 0x01)
                {
                    bss->wpa_ie_len = min(pos[1] + 2, MAX_WPA_IE_LEN);
                    memcpy(bss->wpa_ie, pos, bss->wpa_ie_len);
#ifdef SCAN_RESULT_DEBUG
                    MCD_TRACE(MCD_INFO_TRC, 0,"got WPA IE \n");
#endif
                }
                else
                {
#ifdef SCAN_RESULT_DEBUG
                    MCD_TRACE(MCD_INFO_TRC, 0,"got generic IE: %02x:%02x:%02x:%02x, len %d\n",
                              pos[2], pos[3],
                              pos[4], pos[5],
                              pos[1]);
#endif
                }
                break;

            case WLAN_EID_RSN:
#ifdef SCAN_RESULT_DEBUG
                MCD_TRACE(MCD_INFO_TRC, 0,"got RSN IE\n");
#endif
                bss->rsn_ie_len = min(pos[1] + 2, MAX_WPA_IE_LEN);
                memcpy(bss->rsn_ie, pos, bss->rsn_ie_len);
                break;

            case IWAPI:
#ifdef SCAN_RESULT_DEBUG
                MCD_TRACE(MCD_INFO_TRC, 0,"got WAPI IE\n");
#endif
                bss->wapi_ie_len = min(pos[1] + 2, 100);
                memcpy(bss->wapi_ie, pos, bss->wapi_ie_len);
                break;

            default:
                break;
        }

        pos += pos[1] + 2;
    }

    //  bss->last_scanned = jiffies;
    if(bss_Rst.count < RDA5890_MAX_NETWORK_NUM)
	{
		RDA5890_BSS_DESC* tmp_p;
		UINT8 item_len = sizeof(RDA5890_BSS_DESC);
		tmp_p = (RDA5890_BSS_DESC*)bss_Rst.bss_descs;
		int j;
		for(j=0; j <  bss_Rst.count; j++)
		{
			if(strncmp(tmp_p[j].bssid, bss->bssid, ETH_ALEN)==0)
				break;
		}

		if(j == bss_Rst.count)
		{
			bss_Rst.count++;

            RDA5890_BSS_DESC * bss_desc = COS_MALLOC(sizeof(RDA5890_BSS_DESC));
            memcpy(bss_desc->bssid, bss->bssid, ETH_ALEN);
		    strcpy(bss_desc->ssid, bss->ssid);
		    bss_desc->channel = bss->channel;
		    bss_desc->rssi = bss->rssi;
            bss_desc->dot11i_info = rda5890_network_dot11_mode(bss->wpa_ie, bss->wpa_ie_len, bss->rsn_ie, bss->rsn_ie_len);

            sys_arch_printf("ssid: %s, BSSID:%02x:%02x:%02x:%02x:%02x:%02x\n",bss_desc->ssid,
		                            bss_desc->bssid[0], bss_desc->bssid[1], bss_desc->bssid[2], bss_desc->bssid[3], bss_desc->bssid[4], bss_desc->bssid[5]);
	        sys_arch_printf("channel:%d, rssi:%u\n",bss_desc->channel, bss_desc->rssi);
            sys_arch_printf("dot11_mode : %d\n",bss_desc->dot11i_info);

            wifi_ScanSingleRsp(bss_desc,j);
		}

		memcpy(tmp_p[j].bssid, bss->bssid, ETH_ALEN);
		strcpy(tmp_p[j].ssid, bss->ssid);
		tmp_p[j].channel = bss->channel;
		tmp_p[j].rssi = (tmp_p[j].bssid>bss->rssi) ? tmp_p[j].bssid : bss->rssi;

        tmpbss = find_bss_info_node_Mac(bss->bssid);

        if(tmpbss==0)
        {
            tmpbss = get_free_bss_info_node();
            if(tmpbss==0 )
            {
                show_bss_info_node();
                MCD_TRACE(MCD_INFO_TRC, 0,"#####no free node \n");

                return ;
            }
        }

        memcpy(tmpbss,bss,sizeof(RDA5890_BSS_NETWORK_INFO));
    }
    //if(network_info_time>50)
        //sxr_Sleep(163);
#ifdef SCAN_RESULT_DEBUG
    MCD_TRACE(MCD_INFO_TRC, 0,"rda5890_network_information ret \n");
#endif

    return;
}

#endif

