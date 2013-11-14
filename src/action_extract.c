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
#include "action_extract.h"

extern Context_t *context;

void
extract_entry(OnsenArchivePlugin_t *instance, OnsenArchiveEntry_t *entry,
                char *filename)
{
    int error = 0;
    void *srcFile = NULL;
    char *destFilename = NULL;
    char *outputDir = NULL;
    char *tmpPath = NULL;
    char *tmpDir = NULL;

    if (NULL != context->outputDir) {
        outputDir = context->outputDir;
    } else {
        outputDir = ".";
    }

    destFilename = onsen_build_filename(outputDir, filename);

#if defined(__linux__) || defined(__GNU__)
    /* Correct filenames for an UNIX system. */
    onsen_str_chr_replace(destFilename, '\\', '/');
#endif

    tmpPath = onsen_strdup(destFilename);
    tmpDir = dirname(tmpPath);
    if (-1 == (intptr_t)tmpDir) {
        perror(tmpDir);
        fprintf(stderr, "Failed to get output directory of file ");
        fprintf(stderr, "'%s'.\n", destFilename);
        error = 1;
    }

    if (0 == error) {
        /* Build directory tree for this file. */
        if (0 == onsen_mkdir(tmpDir)) {
            fprintf(stderr, "Failed to create output directory for file ");
            fprintf(stderr, "'%s'.\n", destFilename);
        } else {
            if (context->verbose) {
                /* Make some space for progress indicator */
                printf("       ");
            }

            printf("%s", filename);

            /* Write file to disk. */
            if (0 == context->inputFile->isMmaped) {
                srcFile = (void *)(&(context->inputFile->fd));
            } else {
                srcFile = context->inputFile->data;
            }

            instance->writeFile(context->inputFile->isMmaped,
                                    srcFile,
                                    entry->offset,
                                    0,
                                    destFilename,
                                    entry->compressedSize,
                                    (context->verbose) ? print_progress : NULL,
                                    entry);
        }
    }
    printf("\n");

    onsen_free(tmpPath);
    onsen_free(destFilename);
}
