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







BYTE  Map1_Regs[ 4 ];
DWORD Map1_Cnt;
BYTE  Map1_Latch;
WORD  Map1_Last_Write_Addr;

typedef enum
{
    Map1_SMALL,
    Map1_512K,
    Map1_1024K
} Map1_Size_t;

Map1_Size_t Map1_Size;
DWORD Map1_256K_base;
DWORD Map1_swap;

DWORD Map1_bank1;
DWORD Map1_bank2;
DWORD Map1_bank3;
DWORD Map1_bank4;

DWORD Map1_HI1;
DWORD Map1_HI2;




void Map1_Init()
{
    DWORD size_in_K;


    MapperInit = Map1_Init;


    MapperWrite = Map1_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map0_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map0_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    Map1_Cnt = 0;
    Map1_Latch = 0x00;

    Map1_Regs[ 0 ] = 0x0c;
    Map1_Regs[ 1 ] = 0x00;
    Map1_Regs[ 2 ] = 0x00;
    Map1_Regs[ 3 ] = 0x00;

    size_in_K = ( NesHeader.byRomSize << 1 ) * 8;

    if ( size_in_K == 1024 )
    {
        Map1_Size = Map1_1024K;
    }
    else if(size_in_K == 512)
    {
        Map1_Size = Map1_512K;
    }
    else
    {
        Map1_Size = Map1_SMALL;
    }

    Map1_256K_base = 0;   Map1_swap = 0;

    if( Map1_Size == Map1_SMALL )
    {
        Map1_HI1 = ( NesHeader.byRomSize << 1 ) - 2;
        Map1_HI2 = ( NesHeader.byRomSize << 1 ) - 1;
    }
    else
    {
        Map1_HI1 = ( 256 / 8 ) - 2;
        Map1_HI2 = ( 256 / 8 ) - 1;
    }

    Map1_bank1 = 0;
    Map1_bank2 = 1;
    Map1_bank3 = Map1_HI1;
    Map1_bank4 = Map1_HI2;


    Map1_set_ROM_banks();


    K6502_Set_Int_Wiring( 1, 1 );
}

void Map1_set_ROM_banks()
{
    ROMBANK0 = ROMPAGE( ( (Map1_256K_base << 5) + (Map1_bank1 & ((256/8)-1)) ) % ( NesHeader.byRomSize << 1 ) );
    ROMBANK1 = ROMPAGE( ( (Map1_256K_base << 5) + (Map1_bank2 & ((256/8)-1)) ) % ( NesHeader.byRomSize << 1 ) );
    ROMBANK2 = ROMPAGE( ( (Map1_256K_base << 5) + (Map1_bank3 & ((256/8)-1)) ) % ( NesHeader.byRomSize << 1 ) );
    ROMBANK3 = ROMPAGE( ( (Map1_256K_base << 5) + (Map1_bank4 & ((256/8)-1)) ) % ( NesHeader.byRomSize << 1 ) );
}




