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


#include <cswtype.h>
#include <errorcode.h>
// #include <scl.h>
#include "api_msg.h"
#include <cfw.h>
#include <base.h>
#include <cfw_prv.h>
#include <event.h>
#include <sul.h>
#include <ts.h>
#include "csw_debug.h"

#include <base_prv.h>

#include "cfw_sim.h"
#include "cfw_sim_boot.h"
#include "cfw_sim_prv.h"

#include "cfw_usim.h"
#include "hal_host.h"
#include "sxs_lib.h"

const UINT8 CSW_RID_3G[] = {0xa0, 0x00, 0x00, 0x00, 0x87};
extern CFW_UsimPin gUsimPin[CFW_SIM_COUNT];

/*
================================================================================
Function   : CFW_UiccDecodeDataLength
--------------------------------------------------------------------------------

  Scope      : This function decodes the real length of a TVL object.
                   The length can be stored on 1 or 2 bytes

  Parameters :
   INT8       *dataLgth  :- a pointer to the real length of the TVL object
   INT8       *data      :- a pointer to the raw length data the Appplication ID (AID)
  Return     :
  This function returns the number of bytes the length is stored on (can be 1 or 2 bytes)
================================================================================
*/
UINT8 CFW_UsimDecodeDataLength (UINT8 *dataLgth,  UINT8 *data)
{
    UINT8    lengthSize = 0;

    if ( (*data) <= 0x7F)
    {
        *dataLgth = *data;
        lengthSize = 1;
    }
    else
    {
        *dataLgth = 0;
        if ( (*data) == 0x81)
        {
            if ( *(data + 1) >= 0x80 )
            {
                *dataLgth = *(data + 1);
            }

            lengthSize = 2;
        }
    }

    // returns num of bytes the length is stored on (zero, if data in error)
    return (lengthSize);
}


/*
================================================================================
Function   : CFW_UsimDecFileSize
--------------------------------------------------------------------------------

  Scope      : This function decodes the File size TVL data object (TS 102 221 section 11.1.1.4.1)

  Parameters :
   INT8                   *offset          :- Offset in respData
   UINT8                *respData        :- A pointer to a structure which holds the raw data from the SIM

  Return     :
  This function returns nothing
================================================================================
*/
INT16  CFW_UsimDecFileSize (UINT8  *offset,  UINT8 *respData)
{
    INT16 length;
    UINT8 *ptr = &respData[*offset];    // pointer to TLV object


    if (ptr[1] == 2)   //If the file is an EF, length should always equal 2
    {
        length = (INT16)(COMBINE_TWO_BYTES(ptr[2], ptr[3]));
    }
    else
    {
        length = 0;
    }

    *offset += (2 + ptr[1]);

    return (length);

}



/*
================================================================================
Function   : CFW_UsimDecEfFileDescriptor
--------------------------------------------------------------------------------

  Scope      : This function decodes the File descriptor TVL data object (TS 102 221 section 11.1.1.4.3) inside the FCP template

  Parameters :
   INT8                   *offset          :- Offset in respData
   UINT8                *respData        :- A pointer to a structure which holds the raw data from the SIM
  CFW_UsimEfStatus* pSimEfStatus: EF file status stucture
  Return     :
  This function returns SimUiccDecodeStatus (indicates whether the decoding was successful)
================================================================================
*/
void CFW_UsimDecEfFileDescriptor (UINT8  *offset,  UINT8 *respData, CFW_UsimEfStatus *pSimEfStatus)
{
    UINT8 fileDescLength;
    UINT8 *ptr = &respData[*offset];

    // structure of file
    fileDescLength = ptr[1];


    pSimEfStatus->efStructure = ptr[2]  & 0x07;

    if (fileDescLength == 5)
    {
        pSimEfStatus->recordLength = COMBINE_TWO_BYTES(ptr[4], ptr[5]) ;
        pSimEfStatus->numberOfRecords = ptr[6];
    }
    else
    {
        pSimEfStatus->recordLength = 0;
        pSimEfStatus->numberOfRecords = 0;
    }

    *offset += (2 + ptr[1]);

}



/*
================================================================================
Function   : CFW_UsimDecodeEFFcp
--------------------------------------------------------------------------------

  Scope      : This function decodes the FCP template returned by the SIM after we did a SELECT EF
  Parameters :
   SimDataBlock           *RespData        :- a pointer to the raw FCP template

  Return     :
  NULL
================================================================================
*/

