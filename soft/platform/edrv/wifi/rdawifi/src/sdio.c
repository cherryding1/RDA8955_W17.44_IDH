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

#include "wifi_patch_d.h"
#include "wifi_patch_e.h"
#include "pmd_m.h"


// =============================================================================
//  MACROS
// =============================================================================

#define SDIO_MAX_BLOCK_NUM       128

#define SDIO_BLOCK_SIZE           512
//#define USE_SDIO_DMA              1
//#define USE_WIFI_SLEEP              0
#define HAL_TICK1S       16384

#define SECOND        * HAL_TICK1S
#define MILLI_SECOND  SECOND / 1000
#define MILLI_SECONDS MILLI_SECOND
// Timeouts
#define MCD_READ_TIMEOUT       ( 1 SECOND )
#define MCD_WRITE_TIMEOUT      ( 1 SECOND )


#define WIFI_VERSION_D  0x7

#define WIFI_VERSION_E  0x4
#define WIFI_VERSION_F  0x5
extern UINT8 wifi_version_flag ;


#define MCD_CMD_TIMEOUT_V1        ( 1 SECOND / 1 )

#define WIFI_ASSO_TIMEOUT     (5 SECOND)

// =============================================================================
// EVENT
// =============================================================================


// =============================================================================
// TYPES
// =============================================================================

typedef struct
{
    UINT8   mid;
    UINT32  oid;
    UINT32  pnm1;
    UINT8   pnm2;
    UINT8   prv;
    UINT32  psn;
    UINT32  mdt;
    UINT8   crc;
} MCD_CID_FORMAT_T;



// =============================================================================
// Global variables
// =============================================================================

// Spec Ver2 p96
//#define MCD_SDIO_OCR_VOLT_PROF_MASK  0x00ff8000


/// Relative Card Address Register
/// Nota RCA is sixteen bit long, but is always used
/// as the 16 upper bits of a 32 bits word. Therefore
/// is variable is in fact (RCA<<16), to avoid operation
/// (shift, unshift), to always place the RCA value as the
/// upper bits of a data word.
PRIVATE UINT32 g_SdioRcaReg = 0x00000000;

// Driver Stage Register p.118
// (To adjust bus capacitance)
// TODO Tune and put in calibration ?


PRIVATE UINT32 g_SdioFreq = 200000;

PRIVATE BOOL g_SdioIsDmaEnable = FALSE;

PRIVATE MCD_STATUS_T      g_mcdStatus = MCD_STATUS_NOTOPEN_PRESENT;

//PRIVATE CONST TGT_MCD_CONFIG_T*     g_mcdConfig=NULL;
//PRIVATE MCD_CARD_DETECT_HANDLER_T   g_mcdCardDetectHandler=NULL;

/// Semaphore to ensure proper concurrency of the MCD accesses
/// among all tasks.
PRIVATE UINT8           g_SdioSemaphore   = 0xFF;

/// Current in-progress transfer, if any.
PRIVATE HAL_SDIO_TRANSFER_T g_SdioTransfer =
{
    .sysMemAddr = 0,
    .sdCardAddr = 0,
    .blockNum   = 0,
    .blockSize  = 0,
    .direction  = HAL_SDIO_DIRECTION_WRITE,
};

//PRIVATE MCD_CSD_T g_mcdLatestCsd;

UINT32 ioport_w= 0x07;
UINT32 ioport_r= 0x08;
UINT32 ioport  =0x00;
UINT8 wifiDataInd = 0x00;

BOOL pool_dataIn = FALSE;
// =============================================================================
// Functions
// =============================================================================

// =============================================================================
// Functions for the HAL Driver ?
// =============================================================================

/// Macro to easily implement concurrency in the MCD driver.
/// Enter in the 'critical section'.
//BOOL firstWrite = TRUE;
BOOL useingIrq = FALSE;
UINT16 test_numn =0;
BOOL in_interruptHandle = FALSE;
//extern void * memset(void *,int,size_t);
//PUBLIC VOID *memcpy(VOID *dest, CONST VOID *src, UINT32 n);
void showString(UINT8 * str);
UINT32 tx_data_counter =0x00;
UINT32 rx_data_counter =0x00;
UINT32 transfer_start_time = 0x00;
BOOL wifi_sleep_flag = FALSE;
BOOL sdio_sleep_flag = FALSE;
BOOL wifi_sleep_enable = FALSE;
UINT8 wifi_sleep_enable_flag = 0x00; //0x00: wake up; 0x01: pre-association sleep; 0x10: post-association sleep
BOOL ap_connected =FALSE;
BOOL ap_connecting =FALSE;
BOOL ap_disconnecting =FALSE;
BOOL preasso_timer_enable = TRUE;

BOOL gettingRssi = FALSE;
extern UINT32 wifi_PowerOffCount ;
extern BOOL auth_failed ;
extern UINT32 report_rssi_count ;


extern UINT32 wifi_enableSleepTimerCount;

UINT8 tmp_read_data[16][1664] = {{0x00,},};

UINT16 tmp_read_data_flag =0x0000;


UINT8  associate_tries = 0;
PUBLIC BOOL hal_HstSendEvent(UINT32 ch);
//PUBLIC VOID hal_SdioSendCmd(HAL_SDIO_CMD_T cmd, UINT32 arg, BOOL suspend, UINT8 direct,BOOL multiblock);

VOID wifi_IrqHandler(VOID);

int test_rda5890_generic_set_uchar(unsigned short wid, unsigned char val);
int test_rda5890_generic_set_ulong( unsigned short wid,UINT32 val);
void test_rda5890_card_to_host( UINT8 *packet, unsigned short packet_len);

int sdio_getInputData(void);
VOID restart_wifiPollTimer(VOID);
VOID stop_wifiPollTimer(VOID);
VOID hal_sdioDataInIrqMaskClear(VOID);
VOID hal_sdioDataInIrqMaskSet(VOID);
UINT32 hal_sdioDataInIrqMaskGet(VOID);
VOID hal_sdioDataInIrqMaskSetEx(UINT32 mask_t);
PUBLIC BOOL hal_SdioTransferDone(VOID);
UINT32 hal_read_databyFIFO(void);
void hal_send_databyFIFO(void);
INT  test_rda5890_set_pm_mode(UINT8 val);
UINT32 returnsdmmcSta(void);
PUBLIC VOID hal_SdioReadFlushFifo(void);
PUBLIC BOOL hal_SdioTransferFiFoDone(VOID);
PUBLIC VOID hal_SdioTransferFiFoDoneClr(VOID);
PUBLIC VOID hal_SdioDataIndClr(VOID);
BOOL IsSdioBusy(void);
PUBLIC VOID hal_SdioClose(VOID);
PUBLIC VOID hal_SdioWakeUp(VOID);
PUBLIC VOID hal_SdioSleep(VOID);

PUBLIC VOID hal_SdioReset(VOID);
PUBLIC INT32 hal_SdioReadSdioConfig(VOID);
PUBLIC  HAL_ERR_T hal_SdioTransferinit(HAL_SDIO_TRANSFER_T* transfer);
UINT32 send_test_cmd(UINT32 cmd, UINT32 arg);
UINT32 returnTC(void);
UINT32 returnSdioSta(void);
VOID hal_Write_databyFIFO(UINT32 vData);
VOID ip_WifiConnectedInd(void);
//void Kill_pollTimer(void);
void wifi_setTransMode(void);

VOID Aps_WifiDataInd(UINT8 intflag);
PUBLIC BOOL hal_SdioDataInd(VOID);
VOID setWifiIrqMsk(UINT8 mask);
UINT32 wifi_SendAttRsp(void);
VOID ip_WifiRcvIpPck(UINT8* srcMac,UINT8* destMac,UINT8* pdata, UINT16 len);
VOID Aps_WifiRcvArpPck(UINT8* destMac,UINT8* srcMac,UINT8* pdata, UINT16 len);

int test_rda5890_data_rx(UINT8 *data, unsigned short len);
void setWpaAddr(UINT8* dest, UINT8* src, UINT8* ssid);
void initWifiWapSm(void);
VOID rda5890_WifWakeUp(VOID);
PUBLIC VOID hal_SysAuxClkOut(BOOL enable);
VOID write_wifi_patch_data(VOID);
VOID write_wifi_patch_dataEx(VOID);
VOID start_connect_ap_timer(UINT16 dur);
VOID stop_connect_ap_timer(VOID);
VOID start_assoc_ap_timer(UINT16 dur);
VOID stop_assoc_ap_timer(VOID);
VOID start_wifi_PreAssoWait_timer(UINT8 dur);

VOID stop_wifi_PreAssoWait_timer(VOID);
INT8 sdio_getWidResp(void);

VOID wifi_DisconnectAp(void);
//void setWifiSmField(UINT32 proto,INT32 rsn_enabled,UINT32 pairwise_cipher);
void setWifiSmField(UINT32 proto,INT32 rsn_enabled,UINT32 pairwise_cipher,UINT32 group_cipher);

void check_getRssi_result(void);


//#define KSEG1(addr)     ( (addr) | 0xa0000000 )

struct s_rda5890_wid
{
    //UINT8 pad[500];
    UINT8 wid_pending;
    UINT8 wid_rsp[512];
    UINT16 wid_rsp_len;
    UINT8 wid_msg_id;
    UINT32 wid;
    UINT32 (*callback) (UINT8* pdata, UINT16* len);
};
/*typedef struct{
    UINT8 cur_ssid[33];
    UINT8 cur_pwd[64];
    UINT8 WEP0[32];
    UINT8 auth_type;
    UINT8 dot11_mode;
} wifi_cxt_t;
*/


wifi_cxt_t wifi_context;


INT8 wifi_init_patch_dataEx(VOID);
INT8 wifi_init_patch_data(VOID)
{


    MCD_TRACE(MCD_INFO_TRC, 0, "wifi_init_patch_data....");

//  INT16 ret =0;
    UINT8 i =0;
    stop_wifiPollTimer();
    //    hal_sdioDataInIrqMaskClear();
    for(i=0; i<sizeof(wifi_core_data_patch)/sizeof(wifi_core_data_patch[0]); i++)
    {
        wifi_context.firstWrite = TRUE;
        test_rda5890_generic_set_uchar(0x1B,wifi_core_data_patch[i][0]);

        if(-1== sdio_getInputData())
        {

            return -1;
        }
        wifi_context.firstWrite = TRUE;
        test_rda5890_generic_set_uchar(0x1C,wifi_core_data_patch[i][1]);



        if(-1== sdio_getInputData())
        {

            return -1;
        }
        sxr_Sleep(10 *16384/1000);
    }
    sxr_Sleep(10 *16384/1000);
    restart_wifiPollTimer();
//hal_sdioDataInIrqMaskSet();
    wifi_context. firstWrite = TRUE;
    return 0;
}

INT8 wifi_Disablewakeup(VOID)
{
    MCD_TRACE(MCD_INFO_TRC, 0, "wifi_Disablewakeup....");
    //    hal_sdioDataInIrqMaskClear();
    test_rda5890_generic_set_ulong(0x201e,wifi_core_data_patch2[0][0]);
    if(-1== sdio_getInputData())
    {
        restart_wifiPollTimer();
//hal_sdioDataInIrqMaskSet();

        return -1;
    }

    test_rda5890_generic_set_ulong(0x201f,wifi_core_data_patch2[0][1]);
    if(-1== sdio_getInputData())
    {
        restart_wifiPollTimer();
//hal_sdioDataInIrqMaskSet();

        return -1;
    }

}

INT8 wifi_Enablewakeup(VOID)
{
    MCD_TRACE(MCD_INFO_TRC, 0, "wifi_Enablewakeup....");
    //    hal_sdioDataInIrqMaskClear();
    test_rda5890_generic_set_ulong(0x201e,wifi_core_data_patch2[1][0]);
    if(-1== sdio_getInputData())
    {
        restart_wifiPollTimer();
//hal_sdioDataInIrqMaskSet();

        return -1;
    }

    test_rda5890_generic_set_ulong(0x201f,wifi_core_data_patch2[1][1]);
    if(-1== sdio_getInputData())
    {
        restart_wifiPollTimer();
//hal_sdioDataInIrqMaskSet();

        return -1;
    }

}

INT8 wifi_init_patch_data2(VOID)
{
    MCD_TRACE(MCD_INFO_TRC, 0, "wifi_init_patch_data2....");
//  INT16 ret =0;
    UINT8 i =0;
    stop_wifiPollTimer();
    //    hal_sdioDataInIrqMaskClear();
    for(i=0; i<sizeof(wifi_core_data_patch2)/sizeof(wifi_core_data_patch2[0]); i++)
    {
        wifi_context.firstWrite = TRUE;
        test_rda5890_generic_set_ulong(0x201e,wifi_core_data_patch2[i][0]);
        if(-1== sdio_getInputData())
        {
            restart_wifiPollTimer();
//hal_sdioDataInIrqMaskSet();

            return -1;
        }
        wifi_context.firstWrite = TRUE;
        test_rda5890_generic_set_ulong(0x201f,wifi_core_data_patch2[i][1]);
        if(-1== sdio_getInputData())
        {
            restart_wifiPollTimer();
//hal_sdioDataInIrqMaskSet();

            return -1;
        }
        sxr_Sleep(10 *16384/1000);
    }
    sxr_Sleep(10 *16384/1000);
    restart_wifiPollTimer();
    wifi_context.firstWrite = TRUE;
//hal_sdioDataInIrqMaskSet();
    return 0;
}
UINT8 sem_cnt =11;
void sdio_TakeSemaphore(void)
{
    sxr_TakeSemaphore(g_SdioSemaphore);
    sem_cnt--;
    //   MCD_TRACE(MCD_INFO_TRC, 0, "sdio_TakeSemaphore,  sem_cnt: %d",sem_cnt);
}


void sdio_ReleaseSemaphore(void)
{
    sem_cnt ++;
    sxr_ReleaseSemaphore(g_SdioSemaphore);

    // MCD_TRACE(MCD_INFO_TRC, 0, "sdio_ReleaseSemaphore,  sem_cnt: %d",sem_cnt);
}
#if 1
#define MCD_CS_ENTER \
    if (g_SdioSemaphore != 0xFF)             \
    {                                       \
        sdio_TakeSemaphore();  \
    }                                       \
    else                                    \
    {                                       \
        return MCD_ERR;                     \
    }


/// Macro to easily implement concurrency in the MCD driver.
/// Exit in the 'critical section'.
#define MCD_CS_EXIT \
    {                                       \
    sdio_ReleaseSemaphore();   \
    }

#else
UINT8 sem_cnt =0;
#define MCD_CS_ENTER \
    if (g_SdioSemaphore != 0xFF)             \
    {                                       \
        sxr_TakeSemaphore(g_SdioSemaphore);  \
            sem_cnt++;\
                    MCD_TRACE(MCD_INFO_TRC, 0, "sem_cnt: %d",sem_cnt);\
    }                                       \
    else                                    \
    {                                       \
        return MCD_ERR;                     \
    }


/// Macro to easily implement concurrency in the MCD driver.
/// Exit in the 'critical section'.
#define MCD_CS_EXIT \
    {                                       \
    sxr_ReleaseSemaphore(g_SdioSemaphore);   \
            sem_cnt --;\
                MCD_TRACE(MCD_INFO_TRC, 0, "sem_cnt: %d",sem_cnt);\
    }
#endif

// Wait for a command to be done or timeouts
PRIVATE MCD_ERR_T mcd_SdioWaitCmdOver(VOID)
{
    UINT32 startTime = hal_TimGetUpTime();
    UINT32 time_out;

    //time_out =  (MCD_CARD_V1 == g_mcdVer) ? MCD_CMD_TIMEOUT_V1:MCD_CMD_TIMEOUT_V2;
    time_out = MCD_CMD_TIMEOUT_V1 ;

    while(hal_TimGetUpTime() - startTime < time_out && !hal_SdioCmdDone());

    if (!hal_SdioCmdDone())
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "SDC Waiting is Time out");
        return MCD_ERR_CARD_TIMEOUT;
    }
    else
    {
        return MCD_ERR_NO;
    }
}

/// Update g_mcdStatus
/// @return TRUE is card present (only exact when GPIO is used for card detect.)
/*
PRIVATE BOOL mcd_CardDetectUpdateStatus(VOID)
{
    if(NULL == g_mcdConfig)
    {
        g_mcdConfig = tgt_GetMcdConfig();
    }

    if(g_mcdConfig->cardDetectGpio != HAL_GPIO_NONE)
    {
        BOOL gpioState = !!hal_GpioGet(g_mcdConfig->cardDetectGpio);
        // CARD ?
        if(gpioState == g_mcdConfig->gpioCardDetectHigh)
        { // card detected
            switch(g_mcdStatus)
            {
                case MCD_STATUS_OPEN_NOTPRESENT: // wait for the close !
                case MCD_STATUS_OPEN:
                // No change
                break;
                default:
                g_mcdStatus = MCD_STATUS_NOTOPEN_PRESENT;
            }
            return TRUE;
        }
        else
        { // no card
            switch(g_mcdStatus)
            {
                case MCD_STATUS_OPEN_NOTPRESENT:
                case MCD_STATUS_OPEN:
                    g_mcdStatus = MCD_STATUS_OPEN_NOTPRESENT;
                    break;
                default:
                    g_mcdStatus = MCD_STATUS_NOTPRESENT;
            }
            return FALSE;
        }
    }
    // estimated
    switch(g_mcdStatus)
    {
        case MCD_STATUS_OPEN:
        case MCD_STATUS_NOTOPEN_PRESENT:
            return TRUE;
        default:
            return FALSE;
    }
}


*/
// =============================================================================
// mcd_SdioWaitResp
// -----------------------------------------------------------------------------
/// Wait for a response for a time configured by MCD_RESP_TIMEOUT
/// @return MCD_ERR_NO if a response with a good crc was received,
///         MCD_ERR_CARD_NO_RESPONSE if no response was received within the
/// driver configured timeout.
//          MCD_ERR_CARD_RESPONSE_BAD_CRC if the received response presented
//  a bad CRC.
// =============================================================================
PRIVATE MCD_ERR_T mcd_SdioWaitResp(VOID)
{
    HAL_SDIO_OP_STATUS_T status = hal_SdioGetOpStatus();
    UINT32 startTime = hal_TimGetUpTime();
    UINT32 rsp_time_out;

    // rsp_time_out =  (MCD_CARD_V1 == g_mcdVer) ? MCD_RESP_TIMEOUT_V1:MCD_RESP_TIMEOUT_V2;
    rsp_time_out = MCD_CMD_TIMEOUT_V1 ;
    while(hal_TimGetUpTime() - startTime < rsp_time_out &&status.fields.noResponseReceived)
    {
        //sxr_Sleep(163);
        status = hal_SdioGetOpStatus();
    }

    if (status.fields.noResponseReceived)
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "Response is Time out");
        return MCD_ERR_CARD_NO_RESPONSE;
    }

    if(status.fields.responseCrcError)
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "Response CRC is Error");
        return MCD_ERR_CARD_RESPONSE_BAD_CRC;
    }

    return MCD_ERR_NO;
}



//=============================================================================
// mcd_SdioReadCheckCrc
//-----------------------------------------------------------------------------
/// Check the read state of the sdmmc card.
/// @return MCD_ERR_NO if the Crc of read data was correct,
/// MCD_ERR_CARD_RESPONSE_BAD_CRC otherwise.
//=============================================================================
PRIVATE MCD_ERR_T mcd_SdioReadCheckCrc(VOID)
{
    HAL_SDIO_OP_STATUS_T operationStatus;
    operationStatus = hal_SdioGetOpStatus();
    if (operationStatus.fields.dataError != 0) // 0 means no CRC error during transmission
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "sdC status:%0x", operationStatus.reg);
        return MCD_ERR_CARD_RESPONSE_BAD_CRC;
    }
    else
    {
        return MCD_ERR_NO;
    }
}


//=============================================================================
// mcd_SdioWriteCheckCrc
//-----------------------------------------------------------------------------
/// Check the crc state of the write operation of the sdmmc card.
/// @return MCD_ERR_NO if the Crc of read data was correct,
/// MCD_ERR_CARD_RESPONSE_BAD_CRC otherwise.
//=============================================================================
PRIVATE MCD_ERR_T mcd_SdioWriteCheckCrc(VOID)
{
    HAL_SDIO_OP_STATUS_T operationStatus;
    operationStatus = hal_SdioGetOpStatus();

    if (operationStatus.fields.crcStatus != 2) // 0b010 = transmissionRight TODO a macro ?
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "sdC status:%0x", operationStatus.reg);
        return MCD_ERR_CARD_RESPONSE_BAD_CRC;
    }
    else
    {
        return MCD_ERR_NO;
    }
}




// =============================================================================
// mcd_SdioSendCmd
// -----------------------------------------------------------------------------
/// Send a command to the card, and fetch the response if one is expected.
/// @param cmd CMD to send
/// @param arg Argument of the ACMD.
/// @param resp Buffer to store card response.
/// @param suspend Not supported.
/// @return MCD_ERR_NO if a response with a good crc was received,
///         MCD_ERR_CARD_NO_RESPONSE if no reponse was received within the
/// driver configured timeout.
///          MCD_ERR_CARD_RESPONSE_BAD_CRC if the received response presented
///  a bad CRC.
///         MCD_ERR_CARD_TIMEOUT if the card timedout during procedure.
// =============================================================================
int wifi_ResetReq(VOID);

BOOL sdioCheckBusy(void)
{
    UINT32 startTime = hal_TimGetUpTime();
    UINT32 rsp_time_out;

    // rsp_time_out =  (MCD_CARD_V1 == g_mcdVer) ? MCD_RESP_TIMEOUT_V1:MCD_RESP_TIMEOUT_V2;
    rsp_time_out = 5*16384 ;
    while((hal_TimGetUpTime() - startTime < rsp_time_out) && IsSdioBusy())
    {
        //sxr_Sleep(163);
        ;
    }

    if(IsSdioBusy())
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "SDIO is busy, reset!");
        UINT32 ret =hal_SdioReadSdioConfig();
        UINT32 rrest = returnsdmmcSta();
        hal_HstSendEvent(0x77777777);
        hal_HstSendEvent(ret);
        hal_HstSendEvent(rrest);

        wifi_ResetReq();
        //hal_SdioReset();
        return TRUE;
    }
    return FALSE;
}

MCD_ERR_T sdio_SendCmd(HAL_SDIO_CMD_T cmd, UINT32 arg, UINT32* resp, BOOL suspend)
{
    MCD_ERR_T errStatus = MCD_ERR_NO;
    // MCD_CARD_STATUS_T cardStatus = {0};
    //  UINT32  cmd55Response[4] = {0, 0, 0, 0};


    // Send proper command. If it was an ACMD, the CMD55 have just been sent.
    if( sdioCheckBusy())
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "sdio is busy");

        return MCD_ERR_CMD;
    }
    //  hal_HstSendEvent(cmd);

    hal_SdioSendCmd(cmd, arg, suspend,0,0);

    // Wait for command to be sent
    errStatus = mcd_SdioWaitCmdOver();


    if (MCD_ERR_CARD_TIMEOUT == errStatus)
    {
        if (cmd & HAL_SDIO_ACMD_SEL)
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "ACMD %d Sending Timed out", (cmd & HAL_SDIO_CMD_MASK));
        }
        else
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "CMD %d Sending Timed out", (cmd & HAL_SDIO_CMD_MASK));
        }
        wifi_ResetReq();
        return MCD_ERR_CARD_TIMEOUT;
    }

    // Wait for response and get its argument
    if (hal_SdioNeedResponse(cmd))
    {
        //   MCD_TRACE(MCD_INFO_TRC, 0, "waiting response of CMD 0x%x......", cmd );

        errStatus = mcd_SdioWaitResp();
    }

    if (MCD_ERR_NO != errStatus)
    {
        if (cmd & HAL_SDIO_ACMD_SEL)
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "ACMD %d Response Error", (cmd & HAL_SDIO_CMD_MASK));
        }
        else
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "CMD %d Response Error", (cmd & HAL_SDIO_CMD_MASK));
            wifi_ResetReq();
            return errStatus;
        }

    }

    // Fetch response
    hal_SdioGetResp(cmd, resp, FALSE);

    //FOR DEBUG: MCD_TRACE(MCD_INFO_TRC, 0, "CMD %d Response is %#x", (cmd & HAL_SDIO_CMD_MASK), resp[0]);

    return MCD_ERR_NO;
}




// =============================================================================
// mcd_SdioInitCid
// -----------------------------------------------------------------------------
/// Set the CID in the driver from the data read on the card.
/// @param cid 4 word array read from the card, holding the CID register value.
// =============================================================================


#define MCD_CSD_VERSION_1       0
#define MCD_CSD_VERSION_2       1


/***********************************************************/
/*test sdio card, start. 2010-11-18                                   */
/***********************************************************/
void dump_data(U8 * data, U16 len)
{
    sxs_Dump(MCD_INFO_TRC, 0,(U8 *)data, len);

}

UINT8 SDIO_Reset(VOID)
{
    MCD_TRACE(MCD_INFO_TRC, 0, "Sdio_Reset.......");
    //reset sdio, write 1 to bit3 in register 0x06 in CCCR
    /********************************************************************************/
    /*0x  8                            8                              0      0       0      C                         08               */
    /*     1      000                 1               0         00 0000 0000 0000 110          0          0000 1000   */
    /*   R/W   FUN Number    RAW flag     stuff    (17 bit) Register address        Stuff     write data    */
    /********************************************************************************/

    send_test_cmd(52,0x88000C08);
    return 0;

}
VOID hal_sdioDataInIrqMaskClear(VOID);
PUBLIC MCD_ERR_T SDIO_mcd_Open(MCD_CSD_T* mcdCsd)
{
    MCD_ERR_T                  errStatus   = MCD_ERR_NO;
    UINT32                     response[4] = {0, 0, 0, 0};
    UINT32 ret =0;
//    MCD_CARD_STATUS_T          cardStatus  = {0};
    //  BOOL                       isMmc       = FALSE;
//   HAL_SDIO_DATA_BUS_WIDTH_T dataBusWidth;
//   UINT32 startTime = 0;

    //  MCD_PROFILE_FUNCTION_ENTER(mcd_Open);
    MCD_TRACE(MCD_INFO_TRC, 0, "SDIO_mcd_Open starts ...");

    // Check concurrency. Only one mcd_Open.
    UINT32 cs = hal_SysEnterCriticalSection();

    if (g_SdioSemaphore == 0xFF)
    {
        // Create semaphore and go on with the driver.

        // NOTE: This semaphore is never deleted for now, as
        // 1. sema deletion while other task is waiting for it will cause an error;
        // 2. sema deletion will make trouble if already-open state is considered OK.
        g_SdioSemaphore = sxr_NewSemaphore(1);
    }
    MCD_TRACE(MCD_INFO_TRC, 0, "g_SdioSemaphore: 0x%x",g_SdioSemaphore);

    hal_SysExitCriticalSection(cs);

    // Following operation should be kept protected
    MCD_CS_ENTER;
    /*
        if(NULL == g_mcdConfig)
        {
            g_mcdConfig = tgt_GetMcdConfig();
        }

        if(g_mcdConfig->cardDetectGpio != HAL_GPIO_NONE)
        {
            // Only if GPIO for detection exists, else we try to open anyway.
            if(!mcd_CardDetectUpdateStatus())
            {
                MCD_TRACE(MCD_INFO_TRC, 0, "mcd_Open: GPIO detection - no card");
                MCD_CS_EXIT;
                MCD_PROFILE_FUNCTION_EXIT(mcd_Open);
                return MCD_ERR_NO_CARD;
            }
        }
    */
    if (g_mcdStatus == MCD_STATUS_OPEN)
    {
        // Already opened, just return OK
        MCD_TRACE(MCD_INFO_TRC, 0, "mcd_Open: Already opened");
        //    *mcdCsd = g_mcdLatestCsd;

        MCD_CS_EXIT;
        MCD_PROFILE_FUNCTION_EXIT(mcd_Open);
        return MCD_ERR_NO;
    }

    hal_SdioOpen(0, wifi_IrqHandler);
    sdio_sleep_flag = FALSE;

    ///@todo: should use g_mcdConfig->dat3HasPullDown if true, we can handle
    /// basic card detection as follows:
    /// call hal_SdmmcGetCardDetectPinLevel() to check if card as changed (back 1)
    /// send command ACMD42 to disable internal pull up (so pin goes to 0)
    /// meaning if pin is 1, there was a removal, file system should be told to
    /// remount the card as it might be a different one (or the content might have
    /// been changed externally).

    // RCA = 0x0000 for card identification phase.
    g_SdioRcaReg = 0;

    g_SdioFreq = 200000;
    hal_SdioSetClk(g_SdioFreq);

    // assume it's not an SDHC card




    errStatus = sdio_SendCmd(5, 0x00000000, response, FALSE);

    MCD_TRACE(MCD_INFO_TRC, 0, "CMD 5, err status: %d",errStatus);
    sxs_Dump(MCD_INFO_TRC, 0,(U8 *)response, 16);
    //if(0!=errStatus)
    //  return;
    //  return;
    // hal_SdioClearInt();
    //    return  0;

    errStatus = sdio_SendCmd(5, 0x00300000, response, FALSE);
    //errStatus = sdio_SendCmd(5, 0x00000300, response, FALSE);

    MCD_TRACE(MCD_INFO_TRC, 0, "CMD 5, err status: %d",errStatus);
    sxs_Dump(MCD_INFO_TRC, 0,(U8 *)response, 16);
    //   return  0;
    //  hal_SdioClearInt();
    errStatus = sdio_SendCmd(3, 0, response, FALSE);
    MCD_TRACE(MCD_INFO_TRC, 0, "CMD 3, err status: %d, response: 0x%x",errStatus,response[0]);
    sxs_Dump(MCD_INFO_TRC, 0,(U8 *)response, 16);


    g_SdioRcaReg = response[0] & 0xffff0000;
    memset((void*)response,0x00,16);
    MCD_TRACE(MCD_INFO_TRC, 0, "g_mcdRcaReg: 0x%x",g_SdioRcaReg);

    errStatus = sdio_SendCmd(7, g_SdioRcaReg, response, FALSE);
    MCD_TRACE(MCD_INFO_TRC, 0, "CMD 7, err status: %d, response: 0x%x",errStatus,response[0]);

    // MCD_TRACE(MCD_INFO_TRC, 0, "CMD 7, err status: %d",errStatus);
    sxs_Dump(MCD_INFO_TRC, 0,(U8 *)response, 16);

    //return 0;

    //step1, enable function 1, write 1 to bit1 in register 0x02 in CCCR
    /********************************************************************************/
    /*0x  8                            8                              0      0       0      4                         02               */
    /*     1      000                 1               0         00 0000 0000 0000 010          0          0000 0010   */
    /*   R/W   FUN Number    RAW flag     stuff    (17 bit) Register address        Stuff     write data    */
    /********************************************************************************/

    send_test_cmd(52,0x88000402);

    //          wifi_Write_MAC_Reg_I2C(0x5030002c,0xf808);
    //      sxr_Sleep(200);
    //step2, enable function 1 interrupt, write 1 to  bit1 in register 0x04 in CCCR
    send_test_cmd(52,0x88000803);


    //set Function 1 INT MASK
    UINT32 tmp_addr = 0x88000000;
    tmp_addr = tmp_addr |(1<<28) |( 0x04 <<9)|0x07;
    ret = send_test_cmd(52,tmp_addr);
    if(ret==0x1200)
    {
        MCD_CS_EXIT;
        return  MCD_ERR_NO_CARD;
    }
    send_test_cmd(52,0x00000600);

    //  return 0;
    //sxr_Sleep(200);

    //sxr_Sleep(200);
    //hal_sdioDataInIrqMaskSet();

    //step3, get function I/O port address
    //  get_functionIoAddr(1);
    //   MCD_TRACE(MCD_INFO_TRC, 0, "Funnction 1 I/O port address: 0x%x",ioport);
    //step3, get function I/O port address

    //send_test_cmd(52,0x88000C08);
    //sxr_Sleep(200);

    //sxr_Sleep(80);

    //step4, set data bud withs, 4-bits ,write 0x02 to  bit1 in register 0x07 in CCCR
    send_test_cmd(52,0x88000E02);
    sxr_Sleep(200);
    // Configure the controller to use that many lines.
    hal_SdioSetDataWidth(HAL_SDIO_DATA_BUS_WIDTH_4);

    //enable multiblock
    //  send_test_cmd(52,0x08001000);
    //sxr_Sleep(200);
    //5, set function block size
    UINT16 blocksize = SDIO_BLOCK_SIZE;
    UINT8 bytetmp = blocksize&0xff;
    send_test_cmd(52,0x88000000|(0x110<<9)|bytetmp);

    sxr_Sleep(200);
    bytetmp =( blocksize>>8)&0xff;
    send_test_cmd(52,0x88000000|(0x111<<9)|bytetmp);
    //SDIO_Reset();

    //      return 0;
    hal_SdioEnterDataTransferMode();

    // Set the clock of the SD bus for the fastest possible rate.
    // g_mcdSdmmcFreq = 20000000;
    // g_SdioFreq = 20000000;
    g_SdioFreq = 16000000;   //modify 2011-10-20

    hal_SdioSetClk(g_SdioFreq);


    //  g_mcdLatestCsd = *mcdCsd;
    g_mcdStatus = MCD_STATUS_OPEN;
    // hal_SdioSleep();

    MCD_CS_EXIT;
    //sxr_Sleep(200);
    MCD_PROFILE_FUNCTION_EXIT(mcd_Open);
    return MCD_ERR_NO;

}
VOID setWifiIrqMsk(UINT8 mask)
{
    UINT32 tmp_addr = 0x88000000;
    tmp_addr = tmp_addr |(1<<28) |( 0x04 <<9)|mask;
    send_test_cmd(52,tmp_addr);
    //step2, enable function 1 interrupt, write 1 to  bit1 in register 0x04 in CCCR
    // send_test_cmd(52,0x88000802);
}
VOID stop_wifi_poll_timer(VOID);
VOID Wifi_Sdio_Close(VOID)
{
    //rda5890_WifWakeUp();
    useingIrq = FALSE;
    sxr_Sleep(1638);

    hal_SdioClose();
    /*
    if(g_SdioSemaphore!=0xFF)
    {
        sxr_FreeSemaphore(g_SdioSemaphore);
        g_SdioSemaphore =0xFF;
    }
    */
    sdio_sleep_flag = TRUE;

    stop_wifi_poll_timer();
    g_mcdStatus = MCD_STATUS_NOTOPEN_PRESENT;
    return;
}
/*
PUBLIC VOID hal_SdioReset(VOID)
{
    hal_HstSendEvent(0x77777777);

    hal_SdioClose();
    sxr_Sleep(20);
    hal_SdioOpen(0);
    g_SdioFreq = 2000000;   //modify 2011-10-20

    hal_SdioSetClk(g_SdioFreq);


}
*/

UINT32 send_test_cmd(UINT32 cmd, UINT32 arg)
{
    UINT32 response[4] = {0,0,0,0};
    MCD_ERR_T errStatus = MCD_ERR_NO;
    errStatus = sdio_SendCmd(cmd, arg, response, FALSE);
    //      if((52==cmd)&&(response[3]!=0x00001000))
    //  MCD_TRACE(MCD_INFO_TRC, 0, "CMD %d, arg: 0x%x, ######################",cmd, arg);

    MCD_TRACE(MCD_INFO_TRC, 0, "CMD %d, arg: 0x%x, err status: %d,  response: 0x%x, counter: 0x%x",cmd, arg,errStatus,response[3],test_numn);

    //          sxs_Dump(MCD_INFO_TRC, 0,(U8 *)response, 16);
    return response[3];
}

UINT32 read_test_cmd(UINT32 cmd, UINT32 arg)
{
    UINT32 response[4] = {0,0,0,0};
    MCD_ERR_T errStatus = MCD_ERR_NO;
    errStatus = sdio_SendCmd(cmd, arg, response, FALSE);
    //  if((52==cmd)&&(response[3]!=0x00001000))
    //      MCD_TRACE(MCD_INFO_TRC, 0, "CMD %d, arg: 0x%x, ######################",cmd, arg);
    //
    MCD_TRACE(MCD_INFO_TRC, 0, "CMD %d, arg: 0x%x, err status: %d,  response: 0x%x,  counter: 0x%x",cmd, arg,errStatus,response[3],test_numn);

    //          sxs_Dump(MCD_INFO_TRC, 0,(U8 *)response, 16);
    return response[3];

}
UINT32 read_test_cmd_notrace(UINT32 cmd, UINT32 arg)
{
    UINT32 response[4] = {0,0,0,0};
    MCD_ERR_T errStatus = MCD_ERR_NO;
    errStatus = sdio_SendCmd(cmd, arg, response, FALSE);

    return response[3];

}


void get_functionIoAddr(u8 fun_num)
{
    UINT32                     response[4] = {0, 0, 0, 0};
    MCD_ERR_T                  errStatus   = MCD_ERR_NO;
    UINT32 tmp_addr = 0x00000000;
    U16 port_reg = 0x0000;

    U8 function_num = fun_num;
    //tmp_addr = tmp_addr |(function_num<<28) |(function_num<<8 | ii |0x10000) <<9;
    //tmp_addr = tmp_addr |(function_num<<28) |(function_num<<8 | ii |0x8000) <<9;
    tmp_addr = tmp_addr |(function_num<<28) |( port_reg  <<9);
    errStatus = sdio_SendCmd(52, tmp_addr, response, FALSE);
    MCD_TRACE(MCD_INFO_TRC, 0, "CMD 52, err status: %d, response: 0x%x",errStatus,response[3]);
    ioport |=(response[3]&0xFF);
    tmp_addr = 0x00000000;
    tmp_addr = tmp_addr |(function_num<<28) |( (port_reg+1)  <<9);
    errStatus = sdio_SendCmd(52, tmp_addr, response, FALSE);
    MCD_TRACE(MCD_INFO_TRC, 0, "CMD 52, err status: %d, response: 0x%x",errStatus,response[3]);
    ioport |=(response[3]&0xFF)<<8;
    tmp_addr = 0x00000000;
    tmp_addr = tmp_addr |(function_num<<28) |(( port_reg+2 ) <<9);
    errStatus = sdio_SendCmd(52, tmp_addr, response, FALSE);
    MCD_TRACE(MCD_INFO_TRC, 0, "CMD 52, err status: %d, response: 0x%x",errStatus,response[3]);
    ioport |=(response[3]&0xFF)<<16;
    return;

}

U16 sidodata_size = 0x20;
//#define USE_DMA_TXDATA   1
MCD_ERR_T mcd_SdioSendCmd(HAL_SDIO_CMD_T cmd, UINT32 arg,
                          UINT32* resp, BOOL suspend,UINT8 direct,BOOL multiblock)
{
    MCD_ERR_T errStatus = MCD_ERR_NO;
//   MCD_CARD_STATUS_T cardStatus = {0};
//   UINT32  cmd55Response[4] = {0, 0, 0, 0};


    // Send proper command. If it was an ACMD, the CMD55 have just been sent.
    if( sdioCheckBusy())
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "sdio is busy");
        return MCD_ERR_CMD;
    }
    //hal_HstSendEvent(cmd);
    hal_SdioSendCmd(cmd, arg, suspend,direct,multiblock);

    // Wait for command to be sent
    errStatus = mcd_SdioWaitCmdOver();


    if (MCD_ERR_CARD_TIMEOUT == errStatus)
    {
        if (cmd & HAL_SDIO_ACMD_SEL)
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "ACMD %d Sending Timed out", (cmd & HAL_SDIO_CMD_MASK));
        }
        else
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "CMD %d Sending Timed out", (cmd & HAL_SDIO_CMD_MASK));
        }
        wifi_ResetReq();
        return MCD_ERR_CARD_TIMEOUT;
    }

    // Wait for response and get its argument
    if (hal_SdioNeedResponse(cmd))
    {
        errStatus = mcd_SdioWaitResp();
    }

    if (MCD_ERR_NO != errStatus)
    {
        if (cmd & HAL_SDIO_ACMD_SEL)
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "ACMD %d Response Error", (cmd & HAL_SDIO_CMD_MASK));
        }
        else
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "CMD %d Response Error", (cmd & HAL_SDIO_CMD_MASK));
            return errStatus;
        }
    }

    // Fetch response
    hal_SdioGetResp(cmd, resp, FALSE);

    //FOR DEBUG: MCD_TRACE(MCD_INFO_TRC, 0, "CMD %d Response is %#x", (cmd & HAL_SDIO_CMD_MASK), resp[0]);

    return MCD_ERR_NO;
}


//BOOL firstWrite = TRUE;
UINT32 Sc_Status;
UINT32 mask_sdio =0x00;

VOID usingIrqClr(void)
{
    if(useingIrq)
    {
        //mask_sdio =  hal_sdioDataInIrqMaskGet();
//      hal_sdioDataInIrqMaskClear();
        mask_sdio =  hal_sdioDataInIrqMaskGet();
        //MCD_TRACE(MCD_INFO_TRC, 0, "usingIrqClr,   0x%x", 0x99990000+test_numn);
        //MCD_TRACE(MCD_INFO_TRC, 0, "usingIrqClr");
        //UINT32 tmp_data = 0x99990000+test_numn;
        //hal_HstSendEvent(tmp_data);
#if 1
        hal_sdioDataInIrqMaskClear();
        if(in_interruptHandle)
            mask_sdio = 0x00;

#else

        Sc_Status = hal_SysEnterCriticalSection();
#endif
    }
}
VOID usingIrqSet(void)
{
    if(useingIrq)
    {
        //MCD_TRACE(MCD_INFO_TRC, 0, "usingIrqSet,   0x%x", 0x99991000+test_numn);
        //MCD_TRACE(MCD_INFO_TRC, 0, "usingIrqSet");
        //  hal_sdioDataInIrqMaskSetEx(mask_sdio);
        //UINT32 tmp_data = 0x99991000+test_numn;
        //  hal_HstSendEvent(tmp_data);
#if 1
        if(!in_interruptHandle)
        {
            hal_sdioDataInIrqMaskSetEx(mask_sdio);
        }

        //hal_sdioDataInIrqMaskSet();
#else
        hal_SysExitCriticalSection(Sc_Status);
#endif
        //sxr_ExitSc(Sc_Status);
    }
}
U8 SDIO_WifiDataSingleBlockWrite(UINT32 blockAddr, const U8* pWrite, UINT32 datalen)
{
    MCD_TRACE(MCD_INFO_TRC, 0, "SDIO_WifiDataSingleBlockWrite, datalen: %d",datalen);

    UINT32 cardResponse[4] = {0, 0, 0, 0};
    MCD_ERR_T errStatus = MCD_ERR_NO;
    UINT32 tmp_addr ;
    u16 setlen =0x00;

    sidodata_size = (datalen+3)&0xFFFC;
    UINT16 tmplen =32;
    while(sidodata_size > tmplen)
    {
        tmplen =tmplen<<1;
    }
    sidodata_size = tmplen;
    setlen = sidodata_size>>2;


#if 1
//add flow contrl, 2011-12-26
    if(!wifi_context.firstWrite)
    {
        UINT8 count =0;
        UINT32 ret = 0x00;
        tmp_addr = 0x00000000;
        tmp_addr = tmp_addr |(1<<28) |( 0x05 <<9);
        usingIrqClr();
        ret = read_test_cmd(52,tmp_addr);
        usingIrqSet();
        MCD_TRACE(MCD_INFO_TRC, 0, "new SDIO_WifiDataSingleBlockWrite: 0x%x,  0x%x",(ret & 0x40),count);

        while(((ret & 0x40)== 0x00)&&(count < 30))
        {
            sxr_Sleep(200);
            tmp_addr = 0x00000000;
            tmp_addr = tmp_addr |(1<<28) |( 0x05 <<9);
            usingIrqClr();
            ret = read_test_cmd(52,tmp_addr);
            usingIrqSet();
            count++;

        }
//  MCD_TRACE(MCD_INFO_TRC, 0, "new SDIO_WifiDataSingleBlockWrite: 0x%x,  0x%x",(ret & 0x40),count);

        if(count < 30)
        {
            tmp_addr = 0x88000000;
            tmp_addr = tmp_addr |(1<<28) |( 0x05 <<9)|0x40;
            usingIrqClr();

            send_test_cmd(52,tmp_addr);
            usingIrqSet();

        }
        else
        {


            MCD_TRACE(MCD_INFO_TRC, 0, "WIFI is busy, reset!");
            ret =hal_SdioReadSdioConfig();
            hal_HstSendEvent(0x99999999);
            hal_HstSendEvent(ret);
            wifi_ResetReq();
            return  MCD_ERR;
        }
        //firstWrite = FALSE;
    }
    wifi_context.firstWrite = FALSE;
#endif

    UINT32 blockNum =1;

    U8 byted ;

    U8 function_num = 1;
    tmp_addr = 0x80000000;
    byted = setlen&0xFF;
    usingIrqClr();

    tmp_addr = tmp_addr |(function_num<<28) |( 0x00  <<9)|byted;
    send_test_cmd(52,tmp_addr);

//sxr_Sleep(10);

    byted = (setlen>>8)&0x7F;
    byted = 0x80|byted; //0x80, tx ,need set the BIT15 of the length register, to set data valid.
    tmp_addr = 0x80000000;
    tmp_addr = tmp_addr |(function_num<<28) |( 0x01  <<9)|byted;
    send_test_cmd(52,tmp_addr);
//sxr_Sleep(10);

//blockAddr = 0x110;
    UINT32 startTime = 0;

    HAL_SDIO_CMD_T writeCmd;
    U8 blockMode =0x00;

    g_SdioTransfer.sysMemAddr = (UINT8*) pWrite;
    g_SdioTransfer.sdCardAddr = (UINT8 *)blockAddr;
    g_SdioTransfer.blockNum   = blockNum;
    g_SdioTransfer.blockSize  = sidodata_size;      //g_mcdBlockLen;
    g_SdioTransfer.direction  = HAL_SDIO_DIRECTION_WRITE;


// FIXME find what the heck is that !:
// program_right_num=0;

    // Check buffer.
    MCD_ASSERT(pWrite != NULL, "SDIO write: Buffer is NULL");
    MCD_ASSERT(((UINT32)pWrite & 0x3) ==0,
               "SDIO write: buffer is not aligned! addr=%08x", pWrite);
    MCD_ASSERT(blockNum>=1 && blockNum<= SDIO_MAX_BLOCK_NUM,
               "Block Num is overflow");
    // The command for single block or multiple blocks are differents
    if (blockNum == 1)
    {
        writeCmd = HAL_SDIO_CMD_WRITE_SINGLE_BLOCK;
        blockMode = 0x00;
    }
    else
    {
        writeCmd = HAL_SDIO_CMD_WRITE_MULT_BLOCK;
        blockMode =0x01;
    }

#ifndef __USE_SDMMC1__
    if((wifi_version_flag== WIFI_VERSION_E) ||(wifi_version_flag== WIFI_VERSION_F))
    {
        hal_SdioTransferinit(&g_SdioTransfer);

        U8 iii =0;
        //    UINT32 ret_val =datalen;
        UINT32* tmp_data_p = NULL;
        tmp_data_p = (UINT32 *)pWrite;
        for(iii=0; iii<(sidodata_size/4); iii++)
        {
            hal_Write_databyFIFO(*tmp_data_p);
            //MCD_TRACE(MCD_INFO_TRC, 0, "wrire FIFO: 0x%x",*tmp_data_p);
            tmp_data_p++;

        }
        MCD_TRACE(MCD_INFO_TRC, 0, "wrire to  FIFO: ");

        sxs_Dump(_MMI, 0, (u8*)pWrite, sidodata_size);


        UINT32 tmp_reg =0x90000000;
        MCD_TRACE(MCD_INFO_TRC, 0, "SDIO_WifiDataSingleBlockWrite, sidodata_size: %d",sidodata_size);

        if(sidodata_size ==512)
            tmp_reg|=(0x00|((UINT32)blockAddr)<<9);
        else
            tmp_reg|=(sidodata_size|((UINT32)blockAddr)<<9);
        errStatus = mcd_SdioSendCmd(HAL_SDIO_CMD_IO_RW_EXTENDED, (UINT32)tmp_reg, cardResponse, FALSE,1,0);
        MCD_TRACE(MCD_INFO_TRC, 0, "CMD 53, arg: 0x%x, err status: %d,  response: 0x%x",tmp_reg,errStatus,cardResponse[3]);
        sxs_Dump(MCD_INFO_TRC, 0,(U8 *)cardResponse, 16);
//sxr_Sleep(10);

        if (errStatus != MCD_ERR_NO)
        {
//  Kill_pollTimer();
            usingIrqSet();

            MCD_TRACE(MCD_INFO_TRC, 0, "Set sd write had error");
            return MCD_ERR_CMD;
        }
        // Wait
        startTime = hal_TimGetUpTime();

        while(! hal_SdioTransferFiFoDone())
        {
            //sxr_Sleep(200);
            if (hal_TimGetUpTime() - startTime >  (MCD_READ_TIMEOUT*blockNum*5))
            {
                MCD_TRACE(MCD_INFO_TRC, 0, "write on Sdmmc timeout");
//
//          Kill_pollTimer();
                usingIrqSet();
                wifi_ResetReq();
                return MCD_ERR_CARD_TIMEOUT;

            }
            // sxr_Sleep(10*16384/1000);
        }

        hal_SdioTransferFiFoDoneClr();

        if (mcd_SdioWriteCheckCrc() != MCD_ERR_NO)
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "SDIO Write error");
            usingIrqSet();

            return MCD_ERR;
        }

        //sxr_Sleep(10*16384/1000);
        usingIrqSet();

        return MCD_ERR_NO;
    }
#endif

//#ifdef USE_SDIO_DMA
    if(g_SdioIsDmaEnable)
    {
//#if 0
        // Initiate data migration through Ifc.
        MCD_TRACE(MCD_INFO_TRC, 0, "using DMA ");

        if (hal_SdioTransfer(&g_SdioTransfer) != HAL_ERR_NO)
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "write sd no ifc channel");
            return MCD_ERR_DMA_BUSY;
        }

        UINT32 tmp_reg =0x90000000;
        MCD_TRACE(MCD_INFO_TRC, 0, "SDIO_WifiDataSingleBlockWrite, sidodata_size: %d",sidodata_size);

        if(sidodata_size ==512)
            tmp_reg|=(0x00|((UINT32)blockAddr)<<9);
        else
            tmp_reg|=(sidodata_size|((UINT32)blockAddr)<<9);
        errStatus = mcd_SdioSendCmd(HAL_SDIO_CMD_IO_RW_EXTENDED, (UINT32)tmp_reg, cardResponse, FALSE,1,0);
        MCD_TRACE(MCD_INFO_TRC, 0, "CMD 53, arg: 0x%x, err status: %d,  response: 0x%x",tmp_reg,errStatus,cardResponse[3]);
        sxs_Dump(MCD_INFO_TRC, 0,(U8 *)cardResponse, 16);

        if (errStatus != MCD_ERR_NO)
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "Set sd write had error");
            hal_SdioStopTransfer(&g_SdioTransfer);
            return MCD_ERR_CMD;
        }

        // Wait
        startTime = hal_TimGetUpTime();

        while(!hal_SdioTransferDone())
        {
            //     sxr_Sleep(200);
            if (hal_TimGetUpTime() - startTime >  (MCD_WRITE_TIMEOUT*blockNum*2))
            {
                MCD_TRACE(MCD_INFO_TRC, 0, "Write on Sdmmc timeout");
                // Abort the transfert.
                hal_SdioStopTransfer(&g_SdioTransfer);
                return MCD_ERR_CARD_TIMEOUT;
            }

        }

        if (mcd_SdioWriteCheckCrc() != MCD_ERR_NO)
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "SDIO Write error");
            return MCD_ERR;
        }
    }
    else
    {
//#else
        hal_SdioTransferinit(&g_SdioTransfer);

        U8 iii =0;
        //    UINT32 ret_val =datalen;
        UINT32* tmp_data_p = NULL;
        tmp_data_p = (UINT32 *)pWrite;
        for(iii=0; iii<(sidodata_size/4); iii++)
        {
            hal_Write_databyFIFO(*tmp_data_p);
            //MCD_TRACE(MCD_INFO_TRC, 0, "wrire FIFO: 0x%x",*tmp_data_p);
            tmp_data_p++;

        }
        MCD_TRACE(MCD_INFO_TRC, 0, "wrire to  FIFO: ");

        sxs_Dump(_MMI, 0, (u8*)pWrite, sidodata_size);


        UINT32 tmp_reg =0x90000000;
        MCD_TRACE(MCD_INFO_TRC, 0, "SDIO_WifiDataSingleBlockWrite, sidodata_size: %d",sidodata_size);

        if(sidodata_size ==512)
            tmp_reg|=(0x00|((UINT32)blockAddr)<<9);
        else
            tmp_reg|=(sidodata_size|((UINT32)blockAddr)<<9);
        errStatus = mcd_SdioSendCmd(HAL_SDIO_CMD_IO_RW_EXTENDED, (UINT32)tmp_reg, cardResponse, FALSE,1,0);
        MCD_TRACE(MCD_INFO_TRC, 0, "CMD 53, arg: 0x%x, err status: %d,  response: 0x%x",tmp_reg,errStatus,cardResponse[3]);
        sxs_Dump(MCD_INFO_TRC, 0,(U8 *)cardResponse, 16);
//sxr_Sleep(10);

        if (errStatus != MCD_ERR_NO)
        {
//  Kill_pollTimer();
            usingIrqSet();

            MCD_TRACE(MCD_INFO_TRC, 0, "Set sd write had error");
            return MCD_ERR_CMD;
        }
        // Wait
        startTime = hal_TimGetUpTime();

        while(! hal_SdioTransferFiFoDone())
        {
            //sxr_Sleep(200);
            if (hal_TimGetUpTime() - startTime >  (MCD_READ_TIMEOUT*blockNum*5))
            {
                MCD_TRACE(MCD_INFO_TRC, 0, "write on Sdmmc timeout");
//
//          Kill_pollTimer();
                usingIrqSet();
                wifi_ResetReq();
                return MCD_ERR_CARD_TIMEOUT;

            }
            // sxr_Sleep(10*16384/1000);
        }

        hal_SdioTransferFiFoDoneClr();

        if (mcd_SdioWriteCheckCrc() != MCD_ERR_NO)
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "SDIO Write error");
            usingIrqSet();

            return MCD_ERR;
        }
    }
//  #endif
    //sxr_Sleep(10*16384/1000);
    usingIrqSet();

    return MCD_ERR_NO;

}


U8 SDIO_WifiDataSingleBlockWriteMutiEx(UINT32 blockAddr, const U8* pWrite, UINT32 datalen)
{
    MCD_TRACE(MCD_INFO_TRC, 0, "SDIO_WifiDataSingleBlockWriteMutiEx, datalen: %d",datalen);

    UINT32 cardResponse[4] = {0, 0, 0, 0};
    MCD_ERR_T errStatus = MCD_ERR_NO;
    UINT32 tmp_addr ;
    u16 setlen =0x00;

    sidodata_size = (datalen+3)&0xFFFC;
    UINT16 tmplen =32;
    while(sidodata_size > tmplen)
    {
        tmplen =tmplen<<1;
    }
    sidodata_size = tmplen;
    setlen = sidodata_size>>2;


#if 0
//add flow contrl, 2011-12-26
    if(!wifi_context.firstWrite)
    {
        UINT8 count =0;
        UINT32 ret = 0x00;
        tmp_addr = 0x00000000;
        tmp_addr = tmp_addr |(1<<28) |( 0x05 <<9);
        usingIrqClr();
        ret = read_test_cmd(52,tmp_addr);
        usingIrqSet();
        MCD_TRACE(MCD_INFO_TRC, 0, "new SDIO_WifiDataSingleBlockWrite: 0x%x,  0x%x",(ret & 0x40),count);

        while(((ret & 0x40)== 0x00)&&(count < 30))
        {
            sxr_Sleep(200);
            tmp_addr = 0x00000000;
            tmp_addr = tmp_addr |(1<<28) |( 0x05 <<9);
            usingIrqClr();
            ret = read_test_cmd(52,tmp_addr);
            usingIrqSet();
            count++;

        }
//  MCD_TRACE(MCD_INFO_TRC, 0, "new SDIO_WifiDataSingleBlockWrite: 0x%x,  0x%x",(ret & 0x40),count);

        if(count < 30)
        {
            tmp_addr = 0x88000000;
            tmp_addr = tmp_addr |(1<<28) |( 0x05 <<9)|0x40;
            usingIrqClr();

            send_test_cmd(52,tmp_addr);
            usingIrqSet();

        }
        else
        {


            MCD_TRACE(MCD_INFO_TRC, 0, "WIFI is busy, reset!");
            ret =hal_SdioReadSdioConfig();
            hal_HstSendEvent(0x99999999);
            hal_HstSendEvent(ret);
            wifi_ResetReq();
            return  MCD_ERR;
        }
        //firstWrite = FALSE;
    }
    wifi_context.firstWrite = FALSE;
#endif

    UINT32 blockNum =1;

    U8 byted ;

    U8 function_num = 1;
    tmp_addr = 0x80000000;
    byted = setlen&0xFF;
    usingIrqClr();

    tmp_addr = tmp_addr |(function_num<<28) |( 0x00  <<9)|byted;
//send_test_cmd(52,tmp_addr);

//sxr_Sleep(10);

    byted = (setlen>>8)&0x7F;
    byted = 0x80|byted; //0x80, tx ,need set the BIT15 of the length register, to set data valid.
    tmp_addr = 0x80000000;
    tmp_addr = tmp_addr |(function_num<<28) |( 0x01  <<9)|byted;
//send_test_cmd(52,tmp_addr);
//sxr_Sleep(10);

//blockAddr = 0x110;
    UINT32 startTime = 0;

    HAL_SDIO_CMD_T writeCmd;
    U8 blockMode =0x00;

    g_SdioTransfer.sysMemAddr = (UINT8*) pWrite;
    g_SdioTransfer.sdCardAddr = (UINT8 *)blockAddr;
    g_SdioTransfer.blockNum   = blockNum;
    g_SdioTransfer.blockSize  = sidodata_size;      //g_mcdBlockLen;
    g_SdioTransfer.direction  = HAL_SDIO_DIRECTION_WRITE;


// FIXME find what the heck is that !:
// program_right_num=0;

    // Check buffer.
    MCD_ASSERT(pWrite != NULL, "SDIO write: Buffer is NULL");
    MCD_ASSERT(((UINT32)pWrite & 0x3) ==0,
               "SDIO write: buffer is not aligned! addr=%08x", pWrite);
    MCD_ASSERT(blockNum>=1 && blockNum<= SDIO_MAX_BLOCK_NUM,
               "Block Num is overflow");
    // The command for single block or multiple blocks are differents
    if (blockNum == 1)
    {
        writeCmd = HAL_SDIO_CMD_WRITE_SINGLE_BLOCK;
        blockMode = 0x00;
    }
    else
    {
        writeCmd = HAL_SDIO_CMD_WRITE_MULT_BLOCK;
        blockMode =0x01;
    }

#ifndef __USE_SDMMC1__
    if((wifi_version_flag== WIFI_VERSION_E) ||(wifi_version_flag== WIFI_VERSION_F))
    {
        hal_SdioTransferinit(&g_SdioTransfer);

        U8 iii =0;
        //    UINT32 ret_val =datalen;
        UINT32* tmp_data_p = NULL;
        tmp_data_p = (UINT32 *)pWrite;
        for(iii=0; iii<(sidodata_size/4); iii++)
        {
            hal_Write_databyFIFO(*tmp_data_p);
            //MCD_TRACE(MCD_INFO_TRC, 0, "wrire FIFO: 0x%x",*tmp_data_p);
            tmp_data_p++;

        }
        MCD_TRACE(MCD_INFO_TRC, 0, "wrire to  FIFO: ");

        sxs_Dump(_MMI, 0, (u8*)pWrite, sidodata_size);


        UINT32 tmp_reg =0x90000000;
        MCD_TRACE(MCD_INFO_TRC, 0, "SDIO_WifiDataSingleBlockWrite, sidodata_size: %d",sidodata_size);

        if(sidodata_size ==512)
            tmp_reg|=(0x00|((UINT32)blockAddr)<<9);
        else
            tmp_reg|=(sidodata_size|((UINT32)blockAddr)<<9);
        errStatus = mcd_SdioSendCmd(HAL_SDIO_CMD_IO_RW_EXTENDED, (UINT32)tmp_reg, cardResponse, FALSE,1,0);
        MCD_TRACE(MCD_INFO_TRC, 0, "CMD 53, arg: 0x%x, err status: %d,  response: 0x%x",tmp_reg,errStatus,cardResponse[3]);
        sxs_Dump(MCD_INFO_TRC, 0,(U8 *)cardResponse, 16);
//sxr_Sleep(10);

        if (errStatus != MCD_ERR_NO)
        {
//  Kill_pollTimer();
            usingIrqSet();

            MCD_TRACE(MCD_INFO_TRC, 0, "Set sd write had error");
            return MCD_ERR_CMD;
        }
        // Wait
        startTime = hal_TimGetUpTime();

        while(! hal_SdioTransferFiFoDone())
        {
            //sxr_Sleep(200);
            if (hal_TimGetUpTime() - startTime >  (MCD_READ_TIMEOUT*blockNum*5))
            {
                MCD_TRACE(MCD_INFO_TRC, 0, "write on Sdmmc timeout");
//
//          Kill_pollTimer();
                usingIrqSet();
                wifi_ResetReq();
                return MCD_ERR_CARD_TIMEOUT;

            }
            // sxr_Sleep(10*16384/1000);
        }

        hal_SdioTransferFiFoDoneClr();

        if (mcd_SdioWriteCheckCrc() != MCD_ERR_NO)
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "SDIO Write error");
            usingIrqSet();

            return MCD_ERR;
        }

        //sxr_Sleep(10*16384/1000);
        usingIrqSet();

        return MCD_ERR_NO;
    }
#endif

//#ifdef USE_SDIO_DMA
    if(g_SdioIsDmaEnable)
    {
//#if 0
        // Initiate data migration through Ifc.
        MCD_TRACE(MCD_INFO_TRC, 0, "using DMA ");

        if (hal_SdioTransfer(&g_SdioTransfer) != HAL_ERR_NO)
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "write sd no ifc channel");
            return MCD_ERR_DMA_BUSY;
        }

        UINT32 tmp_reg =0x90000000;
        MCD_TRACE(MCD_INFO_TRC, 0, "SDIO_WifiDataSingleBlockWrite, sidodata_size: %d",sidodata_size);

        if(sidodata_size ==512)
            tmp_reg|=(0x00|((UINT32)blockAddr)<<9);
        else
            tmp_reg|=(sidodata_size|((UINT32)blockAddr)<<9);
        errStatus = mcd_SdioSendCmd(HAL_SDIO_CMD_IO_RW_EXTENDED, (UINT32)tmp_reg, cardResponse, FALSE,1,0);
        MCD_TRACE(MCD_INFO_TRC, 0, "CMD 53, arg: 0x%x, err status: %d,  response: 0x%x",tmp_reg,errStatus,cardResponse[3]);
        sxs_Dump(MCD_INFO_TRC, 0,(U8 *)cardResponse, 16);

        if (errStatus != MCD_ERR_NO)
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "Set sd write had error");
            hal_SdioStopTransfer(&g_SdioTransfer);
            return MCD_ERR_CMD;
        }

        // Wait
        startTime = hal_TimGetUpTime();

        while(!hal_SdioTransferDone())
        {
            //     sxr_Sleep(200);
            if (hal_TimGetUpTime() - startTime >  (MCD_WRITE_TIMEOUT*blockNum*2))
            {
                MCD_TRACE(MCD_INFO_TRC, 0, "Write on Sdmmc timeout");
                // Abort the transfert.
                hal_SdioStopTransfer(&g_SdioTransfer);
                return MCD_ERR_CARD_TIMEOUT;
            }

        }

        if (mcd_SdioWriteCheckCrc() != MCD_ERR_NO)
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "SDIO Write error");
            return MCD_ERR;
        }
    }
    else
    {
//#else
        hal_SdioTransferinit(&g_SdioTransfer);

        U8 iii =0;
        //    UINT32 ret_val =datalen;
        UINT32* tmp_data_p = NULL;
        tmp_data_p = (UINT32 *)pWrite;
        for(iii=0; iii<(sidodata_size/4); iii++)
        {
            hal_Write_databyFIFO(*tmp_data_p);
            //MCD_TRACE(MCD_INFO_TRC, 0, "wrire FIFO: 0x%x",*tmp_data_p);
            tmp_data_p++;

        }
        MCD_TRACE(MCD_INFO_TRC, 0, "wrire to  FIFO: ");

        sxs_Dump(_MMI, 0, (u8*)pWrite, sidodata_size);


        UINT32 tmp_reg =0x90000000;
        MCD_TRACE(MCD_INFO_TRC, 0, "SDIO_WifiDataSingleBlockWrite, sidodata_size: %d",sidodata_size);

        if(sidodata_size ==512)
            tmp_reg|=(0x00|((UINT32)blockAddr)<<9);
        else
            tmp_reg|=(sidodata_size|((UINT32)blockAddr)<<9);
        errStatus = mcd_SdioSendCmd(HAL_SDIO_CMD_IO_RW_EXTENDED, (UINT32)tmp_reg, cardResponse, FALSE,1,0);
        MCD_TRACE(MCD_INFO_TRC, 0, "CMD 53, arg: 0x%x, err status: %d,  response: 0x%x",tmp_reg,errStatus,cardResponse[3]);
        sxs_Dump(MCD_INFO_TRC, 0,(U8 *)cardResponse, 16);
//sxr_Sleep(10);

        if (errStatus != MCD_ERR_NO)
        {
//  Kill_pollTimer();
            usingIrqSet();

            MCD_TRACE(MCD_INFO_TRC, 0, "Set sd write had error");
            return MCD_ERR_CMD;
        }
        // Wait
        startTime = hal_TimGetUpTime();

        while(! hal_SdioTransferFiFoDone())
        {
            //sxr_Sleep(200);
            if (hal_TimGetUpTime() - startTime >  (MCD_READ_TIMEOUT*blockNum*5))
            {
                MCD_TRACE(MCD_INFO_TRC, 0, "write on Sdmmc timeout");
//
//          Kill_pollTimer();
                usingIrqSet();
                wifi_ResetReq();
                return MCD_ERR_CARD_TIMEOUT;

            }
            // sxr_Sleep(10*16384/1000);
        }

        hal_SdioTransferFiFoDoneClr();

        if (mcd_SdioWriteCheckCrc() != MCD_ERR_NO)
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "SDIO Write error");
            usingIrqSet();

            return MCD_ERR;
        }
    }
//  #endif
    //sxr_Sleep(10*16384/1000);
    usingIrqSet();

    return MCD_ERR_NO;

}




U8 SDIO_WifiDataMultBlockWrite(UINT32 blockAddr, const U8* pWrite, UINT32 datalen)
{

#if 0
    MCD_TRACE(MCD_INFO_TRC, 0, "SDIO_WifiDataMultBlockWrite, datalen: %d",datalen);

    UINT32 cardResponse[4] = {0, 0, 0, 0};
    MCD_ERR_T errStatus = MCD_ERR_NO;

    UINT32 setlen =0x00;

    UINT32 blockNum =1;

    blockNum = (datalen+SDIO_BLOCK_SIZE-1)/SDIO_BLOCK_SIZE;

    if(blockNum>1)
    {
//  SDIO_WifiDataSingleBlockWrite(blockAddr,  pWrite, SDIO_BLOCK_SIZE);
//  sxr_Sleep(200);
//  SDIO_WifiDataSingleBlockWrite(blockAddr,  pWrite+SDIO_BLOCK_SIZE, SDIO_BLOCK_SIZE);
//  if(blockNum>2)
//          SDIO_WifiDataSingleBlockWrite(blockAddr,  pWrite+SDIO_BLOCK_SIZE*2, SDIO_BLOCK_SIZE);

//  return 0;
    }
    setlen = (SDIO_BLOCK_SIZE * blockNum)>>2;
    /*
    sidodata_size = (datalen+3)&0xFFFC;
    UINT16 tmplen =8;
    while(sidodata_size > tmplen)
    {
        tmplen =tmplen<<1;
    }
    sidodata_size = tmplen;
    setlen = sidodata_size>>2;
    */

    U8 byted ;

    U8 function_num = 1;

    UINT32 tmp_addr = 0x80000000;

    byted = setlen&0xFF;

    tmp_addr = tmp_addr |(function_num<<28) |( 0x00  <<9)|byted;
    send_test_cmd(52,tmp_addr);

//sxr_Sleep(10);

    byted = (setlen>>8)&0x7F;
    byted = 0x80|byted;
    tmp_addr = 0x80000000;
    tmp_addr = tmp_addr |(function_num<<28) |( 0x01  <<9)|byted;
    send_test_cmd(52,tmp_addr);

//blockAddr = 0x110;
    UINT32 startTime = 0;

    HAL_SDIO_CMD_T writeCmd;
    U8 blockMode =0x00;


    g_SdioTransfer.sysMemAddr = (UINT8*) pWrite;
    g_SdioTransfer.sdCardAddr =(UINT8 *) blockAddr;
    g_SdioTransfer.blockNum   = blockNum;
    g_SdioTransfer.blockSize  = SDIO_BLOCK_SIZE;
    g_SdioTransfer.direction  = HAL_SDIO_DIRECTION_WRITE;

    MCD_TRACE(MCD_INFO_TRC, 0, "SDIO_WifiDataMultBlockWrite, blockSIze: %d,  blockNum: %d", g_SdioTransfer.blockSize, g_SdioTransfer.blockNum );

// FIXME find what the heck is that !:
// program_right_num=0;

    // Check buffer.
    MCD_ASSERT(pWrite != NULL, "SDIO write: Buffer is NULL");
    MCD_ASSERT(((UINT32)pWrite & 0x3) ==0,
               "SDIO write: buffer is not aligned! addr=%08x", pWrite);
    MCD_ASSERT(blockNum>=1 && blockNum<= SDIO_MAX_BLOCK_NUM,
               "Block Num is overflow");
    // The command for single block or multiple blocks are differents
    if (blockNum == 1)
    {
        writeCmd = HAL_SDIO_CMD_WRITE_SINGLE_BLOCK;
        blockMode = 0x00;
    }
    else
    {
        writeCmd = HAL_SDIO_CMD_WRITE_MULT_BLOCK;
        blockMode =0x01;
    }

    // Initiate data migration through Ifc.

    if (hal_SdioTransfer(&g_SdioTransfer) != HAL_ERR_NO)
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "write sd no ifc channel");
        return MCD_ERR_DMA_BUSY;
    }

    UINT32 tmp_reg =0x98000000;
//          MCD_TRACE(MCD_INFO_TRC, 0, "SDIO_WifiDataMultBlockWrite, sidodata_size: %d",sidodata_size);

    /*  if(sidodata_size ==512)
            tmp_reg|=(0x00|((UINT32)blockAddr)<<9);
            else
            */
    tmp_reg|=(blockNum|((UINT32)blockAddr)<<9);
    errStatus = mcd_SdioSendCmd(HAL_SDIO_CMD_IO_RW_EXTENDED, (UINT32)tmp_reg, cardResponse, FALSE,1,1);
    MCD_TRACE(MCD_INFO_TRC, 0, "CMD 53, arg: 0x%x, err status: %d,  response: 0x%x",tmp_reg,errStatus,cardResponse[3]);
    sxs_Dump(MCD_INFO_TRC, 0,(U8 *)cardResponse, 16);

    if (errStatus != MCD_ERR_NO)
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "Set sd write had error");
        hal_SdioStopTransfer(&g_SdioTransfer);
        return MCD_ERR_CMD;
    }

    // Wait
    startTime = hal_TimGetUpTime();

    while(!hal_SdioTransferDone())
    {
        if (hal_TimGetUpTime() - startTime >  (MCD_WRITE_TIMEOUT*blockNum*2))
        {
            //MCD_TRACE(MCD_INFO_TRC, 0, "Write on Sdmmc timeout");
            MCD_TRACE(MCD_INFO_TRC, 0, "Write on Sdmmc timeout, TC: %d, Status: 0x%x", returnTC(),returnsdmmcSta());

            // Abort the transfert.
            hal_SdmmcStopTransfer(&g_SdioTransfer);
            return MCD_ERR_CARD_TIMEOUT;

        }
    }

    if (mcd_SdioWriteCheckCrc() != MCD_ERR_NO)
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "SDIO Write error");
        return MCD_ERR;
    }
    return MCD_ERR_NO;
#else
    MCD_TRACE(MCD_INFO_TRC, 0, "SDIO_WifiDataMultBlockWrite, datalen: %d",datalen);

    UINT32 cardResponse[4] = {0, 0, 0, 0};
    MCD_ERR_T errStatus = MCD_ERR_NO;

    UINT32 setlen =0x00;

    UINT32 blockNum =1;

    blockNum = (datalen+SDIO_BLOCK_SIZE-1)/SDIO_BLOCK_SIZE;
//#ifndef USE_SDIO_DMA
    if(!g_SdioIsDmaEnable)
    {

        if(blockNum>1)
        {
            SDIO_WifiDataSingleBlockWrite(blockAddr,  pWrite, SDIO_BLOCK_SIZE);
            //sxr_Sleep(200);
            SDIO_WifiDataSingleBlockWrite(blockAddr,  pWrite+SDIO_BLOCK_SIZE, SDIO_BLOCK_SIZE);
            if(blockNum>2)
                SDIO_WifiDataSingleBlockWrite(blockAddr,  pWrite+SDIO_BLOCK_SIZE*2, SDIO_BLOCK_SIZE);

            return 0;
        }
    }
//#endif


    setlen = (SDIO_BLOCK_SIZE * blockNum)>>2;
    /*
    sidodata_size = (datalen+3)&0xFFFC;
    UINT16 tmplen =8;
    while(sidodata_size > tmplen)
    {
        tmplen =tmplen<<1;
    }
    sidodata_size = tmplen;
    setlen = sidodata_size>>2;
    */

    U8 byted ;

    U8 function_num = 1;

    UINT32 tmp_addr = 0x80000000;

    byted = setlen&0xFF;

    tmp_addr = tmp_addr |(function_num<<28) |( 0x00  <<9)|byted;
    send_test_cmd(52,tmp_addr);

//sxr_Sleep(10);

    byted = (setlen>>8)&0x7F;
    byted = 0x80|byted;
    tmp_addr = 0x80000000;
    tmp_addr = tmp_addr |(function_num<<28) |( 0x01  <<9)|byted;
    send_test_cmd(52,tmp_addr);

//blockAddr = 0x110;
    UINT32 startTime = 0;

    HAL_SDIO_CMD_T writeCmd;
    U8 blockMode =0x00;


    g_SdioTransfer.sysMemAddr = (UINT8*) pWrite;
    g_SdioTransfer.sdCardAddr =(UINT8 *) blockAddr;
    g_SdioTransfer.blockNum   = blockNum;
    g_SdioTransfer.blockSize  = SDIO_BLOCK_SIZE;
    g_SdioTransfer.direction  = HAL_SDIO_DIRECTION_WRITE;

    MCD_TRACE(MCD_INFO_TRC, 0, "SDIO_WifiDataMultBlockWrite, blockSIze: %d,  blockNum: %d", g_SdioTransfer.blockSize, g_SdioTransfer.blockNum );

// FIXME find what the heck is that !:
// program_right_num=0;

    // Check buffer.
    MCD_ASSERT(pWrite != NULL, "SDIO write: Buffer is NULL");
    MCD_ASSERT(((UINT32)pWrite & 0x3) ==0,
               "SDIO write: buffer is not aligned! addr=%08x", pWrite);
    MCD_ASSERT(blockNum>=1 && blockNum<= SDIO_MAX_BLOCK_NUM,
               "Block Num is overflow");
    // The command for single block or multiple blocks are differents
    if (blockNum == 1)
    {
        writeCmd = HAL_SDIO_CMD_WRITE_SINGLE_BLOCK;
        blockMode = 0x00;
    }
    else
    {
        writeCmd = HAL_SDIO_CMD_WRITE_MULT_BLOCK;
        blockMode =0x01;
    }

    // Initiate data migration through Ifc.

    if (hal_SdioTransfer(&g_SdioTransfer) != HAL_ERR_NO)
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "write sd no ifc channel");
        return MCD_ERR_DMA_BUSY;
    }

    UINT32 tmp_reg =0x98000000;
//          MCD_TRACE(MCD_INFO_TRC, 0, "SDIO_WifiDataMultBlockWrite, sidodata_size: %d",sidodata_size);

    /*  if(sidodata_size ==512)
            tmp_reg|=(0x00|((UINT32)blockAddr)<<9);
            else
            */
    tmp_reg|=(blockNum|((UINT32)blockAddr)<<9);
    errStatus = mcd_SdioSendCmd(HAL_SDIO_CMD_IO_RW_EXTENDED, (UINT32)tmp_reg, cardResponse, FALSE,1,1);
    MCD_TRACE(MCD_INFO_TRC, 0, "CMD 53, arg: 0x%x, err status: %d,  response: 0x%x",tmp_reg,errStatus,cardResponse[3]);
    sxs_Dump(MCD_INFO_TRC, 0,(U8 *)cardResponse, 16);

    if (errStatus != MCD_ERR_NO)
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "Set sd write had error");
        hal_SdioStopTransfer(&g_SdioTransfer);
        return MCD_ERR_CMD;
    }

    // Wait
    startTime = hal_TimGetUpTime();

    while(!hal_SdioTransferDone())
    {
        if (hal_TimGetUpTime() - startTime >  (MCD_WRITE_TIMEOUT*blockNum*2))
        {
            //MCD_TRACE(MCD_INFO_TRC, 0, "Write on Sdmmc timeout");
            MCD_TRACE(MCD_INFO_TRC, 0, "Write on Sdmmc timeout, TC: %d, Status: 0x%x", returnTC(),returnsdmmcSta());

            // Abort the transfert.
            hal_SdmmcStopTransfer(&g_SdioTransfer);
            return MCD_ERR_CARD_TIMEOUT;

        }
    }

    if (mcd_SdioWriteCheckCrc() != MCD_ERR_NO)
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "SDIO Write error");
        return MCD_ERR;
    }
    return MCD_ERR_NO;
#endif
}
U8 SDIO_WifiDataSingleBlockWriteForMutiBlock(UINT32 blockAddr, const U8* pWrite, UINT32 datalen)
{
    MCD_TRACE(MCD_INFO_TRC, 0, "SDIO_WifiDataSingleBlockWrite, datalen: %d",datalen);

    UINT32 cardResponse[4] = {0, 0, 0, 0};
    MCD_ERR_T errStatus = MCD_ERR_NO;
    UINT32 tmp_addr ;
    u16 setlen =0x00;

    sidodata_size = (datalen+3)&0xFFFC;
    UINT16 tmplen =32;
    while(sidodata_size > tmplen)
    {
        tmplen =tmplen<<1;
    }
    sidodata_size = tmplen;
    setlen = sidodata_size>>2;



//add flow contrl, 2011-12-26
    /*
    if(!wifi_context.firstWrite)
    {
          UINT8 count =0;
         UINT32 ret = 0x00;
            tmp_addr = 0x00000000;
            tmp_addr = tmp_addr |(1<<28) |( 0x05 <<9);
            usingIrqClr();
            ret = read_test_cmd(52,tmp_addr);
            usingIrqSet();
            MCD_TRACE(MCD_INFO_TRC, 0, "new SDIO_WifiDataSingleBlockWrite: 0x%x,  0x%x",(ret & 0x40),count);

        while(((ret & 0x40)== 0x00)&&(count < 30))
        {
            sxr_Sleep(200);
            tmp_addr = 0x00000000;
            tmp_addr = tmp_addr |(1<<28) |( 0x05 <<9);
            usingIrqClr();
            ret = read_test_cmd(52,tmp_addr);
            usingIrqSet();
            count++;

        }
    //  MCD_TRACE(MCD_INFO_TRC, 0, "new SDIO_WifiDataSingleBlockWrite: 0x%x,  0x%x",(ret & 0x40),count);

        if(count < 30)
        {
            tmp_addr = 0x88000000;
            tmp_addr = tmp_addr |(1<<28) |( 0x05 <<9)|0x40;
            usingIrqClr();

            send_test_cmd(52,tmp_addr);
            usingIrqSet();

        }else{


            MCD_TRACE(MCD_INFO_TRC, 0, "WIFI is busy, reset!");
            ret =hal_SdioReadSdioConfig();
                hal_HstSendEvent(0x99999999);
                hal_HstSendEvent(ret);
            return  MCD_ERR;
        }
        //firstWrite = FALSE;
    }

    wifi_context.firstWrite = FALSE;

    */
    UINT32 blockNum =1;

    U8 byted ;

    U8 function_num = 1;
    UINT32 startTime = 0;

//    HAL_SDIO_CMD_T writeCmd;
    U8 blockMode =0x00;

    g_SdioTransfer.sysMemAddr = (UINT8*) pWrite;
    g_SdioTransfer.sdCardAddr = (UINT8 *)blockAddr;
    g_SdioTransfer.blockNum   = blockNum;
    g_SdioTransfer.blockSize  = sidodata_size;      //g_mcdBlockLen;
    g_SdioTransfer.direction  = HAL_SDIO_DIRECTION_WRITE;


// FIXME find what the heck is that !:
// program_right_num=0;

    // Check buffer.
    MCD_ASSERT(pWrite != NULL, "SDIO write: Buffer is NULL");
    MCD_ASSERT(((UINT32)pWrite & 0x3) ==0,
               "SDIO write: buffer is not aligned! addr=%08x", pWrite);
    MCD_ASSERT(blockNum>=1 && blockNum<= SDIO_MAX_BLOCK_NUM,
               "Block Num is overflow");
    // The command for single block or multiple blocks are differents

    hal_SdioTransferinit(&g_SdioTransfer);

    U8 iii =0;
    //    UINT32 ret_val =datalen;
    UINT32* tmp_data_p = NULL;
    tmp_data_p = (UINT32 *)pWrite;
    for(iii=0; iii<(sidodata_size/4); iii++)
    {
        hal_Write_databyFIFO(*tmp_data_p);
        //MCD_TRACE(MCD_INFO_TRC, 0, "wrire FIFO: 0x%x",*tmp_data_p);
        tmp_data_p++;

    }
    MCD_TRACE(MCD_INFO_TRC, 0, "wrire to  FIFO: ");

    sxs_Dump(_MMI, 0, (u8*)pWrite, sidodata_size);


    UINT32 tmp_reg =0x90000000;
    MCD_TRACE(MCD_INFO_TRC, 0, "SDIO_WifiDataSingleBlockWrite, sidodata_size: %d",sidodata_size);

    if(sidodata_size ==512)
        tmp_reg|=(0x00|((UINT32)blockAddr)<<9);
    else
        tmp_reg|=(sidodata_size|((UINT32)blockAddr)<<9);
    errStatus = mcd_SdioSendCmd(HAL_SDIO_CMD_IO_RW_EXTENDED, (UINT32)tmp_reg, cardResponse, FALSE,1,0);
    MCD_TRACE(MCD_INFO_TRC, 0, "CMD 53, arg: 0x%x, err status: %d,  response: 0x%x",tmp_reg,errStatus,cardResponse[3]);
    sxs_Dump(MCD_INFO_TRC, 0,(U8 *)cardResponse, 16);
//sxr_Sleep(10);

    if (errStatus != MCD_ERR_NO)
    {
//  Kill_pollTimer();

        MCD_TRACE(MCD_INFO_TRC, 0, "Set sd write had error");
        return MCD_ERR_CMD;
    }
    // Wait
    startTime = hal_TimGetUpTime();

    while(! hal_SdioTransferFiFoDone())
    {
        if (hal_TimGetUpTime() - startTime >  (MCD_READ_TIMEOUT*blockNum*5))
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "write on Sdmmc timeout");
//
//          Kill_pollTimer();
            //usingIrqSet();

            return MCD_ERR_CARD_TIMEOUT;

        }
        // sxr_Sleep(10*16384/1000);
    }

    hal_SdioTransferFiFoDoneClr();

    if (mcd_SdioWriteCheckCrc() != MCD_ERR_NO)
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "SDIO Write error");
        //usingIrqSet();

        return MCD_ERR;
    }


    //usingIrqSet();

    return MCD_ERR_NO;

}

U8 SDIO_WifiDataMultBlockWriteEx(UINT32 blockAddr, const U8* pWrite, UINT32 datalen)
{
    MCD_TRACE(MCD_INFO_TRC, 0, "SDIO_WifiDataMultBlockWrite, datalen: %d",datalen);

    UINT32 cardResponse[4] = {0, 0, 0, 0};
    MCD_ERR_T errStatus = MCD_ERR_NO;

    UINT32 setlen =0x00;

    UINT32 blockNum =1;

    blockNum = (datalen+SDIO_BLOCK_SIZE-1)/SDIO_BLOCK_SIZE;

    setlen = (SDIO_BLOCK_SIZE * blockNum)>>2;

    U8 byted ;

    U8 function_num = 1;

    UINT32 tmp_addr = 0x80000000;

    byted = setlen&0xFF;
    usingIrqSet();

    tmp_addr = tmp_addr |(function_num<<28) |( 0x00  <<9)|byted;
    send_test_cmd(52,tmp_addr);

    //sxr_Sleep(10);

    byted = (setlen>>8)&0x7F;
    byted = 0x80|byted;
    tmp_addr = 0x80000000;
    tmp_addr = tmp_addr |(function_num<<28) |( 0x01  <<9)|byted;
    send_test_cmd(52,tmp_addr);

    UINT8 i = 0;
    for(i=0; i<blockNum; i++ )
    {
        //SDIO_WifiDataSingleBlockWriteForMutiBlock(blockAddr,  pWrite, SDIO_BLOCK_SIZE);
        //sxr_Sleep(200);
#ifndef __USE_SDMMC1__
        SDIO_WifiDataSingleBlockWriteForMutiBlock(blockAddr,  pWrite+SDIO_BLOCK_SIZE * i, SDIO_BLOCK_SIZE);
#else
        SDIO_WifiDataSingleBlockWriteMutiEx(blockAddr,  pWrite+SDIO_BLOCK_SIZE * i, SDIO_BLOCK_SIZE);
#endif
        //sxr_Sleep(163);
        //if(blockNum>2)
        //      SDIO_WifiDataSingleBlockWriteForMutiBlock(blockAddr,  pWrite+SDIO_BLOCK_SIZE*2, SDIO_BLOCK_SIZE);


    }
    usingIrqSet();
    return 0;

}



VOID wifi_SdioSleep(VOID)
{
    MCD_TRACE(MCD_INFO_TRC, 0,  "wifi_SdioSleep...\n");

    hal_HstSendEvent(0x11110032);
    //hal_SdioSetClk(0);
    hal_SdioSleep();
    sdio_sleep_flag = TRUE;


}
VOID wifi_SdioWakeUp(VOID)
{
    MCD_TRACE(MCD_INFO_TRC, 0,  "wifi_SdioWakeUp...\n");

    hal_HstSendEvent(0x11110312);
    hal_SdioWakeUp();
    sdio_sleep_flag = FALSE;

}

U8 SDIO_WifiDataWrite(UINT32 blockAddr, const U8* pWrite, UINT32 datalen)
{
    //if(wifiDataInd>0)
    //  return 0x0F;
    //hal_sdioDataInIrqMaskClear();

#ifndef USE_WIFI_SLEEP
    if(!gettingRssi)
#endif
        wifi_enableSleepTimerCount = 0;
//report_rssi_count = 25;

    MCD_CS_ENTER
    ;
    stop_wifi_PreAssoWait_timer();
    // wifi_SdioWakeUp();

    if(wifi_sleep_flag)
    {
        //return 0;


        rda5890_WifWakeUp( );
        //sxr_Sleep(100);
        wifi_sleep_flag = FALSE;

    }

//rda5890_WifWakeUp( );
    U8 ret = 0;
    UINT32 blockNum =1;
    blockNum = (datalen+SDIO_BLOCK_SIZE-1)/SDIO_BLOCK_SIZE;
    if(1==blockNum)
    {
        //disable multiblock
        //      send_test_cmd(52,0x88001000);
        //sxr_Sleep(200);
        /*
        //5, set function block size
        UINT16 blocksize = SDIO_BLOCK_SIZE;
        UINT8 bytetmp = blocksize&0xff;
        send_test_cmd(52,0x88000000|(0x110<<9)|bytetmp);

        sxr_Sleep(80);
        bytetmp =( blocksize>>8)&0xff;
        send_test_cmd(52,0x88000000|(0x111<<9)|bytetmp);
        */
        ret = SDIO_WifiDataSingleBlockWrite(blockAddr, pWrite, datalen);

    }
    else
    {
        //enable multiblock
        // send_test_cmd(52,0x88001002);
        //5, set function block size
        //  UINT16 blocksize = SDIO_BLOCK_SIZE;

        //  UINT8 bytetmp = blocksize&0xff;
        //  send_test_cmd(52,0x88000000|(0x110<<9)|bytetmp);
        //  bytetmp =( blocksize>>8)&0xff;
        //  send_test_cmd(52,0x88000000|(0x111<<9)|bytetmp);

        //#ifdef USE_SDIO_DMA
        if(g_SdioIsDmaEnable)
        {
            if((wifi_version_flag== WIFI_VERSION_E) ||(wifi_version_flag== WIFI_VERSION_F))
                ret = SDIO_WifiDataMultBlockWriteEx(blockAddr, pWrite, datalen);
            else
                ret = SDIO_WifiDataMultBlockWrite(blockAddr, pWrite, datalen);
        }
        else
        {
            //#else
            ret = SDIO_WifiDataMultBlockWriteEx(blockAddr, pWrite, datalen);
        }

//#endif


    }
    //sxr_Sleep(500);
    //  hal_HstSendEvent(0x33330000+datalen);
//wifi_SdioSleep();
#if 0

    start_wifi_PreAssoWait_timer();

#else
    MCD_TRACE(MCD_INFO_TRC, 0,  "SDIO_WifiDataWrite:  wifi_sleep_flag: %d; ap_connected: %d;  ap_connecting: %d; ap_disconnecting: %d\n",wifi_sleep_flag,ap_connected,ap_connecting,preasso_timer_enable);


    sxs_Dump(_MMI, 0, pWrite, datalen);

    if((!wifi_sleep_flag)&&(ap_connected))
    {
        start_wifi_PreAssoWait_timer(20);

        //  wifi_MAC_sleepEx();
        /*
                UINT32  tmp_addr = 0x88000000;
            tmp_addr = tmp_addr |(1<<28) |( 0x05 <<9)|0x80;
            send_test_cmd(52,tmp_addr);
                    wifi_sleep_flag = TRUE;
                    */

    }
    if((!ap_connected)&&(!ap_connecting)&&(preasso_timer_enable))
    {
        start_wifi_PreAssoWait_timer(50);

    }

#endif
    MCD_CS_EXIT;
    //  hal_sdioDataInIrqMaskSet();

    return ret;
}


//=============================================================================
// mcd_SdioMultBlockRead
//-----------------------------------------------------------------------------
/// Read one or a bunch of data blocks.
/// @param blockAddr Address on the card where to read data.
/// @param pRead Pointer towards the buffer of data to read.
/// @param blockNum Number of blocks to read.
//=============================================================================
U16 sdio_GetReadDataLen(U8 function_num)
{
    U16 datalen =0;
    UINT32 vald ;
    //UINT8 i =0;
    UINT32 tmp_addr = 0x00000000;
    /*for(i=0;i<0x10;i++)
    {
     tmp_addr = 0x00000000;

    tmp_addr = tmp_addr |(function_num<<28) |( i  <<9);
    vald = read_test_cmd(52,tmp_addr);
    sxr_Sleep(20);
    //datalen |= vald &0xFF;
    }
     tmp_addr = 0x00000000;

    tmp_addr = tmp_addr |(function_num<<28) |( 0x05 <<9);
    vald = read_test_cmd(52,tmp_addr);


    MCD_TRACE(MCD_INFO_TRC, 0, "sdio_GetReadAddr, addr: 0x06,  vald: 0x%x",vald);

    */
    tmp_addr = 0x00000000;

    tmp_addr = tmp_addr |(function_num<<28) |( 0x02  <<9);
    vald = read_test_cmd(52,tmp_addr);
    datalen |= vald &0xFF;

    tmp_addr = 0x00000000;
    tmp_addr = tmp_addr |(function_num<<28) |( 0x03  <<9);
    vald = read_test_cmd(52,tmp_addr);
    datalen |= (vald &0xFF)<<8;

    MCD_TRACE(MCD_INFO_TRC, 0, "sdio_GetReadDataLen,  datalen: %d",datalen<<2);

    return (datalen<<2);
}

void sdio_GetReadAddr(U8 function_num,UINT8 addr)
{

    UINT32 vald ;



    UINT32 tmp_addr = 0x00000000;

    tmp_addr = tmp_addr |(function_num<<28) |( addr  <<9);
    vald = read_test_cmd(52,tmp_addr);


    MCD_TRACE(MCD_INFO_TRC, 0, "sdio_GetReadAddr, addr: 0x%x,  vald: 0x%x",addr,vald);


}
//UINT32 hal_read_databyFIFO(void);
/*
#define KSEG1(addr)     ( (addr) | 0xa0000000 )
UINT32 hal_read_databyFIFO(void)
{
   UINT32 val;
   val = hwp_sdmmc2->APBI_FIFO_TxRx;
          //            MCD_TRACE(MCD_INFO_TRC, 0, "read FIFO: 0x%x", *hwp_sdmmc2->APBI_FIFO_TxRx);

   return val;


}
*/

U8 SDIO_WifiDataSingleBlockRead(U8* blockAddr, U8* pRead, UINT32 datalen)
{
    MCD_TRACE(MCD_INFO_TRC, 0, "SDIO_WifiDataSingleBlockRead, buf addr: 0x%x,  datalen: %d",pRead,datalen);
    UINT32                  cardResponse[4]     = {0, 0, 0, 0};
    MCD_ERR_T               errStatus           = MCD_ERR_NO;

    sidodata_size=datalen;
    UINT32 blockNum =1;
    UINT32 startTime = 0;






    HAL_SDIO_CMD_T writeCmd;
    U8 blockMode =0x00;

    g_SdioTransfer.sysMemAddr = (UINT8*) pRead;
    g_SdioTransfer.sdCardAddr = blockAddr;
    g_SdioTransfer.blockNum   = blockNum;
    g_SdioTransfer.blockSize  = sidodata_size;      //g_mcdBlockLen;
    g_SdioTransfer.direction  = HAL_SDIO_DIRECTION_READ;


// FIXME find what the heck is that !:
// program_right_num=0;

    // Check buffer.
    MCD_ASSERT(pRead != NULL, "SDIO write: Buffer is NULL");
    MCD_ASSERT(((UINT32)pRead & 0x3) ==0,
               "SDIO write: buffer is not aligned! addr=%08x", pRead);
    MCD_ASSERT(blockNum>=1 && blockNum<= SDIO_MAX_BLOCK_NUM,
               "Block Num is overflow");

    // The command for single block or multiple blocks are differents
    if (blockNum == 1)
    {
        writeCmd = HAL_SDIO_CMD_READ_SINGLE_BLOCK;
        blockMode = 0x00;
    }
    else
    {
        writeCmd = HAL_SDIO_CMD_READ_MULT_BLOCK;
        blockMode =0x01;
    }

    // Initiate data migration through Ifc.
    // MCD_TRACE(MCD_INFO_TRC, 0, "read FIFO 1: 0x%x",hal_read_databyFIFO());

    //remove 2012-1-5
#if 0
    if((wifi_version_flag== WIFI_VERSION_E) ||(wifi_version_flag== WIFI_VERSION_F))
    {
        hal_SdioTransferinit(&g_SdioTransfer);

        UINT32 tmp_reg =0x10000000;
        //    UINT32 tmp_reg =0x10000000;

        if(sidodata_size ==512)
            tmp_reg|=(0x00|((UINT32)blockAddr)<<9);
        else
            tmp_reg|=(sidodata_size|((UINT32)blockAddr)<<9);    //tmp_reg|=blockAddr;

        // hwp_sdmmc2->apbi_ctrl_sdmmc = SDIO_SOFT_RST_L | SDIO_L_ENDIAN(1);


        errStatus = mcd_SdioSendCmd(HAL_SDIO_CMD_IO_RW_EXTENDED, (UINT32)tmp_reg, cardResponse, FALSE,0,0);

        //  tmp_reg|=(sidodata_size|((UINT32)blockAddr)<<9);
        //tmp_reg|=blockAddr;
        //   errStatus = mcd_SdioSendCmd(HAL_SDIO_CMD_IO_RW_EXTENDED, (UINT32)tmp_reg, cardResponse, FALSE);
        MCD_TRACE(MCD_INFO_TRC, 0, "CMD 53, arg: 0x%x, err status: %d,  response: 0x%x",tmp_reg,errStatus,cardResponse[3]);
        sxs_Dump(MCD_INFO_TRC, 0,(U8 *)cardResponse, 16);
        U8 iii =0;
        UINT32 ret_val =0;
        UINT32* tmp_data_p =0x00;
        startTime = hal_TimGetUpTime();
        //   MCD_TRACE(MCD_INFO_TRC, 0, "read FIFO 3: 0x%x",hal_read_databyFIFO());

        //              Kill_pollTimer();
        //             return MCD_ERR_CARD_TIMEOUT;

        while(! hal_SdioTransferFiFoDone())
        {
            sxr_Sleep(200);
            if (hal_TimGetUpTime() - startTime >  (MCD_READ_TIMEOUT*blockNum*3))
            {
                MCD_TRACE(MCD_INFO_TRC, 0, "Read on Sdio timeout");
                wifi_ResetReq();
                // Kill_pollTimer();
                return MCD_ERR_CARD_TIMEOUT;

            }
            // sxr_Sleep(10*16384/1000);
        }
        //   hwp_sdmmc2->SDMMC_INT_CLEAR = SDIO_DAT_OVER_CL;
        tmp_data_p = (UINT32 *)pRead;
        for(iii=0; iii<(sidodata_size/4); iii++)
        {
            ret_val =hal_read_databyFIFO();
            //  MCD_TRACE(MCD_INFO_TRC, 0, "read FIFO: 0x%x",ret_val);
            *tmp_data_p = ret_val;
            tmp_data_p++;

        }
        MCD_TRACE(MCD_INFO_TRC, 0, "read from FIFO: ");

//  sxs_Dump(_MMI, 0, pRead, 32);

        hal_SdioTransferFiFoDoneClr();


        if (mcd_SdioReadCheckCrc() != MCD_ERR_NO)
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "SDIO Read error");
            return MCD_ERR;
        }

        MCD_TRACE(MCD_INFO_TRC, 0, "################ test ");





// MCD_TRACE(MCD_INFO_TRC, 0, "read FIFO 4: 0x%x",hal_read_databyFIFO());





//hal_SdioReadFlushFifo();
        // Flush Cache
        hal_SysInvalidateCache(pRead, blockNum * sidodata_size);


// MCD_TRACE(MCD_INFO_TRC, 0, "read FIFO 5: 0x%x",hal_read_databyFIFO());
        return MCD_ERR_NO;
    }
#endif
//#ifdef USE_SDIO_DMA
    if(g_SdioIsDmaEnable)
    {
//#if 0
        MCD_TRACE(MCD_INFO_TRC, 0, "using DMA ");

        if (hal_SdioTransfer(&g_SdioTransfer) != HAL_ERR_NO)
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "write sd no ifc channel");
            return MCD_ERR_DMA_BUSY;
        }

        // Initiate data migration of multiple blocks through SD bus.
        //  errStatus = mcd_SdmmcSendCmd(writeCmd, (UINT32)blockAddr, cardResponse, FALSE);
//  MCD_TRACE(MCD_INFO_TRC, 0, "read FIFO 2: 0x%x",hal_read_databyFIFO());

        UINT32 tmp_reg =0x10000000;

        if(sidodata_size ==512)
            tmp_reg|=(0x00|((UINT32)blockAddr)<<9);
        else
            tmp_reg|=(sidodata_size|((UINT32)blockAddr)<<9);    //tmp_reg|=blockAddr;



        errStatus = mcd_SdioSendCmd(HAL_SDIO_CMD_IO_RW_EXTENDED, (UINT32)tmp_reg, cardResponse, FALSE,0,0);
        MCD_TRACE(MCD_INFO_TRC, 0, "CMD 53, arg: 0x%x, err status: %d,  response: 0x%x",tmp_reg,errStatus,cardResponse[3]);
        sxs_Dump(MCD_INFO_TRC, 0,(U8 *)cardResponse, 16);

        if (errStatus != MCD_ERR_NO)
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "Set sd read had error");
            hal_SdioStopTransfer(&g_SdioTransfer);
            return MCD_ERR_CMD;
        }
        // Wait
        startTime = hal_TimGetUpTime();
//   MCD_TRACE(MCD_INFO_TRC, 0, "read FIFO 3: 0x%x",hal_read_databyFIFO());

        while(!hal_SdioTransferDone())
        {
            //sxr_Sleep(200);
            if (hal_TimGetUpTime() - startTime >  (MCD_READ_TIMEOUT*blockNum*2))
            {
                MCD_TRACE(MCD_INFO_TRC, 0, "Read on Sdio timeout");
                //  Kill_pollTimer();

                // Abort the transfert.
                hal_SdioStopTransfer(&g_SdioTransfer);
                wifi_ResetReq();
                return MCD_ERR_CARD_TIMEOUT;
            }
        }
        if (mcd_SdioReadCheckCrc() != MCD_ERR_NO)
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "SDIO Read error");
            return MCD_ERR;
        }

        MCD_TRACE(MCD_INFO_TRC, 0, "################ test ");

// MCD_TRACE(MCD_INFO_TRC, 0, "read FIFO 4: 0x%x",hal_read_databyFIFO());

        hal_SdioReadFlushFifo();
        // Flush Cache
        hal_SysInvalidateCache(pRead, blockNum * sidodata_size);
    }
    else
    {
//#else
        hal_SdioTransferinit(&g_SdioTransfer);

        UINT32 tmp_reg =0x10000000;
        //    UINT32 tmp_reg =0x10000000;

        if(sidodata_size ==512)
            tmp_reg|=(0x00|((UINT32)blockAddr)<<9);
        else
            tmp_reg|=(sidodata_size|((UINT32)blockAddr)<<9);    //tmp_reg|=blockAddr;

        // hwp_sdmmc2->apbi_ctrl_sdmmc = SDIO_SOFT_RST_L | SDIO_L_ENDIAN(1);


        errStatus = mcd_SdioSendCmd(HAL_SDIO_CMD_IO_RW_EXTENDED, (UINT32)tmp_reg, cardResponse, FALSE,0,0);

        //  tmp_reg|=(sidodata_size|((UINT32)blockAddr)<<9);
        //tmp_reg|=blockAddr;
        //   errStatus = mcd_SdioSendCmd(HAL_SDIO_CMD_IO_RW_EXTENDED, (UINT32)tmp_reg, cardResponse, FALSE);
        MCD_TRACE(MCD_INFO_TRC, 0, "CMD 53, arg: 0x%x, err status: %d,  response: 0x%x",tmp_reg,errStatus,cardResponse[3]);
        sxs_Dump(MCD_INFO_TRC, 0,(U8 *)cardResponse, 16);
        U8 iii =0;
        UINT32 ret_val =0;
        UINT32* tmp_data_p =0x00;
        startTime = hal_TimGetUpTime();
        //   MCD_TRACE(MCD_INFO_TRC, 0, "read FIFO 3: 0x%x",hal_read_databyFIFO());

        //              Kill_pollTimer();
        //             return MCD_ERR_CARD_TIMEOUT;

        while(! hal_SdioTransferFiFoDone())
        {
            //sxr_Sleep(200);
            if (hal_TimGetUpTime() - startTime >  (MCD_READ_TIMEOUT*blockNum*3))
            {
                MCD_TRACE(MCD_INFO_TRC, 0, "Read on Sdio timeout");
                wifi_ResetReq();
                // Kill_pollTimer();
                return MCD_ERR_CARD_TIMEOUT;

            }
            // sxr_Sleep(10*16384/1000);
        }
        //   hwp_sdmmc2->SDMMC_INT_CLEAR = SDIO_DAT_OVER_CL;
        tmp_data_p = (UINT32 *)pRead;
        for(iii=0; iii<(sidodata_size/4); iii++)
        {
            ret_val =hal_read_databyFIFO();
            //  MCD_TRACE(MCD_INFO_TRC, 0, "read FIFO: 0x%x",ret_val);
            *tmp_data_p = ret_val;
            tmp_data_p++;

        }
        MCD_TRACE(MCD_INFO_TRC, 0, "read from FIFO: ");

//  sxs_Dump(_MMI, 0, pRead, 32);

        hal_SdioTransferFiFoDoneClr();


        if (mcd_SdioReadCheckCrc() != MCD_ERR_NO)
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "SDIO Read error");
            return MCD_ERR;
        }

        MCD_TRACE(MCD_INFO_TRC, 0, "################ test ");





// MCD_TRACE(MCD_INFO_TRC, 0, "read FIFO 4: 0x%x",hal_read_databyFIFO());





//hal_SdioReadFlushFifo();
        // Flush Cache
        hal_SysInvalidateCache(pRead, blockNum * sidodata_size);
    }
    //#endif

// MCD_TRACE(MCD_INFO_TRC, 0, "read FIFO 5: 0x%x",hal_read_databyFIFO());
    return MCD_ERR_NO;



}

U8 SDIO_WifiDataMultBlockRead(U8* blockAddr, U8* pRead, UINT32 datalen)
{

    UINT32                  cardResponse[4]     = {0, 0, 0, 0};
    MCD_ERR_T               errStatus           = MCD_ERR_NO;


    UINT32 blockNum =1;
    UINT32 startTime = 0;


    blockNum = (datalen+SDIO_BLOCK_SIZE-1)/SDIO_BLOCK_SIZE;
    MCD_TRACE(MCD_INFO_TRC, 0, "SDIO_WifiDataMultBlockRead, datalen: %d; blockNum: %d",datalen,blockNum);
//#ifndef USE_SDIO_DMA
    if(blockNum>1)
    {
        UINT8 i =0;
        for(i=0; i<blockNum; i++)
        {
            SDIO_WifiDataSingleBlockRead(blockAddr,  pRead+SDIO_BLOCK_SIZE*i, SDIO_BLOCK_SIZE);
            // sxr_Sleep(5*16384/1000);

        }
        /*
        SDIO_WifiDataSingleBlockRead(blockAddr,  pRead, SDIO_BLOCK_SIZE);
         sxr_Sleep(10*16384/1000);

        SDIO_WifiDataSingleBlockRead(blockAddr,  pRead+SDIO_BLOCK_SIZE, SDIO_BLOCK_SIZE);
           sxr_Sleep(100*16384/1000);

        if(blockNum>2)
                SDIO_WifiDataSingleBlockRead(blockAddr,  pRead+SDIO_BLOCK_SIZE*2, SDIO_BLOCK_SIZE);
        */
        return 0;
    }
//  #endif


//setlen = (SDIO_BLOCK_SIZE * blockNum)>>2;

//sidodata_size=blockNum * SDIO_BLOCK_SIZE;


    HAL_SDIO_CMD_T writeCmd;
    U8 blockMode =0x00;

    g_SdioTransfer.sysMemAddr = (UINT8*) pRead;
    g_SdioTransfer.sdCardAddr = blockAddr;
    g_SdioTransfer.blockNum   = blockNum;
    g_SdioTransfer.blockSize  = SDIO_BLOCK_SIZE;      //g_mcdBlockLen;
    g_SdioTransfer.direction  = HAL_SDIO_DIRECTION_READ;


// FIXME find what the heck is that !:
// program_right_num=0;

    // Check buffer.
    MCD_ASSERT(pRead != NULL, "SDIO write: Buffer is NULL");
    MCD_ASSERT(((UINT32)pRead & 0x3) ==0,
               "SDIO write: buffer is not aligned! addr=%08x", pRead);
    MCD_ASSERT(blockNum>=1 && blockNum<= SDIO_MAX_BLOCK_NUM,
               "Block Num is overflow");

    // The command for single block or multiple blocks are differents
    if (blockNum == 1)
    {
        writeCmd = HAL_SDIO_CMD_READ_SINGLE_BLOCK;
        blockMode = 0x00;
    }
    else
    {
        writeCmd = HAL_SDIO_CMD_READ_MULT_BLOCK;
        blockMode =0x01;
    }

    // Initiate data migration through Ifc.
    // MCD_TRACE(MCD_INFO_TRC, 0, "read FIFO 1: 0x%x",hal_read_databyFIFO());

    // #ifdef USE_SDIO_DMA
    if(g_SdioIsDmaEnable)
    {
        if (hal_SdioTransfer(&g_SdioTransfer) != HAL_ERR_NO)
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "write sd no ifc channel");
            return MCD_ERR_DMA_BUSY;
        }

        // Initiate data migration of multiple blocks through SD bus.
        //  errStatus = mcd_SdmmcSendCmd(writeCmd, (UINT32)blockAddr, cardResponse, FALSE);
//  MCD_TRACE(MCD_INFO_TRC, 0, "read FIFO 2: 0x%x",hal_read_databyFIFO());

        UINT32 tmp_reg =0x18000000;
        /*
        if(sidodata_size ==512)
            tmp_reg|=(0x00|((UINT32)blockAddr)<<9);
            else
        tmp_reg|=(sidodata_size|((UINT32)blockAddr)<<9);    //tmp_reg|=blockAddr;
        */
        tmp_reg|=(blockNum|((UINT32)blockAddr)<<9); //tmp_reg|=blockAddr;


        errStatus = mcd_SdioSendCmd(HAL_SDIO_CMD_IO_RW_EXTENDED, (UINT32)tmp_reg, cardResponse, FALSE,0,1);
        MCD_TRACE(MCD_INFO_TRC, 0, "CMD 53, arg: 0x%x, err status: %d,  response: 0x%x",tmp_reg,errStatus,cardResponse[3]);
        sxs_Dump(MCD_INFO_TRC, 0,(U8 *)cardResponse, 16);

        if (errStatus != MCD_ERR_NO)
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "Set sd read had error");
            hal_SdioStopTransfer(&g_SdioTransfer);
            return MCD_ERR_CMD;
        }
        // Wait
        startTime = hal_TimGetUpTime();
//   MCD_TRACE(MCD_INFO_TRC, 0, "read FIFO 3: 0x%x",hal_read_databyFIFO());

        while(!hal_SdioTransferDone())
        {
            if (hal_TimGetUpTime() - startTime >  (MCD_READ_TIMEOUT*blockNum*2))
            {

                MCD_TRACE(MCD_INFO_TRC, 0, "Read on Sdmmc timeout, TC: 0x%x, Status: 0x%x", returnTC(),returnsdmmcSta);
                // Abort the transfert.
                hal_SdioStopTransfer(&g_SdioTransfer);
                return MCD_ERR_CARD_TIMEOUT;
            }
        }
    }
    else
    {
//#else
        hal_SdioTransferinit(&g_SdioTransfer);

        UINT32 tmp_reg =0x18000000;
        /*
        if(sidodata_size ==512)
            tmp_reg|=(0x00|((UINT32)blockAddr)<<9);
            else
        tmp_reg|=(sidodata_size|((UINT32)blockAddr)<<9);    //tmp_reg|=blockAddr;
        */
        tmp_reg|=(blockNum|((UINT32)blockAddr)<<9); //tmp_reg|=blockAddr;


        errStatus = mcd_SdioSendCmd(HAL_SDIO_CMD_IO_RW_EXTENDED, (UINT32)tmp_reg, cardResponse, FALSE,0,1);
        MCD_TRACE(MCD_INFO_TRC, 0, "CMD 53, arg: 0x%x, err status: %d,  response: 0x%x",tmp_reg,errStatus,cardResponse[3]);
        sxs_Dump(MCD_INFO_TRC, 0,(U8 *)cardResponse, 16);

        if (errStatus != MCD_ERR_NO)
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "Set sd read had error");
            hal_SdioStopTransfer(&g_SdioTransfer);
            return MCD_ERR_CMD;
        }
        U8 iii =0;
        UINT32 ret_val =0;
        //sxr_Sleep(100*16384/1000);
        for(iii=0; iii<4; iii++)
        {
            ret_val =hal_read_databyFIFO();
            MCD_TRACE(MCD_INFO_TRC, 0, "read FIFO: 0x%x",ret_val);

        }
    }
//#endif


    if (mcd_SdioReadCheckCrc() != MCD_ERR_NO)
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "SDIO Read error");
        return MCD_ERR;
    }

    MCD_TRACE(MCD_INFO_TRC, 0, "################ test ");





// MCD_TRACE(MCD_INFO_TRC, 0, "read FIFO 4: 0x%x",hal_read_databyFIFO());





    hal_SdioReadFlushFifo();
    // Flush Cache
    hal_SysInvalidateCache(pRead, blockNum * sidodata_size);
// MCD_TRACE(MCD_INFO_TRC, 0, "read FIFO 5: 0x%x",hal_read_databyFIFO());
    return MCD_ERR_NO;



}
U8 SDIO_WifiDataRead(U8* blockAddr, U8* pRead, UINT32 datalen)
{
//MCD_CS_ENTER
    U8 ret =0;
    UINT32 blockNum =1;

    blockNum = (datalen+SDIO_BLOCK_SIZE-1)/SDIO_BLOCK_SIZE;
    if(1==blockNum)
    {
        //disable multiblock
        //     send_test_cmd(52,0x88001000);
        //sxr_Sleep(200);
        /*
        //5, set function block size
        UINT16 blocksize = SDIO_BLOCK_SIZE;
        UINT8 bytetmp = blocksize&0xff;
        send_test_cmd(52,0x88000000|(0x110<<9)|bytetmp);

        sxr_Sleep(80);
        bytetmp =( blocksize>>8)&0xff;
        send_test_cmd(52,0x88000000|(0x111<<9)|bytetmp);
        */
        ret = SDIO_WifiDataSingleBlockRead(blockAddr, pRead, datalen);

    }
    else
    {
        //enable multiblock
        //   send_test_cmd(52,0x88001002);
        //5, set function block size
        //UINT16 blocksize = SDIO_BLOCK_SIZE;

        //UINT8 bytetmp = blocksize&0xff;
        //send_test_cmd(52,0x88000000|(0x110<<9)|bytetmp);
        //bytetmp =( blocksize>>8)&0xff;
        //send_test_cmd(52,0x88000000|(0x111<<9)|bytetmp);
        ret = SDIO_WifiDataMultBlockRead(blockAddr, pRead, datalen);



    }
    //  sxr_Sleep(200);
    //MCD_CS_EXIT
    return ret;
}

U8 sdio_MultBlockReadByFIFO(U8* blockAddr, U8* pRead, UINT32 datalen)
{
    UINT32                  cardResponse[4]     = {0, 0, 0, 0};
    MCD_ERR_T               errStatus           = MCD_ERR_NO;

    sidodata_size=datalen;
    UINT32 blockNum =1;
//  UINT32 startTime = 0;






    HAL_SDIO_CMD_T writeCmd;
    U8 blockMode =0x00;

    g_SdioTransfer.sysMemAddr = (UINT8*) pRead;
    g_SdioTransfer.sdCardAddr = blockAddr;
    g_SdioTransfer.blockNum   = blockNum;
    g_SdioTransfer.blockSize  = sidodata_size;      //g_mcdBlockLen;
    g_SdioTransfer.direction  = HAL_SDIO_DIRECTION_READ;


// FIXME find what the heck is that !:
// program_right_num=0;

    // Check buffer.
    MCD_ASSERT(pRead != NULL, "Sdio write: Buffer is NULL");
    MCD_ASSERT(((UINT32)pRead & 0x3) ==0,
               "Sdio write: buffer is not aligned! addr=%08x", pRead);
    MCD_ASSERT(blockNum>=1 && blockNum<= SDIO_MAX_BLOCK_NUM,
               "Block Num is overflow");

    // The command for single block or multiple blocks are differents
    if (blockNum == 1)
    {
        writeCmd = HAL_SDIO_CMD_READ_SINGLE_BLOCK;
        blockMode = 0x00;
    }
    else
    {
        writeCmd = HAL_SDIO_CMD_READ_MULT_BLOCK;
        blockMode =0x01;
    }

    // Initiate data migration through Ifc.
//  MCD_TRACE(MCD_INFO_TRC, 0, "read FIFO 1: 0x%x",hal_read_databyFIFO());

#if 0
    if (hal_SdioTransfer(&g_SdioTransfer) != HAL_ERR_NO)
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "write sd no ifc channel");
        return MCD_ERR_DMA_BUSY;
    }

    // Initiate data migration of multiple blocks through SD bus.
    //  errStatus = mcd_SdmmcSendCmd(writeCmd, (UINT32)blockAddr, cardResponse, FALSE);
    MCD_TRACE(MCD_INFO_TRC, 0, "read FIFO 2: 0x%x",hal_read_databyFIFO());

    UINT32 tmp_reg =0x10000000;

    tmp_reg|=(sidodata_size|((UINT32)blockAddr)<<9);
    //tmp_reg|=blockAddr;
    errStatus = mcd_SdioSendCmd(HAL_SDIO_CMD_IO_RW_EXTENDED, (UINT32)tmp_reg, cardResponse, FALSE,0);
    MCD_TRACE(MCD_INFO_TRC, 0, "CMD 53, arg: 0x%x, err status: %d,  response: 0x%x",tmp_reg,errStatus,cardResponse[3]);
    sxs_Dump(MCD_INFO_TRC, 0,(U8 *)cardResponse, 16);

    if (errStatus != MCD_ERR_NO)
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "Set sd read had error");
        hal_SdioStopTransfer(&g_SdioTransfer);
        return MCD_ERR_CMD;
    }
    // Wait
    startTime = hal_TimGetUpTime();
    MCD_TRACE(MCD_INFO_TRC, 0, "read FIFO 3: 0x%x",hal_read_databyFIFO());

    while(!hal_SdioTransferDone())
    {
        if (hal_TimGetUpTime() - startTime >  (MCD_READ_TIMEOUT*blockNum*2*5))
        {
            MCD_TRACE(MCD_INFO_TRC, 0, "Read on Sdmmc timeout");
            // Abort the transfert.
            hal_SdioStopTransfer(&g_SdioTransfer);
            return MCD_ERR_CARD_TIMEOUT;
        }
    }
#else

    UINT32 tmp_reg =0x10000000;

    tmp_reg|=(sidodata_size|((UINT32)blockAddr)<<9);
    //tmp_reg|=blockAddr;
    errStatus = mcd_SdioSendCmd(HAL_SDIO_CMD_IO_RW_EXTENDED, (UINT32)tmp_reg, cardResponse, FALSE,0,0);
    MCD_TRACE(MCD_INFO_TRC, 0, "CMD 53, arg: 0x%x, err status: %d,  response: 0x%x",tmp_reg,errStatus,cardResponse[3]);
    sxs_Dump(MCD_INFO_TRC, 0,(U8 *)cardResponse, 16);
    U8 iii =0;
    UINT32 ret_val =0;
    // sxr_Sleep(10*16384/1000);
    while(!hal_SdioDataInd())
    {
        ;
    }
    for(iii=0; iii<4; iii++)
    {
        ret_val =hal_read_databyFIFO();
        MCD_TRACE(MCD_INFO_TRC, 0, "read FIFO: 0x%x",ret_val);

    }

#endif


    if (mcd_SdioReadCheckCrc() != MCD_ERR_NO)
    {
        MCD_TRACE(MCD_INFO_TRC, 0, "SDIO Read error");
        return MCD_ERR;
    }

    MCD_TRACE(MCD_INFO_TRC, 0, "################ test ");





//MCD_TRACE(MCD_INFO_TRC, 0, "read FIFO 4: 0x%x",hal_read_databyFIFO());





    hal_SdioReadFlushFifo();
    // Flush Cache
    hal_SysInvalidateCache(pRead, blockNum * sidodata_size);
// MCD_TRACE(MCD_INFO_TRC, 0, "read FIFO 5: 0x%x",hal_read_databyFIFO());
    return MCD_ERR_NO;



}
void mcd_trace_in_other(U8 lengthExp)
{
    MCD_TRACE(MCD_INFO_TRC, 0, "hal_SdmmcTransfer,  lengthExp: %d",lengthExp);

}


/***************************************************************/

s32 g_uMMCOpened=0;
MCD_CSD_T g_sCsdInfo;

#define MMC_TRYING_TIMES 3
void TS_DRV_OpenMMC(void)
{
    MCD_CSD_T mmc_csd;
    MCD_ERR_T result;
    UINT8 try_times = 0;

    //pszDevName = pszDevName;
//    wifi_setTransMode();

    if(g_uMMCOpened==0)
    {
        do
        {
            result = SDIO_mcd_Open(&mmc_csd);
            if(result == MCD_ERR_NO)
            {
                break;
            }
            sxr_Sleep(10);
            try_times++;
        }
        while((result == MCD_ERR_CARD_TIMEOUT ||
                result == MCD_ERR_CARD_TIMEOUT ||
                result == MCD_ERR_DMA_BUSY ||
                result == MCD_ERR_SPI_BUSY) &&
                (try_times < MMC_TRYING_TIMES));
        if(result != MCD_ERR_NO)
        {
            //      CSW_TRACE(BASE_FFS_TS_ID, " mcd_Open failed.Trying times:%d,error code:%d",try_times,result);
            SXS_TRACE(_MMI," mcd_Open failed. result: %d\n",result);

            if(result == MCD_ERR_CARD_TIMEOUT ||
                    result == MCD_ERR_CARD_TIMEOUT ||
                    result == MCD_ERR_DMA_BUSY     ||
                    result == MCD_ERR_SPI_BUSY)
            {
                return ;
            }
            else
            {
                return ;
            }
        }
        g_sCsdInfo = mmc_csd;
        g_uMMCOpened = 1;

    }
    return ;
}



/****************************************************************/

INT8 wifi_getRcvBuf(VOID)
{
    return 0;


    UINT8 i =0;
    UINT16 tmp_flag = tmp_read_data_flag ;
    while(((tmp_flag&0x01) !=0x00)&&(i<16))
    {
        tmp_flag = tmp_flag>>1;
        i++;
    }
    if(i<16)
    {
        tmp_read_data_flag=tmp_read_data_flag|(0x01<<i);
        MCD_TRACE(MCD_INFO_TRC, 0, "wifi_getRcvBuf, index:%d  , tmp_read_data_flag: 0x%x",i,tmp_read_data_flag);

        return i;
    }
    else
        return -1;
}
VOID wifi_ClrRcvBufflag(UINT8 bufIndex)
{
    tmp_read_data_flag = tmp_read_data_flag & (~(0x01<<bufIndex));
    MCD_TRACE(MCD_INFO_TRC, 0, "wifi_ClrRcvBufflag,  tmp_read_data_flag: 0x%x",tmp_read_data_flag);

}



struct s_rda5890_wid rda5890_wid_data, *rda5890_wid;
BOOL protect_test =FALSE;
BOOL rda5890_connected = FALSE;
#define RDA5890_DBGPRINT MCD_TRACE
#define RDA5890_ERRPRINT2 MCD_TRACE
#define RDA5890_ERRPRINT MCD_TRACE

#define RDA5890_DA_WID MCD_INFO_TRC
#define RDA5890_DL_NORM 0
#define RDA5890_DL_CRIT 0

//#define RDA5890_MAX_WID_LEN (256-2)

UINT8 w802_11imode;
UINT32 total_timer =0;
UINT32 useful_timer =0;
extern UINT8 tmp_buf[512];


VOID clear_wid_pending(VOID)
{
    rda5890_wid->wid_pending = 0;
}
VOID rda5890_sendRxIntToDev(VOID)
{

    if(wifi_version_flag== WIFI_VERSION_D)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,  "rda5890_sendRxIntToDev...\n");


        /*
             wifi_Read_MAC_Reg_I2C(0x00100448);

                  wifi_Read_MAC_Reg_I2C(0x50300014);
                 //  wifi_Read_MAC_Reg_I2C(0x50300028);
           wifi_Read_MAC_Reg_I2C(0x50300038);
            //      wifi_Read_MAC_Reg_I2C(0x50300028);
            //  UINT32 tmp_addr = 0x00000000;

            //             tmp_addr = 0x00000000;
          //         tmp_addr = tmp_addr |(1<<28) |( 0x05 <<9);
            //       read_test_cmd(52,tmp_addr);
           wifi_Read_MAC_Reg_I2C(0x1025b4);
               wifi_Read_MAC_Reg_I2C(0x105dbc);
               */
        UINT32  tmp_addr = 0x88000000;
        tmp_addr = tmp_addr |(1<<28) |( 0x05 <<9)|0x20;
        send_test_cmd(52,tmp_addr);
        /*
               sxr_Sleep(1638);
        //   wifi_Read_MAC_Reg_I2C(0x50300038);
          wifi_Read_MAC_Reg_I2C(0x00100448);
                  //  wifi_Read_MAC_Reg_I2C(0x50300028);
            //    wifi_Read_MAC_Reg_I2C(0x5030000c);
            wifi_Read_MAC_Reg_I2C(0x1025b4);
                   wifi_Read_MAC_Reg_I2C(0x105dbc);

             wifi_Read_MAC_Reg_I2C(0x50300038);
        wifi_Read_MAC_Reg_I2C(0x20000018);
        */
    }
}
UINT32 tmp_wifi_enableSleepTimerCount = 0;

#ifdef USB_WIFI_SUPPORT
extern void sdio_send_host_test(UINT16 txlength,UINT8 txbufIdx);
extern void sdio_usbrecv();
extern UINT8 usb_transmit_end_flag;
extern UINT8 hostdata_type;
#endif


UINT8  test_Check_Sdio_DataIn(UINT8 func)
{
    MCD_TRACE(MCD_INFO_TRC, 0, "test_Check_Sdio_DataIn , begin");

//#ifndef USE_WIFI_SLEEP
    tmp_wifi_enableSleepTimerCount = wifi_enableSleepTimerCount;
    if(!gettingRssi)
//#endif
        wifi_enableSleepTimerCount = 0;


    UINT8 val_rsp = 0;
    UINT32 tmp_addr = 0x00000000;
    UINT8 ret = 0x00;

    MCD_CS_ENTER    ;

    // sxr_Sleep(200*16384/1000);
    tmp_addr = 0x00000000;
    tmp_addr = tmp_addr |(1<<28) |( 0x06 <<9);
    ret = read_test_cmd(52,tmp_addr);
    val_rsp = ret & 0x01;
    if(val_rsp == 0x00)
    {
        MCD_CS_EXIT;
        //MCD_TRACE(MCD_INFO_TRC, 0, "test_Check_Sdio_DataIn, no data , end");

        hal_sdioDataInIrqMaskSet();
        return -1;
    }


    /*
        if(wifi_sleep_flag)
        {
            rda5890_WifWakeUp( );
            wifi_sleep_flag = FALSE;

        }
        */
    u16 datalen =0;

    datalen = sdio_GetReadDataLen(func);

    if(datalen>0)
    {
        INT8 ret =wifi_getRcvBuf();
        // MCD_TRACE(MCD_INFO_TRC, 0, "test_Check_Sdio_DataIn,  no buf. ");

        if(ret<0)
        {
            MCD_CS_EXIT;
            MCD_TRACE(MCD_INFO_TRC, 0, "test_Check_Sdio_DataIn,  no buf. ");
            // setWifiIrqMsk(0x37);

            hal_sdioDataInIrqMaskSet();
            in_interruptHandle = FALSE;
            return 0xFF;
        }
        UINT8 bufIdx =(UINT8) ret;
        //memset(tmp_read_data[bufIdx],0xff,1664);
        //MCD_TRACE(MCD_INFO_TRC, 0, "tmp_read_data: ");

        //    sxs_Dump(_MMI, 0, tmp_read_data, 80);

        if( 0==  SDIO_WifiDataRead((U8*)0x08,tmp_read_data[bufIdx],datalen))
        {
            rda5890_sendRxIntToDev();
            //  MCD_TRACE(MCD_INFO_TRC, 0, "test_Check_Sdio_DataIn,  datalen: %d, test_numn: 0x%x",datalen,test_numn);
            MCD_CS_EXIT
            ;
            hal_sdioDataInIrqMaskSet();

            in_interruptHandle = FALSE;
            sxs_Dump(_MMI, 0, tmp_read_data[bufIdx], datalen);
            //sxs_Dump(_MMI, 0, tmp_read_data[bufIdx], datalen);
#ifdef USB_WIFI_SUPPORT
            if((usb_transmit_end_flag == 0xFF)||(usb_transmit_end_flag == 1))
            {

                usb_transmit_end_flag = 0;
                sdio_send_host_test(datalen,bufIdx);

                if(hostdata_type & 0x40)
                {
                    sdio_usbrecv();
                }
            }
            else
            {
                while(1)
                {
                    hal_HstSendEvent(0x11118888);
                    if(usb_transmit_end_flag == 1)
                        break;
                }

                usb_transmit_end_flag = 0;
                sdio_send_host_test(datalen,bufIdx);

                if(hostdata_type & 0x40)
                {
                    sdio_usbrecv();
                }
            }

#else
            test_rda5890_card_to_host(tmp_read_data[bufIdx],datalen);
            //                       ret = SDIO_WifiDataWrite(0x07,tmp_buf,64);
#endif
            wifi_ClrRcvBufflag(bufIdx);
            //wifi_SdioSleep();
            // sxr_Sleep(200);

            //test_rda5890_SetWifiSleepEx();


        }
        else
        {
            sxs_Dump(_MMI, 0, tmp_read_data[bufIdx], datalen);
            wifi_ClrRcvBufflag(bufIdx);


            MCD_CS_EXIT
            ;
            hal_sdioDataInIrqMaskSet();
            in_interruptHandle = FALSE;

        }

    }
    else
    {

        MCD_CS_EXIT
        ;

        hal_sdioDataInIrqMaskSet();
        in_interruptHandle = FALSE;


    }
    MCD_TRACE(MCD_INFO_TRC, 0, "test_Check_Sdio_DataIn,  end. ");

    return 0;
}

UINT8  test_check_recv_Sdio_Data(UINT8 func)
{
    //MCD_TRACE(MCD_INFO_TRC, 0, "test_check_recv_Sdio_Data , begin");

//#ifndef USE_WIFI_SLEEP
    tmp_wifi_enableSleepTimerCount = wifi_enableSleepTimerCount;
    if(!gettingRssi)
//#endif
        wifi_enableSleepTimerCount = 0;


    UINT8 val_rsp = 0;
    UINT32 tmp_addr = 0x00000000;
    UINT8 ret = 0x00;

    MCD_CS_ENTER    ;

    // sxr_Sleep(200*16384/1000);
    tmp_addr = 0x00000000;
    tmp_addr = tmp_addr |(1<<28) |( 0x06 <<9);
    ret = read_test_cmd_notrace(52,tmp_addr);
    val_rsp = ret & 0x01;
    if(val_rsp == 0x00)
    {
        MCD_CS_EXIT;
        //MCD_TRACE(MCD_INFO_TRC, 0, "test_Check_Sdio_DataIn, no data , end");

        //hal_sdioDataInIrqMaskSet();
        return -1;
    }

    MCD_TRACE(MCD_INFO_TRC, 0, "test_check_recv_Sdio_Data , begin");

    /*
        if(wifi_sleep_flag)
        {
            rda5890_WifWakeUp( );
            wifi_sleep_flag = FALSE;

        }
        */
    u16 datalen =0;

    datalen = sdio_GetReadDataLen(func);

    if(datalen>0)
    {
        INT8 ret =wifi_getRcvBuf();
        // MCD_TRACE(MCD_INFO_TRC, 0, "test_Check_Sdio_DataIn,  no buf. ");

        if(ret<0)
        {
            MCD_CS_EXIT;
            MCD_TRACE(MCD_INFO_TRC, 0, "test_Check_Sdio_DataIn,  no buf. ");
            // setWifiIrqMsk(0x37);

            //hal_sdioDataInIrqMaskSet();
            //in_interruptHandle = FALSE;
            return 0xFF;
        }
        UINT8 bufIdx =(UINT8) ret;
        //memset(tmp_read_data[bufIdx],0xff,1664);
        //MCD_TRACE(MCD_INFO_TRC, 0, "tmp_read_data: ");

        //    sxs_Dump(_MMI, 0, tmp_read_data, 80);

        if( 0==  SDIO_WifiDataRead((U8*)0x08,tmp_read_data[bufIdx],datalen))
        {
            rda5890_sendRxIntToDev();
            //  MCD_TRACE(MCD_INFO_TRC, 0, "test_Check_Sdio_DataIn,  datalen: %d, test_numn: 0x%x",datalen,test_numn);
            MCD_CS_EXIT
            ;
            //hal_sdioDataInIrqMaskSet();

            //in_interruptHandle = FALSE;
            sxs_Dump(_MMI, 0, tmp_read_data[bufIdx], datalen);
            //sxs_Dump(_MMI, 0, tmp_read_data[bufIdx], datalen);
//#ifdef USB_WIFI_SUPPORT
#ifdef USB_WIFI_SUPPORT
            if((usb_transmit_end_flag == 0xFF)||(usb_transmit_end_flag == 1))
            {

                usb_transmit_end_flag = 0;
                sdio_send_host_test(datalen,bufIdx);

                if(hostdata_type & 0x40)
                {
                    sdio_usbrecv();
                }
            }
            else
            {
                while(1)
                {
                    sxr_Sleep(200*16384/1000);
                    hal_HstSendEvent(0x11118888);
                    if(usb_transmit_end_flag == 1)
                        break;
                }

                usb_transmit_end_flag = 0;
                sdio_send_host_test(datalen,bufIdx);

                if(hostdata_type & 0x40)
                {
                    sdio_usbrecv();
                }
            }

#else
            test_rda5890_card_to_host(tmp_read_data[bufIdx],datalen);
            //                       ret = SDIO_WifiDataWrite(0x07,tmp_buf,64);
#endif
            wifi_ClrRcvBufflag(bufIdx);
            //wifi_SdioSleep();
            // sxr_Sleep(200);

            //test_rda5890_SetWifiSleepEx();


        }
        else
        {
            sxs_Dump(_MMI, 0, tmp_read_data[bufIdx], datalen);
            wifi_ClrRcvBufflag(bufIdx);


            MCD_CS_EXIT
            ;
            //  hal_sdioDataInIrqMaskSet();
            //  in_interruptHandle = FALSE;

        }

    }
    else
    {

        MCD_CS_EXIT
        ;

        //  hal_sdioDataInIrqMaskSet();
        //  in_interruptHandle = FALSE;


    }
    MCD_TRACE(MCD_INFO_TRC, 0, "test_Check_Sdio_DataIn,  end. ");

    return 0;
}
extern BOOL g_wifi26mClockReq;
/*
VOID test_wifiSleepInd(VOID)
{
        wifi_sleep_flag  = TRUE;
                 UINT32 tmp_addr = 0x88000000;
                    tmp_addr = tmp_addr |(1<<28) |( 0x05<<9)|0x10;
                   send_test_cmd(52,tmp_addr);
            if(g_wifi26mClockReq ==TRUE)
            {
                hal_SysAuxClkOut(FALSE);
                g_wifi26mClockReq = FALSE;
            }
            wifi_SdioSleep();
            hal_sdioDataInIrqMaskSet();

}
VOID test_wifiWakeUpInd(VOID)
{
}
*/

VOID Mac_WifiDataInd(UINT8 intflag);
//VOID hal_SdioirqHandler(UINT8 intnum);
VOID wifi_IrqHandler(VOID)
{
    hal_sdioDataInIrqMaskClear();
    if(sdio_sleep_flag)
    {
        rda5890_WifWakeUpEx();
        //wifi_sleep_flag = FALSE;
    }
    test_numn++;
#ifdef __USE_GPIO__
    if(1)
#else
    if(hal_SdioDataInd())

#endif
    {
        in_interruptHandle = TRUE;

#if 0
        UINT32 tmp_addr = 0x00000000;
        UINT32 ret = 0x00;
        //  UINT32 tmp_data = 0xF0000000 + test_numn;
        //  hal_HstSendEvent(tmp_data);
        tmp_addr = 0x00000000;
        tmp_addr = tmp_addr |(1<<28) |( 0x06 <<9);
        ret = read_test_cmd(52,tmp_addr);
        if(ret ==0x00)
        {
            hal_sdioDataInIrqMaskSet();
            return;
        }

        //tmp_data = 0x88880000 + (ret&0xFF);
        //hal_HstSendEvent(tmp_data);

        //if((ret & 0x01) == 0x01)
#endif
        {
            //UINT32 tmp_data = 0xF0000000 + test_numn;
            //hal_HstSendEvent(tmp_data);

            Mac_WifiDataInd(0);
        }

    }
    else
    {
        hal_sdioDataInIrqMaskSet();
    }
    return;
}

extern BOOL wifiattached ;
UINT32 wifi_DisconnectInd(void);
//VOID receive_wifiDisConnected (VOID);
extern BOOL connect_ind_sent;
VOID ip_WifiDisconnectedInd(void);
UINT32 wifi_poweronContinue(void);
void test_rda5890_wid_status(
    UINT8 *wid_status, unsigned short wid_status_len)
{
    UINT8 mac_status;

    MCD_TRACE(MCD_INFO_TRC, 0, "rda5890_wid_status >>>\n");

    mac_status = wid_status[7];
    if (mac_status == MAC_CONNECTED)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,  "MAC CONNECTED\n");
        rda5890_connected = TRUE;
        MCD_TRACE(MCD_INFO_TRC, 0, "w802_11imode: 0x%x",w802_11imode);

        if(0x00 ==( w802_11imode&0xF8))// no encryption
        {
            ip_WifiConnectedInd();
        }
//      start_wifi_poll_timer();
        stop_assoc_ap_timer();
    }
    else if (mac_status == MAC_DISCONNECTED)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,  "MAC DISCONNECTED,  ap_connected: %d; ap_disconnecting: %d\n",ap_connected ,ap_disconnecting);

        ip_WifiDisconnectedInd();
        rda5890_connected =FALSE;
        connect_ind_sent =FALSE;
        clear_wid_pending();
        stop_assoc_ap_timer();
        //stop_wifi_poll_timer();
        if(ap_connected ||ap_disconnecting)
        {

            if(ap_connected)
                ap_connected = FALSE;
            if(ap_disconnecting)
            {
                ap_disconnecting = FALSE;
                test_rda5890_start_scan();
                sxr_Sleep(10000);
            }
            wifi_DisconnectInd();

        }
        if(test_numn ==1)
        {
            wifi_poweronContinue();
        }
    }
    else
    {
        //RDA5890_ERRPRINT1("Invalid MAC Status 0x%02x\n", mac_status);
    }

    MCD_TRACE(MCD_INFO_TRC, 0,  "rda5890_wid_status <<<\n");
}
void check_scan_resultEx(void);
BOOL get_scan_result_continue =FALSE;
int wifi_get_str_from_MAC(unsigned short wid);
void test_rda5890_wid_response(UINT8 *wid_rsp, unsigned short wid_rsp_len)
{
    MCD_TRACE(MCD_INFO_TRC, 0, "test_rda5890_wid_response, rda5890_wid->wid:   0x%x, rda5890_wid->wid_pending: 0x%x",rda5890_wid->wid,rda5890_wid->wid_pending);

    if (!rda5890_wid->wid_pending)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"no wid pending\n");
        return;
    }
    MCD_TRACE(MCD_INFO_TRC, 0, "test_rda5890_wid_response, rda5890_wid->wid_rsp_len:   0x%x, wid_rsp_len: 0x%x",rda5890_wid->wid_rsp_len,wid_rsp_len);

    if (wid_rsp_len > rda5890_wid->wid_rsp_len)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"not enough space for wid response, size = %d, buf = %d\n",wid_rsp_len, rda5890_wid->wid_rsp_len);
        //  kal_set_eg_events(rda5890_events,EVENT_WID_DONE,KAL_OR);
        return;
    }
    //   sxs_Dump(_MMI, 0, wid_rsp, wid_rsp_len);

    memcpy(rda5890_wid->wid_rsp, wid_rsp, wid_rsp_len);

    rda5890_wid->wid_rsp_len = wid_rsp_len;
    if(rda5890_wid->wid == WID_START_SCAN_REQ )
    {
        rda5890_wid->wid_pending = 0;
        rda5890_wid->wid_msg_id ++;

    }
    else if(rda5890_wid->wid == WID_SITE_SURVEY_RESULTS )
    {

        check_scan_resultEx();

        rda5890_wid->wid_pending = 0;
        rda5890_wid->wid_msg_id ++;
        if(get_scan_result_continue)
        {
            get_scan_result_continue = FALSE;

            wifi_get_str_from_MAC(WID_SITE_SURVEY_RESULTS);
        }


    }
    else if(rda5890_wid->wid == WID_RSSI )
    {
        check_getRssi_result();
        gettingRssi = FALSE;

        rda5890_wid->wid_pending = 0;
        rda5890_wid->wid_msg_id ++;


    }
    else
    {


        rda5890_wid->wid_pending = 0;
        rda5890_wid->wid_msg_id ++;

    }
    //rda5890_wid->wid_pending = 0;
    sxs_Dump(_MMI, 0, rda5890_wid->wid_rsp, rda5890_wid->wid_rsp_len);

    //kal_set_eg_events(rda5890_events,EVENT_WID_DONE,KAL_OR);

    //RDA5890_DBGPRINT(RDA5890_DA_WID, RDA5890_DL_NORM,
    //  "rda5890_wid_response <<<\n");
    //         sxs_Dump(_MMI, 0, rda5890_wid->wid_rsp, rda5890_wid->wid_rsp_len);
    //if(protect_test ==TRUE)
    {
        //      hal_DbgPageProtectDisable(4);
        ///
        //   hal_DbgPageProtectSetup(4, 2, (UINT32)rda5890_wid->wid_rsp, (UINT32)rda5890_wid->wid_rsp+ rda5890_wid->wid_rsp_len);
        //       hal_DbgPageProtectEnable(4);
    }


}

int test_rda5890_check_wid_response(UINT8*wid_rsp, unsigned short wid_rsp_len,
                                    unsigned short wid, UINT8 wid_msg_id,
                                    UINT8* payload_len, UINT8 **ptr_payload)
{
    unsigned short rsp_len;
    unsigned short rsp_wid;
    unsigned char msg_len;
    MCD_TRACE(MCD_INFO_TRC, 0, "test_rda5890_check_wid_response");
    //  sxs_Dump(MCD_INFO_TRC, 0,(U8 *)wid_rsp, wid_rsp_len);

    if (wid_rsp[0] != 'R')
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"wid_rsp[0] != 'R'\n");
        return -1;
    }

    if (wid_rsp[1] != wid_msg_id)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"wid_msg_id not match,wid_rsp[1]:0x%x;  wid_msg_id: 0x%x\n",wid_rsp[1] ,wid_msg_id);
        return -1;
    }

    if (wid_rsp_len < 4)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"wid_rsp_len < 4\n");
        return -1;
    }

    rsp_len = wid_rsp[2] | (wid_rsp[3] << 8);
    if (wid_rsp_len != rsp_len)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"wid_rsp_len not match, %d != %d\n", wid_rsp_len, rsp_len);
        return -1;
    }

    if (wid_rsp_len < 7)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"wid_rsp_len < 7\n");
        return -1;
    }

    rsp_wid = wid_rsp[4] | (wid_rsp[5] << 8);
    if (wid != rsp_wid)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"wid not match, 0x%04x != 0x%04x\n", wid, rsp_wid);
        return -1;
    }

    msg_len = wid_rsp[6];
    if (wid_rsp_len != msg_len + 7)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"msg_len not match, %d + 7 != %d\n", msg_len, wid_rsp_len);
        return -1;
    }
    *payload_len=msg_len;
    //if (payload_len < msg_len) {
    //  RDA5890_ERRPRINT2("payload_len not match, %d  != %d\n", msg_len, payload_len);
    //  return -1;
    //}

    *ptr_payload = wid_rsp + 7;

    return 0;
}
int test_rda5890_check_wid_status(UINT8 *wid_rsp, unsigned short wid_rsp_len,
                                  UINT8 wid_msg_id)
{
    int ret;
    unsigned short wid = WID_STATUS;
    UINT8 *ptr_payload;
    UINT8 status_val;
    UINT8 payload_len;

    ret = test_rda5890_check_wid_response(wid_rsp, wid_rsp_len, wid, wid_msg_id-1,
                                          &payload_len, &ptr_payload);
    if (ret)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"rda5890_check_wid_status, check_wid_response fail\n");
        return ret;
    }

    status_val = ptr_payload[0];
    if (status_val != WID_STATUS_SUCCESS)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"check_wid_status NOT success, status = %d\n", status_val);
        return -1;
    }

    return 0;
}
UINT8 defaulGwMac[6]= {0x00,};
UINT8 rda5890_tx_data_buf[1660] = { 0x00};
void getdefaulGwMac(UINT8* Mac)
{
    memcpy(Mac,defaulGwMac,6);
}

void setdefaulGwMac(UINT8* Mac)
{
    memcpy(defaulGwMac,Mac,6);
}

INT  test_rda5890_SetWifiSleepEx(VOID);
int test_rda5890_data_txEx(UINT8 *data, unsigned short len)
{
}

int test_rda5890_data_tx(UINT8 *data, unsigned short len)
{
//    BOOL result;
    int ret = 0;
    //char data_buf[1660];
    unsigned short data_len;
    //UINT32 flags;

    memset(rda5890_tx_data_buf,0x00,sizeof(rda5890_tx_data_buf));
    data_len = len + 2;
    rda5890_tx_data_buf[0] = (UINT8)(data_len&0xFF);
    rda5890_tx_data_buf[1] = (UINT8)((data_len>>8)&0x0F);
    rda5890_tx_data_buf[1] |= 0x10;  // for DataOut 0x1
    memcpy(rda5890_tx_data_buf + 2, data, len);
    MCD_TRACE(MCD_INFO_TRC, 0,  "test_rda5890_data_tx >>>, len = %d\n", data_len);
    //UINT16 ii =0 ;
    //for(ii=0; ii<sizeof(rda5890_tx_data_buf)-data_len;ii++)
    //  rda5890_tx_data_buf[data_len+ii] = ii;
//  memset(rda5890_tx_data_buf+data_len, 0xFE, sizeof(rda5890_tx_data_buf)-data_len);
    sxs_Dump(_MMI, 0, rda5890_tx_data_buf, data_len);

    /* no_lock workaround */
    //kal_take_mutex(rda5890_wid_mutex);
    //hal_sdioDataInIrqMaskClear();
    ret = SDIO_WifiDataWrite(0x07,rda5890_tx_data_buf,(UINT16)data_len);
    //  hal_sdioDataInIrqMaskSet();
    //result = rda5890_sdio_data_write(rda5890_tx_data_buf, (UINT16)data_len);
    if (ret )
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"host_to_card send failed, ret = %d\n", ret);
        ret = -1;

    }

    /* no_lock workaround */
    //kal_give_mutex(rda5890_wid_mutex);

    MCD_TRACE(MCD_INFO_TRC, 0,  "test_rda5890_data_tx <<<\n");
    //         sxr_Sleep(200);

//test_rda5890_SetWifiSleepEx();
    return ret;
}
UINT32 startTime_ex = 0 ;
#ifdef __SCAN_INFO__
void rda5890_network_information(char *info, unsigned short info_len);
#endif
void test_rda5890_card_to_host(
    UINT8 *packet, unsigned short packet_len)
{
    unsigned char rx_type;
    unsigned short rx_length;
    unsigned char msg_type;

    RDA5890_DBGPRINT(RDA5890_DA_WID, RDA5890_DL_NORM,
                     "rda5890_card_to_host >>>\n");
//              sxs_Dump(_MMI, 0, packet, packet_len);

    rx_type = (unsigned char)packet[1]&0xF0;
    rx_length = (unsigned short)(packet[0] + ((packet[1]&0x0f) << 8));
    MCD_TRACE(MCD_INFO_TRC, 0, "test_rda5890_card_to_host,  rx_type: 0x%x,rx_length: 0x%x, payload_len: 0x%x",rx_type,rx_length,packet_len);

    if (rx_length > packet_len)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"packet_len %d less than header specified length %d\n",   packet_len, rx_length);
        goto out;
    }

    if( rx_type == 0x30 )
    {
//#ifndef USE_WIFI_SLEEP
        //   if(!gettingRssi)
//#endif
        //  wifi_enableSleepTimerCount = 0;
        RDA5890_DBGPRINT(RDA5890_DA_WID, RDA5890_DL_NORM,
                         "Message Packet, startTime_ex: %d s\n", (startTime_ex/16384) );
        msg_type = packet[2];
        if(msg_type == 'I')
        {
            RDA5890_DBGPRINT(RDA5890_DA_WID, RDA5890_DL_NORM,
                             "Indication Message\n");
            test_rda5890_wid_status(packet + 2, rx_length - 2);
        }
        else if (msg_type == 'R')
        {
            RDA5890_DBGPRINT(RDA5890_DA_WID, RDA5890_DL_NORM,
                             "WID Message\n");
            test_rda5890_wid_response(packet + 2, rx_length - 2);
        }
        else if (msg_type == 'N')
        {
            RDA5890_DBGPRINT(RDA5890_DA_WID, RDA5890_DL_NORM,
                             "Network Message\n");
#ifdef  __SCAN_INFO__
            rda5890_network_information(packet + 2,  rx_length - 2);
#endif
        }
        else
        {
            //  RDA5890_ERRPRINT1("Invalid Message Type '%c'\n", msg_type);
        }
    }
    else if(rx_type == 0x20)
    {
        //RDA5890_DBGPRINT1(RDA5890_DA_WID, RDA5890_DL_TRACE,
        //  "Data Packet, len = %d\n", rx_length - 2);
        test_rda5890_data_rx(packet + 2, rx_length - 2);
    }
    else
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"Invalid Packet Type 0x%x\n", rx_type);
    }

out:
    //      MCD_TRACE(MCD_INFO_TRC, 0,"rda5890_card_to_host <<<\n");

//  RDA5890_DBGPRINT(RDA5890_DA_WID, RDA5890_DL_NORM,
    //  "rda5890_card_to_host <<<\n");
    //      sxs_Dump(_MMI, 0, rda5890_wid->wid_rsp, rda5890_wid->wid_rsp_len);

    return;
}
void test_rda5890_wid_init(void)
{

    rda5890_wid = &rda5890_wid_data;
    memset(rda5890_wid, 0, sizeof(struct s_rda5890_wid));

}
#if 0
int test_rda5890_wid_request(UINT8 *wid_req, unsigned short wid_req_len,UINT8 *wid_rsp, unsigned short *wid_rsp_len)
{
    //   unsigned char result;
    int ret = 0;
    UINT8 data_buf[RDA5890_MAX_WID_LEN + 2];
    unsigned short data_len;
//  unsigned int  flags;

    //RDA5890_DBGPRINT(RDA5890_DA_WID, RDA5890_DL_NORM,
    //  "rda5890_wid_request >>>\n");

    /* no_lock workaround */
    //kal_take_mutex(rda5890_wid_mutex);
    if( rda5890_wid->wid_pending == 1)
        return -1;
    //rda5890_wid->wid_msg_id +;
    memset(data_buf,0x00,sizeof(data_buf));
    rda5890_wid->wid_rsp = wid_rsp;
    rda5890_wid->wid_rsp_len = *wid_rsp_len;
    rda5890_wid->wid_pending = 1;

    data_len = wid_req_len + 2;
    data_buf[0] = (UINT8)(data_len&0xFF);
    data_buf[1] = (UINT8)((data_len>>8)&0x0F);
    data_buf[1] |= 0x40;  // for Request(Q/W) 0x4
    memcpy(data_buf + 2, wid_req, wid_req_len);
    ret = SDIO_WifiDataWrite(0x07, data_buf,data_len);
//  result = rda5890_sdio_data_write((kal_uint8 *)data_buf, (kal_uint16)data_len);
//  if (result != KAL_TRUE) {
//      RDA5890_ERRPRINT1("host_to_card send failed, ret = %d\n", ret);
//      rda5890_wid->wid_pending = 0;
//        ret = -1;
//      goto out;
//  }

//        kal_retrieve_eg_events(rda5890_events,EVENT_WID_DONE,
//                KAL_AND_CONSUME,&flags,KAL_SUSPEND);

//  *wid_rsp_len = rda5890_wid->wid_rsp_len;

//out:
    /* no_lock workaround */
    //kal_give_mutex(rda5890_wid_mutex);

//  RDA5890_DBGPRINT(RDA5890_DA_WID, RDA5890_DL_NORM,
    //      "rda5890_wid_request <<<\n");

    return ret;
}
#endif
int rda5990_wid_request(UINT8 *wid_req, unsigned short wid_req_len)
{
    //   unsigned char result;
    int ret = 0;
    UINT8 data_buf[RDA5890_MAX_WID_LEN + 2];
    unsigned short data_len;
    //  unsigned int  flags;

    //RDA5890_DBGPRINT(RDA5890_DA_WID, RDA5890_DL_NORM,
    //  "rda5890_wid_request >>>\n");

    /* no_lock workaround */
    //kal_take_mutex(rda5890_wid_mutex);
    if(rda5890_wid->wid_pending == 1)
    {
        UINT8 i =0;
        while((rda5890_wid->wid_pending == 1)&&(i<30))
        {
            sxr_Sleep(1638);
            i++;
        }
        //if(i==30)
        //  break;
        //  return -1;
    }
    //rda5890_wid->wid_msg_id +;
    memset(data_buf,0x00,sizeof(data_buf));
    rda5890_wid->wid_pending = 1;

    data_len = wid_req_len + 2;
    data_buf[0] = (UINT8)(data_len&0xFF);
    data_buf[1] = (UINT8)((data_len>>8)&0x0F);
    data_buf[1] |= 0x40;  // for Request(Q/W) 0x4
    memcpy(data_buf + 2, wid_req, wid_req_len);

    ret = SDIO_WifiDataWrite(0x07, data_buf,data_len);
//  result = rda5890_sdio_data_write((kal_uint8 *)data_buf, (kal_uint16)data_len);
//  if (result != KAL_TRUE) {
//      RDA5890_ERRPRINT1("host_to_card send failed, ret = %d\n", ret);
//      rda5890_wid->wid_pending = 0;
//        ret = -1;
//      goto out;
//  }

//        kal_retrieve_eg_events(rda5890_events,EVENT_WID_DONE,
//                KAL_AND_CONSUME,&flags,KAL_SUSPEND);

//  *wid_rsp_len = rda5890_wid->wid_rsp_len;

//out:
    /* no_lock workaround */
    //kal_give_mutex(rda5890_wid_mutex);

//  RDA5890_DBGPRINT(RDA5890_DA_WID, RDA5890_DL_NORM,
    //      "rda5890_wid_request <<<\n");

    return ret;
}
extern BOOL readbyfifo;

int sdio_getInputData(void)
{
    MCD_TRACE(MCD_INFO_TRC, 0, "sdio_getInputData enter. ");

    u16 datalen =0;
    UINT32 tmp_addr = 0x00000000;
    UINT8 val_rsp = 0;
    UINT8 count =0;
    UINT8 ret = 0x00;
    //      hal_SdioWakeUp();
    /*
             if(wifi_sleep_flag)

            rda5890_WifWakeUp( );
        while(wifi_sleep_flag)
            sxr_Sleep(20*16384/1000);

    */
    MCD_CS_ENTER;
    while(val_rsp==0x00)
    {
        sxr_Sleep(200*16384/1000);
        tmp_addr = 0x00000000;
        tmp_addr = tmp_addr |(1<<28) |( 0x06 <<9);
        ret = read_test_cmd(52,tmp_addr);
        val_rsp = ret & 0x01;
        if((count++)>50)
        {
            //    hal_SdioSleep();
            MCD_CS_EXIT;
            return -1;
        }
    }
    // MCD_CS_ENTER;
    tmp_addr = 0x88000000;
    tmp_addr = tmp_addr |(0x01<<28) |( 0x05 <<9)|val_rsp;
    ret = read_test_cmd(52,tmp_addr);

    datalen = sdio_GetReadDataLen(1);

    if(datalen>0)
    {
        INT8 retx =wifi_getRcvBuf();
        if(retx<0)
        {
            MCD_CS_EXIT;
            MCD_TRACE(MCD_INFO_TRC, 0, "test_Check_Sdio_DataIn,  no buf. ");
//    hal_SdioSleep();

            return -1;
        }
        UINT8 bufIdx =(UINT8) retx;

        memset(tmp_read_data[bufIdx],0x00,sizeof(tmp_read_data[bufIdx]));
//       MCD_TRACE(MCD_INFO_TRC, 0, "tmp_read_data:");

        //        sxs_Dump(_MMI, 0, tmp_read_data, 80);
        if( 0==  SDIO_WifiDataRead((U8*)0x08,tmp_read_data[bufIdx],datalen))
        {
            rda5890_sendRxIntToDev();
            MCD_CS_EXIT;
            MCD_TRACE(MCD_INFO_TRC, 0, "SDIO_WifiDataRead,  datalen: %d",datalen);
            sxs_Dump(_MMI, 0, tmp_read_data[bufIdx], datalen);
            test_rda5890_card_to_host(tmp_read_data[bufIdx],datalen);
            wifi_ClrRcvBufflag(bufIdx);
//    hal_SdioSleep();

        }

    }
    else
    {
        //  wifi_ClrRcvBufflag(bufIdx);
        //  hal_SdioSleep();

        MCD_CS_EXIT;
    }
    //   sxs_Dump(_MMI, 0, rda5890_wid->wid_rsp, rda5890_wid->wid_rsp_len);

    return 0;
}

int test_rda5890_generic_get_str(unsigned short wid, unsigned char *val, unsigned char *len)
{
    int ret =0x00;
    UINT8 wid_req[6];
    unsigned short wid_req_len = 6;
    UINT8 wid_msg_id = rda5890_wid->wid_msg_id;
    rda5890_wid->wid = wid;
    rda5890_wid->wid_rsp_len =RDA5890_MAX_WID_LEN;
    memset(rda5890_wid->wid_rsp,0x0,512);

    UINT8 *ptr_payload;
//hal_sdioDataInIrqMaskClear();
    wid_req[0] = 'Q';
    wid_req[1] = wid_msg_id;

    wid_req[2] = (UINT8)(wid_req_len&0x00FF);
    wid_req[3] = (UINT8)((wid_req_len&0xFF00) >> 8);

    wid_req[4] = (UINT8)(wid&0x00FF);
    wid_req[5] = (UINT8)((wid&0xFF00) >> 8);
    stop_wifiPollTimer();
    ret = rda5990_wid_request(wid_req, wid_req_len);
    if (ret)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"rda5890_wid_request fail, ret = %d\n", ret);
        goto out;
    }
    if(-1== sdio_getInputData())
    {
        restart_wifiPollTimer();
//hal_sdioDataInIrqMaskSet();
        return -1;
    }
    // UINT8 payload_len =0;
    sxs_Dump(_MMI, 0, rda5890_wid->wid_rsp, rda5890_wid->wid_rsp_len);
    //sxs_Dump(_MMI, 0, rda5890_wid->wid_rsp, rda5890_wid->wid_rsp_len);
    sxr_Sleep(200);

    ret = test_rda5890_check_wid_response(rda5890_wid->wid_rsp, rda5890_wid->wid_rsp_len, wid, wid_msg_id, len, &ptr_payload);
    if (ret)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"check_wid_response fail, ret = %d\n", ret);
        goto out;
    }
    MCD_TRACE(MCD_INFO_TRC, 0, "payload_len: %d",*len);
    //         sxs_Dump(_MMI, 0, ptr_payload, payload_len);

    memcpy(val, ptr_payload, *len);
    sxs_Dump(_MMI, 0, val, *len);

    //     MCD_TRACE(MCD_INFO_TRC, 0, "get wid: 0x%x,  val: %s",wid,val);

out:
    restart_wifiPollTimer();
//hal_sdioDataInIrqMaskSet();

    return ret;
}

int wifi_get_str_from_MAC(unsigned short wid)
{
    int ret =0x00;
    UINT8 wid_req[6];
    unsigned short wid_req_len = 6;
    //UINT8 wid_rsp[RDA5890_MAX_WID_LEN]= {0,};
    //unsigned short wid_rsp_len = RDA5890_MAX_WID_LEN;
    UINT8 wid_msg_id = rda5890_wid->wid_msg_id;
    rda5890_wid->wid = wid;
    rda5890_wid->wid_rsp_len =RDA5890_MAX_WID_LEN;
    memset(rda5890_wid->wid_rsp,0x0,512);
    UINT8 *ptr_payload;
//hal_sdioDataInIrqMaskClear();
    wid_req[0] = 'Q';
    wid_req[1] = wid_msg_id;

    wid_req[2] = (UINT8)(wid_req_len&0x00FF);
    wid_req[3] = (UINT8)((wid_req_len&0xFF00) >> 8);

    wid_req[4] = (UINT8)(wid&0x00FF);
    wid_req[5] = (UINT8)((wid&0xFF00) >> 8);
    stop_wifiPollTimer();
    ret = rda5990_wid_request(wid_req, wid_req_len);
    if (ret)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"rda5890_wid_request fail, ret = %d\n", ret);
        //goto out;
    }
    return ret;


}


int  test_rda5890_generic_set_str(unsigned short wid, unsigned char *val, unsigned char len)
{
    int ret;
    UINT8 wid_req[RDA5890_MAX_WID_LEN];
    unsigned short wid_req_len = 7 + len;
//  UINT8 wid_rsp[RDA5890_MAX_WID_LEN];
//  unsigned short wid_rsp_len = RDA5890_MAX_WID_LEN;
    UINT8 wid_msg_id = rda5890_wid->wid_msg_id;
    rda5890_wid->wid = wid;
    rda5890_wid->wid_rsp_len =RDA5890_MAX_WID_LEN;
    memset(rda5890_wid->wid_rsp,0x0,512);

    wid_req[0] = 'W';
    wid_req[1] = wid_msg_id;

    wid_req[2] = (UINT8)(wid_req_len&0x00FF);
    wid_req[3] = (UINT8)((wid_req_len&0xFF00) >> 8);

    wid_req[4] = (UINT8)(wid&0x00FF);
    wid_req[5] = (UINT8)((wid&0xFF00) >> 8);

    wid_req[6] = (UINT8)(len);
    memcpy(wid_req + 7, val, len);
    ret = rda5990_wid_request(wid_req, wid_req_len);

    //ret = test_rda5890_wid_request(wid_req, wid_req_len, wid_rsp, &wid_rsp_len);
    if (ret)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"rda5890_wid_request fail, ret = %d\n", ret);
        goto out;
    }

    //ret = rda5890_check_wid_status(wid_rsp, wid_rsp_len, wid_msg_id);
    if (ret)
    {
        //RDA5890_ERRPRINT1("check_wid_status fail, ret = %d\n", ret);
        goto out;
    }

out:
    return ret;
}

#ifdef __SCAN_INFO__
BOOL getting_network_info =FALSE;
INT  test_rda5890_set_NetworkInfo( UINT8 val)
{
    MCD_TRACE(MCD_INFO_TRC, 0,  "test_rda5890_set_NetworkInfo...\n");
    if(getting_network_info)
        test_rda5890_generic_set_uchar(WID_NETWORK_INFO_EN,val);
    if(val == 0)
        getting_network_info = FALSE;
    return 0;
}


extern UINT8 network_info_time;
int test_rda5890_start_scan_network_info(void)
{
    MCD_TRACE(MCD_INFO_TRC, 0,  "test_rda5890_start_scan_network_info, getting_network_info: %d \n",getting_network_info);
    hal_HstSendEvent(0x01130001);
    if(!getting_network_info)
        getting_network_info = TRUE;
    else
        return 0;


    network_info_time =0;
    int ret = 0;
    //UINT8 wid_req[12];
    UINT8 wid_req[60];

    unsigned short wid_req_len = 16;
    //UINT8 wid_rsp[32];
    //unsigned short wid_rsp_len = 32;
    unsigned short wid;
    UINT8 wid_msg_id = rda5890_wid->wid_msg_id;
    rda5890_wid->wid_rsp_len =RDA5890_MAX_WID_LEN;
    memset(rda5890_wid->wid_rsp,0x0,512);

//hal_sdioDataInIrqMaskClear();
    //MCD_TRACE(MCD_INFO_TRC, 0,    "rda5890_start_scan >>>\n");

    wid_req[0] = 'W';
    wid_req[1] = wid_msg_id;


    wid = WID_SITE_SURVEY;
    wid_req[4] = (UINT8)(wid&0x00FF);
    wid_req[5] = (UINT8)((wid&0xFF00) >> 8);

    wid_req[6] = (UINT8)(0x01);
    wid_req[7] = (UINT8)(0x01);

    wid = WID_START_SCAN_REQ;
    wid_req[8] = (UINT8)(wid&0x00FF);
    wid_req[9] = (UINT8)((wid&0xFF00) >> 8);

    wid_req[10] = (UINT8)(0x01);
    wid_req[11] = (UINT8)(0x01);


    wid = WID_NETWORK_INFO_EN;
    wid_req[12] = (char)(wid&0x00FF);
    wid_req[13] = (char)((wid&0xFF00) >> 8);

    wid_req[14] = (char)(0x01);
    wid_req[15] = (char)(0x01); // 0x01 scan network info


    rda5890_wid->wid = wid;



    wid_req_len = 16;

    wid_req[2] = (UINT8)(wid_req_len&0x00FF);
    wid_req[3] = (UINT8)((wid_req_len&0xFF00) >> 8);


    ret = rda5990_wid_request(wid_req, wid_req_len);
    if (ret)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"rda5890_wid_request fail, ret = %d\n", ret);

    }
    return ret;

    /*
     if(-1== sdio_getInputData())
    {
            restart_wifiPollTimer();
    //hal_sdioDataInIrqMaskSet();
            return -1;
    }
      wid_rsp_len = rda5890_wid->wid_rsp_len;
    ret = test_rda5890_check_wid_status(wid_rsp, wid_rsp_len, wid_msg_id+1);
    if (ret) {
        //MCD_TRACE(MCD_INFO_TRC, 0,"check_wid_status fail, ret = %d\n", ret);
        goto out;
    }

    //MCD_TRACE(MCD_INFO_TRC, 0,    "Start Scan Done\n");

    out:
    //MCD_TRACE(MCD_INFO_TRC, 0,    "rda5890_start_scan <<<\n");
            restart_wifiPollTimer();
    //hal_sdioDataInIrqMaskSet();

        useingIrq = TRUE;


    return ret;
    */
}
VOID stop_get_network_info_timer(VOID);
#endif
int test_rda5890_start_scan(void)
{


    int ret = 0;
    //UINT8 wid_req[12];
    UINT8 wid_req[60];

    unsigned short wid_req_len = 12;
    //UINT8 wid_rsp[32];
    //unsigned short wid_rsp_len = 32;
    unsigned short wid;
    UINT8 wid_msg_id = rda5890_wid->wid_msg_id;
    rda5890_wid->wid_rsp_len =RDA5890_MAX_WID_LEN;
    memset(rda5890_wid->wid_rsp,0x0,512);

//hal_sdioDataInIrqMaskClear();
    //MCD_TRACE(MCD_INFO_TRC, 0,    "rda5890_start_scan >>>\n");

    wid_req[0] = 'W';
    wid_req[1] = wid_msg_id;

    wid_req[2] = (UINT8)(wid_req_len&0x00FF);
    wid_req[3] = (UINT8)((wid_req_len&0xFF00) >> 8);

    wid = WID_SITE_SURVEY;
    wid_req[4] = (UINT8)(wid&0x00FF);
    wid_req[5] = (UINT8)((wid&0xFF00) >> 8);

    wid_req[6] = (UINT8)(0x01);
    wid_req[7] = (UINT8)(0x01);

    wid = WID_START_SCAN_REQ;
    wid_req[8] = (UINT8)(wid&0x00FF);
    wid_req[9] = (UINT8)((wid&0xFF00) >> 8);

    wid_req[10] = (UINT8)(0x01);
    wid_req[11] = (UINT8)(0x01);
    rda5890_wid->wid = wid;


    wid_req_len = 12;
    UINT8 ssid_len ;
    ssid_len = strlen( wifi_context.cur_ssid);

    if(ssid_len >0)
    {
        UINT8* pWid_req = wid_req + wid_req_len ;
        UINT8  i, addr_1, addr_2;
        i=0;
        ssid_len = strlen( wifi_context.cur_ssid);
        addr_1 = 0x7c;
        addr_2 = 0x81;

        while ( i < ssid_len )
        {
            if(addr_1!= 0xfc )
            {
                addr_1 +=4 ;
            }
            else
            {
                addr_1 = 0x00;
                addr_2 += 1;

            }
            wid = WID_MEMORY_ADDRESS;
            pWid_req[0] = (char)(wid&0x00FF);
            pWid_req[1] = (char)((wid&0xFF00) >> 8);
            pWid_req[2] = 4;
            pWid_req[3] = addr_1;
            pWid_req[4] = addr_2;
            pWid_req[5] = 0x10;
            pWid_req[6] = 0x00;
            wid_req_len += 7;
            pWid_req += 7;

            wid = WID_MEMORY_ACCESS_32BIT;
            pWid_req[0] = (char)(wid&0x00FF);
            pWid_req[1] = (char)((wid&0xFF00) >> 8);
            pWid_req[2] = 4;



            if(ssid_len >= (i + 4))
            {

                pWid_req[3] = wifi_context.cur_ssid[i];
                pWid_req[4] = wifi_context.cur_ssid[i+1];
                pWid_req[5] = wifi_context.cur_ssid[i+2];
                pWid_req[6] = wifi_context.cur_ssid[i+3];
            }
            else
            {
                UINT8 j =0;
                for(j =0; j<4; j++)
                {
                    if(wifi_context.cur_ssid[i+j] != 0x00)
                        pWid_req[3+j] = wifi_context.cur_ssid[i+j];
                    else
                        pWid_req[3+j] = 0x00;

                }
            }



            wid_req_len += 7;
            pWid_req += 7;

            i+=4;


        }
    }
    stop_wifiPollTimer();

    ret = rda5990_wid_request(wid_req, wid_req_len);
    if (ret)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"rda5890_wid_request fail, ret = %d\n", ret);

    }
    MCD_TRACE(MCD_INFO_TRC, 0,"rda5890_wid_request end\n");
    return ret;

    /*
     if(-1== sdio_getInputData())
    {
            restart_wifiPollTimer();
    //hal_sdioDataInIrqMaskSet();
            return -1;
    }
      wid_rsp_len = rda5890_wid->wid_rsp_len;
    ret = test_rda5890_check_wid_status(wid_rsp, wid_rsp_len, wid_msg_id+1);
    if (ret) {
        //MCD_TRACE(MCD_INFO_TRC, 0,"check_wid_status fail, ret = %d\n", ret);
        goto out;
    }

    //MCD_TRACE(MCD_INFO_TRC, 0,    "Start Scan Done\n");

    out:
    //MCD_TRACE(MCD_INFO_TRC, 0,    "rda5890_start_scan <<<\n");
            restart_wifiPollTimer();
    //hal_sdioDataInIrqMaskSet();

        useingIrq = TRUE;


    return ret;
    */
}


struct bss_descs_t
{
    unsigned char bss_descs[sizeof(RDA5890_BSS_DESC)*RDA5890_MAX_NETWORK_NUM ];
#ifdef __SCAN_INFO__
    unsigned char bss_info[sizeof(RDA5890_BSS_NETWORK_INFO)*RDA5890_MAX_NETWORK_NUM ];
#endif
    UINT8 count;
} bss_Rst;


#ifdef __SCAN_INFO__
RDA5890_BSS_NETWORK_INFO* get_free_bss_info_node(void)
{
    UINT8 i = 0;
    for(i=0; i<RDA5890_MAX_NETWORK_NUM; i++)
    {
        if(strlen(&bss_Rst.bss_info[sizeof(RDA5890_BSS_NETWORK_INFO)*i])==0)
            return &bss_Rst.bss_info[sizeof(RDA5890_BSS_NETWORK_INFO)*i];
        //hal_HstSendEvent(0x01130004);

    }
    if(i == RDA5890_MAX_NETWORK_NUM)
        return 0;
}

RDA5890_BSS_NETWORK_INFO* find_bss_info_node_Mac(UINT8 * mac)
{
    RDA5890_BSS_NETWORK_INFO * tmp = bss_Rst.bss_info;

    UINT8 num =0;
    UINT8 i =0;
    for(i=0; i<RDA5890_MAX_NETWORK_NUM; i++)
    {
        if(memcmp(tmp->bssid,mac,6)==0)
        {
            MCD_TRACE(MCD_INFO_TRC, 0,  "find_bss_info_node_Mac , found network info node idex: %d\n", i);

            return tmp;
        }
        //      hal_HstSendEvent(0x01130003);

        tmp++;
    }
    if(i==RDA5890_MAX_NETWORK_NUM)
        return 0;
}
void show_bss_info_node(void)
{
    RDA5890_BSS_NETWORK_INFO * tmp = bss_Rst.bss_info;
    UINT8 num =0;
    UINT8 i =0;
    for(i=0; i<RDA5890_MAX_NETWORK_NUM; i++)
    {
        if(strlen(tmp->ssid)!=0)
        {
            MCD_TRACE(MCD_INFO_TRC, 0,"#### node NO. %d: \n", num );
            showString((UINT8*)tmp->ssid);
            sxs_Dump(_MMI, 0, tmp->bssid, 6);
            num++;

            sxr_Sleep(163);
        }
        tmp++;
    }
    MCD_TRACE(MCD_INFO_TRC, 0,"total node : %d.\n", num);

}
#endif
#if 0
int test_rda5890_get_scan_results(RDA5890_BSS_DESC *bss_desc,UINT8 * num)
{


    int ret = 0;
    int count;
    unsigned char len;
    unsigned char buf[sizeof(RDA5890_BSS_DESC)*RDA5890_MAX_NETWORK_NUM + 2];

    MCD_TRACE(MCD_INFO_TRC, 0,  "rda5890_get_scan_results >>>\n");

    //  ret = test_rda5890_generic_get_str(WID_SITE_SURVEY_RESULTS,buf, &len);
    ret = wifi_get_str_from_MAC(WID_SITE_SURVEY_RESULTS);
    if (ret)
    {
        return ret;
    }



    return 0;

    /*

        MCD_TRACE(MCD_INFO_TRC, 0,  "Get Scan Result, len = %d\n", len);

        if ((len - 2) % sizeof(RDA5890_BSS_DESC)) {
            MCD_TRACE(MCD_INFO_TRC, 0,"Scan Result len not correct, %d\n", len);
            return -1;
        }

        count = (len - 2) / sizeof(RDA5890_BSS_DESC);
          *num = count;
        memcpy((void *)bss_desc, buf + 2, (len - 2));
                sxs_Dump(_MMI, 0, (UINT8*)bss_desc, (len - 2));

        MCD_TRACE(MCD_INFO_TRC, 0,  "rda5890_get_scan_results <<<\n");

            useingIrq = TRUE;


        return count;
        */
}
#endif
int test_rda5890_set_ssid_Ex(unsigned char *ssid, unsigned char ssid_len)
{
    int ret = 0;
    UINT8 wid_req[RDA5890_MAX_WID_LEN];
    //unsigned short wid_req_len = 7 + ssid_len;
//  UINT8 wid_rsp[RDA5890_MAX_WID_LEN];
//  unsigned short wid_rsp_len = RDA5890_MAX_WID_LEN;
    UINT8 wid_msg_id = rda5890_wid->wid_msg_id;
    rda5890_wid->wid = WID_SSID;
    rda5890_wid->wid_rsp_len =RDA5890_MAX_WID_LEN;
    memset(rda5890_wid->wid_rsp,0x0,512);



    UINT32 val;
    UINT16 tmp_len ;


    wid_req[0] = 'W';
    wid_req[1] = wid_msg_id;




    //wid_req[2] = (UINT8)(wid_req_len&0x00FF);
    //wid_req[3] = (UINT8)((wid_req_len&0xFF00) >> 8);



    UINT16 wid = 0;
    tmp_len = 4;
    wid= WID_LISTEN_INTERVAL;
    wid_req[tmp_len] = (char)(wid&0x00FF);
    wid_req[tmp_len+1] = (char)((wid&0xFF00) >> 8);

    wid_req[tmp_len+2] = 1;
    wid_req[tmp_len+3] = 0x06;
    tmp_len+=4;

    if( wifi_context.dot11_mode !=0)
    {
        wid= WID_802_11I_MODE;
        wid_req[tmp_len] = (char)(wid&0x00FF);
        wid_req[tmp_len+1] = (char)((wid&0xFF00) >> 8);

        wid_req[tmp_len+2] = 1;
        wid_req[tmp_len+3] = wifi_context.dot11_mode;
        tmp_len+=4;
    }
    if(wifi_context.auth_type!=0)
    {
        wid = WID_AUTH_TYPE;

        wid_req[tmp_len] = (char)(wid&0x00FF);
        wid_req[tmp_len+1] = (char)((wid&0xFF00) >> 8);

        wid_req[tmp_len+2] = 1;
        wid_req[tmp_len+3] = wifi_context.auth_type;
        tmp_len+=4;
    }
    if(strlen(wifi_context.WEP0) > 4)
    {
        wid = WID_WEP_KEY_VALUE0;

        wid_req[tmp_len] = (char)(wid&0x00FF);
        wid_req[tmp_len+1] = (char)((wid&0xFF00) >> 8);

        wid_req[tmp_len+2] = (UINT8)(strlen(wifi_context.WEP0) );
        memcpy(wid_req +(tmp_len+ 3), wifi_context.WEP0, strlen(wifi_context.WEP0));
        tmp_len += (3 + strlen(wifi_context.WEP0));
    }



    wid= WID_CURRENT_TX_RATE;
    wid_req[tmp_len] = (char)(wid&0x00FF);
    wid_req[tmp_len+1] = (char)((wid&0xFF00) >> 8);

    wid_req[tmp_len+2] = 1;
    wid_req[tmp_len+3] = 0x01;
    tmp_len+=4;


    wid= WID_SSID;
    wid_req[tmp_len] = (UINT8)(wid & 0x00FF);
    wid_req[tmp_len+1] = (UINT8)((wid & 0xFF00) >> 8);

    wid_req[tmp_len+2] = (UINT8)(ssid_len);
    memcpy(wid_req +(tmp_len+ 3), ssid, ssid_len);



    tmp_len +=(3+ ssid_len);

    UINT8* pWid_req = wid_req + tmp_len;
    wid = WID_MEMORY_ADDRESS;
    pWid_req[0] = (char)(wid&0x00FF);
    pWid_req[1] = (char)((wid&0xFF00) >> 8);
    pWid_req[2] = 4;
    pWid_req[3] = 0xec;
    pWid_req[4] = 0x81;
    pWid_req[5] = 0x10;
    pWid_req[6] = 0x00;
    tmp_len += 7;
    pWid_req += 7;

    wid = WID_MEMORY_ACCESS_32BIT;
    pWid_req[0] = (char)(wid&0x00FF);
    pWid_req[1] = (char)((wid&0xFF00) >> 8);
    pWid_req[2] = 4;
    pWid_req[3] = wifi_context.cur_bssid[0];
    pWid_req[4] = wifi_context.cur_bssid[1];
    pWid_req[5] = wifi_context.cur_bssid[2];
    pWid_req[6] = wifi_context.cur_bssid[3];
    tmp_len += 7;
    pWid_req += 7;

    wid = WID_MEMORY_ADDRESS;
    pWid_req[0] = (char)(wid&0x00FF);
    pWid_req[1] = (char)((wid&0xFF00) >> 8);
    pWid_req[2] = 4;
    pWid_req[3] = 0xf0;
    pWid_req[4] = 0x81;
    pWid_req[5] = 0x10;
    pWid_req[6] = 0x00;
    tmp_len += 7;
    pWid_req += 7;

    wid = WID_MEMORY_ACCESS_32BIT;
    pWid_req[0] = (char)(wid&0x00FF);
    pWid_req[1] = (char)((wid&0xFF00) >> 8);
    pWid_req[2] = 4;
    pWid_req[3] = wifi_context.cur_bssid[4];
    pWid_req[4] = wifi_context.cur_bssid[5];
    pWid_req[5] = 0;
    pWid_req[6] = 0;
    tmp_len += 7;
    pWid_req += 7;
    /*
                      wid_req[tmp_len] = (char)(0x201e&0x00FF);
                wid_req[tmp_len+1] = (char)((0x201e&0xFF00) >> 8);

                wid_req[tmp_len+2] = (char)(sizeof(UINT32));
                val = 0x5000050C;
                memcpy(wid_req + tmp_len+3, &val, sizeof(UINT32));

                tmp_len= tmp_len + 3+ sizeof(UINT32);
                //write data
                wid_req[tmp_len] = (char)(0x201f&0x00FF);
                wid_req[tmp_len+1] = (char)((0x201f&0xFF00) >> 8);

                wid_req[tmp_len+2] = (char)(sizeof(UINT32));
                val = 0x00008000;
                memcpy(wid_req + tmp_len+3, &val, sizeof(UINT32));

                tmp_len= tmp_len + 3+ sizeof(UINT32);
    */

    wid_req[2] = (UINT8)(tmp_len&0x00FF);
    wid_req[3] = (UINT8)((tmp_len&0xFF00) >> 8);

    ret = rda5990_wid_request(wid_req, tmp_len);

    if (ret)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"rda5890_wid_request fail, ret = %d\n", ret);

    }
    return ret;

}
int test_rda5890_set_ssid(unsigned char *ssid, unsigned char ssid_len)
{
    int ret =0;

    MCD_TRACE(MCD_INFO_TRC, 0,  "Set SSID:  len = %d\n",  ssid_len);
    showString(ssid);

//      MCD_TRACE(MCD_INFO_TRC, 0,  "Set SSID: %s\n", ssid);

//      stop_wifiPollTimer();
//hal_sdioDataInIrqMaskClear();

    ret = test_rda5890_generic_set_str(WID_SSID, ssid, ssid_len);
    if (ret)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,  "test_rda5890_generic_set_str fail, %d\n",ret);

        //  goto out;
    }

    MCD_TRACE(MCD_INFO_TRC, 0,  "Set SSID Done\n");

    return 0;
    /*
    //protect_test= TRUE;
        if(-1== sdio_getInputData())
        {
            MCD_TRACE(MCD_INFO_TRC, 0,  "Ssdio_getInputData, -1\n");

                restart_wifiPollTimer();
    //hal_sdioDataInIrqMaskSet();
        return -1;
        }
            //sxs_Dump(_MMI, 0, rda5890_wid->wid_rsp, rda5890_wid->wid_rsp_len);

            //   sxs_Dump(_MMI, 0, rda5890_wid->wid_rsp, rda5890_wid->wid_rsp_len);
                //sxs_Dump(_MMI, 0, rda5890_wid->wid_rsp, rda5890_wid->wid_rsp_len);
            //sxr_Sleep(200);

        ret = test_rda5890_check_wid_status(rda5890_wid->wid_rsp, rda5890_wid->wid_rsp_len, rda5890_wid->wid_msg_id);
        if (ret) {
            MCD_TRACE(MCD_INFO_TRC, 0,"check_wid_status fail, ret = %d\n", ret);
            goto out;
        }

    //protect_test= FALSE;


    out:
                restart_wifiPollTimer();
    //hal_sdioDataInIrqMaskSet();

        return ret;*/
}
/*
void show_scan_result(void)
{
    //unsigned char bss_descs[sizeof(RDA5890_BSS_DESC)*RDA5890_MAX_NETWORK_NUM ];
    UINT8 count;
    memset(&bss_Rst,0x00,sizeof(bss_Rst));
        test_rda5890_get_scan_results((RDA5890_BSS_DESC *)(bss_Rst.bss_descs),&count);
    bss_Rst.count = count;
    if(strlen(bss_Rst.bss_descs)<=0)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,  "the scan result is NULL !!!!!!\n");
        return;
    }
    RDA5890_BSS_DESC* tmpbss = (RDA5890_BSS_DESC *)(bss_Rst.bss_descs);
    UINT8 tmpcount =count;
    while(count > 0)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,  "the scan result : the NO.%d \n",tmpcount-count+1);

        showString((UINT8*)tmpbss);
        tmpbss= tmpbss + 1;
        count--;

    }
}
*/
//UINT32 wifi_SendAttRsp(void);
UINT32 wifi_ScanRsp(VOID);
#ifdef __SCAN_INFO__

int wifi_getNetworkInfo_Req(void);
#endif

/*
void check_scan_result(void)
{
    //unsigned char bss_descs[sizeof(RDA5890_BSS_DESC)*RDA5890_MAX_NETWORK_NUM ];
    UINT8 count;
    memset(&bss_Rst,0x00,sizeof(bss_Rst));
        test_rda5890_get_scan_results((RDA5890_BSS_DESC *)(bss_Rst.bss_descs),&count);

        return;



    bss_Rst.count = count;
    if(strlen(bss_Rst.bss_descs)<=0)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,  "the scan result is NULL !!!!!!\n");
        wifi_ScanRsp();
        return;
    }
    RDA5890_BSS_DESC* tmpbss = (RDA5890_BSS_DESC *)(bss_Rst.bss_descs);
    UINT8 tmpcount =count;
    while(count > 0)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,  "the scan result : the NO.%d \n",tmpcount-count+1);

        showString((UINT8*)tmpbss);
        tmpbss= tmpbss + 1;
        count--;

    }
    wifi_ScanRsp();

}
*/
VOID Optimize_scan_result(UINT8* paylod, UINT16 len)
{
    RDA5890_BSS_DESC* tmp;
    RDA5890_BSS_DESC* tmp_p;
    UINT8 item_len = sizeof(RDA5890_BSS_DESC);

    tmp = (RDA5890_BSS_DESC*)paylod;
    tmp_p = (RDA5890_BSS_DESC*)bss_Rst.bss_descs;
    UINT8 ap_count = len/item_len;
    UINT8 i, j;
    for(i=0; i <  ap_count; i++ )
    {
        for(j=0; j <  bss_Rst.count; j++)
        {
            if(strcmp(tmp_p[j].ssid,tmp[i].ssid)==0)
                break;
        }
        if(j  == bss_Rst.count)
        {
            memcpy((UINT8 *)tmp_p + bss_Rst.count *item_len, ((UINT8*)tmp + i *item_len),item_len);
            bss_Rst.count++;
        }
        else
        {
            MCD_TRACE(MCD_INFO_TRC, 0,  "There has been the same name AP, RSSI: 0x%x; my RSSI: 0x%x\n",tmp_p[j].rssi , tmp[i].rssi);

            if(tmp_p[j].rssi < tmp[i].rssi)
            {
                MCD_TRACE(MCD_INFO_TRC, 0,  "replace the AP with the new same name AP\n");

                memcpy((UINT8 *)tmp_p+ j *item_len, ((UINT8 *)tmp + i *item_len),item_len);
            }

        }


    }

}
void check_scan_resultEx(void)
{
    RDA5890_BSS_DESC bss_descs_tmp[6 ];
    UINT8 struct_size = sizeof(RDA5890_BSS_DESC);

    memset(bss_descs_tmp,0x00,struct_size*6);
    UINT16 len_tmp =0;
    UINT8 count = 0x00;
    unsigned char len =0;
    UINT8* pay_load =NULL;
    test_rda5890_check_wid_response(rda5890_wid->wid_rsp, rda5890_wid->wid_rsp_len, rda5890_wid->wid, rda5890_wid->wid_msg_id, &len, &pay_load);


//  MCD_TRACE(MCD_INFO_TRC, 0,  "check_scan_resultEx.  count: 0x%x \n",count);

    if(pay_load==NULL)
    {
        if(bss_Rst.count  ==0x00)
        {
            MCD_TRACE(MCD_INFO_TRC, 0,  "the scan result is NULL !!!!!!\n");
            wifi_ScanRsp();

            return;
        }
    }
    else
    {
        if(wifi_version_flag== WIFI_VERSION_D )
        {
            len_tmp = len-2;
            count = len_tmp /(struct_size-14);
            //  MCD_TRACE(MCD_INFO_TRC, 0,  "sizeof(RDA5890_BSS_DESC) :%d, len_tmp: %d, count: %d\n",sizeof(RDA5890_BSS_DESC),len_tmp,count);

            UINT8 ii =0;
            for(ii=0; ii<count; ii++)
            {
                //     MCD_TRACE(MCD_INFO_TRC, 0, "ii: %d; &bss_descs_tmp[ii] :0x%x \n",ii,&bss_descs_tmp[ii]);
                memcpy(&bss_descs_tmp[ii], pay_load+2+(struct_size-14)*ii,(struct_size-14));
            }
            len_tmp = len_tmp + count*14;
        }
        else if((wifi_version_flag== WIFI_VERSION_E) ||(wifi_version_flag== WIFI_VERSION_F))
        {
            len_tmp = len-2;
            count = len_tmp /struct_size;
            memcpy(bss_descs_tmp, pay_load+2,len_tmp);

        }
        //MCD_TRACE(MCD_INFO_TRC, 0,    "the scan result, data add: 0x%x;  len_tmp:%d;  data :\n",bss_descs_tmp,len_tmp);

        // sxs_Dump(_MMI, 0, (UINT8*)bss_descs_tmp, len_tmp);

        Optimize_scan_result((UINT8* )bss_descs_tmp, len_tmp);
    }
    //memcpy((void *)bss_Rst.bss_descs+bss_Rst.count *sizeof(RDA5890_BSS_DESC), pay_load + 2, (len - 2));
    //  bss_Rst.count += count;

    //MCD_TRACE(MCD_INFO_TRC, 0,    "check_scan_resultEx.  count: 0x%x ,bss_Rst.count  :0x%x\n",count,bss_Rst.count );
    if(bss_Rst.count==0)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,  "the scan result is NULL !!!!!!\n");
        wifi_ScanRsp();
        return;
    }

    UINT8 i =0;

    sxs_Dump(_MMI, 0, (UINT8*)bss_Rst.bss_descs,  bss_Rst.count*struct_size);

    RDA5890_BSS_DESC* tmpbss =( (RDA5890_BSS_DESC *)(bss_Rst.bss_descs));

    for(i =0; i < bss_Rst.count; i++)
    {
        //MCD_TRACE(MCD_INFO_TRC, 0,    "the scan result : the NO.%d \n", i+1);

        //showString((UINT8*)tmpbss->ssid);
        tmpbss= tmpbss + 1;
        //  count--;

    }

    UINT8 res_num = 0;
    if(wifi_version_flag== WIFI_VERSION_D )
    {
        res_num = 5;

    }
    else if((wifi_version_flag== WIFI_VERSION_E) ||(wifi_version_flag== WIFI_VERSION_F))
    {
        res_num = 4;
    }
    if((count==res_num)&&(bss_Rst.count < (res_num+1)))
    {
        get_scan_result_continue = TRUE;

    }
    else
    {
        MCD_TRACE(MCD_INFO_TRC, 0,  "the scan result is posted to application !\n");

        wifi_ScanRsp();
        //test_rda5890_start_scan_network_info();
#ifdef __SCAN_INFO__

        wifi_getNetworkInfo_Req();
#endif
    }


}
int test_rda5890_get_mac_addr(unsigned char *mac_addr)
{
    int ret=0x00;

    MCD_TRACE(MCD_INFO_TRC, 0,  "rda5890_get_mac_addr >>>\n");
    UINT8 widlen;
    ret = test_rda5890_generic_get_str(WID_MAC_ADDR, mac_addr, &widlen);
    if (ret)
    {
        goto out;
    }

    MCD_TRACE(MCD_INFO_TRC, 0,  "Get STA MAC Address: \n");
    sxs_Dump(_MMI, 0, mac_addr, ETH_ALEN);


out:
    MCD_TRACE(MCD_INFO_TRC, 0,  "rda5890_get_mac_addr <<<, ret = %d\n", ret);
    return ret;
}
VOID wifi_setMac(UINT8 *mac_addr, UINT8 len)
{
    MCD_TRACE(MCD_INFO_TRC, 0,  "Set MAC \n");

    UINT32 ret;
    ret = test_rda5890_generic_set_str(WID_MAC_ADDR, mac_addr, len);
    if (ret)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,  "test_rda5890_generic_set_str fail, %d\n",ret);

        return ;
    }


//protect_test= TRUE;
    if(-1== sdio_getInputData())
    {
        MCD_TRACE(MCD_INFO_TRC, 0,  "Ssdio_getInputData, -1\n");

        restart_wifiPollTimer();
//hal_sdioDataInIrqMaskSet();
        return -1;
    }
}
/*
int test_rda5890_generic_get_uchar(
        unsigned short wid, unsigned char *val)
{
    int ret;
    UINT8 len;
    char wid_req[6];
    unsigned short wid_req_len = 6;
    //char wid_rsp[32];
    //unsigned short wid_rsp_len = 32;
    char wid_msg_id = rda5890_wid->wid_msg_id;
        rda5890_wid->wid = wid;
    rda5890_wid->wid_rsp_len =RDA5890_MAX_WID_LEN;
    memset(rda5890_wid->wid_rsp,0x0,512);

    char *ptr_payload;
//hal_sdioDataInIrqMaskClear();

    wid_req[0] = 'Q';
    wid_req[1] = wid_msg_id;

    wid_req[2] = (char)(wid_req_len&0x00FF);
    wid_req[3] = (char)((wid_req_len&0xFF00) >> 8);

    wid_req[4] = (char)(wid&0x00FF);
    wid_req[5] = (char)((wid&0xFF00) >> 8);
                stop_wifiPollTimer();

    ret = rda5990_wid_request(wid_req, wid_req_len);
    if (ret) {
          MCD_TRACE(MCD_INFO_TRC, 0,"rda5890_wid_request fail, ret = %d\n", ret);
        goto out;
    }
     if(-1== sdio_getInputData())
        {
        restart_wifiPollTimer();
//hal_sdioDataInIrqMaskSet();

        return -1;
        }

    // UINT8 payload_len =0;
          ret = test_rda5890_check_wid_response(rda5890_wid->wid_rsp, rda5890_wid->wid_rsp_len, wid, wid_msg_id, &len, (UINT8**)&ptr_payload);
       if (ret) {
           MCD_TRACE(MCD_INFO_TRC, 0,"check_wid_response fail, ret = %d\n", ret);
        goto out;
    }
         MCD_TRACE(MCD_INFO_TRC, 0, "payload_len: %d",len);
   //         sxs_Dump(_MMI, 0, ptr_payload, payload_len);

    memcpy(val, ptr_payload, len);
       sxs_Dump(_MMI, 0, val, len);

    //     MCD_TRACE(MCD_INFO_TRC, 0, "get wid: 0x%x,  val: %s",wid,val);

out:
        restart_wifiPollTimer();
//hal_sdioDataInIrqMaskSet();

    return ret;
}
*/
int test_rda5890_generic_set_uchar(
    unsigned short wid, unsigned char val)
{
    int ret;
    //UINT8 len;
    char wid_req[7 + sizeof(unsigned char)];
    memset(wid_req, 0x00,(7 + sizeof(unsigned char)));
    unsigned short wid_req_len = 7 + sizeof(unsigned char);
    //char wid_rsp[32];
    //memset(wid_rsp,0x00,32);
    //unsigned short wid_rsp_len = 32;
    char wid_msg_id = rda5890_wid->wid_msg_id;
    rda5890_wid->wid = wid;
    rda5890_wid->wid_rsp_len =RDA5890_MAX_WID_LEN;
    memset(rda5890_wid->wid_rsp,0x0,512);

    //MCD_TRACE(MCD_INFO_TRC, 0,"test_rda5890_generic_set_uchar ,wid_req: 0x%x,wid_req[0]=0x%x\n",wid_req, wid_req[0]);
    MCD_TRACE(MCD_INFO_TRC, 0,"test_rda5890_generic_set_uchar ,rda5890_wid->wid_msg_id: 0x%x,rda5890_wid->wid_pending: 0x%x\n",rda5890_wid->wid_msg_id,rda5890_wid->wid_pending);

    wid_req[0] = 'W';
    wid_req[1] = wid_msg_id;

    wid_req[2] = (char)(wid_req_len&0x00FF);
    wid_req[3] = (char)((wid_req_len&0xFF00) >> 8);

    wid_req[4] = (char)(wid&0x00FF);
    wid_req[5] = (char)((wid&0xFF00) >> 8);

    wid_req[6] = (char)(sizeof(unsigned char));
    memcpy(wid_req + 7, &val, sizeof(unsigned char));

    ret = rda5990_wid_request(wid_req, wid_req_len);
    if (ret)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"rda5890_wid_request fail, ret = %d\n", ret);
        goto out;
    }


    //     MCD_TRACE(MCD_INFO_TRC, 0, "get wid: 0x%x,  val: %s",wid,val);

out:
    return ret;

}


int test_rda5890_generic_set_ushort(
    unsigned short wid, unsigned short val)
{
    int ret;
    //UINT8 len;
    char wid_req[7 + sizeof(unsigned short)];
    memset(wid_req, 0x00,(7 + sizeof(unsigned short)));
    unsigned short wid_req_len = 7 + sizeof(unsigned short);
    //char wid_rsp[32];
    //memset(wid_rsp,0x00,32);
    //unsigned short wid_rsp_len = 32;
    char wid_msg_id = rda5890_wid->wid_msg_id;
    rda5890_wid->wid = wid;
    rda5890_wid->wid_rsp_len =RDA5890_MAX_WID_LEN;
    memset(rda5890_wid->wid_rsp,0x0,512);

    //MCD_TRACE(MCD_INFO_TRC, 0,"test_rda5890_generic_set_uchar ,wid_req: 0x%x,wid_req[0]=0x%x\n",wid_req, wid_req[0]);
    MCD_TRACE(MCD_INFO_TRC, 0,"test_rda5890_generic_set_uchar ,rda5890_wid->wid_msg_id: 0x%x,rda5890_wid->wid_pending: 0x%x\n",rda5890_wid->wid_msg_id,rda5890_wid->wid_pending);

    wid_req[0] = 'W';
    wid_req[1] = wid_msg_id;

    wid_req[2] = (char)(wid_req_len&0x00FF);
    wid_req[3] = (char)((wid_req_len&0xFF00) >> 8);

    wid_req[4] = (char)(wid&0x00FF);
    wid_req[5] = (char)((wid&0xFF00) >> 8);

    wid_req[6] = (char)(sizeof(unsigned short));
    memcpy(wid_req + 7, &val, sizeof(unsigned short));

    ret = rda5990_wid_request(wid_req, wid_req_len);
    if (ret)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"rda5890_wid_request fail, ret = %d\n", ret);
        goto out;
    }


    //     MCD_TRACE(MCD_INFO_TRC, 0, "get wid: 0x%x,  val: %s",wid,val);

out:
    return ret;

}


#if 0



static int rda5890_generic_get_ulong(
    unsigned short wid, unsigned long *val)
{
    int ret;
    char wid_req[6];
    unsigned short wid_req_len = 6;
    char wid_rsp[32];
    unsigned short wid_rsp_len = 32;
    char wid_msg_id = rda5890_wid->wid_msg_id++;
    char *ptr_payload;

    wid_req[0] = 'Q';
    wid_req[1] = wid_msg_id;

    wid_req[2] = (char)(wid_req_len&0x00FF);
    wid_req[3] = (char)((wid_req_len&0xFF00) >> 8);

    wid_req[4] = (char)(wid&0x00FF);
    wid_req[5] = (char)((wid&0xFF00) >> 8);

    ret = rda5890_wid_request(wid_req, wid_req_len, wid_rsp, &wid_rsp_len);
    if (ret)
    {
        //RDA5890_ERRPRINT1("rda5890_wid_request fail, ret = %d\n", ret);
        goto out;
    }
    /*
        ret = rda5890_check_wid_response(wid_rsp, wid_rsp_len, wid, wid_msg_id,
            sizeof(unsigned long), &ptr_payload);
        if (ret) {
            RDA5890_ERRPRINT1("check_wid_response fail, ret = %d\n", ret);
            goto out;
        }

        memcpy(val, ptr_payload, sizeof(unsigned long));
    */
out:

    return ret;
}
#endif

int test_rda5890_generic_set_ulong(
    unsigned short wid,UINT32 val)
{
    int ret;
    char wid_req[7 + sizeof(UINT32)];
    unsigned short wid_req_len = 7 + sizeof(UINT32);
//  char wid_rsp[32];
//  unsigned short wid_rsp_len = 32;
    char wid_msg_id = rda5890_wid->wid_msg_id;
    rda5890_wid->wid = wid;
    rda5890_wid->wid_rsp_len =RDA5890_MAX_WID_LEN;
    memset(rda5890_wid->wid_rsp,0x0,512);

    MCD_TRACE(MCD_INFO_TRC, 0,"test_rda5890_generic_set_ulong ,wid_req: 0x%x,wid_req[0]=0x%x\n",wid_req, wid_req[0]);

    wid_req[0] = 'W';
    wid_req[1] = wid_msg_id;

    wid_req[2] = (char)(wid_req_len&0x00FF);
    wid_req[3] = (char)((wid_req_len&0xFF00) >> 8);

    wid_req[4] = (char)(wid&0x00FF);
    wid_req[5] = (char)((wid&0xFF00) >> 8);

    wid_req[6] = (char)(sizeof(UINT32));
    memcpy(wid_req + 7, &val, sizeof(UINT32));

    ret = rda5990_wid_request(wid_req, wid_req_len);
    if (ret)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"rda5890_wid_request fail, ret = %d\n", ret);
        goto out;
    }

    //ret = test_rda5890_check_wid_status(wid_rsp, wid_rsp_len, wid_msg_id);
    //if (ret) {
    //   MCD_TRACE(MCD_INFO_TRC, 0,"check_wid_status fail, ret = %d\n", ret);
    //  goto out;
    //}

out:
    return ret;
}
int test_rda5890_set_wepkey(unsigned short index, unsigned char *key, unsigned char key_len)
{
    int ret;
    unsigned char *key_str;

    unsigned char key_str_len;

    key_str = wifi_context.WEP0;

    if (key_len == 5)    //WEP40
    {
        sprintf(key_str, "%02x%02x%02x%02x%02x\n",
                key[0], key[1], key[2], key[3], key[4]);
        key_str_len = 10;
        key_str[key_str_len] = '\0';
    }
    else if (key_len == 13)  //wep104
    {
        sprintf(key_str, "%02x%02x%02x%02x%02x"
                "%02x%02x%02x%02x%02x"
                "%02x%02x%02x\n",
                key[0], key[1], key[2], key[3], key[4],
                key[5], key[6], key[7], key[8], key[9],
                key[10], key[11], key[12]);
        key_str_len = 26;
        key_str[key_str_len] = '\0';
    }
    else if (key_len == 10)    //WEP40
    {
        key_str_len = 10;
        memcpy(key_str,key,key_len);
        key_str[key_str_len] = '\0';
    }
    else if (key_len == 26)  //wep104
    {

        key_str_len = 26;
        memcpy(key_str,key,key_len);
        key_str[key_str_len] = '\0';
    }
    else
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"Error in WEP Key length %d\n", key_len);
        ret = -1;
        //goto out;
    }

    MCD_TRACE(MCD_INFO_TRC, 0,  "Set WEP KEY[%d]: %s\n", index, key_str);
    // stop_wifiPollTimer();
    return 0;//modify 2012-06-01
    /*
        ret = test_rda5890_generic_set_str((WID_WEP_KEY_VALUE0 + index), key_str, key_str_len);
        if (ret) {
                MCD_TRACE(MCD_INFO_TRC, 0,  "Set WEP KEY[%d] , error\n", index);

        }

        MCD_TRACE(MCD_INFO_TRC, 0,  "Set WEP KEY[%d] Done\n", index);

    return 0;
        if(-1== sdio_getInputData())
        {
                        restart_wifiPollTimer();

                return -1;
        }

        ret = test_rda5890_check_wid_status(rda5890_wid->wid_rsp, rda5890_wid->wid_rsp_len, rda5890_wid->wid_msg_id);
        if (ret) {
            MCD_TRACE(MCD_INFO_TRC, 0,"check_wid_status fail, ret = %d\n", ret);
            goto out;
        }



    out:
                            restart_wifiPollTimer();

        return ret;
        */
}
extern UINT8 *wifi_password;

/*
INT8  test_rda5890_set_wepKid(UINT8 Kid)
{
    int ret;
                        stop_wifiPollTimer();

       ret = test_rda5890_generic_set_uchar(WID_KEY_ID, Kid);
    if (ret) {
        return -1;
    }

    MCD_TRACE(MCD_INFO_TRC, 0,  "Set WEP Kid :%d\n", Kid);

    if(-1== sdio_getInputData())
    {
                        restart_wifiPollTimer();

            return -1;
    }
     return 0;
}
int test_rda5890_set_psk( unsigned char *key, unsigned char key_len)
{
                    stop_wifiPollTimer();

    test_rda5890_generic_set_str((WID_802_11I_PSK ), key, key_len);


    return 0;
    if(-1== sdio_getInputData())
    {
                        restart_wifiPollTimer();

            return -1;
    }
    test_rda5890_check_wid_status(rda5890_wid->wid_rsp, rda5890_wid->wid_rsp_len, rda5890_wid->wid_msg_id);
                    restart_wifiPollTimer();

    return 0;
}
*/
int test_rda5890_set_StrWid(unsigned short wid,  unsigned char *key, unsigned char key_len)
{

    UINT32 mask_tmp =  hal_sdioDataInIrqMaskGet();

    hal_sdioDataInIrqMaskClear();

    test_rda5890_generic_set_str(wid, key, key_len);

    if(-1== sdio_getWidResp())
    {
        // restart_wifiPollTimer();

        return -1;
    }
    hal_sdioDataInIrqMaskSetEx(mask_tmp);
    return 0;
}

int test_rda5890_set_StrWidEx(unsigned short wid,  unsigned char *key, unsigned char key_len)
{



    test_rda5890_generic_set_str(wid, key, key_len);


    return 0;
}
extern UINT8 localMac[6] ;

struct bss_descriptor bss_desc_Used;




unsigned char oui_rsn[3] = {0x00, 0x0F, 0xAC};
unsigned char oui_wpa[3] = {0x00, 0x50, 0xf2};

static void fill_rsn_wpa_ie(unsigned char *data, unsigned char ie_type,
                            struct rda5890_bss_descriptor *bss, size_t *len)
{
    unsigned char index = 0;
    unsigned char *oui;

    if (ie_type == IRSNELEMENT)
    {
        oui = &oui_rsn[0];

        /* Set RSN Information Element element ID */
        data[index] = IRSNELEMENT;
        index += 2;
    }
    else
    {
        oui = &oui_wpa[0];

        /* Set WPA Information Element element ID */
        data[index] = IWPAELEMENT;
        index += 2;

        /* Copy OUI */
        memcpy(&data[index], oui, 3);
        index += 3;
        data[index++] = 0x01;
    }

    /* Set the version of RSN Element to 1 */
    data[index++] = 1;
    data[index++] = 0;

    /* Set Group Cipher Suite */
    memcpy(&data[index], oui, 3);
    index += 3;
    if ((bss->dot11i_info & BIT5) && !(bss->dot11i_info & BIT6))
    {
        /* only CCMP and !TKIP, use CCMP, otherwise, always TKIP */
        data[index++] = CIPHER_TYPE_CCMP;
    }
    else
    {
        data[index++] = CIPHER_TYPE_TKIP;
    }

    /* Set Pairwise cipher Suite */
    if ((bss->dot11i_info & BIT5) && (bss->dot11i_info & BIT6))
    {
        /* both CCMP and TKIP */
        data[index++] = 1;
        data[index++] = 0;

        /* Check BIT7 to determine who goes first */
        if (bss->dot11i_info & BIT7)
        {
            /* BIT7 is 1 => CCMP goes first */
            memcpy(&data[index], oui, 3);
            index += 3;
            data[index++] = CIPHER_TYPE_CCMP;

            //memcpy(&data[index], oui, 3);
            //index += 3;
            //data[index++] = CIPHER_TYPE_TKIP;
        }
        else
        {
            /* BIT7 is 0 => TKIP goes first */
            //  memcpy(&data[index], oui, 3);
            //  index += 3;
            //  data[index++] = CIPHER_TYPE_TKIP;

            memcpy(&data[index], oui, 3);
            index += 3;
            data[index++] = CIPHER_TYPE_CCMP;
        }
    }
    else if ((bss->dot11i_info & BIT5) && !(bss->dot11i_info & BIT6))
    {
        /* CCMP and !TKIP */
        data[index++] = 1;
        data[index++] = 0;

        memcpy(&data[index], oui, 3);
        index += 3;
        data[index++] = CIPHER_TYPE_CCMP;
    }
    else if (!(bss->dot11i_info & BIT5) && (bss->dot11i_info & BIT6))
    {
        /* !CCMP and TKIP */
        data[index++] = 1;
        data[index++] = 0;

        memcpy(&data[index], oui, 3);
        index += 3;
        data[index++] = CIPHER_TYPE_TKIP;
    }
    else
    {
        /* neither CCMP nor TKIP, use TKIP for WPA, and CCMP for RSN */
        data[index++] = 1;
        data[index++] = 0;

        memcpy(&data[index], oui, 3);
        index += 3;
        if (ie_type == IRSNELEMENT)
        {
            data[index++] = CIPHER_TYPE_CCMP;
        }
        else
        {
            data[index++] = CIPHER_TYPE_TKIP;
        }
    }

    /* Set Authentication Suite */
    if ((bss->auth_info & 0x01) && (bss->auth_info & 0x02))
    {
        /* both 802.1X and PSK */
        data[index++] = 1;
        data[index++] = 0;

        //memcpy(&data[index], oui, 3);
        //index += 3;
        //data[index++] = 0x01;

        memcpy(&data[index], oui, 3);
        index += 3;
        data[index++] = 0x02;
    }
    else if ((bss->auth_info & 0x01) && !(bss->auth_info & 0x02))
    {
        /* 802.1X and !PSK */
        data[index++] = 1;
        data[index++] = 0;

        memcpy(&data[index], oui, 3);
        index += 3;
        data[index++] = 0x01;
    }
    else if (!(bss->auth_info & 0x01) && (bss->auth_info & 0x02))
    {
        /* !802.1X and PSK */
        data[index++] = 1;
        data[index++] = 0;

        memcpy(&data[index], oui, 3);
        index += 3;
        data[index++] = 0x02;
    }
    else
    {
        /* neither 802.1X nor PSK, use 802.1X */
        data[index++] = 1;
        data[index++] = 0;

        memcpy(&data[index], oui, 3);
        index += 3;
        data[index++] = 0x01;
    }

    /* The RSN Capabilities, for RSN IE only */
    if (ie_type == IRSNELEMENT)
    {

        //data[index++] = bss->rsn_cap[0];
        data[index++] = 0x00; //modify 2012-06-15, for WMM open setting in AP
        data[index++] = bss->rsn_cap[1];
    }

    /* Set the length of the RSN Information Element */
    data[1] = (index - 2);

    /* Return the Extended Supported Rates element length */
    *len = (size_t)index;
}

/* reconstruct wpa/rsn ie from the dot11i_info and auth_info fields */
/* TODO:
 * assuming RSN and WPA are using same cipher suite, no space to store each
 * assuming grp and unicast are using same cipher suite
 */
static void reconstruct_rsn_wpa_ie(struct bss_descriptor *bss_desc)
{
    bss_desc->wpa_ie_len = 0;
    bss_desc->rsn_ie_len = 0;
//              sxs_Dump(_MMI, 0, &(bss_desc_Used.data), sizeof(RDA5890_BSS_DESC));

    if (bss_desc->data.dot11i_info & BIT0)
    {
        if (bss_desc->data.dot11i_info & BIT4)
        {
            /* RSN IE present */
            MCD_TRACE(MCD_INFO_TRC, 0,  "reconstruct_rsn_wpa_ie  RSN IE present \n");
            fill_rsn_wpa_ie(&bss_desc->rsn_ie[0], IRSNELEMENT,
                            &bss_desc->data, &bss_desc->rsn_ie_len);
        }

        if (bss_desc->data.dot11i_info & BIT3)
        {
            /* WPA IE present */
            MCD_TRACE(MCD_INFO_TRC, 0,  "reconstruct_rsn_wpa_ie WPA IE present \n");
            fill_rsn_wpa_ie(&bss_desc->wpa_ie[0], IWPAELEMENT,
                            &bss_desc->data, &bss_desc->wpa_ie_len);
        }
    }
    else
    {
        MCD_TRACE(MCD_INFO_TRC, 0,  "form_rsn_ie, NO SEC\n");
    }
}

extern UINT8 *wifi_password;

#define WPA_PROTO_WPA    0x01
#define WPA_PROTO_RSN     0x02
INT  test_rda5890_set_preasso_sleep( UINT32 val)
{
    MCD_TRACE(MCD_INFO_TRC, 0,  "test_rda5890_set_preasso_sleep...\n");

    test_rda5890_generic_set_ulong(0x2807,val);
    if(-1== sdio_getInputData())
    {
        restart_wifiPollTimer();
        //hal_sdioDataInIrqMaskSet();

        return -1;
    }
    return 0;
}
INT  test_rda5890_set_TxRate( UINT8 val)
{
    MCD_TRACE(MCD_INFO_TRC, 0,  "test_rda5890_set_TxRate...\n");

    test_rda5890_generic_set_uchar(WID_CURRENT_TX_RATE,val);
    return 0;
}

/*
INT  test_rda5890_set_active_scan_time(USHORT val)
{
    MCD_TRACE(MCD_INFO_TRC, 0,  "test_rda5890_set_active_scan_time...\n");

    test_rda5890_generic_set_ushort(0x100c,val);
         if(-1== sdio_getInputData())
        {
        restart_wifiPollTimer();
        return -1;
        }
         return 0;
}


*/

INT  test_rda5890_set_pm_mode(UINT8 val)
{

    MCD_TRACE(MCD_INFO_TRC, 0,  "test_rda5890_set_pm_mode...mode: 0x%x\n",val);
    //if(val > 0)
//          wifi_sleep_enable_flag |= 0x10;
//  else
//          wifi_sleep_enable_flag &= ~(0x10);

    test_rda5890_generic_set_uchar(0x000b,val);
    if(-1== sdio_getInputData())
    {
        restart_wifiPollTimer();
//hal_sdioDataInIrqMaskSet();

        return -1;
    }
    return 0;
}

/*
INT  test_rda5890_disable_self_cts(void)
{
MCD_TRACE(MCD_INFO_TRC, 0,  "test_rda5890_set_self_cts...mode: 0x%x\n",val);
    test_rda5890_generic_set_uchar(0x0042,0);
         if(-1== sdio_getInputData())
        {
        restart_wifiPollTimer();
        return -1;
        }
         return 0;
}


*/


INT  test_rda5890_SetWifiSleepEx(VOID)
{

    usingIrqClr();
    MCD_TRACE(MCD_INFO_TRC, 0,  "test_rda5890_SetWifiSleepEx...\n");
    //test_rda5890_set_ssid("errorssid",strlen("errorssid"));

    UINT32 tmp_addr = 0x08000000;
    tmp_addr = tmp_addr |(1<<28) |( 0x05 <<9);
    send_test_cmd(52,tmp_addr);

    tmp_addr = 0x88000000;
    tmp_addr = tmp_addr |(1<<28) |( 0x05 <<9)|0x80;
    send_test_cmd(52,tmp_addr);

    //test_rda5890_set_preasso_sleep(0x01000020);//set preasso time to (2^5)/10 s
    //wifi_sleep_enable_flag = 0x10;
    sxr_Sleep(10*16383/1000);
    wifi_sleep_flag = TRUE;
    if(!sdio_sleep_flag)
        wifi_SdioSleep();

    usingIrqSet();
    return 0;
}


INT  wifi_SetSdioSleep(VOID)
{

    usingIrqClr();

    if(!sdio_sleep_flag)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,  "wifi_SetSdioSleep...\n");

        wifi_SdioSleep();
    }

    usingIrqSet();
    return 0;
}


INT  wifi_MAC_sleep(VOID)
{
#ifdef  USE_WIFI_SLEEP
    MCD_TRACE(MCD_INFO_TRC, 0,    "wifi_MAC_sleep...\n");

    //   wifi_DisconnectAp();
#endif

    usingIrqClr();
    //test_rda5890_set_ssid("errorssid",strlen("errorssid"));
    if(!wifi_sleep_flag )
    {
        UINT32 tmp_addr = 0x08000000;
        tmp_addr = tmp_addr |(1<<28) |( 0x05 <<9);
        send_test_cmd(52,tmp_addr);

        tmp_addr = 0x88000000;
        tmp_addr = tmp_addr |(1<<28) |( 0x05 <<9)|0x80;
        send_test_cmd(52,tmp_addr);

        //test_rda5890_set_preasso_sleep(0x01000020);//set preasso time to (2^5)/10 s
        //wifi_sleep_enable_flag = 0x10;
        sxr_Sleep(10*16383/1000);
        wifi_sleep_flag = TRUE;
    }

    if(!sdio_sleep_flag)
        wifi_SdioSleep();
    usingIrqSet();

    return 0;
}
INT  wifi_MAC_sleepEx(VOID)
{
#ifdef  USE_WIFI_SLEEP
    //return 0;

    //  wifi_DisconnectAp();

    usingIrqClr();
    //test_rda5890_set_ssid("errorssid",strlen("errorssid"));

    if(!wifi_sleep_flag )
    {
        MCD_TRACE(MCD_INFO_TRC, 0,  "wifi_MAC_sleepEx...\n");

        UINT32 tmp_addr = 0x08000000;
        tmp_addr = tmp_addr |(1<<28) |( 0x05 <<9);
        send_test_cmd(52,tmp_addr);

        tmp_addr = 0x88000000;
        tmp_addr = tmp_addr |(1<<28) |( 0x05 <<9)|0x80;
        send_test_cmd(52,tmp_addr);

        //test_rda5890_set_preasso_sleep(0x01000020);//set preasso time to (2^5)/10 s
        //wifi_sleep_enable_flag = 0x10;
        sxr_Sleep(10*16383/1000);
        wifi_sleep_flag = TRUE;
    }

    if(!sdio_sleep_flag)
    {
        wifi_SdioSleep();
    }
    usingIrqSet();
#endif
    return 0;
}

/*
INT  test_rda5890_SetWifiWakeEx(VOID)
{

    MCD_TRACE(MCD_INFO_TRC, 0,  "test_rda5890_SetWifiWakeEx...\n");
    //test_rda5890_set_ssid("errorssid",strlen("errorssid"));
        wifi_sleep_enable_flag &=~( 0x01);

    test_rda5890_set_preasso_sleep(0x01000020);//set preasso time to (2^5)/10 s
}
*/
UINT16 pmu_reg_msk= 0xF1C7;
UINT16 wifi_Read_MAC_Reg_I2C_rf(UINT8 reg);
void wifi_Write_MAC_Reg_I2C_rf(UINT8 reg, UINT16 val);

/*UINT16 rda5990_I2C_PMU_read(VOID)
{
    UINT16 read_val = 0x00;
    return wifi_Read_MAC_Reg_I2C_rf();
}
UINT16 rda5990_I2C_PMU_write(VOID)
{

}
*/

VOID rda5890_WifWakeUp(VOID)
{
    MCD_TRACE(MCD_INFO_TRC, 0,  "rda5890_WifWakeUp...\n");
    usingIrqClr();
    if(sdio_sleep_flag)
        wifi_SdioWakeUp();
    //sxr_Sleep(5000);
    UINT16 read_val = 0x00;
//  read_val =  wifi_Read_MAC_Reg_I2C_rf(0xA7);
    //wifi_Write_MAC_Reg_I2C_rf(0xA7, (read_val & pmu_reg_msk) );

    UINT32 tmp_addr = 0x88000000;
    tmp_addr = tmp_addr |(1<<28) |( 0x09 <<9)|0x01;
    send_test_cmd(52,tmp_addr);
    //wifi_sleep_enable_flag =0x00;
//  wifi_Write_MAC_Reg_I2C_rf(0xA7, read_val  );
    wifi_sleep_flag = FALSE;
    //sxr_Sleep(250);
    usingIrqSet();
}

VOID rda5890_WifWakeUpEx(VOID)
{
    MCD_TRACE(MCD_INFO_TRC, 0,  "rda5890_WifWakeUpEx...\n");
    usingIrqClr();
    if(sdio_sleep_flag)
        wifi_SdioWakeUp();
    //sxr_Sleep(5000);
    /*
        UINT32 tmp_addr = 0x88000000;
               tmp_addr = tmp_addr |(1<<28) |( 0x09 <<9)|0x01;
               send_test_cmd(52,tmp_addr);
        //wifi_sleep_enable_flag =0x00;
    */
//  wifi_sleep_flag = FALSE;
    //sxr_Sleep(250);
    usingIrqSet();
}
VOID start_wifi_poll_timer(VOID);
#if 0
int test_start_associate_bss(UINT8* ssid, UINT8 len)
{
    stop_wifi_poll_timer();
    MCD_TRACE(MCD_INFO_TRC, 0,  "test_start_associate_bss\n");

    RDA5890_BSS_DESC* tmp_ssid;
    tmp_ssid =(RDA5890_BSS_DESC*)( bss_Rst.bss_descs);
    UINT8 count = bss_Rst.count;
    while(count>0)
    {
        if(strcmp(tmp_ssid->ssid,ssid)==0)
            break;
        else
            tmp_ssid+=1;
        count--;
    }
    if(useingIrq == TRUE)
        useingIrq = FALSE;
    if(count ==0)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"set ssid,    , for disconect AP\n");
        test_rda5890_set_ssid(ssid,len);
        start_wifi_poll_timer();
        useingIrq = TRUE;
        return 0;
    }
    //struct bss_descriptor bss_desc;
    //tmp_ssid->rsn_cap[0] = 0x08;
    //tmp_ssid->rsn_cap[1] = 0x00;
    memset(&bss_desc_Used,0,sizeof(struct bss_descriptor ));
    memcpy(&(bss_desc_Used.data), tmp_ssid, sizeof(RDA5890_BSS_DESC));
    reconstruct_rsn_wpa_ie(&bss_desc_Used);
    MCD_TRACE(MCD_INFO_TRC, 0,  "reconstruct_rsn_wpa_ie :\n");
    if(bss_desc_Used.rsn_ie_len>0)
        sxs_Dump(_MMI, 0, bss_desc_Used.rsn_ie, bss_desc_Used.rsn_ie_len);

    w802_11imode =  tmp_ssid->dot11i_info;;


    if(w802_11imode>0)//encryption enabale
    {
        UINT8 authType =0;
        UINT8 iMode = 0;
        stop_wifiPollTimer();

        if(0x00 ==( w802_11imode&0xF0))//for WEP
        {

            iMode = 0x07;//0x03:WEP40, 0x07: WEP104
            test_rda5890_generic_set_uchar(WID_802_11I_MODE, iMode);
            MCD_TRACE(MCD_INFO_TRC, 0,"set WID_802_11I_MODE,   %d\n", iMode);
            /*
            if(-1== sdio_getInputData())
            {
                                restart_wifiPollTimer();

                 return -1;
            }*/
//auth type: 0x01 for open system; 2 for share key
            authType = 2;
//
            test_rda5890_generic_set_uchar(WID_AUTH_TYPE, authType);
            MCD_TRACE(MCD_INFO_TRC, 0,"set WID_AUTH_TYPE,   %d\n", authType);
            /*
            if(-1== sdio_getInputData())
            {
                         restart_wifiPollTimer();

            return -1;
            }*/
            test_rda5890_set_wepkey(0, wifi_password, strlen(wifi_password));
            // test_rda5890_set_wepKid(0);


        }
        else
        {

            //for WPA-AES(PSK)
            if(tmp_ssid->dot11i_info &0x10)
            {
                iMode = tmp_ssid->dot11i_info & (~0x08);


            }
            else if(tmp_ssid->dot11i_info &0x08)
            {
                iMode = tmp_ssid->dot11i_info & (~0x10);

            }

            //iMode = tmp_ssid->dot11i_info;
            test_rda5890_generic_set_uchar(WID_802_11I_MODE,iMode);
            MCD_TRACE(MCD_INFO_TRC, 0,"set WID_802_11I_MODE,   0x%x\n", iMode);
            /*
            if(-1== sdio_getInputData())
            {
                restart_wifiPollTimer();

                 return -1;
            }
            */
            authType = 1;

            test_rda5890_generic_set_uchar(WID_AUTH_TYPE,authType);
            MCD_TRACE(MCD_INFO_TRC, 0,"set WID_AUTH_TYPE,   %d\n", authType);
            /*
            if(-1== sdio_getInputData())
            {
                                restart_wifiPollTimer();

                    return -1;
            }*/
            initWifiWapSm();
            setWpaAddr(tmp_ssid->bssid, localMac,ssid);
            UINT32 pro =0;
            INT32 rsn_en=0;
            UINT32 pair_chip=0;
            if(tmp_ssid->dot11i_info &0x10)
            {
                pro = WPA_PROTO_RSN;
                rsn_en =1;
            }
            else if(tmp_ssid->dot11i_info &0x08)
            {

                pro = WPA_PROTO_WPA;
                rsn_en =0;

            }

            if(tmp_ssid->dot11i_info &0x20)
            {
                pair_chip = 0x10;
            }
            setWifiSmField(pro,rsn_en,pair_chip);

        }
    }
    MCD_TRACE(MCD_INFO_TRC, 0,"set ssid,   %d\n", strlen(tmp_ssid->ssid));
    test_rda5890_set_ssid(tmp_ssid->ssid,strlen(tmp_ssid->ssid));
    restart_wifiPollTimer();
    start_wifi_poll_timer();
    useingIrq = TRUE;
    return 0;
}
#endif
int test_start_associate_bss_Ex(RDA5890_BSS_DESC* tmp_bss, UINT8 len)
{
    MCD_TRACE(MCD_INFO_TRC, 0,  "test_start_associate_bss_Ex, useingIrq: %d\n",useingIrq);


    if(len != sizeof(RDA5890_BSS_DESC))
        return -1;
    RDA5890_BSS_DESC* tmp_ssid;
    tmp_ssid =(RDA5890_BSS_DESC*)( bss_Rst.bss_descs);
    UINT8 count = bss_Rst.count;
    while(count>0)
    {
        if((strcmp(tmp_ssid->ssid,tmp_bss->ssid)==0)&& (memcmp(tmp_ssid->bssid, tmp_bss->bssid, 6)==0))
        {
            memset(&bss_desc_Used,0,sizeof(struct bss_descriptor ));
            memcpy(&(bss_desc_Used.data), tmp_ssid, sizeof(RDA5890_BSS_DESC));

            break;
        }
        else
            tmp_ssid+=1;
        count--;
    }
    if(count ==0)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"ssid is not in scan list!\n");
        tmp_ssid = tmp_bss;
    }

//  if(useingIrq == TRUE)
//      useingIrq = FALSE;

    //struct bss_descriptor bss_desc;
    //tmp_ssid->rsn_cap[0] = 0x08;
    //tmp_ssid->rsn_cap[1] = 0x00;
//  memset(&bss_desc_Used,0,sizeof(struct bss_descriptor ));
//  memcpy(&(bss_desc_Used.data), tmp_ssid, sizeof(RDA5890_BSS_DESC));
    sxs_Dump(_MMI, 0, &(bss_desc_Used.data), sizeof(RDA5890_BSS_DESC));

    reconstruct_rsn_wpa_ie(&bss_desc_Used);
    MCD_TRACE(MCD_INFO_TRC, 0,  "reconstruct_rsn_wpa_ie :\n");
    if(bss_desc_Used.rsn_ie_len>0)
        sxs_Dump(_MMI, 0, bss_desc_Used.rsn_ie, bss_desc_Used.rsn_ie_len);

    // w802_11imode =  tmp_ssid->dot11i_info;
    w802_11imode = bss_desc_Used.data.dot11i_info;
    UINT8 authType =0;
    UINT8 iMode = 0;
    wifi_context.dot11_mode =0;
    wifi_context.auth_type =0;

    if(w802_11imode>0)//encryption enabale
    {

        //stop_wifiPollTimer();

        if(0x00 ==( w802_11imode&0xF8))//for WEP
        {

            //iMode = 0x07;//0x03:WEP40, 0x07: WEP104
            if((strlen(wifi_password) ==5) ||(strlen(wifi_password) ==10))
                iMode = 0x03;
            else if((strlen(wifi_password) ==13) ||(strlen(wifi_password) ==26))
                iMode = 0x05;
            else
                iMode = 0x01;

            wifi_context.dot11_mode  = iMode;
            MCD_TRACE(MCD_INFO_TRC, 0,"set WID_802_11I_MODE,   %d\n", wifi_context.dot11_mode);

            //test_rda5890_generic_set_uchar(WID_802_11I_MODE, wifi_context.dot11_mode);
            //sxr_Sleep(164*5);

//auth type: 0x01 for open system; 2 for share key
            //authType = 2;
            // wifi_context.auth_type = tmp_ssid->auth_info;
            wifi_context.auth_type =2;

//
            MCD_TRACE(MCD_INFO_TRC, 0,"set WID_AUTH_TYPE,   %d\n", wifi_context.auth_type);

            //test_rda5890_generic_set_uchar(WID_AUTH_TYPE, wifi_context.auth_type);
            //sxr_Sleep(164*5);
            MCD_TRACE(MCD_INFO_TRC, 0,"set WEP key\n");

            test_rda5890_set_wepkey(0, wifi_context.cur_pwd, strlen(wifi_context.cur_pwd));
            // test_rda5890_set_wepKid(0);
            //sxr_Sleep(164*5);



        }
        else
        {

            //for WPA-AES(PSK)
            if(tmp_ssid->dot11i_info &0x10)
            {
                //iMode = tmp_ssid->dot11i_info & (~0x08);
                iMode = 0x11;


            }
            else if(tmp_ssid->dot11i_info &0x08)
            {
                //iMode = tmp_ssid->dot11i_info & (~0x10);
                iMode = 0x09;

            }

            if(tmp_ssid->dot11i_info & 0x20)
            {

                iMode |= 0x20;
            }
            else if (tmp_ssid->dot11i_info & 0x40)
            {
                iMode |= 0x40;

            }
            /*else if (tmp_ssid->dot11i_info & 0x80)
            {
                    iMode |= 0x80;

            }*/
            wifi_context.dot11_mode= iMode;

            MCD_TRACE(MCD_INFO_TRC, 0,"set WID_802_11I_MODE,   0x%x\n", wifi_context.dot11_mode);

            //test_rda5890_generic_set_uchar(WID_802_11I_MODE,wifi_context.dot11_mode);
            //  sxr_Sleep(800);

            authType = 1;
            wifi_context.auth_type = 1;
            MCD_TRACE(MCD_INFO_TRC, 0,"set WID_AUTH_TYPE,   %d\n", wifi_context.auth_type );
            //  test_rda5890_generic_set_uchar(WID_AUTH_TYPE,wifi_context.auth_type );
            //  sxr_Sleep(800);

            initWifiWapSm();
            setWpaAddr(tmp_ssid->bssid, localMac,tmp_ssid->ssid);
            UINT32 pro =0;
            INT32 rsn_en=0;
            UINT32 pair_chip=0;
            UINT32 group_chiper=0;
            if(tmp_ssid->dot11i_info &0x10)
            {
                pro = WPA_PROTO_RSN;
                rsn_en =1;
            }
            else if(tmp_ssid->dot11i_info &0x08)
            {

                pro = WPA_PROTO_WPA;
                rsn_en =0;

            }

            if((tmp_ssid->dot11i_info &0x20)&&(!(tmp_ssid->dot11i_info & 0x40)))
            {
                group_chiper = 0x10; //WPA_CIPHER_CCMP
            }
            else
            {
                group_chiper = 0x08; //WPA_CIPHER_TKIP
            }

            if((tmp_ssid->dot11i_info &0x20)&&(tmp_ssid->dot11i_info & 0x40))
            {

                if((tmp_ssid->dot11i_info &0x80))

                    pair_chip= 0x10; //WPA_CIPHER_CCMP
                else
                    pair_chip= 0x10; //WPA_CIPHER_CCMP

                //pair_chip= 0x08; //WPA_CIPHER_TKIP

            }
            else if (tmp_ssid->dot11i_info &0x20)
            {
                pair_chip= 0x10; //WPA_CIPHER_CCMP
            }
            else
            {
                pair_chip = 0x08; //WPA_CIPHER_TKIP
            }
            setWifiSmField(pro,rsn_en,pair_chip,group_chiper);

        }
        auth_failed = TRUE;
    }
    MCD_TRACE(MCD_INFO_TRC, 0,"set ssid,   %d\n", strlen(tmp_ssid->ssid));
    //test_rda5890_set_ssid(tmp_ssid->ssid,strlen(tmp_ssid->ssid));
    //report_rssi_count = 15;
    sxs_Dump(_MMI, 0, wifi_context.cur_bssid, 6);

    test_rda5890_set_ssid_Ex(tmp_ssid->ssid,strlen(tmp_ssid->ssid));
    //  restart_wifiPollTimer();
    //  start_wifi_poll_timer();
    start_assoc_ap_timer(WIFI_ASSO_TIMEOUT/16384);
    // useingIrq = TRUE;
    wifi_PowerOffCount =0;

    return 0;
}



int test_rda5890_set_11b_only(UINT8 threshold)
{
    MCD_TRACE(MCD_INFO_TRC, 0, "test_rda5890_set_11b_only : 0x%x \n",threshold);


    UINT8 wid = WID_11G_OPERATING_MODE;
    UINT8 val =threshold;
    int ret;
    //UINT8 len;
    char wid_req[7 + sizeof(unsigned char)];
    memset(wid_req, 0x00,(7 + sizeof(unsigned char)));
    unsigned short wid_req_len = 7 + sizeof(unsigned char);
    //char wid_rsp[32];
    //memset(wid_rsp,0x00,32);
    //unsigned short wid_rsp_len = 32;
    char wid_msg_id = rda5890_wid->wid_msg_id;
    rda5890_wid->wid = wid;
    rda5890_wid->wid_rsp_len =RDA5890_MAX_WID_LEN;
    memset(rda5890_wid->wid_rsp,0x0,512);

    //MCD_TRACE(MCD_INFO_TRC, 0,"test_rda5890_generic_set_uchar ,wid_req: 0x%x,wid_req[0]=0x%x\n",wid_req, wid_req[0]);
    MCD_TRACE(MCD_INFO_TRC, 0,"test_rda5890_generic_set_uchar ,rda5890_wid->wid_msg_id: 0x%x,rda5890_wid->wid_pending: 0x%x\n",rda5890_wid->wid_msg_id,rda5890_wid->wid_pending);
    wid_req[0] = 'W';
    wid_req[1] = wid_msg_id;

    wid_req[2] = (char)(wid_req_len&0x00FF);
    wid_req[3] = (char)((wid_req_len&0xFF00) >> 8);

    wid_req[4] = (char)(wid&0x00FF);
    wid_req[5] = (char)((wid&0xFF00) >> 8);

    wid_req[6] = (char)(sizeof(unsigned char));
    memcpy(wid_req + 7, &val, sizeof(unsigned char));

    ret = rda5990_wid_request(wid_req, wid_req_len);
    if (ret)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"rda5890_wid_request fail, ret = %d\n", ret);
        //goto out;
    }

    /*


        test_rda5890_generic_set_uchar(WID_11G_OPERATING_MODE,threshold);


             if(-1== sdio_getInputData())
            {
            restart_wifiPollTimer();
            return -1;
            }
            */
    return 0;
}



#ifdef __SCAN_INFO__
BOOL get_networkinfo_in_asso = FALSE;

VOID checkWifiOptMode(UINT8 * mac)
{
    RDA5890_BSS_NETWORK_INFO* assoc_bss = NULL;
    assoc_bss = find_bss_info_node_Mac(mac);
    if(NULL!=assoc_bss)
    {
        if(is_ap_support_11b(assoc_bss->rates))
        {
            test_rda5890_set_11b_only(0x00);//2013-1-15, set 11b only
            MCD_TRACE(MCD_INFO_TRC, 0,"######## set 11b only !\n");
            hal_HstSendEvent(0x88880000);


        }
        else
        {
            MCD_TRACE(MCD_INFO_TRC, 0,"########  set auto! \n");
            hal_HstSendEvent(0x88880002);

            test_rda5890_set_11b_only(0x02);//2013-1-15, set  auto
        }
        //sxr_Sleep(163);
    }
    else
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"########  no found the bssid! \n");
        hal_HstSendEvent(0x88880003);

    }
    //rda5890_set_wlan_mode(priv, 2);
    return;
}
#endif
int re_associate_bss_Ex(VOID)
{
    MCD_TRACE(MCD_INFO_TRC, 0,  "re_associate_bss_Ex, useingIrq: %d\n",useingIrq);
#ifdef __SCAN_INFO__
    if(!get_networkinfo_in_asso)
        wifi_getNetworkInfo_Req();
    else
        checkWifiOptMode(wifi_context.cur_bssid);



#endif

    //if(useingIrq == TRUE)
    //  useingIrq = FALSE;

    if(((wifi_context.dot11_mode & 0xF8)==0x00)&&((wifi_context.dot11_mode & 0x01 )==0x01))//WEP encryption
    {
        UINT8 authType =0;
        UINT8 iMode = 0;

        //test_rda5890_generic_set_uchar(WID_802_11I_MODE, wifi_context.dot11_mode);
        //sxr_Sleep(164*5);
        MCD_TRACE(MCD_INFO_TRC, 0,"set WID_802_11I_MODE,   %d\n", wifi_context.dot11_mode);

        //auth type: 0x01 for open system; 2 for share key
        //authType = 2;
        // wifi_context.auth_type = tmp_ssid->auth_info;
        if(wifi_context.auth_type ==0x2)
            wifi_context.auth_type =0x1;
        else if(wifi_context.auth_type ==0x1)
            wifi_context.auth_type =0x2;
        //
        //test_rda5890_generic_set_uchar(WID_AUTH_TYPE, wifi_context.auth_type);
        //sxr_Sleep(164*5);

        MCD_TRACE(MCD_INFO_TRC, 0,"set WID_AUTH_TYPE,   %d\n", wifi_context.auth_type);
        test_rda5890_set_wepkey(0, wifi_context.cur_pwd, strlen(wifi_context.cur_pwd));
        // test_rda5890_set_wepKid(0);
        //sxr_Sleep(164*5);

        MCD_TRACE(MCD_INFO_TRC, 0,"set ssid,   %d\n", strlen(wifi_context.cur_ssid));
        //test_rda5890_set_ssid(wifi_context.cur_ssid,strlen(wifi_context.cur_ssid));
        test_rda5890_set_ssid_Ex(wifi_context.cur_ssid,strlen(wifi_context.cur_ssid));


    }
    else
    {
        test_rda5890_set_ssid_Ex(wifi_context.cur_ssid,strlen(wifi_context.cur_ssid));


    }

    if(associate_tries < (50*16384/WIFI_ASSO_TIMEOUT-1))
    {

        start_assoc_ap_timer(WIFI_ASSO_TIMEOUT/16384);

        associate_tries++;
    }

    //useingIrq = TRUE;

    return 0;
}

void get_assoWpaIe(UINT8 * ie, UINT8* len)
{
    if(ie==0)
        return;
    memcpy(ie,bss_desc_Used.wpa_ie,bss_desc_Used.wpa_ie_len);
    *len = bss_desc_Used.wpa_ie_len;
}

void get_assoRsnIe(UINT8 * ie, UINT8* len)
{
    if(ie==0)
        return;
    memcpy(ie,bss_desc_Used.rsn_ie,bss_desc_Used.rsn_ie_len);
    *len = bss_desc_Used.rsn_ie_len;
}

#define WID_PTA_MODE            0x0042
#define WID_PTA_BLOCK_BT        0x0044
#define WID_ACTIVE_SCAN_TIME    0x100c


INT  test_rda5890_disable_self_cts(void)
{
    MCD_TRACE(MCD_INFO_TRC, 0,  "test_rda5890_set_self_cts...mode\n");
    test_rda5890_generic_set_uchar(WID_PTA_MODE,0);
    if(-1== sdio_getInputData())
    {
        restart_wifiPollTimer();
        return -1;
    }
    return 0;
}



INT  test_rda5890_disable_block_bt(void)
{
    MCD_TRACE(MCD_INFO_TRC, 0,  "test_rda5890_disable_block_bt...mode\n");
    test_rda5890_generic_set_uchar(WID_PTA_BLOCK_BT,0);
    if(-1== sdio_getInputData())
    {
        restart_wifiPollTimer();
        return -1;
    }
    return 0;
}


void  test_rda5890_set_active_scan_time(void)
{
    MCD_TRACE(MCD_INFO_TRC, 0,  "test_rda5890_set_active_scan_time...\n");
    test_rda5890_generic_set_ushort(WID_ACTIVE_SCAN_TIME,80);
    if(-1== sdio_getInputData())
    {
        restart_wifiPollTimer();
        return;
    }
    return ;
}


int test_rda5890_set_link_loss_threshold(UINT8 threshold)
{
    MCD_TRACE(MCD_INFO_TRC, 0, "rda5890_set_link_loss_threshold : 0x%x \n",threshold);
    test_rda5890_generic_set_uchar(WID_LINK_LOSS_THRESHOLD,threshold);
    if(-1== sdio_getInputData())
    {
        restart_wifiPollTimer();
        return -1;
    }
    return 0;
}

int test_rda5890_set_Active_Scan_Time(UINT8 val)
{
    MCD_TRACE(MCD_INFO_TRC, 0, "test_rda5890_set_Active_Scan_Time : 0x%x \n",val);
    test_rda5890_generic_set_uchar(WID_ACTIVE_SCAN_TIME,val);
    if(-1== sdio_getInputData())
    {
        restart_wifiPollTimer();
        return -1;
    }
    return 0;
}

/***********************************************************/
/*test sdio card, end                                  */
/***********************************************************/
//





///////////////////////////////////////////////////////
/*for application*/



void wifi_init(void);

UINT32 wifi_PowerOnRsp(void);
void init_randomMAC(void)
{
    UINT8 * tmp;
    tmp =localMac;
    UINT8 t =0;
    UINT32 startTime ;
    //startTime = hal_TimGetUpTime();

    for(t=0; t<6; t++)
    {
        startTime = hal_TimGetUpTime();

        startTime = (startTime*123+ t*123);
        srand(startTime);
        tmp[t] = rand()& 0xff;
        if(tmp[t]==0x00)
            tmp[t] =0x11;
    }
//   tmp[0] &=0xFE;
//   tmp[0] |=0x20;

    tmp[0] =0x00;
}
//void wifi_Read_core_dataEx(void);
//VOID write_wifi_Core_dataEx(VOID);
#if 1
BOOL first_connect=FALSE;



VOID init_wifi_context_Para(VOID)
{
    rda5890_connected = FALSE;
    first_connect=FALSE;
    g_SdioRcaReg = 0x00000000;


    g_SdioFreq = 200000;

    g_SdioIsDmaEnable = FALSE;
    g_mcdStatus = MCD_STATUS_NOTOPEN_PRESENT;

    if(g_SdioSemaphore   != 0xFF)
    {
        sxr_FreeSemaphore(g_SdioSemaphore);
        g_SdioSemaphore =0xFF;

    }


// g_SdioTransfer =  {0,0, 0,   0,    HAL_SDIO_DIRECTION_WRITE };

//PRIVATE MCD_CSD_T g_mcdLatestCsd;
#ifdef __SCAN_INFO__
    getting_network_info = FALSE;
#endif
    ioport_w= 0x07;
    ioport_r= 0x08;
    ioport  =0x00;
    wifiDataInd = 0x00;

    wifi_context.firstWrite = TRUE;
    useingIrq = FALSE;
    test_numn =0;
    in_interruptHandle = FALSE;
//extern void * memset(void *,int,size_t);
//PUBLIC VOID *memcpy(VOID *dest, CONST VOID *src, UINT32 n);
    tx_data_counter =0x00;
    rx_data_counter =0x00;
    transfer_start_time = 0x00;
    wifi_sleep_flag = FALSE;
    sdio_sleep_flag = FALSE;
    wifi_sleep_enable = FALSE;
    wifi_sleep_enable_flag = 0x00; //0x00: wake up; 0x01: pre-association sleep; 0x10: post-association sleep
    ap_connected =FALSE;
    ap_connecting =FALSE;
    ap_disconnecting =FALSE;
    preasso_timer_enable = TRUE;

    gettingRssi = FALSE;
    wifi_PowerOffCount  = 0;
    auth_failed = FALSE;
    report_rssi_count =0 ;


    wifi_enableSleepTimerCount =0;

//tmp_read_data[16][1664] = {{0x00,},};

    tmp_read_data_flag =0x0000;
    associate_tries = 0;
    memset(&bss_Rst,0x00,sizeof(bss_Rst));

}


BOOL reset_req = FALSE;
VOID wifi_PowerOnEx_2(VOID);
UINT32 wifi_Read_MAC_Reg_I2C(UINT32 reg);
UINT32 wifi_PowerOffRsp(void);
UINT32 wifi_PowerOnFail(void);
int wifi_ResetReqEx(VOID);
PUBLIC UINT32 hal_SdioGetChipId(VOID);
int wifi_mac_data_txReq(UINT8 *data, unsigned short len);
UINT8 tmpdata[1025];
VOID wifi_PowerOnEx_1(VOID)
{
    MCD_CSD_T  mmc_csd;
    first_connect = TRUE;
    init_wifi_context_Para();
    test_rda5890_wid_init();
    wifi_init();
#ifdef USE_SDIO_DMA
    g_SdioIsDmaEnable = hal_SdioDmaIsEnable();
#endif

//    MCD_TRACE(MCD_INFO_TRC, 0,"########## after wifi_init, chip id: 0x%x;  dma enable: %d\n",hal_SdioGetChipId(),g_SdioIsDmaEnable);
    if(SDIO_mcd_Open(&mmc_csd)==MCD_ERR_NO_CARD)
    {
        wifi_Read_MAC_Reg_I2C(0x50090048);
        wifi_PowerOnFail();
        //wifi_ResetReqEx();
        return;
    }
    MCD_TRACE(MCD_INFO_TRC, 0,"########## after SDIO_mcd_Open, get disconnect ind\n");

    //hal_sdioDataInIrqMaskSet();
    if(-1== sdio_getInputData())
    {
        MCD_TRACE(MCD_INFO_TRC, 0,  "Ssdio_getInputData, -1\n");

        //  restart_wifiPollTimer();
//hal_sdioDataInIrqMaskSet();

    }
#ifdef USB_WIFI_SUPPORT
    MCD_TRACE(MCD_INFO_TRC, 0,"##########  USB_WIFI_SUPPORT\n");
    write_wifi_Core_dataEx();
    wifi_init_patch_dataEx();

    rda5890_WifWakeUp();
    test_rda5890_set_preasso_sleep(0x01000020);//enable wifi sleep pre-association set preasso time to (2^5)/10 s
//0x00800080
    test_rda5890_set_pm_mode(2);// //add 2012 06 14  disable wf sleep
    test_rda5890_disable_self_cts();
    test_rda5890_set_active_scan_time();
    test_rda5890_disable_block_bt();
    test_rda5890_set_link_loss_threshold(0x50);
    test_rda5890_set_Active_Scan_Time(50);

    start_wifi_poll_timer();

    sxr_Sleep(200*16384/1000);
    useingIrq = TRUE;
    //MCD_TRACE(MCD_INFO_TRC, 0,"##########   set irq \n");
    hal_sdioDataInIrqMaskSet();
    pool_dataIn = TRUE;

    //MCD_TRACE(MCD_INFO_TRC, 0,"##########   start scan \n");
//sxr_Sleep(5000*16384/1000);
//test_rda5890_start_scan();
//sxr_Sleep(1000);
//test_rda5890_set_11b_only(0x00);//2012-12-31, set 11b only
//wifi_mac_data_txReq(tmpdata,1505);

    if(!reset_req )
        wifi_PowerOnRsp();
    else
    {
        reset_req = FALSE;
        wifi_ResetInd();
    }





#else
    wifi_PowerOnEx_2();
#endif


}

VOID wifi_PowerOnEx_2(VOID)
{
    INT32 ret =0;
    UINT8 ssid[32];
    UINT8 widlen;
    // hal_sdioDataInIrqMaskClear();

    write_wifi_Core_dataEx();
    wifi_init_patch_dataEx();

    rda5890_WifWakeUp();
    test_rda5890_set_preasso_sleep(0x01000020);//enable wifi sleep pre-association set preasso time to (2^5)/10 s
//0x00800080
    test_rda5890_set_pm_mode(2);// //add 2012 06 14  disable wf sleep
    test_rda5890_disable_self_cts();
    test_rda5890_set_active_scan_time();
    test_rda5890_disable_block_bt();
    test_rda5890_set_link_loss_threshold(0x50);
    test_rda5890_set_Active_Scan_Time(50);


#ifdef __11B_ONLY__
    MCD_TRACE(MCD_INFO_TRC, 0,"########## 11b only\n");
    test_rda5890_set_11b_only(0x00);//2012-12-31, set 11b only
#else
    MCD_TRACE(MCD_INFO_TRC, 0,"##########   NO 11b only, \n");

#endif
    start_wifi_poll_timer();
    memset(ssid,0x00,sizeof(ssid));
    MCD_TRACE(MCD_INFO_TRC, 0,"########## geting ssid\n");
    UINT16 *tmpval = localMac;
    if((tmpval[0]==0x00) &&(tmpval[1]==0x00)&&(tmpval[2]==0x00))
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"##########get random MAC:  \n");

        init_randomMAC();
    }
    MCD_TRACE(MCD_INFO_TRC, 0,"########## local MAC:  \n");
    sxs_Dump(_MMI, 0, localMac, 6);
    MCD_TRACE(MCD_INFO_TRC, 0,"########## set local MAC:  \n");

    wifi_setMac(localMac,6);
    test_rda5890_start_scan();

    sxr_Sleep(200*16384/1000);
    useingIrq = TRUE;
    hal_sdioDataInIrqMaskSet();

    //test_rda5890_SetWifiSleepEx();
    if(!reset_req )
        wifi_PowerOnRsp();
    else
    {
        reset_req = FALSE;
        wifi_ResetInd();
    }

}
#if 0
VOID wifi_PowerOnEx(VOID)
{
    MCD_CSD_T  mmc_csd;
    first_connect = TRUE;
    init_wifi_context_Para();
    wifi_init();
    MCD_TRACE(MCD_INFO_TRC, 0,"########## after wifi_init\n");

    //sxr_Sleep(20*16384);

    //wifi_setTransMode();



    SDIO_mcd_Open(&mmc_csd);
    test_rda5890_wid_init();
    //  wifi_Write_MAC_Reg_I2C(0x5030002c,0xf808);
    hal_sdioDataInIrqMaskSet();
    sxr_Sleep(20*16384/1000);

    /* wifi_Read_MAC_Reg_I2C(0x00100448);

           wifi_Read_MAC_Reg_I2C(0x50300014);
           wifi_Read_MAC_Reg_I2C(0x50300028);
    wifi_Read_MAC_Reg_I2C(0x50300038);

        UINT32 tmp_addr = 0x00000000;

    //             tmp_addr = 0x00000000;
           tmp_addr = tmp_addr |(1<<28) |( 0x05 <<9);
           read_test_cmd(52,tmp_addr);

    //wifi_Read_core_dataEx();

    */
    INT32 ret =0;
    UINT8 ssid[32];
    UINT8 widlen;
    hal_sdioDataInIrqMaskClear();

//sxr_Sleep(5*16384);

    write_wifi_Core_dataEx();
//  wifi_init_patch_data();
    wifi_init_patch_dataEx();
    //sxr_Sleep(20*16384/1000);

//  wifi_init_patch_data2();//disable wifi chip wake up BB chip, should match the setting ' hwp_sysCtrl->Clk_Per_Mode = 0x40 " in the hal_open
    /*  UINT32 tmp_addr = 0x88000000;
               tmp_addr = tmp_addr |(1<<28) |( 0x09 <<9)|0x01;
               send_test_cmd(52,tmp_addr);
    */

    rda5890_WifWakeUp();


    //test_rda5890_set_preasso_sleep(0x060000FF);//enable wifi sleep pre-association set preasso time to (2^7)/10 s
    //test_rda5890_set_preasso_sleep(0x08000080);//enable wifi sleep pre-association set preasso time to (2^7)/10 s
#if 1
    test_rda5890_set_preasso_sleep(0x01000020);//enable wifi sleep pre-association set preasso time to (2^5)/10 s

    test_rda5890_set_pm_mode(2);// //add 2012 06 14  disable wf sleep
#endif
    //test_rda5890_disable_self_cts(); // disable self_cts mode
    test_rda5890_disable_self_cts();
    test_rda5890_set_active_scan_time();
    test_rda5890_disable_block_bt();
    test_rda5890_set_link_loss_threshold(0x50);
    start_wifi_poll_timer();

//  test_rda5890_set_preasso_sleep(0x00);//enable wifi sleep pre-association set preasso time to (2^5)/10 s

//  test_rda5890_set_pm_mode(0);// enable sleep post-association.  0, disbable


    //  test_rda5890_set_pm_mode(2);// enable sleep post-association.  0, disbable

    //return ;
//  wifi_init_patch_Sleep();

    //add 2012-1-14
// write_wifi_patch_data();
// write_wifi_patch_dataEx();
//add end
    memset(ssid,0x00,sizeof(ssid));
    MCD_TRACE(MCD_INFO_TRC, 0,"########## geting ssid\n");
    /*
    ret = test_rda5890_generic_get_str(WID_SSID, ssid, &widlen);
        MCD_TRACE(MCD_INFO_TRC, 0,"##########got  ssid: %s \n");

    if(0x00 !=ret)
        goto errorwifi;
    */
    UINT16 *tmpval = localMac;
    if((tmpval[0]==0x00) &&(tmpval[1]==0x00)&&(tmpval[2]==0x00))
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"##########get random MAC:  \n");

        init_randomMAC();
    }
    MCD_TRACE(MCD_INFO_TRC, 0,"########## local MAC:  \n");
    sxs_Dump(_MMI, 0, localMac, 6);
    MCD_TRACE(MCD_INFO_TRC, 0,"########## set local MAC:  \n");

    wifi_setMac(localMac,6);
    /*
                MCD_TRACE(MCD_INFO_TRC, 0,"########## geting local MAC\n");

            ret = test_rda5890_get_mac_addr(localMac);
        if(0x00 !=ret)
        {
            MCD_TRACE(MCD_INFO_TRC, 0,"wifi_APconnect, error: %d \n");
        }else{
            MCD_TRACE(MCD_INFO_TRC, 0,"##########got local MAC:  \n");
            sxs_Dump(_MMI, 0, localMac, 6);
        }
    */

//test_rda5890_SetWifiSleepEx();
    test_rda5890_start_scan();
    sxr_Sleep(500*16384/1000);
    //test_rda5890_SetWifiSleepEx();
    wifi_PowerOnRsp();
    useingIrq = TRUE;
    hal_sdioDataInIrqMaskSet();
    //wifi_MAC_sleep();


//  hal_sdioDataInIrqMaskSet();
//  test_rda5890_SetWifiSleepEx();
    return;

}
#endif
void wifi_close(void);
VOID wifi_ResetInd(VOID);
VOID wifi_Reset_2(VOID);
VOID wifi_Reset_1(VOID)
{
    reset_req = TRUE;
    MCD_TRACE(MCD_INFO_TRC, 0,"########## wifi_Reset_1\n");
    hal_sdioDataInIrqMaskClear();
    MCD_CSD_T  mmc_csd;
    wifi_close();
    first_connect = TRUE;
    init_wifi_context_Para();
    test_rda5890_wid_init();
    wifi_init();
    MCD_TRACE(MCD_INFO_TRC, 0,"########## after wifi_init\n");
    SDIO_mcd_Open(&mmc_csd);
    MCD_TRACE(MCD_INFO_TRC, 0,"########## after SDIO_mcd_Open, get disconnect ind\n");

    //hal_sdioDataInIrqMaskSet();
    if(-1== sdio_getInputData())
    {
        MCD_TRACE(MCD_INFO_TRC, 0,  "Ssdio_getInputData, -1\n");

        //  restart_wifiPollTimer();
//hal_sdioDataInIrqMaskSet();

    }
//wifi_Reset_2();
    wifi_PowerOnEx_2();
    return;

}
VOID wifi_Reset_2(VOID)
{
    MCD_TRACE(MCD_INFO_TRC, 0,"########## wifi_Reset_2\n");
    INT32 ret =0;
    UINT8 ssid[32];
    UINT8 widlen;
    // hal_sdioDataInIrqMaskClear();


    write_wifi_Core_dataEx();
    wifi_init_patch_dataEx();

    rda5890_WifWakeUp();

#if 1
    test_rda5890_set_preasso_sleep(0x01000020);//enable wifi sleep pre-association set preasso time to (2^5)/10 s

    test_rda5890_set_pm_mode(2);// //add 2012 06 14  disable wf sleep
#endif
    test_rda5890_disable_self_cts();
    test_rda5890_set_active_scan_time();
    test_rda5890_disable_block_bt();
    test_rda5890_set_link_loss_threshold(0x50);
    start_wifi_poll_timer();

    memset(ssid,0x00,sizeof(ssid));
    MCD_TRACE(MCD_INFO_TRC, 0,"########## geting ssid\n");
    UINT16 *tmpval = localMac;
    if((tmpval[0]==0x00) &&(tmpval[1]==0x00)&&(tmpval[2]==0x00))
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"##########get random MAC:  \n");

        init_randomMAC();
    }
    MCD_TRACE(MCD_INFO_TRC, 0,"########## local MAC:  \n");
    sxs_Dump(_MMI, 0, localMac, 6);
    MCD_TRACE(MCD_INFO_TRC, 0,"########## set local MAC:  \n");

    wifi_setMac(localMac,6);

    test_rda5890_start_scan();

    useingIrq = TRUE;
    hal_sdioDataInIrqMaskSet();
    sxr_Sleep(500*16384/1000);
    wifi_ResetInd();
    return;
    return;

}
VOID wifi_Reset(VOID)
{
    MCD_TRACE(MCD_INFO_TRC, 0,"########## wifi_Reset\n");

    MCD_CSD_T  mmc_csd;
    wifi_close();
    first_connect = TRUE;
    sxr_Sleep(500*16384/1000);

    wifi_init();
    SDIO_mcd_Open(&mmc_csd);
    test_rda5890_wid_init();
    hal_sdioDataInIrqMaskSet();
    sxr_Sleep(20*16384/1000);

    INT32 ret =0;
    UINT8 ssid[32];
    UINT8 widlen;
    hal_sdioDataInIrqMaskClear();


    write_wifi_Core_dataEx();
    wifi_init_patch_dataEx();

    rda5890_WifWakeUp();

#if 1
    test_rda5890_set_preasso_sleep(0x01000020);//enable wifi sleep pre-association set preasso time to (2^5)/10 s

    test_rda5890_set_pm_mode(2);// //add 2012 06 14  disable wf sleep
#endif
    test_rda5890_disable_self_cts();
    test_rda5890_set_active_scan_time();
    test_rda5890_disable_block_bt();
    test_rda5890_set_link_loss_threshold(0x50);
    start_wifi_poll_timer();

    memset(ssid,0x00,sizeof(ssid));
    MCD_TRACE(MCD_INFO_TRC, 0,"########## geting ssid\n");
    UINT16 *tmpval = localMac;
    if((tmpval[0]==0x00) &&(tmpval[1]==0x00)&&(tmpval[2]==0x00))
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"##########get random MAC:  \n");

        init_randomMAC();
    }
    MCD_TRACE(MCD_INFO_TRC, 0,"########## local MAC:  \n");
    sxs_Dump(_MMI, 0, localMac, 6);
    MCD_TRACE(MCD_INFO_TRC, 0,"########## set local MAC:  \n");

    wifi_setMac(localMac,6);

    test_rda5890_start_scan();
    sxr_Sleep(500*16384/1000);
    useingIrq = TRUE;
    hal_sdioDataInIrqMaskSet();
    wifi_ResetInd();
    return;

}
#else
VOID wifi_PowerOn(VOID)
{
    MCD_CSD_T  mmc_csd;

    wifi_init();
    MCD_TRACE(MCD_INFO_TRC, 0,"########## after wifi_init\n");

    //sxr_Sleep(20*16384);

    //wifi_setTransMode();
    SDIO_mcd_Open(&mmc_csd);
    test_rda5890_wid_init();
    //  wifi_Write_MAC_Reg_I2C(0x5030002c,0xf808);
    hal_sdioDataInIrqMaskSet();
    sxr_Sleep(16384);

    /* wifi_Read_MAC_Reg_I2C(0x00100448);

           wifi_Read_MAC_Reg_I2C(0x50300014);
           wifi_Read_MAC_Reg_I2C(0x50300028);
    wifi_Read_MAC_Reg_I2C(0x50300038);

        UINT32 tmp_addr = 0x00000000;

    //             tmp_addr = 0x00000000;
           tmp_addr = tmp_addr |(1<<28) |( 0x05 <<9);
           read_test_cmd(52,tmp_addr);

    //wifi_Read_core_dataEx();

    */
    INT32 ret =0;
    UINT8 ssid[32];
    UINT8 widlen;
    hal_sdioDataInIrqMaskClear();

//sxr_Sleep(5*16384);


    wifi_init_patch_data();
    wifi_init_patch_data2();//disable wifi chip wake up BB chip, should match the setting ' hwp_sysCtrl->Clk_Per_Mode = 0x40 " in the hal_open
    /*  UINT32 tmp_addr = 0x88000000;
               tmp_addr = tmp_addr |(1<<28) |( 0x09 <<9)|0x01;
               send_test_cmd(52,tmp_addr);
    */

    rda5890_WifWakeUp();


    test_rda5890_set_preasso_sleep(0x01000020);//enable wifi sleep pre-association set preasso time to (2^5)/10 s
    test_rda5890_set_pm_mode(2);// enable sleep post-association.  0, disbable
//  test_rda5890_set_preasso_sleep(0x00);//enable wifi sleep pre-association set preasso time to (2^5)/10 s

//  test_rda5890_set_pm_mode(0);// enable sleep post-association.  0, disbable


    //  test_rda5890_set_pm_mode(2);// enable sleep post-association.  0, disbable

    //return ;
//  wifi_init_patch_Sleep();

    //add 2012-1-14
// write_wifi_patch_data();
// write_wifi_patch_dataEx();
//add end
    memset(ssid,0x00,sizeof(ssid));
    MCD_TRACE(MCD_INFO_TRC, 0,"########## geting ssid\n");
    /*
    ret = test_rda5890_generic_get_str(WID_SSID, ssid, &widlen);
        MCD_TRACE(MCD_INFO_TRC, 0,"##########got  ssid: %s \n");

    if(0x00 !=ret)
        goto errorwifi;
    */
    UINT16 *tmpval = localMac;
    if((tmpval[0]==0x00) &&(tmpval[1]==0x00)&&(tmpval[2]==0x00))
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"##########get random MAC:  \n");

        init_randomMAC();
    }
    MCD_TRACE(MCD_INFO_TRC, 0,"########## local MAC:  \n");
    sxs_Dump(_MMI, 0, localMac, 6);
    MCD_TRACE(MCD_INFO_TRC, 0,"########## set local MAC:  \n");

    wifi_setMac(localMac,6);

    MCD_TRACE(MCD_INFO_TRC, 0,"########## geting local MAC\n");

    ret = test_rda5890_get_mac_addr(localMac);
    if(0x00 !=ret)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"wifi_APconnect, error: %d \n");
    }
    else
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"##########got local MAC:  \n");
        sxs_Dump(_MMI, 0, localMac, 6);
    }
//test_rda5890_SetWifiSleepEx();

    //test_rda5890_SetWifiSleepEx();
    wifi_PowerOnRsp();
    useingIrq = TRUE;
    hal_sdioDataInIrqMaskSet();


//  hal_sdioDataInIrqMaskSet();
//  test_rda5890_SetWifiSleepEx();
    return;

}
#endif
//UINT8 rda_ssid[]="wl_test";
//UINT8 *wifi_password = "wl_te";

//UINT8 *wifi_password = "wl_test123456";
//UINT8 *wifi_password = "5555555555";


//void wifi_Read_MAC_Reg_I2C(UINT32 reg);
//void wifi_Write_MAC_Reg_I2C_rf(UINT8 reg);
UINT32 hal_sdioDataInIrqMaskGet(VOID);

//EV_CFW_WIFI_SCAN_REQ
extern BOOL wifi_poll_timer_start ;
#if 1
VOID start_wifi_scanWait_timer(UINT8 dur);
VOID wifi_ScanApsEx_1(UINT8 dur, UINT8* ssid)
{

#ifdef __SCAN_INFO__
    if(getting_network_info)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,  "getting network information...\n");

        test_rda5890_set_NetworkInfo(0);
        stop_get_network_info_timer();
        sxr_Sleep(10*16384/1000);
    }
#endif
    //BOOL starttimer =wifi_poll_timer_start;
    //stop_wifi_poll_timer();
    int ret =0x00;
    MCD_TRACE(MCD_INFO_TRC, 0,"##########starting scan network. wifi_ScanApsEx_1\n");
    report_rssi_count = 15;
    wifi_PowerOffCount =0;
    UINT32 tmp;
    //hal_sdioDataInIrqMaskSet();
    preasso_timer_enable = FALSE;
    memset(&bss_Rst,0x00,sizeof(bss_Rst));
    memset(wifi_context.cur_ssid, 0x00, sizeof(wifi_context.cur_ssid));
    if(ssid != NULL)
        memcpy(wifi_context.cur_ssid, ssid, strlen(ssid));
    test_rda5890_start_scan();
    start_wifi_scanWait_timer( dur);
    if(ret!=0x00)
    {
        wifi_ScanRsp();
        //  if(starttimer)
        //      start_wifi_poll_timer();
        return;

    }



}
VOID wifi_ScanApsEx_2(VOID)
{
    //BOOL starttimer =wifi_poll_timer_start;
    //stop_wifi_poll_timer();
    int ret =0x00;
    MCD_TRACE(MCD_INFO_TRC, 0,"##########starting wifi_ScanApsEx_2 \n");

    MCD_TRACE(MCD_INFO_TRC, 0,"##########showing scan result: \n");
    preasso_timer_enable = TRUE;

    ret = wifi_get_str_from_MAC(WID_SITE_SURVEY_RESULTS);
    if(ret!=0x00)
    {
        wifi_ScanRsp();
    }
    //if(starttimer)
    //start_wifi_poll_timer();

}
/*
VOID wifi_ScanApsEx(UINT8 dur)
{
    //BOOL starttimer =wifi_poll_timer_start;
    //stop_wifi_poll_timer();
    int ret =0x00;
    MCD_TRACE(MCD_INFO_TRC, 0,"##########starting scan network. \n");
report_rssi_count = 15;
wifi_PowerOffCount =0;
    UINT32 tmp;
    //hal_sdioDataInIrqMaskSet();
preasso_timer_enable = FALSE;
    test_rda5890_start_scan();
    memset(&bss_Rst,0x00,sizeof(bss_Rst));
    if(ret!=0x00)
    {
        wifi_ScanRsp();
    //  if(starttimer)
    //      start_wifi_poll_timer();
        return;

    }

    sxr_Sleep(dur*16384);

    MCD_TRACE(MCD_INFO_TRC, 0,"##########showing scan result: \n");
    preasso_timer_enable = TRUE;

    ret = wifi_get_str_from_MAC(WID_SITE_SURVEY_RESULTS);
    if(ret!=0x00)
    {
        wifi_ScanRsp();
    }
    //if(starttimer)
        //start_wifi_poll_timer();

}
*/
#else
VOID wifi_ScanAps(VOID)
{
    BOOL starttimer =wifi_poll_timer_start;
    stop_wifi_poll_timer();

    MCD_TRACE(MCD_INFO_TRC, 0,"##########starting scan network. \n");
// wifi_Read_MAC_Reg_I2C(0x50000580);
// wifi_Read_MAC_Reg_I2C(0x3001003c);

//  wifi_Read_MAC_Reg_I2C(0x50000584);

//  wifi_Read_MAC_Reg_I2C(0x50000008);
//  wifi_Read_MAC_Reg_I2C(0x20000038);


    //   wifi_Read_MAC_Reg_I2C(0x00106A30);
    //     wifi_Read_MAC_Reg_I2C(0x00106A34);
    //  wifi_Read_MAC_Reg_I2C(0x00100448);
    wifi_Read_MAC_Reg_I2C(0x50300038);
    wifi_Read_MAC_Reg_I2C(0x00100448);
    //    wifi_Read_MAC_Reg_I2C(0x50300038);

//wifi_Write_MAC_Reg_I2C_rf(0xB9);


    UINT32 tmp;
    tmp =hal_sdioDataInIrqMaskGet();
    MCD_TRACE(MCD_INFO_TRC, 0,"##########int mask: 0x%x \n",tmp);
    hal_sdioDataInIrqMaskSet();
    tmp =hal_sdioDataInIrqMaskGet();
    MCD_TRACE(MCD_INFO_TRC, 0,"##########int mask: 0x%x \n",tmp);

    test_rda5890_start_scan();

    sxr_Sleep(5*16384);
    /*

           UINT32 tmp_addr = 0x00000000;
               tmp_addr = tmp_addr |(1<<28) |( 0x06 <<9);
               read_test_cmd(52,tmp_addr);

    */

    MCD_TRACE(MCD_INFO_TRC, 0,"##########showing scan result: \n");
    memset(&bss_Rst,0x00,sizeof(bss_Rst));

    wifi_get_str_from_MAC(WID_SITE_SURVEY_RESULTS);
    //check_scan_result();
    if(starttimer)
        start_wifi_poll_timer();

}

#endif
#if 1
/*
VOID wifi_ConnectApEx_0(wifi_conn_t* conn)
{
MCD_TRACE(MCD_INFO_TRC, 0,"wifi_ConnectAp\n");

    memset(wifi_context.cur_pwd,0x00,64);
    memset(wifi_context.cur_ssid,0x00,33);
    memcpy(wifi_context.cur_ssid,conn->ssid, conn->ssid_len);
    memcpy(wifi_context.cur_pwd,conn->password, conn->pwd_len);

    wifi_password = wifi_context.cur_pwd;
    test_start_associate_bss(conn->ssid, conn->ssid_len);



    hal_sdioDataInIrqMaskSet();
    useingIrq = TRUE;
}*/
#else
VOID wifi_ConnectAp(wifi_conn_t* conn)
{
    MCD_TRACE(MCD_INFO_TRC, 0,"wifi_ConnectAp\n");

    memset(wifi_context.cur_pwd,0x00,64);
    memset(wifi_context.cur_ssid,0x00,33);
    memcpy(wifi_context.cur_ssid,conn->ssid, conn->ssid_len);
    memcpy(wifi_context.cur_pwd,conn->password, conn->pwd_len);

    wifi_password = wifi_context.cur_pwd;
    test_start_associate_bss(conn->ssid, conn->ssid_len);



    hal_sdioDataInIrqMaskSet();
    useingIrq = TRUE;
}
#endif
#if 0
VOID wifi_ConnectApEx_1(RDA5890_BSS_DESC* tmp_bss, UINT8 len, wifi_conn_t* conn)
{
    MCD_TRACE(MCD_INFO_TRC, 0,"wifi_ConnectApEx, pwd: \n");


    memset(wifi_context.cur_pwd,0x00,64);
    memset(wifi_context.cur_ssid,0x00,33);
    memcpy(wifi_context.cur_ssid,conn->ssid, conn->ssid_len);
    memcpy(wifi_context.cur_pwd,conn->password, conn->pwd_len);

    wifi_password = wifi_context.cur_pwd;
    showString(wifi_password);

    test_start_associate_bss_Ex(tmp_bss, len);



    hal_sdioDataInIrqMaskSet();
    useingIrq = TRUE;
}
#else
void store_ConnectInfo(RDA5890_BSS_DESC* tmp_bss, UINT8 len, wifi_conn_t* conn)
{
    MCD_TRACE(MCD_INFO_TRC, 0,"store_ConnectInfo \n");

    memset(wifi_context.cur_pwd,0x00,64);
    memset(wifi_context.cur_ssid,0x00,33);
    memset(wifi_context.WEP0,0x00,32);
    memcpy(wifi_context.cur_ssid,conn->ssid, conn->ssid_len);
    memcpy(wifi_context.cur_pwd,conn->password, conn->pwd_len);
    memcpy(wifi_context.cur_bssid,tmp_bss->bssid, ETH_ALEN);

    wifi_password = wifi_context.cur_pwd;
    MCD_TRACE(MCD_INFO_TRC, 0,"ssid, pwd: \n");
    showString(wifi_context.cur_ssid);

    showString(wifi_context.cur_pwd);

    auth_failed = FALSE;
    //test_rda5890_set_preasso_sleep(0x01000020);//enable wifi sleep pre-association set preasso time to (2^5)/10 s
    memset(&bss_desc_Used,0,sizeof(struct bss_descriptor ));
    memcpy(&(bss_desc_Used.data), tmp_bss, sizeof(RDA5890_BSS_DESC));
}



VOID wifi_ConnectApEx_wifi(RDA5890_BSS_DESC* tmp_bss, UINT8 len, wifi_conn_t* conn, UINT16 dur)
{
    //test_rda5890_start_scan();
    //sxr_Sleep(500*16384/1000);

    MCD_TRACE(MCD_INFO_TRC, 0,"wifi_ConnectApEx_wifi \n");

//add   2013-1-14
#ifdef __SCAN_INFO__
    get_networkinfo_in_asso =FALSE;
#if 0
    RDA5890_BSS_NETWORK_INFO* assoc_bss = NULL;
    assoc_bss = find_bss_info_node_Mac(tmp_bss->bssid);
    if(NULL!=assoc_bss)
    {
        if(is_ap_support_11b(assoc_bss->rates))
        {
            test_rda5890_set_11b_only(0x00);//2013-1-15, set 11b only
            MCD_TRACE(MCD_INFO_TRC, 0,"######## set 11b only !\n");
            hal_HstSendEvent(0x88880000);


        }
        else
        {
            MCD_TRACE(MCD_INFO_TRC, 0,"########  set auto! \n");
            hal_HstSendEvent(0x88880002);

            test_rda5890_set_11b_only(0x02);//2013-1-15, set  auto
        }
        //sxr_Sleep(163);
    }
    else
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"########  no found the bssid! \n");
        hal_HstSendEvent(0x88880003);

    }
    //rda5890_set_wlan_mode(priv, 2);
#else
    checkWifiOptMode(tmp_bss->bssid);
#endif


#endif

    if(first_connect)
    {
        //sxr_Sleep(10*16384);
        first_connect= FALSE;
    }
    associate_tries = 0;
    ap_connecting =TRUE;
    /*
    MCD_TRACE(MCD_INFO_TRC, 0,"wifi_ConnectApEx \n");

    memset(wifi_context.cur_pwd,0x00,64);
    memset(wifi_context.cur_ssid,0x00,33);
    memset(wifi_context.WEP0,0x00,32);
    memcpy(wifi_context.cur_ssid,conn->ssid, conn->ssid_len);
    memcpy(wifi_context.cur_pwd,conn->password, conn->pwd_len);

    wifi_password = wifi_context.cur_pwd;
    MCD_TRACE(MCD_INFO_TRC, 0,"ssid, pwd: \n");
    showString(wifi_context.cur_ssid);

    showString(wifi_context.cur_pwd);

       auth_failed = FALSE;
    //test_rda5890_set_preasso_sleep(0x01000020);//enable wifi sleep pre-association set preasso time to (2^5)/10 s
    memset(&bss_desc_Used,0,sizeof(struct bss_descriptor ));
    memcpy(&(bss_desc_Used.data), tmp_bss, sizeof(RDA5890_BSS_DESC));
    */
    test_start_associate_bss_Ex(&(bss_desc_Used.data), len);

    //hal_sdioDataInIrqMaskSet();
    start_connect_ap_timer(50);
    //useingIrq = TRUE;
}
#endif

UINT32 wifi_assoApRssiInd(UINT8 rssi)
{
    //MCD_TRACE(MCD_INFO_TRC, 0,"##########wifi_assoApRssiInd \n");
    COS_EVENT ev;

    ev.nEventId = EV_CFW_WIFI_CURR_AP_INFO_IND;
    ev.nParam1  = (UINT32)rssi;
    ev.nParam2  = 0;
    ev.nParam3  = 0;
    //      MCD_TRACE(MCD_INFO_TRC, 0,"##########Rssi: %d \n",  (INT8)((UINT8)ev.nParam1));

    COS_SendEvent(COS_GetDefaultMmiTaskHandle(DEFAULT_MMI_TASK_ID), &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
    // ap_connected =FALSE;

    return 0;
}

//VOID write_rssi_switch_data_u32(VOID);
//VOID write_rssi_switch_data_u8(VOID);
VOID wifi_I2c_Write_U8(UINT8  (*u8_reg)[2], UINT8 num);
VOID wifi_I2c_Write_U32(UINT32  (*u32_reg)[2], UINT8 num);

VOID wifi_sdio_set_notch_by_channel( UINT8 channel);

void check_getRssi_result(void)
{
    MCD_TRACE(MCD_INFO_TRC, 0,  "check_getRssi_result. \n");

    UINT8 count = 0x00;
    unsigned char len;
    UINT8* pay_load =NULL;
    UINT8 rssi_tmp =0x00;
    test_rda5890_check_wid_response(rda5890_wid->wid_rsp, rda5890_wid->wid_rsp_len, rda5890_wid->wid, rda5890_wid->wid_msg_id, &len, &pay_load);

    //rssi_tmp = *pay_load ;
    if(pay_load==NULL)
    {

        MCD_TRACE(MCD_INFO_TRC, 0,  "check_getRssi_result fail !!!!!!\n");

    }
    else
    {
        rssi_tmp = *pay_load ;

        if(rssi_tmp  > 215)
        {
            //write_rssi_switch_data_u32();
            wifi_I2c_Write_U32(rssi_switch_data_u32,2);
            wifi_I2c_Write_U8(rssi_switch_data_u8,1);
            //write_rssi_switch_data_u8();

        }
        else
        {
            wifi_sdio_set_notch_by_channel(bss_desc_Used.data.channel);
        }

        MCD_TRACE(MCD_INFO_TRC, 0,  "check_getRssi_result.  rssi: 0x%x \n",*pay_load);

        //rssi_tmp =  rssi_tmp > 127? rssi_tmp -271: rssi_tmp-15;
        //MCD_TRACE(MCD_INFO_TRC, 0,    "check_getRssi_result2 .  rssi: 0x%x \n",rssi_tmp);
        bss_desc_Used.data.rssi = rssi_tmp;
        wifi_assoApRssiInd(rssi_tmp );
    }
    return;

}
VOID wifi_getAssoApRssi(void)
{
    gettingRssi = TRUE;
    wifi_get_str_from_MAC(WID_RSSI);

}
UINT8 check_wifi_rssi(void)
{
    return bss_desc_Used.data.rssi ;
}
VOID wifi_DisconnectAp_wifi(void)
{

//return;
    MCD_TRACE(MCD_INFO_TRC, 0,"wifi_DisconnectAp\n");
    ap_disconnecting = TRUE;
    preasso_timer_enable = FALSE;
    if(!ap_connected)
    {
        //return ;
    }
    //      test_rda5890_set_preasso_sleep(0x08000080);//enable wifi sleep pre-association set preasso time to (2^5)/10 s


    UINT32 mask_tmp ;

//  test_rda5890_start_scan();
//sxr_Sleep(500*16384/1000);


    test_rda5890_set_ssid("error", 5);
    preasso_timer_enable = TRUE;
    //sxr_Sleep (300);
    //test_rda5890_generic_set_uchar(WID_DISCONNECT,0x00);
    //sxr_Sleep (300);


    //test_rda5890_set_ssid("errorssid",strlen("errorssid"));
    mask_tmp =  hal_sdioDataInIrqMaskGet();

    hal_sdioDataInIrqMaskClear();

    if(!wifi_sleep_flag )
    {
        UINT32 tmp_addr = 0x08000000;
        tmp_addr = tmp_addr |(1<<28) |( 0x05 <<9);
        send_test_cmd(52,tmp_addr);

        tmp_addr = 0x88000000;
        tmp_addr = tmp_addr |(1<<28) |( 0x05 <<9)|0x80;
        send_test_cmd(52,tmp_addr);

        //test_rda5890_set_preasso_sleep(0x01000020);//set preasso time to (2^5)/10 s
        //wifi_sleep_enable_flag = 0x10;
        sxr_Sleep(200*16383/1000);
        wifi_sleep_flag = TRUE;
    }

    if(!sdio_sleep_flag)
        wifi_SdioSleep();

    if(!in_interruptHandle)
    {
        hal_sdioDataInIrqMaskSetEx(mask_tmp);
    }



    return ;
    /*
        memset(wifi_context.cur_pwd,0x00,64);
        memset(wifi_context.cur_ssid,0x00,33);

        wifi_password = wifi_context.cur_pwd;

        test_start_associate_bss("error", 5);
        //hal_sdioDataInIrqMaskClear();
        //useingIrq = FALSE;
        */
}

VOID wifi_DisconnectApEx(void)
{
    MCD_TRACE(MCD_INFO_TRC, 0,"wifi_DisconnectApEx\n");
    ap_disconnecting = TRUE;
    if(!ap_connected)
    {
        //return ;
    }

    test_rda5890_set_ssid("error", 5);
    //sxr_Sleep (300);
    //test_rda5890_generic_set_uchar(WID_DISCONNECT,0x00);
    //sxr_Sleep (300);

    return ;
}


UINT32 wifi_ScanRsp(VOID)
{
    MCD_TRACE(MCD_INFO_TRC, 0,"wifi_ScanRsp\n");
    /*
        UINT8 i =0;
        if(bss_Rst.count >0)
        {
            sxs_Dump(_MMI, 0, (UINT8*)bss_Rst.bss_descs,  bss_Rst.count*sizeof(RDA5890_BSS_DESC));

            RDA5890_BSS_DESC* tmpbss =( (RDA5890_BSS_DESC *)(bss_Rst.bss_descs));

            for(i =0; i < bss_Rst.count; i++)
            {
                MCD_TRACE(MCD_INFO_TRC, 0,  "the scan result : the NO.%d \n", i+1);

                showString((UINT8*)tmpbss->ssid);
                tmpbss= tmpbss + 1;
            //  count--;

            }
               sxr_Sleep (500);
        }
        */
    COS_EVENT ev;

    ev.nEventId = EV_CFW_WIFI_SCAN_RSP;
    ev.nParam1  = bss_Rst.bss_descs;
    ev.nParam2  = bss_Rst.count;
    ev.nParam3  = 0;
    COS_SendEvent(COS_GetDefaultMmiTaskHandle(DEFAULT_MMI_TASK_ID), &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
    MCD_TRACE(MCD_INFO_TRC, 0,"wifi_ScanRsp end \n");

    preasso_timer_enable = TRUE;

    return 0;
}



UINT32 wifi_ConnectRsp(UINT8 flag)
{
    MCD_TRACE(MCD_INFO_TRC, 0,"##########wifi_ConnectRsp,  flag: 0x%x\n",flag);

    COS_EVENT ev;
    if((!ap_connecting)&&(!ap_connected ))
    {
        if(flag ==0x00)
            return 0;
        ev.nEventId = EV_CFW_WIFI_CONNECTED_IND;
        MCD_TRACE(MCD_INFO_TRC, 0,"##########send EV_CFW_WIFI_CONNECTED_IND  \n");

    }
    else if(ap_connecting)
    {

        ev.nEventId = EV_CFW_WIFI_CONN_RSP;
        MCD_TRACE(MCD_INFO_TRC, 0,"##########send EV_CFW_WIFI_CONN_RSP  \n");

        stop_connect_ap_timer();
        ap_connecting =FALSE;

    }
    else
    {
        return 0;
    }
    ev.nParam1  = 0x11;
    ev.nParam2  = flag;
    if(flag==0x01)
    {
        stop_wifi_PreAssoWait_timer();
        ap_connected =TRUE;
        ev.nParam3  = 0;
    }
    else
    {
        ap_connected =FALSE;
        if(auth_failed)
            ev.nParam3  = 1;  //auth fail
        else
            ev.nParam3  = 2; // time out

    }

    COS_SendEvent(COS_GetDefaultMmiTaskHandle(DEFAULT_MMI_TASK_ID), &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
    return 0;
}
UINT32 wifi_DisconnectRsp(void)
{
    MCD_TRACE(MCD_INFO_TRC, 0,"##########wifi_DisconnectRsp \n");
    COS_EVENT ev;

    ev.nEventId = EV_CFW_WIFI_DISCONN_RSP;
    ev.nParam1  = 0;
    ev.nParam2  = 0;
    ev.nParam3  = 0;
    COS_SendEvent(COS_GetDefaultMmiTaskHandle(DEFAULT_MMI_TASK_ID), &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
    ap_connected =FALSE;

    return 0;
}
UINT32 wifi_DisconnectInd(void)
{
    MCD_TRACE(MCD_INFO_TRC, 0,"##########wifi_DisconnectInd \n");

    COS_EVENT ev;

    ev.nEventId = EV_CFW_WIFI_DISCONNECT_IND;
    ev.nParam1  = 0x11;
    ev.nParam2  = 0;
    ev.nParam3  = 0;
    COS_SendEvent(COS_GetDefaultMmiTaskHandle(DEFAULT_MMI_TASK_ID), &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
    return 0;
}

UINT32 wifi_NeedPowerOffInd(void)
{
    MCD_TRACE(MCD_INFO_TRC, 0,"##########wifi_NeedPowerOffInd \n");

    COS_EVENT ev;

    ev.nEventId = EV_CFW_WIFI_NEED_POWEROFF_IND;
    ev.nParam1  = 0x11;
    ev.nParam2  = 0;
    ev.nParam3  = 0;
    COS_SendEvent(COS_GetDefaultMmiTaskHandle(DEFAULT_MMI_TASK_ID), &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
    return 0;
}
UINT32 wifi_PowerOnRsp(void)
{
    //  MCD_TRACE(MCD_INFO_TRC, 0,"##########wifi_PowerOnRsp \n");

    COS_EVENT ev;

    ev.nEventId = EV_CFW_WIFI_INIT_RSP;
    ev.nParam1  = localMac;
    ev.nParam2  = 6;
    ev.nParam3  = 0;
    COS_SendEvent(COS_GetDefaultMmiTaskHandle(DEFAULT_MMI_TASK_ID), &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
    return 0;
}

UINT32 wifi_PowerOnFail(void)
{
    MCD_TRACE(MCD_INFO_TRC, 0,"##########wifi_PowerOnRsp \n");

    COS_EVENT ev;

    ev.nEventId = EV_CFW_WIFI_INIT_RSP;
    ev.nParam1  = 0;
    ev.nParam2  = 0;
    ev.nParam3  = 0;
    COS_SendEvent(COS_GetDefaultMmiTaskHandle(DEFAULT_MMI_TASK_ID), &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
    return 0;
}
UINT32 wifi_PowerOffRsp(void)
{
    //  MCD_TRACE(MCD_INFO_TRC, 0,"##########wifi_PowerOffRsp \n");

    COS_EVENT ev;

    ev.nEventId = EV_CFW_WIFI_DEINIT_RSP;
    ev.nParam1  = 0;
    ev.nParam2  = 0;
    ev.nParam3  = 0;
    COS_SendEvent(COS_GetDefaultMmiTaskHandle(DEFAULT_MMI_TASK_ID), &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
    return 0;
}
///////////////////////////////////////////////////////
/*
UINT32 wifi_patch_all_start_addr = 0x00108000;
const UINT32 wifi_patch_all[] =
{
#if 0
 0xEA03DF74, 0xE59F101C, 0xE3A00040, 0xE5C10038,
 0xE1A0F00E, 0xEA03DF6D, 0xE59F1008, 0xE3A00040,
 0xE5C10038, 0xE1A0F00E, 0x50300000, 0xEB03D6D0,
 0xE1A00B84, 0xE1B00BA0, 0x11A00B84, 0x11A00BA0,
 0x12600F80, 0x10804004, 0xE1A00124, 0xE1A0F00E,
 0xEA03D397, 0xE3A00001, 0xE1A0F00E, 0xEA03D692,
 0xE3A00001, 0xE1A0F00E, 0xEB03C7F7, 0xE51F004C,
 0xE5D00038, 0xE3100080, 0x1A000001, 0xE3A00001,
 0xE1A0F00E, 0xE3A00000, 0xE1A0F00E, 0xEB03EAB1,
 0xE51F0070, 0xE5D00038, 0xE3100080, 0x1A000001,
 0xE3A00001, 0xE1A0F00E, 0xE3A00000, 0xE1A0F00E,
 0xEB03D876, 0xE51F0094, 0xE3A01080, 0xE5C01038,
 0xE1A0F00E, 0xEB03C7ED, 0xE1A04000, 0xE3540001,
 0x0A000002, 0xE59F0040, 0xE59F1038, 0xE581003C,
 0xE1A0F00E, 0xEB03EAA8, 0xE1A04000, 0xE3540001,
 0x0A000002, 0xE59F0020, 0xE59F1018, 0xE581003C,
 0xE1A0F00E, 0xEA03E043, 0xE59F0010, 0xE59F1004,
 0xE581003C, 0xE1A0F00E, 0x30010000, 0x2E00A100,
 0x2E00A000, 0xEB03D6D7, 0xE92D4010, 0xE2841001,
 0xE3A0001B, 0xE3A027C0, 0xE200007F, 0xE1A01801,
 0xE1A01821, 0xE1820800, 0xE1814000, 0xE59F10C4,
 0xE5814034, 0xE5910034, 0xE3100680, 0x0AFFFFFC,
 0xE59F10B4, 0xE3A00030, 0xE3A027C0, 0xE200007F,
 0xE1A01801, 0xE1A01821, 0xE1820800, 0xE1814000,
 0xE59F1090, 0xE5814034, 0xE5910034, 0xE3100680,
 0x0AFFFFFC, 0xE59F1084, 0xE3A00030, 0xE3A027C0,
 0xE200007F, 0xE1A01801, 0xE1A01821, 0xE1820800,
 0xE1814000, 0xE59F105C, 0xE5814034, 0xE5910034,
 0xE3100680, 0x0AFFFFFC, 0xE59F0058, 0xE3A02000,
 0xE2822001, 0xE1520000, 0xBAFFFFFC, 0xE59F1040,
 0xE3A00030, 0xE3A027C0, 0xE200007F, 0xE1A01801,
 0xE1A01821, 0xE1820800, 0xE1814000, 0xE59F1014,
 0xE5814034, 0xE5910034, 0xE3100680, 0x0AFFFFFC,
 0xE8BD4010, 0xE1A0F00E, 0x50090000, 0x00000249,
 0x0000024D, 0x00000241, 0x00040000, 0xEA040FC4,
 0xE1A0000E, 0xEBFC4160, 0xE1A0E000, 0xE3A00000,
 0xE1A0F00E, 0xEA039E13, 0xE1A0000E, 0xEBFC415A,
 0xE1A0E000, 0xE1A0F00E,
 #else
 0xEA03DF9C,0xE59F101C,0xE3A00040,0xE5C10038
,0xE1A0F00E,0xEA03DF95,0xE59F1008,0xE3A00040
,0xE5C10038,0xE1A0F00E,0x50300000,0xEB03D6F2
,0xE1A00B84,0xE1B00BA0,0x11A00B84,0x11A00BA0
,0x12600F80,0x10804004,0xE1A00124,0xE1A0F00E
,0xEA03D3CD,0xE3A00001,0xE1A0F00E,0xEA03D6C8
,0xE3A00001,0xE1A0F00E,0xEB03C781,0xE51F004C
,0xE5D00038,0xE3100080,0x1A000001,0xE3A00001
,0xE1A0F00E,0xE3A00000,0xE1A0F00E,0xEB03EAD9
,0xE51F0070,0xE5D00038,0xE3100080,0x1A000001
,0xE3A00001,0xE1A0F00E,0xE3A00000,0xE1A0F00E
,0xEB03D898,0xE51F0094,0xE3A01080,0xE5C01038
,0xE1A0F00E,0xEB03C777,0xE1A04000,0xE3540001
,0x0A000002,0xE59F0040,0xE59F1038,0xE581003C
,0xE1A0F00E,0xEB03EAD0,0xE1A04000,0xE3540001
,0x0A000002,0xE59F0020,0xE59F1018,0xE581003C
,0xE1A0F00E,0xEA03E06B,0xE59F0010,0xE59F1004
,0xE581003C,0xE1A0F00E,0x30010000,0x2E00A100
,0x2E00A000
 #endif
};
*/

/*
const UINT32 wifi_patch_all_ex[][2] =
{
#if 0
 {  0x20040004, 0x0001022c },
{  0x20040024, 0x00108000 },
{  0x20040008, 0x0001025c },
{  0x20040028, 0x00108014 },
{  0x2004000c, 0x000124e8 },
{  0x2004002c, 0x0010802c },
{  0x20040010, 0x000131f0 },
{  0x20040030, 0x00108050 },
{  0x20040014, 0x00012610 },
{  0x20040034, 0x0010805C },
{  0x20040018, 0x00016088 },
{  0x20040038, 0x00108068 },
{  0x2004001c, 0x0000D5C4 },
{  0x2004003c, 0x0010808C },
{  0x20040020, 0x00011ED4 },
{  0x20040040, 0x001080B0 },

{  0x20040100, 0x0001610C },
{  0x20040120, 0x001080C4 },
{  0x20040104, 0x0000D640 },
{  0x20040124, 0x001080E4 },
{  0x20040108, 0x0000FFF4 },
{  0x20040128, 0x00108104 },
{  0x2004010c, 0x000125c4 },
{  0x2004012c, 0x00108124 },
{  0x20040110, 0x00004318 },
{  0x20040130, 0x0010822c },

{  0x20040000, 0x00001fff },
#else
 { 0x20040004, 0x0001018C },
{ 0x20040024, 0x00108000 },
{ 0x20040008, 0x000101BC },
{ 0x20040028, 0x00108014 },
{ 0x2004000c, 0x00012460 },
{ 0x2004002c, 0x0010802c },
{ 0x20040010, 0x00013118 },
{ 0x20040030, 0x00108050 },
{ 0x20040014, 0x00012538 },
{ 0x20040034, 0x0010805C },
{ 0x20040018, 0x00016260 },
{ 0x20040038, 0x00108068 },
{ 0x2004001c, 0x0000D524 },
{ 0x2004003c, 0x0010808C },
{ 0x20040020, 0x00011E4C },
{ 0x20040040, 0x001080B0 },

{ 0x20040100, 0x000162E4 },
{ 0x20040120, 0x001080C4 },
{ 0x20040104, 0x0000D5A0 },
{ 0x20040124, 0x001080E4 },
{ 0x20040108, 0x0000FF54 },
{ 0x20040128, 0x00108104 },

{ 0x20040000, 0x000007ff },
#endif

};
*/
INT8 sdio_getWidResp(void)
{
    MCD_TRACE(MCD_INFO_TRC, 0, "sdio_getWidResp enter. ");

    u16 datalen =0;
    UINT32 tmp_addr = 0x00000000;
    UINT8 val_rsp = 0;
    UINT8 count =0;
    UINT8 ret = 0x00;
    MCD_CS_ENTER;
    while(rda5890_wid->wid_pending == 1)
    {
        while(val_rsp==0x00)
        {
            sxr_Sleep(200*16384/1000);
            tmp_addr = 0x00000000;
            tmp_addr = tmp_addr |(1<<28) |( 0x06 <<9);
            ret = read_test_cmd(52,tmp_addr);
            val_rsp = ret & 0x01;
            if((count++)>20)
            {
                MCD_CS_EXIT;
                return -1;
            }
        }

        tmp_addr = 0x88000000;
        tmp_addr = tmp_addr |(0x01<<28) |( 0x05 <<9)|val_rsp;
        ret = read_test_cmd(52,tmp_addr);

        datalen = sdio_GetReadDataLen(1);

        if(datalen>0)
        {
            INT8 retx =wifi_getRcvBuf();
            if(retx < 0)
            {
                MCD_CS_EXIT;
                MCD_TRACE(MCD_INFO_TRC, 0, "test_Check_Sdio_DataIn,  no buf. ");
                //    hal_SdioSleep();

                return -1;
            }

            UINT8 bufIdx =(UINT8) retx;

            memset(tmp_read_data[bufIdx],0x00,sizeof(tmp_read_data[bufIdx]));
            if( 0==  SDIO_WifiDataRead((U8*)0x08,tmp_read_data[bufIdx],datalen))
            {

                rda5890_sendRxIntToDev();
                MCD_CS_EXIT;
                MCD_TRACE(MCD_INFO_TRC, 0, "SDIO_WifiDataRead,  datalen: %d",datalen);
                sxs_Dump(_MMI, 0, tmp_read_data[bufIdx], datalen);
                test_rda5890_card_to_host(tmp_read_data[bufIdx],datalen);
                wifi_ClrRcvBufflag(bufIdx);
            }

        }
        else
        {

            MCD_CS_EXIT;
        }
        sxs_Dump(_MMI, 0, rda5890_wid->wid_rsp, rda5890_wid->wid_rsp_len);
    }
    return 0;
}


int patch_wid_request(UINT8 *wid_req, unsigned short wid_req_len)
{
    int ret = 0;
    UINT8 data_buf[RDA5890_MAX_WID_LEN + 2];
    unsigned short data_len;
    memset(data_buf,0x00,sizeof(data_buf));
    if(rda5890_wid->wid_pending == 1)
    {
        UINT8 i =0;
        while((rda5890_wid->wid_pending == 1)&&(i<30))
        {
            sxr_Sleep(1638);
            i++;
        }
        if(i==30)
            return -1;
    }
    rda5890_wid->wid_pending = 1;

    data_len = wid_req_len + 2;
    data_buf[0] = (UINT8)(data_len&0xFF);
    data_buf[1] = (UINT8)((data_len>>8)&0x0F);
    data_buf[1] |= 0x40;  // for Request(Q/W) 0x4
    memcpy(data_buf + 2, wid_req, wid_req_len);
    ret = SDIO_WifiDataWrite(0x07, data_buf,data_len);

    sdio_getWidResp();

    return ret;
}
/*
VOID write_wifi_patch_data(VOID)

{
        MCD_TRACE(MCD_INFO_TRC, 0,"write_wifi_patch_data");

    int ret;
    char wid_req[512];
    unsigned short wid_req_len;
    char wid_msg_id ;
            UINT16 tmp_len ;

    //char wid_rsp[32];
    //unsigned short wid_rsp_len = 32;
    UINT16 patch_dada_count = 0;
    UINT16 patch_len = sizeof(wifi_patch_all)/sizeof(wifi_patch_all[0]);
        UINT32 val = 0x00;

    while(patch_dada_count < patch_len)
    {

        memset(wid_req, 0x00,512);
        wid_msg_id = rda5890_wid->wid_msg_id++;
        MCD_TRACE(MCD_INFO_TRC, 0,"test_rda5890_generic_set_ulong ,wid_req: 0x%x,wid_req[0]=0x%x\n",wid_req, wid_req[0]);

        wid_req[0] = 'W';
        wid_req[1] = wid_msg_id;

        tmp_len =4;
        while((tmp_len<500)&&(patch_dada_count < patch_len))
        {
            // write address
            wid_req[tmp_len] = (char)(0x201e&0x00FF);
            wid_req[tmp_len+1] = (char)((0x201e&0xFF00) >> 8);

            wid_req[tmp_len+2] = (char)(sizeof(UINT32));
            val = wifi_patch_all_start_addr + patch_dada_count*4;
            memcpy(wid_req + tmp_len+3, &val, sizeof(UINT32));

            tmp_len= tmp_len + 3+ sizeof(UINT32);
            //write data
            wid_req[tmp_len] = (char)(0x201f&0x00FF);
            wid_req[tmp_len+1] = (char)((0x201f&0xFF00) >> 8);

            wid_req[tmp_len+2] = (char)(sizeof(UINT32));
            val = wifi_patch_all[patch_dada_count];
            memcpy(wid_req + tmp_len+3, &val, sizeof(UINT32));

            tmp_len= tmp_len + 3+ sizeof(UINT32);
            patch_dada_count++;

        }
        wid_req[2] = (char)(tmp_len&0x00FF);
        wid_req[3] = (char)((tmp_len&0xFF00) >> 8);
   firstWrite = TRUE;
        ret = patch_wid_request(wid_req, tmp_len);

    }
    if (ret) {
         MCD_TRACE(MCD_INFO_TRC, 0,"rda5890_wid_request fail, ret = %d\n", ret);
        goto out;
    }

    //ret = test_rda5890_check_wid_status(wid_rsp, wid_rsp_len, wid_msg_id);
    //if (ret) {
    //   MCD_TRACE(MCD_INFO_TRC, 0,"check_wid_status fail, ret = %d\n", ret);
    //  goto out;
    //}

out:
    return ret;
}
*/
#if 0
#define PTA_CFG_06_14

const UINT32 wifi_core_notch_patch[][2] =
{
    {0x5000050C,0x00008000},  //power saving  modify wangxiuzhuznag, 2012-05-17
#ifdef PTA_CFG_06_14/*pta config*/

    {0x50000800,0xFC003E05},
    {0x50000804,0x00000000},
    {0x50000808,0xf9000013},
    {0x5000080c,0x000001C0},
    {0x50000810,0xFFCC0F01},
    {0x50000814,0xFFFF0F33},
    {0x50000818,0x00FF3033},
    {0x5000081C,0xFF000F00},
    {0x50000820,0xFF000F00},
    {0x50000824,0x0000C0CC},
    {0x50000828,0x00100F10},
    {0x50000834,0x00003032},
//{0x50000838,0xFFFFFFFF},
//{0x5000083C,0xFFFFFFFF},

#endif

#if 0    /*pta config*/
    {0x50000800,0xFC003E05},   //  ctrl and mgmt package as hi priority
    {0x50000804,0x00000000},   // all data package as low priority
    //{0x50000808,0xA500001B},   // shorter beacon protect time
    {0x50000808,0xA5000013},   // shorter beacon protect time //modify 2012-06-14 houzhen

    {0x50000810,0x0FCC0F01},   //bt grant
    {0x50000814,0x0fff0F03},   //wf  grant
    {0x50000818,0x00ff0001},   //antSel0 for wl_rx
    {0x5000081C,0x0f000f00},   //antSel1 for wl_tx
    {0x50000820,0x0f000f00},   //antSel2 for wl_pa  dig directly control rf_switch  houzhen Mar 29 2012
    {0x50000824,0xF000f0fe},   //antSel3 for bt_rf
    {0x50000828,0x00100F10},   //penality timer setting
    {0x50000838,0xF8003f2A},   //  rx package lo priority
    {0x5000083c,0x00000003},   // rx package hi priority
    /*end pta config*/
#endif


    /*
    //  {0x50000808,0x6500001b},       //shorter beacon protect time
        {0x50000800,0xFC003E05},   //  ctrl and mgmt package as hi priority
        {0x50000804,0x00000000},   // all data package as low priority
        {0x50000838,0xF8003f2A},   //  rx package lo priority
      {0x5000083c,0x00000003},   // rx package hi priority
        {0x50000808,0xf900001b},   //
        {0x50000810,0x0FCC0F01},   //bt grant
        {0x50000814,0x0fff0F33},   //wf  grant
        {0x50000818,0x00ff0001},   //antSel0 for wl_rx
        {0x5000081C,0x0f000f00},   //antSel1 for wl_tx
        {0x50000820,0x0f000f00},   //antSel2 for wl_pa  dig directly control rf_switch  houzhen Mar 29 2012
        {0x50000824,0xF000f0fe},   //antSel3 for bt_rf
        {0x50000828,0x00100F10},   //penality timer setting

    */

    {  0x30010000, 0x780369AF },   //disable tports wait  100ms;
    {  0x30000010, 0x7000FFFF },//wait 500ms;
//{  0x50000600, 0x00001200 },//write 00 to 09h tx_on earlier wait 500ms;
//{  0x50000600, 0x00001468 },//write 88 to 0ah tx_on earlier wait 500ms;;;item:lna_step to  22db
    {  0x50090054, 0x00000001 },//enable update
    {  0x50090200, 0x00000000 },
    {  0x50090204, 0x00000000 },
    {  0x50090208, 0x00000002 },
    {  0x5009020c, 0x00000004 },
    {  0x50090210, 0x00000006 },
    {  0x50090214, 0x00000008 },
    {  0x50090218, 0x0000000a },
    {  0x5009021c, 0x00000040 },
    {  0x50090220, 0x00000042 },
    {  0x50090224, 0x00000044 },
    {  0x50090228, 0x00000046 },
    {  0x5009022c, 0x00000048 },
    {  0x50090230, 0x0000004a },
    {  0x50090234, 0x00000080 },
    {  0x50090238, 0x00000082 },
    {  0x5009023c, 0x00000084 },
    {  0x50090240, 0x00000086 },
    {  0x50090244, 0x00000088 },
    {  0x50090248, 0x0000008a },
    {  0x5009024c, 0x000000c0 },
    {  0x50090250, 0x000000c2 },
    {  0x50090254, 0x000000c4 },
    {  0x50090258, 0x000000c6 },
    {  0x5009025c, 0x000000c8 },
    {  0x50090260, 0x000000ca },
    {  0x50090264, 0x00000100 },
    {  0x50090268, 0x00000102 },
    {  0x5009026c, 0x00000104 },
    {  0x50090270, 0x00000106 },
    {  0x50090274, 0x00000108 },
    {  0x50090278, 0x00000140 },
    {  0x5009027c, 0x00000142 },//lna =0 end
    {  0x50090280, 0x00000080 },
    {  0x50090284, 0x00000082 },
    {  0x50090288, 0x00000084 },
    {  0x5009028c, 0x00000086 },
    {  0x50090290, 0x00000088 },
    {  0x50090294, 0x0000008a },
    {  0x50090298, 0x000000c0 },
    {  0x5009029c, 0x000000c2 },
    {  0x500902a0, 0x000000c4 },
    {  0x500902a4, 0x000000c6 },
    {  0x500902a8, 0x000000c8 },
    {  0x500902ac, 0x000000ca },
    {  0x500902b0, 0x00000100 },
    {  0x500902b4, 0x00000102 },
    {  0x500902b8, 0x00000104 },
    {  0x500902bc, 0x00000106 },
    {  0x500902c0, 0x00000108 },
    {  0x500902c4, 0x00000140 },
    {  0x500902c8, 0x00000142 },
    {  0x500902cc, 0x00000144 },
    {  0x500902d0, 0x00000146 },
    {  0x500902d4, 0x00000148 },
    {  0x500902d8, 0x00000180 },
    {  0x500902dc, 0x00000182 },
    {  0x500902e0, 0x00000184 },
    {  0x500902e4, 0x000001c0 },
    {  0x500902e8, 0x000001c2 },
    {  0x500902ec, 0x000001c4 },
    {  0x500902f0, 0x000001c6 },
    {  0x500902f4, 0x000001c8 },
    {  0x500902f8, 0x000001ca },
    {  0x500902fc, 0x000001cc },// lna = 01  end
    {  0x50090300, 0x00000102 },
    {  0x50090304, 0x00000104 },
    {  0x50090308, 0x00000106 },
    {  0x5009030c, 0x00000108 },
    {  0x50090310, 0x00000140 },
    {  0x50090314, 0x00000142 },
    {  0x50090318, 0x00000144 },
    {  0x5009031c, 0x00000146 },
    {  0x50090320, 0x00000148 },
    {  0x50090324, 0x00000180 },
    {  0x50090328, 0x00000182 },
    {  0x5009032c, 0x00000184 },
    {  0x50090330, 0x000001c0 },
    {  0x50090334, 0x000001c2 },
    {  0x50090338, 0x000001c4 },
    {  0x5009033c, 0x000001c6 },
    {  0x50090340, 0x000001c8 },
    {  0x50090344, 0x000001ca },
    {  0x50090348, 0x000001cb },
    {  0x5009034c, 0x000001cb },
    {  0x50090350, 0x000001cb },
    {  0x50090354, 0x000001cb },
    {  0x50090358, 0x000001cb },
    {  0x5009035c, 0x000001cb },
    {  0x50090360, 0x000001cb },
    {  0x50090364, 0x000001cb },
    {  0x50090368, 0x000001cb },
    {  0x5009036c, 0x000001cb },
    {  0x50090370, 0x000001cb },
    {  0x50090374, 0x000001cb },
    {  0x50090378, 0x000001cb },
    {  0x5009037c, 0x000001cb },
    {  0x50090054, 0x00000000 },//disable update
//{  0x50000808, 0x65000013 }, // disable prerx_priority;pta config  2012 06 27 enable edr
// {  0x50000810, 0xFFCD0F01 },  //rx beacon priority  2012 06 27 enable edr

//add notch patch
    //ch 1
    {0x001008d0, 0x50090040},
    {0x001008d4, 0x10000000},
    {0x001008d8, 0x50090044},
    {0x001008dc, 0x10000000},
    //ch 2
    {0x00100910, 0x50090040},
    {0x00100914, 0x10000000},
    {0x00100918, 0x50090044},
    {0x0010091c, 0x10000000},
    //ch 3
    {0x00100950, 0x50090040},
    {0x00100954, 0x10000000},
    {0x00100958, 0x50090044},
    {0x0010095c, 0x10000000},
    //ch 4
    {0x00100990, 0x50090040},
    {0x00100994, 0x10000000},
    {0x00100998, 0x50090044},
    {0x0010099c, 0x10000000},
    //ch 5
    {0x001009d0, 0x50090040},
    {0x001009d4, 0x076794b4},
    {0x001009d8, 0x50090044},
    {0x001009dc, 0x10000000},
    //ch 6
    {0x00100a10, 0x50090040},
    {0x00100a14, 0x077c71de},
    {0x00100a18, 0x50090044},
    {0x00100a1c, 0x046d242e},
    //ch 7
    {0x00100a50, 0x50090040},
    {0x00100a54, 0x077e7ec0},
    {0x00100a58, 0x50090044},
    {0x00100a5c, 0x077e7140},
    //ch 8
    {0x00100a90, 0x50090040},
    {0x00100a94, 0x077c7e22},
    {0x00100a98, 0x50090044},
    {0x00100a9c, 0x046d2bd2},
    //ch 9
    {0x00100ad0, 0x50090040},
    {0x00100ad4, 0x10000000},
    {0x00100ad8, 0x50090044},
    {0x00100adc, 0x10000000},
    //ch 10
    {0x00100b10, 0x50090040},
    {0x00100b14, 0x10000000},
    {0x00100b18, 0x50090044},
    {0x00100b1c, 0x10000000},
    //ch 11
    {0x00100b50, 0x50090040},
    {0x00100b54, 0x10000000},
    {0x00100b58, 0x50090044},
    {0x00100b5c, 0x10000000},
    //ch 12
    {0x00100b90, 0x50090040},
    {0x00100b94, 0x07764310},
    {0x00100b98, 0x50090044},
    {0x00100b9c, 0x10000000},
    //ch 13
    {0x00100bd0, 0x50090040},
    {0x00100bd4, 0x10000000},
    {0x00100bd8, 0x50090044},
    {0x00100bdc, 0x10000000},
    //ch 14
    {0x00100c10, 0x50090040},
    {0x00100c14, 0x0779c279},
    {0x00100c18, 0x50090044},
    {0x00100c1c, 0x0779cd87},
//add end




    //{ 0x3001003c, 0x2e00a100 } ,
    { 0x00106b6c, 0x00000002} , //patch for chanel 13

};
#endif


VOID write_wifi_Core_dataEx(VOID)
{
    MCD_TRACE(MCD_INFO_TRC, 0,"write_wifi_Core_dataEx, wifi_version: 0x%x",wifi_version_flag);

    int ret;
    char wid_req[512];
    //unsigned short wid_req_len;
//  char wid_msg_id ;
    UINT16 tmp_len ;
    char wid_msg_id = rda5890_wid->wid_msg_id;
    rda5890_wid->wid = 0x201e;
    rda5890_wid->wid_rsp_len =RDA5890_MAX_WID_LEN;
    memset(rda5890_wid->wid_rsp,0x0,512);

    //char wid_rsp[32];
    //unsigned short wid_rsp_len = 32;
    UINT16 patch_dada_count = 0;
    UINT16 patch_len =0;
    UINT32 val = 0x00;
    UINT32* tmp_array = NULL;
    if(wifi_version_flag== WIFI_VERSION_D )
    {
        tmp_array = wifi_core_notch_patch;
        patch_len = sizeof(wifi_core_notch_patch)/sizeof(wifi_core_notch_patch[0]);
    }
    else if((wifi_version_flag== WIFI_VERSION_E) ||(wifi_version_flag== WIFI_VERSION_F))
    {
        tmp_array = wifi_core_notch_patch_e;
        patch_len = sizeof(wifi_core_notch_patch_e)/sizeof(wifi_core_notch_patch_e[0]);
    }

    while(patch_dada_count < patch_len)
    {

        memset(wid_req, 0x00, 512);
        wid_msg_id = rda5890_wid->wid_msg_id;
        MCD_TRACE(MCD_INFO_TRC, 0,"test_rda5890_generic_set_ulong ,wid_req: 0x%x,wid_req[0]=0x%x\n",wid_req, wid_req[0]);

        wid_req[0] = 'W';
        wid_req[1] = wid_msg_id;

        tmp_len =4;
        while((tmp_len<450)&&(patch_dada_count < patch_len))
        {
            // write address
            wid_req[tmp_len] = (char)(0x201e&0x00FF);
            wid_req[tmp_len+1] = (char)((0x201e&0xFF00) >> 8);

            wid_req[tmp_len+2] = (char)(sizeof(UINT32));
            val = tmp_array[ patch_dada_count*2];
            memcpy(wid_req + tmp_len+3, &val, sizeof(UINT32));

            tmp_len= tmp_len + 3+ sizeof(UINT32);
            //write data
            wid_req[tmp_len] = (char)(0x201f&0x00FF);
            wid_req[tmp_len+1] = (char)((0x201f&0xFF00) >> 8);

            wid_req[tmp_len+2] = (char)(sizeof(UINT32));
            val = tmp_array[ patch_dada_count*2+1];
            memcpy(wid_req + tmp_len+3, &val, sizeof(UINT32));

            tmp_len= tmp_len + 3+ sizeof(UINT32);
            patch_dada_count++;

        }
        wid_req[2] = (char)(tmp_len&0x00FF);
        wid_req[3] = (char)((tmp_len&0xFF00) >> 8);
        wifi_context.firstWrite = TRUE;
        ret = patch_wid_request(wid_req, tmp_len);

    }
    if (ret)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"rda5890_wid_request fail, ret = %d\n", ret);

    }

    //ret = test_rda5890_check_wid_status(wid_rsp, wid_rsp_len, wid_msg_id);
    //if (ret) {
    //   MCD_TRACE(MCD_INFO_TRC, 0,"check_wid_status fail, ret = %d\n", ret);
    //  goto out;
    //}


    return;
}

INT8 wifi_init_patch_dataEx(VOID)
{
    MCD_TRACE(MCD_INFO_TRC, 0, "wifi_init_patch_dataEx...., wifi_version: 0x%x",wifi_version_flag);
    int ret;
    char wid_req[224];
    //unsigned short wid_req_len;
    UINT16 tmp_len ;

    char wid_msg_id = rda5890_wid->wid_msg_id;
    rda5890_wid->wid = 0x201e;
    rda5890_wid->wid_rsp_len =RDA5890_MAX_WID_LEN;
    memset(rda5890_wid->wid_rsp,0x0, 512);

    UINT16 patch_dada_count = 0;
    UINT16 patch_len = 0;
    UINT32 val = 0x00;
    UINT8* tmp_array = NULL;
    if(wifi_version_flag== WIFI_VERSION_D )
    {
        tmp_array = wifi_core_data_patch;
        patch_len = sizeof(wifi_core_data_patch)/sizeof(wifi_core_data_patch[0]);
    }
    else if((wifi_version_flag== WIFI_VERSION_E) ||(wifi_version_flag== WIFI_VERSION_F))
    {
        tmp_array = wifi_core_data_patch_e;
        patch_len = sizeof(wifi_core_data_patch_e)/sizeof(wifi_core_data_patch_e[0]);
    }
    while(patch_dada_count < patch_len)
    {

        memset(wid_req, 0x00, 224);
        wid_msg_id = rda5890_wid->wid_msg_id;
        MCD_TRACE(MCD_INFO_TRC, 0,"test_rda5890_generic_set_ulong ,wid_req: 0x%x,wid_req[0]=0x%x\n",wid_req, wid_req[0]);

        wid_req[0] = 'W';
        wid_req[1] = wid_msg_id;

        tmp_len =4;
        while((tmp_len<200)&&(patch_dada_count < patch_len))
        {
            // write address
            wid_req[tmp_len] = (char)(0x1B&0x00FF);
            wid_req[tmp_len+1] = (char)((0x1B&0xFF00) >> 8);

            wid_req[tmp_len+2] = (char)(sizeof(UINT8));
            val = tmp_array[ patch_dada_count*2];
            memcpy(wid_req + tmp_len+3, &val, sizeof(UINT8));

            tmp_len= tmp_len + 3+ sizeof(UINT8);
            //write data
            wid_req[tmp_len] = (char)(0x1c&0x00FF);
            wid_req[tmp_len+1] = (char)((0x1c&0xFF00) >> 8);

            wid_req[tmp_len+2] = (char)(sizeof(UINT8));
            val = tmp_array[ patch_dada_count*2+1];
            memcpy(wid_req + tmp_len+3, &val, sizeof(UINT8));

            tmp_len= tmp_len + 3+ sizeof(UINT8);
            patch_dada_count++;

        }
        wid_req[2] = (char)(tmp_len&0x00FF);
        wid_req[3] = (char)((tmp_len&0xFF00) >> 8);
        wifi_context.firstWrite = TRUE;
        ret = patch_wid_request(wid_req, tmp_len);

    }
    if (ret)
    {
        MCD_TRACE(MCD_INFO_TRC, 0,"rda5890_wid_request fail, ret = %d\n", ret);

    }


    return ret;





#if 0
//  INT16 ret =0;
    UINT8 i =0;
    stop_wifiPollTimer();
    //    hal_sdioDataInIrqMaskClear();
    for(i=0; i<sizeof(wifi_core_data_patch)/sizeof(wifi_core_data_patch[0]); i++)
    {
        firstWrite = TRUE;
        test_rda5890_generic_set_uchar(0x1B,wifi_core_data_patch[i][0]);

        if(-1== sdio_getInputData())
        {

            return -1;
        }
        firstWrite = TRUE;
        test_rda5890_generic_set_uchar(0x1C,wifi_core_data_patch[i][1]);



        if(-1== sdio_getInputData())
        {

            return -1;
        }
        sxr_Sleep(10 *16384/1000);
    }
    sxr_Sleep(200 *16384/1000);
    restart_wifiPollTimer();
//hal_sdioDataInIrqMaskSet();
    firstWrite = TRUE;
    return 0;
#endif
}



void rda5890_rssi_up_to_200(VOID)
{

    //rda5890_set_core_init(priv, rssi_switch_data_u32, 2);
    //rda5890_set_core_patch(priv, rssi_switch_data_u8, 1);
}
VOID wifi_sdio_set_notch_by_channel( UINT8 channel)
{
    MCD_TRACE(MCD_INFO_TRC, 0,"wifi_sdio_set_notch_by_channel\n");

    int count = 0, index = 0;

    if(wifi_version_flag== WIFI_VERSION_D)
        count  = sizeof(wifi_notch_data)/(sizeof(wifi_notch_data[0]) * 4);
    else if((wifi_version_flag== WIFI_VERSION_E) ||(wifi_version_flag== WIFI_VERSION_F))
        count  = sizeof(wifi_notch_data_E)/(sizeof(wifi_notch_data_E[0]) * 4);
    channel = channel % count;


    if(channel > 1)
        channel -= 1;

    if(wifi_version_flag== WIFI_VERSION_D)
    {
        wifi_I2c_Write_U32(wifi_notch_data[4*channel],4);
        //    rda5890_set_core_init(priv, wifi_notch_data[4*channel], 4);
        wifi_I2c_Write_U8(rssi_switch_default_data_u8, 1);
        // rda5890_set_core_patch(priv, rssi_switch_default_data_u8, 1);
    }
    else if((wifi_version_flag== WIFI_VERSION_E) ||(wifi_version_flag== WIFI_VERSION_F))
    {
        wifi_I2c_Write_U32(wifi_notch_data_E[4*channel],4);
        //    rda5890_set_core_init(priv, wifi_notch_data[4*channel], 4);
        wifi_I2c_Write_U8(rssi_switch_default_data_u8, 1);
        // rda5890_set_core_patch(priv, rssi_switch_default_data_u8, 1);
    }
}
/////////////////////////////////////////////////////////
// for test mode
/////////////////////////////////////////////////////////







