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

#include "ml.h"
#ifdef MMI_ON_HARDWARE_P
#include "errorcode.h"
#endif

#ifdef _FS_SIMULATOR_

#else

#include "sul.h"
#include "cfw_prv.h"
#endif
//#include "sul.h"
//#include "cfw_prv.h"

#define MAX_CHARACTER_SIZE 15

static struct ML_Table ML_table;
static struct ML_Table *pML_table = &ML_table;

BOOL g_Flag           = FALSE;
UINT8 g_Character[12] = { 0, };


UINT32 ML_RegisterTable(struct ML_Table *ML)
{
    struct ML_Table **p = &pML_table;

    if (!p)
    {
        return ERR_INVALID_HANDLE;
    }

    if (ML->next)
    {
        return ERR_BUSY_DEVICE;
    }

    if (1 == g_Flag)
    {
        return ERR_BUSY_DEVICE;
    }
    else
    {
        g_Flag = TRUE;
    }

    while (*p)
    {
        if (ML == *p)
        {
            g_Flag = FALSE;
            return ERR_BUSY_DEVICE;
        }

        p = &(*p)->next;
    }

    ML->next = pML_table;

    pML_table = ML;
    g_Flag    = FALSE;

    return ERR_SUCCESS;
}

UINT32 ML_UnRegisterTable(struct ML_Table *ML)
{
    return ERR_SUCCESS;
}

static struct ML_Table *ML_FindTable(UINT8 *charset)
{
    struct ML_Table *p;

    if (NULL == charset)
    {
        return NULL;
    }
    for (p = pML_table; p; p = p->next)
    {
        if( NULL == p->charset )
        {
            return NULL;
        }
        if (!SUL_StrCaselessCompare((TCHAR *)p->charset, (TCHAR *)charset))
            break;
    }
    return p;
}

UINT32 ML_SetCodePage(UINT8 nCharset[12])
{
    SUL_StrCopy((TCHAR *)g_Character, (TCHAR *) nCharset);
    return ERR_SUCCESS;
}
UINT32 ML_Unicode2LocalLanguageBigEnding(const UINT8 *in, UINT32 in_len, UINT8 **out, UINT32 *out_len, UINT8 nCharset[12])
{
    UINT32 i, o;
    //UINT16 uni = 0x0000;
    INT32 unilen, utflen;
    UINT8 *result;
    struct ML_Table *pTable = NULL;
    int maxlen;

    unilen = utflen = 0x00;

    //
    //Add "\0"
    //
    maxlen = in_len + 1;

    //  CSW_TRACE(_CSW|TLEVEL(CFW_ML_TS_ID)|TDB|TNB_ARG(0),TSTR("---> ML_Unicode2LocalLanguage()\n",0x080000cc));
    maxlen = ALIGN(maxlen, 4);

    *out = result = (UINT8 *)CSW_ML_MALLOC(maxlen);
    if (!*out)
    {
        *out_len = 0;
        return ERR_INVALID_HANDLE;
    }
    SUL_MemSet8(result, 0x00, (UINT16)maxlen);
    //Add by lixp at 20090211 for mult task conflicting
    if( NULL != nCharset )
    {
        pTable = ML_FindTable(nCharset);
        CSW_TC_MEMBLOCK(CFW_ML_TS_ID, (UINT8 *)nCharset, 12 , 16);
    }
    else
    {
        pTable = ML_FindTable(g_Character);
        CSW_TC_MEMBLOCK(CFW_ML_TS_ID, (UINT8 *)g_Character, 12 , 16);
    }
    if( NULL == pTable )
    {

        goto conv_err;
    }
    for (i = o = 0; i < in_len; i += 2, o += unilen)
    {

        /* convert from Unicode to locallanguage */
        UINT16 p = (in[i] << 8) + in[i + 1];
        unilen = pTable->uni2char(p, &result[o], in_len - o);

        CSW_TRACE(_CSW|TSTDOUT|TDB|TNB_ARG(0), TSTR("Input Data output\n",0x080001b9));
        if (unilen < 0)
        {
            goto conv_err;
        }
    }
    result[o] = '\0';
    *out_len  = o;

    // CSW_TRACE(_CSW|TLEVEL(CFW_ML_TS_ID)|TDB|TNB_ARG(0),TSTR("<--- ML_Unicode2LocalLanguage()\n",0x080000ce));

    return ERR_SUCCESS;

conv_err:
    CSW_TRACE(_CSW|TLEVEL(CFW_ML_TS_ID)|TDB|TNB_ARG(0), TSTR("conv_err <--- ML_Unicode2LocalLanguage()\n",0x080000cf));
    CSW_ML_FREE(result);
    *out = NULL;
    return ERR_INVALID_HANDLE;
}