VOID CFW_UsimDecodeEFFcp (UINT8 *RespData, CFW_UsimEfStatus *pSimEfStatus)
{

    UINT8 fcpLength;                   /* can be one or two bytes */
    UINT8 offset = 0;
    UINT8 fcpOffset;
    UINT8 TLVLength, TLVOffset;


    // Decode Fcp template  (see TS 102 221 section 11.1.1.3.1)

    if ( RespData[offset++] == 0x62)          // Fcp template tag
    {

        fcpOffset = CFW_UsimDecodeDataLength (&fcpLength, &RespData[offset]);
        // fcp offset can be 1 or 2
        if (fcpOffset != 0)
        {
            offset += fcpOffset;

            while ( offset <= (fcpLength + fcpOffset)  )
            {

                switch (RespData[offset])
                {
                    // file descriptor   -mandatory
                    case  0x82:
                        CFW_UsimDecEfFileDescriptor (&offset, RespData, pSimEfStatus);
                        break;
                    /*
                                       //   File identifier   -mandatory
                                      case  UICC_FILE_ID_TAG:
                                           decStatus = sim_UiccDecEfId (&offset, RespData);
                                           break;

                                      //      Proprietary info   (TS 102 221 section 11.1.1.4.6)  optional for ef
                                     case  UICC_PROPRIETARY_INFO_TAG:
                                        decStatus = sim_UiccDecProprietaryInfo (&offset, RespData);
                                        break;

                                     //      Life Cycle status Integer - mandatory
                                     case  UICC_LIFE_CYCLE_STATUS_TAG:
                                        decStatus = sim_UiccDecLifeCycleStatus (&offset, RespData);
                                        break;

                                      //   security attributes, compact format (TS 102 221 section 11.1.1.4.7.1)
                                     case  UICC_COMPACT_TAG:
                                         pSimCtx->CurrentEFStatus.format = USIM_COMPACT_FORMAT;
                                        sim_UiccDecEfCompactFormat (&offset, RespData);
                                        break;

                                      //      security attributes - (TS 102 221 section 11.1.1.4.7) - mandatory
                                      // Note: if the ME is not able to decode the security attributes,because they are malformed for example, it should ignore them
                                     case  UICC_EXPANDED_TAG:
                                        pSimCtx->CurrentEFStatus.format = USIM_EXPANDED_FORMAT;
                                        sim_UiccDecEfExpandedFormat (&offset, RespData);
                                        break;

                                      //     security attributes - (TS 102 221 section 11.1.1.4.7) - mandatory
                                     case  UICC_REF_TO_EXPANDED_TAG:
                                        pSimCtx->CurrentEFStatus.format = USIM_REF_TO_EXPANDED_FORMAT;
                                        // Note: if the ME is not able to decode the security attributes, because they are malformed for example, it should ignore them
                                        sim_UiccDecRefToExpandedFormat (&offset, RespData, &pSimCtx->CurrentEFStatus.secAttributes.refToExpandedFormat);
                                        break;
                    */
                    //    Decode File size object (TS 102 221 section 11.1.1.4.1) - Mandatory
                    case 0x80:
                        pSimEfStatus->fileSize = CFW_UsimDecFileSize (&offset, RespData);
                        break;
                    /*
                                     //      Decode Total File size object and Short File Identifier (TS 102 221 section 11.1.1.4.2/8) - Optional
                                     case  UICC_TOTAL_FILE_SIZE_TAG:
                                         sim_UiccDecTotalEfFileSize (&offset, RespData);
                                         break;

                                      //    Short File Identifier
                                     case  UICC_SFI_TAG:
                                         decStatus = sim_UiccDecSfi (&offset, RespData);
                                         break;
                    */
                    default:
                        // Just ignore TLV object we don't know
                        offset++;
                        TLVOffset = CFW_UsimDecodeDataLength (&TLVLength, &RespData[offset]);  // length of tvl object can be coded on1 or 2 bytes
                        offset += TLVOffset + TLVLength;

                        break;
                }

            }
        }
    }

}



/*
================================================================================
Function   : CFW_UsimDecodeEfDirRecord
--------------------------------------------------------------------------------

  Scope      :   This function is used to decode a record in EF DIR
                     Ef DIR contains a list of USIM applications AID

  Parameters :
   SimDataBlock       *dataBlock  :- raw record from EF DIR.  it could be EF_ADN, EF_SNE, etc...
   SimUiccAid           *aid        :- a pointer to a structure representing the Appplication ID (AID)
   SimUiccAidLabel   *label      :- a pointer to a structure holding the application label

  Return     :                  This function returns a decode status SimUiccDecodeStatus
================================================================================
*/
UINT32 CFW_UsimDecodeEfDirRecord (UINT8 *RespData, CFW_UsimAid *aid, CFW_UsimAidLabel *label)
{
    UINT8 dataOffset = 0, templateLength, length;

    label->length = 0;

    hal_HstSendEvent(0x89000000);

    if ( RespData[dataOffset++] == 0x61 )
    {
        templateLength = RespData[dataOffset++];

        while (dataOffset < templateLength)
        {
            switch (RespData[dataOffset])
            {
                case 0x4f:
                    dataOffset++;
                    aid->length = RespData[dataOffset++];
                    /*
                    hal_HstSendEvent(0x90000000 + aid->length);
                    for(int i=0; i<aid->length; i++)
                    hal_HstSendEvent(0x91000000 + RespData[dataOffset+i]);
                    */
                    memcpy (aid->data, &RespData[dataOffset], aid->length);
                    dataOffset += aid->length;

                    if ((aid->length >= (2 + 5)) && (memcmp(&aid->data[0], &CSW_RID_3G[0], sizeof(CSW_RID_3G)) == 0))
                    {
                        label->applicationType =  (UINT16) ((aid->data[5] << 8) | aid->data[6]);
                    }
                    else
                        label->applicationType =  0;

                    break;
                case 0x50:
                {
                    UINT8 tlvObjLength;
                    UINT8 unUsedBytes;
                    dataOffset++;

                    /*label length: At this point, alphaIdSize indicates max number of bytes
                      which are available for use in the EF_DIR file */

                    tlvObjLength = label->length = RespData[dataOffset++];
                    /*
                    hal_HstSendEvent(0x92000000 + aid->length);
                    for(int i=0; i<label->length; i++)
                    hal_HstSendEvent(0x93000000 + RespData[dataOffset+i]);
                    */
                    unUsedBytes = dataOffset;
                    while((RespData[unUsedBytes] == 0xff) && (unUsedBytes <  label->length ))
                    {
                        unUsedBytes++;
                    }
                    if (unUsedBytes == label->length)
                    {
                        /*The bytes aren't actually used: label is empty*/
                        label->length = 0;
                    }
                    else
                    {
                        /*Now get the real alpha ID length */
                        INT16 alphaIdSize = (INT16) label->length & 0x00ff;

                        CFW_GetAdnAlphaIdLength (&RespData[dataOffset],  &alphaIdSize);

                        label->length = (INT8) alphaIdSize & 0xff;
                        if (label->length > 32)
                            label->length = 32;

                        memcpy( label->data,  &RespData[dataOffset],  label->length);
                    }
                    dataOffset += tlvObjLength;
                }
                break;
                default:
                    /*Other DOs may be present as well. See table 13.2 in TS 102 221. Just ignore them*/
                    dataOffset++;
                    length = RespData[dataOffset++];
                    dataOffset += length;
                    break;
            }
        }

        //        hal_HstSendEvent(0x89000001);

        return ERR_SUCCESS;
    }

    return ERR_PARAM_ILLEGAL;
}


/*
** Calculates the length of the alpha identifier data, if any, depending on
** the coding format used on the SIM (i.e. UCS2 or 7bit default alphabet)
** See GSM 11.11 Annex B for possible UCS2 coding schemes.
** Initial value of length is equal to the maximum number of bytes available
** for storing the associated alphaId on the SIM.
*/

