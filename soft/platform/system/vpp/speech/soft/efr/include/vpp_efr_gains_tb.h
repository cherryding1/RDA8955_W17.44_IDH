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



#define NB_QUA_PITCH 16

static const INT16 qua_gain_pitch[NB_QUA_PITCH] =
{
    0, 3277, 6556, 8192, 9830, 11469, 12288, 13107,
    13926, 14746, 15565, 16384, 17203, 18022, 18842, 19661
};

#define NB_QUA_CODE 32

static const INT16 qua_gain_code[NB_QUA_CODE] =
{
    159, 206, 268, 349, 419, 482, 554, 637,
    733, 842, 969, 1114, 1281, 1473, 1694, 1948,
    2241, 2577, 2963, 3408, 3919, 4507, 5183, 5960,
    6855, 7883, 9065, 10425, 12510, 16263, 21142, 27485
};
