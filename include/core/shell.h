#ifndef SHELL_H
#define SHELL_H
#include "env.h"
#include "VFS.h"

typedef struct {
    char **lines;
    size_t len;
    size_t cap;
} MSH_History;

typedef struct {
    VFS* vfs;
    MSH_History history;
    MSH_Env* env;
    char *cwd;
    unsigned char running;
} MSH_ExecContext;

typedef struct {
    const char *VFS_path;
    const char *startup_script;
} MSH_ShellConfig;

extern MSH_ExecContext *MSH_ExecContext_g;

void MSH_CreateContext(void);
void MSH_Loop(MSH_ShellConfig config);
void MSH_ExecuteCommand(const char **args);
void MSH_FreeContext(void);
void MSH_CreateHistory(size_t buf_size);
void MSH_AddToHistory(const char *line);
void MSH_FlushHistory(void);
void MSH_ClearHistory(void);

#endif