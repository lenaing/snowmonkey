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
extern char *OnsenEncodings[];

int sizeHeaderLen = 12;
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

char **filetypes;
char **mediatypes;

extern iconv_t pIconv;

int
numlen(int i)
{
    char szTmp[SNOWMONKEY_MAX_COLUMN_SIZE];
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
        printf("|   Archive size = %ld bytes\n", pInfo->archiveFileSize);
        printf("|   Archive entries count = %d\n", pInfo->archiveEntriesCount);
        printf("|\n");
    }
}

void
init_print_table(OnsenArchiveInfo_t *pInfo)
{
    int i = 0;
    int j = 0;
    int addlFdsCount = 0;
    int iTmpLen = 0;
    int iType = 0;
    long lOffset = 0;
    int iQueriesCount = 0;
    char **a_szQueriedFilenames = NULL;
    int found = 0;
    enum OnsenEncoding pEncoding = ASCII;
    char *szTmpFilename = NULL;
    char *filename;
    char *szTmp;
    void *pFile;
    OnsenPlugin_t *pPlugin;

    iQueriesCount = context->iQueriedFilenamesCount;
    a_szQueriedFilenames = context->a_szQueriedFilenames;

    if (NULL == pInfo->archiveEntries) {
        /* Invalid archive infos */
        return;
    }

    addlFdsCount = pInfo->archiveEntries[0]->addlFdsCount;
    if (0 != addlFdsCount) {
        a_iAddFieldsHeadersLen = calloc(addlFdsCount, sizeof(int));
    }

    filetypes = calloc(pInfo->archiveEntriesCount + 1 , sizeof(char *));
    mediatypes = calloc(pInfo->archiveEntriesCount + 1, sizeof(char *));
    pEncoding = pInfo->archiveFilenamesEncoding;
    for (i = 1; i <= pInfo->archiveEntriesCount; i++) {

        filetypes[i] = NULL;
        mediatypes[i] = NULL;

        if (0 != iQueriesCount) {

            switch(pEncoding) {
                case SHIFT_JIS : {
                    szTmpFilename = onsen_shift_jis2utf8(pIconv, pInfo->archiveEntries[i]->filename);
                    break;
                };
                default : {
                    szTmpFilename = (char *)(pInfo->archiveEntries[i]->filename);
                };
            }

            found = 1;
            for (j = 0; j < iQueriesCount; j++) {
                if (0 == strcmp(szTmpFilename, a_szQueriedFilenames[j])) {
                    found = 0;
                }
            }

            free(szTmpFilename);
        }



        if (found != 0) {
            continue;
        }

        iTmpLen = numlen(pInfo->archiveEntries[i]->size);
        if (iTmpLen > sizeHeaderLen) {
            sizeHeaderLen = iTmpLen;
        }

        iTmpLen = numlen(pInfo->archiveEntries[i]->compressedSize);
        if (iTmpLen > iCompSizeHeaderLen) {
            iCompSizeHeaderLen = iTmpLen;
        }

        for (j = 0; j < addlFdsCount; j++) {
            iTmpLen = strlen(pInfo->archiveEntries[i]->addlFds[j]);
            if (iTmpLen > a_iAddFieldsHeadersLen[j]) {
                a_iAddFieldsHeadersLen[j] = iTmpLen + 1;
            }
        }

        for (j = 0; j < context->iPluginsCount; j++) {
            pPlugin = context->pPlugins[j];

            if (0 == context->pInputFile->isMmaped) {
                pFile = (void *)(&(context->pInputFile->fd));
                lOffset = pInfo->archiveEntries[i]->offset;
            } else {
                pFile = ((unsigned char *)(context->pInputFile->data)) + pInfo->archiveEntries[i]->offset;
                lOffset = pInfo->archiveEntries[i]->size;
            }

            filename = pInfo->archiveEntries[i]->filename;

            iType = pPlugin->isFileSupported(context->pInputFile->isMmaped,
                                                filename,
                                                pFile,
                                                lOffset);
            if (iType > 3) {
                szTmp = calloc(1, SNOWMONKEY_MAX_COLUMN_SIZE);
                pPlugin->getPluginInfo(iType, szTmp,
                                        SNOWMONKEY_MAX_COLUMN_SIZE);
                filetypes[i] = onsen_strdup(szTmp);
                iTmpLen = strlen(szTmp);
                if (iTmpLen > iFiletypeFormat) {
                    iFiletypeFormat = iTmpLen;
                }

                pPlugin->getPluginInfo(iType + 2, szTmp,
                                        SNOWMONKEY_MAX_COLUMN_SIZE);
                mediatypes[i] = onsen_strdup(szTmp);
                iTmpLen = strlen(szTmp);
                if (iTmpLen > iMediatypeFormat) {
                    iMediatypeFormat = iTmpLen;
                }
                free(szTmp);
            }
        }
    }

    if ((int)strlen(SNOWMONKEY_DEFAULT_FILE_TYPE) > iFiletypeFormat) {
        iFiletypeFormat = strlen(SNOWMONKEY_DEFAULT_FILE_TYPE);
    }

    if ((int)strlen(SNOWMONKEY_DEFAULT_MEDIA_TYPE) > iMediatypeFormat) {
        iMediatypeFormat = strlen(SNOWMONKEY_DEFAULT_MEDIA_TYPE);
    }


    sprintf(szSizeHeaderFormat, " %%%ds", sizeHeaderLen);
    sprintf(szCompSizeHeaderFormat, " %%%ds", iCompSizeHeaderLen);
    sprintf(szSizeValueFormat, " %%%dd", sizeHeaderLen);
    sprintf(szCompSizeValueFormat, " %%%dd", iCompSizeHeaderLen);
    sprintf(szFiletypeFormat, " %%%ds", iFiletypeFormat);
    sprintf(szMediatypeFormat, " %%%ds", iMediatypeFormat);
}

