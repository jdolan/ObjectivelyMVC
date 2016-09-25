#include <dirent.h>

#undef UNICODE

#include <errno.h>
#include <windows.h>

struct DIR {
    HANDLE h;
    struct dirent d;
    WIN32_FIND_DATA ffd;
    BOOL valid;
};

DIR *opendir(const char *dirname) {
    DIR *d = malloc(sizeof(DIR));
    char *name = malloc(strlen(dirname) + 3);
    strcpy(name, dirname);
    strcat(name, "\\*");

    d->h = FindFirstFile(name, &d->ffd);

    free(name);

    if (!d->h) {
        free(d);
        d = NULL;
        if (GetLastError() == ERROR_FILE_NOT_FOUND)
            errno = ENOENT;
        else
            errno = EACCES;
    }
    d->valid = TRUE;
    return d;
}

struct dirent *readdir(DIR *dirp) {
    if (!dirp->valid) return NULL;
    dirp->d.d_name = dirp->ffd.cFileName;
    if (!FindNextFile(dirp->h, &dirp->ffd)) dirp->valid = FALSE;
    return &dirp->d;
}

int closedir(DIR *dirp) {
    FindClose(dirp->h);
    free(dirp);
    return 0;
}
