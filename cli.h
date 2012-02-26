#ifndef __CLI_H
#define __CLI_H

#include "config.h"
#include "globals.h"
#include "context.h"
#include <unistd.h>

extern Context_t *context;

void version();
void help();
void usage();
void parse_options(int, char **);

int bOptionPrintUsage;
int bOptionPrintVersion;
int bOptionVerbose;
int bOptionExtract;
int bOptionList;
char *szOptionInputFilename;
char *szOptionPluginsFilenames;
char *szOptionPluginsDirs;

void initialize_cli_context();
void end_cli_context();
#endif /* __CLI_H */