void
free_print_table(void)
{
    free(a_iAddFieldsHeadersLen);
    free(filetypes);
    free(mediatypes);
}

void
print_table_header(OnsenArchiveInfo_t *pInfo)
{
    int i = 0;

    char szTmpFormat[SNOWMONKEY_MAX_FORMAT_STRING_LENGTH];
    int addlFdsCount = 0;

    if (NULL == pInfo->archiveEntries) {
        /* Invalid archive infos */
        return;
    }

    if (context->bVerbose) {
        addlFdsCount = pInfo->archiveEntries[0]->addlFdsCount;

        /* Headers */
        printf("%12s", "Offset");
        printf(szSizeHeaderFormat, "Size");
        printf("%12s", "Compressed");
        printf(szCompSizeHeaderFormat, "Comp. Size");
        printf("%12s", "Encrypted");
        for (i = 0; i < addlFdsCount; i++) {
            sprintf(szTmpFormat, " %%%ds", a_iAddFieldsHeadersLen[i]);
            printf(szTmpFormat, pInfo->archiveEntries[0]->addlFds[i]);
        }
        printf(szFiletypeFormat, "File type");
        printf(szMediatypeFormat, "Media type");
        printf("%s", "  Name");
        printf("\n");

        /* Headers separation */
        printf("%12s ", "-----------");
        repeat_print_char(sizeHeaderLen, '-');
        printf("%12s ", "-----------");
        repeat_print_char(iCompSizeHeaderLen, '-');
        printf("%12s ", "-----------");
        for (i = 0; i < addlFdsCount; i++) {
            repeat_print_char(a_iAddFieldsHeadersLen[i], '-');
        }
        printf(" ");
        repeat_print_char(iFiletypeFormat, '-');
        printf(" ");
        repeat_print_char(iMediatypeFormat, '-');
        printf(" ");
        printf(" %s", "----------------");
        printf("\n");
    }
}

void
print_entry(OnsenArchiveEntry_t *pEntry, char *filename, int posInArchive)
{
    int i;
    char *szFiletype = NULL;
    char *szMediatype = NULL;

    char szTmpFormat[SNOWMONKEY_MAX_FORMAT_STRING_LENGTH];

    if (context->bVerbose) {

        printf("%012X", pEntry->offset);
        printf(szSizeValueFormat, pEntry->size);
        printf("%12s", (pEntry->isCompressed) ? "yes" : "no");
        printf(szCompSizeValueFormat, pEntry->compressedSize);
        printf("%12s", (pEntry->isEncrypted) ? "yes" : "no");
        if (0 != pEntry->addlFdsCount) {
            for (i = 0; i < pEntry->addlFdsCount; i++) {
                if (NULL != pEntry->addlFds[i]) {
                    sprintf(szTmpFormat, " %%%ds", a_iAddFieldsHeadersLen[i]);
                    printf(szTmpFormat, pEntry->addlFds[i]);
                }
            }
        }

        szFiletype = filetypes[posInArchive];
        szMediatype = mediatypes[posInArchive];

        if (NULL == szFiletype) {
            szFiletype = onsen_strdup(SNOWMONKEY_DEFAULT_FILE_TYPE);
        }
        if (NULL == szMediatype) {
            szMediatype = onsen_strdup(SNOWMONKEY_DEFAULT_MEDIA_TYPE);
        }

        printf(szFiletypeFormat, szFiletype);
        printf(szMediatypeFormat, szMediatype);

        printf("  ");

        free(szFiletype);
        free(szMediatype);
    }

    printf("%s\n", filename);
}
