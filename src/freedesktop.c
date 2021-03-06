/*
 * Copyright 2011-2013 - Etienne 'lenaing' GIRONDEL <lenaing@gmail.com>
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
#include "freedesktop.h"

void
comply_to_freedesktop_specs()
{
    comply_to_environment_variables();
}

void
comply_to_environment_variables()
{
    char *value;

    value = getenv("HOME");
    if (NULL == value) {
        /* Wait, WHAT? */
        fprintf(stderr, "|   No HOME variable defined? REALLY?\n");
        fprintf(stderr, "|   Meh. Setting '/' as HOME.\n");
        putenv("HOME=/");
    }

    value = getenv("XDG_DATA_HOME");
    if (NULL == value) {
        putenv("XDG_DATA_HOME=$HOME/.local/share");
    }

    value = getenv("XDG_DATA_CONFIG");
    if (NULL == value) {
        putenv("XDG_DATA_CONFIG=$HOME/.config");
    }

    value = getenv("XDG_DATA_DIRS");
    if (NULL == value) {
        putenv("XDG_DATA_DIRS=/usr/local/share/:/usr/share/");
    }

    value = getenv("XDG_CONFIG_DIRS");
    if (NULL == value) {
        putenv("XDG_CONFIG_DIRS=/etc/xdg/");
    }

    value = getenv("XDG_CACHE_HOME");
    if (NULL == value) {
        putenv("XDG_CACHE_HOME=$HOME/.cache");
    }

}
