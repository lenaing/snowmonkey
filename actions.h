#ifndef __CLI_ACTIONS_H
#define __CLI_ACTIONS_H

#include "globals.h"
#include "context.h"

#include <libonsen/archive_plugin.h>
#include <libonsen/iconv_utils.h>
#include <libonsen/shift_jis_utils.h>
extern Context_t *context;

/* Encoding related */
iconv_t pIconv;
extern char *OnsenEncodings[];

void doList();

#endif /* __CLI_ACTIONS_H */