void Map1_Write( WORD wAddr, BYTE byData )
{

    DWORD dwRegNum;

    if( ( wAddr & 0x6000 ) != ( Map1_Last_Write_Addr & 0x6000 ) )
    {
        Map1_Cnt = 0;
        Map1_Latch = 0x00;
    }
    Map1_Last_Write_Addr = wAddr;

    if ( byData & 0x80 )
    {
        Map1_Cnt = 0;
        Map1_Latch = 0x00;
        return;
    }

    if ( byData & 0x01 ) Map1_Latch |= ( 1 << Map1_Cnt );
    Map1_Cnt++;
    if( Map1_Cnt < 5 ) return;

    dwRegNum = ( wAddr & 0x7FFF ) >> 13;
    Map1_Regs[ dwRegNum ] = Map1_Latch;

    Map1_Cnt = 0;
    Map1_Latch = 0x00;

    switch( dwRegNum )
    {
        case 0:
        {
            if( Map1_Regs[0] & 0x02 )
            {
                if( Map1_Regs[0] & 0x01 )
                {
                    InfoNES_Mirroring( 0 );
                }
                else
                {
                    InfoNES_Mirroring( 1 );
                }
            }
            else
            {
                if( Map1_Regs[0] & 0x01 )
                {
                    InfoNES_Mirroring( 2 );
                }
                else
                {
                    InfoNES_Mirroring( 3 );
                }
            }
        }
        break;

        case 1:
        {
            BYTE byBankNum = Map1_Regs[1];

            if ( Map1_Size == Map1_1024K )
            {
                if ( Map1_Regs[0] & 0x10 )
                {
                    if ( Map1_swap )
                    {
                        Map1_256K_base = (Map1_Regs[1] & 0x10) >> 4;
                        if(Map1_Regs[0] & 0x08)
                        {
                            Map1_256K_base |= ((Map1_Regs[2] & 0x10) >> 3);
                        }
                        Map1_set_ROM_banks();
                        Map1_swap = 0;
                    }
                    else
                    {
                        Map1_swap = 1;
                    }
                }
                else
                {
                    Map1_256K_base = ( Map1_Regs[1] & 0x10 ) ? 3 : 0;
                    Map1_set_ROM_banks();
                }
            }
            else if((Map1_Size == Map1_512K) && (!NesHeader.byVRomSize))
            {
                Map1_256K_base = (Map1_Regs[1] & 0x10) >> 4;
                Map1_set_ROM_banks();
            }
            else if ( NesHeader.byVRomSize )
            {
                if ( Map1_Regs[0] & 0x10 )
                {
                    byBankNum <<= 2;
                    PPUBANK[ 0 ] = VROMPAGE( (byBankNum+0) % (NesHeader.byVRomSize << 3) );
                    PPUBANK[ 1 ] = VROMPAGE( (byBankNum+1) % (NesHeader.byVRomSize << 3) );
                    PPUBANK[ 2 ] = VROMPAGE( (byBankNum+2) % (NesHeader.byVRomSize << 3) );
                    PPUBANK[ 3 ] = VROMPAGE( (byBankNum+3) % (NesHeader.byVRomSize << 3) );
                    InfoNES_SetupChr();
                }
                else
                {
                    byBankNum <<= 2;
                    PPUBANK[ 0 ] = VROMPAGE( (byBankNum+0) % (NesHeader.byVRomSize << 3) );
                    PPUBANK[ 1 ] = VROMPAGE( (byBankNum+1) % (NesHeader.byVRomSize << 3) );
                    PPUBANK[ 2 ] = VROMPAGE( (byBankNum+2) % (NesHeader.byVRomSize << 3) );
                    PPUBANK[ 3 ] = VROMPAGE( (byBankNum+3) % (NesHeader.byVRomSize << 3) );
                    PPUBANK[ 4 ] = VROMPAGE( (byBankNum+4) % (NesHeader.byVRomSize << 3) );
                    PPUBANK[ 5 ] = VROMPAGE( (byBankNum+5) % (NesHeader.byVRomSize << 3) );
                    PPUBANK[ 6 ] = VROMPAGE( (byBankNum+6) % (NesHeader.byVRomSize << 3) );
                    PPUBANK[ 7 ] = VROMPAGE( (byBankNum+7) % (NesHeader.byVRomSize << 3) );
                    InfoNES_SetupChr();
                }
            }
        }
        break;

        case 2:
        {
            BYTE byBankNum = Map1_Regs[2];

            if((Map1_Size == Map1_1024K) && (Map1_Regs[0] & 0x08))
            {
                if(Map1_swap)
                {
                    Map1_256K_base =  (Map1_Regs[1] & 0x10) >> 4;
                    Map1_256K_base |= ((Map1_Regs[2] & 0x10) >> 3);
                    Map1_set_ROM_banks();
                    Map1_swap = 0;
                }
                else
                {
                    Map1_swap = 1;
                }
            }

            if(!NesHeader.byVRomSize)
            {
                if ( Map1_Regs[ 0 ] & 0x10 )
                {
                    byBankNum <<= 2;
#if 0
                    PPUBANK[ 4 ] = VRAMPAGE0( byBankNum+0 );
                    PPUBANK[ 5 ] = VRAMPAGE0( byBankNum+1 );
                    PPUBANK[ 6 ] = VRAMPAGE0( byBankNum+2 );
                    PPUBANK[ 7 ] = VRAMPAGE0( byBankNum+3 );
#else
                    PPUBANK[ 4 ] = CRAMPAGE( byBankNum+0 );
                    PPUBANK[ 5 ] = CRAMPAGE( byBankNum+1 );
                    PPUBANK[ 6 ] = CRAMPAGE( byBankNum+2 );
                    PPUBANK[ 7 ] = CRAMPAGE( byBankNum+3 );
#endif
                    InfoNES_SetupChr();
                    break;
                }
            }

            // set 4K VROM bank at $1000
            if(Map1_Regs[0] & 0x10)
            {
                // swap 4K
                byBankNum <<= 2;
                PPUBANK[ 4 ] = VROMPAGE( (byBankNum+0) % (NesHeader.byVRomSize << 3) );
                PPUBANK[ 5 ] = VROMPAGE( (byBankNum+1) % (NesHeader.byVRomSize << 3) );
                PPUBANK[ 6 ] = VROMPAGE( (byBankNum+2) % (NesHeader.byVRomSize << 3) );
                PPUBANK[ 7 ] = VROMPAGE( (byBankNum+3) % (NesHeader.byVRomSize << 3) );
                InfoNES_SetupChr();
            }
        }
        break;

        case 3:
        {
            BYTE byBankNum = Map1_Regs[3];

            // set ROM bank
            if ( Map1_Regs[0] & 0x08 )
            {
                // 16K of ROM
                byBankNum <<= 1;

                if ( Map1_Regs[0] & 0x04 )
                {
                    // 16K of ROM at $8000
                    Map1_bank1 = byBankNum;
                    Map1_bank2 = byBankNum+1;
                    Map1_bank3 = Map1_HI1;
                    Map1_bank4 = Map1_HI2;
                }
                else
                {
                    // 16K of ROM at $C000
                    if(Map1_Size == Map1_SMALL)
                    {
                        Map1_bank1 = 0;
                        Map1_bank2 = 1;
                        Map1_bank3 = byBankNum;
                        Map1_bank4 = byBankNum+1;
                    }
                }
            }
            else
            {
                // 32K of ROM at $8000
                byBankNum <<= 1;

                Map1_bank1 = byBankNum;
                Map1_bank2 = byBankNum+1;
                if(Map1_Size == Map1_SMALL)
                {
                    Map1_bank3 = byBankNum+2;
                    Map1_bank4 = byBankNum+3;
                }
            }
            Map1_set_ROM_banks();
        }
        break;
    }
}
