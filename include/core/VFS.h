#ifndef VFS_H
#define VFS_H

#include <stddef.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

typedef enum {
    VFS_DIRECTORY,
    VFS_FILE
} VFS_NodeType;

typedef struct {
    char *xml_path;
    xmlDocPtr doc;
    xmlNodePtr root;
} VFS;

void VFS_Create();
void VFS_LoadXML(const char *path);
void VFS_Serialize(const char *path);
xmlNodePtr VFS_Locate(const char *path);
char *VFS_GetPath(xmlNodePtr node);

void VFS_Mkdir(const char *path);
void VFS_Rmdir(const char *path);
void VFS_Touch(const char *path);
char *VFS_GetCWD(void);

void VFS_Clear(void);


#endif