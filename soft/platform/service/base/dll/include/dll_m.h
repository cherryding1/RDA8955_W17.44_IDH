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

#ifndef _DLL_M_H_
#define _DLL_M_H_

#include "elf.h"

// =============================================================================
// MACROS
// =============================================================================

#define FLTMAG      "bFLT"
#define SFLTMAG     (sizeof(FLTMAG)-1)

#define PATH_MAX     128


#define  DLL_TO_STRING(B)   _DLL_TO_STRING(B)
#define  _DLL_TO_STRING(B)  # B

typedef enum
{
    DL_MALLOC_LOCAL, //malloc from local
    DL_MALLOC_HEAP  //malloc from heap
} DL_MALLOC_T;

/* Env. var for directory search of libraries */
#define LIBPATHENV              "LD_LIBRARY_PATH"
#define LDPRELOADENV            "LD_PRELOAD"

#define DLDEBUGENV              "DL_DEBUG"
#define LDTLOENV                "LD_TLO"


/* Object flags */
#define OBJFLAG_INIT            0x0001  /* object has .init processing done */
#define OBJFLAG_RESOLVED        0x0002  /* object has been resolved */
#define OBJFLAG_FINI            0x0004  /* Don't do .fini section */
#define OBJFLAG_SYMBOLIC        0x0010  /* DT_SYMBOLIC */
#define OBJFLAG_LAZY            0x0020  /* lazy binding for code references */
#define OBJFLAG_RELSDONE        0x0040  /* Relative relocs have been done */
#define OBJFLAG_REVERSED        0x0080  /* Reverse relocations (for bootstrapping) */
#define OBJFLAG_NOSONAME        0x0100  /* No soname in object */
#define OBJFLAG_EXECUTABLE      0x0200  /* Object is the program */
#define OBJFLAG_BEGANINIT       0x0400  /* Init process was started on this object, but not finished */
#define OBJFLAG_BEGANFINI       0x0800  /* Fini process was started on this object, but not finished */
#define OBJFLAG_TEXTREL         0x1000  /* Object has text segment relocations */
#define OBJFLAG_INITARRAY       0x2000  /* object has .init_array done */
#define OBJFLAG_FINIARRAY       0x4000  /* object jas .fini_array done */
#define OBJFLAG_PREINIT         0x8000  /* object has .preinit_array  done */
#define OBJFLAG_BEGANINITARRAY      0x10000  /* .init_array process was started on this object, but not finished */
#define OBJFLAG_BEGANFINIARRAY      0x20000  /* .fini_array process was started on this object, but not finished */
#define OBJFLAG_BEGANPREINIT        0x40000  /* .preinit_array process was started on this object, but not finished */
#define OBJFLAG_NEW_RELOCS          0x80000  /* newer version of MIPS relocs present */
#define OBJFLAG_LD_PRELOAD      0x100000    /* mark the object as LD_PRELOAD-ed */
#define OBJFLAG_LOCALGOTREL            0x200000  /* object has local GOT relocaed */


/* The MODE argument to `dlopen' contains one of the following: */
#define RTLD_LAZY   0x00001 /* Lazy function call binding.  */
#define RTLD_NOW    0x00002 /* Immediate function call binding.  */
#define RTLD_BINDING_MASK   0x3 /* Mask of binding time value.  */
#define RTLD_NOLOAD 0x00004 /* Do not load the object.  */
#define RTLD_DEEPBIND   0x00008 /* Use deep binding.  */

/* If the following bit is set in the MODE argument to `dlopen',
   the symbols of the loaded object and its dependencies are made
   visible as if the object were linked directly into the program.  */
#define RTLD_GLOBAL 0x00100

/* Unix98 demands the following flag which is the inverse to RTLD_GLOBAL.
   The implementation does this by default and so we can define the
   value to zero.  */
#define RTLD_LOCAL  0

/* Do not delete object when closed.  */
#define RTLD_NODELETE   0x01000

/* If the first argument of `dlsym' or `dlvsym' is set to RTLD_NEXT
   the run-time address of the symbol called NAME in the next shared
   object is returned.  The "next" relation is defined by the order
   the shared objects were loaded.  */
# define RTLD_NEXT  ((void *) -1l)

/* If the first argument to `dlsym' or `dlvsym' is set to RTLD_DEFAULT
   the run-time address of the symbol called NAME in the global scope
   is returned.  */
# define RTLD_DEFAULT   ((void *) 0)


