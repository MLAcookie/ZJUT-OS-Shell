#pragma once
#include "tokenizer.h"

typedef int cmd_fun_t(struct tokens *tokens);

typedef struct fun_desc
{
    cmd_fun_t *fun;
    char *cmd;
    char *doc;
} fun_desc_t;

extern fun_desc_t cmd_table[];

int cmd_get_size();

int cmd_exit(struct tokens *tokens);
int cmd_pwd(struct tokens *tokens);
int cmd_cd(struct tokens *tokens);
int cmd_help(struct tokens *tokens);
