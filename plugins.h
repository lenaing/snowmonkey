#ifndef __PLUGINS_H
#define __PLUGINS_H

#include "globals.h"
#include "context.h"
#include "datafiles.h"
#include <fcntl.h>

#define MAX_PLUGINS 255

extern Context_t *context;

void load_plugins();
void unload_plugins();
int check_fileformat_supported_by_plugins();

#endif /* __PLUGINS_H */
