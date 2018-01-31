def rda5800_Write(addr, data)
    # warning: if not done by the embedded code, need to open
    # the SPI here !
    wrData = 0
    reorder = []

    wrData = ((addr & 0xf0) << 24) | (0<<27) | ((addr & 0x0f) << 23) | (data << 7)

    reorder[0] = ((wrData & 0xff000000) >> 24)
    reorder[1] = ((wrData & 0x00ff0000) >> 16)
    reorder[2] = ((wrData & 0x0000ff00) >> 8)
    reorder[3] = ((wrData & 0x000000ff) >> 0)

    4.times {|i|  $SPI2.rxtx_buffer.wf reorder[i]}
end




def rda5800_Read(addr)
    # warning: if not done by the embedded code, need to open
    # the SPI here !
    wrData = 0
    reorder = []

    wrData = ((addr & 0xf0) << 24) | (1<<27) | ((addr & 0x0f) << 23) | (0 << 7)

    reorder[0] = ((wrData & 0xff000000) >> 24)
    reorder[1] = ((wrData & 0x00ff0000) >> 16)
    reorder[2] = ((wrData & 0x0000ff00) >> 8)
    reorder[3] = ((wrData & 0x000000ff) >> 0)

    # Send Read Command
    4.times {|i|  $SPI2.rxtx_buffer.wf reorder[i]}

    # Get result
    4.times {|i|  reorder[i] = $SPI2.rxtx_buffer.RF ; puts "read %02x @ %d" % [reorder[i] , i]}

    # Frame size being 25, only the first LSB bit of the last received byte 
    # contains proper data, thus the masking.
    rdData = reorder[0] << 24 | reorder[1] << 16 | reorder[2] << 8 | (reorder[3] & 0x1) << 7
    
    # We keep only the register value, ie the data payload
    reg =  ((rdData & (0xFFFF << 7)) >> 7)
    xp reg


end



