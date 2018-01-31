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
























#ifndef RF_NAMES_H
#define RF_NAMES_H

#include "calib_m.h"



/* -------------------------------------------------- */
/* ----- XCV identifiers. */

enum
{
    DEFAULT_CALIB_XCV_OBSOLETE_0 = CALIB_XCV_MASK,
    DEFAULT_CALIB_XCV_OBSOLETE_1,
    DEFAULT_CALIB_XCV_OBSOLETE_2,
    DEFAULT_CALIB_XCV_OBSOLETE_3,
    DEFAULT_CALIB_XCV_OBSOLETE_4,
    DEFAULT_CALIB_XCV_OBSOLETE_5,
    DEFAULT_CALIB_XCV_RDA6220_GALLITE,
    DEFAULT_CALIB_XCV_RDA6220_GALLITE_CT1129,
    DEFAULT_CALIB_XCV_8808,
    DEFAULT_CALIB_XCV_8809,
    DEFAULT_CALIB_XCV_RDA6220E,
    DEFAULT_CALIB_XCV_8810,
    DEFAULT_CALIB_XCV_8809P,
    DEFAULT_CALIB_XCV_8850,
    DEFAULT_CALIB_XCV_8809E2,
    DEFAULT_CALIB_XCV_8955,
    DEFAULT_CALIB_XCV_QTY
};

/* -------------------------------------------------- */
/* ----- XCV names. */

#define DEFAULT_CALIB_XCV_NAMES_STR     "XCV Obsolete 0", \
                                        "XCV Obsolete 1", \
                                        "XCV Obsolete 2", \
                                        "XCV Obsolete 3", \
                                        "XCV Obsolete 4", \
                                        "XCV Obsolete 5", \
                                        "XCV RDA 6220 Gallite", \
                                        "XCV RDA 6220 Gallite CT1129", \
                                        "XCV 8808", \
                                        "XCV 8809", \
                                        "XCV RDA 6220E", \
                                        "XCV 8810", \
                                        "XCV 8809P", \
                                        "XCV 8850", \
                                        "XCV 8809E2", \
                                        "XCV 8955"

/* -------------------------------------------------- */
/* ----- PA identifiers. */

enum
{
    DEFAULT_CALIB_PA_OBSOLETE_0 = CALIB_PA_MASK,
    DEFAULT_CALIB_PA_OBSOLETE_1,
    DEFAULT_CALIB_PA_OBSOLETE_2,
    DEFAULT_CALIB_PA_OBSOLETE_3,
    DEFAULT_CALIB_PA_OBSOLETE_4,
    DEFAULT_CALIB_PA_OBSOLETE_5,
    DEFAULT_CALIB_PA_OBSOLETE_6,
    DEFAULT_CALIB_PA_OBSOLETE_7,
    DEFAULT_CALIB_PA_RDA6625,
    DEFAULT_CALIB_PA_RDA6231,
    DEFAULT_CALIB_PA_8809P,
    DEFAULT_CALIB_PA_HS8292U,
    DEFAULT_CALIB_PA_DUMMY,
    DEFAULT_CALIB_PA_QTY
};

/* -------------------------------------------------- */
/* ----- PA names. */

#define DEFAULT_CALIB_PA_NAMES_STR      "PA Obsolete 0", \
                                        "PA Obsolete 1", \
                                        "PA Obsolete 2", \
                                        "PA Obsolete 3", \
                                        "PA Obsolete 4", \
                                        "PA Obsolete 5", \
                                        "PA Obsolete 6", \
                                        "PA Obsolete 7", \
                                        "PA RDA 6625", \
                                        "PA RDA 6231", \
                                        "PA RDA 8809P", \
                                        "PA RDA HS8292U", \
                                        "Dummy PA"

/* -------------------------------------------------- */
/* ----- SW identifiers. */

enum
{
    DEFAULT_CALIB_SW_OBSOLETE_0 = CALIB_SW_MASK,
    DEFAULT_CALIB_SW_OBSOLETE_1,
    DEFAULT_CALIB_SW_OBSOLETE_2,
    DEFAULT_CALIB_SW_OBSOLETE_3,
    DEFAULT_CALIB_SW_OBSOLETE_4,
    DEFAULT_CALIB_SW_OBSOLETE_5,
    DEFAULT_CALIB_SW_OBSOLETE_6,
    DEFAULT_CALIB_SW_OBSOLETE_7,
    DEFAULT_CALIB_SW_OBSOLETE_8,
    DEFAULT_CALIB_SW_OBSOLETE_9,
    DEFAULT_CALIB_SW_RDA6625,
    DEFAULT_CALIB_SW_RDA6231,
    DEFAULT_CALIB_SW_8809P,
    DEFAULT_CALIB_SW_HS8292U,
    DEFAULT_CALIB_SW_DUMMY,
    DEFAULT_CALIB_SW_QTY
};

/* -------------------------------------------------- */
/* ----- SW names. */

#define DEFAULT_CALIB_SW_NAMES_STR      "SW Obsolete 0", \
                                        "SW Obsolete 1", \
                                        "SW Obsolete 2", \
                                        "SW Obsolete 3", \
                                        "SW Obsolete 4", \
                                        "SW Obsolete 5", \
                                        "SW Obsolete 6", \
                                        "SW Obsolete 7", \
                                        "SW Obsolete 8", \
                                        "SW Obsolete 9", \
                                        "SW RDA 6625", \
                                        "SW RDA 6231", \
                                        "SW RDA 8809P", \
                                        "SW RDA HS8292U", \
                                        "Dummy SW"


#endif /* RF_NAMES_H */
