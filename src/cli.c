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
#include "cli.h"

extern Context_t *context;

int optionPrintUsage;
int optionPrintVersion;
int optionVerbose;
int optionQueriedFilenamesCount;
int optionSelectPlugin = -1;
SnowmonkeyActionMode action;

char *optionInputFilename;
char *optionOutputDir;
char *optionPluginsFilenames;
char *optionPluginsDirs;
char **optionQueriedFilenames;

#ifdef HAVE_GETOPT_LONG
    #include <getopt.h>

    static struct option longOptions[] = {
        { "extract",        0, NULL, 'x' },
        { "get",            0, NULL, 'x' },
        { "file",           1, NULL, 'f' },
        { "plugin",         1, NULL, 'n' },
        { "plugins",        1, NULL, 'p' },
        { "plugins-dirs",   1, NULL, 'P' },
        { "directory",      1, NULL, 'C' },
        { "help",           0, NULL, 'h' },
        { "list",           0, NULL, 't' },
        { "usage",          0, &optionPrintUsage, 1 },
        { "verbose",        0, NULL, 'v' },
        { "version",        0, &optionPrintVersion, 1 },
        { NULL,             0, NULL, 0 }
    };
#endif

void
version()
{
    static char const version[] = "snowmonkey v%s.%s%s\n\
Copyright (C) 2011, 2013 Etienne 'lenaing' Girondel.\n\
CeCILL License:\n\
    FR : <http://www.cecill.info/licences/Licence_CeCILL_V2-fr.txt>.\n\
    EN : <http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt>.\n\
This is free software: you are free to change and redistribute it.\n\
\n\
Written by Etienne 'lenaing' Girondel <lenaing@gmail.com>.\n";
    fprintf(stderr, version, 
                    SM_VERSION_MAJOR,
                    SM_VERSION_MINOR,
                    SM_VERSION_RELEASE);
}

void
help()
{
    /* Help start. */
    static char const help[] = "Usage: snowmonkey [OPTION...] [FILES]...\n\
snowmonkey can list or restore individual files from a disk archive.\n\
\n\
Examples:\n\
  snowmonkey -tvf archive.dat\t# List all files in archive.dat verbosely.\
\n\
  snowmonkey -xf archive.dat\t# Extract all files from archive.dat.\n\n";

    /* Help on options. */
#ifdef HAVE_GETOPT_LONG
    static char const helpOptions[] = "\
 Main operation mode:\n\n\
  -t, --list                 List the contents of an archive.\n\
  -x, --extract, --get       Extract files from an archive.\n\n\
 Device selection and switching:\n\n\
  -f, --file=ARCHIVE         Use archive file or device ARCHIVE.\n\n\
 Local file selection:\n\n\
  -C, --directory=DIR        Output to directory DIR.\n\n\
";
    static char const helpOptions2[] = "\
 Plugins selection:\n\n\
  -n, --plugin               Select plugin ID.\n\
  -p, --plugins              Load plugins only if they match specified\n\
                             filenames.\n\
  -P, --plugins-dir          Search plugins only in specified directories.\n\n\
 Informative output:\n\n\
  -v, --verbose              Verbosely list files processed.\n\n";
    static char const helpOptions3[] = "\
 Other options:\n\n\
  -h, -?, --help             Give this help list.\n\
          --usage            Give a short usage message.\n\
          --version          Print program version.\n\n";
#else
    static char const helpOptions[] = "\
 Main operation mode:\n\n\
  -t                         List the contents of an archive.\n\
  -x                         Extract files from an archive.\n\n\
 Device selection and switching:\n\n\
  -f                         Use given archive file or device.\n\n\
 Local file selection:\n\n\
  -C                         Output to given directory.\n\n";
    static char const helpOptions2[] = "\
 Plugins selection:\n\n\
  -n                         Select plugin ID.\n\
  -p                         Load plugins only if they match specified\n\
                             filenames.\n\
  -P                         Search plugins only in specified directories.\n\n\
 Informative output:\n\n\
  -v                         Verbosely list files processed.\n\n\
 Other options:\n\n\
  -h, -?                     Give this help list.\n\n";
#endif

    /* Help end. */
    static char const helpEnd[] = "Report bugs to <lenaing@gmail.com>.\n";
    fprintf(stderr, help);
    fprintf(stderr, helpOptions);
    fprintf(stderr, helpOptions2);
#ifdef HAVE_GETOPT_LONG
    fprintf(stderr, helpOptions3);
#endif
    fprintf(stderr, helpEnd);
}

