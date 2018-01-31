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

#include "tsdp_calib.h"


// ============================================================================
//  SetCalibrationMatrix
// ----------------------------------------------------------------------------
/// Description: Calling this function with valid input data
///              in the display and screen input arguments
///              causes the calibration factors between the
///              screen and display points to be calculated,
///              and the output argument - matrixPtr - to be
///              populated.
///
///              This function needs to be called only when new
///              calibration factors are desired.
///
/// @param displayPtr (input) Pointer to an array of three samples, reference points.
/// @param screenPtr  (input) Pointer to the array of touch screen points
///                           corresponding to the reference display points.
/// @param matrixPtr  (output)Pointer to calibration factors matrix computed
///                           for the set of points being provided.
///
///
///  @return Calibration succesful flag :
///          CALIBRATION_SUCCESSFUL - the calibration matrix was correctly
///          calculated and its value is in the output argument.
///          CALIBRATION_NOT_SUCCESSFUL- an error was detected and the
///          function failed to return a valid set of matrix values.
///          The only time this sample code returns
///          CALIBRATION_NOT_SUCCESSFUL is when Divider == 0
///
///
///
///
///         Divider =  (Xs0 - Xs2)*(Ys1 - Ys2) - (Xs1 - Xs2)*(Ys0 - Ys2)
///
///
///
///                 (Xd0 - Xd2)*(Ys1 - Ys2) - (Xd1 - Xd2)*(Ys0 - Ys2)
///           A = ---------------------------------------------------
///                                   Divider
///
///
///                (Xs0 - Xs2)*(Xd1 - Xd2) - (Xd0 - Xd2)*(Xs1 - Xs2)
///           B = ---------------------------------------------------
///                                   Divider
///
///
///                 Ys0*(Xs2*Xd1 - Xs1*Xd2) +
///                            Ys1*(Xs0*Xd2 - Xs2*Xd0) +
///                                           Ys2*(Xs1*Xd0 - Xs0*Xd1)
///            C = ---------------------------------------------------
///                                   Divider
///
///
///                 (Yd0 - Yd2)*(Ys1 - Ys2) - (Yd1 - Yd2)*(Ys0 - Ys2)
///            D = ---------------------------------------------------
///                                   Divider
///
///
///                 (Xs0 - Xs2)*(Yd1 - Yd2) - (Yd0 - Yd2)*(Xs1 - Xs2)
///            E = ---------------------------------------------------
///                                  Divider
///
///
///                 Ys0*(Xs2*Yd1 - Xs1*Yd2) +
///                             Ys1*(Xs0*Yd2 - Xs2*Yd0) +
///                                           Ys2*(Xs1*Yd0 - Xs0*Yd1)
///            F = ---------------------------------------------------
///                                  Divider
///
///
///
///
///                NOTE!    NOTE!    NOTE!
///
///  SetCalibrationMatrix() and GetDisplayPoint() will do fine
///  for you as they are, provided that your digitizer
///  resolution does not exceed 10 bits (1024 values).  Higher
///  resolutions may cause the integer operations to overflow
///  and return incorrect values.  If you wish to use these
///  functions with digitizer resolutions of 12 bits (4096
///  values) you will either have to a) use 64-bit signed
///  integer variables and math, or b) judiciously modify the
///  operations to scale results by a factor of 2 or even 4.
// ============================================================================
UINT32 SetCalibrationMatrix ( TSD_POINT_T* displayPtr, TSD_POINT_T* screenPtr,
                              TSD_MATRIX_T* matrixPtr)
{

    UINT32  retValue = CALIBRATION_SUCCESSFUL ;
#ifdef TSD_CALIB_IS3DOT
    UINT32 X_TRUE=0,Y_TRUE=0;

    if(screenPtr[0].x&&screenPtr[1].x&&screenPtr[0].y&&screenPtr[1].y)
    {
        if(screenPtr[0].x>screenPtr[1].x)
            X_TRUE=(screenPtr[0].x>(screenPtr[1].x<<1))?1:0;
        else
            X_TRUE=(screenPtr[1].x>(screenPtr[0].x<<1))?1:0;

        if(screenPtr[0].y>screenPtr[1].y)
            Y_TRUE=(screenPtr[0].y>(screenPtr[1].y<<1))?1:0;
        else
            Y_TRUE=(screenPtr[1].y>(screenPtr[0].y<<1))?1:0;

        if(X_TRUE&&Y_TRUE)
            matrixPtr->Divider = 1;
        else
            matrixPtr->Divider = 0;
    }
    else
        matrixPtr->Divider = 0;

#else
    matrixPtr->Divider = (  ((INT32)screenPtr[0].x - (INT32)screenPtr[2].x ) *
                            ((INT32)screenPtr[1].y - (INT32)screenPtr[2].y ) ) -
                         (  ((INT32)screenPtr[1].x - (INT32)screenPtr[2].x ) *
                            ((INT32)screenPtr[0].y - (INT32)screenPtr[2].y ) );
#endif



    if( matrixPtr->Divider == 0 )
    {
        retValue = CALIBRATION_NOT_SUCCESSFUL ;
    }
    else
    {
#ifdef TSD_CALIB_IS3DOT

        matrixPtr->An = (INT32)screenPtr[0].x|(((INT32)displayPtr[0].x)<<16);

        matrixPtr->Bn = (INT32)displayPtr[1].x -(INT32) displayPtr[0].x;

        matrixPtr->Cn = (INT32)screenPtr[1].x -(INT32) screenPtr[0].x;


        matrixPtr->Dn = (INT32)screenPtr[0].y|(((INT32)displayPtr[0].y)<<16);

        matrixPtr->En = (INT32)displayPtr[1].y -(INT32) displayPtr[0].y;

        matrixPtr->Fn = (INT32)screenPtr[1].y -(INT32) screenPtr[0].y;


#else
        matrixPtr->An = (((INT32)displayPtr[0].x - (INT32)displayPtr[2].x) * ((INT32)screenPtr[1].y -(INT32) screenPtr[2].y)) -
                        (((INT32)displayPtr[1].x - (INT32)displayPtr[2].x) * ((INT32)screenPtr[0].y - (INT32)screenPtr[2].y)) ;

        matrixPtr->Bn = (((INT32)screenPtr[0].x -(INT32) screenPtr[2].x) * ((INT32)displayPtr[1].x - (INT32)displayPtr[2].x)) -
                        (((INT32)displayPtr[0].x - (INT32)displayPtr[2].x) * ((INT32)screenPtr[1].x - (INT32)screenPtr[2].x)) ;

        matrixPtr->Cn = ((INT32)screenPtr[2].x * (INT32)displayPtr[1].x - (INT32)screenPtr[1].x * (INT32)displayPtr[2].x) * (INT32)screenPtr[0].y +
                        ((INT32)screenPtr[0].x * (INT32)displayPtr[2].x - (INT32)screenPtr[2].x * (INT32)displayPtr[0].x) * (INT32)screenPtr[1].y +
                        ((INT32)screenPtr[1].x * (INT32)displayPtr[0].x - (INT32)screenPtr[0].x * (INT32)displayPtr[1].x) * (INT32)screenPtr[2].y ;

        matrixPtr->Dn = (((INT32)displayPtr[0].y - (INT32)displayPtr[2].y) * ((INT32)screenPtr[1].y - (INT32)screenPtr[2].y)) -
                        (((INT32)displayPtr[1].y - (INT32)displayPtr[2].y) * ((INT32)screenPtr[0].y - (INT32)screenPtr[2].y)) ;

        matrixPtr->En = (((INT32)screenPtr[0].x - (INT32)screenPtr[2].x) * ((INT32)displayPtr[1].y - (INT32)displayPtr[2].y)) -
                        (((INT32)displayPtr[0].y - (INT32)displayPtr[2].y) * ((INT32)screenPtr[1].x - (INT32)screenPtr[2].x)) ;

        matrixPtr->Fn = ((INT32)screenPtr[2].x * (INT32)displayPtr[1].y - (INT32)screenPtr[1].x * (INT32)displayPtr[2].y) * (INT32)screenPtr[0].y +
                        ((INT32)screenPtr[0].x * (INT32)displayPtr[2].y - (INT32)screenPtr[2].x * (INT32)displayPtr[0].y) * (INT32)screenPtr[1].y +
                        ((INT32)screenPtr[1].x * (INT32)displayPtr[0].y - (INT32)screenPtr[0].x * (INT32)displayPtr[1].y) * screenPtr[2].y ;
#endif
    }

    return( retValue ) ;
}

