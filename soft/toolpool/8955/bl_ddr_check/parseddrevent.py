#!/usr/bin/python

import sys
import optparse

res = {}

def parse_event(fname):
    global res
    print("parse %s ......" %(fname))
    fh = open(fname)
    for line in fh.readlines():
        if not line.startswith('Detected event: 0x'):
            continue

        event = int(line.split()[2][:-1], 0)
        if (event & 0xff700000) == 0xdd000000:
            vol = (event >> 16) & 0xff
            freq = event & 0xfff
            if vol not in res:
                res[vol] = {}
            res[vol][freq] = {}
        elif (event & 0xffff0000) == 0xdd3f0000:
            res[vol][freq]['min'] = 0
            res[vol][freq]['max'] = 0
            res[vol][freq]['dqs'] = 0
            res[vol][freq]['mr0'] = 0
            res[vol][freq]['mr4'] = 0
        elif (event & 0xffff0000) == 0xdd300000:
            vmin = (event >> 8) & 0xff
            vmax = (event & 0xff)
            if 'min' in res[vol][freq]:
                if vmin > res[vol][freq]['min']:
                    print("update 0x%x %dM min from %d to %d" % (
                            vol, freq, res[vol][freq]['min'], vmin))
                    res[vol][freq]['min'] = vmin
            else:
                res[vol][freq]['min'] = vmin
            if 'max' in res[vol][freq]:
                if vmax < res[vol][freq]['max']:
                    print("update 0x%x %dM max from %d to %d" % (
                            vol, freq, res[vol][freq]['max'], vmax))
                    res[vol][freq]['max'] = vmax
            else:
                res[vol][freq]['max'] = vmax
        elif (event & 0xffff0000) == 0xdd400000:
            res[vol][freq]['dqs'] = (event & 0xffff)
        elif (event & 0xffff0000) == 0xdd500000:
            res[vol][freq]['mr0'] = (event & 0xff)
            res[vol][freq]['mr4'] = ((event >> 8) & 0xff)
    fh.close();

def parse_input(fname):
    global res
    print("parse %s ......" %(fname))
    fh = open(fname)
    for line in fh.readlines():
        fields = line.split()
        if len(fields) != 10:
            continue

        vol = int(fields[1][:-1], 0)
        freq = int(fields[2][17:-2], 0)
        vmin = int(fields[3][:-1], 0)
        vmax = int(fields[4][:-1], 0)
        dqs = int(fields[6][:-1], 0)
        mr0 = int(fields[7][:-1], 0)
        mr4 = int(fields[8], 0)

        if vol not in res:
            res[vol] = {}
        if freq not in res[vol]:
            res[vol][freq] = {}
            res[vol][freq]['min'] = vmin
            res[vol][freq]['max'] = vmax
            res[vol][freq]['dqs'] = dqs
            res[vol][freq]['mr0'] = mr0
            res[vol][freq]['mr4'] = mr4
        else:
            if vmin > res[vol][freq]['min']:
                print("update 0x%x %dM min from %d to %d" % (
                        vol, freq, res[vol][freq]['min'], vmin))
                res[vol][freq]['min'] = vmin
            if vmax < res[vol][freq]['max']:
                print("update 0x%x %dM max from %d to %d" % (
                        vol, freq, res[vol][freq]['max'], vmax))
                res[vol][freq]['max'] = vmax
    fh.close()

def main(argv):
    opt = optparse.OptionParser(usage="""usage: %prog [option]

This utility will parse DDR check output event, and generate
needed header files.
""")

    opt.add_option("--event", action="append", dest="event",
                   help="File name for RAMRUN events")
    opt.add_option("--input", action="append", dest="input",
                   help="Existed ddrinit.h file name")
    opt.add_option("--ddrinit", action="store", dest="ddrinit",
                   default="ddrinit.h",
                   help="Output ddrinit.h file name")
    opt.add_option("--freq", action="store", dest="freq",
                   default="freq.h",
                   help="Output freq.h file name")

    opt, argv = opt.parse_args(argv)

    if opt.input is not None:
        for fname in opt.input:
            parse_input(fname)

    if opt.event is not None:
        for fname in opt.event:
            parse_event(fname)

    vols = sorted(res.keys())
    freqs = sorted(res[vols[0]].keys())

    for vol in vols:
        for freq in freqs:
            res[vol][freq]['ave'] = (res[vol][freq]['min'] + res[vol][freq]['max']) / 2

    for vol in vols:
        for freq in freqs:
            if res[vol][freq]['max'] == 127:
                dfreq = freq * 2
                ave = res[vol][dfreq]['ave'] * 2 + 3
                if ave > 127:
                    ave = 127
                print("change %s %dM best from %d to %d" %(hex(vol), freq, res[vol][freq]['ave'], ave))
                res[vol][freq]['ave'] = ave

    if opt.ddrinit is not None:
        fh = open(opt.ddrinit, 'w')
        for vol in vols:
            for freq in freqs:
                fh.write("    { 0x%x, HAL_SYS_MEM_FREQ_%dM, %d, %d, %d, 0x%x, 0x%x, 0x%x },\n" % (
                        vol,
                        freq,
                        res[vol][freq]['min'],
                        res[vol][freq]['max'],
                        res[vol][freq]['ave'],
                        res[vol][freq]['dqs'],
                        res[vol][freq]['mr0'],
                        res[vol][freq]['mr4']))
            fh.write("\n")
        fh.close()

    if opt.freq is not None:
        fh = open(opt.freq, 'w')
        for vol in vols:
            if (vol & 0x80) == 0:
                fh.write("/* LDO=%d */\n"%(vol))
                for freq in freqs:
                    clk = res[vol][freq]['ave']
                    dqs = res[vol][freq]['dqs'] | (clk << 16)
                    mr0 = res[vol][freq]['mr0']
                    mr4 = res[vol][freq]['mr4']
                    line = "#define TGT_DDR_LDO%d_%d { .vcore = %s, .mem_clk = DDR_FREQ_%dM, .ddr = { .ddr_clk_ctrl = %s, .ddr_dqs_ctrl = %s, .mr0 = %s, .mr4 = %s, }, }\n"%(
                            vol, freq, hex(vol), freq, hex(clk), hex(dqs), hex(mr0), hex(mr4))
                    fh.write(line)
            else:
                fh.write("/* DCDC=%d */\n"%(vol&0xf))
                for freq in freqs:
                    clk = res[vol][freq]['ave']
                    dqs = res[vol][freq]['dqs'] | (clk << 16)
                    mr0 = res[vol][freq]['mr0']
                    mr4 = res[vol][freq]['mr4']
                    line = "#define TGT_DDR_DCDC%d_%d { .vcore = %s, .mem_clk = DDR_FREQ_%dM, .ddr = { .ddr_clk_ctrl = %s, .ddr_dqs_ctrl = %s, .mr0 = %s, .mr4 = %s, }, }\n"%(
                            vol&0xf, freq, hex(vol), freq, hex(clk), hex(dqs), hex(mr0), hex(mr4))
                    fh.write(line)
            fh.write("\n")
        fh.close()

    return 0

if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))

