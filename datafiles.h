#ifndef __DATAFILES_H
#define __DATAFILES_H

#include "globals.h"
#include "string_utils.h"
#include <wordexp.h>
#include <sys/stat.h>

#include <libonsen/string_utils.h>

char *get_data_dirs();
char **find_data_files(char *);

#endif /* __DATAFILES_H */
