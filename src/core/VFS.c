#include "VFS.h"
#include "io.h"
#include "shell.h"
#include "memory.h"
#include <string.h>

void VFS_LoadXML(const char *vfs_path) {
    xmlDocPtr doc = xmlReadFile(vfs_path, NULL, 0);
    if (doc == NULL) {
        MSH_perror("Failed to parse\n");
        return;
    }

    xmlNodePtr root = xmlDocGetRootElement(doc);
    if (root == NULL) {
        MSH_perror("Empty document\n");
        xmlFreeDoc(doc);
        return;
    }

    MSH_ExecContext_g->vfs = MSH_Malloc(sizeof(VFS));
    VFS* _vfs = MSH_ExecContext_g->vfs;
    _vfs->doc = doc;
    _vfs->root = root;
    _vfs->xml_path = vfs_path;
}

static char **VFS_SplitPath(const char *path, int *parts_n) {
    if (!path) { *parts_n = 0; return NULL;}
    char *tmp;
    if (path[0] == '/')
        tmp = strdup(path + 1);
    else 
        tmp = strdup(path);
    int cap = 8, n = 0;
    char **parts = MSH_Malloc(cap * sizeof(*parts));
    char *tok = strtok(tmp, "/");
    while (tok) {
        if (n >= cap) { cap = 1 + cap * 3 / 2; parts = MSH_Realloc(parts, cap * sizeof(char*)); }
        parts[n++] = strdup(tok);
        tok = strtok(NULL, "/");
    }
    free(tmp);
    *parts_n = n;
    return parts;
}

static void free_parts(char **parts, int n) {
    if (!parts) return;
    for (int idx = 0; idx < n; ++idx) { free(parts[idx]); }
    free(parts);
}

xmlNodePtr VFS_Locate(const char *path) {
    if (!MSH_ExecContext_g->vfs || !path) { return NULL; }
    VFS *vfs = MSH_ExecContext_g->vfs;
    if (!strcmp(path, "/")) { return vfs->root; }

    
    int n;
    char **parts = VFS_SplitPath(path, &n);

    if (n == 0) { free_parts(parts,n); return vfs->root; }

    xmlNodePtr cur;
    if (path[0] != '/') { cur = VFS_Locate(VFS_GetCWD()); }
    else { cur = vfs->root; }

    for (int idx = 0; idx < n && cur; ++idx) {
        xmlNodePtr child;
        xmlNodePtr found = NULL;
        for (child = cur->children; child; child = child->next) {
            if (child->type != XML_ELEMENT_NODE) continue;
            xmlChar *nm = xmlGetProp(child, (const xmlChar*)"name");
            if (!nm) continue;
            if (!strcmp((const char*)nm, parts[idx])) {
                found = child;
                xmlFree(nm);
                break;
            }
            xmlFree(nm);
        }
        cur = found;
    }

    free_parts(parts,n);

    return cur; // Might be NULL
}

char *VFS_GetPath(xmlNodePtr node) {
    if (!node) { return NULL; }
    
    char *path = MSH_Malloc(MSH_BUFSIZE * sizeof(*path));
    path[0] = '/';
    path[1] = '\0';

    if (!node->parent || !node->parent->parent) { return path; }

    size_t path_cap = MSH_BUFSIZE;
    size_t path_len = 1;
    xmlNodePtr cur = node;
    xmlNodePtr traversed[MSH_BUFSIZE];
    int position = 0;
    while (cur->parent && cur->parent->parent) {
        if (position >= MSH_BUFSIZE) { MSH_perror("Too long path. WCYD!\n"); free(path); return NULL; }
        traversed[position++] = cur;
        cur = cur->parent;
    }

    for (--position; position >= 0; --position) {
        xmlChar *name = xmlGetProp(traversed[position], (const xmlChar*)"name");
        if (name) {
            MSH_Strcat(&path, name, &path_cap, &path_len, strlen(name));
            path[path_len++] = '/';
            path[path_len] = '\0';
        }
        xmlFree(name);
    }

    path[path_len - 1] = '\0';

    return path;
}

char *VFS_GetCWD(void) {
    return MSH_ExecContext_g->cwd;
}

void VFS_Clear(void) {
    if (!MSH_ExecContext_g || !MSH_ExecContext_g->vfs) { return; }
    xmlFreeDoc(MSH_ExecContext_g->vfs->doc);
    xmlCleanupParser();
}