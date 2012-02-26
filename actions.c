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
        fprintf(stdout, "|   Archive size = %ld\n", pInfo->lArchiveFileSize);
        fprintf(stdout, "|   Archive entries count = %d\n", pInfo->iArchiveEntriesCount);
        fprintf(stdout, "|\n");
    }

    switch(pInfo->eArchiveFilenamesEncoding) {
        case SHIFT_JIS : {
            pIconv = onsen_iconv_init("UTF-8", OnsenEncodings[pInfo->eArchiveFilenamesEncoding]);
            break;
        };
        default : {
        };
    }


    /* For each file in archive, print info */
    for (i = 0; i < pInfo->iArchiveEntriesCount; i++) {

        if (NULL == pInfo->a_pArchiveEntries) {
            /* No archive entry defined. Stop here.*/
            break;
        }

        pEntry = pInfo->a_pArchiveEntries[i];
        if (NULL == pEntry) {
            /* Invalid entry, check next. */
            continue;
        }

        if (pIconv != NULL) {
            
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

        if (context->bVerbose) {
            fprintf(stdout, "| Offset : %04X\n", pEntry->iOffset);
            fprintf(stdout, "| Size : %d\n", pEntry->iSize);
            fprintf(stdout, "| Compressed size : %d\n", pEntry->iCompressedSize);
            fprintf(stdout, "| Encrypted : %s\n", (pEntry->bEncrypted) ? "yes" : "no");
            fprintf(stdout, "| Compressed : %s\n", (pEntry->bCompressed) ? "yes" : "no");
            fprintf(stdout, "| Additional fields :\n");
            for (j = 0; j < pEntry->iAddlFdsCount; j++) {
                if (NULL != pEntry->a_szAddlFds[j]) {
                    fprintf(stdout, "|\t %s : %s\n", pInfo->a_pArchiveEntries[pInfo->iArchiveEntriesCount]->a_szAddlFds[j], pEntry->a_szAddlFds[j]);
                }
            }
            fprintf(stdout, "|\n");
        }
    }
    if (pIconv != NULL) {
        onsen_iconv_cleanup(pIconv);
    }
    onsen_free_archive_info(pInfo);
}
