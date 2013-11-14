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
#include "action_list.h"
extern Context_t *context;
extern char *OnsenEncodings[];

int sizeHeaderLen = 12;
int compSizeHeaderLen = 12;
int filetypeFormatLen = 12;
int mediatypeFormatLen = 12;
int *addFieldsHeadersLen = NULL;
char sizeHeaderFormat[100];
char sizeValueFormat[100];
char compSizeHeaderFormat[100];
char compSizeValueFormat[100];
char filetypeFormat[100];
char mediatypeFormat[100];

char **filetypes;
char **mediatypes;

extern iconv_t pIconv;

int
numlen(int i)
{
    char tmp[SNOWMONKEY_MAX_COLUMN_SIZE];
    return sprintf(tmp, "%d", i);
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
print_info(OnsenArchiveInfo_t *info)
{
    if (context->verbose) {
        printf("|   Archive filename = %s\n", context->inputFilename);
        printf("|   Archive size = %ld bytes\n", info->archiveFileSize);
        printf("|   Archive entries count = %d\n", info->archiveEntriesCount);
        printf("|\n");
    }
}

void
init_print_table(OnsenArchiveInfo_t *info)
{
    int i = 0;
    int j = 0;
    int addlFdsCount = 0;
    int found = 0;
    int queriesCount = 0;
    int tmpLen = 0;
    int type = 0;
    long offset = 0;
    char *tmp = NULL;
    char *filename = NULL;
    char **queriedFilenames = NULL;
    void *file;
    enum OnsenEncoding encoding = ASCII;
    OnsenPlugin_t *plugin;
    OnsenArchiveEntry_t *entry;

    queriesCount = context->queriedFilenamesCount;
    queriedFilenames = context->queriedFilenames;

    if (NULL == info->archiveEntries) {
        /* Invalid archive infos */
        return;
    }

    addlFdsCount = info->archiveEntries[0]->addlFdsCount;
    if (0 != addlFdsCount) {
        addFieldsHeadersLen = calloc(addlFdsCount, sizeof(int));
    }

    filetypes = calloc(info->archiveEntriesCount + 1 , sizeof(char *));
    mediatypes = calloc(info->archiveEntriesCount + 1, sizeof(char *));
    encoding = info->archiveFilenamesEncoding;
    for (i = 1; i <= info->archiveEntriesCount; i++) {

        entry = info->archiveEntries[i];
        filename = entry->filename;
        filetypes[i] = NULL;
        mediatypes[i] = NULL;

        if (0 != queriesCount) {
            switch(encoding) {
                case SHIFT_JIS : {
                    tmp = onsen_shift_jis2utf8(pIconv, filename);
                    break;
                };
                default : {
                    tmp = (char *)(filename);
                };
            }

            found = 1;
            for (j = 0; j < queriesCount; j++) {
                if (0 == strcmp(tmp, queriedFilenames[j])) {
                    found = 0;
                }
            }

            free(tmp);
        }



        if (found != 0) {
            continue;
        }

        tmpLen = numlen(entry->size);
        if (tmpLen > sizeHeaderLen) {
            sizeHeaderLen = tmpLen;
        }

        tmpLen = numlen(entry->compressedSize);
        if (tmpLen > compSizeHeaderLen) {
            compSizeHeaderLen = tmpLen;
        }

        for (j = 0; j < addlFdsCount; j++) {
            tmpLen = strlen(entry->addlFds[j]);
            if (tmpLen > addFieldsHeadersLen[j]) {
                addFieldsHeadersLen[j] = tmpLen + 1;
            }
        }

        for (j = 0; j < context->pluginsCount; j++) {
            plugin = context->plugins[j];

            if (0 == context->inputFile->isMmaped) {
                file = (void *)(&(context->inputFile->fd));
                offset = entry->offset;
            } else {
                file = ((unsigned char *)(context->inputFile->data)) +
                                               entry->offset;
                offset = entry->size;
            }

            type = plugin->isFileSupported(context->inputFile->isMmaped,
                                                filename,
                                                file,
                                                offset);
            if (type > 3) {
                tmp = calloc(1, SNOWMONKEY_MAX_COLUMN_SIZE);
                plugin->getPluginInfo(type, tmp, SNOWMONKEY_MAX_COLUMN_SIZE);
                filetypes[i] = onsen_strdup(tmp);
                tmpLen = strlen(tmp);
                if (tmpLen > filetypeFormatLen) {
                    filetypeFormatLen = tmpLen;
                }

                plugin->getPluginInfo(type +2, tmp, SNOWMONKEY_MAX_COLUMN_SIZE);
                mediatypes[i] = onsen_strdup(tmp);
                tmpLen = strlen(tmp);
                if (tmpLen > mediatypeFormatLen) {
                    mediatypeFormatLen = tmpLen;
                }
                free(tmp);
            }
        }
    }

    if ((int)strlen(SNOWMONKEY_DEFAULT_FILE_TYPE) > filetypeFormatLen) {
        filetypeFormatLen = strlen(SNOWMONKEY_DEFAULT_FILE_TYPE);
    }

    if ((int)strlen(SNOWMONKEY_DEFAULT_MEDIA_TYPE) > mediatypeFormatLen) {
        mediatypeFormatLen = strlen(SNOWMONKEY_DEFAULT_MEDIA_TYPE);
    }

    sprintf(sizeHeaderFormat, " %%%ds", sizeHeaderLen);
    sprintf(compSizeHeaderFormat, " %%%ds", compSizeHeaderLen);
    sprintf(sizeValueFormat, " %%%dd", sizeHeaderLen);
    sprintf(compSizeValueFormat, " %%%dd", compSizeHeaderLen);
    sprintf(filetypeFormat, " %%%ds", filetypeFormatLen);
    sprintf(mediatypeFormat, " %%%ds", mediatypeFormatLen);
}

void
free_print_table(void)
{
    free(addFieldsHeadersLen);
    free(filetypes);
    free(mediatypes);
}

void
print_table_header(OnsenArchiveInfo_t *info)
{
    int i = 0;
    int addlFdsCount = 0;
    char tmformat[SNOWMONKEY_MAX_FORMAT_STRING_LENGTH];

    if (NULL == info->archiveEntries) {
        /* Invalid archive infos */
        return;
    }

    if (context->verbose) {
        addlFdsCount = info->archiveEntries[0]->addlFdsCount;

        /* Headers */
        printf("%12s", "Offset");
        printf(sizeHeaderFormat, "Size");
        printf("%12s", "Compressed");
        printf(compSizeHeaderFormat, "Comp. Size");
        printf("%12s", "Encrypted");
        for (i = 0; i < addlFdsCount; i++) {
            sprintf(tmformat, " %%%ds", addFieldsHeadersLen[i]);
            printf(tmformat, info->archiveEntries[0]->addlFds[i]);
        }
        printf(" ");
        printf(filetypeFormat, "File type");
        printf(mediatypeFormat, "Media type");
        printf("%s", "  Name");
        printf("\n");

        /* Headers separation */
        printf("%12s ", "-----------");
        repeat_print_char(sizeHeaderLen, '-');
        printf("%12s ", "-----------");
        repeat_print_char(compSizeHeaderLen, '-');
        printf("%12s ", "-----------");
        for (i = 0; i < addlFdsCount; i++) {
            repeat_print_char(addFieldsHeadersLen[i], '-');
        }
        printf(" ");
        repeat_print_char(filetypeFormatLen, '-');
        printf(" ");
        repeat_print_char(mediatypeFormatLen, '-');
        printf(" ");
        printf(" %s", "----------------");
        printf("\n");
    }
}

void
print_entry(OnsenArchiveEntry_t *entry, char *filename, int posInArchive)
{
    int i;
    char *filetype = NULL;
    char *mediatype = NULL;
    char tmpFormat[SNOWMONKEY_MAX_FORMAT_STRING_LENGTH];

    if (context->verbose) {

        printf("%012X", entry->offset);
        printf(sizeValueFormat, entry->size);
        printf("%12s", (entry->isCompressed) ? "yes" : "no");
        printf(compSizeValueFormat, entry->compressedSize);
        printf("%12s", (entry->isEncrypted) ? "yes" : "no");
        if (0 != entry->addlFdsCount) {
            for (i = 0; i < entry->addlFdsCount; i++) {
                if (NULL != entry->addlFds[i]) {
                    sprintf(tmpFormat, " %%%ds", addFieldsHeadersLen[i]);
                    printf(tmpFormat, entry->addlFds[i]);
                }
            }
        }

        filetype = filetypes[posInArchive];
        mediatype = mediatypes[posInArchive];

        if (NULL == filetype) {
            filetype = onsen_strdup(SNOWMONKEY_DEFAULT_FILE_TYPE);
        }
        if (NULL == mediatype) {
            mediatype = onsen_strdup(SNOWMONKEY_DEFAULT_MEDIA_TYPE);
        }

        printf(" ");
        printf(filetypeFormat, filetype);
        printf(mediatypeFormat, mediatype);

        printf("  ");

        free(filetype);
        free(mediatype);
    }

    printf("%s\n", filename);
}
