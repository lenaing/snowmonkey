#include "file.h"

void
load_file()
{
    if (NULL != context->szInputFilename) {
        if (context->bVerbose) {
            fprintf(stdout, "|   Opening file '%s'\n", context->szInputFilename);
        }
        context->fdInputFile = open(context->szInputFilename, O_RDONLY);
        context->lInputFileSize = lseek(context->fdInputFile, 0, SEEK_END);
        context->pInputFile = mmap(NULL, context->lInputFileSize, PROT_READ, MAP_PRIVATE, context->fdInputFile, 0);

        if (NULL == context->pInputFile) {
            /* Failed to open given file.
             * TODO */ 
        }
    }
}

void
unload_file()
{
    if (NULL != context->pInputFile) {
        if (context->bVerbose) {
            fprintf(stdout, "|   Closing file '%s'\n", context->szInputFilename);
        }
        munmap(context->pInputFile, context->lInputFileSize);
        close(context->fdInputFile);
    }
}
