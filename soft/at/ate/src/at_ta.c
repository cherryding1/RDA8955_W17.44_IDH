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

#include "at.h"
#include "at_cmd_gc.h"
#include "at_ta.h"

BOOL charset_gsm2ascii(UINT8 *src, UINT16 slen, UINT8 *dst, UINT16 *dlen);
BOOL charset_gsm2ucs2(UINT8 *src, UINT16 slen, UINT8 *dst, UINT16 *dlen);
BOOL charset_gsm2gbk(UINT8 *src, UINT16 slen, UINT8 *dst, UINT16 *dlen);
BOOL charset_ascii2ucs2(UINT8 *src, UINT16 slen, UINT8 *dst, UINT16 *dlen);
BOOL charset_ucs2togbk(UINT8 *src, UINT16 slen, UINT8 *dst, UINT16 *dlen);
BOOL charset_gbk2ucs2(UINT8 *src, UINT16 slen, UINT8 *dst, UINT16 *dlen);

// If the result is cs_cvt_NEEDLESS then it means this function do nothing! so if you want to copy data from src to dst, you must
// do it by yourself.
ta_chset_convert_result_t ate_TaConvert(ta_convert_mode_t ds, UINT8 *src, UINT16 slen, UINT8 *dst, UINT16 *p_dlen)
{
    at_chset_t te_cs, mt_cs;

    return cs_cvt_needless; // added by yangtt and zhangxb 2008-05-09 for bug 8257
    te_cs = cfg_GetTeChset();
    mt_cs = cfg_GetMtChset();

    AT_TC(g_sw_SA, "te_cs=%d,mt_cs=%d.", te_cs, mt_cs);

    // Data comes from TE and to MT.

    if (ds == mode_te2mt)
    {
        switch (te_cs)
        {
        case cs_gsm:
            switch (mt_cs)
            {
            case cs_gsm:
                return cs_cvt_needless;

            case cs_hex:
                charset_gsm2ascii(src, slen, dst, p_dlen);
                return cs_cvt_okay;

            case cs_ucs2:
                charset_gsm2ucs2(src, slen, dst, p_dlen);
                return cs_cvt_okay;

            case cs_gbk:
                charset_gsm2gbk(src, slen, dst, p_dlen);
                return cs_cvt_okay;

            case cs_COUNT_:
            case cs_INVALID_:
            default:
                return cs_cvt_failed;
            }
            break;

        case cs_hex:
            switch (mt_cs)
            {
            case cs_gsm:
                return cs_cvt_failed;

            case cs_hex:
                return cs_cvt_needless;

            case cs_ucs2:
                charset_ascii2ucs2(src, slen, dst, p_dlen);
                return cs_cvt_okay;

            case cs_gbk:
                return cs_cvt_needless;

            case cs_COUNT_:
            case cs_INVALID_:
            default:
                return cs_cvt_failed;
            }
            break;

        case cs_ucs2:
            switch (mt_cs)
            {
            case cs_gsm:
                return cs_cvt_failed;

            case cs_hex:
                return cs_cvt_failed;

            case cs_ucs2:
                return cs_cvt_needless;

            case cs_gbk:
                charset_ucs2togbk(src, slen, dst, p_dlen);
                return cs_cvt_okay;

            case cs_COUNT_:
            case cs_INVALID_:
            default:
                return cs_cvt_failed;
            }
            break;

        case cs_gbk:
            switch (mt_cs)
            {
            case cs_gsm:
                return cs_cvt_failed;

            case cs_hex:
                return cs_cvt_failed;

            case cs_ucs2:
                charset_gbk2ucs2(src, slen, dst, p_dlen);
                return cs_cvt_okay;

            case cs_gbk:
                return cs_cvt_needless;

            case cs_COUNT_:
            case cs_INVALID_:
            default:
                return cs_cvt_failed;
            }
            break;

        case cs_COUNT_:
        case cs_INVALID_:
        default:
            return cs_cvt_failed;
        }
    }

    // Data comes from MT and to TE.
    else if (ds == mode_mt2te)
    {
        // Just support GBK char set on MT side.
        switch (mt_cs)
        {
        case cs_gbk:
            switch (te_cs)
            {
            case cs_gsm:
            case cs_hex:
                return cs_cvt_failed;

            case cs_ucs2:
                charset_gbk2ucs2(src, slen, dst, p_dlen);
                return cs_cvt_okay;

            case cs_gbk:
                return cs_cvt_needless;

            case cs_COUNT_:
            case cs_INVALID_:
            default:
                return cs_cvt_failed;
            }
            break;

        case cs_COUNT_:
        case cs_INVALID_:
        default:
            return cs_cvt_failed;
        }
    }

    return cs_cvt_INVALID_;
}

