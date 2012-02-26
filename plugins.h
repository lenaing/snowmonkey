#ifndef __PLUGINS_H
#define __PLUGINS_H

#include "globals.h"
#include "context.h"
#include "datafiles.h"
#include <fcntl.h>

#define MAX_PLUGINS 255

#define PLUGINS_NAMES_DELIMITER ","
#define PLUGINS_BASE_DIR "plugins/"
#define PLUGINS_EXTENSION ".so"

extern Context_t *context;

void load_plugins(char *, char *);
void load_plugins_from_filenames(char **);

void unload_plugins();
int check_fileformat_supported_by_plugins();

#endif /* __PLUGINS_H */