// ============================================================================
//  GetDisplayPoint
// ----------------------------------------------------------------------------
/// Given a valid set of calibration factors and a point value reported by the
/// touch screen controler, this function calculates and returns the true
/// (or closest to true) display point below the spot where the touch screen
/// was touched.
///
/// The function simply solves for Xd and Yd by implementing the computations
/// required by the translation matrix.
///
///                                             /-     -\ .
///             /-    -\     /-            -\   |       |
///             |      |     |              |   |   Xs  |
///             |  Xd  |     | A    B    C  |   |       |
///             |      |  =  |              | * |   Ys  |
///             |  Yd  |     | D    E    F  |   |       |
///             |      |     |              |   |   1   |
///             \-    -/     \-            -/   |       |
///                                             \-     -/
///
/// It must be kept brief to avoid consuming CPU cycles.
///
///
/// @param displayPtr (output) Pointer to the calculated (true) display point.
/// @param screenPtr  (input)  Pointer to the reported touch screen point.
/// @param matrixPtr  (input)  Pointer to calibration factors matrix previously
///                   calculated from a call to #SetCalibrationMatrix()
///
/// @return Calibration succesful flag :
///         CALIBRATION_SUCCESSFUL - the display point was correctly
///         calculated and its value is in the output argument.
///         CALIBRATION_NOT_SUCCESSFUL- an error was detected and the
///         function failed to return a valid point.
// ============================================================================
UINT32 GetDisplayPoint ( TSD_POINT_T* displayPtr,
                         TSD_POINT_T* screenPtr,
                         TSD_MATRIX_T* matrixPtr )
{

    UINT32  retValue = CALIBRATION_SUCCESSFUL ;

    if( matrixPtr->Divider != 0 )
    {

#ifdef TSD_CALIB_IS3DOT

        displayPtr->x = (  (INT32)screenPtr->x - (matrixPtr->An&0xffff)) *
                        matrixPtr->Bn/matrixPtr->Cn
                        +(matrixPtr->An>>16) ;



        displayPtr->y = (  (INT32)screenPtr->y - (matrixPtr->Dn&0xffff)) *
                        matrixPtr->En/matrixPtr->Fn
                        +(matrixPtr->Dn>>16) ;

#else

        displayPtr->x = ( (matrixPtr->An * (INT32)screenPtr->x) +
                          (matrixPtr->Bn * (INT32)screenPtr->y) +
                          matrixPtr->Cn
                        ) / matrixPtr->Divider ;


        displayPtr->y = ( (matrixPtr->Dn * (INT32)screenPtr->x) +
                          (matrixPtr->En * (INT32)screenPtr->y) +
                          matrixPtr->Fn
                        ) / matrixPtr->Divider ;
#endif
    }
    else
    {

        retValue = CALIBRATION_NOT_SUCCESSFUL ;
    }


    return( retValue ) ;

}

