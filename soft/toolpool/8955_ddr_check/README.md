DDR check for 8809z
===================

8809z uses DDR PSRAM, and the timing depends on core voltage and DDR PSRAM
frequency. This ramrun will probe all vore voltages and DDR PSRAM frequency to
check the working range.

The startup in **main** is carefully tuned. 8809z vcore mode can only be
changed after XCV is initialized. However, when the complete **hal_Open** is
called, SRAM size is not enough.

Change frequency to 312M needs the *current* state of DDR PSRAM can work. So,
each time to change 312M DDR timing, we will set to known good 104M timing and
then change to 312M timing to be probed.

A very simple DDR check method is adopted. From experiments, even with complex
DDR check method, the result valid range is nearly the same.

### build

The build command is:

    ctmake CT_TARGET=8809z_ramrun CT_RELEASE=release CT_USER=ADMIN \
    CT_OPT=dbg_size WITH_SVN=0 WITHOUT_WERROR=1 CT_USERGEN=no \
    CT_RESGEN=no CT_PRODUCT=test

### run

Run the command in coolwatcher:

    ramrun "D:/8809z_ddr_check_host_8809z_ramrun_test_release_ramrun.lod"

### collect result

The results are output through events. It is hard to read the events directly.
So, a script is written to parse the events. Copy events to a file (for example,
"ddr.event"), and run:

    ./parseddrevent.py ddr.event

There are 2 files will be generated:

* "out.csv": list working range for each core voltage and DDR frequency
* "header.h": the content can be pasted into "tgt_ddr_freq_cfg.h"

