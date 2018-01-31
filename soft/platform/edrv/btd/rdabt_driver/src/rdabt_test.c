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


#ifdef __BT_RDABT__
#include "kal_release.h"        /* Basic data type */
#include "rdabt_drv.h"
#include "edrvp_debug.h"


#include "btd_config.h"
#include "tgt_btd_cfg.h"
#include "bt.h"
#include "pmd_m.h"
#include "hal_sys.h"
#include "mmi_trace.h"
extern CONST TGT_BTD_CONFIG_T* g_btdConfig;
extern uint8 rdabt_chip_sel;
//extern uint8 bt_sleephdl;


typedef enum
{
    TestMode_Signal=1,
    TestMode_NorSignal_Tx=2,
    TestMode_NorSignal_Rx=3
} TestMode_Enum;
TestMode_Enum  TestMode;

const uint32 rdabt_enable_spi2[][2] =
{
    {0x40240000,0x0004F398},//enable spi2

};




const uint32 rdabt_vco_test_578[][2] =
{
    {0x40240000,0x0004F398},//enable spi2
    {0x0000003f,0x00000000},//page 0
    {0x0000001a,0x000003D1},//1a
    {0x00000002,0x0000f180},//02
    {0x40240000,0x0000F298},//disable spi2
    {0x40200000,0x38001aaf},
    {0x40200020,0x00000157}
};

const uint32 rdabt_vco_test_r16[][2] =
{
    {0x40200020,0x00000043},
//{0x40240000,0x0004F398},//enable spi2
    {0x0000003f,0x00000000},//page 0
    {0x0000000e,0x00000930},//0e
    {0x0000001f,0x000003ff},//1f
    {0x00000020,0x000003ff},//20
    {0x00000002,0x0000f000},//00
//{0x40240000,0x0000F298},//disable spi2
};


const uint8 rdabt_rf_singlehop[] =
{
    0x01,0x13,0xFc,0x01,0x00
};

const uint8 rdabt_rf_inquiry[] =
{
    0x01,0x01,0x04, 0x05,0x33, 0x8B,0x9E, 0x30,0x00
};

const uint8 rdabt_enable_dut[] =
{
    0x01,0x03, 0x18, 0x00
};
const uint8 rdabt_enable_allscan[] =
{
    0x01, 0x1a, 0x0c, 0x01, 0x03
};
const uint8 rdabt_autoaccept_connect[] =
{
    0x01,0x05, 0x0c, 0x03, 0x02, 0x00, 0x02
};

const uint8 rdabt_hci_reset[] =
{
    0x01,0x03,0x0c,0x00
};





const uint32 rdabt_test_pskey_578[][2] =
{
    {0x40240000,0x1004fb98},//SCU: SPI2_clk_en pclk_SPI2_en
    {0x4024002c,0x00501000},//hostwake, cbt test, yuanwei_20100113
    {0x10000030,0x000000fd},//Max_output_power
    {0x10000038,0x0f054000},//Radio_setup_time
    {0x1000003c,0xa0a0bf30},//TxRx_low_setup_time TxRx_setup_time
    {0x10000034,0x05025040},//disable sleep
    {0x10000000,0x0000f000},//PSKEY: Modify Flag


};

const uint32 rdabt_dut_trap_5[][2] =
{
    {0x40180004,0x000146c4},//patch
    {0x40180024,0x00021800},//patch
    {0x4018000c,0x00013d84},//patch
    {0x4018002c,0x00021800},//patch



    {0x100000c0,0xe248fc8c},//DV packet and packet length patch
    {0x100000c4,0xe3540008},
    {0x100000c8,0x03a06009},
    {0x100000cc,0x11a06002},
    {0x100000d0,0xe1c060bc},
    {0x100000d4,0xe3a00470},
    {0x100000d8,0xe2800e61},
    {0x100000dc,0xe5901000},
    {0x100000e0,0xe2400e40},
    {0x100000e4,0xe5801000},
    {0x100000e8,0xe2800e40},
    {0x100000ec,0xe5901004},
    {0x100000f0,0xe2400e40},
    {0x100000f4,0xe5801004},
    {0x100000f8,0xe3a01bb9},
    {0x100000fc,0xe281ffc9},

    {0x40180008,0x0002e6fc},//patch
    {0x40180028,0x100000c0},//patch

    {0x10009db0,0xe3d20007},//patch//patch for power control
    {0x40180010,0x00007578},//patch
    {0x40180030,0x10009db0},//patch
    {0x40180000,0x0000000f},//patch

};


const uint32 rdabt_dut_trap_7[][2] =
{
    {0x40180000,0x00000000},
    {0x40180044,0x00000001},
    {0x40180004,0x0002fbdc},//for cmu200 DH3&DH5, 5868+ R7
    {0x40180024,0x00030010},
    {0x40180008,0x0002fbe8},
    {0x40180028,0x00001fbc},

    {0x1000affc,0xe3520006},//for power ctrl, 5868+ R7
    {0x4018000c,0x000073b4},
    {0x4018002c,0x1000affc},

    {0x40180000,0x00000007},//flag
};

const uint32 rdabt_dut_trap_8[][2] =
{

    {0x10007ffc,0xe352000b},//patch//patch for power control
    {0x40180004,0x000067fc},//patch
    {0x40180024,0x10007ffc},//patch


    {0x40180008,0x0002F394},//patch
    {0x40180028,0x00001788},//patch

    {0x4018000C,0x0002F3A0},//patch
    {0x4018002C,0x0002FBA8},//patch


    {0x40180000,0x00000007},//patch
};

const uint32 rdabt_test_pskey_10[][2]=
{
    {0x800000a4,0x00000000},//sleep
    {0x800000a8,0x0bbfbf30},//min power level
    {0x80000040,0x06000000} //PSKEY: modify flag
};

const uint32 rdabt_dut_trap_10[][2] =
{

    {0x40180004,0x0002F9C4},//patch//DH3$DH5 PATCH
    {0x40180024,0x00001760},//patch

    {0x40180008,0x0002F9D0},//patch
    {0x40180028,0x000301D8},//patch

    {0x40180000,0x00000003},//patch
};


const uint32 rdabt_dut_trap_11[][2] =
{

    {0x40180100,0x0002ED2C},
    {0x40180120,0x000309E4},
    {0x8000006c,0xe3a01007},
    {0x40180104,0x0001cec8},
    {0x40180124,0x8000006c},
    {0x40180108,0x00002A98},
    {0x40180128,0x00003074},
    {0x4018010c,0x0000f928},
    {0x4018012c,0x0001ab14},

    {0x8000000C,0xea00007b},
    {0x80000200,0xe3500000},
    {0x80000204,0x03a00ba5},
    {0x80000208,0x0280ff85},
    {0x8000020c,0xe5d40004},
    {0x80000210,0xe3500000},
    {0x80000214,0x03a00ba5},
    {0x80000218,0x0280ff85},
    {0x8000021c,0xe3a00ba5},
    {0x80000220,0xe280ff4d},
    {0x40180110,0x0002952c},
    {0x40180130,0x00032d3C},


    {0x40180000,0x00001f00},

};

const uint32 rdabt_test_pskey_11[][2]=
{
    {0x800000a4,0x00000000},//hostwake
    {0x800000a8,0x0bbfbf30},//min power level
    {0x80000040,0x06000000} //PSKEY: modify flag
};

const uint32 rdabt_vco_test_10[][2] =
{
//{0x40240000,0x0004F398},//enable spi2
    {0x0000003f,0x00000000},//page 0
    {0x0000002b,0x000000b2},//3//2b
    {0x00000002,0x0000f180},//02
//{0x000002FC,0x00000000},//page 0
    {0x40200020,0x00000041},//tx on
};

const uint32 rdabt_dut_trap_12[][2] =
{

    {0x40180100,0x000068b8},//inc power
    {0x40180120,0x000069f4},

    {0x40180104,0x000066b8},//dec power
    {0x40180124,0x000069f4},

//{0x40180108,0x0002ec44},////for cbt test
//{0x40180128,0x00000014},
    {0x4018010c,0x0000f8c4},///all rxon
    {0x4018012c,0x00026948},
    {0x40180000,0x00000b00},
//{0x40180000,0x00000700},
};

const uint32 rdabt_pskey_VCO_16[][2] =
{
    {0x40240000,0x0004f39c},   //; SPI2_CLK_EN PCLK_SPI2_EN
    {0x800000C0,0x0000000B},   //; CHIP_PS PSKEY: Total number -----------------
    {0x800000C4,0x003F0000},   //;         PSKEY: Page 0
    {0x800000C8,0x0041000B},   //;//TX_DC_OFFSET+/_
    {0x800000CC,0x0043BFC6},   //;//43H=BFC6
    {0x800000D0,0x00440500},   //;//44H=0580;
    {0x800000D4,0x00694075},   //;//TX_DC_OFFSET+/_
    {0x800000D8,0x006B10C0},   //;//
    {0x800000DC,0x003F0001},   //;//TX_DC_OFFSET+/_
    {0x800000E0,0x00453000},   //;
    {0x800000E4,0x0047F13B},   //;
    {0x800000E8,0x00490008},   //;
    {0x800000EC,0x003F0000},   //;
    {0x80000040,0x10000000},   //;         PSKEY: Flage
    {0x40240000,0x0000f29c},   //; SPI2_CLK_EN PCLK_SPI2_EN
};

