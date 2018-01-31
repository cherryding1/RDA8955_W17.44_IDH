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

#ifndef __REG_DEFAULT_H__
#define __REG_DEFAULT_H__
#include "cfw_multi_sim.h"
#if(CFW_SIM_NUMBER==1)
#include "csw_cfg_default1.h"
#elif(CFW_SIM_NUMBER==2)
#include "csw_cfg_default2.h"
#elif(CFW_SIM_NUMBER==3)
#include "csw_cfg_default3.h"
#elif(CFW_SIM_NUMBER==4)
#include "csw_cfg_default4.h"
#endif
#endif//__REG_DEFAULT_H__





