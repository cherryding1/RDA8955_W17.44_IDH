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
#include "cfw.h"

#include "csw.h"

#include "fs.h"

#include "errorcode.h"

#include "dsm_cf.h"

#include "sxr_ops.h"
#include "sxs_io.h"

#include "string.h"



#include "hal_debug.h"

#include "dll_m.h"

#include "dll_cfg.h"

#include "dll_map.h"

#include "sxr_usrmem.h"

#ifdef  _DL_SIMU_

UINT32 test_add[] =
{

#include "test-add.hex"
};

#endif



#define     COS_HEAP_INDEX             3

PRIVATE UINT32 dll_heapIndex = SXR_STD_MEM;



PRIVATE CONST CHAR DL_E_INV_NAME[]      = "Invalid name";

PRIVATE CONST CHAR DL_E_BAD_LIB[]        = "Shared library is corrupted";

PRIVATE CONST CHAR DL_E_NOT_FOUND[]      = "Library cannot be found";

PRIVATE CONST CHAR DL_E_NO_MEMORY[]      = "Not enough memory";

PRIVATE CONST CHAR DL_E_NO_SYM[]         = "Symbol not found";

PRIVATE CONST CHAR DL_E_UNRES_SYM[]      = "Unresolved symbols";

PRIVATE CONST CHAR DL_E_INV_HANDLE[]     = "Invalid handle";

PRIVATE CONST CHAR DL_E_BAD_MODE[]       = "Invalid mode";

PRIVATE CONST CHAR DL_E_NO_EXE[]        = "Executable not found";
PRIVATE CONST CHAR DL_E_NO_SYM_EXE[]     = "No symbols in executable";



#ifndef VARIANT_so

PRIVATE CONST CHAR DL_E_NO_DYN[]         = "No dynamic section in executable";

#endif

PRIVATE UINT32                  _dl_error_key = -1;

PRIVATE CONST CHAR      *_dl_error = NULL;
PRIVATE list_head_t         _dl_all_objects;

PRIVATE list_head_t         _dl_start_objects;


PRIVATE VOID _dl_error_raise(const CHAR *const e_str)

{
    if(_dl_error_key == -1)
    {
        _DL_TRACEOUT(1, e_str);
        _dl_error = e_str;
    }
    else
    {
        DLL_ASSERT(FALSE, e_str);

    }
}



PRIVATE CONST CHAR *_dl_error_message(void)
{
    return _dl_error;
}



PUBLIC VOID *_dl_alloc(INT32 size)

{
    size_t *iaddr;

    UINT32 nCallerAdd;
    COS_GET_RA(&nCallerAdd);

    switch(dll_heapIndex)
    {
        case COS_HEAP_INDEX:
        {
            iaddr = (size_t *) sxr_UsrMalloc(size);//


            //    _DL_TRACEOUT(1, "_dl_alloc:0x%x", iaddr);

            if (iaddr == NULL)
            {

                iaddr = (size_t *) _sxr_HMalloc(size + sizeof (size_t), COS_HEAP_INDEX);
            }
        }
        break;

        default:
            iaddr = _sxr_HMalloc(size + sizeof (size_t), dll_heapIndex);
            break;

    }

    if (iaddr == NULL)
    {
        return NULL;

    }


    sxr_UpMemCallerAdd(iaddr, nCallerAdd);

    iaddr[0] = size;

    return (&iaddr[1]);

}



PUBLIC VOID _dl_free(VOID *ptr)

{
    size_t *lenloc;

    if (ptr == NULL)

        return;

    lenloc = (size_t *)ptr - 1;

    //_DL_TRACEOUT(1, "_dl_free:0x%x", lenloc);

    sxr_Free(lenloc);

}





PUBLIC VOID *_dl_realloc(VOID *oldaddr, size_t size)

{

    VOID *addr;

    size_t oldsize;



    addr = _dl_alloc(size);

    if (oldaddr != NULL)
    {

        oldsize = ((size_t *)oldaddr)[-1];

        if (addr != NULL)
        {

            memcpy(addr, oldaddr, (oldsize > size ? size : oldsize));

            _dl_free(oldaddr);

        }

    }

    return (addr);

}





PRIVATE CHAR *_dl_strdup(CONST CHAR *s)

{

    CHAR *t = NULL;

    if(s && (t = (CHAR *)_dl_alloc((strlen(s) + 1))))

        strcpy(t, s);

    return t;

}







PRIVATE INT32 _dl_fileOpen(CONST CHAR *name,  UINT32 iFlag, UINT32 iMode)

