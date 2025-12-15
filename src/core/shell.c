#include "shell.h"
#include "parser.h"
#include "io.h"
#include "stdlib.h"
#include <string.h>
#include "memory.h"
#include "builtins.h"

MSH_ExecContext *MSH_ExecContext_g = NULL;

void MSH_PrintBanner(void) {
    printf(
       M"\n\n"
        "███╗   ███╗███████╗██╗  ██╗    ██╗    ██╗██╗██████╗      ███╗        ███╗ \n"
        "████╗ ████║██╔════╝██║  ██║    ██║    ██║██║██╔══██╗    ██╔██╗      ██╔██╗\n"
        "██╔████╔██║███████╗███████║    ██║ █╗ ██║██║██████╔╝    ╚═╝╚═╝█████╗╚═╝╚═╝\n"
        "██║╚██╔╝██║╚════██║██╔══██║    ██║███╗██║██║██╔═══╝           ╚════╝      \n"
        "██║ ╚═╝ ██║███████║██║  ██║    ╚███╔███╔╝██║██║                           \n"
        "╚═╝     ╚═╝╚══════╝╚═╝  ╚═╝     ╚══╝╚══╝ ╚═╝╚═╝                           \n"
    "\n\n"RST);
}

void MSH_Loop(MSH_ShellConfig config)
{
    char* line;
    MSH_Token* args;
    char** s_args;

    MSH_CreateContext();
    MSH_InsertVar("vfs_path", config.VFS_path);
    MSH_InsertVar("startup_script", config.startup_script);
    VFS_LoadXML(MSH_GetVar("vfs_path"));
    MSH_PrintBanner();
    // REPL
    while (MSH_ExecContext_g->running && (line = MSH_read_line())) {
        MSH_AddToHistory(line);
        MSH_ExpandVariables(&line);
        args = MSH_SplitLine(line);
        s_args = MSH_ParseTokens(args);
        
        if (!s_args) {
            for (size_t i = 0; i < sizeof(args) / sizeof(*args); ++i)
            {
                if (args[i].str >= line && args[i].str < line + strlen(line))
                {
                    if (args[i].owns)
                        free(args[i].str);
                }
            }
            free(line);
            free(args);
            continue;
        }

        MSH_ExecuteCommand(s_args);

        // Cleanup
        for (size_t i = 0; i < sizeof(args) / sizeof(*args); ++i)
        {
            if (args[i].str >= line && args[i].str < line + strlen(line))
            {
                if (args[i].owns)
                    free(args[i].str);
            }
        }
        free(line);
        free(args);
        free(s_args);
    }

    MSH_FreeContext();
}

void MSH_ExecuteCommand(const char **args) {
    if (!args || !*args) { return; }

    const char *cmd_name = args[0];
    MSH_BuiltinFn command = MSH_FindBuiltin(cmd_name);
    if (command) { command(args + 1); }
    else {
        MSH_perror("Command '%s' not found.\n", cmd_name);
    }
}

void MSH_CreateContext(void) {
    MSH_ExecContext_g = MSH_Malloc(sizeof(MSH_ExecContext));
    MSH_ExecContext_g->cwd = MSH_Malloc(MSH_BUFSIZE * sizeof(char));
    MSH_ExecContext_g->cwd[0] = '/';
    MSH_ExecContext_g->cwd[1] = '\0';
    MSH_ExecContext_g->running = 1;
    MSH_CreateEnv(MSH_BUFSIZE);
    MSH_CreateHistory(MSH_BUFSIZE);
}

void MSH_FreeContext(void) {
    MSH_FreeEnv();
    MSH_ClearHistory();
    free(MSH_ExecContext_g->cwd);
    free(MSH_ExecContext_g);
}

void MSH_CreateHistory(size_t buf_size) {
    if (!MSH_ExecContext_g) {
        MSH_perror("No context to create history\n");
        return;
    }

    char **lines = MSH_Malloc(buf_size * sizeof(*lines));
    MSH_ExecContext_g->history.cap = buf_size;
    MSH_ExecContext_g->history.lines = lines;
    MSH_ExecContext_g->history.len = 0;
}

void MSH_AddToHistory(const char *line) {
    if (!MSH_ExecContext_g) {
        MSH_perror("No context to add history\n");
        return;
    }

    if (!MSH_ExecContext_g->history.lines) {
        MSH_CreateHistory(MSH_BUFSIZE);
    }

    size_t line_sz = strlen(line);
    char *new_line = MSH_Malloc((line_sz + 1) * sizeof(*line));
    memcpy(new_line, line, line_sz);
    new_line[line_sz] = '\0';

    if (MSH_ExecContext_g->history.len - 1 >= MSH_ExecContext_g->history.cap) {
        size_t new_cap = 1 + MSH_ExecContext_g->history.cap * 3 / 2;
        MSH_ExecContext_g->history.lines = MSH_Realloc(
            MSH_ExecContext_g->history.lines,
            new_cap * sizeof(*MSH_ExecContext_g->history.lines)
        );
        MSH_ExecContext_g->history.cap = new_cap;
    }

    MSH_ExecContext_g->history.lines[MSH_ExecContext_g->history.len++] = new_line;
}

void MSH_ClearHistory(void) {
    if (!MSH_ExecContext_g) {
        return;
    }

    if (!MSH_ExecContext_g->history.lines) {
        return;
    }

    for (int idx = 0; idx < MSH_ExecContext_g->history.len; ++idx) {
        free(MSH_ExecContext_g->history.lines[idx]);
    }

    free(MSH_ExecContext_g->history.lines);
}

void MSH_FlushHistory(void) {
    MSH_ClearHistory();
    MSH_CreateHistory(MSH_BUFSIZE);
}