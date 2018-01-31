#ifdef AT_LWM2M_SUPPORT

#include "at.h"

#define LWM2M_STRING_SIZE 60
#define LWM2M_PARAM_SIZE 20

/**
 * connect to the server
 */
VOID AT_LWM2M_CmdFunc_START(AT_CMD_PARA *pParam);

/**
 * execute lwm2m client itself command
 */
VOID AT_LWM2M_CmdFunc_COMMAND(AT_CMD_PARA *pParam);

#endif
