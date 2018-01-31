copy below files to project folder:
copy ram_check_all_904 to folder soft\toolpool\ram_check_all_904
copy memd\flsh_spi32m to folder soft\platform\edrv\memd\flsh_spi32m
copy 8851c_gh021_104M to folder soft\target\8851c_gh021_104M

Set cygwin path to the project folder
Open Cygwin

input string: "work"

change dir to /soft/toolpool/ram_check_all_904
for example:
rda@soft03 /cygdrive/e/Verigy/RDA8809/8809_Soft/soft/toolpool/ram_check_all_904

make project
$ prjmake CT_TARGET=8851c_gh021_104M CT_OPT=dbg_size WITHOUT_WERROR=y CT_RELEASE=release CT_PRODUCT=904_1

or use below command to make project
$ make CT_TARGET=8851c_gh021_104M CT_OPT=dbg_size WITHOUT_WERROR=y CT_RELEASE=release CT_PRODUCT=904_1

below command can create asm program
$ make CT_TARGET=8851c_gh021_104M CT_OPT=dbg_size WITHOUT_WERROR=y CT_RELEASE=release CT_PRODUCT=904_1 dump

ramrun lod would be created:
\soft\hex\ram_check_all_904_host_8851c_gh021_104M_904_1_release\ram_check_all_904_host_8851c_gh021_104M_904_1_release_ramrun.lod
