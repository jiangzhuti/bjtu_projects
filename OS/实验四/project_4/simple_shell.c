#include "simplefs.h"
#include <string.h>
#include <stdio.h>

static char prompt[8];
static char go_exit;
static char cmd_name[16];
static char cmd_args[16][16];
static unsigned arg_cnt;

static int ss_nop();
static int ss_ls();
static int ss_touch();
static int ss_rm();
static int ss_exit();
static int ss_cat();
static int ss_echo();
static int ss_mkfs();
static int ss_loadfs();
static int ss_storefs();

struct builtin_cmd_t {
    const char *cmd_name;
    int (*cmd_fn)(void);
    unsigned arg_cnt;
};

#define BUILTIN_CMD_CNT 10
struct builtin_cmd_t builtin_cmd_table [BUILTIN_CMD_CNT] = {
{"NOP", ss_nop, 0},
{"mkfs", ss_mkfs, 1},
{"loadfs", ss_loadfs, 1},
{"storefs", ss_storefs, 1},
{"ls", ss_ls, 0},
{"touch", ss_touch, 1},
{"rm", ss_rm, 1},
{"cat", ss_cat, 1},
{"echo", ss_echo, 2},
{"exit", ss_exit, 0}
};

static int ss_nop()
{return 0;}

static int ss_mkfs()
{
    return sf_mkfs(cmd_args[0]);
}

static int ss_loadfs()
{
    return sf_load(cmd_args[0]);
}

static int ss_storefs()
{
    return sf_store(cmd_args[0]);
}

static int ss_ls()
{
    sf_directory();
    return 0;
}

static int ss_touch()
{
    return sf_create(cmd_args[0]);
}

static int ss_rm()
{
    return sf_destroy(cmd_args[0]);
}

static int ss_cat()
{
    static char buffer[256];
    unsigned i, count;
    sf_file file = sf_open(cmd_args[0]);
    if (file < 0) return file;
    count = sf_read(file, buffer, 256);
    for (i = 0; i < count; ++i)
        putchar(buffer[i]);
    sf_close(file);
    return 0;
}

static int ss_echo()
{
    static char buffer[256];
    sprintf(buffer, "%s", cmd_args[0]);
    sf_file file = sf_open(cmd_args[1]);
    if (file < 0) return file;
    sf_write(file, buffer, strlen(buffer));
    sf_close(file);
    return 0;
}

static int ss_exit()
{
    go_exit = 1;
    return 0;
}

static void ss_get_cmd()
{
    const char *dlmt = " ";
    char *pch;
    static char cmdline[32];
    unsigned len;
    fgets(cmdline, 32, stdin);
    len = strlen(cmdline);
    if (cmdline[len - 1] == '\n') {
        cmdline[len - 1] = '\0';
        --len;
    }
    arg_cnt = 0;
    pch = strtok(cmdline, dlmt);
    if (pch == NULL) {
        strcpy(cmd_name, "NOP");
    }
    strcpy(cmd_name, pch);
    pch = strtok(NULL, dlmt);
    while (pch != NULL) {
        strcpy(cmd_args[arg_cnt], pch);
        arg_cnt++;
        pch = strtok(NULL, dlmt);
    }
}

static void ss_exec_cmd()
{
    unsigned i;
    int ret;
    for (i = 0; i < BUILTIN_CMD_CNT; ++i) {
        if (!strcmp(cmd_name, builtin_cmd_table[i].cmd_name)) {
            if (builtin_cmd_table[i].arg_cnt > arg_cnt) {
                printf("too few arg for command %s\n", builtin_cmd_table[i].cmd_name);
                return;
            }
            ret = builtin_cmd_table[i].cmd_fn();
            if (ret < 0)
                puts(sf_err(ret));
            return;
        }
    }
    puts("UNKONWN COMMAND");
}

static void ss_set_prompt(const char *pmt)
{
    strcpy(prompt, pmt);
}

void ss_start(const char *s)
{
    ss_set_prompt(s);
    go_exit = 0;
    while (!go_exit) {
        printf("%s", prompt);
        ss_get_cmd();
        ss_exec_cmd();
    }
}
