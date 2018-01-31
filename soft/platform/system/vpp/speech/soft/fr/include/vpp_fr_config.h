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





#ifndef CONFIG_H
#define CONFIG_H

//define SIGHANDLER_T    1               /* signal handlers are void     */
//define HAS_SYSV_SIGNAL 1               /* sigs not blocked/reset?      */

#define HAS_STDLIB_H    1               /* /usr/include/stdlib.h        */
//define HAS_LIMITS_H    1               /* /usr/include/limits.h        */
#define HAS_FCNTL_H     1               /* /usr/include/fcntl.h         */
//define HAS_ERRNO_DECL  1               /* errno.h declares errno       */

#define HAS_FSTAT       1               /* fstat syscall                */
#define HAS_FCHMOD      1               /* fchmod syscall               */
#define HAS_FCHOWN      1               /* fchown syscall               */

#define HAS_STRING_H    1               /* /usr/include/string.h        */
//define HAS_STRINGS_H   1               /* /usr/include/strings.h       */

#define HAS_UTIME       1               /* POSIX utime(path, times)     */
//define HAS_UTIMES      1               /* use utimes() syscall instead */
//define HAS_UTIME_H     1               /* UTIME header file            */
//define HAS_UTIMBUF     1               /* struct utimbuf               */
//define HAS_UTIMEUSEC   1               /* microseconds in utimbuf?     */

#endif  /* CONFIG_H */
