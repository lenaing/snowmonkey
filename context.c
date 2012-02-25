#include "context.h"

Context_t *
new_context()
{
    Context_t *pContext;
    pContext = malloc(sizeof(Context_t));

    pContext->szArchiveFilename = NULL;
    pContext->pArchiveFile = NULL;

    pContext->pPlugins = NULL;
    pContext->iPluginsCount = 0;

    pContext->bList = 0;
    pContext->bExtract = 0;
    pContext->bVerbose = 0;

    return pContext;
}

void
free_context(Context_t *pContext)
{
    if (NULL != pContext) {
        /* TODO. */
        free(pContext);
    }
}
