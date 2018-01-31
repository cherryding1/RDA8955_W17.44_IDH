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





#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "errno.h"

/* declarations to provide consistent linkage */
extern char *optarg;
extern int optind;
extern int opterr;

int opterr = 1,     /* if error message should be printed */
    optind = 1,     /* index into parent argv vector */
    optopt,         /* character checked for validity */
    optreset;       /* reset getopt */
char    *optarg;        /* argument associated with option */

#define BADCH   (int)'?'
#define BADARG  (int)':'
#define EMSG    ""
static char *place = EMSG;      /* option letter processing */

/*
 * getopt --
 *  Parse argc/argv argument vector.
 */

void getoptrst()
{
    opterr = 1;
    optind = 1;
    optopt =0;
    optreset = 0;
    optarg = NULL;
    place = EMSG;
}

int
getopt(int nargc, char * const *nargv, const char* ostr)
{
    char *oli;              /* option letter list index */

    if (optreset || !*place)        /* update scanning pointer */
    {
        optreset = 0;
        if (optind >= nargc || *(place = nargv[optind]) != '-')
        {
            place = EMSG;
            return (EOF);
        }
        if (place[1] && *++place == '-')    /* found "--" */
        {
            ++optind;
            place = EMSG;
            return (EOF);
        }
    }                   /* option letter okay? */
    if ((optopt = (int)*place++) == (int)':' ||
            !(oli = strchr(ostr, optopt)))
    {
        /*
         * if the user didn't specify '-' as an option,
         * assume it means EOF.
         */
        if (optopt == (int)'-')
            return (EOF);
        if (!*place)
            ++optind;
        if (opterr && *ostr != ':')
            fprintf(stderr,
                          "%s: illegal option -- %c,%x,%d,%d,%c\n", __FILE__, optopt,optopt,optind,*place,*ostr);
        return (BADCH);
    }
    if (*++oli != ':')              /* don't need argument */
    {
        optarg = NULL;
        if (!*place)
            ++optind;
    }
    else                    /* need an argument */
    {
        if (*place)         /* no white space */
            optarg = place;
        else if (nargc <= ++optind)     /* no arg */
        {
            place = EMSG;
            if (*ostr == ':')
                return (BADARG);
            if (opterr)
                fprintf(stderr,
                              "%s: option requires an argument -- %c\n",
                              __FILE__, optopt);
            return (BADCH);
        }
        else                /* white space */
            optarg = nargv[optind];
        place = EMSG;
        ++optind;
    }
    return (optopt);            /* dump back option letter */
}
