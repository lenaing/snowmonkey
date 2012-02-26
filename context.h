#ifndef __CONTEXT_H
#define __CONTEXT_H

#include "globals.h"
#include <libonsen/plugin.h>

typedef struct _Context_s Context_t;

struct _Context_s
{
    /* Archive files. */
    char *szInputFilename;                  /* Input filename.                */
    unsigned char *pInputFile;              /* Current input file.            */
    long lInputFileSize;                    /* Current input file size.       */
    int fdInputFile;                        /* Current input file descriptor. */

    /* Plugins. */
    OnsenPlugin_t **pPlugins;               /* List of loaded plugins.        */
    int iPluginsCount;                      /* Count of loaded plugins.       */
    char *szPluginsFilenames;               /* List of plugins filenames to
                                             * load, without extension and
                                             * separated by commas.           */
    char *szPluginsDirs;                    /* List of dirs where to look for
                                             * plugins, separated by colons. */

    /* Actions options. */
    int bList;                              /* List archive content.          */
    int bExtract;                           /* Extract content from archive.  */

    /* Help options. */
    int bVerbose;                           /* Verbose argument detected.     */
};

Context_t *new_context(void);
void free_context(Context_t *);

#endif /* __CONTEXT_H */
