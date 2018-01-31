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

#ifndef _USBAT_CDC_ID_H_
#define _USBAT_CDC_ID_H_

#ifdef _TO_ADAPT_SPRD_COM_PC_DRIVER_
#define CONFIG_USBD_VENDORID         0x1782  /*sprd  */
#define CONFIG_USBD_PRODUCTID_CDCACM     0x3d00 /* sprd */
#else

#ifdef SUPPORT_SPRD_CDCID
#define CONFIG_USBD_VENDORID         0x1782  /*sprd  */
#define CONFIG_USBD_PRODUCTID_CDCACM     0x3d00 /* sprd */
#else
#define CONFIG_USBD_VENDORID       0x0525  /* Linux/NetChip */
#define CONFIG_USBD_PRODUCTID_CDCACM    0xa4a7  /* CDC ACM */
#endif

#endif
#endif //  