UINT32 ML_Unicode2LocalLanguage(const UINT8 *in, UINT32 in_len, UINT8 **out, UINT32 *out_len, UINT8 nCharset[12])
{
    UINT32 i, o;
    //UINT16 uni = 0x0000;
    INT32 unilen, utflen;
    UINT8 *result;
    struct ML_Table *pTable = NULL;
    int maxlen;

    unilen = utflen = 0x00;

    //
    //Add "\0"
    //
    maxlen = in_len + 1;

    //  CSW_TRACE(_CSW|TLEVEL(CFW_ML_TS_ID|C_DETAIL)|TDB|TNB_ARG(0),TSTR("---> ML_Unicode2LocalLanguage()\n",0x080000d0));
    maxlen = ALIGN(maxlen, 4);

    *out = result = (UINT8 *)CSW_ML_MALLOC(maxlen);
    if (!*out)
    {
        *out_len = 0;
        return ERR_INVALID_HANDLE;
    }
    SUL_MemSet8(result, 0x00, (UINT16)maxlen);
    //Add by lixp at 20090211 for mult task conflicting
    if( NULL != nCharset )
    {
        pTable = ML_FindTable(nCharset);
        CSW_TC_MEMBLOCK((CFW_ML_TS_ID | C_DETAIL), (UINT8 *)nCharset, 12 , 16);
    }
    else
    {
        pTable = ML_FindTable(g_Character);
        CSW_TC_MEMBLOCK((CFW_ML_TS_ID | C_DETAIL), (UINT8 *)g_Character, 12 , 16);
    }
    if( NULL == pTable )
    {

        //ml_Assert(0,"g_Character = %s",g_Character);
        goto conv_err;
    }
    for (i = o = 0; i < in_len; i += 2, o += unilen)
    {

        /* convert from Unicode to locallanguage */
        unilen = pTable->uni2char((*(UINT16 *)(&in[i])), &result[o], in_len - o);
        if (unilen < 0)
        {
            CSW_TRACE((CFW_ML_TS_ID | C_DETAIL), "Input Data[0x%x]\n", (*(UINT16 *)(&in[i])));
            goto conv_err;
        }
    }
    result[o] = '\0';
    *out_len  = o;

    // CSW_TRACE(_CSW|TLEVEL(CFW_ML_TS_ID|C_DETAIL)|TDB|TNB_ARG(0),TSTR("<--- ML_Unicode2LocalLanguage()\n",0x080000d1));

    return ERR_SUCCESS;

conv_err:
    CSW_TRACE((CFW_ML_TS_ID | C_DETAIL), "conv_err <--- ML_Unicode2LocalLanguage()\n");
    CSW_ML_FREE(result);
    *out = NULL;
    return ERR_INVALID_HANDLE;
}

