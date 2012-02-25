#ifndef __CONTEXT_H
#define __CONTEXT_H

#include "globals.h"
#include <libonsen/plugin.h>

typedef struct _Context_s Context_t;

struct _Context_s
{
    /* Archive files. */
    char *szArchiveFilename;                /* Archive filename.              */
    unsigned char *pArchiveFile;            /* Current archive file.          */
    long lArchiveFileSize;
    int fdArchiveFile;

    /* Plugins. */
    OnsenPlugin_t **pPlugins;               /* List of loaded plugins.        */
    int iPluginsCount;                      /* Count of loaded plugins.       */

    /* Actions options. */
    int bList;                              /* List archive content.          */
    int bExtract;                           /* Extract content from archive.  */

    /* Help options. */
    int bVerbose;                           /* Verbose argument detected.     */
};

Context_t *new_context(void);
void free_context(Context_t *);

#endif /* __CONTEXT_H */
