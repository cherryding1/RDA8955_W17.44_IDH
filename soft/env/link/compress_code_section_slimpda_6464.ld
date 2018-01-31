XCPU_OVERLAY_BASE_FIRST = .;

OVERLAY XCPU_OVERLAY_BASE_FIRST : NOCROSSREFS AT (_compressed_code_load_start)
{
    /* .init section is used to list all init function which is only execlute once when power on*/
    .xcpu_overlay_init_func
    {
    *(.init)
    LONG(0)
    . = ALIGN(0x20);
    }
}

#define FLASH_SIZE_OF_XCPU_OVERLAY_0 0 + SIZEOF(.xcpu_overlay_init_func) 
#define MAX_SIZE_OF_XCPU_OVERLAY_0 MAX(SIZEOF(.xcpu_overlay_init_func), 0)
_xcpu_overlay_first_max_section = MAX_SIZE_OF_XCPU_OVERLAY_0;
_xcpu_overlay_first_flash_size = FLASH_SIZE_OF_XCPU_OVERLAY_0;
XCPU_OVERLAY_BASE_SECOND = XCPU_OVERLAY_BASE_FIRST + _xcpu_overlay_first_max_section;
xcpu_overlay_flash_second = _compressed_code_load_start + _xcpu_overlay_first_flash_size;
OVERLAY XCPU_OVERLAY_BASE_SECOND : NOCROSSREFS AT (xcpu_overlay_flash_second)
{
    .overlay_2_section_1
    {
    /*  *FontRes.o(.text .text.* .rodata .rodata.*)  */
    *CustMenuRes.o(.text .text.* .rodata .rodata.*)

    /*************  JAVA  ***********************/
    *ROMImage.o(.text .text.* .rodata .rodata.*)
        LONG(0)
        . = ALIGN(0x20);
    }
    .overlay_2_section_2
    {
        LONG(0)
        . = ALIGN(0x20);
    }
    .overlay_2_section_3
    {
        LONG(0)
        . = ALIGN(0x20);
    }
}
#define FLASH_SIZE_OF_XCPU_OVERLAY_1 0 + SIZEOF(.overlay_2_section_1) + SIZEOF(.overlay_2_section_2) + SIZEOF(.overlay_2_section_3)
#define MAX_SIZE_OF_XCPU_OVERLAY_1 MAX(SIZEOF(.overlay_2_section_1), MAX(SIZEOF(.overlay_2_section_2), MAX(SIZEOF(.overlay_2_section_3),0)))
_xcpu_overlay_second_max_section = MAX_SIZE_OF_XCPU_OVERLAY_1;
_xcpu_overlay_second_flash_size = FLASH_SIZE_OF_XCPU_OVERLAY_1;
XCPU_OVERLAY_BASE_THIRD = XCPU_OVERLAY_BASE_SECOND + _xcpu_overlay_second_max_section;
xcpu_overlay_flash_third = xcpu_overlay_flash_second + _xcpu_overlay_second_flash_size;
OVERLAY XCPU_OVERLAY_BASE_THIRD : NOCROSSREFS AT (xcpu_overlay_flash_third)
{
    .overlay_3_section_1
    {
       LONG(0)
        . = ALIGN(0x20);
    }
    .overlay_3_section_2
    {
        LONG(0)
        . = ALIGN(0x20);
    }
    .overlay_3_section_3
    {

        LONG(0)
        . = ALIGN(0x20);
    }
}
#define FLASH_SIZE_OF_XCPU_OVERLAY_2 0 + SIZEOF(.overlay_3_section_1) + SIZEOF(.overlay_3_section_2) + SIZEOF(.overlay_3_section_3)
#define MAX_SIZE_OF_XCPU_OVERLAY_2 MAX(SIZEOF(.overlay_3_section_1), MAX(SIZEOF(.overlay_3_section_2), MAX(SIZEOF(.overlay_3_section_3),0)))
_xcpu_overlay_third_max_section = MAX_SIZE_OF_XCPU_OVERLAY_2;
_xcpu_overlay_third_flash_size = FLASH_SIZE_OF_XCPU_OVERLAY_2;

XCPU_OVERLAY_BASE_FOURTH = XCPU_OVERLAY_BASE_THIRD + _xcpu_overlay_third_max_section;
xcpu_overlay_flash_fourth = xcpu_overlay_flash_third + _xcpu_overlay_third_flash_size;
OVERLAY XCPU_OVERLAY_BASE_FOURTH : NOCROSSREFS AT (xcpu_overlay_flash_fourth)
{
    .overlay_4_section_1
    {
        . = ALIGN(0x20);
    }
    .overlay_4_section_2
    {
        . = ALIGN(0x20);
    }   
}
#define FLASH_SIZE_OF_XCPU_OVERLAY_3 0 + SIZEOF(.overlay_4_section_1) + SIZEOF(.overlay_4_section_2) 
#define MAX_SIZE_OF_XCPU_OVERLAY_3 MAX(SIZEOF(.overlay_4_section_1), MAX(SIZEOF(.overlay_4_section_2), 0))
_xcpu_overlay_fourth_max_section = MAX_SIZE_OF_XCPU_OVERLAY_3;
_xcpu_overlay_fourth_flash_size = FLASH_SIZE_OF_XCPU_OVERLAY_3;

_xcpu_overlay_total_ram_size = 	_xcpu_overlay_first_max_section + _xcpu_overlay_second_max_section + _xcpu_overlay_third_max_section + _xcpu_overlay_fourth_max_section;
_xcpu_overlay_flash_end = xcpu_overlay_flash_fourth  + _xcpu_overlay_fourth_flash_size;

