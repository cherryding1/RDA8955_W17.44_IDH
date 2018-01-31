#!/usr/bin/env ruby
include CoolHost
require 'help.rb'

def usbEp0InStatus()
  puts "EP0 In CTL"
  puts "  EP activity: " + $USBC.DIEPCTL0.USBActEP.R.to_s;
  puts "  Nak Status " + $USBC.DIEPCTL0.NAKSts.R.to_s;
  puts "  Stall Status " + $USBC.DIEPCTL0.Stall.R.to_s;
  puts "  Ep Disable " + $USBC.DIEPCTL0.EPDis.R.to_s;
  puts "  Ep Enable " + $USBC.DIEPCTL0.EPEna.R.to_s;
  puts "EP0 In Interrupt"
  puts "  Transfert complete " + $USBC.DIEPINT0.XferCompl.R.to_s;
  puts "  TimeOut " + $USBC.DIEPINT0.TimeOut.R.to_s;
end

def usbEp0OutStatus()
  puts "EP0 Out CTL"
  puts "  EP activity: " + $USBC.DOEPCTL0.USBActEP.R.to_s;
  puts "  Nak Status " + $USBC.DOEPCTL0.NAKSts.R.to_s;
  puts "  Stall Status " + $USBC.DOEPCTL0.Stall.R.to_s;
  puts "  Ep Disable " + $USBC.DOEPCTL0.EPDis.R.to_s;
  puts "  Ep Enable " + $USBC.DOEPCTL0.EPEna.R.to_s;
  puts "EP0 Out Interrupt"
  puts "  Transfert complete " + $USBC.DOEPINT0.XferCompl.R.to_s;
  puts "  Setup " + $USBC.DOEPINT0.Setup.R.to_s;
end

def usbEpInStatus(num)
  if(num == 0)
    usbEp0InStatus();
  else
    puts "EP" + num.to_s() + " In CTL"
    puts "  EP activity: " + $USBC.DIEPnCONFIG[num-1].DIEPCTL.USBActEP.R.to_s;
    puts "  Nak Status " + $USBC.DIEPnCONFIG[num-1].DIEPCTL.NAKSts.R.to_s;
    puts "  Stall Status " + $USBC.DIEPnCONFIG[num-1].DIEPCTL.Stall.R.to_s;
    puts "  Ep Disable " + $USBC.DIEPnCONFIG[num-1].DIEPCTL.EPDis.R.to_s;
    puts "  Ep Enable " + $USBC.DIEPnCONFIG[num-1].DIEPCTL.EPEna.R.to_s;
    puts "EP" + num.to_s() + " In Interrupt"
    puts "  Transfert complete " + $USBC.DIEPnCONFIG[num-1].DIEPINT.XferCompl.R.to_s;
    puts "  TimeOut " + $USBC.DIEPnCONFIG[num-1].DIEPINT.TimeOut.R.to_s;
  end  
end

def usbEpOutStatus(num)
  if(num == 0)
    usbEp0InStatus();
  else
    puts "EP" + num.to_s() + " Out CTL"
    puts "  EP activity: " + $USBC.DOEPnCONFIG[num-1].DOEPCTL.USBActEP.R.to_s;
    puts "  Nak Status " + $USBC.DOEPnCONFIG[num-1].DOEPCTL.NAKSts.R.to_s;
    puts "  Stall Status " + $USBC.DOEPnCONFIG[num-1].DOEPCTL.Stall.R.to_s;
    puts "  Ep Disable " + $USBC.DOEPnCONFIG[num-1].DOEPCTL.EPDis.R.to_s;
    puts "  Ep Enable " + $USBC.DOEPnCONFIG[num-1].DOEPCTL.EPEna.R.to_s;
    puts "EP" + num.to_s() + " Out Interrupt"
    puts "  Transfert complete " + $USBC.DOEPnCONFIG[num-1].DOEPINT.XferCompl.R.to_s;
    puts "  Setup " + $USBC.DOEPnCONFIG[num-1].DOEPINT.Setup.R.to_s;
  end  
end

def usbStatus()
  if($USBC.GHWCFG1.R != 0x40)
    puts "HardWare config register (GHWCFG1) have bad value. Do you have, start the usb clock ?";
  end

  if($USBC.GAHBCFG.GlblIntrMsk.R == 1)
    puts "Global interrupt enable";
  else
    puts "Global interrupt disable";
  end

  dmaEnable = $USBC.GAHBCFG.DMAEn.R;
  if(dmaEnable == 1)
    puts "USB DMA enable";
  else
    puts "USB DMA disable";
  end

  puts "Interrupt"
  puts "  Usb reset " + $USBC.GINTSTS.USBRst.R.to_s();
  puts "  Enum Done " + $USBC.GINTSTS.EnumDone.R.to_s();
  puts "  IN EP " + $USBC.GINTSTS.IEPInt.R.to_s();
  puts "  OUT EP " + $USBC.GINTSTS.OEPInt.R.to_s();

  addr=$USBC.DCFG.DevAddr.R;
  if(addr == 0)
    puts "No Usb address already configured"
  else
    puts "Address USB " + addr.to_s();
  end

  puts "Global in nak status " + $USBC.DCTL.GNPINNakSts.R.to_s() + " out nak status " + $USBC.DCTL.GOUTNakSts.R.to_s();
  
  if(addr != 0 && dmaEnable == 1)
    usbEpInStatus(0);
    usbEpOutStatus(0);
  end
end


