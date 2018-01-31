def flash_write_loop
    512.times { |sector|
        single_write((sector*0x8000), sector)
        puts "loop %d addr 0x%08x" % [sector, (sector*0x8000)] 
    }
end

def flash_check_loop
    512.times { |sector|
        a=R16(0x01000000+(sector*0x8000))
        puts "addr 0x%08x" % (sector*0x8000)
    }
end

def single_write (wr_addr, wr_val)
    # single 16 bit write to wr_addr.  Can be physical or logical address
    # as the flash base address is automatically added.
    w16(0x01000aaa,0x00aa)
    w16(0x01000554,0x0055)
    w16(0x01000aaa,0x00a0)
    w16(0x01000000|(wr_addr),wr_val)
end

def flash_erase_sector (er_addr)
    # single sector erase.  Erase address can be the logical or physical
    # start address of the sector to be erased.
    w16(0x01000aaa,0x00aa)
    w16(0x01000554,0x0055)
    w16(0x01000aaa,0x0080)
    w16(0x01000aaa,0x00aa)
    w16(0x01000554,0x0055)
    w16(0x01000000|er_addr,0x0030)
end

def flash_asp_entry(bank_addr)
    w16(0x01000aaa,0x00aa)
    w16(0x01000554,0x0055)
    w16(0x01000AAA|(bank_addr),0x00E0)
end

def flash_asp_dyb_prog(bank_sect_addr,val)
    w16(0x01000000|(bank_sect_addr),0x00A0)
    w16(0x01000000|(bank_sect_addr),val)
end

def flash_asp_dyb_read(bank_sect_addr)
    r16(0x01000000|(bank_sect_addr))
end

def flash_asp_exit()
    w16(0x01000000,0x0090)
    w16(0x01000000,0x0000)
end

