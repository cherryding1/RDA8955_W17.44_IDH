T卡升级操作说明

#T卡升级操作说明:
##文件说明
 T卡根目录下放置3个文件如下：
1）update file：将要烧入flash中的系统文件或者补丁文件。
2）cfp file：校准区更新文件，该文件包含了RF校准数据和Audio校准数据，ramrun程序只更新Audio校准数据，RF校准数据保留flash校准区上原有的内容。
3）t-ramrun file：用于T卡升级的引导lod，在ram中运行，负责将update file烧入flash中。
4）update.cfg：配置文件，用于指定t-ramrun file 和update file 的文件名，这样T卡中可以保存不同文件名的多个版本update file与t-ramrun file。将来还可以扩展升级模式，进行音频校准区的升级。

##修改配置文件
在T卡根目录下创建文件：update.cfg并按照以下格式编辑：   
 T_RAMRUN=t-ramrun file name
 T_UPDATE=upate file name 
 T_CFP=cfp file name
 #Clear user data region option. 
 T_CLEAR_USER_DATA=option(YES/NO) 

Example:
 T_RAMRUN=ramImage.lod
 T_UPDATE=8809z_x3_debug_flash.lod
 T_CFP=Calib.cfp
 #Clear user data region option. 
 T_CLEAR_USER_DATA=YES 
支持注释语句.升级程序解释配置项时,将忽略'#'开始直到回车换行的位置的内容. 

##启动T卡升级
在Idle界面下输入"*#007#",开始加载ramrunImg.lod,加载完成后自动重启。
重启后ramrun程序将update file写入flash的同时在LCD显示更新进度，更新完成后自动重启，完成升级过程。

##生成t_ram_run lod
开发人员需要关注本节内容，维护人员可以略过。
###RAMRUN生成方法
T-Ramrun放在soft\toolpool\plugins\fastpf\flash_programmers\t_ramrun
其生成、配置工程位于soft\toolpool\t_flash_programmer

1)配置install.rb
使用前配置soft\toolpool\t_flash_programmer\install.rb，将需要的target填入ff_target中，比如：
ff_target = [
    "8809z_x3",
]

2）运行install.rb
在soft\toolpool\t_flash_programmer运行install.rb：
soft/toolpool/t_flash_programmer
$ install.rb
Generating all the t-flash programmer files...
TFP for 8809z_x3...
Done.

3）t-ramrun目录
生成的ramrun会自动copy到soft\toolpool\plugins\fastpf\flash_programmers\t_ramrun

###更新libSourceDepends_release.a
Section2中的编译方式是基于libSourceDepends_release.a。而如果需要更新这个库，需要以下步骤：
 运行copySrc.sh，将需要的源文件copy到SourceDepends
cd toolpool/t_flash_programmer
$ copySrc.sh
Start to copy src to SourceDepends
运行install.rb，Makefile发现有SourceDepends/src存在，则使用源文件编译。

##注意事宜
1．需要为每个工程（target）分别出一个单独的t-ramrun lod。这主要是因为不同工程LCD不同所致。
2．T卡升级依赖原系统正常工作，如升级失败导致不能开机，则不能再次升级。在这种情况下可以使用coolwatcher或产线工具升级。因此升级时注意不要掉电。


