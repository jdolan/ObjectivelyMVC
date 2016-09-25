#pragma once

#include <io.h>

#define S_ISDIR(m) (((m) & _S_IFMT) == _S_IFDIR)

#define F_OK 0
#define X_OK 0 /* no execute bit on windows */
#define W_OK 2
#define R_OK 4

typedef struct DIR DIR;
typedef struct dirent {
    const char *d_name;
} dirent;

DIR *opendir(const char *dirname);
struct dirent *readdir(DIR *dirp);
int closedir(DIR *dirp);