#ifdef __RDA_CHIP_R17_8809__
const uint32 rdabt_test_17[][2] =
{
    {0x80000000,0xea001a67}, //           //0x800069a4  Lslcacc_TX_TEST_PATCH
    {0x40180004,0x0001a5e8}, //LSLCacc_Build_TX_Test
    {0x40180024,0x0002ee10}, //

    {0x80000004,0xea00003f}, //    b 0x80000108  //  b 0x800090dc
    {0x80000100,0xe51ff004}, //    ....    LDR      pc,0x80008e04
    {0x80000104,0x80006f6c}, //    4...    DCD    1076
    {0x80000108,0xebfffffc}, //   bl lslc_acc_bbtest_pka
    {0x8000010c,0xe8bd8070}, //   LDMFD    r13!,{r4-r6,pc}
    {0x40180008,0x0001dfa8}, //   //LSLCslot_Handle_PKA
    {0x40180028,0x0002ee14}, //


    {0x80000008,0xea0019d8}, //    //b 0x80006770  BT_Start_rx_test_process
    {0x4018000c,0x00020140}, //     //TC_MTK_177
    {0x4018002c,0x0002ee18}, //

    {0x8000000c,0xea00004f}, //              //0x80000150
    {0x80000150,0xe59f0014}, //      ldr r0,8000016c  bt_rx_test_enable  80006fec
    {0x80000154,0xe5900000}, //             ldr r0,[r0,0]
    {0x80000158,0xe3500000}, // cmp r0,0
    {0x8000015c,0x092d40f8}, //    .@-.    STMFDeq    r13!,{r3-r7,r14}
    {0x80000160,0x03a00a1e}, //  moveq r0,0x1e000
    {0x80000164,0x0280ff7e}, // addeq pc,r0,0x1f8
    {0x80000168,0xea001b0D}, //       //b bt_rx_Test_receive_hdr  0x80006da4
    {0x8000016c,0x80006fec}, //  //bt_rx_test_enable
    {0x40180010,0x0001e1f4}, //    //LSLCslot_Handle_PKD_RX_HDR
    {0x40180030,0x0002ee1c}, //

    {0x80000010,0xea000056}, //              //0x80000150
    {0x80000170,0xe59f0014}, //  ldr r0,8000018c  bt_rx_test_enable
    {0x80000174,0xe5900000}, //             ldr r0,[r0,0]
    {0x80000178,0xe3500000}, // cmp r0,0
    {0x8000017c,0x092d4070}, //    p@-.    STMFDeq    r13!,{r4-r6,r14}
    {0x80000180,0x03a00a1e}, //  moveq r0,0x1e000
    {0x80000184,0x0280f078}, // addeq pc,r0,0x78
    {0x80000188,0xea001ab6}, //       //b bt_rx_Test_process_packet  0x80006c68
    {0x8000018c,0x80006fec}, //  //bt_rx_test_enable
    {0x40180010,0x0001e074}, //  //LSLCslot_Handle_PKD
    {0x40180030,0x0002ee20}, //





    {0x80006700,0xe51ff004}, //    ....    LDR      pc,0x80006704
    {0x80006704,0x0000321c}, //    D2..    DCD    12868
    {0x80006708,0xe51ff004}, //    ....    LDR      pc,0x8000670c
    {0x8000670c,0x00003774}, //    .7..    DCD    14236
    {0x80006710,0xe51ff004}, //    ....    LDR      pc,0x80006714
    {0x80006714,0x00029368}, //    D...    DCD    168516
    {0x80006718,0xe51ff004}, //    ....    LDR      pc,0x8000671c
    {0x8000671c,0x00000c20}, //    H...    DCD    3144
    {0x80006720,0xe51ff004}, //    ....    LDR      pc,0x80006724
    {0x80006724,0x0001a234}, //    \...    DCD    107100
    {0x80006728,0xe51ff004}, //    ....    LDR      pc,0x8000672c
    {0x8000672c,0x0001a07c}, //    ....    DCD    106660
    {0x80006730,0xe51ff004}, //    ....    LDR      pc,0x80006734
    {0x80006734,0x000030c0}, //    .0..    DCD    12520
    {0x80006738,0xe51ff004}, //    ....    LDR      pc,0x8000673c
    {0x8000673c,0x0002ba7c}, //    X...    DCD    178520
    {0x80006740,0xe51ff004}, //    ....    LDR      pc,0x80006744
    {0x80006744,0x00004c0c}, //    4L..    DCD    19508
    {0x80006748,0xe51ff004}, //    ....    LDR      pc,0x8000674c
    {0x8000674c,0x000003b4}, //    ....    DCD    948
    {0x80006750,0xe51ff004}, //    ....    LDR      pc,0x80006754
    {0x80006754,0x0000524c}, //    tR..    DCD    21108
    {0x80006758,0xe51ff004}, //    ....    LDR      pc,0x8000675c
    {0x8000675c,0x0001aa30}, //    ....    DCD    108756
    {0x80006760,0xe51ff004}, //    ....    LDR      pc,0x80006764
    {0x80006764,0x00004c98}, //    .L..    DCD    19648
    {0x80006768,0xe51ff004}, //    ....    LDR      pc,0x8000676c
    {0x8000676c,0x00005278}, //    .R..    DCD    21152
    {0x80006770,0xe92d43fe}, //    .C-.    STMFD    r13!,{r1-r9,r14}
    {0x80006774,0xe5d05000}, //    .P..    LDRB     r5,[r0,#0]
    {0x80006778,0xe5d06001}, //    .`..    LDRB     r6,[r0,#1]
    {0x8000677c,0xe1a04000}, //    .@..    MOV      r4,r0
    {0x80006780,0xe2800002}, //    ....    ADD      r0,r0,#2
    {0x80006784,0xebffffdd}, //    ....    BL       $Ven$AA$L$$HCIparam_Get_Uint32  ; 0x80006700
    {0x80006788,0xe1a09000}, //    ....    MOV      r9,r0
    {0x8000678c,0xe5d47006}, //    .p..    LDRB     r7,[r4,#6]
    {0x80006790,0xe5d48007}, //    ....    LDRB     r8,[r4,#7]
    {0x80006794,0xe2840008}, //    ....    ADD      r0,r4,#8
    {0x80006798,0xebffffd8}, //    ....    BL       $Ven$AA$L$$HCIparam_Get_Uint32  ; 0x80006700
    {0x8000679c,0xe1a04000}, //    .@..    MOV      r4,r0
    {0x800067a0,0xe3a00000}, //    ....    MOV      r0,#0
    {0x800067a4,0xe59f10c4}, //    ....    LDR      r1,0x80006870
    {0x800067a8,0xebffffd6}, //    ....    BL       $Ven$AA$L$$HCeg_Command_Status_Event  ; 0x80006708
    {0x800067ac,0xe3550000}, //    ..U.    CMP      r5,#0
    {0x800067b0,0x0a00002c}, //    ,...    BEQ      0x80006868
    {0x800067b4,0xe3570000}, //    ..W.    CMP      r7,#0
    {0x800067b8,0x03c00480}, //    ....    BICEQ    r0,r0,#0x80000000
    {0x800067bc,0xe59f10b0}, //    ....    LDR      r1,0x80006874
    {0x800067c0,0x13800480}, //    ....    ORRNE    r0,r0,#0x80000000
    {0x800067c4,0xe5810014}, //    ....    STR      r0,[r1,#0x14]
    {0x800067c8,0xe59f00a8}, //    ....    LDR      r0,0x80006878
    {0x800067cc,0xe3a01001}, //    ....    MOV      r1,#1
    {0x800067d0,0xe8800012}, //    ....    STMIA    r0,{r1,r4}
    {0x800067d4,0xe3a01000}, //    ....    MOV      r1,#0
    {0x800067d8,0xe5801008}, //    ....    STR      r1,[r0,#8]
    {0x800067dc,0xe5808018}, //    ....    STR      r8,[r0,#0x18]
    {0x800067e0,0xe580100c}, //    ....    STR      r1,[r0,#0xc]
    {0x800067e4,0xe5801010}, //    ....    STR      r1,[r0,#0x10]
    {0x800067e8,0xe5801014}, //    ....    STR      r1,[r0,#0x14]
    {0x800067ec,0xe2860cc0}, //    ....    ADD      r0,r6,#0xc000
    {0x800067f0,0xe2800dc6}, //    ....    ADD      r0,r0,#0x3180
    {0x800067f4,0xe3800b80}, //    ....    ORR      r0,r0,#0x20000
    {0x800067f8,0xebffffc4}, //    ....    BL       $Ven$AA$L$$_HWradio_ProgNow  ; 0x80006710
    {0x800067fc,0xe28d2004}, //    . ..    ADD      r2,r13,#4
    {0x80006800,0xe28d1008}, //    ....    ADD      r1,r13,#8
    {0x80006804,0xe1a00009}, //    ....    MOV      r0,r9
    {0x80006808,0xebffffc2}, //    ....    BL       $Ven$AA$L$$BTaddr_Build_Sync_Word  ; 0x80006718
    {0x8000680c,0xe28d0004}, //    ....    ADD      r0,r13,#4
    {0x80006810,0xebffffc2}, //    ....    BL       $Ven$AA$L$$LSLCacc_Build_AccessCode  ; 0x80006720
    {0x80006814,0xeb00004f}, //    O...    BL       LSLCacc_Prepare_Rx_packet  ; 0x80006958
    {0x80006818,0xe59f005c}, //    \...    LDR      r0,0x8000687c
    {0x8000681c,0xe5d01001}, //    ....    LDRB     r1,[r0,#1]
    {0x80006820,0xe5d02008}, //    . ..    LDRB     r2,[r0,#8]
    {0x80006824,0xe26110dd}, //    ..a.    RSB      r1,r1,#0xdd
    {0x80006828,0xe2622f9b}, //    ./b.    RSB      r2,r2,#0x26c
    {0x8000682c,0xe3822c80}, //    .,..    ORR      r2,r2,#0x8000
    {0x80006830,0xe2811e40}, //    @...    ADD      r1,r1,#0x400
    {0x80006834,0xe1812802}, //    .(..    ORR      r2,r1,r2,LSL #16
    {0x80006838,0xe3a01470}, //    p...    MOV      r1,#0x70000000
    {0x8000683c,0xe5812140}, //    @!..    STR      r2,[r1,#0x140]
    {0x80006840,0xe5d02002}, //    . ..    LDRB     r2,[r0,#2]
    {0x80006844,0xe5d00009}, //    ....    LDRB     r0,[r0,#9]
    {0x80006848,0xe2622015}, //    . b.    RSB      r2,r2,#0x15
    {0x8000684c,0xe26000be}, //    ..`.    RSB      r0,r0,#0xbe
    {0x80006850,0xe2800e40}, //    @...    ADD      r0,r0,#0x400
    {0x80006854,0xe2822f40}, //    @/..    ADD      r2,r2,#0x100
    {0x80006858,0xe1820800}, //    ....    ORR      r0,r2,r0,LSL #16
    {0x8000685c,0xe3800c80}, //    ....    ORR      r0,r0,#0x8000
    {0x80006860,0xe5810158}, //    X...    STR      r0,[r1,#0x158]
    {0x80006864,0xe8bd83fe}, //    ....    LDMFD    r13!,{r1-r9,pc}
    {0x80006868,0xeb000004}, //    ....    BL       BT_start_rx_test_end  ; 0x80006880
    {0x8000686c,0xeafffffc}, //    ....    B        0x80006864
    {0x80006870,0x0000fd77}, //    w...    DCD    64887
    {0x80006874,0x80001720}, //    h...    DCD    2147517032    //_ctrl_reg_shadow
    {0x80006878,0x80006fec}, //    .o..    DCD    2147512300    //bt_rx_test_enable
    {0x8000687c,0x80002cf8}, //    @...    DCD    2147522624   //Driver_Data
    {0x80006880,0xe92d403e}, //    >@-.    STMFD    r13!,{r1-r5,r14}
    {0x80006884,0xe59f40bc}, //    .@..    LDR      r4,0x80006948
    {0x80006888,0xe3a00017}, //    ....    MOV      r0,#0x17
    {0x8000688c,0xe98d0011}, //    ....    STMIB    r13,{r0,r4}
    {0x80006890,0xe59f50b4}, //    .P..    LDR      r5,0x8000694c
    {0x80006894,0xe3a00000}, //    ....    MOV      r0,#0
    {0x80006898,0xe5850000}, //    ....    STR      r0,[r5,#0]
    {0x8000689c,0xe59f00ac}, //    ....    LDR      r0,0x80006950
    {0x800068a0,0xe3a01000}, //    ....    MOV      r1,#0
    {0x800068a4,0xe5902000}, //    . ..    LDR      r2,[r0,#0]
    {0x800068a8,0xe3c22003}, //    . ..    BIC      r2,r2,#3
    {0x800068ac,0xe1811002}, //    ....    ORR      r1,r1,r2
    {0x800068b0,0xe5801000}, //    ....    STR      r1,[r0,#0]
    {0x800068b4,0xe3a00000}, //    ....    MOV      r0,#0
    {0x800068b8,0xe3a01470}, //    p...    MOV      r1,#0x70000000
    {0x800068bc,0xe5810170}, //    p...    STR      r0,[r1,#0x170]
    {0x800068c0,0xe59f008c}, //    ....    LDR      r0,0x80006954
    {0x800068c4,0xe5d02001}, //    . ..    LDRB     r2,[r0,#1]
    {0x800068c8,0xe5d03008}, //    .0..    LDRB     r3,[r0,#8]
    {0x800068cc,0xe26220dd}, //    . b.    RSB      r2,r2,#0xdd
    {0x800068d0,0xe2822e40}, //    @...    ADD      r2,r2,#0x400
    {0x800068d4,0xe2633f9b}, //    .?c.    RSB      r3,r3,#0x26c
    {0x800068d8,0xe1822803}, //    .(..    ORR      r2,r2,r3,LSL #16
    {0x800068dc,0xe5812140}, //    @!..    STR      r2,[r1,#0x140]
    {0x800068e0,0xe5d02002}, //    . ..    LDRB     r2,[r0,#2]
    {0x800068e4,0xe5d00009}, //    ....    LDRB     r0,[r0,#9]
    {0x800068e8,0xe2622f90}, //    ./b.    RSB      r2,r2,#0x240
    {0x800068ec,0xe26000be}, //    ..`.    RSB      r0,r0,#0xbe
    {0x800068f0,0xe2800e40}, //    @...    ADD      r0,r0,#0x400
    {0x800068f4,0xe3800c80}, //    ....    ORR      r0,r0,#0x8000
    {0x800068f8,0xe1820800}, //    ....    ORR      r0,r2,r0,LSL #16
    {0x800068fc,0xe3800c80}, //    ....    ORR      r0,r0,#0x8000
    {0x80006900,0xe5810158}, //    X...    STR      r0,[r1,#0x158]
    {0x80006904,0xe2840007}, //    ....    ADD      r0,r4,#7
    {0x80006908,0xe5951010}, //    ....    LDR      r1,[r5,#0x10]
    {0x8000690c,0xebffff85}, //    ....    BL       $Ven$AA$L$$LMutils_Set_Uint32  ; 0x80006728
    {0x80006910,0xe284000b}, //    ....    ADD      r0,r4,#0xb
    {0x80006914,0xe5951008}, //    ....    LDR      r1,[r5,#8]
    {0x80006918,0xebffff82}, //    ....    BL       $Ven$AA$L$$LMutils_Set_Uint32  ; 0x80006728
    {0x8000691c,0xe284000f}, //    ....    ADD      r0,r4,#0xf
    {0x80006920,0xe5951014}, //    ....    LDR      r1,[r5,#0x14]
    {0x80006924,0xebffff7f}, //    ....    BL       $Ven$AA$L$$LMutils_Set_Uint32  ; 0x80006728
    {0x80006928,0xe2840013}, //    ....    ADD      r0,r4,#0x13
    {0x8000692c,0xe595100c}, //    ....    LDR      r1,[r5,#0xc]
    {0x80006930,0xebffff7c}, //    |...    BL       $Ven$AA$L$$LMutils_Set_Uint32  ; 0x80006728
    {0x80006934,0xe3a02000}, //    . ..    MOV      r2,#0
    {0x80006938,0xe28d1004}, //    ....    ADD      r1,r13,#4
    {0x8000693c,0xe28d0008}, //    ....    ADD      r0,r13,#8
    {0x80006940,0xebffff7a}, //    z...    BL       $Ven$AA$L$$HCIT_Chimera_16550_Tx_Char_Polled  ; 0x80006730
    {0x80006944,0xe8bd803e}, //    >...    LDMFD    r13!,{r1-r5,pc}
    {0x80006948,0x8000700c}, //    ....    DCD    2147526134      //rx_result_buff
    {0x8000694c,0x80006fec}, //    .o..    DCD    2147512300    //bt_rx_test_enable
    {0x80006950,0x70000048}, //    H..p    DCD    1879048264
    {0x80006954,0x80002cf8}, //    @...    DCD    2147522624    //Driver_Data
    {0x80006958,0xe92d4010}, //    .@-.    STMFD    r13!,{r4,r14}
    {0x8000695c,0xe3a01470}, //    p...    MOV      r1,#0x70000000
    {0x80006960,0xe5910020}, //     ...    LDR      r0,[r1,#0x20]
    {0x80006964,0xe3c0001f}, //    ....    BIC      r0,r0,#0x1f
    {0x80006968,0xe3c004ff}, //    ....    BIC      r0,r0,#0xff000000
    {0x8000696c,0xe59f402c}, //    ,@..    LDR      r4,0x800069a0
    {0x80006970,0xe3800041}, //    A...    ORR      r0,r0,#0x41
    {0x80006974,0xe5840008}, //    ....    STR      r0,[r4,#8]
    {0x80006978,0xe5910024}, //    $...    LDR      r0,[r1,#0x24]
    {0x8000697c,0xe3c004ff}, //    ....    BIC      r0,r0,#0xff000000
    {0x80006980,0xe3c008bf}, //    ....    BIC      r0,r0,#0xbf0000
    {0x80006984,0xe584000c}, //    ....    STR      r0,[r4,#0xc]
    {0x80006988,0xe3a00003}, //    ....    MOV      r0,#3
    {0x8000698c,0xe5c40020}, //     ...    STRB     r0,[r4,#0x20]
    {0x80006990,0xebffff68}, //    h...    BL       $Ven$AA$L$$_LSLCacc_Move_CC_Shadow_to_Hardware  ; 0x80006738
    {0x80006994,0xe5d40020}, //     ...    LDRB     r0,[r4,#0x20]
    {0x80006998,0xe8bd4010}, //    .@..    LDMFD    r13!,{r4,r14}
    {0x8000699c,0xeaffff67}, //    g...    B        $Ven$AA$L$$HW_set_rx_mode  ; 0x80006740
    {0x800069a0,0x80001720}, //    h...    DCD    2147517032   //_ctrl_reg_shadow
    {0x800069a4,0xe92d4038}, //    8@-.    STMFD    r13!,{r3-r5,r14}
    {0x800069a8,0xe59f5064}, //    dP..    LDR      r5,0x80006a14
    {0x800069ac,0xe1a04000}, //    .@..    MOV      r4,r0
    {0x800069b0,0xe3a00000}, //    ....    MOV      r0,#0
    {0x800069b4,0xe5850000}, //    ....    STR      r0,[r5,#0]
    {0x800069b8,0xe5d40000}, //    ....    LDRB     r0,[r4,#0]
    {0x800069bc,0xe350004f}, //    O.P.    CMP      r0,#0x4f
    {0x800069c0,0x11a00004}, //    ....    MOVNE    r0,r4
    {0x800069c4,0x18bd4038}, //    8@..    LDMNEFD  r13!,{r3-r5,r14}
    {0x800069c8,0x1a000012}, //    ....    BNE      Lslcacc_Tx_test_start_test  ; 0x80006a18
    {0x800069cc,0xe5950000}, //    ....    LDR      r0,[r5,#0]
    {0x800069d0,0xe3500000}, //    ..P.    CMP      r0,#0
    {0x800069d4,0x1a00000d}, //    ....    BNE      0x80006a10
    {0x800069d8,0xe5d41000}, //    ....    LDRB     r1,[r4,#0]
    {0x800069dc,0xe3a0004f}, //    O...    MOV      r0,#0x4f
    {0x800069e0,0xebffff58}, //    X...    BL       $Ven$AA$L$$__rt_udiv  ; 0x80006748
    {0x800069e4,0xe5c41000}, //    ....    STRB     r1,[r4,#0]
    {0x800069e8,0xe1a00004}, //    ....    MOV      r0,r4
    {0x800069ec,0xeb000009}, //    ....    BL       Lslcacc_Tx_test_start_test  ; 0x80006a18
    {0x800069f0,0xe3a01000}, //    ....    MOV      r1,#0
    {0x800069f4,0xe3a00064}, //    d...    MOV      r0,#0x64
    {0x800069f8,0xebffff54}, //    T...    BL       $Ven$AA$L$$HWdelay_Wait_For_ms  ; 0x80006750
    {0x800069fc,0xebffff55}, //    U...    BL       $Ven$AA$L$$LSLCacc_Exit_Test  ; 0x80006758
    {0x80006a00,0xe5d40000}, //    ....    LDRB     r0,[r4,#0]
    {0x80006a04,0xe2800001}, //    ....    ADD      r0,r0,#1
    {0x80006a08,0xe5c40000}, //    ....    STRB     r0,[r4,#0]
    {0x80006a0c,0xeaffffee}, //    ....    B        0x800069cc
    {0x80006a10,0xe8bd8038}, //    8...    LDMFD    r13!,{r3-r5,pc}
    {0x80006a14,0x80006fe8}, //    .o..    DCD    2147512296  //bbtest_stop
    {0x80006a18,0xe5d01002}, //    ....    LDRB     r1,[r0,#2]
    {0x80006a1c,0xe3510000}, //    ..Q.    CMP      r1,#0
    {0x80006a20,0x0a000002}, //    ....    BEQ      Lslcacc_build_Tx_Test_burst  ; 0x80006a30
    {0x80006a24,0xe3510001}, //    ..Q.    CMP      r1,#1
    {0x80006a28,0x0a000053}, //    S...    BEQ      Lslcacc_build_Tx_Test_continues  ; 0x80006b7c
    {0x80006a2c,0xe1a0f00e}, //    ....    MOV      pc,r14
    {0x80006a30,0xe92d4038}, //    8@-.    STMFD    r13!,{r3-r5,r14}
    {0x80006a34,0xe3a05470}, //    pT..    MOV      r5,#0x70000000
    {0x80006a38,0xe1a04000}, //    .@..    MOV      r4,r0
    {0x80006a3c,0xe5950020}, //     ...    LDR      r0,[r5,#0x20]
    {0x80006a40,0xe5d41014}, //    ....    LDRB     r1,[r4,#0x14]
    {0x80006a44,0xe3c00007}, //    ....    BIC      r0,r0,#7
    {0x80006a48,0xe2011007}, //    ....    AND      r1,r1,#7
    {0x80006a4c,0xe1810000}, //    ....    ORR      r0,r1,r0
    {0x80006a50,0xe5850020}, //     ...    STR      r0,[r5,#0x20]
    {0x80006a54,0xe5950024}, //    $...    LDR      r0,[r5,#0x24]
    {0x80006a58,0xe3c00480}, //    ....    BIC      r0,r0,#0x80000000
    {0x80006a5c,0xe5850024}, //    $...    STR      r0,[r5,#0x24]
    {0x80006a60,0xe5d40014}, //    ....    LDRB     r0,[r4,#0x14]
    {0x80006a64,0xe59f1100}, //    ....    LDR      r1,0x80006b6c
    {0x80006a68,0xe5c10000}, //    ....    STRB     r0,[r1,#0]
    {0x80006a6c,0xe5d40006}, //    ....    LDRB     r0,[r4,#6]
    {0x80006a70,0xe3a01003}, //    ....    MOV      r1,#3
    {0x80006a74,0xe0810280}, //    ....    ADD      r0,r1,r0,LSL #5
    {0x80006a78,0xe5d41000}, //    ....    LDRB     r1,[r4,#0]
    {0x80006a7c,0xe1a00b80}, //    ....    MOV      r0,r0,LSL #23
    {0x80006a80,0xe0810820}, //     ...    ADD      r0,r1,r0,LSR #16
    {0x80006a84,0xe3800b80}, //    ....    ORR      r0,r0,#0x20000
    {0x80006a88,0xebffff20}, //     ...    BL       $Ven$AA$L$$_HWradio_ProgNow  ; 0x80006710
    {0x80006a8c,0xe5950024}, //    $...    LDR      r0,[r5,#0x24]
    {0x80006a90,0xe3800e80}, //    ....    ORR      r0,r0,#0x800
    {0x80006a94,0xe5850024}, //    $...    STR      r0,[r5,#0x24]
    {0x80006a98,0xe5d40001}, //    ....    LDRB     r0,[r4,#1]
    {0x80006a9c,0xe5d41002}, //    ....    LDRB     r1,[r4,#2]
    {0x80006aa0,0xe1800081}, //    ....    ORR      r0,r0,r1,LSL #1
    {0x80006aa4,0xe5d41003}, //    ....    LDRB     r1,[r4,#3]
    {0x80006aa8,0xe1800181}, //    ....    ORR      r0,r0,r1,LSL #3
    {0x80006aac,0xe1d411b0}, //    ....    LDRH     r1,[r4,#0x10]
    {0x80006ab0,0xe1800381}, //    ....    ORR      r0,r0,r1,LSL #7
    {0x80006ab4,0xe1d411b2}, //    ....    LDRH     r1,[r4,#0x12]
    {0x80006ab8,0xe1800801}, //    ....    ORR      r0,r0,r1,LSL #16
    {0x80006abc,0xe5d41005}, //    ....    LDRB     r1,[r4,#5]
    {0x80006ac0,0xe3510002}, //    ..Q.    CMP      r1,#2
    {0x80006ac4,0x03800004}, //    ....    ORREQ    r0,r0,#4
    {0x80006ac8,0xe58500f4}, //    ....    STR      r0,[r5,#0xf4]
    {0x80006acc,0xe59f009c}, //    ....    LDR      r0,0x80006b70
    {0x80006ad0,0xe5850008}, //    ....    STR      r0,[r5,#8]
    {0x80006ad4,0xe59f0098}, //    ....    LDR      r0,0x80006b74
    {0x80006ad8,0xe585000c}, //    ....    STR      r0,[r5,#0xc]
    {0x80006adc,0xe5950030}, //    0...    LDR      r0,[r5,#0x30]
    {0x80006ae0,0xe5d41005}, //    ....    LDRB     r1,[r4,#5]
    {0x80006ae4,0xe1a00720}, //     ...    MOV      r0,r0,LSR #14
    {0x80006ae8,0xe1a00700}, //    ....    MOV      r0,r0,LSL #14
    {0x80006aec,0xe3a02000}, //    . ..    MOV      r2,#0
    {0x80006af0,0xe3510000}, //    ..Q.    CMP      r1,#0
    {0x80006af4,0x05852034}, //    4 ..    STREQ    r2,[r5,#0x34]
    {0x80006af8,0xe3800b80}, //    ....    ORR      r0,r0,#0x20000
    {0x80006afc,0x13a01480}, //    ....    MOVNE    r1,#0x80000000
    {0x80006b00,0x15851034}, //    4...    STRNE    r1,[r5,#0x34]
    {0x80006b04,0xe1d411b6}, //    ....    LDRH     r1,[r4,#0x16]
    {0x80006b08,0xe0810000}, //    ....    ADD      r0,r1,r0
    {0x80006b0c,0xe5d41004}, //    ....    LDRB     r1,[r4,#4]
    {0x80006b10,0xe0800501}, //    ....    ADD      r0,r0,r1,LSL #10
    {0x80006b14,0xe5850030}, //    0...    STR      r0,[r5,#0x30]
    {0x80006b18,0xe59f0058}, //    X...    LDR      r0,0x80006b78
    {0x80006b1c,0xe5d01001}, //    ....    LDRB     r1,[r0,#1]
    {0x80006b20,0xe5d03008}, //    .0..    LDRB     r3,[r0,#8]
    {0x80006b24,0xe26110dd}, //    ..a.    RSB      r1,r1,#0xdd
    {0x80006b28,0xe2811e40}, //    @...    ADD      r1,r1,#0x400
    {0x80006b2c,0xe2633f9b}, //    .?c.    RSB      r3,r3,#0x26c
    {0x80006b30,0xe1811803}, //    ....    ORR      r1,r1,r3,LSL #16
    {0x80006b34,0xe5851140}, //    @...    STR      r1,[r5,#0x140]
    {0x80006b38,0xe5d01002}, //    ....    LDRB     r1,[r0,#2]
    {0x80006b3c,0xe5d00009}, //    ....    LDRB     r0,[r0,#9]
    {0x80006b40,0xe2611f90}, //    ..a.    RSB      r1,r1,#0x240
    {0x80006b44,0xe26000be}, //    ..`.    RSB      r0,r0,#0xbe
    {0x80006b48,0xe2800e40}, //    @...    ADD      r0,r0,#0x400
    {0x80006b4c,0xe3800c80}, //    ....    ORR      r0,r0,#0x8000
    {0x80006b50,0xe1810800}, //    ....    ORR      r0,r1,r0,LSL #16
    {0x80006b54,0xe5850158}, //    X...    STR      r0,[r5,#0x158]
    {0x80006b58,0xe5852170}, //    p!..    STR      r2,[r5,#0x170]
    {0x80006b5c,0xe5950028}, //    (...    LDR      r0,[r5,#0x28]
    {0x80006b60,0xe3c00040}, //    @...    BIC      r0,r0,#0x40
    {0x80006b64,0xe5850028}, //    (...    STR      r0,[r5,#0x28]
    {0x80006b68,0xe8bd8038}, //    8...    LDMFD    r13!,{r3-r5,pc}
    {0x80006b6c,0x80003aa8}, //    ....    DCD    2147526128     //RDA_TEST_MODE_AM_ADDR
    {0x80006b70,0x331a3ae2}, //    .:.3    DCD    857357026      //sync word
    {0x80006b74,0x4e7a2cce}, //    .,zN    DCD    1316629710      //sync word
    {0x80006b78,0x80002cf8}, //    @...    DCD    2147522624    //Driver_Data
    {0x80006b7c,0xe92d4010}, //    .@-.    STMFD    r13!,{r4,r14}
    {0x80006b80,0xe1a04000}, //    .@..    MOV      r4,r0
    {0x80006b84,0xe5d00006}, //    ....    LDRB     r0,[r0,#6]
    {0x80006b88,0xe3a01003}, //    ....    MOV      r1,#3
    {0x80006b8c,0xe0810280}, //    ....    ADD      r0,r1,r0,LSL #5
    {0x80006b90,0xe5d41000}, //    ....    LDRB     r1,[r4,#0]
    {0x80006b94,0xe1a00b80}, //    ....    MOV      r0,r0,LSL #23
    {0x80006b98,0xe0810820}, //     ...    ADD      r0,r1,r0,LSR #16
    {0x80006b9c,0xe3800b80}, //    ....    ORR      r0,r0,#0x20000
    {0x80006ba0,0xebfffeda}, //    ....    BL       $Ven$AA$L$$_HWradio_ProgNow  ; 0x80006710
    {0x80006ba4,0xe3a02470}, //    p$..    MOV      r2,#0x70000000
    {0x80006ba8,0xe5920024}, //    $...    LDR      r0,[r2,#0x24]
    {0x80006bac,0xe3800e80}, //    ....    ORR      r0,r0,#0x800
    {0x80006bb0,0xe5820024}, //    $...    STR      r0,[r2,#0x24]
    {0x80006bb4,0xe5d40001}, //    ....    LDRB     r0,[r4,#1]
    {0x80006bb8,0xe5d41002}, //    ....    LDRB     r1,[r4,#2]
    {0x80006bbc,0xe1800081}, //    ....    ORR      r0,r0,r1,LSL #1
    {0x80006bc0,0xe5d41003}, //    ....    LDRB     r1,[r4,#3]
    {0x80006bc4,0xe1800181}, //    ....    ORR      r0,r0,r1,LSL #3
    {0x80006bc8,0xe1d411b0}, //    ....    LDRH     r1,[r4,#0x10]
    {0x80006bcc,0xe1800381}, //    ....    ORR      r0,r0,r1,LSL #7
    {0x80006bd0,0xe1d411b2}, //    ....    LDRH     r1,[r4,#0x12]
    {0x80006bd4,0xe5d43005}, //    .0..    LDRB     r3,[r4,#5]
    {0x80006bd8,0xe1801801}, //    ....    ORR      r1,r0,r1,LSL #16
    {0x80006bdc,0xe3a00000}, //    ....    MOV      r0,#0
    {0x80006be0,0xe3530000}, //    ..S.    CMP      r3,#0
    {0x80006be4,0x05820034}, //    4...    STREQ    r0,[r2,#0x34]
    {0x80006be8,0x13a03480}, //    .4..    MOVNE    r3,#0x80000000
    {0x80006bec,0x15823034}, //    40..    STRNE    r3,[r2,#0x34]
    {0x80006bf0,0xe5d43005}, //    .0..    LDRB     r3,[r4,#5]
    {0x80006bf4,0xe1a04002}, //    .@..    MOV      r4,r2
    {0x80006bf8,0xe3530002}, //    ..S.    CMP      r3,#2
    {0x80006bfc,0x03811004}, //    ....    ORREQ    r1,r1,#4
    {0x80006c00,0xe58210f4}, //    ....    STR      r1,[r2,#0xf4]
    {0x80006c04,0xe59f1058}, //    X...    LDR      r1,0x80006c64
    {0x80006c08,0xe5d12001}, //    . ..    LDRB     r2,[r1,#1]
    {0x80006c0c,0xe5d13008}, //    .0..    LDRB     r3,[r1,#8]
    {0x80006c10,0xe26220dd}, //    . b.    RSB      r2,r2,#0xdd
    {0x80006c14,0xe2822e40}, //    @...    ADD      r2,r2,#0x400
    {0x80006c18,0xe2633f9b}, //    .?c.    RSB      r3,r3,#0x26c
    {0x80006c1c,0xe1822803}, //    .(..    ORR      r2,r2,r3,LSL #16
    {0x80006c20,0xe3822c80}, //    .,..    ORR      r2,r2,#0x8000
    {0x80006c24,0xe5842140}, //    @!..    STR      r2,[r4,#0x140]
    {0x80006c28,0xe5d12002}, //    . ..    LDRB     r2,[r1,#2]
    {0x80006c2c,0xe5d11009}, //    ....    LDRB     r1,[r1,#9]
    {0x80006c30,0xe2622f90}, //    ./b.    RSB      r2,r2,#0x240
    {0x80006c34,0xe26110be}, //    ..a.    RSB      r1,r1,#0xbe
    {0x80006c38,0xe2811e40}, //    @...    ADD      r1,r1,#0x400
    {0x80006c3c,0xe3811c80}, //    ....    ORR      r1,r1,#0x8000
    {0x80006c40,0xe1821801}, //    ....    ORR      r1,r2,r1,LSL #16
    {0x80006c44,0xe5841158}, //    X...    STR      r1,[r4,#0x158]
    {0x80006c48,0xe5840170}, //    p...    STR      r0,[r4,#0x170]
    {0x80006c4c,0xe3a00003}, //    ....    MOV      r0,#3
    {0x80006c50,0xebfffec2}, //    ....    BL       $Ven$AA$L$$HW_set_tx_mode  ; 0x80006760
    {0x80006c54,0xe5940028}, //    (...    LDR      r0,[r4,#0x28]
    {0x80006c58,0xe3800040}, //    @...    ORR      r0,r0,#0x40
    {0x80006c5c,0xe5840028}, //    (...    STR      r0,[r4,#0x28]
    {0x80006c60,0xe8bd8010}, //    ....    LDMFD    r13!,{r4,pc}
    {0x80006c64,0x80002cf8}, //    @...    DCD    2147522624    //Driver_Data
    {0x80006c68,0xe92d41f0}, //    .A-.    STMFD    r13!,{r4-r8,r14}
    {0x80006c6c,0xe3a07470}, //    pt..    MOV      r7,#0x70000000
    {0x80006c70,0xe5970050}, //    P...    LDR      r0,[r7,#0x50]
    {0x80006c74,0xe59f6118}, //    .a..    LDR      r6,0x80006d94
    {0x80006c78,0xe20050ff}, //    .P..    AND      r5,r0,#0xff
    {0x80006c7c,0xe596000c}, //    ....    LDR      r0,[r6,#0xc]
    {0x80006c80,0xe0800185}, //    ....    ADD      r0,r0,r5,LSL #3
    {0x80006c84,0xe586000c}, //    ....    STR      r0,[r6,#0xc]
    {0x80006c88,0xe5960008}, //    ....    LDR      r0,[r6,#8]
    {0x80006c8c,0xe2800001}, //    ....    ADD      r0,r0,#1
    {0x80006c90,0xe5860008}, //    ....    STR      r0,[r6,#8]
    {0x80006c94,0xe5970050}, //    P...    LDR      r0,[r7,#0x50]
    {0x80006c98,0xe1a00280}, //    ....    MOV      r0,r0,LSL #5
    {0x80006c9c,0xe1b00fa0}, //    ....    MOVS     r0,r0,LSR #31
    {0x80006ca0,0x0a000012}, //    ....    BEQ      0x80006cf0
    {0x80006ca4,0xe5960010}, //    ....    LDR      r0,[r6,#0x10]
    {0x80006ca8,0xe3a04000}, //    .@..    MOV      r4,#0
    {0x80006cac,0xe2800001}, //    ....    ADD      r0,r0,#1
    {0x80006cb0,0xe5860010}, //    ....    STR      r0,[r6,#0x10]
    {0x80006cb4,0xe59f80dc}, //    ....    LDR      r8,0x80006d98
    {0x80006cb8,0xea00000a}, //    ....    B        0x80006ce8
    {0x80006cbc,0xe59f00d8}, //    ....    LDR      r0,0x80006d9c
    {0x80006cc0,0xe5961018}, //    ....    LDR      r1,[r6,#0x18]
    {0x80006cc4,0xe7d00004}, //    ....    LDRB     r0,[r0,r4]
    {0x80006cc8,0xe7d81001}, //    ....    LDRB     r1,[r8,r1]
    {0x80006ccc,0xe1510000}, //    ..Q.    CMP      r1,r0
    {0x80006cd0,0x0a000003}, //    ....    BEQ      0x80006ce4
    {0x80006cd4,0xeb000069}, //    i...    BL       bt_rx_test_calc_bit_error  ; 0x80006e80
    {0x80006cd8,0xe5961014}, //    ....    LDR      r1,[r6,#0x14]
    {0x80006cdc,0xe0800001}, //    ....    ADD      r0,r0,r1
    {0x80006ce0,0xe5860014}, //    ....    STR      r0,[r6,#0x14]
    {0x80006ce4,0xe2844001}, //    .@..    ADD      r4,r4,#1
    {0x80006ce8,0xe1540005}, //    ..T.    CMP      r4,r5
    {0x80006cec,0x3afffff2}, //    ...:    BCC      0x80006cbc
    {0x80006cf0,0xe5b61004}, //    ....    LDR      r1,[r6,#4]!
    {0x80006cf4,0xe5960004}, //    ....    LDR      r0,[r6,#4]
    {0x80006cf8,0xe1500001}, //    ..P.    CMP      r0,r1
    {0x80006cfc,0x08bd41f0}, //    .A..    LDMEQFD  r13!,{r4-r8,r14}
    {0x80006d00,0x0afffede}, //    ....    BEQ      BT_start_rx_test_end  ; 0x80006880
    {0x80006d04,0xe59f4094}, //    .@..    LDR      r4,0x80006da0
    {0x80006d08,0xe5d40001}, //    ....    LDRB     r0,[r4,#1]
    {0x80006d0c,0xe5d41008}, //    ....    LDRB     r1,[r4,#8]
    {0x80006d10,0xe26000dd}, //    ..`.    RSB      r0,r0,#0xdd
    {0x80006d14,0xe2800e40}, //    @...    ADD      r0,r0,#0x400
    {0x80006d18,0xe2611f9b}, //    ..a.    RSB      r1,r1,#0x26c
    {0x80006d1c,0xe1800801}, //    ....    ORR      r0,r0,r1,LSL #16
    {0x80006d20,0xe5870140}, //    @...    STR      r0,[r7,#0x140]
    {0x80006d24,0xe5d40002}, //    ....    LDRB     r0,[r4,#2]
    {0x80006d28,0xe5d41009}, //    ....    LDRB     r1,[r4,#9]
    {0x80006d2c,0xe2600015}, //    ..`.    RSB      r0,r0,#0x15
    {0x80006d30,0xe26110be}, //    ..a.    RSB      r1,r1,#0xbe
    {0x80006d34,0xe2811e40}, //    @...    ADD      r1,r1,#0x400
    {0x80006d38,0xe2800f40}, //    @...    ADD      r0,r0,#0x100
    {0x80006d3c,0xe1800801}, //    ....    ORR      r0,r0,r1,LSL #16
    {0x80006d40,0xe3800c80}, //    ....    ORR      r0,r0,#0x8000
    {0x80006d44,0xe5870158}, //    X...    STR      r0,[r7,#0x158]
    {0x80006d48,0xebffff02}, //    ....    BL       LSLCacc_Prepare_Rx_packet  ; 0x80006958
    {0x80006d4c,0xe5d40001}, //    ....    LDRB     r0,[r4,#1]
    {0x80006d50,0xe5d41008}, //    ....    LDRB     r1,[r4,#8]
    {0x80006d54,0xe26000dd}, //    ..`.    RSB      r0,r0,#0xdd
    {0x80006d58,0xe2611f9b}, //    ..a.    RSB      r1,r1,#0x26c
    {0x80006d5c,0xe3811c80}, //    ....    ORR      r1,r1,#0x8000
    {0x80006d60,0xe2800e40}, //    @...    ADD      r0,r0,#0x400
    {0x80006d64,0xe1800801}, //    ....    ORR      r0,r0,r1,LSL #16
    {0x80006d68,0xe5870140}, //    @...    STR      r0,[r7,#0x140]
    {0x80006d6c,0xe5d40002}, //    ....    LDRB     r0,[r4,#2]
    {0x80006d70,0xe5d41009}, //    ....    LDRB     r1,[r4,#9]
    {0x80006d74,0xe2600015}, //    ..`.    RSB      r0,r0,#0x15
    {0x80006d78,0xe26110be}, //    ..a.    RSB      r1,r1,#0xbe
    {0x80006d7c,0xe2811e40}, //    @...    ADD      r1,r1,#0x400
    {0x80006d80,0xe2800f40}, //    @...    ADD      r0,r0,#0x100
    {0x80006d84,0xe1800801}, //    ....    ORR      r0,r0,r1,LSL #16
    {0x80006d88,0xe3800c80}, //    ....    ORR      r0,r0,#0x8000
    {0x80006d8c,0xe5870158}, //    X...    STR      r0,[r7,#0x158]
    {0x80006d90,0xe8bd81f0}, //    ....    LDMFD    r13!,{r4-r8,pc}
    {0x80006d94,0x80006fec}, //    .o..    DCD    2147512300  //bt_rx_test_enable
    {0x80006d98,0x80007008}, //    ....    DCD    2147526130  //rx_patten_data
    {0x80006d9c,0x70000600}, //    ...p    DCD    1879049728
    {0x80006da0,0x80002cf8}, //    @...    DCD    2147522624   //Driver_Data
    {0x80006da4,0xe92d4038}, //    8@-.    STMFD    r13!,{r3-r5,r14}
    {0x80006da8,0xe3a05470}, //    pT..    MOV      r5,#0x70000000
    {0x80006dac,0xe5950050}, //    P...    LDR      r0,[r5,#0x50]
    {0x80006db0,0xe1a00300}, //    ....    MOV      r0,r0,LSL #6
    {0x80006db4,0xe1b00fa0}, //    ....    MOVS     r0,r0,LSR #31
    {0x80006db8,0x0a00002d}, //    -...    BEQ      0x80006e74
    {0x80006dbc,0xe59f00b4}, //    ....    LDR      r0,0x80006e78
    {0x80006dc0,0xe5901008}, //    ....    LDR      r1,[r0,#8]
    {0x80006dc4,0xe2811001}, //    ....    ADD      r1,r1,#1
    {0x80006dc8,0xe5801008}, //    ....    STR      r1,[r0,#8]
    {0x80006dcc,0xe5902010}, //    . ..    LDR      r2,[r0,#0x10]
    {0x80006dd0,0xe2822001}, //    . ..    ADD      r2,r2,#1
    {0x80006dd4,0xe5802010}, //    . ..    STR      r2,[r0,#0x10]
    {0x80006dd8,0xe5900004}, //    ....    LDR      r0,[r0,#4]
    {0x80006ddc,0xe1510000}, //    ..Q.    CMP      r1,r0
    {0x80006de0,0x08bd4038}, //    8@..    LDMEQFD  r13!,{r3-r5,r14}
    {0x80006de4,0x0afffea5}, //    ....    BEQ      BT_start_rx_test_end  ; 0x80006880
    {0x80006de8,0xe59f408c}, //    .@..    LDR      r4,0x80006e7c
    {0x80006dec,0xe5d40001}, //    ....    LDRB     r0,[r4,#1]
    {0x80006df0,0xe5d41008}, //    ....    LDRB     r1,[r4,#8]
    {0x80006df4,0xe26000dd}, //    ..`.    RSB      r0,r0,#0xdd
    {0x80006df8,0xe2800e40}, //    @...    ADD      r0,r0,#0x400
    {0x80006dfc,0xe2611f9b}, //    ..a.    RSB      r1,r1,#0x26c
    {0x80006e00,0xe1800801}, //    ....    ORR      r0,r0,r1,LSL #16
    {0x80006e04,0xe5850140}, //    @...    STR      r0,[r5,#0x140]
    {0x80006e08,0xe5d40002}, //    ....    LDRB     r0,[r4,#2]
    {0x80006e0c,0xe5d41009}, //    ....    LDRB     r1,[r4,#9]
    {0x80006e10,0xe2600015}, //    ..`.    RSB      r0,r0,#0x15
    {0x80006e14,0xe26110be}, //    ..a.    RSB      r1,r1,#0xbe
    {0x80006e18,0xe2811e40}, //    @...    ADD      r1,r1,#0x400
    {0x80006e1c,0xe2800f40}, //    @...    ADD      r0,r0,#0x100
    {0x80006e20,0xe1800801}, //    ....    ORR      r0,r0,r1,LSL #16
    {0x80006e24,0xe3800c80}, //    ....    ORR      r0,r0,#0x8000
    {0x80006e28,0xe5850158}, //    X...    STR      r0,[r5,#0x158]
    {0x80006e2c,0xebfffec9}, //    ....    BL       LSLCacc_Prepare_Rx_packet  ; 0x80006958
    {0x80006e30,0xe5d40001}, //    ....    LDRB     r0,[r4,#1]
    {0x80006e34,0xe5d41008}, //    ....    LDRB     r1,[r4,#8]
    {0x80006e38,0xe26000dd}, //    ..`.    RSB      r0,r0,#0xdd
    {0x80006e3c,0xe2611f9b}, //    ..a.    RSB      r1,r1,#0x26c
    {0x80006e40,0xe3811c80}, //    ....    ORR      r1,r1,#0x8000
    {0x80006e44,0xe2800e40}, //    @...    ADD      r0,r0,#0x400
    {0x80006e48,0xe1800801}, //    ....    ORR      r0,r0,r1,LSL #16
    {0x80006e4c,0xe5850140}, //    @...    STR      r0,[r5,#0x140]
    {0x80006e50,0xe5d40002}, //    ....    LDRB     r0,[r4,#2]
    {0x80006e54,0xe5d41009}, //    ....    LDRB     r1,[r4,#9]
    {0x80006e58,0xe2600015}, //    ..`.    RSB      r0,r0,#0x15
    {0x80006e5c,0xe26110be}, //    ..a.    RSB      r1,r1,#0xbe
    {0x80006e60,0xe2811e40}, //    @...    ADD      r1,r1,#0x400
    {0x80006e64,0xe2800f40}, //    @...    ADD      r0,r0,#0x100
    {0x80006e68,0xe1800801}, //    ....    ORR      r0,r0,r1,LSL #16
    {0x80006e6c,0xe3800c80}, //    ....    ORR      r0,r0,#0x8000
    {0x80006e70,0xe5850158}, //    X...    STR      r0,[r5,#0x158]
    {0x80006e74,0xe8bd8038}, //    8...    LDMFD    r13!,{r3-r5,pc}
    {0x80006e78,0x80006fec}, //    .o..    DCD    2147512300    //bt_rx_test_enable
    {0x80006e7c,0x80002cf8}, //    @...    DCD    2147522624     //Driver_Data
    {0x80006e80,0xe59f20e0}, //    . ..    LDR      r2,0x80006f68
    {0x80006e84,0xe1a01000}, //    ....    MOV      r1,r0
    {0x80006e88,0xe5922018}, //    . ..    LDR      r2,[r2,#0x18]
    {0x80006e8c,0xe3a00000}, //    ....    MOV      r0,#0
    {0x80006e90,0xe3a0c001}, //    ....    MOV      r12,#1
    {0x80006e94,0xe3520000}, //    ..R.    CMP      r2,#0
    {0x80006e98,0x0a000009}, //    ....    BEQ      0x80006ec4
    {0x80006e9c,0xe3520001}, //    ..R.    CMP      r2,#1
    {0x80006ea0,0x03a02000}, //    . ..    MOVEQ    r2,#0
    {0x80006ea4,0x0a00000e}, //    ....    BEQ      0x80006ee4
    {0x80006ea8,0xe3520002}, //    ..R.    CMP      r2,#2
    {0x80006eac,0x03a02000}, //    . ..    MOVEQ    r2,#0
    {0x80006eb0,0x0a000018}, //    ....    BEQ      0x80006f18
    {0x80006eb4,0xe3520003}, //    ..R.    CMP      r2,#3
    {0x80006eb8,0x11a0f00e}, //    ....    MOVNE    pc,r14
    {0x80006ebc,0xe3a02000}, //    . ..    MOV      r2,#0
    {0x80006ec0,0xea000020}, //     ...    B        0x80006f48
    {0x80006ec4,0xe111021c}, //    ....    TST      r1,r12,LSL r2
    {0x80006ec8,0x12800001}, //    ....    ADDNE    r0,r0,#1
    {0x80006ecc,0xe2822001}, //    . ..    ADD      r2,r2,#1
    {0x80006ed0,0xe20220ff}, //    . ..    AND      r2,r2,#0xff
    {0x80006ed4,0x120000ff}, //    ....    ANDNE    r0,r0,#0xff
    {0x80006ed8,0xe3520008}, //    ..R.    CMP      r2,#8
    {0x80006edc,0x3afffff8}, //    ...:    BCC      0x80006ec4
    {0x80006ee0,0xe1a0f00e}, //    ....    MOV      pc,r14
    {0x80006ee4,0xe1a03082}, //    .0..    MOV      r3,r2,LSL #1
    {0x80006ee8,0xe111031c}, //    ....    TST      r1,r12,LSL r3
    {0x80006eec,0xe08c3082}, //    .0..    ADD      r3,r12,r2,LSL #1
    {0x80006ef0,0x12800001}, //    ....    ADDNE    r0,r0,#1
    {0x80006ef4,0x120000ff}, //    ....    ANDNE    r0,r0,#0xff
    {0x80006ef8,0xe111031c}, //    ....    TST      r1,r12,LSL r3
    {0x80006efc,0x02800001}, //    ....    ADDEQ    r0,r0,#1
    {0x80006f00,0xe2822001}, //    . ..    ADD      r2,r2,#1
    {0x80006f04,0xe20220ff}, //    . ..    AND      r2,r2,#0xff
    {0x80006f08,0x020000ff}, //    ....    ANDEQ    r0,r0,#0xff
    {0x80006f0c,0xe3520004}, //    ..R.    CMP      r2,#4
    {0x80006f10,0x3afffff3}, //    ...:    BCC      0x80006ee4
    {0x80006f14,0xe1a0f00e}, //    ....    MOV      pc,r14
    {0x80006f18,0xe111021c}, //    ....    TST      r1,r12,LSL r2
    {0x80006f1c,0xe2823004}, //    .0..    ADD      r3,r2,#4
    {0x80006f20,0x12800001}, //    ....    ADDNE    r0,r0,#1
    {0x80006f24,0x120000ff}, //    ....    ANDNE    r0,r0,#0xff
    {0x80006f28,0xe111031c}, //    ....    TST      r1,r12,LSL r3
    {0x80006f2c,0x02800001}, //    ....    ADDEQ    r0,r0,#1
    {0x80006f30,0xe2822001}, //    . ..    ADD      r2,r2,#1
    {0x80006f34,0xe20220ff}, //    . ..    AND      r2,r2,#0xff
    {0x80006f38,0x020000ff}, //    ....    ANDEQ    r0,r0,#0xff
    {0x80006f3c,0xe3520004}, //    ..R.    CMP      r2,#4
    {0x80006f40,0x3afffff4}, //    ...:    BCC      0x80006f18
    {0x80006f44,0xe1a0f00e}, //    ....    MOV      pc,r14
    {0x80006f48,0xe111021c}, //    ....    TST      r1,r12,LSL r2
    {0x80006f4c,0x02800001}, //    ....    ADDEQ    r0,r0,#1
    {0x80006f50,0xe2822001}, //    . ..    ADD      r2,r2,#1
    {0x80006f54,0xe20220ff}, //    . ..    AND      r2,r2,#0xff
    {0x80006f58,0x020000ff}, //    ....    ANDEQ    r0,r0,#0xff
    {0x80006f5c,0xe3520008}, //    ..R.    CMP      r2,#8
    {0x80006f60,0x3afffff8}, //    ...:    BCC      0x80006f48
    {0x80006f64,0xe1a0f00e}, //    ....    MOV      pc,r14
    {0x80006f68,0x80006fec}, //    .o..    DCD    2147512300   //bt_rx_test_enable
    {0x80006f6c,0xe59f006c}, //    l...    LDR      r0,0x80006fe0
    {0x80006f70,0xe5d00000}, //    ....    LDRB     r0,[r0,#0]
    {0x80006f74,0xe35000ff}, //    ..P.    CMP      r0,#0xff
    {0x80006f78,0x01a0f00e}, //    ....    MOVEQ    pc,r14
    {0x80006f7c,0xe92d4008}, //    .@-.    STMFD    r13!,{r3,r14}
    {0x80006f80,0xe3a00014}, //    ....    MOV      r0,#0x14
    {0x80006f84,0xebfffdf7}, //    ....    BL       $Ven$AA$L$$HWdelay_Wait_For_us  ; 0x80006768
    {0x80006f88,0xe3a00001}, //    ....    MOV      r0,#1
    {0x80006f8c,0xebfffdf3}, //    ....    BL       $Ven$AA$L$$HW_set_tx_mode  ; 0x80006760
    {0x80006f90,0xe59f004c}, //    L...    LDR      r0,0x80006fe4
    {0x80006f94,0xe5d01001}, //    ....    LDRB     r1,[r0,#1]
    {0x80006f98,0xe5d02008}, //    . ..    LDRB     r2,[r0,#8]
    {0x80006f9c,0xe26110dd}, //    ..a.    RSB      r1,r1,#0xdd
    {0x80006fa0,0xe2811e40}, //    @...    ADD      r1,r1,#0x400
    {0x80006fa4,0xe2622f9b}, //    ./b.    RSB      r2,r2,#0x26c
    {0x80006fa8,0xe1812802}, //    .(..    ORR      r2,r1,r2,LSL #16
    {0x80006fac,0xe3a01470}, //    p...    MOV      r1,#0x70000000
    {0x80006fb0,0xe5812140}, //    @!..    STR      r2,[r1,#0x140]
    {0x80006fb4,0xe5d02002}, //    . ..    LDRB     r2,[r0,#2]
    {0x80006fb8,0xe5d00009}, //    ....    LDRB     r0,[r0,#9]
    {0x80006fbc,0xe2622f90}, //    ./b.    RSB      r2,r2,#0x240
    {0x80006fc0,0xe26000be}, //    ..`.    RSB      r0,r0,#0xbe
    {0x80006fc4,0xe2800e40}, //    @...    ADD      r0,r0,#0x400
    {0x80006fc8,0xe3800c80}, //    ....    ORR      r0,r0,#0x8000
    {0x80006fcc,0xe1820800}, //    ....    ORR      r0,r2,r0,LSL #16
    {0x80006fd0,0xe5810158}, //    X...    STR      r0,[r1,#0x158]
    {0x80006fd4,0xe3a00000}, //    ....    MOV      r0,#0
    {0x80006fd8,0xe5810170}, //    p...    STR      r0,[r1,#0x170]
    {0x80006fdc,0xe8bd8008}, //    ....    LDMFD    r13!,{r3,pc}
    {0x80006fe0,0x80003aa8}, //    ....    DCD    2147526128  //RDA_TEST_MODE_AM_ADDR
    {0x80006fe4,0x80002cf8}, //    @...    DCD    2147522624   //Driver_Data
    {0x80006fe8,0x00000000}, //    bbtest_stop
    {0x80006fec,0x00000000}, //   bt_rx_test_enable
    {0x80006ff0,0x00000000}, //             total_test_packet
    {0x80006ff4,0x00000000}, //       total_received_packet
    {0x80006ff8,0x00000000}, //     total_received_bits
    {0x80006ffc,0x00000000}, //    total_error_packet
    {0x80007000,0x00000000}, //   total_error_bit
    {0x80007004,0x00000000}, //   rx_test_patten
    {0x80007008,0xfff0aa00}, //          rx_patten_data
    {0x8000700c,0x01140e04}, //          rx_result_buff
    {0x80007010,0x0000fd77}, //
    {0x80007014,0x00000000}, //
    {0x80007018,0x00000000}, //
    {0x8000701c,0x00000000}, //
    {0x80007020,0x00000000}, //
    {0x40180000,0x0000001f}, //
};

