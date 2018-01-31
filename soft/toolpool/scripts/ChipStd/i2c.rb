#!/usr/bin/env ruby
include CoolHost
require 'help.rb'

I2C_MASTER_WR = (0x1<<12)
I2C_MASTER_STA = (0x1<<16)
IRQ_STATUS = (0x1<<4)
I2C_MASTER_IRQ_CLR = (0x1<<0)
I2C_MASTER_RD = (0x1<<4)
I2C_MASTER_ACK = (0x1<<0)
I2C_MASTER_STO = (0x1<<8)

addHelpEntry("i2c","i2cw","mem_addr, value","","write 'value' in the 'mem_addr' register in device 0x10.")
def i2cw(mem_addr, val)
    # write slave address
    command = (0x20);
	$I2C_MASTER.TXRX_BUFFER.w(command);

	command = I2C_MASTER_WR | I2C_MASTER_STA;
	$I2C_MASTER.CMD.w(command);

	# polling on IRQ
	status = 0;
	while(status==0)
	  status = $I2C_MASTER.STATUS.R & IRQ_STATUS;
	end  

	# clear IRQ
	command = I2C_MASTER_IRQ_CLR;
	$I2C_MASTER.IRQ_CLR.w(command);

	# write memory address
	command = mem_addr;
	$I2C_MASTER.TXRX_BUFFER.w(command);

	command = I2C_MASTER_WR;
	$I2C_MASTER.CMD.w(command);

	# polling on IRQ
	status = 0;
	while(status==0)
	  status = $I2C_MASTER.STATUS.R & IRQ_STATUS;
	end 	
	
	# clear IRQ
	command = I2C_MASTER_IRQ_CLR;
	$I2C_MASTER.IRQ_CLR.w(command);	

    # write slave address
    $I2C_MASTER.TXRX_BUFFER.w(val);

	command = I2C_MASTER_WR | I2C_MASTER_STO;
	$I2C_MASTER.CMD.w(command);

	# polling on IRQ
	status = 0;
	while(status==0)
	  status = $I2C_MASTER.STATUS.R & IRQ_STATUS;
	end  

	# clear IRQ
	command = I2C_MASTER_IRQ_CLR;
	$I2C_MASTER.IRQ_CLR.w(command);
	
end

addHelpEntry("i2c","i2cr","mem_addr","value","read value in the 'mem_addr' register in device of slave address 0x10.")
def i2cr(mem_addr)
    # write slave address
    command = (0x20);
	$I2C_MASTER.TXRX_BUFFER.w(command);

	command = I2C_MASTER_WR | I2C_MASTER_STA;
	$I2C_MASTER.CMD.w(command);

	# polling on IRQ
	status = 0;
	while(status==0)
	  status = $I2C_MASTER.STATUS.R & IRQ_STATUS;
	end  

	# clear IRQ
	command = I2C_MASTER_IRQ_CLR;
	$I2C_MASTER.IRQ_CLR.w(command);

	# write memory address
	command = mem_addr;
	$I2C_MASTER.TXRX_BUFFER.w(command);

	command = I2C_MASTER_WR;
	$I2C_MASTER.CMD.w(command);

	# polling on IRQ
	status = 0;
	while(status==0)
	  status = $I2C_MASTER.STATUS.R & IRQ_STATUS;
	end 	
	
	# clear IRQ
	command = I2C_MASTER_IRQ_CLR;
	$I2C_MASTER.IRQ_CLR.w(command);	
	
	# write slave address + R/W = '1'
    command = (0x21);
	$I2C_MASTER.TXRX_BUFFER.w(command);

	command = I2C_MASTER_WR | I2C_MASTER_STA;
	$I2C_MASTER.CMD.w(command);

	# polling on IRQ
	status = 0;
	while(status==0)
	  status = $I2C_MASTER.STATUS.R & IRQ_STATUS;
	end 	
	
	# clear IRQ
	command = I2C_MASTER_IRQ_CLR;
	$I2C_MASTER.IRQ_CLR.w(command);	
 
	# read value
	command = I2C_MASTER_RD | I2C_MASTER_ACK | I2C_MASTER_STO;
	$I2C_MASTER.CMD.w(command);	

	# polling on IRQ
	status = 0;
	while(status==0)
	  status = $I2C_MASTER.STATUS.R & IRQ_STATUS;
	end   	
	
	# clear IRQ
	command = I2C_MASTER_IRQ_CLR;
	$I2C_MASTER.IRQ_CLR.w(command);		
	
	#return value
	hex_addr = format("%02X", mem_addr);
	value = $I2C_MASTER.TXRX_BUFFER.R & 0xff;
	hex_value = format("%02X", value);
	puts("REG 0x#{hex_addr} : 0x#{hex_value}");
	return(value);
	
end

addHelpEntry("i2c","i2cR","","value","read value in register [from 0x0 to 0x1b] in device of slave address 0x10.")
def i2cR()
	i = 0x0;
	while(i<=0x1b)
	  i2cr(i);
	  i = i+1;
	end 
end