{

    INT32 result;

    INT32 fd = -1;

    UINT32 uni_len = 0;

    UINT8 *uni_name = NULL;

    if((result = ML_LocalLanguage2UnicodeBigEnding(name,  DSM_StrLen(name), &uni_name, &uni_len, NULL) ) != ERR_SUCCESS)
    {

        if(uni_name)
        {

            CSW_Free(uni_name);

            uni_name = NULL;

        }

        _DL_TRACEOUT(1, "dll:%s:%u:ML_LocalLanguage2UnicodeBigEnding ret %d", __FUNCTION__, __LINE__, result);

        return -1;

    }



    fd = FS_Open(uni_name, iFlag, iMode);



    if(uni_name)
    {

        CSW_Free(uni_name);

        uni_name = NULL;

    }

    return fd;



}





PRIVATE INT32 _dl_fileClose(INT32 fd)

{

    if(fd < 0)

    {

        _DL_TRACEOUT(1, "%s:%d: 0x%x not a FILE pointer", __FUNCTION__, __LINE__, fd);

        return -1;

    }

    return FS_Close(fd);

}



PRIVATE INT32 _dl_fileSize(INT32 fd)

{

    if(fd < 0)

    {

        _DL_TRACEOUT(1, "%s:%d: 0x%x not a FILE pointer", __FUNCTION__, __LINE__, fd);

        return -1;

    }

    return (INT32)FS_GetFileSize(fd);

}



// _dl_stat : show stat of a file
// not implement yet
PRIVATE INT32 _dl_stat(INT32 fd, DL_STAT *st)
{
    return 0;
}




PRIVATE VOID *_dl_readfile(INT32 fd, INT32 offset, INT32 size)

{

    VOID *buf;

    if(fd < 0)

    {

        _DL_TRACEOUT(1, "%s:%d: 0x%x not a FILE pointer", __FUNCTION__, __LINE__, fd);

        return NULL;

    }

    buf = _dl_alloc(size);

    if(buf == NULL)

    {

        _DL_TRACEOUT(1, " %s:%d: malloc failed", __FUNCTION__, __LINE__);

        return NULL;

    }

    memset(buf, 0, size);



    if(FS_Seek(fd, (INT64)offset, FS_SEEK_SET) < 0)

    {

        _DL_TRACEOUT(1, "%s:%d: fseek failed", __FUNCTION__, __LINE__);

        _dl_free(buf);

        return NULL;

    }

    //printf("[@] after fseek,position:%#x\n",ftell(fd));

    if(FS_Read(fd, buf, size) < 0)

    {

        _DL_TRACEOUT(1, "%s:%d: read failed", __FUNCTION__, __LINE__);

        _dl_free(buf);

        return NULL;

    }

    return buf;

}





PRIVATE CONST CHAR *name_only(CONST CHAR *pathname)
{
    CONST CHAR *p;


    if (NULL == pathname) return "";


    for (p = pathname + strlen(pathname); p != pathname && p[-1] != '/'; --p)
    {
        //nothing to do
    }
    return p;
}


#ifndef  _DL_SIMU_

PRIVATE INT32 searchpath(CONST CHAR *name, CONST CHAR *path, INT32 amode, CHAR *buffer, INT32 bufsize)

{

    CHAR                *b;

    INT32                   n;

    INT32                 debug = 0;

    INT32 fd = -1;



    if(dl_getenv(DLDEBUGENV))
    {

        debug = 1;

        CHAR *path_cp;

        path_cp = (path == NULL) ? "NULL" : (PCHAR)path;

        _DL_TRACEOUT(1, "dll:%s:%u: name=%s path=%s amode=%d bufsize=%d\n", __FUNCTION__, __LINE__, name, path_cp, amode, bufsize);

    }



    if (path == NULL)
    {

        return -1;

    }



    bufsize -= strlen(name) + 1;



    do
    {

        for (n = bufsize, *(b = buffer) = 0; path && *path && *path != ':'; n--, path++)
        {

            if (n > 0 && *path != ' ')
            {

                *b++ = *path;

            }

        }



        if (n > 0)
        {



            if (*buffer && b[-1] != '/')
            {

                *b++ = '/';

            }

            strcpy(b, name);



            if(debug)  _DL_TRACEOUT(1, "dll:%s:%u: trying %s\n", __FUNCTION__, __LINE__, buffer);



            if((fd = _dl_fileOpen(buffer, FS_O_RDONLY, 0)) >= 0)
            {



                if(debug)  _DL_TRACEOUT(1, "dll:%s:%u:%s: success\n", __FUNCTION__, __LINE__, buffer);



                return fd;

            }



            _DL_TRACEOUT(1, "dll:%s:%u:%s: failed (%d)\n", __FUNCTION__, __LINE__, buffer, fd);

            /* Don't change the errno value. */

        }



        if (path && *path == ':')
        {

            path++;

        }

    }
    while (path && *path);



    *buffer = 0;

    return -1;

}



