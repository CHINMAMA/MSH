#ifndef ENV_H
#define ENV_H

#include <stddef.h>

typedef struct {
    char *name;
    char *val;
} MSH_EnvVar;

typedef struct {
    MSH_EnvVar *vars;
    size_t size;
} MSH_Env;

void MSH_CreateEnv(size_t size);
void MSH_InsertVar(const char *name, const char *val);
unsigned char MSH_HasVar(const char *name);
const char *MSH_GetVar(const char *name);
void MSH_FreeEnv();


#endif