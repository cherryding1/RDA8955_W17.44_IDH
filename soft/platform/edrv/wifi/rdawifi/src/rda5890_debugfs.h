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

#ifndef _RDA5890_DEBUGFS_H_
#define _RDA5890_DEBUGFS_H_

void rda5890_debugfs_init(void);
void rda5890_debugfs_remove(void);

void rda5890_debugfs_init_one(struct rda5890_private *priv);
void rda5890_debugfs_remove_one(struct rda5890_private *priv);

/* This is for SDIO testing */
void rda5890_sdio_test_card_to_host(char *buf, unsigned short len);

#endif