PRIVATE INT32 _dl_find_file(CONST CHAR *name, CONST CHAR *libpath, CONST CHAR *rpath, CONST CHAR **fullpath)

{

    CHAR pathname[PATH_MAX + 1];

    INT32 fd = -1;

    INT32 debug = 0;



    if(dl_getenv(DLDEBUGENV))
    {

        CHAR *rpathcp, *libpathcp;

        rpathcp = (rpath == NULL) ? "NULL" : (PCHAR)rpath;

        libpathcp = (libpath == NULL) ? "NULL" : (PCHAR)libpath;

        debug = 1;

        _DL_TRACEOUT(1, "dll:%s:%u: name=%s libpath=%s rpath=%s\n", __FUNCTION__, __LINE__, name, libpathcp, rpathcp);

    }



    if (NULL == name) return -1;



    /*

     * Search standard directories in the right order,

     * see ABI section on the dynamic loader. Order is

     * DT_RPATH, LD_LIBRARY_PATH, /usr/lib(_CS_LIBPATH).

     */

    //PR 7654 and Unix98 say we look at the whole path for a slash character



    for(fd = 0; name[fd] != '/' && name[fd] != '\0'; fd++)
    {
        ;
    }



    if (name[fd] != '/')
    {

        /* First - search DT_RPATH */

        if ((fd = searchpath(name, rpath, 0, pathname, sizeof pathname)) == -1)
        {

            /* Next, search LD_LIBRARY_PATH, unless we are a suid executable */

            if ((fd = searchpath(name, libpath, 0, pathname, sizeof pathname)) == -1)
            {

                return -1;

            }

        }

        name = pathname;

    }
    else
    {



        if((fd = _dl_fileOpen(name, FS_O_RDONLY, 0)) < 0)
        {

            _DL_TRACEOUT(1, "dll:%s:%u:%s: sopen failed (%d)\n", __FUNCTION__, __LINE__, name, fd);

            return -1;

        }

    }



    if (fullpath)
    {

        *fullpath = _dl_strdup(name);

    }



    if(debug)

        _DL_TRACEOUT(1, "dll:%s:%u:%s: found\n",  __FUNCTION__, __LINE__, name);



    return fd;



}

#endif







PRIVATE VOID initElfSynList(Elf_sym_list *symList)

{

    symList->e_numsImport = 0;

    symList->e_numsExport = 0;

    symList->e_asympEx = NULL;

    symList->e_asympIm = NULL;

}





PRIVATE VOID dyn_insertSym(struct object *obj, Elf32_Sym *sym, CHAR *symName)

{

    Elf_Asym **asymp_prev = NULL, **asymp_curr = NULL;

    Elf_sym_list *symList = &obj->symList;



    UINT32 flag = sym->st_other;



    if(flag == 0)
    {

        asymp_curr = &symList->e_asympEx;

        symList->e_numsExport++;

    }

    else
    {

        asymp_curr = &symList->e_asympIm;

        symList->e_numsImport++;

    }



    if(*asymp_curr == NULL)
    {

        (*asymp_curr) = (Elf_Asym *)_dl_alloc(sizeof(Elf_Asym));

    }

    else
    {



        asymp_prev = asymp_curr;

        asymp_curr = &(*asymp_curr)->next;



        while(*asymp_curr != NULL)
        {

            asymp_prev = asymp_curr;

            asymp_curr = &(*asymp_curr)->next;

        }



        (*asymp_curr) = (Elf_Asym *)_dl_alloc(sizeof(Elf_Asym));

        (*asymp_prev)->next = *asymp_curr;



    }



    (*asymp_curr)->as_name = _dl_strdup(symName);

    (*asymp_curr)->next = NULL;

    (*asymp_curr)->as_value = sym->st_value;

    (*asymp_curr)->as_size = sym->st_size;

    (*asymp_curr)->as_info = sym->st_info;

    (*asymp_curr)->as_shndx = sym->st_shndx;



}







PRIVATE VOID freeElfSynList(Elf_sym_list *symList)

{

    INT32 i;

    Elf_Asym *a_sym, *a_sym_next;



    a_sym = symList->e_asympEx;



    for(i = 0; i < symList->e_numsExport; i++)
    {

        if(a_sym != NULL)
        {

            if(a_sym->as_name != NULL) _dl_free(a_sym->as_name);

            a_sym_next = a_sym->next;

            _dl_free(a_sym);

            a_sym = a_sym_next;

        }



    }



    a_sym = symList->e_asympIm;



    for(i = 0; i < symList->e_numsImport; i++)
    {

        if(a_sym != NULL)
        {

            if(a_sym->as_name != NULL) _dl_free(a_sym->as_name);

            a_sym_next = a_sym->next;

            _dl_free(a_sym);

            a_sym = a_sym->next;

        }

    }

}







