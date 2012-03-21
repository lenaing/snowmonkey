#ifndef __DATAFILES_H
#define __DATAFILES_H

#include "globals.h"
#include "string_utils.h"
#include <wordexp.h>
#include <sys/stat.h>

#include <libonsen/string_utils.h>

#define MAX_SEARCH_RESULTS 255
#define MAX_STRING_LENGTH 4096

char *get_data_dirs(char *);
char **find_data_files(char *, char *);
int file_exists(char *);

#endif /* __DATAFILES_H */