/*
================================================================================
Function   : CFW_GetAdnAlphaIdLength
--------------------------------------------------------------------------------

  Scope      :   This function is used to Calculates the length of the alpha identifier data, if any, depending on
                     the coding format used on the SIM (i.e. UCS2 or 7bit default alphabet)
                     See GSM 11.11 Annex B for possible UCS2 coding schemes.
                     Initial value of length is equal to the maximum number of bytes available for storing the associated alphaId on the SIM

  Parameters :
   SimDataBlock       *dataBlock  :- raw record from EF DIR.  it could be EF_ADN, EF_SNE, etc...
   SimUiccAid           *aid        :- a pointer to a structure representing the Appplication ID (AID)
   SimUiccAidLabel   *label      :- a pointer to a structure holding the application label

  Return     :                  This function returns a decode status SimUiccDecodeStatus
================================================================================
*/

VOID CFW_GetAdnAlphaIdLength ( UINT8 *data,    INT16  *length )
{
    UINT8     numUsedBytes = 0;
    UINT8     maxNumAlphaIdBytes = *length;
    bool      endOfAlphaId = FALSE;

    /* check first byte of alpha id info to verify coding scheme used */
    switch ( *data )
    {
        case 0x80:
            /* On the SIM, the length of the alpha ID is limited.
             * On some SIMs, this length will be an even number,
             * on some others it will be an odd number.
             * When the alpha ID is formed of UCS2 characters,
             * the first byte is set to 0x80.
             * Each UCS2 characters occupies 2 bytes.
             * Hence if the max length of the alpha ID is an even number,
             * the last byte will always be unused.
             * so we can decrement maxNumAlphaIdBytes */
            if ((maxNumAlphaIdBytes >= 1) && (maxNumAlphaIdBytes % 2 == 0))
            {
                maxNumAlphaIdBytes--;
                /*The last byte is always unusable...*/
            }

            while ( (numUsedBytes < maxNumAlphaIdBytes) &&
                    (endOfAlphaId == FALSE) )
            {

                /* When the SIM manager spots an 0xff byte in odd position,
                 * it should check whether the following byte is set to 0xff as well.
                 * If the next byte is set to 0xff, then the SIM manager will decide this marks the end of the alpha ID.*/

                if ( (*data == 0xFF) &&
                        (numUsedBytes % 2 != 0) )
                {
                    if (((numUsedBytes + 1) < maxNumAlphaIdBytes) && (*(data + 1) == 0xFF)) /*check if next byte is also set to 0xff*/
                    {
                        endOfAlphaId = TRUE;
                    }
                    else
                    {
                        numUsedBytes++;
                    }
                }
                else
                {
                    numUsedBytes++;
                }
                data++;
            }
            *length = numUsedBytes;
            break;
        case 0x81:
            /*
            ** Alpha Id length = value of octet2  +  3 (for base pointer offset,
            **                                          num chars, and UCS2
            **                                          coding bytes)
            */
            *length = *(data + 1) + 3;
            break;
        case 0x82:
            /*
            ** Alpha Id length = value of octet2  +  4 (for base pointer value,
            **                                          num chars, and UCS2
            **                                          coding bytes)
            */
            *length = *(data + 1) + 4;
            break;
        default:                  /* uses 7bit GSM alphabet, bit8 = 0 */
            while(( *data != 0xff  ) &&
                    ( numUsedBytes < maxNumAlphaIdBytes ) )
            {
                numUsedBytes++;
                data++;
            }
            *length = numUsedBytes;
            break;
    }
}

/*
================================================================================
Function   : CFW_UsimDecodeDFFcp
--------------------------------------------------------------------------------

  Scope      : This function decodes the FCP template returned by the SIM after we did a SELECT DF command
  Parameters :
   SimDataBlock           *RespData        :- a pointer to the raw FCP template

  Return     :
  This function returns a status UsimDecodeStatus
================================================================================
*/
BOOL CFW_UsimDecodeDFFcp ( UINT8 *RespData, CFW_UsimPin *UsimPin)
{

    UINT8 fcpLength;                   // can be one or two bytes
    UINT8 offset = 0;
    UINT8 fcpOffset;
    UINT8 tvlLength, tvlOffset;

    BOOL decStatus = TRUE;

    //    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("(CFW_UsimDecodeDFFcp) RespData[offset++] = 0x%x",0x08100f8c), RespData[offset]);

    // Decode Fcp template  (see TS 102 221 section 11.1.1.3.1)
    if ( RespData[offset++] == 0x62)          // Fcp template tag
    {
        fcpOffset = CFW_UsimDecodeDataLength (&fcpLength, &RespData[offset]);

        //        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("fcpOffset = 0x%x",0x08100f8d), fcpOffset);

        if (fcpOffset != 0)        //fcp offset can be 1 or 2
        {
            offset += fcpOffset;

            //           CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("offset = 0x%x",0x08100f8e), offset);

            while ( offset < (fcpLength + fcpOffset + 1) )
            {

                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(3), TSTR("offset = 0x%x; total length = 0x%x; RespData[offset] = 0x%x",0x08100f8f), offset, (fcpLength + fcpOffset + 1), RespData[offset]);

                switch (RespData[offset])
                {
                    /*
                                        //file descriptor   -mandatory
                                        case  USIM_FILE_DESCRIPTOR_TAG:
                                            decStatus = CFW_UsimDecDirFileDescriptor (&offset, RespData);
                                        break;
                               //    File identifier;  file ID not mandatory for ADF
                                        case  USIM_FILE_ID_TAG:
                                            decStatus = Usim_DecDirId (&offset, RespData);
                                        break;

                                        // add DF name decode for ADF directory decode
                                        case USIM_DF_NAME_TAG:
                                            decStatus = Usim_DecDfName (&offset, RespData);
                                        break;

                                         //     Proprietary info   (TS 102 221 section 11.1.1.4.6)
                                        case  USIM_PROPRIETARY_INFO_TAG:
                                            Usim_DecProprietaryInfo (&offset, RespData);
                                            break;

                                        case  USIM_EXPANDED_TAG:
                                        case  USIM_COMPACT_TAG:
                                        case  USIM_REF_TO_EXPANDED_TAG:
                                            // security attributes,  (TS 102 221 section 11.1.1.4.7)
                                            Usim_DecDirSecurityAttributes (&offset, RespData);
                                            break;
                    */
                    case 0xC6:
                        // Decode PIN status template DO (TS 102 221 section 11.1.1.4.10) - Mandatory
                        CFW_UsimDecPinStatusTemplateDO (&offset, RespData, UsimPin);
                        break;
                    /*
                                        case  USIM_TOTAL_FILE_SIZE_TAG:
                                            // Decode Total File size object and Short File Identifier (TS 102 221 section 11.1.1.4.2/8) - Optional
                                                Usim_DecTotalDirFileSize (&offset, RespData);
                                            break;

                                        case  USIM_LIFE_CYCLE_STATUS_TAG:
                                            //     Life Cycle status Integer - mandatory
                                            decStatus = Usim_DecLifeCycleStatus (&offset, RespData);
                                            break;
                    */
                    default:
                        // Just ignore TLV object we don't know
                        offset++;
                        tvlOffset = CFW_UsimDecodeDataLength (&tvlLength, &RespData[offset]);  //  length of tvl object can be coded on1 or 2 bytes
                        offset += tvlOffset + tvlLength;
                        break;
                }
            }

        }

    }
    else
    {
        decStatus = FALSE;  // not an Fcp template
    }

    return (decStatus);
}


