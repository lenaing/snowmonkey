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
#include "context.h"

Context_t *
new_context()
{
    Context_t *context;
    context = malloc(sizeof(Context_t));

    context->inputFilename = NULL;
    context->outputDir = NULL;
    context->queriedFilenames = NULL;
    context->queriedFilenamesCount = 0;

    context->plugins = NULL;
    context->pluginsCount = 0;
    context->pluginsDirs = NULL;
    context->pluginsFilenames = NULL;
    context->defaultPlugin = -1;

    context->action = SNOWMONKEY_LIST;
    context->verbose = 0;

    return context;
}

void
free_context(Context_t *context)
{
    if (NULL != context) {
        if (NULL != context->queriedFilenames) {
            free(context->queriedFilenames);
        }

        free(context);
    }
}