UINT32 ML_LocalLanguage2Unicode(const UINT8 *in, UINT32 in_len, UINT8 **out, UINT32 *out_len, UINT8 nCharset[12])
{
    struct ML_Table *pTable = NULL;
    INT32 i, o;
    UINT16 uni = 0;
    INT32 unilen, utflen;
    UINT8 *result;
    int maxlen;
    unilen = utflen = 0;

    maxlen = (3 * in_len) + 1;

    CSW_TRACE((CFW_ML_TS_ID | C_DETAIL), "---> ML_LocalLanguage2Unicode()\n");

    maxlen = ALIGN(maxlen, 4);
    *out = result = (UINT8 *)CSW_ML_MALLOC(maxlen);
    if (!*out)
    {
        CSW_TRACE((CFW_ML_TS_ID | C_DETAIL), "ML_LocalLanguage2Unicode() cannot allocate memory\n");
        *out_len = 0;
        return ERR_INVALID_HANDLE;
    }
    SUL_MemSet8(result, 0x00, maxlen);
    //Add by lixp at 20090211 for mult task conflicting
    if( NULL != nCharset )
    {
        pTable = ML_FindTable(nCharset);
    }
    else
    {
        pTable = ML_FindTable(g_Character);
    }
    if( NULL == pTable )
    {
        goto conv_err;
    }
    for (i = o = 0; i < (INT32)in_len; i += unilen, o += 2)
    {

        /* convert from locallanguage to unicode */
        unilen = pTable->char2uni((UINT8 *)&in[i], in_len - i, &uni);
        if (unilen < 0)
        {
            goto conv_err;
        }
        result[o] = uni >> 8;
        result[o + 1] = uni & 0xFF;

    }

    result[o] = '\0';
    *out_len  = o;

    CSW_TRACE((CFW_ML_TS_ID | C_DETAIL), "<--- ML_LocalLanguage2Unicode()\n");

    return ERR_SUCCESS;

conv_err:
    CSW_TRACE((CFW_ML_TS_ID | C_DETAIL), "conv_err<--- ML_LocalLanguage2Unicode()\n");
    CSW_ML_FREE(result);
    *out = NULL;
    return ERR_INVALID_HANDLE;
}

UINT32 ML_LocalLanguage2UnicodeBigEnding(const UINT8 *in, UINT32 in_len, UINT8 **out, UINT32 *out_len, UINT8 nCharset[12])
{
    struct ML_Table *pTable = NULL;
    INT32 i, o;
    UINT16 uni = 0;
    INT32 unilen, utflen;
    UINT8 *result;
    int maxlen = (3 * in_len) + 1;

    unilen = utflen = 0;

    maxlen = ALIGN(maxlen, 4);
    *out = result = (UINT8 *)CSW_ML_MALLOC(maxlen);
    if (!*out)
    {
        CSW_TRACE((CFW_ML_TS_ID | C_DETAIL), "ML_LocalLanguage2UnicodeBigEnding() cannot allocate memory\n");
        *out_len = 0;
        return ERR_INVALID_HANDLE;
    }
    SUL_MemSet8(result, 0x00, maxlen);

    if( NULL != nCharset )
    {
        pTable = ML_FindTable(nCharset);
    }
    else
    {
        pTable = ML_FindTable(g_Character);
    }
    if( NULL == pTable )
    {
        CSW_TRACE((CFW_ML_TS_ID | C_DETAIL), "Table NULL g_Character %s\n", g_Character);
        CSW_TRACE((CFW_ML_TS_ID | C_DETAIL), "Table NULL g_Character %s\n", nCharset);
        goto conv_err;
    }

    for (i = o = 0; i < (INT32)in_len; i += unilen, o += 2)
    {

        unilen = pTable->char2uni((UINT8 *)&in[i], in_len - i, &uni);
        if (unilen < 0)
        {
            CSW_TRACE((CFW_ML_TS_ID | C_DETAIL), "char2uni NULL in %d\n", in);
            goto conv_err;
        }

        result[o] = uni & 0xFF ;
        result[o + 1] = uni >> 8;

    }

    result[o] = '\0';
    *out_len  = o;

    return ERR_SUCCESS;

conv_err:
    CSW_ML_FREE(result);
    *out = NULL;
    return ERR_INVALID_HANDLE;
}
UINT32 ML_InitCP0(void);
UINT32 ML_InitCP1256(void);
UINT32 ML_InitCP1258(void);
UINT32 ML_InitCP874(void);
UINT32 ML_InitCP936(void);
UINT32 ML_InitISO8859_1(void);
UINT32 ML_InitISO8859_5(void);
UINT32 ML_InitISO8859_6(void);

