1 UINT32 cfg_readdefault(UINT8 *uSubkey,UINT8 *name, void *data);该函数是新增加的函数，有函数体，但是没有声明，因此编译有警告。
2 结构EMOD_PARAMETERS是工程模式新增加的cfg函数需要的结构，已经添加到cfw_prv.h文件中。
typedef struct _EMOD_PARAMETERS
{	
UINT8 CC_Cause;
}EMOD_PARAMETERS;

