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
get_data_dirs(char *szComponent)
{
    char *pSaved = NULL;
    char *szDataDirs = NULL;
    char *szDelimiters = ":";
    char *szToken = NULL;
    char *szTmpEnv = NULL;
    char *szTmpXdg = NULL;

    szDataDirs = calloc(MAX_STRING_LENGTH, sizeof(char));

    /* Parse XDG_DATA_HOME first. */
    szTmpXdg = getenv("XDG_DATA_HOME");
    if (NULL != szTmpXdg) {
        strcat(szDataDirs, szTmpXdg);

        if (! is_slashed(szTmpXdg)) {
            strcat(szDataDirs, "/");
        }

        strcat(szDataDirs, szComponent);
        strcat(szDataDirs, "/");
    }

    /* Parse XDG_DATA_DIRS now. */
    szTmpXdg = getenv("XDG_DATA_DIRS");
    if (NULL != szTmpXdg) {

        szTmpEnv = onsen_strdup(szTmpXdg);
        if (NULL != szTmpEnv) {

            szToken = strtok_r(szTmpEnv, szDelimiters, &pSaved);
            while (NULL != szToken) {
                strcat(szDataDirs, ":");
                strcat(szDataDirs, szToken);

                if (! is_slashed(szToken)) {
                    strcat(szDataDirs, "/");
                }
                strcat(szDataDirs, szComponent);
                strcat(szDataDirs, "/");

                szToken = strtok_r(NULL, szDelimiters, &pSaved);
            }

            onsen_free(szTmpEnv);
        }
    }

    return szDataDirs;
}

char **
find_data_files(char *szSearch, char *szDataDirs)
{
    int i;
    int iCount = 0;
    wordexp_t sWords;
    char *pSaved = NULL;
    char *szCurDir = NULL;
    char *szDelimiters = ":";
    char *szFilename = NULL;
    char *szTmpDir = NULL;
    char **a_szFilenames = NULL;

    a_szFilenames = calloc(MAX_SEARCH_RESULTS, sizeof(char *));
    szTmpDir = calloc(MAX_STRING_LENGTH, sizeof(char));

    szCurDir = strtok_r(szDataDirs, szDelimiters, &pSaved);
    strcat(szTmpDir, szCurDir);
    if (! is_slashed(szCurDir)) {
        strcat(szTmpDir, "/");
    }
    strcat(szTmpDir, szSearch);

    wordexp(szTmpDir, &sWords, WRDE_NOCMD);
    while (NULL != szCurDir) {
        szCurDir = strtok_r(NULL, szDelimiters, &pSaved);
        if (NULL != szCurDir) {
            szTmpDir[0] = '\0';
            strcat(szTmpDir, szCurDir);
            if (! is_slashed(szCurDir)) {
                strcat(szTmpDir, "/");
            }
            strcat(szTmpDir, szSearch);
            wordexp(szTmpDir, &sWords, WRDE_APPEND|WRDE_NOCMD);
        }
    }

    for (i = 0; i < (int)((&sWords)->we_wordc); i++) {
        szFilename = ((&sWords)->we_wordv)[i];
        if (0 == file_exists(szFilename)) {
            a_szFilenames[iCount++] = onsen_strdup(szFilename);
        }
    }
    a_szFilenames[iCount] = NULL;

    wordfree(&sWords);
    free(szTmpDir);
    return a_szFilenames;
}


int
file_exists(char *szFilename)
{
    struct stat sSt;
    return (-1 == stat(szFilename, &sSt));
}
