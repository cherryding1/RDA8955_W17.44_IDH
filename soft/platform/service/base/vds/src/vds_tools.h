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

#ifndef _VDS_TOOLS_H_
#define _VDS_TOOLS_H_

BOOL vds_GetLineFromStr( char *line, int n, char *str, int *offset );
UINT32 vds_CRC32( CONST VOID *pvData, UINT32 iLen );
UINT32 vds_GetPBDCRC( VDS_PBD *psPBD );
INT32 vds_SetPBDCRC( VDS_PARTITION *psCP, UINT16 iSN, UINT16 iPBN, UINT32 iCRC );
BOOL vds_IsCRCChecked( UINT8 *pBuff, UINT32 iLen, UINT32 iCRC, UINT32 iMode );


void vds_str_to_hex( char *str, long *len, char *buf );
void vds_hex_to_str( char *hex, long len, char *buf, char separator );


#endif // _VDS_TOOLS_H_




