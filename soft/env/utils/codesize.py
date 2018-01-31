#!/usr/bin/python

import os, sys
import re
import optparse

files = {}
libs = {}
total = {"text": 0, "rodata": 0, "data": 0, "bss": 0}

extext = []
exrodata = []
exdata = []
exbss = []


def libname(lib):
    return lib.split("/")[-1]


def add_entry(section_name, size, lib, obj):
    global files, libs, total
    global extext, exrodata, exdata, exbss

    if section_name.startswith(".text"):
        type = "text"
    elif section_name.startswith(".rodata"):
        type = "rodata"
    elif section_name.startswith(".data"):
        type = "data"
    elif section_name.startswith(".bss"):
        type = "bss"
    elif section_name in extext:
        type = "text"
    elif section_name in exrodata:
        type = "rodata"
    elif section_name in exdata:
        type = "data"
    elif section_name in exbss:
        type = "bss"
    else:
        print "unknown section:", section_name, size, lib, obj
        sys.exit(1)

    lib = libname(lib)
    if obj in files:
        files[obj][type] += size
    else:
        files[obj] = {"lib": lib, "text": 0, "rodata": 0, "data": 0, "bss": 0}
        files[obj][type] += size

    if lib in libs:
        libs[lib][type] += size
    else:
        libs[lib] = {"text": 0, "rodata": 0, "data": 0, "bss": 0}
        libs[lib][type] += size

    total[type] += size
    return


def parse_map(fname):
    try:
        fh = open(fname, 'r')
    except:
        print "Failed to open %s for read!" % fname
        sys.exit(1)

    map_found = False
    section_name = ''
    r3 = re.compile(r"\s+0xffffffff([0-9a-fA-F]{8})\s+0x([0-9a-fA-F]+)\s+(.*)\((.*)\)")
    r4 = re.compile(r"\s+(\S+)\s+0xffffffff([0-9a-fA-F]{8})\s+0x([0-9a-fA-F]+)\s+(.*)\((.*)\)")
    for line in fh.readlines():
        if line.startswith("Linker script and memory map"):
            map_found = True
            continue
        if not map_found:
            continue

        fields = line.split()
        if len(fields) == 1:
            section_name = fields[0]
        elif len(fields) == 3:
            if section_name.startswith(".avcpu"):
                continue
            m = r3.match(line)
            if m:
                size = int(m.group(2), 16)
                lib = m.group(3)
                obj = m.group(4)
                add_entry(section_name, size, lib, obj)
        elif len(fields) == 4:
            m = r4.match(line)
            if m:
                section_name = m.group(1)
                size = int(m.group(3), 16)
                lib = m.group(4)
                obj = m.group(5)
                add_entry(section_name, size, lib, obj)

    return files


def sizelist(f):
    sl = [f["text"], f["rodata"], f["data"], f["bss"],
          f["text"] + f["rodata"],
          f["text"] + f["rodata"] + f["data"]]
    ss = [str(s) for s in sl]
    return ", ".join(ss)


def main(argv):
    opt = optparse.OptionParser(usage="""usage %prog [options]

This utility will analyze code size from map file.

Code size are expressed in concept of: text, rodata, data, bss
However, there are various section names in object files. The rules:
* .text*:   text
* .rodata*: rodata
* .data*:   data
* .bss*:    bss
Others should be specified explicitly in this code, or pass by options.

The utility will output 2 files, size by object file and size by library.
""")

    opt.add_option("--map", action="store", dest="map",
                   help="map file created at linking.")
    opt.add_option("--extext", action="store", dest="extext",
                   help="extra text section names (separated by comma)")
    opt.add_option("--exrodata", action="store", dest="exrodata",
                   help="extra rodata section names (separated by comma)")
    opt.add_option("--exdata", action="store", dest="exdata",
                   help="extra data section names (separated by comma)")
    opt.add_option("--exbss", action="store", dest="exbss",
                   help="extra bss section names (separated by comma)")
    opt.add_option("--list-extra", action="store_true", dest="list_extra", default=False,
                   help="list extra section names and exit")
    opt.add_option("--outobj", action="store", dest="outobj", default="outobj.csv",
                   help="detailed information by object file (default: outobj.csv)")
    opt.add_option("--outlib", action="store", dest="outlib", default="outlib.csv",
                   help="detailed information by library (default: outlib.csv)")

    opt, argv = opt.parse_args(argv)

    global extext, exrodata, exdata, exbss
    extext.extend([".boot_sector_start",
                   ".boottext",
                   ".irqtext",
                   ".sramtext",
                   ".romtext",
                   ".bootsramtext",
                   ".ram"])
    exrodata.extend([".bootrodata",
                     ".roresdata",
                     ".robsdata",
                     ".extra"])
    exdata.extend([".bootsramdata",
                   ".sramdata",
                   ".sramucdata",
                   ".srroucdata",
                   ".ucdata"])
    exbss.extend(["COMMON",
                  ".scommon",
                  ".backup",
                  ".bootsrambss",
                  ".ucbss",
                  ".srambss",
                  ".sramucbss",
                  ".ucbackup",
                  ".xcv_reg_value",
                  ".abb_reg_value",
                  ".pmu_reg_value",
                  ".hal_boot_sector_reload_struct",
                  ".boot_sector_reload_struct_ptr",
                  ".boot_sector_struct",
                  ".boot_sector_struct_ptr",
                  ".fixptr",
                  ".sram_overlay"])

    if opt.extext is not None:
        extext.extend(opt.extext.split(","))
    if opt.exrodata is not None:
        exrodata.extend(opt.exrodata.split(","))
    if opt.exdata is not None:
        exdata.extend(opt.exdata.split(","))
    if opt.exbss is not None:
        exbss.extend(opt.exbss.split(","))

    if opt.list_extra:
        print "Extra text sections:\n   ", "\n    ".join(extext)
        print "Extra rodata sections:\n   ", "\n    ".join(exrodata)
        print "Extra data sections:\n   ", "\n    ".join(exdata)
        print "Extra bss sections:\n   ", "\n    ".join(exbss)
        return 0

    if not opt.map:
        print "No map file specified!"
        return 1

    parse_map(opt.map)

    global files, libs, total
    print "total: text, rodata, data, bss, code, flash"
    print "      ", sizelist(total)

    print "size by library to %s ..." %(opt.outlib)
    try:
        fh = open(opt.outlib, "w")
        fh.write("library name, text, rodata, data, bss, code, flash\n")
        for n, l in libs.items():
            fh.write("%s, %s\n" % (n, sizelist(l)))
        fh.close()
    except:
        print "Failed to write %s!" % (opt.outlib)

    print "size by object to %s ..." %(opt.outobj)
    try:
        fh = open(opt.outobj, "w")
        fh.write("object name, library name, text, rodata, data, bss, code, flash\n")
        for n, l in files.items():
            fh.write("%s, %s, %s\n" % (n, l["lib"], sizelist(l)))
        fh.close()
    except:
        print "Failed to write %s!" % (opt.outobj)

    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
