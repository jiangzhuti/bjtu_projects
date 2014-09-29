#include "monitor_def.h"
#include "monitor_common.h"
#include <sys/time.h>

int exec_cmdline(const char *cmdline)
{
    pid_t pid;
    if (cmdline == NULL) {
        return 0;
    }

    if ((pid = fork()) < 0) {
        return 0;
    } else if (pid == 0) {
        execl("/bin/sh", "sh", "-c", cmdline, (char *)0);
        _exit(127);
    } else {
        return pid;
    }
}

