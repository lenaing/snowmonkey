#
# Copyright 2011, 2013 - Etienne 'lenaing' GIRONDEL <lenaing@gmail.com>
# 
# snowmonkey :
# ------------
# This software is a libonsen CLI interface.
# 
# This software is governed by the CeCILL license under French law and
# abiding by the rules of distribution of free software.  You can  use, 
# modify and/ or redistribute the software under the terms of the CeCILL
# license as circulated by CEA, CNRS and INRIA at the following URL
# "http://www.cecill.info". 
# 
# As a counterpart to the access to the source code and  rights to copy,
# modify and redistribute granted by the license, users are provided only
# with a limited warranty  and the software's author,  the holder of the
# economic rights,  and the successive licensors  have only  limited
# liability. 
# 
# In this respect, the user's attention is drawn to the risks associated
# with loading,  using,  modifying and/or developing or reproducing the
# software by the user in light of its specific status of free software,
# that may mean  that it is complicated to manipulate,  and  that  also
# therefore means  that it is reserved for developers  and  experienced
# professionals having in-depth computer knowledge. Users are therefore
# encouraged to load and test the software's suitability as regards their
# requirements in conditions enabling the security of their systems and/or 
# data to be ensured and,  more generally, to use and operate it in the 
# same conditions as regards security. 
# 
# The fact that you are presently reading this means that you have had
# knowledge of the CeCILL license and that you accept its terms.
#
bin_PROGRAMS = snowmonkey

snowmonkey_CFLAGS = -W -Wall -ansi -pedantic -g3
snowmonkey_SOURCES = \
    action_extract.c action_extract.h \
    action_list.c action_list.h \
    actions.c actions.h \
    cli.c cli.h \
    constants.h \
    context.c context.h \
    datafiles.c datafiles.h \
    file.c file.h \
    freedesktop.c freedesktop.h \
    globals.h \
    iconv_utils.c \
    main.c iconv_utils.h \
    plugins.c plugins.h \
    progress.c progress.h \
    shift_jis_utils.c shift_jis_utils.h

snowmonkey_LDFLAGS = -ldl -lonsen
