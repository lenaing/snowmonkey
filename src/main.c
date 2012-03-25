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
#include "config.h"
#include "globals.h"
#include "cli.h"
#include "context.h"
#include "freedesktop.h"
#include "plugins.h"
#include "file.h"
#include "actions.h"
#include <libonsen/onsen.h>

#define LIBONSEN_VERBOSITY 5

Context_t *context;

void start()
{
    /* Get an unbuffered output. */
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    if (context->bVerbose) {
        fprintf(stdout, "            @(⌒⌒ )@            \n\n");
        fprintf(stdout, "       Snow Monkey v%d.%d%s      \n",
                        SM_VERSION_MAJOR,
                        SM_VERSION_MINOR,
                        SM_VERSION_RELEASE);
        fprintf(stdout, "--------------------------------\n");
    }
    onsen_init(LIBONSEN_VERBOSITY);
}

void stop()
{
    if (context->bVerbose) {
        fprintf(stdout, "---------------------------------\n");
        fprintf(stdout, " Snow monkey ended successfully! \n");
        fprintf(stdout, "            @(⌒_⌒)@              \n");
    }
}

void run() {
    comply_to_freedesktop_specs();
    load_plugins(context->szPluginsFilenames, context->szPluginsDirs);

    if (context->bVerbose) {
        fprintf(stdout, "|   Successfully loaded %d plugins.\n",
                context->iPluginsCount);
    }

    if (0 == context->iPluginsCount) {
        fprintf(stdout, "|   No plugin loaded, can't open a file without a plugin...\n");
        
    } else {
        if (0 == file_exists(context->szInputFilename)) {
            load_file();
            if (1 == check_fileformat_supported_by_plugins()) {
                if (context->bList) {
                    do_list();
                }
            }
            unload_file();
        } else {
            if (NULL == context->szInputFilename) {
                printf("No input file specified!\n");
            } else {
                printf("Unable to open file '%s'!\n", context->szInputFilename);
            }
        }
    }
    unload_plugins();
}


int main(int argc, char *argv[])
{
    parse_options(argc, argv);
    initialize_cli_context();
    start();
    run();
    stop();
    end_cli_context();
    return EXIT_SUCCESS;
}
