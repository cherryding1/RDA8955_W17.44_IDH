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






#define SECOND        * HAL_TICK1S

#define MILLI_SECOND  SECOND / 1000

#define MILLI_SECONDS MILLI_SECOND

#define SECONDS       SECOND

#define MINUTE        * ( 60 SECOND )

#define MINUTES       MINUTE

#define HOUR          * ( 60 MINUTE )

#define HOURS         HOUR

#define FS_TRACE_TIMER_CNT (1 SECOND)



// (0xabcd| SXS_TIMER_EVT_ID)

#define FSTRACE_TIMER1 PRV_CFW_TRACE_TO_FLASH_TIMER_ID

#define MAX_TRACE_LEN 512



typedef struct

{

    u32 eventId;

    u32 nParam1;

    u32 nParam2;

    u32 nParam3;

} fsTrace_event_t;





// declaration of function



// =============================================================================
// bal_fsTraceProcess

// -----------------------------------------------------------------------------
/// Read a bunch of data from the debug remote buffer and output to tFlash card.

// =============================================================================

PUBLIC VOID bal_fsTraceProcess(VOID);



// =============================================================================
// bal_fstraceStart

// -----------------------------------------------------------------------------
/// start tFlash trace recorsing

// =============================================================================

PUBLIC VOID  bal_fstraceStart(VOID);



// =============================================================================
// bal_fstraceStop

// -----------------------------------------------------------------------------
/// stop tFlash trace recorsing

// =============================================================================

PUBLIC VOID  bal_fstraceStop(VOID);