PRIVATE VOID init(const list_head_t *objs)

{

    struct objlist      *o;



    list_backward(objs, o)
    {



        if (o->object->flags & (OBJFLAG_INIT | OBJFLAG_BEGANINIT))
        {

            continue;

        }



        // TODO: INIT



        o->object->flags &= ~OBJFLAG_BEGANINIT;

        o->object->flags |= OBJFLAG_INIT;

    }

}



PRIVATE VOID init_array(const list_head_t *objs)

{

    struct objlist      *o;



    list_backward(objs, o)
    {



        if (o->object->flags

                & (OBJFLAG_BEGANINITARRAY | OBJFLAG_INITARRAY))
        {

            continue;

        }

        //TODO: init array

        o->object->flags &= ~OBJFLAG_BEGANINITARRAY;

        o->object->flags |= OBJFLAG_INITARRAY;

    }

    return;

}





/*
  If the name is specified, then the name must match.
  If the stat is specified, then the stat must match.
  If both are specified, then both must match.
*/
static struct object *find_object_stat(const list_head_t *list, const char *name, DL_STAT *st, struct objlist **listentry)
{
    struct objlist      *l;

    list_forward(list, l)
    {
        if (l->object->name &&
                (!name || strcmp(name, l->object->name) == 0) &&
                (!st || (st->st_ino == l->object->sinfo.st_ino && st->st_dev == l->object->sinfo.st_dev)))
        {
            if(listentry)
            {
                *listentry = l;
            }
            return l->object;
        }
    }
    return 0;
}



PRIVATE INT32 load_object(struct object *obj,  CHAR *name, const CHAR *libpath, const CHAR *rpath,

                          struct object **dupobj, list_head_t *objectlist)

{



    Elf32_Sym *sym;

    INT32 fd;

    const char          *fullpath = NULL;

    UINT32 i = 0;

    struct dll_hdr *hdr;

    CHAR  *image;

    INT32  st_size;



    if(obj != NULL)

        initElfSynList(&obj->symList);



    if(name == NULL)
    {

        return -1;  //return -1 when no file name

    }





    if(dl_getenv(DLDEBUGENV))
    {

        _DL_TRACEOUT(1, "load_object: attempt load of %s\n", name);

    }



#ifndef  _DL_SIMU_

    if ((fd = _dl_find_file(name, libpath, rpath, NULL)) == -1)
    {

        _dl_error_raise(DL_E_NOT_FOUND);

        return -1;

    }



    st_size = _dl_fileSize(fd);


#if 0  //not implement yet
    if(_dl_stat(fd, &obj->sinfo) == -1)
    {
        if (fullpath != NULL)
        {
            _dl_free((char *)fullpath);
        }
        _dl_fileClose(fd);
        _dl_error_raise(DL_E_NOT_FOUND);
        return -1;
    }
#endif

    if(dupobj != NULL && objectlist != NULL &&
            ((*dupobj = find_object_stat(objectlist, name_only(name), NULL, NULL)) != NULL))
    {
        if (fullpath != NULL)
        {
            _dl_free((char *)fullpath);
        }
        _dl_fileClose(fd);
        return 0;
    }



    if((image = (CHAR *)_dl_readfile(fd, 0, st_size)) == NULL)

        return -1;



    _dl_fileClose(fd);



    if(dl_getenv(DLDEBUGENV))
    {
        _DL_TRACEOUT(1, "load_object: load object %s@0x%x\n", name, image);
    }
#else

    image = (CHAR *)test_add;

#endif



    hdr = (struct dll_hdr *)image;



    if(memcmp(hdr->magic, FLTMAG, SFLTMAG))
    {

        _dl_error_raise(DL_E_BAD_LIB);

        return -1;

    }



    obj->flags = 0;



    obj->name = _dl_strdup(name_only(name));



    obj->image = image;



    obj->got = (UINT32 *)(image + hdr->got_off);



    obj->numgot = hdr->numgot;



    sym = (Elf32_Sym *)(image + hdr->reloc_start);



    for(i = 0; i < hdr->reloc_count; i++)
    {



        dyn_insertSym(obj, sym, image + hdr->dynsym + sym->st_name);



        sym++;

    }



    return 0;



}





PRIVATE struct object *find_object(const list_head_t *list, const CHAR *name, struct objlist **listentry)

{

    struct objlist      *l;



    list_forward(list, l)
    {

        if (l->object->name &&

                (!name || strcmp(name, l->object->name) == 0) )

        {

            if(listentry)
            {

                *listentry = l;

            }

            return l->object;

        }

    }

    return 0;

}







PRIVATE const UINT32 lookup_global(const CHAR *name, const list_head_t *this, const struct object *omit, INT32 jumpslot, struct object **obj)

