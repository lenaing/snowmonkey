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
#include "actions.h"

iconv_t pIconv = NULL;
extern char *OnsenEncodings[];
extern Context_t *context;

void
process_file(enum ActionMode mode)
{
    OnsenArchivePlugin_t *pInstance = NULL;
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

    /* Retrieve archive info */
    pInfo = onsen_new_archive_info();
    pInstance = context->pPlugins[0]->pInstance;

    if (0 == context->pInputFile->bIsMmaped) {
        lOffset = 0;
        pFile = (void *)(&(context->pInputFile->iFd));
    } else {
        lOffset = context->pInputFile->lFileSize;
        pFile = context->pInputFile->pData;
    }

    rc = pInstance->getArchiveInfo(context->pInputFile->bIsMmaped,
                                    lOffset,
                                    context->szInputFilename,
                                    pFile,
                                    pInfo);
    if (0 == rc) {
        if ((context->bVerbose) || (NULL == pInfo->a_pArchiveEntries[0])) {
            fprintf(stderr, "|   Failed to read archive info.\n");
        }
        onsen_free_archive_info(pInfo);
        return;
    }

    /* Iconv stuff */
    pEncoding = pInfo->eArchiveFilenamesEncoding;
    switch(pEncoding) {
        case SHIFT_JIS : {
            pIconv = onsen_iconv_init("UTF-8", OnsenEncodings[pEncoding]);
            break;
        };
        default : {
        };
    }

    if (mode == LIST) {
        /* Output results */
        print_info(pInfo);
        init_print_table(pInfo);
        print_table_header(pInfo);
    }

    for (i = 1; i <= pInfo->iArchiveEntriesCount; i++) {
        if (NULL == pInfo->a_pArchiveEntries) {
            /* No archive entry defined. Stop here.*/
            break;
        }

        pEntry = pInfo->a_pArchiveEntries[i];
        if (NULL == pEntry) {
            /* Invalid entry, check next. */
            continue;
        }

        switch(pEncoding) {
            case SHIFT_JIS : {
                szTmp = onsen_shift_jis2utf8(pIconv, pEntry->szFilename);
                szTmpFilename = szTmp;
                break;
            };
            default : {
                szTmpFilename = (char *)(pEntry->szFilename);
            };
        }

        if (0 != iQueriesCount) {
            for (j = 0; j < iQueriesCount; j++) {
                if (0 == strcmp(szTmpFilename, a_szQueriedFilenames[j])) {
                    if (mode == EXTRACT) {
                        extract_entry(pInstance, pEntry, szTmpFilename);
                    } else {
                        print_entry(pEntry, szTmpFilename);
                    }
                    break;
                }
            }
        } else {
            if (mode == EXTRACT) {
                extract_entry(pInstance, pEntry, szTmpFilename);
            } else {
                print_entry(pEntry, szTmpFilename);
            }
        }

        if (NULL != szTmp) {
            free(szTmp);
        }
    }

    if (mode == LIST) {
        free_print_table();
    }

    if (pIconv != NULL) {
        onsen_iconv_cleanup(pIconv);
    }
    onsen_free_archive_info(pInfo);
}