UINT32 ML_Init()
{
    //#ifdef ISO8859_1
    ML_InitISO8859_1();
    //#endif
#ifdef CP0
    ML_InitCP0();
#endif

#ifdef CP437
    ML_InitCP437();
#endif

#ifdef CP737
    ML_InitCP737();
#endif

#ifdef CP775
    ML_InitCP775();
#endif

#ifdef CP850
    ML_InitCP850();
#endif

#ifdef CP852
    ML_InitCP852();
#endif

#ifdef CP855
    ML_InitCP855();
#endif

#ifdef CP857
    ML_InitCP857();
#endif

#ifdef CP860
    ML_InitCP860();
#endif

#ifdef CP861
    ML_InitCP861();
#endif

#ifdef CP862
    ML_InitCP862();
#endif

#ifdef CP863
    ML_InitCP863();
#endif

#ifdef CP864
    ML_InitCP864();
#endif

#ifdef CP865
    ML_InitCP865();
#endif

#ifdef CP869
    ML_InitCP869();
#endif

#ifdef CP874
    ML_InitCP874();
#endif

#ifdef CP932
    ML_InitCP932();
#endif

#ifdef CP936
    ML_InitCP936();
#endif

#ifdef CP950
    ML_InitCP950();
#endif

#ifdef CP1250
    ML_InitCP1250();
#endif

#ifdef CP1251
    ML_InitCP1251();
#endif

#ifdef CP1255
    ML_InitCP1255();
#endif

#ifdef CP1256
    ML_InitCP1256();
#endif

#ifdef CP1258
    ML_InitCP1258();
#endif

#ifdef ISO8859_1
    //ML_InitISO8859_1();
#endif

#ifdef ISO8859_2
    ML_InitISO8859_2();
#endif

#ifdef ISO8859_3
    ML_InitISO8859_3();
#endif

#ifdef ISO8859_4
    ML_InitISO8859_4();
#endif

#ifdef ISO8859_5
    ML_InitISO8859_5();
#endif

#ifdef ISO8859_6
    ML_InitISO8859_6();
#endif

#ifdef ISO8859_7
    ML_InitISO8859_7();
#endif

#ifdef ISO8859_9
    ML_InitISO8859_9();
#endif

#ifdef ISO8859_13
    ML_InitISO8859_13();
#endif

#ifdef ISO8859_14
    ML_InitISO8859_14();
#endif

#ifdef ISO8859_15
    ML_InitISO8859_15();
#endif
    return ERR_SUCCESS;
}
const char CSD_g_Character[][MAX_CHARACTER_SIZE] =
{
//English

    "ISO8859_1",

#if defined(__MMI_LANG_FRENCH__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_SPANISH__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_PORTUGUESE__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_ITALIAN__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_GERMAN__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_MALAY__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_INDONESIAN__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_VIETNAMESE__)

    "CP1258",
#endif

#if defined(__MMI_LANG_TURKISH__)

    "CP857",
#endif

#if defined(__MMI_LANG_RUSSIAN__)

    "ISO8859_5",
#endif

//Arabic language
#if defined(__MMI_LANG_ARABIC__)

    "ISO8859_6",
#endif

#if defined(__MMI_LANG_UYGHUR__)

    "ISO8859_1",
#endif
//Thai language
#if defined(__MMI_LANG_THAI__)

    "CP874",
#endif


//Traditional Chinese
#if defined(__MMI_LANG_TR_CHINESE__)

    "CP936",
#endif

//Simplified Chinese
#if defined(__MMI_LANG_SM_CHINESE__)

    "CP936",
#endif

#if defined(__MMI_LANG_HINDI__)         //guoyt added for adding hindi lang

    "CP0",
    // "ISO8859_1",
#endif
#if defined(__MMI_LANG_PERSIAN__)
    "CP1256",
#endif
#if defined(__MMI_LANG_URDU__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_LAO__)
    "ISO8859_1",
#endif


//qiff add for tamil to configure system profile
#if defined(__MMI_LANG_MARATHI__)

    "ISO8859_1",
#endif
#if defined(__MMI_LANG_TAMIL__)
    "ISO8859_1",
#endif

#if defined(__MMI_LANG_KHMER__)
    "ISO8859_1",
#endif

#if defined(__MMI_LANG_GUJARATI__)

    "ISO8859_1",
#endif
#if defined(__MMI_LANG_BENGALI__)
    "ISO8859_1",
#endif
#if defined(__MMI_LANG_PUNJABI__)

    "ISO8859_1",
#endif
#if defined(__MMI_LANG_TELUGU__)

    "ISO8859_1",
#endif
#if defined(__MMI_LANG_KANNADA__)

    "ISO8859_1",
#endif
#if defined(__MMI_LANG_MALAYALAM__)

    "ISO8859_1",
#endif
#if defined(__MMI_LANG_ORIYA__)

    "ISO8859_1",
#endif
//qiff add end

#if defined(__MMI_LANG_MYANMAR__)
    "ISO8859_1",
#endif

#if defined(__MMI_LANG_ZULU__)
    "ISO8859_1",
#endif

#if defined(__MMI_LANG_XHOSA__)
    "ISO8859_1",
#endif

#if defined(__MMI_LANG_SWAHILI__)
    "ISO8859_1",
#endif

#if defined(__MMI_LANG_CZECH__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_POLISH__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_NORWEGIAN__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_HUNGARIAN__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_SLOVAK__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_DUTCH__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_SWEDISH__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_CROATIAN__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_SLOVENIAN__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_AFRIKAANS__)
    "ISO8859_1",
#endif

#if defined(__MMI_LANG_LITHUANIAN__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_LATVIAN__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_ESTONIAN__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_MOLDOVAN__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_ALBANIAN__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_AZERBAIJANI__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_CATALAN__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_CA_FRENCH__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_ICELANDIC__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_SA_SPANISH__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_SA_PORTUGUESE__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_SESOTHO__)
    "ISO8859_1",
#endif

#if defined(__MMI_LANG_TAGALOG__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_UK_ENGLISH__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_YORUBA__)
    "ISO8859_1",
#endif

#if defined(__MMI_LANG_DANISH__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_FINNISH__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_ROMANIAN__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_HAUSA__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_BULGARIAN__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_UKRAINIAN__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_MACEDONIAN__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_SERBIAN__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_GREEK__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_HEBREW__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_ASSAMESE__)

    "ISO8859_1",
#endif

#if defined(__MMI_LANG_AMHARIC__)
    "ISO8859_1",
#endif
#if defined(__MMI_LANG_OROMO__)
    "ISO8859_1",
#endif

#if defined(__MMI_LANG_IGBO__)
    "ISO8859_1",
#endif

#if defined(__MMI_LANG_KOREAN__)
    "ISO8859_1",
#endif

#if defined(__MMI_LANG_TIBETAN__)
    "ISO8859_1",
#endif

#if defined(__MMI_LANG_ARMENIAN__)
    "ISO8859_1",
#endif

#if defined(__MMI_LANG_GEORGIAN__)
    "ISO8859_1",
#endif

#if defined(__MMI_LANG_KAZAKH__)

    "ISO8859_1",
#endif
};
U8 get_csd_Character_num(void)
{
    return sizeof(CSD_g_Character)/MAX_CHARACTER_SIZE;
}