#define _iszero(_q)             (!(_q)->next && !(_q)->prev)
#define _isempty(_q)            ((_q)->next == (_q))
#define _islast(_q,_e)          ((_e)->next == (_q))
#define _atend(_q,_e)           ((_e) == (_q))
#define _empty(_q)              ((_q)->next = (_q)->prev = (_q))
#define _first(_q)              ((void *)((_q)->next))
#define _last(_q)               ((void *)((_q)->prev))
#define _next(_e)               ((void *)(((list_head_t *)(_e))->next))
#define _prev(_e)               ((void *)(((list_head_t *)(_e))->prev))
#define _delete(_e)             ((_e)->prev->next = (_e)->next, (_e)->next->prev = (_e)->prev)
#define _insert(_q,_e)          ((_e)->next = (_q),         \
                                 (_e)->prev = (_q)->prev,   \
                                 (_q)->prev->next = (_e),   \
                                 (_q)->prev = (_e))
#define _append(_q,_e)          ((_e)->next = (_q)->next,   \
                                 (_e)->prev = (_q),         \
                                 (_q)->next->prev = (_e),   \
                                 (_q)->next = (_e) )
#define _forward(_q,_e)         for ((_e) = _first(_q); !_atend((void *)(_q), (_e)); (_e) = _next(_e))
#define _backward(_q,_e)        for ((_e) = _last(_q); !_atend((void *)(_q), (_e)); (_e) = _prev(_e))

#define list_iszero(_q)         _iszero((list_head_t *)(_q))
#define list_isempty(_q)        _isempty ((list_head_t *)(_q))
#define list_islast(_q,_e)      _islast((list_head_t *)(_q), (list_head_t *)(_e))
#define list_atend(_q,_e)       _atend((list_head_t *)(_q), (list_head_t *)(_e))
#define list_empty(_q)          _empty((list_head_t *)(_q))
#define list_first(_q)          _first((list_head_t *)(_q))
#define list_last(_q)           _last((list_head_t *)(_q))
#define list_next(_q)           _next((list_head_t *)(_q))
#define list_prev(_q)           _prev((list_head_t *)(_q))
#define list_delete(_e)         _delete((list_head_t *)(_e))
#define list_insert(_q,_e)      _insert((list_head_t *)(_q), (list_head_t *)(_e))
#define list_append(_q,_e)      _append((list_head_t *)(_q), (list_head_t *)(_e))
#define list_forward(_q,_e)     _forward((list_head_t *)(_q), (_e))
#define list_backward(_q,_e)    _backward((list_head_t *)(_q), (_e))

/* Link map list macros (quite ugly, because of where the ptr's are) */
#define OBJ(_o)                 ((struct object *)_o)
#define PREV(_o)                (OBJ(_o)->_link_map.l_prev)
#define NEXT(_o)                (OBJ(_o)->_link_map.l_next)
#define LMPREV(_o)              (_o->l_prev)
#define LMNEXT(_o)              (_o->l_next)

#define link_map_delete(_q,_o)                  \
    do {                                        \
        if(PREV(_o)) {                          \
            PREV(_o)->l_next = NEXT(_o);        \
        } else if(OBJ(*_q) == OBJ(_o)) {        \
            (*_q) = NEXT(_o);                   \
        }                                       \
        if(NEXT(_o)) {                          \
            NEXT(_o)->l_prev = PREV(_o);        \
        }                                       \
    } while (0)


#define link_map_insert(_q,_e)                              \
    do {                                                    \
        NEXT(_e) = (_q);                                    \
        if(_q) { PREV(_e) = PREV(_q);                       \
            (PREV(_q)) ? (PREV(_q)->l_next = (_e)) : (_q);  \
            PREV(_q) = (_e); }                              \
    } while (0)                                             \

#define link_map_append(_q,_e)                                      \
    do {                                                            \
        struct link_map *_lm = _q;                                  \
        while(_lm && LMNEXT(_lm)) _lm = LMNEXT(_lm);                \
        PREV(_e) = (_lm);                                           \
        if(_lm) { NEXT(_e) = LMNEXT(_lm);                           \
            (LMNEXT(_lm)) ? (LMNEXT(_lm)->l_prev = (_e)) : (_lm);   \
            LMNEXT(_lm) = (_e);                                     \
        }                                                           \
    } while (0)


// =============================================================================
// TYPES
// =============================================================================

typedef struct elf32_phdr Elf32_Phdr ;
typedef struct elf32_shdr Elf32_Shdr;
typedef struct elf32_sym Elf32_Sym ;
typedef struct elf32_hdr Elf32_Ehdr;


//dll header
struct dll_hdr
{
    CHAR magic[4];
    UINT32 entry;        /* Offset of first executable instruction
                                   with text segment from beginning of file */
    UINT32 got_off;
    UINT32 numgot;
    UINT32 reloc_start;  /* Offset of relocation records from
                                   beginning of file */
    UINT32 reloc_count;  /* Number of relocation records */
    UINT32 dynsym;
    UINT32 build_date;   /* When the program/library was built */
};