BOOL charset_gsm2ascii(UINT8 *src, UINT16 slen, UINT8 *dst, UINT16 *dlen)
{
    INT32 r = SUL_GSM2ASCII(src, slen, dst);

    if (r == -1)
    {
        *dlen = 0;
        AT_TC(g_sw_SA, "call SUL_GSM2ASCII() failed. *dlen=%d,last_err_code=%d", *dlen, SUL_GetLastError());
        return FALSE;
    }

    {
        *dlen = (UINT16)r;
        return TRUE;
    }
}

BOOL charset_gsm2ucs2(UINT8 *src, UINT16 slen, UINT8 *dst, UINT16 *dlen)
{
    *dlen = SUL_GSM2UCS2(src, dst, slen);

    if (*dlen == 0 || *dlen == (UINT16)-1)
    {
        AT_TC(g_sw_SA, "call SUL_GSM2UCS2() failed. *dlen=%d,last_err_code=%d", *dlen, SUL_GetLastError());
        return FALSE;
    }

    return TRUE;
}

BOOL charset_gsm2gbk(UINT8 *src, UINT16 slen, UINT8 *dst, UINT16 *dlen)
{
    INT32 r = SUL_GSM2ASCII(src, slen, dst);

    if (r == -1)
    {
        *dlen = 0;
        AT_TC(g_sw_SA, "call SUL_GSM2ASCII() failed. *dlen=%d,last_err_code=%d", *dlen, SUL_GetLastError());
        return FALSE;
    }

    {
        *dlen = (UINT16)r;
        return TRUE;
    }
}

BOOL charset_ascii2ucs2(UINT8 *src, UINT16 slen, UINT8 *dst, UINT16 *dlen)
{
    *dlen = SUL_GSM2UCS2(src, dst, slen);

    if (*dlen == 0 || *dlen == (UINT16)-1)
    {
        AT_TC(g_sw_SA, "call SUL_GSM2UCS2() failed. *dlen=%d,last_err_code=%d", *dlen, SUL_GetLastError());
        return FALSE;
    }

    return TRUE;
}

BOOL charset_ucs2togbk(UINT8 *src, UINT16 slen, UINT8 *dst, UINT16 *dlen)
{
    *dlen = AT_UtilUcs2ToGbk(src, slen, dst);

    if (*dlen == 0)
    {
        AT_TC(g_sw_SA, "call AT_UtilUcs2ToGbk() failed. *dlen=%d,last_err_code=%d", *dlen, SUL_GetLastError());
        return FALSE;
    }

    return TRUE;
}

BOOL charset_gbk2ucs2(UINT8 *src, UINT16 slen, UINT8 *dst, UINT16 *dlen)
{
    *dlen = AT_UtilGbk2Ucs2(src, slen, dst);

    if (*dlen == 0)
    {
        AT_TC(g_sw_SA, "call charset_gbk2ucs2() failed. *dlen=%d,last_err_code=%d", *dlen, SUL_GetLastError());
        return FALSE;
    }

    return TRUE;
}
