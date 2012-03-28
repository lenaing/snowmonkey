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
print_info(OnsenArchiveInfo_t *pInfo)
{
    if (context->bVerbose) {
        printf("|   Archive filename = %s\n", context->szInputFilename);
        printf("|   Archive size = %ld bytes\n", pInfo->lArchiveFileSize);
        printf("|   Archive entries count = %d\n", pInfo->iArchiveEntriesCount);
        printf("|\n");
    }
}

void
print_header(OnsenArchiveInfo_t *pInfo)
{
    /* TODO : Adapt to field size? */
    int i = 0;

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
}

void
extract_entry(OnsenArchivePlugin_t *pInstance, OnsenArchiveEntry_t *pEntry, char *szFilename)
{
    char *szDestFilename = NULL;
    char *szOutputDir = NULL;

    /* Correct filename for an UNIX system. */
    /* TODO : add option to set output directory */
    szDestFilename = onsen_build_filename(".", szFilename);
    onsen_str_chr_replace(szDestFilename,'\\', '/');

    /* Build directory tree. */
    szOutputDir = onsen_basedir(szDestFilename);
    onsen_mkdir(szOutputDir);

    /* Write file to disk. */
    pInstance->writeFile(1,
                            context->pInputFile,
                            pEntry->iOffset,
                            0,
                            szDestFilename,
                            pEntry->iCompressedSize,
                            NULL, /* progress, */
                            pEntry);

    printf("%s\n", szFilename);

    /* Free path and filename */
    onsen_free(szOutputDir);
    onsen_free(szDestFilename);
}

void
print_entry(OnsenArchiveEntry_t *pEntry, char *szFilename)
{
    /* TODO : Adapt to field size? */
    int i;

    if (context->bVerbose) {
        printf(" %011X", pEntry->iOffset);
        printf("%12d", pEntry->iSize);
        printf("%12s", (pEntry->bCompressed) ? "yes" : "no");
        printf("%12d", pEntry->iCompressedSize);
        printf("%12s", (pEntry->bEncrypted) ? "yes" : "no");
        if (0 != pEntry->iAddlFdsCount) {
            for (i = 0; i < pEntry->iAddlFdsCount; i++) {
                if (NULL != pEntry->a_szAddlFds[i]) {
                    printf("%12s", pEntry->a_szAddlFds[i]);
                }
            }
        }
        printf("  ");
    }
    printf("%s\n", szFilename);
}

void
process(int bExtracting)
{
    OnsenArchivePlugin_t *pInstance = NULL;
    OnsenArchiveInfo_t *pInfo = NULL;
    OnsenArchiveEntry_t *pEntry = NULL;
    enum OnsenEncoding pEncoding = ASCII;
    int i, j;
    int rc = 0;
    int iQueriesCount = 0;
    char *szTmp;
    char *szTmpFilename;
    char **a_szQueriedFilenames = NULL;

    iQueriesCount = context->iQueriedFilenamesCount;
    a_szQueriedFilenames = context->a_szQueriedFilenames;

    /* Retrieve archive info */
    pInfo = onsen_new_archive_info();
    pInstance = context->pPlugins[0]->pInstance;
    rc = pInstance->getArchiveInfo(1,
                                    context->pInputFile,
                                    context->lInputFileSize,
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

    if (! bExtracting) {
        /* Output results */
        print_info(pInfo);
        print_header(pInfo);
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
                    if (bExtracting) {
                        extract_entry(pInstance, pEntry, szTmpFilename);
                    } else {
                        print_entry(pEntry, szTmpFilename);
                    }
                    break;
                }
            }
        } else {
            if (bExtracting) {
                extract_entry(pInstance, pEntry, szTmpFilename);
            } else {
                print_entry(pEntry, szTmpFilename);
            }
        }

        if (NULL != szTmp) {
            free(szTmp);
        }
    }

    if (pIconv != NULL) {
        onsen_iconv_cleanup(pIconv);
    }
    onsen_free_archive_info(pInfo);
}
