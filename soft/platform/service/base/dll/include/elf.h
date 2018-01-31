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


#ifndef _ELF_H_
#define _ELF_H_
//#pragma pack(1)
//typedef unsigned char BYTE;
/*                      */
/* 32-bit ELF 数据结构. */
typedef unsigned long   Elf32_Addr;
typedef unsigned short  Elf32_Half;
typedef unsigned long   Elf32_Off;
typedef unsigned long   Elf32_Sword;
typedef unsigned long   Elf32_Word;


/*               */
/* ELF head table */
#define EI_MAG0     0       /* e_ident[] indexes */
#define EI_MAG1     1
#define EI_MAG2     2
#define EI_MAG3     3
#define EI_CLASS    4
#define EI_DATA     5
#define EI_VERSION  6
#define EI_OSABI    7
#define EI_PAD      8

#define ELFMAG0     0x7f        /* EI_MAG */
#define ELFMAG1     'E'
#define ELFMAG2     'L'
#define ELFMAG3     'F'
#define ELFMAG      "\177ELF"
#define SELFMAG     4

#define EI_NIDENT 16
struct elf32_hdr
{
    unsigned char   e_ident[EI_NIDENT];
    Elf32_Half  e_type;     //filt type
    Elf32_Half  e_machine;  //target machine
    Elf32_Word  e_version;  //file version
    Elf32_Addr  e_entry;    /* Entry point */
    Elf32_Off       e_phoff;    //program head table's file offset
    Elf32_Off       e_shoff;    //section head table's file offset
    Elf32_Word  e_flags;    //
    Elf32_Half  e_ehsize;   //elf head's size
    Elf32_Half  e_phentsize;//the size of the entry in the program head table
    Elf32_Half  e_phnum;    //the number of the entry in the program head table
    Elf32_Half  e_shentsize;//the size of the entry in the program head table
    Elf32_Half  e_shnum;    //the number of the entry in the section head table
    Elf32_Half  e_shstrndx; //the section head table index of the entry associate with the section name string table
};

#define EV_NONE     0       /* e_version, EI_VERSION */
#define EV_CURRENT  1
#define EV_NUM      2

/* These constants define the different elf file types */
#define ET_NONE   0
#define ET_REL    1
#define ET_EXEC   2
#define ET_DYN    3
#define ET_CORE   4
#define ET_LOPROC 0xff00
#define ET_HIPROC 0xffff

/*                       */
/* Entry of segment header */
struct elf32_phdr
{
    Elf32_Word  p_type;
    Elf32_Off       p_offset;
    Elf32_Addr  p_vaddr;
    Elf32_Addr  p_paddr;
    Elf32_Word  p_filesz;
    Elf32_Word  p_memsz;
    Elf32_Word  p_flags;
    Elf32_Word  p_align;
};

/* These constants are for the segment types stored in the image headers */
#define PT_NULL    0
#define PT_LOAD    1
#define PT_DYNAMIC 2
#define PT_INTERP  3
#define PT_NOTE    4
#define PT_SHLIB   5
#define PT_PHDR    6
#define PT_LOPROC  0x70000000
#define PT_HIPROC  0x7fffffff

/* These constants define the permissions on sections in the program
   header, p_flags. */
#define PF_R            0x4             /* Segment is readable */
#define PF_W            0x2             /* Segment is writable */
#define PF_X            0x1             /* Segment is executable */
#define PF_MASKOS       0x0ff00000      /* OS-specific */
#define PF_MASKPROC     0xf0000000      /* Processor-specific */


/*          */
/*entry of section table*/
/* special section indexes */
#define SHN_UNDEF   0
#define SHN_LORESERVE   0xff00
#define SHN_LOPROC  0xff00
#define SHN_HIPROC  0xff1f
#define SHN_ABS     0xfff1
#define SHN_COMMON  0xfff2
#define SHN_HIRESERVE   0xffff