const uint32 rdabt_pll_test_pskey_17[][2] =
{
// item3:8809bt_VerB_PLL_DIGI_SET
    {0x40240000, 0x0004f39c}, // SPI2_CLK_EN PCLK_SPI2_EN
    {0x800000C0, 0x0000000b}, // CHIP_PS PSKEY: Total number -----------------
    {0x800000C4, 0x003f0000}, // PSKEY: Page 0
    {0x800000C8, 0x0041000b}, // TX_DC_OFFSET+/_
    {0x800000CC, 0x0043bfc6}, // 43H=BFC6
    {0x800000D0, 0x00440500}, // 44H=0580;
    {0x800000D4, 0x00694075}, // TX_DC_OFFSET+/_
    {0x800000D8, 0x006b10c0}, //
    {0x800000DC, 0x003f0001}, // PSKEY: Page 1
    {0x800000E0, 0x00453000}, //
    {0x800000E4, 0x0047f13b}, // 20120512
    {0x800000E8, 0x00490008}, //
    {0x800000EC, 0x003f0000}, // PSKEY: Page 0
    {0x80000040, 0x10000000}, // PSKEY: Flage
    {0x40240000, 0x0000f29c}, // Disable SPI2_CLK_EN PCLK_SPI2_EN
};

const uint16 rdabt_pll_test_set_17[][2] =
{
// item3:8809bt_VerB_PLL_SET
    {0x3F, 0x0000}, // page 0
    {0x0E, 0x0930}, // Tx_sys_cal_bit_dr=1
    {0x1F, 0x03ff}, // Tx_sys_cal_bit_q_reg<9:0>
    {0x20, 0x03ff}, // Tx_sys_cal_bit_q_reg<9:0>
    {0x02, 0xf000}, // Set freq no
};