/*
================================================================================
Function   : CFW_UsimDecPinStatusTemplateDO
--------------------------------------------------------------------------------

  Scope      :  This function decodes the PIN status template DO TVL data object (TS 102 221 section 11.1.1.4.10)

  Parameters :
   INT8                   *offset          :- Offset in respData
   UINT8                *respData        :- A pointer to a structure which holds the raw data from the SIM
  Return     :
  This function returns  the DECODE Status UsimDecodeStatus
================================================================================
*/
VOID CFW_UsimDecPinStatusTemplateDO  (UINT8  *offset,  UINT8 *respData, CFW_UsimPin *UsimPin)
{
    UINT8 length, templateLength, psDoLength, startOffset;
    UINT8  pinStatus, keyRefNum = 0;
    BOOL usePin = TRUE;
    BOOL enabled;


    UsimPin->pin1 = CFW_USIM_NO_PIN;
    //   pSimCtx->currentDirStatus.typeOfData = USIM_USIM_DATA;
    UsimPin->usimSpecificData.localPin = CFW_USIM_NO_PIN;

    (*offset)++;

    templateLength = respData[(*offset)++];
    startOffset = *offset;

    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(2), TSTR("templateLength:  0x%x   startOffset:  0x%x",0x08100f90), templateLength,  startOffset);

    if (respData[(*offset)++] == 0x90)
    {
        psDoLength = respData[(*offset)++];  // Length of PS DO can be one or more bytes...
        pinStatus = respData[*offset];       // remember this for later
        (*offset) += psDoLength;

        while (*offset < (templateLength + startOffset))
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("respData[*offset]   0x%x  ",0x08100f91), respData[*offset]);

            switch (respData[*offset])
            {
                case 0x95:
                    // The key reference is preceded by a usage qualifier
                    CFW_UsimUsageQualifier (offset,  respData,  &usePin);
                    break;

                case 0x83:
                    enabled = (BOOL) ((pinStatus >> (7 - keyRefNum)) & 0x01);
                    keyRefNum ++;
                    CFW_UsimDecKeyReference (offset, respData, usePin, enabled, UsimPin);
                    usePin = TRUE;  //by default, set it to TRUE (for next key ref)
                    break;

                default:
                    (*offset)++;
                    length = respData[(*offset)++];
                    (*offset) += length;
                    break;
            }
        }
    }
    else
    {
        (*offset) += templateLength;
    }

}


/*
================================================================================
Function   : CFW_UsimDecPinStatusTemplateDO
--------------------------------------------------------------------------------

  Scope      :  This function decodes the usage qualifier TVL data object (TS 102 221 section 11.1.1.4.10)

  Parameters :
   INT8                   *offset          :- Offset in respData
   UINT8                *respData        :- A pointer to a structure  which holds the raw data from the SIM
   BOOL                *usePin          :-whether or not the PIN is used
  Return     :
  This function returns  the DECODE Status UsimDecodeStatus
================================================================================
*/
VOID CFW_UsimUsageQualifier (UINT8  *offset,  UINT8 *respData, BOOL *usePin)
{
    UINT8 *ptr = &respData[*offset];

    if (ptr[1] == 1)   // length of the usage qualifier data object should always be 1
    {
        *usePin = (BOOL)(ptr[2] == 0x08);
    }

    (*offset) += (2 + ptr[1] );
}



/*
================================================================================
Function   : CFW_UsimDecKeyReference
--------------------------------------------------------------------------------

  Scope      :  This function decodes the usage qualifier TVL data object (TS 102 221 section 11.1.1.4.10)

  Parameters :
   INT8                   *offset          :- Offset in respData
   UINT8                *respData        :- A pointer to a structure  which holds the raw data from the SIM
   BOOL                *usePin          :-whether or not the PIN is used
  Return     :
  This function returns  the DECODE Status UsimDecodeStatus
================================================================================
*/
VOID CFW_UsimDecKeyReference (UINT8  *offset,  UINT8 *respData, BOOL usePin, BOOL enabled, CFW_UsimPin *UsimPin)
{
    UINT8 *ptr = &respData[*offset];

    if (ptr[1] == 1)   // length of the key reference data object should always be 1
    {
        CFW_UsimSetPinStatus(ptr[2], usePin,  enabled, UsimPin);
    }

    (*offset) += (2 + ptr[1] );
}


