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

/* TODO : Move all the listing action to a separate file. */
int iSizeHeaderLen = 12;
int iCompSizeHeaderLen = 12;
int *a_iAddFieldsHeadersLen = NULL;
char szSizeHeaderFormat[100];
char szSizeValueFormat[100];
char szCompSizeHeaderFormat[100];
char szCompSizeValueFormat[100];

int
numlen(int i)
{
    char szTmp[MAX_COLUMN_SIZE];
    return sprintf(szTmp, "%d", i);
}

void repeat_print_char(int n, char c)
{
    int i;

    for (i = 0; i < n; i++) {
        printf("%c", c);
    }
}

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
init_print_table(OnsenArchiveInfo_t *pInfo)
{
    int i = 0;
    int j = 0;
    int iTmpLen = 0;
    int iAddlFdsCount = 0;

    if (NULL == pInfo->a_pArchiveEntries) {
        /* Invalid archive infos */
        return;
    }

    iAddlFdsCount = pInfo->a_pArchiveEntries[0]->iAddlFdsCount;
    if (0 != iAddlFdsCount) {
        a_iAddFieldsHeadersLen = calloc(iAddlFdsCount, sizeof(int));
    }

    for (i = 0; i < pInfo->iArchiveEntriesCount; i++) {
        iTmpLen = numlen(pInfo->a_pArchiveEntries[i]->iSize);
        if (iTmpLen > iSizeHeaderLen) {
            iSizeHeaderLen = iTmpLen;
        }

        iTmpLen = numlen(pInfo->a_pArchiveEntries[i]->iCompressedSize);
        if (iTmpLen > iCompSizeHeaderLen) {
            iCompSizeHeaderLen = iTmpLen;
        }

        for (j = 0; j < iAddlFdsCount; j++) {
            iTmpLen = strlen(pInfo->a_pArchiveEntries[i]->a_szAddlFds[j]);
            if (iTmpLen > a_iAddFieldsHeadersLen[j]) {
                a_iAddFieldsHeadersLen[j] = iTmpLen + 1;
            }
        }
    }

    sprintf(szSizeHeaderFormat, " %%%ds", iSizeHeaderLen);
    sprintf(szCompSizeHeaderFormat, " %%%ds", iCompSizeHeaderLen);
    sprintf(szSizeValueFormat, " %%%dd", iSizeHeaderLen);
    sprintf(szCompSizeValueFormat, " %%%dd", iCompSizeHeaderLen);
}

void
free_print_table()
{
    free(a_iAddFieldsHeadersLen);
}

void
print_table_header(OnsenArchiveInfo_t *pInfo)
{
    int i = 0;

    char szTmpFormat[MAX_FMT_STR_LEN];
    int iAddlFdsCount = 0;

    if (NULL == pInfo->a_pArchiveEntries) {
        /* Invalid archive infos */
        return;
    }

    if (context->bVerbose) {
        iAddlFdsCount = pInfo->a_pArchiveEntries[0]->iAddlFdsCount;

        /* Headers */
        printf("%12s", "Offset");
        printf(szSizeHeaderFormat, "Size");
        printf("%12s", "Compressed");
        printf(szCompSizeHeaderFormat, "Comp. Size");
        printf("%12s", "Encrypted");
        for (i = 0; i < iAddlFdsCount; i++) {
            sprintf(szTmpFormat, " %%%ds", a_iAddFieldsHeadersLen[i]);
            printf(szTmpFormat, pInfo->a_pArchiveEntries[0]->a_szAddlFds[i]);
        }
        printf("%s", "  Name");
        printf("\n");

        /* Headers separation */
        printf("%12s ", "-----------");
        repeat_print_char(iSizeHeaderLen, '-');
        printf("%12s ", "-----------");
        repeat_print_char(iCompSizeHeaderLen, '-');
        printf("%12s ", "-----------");
        for (i = 0; i < iAddlFdsCount; i++) {
            repeat_print_char(a_iAddFieldsHeadersLen[i], '-');
            printf(" ");
        }
        printf(" %s", "----------------");
        printf("\n");
    }
}

void
extract_entry(OnsenArchivePlugin_t *pInstance, OnsenArchiveEntry_t *pEntry,
                char *szFilename)
{
    char *szDestFilename = NULL;
    char *szOutputDir = NULL;

    if (NULL != context->szOutputDir) {
        szOutputDir = context->szOutputDir;
    } else {
        szOutputDir = ".";
    }

    szDestFilename = onsen_build_filename(szOutputDir, szFilename);

#if defined(__linux__) || defined(__GNU__)
    /* Correct filenames for an UNIX system. */
    onsen_str_chr_replace(szDestFilename, '\\', '/');
#endif

    /* Build directory tree. */
    onsen_mkdir(szOutputDir);

    if (context->bVerbose) {
        /* Make some space for progress indicator */
        printf("       ");
    }

    printf("%s", szFilename);

    /* Write file to disk. */
    pInstance->writeFile(1,
                            context->pInputFile,
                            pEntry->iOffset,
                            0,
                            szDestFilename,
                            pEntry->iCompressedSize,
                            (context->bVerbose) ? print_progress : NULL,
                            pEntry);

    printf("\n");

    onsen_free(szDestFilename);
}

void
print_entry(OnsenArchiveEntry_t *pEntry, char *szFilename)
{
    int i;
    char szTmpFormat[MAX_FMT_STR_LEN];

    if (context->bVerbose) {
        printf("%012X", pEntry->iOffset);
        printf(szSizeValueFormat, pEntry->iSize);
        printf("%12s", (pEntry->bCompressed) ? "yes" : "no");
        printf(szCompSizeValueFormat, pEntry->iCompressedSize);
        printf("%12s", (pEntry->bEncrypted) ? "yes" : "no");
        if (0 != pEntry->iAddlFdsCount) {
            for (i = 0; i < pEntry->iAddlFdsCount; i++) {
                if (NULL != pEntry->a_szAddlFds[i]) {
                    sprintf(szTmpFormat, " %%%ds", a_iAddFieldsHeadersLen[i]);
                    printf(szTmpFormat, pEntry->a_szAddlFds[i]);
                }
            }
        }
        printf("  ");
    }
    printf("%s\n", szFilename);
}

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
    char *szTmp = NULL;
    char *szTmpFilename = NULL;
    char **a_szQueriedFilenames = NULL;

    iQueriesCount = context->iQueriedFilenamesCount;
    a_szQueriedFilenames = context->a_szQueriedFilenames;

    /* Retrieve archive info */
    pInfo = onsen_new_archive_info();
    pInstance = context->pPlugins[0]->pInstance;
    rc = pInstance->getArchiveInfo(1,
                                    context->pInputFile,
                                    context->lInputFileSize,
                                    context->szInputFilename,
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