{

    struct objlist      *ol;

    Elf_Asym *asym;



    list_forward(this, ol)
    {

        asym = ol->object->symList.e_asympEx;

        while(asym != NULL)
        {

            if(strcmp(asym->as_name, name) == 0)
            {

                return asym->as_value;

            }

            asym = asym->next;

        }

    }

    return 0;

}







PRIVATE INT32  relocate_got(struct objlist *ol, INT32 mode)

{

    UINT32                *got = ol->object->got;

    struct object           *obj = ol->object;

    CONST list_head_t       *this = ol->root;

    UINT32    i, n;

    UINT32 entry;

    struct dll_hdr *hdr;

    Elf_Asym *asym;

    UINT32 value;

    INT32   ret = 0;

    INT32 debug = 0;



    if((hdr = (struct dll_hdr *)obj->image)  == NULL)
    {

        return -1;

    }



    entry = (UINT32)(obj->image + hdr->entry);

    if(dl_getenv(DLDEBUGENV))
    {

        _DL_TRACEOUT(1, "relocate_got:image@0x%x, entry@0x%x", obj->image, entry);

        debug = 1;

    }



    i = 2;

    n = obj->numgot;



    asym = obj->symList.e_asympIm;



    while (i < n)
    {

        if((asym != NULL) && (asym->as_shndx == i))
        {

            if((value = lookup_global(asym->as_name, this, NULL, 1, NULL)) != 0)  //find global external
            {

                //TODO: update global got entry

                got[i] = value;

                if(debug)
                {

                    _DL_TRACEOUT(1, "relocate_got:got[%d]=0x%x(%s)", i, got[i], asym->as_name);

                }

            }
            else

            {

                _dl_error_raise(DL_E_NO_SYM_EXE);

                if(debug)
                {

                    _DL_TRACEOUT(1, "relocate_got:symbol %s not found!", asym->as_name);

                }

                ret = -1;

            }



            asym = asym->next;

        }

        else
        {

            got[i] += entry;

            if(debug)
            {

                _DL_TRACEOUT(1, "relocate_got:got[%d]=0x%x", i, got[i]);

            }

        }



        i++;



    }

    return ret;

}







PRIVATE INT32  relocate_rels(struct object *obj)

{

    UINT32 entry;

    UINT32 plt_base;

    struct dll_hdr *hdr;

    Elf_Asym *asym;

    INT32 debug = 0;

    mips_exec_plt_struct_t *plt;


    if(dl_getenv(DLDEBUGENV))
    {

        debug = 1;

    }



    if((hdr = (struct dll_hdr *)obj->image)  == NULL)
    {

        return -1;

    }



    plt_base = (UINT32)(obj->image + sizeof(struct dll_hdr));



    entry = (UINT32)(obj->image + hdr->entry);



    asym = obj->symList.e_asympEx;



    plt = (mips_exec_plt_struct_t *)plt_base;

    while(asym != NULL)
    {

        if (ELF32_ST_TYPE(asym->as_info) == STT_FUNC)
        {



            unsigned short *rel = (unsigned short *)(plt);


            *(rel + 2) = (unsigned int)(entry + asym->as_value) & 0xffff;

            /*

                    if(*(rel+2) < 0x8000){

                        *rel = (UINT32)(entry + asym->as_value) >> 16;
                    }
                    else{
                         *rel = ((UINT32)(entry + asym->as_value) >> 16) + 1;
                    }
            */
            *rel =  (UINT32)(entry + asym->as_value) >> 16;



            if(debug)
            {

                _DL_TRACEOUT(1, "relocate_rels:%s:0x%x->0x%x(STT_FUNC)", asym->as_name, asym->as_value, plt);

            }



            asym->as_value = (UINT32 )plt;//reload as_value;

            //sxs_fprintf(TSTDOUT, "plt %d:0x%x 0x%x", asym->as_shndx,plt_tmp[0], plt_tmp[1]);


            plt++;
        }


        else if(ELF32_ST_TYPE(asym->as_info) == STT_OBJECT)
        {
            _DL_TRACEOUT(1, "relocate_rels:%s:0x%x->0x%x(STT_OBJECT)\n", asym->as_name, asym->as_value, entry + asym->as_value);
            asym->as_value = (unsigned int)(entry + asym->as_value);
        }

        else
        {
            _DL_TRACEOUT(1, "relocate_rels:unknow type 0x%x\n", ELF32_ST_TYPE(asym->as_info));
        }

        asym = asym->next;



    }

    return 0;

}







PRIVATE INT32 resolve(const list_head_t *this, INT32 mode)

