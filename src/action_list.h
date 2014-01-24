/*
 * Copyright 2011-2014 - Etienne 'lenaing' GIRONDEL <lenaing@gmail.com>
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
#ifndef __SNOWMONKEY_ACTION_LIST_H
#define __SNOWMONKEY_ACTION_LIST_H

#include "globals.h"
#include "context.h"
#include "iconv_utils.h"
#include "shift_jis_utils.h"
#include <libonsen/archive_plugin.h>
#include <libonsen/string_utils.h>

int  numlen(int);
void repeat_print_char(int, char);

void init_print_table(OnsenArchiveInfo_t *);
void free_print_table(void);

void print_info(OnsenArchiveInfo_t *);
void print_table_header(OnsenArchiveInfo_t *);
void print_entry(OnsenArchiveEntry_t *, char *, int);

#endif /* __SNOWMONKEY_ACTION_LIST_H */
