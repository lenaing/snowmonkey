#include "datafiles.h"

char *
get_data_dirs(char *component) {
    char *xdgTmp;
    char *envTmp;
    char *dataDirs;
    
    char *token = NULL;
    char *delimiters = ":";
    char *saved = NULL;

    dataDirs = calloc(MAX_STRING_LENGTH, sizeof(char));

    /* Parse XDG_DATA_HOME first. */
    xdgTmp = getenv("XDG_DATA_HOME");
    if (NULL != xdgTmp) {
        strcat(dataDirs, xdgTmp);

        if (! is_slashed(xdgTmp)) {
            strcat(dataDirs, "/");
        }

        strcat(dataDirs, component);
        strcat(dataDirs, "/");
    }

    /* Parse XDG_DATA_DIRS now. */
    xdgTmp = getenv("XDG_DATA_DIRS");
    if (NULL != xdgTmp) {

        envTmp = onsen_strdup(xdgTmp);
        if (NULL != envTmp) {

            token = strtok_r(envTmp, delimiters, &saved);
            while (NULL != token) {
                strcat(dataDirs, ":");
                strcat(dataDirs, token);

                if (! is_slashed(token)) {
                    strcat(dataDirs, "/");
                }
                strcat(dataDirs, component);
                strcat(dataDirs, "/");

                token = strtok_r(NULL, delimiters, &saved);
            }

            onsen_free(envTmp);
        }
    }

    return dataDirs;
}

char **
find_data_files(char *search, char *dataDirs) {
    int i;
    int count = 0;
    wordexp_t mots;
    char *tmpDir;
    char *curDir;
    char *delimiters = ":";
    char *saved;
    
    char **filenames;
    char *filename;

    filenames = calloc(MAX_SEARCH_RESULTS, sizeof(char *));
    tmpDir = calloc(MAX_STRING_LENGTH, sizeof(char));

    curDir = strtok_r(dataDirs, delimiters, &saved);
    strcat(tmpDir, curDir);
    if (! is_slashed(curDir)) {
        strcat(tmpDir, "/");
    }
    strcat(tmpDir, search);

    wordexp(tmpDir, &mots, WRDE_NOCMD);
    while (NULL != curDir) {
        curDir = strtok_r(NULL, delimiters, &saved);
        if (NULL != curDir) {
            tmpDir[0] = '\0';
            strcat(tmpDir, curDir);
            if (! is_slashed(curDir)) {
                strcat(tmpDir, "/");
            }
            strcat(tmpDir, search);
            wordexp(tmpDir, &mots, WRDE_APPEND|WRDE_NOCMD);
        }
    }

    for (i = 0; i < (int)((&mots)->we_wordc); i++) {
        filename = ((&mots)->we_wordv)[i];
        if (0 == file_exists(filename)) {
            filenames[count++] = onsen_strdup(filename);
        }
    }
    filenames[count] = NULL;
    wordfree(&mots);
    free(tmpDir);
    return filenames;
}


int file_exists(char *filename) {
    struct stat st;
    if (-1 == stat(filename, &st)) {
        return 1;
    } else {
        return 0;
    }
}
