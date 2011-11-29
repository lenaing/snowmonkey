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

#include <math.h>
#include <libonsen/plugin.h>
#include <libonsen/archive_plugin.h>
#include <libonsen/string_utils.h>
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

double round(double d)
{
  return floor(d + 0.5);
}

void progress(int total, int current, void *data) {

   
    int progressBarWidth;
    float f;
    int iPos;
    int iProgressChars;

    (void) data;
    iPos = 0;
    progressBarWidth=40;

    f = (float)current / (float)total;
    
    iProgressChars = round(f * progressBarWidth);
    
    printf("[");
   
    for ( ; iPos < iProgressChars; iPos++) {
        printf("*");
    }
   
    for ( ; iPos < progressBarWidth; iPos++) {
        printf("-");
    }
   
    printf("] %3.0f%%\r", f*100);
    if (total == current) {
        printf("\n");
    }
    fflush(stdout);
}



int main(/*int argc, char *argv[]*/)
{
    OnsenPlugin_t *pPlugin;
    OnsenArchivePlugin_t *pInstance;
    OnsenArchiveInfo_t *pInfo;
    OnsenArchiveEntry_t *pEntry;

    FILE *pFile;
    int rc;
    int i, j;
    int writeToDisk = 0;

    char *szPlugin = "plugins/dummy.so";
    char *szSrcFile = "/home/lenain/dummy.bod";
    char *szPath = "/home/lenain/tmp/";
    char *szPath2 = "/home/lenain/tmp2/";
    char *szDestFilename;
    char *szOutputDir;

    start();

    /* Load plugin */
    pPlugin = onsen_new_plugin();
    rc = onsen_load_plugin(pPlugin, szPlugin);
    if (rc != 0) {
        /* Failed to load plugin. Abort! ABORT! */
        onsen_unload_plugin(pPlugin);
        onsen_free_plugin(pPlugin);
        onsen_err_critical("Failed to load plugin file '%s'. Aborting.",
                           szPlugin);
    }

    pFile = onsen_open_file(szSrcFile,"rb");
    if (NULL == pFile) {
        /* Failed to open given file. */
        onsen_unload_plugin(pPlugin);
        onsen_free_plugin(pPlugin);
        onsen_err_critical("Failed to open file '%s'. Aborting.", szSrcFile);
    }

    rc = pPlugin->isFileSupported(szSrcFile, 0, pFile);
    if (0 == rc) {
        onsen_out_ok("File '%s' is supported!", szSrcFile);
    } else {
        /* File is unsupported. */
        onsen_unload_plugin(pPlugin);
        onsen_free_plugin(pPlugin);
        onsen_err_critical("File '%s' is NOT supported. Aborting", szSrcFile);
    }

    /* onsen_get_archive_info example ----------------------------------------*/
    onsen_err_warning("");
    onsen_err_warning("Example of onsen_get_archive_info usage :");
    onsen_err_warning("");

    /* Retrieve archive info */
    pInfo = onsen_new_archive_info();
    pInstance = pPlugin->pInstance;
    pInstance->getArchiveInfo(szSrcFile, 0, 0, pInfo);

    /* Print archive info */
    onsen_out_ok("Archive size = %ld", pInfo->lArchiveFileSize);
    onsen_out_ok("Archive entries count = %d", pInfo->iArchiveEntriesCount);
    onsen_out_ok("");

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

        onsen_out_ok("File '%s' :", pEntry->szFilename);
        onsen_out_ok("\t Size : %d", pEntry->iSize);
        onsen_out_ok("\t Commpressed size : %d", pEntry->iCompressedSize);
        onsen_out_ok("\t Encrypted : %s", (pEntry->bEncrypted) ? "yes" : "no");
        onsen_out_ok("\t Compressed : %s", (pEntry->bCompressed) ? "yes" : "no");
        onsen_out_ok("\t Additional fields :");
        for (j = 0; j < pEntry->iAddlFdsCount; j++) {
            if (NULL != pEntry->a_szAddlFds[j]) {
                onsen_out_ok("\t\t %s : %s", pInfo->a_pArchiveEntries[pInfo->iArchiveEntriesCount]->a_szAddlFds[j], pEntry->a_szAddlFds[j]);
            }
        }

        if (writeToDisk) {
            /* Correct filename for an UNIX system. */
            szDestFilename = onsen_build_filename(szPath, pEntry->szFilename);
            onsen_str_chr_replace(szDestFilename,'\\', '/');

            /* Build directory tree. */
            szOutputDir = onsen_basedir(szDestFilename);
            onsen_mkdir(szOutputDir);

            /* Write file to disk. */
            pInstance->writeFile(szSrcFile, 0, pEntry->iOffset, pEntry->iSize, szDestFilename, 0, progress, NULL);

            /* Free path and filename */
            onsen_free(szOutputDir);
            onsen_free(szDestFilename);
        }
    }

    onsen_free_archive_info(pInfo);

    /* onsen_get_file_info example -------------------------------------------*/
    onsen_err_warning("");
    onsen_err_warning("Example of onsen_get_file_info usage :");
    onsen_err_warning("");

    pEntry = onsen_new_archive_entry();
    rc = pInstance->getFileInfo(szSrcFile, 0, "blah0", 0, pEntry);

    if (0 == rc) {
        onsen_out_ok("File '%s' :", pEntry->szFilename);
        onsen_out_ok("\t Size : %d", pEntry->iSize);
        onsen_out_ok("\t Commpressed size : %d", pEntry->iCompressedSize);
        onsen_out_ok("\t Encrypted : %s", (pEntry->bEncrypted) ? "yes" : "no");
        onsen_out_ok("\t Compressed : %s", (pEntry->bCompressed) ? "yes" : "no");
        onsen_out_ok("\t Additional fields :");
        for (j = 0; j < pEntry->iAddlFdsCount; j++) {
            if (NULL != pEntry->a_szAddlFds[j]) {
                onsen_out_ok("\t\t %s : %s", "CRC", pEntry->a_szAddlFds[j]);
            }
        }

        if (writeToDisk) {
            /* Correct filename for an UNIX system. */
            szDestFilename = onsen_build_filename(szPath2, pEntry->szFilename);
            onsen_str_chr_replace(szDestFilename,'\\', '/');

            /* Build directory tree. */
            szOutputDir = onsen_basedir(szDestFilename);
            onsen_mkdir(szOutputDir);

            /* Write file to disk. */
            pInstance->writeFile(szSrcFile, 0, pEntry->iOffset, pEntry->iSize, szDestFilename, 0, progress, NULL);

            /* Free path and filename */
            onsen_free(szOutputDir);
            onsen_free(szDestFilename);
        }
    }
    
    onsen_free_archive_entry(pEntry);
    onsen_unload_plugin(pPlugin);
    onsen_free_plugin(pPlugin);

    onsen_close_file(pFile);
    stop();
    return EXIT_SUCCESS;
}
