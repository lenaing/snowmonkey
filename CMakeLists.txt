#
# Copyright 2011 - Etienne 'lenaing' GIRONDEL <lenaing@gmail.com>
# 
# snowmonkey :
# ------------
# This software is a libonsen CLI interface under heavy development.
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

# CMake requirements
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# snowmonkey -------------------------------------------------
PROJECT(SnowMonkey)

SET(SM_VERSION_MAJOR 0)
SET(SM_VERSION_MINOR 1)
SET(SM_VERSION_RELEASE "a")
SET(SM_VERSION_CODENAME "'Slippery Steel'")

CONFIGURE_FILE(
    "${PROJECT_SOURCE_DIR}/config.h.in"
    "${PROJECT_SOURCE_DIR}/config.h"
)

# Need libonsen headers. We expect to find them in ../inc/
INCLUDE_DIRECTORIES(
    "${PROJECT_SOURCE_DIR}/../inc/"
)

# Sources
SET(
    snowmonkey_SRCS
        main.c
        cli.c
        actions.c
        file.c
        plugins.c
        datafiles.c
        context.c
        freedesktop.c
        string_utils.c
)

# Need link to libonsen
# TODO : unlink dirs
LINK_DIRECTORIES(${LINK_DIRECTORIES} ../../dist/lib/)
SET(ONSEN_LIB onsen)

# Usual stuff
ADD_DEFINITIONS(-W -Wall -ansi -pedantic)
ADD_EXECUTABLE(snowmonkey ${snowmonkey_SRCS})
TARGET_LINK_LIBRARIES(snowmonkey ${ONSEN_LIB} dl)