struct elf32_shdr
{
    Elf32_Word  sh_name;        //the name of the section, index into section head string table
    Elf32_Word  sh_type;        //section type
    Elf32_Word  sh_flags;       //writable?? Alloc?? execinstr??
    Elf32_Addr  sh_addr;        //address in the memory, if it will appear in the memory
    Elf32_Off       sh_offset;      //address in the file
    Elf32_Word  sh_size;        //section's size
    Elf32_Word  sh_link;        //Index link
    Elf32_Word  sh_info;        //extra information
    Elf32_Word  sh_addralign;   //align
    Elf32_Word  sh_entsize;     //if this section holds a table of fixed size entry, the member gives the size of the entry
};

/* sh_type */
#define SHT_NULL    0
#define SHT_PROGBITS    1       //the format and meaning determined solely by the program
#define SHT_SYMTAB  2           //symbol table for link editing, can also use in dynamic link
#define SHT_STRTAB  3           //string table
#define SHT_RELA    4           //holds relocation entry with explicit addent
#define SHT_HASH    5           //symbol hash table, for dynamic link symbol
#define SHT_DYNAMIC 6           //holds information for dynamic link
#define SHT_NOTE    7
#define SHT_NOBITS  8           //no bit
#define SHT_REL     9           //holds relocation entry withouw explicit addent
#define SHT_SHLIB   10          //reserved
#define SHT_DYNSYM  11          //minimal set of dynamic linking symbol
#define SHT_NUM     12
#define SHT_LOPROC  0x70000000
#define SHT_HIPROC  0x7fffffff
#define SHT_LOUSER  0x80000000
#define SHT_HIUSER  0xffffffff

//sh_link and sh_info
//sh_type           sh_link                     sh_info
//SHT_DYNAMIC       index of string table
//                  used by entry
//SHT_HASH          index of symbol table
//                  to which the hash applies
//SHT_REL           index of associate symbol   index of the section to
//SHT_RELA          table                       which the relocation apliew
//SHT_SYMTAL        index of the associate
//SHT_DYNTAL        string table                one greater than the last local
//                                              symbol(binding with STB_LOCAL)

//.bss          SHT_NOBITS          SHF_ALLOC + SHF_WRITE
//.data         SHT_PROGBITS        SHF_ALLOC + SHF_WRITE
//.dynamic      SHT_DYNAMIC         see below
//.dynstr       SHT_STRTAB          SHF_ALLOC
//.dynsym       SHT_DYNSYM          SHF_ALLOC
//.hash         SHT_HASH            SHF_ALLOC
//.plt          SHT_PROGBITS        see below
//.rel.data     SHT_REL             see below
//.rel.text     SHT_REL             see below
//.rodata       SHT_PROGBITS        SHF_ALLOC
//.text         SHT_PROGBITS        SHF_ALLOC + SHF_EXECINSTR
//.shstrtab     SHT_STRTAB          none
//.strtab       SHT_STRTAB          see below
//.symtab       SHT_SYMTAB          see below


/* sh_flags */
#define SHF_WRITE   0x1             //writable
#define SHF_ALLOC   0x2             //occupies in memory
#define SHF_EXECINSTR   0x4         //contains executable machine instruction
#define SHF_MASKPROC    0xf0000000













/* This is the info that is needed to parse the dynamic section of the file */
#define DT_NULL     0
#define DT_NEEDED   1
#define DT_PLTRELSZ 2
#define DT_PLTGOT   3
#define DT_HASH     4
#define DT_STRTAB   5
#define DT_SYMTAB   6
#define DT_RELA     7
#define DT_RELASZ   8
#define DT_RELAENT  9
#define DT_STRSZ    10
#define DT_SYMENT   11
#define DT_INIT     12
#define DT_FINI     13
#define DT_SONAME   14
#define DT_RPATH    15
#define DT_SYMBOLIC 16
#define DT_REL      17
#define DT_RELSZ    18
#define DT_RELENT   19
#define DT_PLTREL   20
#define DT_DEBUG    21
#define DT_TEXTREL  22
#define DT_JMPREL   23
#define DT_LOPROC   0x70000000
#define DT_HIPROC   0x7fffffff


