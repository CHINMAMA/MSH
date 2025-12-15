#include "io.h"
#include "VFS.h"
#include <readline/readline.h>

char *MSH_read_line(void)
{
    char *cwd = VFS_GetCWD();
    char prompt[MSH_PROMPTSIZE] = "";
    strncat(prompt, G"chinchopa"RST":"B, MSH_PROMPTSIZE - strlen(prompt));
    strncat(prompt, cwd, MSH_PROMPTSIZE - strlen(prompt));
    strncat(prompt, RST"$ ", MSH_PROMPTSIZE - strlen(prompt));

    char *line = readline(prompt);

    if (line == NULL)
    {
        MSH_perror("EOF\n");
        return NULL;
    }

    return line;
}