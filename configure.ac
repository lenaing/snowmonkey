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

AC_INIT([snowmonkey], [0.2], [lenaing@gmail.com])
AC_PREREQ([2.69])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_AUX_DIR([build-aux])
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE

dnl --------------------------------------------
dnl Versioning
dnl --------------------------------------------
SM_VERSION_MAJOR=0
SM_VERSION_MINOR=3
SM_VERSION_RELEASE=b
AC_SUBST(SM_VERSION_MAJOR)
AC_SUBST(SM_VERSION_MINOR)
AC_SUBST(SM_VERSION_RELEASE)
SM_VERSION="$SM_VERSION_MAJOR.$SM_VERSION_MINOR$SM_VERSION_RELEASE"
AC_SUBST(SM_VERSION)

dnl Checks for programs
dnl --------------------------------------------
AC_PROG_CC
AC_PROG_CC_C_O

PKG_PROG_PKG_CONFIG


dnl Checks for libraries
dnl --------------------------------------------
AC_CHECK_LIB([onsen], onsen_init, , [AC_MSG_ERROR([You must have libonsen-dev installed])])


AC_CHECK_FUNCS([getopt_long], , AC_DEFINE([HAVE_GETOPT_LONG]))

dnl --------------------------------------------
dnl Output files
dnl --------------------------------------------
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
    Makefile
    src/Makefile
    src/version.h
    man/Makefile
])
AC_OUTPUT
