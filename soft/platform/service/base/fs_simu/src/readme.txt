FS Sumulator


1.文件系统模拟器相关的预定义(Preprocessor definitions):

_FS_SIMULATOR_,_REG_SMS_MERGE,_USERGEN,_USERGEN_SHELL,_DSM_SIMUFLASH_FILE


_FS_SIMULATOR_:标识模拟器环境

_REG_SMS_MERGE:标识REG 与 SMS 模块共用CSW分区

_USERGEN:编译预设用户数据区的子函数

_DSM_SIMUFLASH_FILE:以Img file方式模拟 flash.

_DSM_SIMUFLASH_MEMORY::以memory方式模拟 flash.

DSM_SHELL_COMMAND: Dsm Shell 

FAT_SHELL_COMMAND: Fat Shell 

VDS_SHELL_COMMAND: VDS Shell

_USERGEN_SHELL：Usergen Shell.


2.目录说明：

base:该目录与工程中的soft/${PLATFORM_SERVICE}/base目录对应，只包含其下与文件系统相关的子目录和文件。代码完全相同。

include:该目录与工程中的soft/${PLATFORM_SERVICE}/include目录对应，只包含与文件系统相关的头文件。代码完全相同。

simulator:FS模拟器代码，包括shell,flash driver simulator,t-flash dirver simulator.

target:该目录与工程中的soft/target目录对应，只包含其下与文件系统相关的子目录和文件，代码有所改变。

