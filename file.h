#ifndef __CLI_FILE_H
#define __CLI_FILE_H

#include "globals.h"
#include "context.h"
#include <fcntl.h>
#include <sys/mman.h>

extern Context_t *context;

void load_file();
void unload_file();

#endif /* __CLI_FILE_H */
