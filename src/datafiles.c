/*
 * Copyright 2011-2014 - Etienne 'lenaing' GIRONDEL <lenaing@gmail.com>
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
get_data_dirs(char *component)
{
    char *saved = NULL;
    char *dataDirs = NULL;
    char *delimiters = ":";
    char *token = NULL;
    char *tmpEnv = NULL;
    char *tmpXDG = NULL;

    dataDirs = calloc(SNOWMONKEY_MAX_STRING_LENGTH, sizeof(char));

    /* Parse XDG_DATA_HOME first. */
    tmpXDG = getenv("XDG_DATA_HOME");
    if (NULL != tmpXDG) {
        strcat(dataDirs, tmpXDG);

        if (! onsen_str_is_slashed(tmpXDG)) {
            strcat(dataDirs, "/");
        }

        strcat(dataDirs, component);
        strcat(dataDirs, "/");
    }

    /* Parse XDG_DATA_DIRS now. */
    tmpXDG = getenv("XDG_DATA_DIRS");
    if (NULL != tmpXDG) {

        tmpEnv = onsen_strdup(tmpXDG);
        if (NULL != tmpEnv) {

            token = strtok_r(tmpEnv, delimiters, &saved);
            while (NULL != token) {
                strcat(dataDirs, ":");
                strcat(dataDirs, token);

                if (! onsen_str_is_slashed(token)) {
                    strcat(dataDirs, "/");
                }
                strcat(dataDirs, component);
                strcat(dataDirs, "/");

                token = strtok_r(NULL, delimiters, &saved);
            }

            onsen_free(tmpEnv);
        }
    }

    return dataDirs;
}

char **
find_data_files(char *szSearch, char *dataDirs)
{
    int i;
    int count = 0;
    wordexp_t words;
    char *saved = NULL;
    char *curDir = NULL;
    char *delimiters = ":";
    char *filename = NULL;
    char *tmpDir = NULL;
    char **filenames = NULL;

    filenames = calloc(SNOWMONKEY_MAX_SEARCH_RESULTS, sizeof(char *));
    tmpDir = calloc(SNOWMONKEY_MAX_STRING_LENGTH, sizeof(char));

    curDir = strtok_r(dataDirs, delimiters, &saved);
    strcat(tmpDir, curDir);
    if (! onsen_str_is_slashed(curDir)) {
        strcat(tmpDir, "/");
    }
    strcat(tmpDir, szSearch);

    wordexp(tmpDir, &words, WRDE_NOCMD);
    while (NULL != curDir) {
        curDir = strtok_r(NULL, delimiters, &saved);
        if (NULL != curDir) {
            tmpDir[0] = '\0';
            strcat(tmpDir, curDir);
            if (! onsen_str_is_slashed(curDir)) {
                strcat(tmpDir, "/");
            }
            strcat(tmpDir, szSearch);
            wordexp(tmpDir, &words, WRDE_APPEND | WRDE_NOCMD);
        }
    }

    for (i = 0; i < (int)((&words)->we_wordc); i++) {
        filename = ((&words)->we_wordv)[i];
        if (0 == file_exists(filename)) {
            filenames[count++] = onsen_strdup(filename);
        }
    }
    filenames[count] = NULL;

    wordfree(&words);
    free(tmpDir);
    return filenames;
}


int
file_exists(char *filename)
{
    struct stat s;
    return (-1 == stat(filename, &s));
}
