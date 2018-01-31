/**************************************************************************
	Generated automatically by Codegenix(TM) - (c) 2000-2004 Dafocus
	EnDec version 1.0.75
	GENERATOR: ed-c
	http://www.Dafocus.it/
**************************************************************************/

#ifndef __ed_c_h_H
#define __ed_c_h_H
#include "ed_data.h"


#ifdef __cplusplus
extern "C" {
#endif

#include "rrd_ed_c.h"
#include "rri_ed_c.h"
#include "mm_ed_c.h"
#include "cc_ed_c.h"


/*-----------------------------------*/
typedef struct _c_TSparePadding {
	int __Dummy_Field__;

}	c_TSparePadding;
#define INIT_c_TSparePadding(sp) ED_RESET_MEM ((sp), sizeof (c_TSparePadding))
#define FREE_c_TSparePadding(sp)

#ifdef __cplusplus
};
#endif

#endif

