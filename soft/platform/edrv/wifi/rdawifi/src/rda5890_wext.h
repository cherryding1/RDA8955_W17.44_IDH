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


#ifndef _RDA5890_WEXT_H_
#define _RDA5890_WEXT_H_

extern struct iw_handler_def rda5890_wext_handler_def;

void rda5890_indicate_disconnected(struct rda5890_private *priv);
void rda5890_indicate_connected(struct rda5890_private *priv);
void rda5890_assoc_done_worker(struct work_struct *work);
void rda5890_assoc_worker(struct work_struct *work);
void rda5890_scan_worker(struct work_struct *work);

#endif
