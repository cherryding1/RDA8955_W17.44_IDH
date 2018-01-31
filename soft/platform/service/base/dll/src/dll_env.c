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

#include "cs_types.h"
#include "stdio.h"
#include "sxr_ops.h"
#include "sxs_io.h"
#include "hal_debug.h"
#include "string.h"
#include "dll_m.h"
#include "dll_cfg.h"

#ifdef _DL_ENV_MALLOC_
LOCAL **dl_environ;     /* pointer to enviroment */
LOCAL INT32 reall = 0;      /* flag to reallocate space, if putenv is called
                   more than once */
#else
#define MAX_ENV_NUM 8
#define MAX_ENV_STR 80
LOCAL CHAR dl_environ[MAX_ENV_NUM][MAX_ENV_STR];
LOCAL INT32 dl_env_count = 0;
#endif

PRIVATE INT32   find(CHAR *);
PRIVATE INT32   match(CHAR *, CHAR *);
PRIVATE CHAR *nvmatch();

/* namelength - determine length of name in "name=whatever" */

PRIVATE INT32 namelength(CHAR   *name)
{
    CHAR   *equal;

    equal = strchr(name, '=');
    return ((equal == 0) ? strlen(name) : (equal - name));
}

/* findenv - given name, locate name=value */

PRIVATE CHAR **findenv(CHAR *name, INT32 len)
{
#ifdef _DL_ENV_MALLOC_
    CHAR  **envp;
#else
    CHAR  (*envp)[MAX_ENV_STR];
#endif
#ifdef _DL_ENV_MALLOC_
    for (envp = dl_environ; envp && *envp; envp++)
    {
#else
    for (envp = dl_environ; envp - dl_environ < dl_env_count; envp++)
    {
#endif
        if (strncmp(name, *envp, len) == 0 && (*envp)[len] == '=')
            return ((CHAR **)envp);
    }

    return (0);
}

/*  find - find where s2 is in dl_environ
 *
 *  input - str = string of form name=value
 *
 *  output - index of name in dl_environ that matches "name"
 *       -size of table, if none exists
*/
PRIVATE INT32  find(CHAR *str)
{
    INT32 ct = 0;   /* index into dl_environ */
#ifdef _DL_ENV_MALLOC_
    while(dl_environ != NULL && dl_environ[ct] != NULL)
    {
#else
    while(ct < dl_env_count)
    {
#endif
        if (match(dl_environ[ct], str)  != 0)
            return (ct);
        ct++;
    }
    return (-(++ct));
}

/*
 *  s1 is either name, or name=value
 *  s2 is name=value
 *  if names match, return value of 1,
 *  else return 0
 */
PRIVATE INT32  match(CHAR *s1, CHAR *s2)
{
    while(*s1 == *s2++)
    {
        if (*s1 == '=')
            return (1);
        s1++;
    }
    return (0);
}

/*
 *  s1 is either name, or name=value
 *  s2 is name=value
 *  if names match, return value of s2, else NULL
 *  used for dl_environment searching: see getenv
 */
PRIVATE PCHAR nvmatch(CHAR *s1, CHAR *s2)
{
    while(*s1 == *s2++)
        if(*s1++ == '=')
            return(s2);

    if(*s1 == '\0' && *(s2 - 1) == '=')
        return(s2);

    return(NULL);
}


/*  putenv - change dl_environment variables
 *
 *  input - char *change = a pointer to a string of the form
 *                 "name=value"
 *
 *  output - 0, if successful
 *       1, otherwise
 */
PRIVATE INT32  putenv(CHAR *change)
{
#ifdef _DL_ENV_MALLOC_
    CHAR **newenv;          /* points to new dl_environment */
#endif
    INT32 which;        /* index of variable to replace */

    if ((which = find(change)) < 0)
    {
        /* if a new variable */
        /* which is negative of table size, so invert and
           count new element */
        which = (-which) + 1;

#ifdef _DL_ENV_MALLOC_
        if (reall)
        {
            /* we have expanded dl_environ before */
            newenv = (char **)_dl_realloc(dl_environ,
                                          which * sizeof(char *));
            if (newenv == NULL)  return (-1);
            /* now that we have space, change dl_environ */
            dl_environ = newenv;
        }
        else
        {
            /* dl_environ points to the original space */
            reall++;
            newenv = (char **)_dl_alloc(which * sizeof(char *));
            if (newenv == NULL)  return (-1);
            if(dl_environ != NULL)
                (void)memcpy((char *)newenv, (char *)dl_environ,
                             (int)(which * sizeof(char *)));
            dl_environ = newenv;
        }
        dl_environ[which - 2] = change;
        dl_environ[which - 1] = NULL;
#else
        strncpy(dl_environ[which - 2], change, MAX_ENV_STR);
        dl_environ[which - 1][0] = '\0';
        if(dl_env_count < MAX_ENV_NUM)
        {
            dl_env_count++;
        }
        else
        {
            DLL_ASSERT(FALSE, "putenv:number of enviroment var overflow!");
        }
#endif
    }
    else
    {
        /* we are replacing an old variable */
#ifdef _DL_ENV_MALLOC_
        dl_environ[which] = change;
#else
        strncpy(dl_environ[which], change, MAX_ENV_STR);
#endif
    }
    return (0);
}

// =============================================================================
// dl_setenv
// -----------------------------------------------------------------------------
/// update or insert dl_environment (name,value) pair
/// @param: name  The name of the environment variable that you want to set.
/// @param: value  NULL, or the value for the environment variable; see below.
/// @param: clobber
///     A nonzero value if you want the function to overwrite the variable if it exists, or 0 if you don't want
///     to overwrite the variable.
/// @return:  0 if success, or Nonzero if an error occurs.
// =============================================================================
PUBLIC INT32 dl_setenv(PCHAR name, PCHAR value, INT32  clobber)
{
#ifdef _DL_ENV_MALLOC_
    PCHAR cp;
#else
    char cp[MAX_ENV_STR];
#endif

    if (clobber == 0 && dl_getenv(name) != 0)
        return (0);
#ifdef _DL_ENV_MALLOC_
    if ((cp = _dl_alloc(strlen(name) + strlen(value) + 2)) == 0)
        return (1);
#endif

    snprintf(cp, MAX_ENV_STR, "%s=%s", name, value);
    return (putenv(cp));
}

// =============================================================================
// dl_getenv
// -----------------------------------------------------------------------------
/// remove variable from environment
/// @param: name  The name of the environment variable whose value you want to get.
/// @return:  the ptr to value associated with name, if any, else NULL
// =============================================================================
PUBLIC PCHAR dl_getenv(PCHAR name)
{
    CHAR *v;
#ifdef _DL_ENV_MALLOC_
    CHAR  **p = dl_environ;
#else
    CHAR  (*p)[MAX_ENV_STR] = dl_environ;
#endif

    if(p == NULL)
        return(NULL);
#ifdef _DL_ENV_MALLOC_
    while(*p != NULL)
    {
#else
    while(*p != NULL && (p - dl_environ < dl_env_count))
    {
#endif
        if((v = nvmatch(name, *p++)) != NULL)
            return(v);
    }
    return(NULL);
}

// =============================================================================
// dl_unsetenv
// -----------------------------------------------------------------------------
/// remove variable from environment
/// @param: name  The name of the environment variable that you want to delete.
/// @return:  NULL
// =============================================================================
PUBLIC VOID  dl_unsetenv(PCHAR name)
{
#ifdef _DL_ENV_MALLOC_
    CHAR  **envp;
#else
    CHAR  (*envp)[MAX_ENV_STR];
#endif

#ifdef _DL_ENV_MALLOC_
    // TODO: /* if we malloc-ed it, free it first */
    if ((envp = findenv(name, namelength(name))) != 0)
    {
        while (envp[0] = envp[1])
            envp++;
    }
#else
    if ((envp = (CHAR (*)[MAX_ENV_STR])findenv(name, namelength(name))) != 0)
    {
        while(envp - dl_environ < dl_env_count)
        {
            strncpy(envp[0], envp[1], MAX_ENV_STR);
            envp++;
        }
        envp[0][0] = '\0';
        dl_env_count--;
    }
#endif

}

// =============================================================================
// dl_envInit
// -----------------------------------------------------------------------------
/// enviroment variable register
/// @return:  NULL.
// =============================================================================
PUBLIC VOID dl_envInit()
{
    //dl_allocHeapIndex(3);
#ifdef LD_LIBRARY_PATH
    dl_setenv(LIBPATHENV, DLL_TO_STRING(LD_LIBRARY_PATH), 1);
#else
    dl_setenv(LIBPATHENV, "/:/lib/:/t:/t/lib", 1);
#endif
    dl_setenv(DLDEBUGENV, "yes", 1);
}

