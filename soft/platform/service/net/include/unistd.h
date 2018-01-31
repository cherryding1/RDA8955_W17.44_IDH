#ifndef __UNISTD_H__
#define __UNISTD_H__
#include "sxs_io.h"
#include "lwipopts.h"

#ifndef __u_char_defined
typedef unsigned long  u_long;
typedef unsigned int   u_int;
typedef unsigned short u_short;
typedef unsigned char  u_char;
#define __u_char_defined
#endif

extern char *optarg;
extern int optind;
extern int opterr;

#define fpos_t int
#define signal(A,B)
typedef int FILE;
#define PRId64 "lu"
#define STDIN_FILENO 0
#define stdout 1
#define stderr TRUE
#define printf(...)

#define fflush(...)

int getopt(int nargc, char * const *nargv, const char* ostr);

#define fprintf(a,...) sys_arch_printf(__VA_ARGS__)
#define perror(...) sys_arch_printf(__VA_ARGS__)
#ifdef exit
#define exit(x) do { sys_arch_printf("exit with %d\n",x);return x;} while(0)
#endif

#endif