/*
================================================================================
Function   : CFW_UsimSetPinStatus
--------------------------------------------------------------------------------

  Scope      :  This function decodes the usage qualifier TVL data object (TS 102 221 section 11.1.1.4.10)

  Parameters :
   BOOL                *usePin          :-whether or not the PIN is used
  Return     :
  This function returns  the DECODE Status UsimDecodeStatus
================================================================================
*/
void CFW_UsimSetPinStatus (UINT8 keyReference, BOOL  usePin,  BOOL  enabled, CFW_UsimPin *UsimPin)
{

    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("keyReference   0x%x  ",0x08100f92), keyReference);

    if ((keyReference & 0x0f) != CFW_USIM_NO_PIN)
    {
        if ( keyReference == CFW_USIM_UNIVERSAL_PIN) //Universal PIN
        {
            /* The status of the universal PIN is stored in first position */
            UsimPin->UniversalPinSupportedByCard = TRUE;
            UsimPin->UniversalPin.enabled = enabled;
            UsimPin->UniversalPin.used = usePin;
        }
        else  if (( keyReference & 0xf0) == 0x00)  // level 1 PIN
        {
            // if key reference > MAX_NUM_PIN_APP, then it is an ADM
            if (keyReference <= 8)
            {
                UsimPin->pin1 = (CFW_UsimKeyRefValue) keyReference;
                UsimPin->pinAppStatus[keyReference - 1].enabled = enabled;
                UsimPin->pinAppStatus[keyReference - 1].used = usePin;
                UsimPin->pinAppStatus[keyReference - 1].secretCodeInitialised = TRUE;
                UsimPin->usimSpecificData.pinStatus = UsimPin->pinAppStatus[keyReference - 1];
                UsimPin->usimSpecificData.applicationPin = UsimPin->pin1;
            }
        }
        else if  ((keyReference & 0xf0) == 0x80)  // level 2 PIN
        {
            INT8 idx = keyReference & 0x0f;

            if (idx <= 8)
            {
                UsimPin->usimSpecificData.localPin = (CFW_UsimKeyRefValue) keyReference;
                UsimPin->usimSpecificData.pin2Status.enabled = enabled;
                UsimPin->usimSpecificData.pin2Status.used = usePin;
                UsimPin->usimSpecificData.pin2Status.secretCodeInitialised = TRUE;
            }
        }
    }
}


UINT32 CFW_CfgUSimGetPinParam(CFW_UsimPin **pUSimPinParam, CFW_SIM_ID nSimID)
{
    UINT32 nRet = ERR_SUCCESS;

    nRet = CFW_CheckSimId(nSimID);
    if(ERR_SUCCESS == nRet)
    {
        *pUSimPinParam = &(gUsimPin[nSimID]);
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("CFW_CfgUSimGetPinParam, error ret =0x%x\n",0x08100f93), nRet);
    }
    return nRet;
}