const uint32 rdabt_pll_test_txon_17[][2] =
{
// item3:TX_ON
    {0x40200020, 0x00000043}, //
};
#endif

#ifdef __RDA_CHIP_R18_8809E__
const uint32 rdabt_test_18[][2] =
{
    {0x80000520,0x00040dba},
};

const uint32 rdabt_tx_test_trap_r18[][2]=
{
    {0x80000200,0xe51ff004},
    {0x80000204,0x000319bc},
    {0x80000208,0xe51ff004},
    {0x8000020c,0x00020A24},
    {0x80000210,0xe51ff004},
    {0x80000214,0x0001eb60},
    {0x80000218,0xe51ff004},
    {0x8000021c,0x0001ed90},
    {0x80000220,0xe92d4010},
    {0x80000224,0xe1a04000},
    {0x80000228,0xe59f0044},
    {0x8000022c,0xe3a01000},
    {0x80000230,0xe1c010b0},
    {0x80000234,0xe5d4000a},
    {0x80000238,0xe59f2038},
    {0x8000023c,0xe3500001},
    {0x80000240,0x15821000},
    {0x80000244,0x1a000002},
    {0x80000248,0xe3a00001},
    {0x8000024c,0xe5820000},
    {0x80000250,0xeb00000a},
    {0x80000254,0xe1a01004},
    {0x80000258,0xe59f401c},
    {0x8000025c,0xe3a02014},
    {0x80000260,0xe1a00004},
    {0x80000264,0xebffffe5},
    {0x80000268,0xe1a00004},
    {0x8000026c,0xe8bd4010},
    {0x80000270,0xeaffffe4},
    {0x80000274,0x800013dc},
    {0x80000278,0x800002cc},
    {0x8000027c,0x80001434},
    {0x80000280,0xe59f1004},
    {0x80000284,0xe59f0004},
    {0x80000288,0xeaffffe0},
    {0x8000028c,0x22115876},
    {0x80000290,0x800002d0},
    {0x80000294,0xe92d4008},
    {0x80000298,0xe3a00470},
    {0x8000029c,0xe590001c},
    {0x800002a0,0xe2800003},
    {0x800002a4,0xe3c01003},
    {0x800002a8,0xe59f0014},
    {0x800002ac,0xebffffd9},
    {0x800002b0,0xe59f1010},
    {0x800002b4,0xe5c1000b},
    {0x800002b8,0xe8bd4008},
    {0x800002bc,0xe1a00001},
    {0x800002c0,0xeaffffd0},
    {0x800002c4,0x800002d0},
    {0x800002c8,0x80001434},
    {0x800002cc,0x00000000},
    {0x800002d0,0x00000000},
    {0x800002d4,0x00000000},
    {0x800002d8,0x00000000},
    {0x800002dc,0x00000000},

    {0x800002e0,0xeb000001},
    {0x800002e4,0xe3a01b88},
    {0x800002e8,0xe281ff8f},
    {0x800002ec,0xe51ff004},
    {0x800002f0,0x80000220},
    {0x80000018,0xea0000b0},
    {0x4018010c,0x00022238},
    {0x4018012c,0x00031e2c},


    {0x80000320,0xe59f0018},
    {0x80000324,0xe5900000},
    {0x80000328,0xe3500001},
    {0x8000032c,0x0b000004},
    {0x80000330,0xe59f0004},
    {0x80000334,0xe3a01bc6},
    {0x80000338,0xe281fffb},
    {0x8000033c,0x800027d4},
    {0x80000340,0x800002cc},
    {0x80000344,0xe51ff004},
    {0x80000348,0x80000294},
    {0x8000001c,0xea0000bf},
    {0x40180110,0x00031be8},
    {0x40180130,0x00031e30},
    {0x40180000,0x0000ffff},
};
const uint16 rdabt_rf_init_for_test_18[][2] =
{
    {0x3F, 0x0000}, // page 0
    {0x01, 0x1fff}, // Padrv_gain_tb_en=1
    //{0x06, 0x161c}, // Padrv_op_gain=11;Padrv_ibit_psk<3:0>=000;Padrv_input_range_psk<1:0>=11
    {0x06,0x6604},
    {0x07, 0x040d}, //
    {0x08, 0x8326}, // padrv_lo_tune_psk[5:0]=10000;Lna_notch_en=1
    {0x09, 0x04b5}, // rmx_imgrej=1
    {0x0B, 0x230f}, // Filter_cal_mode =1
    {0x0C, 0x85e8}, // filter_bpmode<2:0>=101
    {0x0E, 0x0920}, // Tx_cal_polarity=1
    {0x0F, 0x8db3}, // adc_iq_swap=1
    {0x10, 0x1400}, // tx_sys_cal_sel<1:0>=10
    //{0x12, 0x560c}, // 12H,16'h1604;//Filter_cal_mode=0;padrv_ibit<3:0>=000;padrv_input_range<1:0>=00
    {0x12,0x564c},
    //{0x12, 0x1604}, // Lower the BT Tx power
    {0x14, 0x4ecc}, //
    {0x18, 0x0812}, // pll_refmulti2_en=1;
    {0x19, 0x10c8}, // pll_adcclk_en=1
    {0x1E, 0x3024}, // Pll_lpf_gain_tx<1:0>=00;Pll_pfd_res_tx<5:0>=100100
    // TX GAIN
    {0x23, 0x7777}, // PSK
    //{0x24, 0x2368}, //
    //{0x24, 0x2369}, //
    {0x24, 0x236a}, //
    {0x27, 0x5555}, // GFSK
    {0x28, 0x1123}, //
    {0x32, 0x0200}, // tx_dsp_reset_delay=2
    // AGC_by_xudonglin
    {0x3F, 0x0001}, // page 1
    {0x00, 0x020f}, // 80H,16'hd19f;
    {0x01, 0xf9cf}, //
    {0x02, 0xfc2f}, //
    {0x03, 0xf92f}, //
    {0x04, 0xfa2f}, //
    {0x05, 0xfc2f}, //
    {0x06, 0xfb3f}, //
    {0x07, 0x7fff}, //
    {0x0A, 0x0018}, // thermo_pll_vcoibit_8<3:0>=1000
    // apc;padrv_gain
    {0x18, 0xfef3}, //
    {0x19, 0xfef3}, //
    {0x1A, 0xfef3}, //
    {0x1B, 0xfef3}, //
    {0x1C, 0xfef3}, //
    {0x1D, 0xfff3}, //
    {0x1E, 0xfff3}, //
    {0x1F, 0xfff3}, // padrv_gain;9FH,16'hFFEC;padrv_gain20101103;improve ACPR;
    {0x22, 0x0e93}, // default
    {0x25, 0x03e1}, // default
    {0x26, 0x47a5}, // default;set voltage=1.2v
    {0x27,0x0108},  //osc_stable_timer
    {0x28, 0x6800}, // default
    {0x2D, 0x006a}, // default OFF_FM;
    {0x2F, 0x1100}, // default
    {0x32, 0x88f9}, // TM=001;DN=1111
    {0x3F, 0x0000}, // page 0
};
#endif

