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






















#ifndef __EVENT_PRV_H__
#define __EVENT_PRV_H__

#define EV_CFW_INIT_REQ          (EV_CSW_REQ_BASE+1)
#define EV_CFW_XXX_END_          (EV_CSW_REQ_BASE+1)

#if (EV_CFW_XXX_END_ >= (EV_CSW_REQ_END_))
#error 'Error: invalid EV_CFW_XXX_END_ definition!'
#endif


#endif // _H_


