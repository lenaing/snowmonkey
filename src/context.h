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
#ifndef __SNOWMONKEY_CONTEXT_H
#define __SNOWMONKEY_CONTEXT_H

#include "globals.h"
#include <libonsen/plugin.h>
#include <libonsen/file_utils.h>

/* Available action modes. */
enum _SnowmonkeyActionMode_e {
    SNOWMONKEY_LIST,
    SNOWMONKEY_EXTRACT
};
typedef enum _SnowmonkeyActionMode_e SnowmonkeyActionMode;

typedef struct _Context_s Context_t;
struct _Context_s
{
    /* Archive files. */
    char *inputFilename;                    /* Input filename.                */
    OnsenFile_t *inputFile;                 /* Input file metadata.           */

    char *outputDir;                        /* Output directory.              */

    /* Queried files. */
    char **queriedFilenames;                /* Queried files.                 */
    int queriedFilenamesCount;              /* Count of queried files.        */

    /* Plugins. */
    OnsenPlugin_t **plugins;                /* List of loaded plugins.        */
    int pluginsCount;                       /* Count of loaded plugins.       */
    int selectedPlugin;                     /* Selected plugin.               */
    int defaultPlugin;                      /* Default working archive plugin.*/
    char *pluginsDirs;                      /* List of dirs where to look for
                                             * plugins, separated by colons.  */
    char *pluginsFilenames;                 /* List of plugins filenames to
                                             * load, without extension and
                                             * separated by commas.           */

    /* Actions options. */
    SnowmonkeyActionMode action;            /* Selected main action mode.     */

    /* Help options. */
    int verbose;                            /* Verbose argument detected.     */
};

Context_t *new_context(void);
void       free_context(Context_t *);

#endif /* __SNOWMONKEY_CONTEXT_H */
