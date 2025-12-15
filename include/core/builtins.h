#ifndef BUILTINS_H
#define BUILTINS_H

typedef int (*MSH_BuiltinFn)(const char **);

typedef struct
{
    const char *name;
    MSH_BuiltinFn fn;
} builtin;

MSH_BuiltinFn MSH_FindBuiltin(const char *name);


#endif