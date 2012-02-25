#include "datafiles.h"

char *
get_data_dirs() {
    char *xdgTmp;
    char *dataDirs;
    
    char *token;
    char *delimiters = ":";
    

    dataDirs = calloc(4096, sizeof(char));

    /* Parse XDG_DATA_HOME first. */
    xdgTmp = getenv("XDG_DATA_HOME");
    strcat(dataDirs, xdgTmp);

    if (! is_slashed(xdgTmp)) {
        strcat(dataDirs, "/");
    }
    strcat(dataDirs, "snowmonkey/");

    /* Parse XDG_DATA_DIRS now. */
    xdgTmp = getenv("XDG_DATA_DIRS");
    token = strtok(xdgTmp, delimiters);
    while (NULL != token) {
        strcat(dataDirs, ":");
        strcat(dataDirs, token);

        if (! is_slashed(token)) {
            strcat(dataDirs, "/");
        }
        strcat(dataDirs, "snowmonkey/");

        token = strtok(NULL, delimiters);
    }

    return dataDirs;
}

char **
find_data_files(char *search) {
    int i;
    int count = 0;
    wordexp_t mots;
    char *dataDirs;
    char *tmpDir;
    char *curDir;
    char *delimiters = ":";
    struct stat st;
    char **filenames;
    char *filename;

    filenames = calloc(255, sizeof(char *));
    tmpDir = calloc(4096, sizeof(char));
    dataDirs = get_data_dirs();

    curDir = strtok(dataDirs, delimiters);
    strcat(tmpDir, curDir);
    strcat(tmpDir, search);
    wordexp(tmpDir, &mots, WRDE_NOCMD);
    while (NULL != curDir) {
        curDir = strtok(NULL, delimiters);
        if (NULL != curDir) {
            tmpDir[0] = '\0';
            strcat(tmpDir, curDir);
            strcat(tmpDir, search);
            wordexp(tmpDir, &mots, WRDE_APPEND|WRDE_NOCMD);
        }
    }


    for (i = 0; i < (int)((&mots)->we_wordc); i++) {
        filename = ((&mots)->we_wordv)[i];
        if (-1 == stat(filename, &st)) {
        } else {
            filenames[count] = onsen_strdup(filename);
            count++;
        }
    }
    filenames[count] = NULL;
    wordfree(&mots);
    free(dataDirs);
    free(tmpDir);
    return filenames;
}
