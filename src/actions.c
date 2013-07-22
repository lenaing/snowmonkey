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
    OnsenArchivePlugin_t *instance = NULL;
    OnsenArchiveInfo_t *pInfo = NULL;
    OnsenArchiveEntry_t *pEntry = NULL;
    enum OnsenEncoding pEncoding = ASCII;
    int i, j;
    int rc = 0;
    int iQueriesCount = 0;
    long lOffset = 0;
    void *pFile;
    char *szTmp = NULL;
    char *szTmpFilename = NULL;
    char **a_szQueriedFilenames = NULL;

    iQueriesCount = context->iQueriedFilenamesCount;
    a_szQueriedFilenames = context->a_szQueriedFilenames;

    
    /* Limits to Onsen Archive plugins */
    if (context->pPlugins[context->iSelectedPlugin]->type == ONSEN_PLUGIN_ARCHIVE) {

        if (0 == context->pInputFile->isMmaped) {
            lOffset = 0;
            pFile = (void *)(&(context->pInputFile->fd));
        } else {
            lOffset = context->pInputFile->fileSize;
            pFile = context->pInputFile->data;
        }

        if (!context->pPlugins[context->iSelectedPlugin]->isFileSupported(context->pInputFile->isMmaped,
                                        context->szInputFilename,
                                        pFile,
                                        lOffset)) {
            return;
        }
    }

    fprintf(stdout, "|   Using plugin : %s\n", context->pPlugins[context->iSelectedPlugin]->name);

    /* Retrieve archive info */
    pInfo = onsen_new_archive_info();
    instance = context->pPlugins[context->iSelectedPlugin]->instance;

    rc = instance->getArchiveInfo(context->pInputFile->isMmaped,
                                    lOffset,
                                    context->szInputFilename,
                                    pFile,
                                    pInfo);
    if (0 == rc) {
        if ((context->bVerbose)/* || (NULL == pInfo->archiveEntries[0])*/) {
            fprintf(stderr, "|   Failed to read archive info.\n");
        }
        /* Info may have been freed by plugin */
        if (pInfo != NULL) {
            onsen_free_archive_info(pInfo);
        }
        return;
    }

    /* Iconv stuff */
    pEncoding = pInfo->archiveFilenamesEncoding;
    switch(pEncoding) {
        case SHIFT_JIS : {
            pIconv = iconv_init("UTF-8", OnsenEncodings[pEncoding]);
            break;
        };
        default : {
        };
    }

    if (mode == SNOWMONKEY_LIST) {
        /* Output results */
        print_info(pInfo);
        if (context->bVerbose) {
            init_print_table(pInfo);
            print_table_header(pInfo);
        }
    }

    for (i = 1; i <= pInfo->archiveEntriesCount; i++) {
        if (NULL == pInfo->archiveEntries) {
            /* No archive entry defined. Stop here.*/
            break;
        }

        pEntry = pInfo->archiveEntries[i];
        if (NULL == pEntry) {
            /* Invalid entry, check next. */
            continue;
        }

        switch(pEncoding) {
            case SHIFT_JIS : {
                szTmp = onsen_shift_jis2utf8(pIconv, pEntry->filename);
                szTmpFilename = szTmp;
                break;
            };
            default : {
                szTmpFilename = (char *)(pEntry->filename);
            };
        }

        if (0 != iQueriesCount) {
            for (j = 0; j < iQueriesCount; j++) {
                if (0 == strcmp(szTmpFilename, a_szQueriedFilenames[j])) {
                    if (mode == SNOWMONKEY_EXTRACT) {
                        extract_entry(instance, pEntry, szTmpFilename);
                    } else {
                        print_entry(pEntry, szTmpFilename, i);
                    }
                    break;
                }
            }
        } else {
            if (mode == SNOWMONKEY_EXTRACT) {
                extract_entry(instance, pEntry, szTmpFilename);
            } else {
                print_entry(pEntry, szTmpFilename, i);
            }
        }

        if (NULL != szTmp) {
            free(szTmp);
        }
    }

    if (mode == SNOWMONKEY_LIST) {
        if (context->bVerbose) {
            free_print_table();
        }
    }

    if (pIconv != NULL) {
        iconv_cleanup(pIconv);
    }
    onsen_free_archive_info(pInfo);
}
