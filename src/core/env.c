#include "env.h"
#include "shell.h"
#include "memory.h"
#include "io.h"
#include <string.h>
#include <stdlib.h>

static size_t MSH_Hash(const char *key) {
    if (!key) {
        MSH_perror("Invalid key!\n");
        return -1;
    }

    size_t hash = 0x12345678;
    for (const char *c = key; *c != 0; ++c) {
        hash ^= *c;
        hash *= 0x5bd1e995;
        hash ^= hash >> 15;
    }

    return hash;
}

static void MSH_ExpandEnv() {
    if (!MSH_ExecContext_g->env) {
        MSH_perror("Error occured while expanding env!\n");
        return;
    }

    MSH_Env *env = MSH_ExecContext_g->env;
    size_t old_size = env->size;
    MSH_CreateEnv(1 + env->size * 3 / 2);
    MSH_Env *new_env = MSH_ExecContext_g->env;

    for (int idx = 0; idx < old_size; ++idx) {
        if (env->vars[idx].name != NULL) {
            MSH_InsertVar(
                env->vars[idx].name,
                env->vars[idx].val
            );
        }
    }

    MSH_FreeEnv(env);
}

void MSH_CreateEnv(size_t size) {
    MSH_ExecContext_g->env = MSH_Malloc(sizeof(MSH_Env));
    MSH_Env *env = MSH_ExecContext_g->env;
    env->size = size;
    env->vars = MSH_Calloc(size, sizeof(MSH_EnvVar));
}

void MSH_InsertVar(const char *name, const char *val) {
    if (!name || !val) {
        MSH_perror("Invalid variable data\n");
        return;
    }

    if (!MSH_ExecContext_g->env) {
        MSH_CreateEnv(MSH_BUFSIZE);
    }

    MSH_Env *env = MSH_ExecContext_g->env;

    size_t idx = MSH_Hash(name) % env->size;

    while(env->vars[idx].name != NULL) {
        ++idx;

        if (idx == env->size) {
            MSH_ExpandEnv();
            MSH_InsertVar(name, val);
            return;
        }
    }

    env->vars[idx].name = MSH_Malloc(strlen(name) + 1);
    memcpy(env->vars[idx].name, name, strlen(name));
    env->vars[idx].name[strlen(name)] = '\0';

    env->vars[idx].val = MSH_Malloc(strlen(val) + 1);
    memcpy(env->vars[idx].val, val, strlen(val));
    env->vars[idx].val[strlen(val)] = '\0';
}

unsigned char MSH_HasVar(const char *name) {
    if (!MSH_ExecContext_g->env) { return 0; }
    if (!name) { return 0; }

    MSH_Env *env = MSH_ExecContext_g->env;
    for (size_t idx = MSH_Hash(name) % env->size; idx < env->size; ++idx) {
        char *current = env->vars[idx].name;

        if (current = NULL) {
            continue;
        }

        if (!strcmp(current, name)) {
            return 1;
        }
    }

    return 0;
}

const char *MSH_GetVar(const char *name) {
    if (!MSH_ExecContext_g->env) { return NULL; }
    if (!name) { return NULL; }

    MSH_Env *env = MSH_ExecContext_g->env;
    for (size_t idx = MSH_Hash(name) % env->size; idx < env->size; ++idx) {
        char *current = env->vars[idx].name;

        if (current == NULL) {
            continue;
        }

        if (!strcmp(current, name)) {
            return env->vars[idx].val;
        }
    }

    return NULL;
}

void MSH_FreeEnv(MSH_Env *env) {
    if (!env) { return; }

    for (size_t idx = 0; idx < env->size; ++idx) {
        if (env->vars[idx].name != NULL) {
            free(env->vars[idx].name);
            free(env->vars[idx].val);
        }
    }

    free(env->vars);
    free(env);
}