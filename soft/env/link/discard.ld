/* All sections we want to discard from the final linked elf.*/

#define DEBUG_SECTION(a)        \
    a   (0) : { *(EXCLUDE_FILE(*.elf) a ) }

#define DEBUG_SECTION2(a, b)    \
    a   (0) : { *(EXCLUDE_FILE(*.elf) a b ) }

    /*
     * DWARF debug sections.
     * Symbols in the DWARF debugging sections are relative to
     * the beginning of the section so we begin them at 0.
     */
    /* DWARF 1 */
    DEBUG_SECTION(.debug)
    DEBUG_SECTION(.line)
    /* GNU DWARF 1 extensions */
    DEBUG_SECTION(.debug_srcinfo)
    DEBUG_SECTION(.debug_sfnames)
    /* DWARF 1.1 and DWARF 2 */
    DEBUG_SECTION(.debug_aranges)
    DEBUG_SECTION(.debug_pubnames)
    /* DWARF 2 */
    DEBUG_SECTION2(.debug_info, .gnu.linkonce.wi.*)
    DEBUG_SECTION(.debug_abbrev)
    DEBUG_SECTION(.debug_line)
    DEBUG_SECTION(.debug_frame)
    DEBUG_SECTION(.debug_str)
    DEBUG_SECTION(.debug_loc)
    DEBUG_SECTION(.debug_macinfo)
    /* SGI/MIPS DWARF 2 extensions */
    DEBUG_SECTION(.debug_weaknames)
    DEBUG_SECTION(.debug_funcnames)
    DEBUG_SECTION(.debug_typenames)
    DEBUG_SECTION(.debug_varnames)

    /* Stabs debugging sections.  */
    DEBUG_SECTION(.stab)
    DEBUG_SECTION(.stabstr)
    DEBUG_SECTION(.stab.excl)
    DEBUG_SECTION(.stab.exclstr)
    DEBUG_SECTION(.stab.index)
    DEBUG_SECTION(.stab.indexstr)
    DEBUG_SECTION(.comment)

    /* These must appear regardless of  .  */
    .gptab.sdata :
    {
        *(.gptab.data)
        *(.gptab.sdata)
    }
    .gptab.sbss :
    {
        *(.gptab.bss)
        *(.gptab.sbss)
    }

    /DISCARD/ :
    {
#if (DISCARD_UNLISTED_SECTIONS == 1)
        /* Error if a required input section is not listed in any output section */
        /* Useful to check the missing section descriptions */
        *(*)
#else
        /* We don't build a relocatable object */
        *(.rel.dyn)
        *(.note.GNU-stack)
        /* The eh_frame data is used to unwind the stack when an exception is
         * thrown. It records things like where to find the return address and
         * which registers are stored where.
         * (From: http://archives.devshed.com/forums/development-94/how-is-the-eh-frame-section-used-2316212.html
         */
        *(.eh_frame)
        /* C++ constructors. It should not be used. */
        /*(.ctors)*/
#endif
    }