{

    struct objlist      *o;

    INT32                 ret = 0;



    list_backward(this, o)
    {

        struct object       *obj = o->object;

        if (!(obj->flags & OBJFLAG_RESOLVED))
        {

            if(relocate_got(o, mode))

                ret = -1;

            if(relocate_rels(obj))

                ret = -1;

            /* The internal relative relocs are done, don't do them again. */

            obj->flags |= (OBJFLAG_RELSDONE | OBJFLAG_RESOLVED);

        }

    }



    return ret;



}







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
PUBLIC  VOID *dlopen( CHAR *name, INT32 mode)

{

    list_head_t         *handle;

    struct objlist      *olist, *ol;

    struct object       stkobj, *obj;

    CHAR                *libpath = NULL;

    INT32                 debug = 0;



    if(dl_getenv(DLDEBUGENV))
    {

        debug = 1;

        _DL_TRACEOUT(1, "dlopen(%s,0x%x)", name ? name : "NULL", mode);

        // snprintf(namecopy,sizeof(namecopy)-2,"dlopen(%s,%d)", name ? name : "NULL", mode);

        //DLL_TRACE(_PAL| TLEVEL (1), 0, namecopy);

    }

    /*

     * if the _dl_alloc routine was unable to allocate memory at startup

     * then no dl function will work.

     */

    if (list_iszero(&_dl_start_objects))
    {

        list_empty(&_dl_start_objects);

    }



    if (list_iszero(&_dl_all_objects))
    {

        list_empty(&_dl_all_objects);

    }





    /*

     * if passed a 0 for the pathname, returns a handle to the executable

     */

    if (!name)
    {



        struct objlist     *l;



        if(!list_isempty(&_dl_start_objects))
        {

            list_forward(&_dl_start_objects, l)
            {

                if(l->object->flags & OBJFLAG_EXECUTABLE)
                {

                    return &_dl_start_objects;

                }

                else

                {

                    _dl_error_raise(DL_E_NO_EXE);
                    _DL_TRACEOUT(1, "dlopen:_dl_start_objects are destroyed");

                }

            }

        }



        if (!(obj = _dl_alloc(sizeof * obj)))
        {
            _dl_error_raise(DL_E_NO_MEMORY);
            return 0;
        }

        memset(obj, 0x0, sizeof * obj);

        initElfSynList(&obj->symList);

        if (!(l = _dl_alloc(sizeof * l)))
        {

            _dl_error_raise(DL_E_NO_MEMORY);

            return 0;

        }



        obj->flags = OBJFLAG_INIT | OBJFLAG_FINI | OBJFLAG_EXECUTABLE | OBJFLAG_INITARRAY | OBJFLAG_FINIARRAY | OBJFLAG_RESOLVED;

        l->flags = 0;

        l->object = obj;

        l->root = &_dl_start_objects;

        list_insert(&_dl_start_objects, l);


        /*
         * place the object at the end of the all list
         */
        if (!(l = _dl_alloc(sizeof * l)))
        {
            //@@@ memory leak of obj
            _dl_error_raise(DL_E_NO_MEMORY);
            return 0;
        }
        l->flags = 0;
        l->object = obj;
        l->root = &_dl_all_objects;
        list_insert(&_dl_all_objects, l);


        return &_dl_start_objects;

    }




    /*

     * create the handle with an initially empty list of objects

     */

    if (!(handle = _dl_alloc(sizeof * handle)))
    {

        _dl_error_raise(DL_E_NO_MEMORY);
        return 0;

    }



    list_empty(handle);



    /*
     * Load the dll and tell the load_object() function that we want to
     * verify that this dll is unique with respect to the other dll's
     * which have been loaded and placed in the _dl_all_objects list.
     */
    obj = NULL;
    memset(&stkobj, 0x0, sizeof stkobj);

    libpath = dl_getenv(LIBPATHENV);

    if (load_object(&stkobj, name, libpath, NULL,
                    &obj, &_dl_all_objects) == -1)
    {
        if(debug)
        {
            _DL_TRACEOUT(1, "dlopen:load object fail");
        }
        return 0;
    }

    if (obj == NULL)
    {
        struct objlist     *l;

        if (!(obj = _dl_alloc(sizeof * obj)))
        {
            _dl_error_raise(DL_E_NO_MEMORY);
            return 0;
        }
        memcpy(obj, &stkobj, sizeof stkobj);
        /*
         * place the object at the end of the all list
         */
        if (!(l = _dl_alloc(sizeof * l)))
        {
            //@@@ memory leak of obj
            _dl_error_raise(DL_E_NO_MEMORY);
            return 0;
        }
        l->flags = 0;
        l->object = obj;
        l->root = &_dl_all_objects;
        list_insert(&_dl_all_objects, l);
    }




    /*

     * If mode indicates global binding for the object, respect that.

     */

    if((mode & RTLD_GLOBAL) && !find_object(&_dl_start_objects, obj->name, NULL))
    {

        struct objlist     *l;

        if (!(l = _dl_alloc(sizeof * l)))
        {

            _dl_error_raise(DL_E_NO_MEMORY);

            return 0;

        }

        l->flags = 0;

        l->object = obj;

        l->root = &_dl_start_objects;

        list_insert(&_dl_start_objects, l);

    }



    if (!(olist = _dl_alloc(sizeof * olist)))
    {

        _dl_error_raise(DL_E_NO_MEMORY);

        return 0;

    }

    obj->refcount++;

    olist->flags = 0;

    olist->object = obj;

    olist->root = handle;

    list_insert(handle, olist);





    /*

     * Prepend the global start list.

    */



    if( !list_iszero(&_dl_start_objects) && !list_isempty(&_dl_start_objects) )
    {



        list_forward(&_dl_start_objects, ol)
        {

            struct objlist      *l;



            if(ol->object != obj)
            {

                if (!(l = _dl_alloc(sizeof * l)))
                {

                    _dl_error_raise(DL_E_NO_MEMORY);

                    return 0;

                }

                ol->object->refcount++;

                l->flags = ol->object->flags ;

                l->object = ol->object;

                l->root = handle;

                list_insert(handle, l);

            }



        }

    }



    if(resolve(handle, RTLD_NOW) == -1)
    {

        _dl_error_raise(DL_E_UNRES_SYM);

        return 0;

    }



    /*

     * perform .init processing

     */

    init(handle);



    /*

     * perform .init_array processing

     */

    init_array(handle);



    if(debug)
    {

        _DL_TRACEOUT(1, "dlopen: return 0x%x", handle);

    }



    return handle;

}





