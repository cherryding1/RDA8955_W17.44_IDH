##############################################################################
##           Copyright (C) 2005, Coolsand Technologies, Inc. 
##                        All Rights Reserved   
##############################################################################

#
# File description[文件内容介绍]
#
1> cfw_sms_mmi.c  : All APIs defined for MMI using
2> cfw_sms_main.c : Main process
3> cfw_sms_tool.c : Tool APIs
4> cfw_sms_prv.h  : Tool APIs define
5> cfw_sms_db.c   : ME database api
6> cfw_sms_db.h   : ME database api define 

#
# Version description[版本实现功能介绍]
#
V1.0.1
1> 发送接收一般短信及超长短信(文本及PDU).
2> 对短信进行Write/read/list/delete/copy
3> 统计存储器的存储情况
4> 不能存储新的或编辑已有的超长短信(未完成)
5> 不能统计各状态的短信个数(未完成)
6> SMS初始化还没有优化(未完成)

1> cfw_sms_main.c  改动CFW_DecomposePDU的使用， 原因:在read某种状态的短信时可能会死机
2> cfw_sms_tool.c  改动对于超长短信的nTP_UDL的计算,不能减去头的长度 


V1.0.2
1> 统计存储器的存储情况
2> 可以在SIM和ME上存储新的超长短信;还不能编辑已有的超长短信
3> 能统计各状态的短信个数
4> SMS初始化优化

1> cfw_sms_main.c  改动CFW_DecomposePDU的使用， 原因:在read某种状态的短信时可能会死机
2> cfw_sms_tool.c  改动对于超长短信的nTP_UDL的计算,不能减去头的长度 


2007.06.13 xueww[+]
1> cfw_sms_tool.c 改动sms_mt_Parse_SmsPPReceiveInd,使得发送给MMI的事件参数pEvent->nParam2能够标识长短信的信息.
2> cfw_sms_main.c 改动sms_ReadAoProc,使得发送给MMI的事件参数pEvent->nParam2能够标识长短信的信息.