//elf symbol
typedef struct _Elf_Asym
{
    CHAR    *as_name;
    UINT32           as_value;
    UINT32           as_size;
    UINT32           as_info;
    UINT32           as_shndx;
    struct _Elf_Asym       *next;
} Elf_Asym;

//elf symbol list
typedef struct
{
    Elf_Asym *e_asympEx;
    UINT32 e_numsExport;
    Elf_Asym *e_asympIm;
    UINT32 e_numsImport;
} Elf_sym_list;


typedef struct _DL_STAT
{
    UINT32  st_dev;        //Device ID of device containing file
    UINT32  st_ino;        //File serial number
    UINT16  st_mode;       //Mode of file
    UINT16  st_nlink;      //Number of hard links to the file
    UINT32  st_size;       //For regular files, the file size in bytes
    UINT32  st_atime;      //Time of last access to the file
    UINT32  st_mtime;      //Time of last data modification
    UINT32  st_ctime;      //Time of last status(or inode) change
} DL_STAT;

//object
struct object
{
    UINT32  flags;
    INT32           refcount;
    CHAR          *image;
    CHAR          *name;
    Elf_sym_list  symList;
    UINT32  *got;
    UINT32  numgot;
    DL_STAT    sinfo;
};

/* List handling structure and macros */
typedef struct list_head        list_head_t;

struct list_head
{
    list_head_t             *next;
    list_head_t             *prev;
};

struct objlist
{
    list_head_t                     list;
    struct object                   *object;
    list_head_t                     *root;
    unsigned                        flags;
};

typedef struct
{
    unsigned int entry[7];
    unsigned short valid;
    unsigned short index;
} mips_exec_plt_struct_t;

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================


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
PUBLIC INT32   dl_setenv(PCHAR name, PCHAR value, INT32  clobber);


// =============================================================================
// dl_getenv
// -----------------------------------------------------------------------------
/// remove variable from environment
/// @param: name  The name of the environment variable whose value you want to get.
/// @return:  the ptr to value associated with name, if any, else NULL
// =============================================================================
PUBLIC PCHAR dl_getenv(PCHAR name);

// =============================================================================
// dl_unsetenv
// -----------------------------------------------------------------------------
/// remove variable from environment
/// @param: name  The name of the environment variable that you want to delete.
/// @return:  NULL
// =============================================================================
PUBLIC VOID  dl_unsetenv(PCHAR name);

// =============================================================================
// dlopen
// -----------------------------------------------------------------------------
/// Gain access to an executable object file
/// @param: pathname
///   NULL, or the path to the executable object file that you want to access.
/// @param: mode
///  Flags that control how dlopen() operates; see "The mode," below.
///     RTLD_GLOBAL
///        Make the object's global symbols available to any other object. Symbol lookup using dlopen( 0, mode )
///        and an associated dlsym() are also able to find the object's symbols.
///     RTLD_LOCAL
///        Make the object's global symbols available only to objects in the same group.
/// @return:  A handle to the object, or NULL if an error occurs.
// =============================================================================
PUBLIC  VOID *dlopen( CHAR *name, INT32 mode);

// =============================================================================
// dlsym
// -----------------------------------------------------------------------------
/// The dlsym() function lets a process obtain the address of the symbol specified by name defined in a shared object.
/// @param: handle   A handle for a shared object
/// @param: name name of the shared object
/// @return:  A pointer to the named symbol for success, or NULL if an error occurs.
// =============================================================================
PUBLIC VOID *dlsym(VOID *handle, CONST CHAR *name);

// =============================================================================
// dlclose
// -----------------------------------------------------------------------------
/// Close a shared object
/// @param: handle   A handle for a shared object
/// @return:  0 for success, or a nonzero value if an error occurs.
// =============================================================================
PUBLIC INT32 dlclose(VOID *handle);

// =============================================================================
// dlreg
// -----------------------------------------------------------------------------
/// Close a shared object
/// @param: handle   A handle for a shared object
/// @param: name name of the symbol
/// @param: value of the symbol
/// @return:  NULL
// =============================================================================
PUBLIC VOID dlreg(VOID *handle, CHAR *name, UINT32 value);

// =============================================================================
// dlerror
// -----------------------------------------------------------------------------
/// Get dynamic loading diagnostic information
/// @return:  A pointer to an error description, or NULL.
// =============================================================================
PUBLIC CHAR *dlerror(VOID) ;

// =============================================================================
// dl_allocHeapIndex
// -----------------------------------------------------------------------------
/// define a heap index dll used for malloc
/// @return:  NULL.
// =============================================================================
PUBLIC VOID dl_allocHeapIndex(INT32 heap_index);


// =============================================================================
// dl_envInit
// -----------------------------------------------------------------------------
/// enviroment variable register
/// @return:  NULL.
// =============================================================================
PUBLIC VOID dl_envInit();

#endif

