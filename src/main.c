#include "shell.h"
#include <stdio.h>

#define PROJECT_DIR "/home/chinchopa/projects/MSH"

int main(int argc, char* argv[])
{
    char src_dir[1024];

    MSH_ShellConfig config = {
        .VFS_path = PROJECT_DIR"/VFS.xml",
        .startup_script = PROJECT_DIR"scripts/startup"
    };
    MSH_Loop(config);
    return 0;
}