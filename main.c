/*
 * Copyright 2011 - Etienne 'lenaing' GIRONDEL <lenaing@gmail.com>
 * 
 * snowmonkey :
 * ------------
 * This software is a libonsen CLI interface under heavy development.
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

#include <libonsen/archive_plugin.h>
#include <libonsen/file_utils.h>
#include <libonsen/onsen.h>

void start()
{
    /* Get an unbuffered output. */
    setbuf(stdout, NULL);
    fprintf(stdout, "\n");
    fprintf(stdout, "            @(⌒⌒ )@            \n\n");
    fprintf(stdout, "       Snow Monkey v%d.%d%s      \n",
                    SM_VERSION_MAJOR,
                    SM_VERSION_MINOR,
                    SM_VERSION_RELEASE);
    fprintf(stdout, "--------------------------------\n");
    onsen_init();
}

void stop()
{
    fprintf(stdout, "--------------------------------\n");
    fprintf(stdout, " Snow monkey ended successfully! \n");
    fprintf(stdout, "            @(⌒_⌒)@            \n\n");
}

int main(/*int argc, char *argv[]*/)
{
    OnsenArchivePlugin_t *pPlugin; 
    FILE *pFile;

    start();

    pFile = onsen_open_file("/home/lenain/Public/dummy.blah","rb");


    pPlugin = onsen_new_archive_plugin();
    onsen_load_archive_plugin(pPlugin,"plugins/dummy.so");


    pPlugin->openArchive(pPlugin, pFile);

    pPlugin->extractAllArchive(pPlugin, "/home/lenain/tmp/");

    pPlugin->closeArchive(pPlugin);

    onsen_unload_archive_plugin(pPlugin);
    onsen_free_archive_plugin(pPlugin);

    onsen_close_file(pFile);
    stop();
    return EXIT_SUCCESS;
}
