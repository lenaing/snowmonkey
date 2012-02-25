#include "file.h"

void
load_file()
{
    if (NULL != context->szArchiveFilename) {
        if (context->bVerbose) {
            fprintf(stdout, "|   Opening file '%s'\n", context->szArchiveFilename);
        }
        context->fdArchiveFile = open(context->szArchiveFilename, O_RDONLY);
        context->lArchiveFileSize = lseek(context->fdArchiveFile, 0, SEEK_END);
        context->pArchiveFile = mmap(NULL, context->lArchiveFileSize, PROT_READ, MAP_PRIVATE, context->fdArchiveFile, 0);

        if (NULL == context->pArchiveFile) {
            /* Failed to open given file.
             * TODO */ 
        }
    }
}

void
unload_file()
{
    if (NULL != context->pArchiveFile) {
        if (context->bVerbose) {
            fprintf(stdout, "|   Closing file '%s'\n", context->szArchiveFilename);
        }
        munmap(context->pArchiveFile, context->lArchiveFileSize);
        close(context->fdArchiveFile);
    }
}