// =============================================================================
// dlsym
// -----------------------------------------------------------------------------
/// The dlsym() function lets a process obtain the address of the symbol specified by name defined in a shared object.
/// @param: handle   A handle for a shared object
/// @param: name name of the shared object
/// @return:  A pointer to the named symbol for success, or NULL if an error occurs.
// =============================================================================
PUBLIC VOID *dlsym(VOID *handle, CONST CHAR *name)

{

    UINT32  value;

    struct object *obj;

    INT32 debug = 0;



    if(dl_getenv(DLDEBUGENV))
    {

        debug = 1;

        _DL_TRACEOUT(1, "dlsym(0x%x,%s)=", handle, name);

        //snprintf(namecopy, sizeof(namecopy)-2,"dlsym(0x%x,%s)=", handle, name);

        //DLL_TRACE(_PAL| TLEVEL (1), 0, namecopy);

    }



    if(handle == RTLD_DEFAULT)   // Do search off the global list
    {

        handle = &_dl_start_objects;

    }



    if ((value = lookup_global(name, handle, NULL, 0, &obj )))
    {

        _DL_TRACEOUT(1, "0x%x", value);

        return (VOID *)value;

    }

    _dl_error_raise(DL_E_NO_SYM);

    _DL_TRACEOUT(1, "NULL");

    return NULL;



}





PRIVATE VOID _dl_free_object(struct object *obj)

{

    // Free all sym list

    freeElfSynList(&obj->symList);

    _dl_free(obj->image);
    _dl_free(obj->name);
    _dl_free(obj);

}





PRIVATE INT32 _dlclose(VOID *handle, UINT32 flags)

{

    struct objlist      *o;

    if(handle == NULL || handle == RTLD_DEFAULT)
    {

        _dl_error_raise(DL_E_INV_HANDLE);

        return -1;

    }
    else if ((list_head_t *)handle == &_dl_start_objects)
    {

        return 0;

    }



    while(!list_atend((list_head_t *)handle, o = list_first((list_head_t *)handle)))
    {

        struct object       *obj = o->object;

        /*

         * Delete object from dll list.

         */

        list_delete(o);

        _dl_free(o);



        if (--obj->refcount == 0)
        {

            /*

             * Remove from all objects list

             */



            list_forward(&_dl_all_objects, o)
            {

                if(o->object == obj)
                {

                    list_delete(o);

                    _dl_free(o);

                    break;

                }

            }

            /*

             * If the object was also on the start (global) list, remove it.

             */

            list_forward(&_dl_start_objects, o)
            {

                if(o->object == obj)
                {

                    list_delete(o);

                    _dl_free(o);

                    break;

                }

            }



            _dl_free_object(obj);

        }



    }



    _dl_free(handle);

    return 0;



}



// =============================================================================
// dlclose

// -----------------------------------------------------------------------------
/// Close a shared object
/// @param: handle   A handle for a shared object
/// @return:  0 for success, or a nonzero value if an error occurs.
// =============================================================================
PUBLIC INT32 dlclose(VOID *handle)

