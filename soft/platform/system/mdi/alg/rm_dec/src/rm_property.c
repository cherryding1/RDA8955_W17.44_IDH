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



#include "helix_types.h"
//#include "helix_memory.h"
//#include "helix_string.h"
#include "rm_property.h"

const char* rm_property_get_name(rm_property* prop)
{
    const char* pRet = (const char*)HXNULL;

    if (prop)
    {
        pRet = (const char*) prop->pName;
    }

    return pRet;
}

UINT32 rm_property_get_type(rm_property* prop)
{
    UINT32 ulRet = 0;

    if (prop)
    {
        ulRet = prop->ulType;
    }

    return ulRet;
}

UINT32 rm_property_get_value_uint32(rm_property* prop)
{
    UINT32 ulRet = 0;

    if (prop)
    {
        ulRet = (UINT32) prop->pValue;
    }

    return ulRet;
}

const char* rm_property_get_value_cstring(rm_property* prop)
{
    const char* pRet = (const char*)HXNULL;

    if (prop)
    {
        pRet = (const char*) prop->pValue;
    }

    return pRet;
}

UINT32 rm_property_get_value_buffer_length(rm_property* prop)
{
    UINT32 ulRet = 0;

    if (prop)
    {
        ulRet = prop->ulValueLen;
    }

    return ulRet;
}

BYTE* rm_property_get_value_buffer(rm_property* prop)
{
    BYTE* pRet = (BYTE*)HXNULL;

    if (prop)
    {
        pRet = prop->pValue;
    }

    return pRet;
}

void rm_property_cleanup(rm_property* pProp, rm_free_func_ptr fpFree, void* pUserMem)
{
    if (pProp && fpFree)
    {
        /* Cleanup the name */
        if (pProp->pName)
        {
            fpFree(pUserMem, pProp->pName);
        }
        /* Cleanup the value */
        if (pProp->pValue &&
                (pProp->ulType == RM_PROPERTY_TYPE_BUFFER ||
                 pProp->ulType == RM_PROPERTY_TYPE_CSTRING))
        {
            fpFree(pUserMem, pProp->pValue);
        }
        /* Zero out the struct */
        memset(pProp, 0, sizeof(rm_property));
    }
}

void rm_property_set_cleanup(rm_property_set* pSet, rm_free_func_ptr fpFree, void* pUserMem)
{
    UINT32 i = 0;

    if (pSet && fpFree)
    {
        /* Do we have any properties? */
        if (pSet->ulNumProperties && pSet->property)
        {
            /* Clean up the individual properties */
            for (i = 0; i < pSet->ulNumProperties; i++)
            {
                rm_property_cleanup(&pSet->property[i], fpFree, pUserMem);
            }
            /* Clean up the array of properties */
            fpFree(pUserMem, pSet->property);
        }
        /* Zero out the struct */
        memset(pSet, 0, sizeof(rm_property_set));
    }
}

HX_RESULT find_property_in_set(rm_property_set* pSet, const char* pszName, rm_property** ppProp)
{
    HX_RESULT retVal = HXR_FAIL;
    UINT32    i      = 0;

    if (pSet && pszName && ppProp && pSet->ulNumProperties && pSet->property)
    {
        for (i = 0; i < pSet->ulNumProperties; i++)
        {
            if (pSet->property[i].pName &&
                    !strcmp(pSet->property[i].pName, pszName))
            {
                break;
            }
        }
        if (i < pSet->ulNumProperties)
        {
            *ppProp = &pSet->property[i];
            retVal  = HXR_OK;
        }
    }

    return retVal;
}

HXBOOL is_property_present(rm_property_set* pSet, const char* pszName, UINT32* pulType)
{
    HXBOOL       bRet  = FALSE;
    HX_RESULT    rv    = HXR_OK;
    rm_property* pProp = NULL;

    if (pulType)
    {
        rv = find_property_in_set(pSet, pszName, &pProp);
        if (rv == HXR_OK)
        {
            bRet     = TRUE;
            *pulType = pProp->ulType;
        }
    }

    return bRet;
}

HX_RESULT get_rm_property_int(rm_property_set* pSet,
                              const char*      pszStr,
                              UINT32*          pulVal)
{
    HX_RESULT    retVal = HXR_FAIL;
    rm_property* pProp  = NULL;

    if (pulVal)
    {
        retVal = find_property_in_set(pSet, pszStr, &pProp);
        if (retVal == HXR_OK)
        {
            *pulVal = (UINT32) pProp->pValue;
        }
    }

    return retVal;
}

HX_RESULT get_rm_property_buf(rm_property_set* pSet,
                              const char*      pszStr,
                              BYTE**           ppBuf,
                              UINT32*          pulLen)
{
    HX_RESULT    retVal = HXR_FAIL;
    rm_property* pProp  = NULL;

    if (ppBuf && pulLen)
    {
        retVal = find_property_in_set(pSet, pszStr, &pProp);
        if (retVal == HXR_OK)
        {
            *ppBuf  = pProp->pValue;
            *pulLen = pProp->ulValueLen;
        }
    }

    return retVal;
}

HX_RESULT get_rm_property_str(rm_property_set* pSet,
                              const char*      pszStr,
                              char**           ppszStr)
{
    HX_RESULT    retVal = HXR_FAIL;
    rm_property* pProp  = NULL;

    if (ppszStr)
    {
        retVal = find_property_in_set(pSet, pszStr, &pProp);
        if (retVal == HXR_OK)
        {
            *ppszStr = (char*) pProp->pValue;
        }
    }

    return retVal;
}