void Rdabt_test_pskey_write_r10(void)
{
    uint16 i;
    for(i=0; i<sizeof(rdabt_test_pskey_10)/sizeof(rdabt_test_pskey_10[0]); i++)
    {
        rdabt_wirte_memory(rdabt_test_pskey_10[i][0],&rdabt_test_pskey_10[i][1],1,0);
        RDABT_DELAY(1);
    }
}


void rdabt_vco_test_write_r10(void)
{
    uint16 i;
    rdabt_wirte_memory(rdabt_enable_spi2[0][0],&rdabt_enable_spi2[0][1],1,0);
    for(i=1; i<(sizeof(rdabt_vco_test_10)/sizeof(rdabt_vco_test_10[0])-1); i++)
    {
        rdabt_wirte_memory(rdabt_vco_test_10[i][0],&rdabt_vco_test_10[i][1],1,1);
        RDABT_DELAY(1);
    }
    rdabt_wirte_memory(rdabt_vco_test_10[i][0],&rdabt_vco_test_10[i][1],1,0);

}



void Rdabt_dut_pskey_write_r578(void)
{
    uint16 i;
    for(i=0; i<sizeof(rdabt_test_pskey_578)/sizeof(rdabt_test_pskey_578[0]); i++)
    {
        rdabt_wirte_memory(rdabt_test_pskey_578[i][0],&rdabt_test_pskey_578[i][1],1,0);
        RDABT_DELAY(1);
    }
}