/*
================================================================================
Function   : CFW_USimInitPinStatus
--------------------------------------------------------------------------------

  Scope      :  This function set the initial value of PIN and PUK

  Parameters :
   api_SimVerifyCHVCnf_t   *pSimVerifyCHVCnf      : the status of the sim confirmation
   SIM_CHV_PARAM            *pG_Chv_Param          : CHV parameter
   CFW_UsimPin                *UsimPin         : PIN  status of the sim card
   CFW_SIM_ID                  nSimID          : SIM ID

  Return     :
  This function returns  the remain times of PIN1/2 and PUK1/2
================================================================================
*/
UINT32 CFW_USimInitPinStatus(api_SimVerifyCHVCnf_t *pSimVerifyCHVCnf, SIM_CHV_PARAM *pG_Chv_Param, CFW_UsimPin *UsimPin, CFW_SIM_ID nSimID)
{

    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(3), TSTR("(CFW_USimInitPinStatus: %d) UsimPin->usimSpecificData.pinStatus.enabled  / used: %d / %d",0x08100f94), nSimID, UsimPin->usimSpecificData.pinStatus.enabled, UsimPin->usimSpecificData.pinStatus.used);

    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(4), TSTR("1Times: %d-%d-%d-%d",0x08100f95), pG_Chv_Param->nPin1Remain, pG_Chv_Param->nPuk1Remain, pG_Chv_Param->nPin2Remain, pG_Chv_Param->nPuk2Remain);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(4), TSTR("Status: Pin1/Pin2/Puk1/Puk2:  %d-%d-%d-%d",0x08100f96),
              pG_Chv_Param->nPin1Status, pG_Chv_Param->nPin2Status, pG_Chv_Param->nPuk1Status, pG_Chv_Param->nPuk2Status);

    pG_Chv_Param->nPin1Max  = 3;
    pG_Chv_Param->nPuk1Max = 10;
    pG_Chv_Param->nPin2Max  = 3;
    pG_Chv_Param->nPuk2Max = 10;

    if ((0x63 == pSimVerifyCHVCnf->Sw1) || (0x69 == pSimVerifyCHVCnf->Sw1))
    {
        if((pSimVerifyCHVCnf->Sw2 <= 0xcf) && (pSimVerifyCHVCnf->Sw2 >= 0xc1))
        {
            pG_Chv_Param->nPin1Remain  = pSimVerifyCHVCnf->Sw2 & 0x0f;
            pG_Chv_Param->nPuk1Remain = 10;
            pG_Chv_Param->nPin2Remain  = 3;
            pG_Chv_Param->nPuk2Remain = 10;

            if(pG_Chv_Param->nPin1Remain > pG_Chv_Param->nPin1Max)
                pG_Chv_Param->nPin1Max = pG_Chv_Param->nPin1Remain;
        }
        else if((pSimVerifyCHVCnf->Sw2 == 0xc0) || ((pSimVerifyCHVCnf->Sw1 == 0x69) && (pSimVerifyCHVCnf->Sw2 == 0x83)))
        {
            pG_Chv_Param->nPin1Remain  = 0;
            pG_Chv_Param->nPuk1Remain = 10;
            pG_Chv_Param->nPin2Remain  = 3;
            pG_Chv_Param->nPuk2Remain = 10;
        }
        else
        {
            return 0;
        }

        if((UsimPin->usimSpecificData.pinStatus.enabled == FALSE) || (UsimPin->usimSpecificData.pinStatus.used == FALSE))
        {
            pG_Chv_Param->nPin1Status = CHV_DISABLE;
            pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN1DISABLE;
        }
        else
        {
            if((pG_Chv_Param->nPin1Status == CHV_UNINIT) || (pG_Chv_Param->nPin1Status == CHV_UNKNOW))
                pG_Chv_Param->nPin1Status = CHV_NOVERIFY;
        }

        if((UsimPin->usimSpecificData.pin2Status.enabled == FALSE) || (UsimPin->usimSpecificData.pin2Status.used == FALSE))
        {
            pG_Chv_Param->nPin2Status = CHV_DISABLE;
            pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN1DISABLE;
        }
        else
        {
            if((pG_Chv_Param->nPin2Status == CHV_UNINIT) || (pG_Chv_Param->nPin2Status == CHV_UNKNOW))
                pG_Chv_Param->nPin2Status = CHV_NOVERIFY;
        }

        pG_Chv_Param->nPuk1Status = CHV_INIT;
        pG_Chv_Param->nPuk2Status = CHV_INIT;
    }
    else
    {
        return 0;
    }

    if (pG_Chv_Param->nPin1Status == CHV_NOVERIFY)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("USIM nPin1Status: CHV_NOVERIFY, nPuk1Status: %d\n",0x08100f97), pG_Chv_Param->nPuk1Status);
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("USIM nPuk1Remain: %d\n",0x08100f98), pG_Chv_Param->nPuk1Remain);
        if (pG_Chv_Param->nPuk1Remain == 0)
            pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN1BLOCK;  // PIN1 block.
        else if (pG_Chv_Param->nPin1Remain == 0)
            pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PUK1; // need PUK1.
        else
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(0), TSTR("USIM Need input PIN1\n",0x08100f99));
            pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN1; // need PIN1
        }
    }
    else if (pG_Chv_Param->nPin1Status == CHV_VERIFY)
    {
        // This process to report puk1 status after power on
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("USIM nPin1Status: CHV_VERIFY, nPuk1Remain: %d\n",0x08100f9a), pG_Chv_Param->nPuk1Remain);
        if (pG_Chv_Param->nPuk1Remain == 0)
            pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN1BLOCK;  // PIN1 block.
        else if (pG_Chv_Param->nPin1Remain == 0)
            pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PUK1; // need PUK1.
        else
            pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN1READY;  // PIN1 ready.
    }
    else if (pG_Chv_Param->nPin1Status == CHV_DISABLE)
    {
        // This process to report puk1 status after power on
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("USIM nPin1Status: CHV_DISABLE, nPuk1Remain: %d\n",0x08100f9b), pG_Chv_Param->nPuk1Remain);

        if (pG_Chv_Param->nPuk1Remain == 0)
            pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN1BLOCK;  // PIN1 block.
        else if (pG_Chv_Param->nPin1Remain == 0)
            pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PUK1; // need PUK1.
        else
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(0), TSTR("USIM PIN1 Disable\n",0x08100f9c));
            pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN1DISABLE;  // PIN1 disable.
        }
    }

    if (((pG_Chv_Param->nPin1Status == CHV_VERIFY) || (pG_Chv_Param->nPin1Status == CHV_DISABLE))
            && (pG_Chv_Param->bCHV2VerifyReq == TRUE))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(3), TSTR("USIM nPuk2Status: %d, nPin2Remain: %d nPuk1Remain: %d\n",0x08100f9d), pG_Chv_Param->nPuk2Status,
                  pG_Chv_Param->nPin2Remain, pG_Chv_Param->nPuk2Remain);

        if (pG_Chv_Param->nPuk2Remain == 0)
            pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN2BLOCK;  // PIN2 block.
        else if (pG_Chv_Param->nPin2Remain == 0)
            pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PUK2; // need PUK2.
        else
            pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN2; // need PIN2

    }


    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(4), TSTR("nPuk2Remain %d nPin2Remain %d nPuk1Remain %d nPin1Remain %d\n",0x08100f9e),
              pG_Chv_Param->nPuk2Remain, pG_Chv_Param->nPin2Remain, pG_Chv_Param->nPuk1Remain, pG_Chv_Param->nPin1Remain);

    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(4), TSTR("Status: Pin1/Pin2/Puk1/Puk2:  %d-%d-%d-%d",0x08100f9f),
              pG_Chv_Param->nPin1Status, pG_Chv_Param->nPin2Status, pG_Chv_Param->nPuk1Status, pG_Chv_Param->nPuk2Status);

    return (((UINT16)pG_Chv_Param->nPuk2Remain << 12) | ((UINT16)pG_Chv_Param->nPin2Remain << 8) | (pG_Chv_Param->nPuk1Remain << 4) | pG_Chv_Param->nPin1Remain);
}


