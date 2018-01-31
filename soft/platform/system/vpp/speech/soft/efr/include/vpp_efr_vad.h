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




struct _fp
{
    INT16 e;
    INT16 m;
};

typedef struct _fp Pfloat;

void vad_reset (void);

INT16 vad_computation (
    INT16 r_h[],
    INT16 r_l[],
    INT16 scal_acf,
    INT16 rc[],
    INT16 ptch
);

void energy_computation (
    INT16 r_h[],
    INT16 scal_acf,
    INT16 rvad[],
    INT16 scal_rvad,
    Pfloat * acf0,
    Pfloat * pvad
);

void acf_averaging (
    INT16 r_h[],
    INT16 r_l[],
    INT16 scal_acf,
    INT32 L_av0[],
    INT32 L_av1[]
);

void predictor_values (
    INT32 L_av1[],
    INT16 rav1[],
    INT16 *scal_rav1
);

void schur_recursion (
    INT32 L_av1[],
    INT16 vpar[]
);

void step_up (
    INT16 np,
    INT16 vpar[],
    INT16 aav1[]
);

void compute_rav1 (
    INT16 aav1[],
    INT16 rav1[],
    INT16 *scal_rav1
);

INT16 spectral_comparison (
    INT16 rav1[],
    INT16 scal_rav1,
    INT32 L_av0[]
);

void threshold_adaptation (
    INT16 stat,
    INT16 ptch,
    INT16 tone,
    INT16 rav1[],
    INT16 scal_rav1,
    Pfloat pvad,
    Pfloat acf0,
    INT16 rvad[],
    INT16 *scal_rvad,
    Pfloat * thvad
);

void tone_detection (
    INT16 rc[],
    INT16 *tone
);

INT16 vad_decision (
    Pfloat pvad,
    Pfloat thvad
);

INT16 vad_hangover (
    INT16 vvad
);

void periodicity_update (
    INT16 lags[],
    INT16 *ptch
);
