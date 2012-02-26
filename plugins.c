#include "plugins.h"

void
load_plugins_from_filenames(char **a_szFilenames)
{
    int i = 0;
    int iLoaded = 0;
    int rc;

    OnsenPlugin_t **a_pPlugins = NULL;
    OnsenPlugin_t *pPlugin = NULL;

    a_pPlugins = calloc(MAX_PLUGINS, sizeof(OnsenPlugin_t *));

    while (NULL != a_szFilenames[i]) {
        pPlugin = onsen_new_plugin();
        rc = onsen_load_plugin(pPlugin, a_szFilenames[i]);
        if (rc != 0) {
            onsen_free_plugin(pPlugin);
        } else {
            a_pPlugins[iLoaded] = pPlugin;
            iLoaded++;
        }

        i++;
    }

    i = 0;
    while(NULL != a_szFilenames[i]) {
        free(a_szFilenames[i]);
        i++;
    }
    free(a_szFilenames);

    /* Fill in context. */
    context->pPlugins = a_pPlugins;
    context->iPluginsCount = iLoaded;
}

void
load_plugins(char *szPluginsNames, char *szPluginsDirs)
{
    int i;
    int iFound = 0;                 /* Matching files found.                  */
    char *rec = NULL;               /* Record pointer for tokenization.       */

    char **a_szFilenames = NULL;    /* Matching filenames found               */
    char *szSearchCriteria = NULL;  /* Search criteria                        */
    char *szDataDirs = NULL;        /* Data directories in which to look for
                                     * files                                  */

    char **a_szTmpFilenames = NULL; /* Temporary array of filenames.          */
    char *szTmpFilename = NULL;     /* Temporary filename.                    */
    char *szCurFilename = NULL;     /* Current filename.                      */

    if (NULL == szPluginsDirs) {
        /* No plugins directories specified, look in every available 
         * directories. */
        szDataDirs = get_data_dirs();
    } else {
        /* Look in given directories. */
        szDataDirs = szPluginsDirs;
    }

    if (NULL == szPluginsNames) {
        /* Search for all available plugins in directories */

        if (NULL == szPluginsDirs) {
            /* Search is "plugins/ *.so" for common directories */
            szSearchCriteria = calloc(strlen(PLUGINS_BASE_DIR) + 1 +
                                      strlen(PLUGINS_EXTENSION) + 1,
                                      sizeof(char));
            strcat(szSearchCriteria, PLUGINS_BASE_DIR);
            strcat(szSearchCriteria, "*");
            strcat(szSearchCriteria, PLUGINS_EXTENSION);
        } else {
            /* Search is "*.so" for all others directories */
            szSearchCriteria = calloc(1 + strlen(PLUGINS_EXTENSION) + 1,
                                      sizeof(char));
            strcat(szSearchCriteria, "*");
            strcat(szSearchCriteria, PLUGINS_EXTENSION);
        }

        a_szFilenames = find_data_files(szSearchCriteria, szDataDirs);
        free(szSearchCriteria);
    } else {
        /* Search for plugins with given names in directories */
        a_szFilenames = calloc(MAX_SEARCH_RESULTS, sizeof(char *));
        szCurFilename = strtok_r(szPluginsNames, PLUGINS_NAMES_DELIMITER, &rec);

        while (NULL != szCurFilename) {
            szTmpFilename = calloc(MAX_STRING_LENGTH, sizeof(char));
            if (NULL == szPluginsDirs) {
                strcat(szTmpFilename, PLUGINS_BASE_DIR);
            }
            strcat(szTmpFilename, szCurFilename);
            strcat(szTmpFilename, PLUGINS_EXTENSION);

            a_szTmpFilenames = find_data_files(szTmpFilename, szDataDirs);

            i = 0;
            while(NULL != a_szTmpFilenames[i]) {
                a_szFilenames[iFound++] = onsen_strdup(a_szTmpFilenames[i]);
                free(a_szTmpFilenames[i]);
                i++;
            }

            free(a_szTmpFilenames);
            free(szTmpFilename);

            szCurFilename = strtok_r(NULL, PLUGINS_NAMES_DELIMITER, &rec);
        }
    }

    if (NULL == szPluginsDirs) {
        free(szDataDirs);
    }

    /* Try to load found files as plugins */
    load_plugins_from_filenames(a_szFilenames);
}

void
unload_plugins()
{
    int i = 0;
    OnsenPlugin_t *pPlugin;

    for (i = 0; i < context->iPluginsCount; i++) {
        pPlugin = context->pPlugins[i];
        onsen_free_plugin(pPlugin);
    }

    free(context->pPlugins);
    context->iPluginsCount = 0;
}

int
check_fileformat_supported_by_plugins()
{
    int i;
    OnsenPlugin_t *pPlugin;
    for (i = 0; i < context->iPluginsCount; i++) {
        pPlugin = context->pPlugins[i];
        if (pPlugin->isFileSupported(context->szInputFilename,
                                     1,
                                     context->pInputFile)) {
            return 0;
        }
    }
    return 1;
}
