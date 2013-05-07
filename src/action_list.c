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
#include <magic.h>
extern Context_t *context;

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
    int bUpdatedFileTypeColumnSize = 0;
    int bUpdatedMediaTypeColumnSize = 0;
    long lOffset = 0;
    char *filename;
    char *szTmp;
    void *pFile;
    OnsenPlugin_t *pPlugin;
    /*magic_t cookie;*/

    if (NULL == pInfo->archiveEntries) {
        /* Invalid archive infos */
        return;
    }

    /*cookie = magic_open(MAGIC_NONE);
    if (0 != magic_load(cookie, NULL)) {
        printf("cannot load magic database - %s\n", magic_error(cookie));
    }*/
    addlFdsCount = pInfo->archiveEntries[0]->addlFdsCount;
    if (0 != addlFdsCount) {
        a_iAddFieldsHeadersLen = calloc(addlFdsCount, sizeof(int));
    }

    for (i = 1; i <= pInfo->archiveEntriesCount; i++) {
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

            if (iType > 0) {
                szTmp = calloc(1, SNOWMONKEY_MAX_COLUMN_SIZE);
                pPlugin->getPluginInfo(iType + 1, szTmp,
                                        SNOWMONKEY_MAX_COLUMN_SIZE);
                iTmpLen = strlen(szTmp);
                if (iTmpLen > iFiletypeFormat) {
                    iFiletypeFormat = iTmpLen;
                }

                pPlugin->getPluginInfo(iType + 2, szTmp,
                                        SNOWMONKEY_MAX_COLUMN_SIZE);
                iTmpLen = strlen(szTmp);
                if (iTmpLen > iMediatypeFormat) {
                    iMediatypeFormat = iTmpLen;
                }
                free(szTmp);
                bUpdatedFileTypeColumnSize = 1;
                bUpdatedMediaTypeColumnSize = 1;
            }
            /* TODO : Fallback to Mediatype */
        }

        /*if (j == context->iPluginsCount) {

            if (1 == context->pInputFile->isMmaped) {
                magic_setflags(cookie, MAGIC_NONE);
                szTmp = onsen_strdup(magic_buffer(cookie, (const void *)(context->pInputFile->data + pInfo->archiveEntries[i]->offset), pInfo->archiveEntries[i]->size));
                iTmpLen = strlen(szTmp);
                if (iTmpLen > iFiletypeFormat) {
                    iFiletypeFormat = iTmpLen;
                }
                free(szTmp);
                magic_setflags(cookie, MAGIC_MIME_TYPE);
                szTmp = onsen_strdup(magic_buffer(cookie, (const void *)(context->pInputFile->data + pInfo->archiveEntries[i]->offset), pInfo->archiveEntries[i]->size));
                iTmpLen = strlen(szTmp);
                if (iTmpLen > iMediatypeFormat) {
                    iMediatypeFormat = iTmpLen;
                }
                free(szTmp);
                bUpdatedMediaTypeColumnSize = 1;
            }

        }*/

    }

    if (0 == bUpdatedFileTypeColumnSize) {
        if ((int)strlen(SNOWMONKEY_DEFAULT_FILE_TYPE) > iFiletypeFormat) {
            iFiletypeFormat = strlen(SNOWMONKEY_DEFAULT_FILE_TYPE);
        }
    }

    if (0 == bUpdatedMediaTypeColumnSize) {
        if ((int)strlen(SNOWMONKEY_DEFAULT_MEDIA_TYPE) > iMediatypeFormat) {
            iMediatypeFormat = strlen(SNOWMONKEY_DEFAULT_MEDIA_TYPE);
        }
    }


    /*magic_close(cookie);*/

    sprintf(szSizeHeaderFormat, " %%%ds", sizeHeaderLen);
    sprintf(szCompSizeHeaderFormat, " %%%ds", iCompSizeHeaderLen);
    sprintf(szSizeValueFormat, " %%%dd", sizeHeaderLen);
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
        repeat_print_char(iFiletypeFormat, '-');
        printf(" ");
        repeat_print_char(iMediatypeFormat, '-');
        printf(" ");
        printf(" %s", "----------------");
        printf("\n");
    }
}

void
print_entry(OnsenArchiveEntry_t *pEntry, char *filename)
{
    int i;
    int iType = -1;
    long lEntryOffset;
    char *szEntryFilename = NULL;
    char *szFiletype = NULL;
    char *szMediatype = NULL;
    char *szTmp = NULL;
    char *szTmp2 = NULL;
    void *pEntryFile;
    OnsenPlugin_t *pPlugin;
    /*magic_t cookie;*/
    char szTmpFormat[SNOWMONKEY_MAX_FORMAT_STRING_LENGTH];

    /*cookie = magic_open(MAGIC_NONE);
    if (0 != magic_load(cookie, NULL)) {
        printf("cannot load magic database - %s\n", magic_error(cookie));
    }*/

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

        szEntryFilename = pEntry->filename;

        if (0 == context->pInputFile->isMmaped) {
            pEntryFile = (void *)(&(context->pInputFile->fd));
            lEntryOffset = pEntry->offset;
        } else {
            pEntryFile = ((unsigned char *)(context->pInputFile->data)) + pEntry->offset;
            lEntryOffset = pEntry->size;
        }

        for (i = 0; i < context->iPluginsCount; i++) {
            pPlugin = context->pPlugins[i];

            iType = pPlugin->isFileSupported(context->pInputFile->isMmaped,
                                                szEntryFilename,
                                                pEntryFile,
                                                lEntryOffset);

            if (iType > 0) {
                szTmp = calloc(1, SNOWMONKEY_MAX_COLUMN_SIZE);
                pPlugin->getPluginInfo(iType, szTmp,
                                        SNOWMONKEY_MAX_COLUMN_SIZE);
                szFiletype = szTmp;

                szTmp2 = calloc(1, SNOWMONKEY_MAX_COLUMN_SIZE);
                pPlugin->getPluginInfo(iType + 2, szTmp2,
                                    SNOWMONKEY_MAX_COLUMN_SIZE);
                szMediatype = szTmp2;
                break;
            } 
        }

        /*if (i == context->iPluginsCount) {

            if (1 == context->pInputFile->isMmaped) {
                magic_setflags(cookie, MAGIC_NONE);
                szTmp = onsen_strdup(magic_buffer(cookie, (const void *)(context->pInputFile->data + pEntry->offset), pEntry->size));
                szFiletype = szTmp;
                
                magic_setflags(cookie, MAGIC_MIME_TYPE);
                szTmp2 = onsen_strdup(magic_buffer(cookie, (const void *)(context->pInputFile->data + pEntry->offset), pEntry->size));
                szMediatype = szTmp2;
            }

        }*/

        if (NULL == szFiletype) {
            szFiletype = onsen_strdup(SNOWMONKEY_DEFAULT_FILE_TYPE);
        }
        if (NULL == szMediatype) {
            szMediatype = onsen_strdup(SNOWMONKEY_DEFAULT_MEDIA_TYPE);
        }

        printf(szFiletypeFormat, szFiletype);
        printf(szMediatypeFormat, szMediatype);

        printf("  ");
    }

    /*magic_close(cookie);*/

    printf("%s\n", filename);
    if (NULL != szFiletype) {
        free(szFiletype);
    }
    if (NULL != szMediatype) {
        free(szMediatype);
    }
}