void
usage()
{
    static char const optionsUsage[] = "\
Usage: snowmonkey [-txhv?] [-f ARCHIVE] [FILES]\n\
            [-C DIR] [-n ID]\n";
#ifdef HAVE_GETOPT_LONG
    static char const longOptionsUsage[] = "\
            [--file]\n\
            [--verbose]\n\
            [--list] [--extract] [--get]\n\
            [--plugin] [--plugins] [--plugins-dir]\n\
            [--help] [--usage] [--version]\n";
#endif
    fprintf(stderr, optionsUsage);
#ifdef HAVE_GETOPT_LONG
    fprintf(stderr, longOptionsUsage);
#endif
}


void
parse_options(int argc, char *argv[])
{
    int i;
    int mainOperationModeArgs = 0;
    int error = 0;
#ifdef HAVE_GETOPT_LONG
    int optionIndex = 0;
#endif
    char option;
    char *options = ":C:f:n:hp:P:tvx";

    while (1) {

#ifdef HAVE_GETOPT_LONG
        option = getopt_long(argc, argv, options, longOptions, &optionIndex);
#else
        option = getopt(argc, argv, options);
#endif

        if (option == -1) {
            break;
        }

        if (1 == optionPrintUsage) {
            usage();
            exit(EXIT_SUCCESS);
        }

        if (1 == optionPrintVersion) {
            version();
            exit(EXIT_SUCCESS);
        }

        switch (option) {
            case 'C' : {
                if (NULL == optionOutputDir) {
                    optionOutputDir = optarg;
                } else {
                    error = 1;
                    fprintf(stderr, "Error: You can only specify one output ");
                    fprintf(stderr, "directory.\n");
                }
            }
                break;
            case 'f' : {
                if (NULL == optionInputFilename) {
                    optionInputFilename = optarg;
                } else {
                    error = 1;
                    fprintf(stderr, "Error: You can only specify one archive ");
                    fprintf(stderr, "file.\n");
                }
            }
                break;
            case 'n' : {
                for (i = 0; i < (int)strlen(optarg); i++) {
                    if (!isdigit(optarg[i])) {
                        error = 1;
                        fprintf(stderr, "Plugin select value is not an ");
                        fprintf(stderr, "integer.\n");
                        break;
                    }
                }
                optionSelectPlugin = atoi(optarg);
            }
                break;
            case 'p' :
                optionPluginsFilenames = optarg;
                break;
            case 'P' : 
                optionPluginsDirs = optarg;
                break;
            case 't' : {
                action = SNOWMONKEY_LIST;
                mainOperationModeArgs++;
            }
                break;
            case 'v' :
                optionVerbose = 1;
                break;
            case 'x' : {
                action = SNOWMONKEY_EXTRACT;
                mainOperationModeArgs++;
            }
                break;
            case '?' :
            case 'h' :
                help();
                exit(EXIT_SUCCESS);
            case ':' :
                error = 1;
                fprintf(stderr, "Error: Missing arguments.\n");
            default :
                break;
        }

    }

    if (1 != mainOperationModeArgs) {
        error = 1;
        fprintf(stderr, "You may not specify more than one `-tx' ");
        fprintf(stderr, "option.\n");
    }

    if (optind != argc) {
        optionQueriedFilenamesCount = (argc - optind);
        optionQueriedFilenames = calloc(optionQueriedFilenamesCount,
                                            sizeof(char *));
        for (i = 0; i < optionQueriedFilenamesCount; i++) {
            optionQueriedFilenames[i] = argv[optind++];
        }
    }

    if (1 == error) {
        usage();
        exit(EXIT_FAILURE);
    }

}

void
initialize_cli_context()
{
    context = new_context();
    context->inputFilename = optionInputFilename;
    context->outputDir = optionOutputDir;
    context->queriedFilenames = optionQueriedFilenames;
    context->queriedFilenamesCount = optionQueriedFilenamesCount;
    context->pluginsFilenames = optionPluginsFilenames;
    context->pluginsDirs = optionPluginsDirs;
    context->selectedPlugin = optionSelectPlugin;
    context->defaultPlugin = -1;
    context->action = action;
    context->verbose = optionVerbose;
}

void
end_cli_context()
{
    free_context(context);
}
