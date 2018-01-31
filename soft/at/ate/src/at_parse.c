/* Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/

#define LOCAL_LOG_LEVEL LOG_LEVEL_VERBOSE

#include "at_parse.h"
#include "at_sa.h"
#include "at_utils.h"

struct AT_PARSE_CONTEXT_T
{
    AT_COMMAND_T *cmds;
    AT_COMMAND_T *currcmd;
};

enum
{
    AT_PARSE_ERR_UNKNOWN_CMD = 2000,
    AT_PARSE_ERR_NOMEM,
    AT_PARSE_ERR_SYNTAX,
    AT_PARSE_ERR_NO_CMD,
    AT_PARSE_ERR_TOO_MANY_PARAM
};

#define HEX2NUM(c) (((c) >= '0' && (c) <= '9') ? ((c) - '0') : ((c) >= 'A' && (c) <= 'F') ? ((c) - ('A' - 0xa)) : ((c) - ('a' - 0xa)))

// APIs from bison/flex
int at_parse_parse(void *scanner);
int at_parse_lex_init_extra(void *extra, void **scanner);
void *at_parse__scan_bytes(const char *bytes, unsigned len, void *scanner);
int at_parse_lex_destroy(void *scanner);
extern unsigned at_parse_get_leng(void *scanner);
char *at_parse_get_text(void *scanner);
void *at_parse_get_extra(void *yyscanner);

// =============================================================================
// at_CmdlistTail: get the tail cmd
// =============================================================================
static AT_COMMAND_T *at_CmdlistTail(AT_COMMAND_T *head)
{
    if (head == NULL)
        return NULL;

    while (head->next != NULL)
        head = head->next;
    return head;
}

// =============================================================================
// at_CmdlistCount
// =============================================================================
unsigned at_CmdlistCount(AT_COMMAND_T *cmd)
{
    if (cmd == NULL)
        return 0;

    unsigned count = 0;
    while (cmd != NULL)
    {
        count++;
        cmd = cmd->next;
    }
    return count;
}

// =============================================================================
// at_CmdlistFree: free one command
// =============================================================================
void at_CmdlistFree(AT_COMMAND_T *cmd)
{
    if (cmd == NULL)
        return;
    for (int n = 0; n < cmd->paramCount; n++)
        at_free(cmd->params[n]);
    cmd->paramCount = 0;
    at_free(cmd);
}

// =============================================================================
// at_CmdlistPushBack: push a command to list and update head
// =============================================================================
void at_CmdlistPushBack(AT_COMMAND_T **head, AT_COMMAND_T *node)
{
    if (head == NULL || node == NULL)
        return;

    node->next = NULL;
    if (*head == NULL)
    {
        *head = node;
    }
    else
    {
        AT_COMMAND_T *tail = at_CmdlistTail(*head);
        tail->next = node;
    }
}

// =============================================================================
// at_CmdlistPopFront: pop the first command from list and update head
// =============================================================================
AT_COMMAND_T *at_CmdlistPopFront(AT_COMMAND_T **head)
{
    if (head == NULL || *head == NULL)
        return NULL;

    AT_COMMAND_T *node = *head;
    *head = node->next;
    node->next = NULL;
    return node;
}

// =============================================================================
// at_CmdlistFreeAll: free all commands from list and update head
// =============================================================================
void at_CmdlistFreeAll(AT_COMMAND_T **head)
{
    AT_COMMAND_T *cmd;
    while ((cmd = at_CmdlistPopFront(head)) != NULL)
        at_CmdlistFree(cmd);
}

// =============================================================================
// at_parse_alloc: malloc used in flex/bison
// =============================================================================
void *at_parse_alloc(unsigned size, void *scanner)
{
    return at_malloc(size);
}

// =============================================================================
// at_parse_realloc: realloc used in flex/bison
// =============================================================================
void *at_parse_realloc(void *ptr, unsigned size, void *scanner)
{
    return at_realloc(ptr, size);
}

// =============================================================================
// at_parse_free: free used in flex/bison
// =============================================================================
void at_parse_free(void *ptr, void *scanner)
{
    at_free(ptr);
}

// =============================================================================
// at_ParseStartCmdText: start parsing a command
// =============================================================================
static int at_ParseStartCmdText(struct AT_PARSE_CONTEXT_T *ctx, char *text, unsigned length)
{
    ATCMDLOGS(V, TSM(1), "AT CMD parse start cmd len=%d: %s", length, text);
    const SA_COMMAND_TABLE_T *desc = at_GetCmdDescriptor((const uint8_t *)text, length);
    if (desc == NULL)
        return AT_PARSE_ERR_UNKNOWN_CMD;

    AT_COMMAND_T *cmd = (AT_COMMAND_T *)at_malloc(sizeof(*cmd));
    if (cmd == NULL)
        return AT_PARSE_ERR_NOMEM;

    cmd->next = NULL;
    cmd->desc = desc;
    cmd->type = 0;
    cmd->paramCount = 0;
    memset(cmd->params, 0, sizeof(cmd->params));

    at_CmdlistPushBack(&ctx->cmds, cmd);
    ctx->currcmd = cmd;
    return 0;
}

// =============================================================================
// at_ParseStartCmd
// =============================================================================
int at_ParseStartCmd(void *scanner)
{
    struct AT_PARSE_CONTEXT_T *ctx = (struct AT_PARSE_CONTEXT_T *)at_parse_get_extra(scanner);
    return at_ParseStartCmdText(ctx, at_parse_get_text(scanner), at_parse_get_leng(scanner));
}

// =============================================================================
// at_ParseStartCmdNone: for "AT\r"
// =============================================================================
int at_ParseStartCmdNone(void *scanner)
{
    struct AT_PARSE_CONTEXT_T *ctx = (struct AT_PARSE_CONTEXT_T *)at_parse_get_extra(scanner);
    return at_ParseStartCmdText(ctx, "AT", 2);
}

// =============================================================================
// at_ParseSetCmdType
// =============================================================================
int at_ParseSetCmdType(void *scanner, uint8_t type)
{
    ATCMDLOG(V, "AT CMD parse set cmd type=%d", type);
    struct AT_PARSE_CONTEXT_T *ctx = (struct AT_PARSE_CONTEXT_T *)at_parse_get_extra(scanner);
    if (ctx->currcmd == NULL)
        return AT_PARSE_ERR_NO_CMD;
    ctx->currcmd->type = type;
    return 0;
}

// =============================================================================
// at_ParsePushParamText
// =============================================================================
static int at_ParsePushParamText(struct AT_PARSE_CONTEXT_T *ctx, uint8_t type, char *text, unsigned leng)
{
    ATCMDLOGS(V, TSM(2), "AT CMD parse push param type=%d len=%d: %s", type, leng, text);
    if (ctx->currcmd == NULL)
        return AT_PARSE_ERR_NO_CMD;
    if (ctx->currcmd->paramCount >= AT_COMMAND_PARAM_MAX)
        return AT_PARSE_ERR_TOO_MANY_PARAM;

    // HACK: the last ';' can be handled, so remove it here
    if (type == AT_CMDPARAM_DSTRING && text[leng - 1] == ';')
        leng--;

    AT_COMMAND_PARAM_T *param = (AT_COMMAND_PARAM_T *)at_malloc(sizeof(*param) + leng);
    param->type = type;
    param->length = leng;
    memcpy(param->value, text, leng);
    param->value[leng] = 0;
    ctx->currcmd->params[ctx->currcmd->paramCount++] = param;
    return 0;
}

// =============================================================================
// at_ParsePushNumber
// =============================================================================
int at_ParsePushNumber(void *scanner)
{
    struct AT_PARSE_CONTEXT_T *ctx = (struct AT_PARSE_CONTEXT_T *)at_parse_get_extra(scanner);
    return at_ParsePushParamText(ctx, AT_CMDPARAM_NUMBER, at_parse_get_text(scanner), at_parse_get_leng(scanner));
}

// =============================================================================
// at_ParsePushString
// =============================================================================
int at_ParsePushString(void *scanner)
{
    struct AT_PARSE_CONTEXT_T *ctx = (struct AT_PARSE_CONTEXT_T *)at_parse_get_extra(scanner);
    return at_ParsePushParamText(ctx, AT_CMDPARAM_STRING, at_parse_get_text(scanner), at_parse_get_leng(scanner));
}

// =============================================================================
// at_ParsePushDString
// =============================================================================
int at_ParsePushDString(void *scanner)
{
    struct AT_PARSE_CONTEXT_T *ctx = (struct AT_PARSE_CONTEXT_T *)at_parse_get_extra(scanner);
    return at_ParsePushParamText(ctx, AT_CMDPARAM_DSTRING, at_parse_get_text(scanner), at_parse_get_leng(scanner));
}

// =============================================================================
// at_ParsePushDtmfchar
// =============================================================================
int at_ParsePushDtmfchar(void *scanner)
{
    struct AT_PARSE_CONTEXT_T *ctx = (struct AT_PARSE_CONTEXT_T *)at_parse_get_extra(scanner);
    return at_ParsePushParamText(ctx, AT_CMDPARAM_DTMFCHAR, at_parse_get_text(scanner), at_parse_get_leng(scanner));
}

// =============================================================================
// at_ParsePushEmpty
// =============================================================================
int at_ParsePushEmpty(void *scanner)
{
    struct AT_PARSE_CONTEXT_T *ctx = (struct AT_PARSE_CONTEXT_T *)at_parse_get_extra(scanner);
    return at_ParsePushParamText(ctx, AT_CMDPARAM_EMPTY, "", 0);
}

// =============================================================================
// at_ParseLine
// =============================================================================
int at_ParseLine(const uint8_t *cmdline, unsigned length, AT_COMMAND_T **cmds)
{
    int result = 0;
    void *scanner = NULL;
    struct AT_PARSE_CONTEXT_T ctx;

    *cmds = NULL;
    ctx.cmds = NULL;
    ctx.currcmd = NULL;

    if (at_parse_lex_init_extra(&ctx, &scanner) != 0)
    {
        result = AT_PARSE_ERR_NOMEM;
        goto destroy_exit;
    }

    if (at_parse__scan_bytes(cmdline, length, scanner) == NULL)
    {
        result = AT_PARSE_ERR_NOMEM;
        goto destroy_exit;
    }

    if (at_parse_parse(scanner) != 0)
    {
        result = AT_PARSE_ERR_SYNTAX;
        goto destroy_exit;
    }

    *cmds = ctx.cmds;
    ctx.cmds = NULL;

destroy_exit:
    at_parse_lex_destroy(scanner);
    return result;
}

// =============================================================================
// at_ParamUint
// =============================================================================
uint32_t at_ParamUint(AT_COMMAND_PARAM_T *param, bool *paramok)
{
    if (!*paramok || param == NULL || param->type != AT_CMDPARAM_NUMBER)
        goto failed;

    return strtoul((const char *)param->value, NULL, 10);

failed:
    *paramok = false;
    return 0;
}

// =============================================================================
// at_ParamUintInRange
// =============================================================================
uint32_t at_ParamUintInRange(AT_COMMAND_PARAM_T *param, uint32_t minval,
                             uint32_t maxval, bool *paramok)
{
    if (!*paramok || param == NULL || param->type != AT_CMDPARAM_NUMBER)
        goto failed;

    uint32_t res = strtoul((const char *)param->value, NULL, 10);
    if (res < minval || res > maxval)
        goto failed;
    return res;

failed:
    *paramok = false;
    return minval;
}

// =============================================================================
// at_ParamDefUintInRange
// =============================================================================
uint32_t at_ParamDefUintInRange(AT_COMMAND_PARAM_T *param, uint32_t defval,
                                uint32_t minval, uint32_t maxval, bool *paramok)
{
    if (!*paramok)
        goto failed;
    if (param == NULL || param->type == AT_CMDPARAM_EMPTY)
        return defval;
    if (param->type != AT_CMDPARAM_NUMBER)
        goto failed;

    uint32_t res = strtoul((const char *)param->value, NULL, 10);
    if (res < minval || res > maxval)
        goto failed;
    return res;

failed:
    *paramok = false;
    return defval;
}

// =============================================================================
// at_ParamUintCheckList (check a value in list)
// =============================================================================
static bool at_ParamUintCheckList(uint32_t val, const uint32_t *list, unsigned count)
{
    for (unsigned n = 0; n < count; n++)
    {
        if (val == *list++)
            return true;
    }
    return false;
}

// =============================================================================
// at_ParamUintInList
// =============================================================================
uint32_t at_ParamUintInList(AT_COMMAND_PARAM_T *param, const uint32_t *list,
                            unsigned count, bool *paramok)
{
    if (!*paramok || param == NULL || param->type != AT_CMDPARAM_NUMBER)
        goto failed;

    uint32_t res = strtoul((const char *)param->value, NULL, 10);
    if (!at_ParamUintCheckList(res, list, count))
        goto failed;
    return res;

failed:
    *paramok = false;
    return list[0];
}

// =============================================================================
// at_ParamDefUintInList
// =============================================================================
uint32_t at_ParamDefUintInList(AT_COMMAND_PARAM_T *param, uint32_t defval,
                               const uint32_t *list, unsigned count, bool *paramok)
{
    if (!*paramok)
        goto failed;
    if (param == NULL || param->type == AT_CMDPARAM_EMPTY)
        return defval;
    if (param->type != AT_CMDPARAM_NUMBER)
        goto failed;

    uint32_t res = strtoul((const char *)param->value, NULL, 10);
    if (!at_ParamUintCheckList(res, list, count))
        goto failed;
    return res;

failed:
    *paramok = false;
    return defval;
}

// =============================================================================
// at_ParamStrParse (param is already checked)
// =============================================================================
static const uint8_t *at_ParamStrParse(AT_COMMAND_PARAM_T *param)
{
    if (param->type == AT_CMDPARAM_STRING_PARSED)
        return param->value;

    // param->value must be started and ended with double-quote
    uint8_t *s = param->value + 1;
    uint8_t *d = param->value;
    uint16_t length = param->length - 2;
    while (length > 0)
    {
        uint8_t c = *s++;
        length--;
        // It is more permissive than SPEC.
        if (c == '\\' && length >= 2 && SUL_isalnum(s[0]) && SUL_isalnum(s[1]))
        {
            *d++ = (HEX2NUM(s[0]) << 4) | HEX2NUM(s[1]);
            s += 2;
            length -= 2;
        }
        else
        {
            *d++ = c;
        }
    }
    *d = '\0';
    param->type = AT_CMDPARAM_STRING_PARSED;
    return param->value;
}

// =============================================================================
// at_ParamStr
// =============================================================================
const uint8_t *at_ParamStr(AT_COMMAND_PARAM_T *param, bool *paramok)
{
    if (!*paramok || param == NULL ||
        !(param->type == AT_CMDPARAM_STRING || param->type == AT_CMDPARAM_STRING_PARSED) ||
        param->length < 2)
    {
        *paramok = false;
        return "";
    }

    return at_ParamStrParse(param);
}

// =============================================================================
// at_ParamOptStr
// =============================================================================
const uint8_t *at_ParamOptStr(AT_COMMAND_PARAM_T *param, bool *paramok)
{
    if (!*paramok)
        goto failed;
    if (param == NULL || param->type == AT_CMDPARAM_EMPTY)
        return NULL;
    if (!(param->type == AT_CMDPARAM_STRING || param->type == AT_CMDPARAM_STRING_PARSED) ||
        param->length < 2)
        goto failed;

    return at_ParamStrParse(param);

failed:
    *paramok = false;
    return NULL;
}

// =============================================================================
// at_ParamDtmf
// =============================================================================
const uint8_t *at_ParamDtmf(AT_COMMAND_PARAM_T *param, bool *paramok)
{
    if (!*paramok)
        goto failed;
    if (param == NULL || param->type == AT_CMDPARAM_EMPTY)
        goto failed;
    if (param->type == AT_CMDPARAM_DTMF_PARSED || param->type == AT_CMDPARAM_DTMFCHAR)
        return param->value;

    if (param->type == AT_CMDPARAM_NUMBER)
    {
        if (param->length == 1)
            return param->value;
        goto failed;
    }

    if (param->type == AT_CMDPARAM_STRING)
    {
        if (param->length < 2)
            goto failed;

        int np = 0;
        for (int n = 1; n < param->length - 1; n++)
        {
            if (param->value[n] == ',' || param->value[n] == ' ')
                continue;
            param->value[np++] = param->value[n];
        }
        param->length = np;
        param->value[np] = 0;
        param->type = AT_CMDPARAM_DTMF_PARSED;
        return param->value;
    }

failed:
    *paramok = false;
    return "";
}
