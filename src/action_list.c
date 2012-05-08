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
#include "action_list.h"

extern Context_t *context;

int iSizeHeaderLen = 12;
int iCompSizeHeaderLen = 12;
int iFiletypeFormat = 12;
int iMediatypeFormat = 12;
int *a_iAddFieldsHeadersLen = NULL;
char szSizeHeaderFormat[100];
char szSizeValueFormat[100];
char szCompSizeHeaderFormat[100];
char szCompSizeValueFormat[100];
char szFiletypeFormat[100];
char szMediatypeFormat[100];

int
numlen(int i)
{
    char szTmp[MAX_COLUMN_SIZE];
    return sprintf(szTmp, "%d", i);
}

void
repeat_print_char(int n, char c)
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
    int iAddlFdsCount = 0;
    int iTmpLen = 0;
    int iType = 0;
    int bUpdatedMediaTypeColumnSize = 0;
    long lOffset = 0;
    char *szFilename;
    char *szTmp;
    void *pFile;
    OnsenPlugin_t *pPlugin;




    if (NULL == pInfo->a_pArchiveEntries) {
        /* Invalid archive infos */
        return;
    }

    iAddlFdsCount = pInfo->a_pArchiveEntries[0]->iAddlFdsCount;
    if (0 != iAddlFdsCount) {
        a_iAddFieldsHeadersLen = calloc(iAddlFdsCount, sizeof(int));
    }

    for (i = 1; i <= pInfo->iArchiveEntriesCount; i++) {
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

        for (j = 0; j < context->iPluginsCount; j++) {
            pPlugin = context->pPlugins[j];

            if (0 == context->pInputFile->bIsMmaped) {
                pFile = (void *)(&(context->pInputFile->iFd));
                lOffset = pInfo->a_pArchiveEntries[i]->iOffset;
            } else {
                pFile = context->pInputFile->pData;
                lOffset = pInfo->a_pArchiveEntries[i]->iSize;
            }

            szFilename = pInfo->a_pArchiveEntries[i]->szFilename;

            iType = pPlugin->isFileSupported(context->pInputFile->bIsMmaped,
                                                szFilename,
                                                pFile,
                                                lOffset);

            if (iType > 0) {
                szTmp = calloc(1, MAX_COLUMN_SIZE);
                pPlugin->getPluginInfo(iType + 1, szTmp, MAX_COLUMN_SIZE);
                iTmpLen = strlen(szTmp);
                if (iTmpLen > iFiletypeFormat) {
                    iFiletypeFormat = iTmpLen;
                }

                pPlugin->getPluginInfo(iType + 2, szTmp, MAX_COLUMN_SIZE);
                iTmpLen = strlen(szTmp);
                if (iTmpLen > iMediatypeFormat) {
                    iMediatypeFormat = iTmpLen;
                }
                free(szTmp);
                bUpdatedMediaTypeColumnSize = 1;
            }
            /* TODO : Fallback to Mediatype */
        }
    }

    if (0 == bUpdatedMediaTypeColumnSize) {
        iMediatypeFormat = strlen(DEFAULT_MEDIA_TYPE);
    }

    sprintf(szSizeHeaderFormat, " %%%ds", iSizeHeaderLen);
    sprintf(szCompSizeHeaderFormat, " %%%ds", iCompSizeHeaderLen);
    sprintf(szSizeValueFormat, " %%%dd", iSizeHeaderLen);
    sprintf(szCompSizeValueFormat, " %%%dd", iCompSizeHeaderLen);
    sprintf(szFiletypeFormat, " %%%ds", iFiletypeFormat);
    sprintf(szMediatypeFormat, " %%%ds", iMediatypeFormat);
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
        printf(szFiletypeFormat, "File type");
        printf(szMediatypeFormat, "Media type");
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
        }
        repeat_print_char(iFiletypeFormat, '-');
        printf(" ");
        repeat_print_char(iMediatypeFormat, '-');
        printf(" ");
        printf(" %s", "----------------");
        printf("\n");
    }
}

void
print_entry(OnsenArchiveEntry_t *pEntry, char *szFilename)
{
    int i;
    int iType = 0;
    long lEntryOffset;
    char *szEntryFilename;
    char *szFiletype;
    char *szMediatype;
    char *szTmp;
    char *szTmp2;
    void *pEntryFile;
    OnsenPlugin_t *pPlugin;
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

        szEntryFilename = pEntry->szFilename;

        if (0 == context->pInputFile->bIsMmaped) {
            pEntryFile = (void *)(&(context->pInputFile->iFd));
            lEntryOffset = pEntry->iOffset;
        } else {
            pEntryFile = context->pInputFile->pData;
            lEntryOffset = pEntry->iSize;
        }

        for (i = 0; i < context->iPluginsCount; i++) {
            pPlugin = context->pPlugins[i];

            iType = pPlugin->isFileSupported(context->pInputFile->bIsMmaped,
                                                szEntryFilename,
                                                pEntryFile,
                                                lEntryOffset);
            if (iType > 0) {
                szTmp = calloc(1, MAX_COLUMN_SIZE);
                pPlugin->getPluginInfo(iType + 1, szTmp, MAX_COLUMN_SIZE);
                szFiletype = szTmp;

                szTmp2 = calloc(1, MAX_COLUMN_SIZE);
                pPlugin->getPluginInfo(iType + 2, szTmp2, MAX_COLUMN_SIZE);
                szMediatype = szTmp2;
                break;
            } 
        }

        if (context->iPluginsCount == i) {
            szFiletype = onsen_strdup(DEFAULT_FILE_TYPE);
            szMediatype = onsen_strdup(DEFAULT_MEDIA_TYPE);
            /* TODO : Fallback to Mediatype */
        }

        printf(szFiletypeFormat, szFiletype);
        printf(szMediatypeFormat, szMediatype);

        printf("  ");
    }
    printf("%s\n", szFilename);
    if (NULL != szFiletype) {
        free(szFiletype);
    }
    if (NULL != szMediatype) {
        free(szMediatype);
    }
}
