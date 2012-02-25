#include "plugins.h"

void
load_plugins()
{
    int i = 0;
    int iLoaded = 0;
    int rc;
    char **a_szFilenames;
    OnsenPlugin_t **a_pPlugins = NULL;
    OnsenPlugin_t *pPlugin = NULL;

    a_pPlugins = calloc(MAX_PLUGINS, sizeof(OnsenPlugin_t *));

    a_szFilenames = find_data_files("plugins/*.so");

    /* Try to load files as plugins. */
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

    /* Free filenames list. */
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
        if (pPlugin->isFileSupported(context->szArchiveFilename, 1, context->pArchiveFile)) {
            return 0;
        }
    }
    return 1;
}
