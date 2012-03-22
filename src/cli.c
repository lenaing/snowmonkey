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
#include "cli.h"

#ifdef HAS_LONG_OPT
    #include <getopt.h>
#endif

void
version()
{
    static char const version[] = "snowmonkey v%d.%d%s\n\
Copyright (C) 2011, 2012 Etienne 'lenaing' Girondel.\n\
CeCILL License: \n\
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
    static char const help[] = "Usage: snowmonkey [OPTION...] [FILE]...\n\
snowmonkey can list or restore individual files from a disk archive.\n\
\n\
Examples:\n\
  snowmonkey -tvf archive.dat\t# List all files in archive.dat verbosely.\
\n\
  snowmonkey -xf archive.dat\t# Extract all files from archive.dat.\n\n";

    /* Help on options. */
#ifdef HAS_LONG_OPT
    static char const helpOptions[] = "\
 Main operation mode:\n\n\
  -t, --list                 List the contents of an archive.\n\
  -x, --extract, --get       Extract files from an archive.\n\n\
 Device selection and switching:\n\n\
  -f, --file=ARCHIVE         Use archive file or device ARCHIVE.\n\n\
 Plugins selection:\n\n\
  -p, --plugins              Load plugins only if they match specified\n\
                             filenames.\n\
  -P, --plugins-dir          Search plugins only in specified directories.\n\n\
";
    static char const helpOptions2[] = "\
 Informative output:\n\n\
  -v, --verbose              Verbosely list files processed.\n\n\
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
  -f                         Use archive file or device ARCHIVE.\n\n\
 Plugins selection:\n\n\
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
#ifdef HAS_LONG_OPT
    fprintf(stderr, helpOptions2);
#endif
    fprintf(stderr, helpEnd);
}

void
usage()
{
#ifdef HAS_LONG_OPT
    static char const optionsUsage[] = "\
            [--file]\n\
            [--verbose]\n\
            [--list] [--extract] [--get]\n\
            [--plugins] [--plugins-dir]\n\
            [--help] [--usage] [--version]\n";
#endif
    fprintf(stderr, "Usage: snowmonkey [-txhv?] [-f ARCHIVE]\n");
#ifdef HAS_LONG_OPT
    fprintf(stderr, optionsUsage);
#endif
}


void
parse_options(int argc, char *argv[])
{
    char *options = ":f:hp:P:tvx";
    int option;
    int error = 0;
#ifdef HAS_LONG_OPT
    int optionIndex = 0;
    static struct option longOptions[] = {
        { "extract", 0, NULL, 'x' },
        { "get",     0, NULL, 'x' },
        { "file",    1, NULL, 'f' },
        { "plugins", 1, NULL, 'p' },
        { "plugins-dirs", 1, NULL, 'P' },
        { "help",    0, NULL, 'h' },
        { "list",    0, NULL, 't' },
        { "usage",   0, &bOptionPrintUsage, 1 },
        { "verbose", 0, NULL, 'v' },
        { "version", 0, &bOptionPrintVersion, 1 },
        { NULL,      0, NULL, 0 }
    };
#endif

    /* Start parsing options. */
    while (1) {

#ifdef HAS_LONG_OPT
        option = getopt_long(argc, argv, options, longOptions, &optionIndex);
#else
        option = getopt(argc, argv, options);
#endif

        if (option == -1) {
            break;
        }

        if (1 == bOptionPrintUsage) {
            usage();
            exit(EXIT_SUCCESS);
        }

        if (1 == bOptionPrintVersion) {
            version();
            exit(EXIT_SUCCESS);
        }

        /* Check option. */
        switch (option) {
            case 'f' : {
                if (NULL == szOptionInputFilename) {
                    szOptionInputFilename = optarg;
                } else {
                    error = 1;
                    fprintf(stderr, "Error : You can only specify one archive.\n");
                }
            }
                break;
            case 'p' :
                szOptionPluginsFilenames = optarg;
                break;
            case 'P' : 
                szOptionPluginsDirs = optarg;
                break;
            case 't' :
                bOptionList = 1;
                break;
            case 'v' :
                bOptionVerbose = 1;
                break;
            case 'x' :
                bOptionExtract = 1;
                break;
            case '?' :
            case 'h' :
                help();
                exit(EXIT_SUCCESS);
            case ':' :
                error = 1;
                fprintf(stderr, "Error : Missing arguments.\n");
            default :
                break;
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
    context->szInputFilename = szOptionInputFilename;
    context->szPluginsFilenames = szOptionPluginsFilenames;
    context->szPluginsDirs = szOptionPluginsDirs;
    context->bExtract = bOptionExtract;
    context->bList = bOptionList;
    context->bVerbose = bOptionVerbose;
}

void
end_cli_context()
{
    free_context(context);
}
