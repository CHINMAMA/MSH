#include <stddef.h>
#include "shell.h"
#include "builtins.h"
#include <string.h>
#include "io.h"

int cmd_exit(const char **args);
int cmd_conf_dump(const char **args);
// int cmd_vfs_info(const char **args);
int cmd_ls(const char **args);
int cmd_cd(const char **args);
int cmd_echo(const char **args);
// int cmd_uname(const char **args);
int cmd_history(const char **args);
// int cmd_mkdir(const char **args);
// int cmd_rmdir(const char **args);
// int cmd_touch(const char **args);

builtin builtins[] = {
    { "exit", cmd_exit },
    { "conf-dump", cmd_conf_dump },
    // { "vfs-info", cmd_vfs_info },
    { "ls", cmd_ls },
    { "cd", cmd_cd },
    { "echo", cmd_echo },
    // { "uname", cmd_uname },
    { "history", cmd_history },
    // { "mkdir", cmd_mkdir },
    // { "rmdir", cmd_rmdir },
    // { "touch", cmd_touch },
    { NULL, NULL }
};

MSH_BuiltinFn MSH_FindBuiltin(const char *name) {
    for (int i = 0; builtins[i].name; ++i) {
        if (strcmp(builtins[i].name, name) == 0) { return builtins[i].fn; }
    }
    return NULL;
}

int cmd_exit(const char **) {
    MSH_ExecContext_g->running = 0;
    return 0;
}

int cmd_conf_dump(const char **) {
    const char *vfs_path = MSH_GetVar("vfs_path");
    const char *startup_script = MSH_GetVar("startup_script");
    printf("vfs_path: %s\n", vfs_path);
    printf("startup_script: %s\n", startup_script);
    return 0;
}

int cmd_history(const char **) {
    if (!MSH_ExecContext_g) {
        MSH_perror("Context not initialized\n");
        return 1;
    }

    if (!MSH_ExecContext_g->history.lines) {
        return 0;
    }

    MSH_History hist = MSH_ExecContext_g->history;
    for (int idx = 0; idx < hist.len; ++idx) {
        printf("%d: %s\n", idx + 1, hist.lines[idx]);
    }

    return 0;
}

int cmd_echo(const char **args) {
    if (!args) { return 0; }
    while (*args) {
        printf("%s ", *args);
        ++args;
    }

    printf("\n");
}

int cmd_cd(const char **args) {
    if (!args || !*args) { return 0; }

    xmlNodePtr cur = VFS_Locate(*args);
    if (!cur) { MSH_perror("Path '%s' does not exist.\n", *args); return 1; }
    if (strcmp(cur->name, "dir")) { MSH_perror("'%s' is not a directory.\n", *args); return 1; }
    free(MSH_ExecContext_g->cwd);
    MSH_ExecContext_g->cwd = VFS_GetPath(cur);

    return 0;
}

int cmd_ls(const char **) {
    xmlNodePtr cur = VFS_Locate(VFS_GetCWD());
    if (!cur->children) { return 0; }
    cur = cur->children;
    do {
        xmlChar *name = xmlGetProp(cur, (const xmlChar*)"name");
        if (cur->type == XML_ELEMENT_NODE && name) {
            printf("%s ", name);
        }
        xmlFree(name);

        if (cur->next) { cur = cur->next; }
    } while (cur->next);

    printf("\n");
}