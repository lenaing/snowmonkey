/*
 * Copyright 2011, 2012 - Etienne 'lenaing' GIRONDEL <lenaing@gmail.com>
 * 
 * snowmonkey :
 * ------------
 * This software is a libonsen CLI interface.
 * 
 * This software is governed by the CeCILL license under French law and
 * abiding by the rules of distribution of free software.  You can  use, 
 * modify and/ or redistribute the software under the terms of the CeCILL
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * "http://www.cecill.info". 
 * 
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability. 
 * 
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or 
 * data to be ensured and,  more generally, to use and operate it in the 
 * same conditions as regards security. 
 * 
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL license and that you accept its terms.
 */
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
