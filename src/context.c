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
#include "context.h"

Context_t *
new_context()
{
    Context_t *pContext;
    pContext = malloc(sizeof(Context_t));

    pContext->szInputFilename = NULL;
    pContext->pInputFile = NULL;
    pContext->szOutputDir = NULL;
    pContext->a_szQueriedFilenames = NULL;
    pContext->iQueriedFilenamesCount = 0;

    pContext->pPlugins = NULL;
    pContext->iPluginsCount = 0;
    pContext->szPluginsDirs = NULL;
    pContext->szPluginsFilenames = NULL;

    pContext->bList = 0;
    pContext->bExtract = 0;
    pContext->bVerbose = 0;

    return pContext;
}

void
free_context(Context_t *pContext)
{
    if (NULL != pContext) {
        if (NULL != pContext->a_szQueriedFilenames) {
            free(pContext->a_szQueriedFilenames);
        }

        free(pContext);
    }
}
