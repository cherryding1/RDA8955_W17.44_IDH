#!/usr/bin/env ruby
include CoolHost


module Sdmmc
    # ACMD command flag
    SDMMC_ACMD_SEL                  = 0x80000000
    # Command list.
	SDMMC_CMD_GO_IDLE_STATE         = 0
    SDMMC_CMD_MMC_SEND_OP_COND      = 1
	SDMMC_CMD_ALL_SEND_CID	        = 2
	SDMMC_CMD_SEND_RELATIVE_ADDR	= 3
	SDMMC_CMD_SET_DSR		        = 4
	SDMMC_CMD_SELECT_CARD	        = 7
    SDMMC_CMD_SEND_IF_COND          = 8
	SDMMC_CMD_SEND_CSD		        = 9
	SDMMC_CMD_STOP_TRANSMISSION	    = 12
	SDMMC_CMD_SEND_STATUS	        = 13
	SDMMC_CMD_SET_BLOCKLEN	        = 16
	SDMMC_CMD_READ_SINGLE_BLOCK	    = 17
	SDMMC_CMD_READ_MULT_BLOCK		= 18
	SDMMC_CMD_WRITE_SINGLE_BLOCK	= 24
	SDMMC_CMD_WRITE_MULT_BLOCK	    = 25
	SDMMC_CMD_APP_CMD		        = 55
	SDMMC_CMD_SET_BUS_WIDTH		    = (6 | SDMMC_ACMD_SEL)
	SDMMC_CMD_SEND_NUM_WR_BLOCKS	= (22| SDMMC_ACMD_SEL)
	SDMMC_CMD_SET_WR_BLK_COUNT	    = (23| SDMMC_ACMD_SEL)
	SDMMC_CMD_SEND_OP_COND	        = (41| SDMMC_ACMD_SEL)

    
    #=============================================================================
    #hal_SdmmcSendCmd
    #-----------------------------------------------------------------------------
    # Send a command to a SD card plugged to the sdmmc port.
    # @param cmd Command
    # @param arg Argument of the command
    # @param suspend Feature not implemented yet.
    #=============================================================================
    def hal_SdmmcSendCmd(cmd, arg)
        $SDMMC.SDMMC_CONFIG.w 0x00000000
        
        configReg = 0
        # configReg = $SDMMC.SDMMC_CONFIG.wl(0,0)
        configReg = $SDMMC.SDMMC_CONFIG.prepl(configReg)

        case cmd
            when SDMMC_CMD_GO_IDLE_STATE
                configReg = $SDMMC.SDMMC_CONFIG.SDMMC_SENDCMD.wl(configReg, 1)
                

                
            when SDMMC_CMD_ALL_SEND_CID
                configReg = $SDMMC.SDMMC_CONFIG.RSP_SEL.wl(configReg, 2) # R2
                configReg = $SDMMC.SDMMC_CONFIG.RSP_EN.wl(configReg, 1)
                configReg = $SDMMC.SDMMC_CONFIG.SDMMC_SENDCMD.wl(configReg, 1)
                # 0x51
                

            when SDMMC_CMD_SEND_RELATIVE_ADDR
                configReg = $SDMMC.SDMMC_CONFIG.RSP_SEL.wl(configReg, 0)
                configReg = $SDMMC.SDMMC_CONFIG.RSP_EN.wl(configReg, 1)
                configReg = $SDMMC.SDMMC_CONFIG.SDMMC_SENDCMD.wl(configReg, 1)
                # 0x11
                

            when SDMMC_CMD_SEND_IF_COND
                configReg = $SDMMC.SDMMC_CONFIG.RSP_SEL.wl(configReg, 0)
                configReg = $SDMMC.SDMMC_CONFIG.RSP_EN.wl(configReg, 1)
                configReg = $SDMMC.SDMMC_CONFIG.SDMMC_SENDCMD.wl(configReg, 1)
                # 0x11
                

            when SDMMC_CMD_SET_DSR
                configReg = $SDMMC.SDMMC_CONFIG.SDMMC_SENDCMD.wl(configReg, 1)
                

            when SDMMC_CMD_SELECT_CARD
                configReg = $SDMMC.SDMMC_CONFIG.RSP_SEL.wl(configReg, 0)
                configReg = $SDMMC.SDMMC_CONFIG.RSP_EN.wl(configReg, 1)
                configReg = $SDMMC.SDMMC_CONFIG.SDMMC_SENDCMD.wl(configReg, 1)
                

            when SDMMC_CMD_SEND_CSD		        
                configReg = $SDMMC.SDMMC_CONFIG.RSP_SEL.wl(configReg, 2) # R2
                configReg = $SDMMC.SDMMC_CONFIG.RSP_EN.wl(configReg, 1)
                configReg = $SDMMC.SDMMC_CONFIG.SDMMC_SENDCMD.wl(configReg, 1)
                

            when SDMMC_CMD_STOP_TRANSMISSION	
                # TODO
                configReg = 0
                

            when SDMMC_CMD_SEND_STATUS	        
                configReg = $SDMMC.SDMMC_CONFIG.RSP_SEL_OTHER .wl(configReg, 1)
                configReg = $SDMMC.SDMMC_CONFIG.RSP_EN.wl(configReg, 1)
                configReg = $SDMMC.SDMMC_CONFIG.SDMMC_SENDCMD.wl(configReg, 1)
                

            when SDMMC_CMD_SET_BLOCKLEN	        
                configReg = $SDMMC.SDMMC_CONFIG.RSP_SEL.wl(configReg, 0)
                configReg = $SDMMC.SDMMC_CONFIG.RSP_EN.wl(configReg, 1)
                configReg = $SDMMC.SDMMC_CONFIG.SDMMC_SENDCMD.wl(configReg, 1)
                

            when SDMMC_CMD_READ_SINGLE_BLOCK	    
                configReg = $SDMMC.SDMMC_CONFIG.SDMMC_RD_WT_SEL_READ.wl(configReg, 1)
                configReg = $SDMMC.SDMMC_CONFIG.SDMMC_RD_WT_EN.wl(configReg, 1)
                configReg = $SDMMC.SDMMC_CONFIG.RSP_SEL.wl(configReg, 0)
                configReg = $SDMMC.SDMMC_CONFIG.RSP_EN.wl(configReg, 1)
                configReg = $SDMMC.SDMMC_CONFIG.SDMMC_SENDCMD.wl(configReg, 1)
                # 0x111
                

            when SDMMC_CMD_READ_MULT_BLOCK		
                configReg = $SDMMC.SDMMC_CONFIG.SDMMC_S_M_SEL_MULTIPLE.wl(configReg, 1)
                configReg = $SDMMC.SDMMC_CONFIG.SDMMC_RD_WT_SEL_READ.wl(configReg, 1)
                configReg = $SDMMC.SDMMC_CONFIG.SDMMC_RD_WT_EN.wl(configReg, 1)
                configReg = $SDMMC.SDMMC_CONFIG.RSP_SEL.wl(configReg, 0)
                configReg = $SDMMC.SDMMC_CONFIG.RSP_EN.wl(configReg, 1)
                configReg = $SDMMC.SDMMC_CONFIG.SDMMC_SENDCMD.wl(configReg, 1)
                # 0x511
                

            when SDMMC_CMD_WRITE_SINGLE_BLOCK	
                configReg = $SDMMC.SDMMC_CONFIG.SDMMC_RD_WT_SEL_WRITE.wl(configReg, 1)
                configReg = $SDMMC.SDMMC_CONFIG.SDMMC_RD_WT_EN.wl(configReg, 1)
                configReg = $SDMMC.SDMMC_CONFIG.RSP_SEL.wl(configReg, 0)
                configReg = $SDMMC.SDMMC_CONFIG.RSP_EN.wl(configReg, 1)
                configReg = $SDMMC.SDMMC_CONFIG.SDMMC_SENDCMD.wl(configReg, 1)
                # 0x311
                

            when SDMMC_CMD_WRITE_MULT_BLOCK	    
                configReg = $SDMMC.SDMMC_CONFIG.SDMMC_S_M_SEL_MULTIPLE.wl(configReg, 1)
                configReg = $SDMMC.SDMMC_CONFIG.SDMMC_RD_WT_SEL_WRITE.wl(configReg, 1)
                configReg = $SDMMC.SDMMC_CONFIG.SDMMC_RD_WT_EN.wl(configReg, 1)
                configReg = $SDMMC.SDMMC_CONFIG.RSP_SEL.wl(configReg, 0)
                configReg = $SDMMC.SDMMC_CONFIG.RSP_EN.wl(configReg, 1)
                configReg = $SDMMC.SDMMC_CONFIG.SDMMC_SENDCMD.wl(configReg, 1)
                # 0x711
                

            when SDMMC_CMD_APP_CMD		        
                configReg = $SDMMC.SDMMC_CONFIG.RSP_SEL.wl(configReg, 0)
                configReg = $SDMMC.SDMMC_CONFIG.RSP_EN.wl(configReg, 1)
                configReg = $SDMMC.SDMMC_CONFIG.SDMMC_SENDCMD.wl(configReg, 1)
                # 0x11
                

            when SDMMC_CMD_SET_BUS_WIDTH		    
                configReg = $SDMMC.SDMMC_CONFIG.RSP_SEL.wl(configReg, 0)
                configReg = $SDMMC.SDMMC_CONFIG.RSP_EN.wl(configReg, 1)
                configReg = $SDMMC.SDMMC_CONFIG.SDMMC_SENDCMD.wl(configReg, 1)
                # 0x11
                

            when SDMMC_CMD_SEND_NUM_WR_BLOCKS	
                configReg = $SDMMC.SDMMC_CONFIG.SDMMC_RD_WT_SEL_READ.wl(configReg, 1)
                configReg = $SDMMC.SDMMC_CONFIG.SDMMC_RD_WT_EN.wl(configReg, 1)
                configReg = $SDMMC.SDMMC_CONFIG.RSP_SEL.wl(configReg, 0)
                configReg = $SDMMC.SDMMC_CONFIG.RSP_EN.wl(configReg, 1)
                configReg = $SDMMC.SDMMC_CONFIG.SDMMC_SENDCMD.wl(configReg, 1)
                # 0x111
                

            when SDMMC_CMD_SET_WR_BLK_COUNT	    
                configReg = $SDMMC.SDMMC_CONFIG.RSP_SEL.wl(configReg, 0)
                configReg = $SDMMC.SDMMC_CONFIG.RSP_EN.wl(configReg, 1)
                configReg = $SDMMC.SDMMC_CONFIG.SDMMC_SENDCMD.wl(configReg, 1)
                # 0x11
                

            when SDMMC_CMD_MMC_SEND_OP_COND
            when SDMMC_CMD_SEND_OP_COND
                configReg = $SDMMC.SDMMC_CONFIG.RSP_SEL.wl(configReg, 1) # R3
                configReg = $SDMMC.SDMMC_CONFIG.RSP_EN.wl(configReg, 1)
                configReg = $SDMMC.SDMMC_CONFIG.SDMMC_SENDCMD.wl(configReg, 1)
                # 0x31
                

            else
                # TODO Raise excpetion ?
                puts "SDMMC: Unknown command %d" % [cmd]
            end
                
        # TODO Add suspend management
        $SDMMC.SDMMC_CMD_INDEX.COMMAND.w cmd
        $SDMMC.SDMMC_CMD_ARG.ARGUMENT.w arg
        $SDMMC.SDMMC_CONFIG.w configReg

    end





    addHelpEntry("SDMMC", "sdmmc_SendCmd", "cmd, arg", "", "Send a command to the SD card")
    def sdmmc_SendCmd(cmd, arg)
        puts "Send SD command %d with argument %#x" % [cmd, arg]
        hal_SdmmcSendCmd(cmd, arg)
    end

end

# Include defined Sdmmc Module.
include Sdmmc