void Rdabt_dut_trap_write_r5(void)
{
    uint16 i;
    for(i=0; i<sizeof(rdabt_dut_trap_5)/sizeof(rdabt_dut_trap_5[0]); i++)
    {
        rdabt_wirte_memory(rdabt_dut_trap_5[i][0],&rdabt_dut_trap_5[i][1],1,0);
        RDABT_DELAY(1);
    }
}


void Rdabt_dut_trap_write_r7(void)
{
    uint16 i;
    for(i=0; i<sizeof(rdabt_dut_trap_7)/sizeof(rdabt_dut_trap_7[0]); i++)
    {
        rdabt_wirte_memory(rdabt_dut_trap_7[i][0],&rdabt_dut_trap_7[i][1],1,0);
        RDABT_DELAY(1);
    }
}

void Rdabt_dut_trap_write_r8(void)
{
    uint16 i;
    for(i=0; i<sizeof(rdabt_dut_trap_8)/sizeof(rdabt_dut_trap_8[0]); i++)
    {
        rdabt_wirte_memory(rdabt_dut_trap_8[i][0],&rdabt_dut_trap_8[i][1],1,0);
        RDABT_DELAY(1);
    }
}

void Rdabt_dut_trap_write_r10(void)
{
    uint16 i;
    for(i=0; i<sizeof(rdabt_dut_trap_10)/sizeof(rdabt_dut_trap_10[0]); i++)
    {
        rdabt_wirte_memory(rdabt_dut_trap_10[i][0],&rdabt_dut_trap_10[i][1],1,0);
        RDABT_DELAY(1);
    }
}

void Rdabt_test_pskey_write_r11(void)
{
    uint16 i;
    for(i=0; i<sizeof(rdabt_test_pskey_11)/sizeof(rdabt_test_pskey_11[0]); i++)
    {
        rdabt_wirte_memory(rdabt_test_pskey_11[i][0],&rdabt_test_pskey_11[i][1],1,0x10);
        RDABT_DELAY(1);
    }
}


void Rdabt_dut_trap_write_r11(void)
{
    uint16 i;
    for(i=0; i<sizeof(rdabt_dut_trap_11)/sizeof(rdabt_dut_trap_11[0]); i++)
    {
        rdabt_wirte_memory(rdabt_dut_trap_11[i][0],&rdabt_dut_trap_11[i][1],1,0x10);
        RDABT_DELAY(1);
    }
}


void Rdabt_dut_trap_write_r12(void)
{
    uint16 i;
    for(i=0; i<sizeof(rdabt_dut_trap_12)/sizeof(rdabt_dut_trap_12[0]); i++)
    {
        rdabt_wirte_memory(rdabt_dut_trap_12[i][0],&rdabt_dut_trap_12[i][1],1,0x0);
        RDABT_DELAY(1);
    }
}

void Rdabt_vco_test_Write_r578()
{
    uint16 i;
    rdabt_wirte_memory(rdabt_enable_spi2[0][0],&rdabt_enable_spi2[0][1],1,0);
    for(i=0; i<3; i++)
    {
        rdabt_wirte_memory(rdabt_vco_test_578[i][0],&rdabt_vco_test_578[i][1],1,1);
        RDABT_DELAY(1);
    }
    for(i=3; i<sizeof(rdabt_vco_test_578)/sizeof(rdabt_vco_test_578[0]); i++)
    {
        rdabt_wirte_memory(rdabt_vco_test_578[i][0],&rdabt_vco_test_578[i][1],1,0);
        RDABT_DELAY(1);
    }
}

void Rdabt_Pskey_Write_VCO_r16(void)
{
    int32 i;
    for(i=0; i<sizeof(rdabt_pskey_VCO_16)/sizeof(rdabt_pskey_VCO_16[0]); i++)
    {
        rdabt_wirte_memory(rdabt_pskey_VCO_16[i][0],&rdabt_pskey_VCO_16[i][1],1,0);
        RDABT_DELAY(1);
    }
    RDABT_DELAY(10);
}

void Rdabt_vco_test_Write_r16()
{
    uint16 i;
    rdabt_wirte_memory(rdabt_enable_spi2[0][0],&rdabt_enable_spi2[0][1],1,0);
    rdabt_wirte_memory(rdabt_vco_test_r16[0][0],&rdabt_vco_test_r16[0][1],1,0);

    RDABT_DELAY(2);
    for(i=1; i<sizeof(rdabt_vco_test_r16)/sizeof(rdabt_vco_test_r16[0]); i++)
    {
        rdabt_wirte_memory(rdabt_vco_test_r16[i][0],&rdabt_vco_test_r16[i][1],1,1);
        RDABT_DELAY(1);
    }
}

#ifdef __RDA_CHIP_R17_8809__
void Rdabt_pll_test_write_r17()
{
    uint16 i;
    for(i=0; i<sizeof(rdabt_pll_test_set_17)/sizeof(rdabt_pll_test_set_17[0]); i++)
    {
        rdabt_iic_rf_write_data(rdabt_pll_test_set_17[i][0],&rdabt_pll_test_set_17[i][1],1);
    }

    for(i=0; i<sizeof(rdabt_pll_test_pskey_17)/sizeof(rdabt_pll_test_pskey_17[0]); i++)
    {
        rdabt_wirte_memory(rdabt_pll_test_pskey_17[i][0],&rdabt_pll_test_pskey_17[i][1],1,0x0);
        RDABT_DELAY(1);
    }

    for(i=0; i<sizeof(rdabt_pll_test_txon_17)/sizeof(rdabt_pll_test_txon_17[0]); i++)
    {
        rdabt_wirte_memory(rdabt_pll_test_txon_17[i][0],&rdabt_pll_test_txon_17[i][1],1,0x0);
        RDABT_DELAY(1);
    }
}

void Rdabt_test_write_r17()
{
    uint16 i;
    for(i=0; i<sizeof(rdabt_test_17)/sizeof(rdabt_test_17[0]); i++)
    {
        rdabt_wirte_memory(rdabt_test_17[i][0],&rdabt_test_17[i][1],1,0x0);
        RDABT_DELAY(1);
    }
}
#endif

#ifdef __RDA_CHIP_R18_8809E__
void  RDABT_rf_Intialization_for_test_r18(void)
{
    uint16 i=0;
    for(i=0; i<sizeof(rdabt_rf_init_for_test_18)/sizeof(rdabt_rf_init_for_test_18[0]); i++)
    {
        rdabt_iic_rf_write_data(rdabt_rf_init_for_test_18[i][0],&rdabt_rf_init_for_test_18[i][1],1);
    }
    RDABT_DELAY(10);
}
void Rdabt_test_write_r18()
{
    uint16 i;
    for(i=0; i<sizeof(rdabt_test_18)/sizeof(rdabt_test_18[0]); i++)
    {
        rdabt_wirte_memory(rdabt_test_18[i][0],&rdabt_test_18[i][1],1,0x0);
        RDABT_DELAY(1);
    }
}

void Rdabt_test_tx_trap_r18()
{

    uint16 i;
    for(i=0; i<sizeof(rdabt_tx_test_trap_r18)/sizeof(rdabt_tx_test_trap_r18[0]); i++)
    {
        rdabt_wirte_memory(rdabt_tx_test_trap_r18[i][0],&rdabt_tx_test_trap_r18[i][1],1,0x0);
        RDABT_DELAY(1);
    }
}
#endif
void Rdabt_rf_singlehop_Write()
{
    uint16 num_send;
    rdabt_uart_tx(rdabt_rf_singlehop,sizeof(rdabt_rf_singlehop),&num_send);
    RDABT_DELAY(1);
    rdabt_uart_tx(rdabt_rf_inquiry,sizeof(rdabt_rf_inquiry),&num_send);
    RDABT_DELAY(1);
}

void RDAbt_enable_dut_write(void)
{
    uint16 num_send;
    rdabt_uart_tx(rdabt_enable_allscan,sizeof(rdabt_enable_allscan),&num_send);
    RDABT_DELAY(5);
    rdabt_uart_tx(rdabt_autoaccept_connect,sizeof(rdabt_autoaccept_connect),&num_send);
    RDABT_DELAY(5);
    rdabt_uart_tx(rdabt_enable_dut,sizeof(rdabt_enable_dut),&num_send);
    RDABT_DELAY(5);
}







void rda_bt_poweroff_for_test(void)
{

    pmd_EnablePower(PMD_POWER_BT, FALSE);
    hal_GpioClr(g_btdConfig->pinReset);

    I2C_Close();
    rdabt_uart_stop();
//  pmd_SetCoreVoltage(PMD_CORE_VOLTAGE_USER_BT, PMD_CORE_VOLTAGE_LOW);
#ifdef __RDA_SHARE_CRYSTAL__
//      L1SM_SleepEnable(bt_sleephdl);
#endif
    TurnOffBacklight();

}

uint8 bt_test_mode_flag=0;
uint8 rda_bt_poweron_for_test(void)
{
    TurnOnBacklight(0);
    // Enable 26M AUX clock
    // pmd_SetCoreVoltage(PMD_CORE_VOLTAGE_USER_BT, PMD_CORE_VOLTAGE_BTTEST);


    pal_EnableAuxClkCtrl(TRUE);
    // Enable 26M AUX clock
    hal_SysAuxClkOut(TRUE);
    hal_GpioClr(g_btdConfig->pinReset);
    pmd_EnablePower(PMD_POWER_BT, TRUE);
    sxr_Sleep(168*50);/*10 ms*/
    hal_GpioSet(g_btdConfig->pinReset);
    I2C_Open();


    bt_test_mode_flag=1;
    kal_prompt_trace(1,"rda_bt_poweron_for_test1");
    RDABT_DELAY(10);

#ifdef __RDA_SHARE_CRYSTAL__
//  if(bt_sleephdl == 0xff)
//      bt_sleephdl = L1SM_GetHandle();
//       L1SM_SleepDisable(bt_sleephdl);
#endif
    rdabt_chip_sel = 0;
    rda_bt_power_on();
    rda_bt_uart_init();
    rda_bt_rf_init();
    RDA_bt_Power_On_Reset();
    RDABT_DELAY(70);



    kal_prompt_trace(1, "rdabt_chip_sel = %d",rdabt_chip_sel);

    RDABT_DELAY(70);
    switch(rdabt_chip_sel)
    {
#ifdef __RDA_CHIP_R5_5868__
        case RDA_BT_R5_5868_ENUM:
            Rdabt_Pskey_Write_rf_r5();
            RDABT_DELAY(10);
            Rdabt_dut_pskey_write_r578();
            break;
#endif
#ifdef __RDA_CHIP_R7_5868plus__
        case RDA_BT_R7_5868PLUS_ENUM:
            Rdabt_Pskey_Write_rf_r7();
            RDABT_DELAY(10);
            Rdabt_dut_pskey_write_r578();
            break;
#endif
#ifdef __RDA_CHIP_R8_5870__
        case RDA_BT_R8_5870_ENUM:
            Rdabt_Pskey_Write_rf_r8();
            RDABT_DELAY(10);
            Rdabt_dut_pskey_write_r578();
            break;
#endif
#if defined( __RDA_CHIP_R10_5868E__ ) || defined(__RDA_CHIP_R10_5872__)
        case RDA_BT_R10_5872_ENUM:
        case RDA_BT_R10_5868E_ENUM:
            Rdabt_core_uart_Intialization_r10();
            RDABT_DELAY(10); //wait for first uart data
            Rdabt_Pskey_Write_rf_r10();
            Rdabt_test_pskey_write_r10();
            break;
#endif
#if defined(__RDA_CHIP_R10_5868H__) || defined(__RDA_CHIP_R10_5872H__)
        case RDA_BT_R10_AL_5868H_ENUM:
        case RDA_BT_R10_AL_5872H_ENUM:
            RDABT_rf_Intialization_r10_al();
            Rdabt_Pskey_Write_rf_r10_al();
            RDABT_DELAY(5); //wait for digi running
            rdabt_DC_write_r10_al();
            Rdabt_test_pskey_write_r10();
            break;
#endif
#ifdef __RDA_CHIP_R10_5870E__
        case RDA_BT_R10_5870E_ENUM:
            RDABT_rf_Intialization_r10_e();
            Rdabt_Pskey_Write_rf_r10_e();
            RDABT_DELAY(5); //wait for digi running
            rdabt_DC_write_r10_e();
            Rdabt_test_pskey_write_r10();
            break;
#endif
#ifdef __RDA_CHIP_R11_5872P__
        case RDA_BT_R11_5872P_ENUM:
            Rdabt_Pskey_Write_rf_r11_p();
            RDABT_rf_Intialization_r11_p();
            rda_bt_pin_to_low();
            rda_bt_ldoon_toggle_high();
            rda_bt_reset_to_high();
            RDABT_DELAY(10); //wait for digi running
            Rdabt_Pskey_Write_rf_r11_p();
            RDABT_rf_Intialization_r11_p();
            rdabt_DC_write_r11_p();
            Rdabt_test_pskey_write_r11();

            break;
#endif
#ifdef __RDA_CHIP_R11_5875__
        case RDA_BT_R11_5875_ENUM:
            // rda_bt_ldoon_toggle_high();
            //  rda_bt_reset_to_high();
            //  RDABT_DELAY(5); //wait for digi running
            RDABT_rf_Intialization_r11();
            Rdabt_Pskey_Write_rf_r11();
            rdabt_DC_write_r11();
            Rdabt_test_pskey_write_r11();
            break;
#endif
#ifdef __RDA_CHIP_R12_5876__
        case RDA_BT_R12_5876_ENUM:
            //  rda_bt_ldoon_toggle_high();
            //   rda_bt_reset_to_high();
            //   RDABT_DELAY(5); //wait for digi running
            RDABT_rf_Intialization_r12();
            Rdabt_Pskey_Write_rf_r12();
            rdabt_DC_write_r12();
            Rdabt_test_pskey_write_r11();
            break;
#endif
#ifdef __RDA_CHIP_R12_5870P__
        case RDA_BT_R12_5870P_ENUM:
            //  rda_bt_ldoon_toggle_high();
            //   rda_bt_reset_to_high();
            //   RDABT_DELAY(5); //wait for digi running
            RDABT_rf_Intialization_r12();
            Rdabt_Pskey_Write_rf_r12();
            rdabt_DC_write_r12();
            Rdabt_test_pskey_write_r11();
            break;
#endif
#ifdef __RDA_CHIP_R12_5876M__
        case RDA_BT_R12_5876M_ENUM:
            //  rda_bt_ldoon_toggle_high();
            //   rda_bt_reset_to_high();
            //   RDABT_DELAY(5); //wait for digi running
            RDABT_rf_Intialization_r12_m();
            Rdabt_Pskey_Write_rf_r12_m();
            rdabt_DC_write_r12_m();
            Rdabt_test_pskey_write_r11();
            break;
#endif
#ifdef __RDA_CHIP_R16_5876P__
        case RDA_BT_R16_5876P_ENUM:
            //  rda_bt_ldoon_toggle_high();
            //   rda_bt_reset_to_high();
            //   RDABT_DELAY(5); //wait for digi running
            RDABT_rf_Intialization_r16();
            Rdabt_Pskey_Write_rf_r16();
            rdabt_DC_write_r16();
            Rdabt_test_pskey_write_r11();
            break;
#endif
#ifdef __RDA_CHIP_R12_5990__
        case RDA_BT_R12_5990_ENUM:
            //  rda_bt_ldoon_toggle_high();
            //  rda_bt_reset_to_high();
            //  RDABT_DELAY(5); //wait for digi running
            RDABT_rf_Intialization_r12();
            Rdabt_Pskey_Write_rf_r12();
            rdabt_DC_write_r12();
            Rdabt_test_pskey_write_r11();
            break;
#endif
#ifdef __RDA_CHIP_R17_8809__
        case RDA_BT_R17_8809_ENUM:
            RDABT_rf_Intialization_r17();
            Rdabt_Pskey_Write_rf_r17();
            rdabt_DC_write_r17();
            Rdabt_test_write_r17();

            kal_prompt_trace(1,"rda_bt_poweron_for_test  RDA_BT_R17_8809_ENUM");
            RDABT_DELAY(10);


            break;
#endif
#ifdef __RDA_CHIP_R18_8809E__
        case RDA_BT_R18_8809E_ENUM:
            RDABT_rf_Intialization_for_test_r18();
            Rdabt_Pskey_Write_rf_r18();
            rdabt_DC_write_r18();
            Rdabt_test_write_r18();

            kal_prompt_trace(1,"rda_bt_poweron_for_test  RDA_BT_R18_8809E_ENUM");
            if(TestMode_NorSignal_Tx==TestMode)
                Rdabt_test_tx_trap_r18();
            break;
#endif

        default:
//      ASSERT(0);  // xiaoyifeng
            break;
    }

    // Disable 26M AUX clock
    //hal_SysAuxClkOut(FALSE);
    return 1;
}

