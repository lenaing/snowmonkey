/*
 * Copyright 2011-2013 - Etienne 'lenaing' GIRONDEL <lenaing@gmail.com>
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
#include "plugins.h"

void
load_plugins_from_filenames(char **filenames)
{
    OnsenPlugin_t **plugins = NULL;
    OnsenPlugin_t *plugin = NULL;
    int i = 0;
    int loaded = 0;
    int rc;

    plugins = calloc(SNOWMONKEY_MAX_PLUGINS, sizeof(OnsenPlugin_t *));

    while (NULL != filenames[i]) {
        plugin = onsen_new_plugin();
        rc = onsen_load_plugin(plugin, filenames[i]);
        if (rc != 0) {
            onsen_free_plugin(plugin);
        } else {
            plugins[loaded] = plugin;
            loaded++;
        }

        i++;
    }

    i = 0;
    while(NULL != filenames[i]) {
        free(filenames[i]);
        i++;
    }
    free(filenames);

    /* Fill in context. */
    context->plugins = plugins;
    context->pluginsCount = loaded;
}

void
load_plugins(char *pluginsNames, char *pluginsDirs)
{
    int i;
    int found = 0;                  /* Matching files found.                  */
    char *rec = NULL;               /* Record pointer for tokenization.       */

    char **filenames = NULL;        /* Matching filenames found               */
    char *searchCriteria = NULL;    /* Search criteria                        */
    char *dataDirs = NULL;          /* Data directories in which to look for
                                     * files                                  */

    char **tmfilenames = NULL;      /* Temporary array of filenames.          */
    char *tmfilename = NULL;        /* Temporary filename.                    */
    char *curFilename = NULL;       /* Current filename.                      */

    if (NULL == pluginsDirs) {
        /* No plugins directories specified, look in every available 
         * directories. */
        dataDirs = get_data_dirs("libonsen");
    } else {
        /* Look in given directories. */
        dataDirs = pluginsDirs;
    }

    if (NULL == pluginsNames) {
        /* Search for all available plugins in directories */

        if (NULL == pluginsDirs) {
            /* Search is "plugins/ *.so" for common directories */
            searchCriteria = calloc(strlen(SNOWMONKEY_PLUGINS_BASE_DIR) + 3 +
                                    strlen(SNOWMONKEY_PLUGINS_EXTENSION) + 1,
                                    sizeof(char));
            strcat(searchCriteria, SNOWMONKEY_PLUGINS_BASE_DIR);
            strcat(searchCriteria, "*/*");
            strcat(searchCriteria, SNOWMONKEY_PLUGINS_EXTENSION);
        } else {
            /* Search is "*.so" for all others directories */
            searchCriteria = calloc(1 + strlen(SNOWMONKEY_PLUGINS_EXTENSION)
                                        + 1,
                                      sizeof(char));
            strcat(searchCriteria, "*");
            strcat(searchCriteria, SNOWMONKEY_PLUGINS_EXTENSION);
        }

        filenames = find_data_files(searchCriteria, dataDirs);
        free(searchCriteria);
    } else {
        /* Search for plugins with given names in directories */
        filenames = calloc(SNOWMONKEY_MAX_SEARCH_RESULTS, sizeof(char *));
        curFilename = strtok_r(pluginsNames,
                                    SNOWMONKEY_PLUGINS_NAMES_DELIMITER,
                                    &rec);

        while (NULL != curFilename) {
            tmfilename = calloc(SNOWMONKEY_MAX_STRING_LENGTH, sizeof(char));
            if (NULL == pluginsDirs) {
                strcat(tmfilename, SNOWMONKEY_PLUGINS_BASE_DIR);
            }
            strcat(tmfilename, curFilename);
            strcat(tmfilename, SNOWMONKEY_PLUGINS_EXTENSION);

            tmfilenames = find_data_files(tmfilename, dataDirs);

            i = 0;
            while(NULL != tmfilenames[i]) {
                filenames[found++] = onsen_strdup(tmfilenames[i]);
                free(tmfilenames[i]);
                i++;
            }

            free(tmfilenames);
            free(tmfilename);

            curFilename = strtok_r(NULL,
                                        SNOWMONKEY_PLUGINS_NAMES_DELIMITER,
                                        &rec);
        }
    }

    if (NULL == pluginsDirs) {
        free(dataDirs);
    }

    /* Try to load found files as plugins */
    load_plugins_from_filenames(filenames);
}

void
unload_plugins()
{
    OnsenPlugin_t *plugin;
    int i;

    for (i = 0; i < context->pluginsCount; i++) {
        plugin = context->plugins[i];
        onsen_free_plugin(plugin);
    }

    free(context->plugins);
    context->pluginsCount = 0;
}

int
check_fileformat_supported_by_plugins()
{
    OnsenPlugin_t *plugin;
    int i;
    long offset;
    void *file;
    int res = 0;

    for (i = 0; i < context->pluginsCount; i++) {
        plugin = context->plugins[i];

        /* Limits to Onsen Archive plugins */
        if (plugin->type == ONSEN_PLUGIN_ARCHIVE) {

            if (0 == context->inputFile->isMmaped) {
                file = (void *)(&(context->inputFile->fd));
                offset = 0;
            } else {
                file = context->inputFile->data;
                offset = context->inputFile->fileSize;
            }

            if (plugin->isFileSupported(context->inputFile->isMmaped,
                                            context->inputFilename,
                                            file,
                                            offset) > 0) {
                if (context->defaultPlugin == -1) {
                    context->defaultPlugin = i;
                }
                res++;
            }
        }
    }

    return res;
}

void
print_available_plugins()
{
    OnsenPlugin_t *plugin;
    int i;
    long offset;
    void *file;

    for (i = 0; i < context->pluginsCount; i++) {
        plugin = context->plugins[i];

        /* Limits to Onsen Archive plugins */
        if (plugin->type == ONSEN_PLUGIN_ARCHIVE) {

            if (0 == context->inputFile->isMmaped) {
                file = (void *)(&(context->inputFile->fd));
                offset = 0;
            } else {
                file = context->inputFile->data;
                offset = context->inputFile->fileSize;
            }

            if (plugin->isFileSupported(context->inputFile->isMmaped,
                                            context->inputFilename,
                                            file,
                                            offset)) {
                printf("[%d] %s\n", i, plugin->name);
            }
        }
    }
    if (context->selectedPlugin != -1) {
        printf("|   Selected plugin [%d] %s\n", context->selectedPlugin,
                             context->plugins[context->selectedPlugin]->name);
    } else {
        printf("|   Please select one of the above plugin through the ");
        printf("-n ");
#ifdef HAVE_GETOPT_LONG
        printf("or the --plugin ");
#endif
        printf("option to process file.\n");
    }
}
