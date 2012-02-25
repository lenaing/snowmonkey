#include "cli.h"

#ifdef HAS_LONG_OPT
    #include <getopt.h>
#endif

void
version()
{
    static char const version[] = "snowmonkey v%d.%d%s\n\
Copyright (C) 2011 Etienne 'lenaing' Girondel.\n\
BSD License: <http://www.opensource.org/licenses/bsd-license.php>.\n\
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
 Device selection and switching:\n\n\
  -f, --file=ARCHIVE         Use archive file or device ARCHIVE.\n\n\
 Informative output:\n\n\
  -v, --verbose              Verbosely list files processed.\n\n\
 Main operation mode:\n\n\
  -t, --list                 List the contents of an archive.\n\
  -x, --extract              Extract files from an archive.\n\n\
 Other options:\n\n\
  -h, -?, --help             Give this help list.\n\
          --usage            Give a short usage message.\n\
          --version          Print program version.\n\n";
#else
    static char const helpOptions[] = "\
 Device selection and switching:\n\n\
  -f                         Use archive file or device ARCHIVE.\n\n\
 Informative output:\n\n\
  -v                         Verbosely list files processed.\n\n\
 Main operation mode:\n\n\
  -t                         List the contents of an archive.\n\
  -x                         Extract files from an archive.\n\n\
 Other options:\n\n\
  -h, -?                     Give this help list.\n\n";
#endif

    /* Help end. */
    static char const helpEnd[] = "Report bugs to <lenaing@gmail.com>.\n";
    fprintf(stderr, help);
    fprintf(stderr, helpOptions);
    fprintf(stderr, helpEnd);
}

void
usage()
{
#ifdef HAS_LONG_OPT
    static char const optionsUsage[] = "\
            [--file]\n\
            [--verbose]\n\
            [--list] [--extract]\n\
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
    char *options = ":f:hp:tvx";
    int option;
    int error = 0;
#ifdef HAS_LONG_OPT
    int optionIndex = 0;
    static struct option longOptions[] = {
        { "extract", 0, NULL, 'x' },
        { "file",    0, NULL, 'f' },
        /* TODO { "plugins", 0, NULL, 'p' }, */
        /* TODO { "plugins-dir", 0, NULL, 'P' }, */
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
            case 'f' :
                if (NULL == szOptionArchiveFilename) {
                    szOptionArchiveFilename = optarg;
                } else {
                    error = 1;
                    fprintf(stderr, "Error : You can only specify one archive.\n");
                }
                break;
            case 't' :
                bOptionList = 1;
                break;
            case 'v' :
                bOptionVerbose++;
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
initialize_context()
{
    context = new_context();
    context->szArchiveFilename = szOptionArchiveFilename;
    context->bExtract = bOptionExtract;
    context->bList = bOptionList;
    context->bVerbose = bOptionVerbose;
}

void
end_context()
{
    free_context(context);
}