void RDAbt_send_bbtest_singlehop_inquiry(void)
{
    Rdabt_rf_singlehop_Write();
}


void RDAbt_send_vco_test(void)
{
#ifdef __RDA_CHIP_R17_8809__
    // 8809 has no VCO test. Using PLL test instead.
    Rdabt_pll_test_write_r17();
    return;
#endif

    if(rdabt_chip_sel == RDA_BT_R5_5868_ENUM|| rdabt_chip_sel == RDA_BT_R7_5868PLUS_ENUM|| rdabt_chip_sel == RDA_BT_R8_5870_ENUM)
        Rdabt_vco_test_Write_r578();
    else if(rdabt_chip_sel == RDA_BT_R10_5872_ENUM || rdabt_chip_sel == RDA_BT_R10_5868E_ENUM
            || rdabt_chip_sel ==RDA_BT_R10_AL_5868H_ENUM || rdabt_chip_sel ==RDA_BT_R10_AL_5872H_ENUM
            || rdabt_chip_sel == RDA_BT_R10_5870E_ENUM || rdabt_chip_sel == RDA_BT_R11_5872P_ENUM
            || rdabt_chip_sel == RDA_BT_R11_5875_ENUM  || rdabt_chip_sel == RDA_BT_R12_5876_ENUM
            || rdabt_chip_sel == RDA_BT_R12_5990_ENUM)
        rdabt_vco_test_write_r10();
    else if(rdabt_chip_sel == RDA_BT_R16_5876P_ENUM)
    {
        RDABT_DELAY(10);
        Rdabt_Pskey_Write_VCO_r16();
        Rdabt_vco_test_Write_r16();
    }
}


void rda_bt_test_enable_dut(void)
{
    switch(rdabt_chip_sel)
    {
        case RDA_BT_R5_5868_ENUM:
            Rdabt_dut_trap_write_r5();
            break;
        case RDA_BT_R7_5868PLUS_ENUM:
            Rdabt_dut_trap_write_r7();
            break;
        case RDA_BT_R8_5870_ENUM:
            Rdabt_dut_trap_write_r8();
            break;
        case RDA_BT_R10_5872_ENUM:
        case RDA_BT_R10_5868E_ENUM:
        case RDA_BT_R10_AL_5868H_ENUM:
        case RDA_BT_R10_AL_5872H_ENUM:
        case RDA_BT_R10_5870E_ENUM:
            Rdabt_dut_trap_write_r10();
            break;
        case RDA_BT_R11_5872P_ENUM:
        case RDA_BT_R11_5875_ENUM:
            Rdabt_dut_trap_write_r11();
            break;
        case RDA_BT_R12_5876_ENUM:
        case RDA_BT_R12_5870P_ENUM:
        case RDA_BT_R12_5876M_ENUM:
            Rdabt_dut_trap_write_r12();
            break;
#ifdef __RDA_CHIP_R16_5876P__
        case RDA_BT_R16_5876P_ENUM:
            Rdabt_trap_write_r16();
            break;
#endif
        case RDA_BT_R12_5990_ENUM:
            break;
        case RDA_BT_R17_8809_ENUM:
            Rdabt_trap_write_r17();
            kal_prompt_trace(1,"Rdabt_trap_write_r17 call ");
            RDABT_DELAY(10);


            break;
        default:
//      ASSERT(0);  // xiaoyifeng
            break;
    }

    RDAbt_enable_dut_write();

#ifdef BT_USE_32K_CLK_PIN
    extern BOOL g_RdaBt32kClockRequested;
    if (g_RdaBt32kClockRequested)
    {
        hal_Sys32kClkOut(FALSE);
        g_RdaBt32kClockRequested = FALSE;
    }
#else
    extern HAL_SYS_CLOCK_OUT_ID_T g_RdaBtClockOutId;
    if ((g_RdaBtClockOutId < HAL_SYS_CLOCK_OUT_ID_QTY) &&
            (g_RdaBtClockOutId >= HAL_SYS_CLOCK_OUT_ID_1))
    {
        hal_SysClkOutClose(g_RdaBtClockOutId);
        g_RdaBtClockOutId = HAL_SYS_CLOCK_OUT_ID_QTY;
    }
#endif


}

void RDA5868_send_vco_test(void)
{
    RDAbt_send_vco_test();
}

void RDA5868_send_bbtest_singlehop_inquiry(void)
{
    RDAbt_send_bbtest_singlehop_inquiry();
}
void RDA_bt_send_cmu200_enable_dut(void)
{
    rda_bt_test_enable_dut();
}

void RDA_bt_send_tc3000_enable_dut(void)
{
    rda_bt_test_enable_dut();
}

void RDA_bt_send_n4010_enable_dut(void)
{
    rda_bt_test_enable_dut();
}


const uint8 rda5868_read_lmpversion[10] = {0x01 ,0x01 ,0x10 ,0x00};

uint8 rdabt_get_lmpversion(void)
{
    uint16 rx_length=0;
    uint8 rx_buff[128];
    uint8 status;
    uint16 num_send;
    uint16 i;
    EDRV_TRACE(EDRV_BTD_TRC, 0, "rdabt_get_lmpversion  is called!");
    EDRV_TRACE(EDRV_BTD_TRC, 0, "rdabt_get_lmpversion  is called!");
    //UART_ClrRxBuffer(BT_UART_PORT,MOD_MMI);
    i= rdabt_uart_tx((uint8 *)rda5868_read_lmpversion,sizeof(rda5868_read_lmpversion),&num_send);
    EDRV_TRACE(EDRV_BTD_TRC, 0, "rdabt_adp_uart_tx i=%d",i);
    sxr_Sleep(156*5);//90ms
    rx_length = UART_GetBytes(BT_UART_PORT, rx_buff, 128, &status, MOD_MMI);
    //EDRV_TRACE(EDRV_BTD_TRC, 0,"rda5868_chipid = %x",rda5868_chipid);
    EDRV_TRACE(EDRV_BTD_TRC, 0, "rdabt_get_lmpversion rx_length=%x",rx_length);
    for(i=0; i<rx_length; i++)
        EDRV_TRACE(EDRV_BTD_TRC, 0, "rx_buff[%d]=0x%x",i,rx_buff[i]);
    return rx_buff[66];

}

#define _RDA_BT_FCC_TEST_ 1

#ifdef _RDA_BT_FCC_TEST_

const uint8 rdabt_DHn_test_r17[] =
{
    0x01,0x10,0xFD,0x18,0x00,0x00,0x00,0x02,0x04,0x00,0x0F,0x00,0x33,0x1A,0x3A,0xE2,0x4E,0x7A,0x2C,0xCE,0xFF,0x01,0x55,0x55,0x00,0x00,0x1B,0x00,
};


const uint8 rdabt_2DHn_test_r17[] =
{
    0x01,0x10,0xFD,0x18,0x00,0x00,0x00,0x02,0x04,0x01,0x0F,0x00,0x33,0x1A,0x3A,0xE2,0x4E,0x7A,0x2C,0xCE,0xFF,0x01,0x55,0x55,0x00,0x00,0x36,0x00,
};

const uint8 rdabt_3DHn_test_r17[] =
{
    0x01,0x10,0xFD,0x18,0x00,0x00,0x00,0x02,0x08,0x02,0x0F,0x00,0x33,0x1A,0x3A,0xE2,0x4E,0x7A,0x2C,0xCE,0xFF,0x01,0x55,0x55,0x00,0x00,0x53,0x00,
};

const uint8 rdabt_GFSK_test[] = //GFSK test
{
    0x01,0x10,0xFD,0x18,
    0x00,0x00,0x00,0x00,0x03,0x00,
    0x0f,0x00,0x33,0x1a,0x3a,0xe2,
    0x4e,0x7a,0x2c,0xce,0xff,0x01,
    0x55,0x55,0x00,0x00,0xff,0x00,
};
const uint8 rdabt_4DPSK_test[] = //4DPSK test
{
    0x01,0x10,0xFD,0x18,
    0x00,0x00,0x01,0x04,0x04,0x01,
    0x0F,0x00,0x33,0x1A,0x3A,0xE2,
    0x4E,0x7A,0x2C,0xCE,0xFF,0x01,
    0x55,0x55,0x00,0x00,0xFF,0x00,
};
const uint8 rdabt_8DFSK_test[] = //8DFSK test
{
    0x01,0x10,0xFD,0x18,
    0x00,0x00,0x01,0x04,0x08,0x02,
    0x0F,0x00,0x33,0x1A,0x3A,0xE2,
    0x4E,0x7A,0x2C,0xCE,0xFF,0x01,
    0x55,0x55,0x00,0x00,0xFF,0x00,

};
/*{
    0x3f,0x110,0x18,
    0xff,0x00,0x01,0x04,0x08,0x02,
    0x0F,0x00,0x33,0x1A,0x3A,0xE2,
    0x4E,0x7A,0x2C,0xCE,0xFF,0x01,
    0x55,0x55,0x00,0x00,0xFF,0x00,
}
*/

const uint8 rdabt_init_cmd[] =
{
    0x01,0x10,0xFD,0x18,
    0x00,0x00,0x01,0x00,0x03,0x00,
    0x0F,0x00,0x33,0x1A,0x3A,0xE2,
    0x4E,0x7A,0x2C,0xCE,0xFF,0x01,
    0x55,0x55,0x00,0x00,0x12,0x00,
};
void init_cmd(void)
{
    uint16 num_send;
    rdabt_uart_tx(rdabt_init_cmd,sizeof(rdabt_init_cmd),&num_send);
    RDABT_DELAY(5);
}
void RdaBt_Stop_cmd(void)
{
    uint16 num_send;
    uint8 stop_test_cmd[]= {0x01,0x12,0xFD,0x00,}; //stop test

    rdabt_uart_tx(stop_test_cmd,sizeof(stop_test_cmd),&num_send);
    RDABT_DELAY(5);
}

void RdaBt_Stop_Test(void)
{
    uint16 num_send;
    uint8 stop_test_cmd[]= {0x01,0x12,0xFD,0x00,}; //stop test

    rdabt_uart_tx(stop_test_cmd,sizeof(stop_test_cmd),&num_send);
    RDABT_DELAY(5);
    rda_bt_poweroff_for_test();
}

typedef enum
{
    CmdGFSK,
    Cmd4DQPSK,
    Cmd8DPSK,
} RdaBtTestCmd;

void RdaBtTest_send_cmd(unsigned char cmd,unsigned char freq)
{
}

#if _RDA_BT_FCC_TEST_
void RdaBtTest_send_manual_tx_cmd_r17(UINT32 type,UINT32 package,UINT32 freq,UINT32 pwrLevel,UINT32 msgPattern)
{
    uint16 num_send;
    uint8 power_Level = 0x0f;
    uint8 uart_buff[50]= {0,};

    memset(uart_buff,0x00,sizeof(uart_buff));



    if(0 == pwrLevel)
    {
        power_Level = 0x0d;
    }
    else if(1 == pwrLevel)
    {
        power_Level = 0x0e;
    }
    else if(2 == pwrLevel)
    {
        power_Level = 0x0f;
    }
    RdaBt_Stop_cmd();

    switch(type)
    {
        case CmdGFSK:
            //GFSK test
            memcpy(uart_buff,rdabt_DHn_test_r17,sizeof(rdabt_DHn_test_r17));
            uart_buff[4]=freq;
            uart_buff[7]=msgPattern;
            uart_buff[10] = power_Level;

            if (0==package)
            {
                uart_buff[8]=0x04;
                uart_buff[26]=0x1B;
                uart_buff[27]=0x00;
            }
            else if (1==package)
            {
                uart_buff[8]=0x0B;
                uart_buff[26]=0xB7;
                uart_buff[27]=0x00;
            }
            else if (2==package)
            {
                uart_buff[8]=0x0f;
                uart_buff[26]=0x53;
                uart_buff[27]=0x01;
            }

            rdabt_uart_tx(uart_buff,sizeof(rdabt_DHn_test_r17),&num_send);
            RDABT_DELAY(5);

            break;
        case Cmd4DQPSK:
            //4DPSK test
            memcpy(uart_buff,rdabt_2DHn_test_r17,sizeof(rdabt_2DHn_test_r17));
            uart_buff[4]=freq;
            uart_buff[7]=msgPattern;
            uart_buff[10] = power_Level;

            if (0==package)
            {
                uart_buff[8]=0x04;
                uart_buff[26]=0x36;
                uart_buff[27]=0x00;
            }
            else if (1==package)
            {
                uart_buff[8]=0x0A;
                uart_buff[26]=0x6F;
                uart_buff[27]=0x01;
            }
            else if (2==package)
            {
                uart_buff[8]=0x0E;
                uart_buff[26]=0xA7;
                uart_buff[27]=0x02;
            }

            rdabt_uart_tx(uart_buff,sizeof(rdabt_2DHn_test_r17),&num_send);
            RDABT_DELAY(5);

            break;
        case Cmd8DPSK:
            //8DFSK test
            memcpy(uart_buff,rdabt_3DHn_test_r17,sizeof(rdabt_3DHn_test_r17));
            uart_buff[4]=freq;
            uart_buff[7]=msgPattern;
            uart_buff[10] = power_Level;

            if (0==package)
            {
                uart_buff[8]=0x08;
                uart_buff[26]=0x53;
                uart_buff[27]=0x00;
            }
            else if (1==package)
            {
                uart_buff[8]=0x0B;
                uart_buff[26]=0x28;
                uart_buff[27]=0x02;
            }
            else if (2==package)
            {
                uart_buff[8]=0x0F;
                uart_buff[26]=0xFD;
                uart_buff[27]=0x03;
            }
            rdabt_uart_tx(uart_buff,sizeof(rdabt_3DHn_test_r17),&num_send);
            RDABT_DELAY(5);

            break;
        default:
            break;

    }

}