/*
================================================================================
Function   : CFW_USimInitPINStatus
--------------------------------------------------------------------------------

  Scope      :  This function get the value of PIN and PUK

  Parameters :
   api_SimVerifyCHVCnf_t   *pSimVerifyCHVCnf      : the status of the sim confirmation
   SIM_CHV_PARAM            *pG_Chv_Param          : CHV parameter
   UINT8                            *PinFlag         : 1: PIN1; 2: PIN2;
   CFW_UsimPin                *UsimPin         : PIN  status of the sim card
   CFW_SIM_ID                  nSimID          : SIM ID

  Return     :
  This function returns  the remain times of PIN1/2 and PUK1/2
================================================================================
*/
UINT32 CFW_USimParsePinStatus(api_SimVerifyCHVCnf_t *pSimVerifyCHVCnf, SIM_CHV_PARAM *pG_Chv_Param, UINT8 PinFlag, CFW_SIM_ID nSimID)
{

    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(2), TSTR("(CFW_USimParsePinStatus: %d) PinFlag: %d ",0x08100fa0), nSimID, PinFlag);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(4), TSTR("1Times: %d-%d-%d-%d",0x08100fa1), pG_Chv_Param->nPin1Remain, pG_Chv_Param->nPuk1Remain, pG_Chv_Param->nPin2Remain, pG_Chv_Param->nPuk2Remain);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(4), TSTR("Status: Pin1/Pin2/Puk1/Puk2:  %d-%d-%d-%d",0x08100fa2),
              pG_Chv_Param->nPin1Status, pG_Chv_Param->nPin2Status, pG_Chv_Param->nPuk1Status, pG_Chv_Param->nPuk2Status);

    if ((0x63 == pSimVerifyCHVCnf->Sw1) || (0x69 == pSimVerifyCHVCnf->Sw1))
    {
        if(PinFlag == 1) // PIN1
        {
            if((pSimVerifyCHVCnf->Sw2 <= 0xcf) && (pSimVerifyCHVCnf->Sw2 >= 0xc1))
            {
                pG_Chv_Param->nPin1Remain  = pSimVerifyCHVCnf->Sw2 & 0x0f;

                if(pG_Chv_Param->nPin1Remain > pG_Chv_Param->nPin1Max)
                    pG_Chv_Param->nPin1Max = pG_Chv_Param->nPin1Remain;
            }
            else if((pSimVerifyCHVCnf->Sw2 == 0xc0) || ((pSimVerifyCHVCnf->Sw1 == 0x69) && (pSimVerifyCHVCnf->Sw2 == 0x83)))
            {
                pG_Chv_Param->nPin1Remain  = 0;
            }
            else
            {
                return 0;
            }
        }
        else  if(PinFlag == 2) // PIN2
        {
            if((pSimVerifyCHVCnf->Sw2 <= 0xcf) && (pSimVerifyCHVCnf->Sw2 >= 0xc1))
            {
                pG_Chv_Param->nPin2Remain  = pSimVerifyCHVCnf->Sw2 & 0x0f;

                if(pG_Chv_Param->nPin2Remain > pG_Chv_Param->nPin2Max)
                    pG_Chv_Param->nPin2Max = pG_Chv_Param->nPin2Remain;
            }
            else if((pSimVerifyCHVCnf->Sw2 == 0xc0) || ((pSimVerifyCHVCnf->Sw1 == 0x69) && (pSimVerifyCHVCnf->Sw2 == 0x83)))
            {
                pG_Chv_Param->nPin2Remain  = 0;
            }
            else
            {
                return 0;
            }
        }
        else
            return 0;
    }
    else
    {
        return 0;
    }

    if (pG_Chv_Param->nPin1Status == CHV_NOVERIFY)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("USIM nPin1Status: CHV_NOVERIFY, nPuk1Status: %d\n",0x08100fa3), pG_Chv_Param->nPuk1Status);
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("USIM nPuk1Remain: %d\n",0x08100fa4), pG_Chv_Param->nPuk1Remain);
        if (pG_Chv_Param->nPuk1Remain == 0)
            pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN1BLOCK;  // PIN1 block.
        else if (pG_Chv_Param->nPin1Remain == 0)
            pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PUK1; // need PUK1.
        else
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(0), TSTR("USIM Need input PIN1\n",0x08100fa5));
            pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN1; // need PIN1
        }
    }
    else if (pG_Chv_Param->nPin1Status == CHV_VERIFY)
    {
        // This process to report puk1 status after power on
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("USIM nPin1Status: CHV_VERIFY, nPuk1Remain: %d\n",0x08100fa6), pG_Chv_Param->nPuk1Remain);
        if (pG_Chv_Param->nPuk1Remain == 0)
            pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN1BLOCK;  // PIN1 block.
        else if (pG_Chv_Param->nPin1Remain == 0)
            pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PUK1; // need PUK1.
        else
            pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN1READY;  // PIN1 ready.
    }
    else if (pG_Chv_Param->nPin1Status == CHV_DISABLE)
    {
        // This process to report puk1 status after power on
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("USIM nPin1Status: CHV_DISABLE, nPuk1Remain: %d\n",0x08100fa7), pG_Chv_Param->nPuk1Remain);

        if (pG_Chv_Param->nPuk1Remain == 0)
            pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN1BLOCK;  // PIN1 block.
        else if (pG_Chv_Param->nPin1Remain == 0)
            pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PUK1; // need PUK1.
        else
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(0), TSTR("USIM PIN1 Disable\n",0x08100fa8));
            pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN1DISABLE;  // PIN1 disable.
        }
    }

    if (((pG_Chv_Param->nPin1Status == CHV_VERIFY) || (pG_Chv_Param->nPin1Status == CHV_DISABLE))
            && (pG_Chv_Param->bCHV2VerifyReq == TRUE))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(3), TSTR("USIM nPuk2Status: %d, nPin2Remain: %d nPuk1Remain: %d\n",0x08100fa9), pG_Chv_Param->nPuk2Status,
                  pG_Chv_Param->nPin2Remain, pG_Chv_Param->nPuk2Remain);

        if (pG_Chv_Param->nPuk2Remain == 0)
            pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN2BLOCK;  // PIN2 block.
        else if (pG_Chv_Param->nPin2Remain == 0)
            pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PUK2; // need PUK2.
        else
            pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN2; // need PIN2

    }


    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(4), TSTR("2Times: %d-%d-%d-%d",0x08100faa), pG_Chv_Param->nPin1Remain, pG_Chv_Param->nPuk1Remain, pG_Chv_Param->nPin2Remain, pG_Chv_Param->nPuk2Remain);
    return (((UINT16)pG_Chv_Param->nPuk2Remain << 12) | ((UINT16)pG_Chv_Param->nPin2Remain << 8) | (pG_Chv_Param->nPuk1Remain << 4) | pG_Chv_Param->nPin1Remain);
}