#define ELF_ST_BIND(x)      ((x) >> 4)
#define ELF_ST_TYPE(x)      (((unsigned int) x) & 0xf)
#define ELF32_ST_BIND(x)    ELF_ST_BIND(x)
#define ELF32_ST_TYPE(x)    ELF_ST_TYPE(x)
#define ELF64_ST_BIND(x)    ELF_ST_BIND(x)
#define ELF64_ST_TYPE(x)    ELF_ST_TYPE(x)

struct dynamic
{
    Elf32_Sword d_tag;
    union
    {
        Elf32_Sword d_val;
        Elf32_Addr  d_ptr;
    } d_un;
} ;


/*      */
/* The following are used with relocations */
struct elf32_rel
{
    Elf32_Addr  r_offset;           //the location to which apply the relocation action. for relocatable file, it specify the offset to the section. and for others specify the virtual address
    Elf32_Word  r_info;             //SYMBOL<<8+TYPE&0xff. symbol is the symbol table index, type is in page 29 of elf_format.pdf
} ;

#define     R_386_32    1
#define     R_386_PC32  2
//some type
//R_386_32      1       word32      S + A
//R_386_PC32    2       word32      S + A - P


struct elf32_rela
{
    Elf32_Addr  r_offset;
    Elf32_Word  r_info;
    Elf32_Sword r_addend;
} ;

#define ELF32_R_SYM(x) ((x) >> 8)
#define ELF32_R_TYPE(x) ((x) & 0xff)


/*          */
/* This info is needed when parsing the symbol table */
//symbol entry
struct elf32_sym
{
    Elf32_Word  st_name;        //index into the symbol string table
    Elf32_Addr  st_value;
    Elf32_Word  st_size;        //size of the symbol. 0 for no size or unkown size
    unsigned char   st_info;        //BIND<<4+TYPE&0x0f
    unsigned char   st_other;       //0 for reserve
    Elf32_Half  st_shndx;       //relevant section table index, some indicates special meanings
} ;
//TYPE
#define STT_NOTYPE  0
#define STT_OBJECT  1           //data object
#define STT_FUNC    2
#define STT_SECTION 3
#define STT_FILE    4
#define STT_COMMON  5
#define STT_TLS     6
//BIND
#define STB_LOCAL  0
#define STB_GLOBAL 1
#define STB_WEAK   2
//st_value
//  In relocatable files, st_value holds alignment constraints for a symbol whose section index is SHN_COMMON.
//  In relocatable files, st_value holds a section offset for a defined symbol. That is, st_value is an
//offset from the beginning of the section that st_shndx identifies.
//  In executable and shared object files, st_value holds a virtual address. To make these files’ symbols
//more useful for the dynamic linker, the section offset (file interpretation) gives way to a virtual
//address (memory interpretation) for which the section number is irrelevant.















#define EI_NIDENT   16

#define ELFOSABI_NONE   0
#define ELFOSABI_LINUX  3

#ifndef ELF_OSABI
#define ELF_OSABI ELFOSABI_NONE
#endif

/* Notes used in ET_CORE */
#define NT_PRSTATUS 1
#define NT_PRFPREG  2
#define NT_PRPSINFO 3
#define NT_TASKSTRUCT   4
#define NT_AUXV     6
#define NT_PRXFPREG     0x46e62b7f      /* copied from gdb5.1/include/elf/common.h */

/* Note header in a PT_NOTE section */
typedef struct elf32_note
{
    Elf32_Word  n_namesz;   /* Name size */
    Elf32_Word  n_descsz;   /* Content size */
    Elf32_Word  n_type;     /* Content type */
} Elf32_Nhdr;

#define elf_hdr     elf32_hdr
#define elf_phdr    elf32_phdr
#define elf_shdr    elf32_shdr
#define elf_note    elf32_note
#define elf_addr_t  Elf32_Off

#endif

