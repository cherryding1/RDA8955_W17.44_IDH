CFLAGS := -Wall -Wmissing-prototypes -Wstrict-prototypes -O2 -fomit-frame-pointer -std=gnu89 -DKBUILD_NO_NLS=1 -DNCURSES_WIDECHAR=1 -DLOCALE
ifneq "$(shell uname | grep -c Linux)" "0"
	# For Linux
	CC := gcc
	CFLAGS += -DCURSES_LOC="<ncurses.h>"
	LDFLAGS :=
	LDLIBS := -lncurses
else
	tooldir := $(shell cygpath -am ${CSDTK4INSTALLDIR}/mingw32/opt/)
	# For Windows, mingw32
	CC := i686-w64-mingw32-gcc.exe
	CFLAGS += -DCURSES_LOC="<ncursesw/ncurses.h>"
	CFLAGS += -I $(tooldir)/include
	LDFLAGS := -static -L $(tooldir)/lib -mconsole
	LDLIBS := -lncursesw -lregex
endif



mconf_src := mconf.c zconf.tab.c
lxdialog_src := lxdialog/checklist.c lxdialog/util.c lxdialog/inputbox.c lxdialog/textbox.c lxdialog/yesno.c lxdialog/menubox.c
zconf_tab_out := zconf.hash.c
conf_src := conf.c zconf.tab.c

conf_tab: zconf.gperf
	gperf -t --output-file $(zconf_tab_out) -a -C -E -g -k '1,3,$$' -p -t zconf.gperf

conf_y: 
	bison -t -l -p zconf zconf.y
	
.PHONY: mconf
#mconf: $(mconf_src) $(lxdialog_src) conf_tab conf_y 
mconf: $(mconf_src) $(lxdialog_src)
	$(CC) $(CFLAGS) $(mconf_src) $(lxdialog_src) $(LDFLAGS) $(LDLIBS) -o mconf

.PHONY: conf
conf: $(conf_src)
	$(CC) $(CFLAGS) $(conf_src) $(LDFLAGS) $(LDLIBS) -o conf
