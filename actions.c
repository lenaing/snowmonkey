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

void
doList()
{
    int i, j;
    char *szTmpDestFilename;
    OnsenArchivePlugin_t *pInstance = NULL;
    OnsenArchiveInfo_t *pInfo = NULL;
    OnsenArchiveEntry_t *pEntry = NULL;

    /* Retrieve archive info */
    pInfo = onsen_new_archive_info();
    pInstance = context->pPlugins[0]->pInstance;
    pInstance->getArchiveInfo(1, context->pInputFile, context->lInputFileSize, pInfo);

    /* Print archive info */
    if (context->bVerbose) {
        printf("|   Archive filename = %s\n", context->szInputFilename);
        printf("|   Archive size = %ld bytes\n", pInfo->lArchiveFileSize);
        printf("|   Archive entries count = %d\n", pInfo->iArchiveEntriesCount);
        printf("|\n");
    }

    switch(pInfo->eArchiveFilenamesEncoding) {
        case SHIFT_JIS : {
            pIconv = onsen_iconv_init("UTF-8", OnsenEncodings[pInfo->eArchiveFilenamesEncoding]);
            break;
        };
        default : {
        };
    }

    if (NULL == pInfo->a_pArchiveEntries[0]) {
        printf("argh\n");
    }

    if (context->bVerbose) {
        printf("%12s", "Offset");
        printf("%12s", "Size");
        printf("%12s", "Compressed");
        printf("%12s", "Comp. Size");
        printf("%12s", "Encrypted");
        for (i = 0; i < pInfo->a_pArchiveEntries[0]->iAddlFdsCount; i++) {
            printf("%12s", pInfo->a_pArchiveEntries[0]->a_szAddlFds[i]);
        }
        printf("%s", "  Name");
        printf("\n");
        printf("%12s", "-----------");
        printf("%12s", "-----------");
        printf("%12s", "-----------");
        printf("%12s", "-----------");
        printf("%12s", "-----------");
        for (i = 0; i < pInfo->a_pArchiveEntries[0]->iAddlFdsCount; i++) {
            printf("%12s", "-----------");
        }
        printf("%s", "  ------------------");
        printf("\n");
    }


    /* For each file in archive, print info */
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

        /* TODO : Adapt to field size? */
        if (context->bVerbose) {
            printf(" %011X", pEntry->iOffset);
            printf("%12d", pEntry->iSize);
            printf("%12s", (pEntry->bCompressed) ? "yes" : "no");
            printf("%12d", pEntry->iCompressedSize);
            printf("%12s", (pEntry->bEncrypted) ? "yes" : "no");
            if (0 != pEntry->iAddlFdsCount) {
                for (j = 0; j < pEntry->iAddlFdsCount; j++) {
                    if (NULL != pEntry->a_szAddlFds[j]) {
                        printf("%12s", pEntry->a_szAddlFds[j]);
                    }
                }
            }
            printf("  ");
        }

        switch(pInfo->eArchiveFilenamesEncoding) {
            case SHIFT_JIS : {
                szTmpDestFilename = onsen_shift_jis2utf8(pIconv, pEntry->szFilename);
                printf("%s\n", szTmpDestFilename);
                free(szTmpDestFilename);
                break;
            };
            default : {
                printf("%s\n", pEntry->szFilename);
            };
        }


    }
    if (pIconv != NULL) {
        onsen_iconv_cleanup(pIconv);
    }
    onsen_free_archive_info(pInfo);
}
