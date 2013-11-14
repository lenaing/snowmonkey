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
#include "actions.h"

iconv_t pIconv = NULL;
extern char *OnsenEncodings[];
extern Context_t *context;

void
process_file(SnowmonkeyActionMode mode)
{
    int i, j;
    int rc = 0;
    int queriesCount = 0;
    long offset = 0;
    void *file;
    char *tmp = NULL;
    char *tmpFilename = NULL;
    char **queriedFilenames = NULL;
    enum OnsenEncoding encoding = ASCII;
    OnsenArchivePlugin_t *instance = NULL;
    OnsenArchiveInfo_t *info = NULL;
    OnsenArchiveEntry_t *entry = NULL;
    OnsenPlugin_t *plugin = NULL;

    queriesCount        = context->queriedFilenamesCount;
    queriedFilenames    = context->queriedFilenames;
    plugin              = context->plugins[context->selectedPlugin];

    /* Limits to Onsen Archive plugins */
    if (plugin->type == ONSEN_PLUGIN_ARCHIVE) {

        if (0 == context->inputFile->isMmaped) {
            file = (void *)(&(context->inputFile->fd));
            offset = 0;
        } else {
            file = context->inputFile->data;
            offset = context->inputFile->fileSize;
        }

        if (!plugin->isFileSupported(context->inputFile->isMmaped,
                                      context->inputFilename,
                                      file,
                                      offset)) {
            fprintf(stderr, "|   Ouch. Plugin %s ", plugin->name);
            fprintf(stderr, "cant handle this file. Sorry.\n");
            return;
        }
    }

    printf("|   Using plugin : %s\n", plugin->name);

    /* Retrieve archive info */
    info = onsen_new_archive_info();
    instance = plugin->instance;

    rc = instance->getArchiveInfo(context->inputFile->isMmaped,
                                   offset,
                                   context->inputFilename,
                                   file,
                                   info);
    if (0 == rc) {
        if (context->verbose) {
            fprintf(stderr, "|   Failed to read archive info.\n");
        }
        /* Info may have been freed by plugin */
        if (info != NULL) {
            onsen_free_archive_info(info);
        }
        return;
    }

    /* Iconv stuff */
    encoding = info->archiveFilenamesEncoding;
    switch(encoding) {
        case SHIFT_JIS : {
            pIconv = iconv_init("UTF-8", OnsenEncodings[encoding]);
            break;
        };
        default : {
        };
    }

    if (mode == SNOWMONKEY_LIST) {
        /* Output results */
        print_info(info);
        if (context->verbose) {
            init_print_table(info);
            print_table_header(info);
        }
    }

    for (i = 1; i <= info->archiveEntriesCount; i++) {
        if (NULL == info->archiveEntries) {
            /* No archive entry defined. Stop here.*/
            break;
        }

        entry = info->archiveEntries[i];
        if (NULL == entry) {
            /* Invalid entry, check next. */
            continue;
        }

        switch(encoding) {
            case SHIFT_JIS : {
                tmp = onsen_shift_jis2utf8(pIconv, entry->filename);
                tmpFilename = tmp;
                break;
            };
            default : {
                tmpFilename = (char *)(entry->filename);
            };
        }

        if (0 != queriesCount) {
            for (j = 0; j < queriesCount; j++) {
                if (0 == strcmp(tmpFilename, queriedFilenames[j])) {
                    if (mode == SNOWMONKEY_EXTRACT) {
                        extract_entry(instance, entry, tmpFilename);
                    } else {
                        print_entry(entry, tmpFilename, i);
                    }
                    break;
                }
            }
        } else {
            if (mode == SNOWMONKEY_EXTRACT) {
                extract_entry(instance, entry, tmpFilename);
            } else {
                print_entry(entry, tmpFilename, i);
            }
        }

        if (NULL != tmp) {
            free(tmp);
        }
    }

    if (mode == SNOWMONKEY_LIST) {
        if (context->verbose) {
            free_print_table();
        }
    }

    if (pIconv != NULL) {
        iconv_cleanup(pIconv);
    }
    onsen_free_archive_info(info);
}