{

    return _dlclose(handle, 0);

}



// =============================================================================
// dlreg

// -----------------------------------------------------------------------------
/// Close a shared object
/// @param: handle   A handle for a shared object
/// @param: name name of the symbol
/// @param: value of the symbol
/// @return:  NULL
// =============================================================================
PUBLIC VOID dlreg(VOID *handle, CHAR *name, UINT32 value)

{

    struct objlist      *o;

    struct object       *obj;

    Elf32_Sym sym = {0, 0, 0, 0, 0, 0};

    list_forward(handle, o)
    {

        obj = o->object;

        sym.st_value = value;

        if(obj->flags & OBJFLAG_EXECUTABLE)
        {

            dyn_insertSym(obj, &sym, _dl_strdup(name));

        }

    }

}



// =============================================================================
// dlerror

// -----------------------------------------------------------------------------
/// Get dynamic loading diagnostic information
/// @return:  A pointer to an error description, or NULL.
// =============================================================================
PUBLIC CHAR *dlerror(void)
{
    const char *const str = _dl_error_message();
    _dl_error_raise(NULL);
    return (char *) str;
}




// =============================================================================

// dl_allocHeapIndex

// -----------------------------------------------------------------------------

/// define a heap index dll used for malloc/// @return:  NULL.

// =============================================================================

PUBLIC VOID dl_allocHeapIndex(INT32 heap_index)

{

    dll_heapIndex = heap_index;

}




#ifdef _DLL_TEST
typedef struct
{
    int a;
    int b;
    char *pstr;
} dll_struct_t;

dll_struct_t *(*test_ptr_func)();
int (*test_add_func)(u32, u32);

int (*test_sub_func)(u32, u32);

int (*test_addr_func)();
void (*test_callback_func)();

list_head_t         *dl_handle;

list_head_t         *dl_handle_main;



VOID load_dll_test()

{



    dll_struct_t *pDllStruct = NULL;
    dll_struct_t *dll_struct;

    dll_RegisterYourself();


    dl_handle_main = dlopen(NULL, RTLD_GLOBAL);



    dlreg(dl_handle_main, "sxs_fprintf", (u32)sxs_fprintf);

    dlreg(dl_handle_main, "_sxr_StartFunctionTimer", (u32)_sxr_StartFunctionTimer);

    dl_handle = dlopen("test-add.dll", RTLD_LOCAL);



    test_add_func = ( int (*)(u32, u32))dlsym(dl_handle, "add");

    test_sub_func = ( int (*)(u32, u32))dlsym(dl_handle, "sub");
    test_addr_func = ( int (*)(u32))dlsym(dl_handle, "elf_machine_relative");
    test_ptr_func = (dll_struct_t * )dlsym(dl_handle, "dll_getStructPtr");
    test_callback_func = ( void (*)(void))dlsym(dl_handle, "test_callback");


    if(test_sub_func != NULL)

        sxs_fprintf(TSTDOUT, "test_sub_func(100,2)=%d", (int)test_sub_func(100, 2));


    if(test_add_func != NULL)

        sxs_fprintf(TSTDOUT, "test_add_func(2,3)=%d", (u32)test_add_func(2, 3));


    if(test_addr_func != NULL)
        sxs_fprintf(TSTDOUT, "test_addr_func(0)=0x%x", (u32)test_addr_func(0));

    if(test_ptr_func != NULL)
    {
        pDllStruct = test_ptr_func();
        // sxs_fprintf(TSTDOUT, "test_add_func()=0x%x",(u32)pDllStruct);
        //if(pDllStruct != NULL){
        //            sxs_fprintf(TSTDOUT, "[a,b,pstr]=0x%x,0x%x,0x%x",(u32)pDllStruct->a,pDllStruct->b, pDllStruct->pstr);
        //}
    }

    dll_struct = (dll_struct_t *)dlsym(dl_handle, "g_dll_struct");
    sxs_fprintf(TSTDOUT, "dll_struct[%x,%x,%x]", dll_struct->a, dll_struct->b, dll_struct->pstr);

    if(test_callback_func != NULL)
    {
        sxs_fprintf(TSTDOUT, "test_callback_func@0x%x", (u32)test_callback_func);
        test_callback_func();
    }




    sxs_fprintf(TSTDOUT, dl_getenv(LIBPATHENV));



}



VOID run_dl_test()

{
    UINT32 sum = 0;



    sxs_fprintf(TSTDOUT, "test_add_func=0x%x", (u32)test_add_func);



    sum = test_add_func(2, 3);



    sxs_fprintf(TSTDOUT, "test_add_func(2,3)=%d", sum);


}


VOID unload_dll_test()

{

    dlclose(dl_handle);

    dlclose(dl_handle_main);
}

#endif



