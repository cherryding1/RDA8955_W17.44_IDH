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




typedef enum
{
    MCD_CARD_STATE_IDLE    = 0,
    MCD_CARD_STATE_READY   = 1,
    MCD_CARD_STATE_IDENT   = 2,
    MCD_CARD_STATE_STBY    = 3,
    MCD_CARD_STATE_TRAN    = 4,
    MCD_CARD_STATE_DATA    = 5,
    MCD_CARD_STATE_RCV     = 6,
    MCD_CARD_STATE_PRG     = 7,
    MCD_CARD_STATE_DIS     = 8
} MCD_CARD_STATE_T;


typedef union
{
    UINT32 reg;
    struct
    {
        UINT32                          :3;
        UINT32 akeSeqError              :1;
        UINT32                          :1;
        UINT32 appCmd                   :1;
        UINT32                          :2;
        UINT32 readyForData             :1;
        MCD_CARD_STATE_T currentState   :4;
        UINT32 eraseReset               :1;
        UINT32 cardEccDisabled          :1;
        UINT32 wpEraseSkip              :1;
        UINT32 csdOverwrite             :1;
        UINT32                          :2;
        UINT32 error                    :1;
        UINT32 ccError                  :1;
        UINT32 cardEccFailed            :1;
        UINT32 illegalCommand           :1;
        UINT32 comCrcError              :1;
        UINT32 lockUnlockFail           :1;
        UINT32 cardIsLocked             :1;
        UINT32 wpViolation              :1;
        UINT32 eraseParam               :1;
        UINT32 eraseSeqError            :1;
        UINT32 blockLenError            :1;
        UINT32 addressError             :1;
        UINT32 outOfRange               :1;
    } fields;
} MCD_CARD_STATUS_T;







typedef enum
{
    MCD_CARD_NORMAL,
    MCD_CARD_SMALL,
    MCD_CARD_SMALL_SPECIAL
} MCD_CARD_PRO_T;



typedef struct
{
    MCD_CSD_T              csdinfo;
    BOOL                   open;
    BOOL                   oper_fail;
    BOOL                   init;
    BOOL                   present;
    BOOL                   capcity_small;
    BOOL                   capcity_small_special;    MCD_CARD_PRO_T           try_pro;

} MCD_TWO_SDMMC_CONTROL;