//typedef struct bt_test_burst
//{
//u_int32 t_syncword_low; //syncword
//u_int32 t_syncword_hight;
//u_int16 t_tx_len; //packet length
//u_int8 hop_enable; //1:enable 0:disable
//u_int8 t_hop_frq; /* val: [0.....78]*/
//u_int8 t_msg_type; //0...3
//u_int8 t_tx_packet_type;//packet type
//u_int8 t_EDR_mode; // 1,2,3
//u_int8 t_power_level; //power level
//u_int8 crc_init;
//u_int8 is_whiten;
//u_int8 whiten_init;
//u_int8 t_am_addr; /* 3 bits*/
//} t_bt_test_burst;

typedef struct
{
    u_int32 t_head;
    u_int32 t_syncword_low; //syncword
    u_int32 t_syncword_hight;
    u_int16 t_tx_len; //packet length
    u_int8 hop_enable; //1:enable 0:disable
    u_int8 t_hop_frq; /* val: [0.....78]*/
    u_int8 t_msg_type; //0...3
    u_int8 t_tx_packet_type;//packet type
    u_int8 t_EDR_mode; // 1,2,3
    u_int8 t_power_level; //power level
    u_int8 crc_init;
    u_int8 is_whiten;
    u_int8 whiten_init;
    u_int8 t_am_addr; /* 3 bits*/
} bt_test_burst;

const bt_test_burst t_bt_test_burst=
{
    0x14fd1001,
    0xE23A1A33,             /*t_syncword_low*/
    0xCE2C7A4E,             /*t_syncword_hight*/
    0x0012,                 /*t_tx_len*/
    0x00,                   /*hop_enable*/
    0x00,                   /*t_hop_frq*/
    0x02,                   /*t_msg_type*/
    0x04,                   /*t_tx_packet_type*/
    0x00,                   /*t_EDR_mode*/
    0x0f,                   /*t_power_level*/
    0x00,                   /*crc_init*/
    0x00,                   /*is_whiten*/
    0x00,                   /*whiten_init*/
    0x00,                   /*t_am_addr*/
};

void RdaBtTest_send_manual_tx_cmd_r18(UINT32 type,UINT32 package,UINT32 freq,UINT32 pwrLevel,UINT32 msgPattern)
{
    uint16 num_send;
    uint8 power_Level = 0x0f,i=0;
    bt_test_burst t_bt_test_burst_tmp= {0,};
    uint8 *buffer_t=(uint8 *)&t_bt_test_burst_tmp;
    kal_prompt_trace(1,"RdaBtTest_send_manual_tx_cmd_r18 %d,%d,%d,%d,%d",type,package,freq,pwrLevel,msgPattern);
    if(0 == pwrLevel)
    {
        power_Level = 0x0d;
    }
    else if(1 == pwrLevel)
    {
        power_Level = 0x0e;
    }
    else if(2 == pwrLevel)
    {
        power_Level = 0x0f;
    }
    RdaBt_Stop_cmd();

    memcpy((uint8 *)&t_bt_test_burst_tmp,(uint8 *)&t_bt_test_burst,sizeof(bt_test_burst));
    t_bt_test_burst_tmp.t_hop_frq = freq;                   /*t_hop_frq*/
    t_bt_test_burst_tmp.t_power_level = power_Level;        /*t_power_level*/
    t_bt_test_burst_tmp.t_msg_type = msgPattern;
    if (freq>78) t_bt_test_burst_tmp.hop_enable=1;  /*hop_enable*/
    switch(type)
    {
        case CmdGFSK:
            //GFSK test
            t_bt_test_burst_tmp.t_EDR_mode = 0;                     /*t_EDR_mode*/

            if (0==package)//DH1
            {
                t_bt_test_burst_tmp.t_tx_packet_type=0x04;
                t_bt_test_burst_tmp.t_tx_len=0x001B;
            }
            else if (1==package)//DH3
            {
                t_bt_test_burst_tmp.t_tx_packet_type=0x0B;
                t_bt_test_burst_tmp.t_tx_len=0x00B7;
            }
            else if (2==package)//DH5
            {
                t_bt_test_burst_tmp.t_tx_packet_type=0x0F;
                t_bt_test_burst_tmp.t_tx_len=0x0153;
            }

            rdabt_uart_tx((uint8 *)&t_bt_test_burst_tmp,sizeof(bt_test_burst),&num_send);
            RDABT_DELAY(5);

            break;

        case Cmd4DQPSK:
            //4DPSK test
            t_bt_test_burst_tmp.t_EDR_mode = 1;                     /*t_EDR_mode*/

            if (0==package)//2DH1
            {
                t_bt_test_burst_tmp.t_tx_packet_type=0x04;              /*t_tx_packet_type*/
                t_bt_test_burst_tmp.t_tx_len=0x0036;                /*t_tx_len*/
            }
            else if (1==package)//2DH3
            {
                t_bt_test_burst_tmp.t_tx_packet_type=0x0A;              /*t_tx_packet_type*/
                t_bt_test_burst_tmp.t_tx_len=0x016F;                /*t_tx_len*/
            }
            else if (2==package)//2DH5
            {
                t_bt_test_burst_tmp.t_tx_packet_type=0x0E;              /*t_tx_packet_type*/
                t_bt_test_burst_tmp.t_tx_len=0x02A7;                /*t_tx_len*/
            }

            rdabt_uart_tx((uint8 *)&t_bt_test_burst_tmp,sizeof(bt_test_burst),&num_send);
            RDABT_DELAY(5);

            break;
        case Cmd8DPSK:
            //8DFSK test
            t_bt_test_burst_tmp.t_EDR_mode = 2;                     /*t_EDR_mode*/


            if (0==package)//3DH1
            {
                t_bt_test_burst_tmp.t_tx_packet_type=0x08;          /*t_tx_packet_type*/
                t_bt_test_burst_tmp.t_tx_len=0x0053;            /*t_tx_len*/
            }
            else if (1==package)//3DH3
            {
                t_bt_test_burst_tmp.t_tx_packet_type=0x0B;          /*t_tx_packet_type*/
                t_bt_test_burst_tmp.t_tx_len=0x0228;            /*t_tx_len*/
            }
            else if (2==package)//3DH5
            {
                t_bt_test_burst_tmp.t_tx_packet_type=0x0F;          /*t_tx_packet_type*/
                t_bt_test_burst_tmp.t_tx_len=0x03FD;            /*t_tx_len*/
            }

            rdabt_uart_tx((uint8 *)&t_bt_test_burst_tmp,sizeof(bt_test_burst),&num_send);
            RDABT_DELAY(5);

            break;
        default:
            break;

    }
    //for(i=0;i<sizeof(bt_test_burst);i++)
    kal_prompt_trace(1,"%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x",buffer_t[0],buffer_t[1],buffer_t[2],buffer_t[3],
                     buffer_t[4],buffer_t[5],buffer_t[6],buffer_t[7],buffer_t[8],buffer_t[9]
                     ,buffer_t[10],buffer_t[11],buffer_t[12],buffer_t[13],buffer_t[14],buffer_t[15],buffer_t[16],buffer_t[17],buffer_t[18],buffer_t[19],
                     buffer_t[20],buffer_t[21]
                     ,buffer_t[22],buffer_t[23]);

}


void RdaBtTest_send_manual_tx_cmd(UINT32 type,UINT32 package,UINT32 freq,UINT32 pwrLevel,UINT32 msgPattern)
{
    switch(rdabt_chip_sel)
    {
#ifdef __RDA_CHIP_R5_5868__
        case RDA_BT_R5_5868_ENUM:
            break;
#endif
#ifdef __RDA_CHIP_R7_5868plus__
        case RDA_BT_R7_5868PLUS_ENUM:
            break;
#endif
#ifdef __RDA_CHIP_R8_5870__
        case RDA_BT_R8_5870_ENUM:
            break;
#endif
#if defined( __RDA_CHIP_R10_5868E__ ) || defined(__RDA_CHIP_R10_5872__)
        case RDA_BT_R10_5872_ENUM:
        case RDA_BT_R10_5868E_ENUM:
            break;
#endif
#if defined(__RDA_CHIP_R10_5868H__) || defined(__RDA_CHIP_R10_5872H__)
        case RDA_BT_R10_AL_5868H_ENUM:
        case RDA_BT_R10_AL_5872H_ENUM:
            break;
#endif
#ifdef __RDA_CHIP_R10_5870E__
        case RDA_BT_R10_5870E_ENUM:
            break;
#endif
#ifdef __RDA_CHIP_R11_5872P__
        case RDA_BT_R11_5872P_ENUM:
            break;
#endif
#ifdef __RDA_CHIP_R11_5875__
        case RDA_BT_R11_5875_ENUM:
            break;
#endif
#ifdef __RDA_CHIP_R12_5876__
        case RDA_BT_R12_5876_ENUM:
            break;
#endif
#ifdef __RDA_CHIP_R12_5870P__
        case RDA_BT_R12_5870P_ENUM:
            break;
#endif
#ifdef __RDA_CHIP_R12_5876M__
        case RDA_BT_R12_5876M_ENUM:
            break;
#endif
#ifdef __RDA_CHIP_R16_5876P__
        case RDA_BT_R16_5876P_ENUM:
            break;
#endif
#ifdef __RDA_CHIP_R12_5990__
        case RDA_BT_R12_5990_ENUM:
            break;
#endif
#ifdef __RDA_CHIP_R17_8809__
        case RDA_BT_R17_8809_ENUM:
            RdaBtTest_send_manual_tx_cmd_r17(type,package,freq,pwrLevel,msgPattern);
            break;
#endif
#ifdef __RDA_CHIP_R18_8809E__
        case RDA_BT_R18_8809E_ENUM:
            RdaBtTest_send_manual_tx_cmd_r18(type,package,freq,pwrLevel,msgPattern);
            break;
#endif

        default:
            //      ASSERT(0);  // xiaoyifeng
            break;
    }

}

#else
void RdaBtTest_send_manual_tx_cmd(UINT32 type,UINT32 package,UINT32 freq,UINT32 pwrLevel)
{
    hal_HstSendEventCust(0xffffffff);
    hal_HstSendEventCust(0xbbbb0fff);
    hal_HstSendEventCust(type);
    hal_HstSendEventCust(package);
    hal_HstSendEventCust(freq);
    hal_HstSendEventCust(pwrLevel);

    uint16 num_send;
    uint8 power_in = 0x0f;
    if(0 == pwrLevel)
    {
        power_in = 0x00;
    }
    else if(1 == pwrLevel)
    {
        power_in = 0x08;
    }
    else if(2 == pwrLevel)
    {
        power_in = 0x0f;
    }
    RdaBt_Stop_cmd();
    if((0 == type)&&(0 == package))
    {
        rdabt_DH1_test[4]=freq;
        rdabt_DH1_test[10] = power_in;
        rdabt_uart_tx(rdabt_DH1_test,sizeof(rdabt_DH1_test),&num_send);
        RDABT_DELAY(5);
    }
    else if((0 == type)&&(1 == package))
    {
        rdabt_DH3_test[4]=freq;
        rdabt_DH3_test[10] = power_in;
        rdabt_uart_tx(rdabt_DH3_test,sizeof(rdabt_DH3_test),&num_send);
        RDABT_DELAY(5);
    }
    else if((0 == type)&&(2 == package))
    {
        rdabt_DH5_test[4]=freq;
        rdabt_DH5_test[10] = power_in;
        rdabt_uart_tx(rdabt_DH5_test,sizeof(rdabt_DH5_test),&num_send);
        RDABT_DELAY(5);
    }
    else if((1 == type)&&(0 == package))
    {
        rdabt_2DH1_test[4]=freq;
        rdabt_2DH1_test[10] = power_in;
        rdabt_uart_tx(rdabt_2DH1_test,sizeof(rdabt_2DH1_test),&num_send);
        RDABT_DELAY(5);
    }
    else if((1 == type)&&(1 == package))
    {
        rdabt_2DH3_test[4]=freq;
        rdabt_2DH3_test[10] = power_in;
        rdabt_uart_tx(rdabt_2DH3_test,sizeof(rdabt_2DH3_test),&num_send);
        RDABT_DELAY(5);
    }
    else if((1 == type)&&(2 == package))
    {
        rdabt_2DH5_test[4]=freq;
        rdabt_2DH5_test[10] = power_in;
        rdabt_uart_tx(rdabt_2DH5_test,sizeof(rdabt_2DH5_test),&num_send);
        RDABT_DELAY(5);
    }
    else if((2 == type)&&(0 == package))
    {
        rdabt_3DH1_test[4]=freq;
        rdabt_3DH1_test[10] = power_in;
        rdabt_uart_tx(rdabt_3DH1_test,sizeof(rdabt_3DH1_test),&num_send);
        RDABT_DELAY(5);
    }
    else if((2 == type)&&(1 == package))
    {
        rdabt_3DH3_test[4]=freq;
        rdabt_3DH3_test[10] = power_in;
        rdabt_uart_tx(rdabt_3DH3_test,sizeof(rdabt_3DH3_test),&num_send);
        RDABT_DELAY(5);
    }
    else if((2 == type)&&(2 == package))
    {
        rdabt_3DH5_test[4]=freq;
        rdabt_3DH5_test[10] = power_in;
        rdabt_uart_tx(rdabt_3DH5_test,sizeof(rdabt_3DH5_test),&num_send);
        RDABT_DELAY(5);
    }
}
#endif

void Rdabt_manual_tx_test(UINT32 type,UINT32 package,UINT32 freq,UINT32 pwrLevel,UINT32 msgPattern)
{
    TestMode=TestMode_NorSignal_Tx;
    rda_bt_poweroff_for_test();
    rda_bt_poweron_for_test();
    RdaBtTest_send_manual_tx_cmd(type,package,freq,pwrLevel,msgPattern);
}

#endif
#endif

