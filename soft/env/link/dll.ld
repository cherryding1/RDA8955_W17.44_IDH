
ENTRY(_start)

SECTIONS
{
    . = 0;

    . = ALIGN(4);
    .text       :
    {
      *(.text)
	  *(.text.*)
    }

    . = ALIGN(4);
    .rodata  :
    {
      *(.rodata)
      *(.rodata.*)
    }

    . = ALIGN(4);
    .data  :
    {
     *(.data)
     *(.data.*)

    }

    . = ALIGN(4);
    .sdata  :
    {
     *(.sdata)
     *(.sdata.*)
    }

    . = ALIGN(4);
    .scommon  :
    {
     *(.scommon)
     *(.scommon.*)
    }

    . = .;

    _gp = ALIGN(16)+0x7ff0;
  /*  _gp = 0x7ff0; */
    . = ALIGN(16);
    __got_start = .;
    .got  : { *(.got) }
    __got_end = .;

    . = ALIGN(4);
    .sbss  :
    {
     *(.sbss)
     *(.sbss.*)
    }
    .bss  :
    {
     *(.bss)
     *(.bss.*)
    }

 .dynamic :{ *(.dynamic)}
 .interp         : { *(.interp) }
 .hash           : { *(.hash) }
 .dynsym         : { *(.dynsym) KEEP(*(.dynsym))}
 .dynstr         : { *(.dynstr) KEEP(*(.dynstr))}
 . = ALIGN(16);
 .rel.dyn        : { *(.rel.dyn) }
 .gnu.version    : { *(.gnu.version) }
 .gnu.version_d : { *(.gnu.version_d) }
 .gnu.version_r : { *(.gnu.version_r) }

    __u_boot_cmd_start = .;
    .u_boot_cmd : { *(.u_boot_cmd) }
    __u_boot_cmd_end = .;

    uboot_end_data = .;
    num_got_entries = (__got_end - __got_start) >> 2;


    uboot_end = .;
    . = ALIGN(4);
}