/*
================================================================================
Function   : CFW_USimInitPINStatus
--------------------------------------------------------------------------------

  Scope      :  This function get the value of PIN and PUK

  Parameters :
   api_SimVerifyCHVCnf_t   *pSimVerifyCHVCnf      : the status of the sim confirmation
   SIM_CHV_PARAM            *pG_Chv_Param          : CHV parameter
   UINT8                            *PukFlag         : 1: PUK1; 2: PUK2;
   CFW_UsimPin                *UsimPin         : PIN  status of the sim card
   CFW_SIM_ID                  nSimID          : SIM ID

  Return     :
  This function returns  the remain times of PIN1/2 and PUK1/2
================================================================================
*/
UINT32 CFW_USimParsePukStatus(api_SimVerifyCHVCnf_t *pSimVerifyCHVCnf, SIM_CHV_PARAM *pG_Chv_Param, UINT8 PukFlag, CFW_SIM_ID nSimID)
{
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(6), TSTR("(CFW_USimParsePukStatus: %d %d) Status: Pin1/Pin2/Puk1/Puk2:  %d-%d-%d-%d",0x08100fab), nSimID, PukFlag,
              pG_Chv_Param->nPin1Status, pG_Chv_Param->nPin2Status, pG_Chv_Param->nPuk1Status, pG_Chv_Param->nPuk2Status);

    if ((0x63 == pSimVerifyCHVCnf->Sw1) || (0x69 == pSimVerifyCHVCnf->Sw1))
    {
        if(PukFlag == 1) // PUK1
        {
            if((pSimVerifyCHVCnf->Sw2 <= 0xca) && (pSimVerifyCHVCnf->Sw2 >= 0xc1))
            {
                pG_Chv_Param->nPuk1Remain = pSimVerifyCHVCnf->Sw2 & 0x0f;

                if(pG_Chv_Param->nPuk1Remain > pG_Chv_Param->nPuk1Max)
                    pG_Chv_Param->nPuk1Max = pG_Chv_Param->nPuk1Remain;
            }
            else if((pSimVerifyCHVCnf->Sw2 == 0xc0) || ((pSimVerifyCHVCnf->Sw1 == 0x69) && (pSimVerifyCHVCnf->Sw2 == 0x83)))
            {
                pG_Chv_Param->nPuk1Remain = 0;
            }
            else
            {
                return 0;
            }
        }
        else if(PukFlag == 2) // PUK2
        {
            if((pSimVerifyCHVCnf->Sw2 <= 0xca) && (pSimVerifyCHVCnf->Sw2 >= 0xc1))
            {
                pG_Chv_Param->nPuk2Remain = pSimVerifyCHVCnf->Sw2 & 0x0f;

                if(pG_Chv_Param->nPuk2Remain > pG_Chv_Param->nPuk2Max)
                    pG_Chv_Param->nPuk2Max = pG_Chv_Param->nPuk2Remain;
            }
            else if((pSimVerifyCHVCnf->Sw2 == 0xc0) || ((pSimVerifyCHVCnf->Sw1 == 0x69) && (pSimVerifyCHVCnf->Sw2 == 0x83)))
            {
                pG_Chv_Param->nPuk2Remain = 0;
            }
            else
            {
                return 0;
            }
        }
        else
            return 0;
    }
    else
    {
        return 0;
    }


    if (pG_Chv_Param->nPuk1Status == CHV_UNKNOW)
        pG_Chv_Param->nPuk1Status = CHV_INIT;

    if (pG_Chv_Param->nPuk2Status == CHV_UNKNOW)
        pG_Chv_Param->nPuk2Status = CHV_INIT;


    if(PukFlag == 1)
    {
        if (pG_Chv_Param->nPin1Status == CHV_NOVERIFY)
        {
            if (pG_Chv_Param->nPuk1Remain == 0)
                pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN1BLOCK;  // PIN1 block.
            else if (pG_Chv_Param->nPin1Remain == 0)
                pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PUK1; // need PUK1.
            else
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(0), TSTR("USIM Need input PIN1\n",0x08100fac));
                pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN1; // need PIN1
            }
        }
        else if (pG_Chv_Param->nPin1Status == CHV_VERIFY)
        {
            // This process to report puk1 status after power on
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("1USIM nPin1Status: CHV_VERIFY, nPuk1Remain: %d\n",0x08100fad), pG_Chv_Param->nPuk1Remain);
            if (pG_Chv_Param->nPuk1Remain == 0)
                pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN1BLOCK;  // PIN1 block.
            else if (pG_Chv_Param->nPin1Remain == 0)
                pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PUK1; // need PUK1.
            else
                pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN1READY;  // PIN1 ready.
        }
        else if (pG_Chv_Param->nPin1Status == CHV_DISABLE)
        {
            // This process to report puk1 status after power on
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("2USIM nPin1Status: CHV_DISABLE, nPuk1Remain: %d\n",0x08100fae), pG_Chv_Param->nPuk1Remain);

            if (pG_Chv_Param->nPuk1Remain == 0)
                pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN1BLOCK;  // PIN1 block.
            else if (pG_Chv_Param->nPin1Remain == 0)
                pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PUK1; // need PUK1.
            else
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(0), TSTR("USIM PIN1 Disable\n",0x08100faf));
                pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN1DISABLE;  // PIN1 disable.
            }
        }
    }
    else if(PukFlag == 2)
    {
        if (((pG_Chv_Param->nPin1Status == CHV_VERIFY) || (pG_Chv_Param->nPin1Status == CHV_DISABLE))
                && (pG_Chv_Param->bCHV2VerifyReq == TRUE))
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(3), TSTR("USIM nPuk2Status: %d, nPin2Remain: %d nPuk1Remain: %d\n",0x08100fb0), pG_Chv_Param->nPuk2Status,
                      pG_Chv_Param->nPin2Remain, pG_Chv_Param->nPuk2Remain);

            if (pG_Chv_Param->nPuk2Remain == 0)
                pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN2BLOCK;  // PIN2 block.
            else if (pG_Chv_Param->nPin2Remain == 0)
                pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PUK2; // need PUK2.
            else
                pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN2; // need PIN2
        }
    }


    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(4), TSTR("nPuk2Remain %d nPin2Remain %d nPuk1Remain %d nPin1Remain %d\n",0x08100fb1),
              pG_Chv_Param->nPuk2Remain, pG_Chv_Param->nPin2Remain, pG_Chv_Param->nPuk1Remain, pG_Chv_Param->nPin1Remain);

    return (((UINT16)pG_Chv_Param->nPuk2Remain << 12) | ((UINT16)pG_Chv_Param->nPin2Remain << 8) | (pG_Chv_Param->nPuk1Remain << 4) | pG_Chv_Param->nPin1Remain);
}

