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





#ifndef _WRAPPER_H
#define _WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

/* indicates desired playback state based on user input */
typedef enum {Stop, Play, Pause, Quit} uiSignal;

/* debug.c */
void DebugMemCheckInit(void);
void DebugMemCheckStartPoint(void);
void DebugMemCheckEndPoint(void);
void DebugMemCheckFree(void);

/* timing.c */
int InitTimer(void);
unsigned int ReadTimer(void);
int FreeTimer(void);
unsigned int GetClockFrequency(void);
unsigned int CalcTimeDifference(unsigned int startTime, unsigned int endTime);

/* userint.c */
uiSignal PollHardware(void);

#ifdef __cplusplus
}
#endif


#endif /* _WRAPPER_H */
